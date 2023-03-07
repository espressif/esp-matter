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
#include "gpUpgrade.h"
#include "gpUpgrade_defs.h"

#if defined(GP_DIVERSITY_LOG)
#include "gpLog.h"
#endif

#include "gpUpgrade_flash.h"
#include "gpUpgrade_hash.h"

#ifdef GP_COMP_EXTSTORAGE
#include "gpExtStorage.h"
#endif // GP_COMP_EXTSTORAGE

#ifdef GP_UPGRADE_DIVERSITY_COMPRESSION
#include "lzma.h"
#endif // GP_UPGRADE_DIVERSITY_COMPRESSION

#include "hal_user_license.h"


extern const UInt32 appImageLowerFlashStart;

#if defined(GP_COMP_EXTSTORAGE) || defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
extern UInt32 upgrade_image_user_license_start__;
#endif // GP_COMP_EXTSTORAGE || GP_UPGRADE_DIVERSITY_COMPRESSION

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static gpUpgrade_Status_t Upgrade_InstallImage();

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void hal__AtomicOn(void)
{

}

void hal__AtomicOff (void)
{

}

#if defined(GP_COMP_EXTSTORAGE)
static gpUpgrade_Status_t Upgrade_InstallImage()
{
    UInt8 retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;

    UInt32 section1Offset;
    UInt32 section1Size;
    UInt32 section2Offset;
    UInt32 section2Size;

    const UInt32 externalUserLicenseAddr = (UInt32)&upgrade_image_user_license_start__;

    gpHal_FlashRead(externalUserLicenseAddr + EXTENDED_USER_LICENSE_SECTION_1_START_ADDRESS_OFFSET_OFFSET, sizeof(UInt32), (UInt8*)&section1Offset);
    gpHal_FlashRead(externalUserLicenseAddr + EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET, sizeof(UInt32), (UInt8*)&section1Size);
    gpHal_FlashRead(externalUserLicenseAddr + EXTENDED_USER_LICENSE_SECTION_2_START_ADDRESS_OFFSET_OFFSET, sizeof(UInt32), (UInt8*)&section2Offset);
    gpHal_FlashRead(externalUserLicenseAddr + EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET, sizeof(UInt32), (UInt8*)&section2Size);

    UInt16 numSectors_section1 = ((section1Size + section1Offset - FLASH_ALIGN_SECTOR(section1Offset)) % FLASH_SECTOR_SIZE) == 0 ?
        ((section1Size + section1Offset - FLASH_ALIGN_SECTOR(section1Offset)) / FLASH_SECTOR_SIZE) :
        ((section1Size + section1Offset - FLASH_ALIGN_SECTOR(section1Offset)) / FLASH_SECTOR_SIZE) + 1;
    UInt16 numSectors_section2 = ((section2Size + section2Offset - FLASH_ALIGN_SECTOR(section2Offset)) % FLASH_SECTOR_SIZE) == 0 ?
        ((section2Size + section2Offset - FLASH_ALIGN_SECTOR(section2Offset)) / FLASH_SECTOR_SIZE) :
        ((section2Size + section2Offset - FLASH_ALIGN_SECTOR(section2Offset)) / FLASH_SECTOR_SIZE) + 1;
    UInt16 numSectors_licenses = ((LOADED_USER_LICENSE_TOTAL_SIZE + EXTENDED_USER_LICENSE_TOTAL_SIZE) % FLASH_SECTOR_SIZE) == 0 ?
        ((LOADED_USER_LICENSE_TOTAL_SIZE + EXTENDED_USER_LICENSE_TOTAL_SIZE) / FLASH_SECTOR_SIZE) :
        ((LOADED_USER_LICENSE_TOTAL_SIZE + EXTENDED_USER_LICENSE_TOTAL_SIZE) / FLASH_SECTOR_SIZE) + 1;

    /* Wipe all related areas */
    gpUpgrade_FlashErase(GP_MM_FLASH_ALT_START + FLASH_ALIGN_SECTOR(section1Offset), numSectors_section1);
    if (section2Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
        gpUpgrade_FlashErase(GP_MM_FLASH_ALT_START + FLASH_ALIGN_SECTOR(section2Offset), numSectors_section2);
    }
    gpUpgrade_FlashErase(appImageLowerFlashStart, numSectors_licenses);

    /* Install image part 1 */
    while(retries--)
    {
        if(gpUpgrade_FlashInstallImage(GP_MM_FLASH_ALT_START + section1Offset, section1Offset, section1Size) == gpUpgrade_StatusSuccess)
        {
            if(gpUpgrade_FlashCompare(GP_MM_FLASH_ALT_START + section1Offset, section1Offset, section1Size))
            {
                break;
            }
        }
    }

    /* Install image part 2 if needed */
    if (section2Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
        retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;
        while(retries--)
        {
            if(gpUpgrade_FlashInstallImage(GP_MM_FLASH_ALT_START + section2Offset, section2Offset, section2Size) == gpUpgrade_StatusSuccess)
            {
                if(gpUpgrade_FlashCompare(GP_MM_FLASH_ALT_START + section2Offset, section2Offset, section2Size))
                {
                    break;
                }
            }
        }
    }

    /* Install user license, copy from internal flash! */
    retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;
    gpUpgrade_SetFlashLoadSource(gpUpgrade_FlashLoadSourceInternal);
    while(retries--)
    {
        if(gpUpgrade_FlashInstallImage(appImageLowerFlashStart, externalUserLicenseAddr, LOADED_USER_LICENSE_TOTAL_SIZE + EXTENDED_USER_LICENSE_TOTAL_SIZE) == gpUpgrade_StatusSuccess)
        {
            if(gpUpgrade_FlashCompare(appImageLowerFlashStart, externalUserLicenseAddr, LOADED_USER_LICENSE_TOTAL_SIZE + EXTENDED_USER_LICENSE_TOTAL_SIZE))
            {
                gpUpgrade_SetFlashLoadSource(gpUpgrade_FlashLoadSourceExternal);
                return gpUpgrade_StatusSuccess;
            }
        }
    }

    gpUpgrade_SetFlashLoadSource(gpUpgrade_FlashLoadSourceExternal);
    return gpUpgrade_StatusFailedVerify;
}

#elif defined(GP_UPGRADE_DIVERSITY_COMPRESSION)

static gpUpgrade_Status_t Upgrade_InstallImage()
{
    UInt8 retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;

    UInt32 comprSection1Offset, comprSection1Size, comprSection1Addr;
    UInt32 comprSection2Offset, comprSection2Size;
    UInt32 otaSectionAddr;
    UInt32 AppLoadCompMW = LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD;

    gpHal_FlashError_t res = gpHal_FlashError_Success;

    const UInt32 compressedUserLicenseAddr = (UInt32)&upgrade_image_user_license_start__;

    // FIXME: return specific read error
    res = gpHal_FlashRead(compressedUserLicenseAddr + EXTENDED_USER_LICENSE_SECTION_1_START_ADDRESS_OFFSET_OFFSET, sizeof(UInt32), (UInt8*)&comprSection1Offset);
    
    if (res != gpHal_FlashError_Success)
    {
        return gpUpgrade_StatusFailedVerify;
    }
    res = gpHal_FlashRead(compressedUserLicenseAddr + EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET, sizeof(UInt32), (UInt8*)&comprSection1Size);
    if (res != gpHal_FlashError_Success)
    {
        return gpUpgrade_StatusFailedVerify;
    }
    res = gpHal_FlashRead(compressedUserLicenseAddr + EXTENDED_USER_LICENSE_SECTION_2_START_ADDRESS_OFFSET_OFFSET, sizeof(UInt32), (UInt8*)&comprSection2Offset);
    if (res != gpHal_FlashError_Success)
    {
        return gpUpgrade_StatusFailedVerify;
    }
    res = gpHal_FlashRead(compressedUserLicenseAddr + EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET, sizeof(UInt32), (UInt8*)&comprSection2Size);
    if (res != gpHal_FlashError_Success)
    {
        return gpUpgrade_StatusFailedVerify;
    }
    // Set addresses of compressed sections
    comprSection1Addr = GP_MM_FLASH_ALT_START + comprSection1Offset;

    // Validate the lzma image header
    if (lzma_IsValidInput((UInt8*)(comprSection1Addr), comprSection1Size) != lzma_ResultSuccess)
    {
        return gpUpgrade_StatusPreCheckFailed;
    }

    // Fetch decompressed image size from the compressed image header
    UInt32 decompressedSize = lzma_GetDecompressedSize((UInt8*)(comprSection1Addr), comprSection1Size);

    // Basic check, make sure that the decompressed image will not overwrite the ota area with the compressed image
    if (comprSection2Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
        otaSectionAddr = GP_MM_FLASH_ALT_START + comprSection2Offset;
    }
    else
    {
        otaSectionAddr = comprSection1Addr - FLASH_SECTOR_SIZE;
    }
    if ((appImageLowerFlashStart + decompressedSize) > (otaSectionAddr))
    {
        return gpUpgrade_StatusPreCheckFailed;
    }

    // Determine section sizes to erase
    UInt16 numSectors_section1 = ((decompressedSize) % FLASH_SECTOR_SIZE) == 0 ?
        ((decompressedSize) / FLASH_SECTOR_SIZE) :
        ((decompressedSize) / FLASH_SECTOR_SIZE) + 1;
    UInt16 numSectors_section2 = ((comprSection2Size) % FLASH_SECTOR_SIZE) == 0 ?
        ((comprSection2Size) / FLASH_SECTOR_SIZE) :
        ((comprSection2Size) / FLASH_SECTOR_SIZE) + 1;

    // Validate the output buffer is aligned
    if (lzma_IsValidOutput((UInt8*)appImageLowerFlashStart) != lzma_ResultSuccess)
    {
        return gpUpgrade_StatusPreCheckFailed;
    }

    /* Wipe all related areas */
    gpUpgrade_FlashErase(appImageLowerFlashStart, numSectors_section1);
    if (comprSection2Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
        gpUpgrade_FlashErase(GP_UPGRADE_APP_JUMP_TABLE_ADDR(appImageLowerFlashStart), numSectors_section2);

        /* Install jump tables */
        while(retries--)
        {
            if(gpUpgrade_FlashInstallImage(GP_UPGRADE_APP_JUMP_TABLE_ADDR(appImageLowerFlashStart), GP_MM_FLASH_ALT_START + comprSection2Offset, comprSection2Size) == gpUpgrade_StatusSuccess)
            {
                if(gpUpgrade_FlashCompare(GP_UPGRADE_APP_JUMP_TABLE_ADDR(appImageLowerFlashStart), GP_MM_FLASH_ALT_START + comprSection2Offset, comprSection2Size))
                {
                    break;
                }
            }
        }
        retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;
    }

    /* Install application image with license */
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
    HAL_SET_MCU_CLOCK_SPEED(GP_WB_ENUM_CLOCK_SPEED_M64);
#endif
    lzma_result lzmares = lzma_ResultDataError;
    while(retries--)
    {
        lzmares = lzma_Decode((UInt8*)comprSection1Addr, comprSection1Size, (UInt8*)appImageLowerFlashStart);
        if(lzmares == lzma_ResultSuccess)
        {
            // Still need to set the program loaded magic word, the license got extracted with the app and the MW is therefore not yet set
            if(gpHal_FlashProgramSector(appImageLowerFlashStart + LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET, sizeof(AppLoadCompMW), (UInt8*)&AppLoadCompMW) == gpHal_FlashError_Success)
            {
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
                HAL_SET_MCU_CLOCK_SPEED(GP_WB_ENUM_CLOCK_SPEED_M32);
#endif
                return gpUpgrade_StatusSuccess;
            }
        }
        else
        {
#if defined(GP_DIVERSITY_LOG)
            GP_LOG_PRINTF("ERR: lzma_Decode ret %ld",0, lzmares);
#endif
        }
    }
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
    HAL_SET_MCU_CLOCK_SPEED(GP_WB_ENUM_CLOCK_SPEED_M32);
#endif

    return gpUpgrade_StatusFailedVerify;
}

#endif

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/* Used in the bootloader build */
void gpUpgrade_selectActiveApplication (void)
{
    UInt32 lowerFlashAppLoadCompMW, upperFlashAppLoadCompMW;
    UInt8 lowerFlashAppFreshnessCntr, upperFlashAppFreshnessCntr;
    UInt32 lowerFlashAppCrc, upperFlashAppCrc;

    UInt8 flashSwapEnabled = 0;

#if   defined(GP_COMP_EXTSTORAGE) || defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
    /* The user license of the external image is copied to internal flash */
    const UInt32 appImageUpperFlashStart = (UInt32)&upgrade_image_user_license_start__;
#endif

    gpHal_FlashRead(appImageLowerFlashStart + LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET, sizeof(UInt32), (UInt8*)&lowerFlashAppLoadCompMW);
    gpHal_FlashRead(appImageUpperFlashStart + LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET, sizeof(UInt32), (UInt8*)&upperFlashAppLoadCompMW);

    gpHal_FlashRead(appImageLowerFlashStart + LOADED_USER_LICENSE_FRESHNESS_COUNTER_OFFSET, sizeof(UInt8), (UInt8*)&lowerFlashAppFreshnessCntr);
    gpHal_FlashRead(appImageUpperFlashStart + LOADED_USER_LICENSE_FRESHNESS_COUNTER_OFFSET, sizeof(UInt8), (UInt8*)&upperFlashAppFreshnessCntr);

    gpHal_FlashRead(appImageLowerFlashStart + USER_LICENSE_CRC_VALUE_OFFSET, sizeof(UInt32), (UInt8*)&lowerFlashAppCrc);
    gpHal_FlashRead(appImageUpperFlashStart + USER_LICENSE_CRC_VALUE_OFFSET, sizeof(UInt32), (UInt8*)&upperFlashAppCrc);

    /* If both flash areas have no valid load completed magic word, it
     * could be the first time an application is loaded into Flash and no
     * new firmware images have been downloaded yet
     */
    if ((lowerFlashAppLoadCompMW != LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD) &&
            (upperFlashAppLoadCompMW != LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD))
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("ERR: MW=%lx, but LowFSCompMW=%lx UpFSCompMW=%lx",0,LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD, lowerFlashAppLoadCompMW, upperFlashAppLoadCompMW);
        gpLog_Flush();
#endif
        return;
    }

    /*
     * Set higherFlashApp (in upper half of flash) as active if it has a higher freshness counter if both apps (lower &
     * upper flash) have valid load completed magic word OR if only higherFlashApp has valid load completed magic word
     */
    if ((lowerFlashAppLoadCompMW == LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD) &&
            (upperFlashAppLoadCompMW == LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD))
    {
        /* we have two valid applications, select the one with higher freshness counter */
        /* Note that this -1 is used, with the assumption that the freshness counter delta between high and low is always 1
         * This strategy then is immune to one of the counters overflowing */
        if ((UInt8)(upperFlashAppFreshnessCntr - 1) == lowerFlashAppFreshnessCntr)
        {
            flashSwapEnabled = 1;
        }
    } else if (upperFlashAppLoadCompMW == LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD)
    {
        flashSwapEnabled = 1;
    }

    // Check the CRC of the upgrade image
    if (flashSwapEnabled)
    {
        if (upperFlashAppCrc != gpUpgrade_HashImageCrc(appImageUpperFlashStart))
        {
            // Failed CRC don't upgrade!
            flashSwapEnabled = 0;
        }
    }

#if   defined(GP_COMP_EXTSTORAGE) || defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
    /* Copy the external image to internal flash if flashSwapEnabled == 1 */
    if (flashSwapEnabled)
    {
        Upgrade_InstallImage();
    }
#endif
}
