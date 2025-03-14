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

    module: cosa_motiondetection_dml.c

        Code for the data model and API calls for the
        motion detection logic handled with TRs

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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ansc_platform.h"
#include "ccsp_trace.h"
#include "ccsp_syslog.h"
#include "cosa_motiondetection_dml.h"
#include "cosa_motiondetection_api.h"
#include "ctype.h"

/**********************************************************************
                            EXTERN
**********************************************************************/

extern ANSC_HANDLE bus_handle;
extern char g_Subsystem[32];

/**********************************************************************
                            DEFINES
**********************************************************************/

#define MAX_STR_SIZE 1024

/***********************************************************************

 APIs for Object:

    Device.WiFi.X_RDK_MotionDetection.

    *  MotionDetection_GetParamBoolValue
    *  MotionDetection_SetParamBoolValue
    *  MotionDetection_GetParamIntValue
    *  MotionDetection_SetParamIntValue
    *  MotionDetection_GetParamUlongValue
    *  MotionDetection_SetParamUlongValue
    *  MotionDetection_GetParamStringValue
    *  MotionDetection_SetParamStringValue

***********************************************************************/

BOOL
MotionDetection_GetParamBoolValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        BOOL* pBool
    )
{
    if (AnscEqualString(ParamName, "Enable", TRUE))
    {
        *pBool = MotionDetectionConfig_GetBOOL(ParamName);
        return TRUE;
    }

    return FALSE;
}

BOOL
MotionDetection_SetParamBoolValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        BOOL bValue
    )
{
    if (AnscEqualString(ParamName, "Enable", TRUE))
    {
        MotionDetectionConfig_SetBOOL(ParamName, bValue);
        return TRUE;
    }

    return FALSE;
}

BOOL
MotionDetection_GetParamUlongValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        ULONG* pUlong
    )
{
    if (AnscEqualString(ParamName, "Motion", TRUE))
    {
        *pUlong = MotionDetector_GetMotion();
        return TRUE;
    }

    else if (AnscEqualString(ParamName, "SensingThresholdUp", TRUE))
    {
        *pUlong = MotionDetectionConfig_GetULONG(ParamName);
        return TRUE;
    }

    else if (AnscEqualString(ParamName, "SensingThresholdDown", TRUE))
    {
        *pUlong = MotionDetectionConfig_GetULONG(ParamName);
        return TRUE;
    }

    else if (AnscEqualString(ParamName, "MonitorInterval", TRUE))
    {
        *pUlong = MotionDetectionConfig_GetULONG(ParamName);
        return TRUE;
    }

    return FALSE;
}

BOOL
MotionDetection_SetParamUlongValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        ULONG ulValue
    )
{
    if (AnscEqualString(ParamName, "SensingThresholdUp", TRUE))
    {
        MotionDetectionConfig_SetULONG(ParamName, ulValue);
        return TRUE;
    }

    else if (AnscEqualString(ParamName, "SensingThresholdDown", TRUE))
    {
        MotionDetectionConfig_SetULONG(ParamName, ulValue);
        return TRUE;
    }

    else if (AnscEqualString(ParamName, "MonitorInterval", TRUE))
    {
        MotionDetectionConfig_SetULONG(ParamName, ulValue);
        MotionDetection_ChangeMonitor();
        return TRUE;
    }

    return FALSE;
}

BOOL
MotionDetection_GetParamStringValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        char* pValue,
        ULONG* pUlSize
    )
{
    if (AnscEqualString(ParamName, "Status", TRUE))
    {
        AnscCopyString(pValue, MotionDetector_GetStatus());
        *pUlSize = MAX_STR_SIZE;
        return TRUE;
    }

    else if (AnscEqualString(ParamName, "SensingDeviceMACAddress", TRUE))
    {
        char *MAC = MotionDetectionConfig_GetString(ParamName);
        AnscCopyString(pValue, MAC);
        *pUlSize = MAX_STR_SIZE;
        free(MAC);
        return TRUE;
    }

    return FALSE;
}

BOOL
MotionDetection_SetParamStringValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        char* strValue
    )
{
    if (AnscEqualString(ParamName, "SensingDeviceMACAddress", TRUE))
    {
        MotionDetectionConfig_SetString(ParamName, strValue);
        MotionDetection_ChangeMonitor();
        return TRUE;
    }

    return FALSE;
}

/***********************************************************************

 APIs for Object:

    Device.WiFi.X_RDK_MotionDetection.SensingEvents.{i}.

    *  MotionDetection_SensingEvents_GetEntryCount
    *  MotionDetection_SensingEvents_GetEntry
    *  MotionDetection_SensingEvents_IsUpdated
    *  MotionDetection_SensingEvents_GetParamStringValue

***********************************************************************/

ULONG
MotionDetection_SensingEvents_GetEntryCount
    (
        ANSC_HANDLE
    )
{
    return MotionDetectionEvents_GetCount();
}

ANSC_HANDLE
MotionDetection_SensingEvents_GetEntry
    (
        ANSC_HANDLE hInsContext,
        ULONG nIndex,
        ULONG* pInsNumber
    )
{
    *pInsNumber = nIndex + 1;
    return MotionDetectionEvents_GetEventByIndex(nIndex);
}

BOOL
MotionDetection_SensingEvents_IsUpdated
    (
        ANSC_HANDLE hInsContext
    )
{
	UNREFERENCED_PARAMETER(hInsContext);
	return TRUE;
}

ULONG
MotionDetection_SensingEvents_Synchronize
    (
        ANSC_HANDLE hInsContext
    )
{
    return ANSC_STATUS_SUCCESS;
}

BOOL
MotionDetection_SensingEvents_GetParamStringValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        char* pValue,
        ULONG* pUlSize
    )
{
    MOTIONDETECTION_SENSINGEVENT_t *SensingEvent = (MOTIONDETECTION_SENSINGEVENT_t*) hInsContext;

    if (AnscEqualString(ParamName, "SensingDevice", TRUE))
    {
        AnscCopyString(pValue, SensingEvent->SensingDevice);
        *pUlSize = MAX_STR_SIZE;
        return TRUE;
    }

    else if (AnscEqualString(ParamName, "Type", TRUE))
    {
        AnscCopyString(pValue, SensingEvent->Type);
        *pUlSize = MAX_STR_SIZE;
        return TRUE;
    }

    else if (AnscEqualString(ParamName, "Time", TRUE))
    {
        AnscCopyString(pValue, SensingEvent->Time);
        *pUlSize = MAX_STR_SIZE;
        return TRUE;
    }

    return FALSE;
}

ULONG
MotionDetection_Commit
    (
        ANSC_HANDLE hInsContext
    )
{
    return 0;
}