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

    module: plugin_main.c

        Implement COSA Data Model Library Init and Unload apis.

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

#include "ansc_platform.h"
#include "ansc_load_library.h"
#include "cosa_plugin_api.h"
#include "plugin_main.h"
#include "cosa_motiondetection_dml.h"
#include "cosa_motiondetection_api.h"

/**********************************************************************
                            DEFINES
**********************************************************************/

#define THIS_PLUGIN_VERSION 1


/**********************************************************************
                            FUNCTIONS
**********************************************************************/

int ANSC_EXPORT_API
COSA_Init
    (
        ULONG uMaxVersionSupported,
        void* hCosaPlugInfo /* PCOSA_PLUGIN_INFO passed in by the caller */
    )
{
    PCOSA_PLUGIN_INFO pPlugInfo = (PCOSA_PLUGIN_INFO)hCosaPlugInfo;
    if (uMaxVersionSupported < THIS_PLUGIN_VERSION)
    {
        /* this version is not supported */
        return -1;
    }

    pPlugInfo->uPluginVersion = THIS_PLUGIN_VERSION;

    /*
     * Registering the back-end API for the data model.
     */
	pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_GetParamBoolValue",  MotionDetection_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_SetParamBoolValue",  MotionDetection_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_GetParamUlongValue",  MotionDetection_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_SetParamUlongValue",  MotionDetection_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_GetParamStringValue",  MotionDetection_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_SetParamStringValue",  MotionDetection_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_SensingEvents_GetEntryCount",  MotionDetection_SensingEvents_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_SensingEvents_GetEntry",  MotionDetection_SensingEvents_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_SensingEvents_GetParamStringValue",  MotionDetection_SensingEvents_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_SensingEvents_Synchronize",  MotionDetection_SensingEvents_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_SensingEvents_IsUpdated",  MotionDetection_SensingEvents_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MotionDetection_Commit",  MotionDetection_Commit);

    MotionDetection_ChangeMonitor();

    return 0;
}

BOOL ANSC_EXPORT_API
COSA_IsObjectSupported
    (
        char* pObjName
    )
{
    return TRUE;
}

void ANSC_EXPORT_API
COSA_Unload
    (
        void
    )
{
    MotionDetectionEvents_Free();
}
