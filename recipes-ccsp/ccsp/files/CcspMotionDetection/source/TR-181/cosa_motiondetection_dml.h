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

    module: cosa_motiondetection_dml.h

        Definitions for the data model functions to be registered.

    ---------------------------------------------------------------

    author:

        GL Team (RDK-B 2025 Hackathon)

    ---------------------------------------------------------------

    revision:

        03/14/2025    initial revision.

**********************************************************************/

#ifndef  _COSA_APIS_MOTIONDETECTION_H
#define  _COSA_APIS_MOTIONDETECTION_H

#include "slap_definitions.h"
#include "ccsp_psm_helper.h"

/***********************************************************************

 APIs for Object:

    Device.WiFi.X_RDK_MotionDetection.

    *  MotionDetection_GetParamBoolValue
    *  MotionDetection_SetParamBoolValue
    *  MotionDetection_GetParamUlongValue
    *  MotionDetection_SetParamUlongValue
    *  MotionDetection_GetParamStringValue
    *  MotionDetection_SetParamStringValue
    *  MotionDetection_Commit

***********************************************************************/

BOOL
MotionDetection_GetParamBoolValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        BOOL* pBool
    );

BOOL
MotionDetection_SetParamBoolValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        BOOL bValue
    );

BOOL
MotionDetection_GetParamUlongValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        ULONG* pUlong
    );

BOOL
MotionDetection_SetParamUlongValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        ULONG ulValue
    );

BOOL
MotionDetection_GetParamStringValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        char* pValue,
        ULONG* pUlSize
    );

BOOL
MotionDetection_SetParamStringValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        char* strValue
    );

ULONG
MotionDetection_Commit
    (
        ANSC_HANDLE hInsContext
    );

/***********************************************************************

 APIs for Object:

    Device.WiFi.X_RDK_MotionDetection.SensingEvents.{i}.

    *  MotionDetection_SensingEvents_GetEntryCount
    *  MotionDetection_SensingEvents_GetEntry
    *  MotionDetection_SensingEvents_IsUpdated
    *  MotionDetection_SensingEvents_Synchronize
    *  MotionDetection_SensingEvents_GetParamStringValue

***********************************************************************/

ULONG
MotionDetection_SensingEvents_GetEntryCount
    (
        ANSC_HANDLE
    );

ANSC_HANDLE
MotionDetection_SensingEvents_GetEntry
    (
        ANSC_HANDLE hInsContext,
        ULONG nIndex,
        ULONG* pInsNumber
    );

BOOL
MotionDetection_SensingEvents_GetParamStringValue
    (
        ANSC_HANDLE hInsContext,
        char* ParamName,
        char* pValue,
        ULONG* pUlSize
    );

BOOL
MotionDetection_SensingEvents_IsUpdated
    (
        ANSC_HANDLE hInsContext
    );

ULONG
MotionDetection_SensingEvents_Synchronize
    (
        ANSC_HANDLE hInsContext
    );

#endif
