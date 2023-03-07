/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, 2019, Qorvo Inc
 *
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpVersion.h"
#ifdef GP_COMP_COM
#include "gpCom.h"
#endif //GP_COMP_COM

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_VERSION

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

extern gpVersion_ModuleInfo_t Version_ModuleMapping[GP_DIVERSITY_VERSION_MAX_MODULE_TABLE_SIZE];

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void Version_Init(void);
void Version_UpdateModuleVersion(gpVersion_ModuleInfo_t* moduleInfo, gpVersion_ReleaseInfo_t apiVersion, UInt32 commId);
void Version_RegisterModule_Common(UInt8 moduleId, gpVersion_ReleaseInfo_t* versions, UInt8 nrOfVersions, gpVersion_cbSetModuleVersion_t cbSetModuleApiVersion);
void Version_DeRegisterModule_Common(UInt8 moduleId);
#ifdef GP_COMP_COM
void Version_ConnectionClose(UInt8 moduleId, gpCom_CommunicationId_t communicationId);
#endif


