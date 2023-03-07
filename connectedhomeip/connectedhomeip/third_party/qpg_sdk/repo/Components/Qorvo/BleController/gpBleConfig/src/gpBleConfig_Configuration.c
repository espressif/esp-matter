/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Bluetooth Low Energy
 *   Declarations of the public functions and enumerations of gpBle.
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_BLECONFIG

#include "gpBleConfig.h"

/*****************************************************************************
 *                    Data Definitions
 *****************************************************************************/

static const gpBle_Configuration_t ROM gpBle_Configuration FLASH_PROGMEM =
{
    .hciVersion = GP_DIVERSITY_BLECONFIG_VERSION_ID,
    .hciRevision =GP_DIVERSITY_BLECONFIG_HCI_REVISION_ID,
    .lmppalVersion = GP_DIVERSITY_BLECONFIG_VERSION_ID,
    .lmppalSubversion = GP_DIVERSITY_BLECONFIG_SUBVERSION_ID,
    .companyId = GP_DIVERSITY_BLECONFIG_COMPANY_ID
};

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

UInt8 gpBleConfig_GetLlVersion(void)
{
    return gpBle_Configuration.lmppalVersion;
}

UInt16 gpBleConfig_GetCompanyId(void)
{
    return gpBle_Configuration.companyId;
}

UInt16 gpBleConfig_GetLlSubversion(void)
{
    return gpBle_Configuration.lmppalSubversion;
}

void gpBleConfig_GetConfig(gpBle_Configuration_t* pConfig)
{
    GP_ASSERT_DEV_INT(pConfig != NULL);

    MEMCPY(pConfig, &gpBle_Configuration, sizeof(gpBle_Configuration));
}
