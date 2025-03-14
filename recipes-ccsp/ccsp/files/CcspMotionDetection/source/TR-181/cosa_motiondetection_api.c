/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2025 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
/***********************************************************************

    module: cosa_motiondetection_api.c

        The back-end API for motion detection.
        Handles syscfg and a separate thread to monitor events.

    ---------------------------------------------------------------

    author:

        GL Team (RDK-B 2025 Hackathon)

    ---------------------------------------------------------------

    revision:

        03/14/2025    initial revision.

**********************************************************************/

/**********************************************************************
                            INCLUDES
**********************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdatomic.h>

#include <MotionDetection/md.h>

#include "cosa_motiondetection_api.h"
#include "ansc_platform.h"
#include "ccsp_trace.h"
#include "ccsp_syslog.h"
#include "stdlib.h"
#include "syscfg/syscfg.h"
#include "ctype.h"

/**********************************************************************
                            DEFINES
**********************************************************************/

#define SYSCONFIG_BUFFER 256
#define SYSCONFIG_ENTRY_FORMAT "MotionDetection_%s"

#define TIME_BUFFER 32
#define MAX_SENSING_EVENTS 256

#define MOTIONDETECTION_SENSING_INTERFACE "wifi1"
#define MOTIONDETECTION_MIN_MONITOR_INTERVAL 100

/**********************************************************************
                        SENSING EVENTS
**********************************************************************/

/*
 * This is the actual percentage of the motion, achieved from the motion detection.
 * It's handled by the collecting thread.
 */
UINT uCurrentMotion = 0;

/*
 * The status of if we are inside a motion event or not. This value is initially false,
 * but upon uCurrentMotion reaching SensingThresholdUp, it's set to TRUE
 * It becomes FALSE after the uCurrentMotion falls below SensingThresholdDown
 */
BOOL bDuringMotion = FALSE;

/*
 * Sensing events are implemented as a linked list of sensing event structures.
 * This is the start of the list.
 */
MOTIONDETECTION_SENSINGEVENT_t *SensingEvents = NULL;

/*
 * Monitoring thread related variables.
 * The thread runs in the background and collects data from the motion detection.
 * For data safety, a mutex and an atomic run flag were added as well.
 */
pthread_t CollectingThread = 0;
pthread_mutex_t Mutex = PTHREAD_MUTEX_INITIALIZER;
atomic_int isMonitoring = 0;

/******************************
        EVENT FUNCTIONS
******************************/

static ULONG
MotionDetectionEvents_GetEventCount
    (
        VOID
    )
{
    ULONG ulCount = 0;

    MOTIONDETECTION_SENSINGEVENT_t *EventIterator = SensingEvents;

    while (EventIterator != NULL)
    {
        ulCount++;
        EventIterator = EventIterator->NextEvent;
    }

    return ulCount;
}

ULONG
MotionDetectionEvents_GetCount
    (
        VOID
    )
{
    pthread_mutex_lock(&Mutex);
    ULONG ulCount = MotionDetectionEvents_GetEventCount();
    pthread_mutex_unlock(&Mutex);

    return ulCount;
}

ANSC_HANDLE
MotionDetectionEvents_GetEventByIndex
    (
        ULONG nIndex
    )
{
    ULONG ulCount = 0;

    pthread_mutex_lock(&Mutex);

    MOTIONDETECTION_SENSINGEVENT_t *EventIterator = SensingEvents;

    while (EventIterator != NULL)
    {
        if (nIndex == ulCount)
        {
            pthread_mutex_unlock(&Mutex);
            return (ANSC_HANDLE)EventIterator;
        }
        ulCount++;
        EventIterator = EventIterator->NextEvent;
    }

    pthread_mutex_unlock(&Mutex);

    return NULL;
}

VOID
MotionDetectionEvents_Free
    (
        VOID
    )
{
    MOTIONDETECTION_SENSINGEVENT_t *NextEvent = NULL;

    pthread_mutex_lock(&Mutex);

    MOTIONDETECTION_SENSINGEVENT_t *EventIterator = SensingEvents;

    while (EventIterator != NULL)
    {
        NextEvent = EventIterator->NextEvent;
        free(EventIterator->SensingDevice);
        free(EventIterator->Time);
        free(EventIterator->Type);
        free(EventIterator);
        EventIterator = NextEvent;
    }

    pthread_mutex_unlock(&Mutex);
}

VOID
MotionDetectionEvents_TriggerEvent
    (
        char* EventName
    )
{
    MOTIONDETECTION_SENSINGEVENT_t* NewEntry = NULL;
    time_t TriggerTime;
    struct tm * TimeInfo;
    char TimeBuffer[TIME_BUFFER];

    time(&TriggerTime);
    strftime(TimeBuffer, sizeof(TimeBuffer), "%Y-%m-%dT%H:%M:%SZ", gmtime(&TriggerTime));

    NewEntry = (MOTIONDETECTION_SENSINGEVENT_t*)calloc(1, sizeof(MOTIONDETECTION_SENSINGEVENT_t));
    NewEntry->SensingDevice = MotionDetectionConfig_GetString("SensingDeviceMACAddress");
    NewEntry->Type = strdup(EventName);
    NewEntry->Time = strdup(TimeBuffer);
    NewEntry->NextEvent = NULL;

    pthread_mutex_lock(&Mutex);

    if (SensingEvents == NULL)
    {
        SensingEvents = NewEntry;
    }
    else
    {
        MOTIONDETECTION_SENSINGEVENT_t* CurrentEvent = SensingEvents;

        /*
         * When there are too many events
         */
        while (MotionDetectionEvents_GetEventCount() >= MAX_SENSING_EVENTS)
        {
            CurrentEvent = SensingEvents->NextEvent;
            free(SensingEvents->SensingDevice);
            free(SensingEvents->Time);
            free(SensingEvents->Type);
            free(SensingEvents);
            SensingEvents = CurrentEvent;
        }

        CurrentEvent = SensingEvents;

        while (CurrentEvent->NextEvent != NULL)
        {
            CurrentEvent = CurrentEvent->NextEvent;
        }

        CurrentEvent->NextEvent = NewEntry;
    }

    pthread_mutex_unlock(&Mutex);
}

/**********************************************************************
                        CONFIGURATION
**********************************************************************/

/*
 * Common entry functions for the configuration that wrap syscfg calls.
 * Basic get and set functions return strings taken from the syscfg directly.
 * Wrapper functions return the respective data types.
 */

static char*
MotionDetectionConfig_Get
    (
        char* EntryName
    )
{
    char ConfigEntry[SYSCONFIG_BUFFER] = {0};
    char *ConfigGetBuffer = NULL;

    snprintf(ConfigEntry, sizeof(ConfigEntry), SYSCONFIG_ENTRY_FORMAT, EntryName);
    if (ConfigEntry == NULL)
    {
        return NULL;
    }

    ConfigGetBuffer = (char *)calloc(SYSCONFIG_BUFFER, (sizeof(char)));
    if (ConfigGetBuffer == NULL)
    {
        return NULL;
    }

    syscfg_init();

    syscfg_get(NULL, ConfigEntry, ConfigGetBuffer, SYSCONFIG_BUFFER);
    return ConfigGetBuffer;
}

static BOOL
MotionDetectionConfig_Set
    (
        char* EntryName,
        char *Value
    )
{
    char ConfigEntry[SYSCONFIG_BUFFER] = {0};

    snprintf(ConfigEntry,sizeof(ConfigEntry), SYSCONFIG_ENTRY_FORMAT, EntryName);

    if (ConfigEntry == NULL || Value == NULL)
    {
        return FALSE;
    }

    if (syscfg_set(NULL, ConfigEntry, Value) != 0)
    {
        return FALSE;
    }

    if (syscfg_commit() != 0)
    {
        return FALSE;
    }

    return TRUE;
}

/****************************
        GET FUNCTIONS
****************************/

UINT
MotionDetectionConfig_GetUINT
    (
        char* EntryName
    )
{
    UINT uResult;
    char* GetBuffer = MotionDetectionConfig_Get(EntryName);

    uResult = (UINT)atoi(GetBuffer);

    free(GetBuffer);
    return uResult;
}

ULONG
MotionDetectionConfig_GetULONG
    (
        char* EntryName
    )
{
    ULONG ulResult;
    char* GetBuffer = MotionDetectionConfig_Get(EntryName);

    ulResult = (ULONG)atoi(GetBuffer);

    free(GetBuffer);
    return ulResult;
}

BOOL
MotionDetectionConfig_GetBOOL
    (
        char* EntryName
    )
{
    BOOL bResult;
    char* GetBuffer = MotionDetectionConfig_Get(EntryName);

    bResult = (BOOL)atoi(GetBuffer);

    free(GetBuffer);
    return bResult;
}

char*
MotionDetectionConfig_GetString
    (
        char* EntryName
    )
{
    return MotionDetectionConfig_Get(EntryName);
}

/****************************
        SET FUNCTIONS
****************************/

BOOL
MotionDetectionConfig_SetUINT
    (
        char* EntryName,
        UINT uValue
    )
{
    char ConfigSetBuffer[SYSCONFIG_BUFFER] = {0};
    snprintf(ConfigSetBuffer, sizeof(ConfigSetBuffer), "%u", uValue);
    return MotionDetectionConfig_Set(EntryName, ConfigSetBuffer);
}

BOOL
MotionDetectionConfig_SetULONG
    (
        char* EntryName,
        ULONG ulValue
    )
{
    char ConfigSetBuffer[SYSCONFIG_BUFFER] = {0};
    snprintf(ConfigSetBuffer, sizeof(ConfigSetBuffer), "%lu", ulValue);
    return MotionDetectionConfig_Set(EntryName, ConfigSetBuffer);
}

BOOL
MotionDetectionConfig_SetBOOL
    (
        char* EntryName,
        BOOL bValue
    )
{
    char ConfigSetBuffer[SYSCONFIG_BUFFER] = {0};
    snprintf(ConfigSetBuffer, sizeof(ConfigSetBuffer), "%u", bValue);
    return MotionDetectionConfig_Set(EntryName, ConfigSetBuffer);
}

BOOL
MotionDetectionConfig_SetString
    (
        char* EntryName,
        char* strValue
    )
{
    return MotionDetectionConfig_Set(EntryName, strValue);
}

/**********************************************************************
                        MOTION DETECTION API
**********************************************************************/

char*
MotionDetector_GetStatus
    (
        VOID
    )
{
    if (MotionDetectionConfig_GetBOOL("Enable") == TRUE)
    {
        if (md_is_monitoring_active() == TRUE)
        {
            return "ACTIVE";
        }
        else
        {
            return "IDLE";
        }
    }

    return "DISABLED";
}

ULONG
MotionDetector_GetMotion
    (
        VOID
    )
{
    if (md_is_monitoring_active()  == FALSE || MotionDetectionConfig_GetBOOL("Enable") == FALSE)
    {
        return 0;
    }

    pthread_mutex_lock(&Mutex);
    ULONG ulMotion = uCurrentMotion;
    pthread_mutex_unlock(&Mutex);

    return ulMotion;
}

VOID*
thread_MotionDetection_Monitoring
    (
        VOID* Arguments
    )
{
    while (atomic_load(&isMonitoring))
    {
        usleep(MotionDetectionConfig_GetULONG("MonitorInterval") * 1000);

        pthread_mutex_lock(&Mutex);
        uCurrentMotion = md_get_motion();
        pthread_mutex_unlock(&Mutex);

        if (bDuringMotion == FALSE && uCurrentMotion >= MotionDetectionConfig_GetULONG("SensingThresholdUp"))
        {
            bDuringMotion = TRUE;
            MotionDetectionEvents_TriggerEvent("MOTION_STARTED");
            continue;
        }

        if (bDuringMotion == TRUE && uCurrentMotion <= MotionDetectionConfig_GetULONG("SensingThresholdDown"))
        {
            bDuringMotion = FALSE;
            MotionDetectionEvents_TriggerEvent("MOTION_STOPPED");
            continue;
        }
    }
}

VOID
MotionDetection_ChangeMonitor
    (
        VOID
    )
{
    char* MAC = MotionDetectionConfig_GetString("SensingDeviceMACAddress");
    ULONG ulInterval = MotionDetectionConfig_GetULONG("MonitorInterval");

    if (strlen(MAC) == 0 || MotionDetectionConfig_GetBOOL("Enable") == FALSE)
    {
        md_stop_monitoring();
        if (atomic_load(&isMonitoring))
        {
            atomic_store(&isMonitoring, 0);
            pthread_join(CollectingThread, NULL);
        }
        return;
    }

    if (ulInterval < MOTIONDETECTION_MIN_MONITOR_INTERVAL)
    {
        ulInterval = MOTIONDETECTION_MIN_MONITOR_INTERVAL;
    }

    md_start_monitoring(MOTIONDETECTION_SENSING_INTERFACE, MAC, ulInterval);
    atomic_store(&isMonitoring, 1);
    if (pthread_create(&CollectingThread, NULL, thread_MotionDetection_Monitoring, NULL) != 0)
    {
        atomic_store(&isMonitoring, 0);
    }
}
