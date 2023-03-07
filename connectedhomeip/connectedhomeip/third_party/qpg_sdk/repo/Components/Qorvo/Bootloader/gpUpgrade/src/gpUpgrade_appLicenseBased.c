/*
 *   Copyright (c) 2017, Qorvo Inc
 *
 *   Upgrade functionality
 *   Implementation of gpUpgrade
 *
 *   This software is owned by Qorvo Inc
 *   and protected under applicable copyright laws.
 *   It is delivered under the terms of the license
 *   and is intended and supplied for use solely and
 *   exclusively with products manufactured by
 *   Qorvo Inc.
 *
 *
 *   THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *   CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *   IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *   LIMITED TO, IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *   QORVO INC. SHALL NOT, IN ANY
 *   CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *   INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *   FOR ANY REASON WHATSOEVER.
 *
 *   $Header$
 *   $Change$
 *   $DateTime$
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_UPGRADE

#include "gpHal.h"
#include "gpUtils.h"
#include "gpLog.h"
#include "gpUpgrade.h"
#include "gpUpgrade_defs.h"

#include "hal_user_license.h"

#if defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)
#include "gpExtStorage.h"
#endif // GP_COMP_EXTSTORAGE


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

extern const UInt32 appImageLowerFlashStart;
#if defined(GP_COMP_EXTSTORAGE) || defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
extern const UInt32 upgImageUserLicenseStart;
#endif // GP_COMP_EXTSTORAGE || GP_UPGRADE_DIVERSITY_COMPRESSION

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpUpgrade_Init(void)
{
    gpHal_FlashError_t ret;
    UInt32 loadCompleteMW = 0;
#if   defined(GP_COMP_EXTSTORAGE) || defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
    /* Invalidate load completed magic word in preperation of downloading a new image */
    ret = gpHal_FlashProgramSector(upgImageUserLicenseStart + LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET, sizeof(loadCompleteMW), (UInt8*)&loadCompleteMW);
#endif
    GP_ASSERT_SYSTEM(ret == gpHal_FlashError_Success);
}

gpUpgrade_Status_t gpUpgrade_SetPendingImage(UInt32 swVer, UInt32 hwVer, UInt32 startAddr, UInt32 imgSz)
{
    /* TODO : remove the input arguments */
    gpHal_FlashError_t ret;
    UInt32 progLoadMW;
    UInt32 loadCompleteMW;
    UInt8 activeImagefreshnessCounter, pendingImagefreshnessCounter;
    UInt32 activeImageAddress, pendingImageAddress;
#if   defined(GP_COMP_EXTSTORAGE) || defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
    /* Copy the license of the external image to internal flash */
    UInt8 upgLicense[LOADED_USER_LICENSE_TOTAL_SIZE+EXTENDED_USER_LICENSE_TOTAL_SIZE];
#if defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)
    gpExtStorage_ReadBlock(gpUpgrade_GetOtaAreaStartAddress(), LOADED_USER_LICENSE_TOTAL_SIZE+EXTENDED_USER_LICENSE_TOTAL_SIZE, upgLicense);
#elif defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
    gpHal_FlashRead(gpUpgrade_GetOtaAreaStartAddress(), LOADED_USER_LICENSE_TOTAL_SIZE+EXTENDED_USER_LICENSE_TOTAL_SIZE, upgLicense);
#endif
    ret = gpHal_FlashProgramSector(upgImageUserLicenseStart , LOADED_USER_LICENSE_TOTAL_SIZE+EXTENDED_USER_LICENSE_TOTAL_SIZE, upgLicense);
    GP_ASSERT_SYSTEM(ret == gpHal_FlashError_Success);

    activeImageAddress = appImageLowerFlashStart;
    pendingImageAddress = upgImageUserLicenseStart;
#endif
    GP_LOG_PRINTF("Active_image_addr 0x%lx, Pending_image_addr 0x%lx", 0, activeImageAddress, pendingImageAddress);
    /* TODO: The OTA flow must call this function only after making sure that the image is valid
       This sanity check can be removed in the future.
       */
    gpHal_FlashRead(pendingImageAddress + USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD_OFFSET, sizeof(progLoadMW), (UInt8*)&progLoadMW);
    if (progLoadMW != USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD)
    {
        GP_LOG_SYSTEM_PRINTF("gpUpgrade_SetPendingImage failed, pending image has invalid magic word", 0);
        return gpUpgrade_StatusInvalidImage;
    }


    gpHal_FlashRead(activeImageAddress + LOADED_USER_LICENSE_FRESHNESS_COUNTER_OFFSET, sizeof(activeImagefreshnessCounter), (UInt8*)&activeImagefreshnessCounter);

    /* Update the freshness counter in the pending image so that is is picked up by the UMB on next reboot */
    pendingImagefreshnessCounter = activeImagefreshnessCounter + 1;
    ret = gpHal_FlashProgramSector(pendingImageAddress + LOADED_USER_LICENSE_FRESHNESS_COUNTER_OFFSET, sizeof(pendingImagefreshnessCounter), (UInt8*)&pendingImagefreshnessCounter);
    GP_ASSERT_SYSTEM(ret == gpHal_FlashError_Success);

    /* Write the load complete magic word to indicate that the pending image is ready to be installed */
    loadCompleteMW = LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD;
    ret = gpHal_FlashProgramSector(pendingImageAddress + LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET, sizeof(loadCompleteMW), (UInt8*)&loadCompleteMW);
    GP_ASSERT_SYSTEM(ret == gpHal_FlashError_Success);
    GP_LOG_PRINTF("Freshness Counter (Active Image) %x, (Pending Image) %lx", 0, activeImagefreshnessCounter, pendingImagefreshnessCounter);

    return gpUpgrade_StatusSuccess;
}


void gpUpgrade_Reset(void)
{
    UInt32* addr = (UInt32*) GP_MM_RAM_CRC_START;
    *addr = GP_UPGRADE_MAGIC_WORD;
    GP_WB_WRITE_PMUD_SOFT_POR_BOOTLOADER(1);
}
