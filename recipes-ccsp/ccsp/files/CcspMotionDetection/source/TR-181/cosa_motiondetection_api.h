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

    module: cosa_motiondetection_api.h

        Exposing API functions for the data model

    ---------------------------------------------------------------

    author:

        GL Team (RDK-B 2025 Hackathon)

    ---------------------------------------------------------------

    revision:

        03/14/2025    initial revision.

**********************************************************************/

#ifndef _MOTIONDETECTION_APIS
#define _MOTIONDETECTION_APIS

/**********************************************************************
                            INCLUDES
**********************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ansc_platform.h"
#include "ccsp_trace.h"
#include "ccsp_syslog.h"

/**********************************************************************
                        TYPE DEFINITIONS
**********************************************************************/

/*
 * Sensing Event structure.
 * It's an entry in a linked list.
 * All the strings are allocated.
 */
typedef struct
_MOTIONDETECTION_SENSINGEVENT
{
    char *SensingDevice;
    char *Type;
    char *Time;
    struct _MOTIONDETECTION_SENSINGEVENT* NextEvent;
} MOTIONDETECTION_SENSINGEVENT_t;

/***********************************************************************

 APIs for motion detection events:

    *  MotionDetectionEvents_GetCount
    *  MotionDetectionEvents_GetEventByIndex
    *  MotionDetectionEvents_Free
    *  MotionDetectionEvents_TriggerEvent

***********************************************************************/

ULONG
MotionDetectionEvents_GetCount
    (
        VOID
    );

ANSC_HANDLE
MotionDetectionEvents_GetEventByIndex
    (
        ULONG nIndex
    );

VOID
MotionDetectionEvents_Free
    (
        VOID
    );

VOID
MotionDetectionEvents_TriggerEvent
    (
        char* EventName
    );

/***********************************************************************

 APIs for motion detection configuration:

    *  MotionDetectionConfig_GetUINT
    *  MotionDetectionConfig_GetULONG
    *  MotionDetectionConfig_GetBOOL
    *  MotionDetectionConfig_GetString
    *  MotionDetectionConfig_SetUINT
    *  MotionDetectionConfig_SetULONG
    *  MotionDetectionConfig_SetBOOL
    *  MotionDetectionConfig_SetString

***********************************************************************/

UINT
MotionDetectionConfig_GetUINT
    (
        char* EntryName
    );

ULONG
MotionDetectionConfig_GetULONG
    (
        char* EntryName
    );

BOOL
MotionDetectionConfig_GetBOOL
    (
        char* EntryName
    );

char*
MotionDetectionConfig_GetString
    (
        char* EntryName
    );

BOOL
MotionDetectionConfig_SetUINT
    (
        char* EntryName,
        UINT uValue
    );

BOOL
MotionDetectionConfig_SetULONG
    (
        char* EntryName,
        ULONG ulValue
    );

BOOL
MotionDetectionConfig_SetBOOL
    (
        char* EntryName,
        BOOL bValue
    );

BOOL
MotionDetectionConfig_SetString
    (
        char* EntryName,
        char* strValue
    );

/***********************************************************************

 APIs for motion detection monitoring:

    *  MotionDetector_IsRunning
    *  MotionDetector_GetStatus
    *  MotionDetector_GetMotion
    *  MotionDetection_ChangeMonitor

***********************************************************************/

BOOL
MotionDetector_IsRunning
    (
        VOID
    );

char*
MotionDetector_GetStatus
    (
        VOID
    );

ULONG
MotionDetector_GetMotion
    (
        VOID
    );

VOID
MotionDetection_ChangeMonitor
    (
        VOID
    );

#endif