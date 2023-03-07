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
#include "gpUpgrade.h"
#include "gpUpgrade_flash.h"
#include "gpUpgrade_defs.h"

#include "hal_user_license.h"
#include "gpSecureBoot.h"

#if (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))
#include "gpExtStorage.h"
#endif

#if defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
#include "lzma.h"
#endif

#if defined(GP_DIVERSITY_LOG)
#include "gpLog.h"
#endif

extern const UInt32 appImageLowerFlashStart;
#if   (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)) || defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
extern const UInt32 upgImageUserLicenseStart;
extern UInt32 umb_failed_copy_attempts_start__;
#define GP_UPGRADE_SECBOOT_EXTSTORAGE_MAX_ATTEMPTS 5
#endif

#if !defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)
#error define secure bootloader
#endif

#if (GP_DIVERSITY_FLASH_APP_START_OFFSET != 0x6000) && (GP_DIVERSITY_FLASH_APP_START_OFFSET != 0x8000)
#error invalid app start offset used
#endif


#if defined(GP_UPGRADE_DIVERSITY_COMPRESSION) || (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))
static void Upgrade_SecureBoot_InstallImage(UInt32 upgImageUserLicenseStart);
#endif

void gpUpgrade_SecureBoot_LockBootloader(void)
{
    #if (GP_DIVERSITY_FLASH_APP_START_OFFSET == 0x6000)
    /* Runtime write lock of bootloader, excluding jumptable area and flash swap and RMA status sector */
    UInt32 block0 = GPHAL_FLASH_WRITE_LOCK_REGION_0_2K | // Lock native user license
    /* Keep jumptable 4K area (2K-6K) unlocked */
    GPHAL_FLASH_WRITE_LOCK_REGION_6_8K |
    GPHAL_FLASH_WRITE_LOCK_REGION_8_10K |
    GPHAL_FLASH_WRITE_LOCK_REGION_10_12K |
    GPHAL_FLASH_WRITE_LOCK_REGION_12_14K |
    GPHAL_FLASH_WRITE_LOCK_REGION_14_16K |
    GPHAL_FLASH_WRITE_LOCK_REGION_16_18K |
    GPHAL_FLASH_WRITE_LOCK_REGION_18_20K |
    /* GPHAL_FLASH_WRITE_LOCK_REGION_20_22K; */ // flash swap status and failed attempts counter
    GPHAL_FLASH_WRITE_LOCK_REGION_22_24K;

    #elif (GP_DIVERSITY_FLASH_APP_START_OFFSET == 0x8000)
    UInt32 block0 = GPHAL_FLASH_WRITE_LOCK_REGION_0_2K | // Lock native user license
    /* Keep jumptable 4K area (2K-6K) unlocked */
    GPHAL_FLASH_WRITE_LOCK_REGION_6_8K |
    GPHAL_FLASH_WRITE_LOCK_REGION_8_10K |
    GPHAL_FLASH_WRITE_LOCK_REGION_10_12K |
    GPHAL_FLASH_WRITE_LOCK_REGION_12_14K |
    GPHAL_FLASH_WRITE_LOCK_REGION_14_16K |
    GPHAL_FLASH_WRITE_LOCK_REGION_16_18K |
    GPHAL_FLASH_WRITE_LOCK_REGION_18_20K |
    GPHAL_FLASH_WRITE_LOCK_REGION_20_22K |
    GPHAL_FLASH_WRITE_LOCK_REGION_22_24K |
    GPHAL_FLASH_WRITE_LOCK_REGION_24_26K |
    GPHAL_FLASH_WRITE_LOCK_REGION_26_28K |
    /* GPHAL_FLASH_WRITE_LOCK_REGION_28_30K */ // flash swap status and failed attempts counter
    GPHAL_FLASH_WRITE_LOCK_REGION_30_32K;

    #endif

    /* Apply write locks */
    gpHal_FlashWriteLockFineCoarse(block0,0,0,0);

     /* Lock Redundant sector remaps */
    gpHal_FlashLockRedundantSectors();

}

#if   defined(GP_UPGRADE_DIVERSITY_COMPRESSION) || (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))

void gpUpgrade_SecureBoot_selectActiveApplication (void)
{
    Bool upgImgValid = true;
    Bool actImgValid = true;

    UInt32 actFlashAppLoadCompMW, upgFlashAppLoadCompMW;
    UInt32 actFlashAppLoadedMW, upgFlashAppLoadedMW;
    UInt32 actFlashAppRollbackProtectionCounter, upgFlashAppRollbackProtectionCounter;
    UInt8 actFlashAppFreshnessCntr, upgFlashAppFreshnessCntr;
    const UInt32 copyAttemptsAddr = (UInt32)&umb_failed_copy_attempts_start__;

    UInt8 copyAttempts = (*(UInt8 *)copyAttemptsAddr) + 1;

    if (copyAttempts > GP_UPGRADE_SECBOOT_EXTSTORAGE_MAX_ATTEMPTS)
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("Max copy attempts exceeded!",0);
        HAL_WAIT_MS(1000);
#endif
        gpUpgrade_SecureBoot_cbFatalError(gpUpgrade_SecureBoot_StatusMaxCopyAttemptsExceeded);
    }
    else
    {
        /* Application image at internal and external flash area has to be at same offset from internal and external flash_start respectively*/
        gpHal_FlashRead(appImageLowerFlashStart + LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET, sizeof(UInt32), (UInt8*)&actFlashAppLoadCompMW);
        gpHal_FlashRead(upgImageUserLicenseStart + LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET, sizeof(UInt32), (UInt8*)&upgFlashAppLoadCompMW);

        gpHal_FlashRead(appImageLowerFlashStart + LOADED_USER_LICENSE_FRESHNESS_COUNTER_OFFSET, sizeof(UInt8), (UInt8*)&actFlashAppFreshnessCntr);
        gpHal_FlashRead(upgImageUserLicenseStart + LOADED_USER_LICENSE_FRESHNESS_COUNTER_OFFSET, sizeof(UInt8), (UInt8*)&upgFlashAppFreshnessCntr);

        gpHal_FlashRead(appImageLowerFlashStart + USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD_OFFSET, sizeof(UInt32), (UInt8*)&actFlashAppLoadedMW);
        gpHal_FlashRead(upgImageUserLicenseStart + USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD_OFFSET, sizeof(UInt32), (UInt8*)&upgFlashAppLoadedMW);

        gpHal_FlashRead(appImageLowerFlashStart + EXTENDED_USER_LICENSE_ROLLBACK_PROTECTION_OFFSET, sizeof(UInt32), (UInt8*)&actFlashAppRollbackProtectionCounter);
        gpHal_FlashRead(upgImageUserLicenseStart + EXTENDED_USER_LICENSE_ROLLBACK_PROTECTION_OFFSET, sizeof(UInt32), (UInt8*)&upgFlashAppRollbackProtectionCounter);

        /* If both images don't have the program loaded MW set... */
        if (actFlashAppLoadedMW != USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD)
        {
#if defined(GP_DIVERSITY_LOG)
            GP_LOG_SYSTEM_PRINTF("Active Img: Program loaded MW incorrect",0);
            HAL_WAIT_MS(1000);
#endif
            actImgValid = false;
        }

        if (upgFlashAppLoadedMW != USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD)
        {
#if defined(GP_DIVERSITY_LOG)
            GP_LOG_SYSTEM_PRINTF("Upgrade Img: PL MW incorrect",0);
            HAL_WAIT_MS(1000);
#endif
            upgImgValid = false;
        }

        if (actImgValid)
        {
            if (actFlashAppLoadCompMW != LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD)
            {
#if defined(GP_DIVERSITY_LOG)
                GP_LOG_SYSTEM_PRINTF("Active Img: LC MW incorrect",0);
                HAL_WAIT_MS(100);
#endif
                actImgValid = false;
            }
        }

        if (upgImgValid)
        {
            if (upgFlashAppLoadCompMW != LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD)
            {
#if defined(GP_DIVERSITY_LOG)
                GP_LOG_SYSTEM_PRINTF("Upgrade Img: LC MW incorrect",0);
                HAL_WAIT_MS(100);
#endif
                upgImgValid = false;
            }
        }

        /* Authenticate images */
        if (actImgValid)
        {
            if(gpSecureBoot_AuthenticateImage(GP_MM_FLASH_ALT_START, GP_DIVERSITY_FLASH_APP_START_OFFSET) == false)
            {
#if defined(GP_DIVERSITY_LOG)
                GP_LOG_SYSTEM_PRINTF("Active Img: Authentication failed",0);
                HAL_WAIT_MS(1000);
#endif
                actImgValid = false;
            }
        }

        if (upgImgValid)
        {
#if defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
            if(gpSecureBoot_AuthenticateImage(GP_MM_FLASH_ALT_START, upgImageUserLicenseStart & 0xFFFFF) == false)
#elif (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))
            if(gpSecureBoot_ExtStorage_AuthenticateImage(0x0, upgImageUserLicenseStart) == false)
#endif
            {
#if defined(GP_DIVERSITY_LOG)
                GP_LOG_SYSTEM_PRINTF("Upgrade Img: Authentication failed",0);
                HAL_WAIT_MS(1000);
#endif
                upgImgValid = false;
            }
        }

        if(actImgValid == false && upgImgValid == false)
        {
#if defined(GP_DIVERSITY_LOG)
            GP_LOG_SYSTEM_PRINTF("Both images not valid, abort",0);
            HAL_WAIT_MS(1000);
#endif
            gpUpgrade_SecureBoot_cbFatalError(gpUpgrade_SecureBoot_StatusNoValidImage);

        }
        else if(actImgValid == true && upgImgValid == false)
        {
            // No action required
#if defined(GP_DIVERSITY_LOG)
            GP_LOG_SYSTEM_PRINTF("Booting active image",0);
            HAL_WAIT_MS(1000);
#endif
        }
        else
        {
            //Check freshness counter in case active image is valid
            if (actImgValid == true)
            {
#if defined(GP_DIVERSITY_LOG)
                GP_LOG_SYSTEM_PRINTF("Both images valid, checking freshness counters",0);
                HAL_WAIT_MS(100);
#endif
                if ((upgFlashAppFreshnessCntr - 1) == actFlashAppFreshnessCntr)
                {
#if defined(GP_DIVERSITY_LOG)
                    GP_LOG_SYSTEM_PRINTF("Upgrade image is fresher, checking rollbackprotection counters",0);
                    HAL_WAIT_MS(100);
#endif
                    if(actFlashAppRollbackProtectionCounter <= upgFlashAppRollbackProtectionCounter)
                    {
#if defined(GP_DIVERSITY_LOG)
                        GP_LOG_SYSTEM_PRINTF("OK, invalidating active image",0);
                        HAL_WAIT_MS(100);
#endif
                        //Set active image valid to false to start the update below
                        actImgValid = false;
                    }
                }
            }

            // Wipe active flash and overwrite with upgrade flash image
            if (actImgValid == false)
            {

#if defined(GP_DIVERSITY_LOG)
                GP_LOG_SYSTEM_PRINTF("Overwrite active image with valid upgrade image",0);
                HAL_WAIT_MS(1000);
#endif
                // Install the upgrade image
                Upgrade_SecureBoot_InstallImage(upgImageUserLicenseStart);

                // Check the newly written image for authenticity before rebooting
                // This is to avoid attacks on the SPI lines where data is inserted during the program read
                gpHal_FlashRead(appImageLowerFlashStart + LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET, sizeof(UInt32), (UInt8*)&actFlashAppLoadCompMW);
                gpHal_FlashRead(appImageLowerFlashStart + USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD_OFFSET, sizeof(UInt32), (UInt8*)&actFlashAppLoadedMW);
                actImgValid = true;

                if (actFlashAppLoadedMW != USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD)
                {
#if defined(GP_DIVERSITY_LOG)
                    GP_LOG_SYSTEM_PRINTF("After install: Active Img: Program loaded MW incorrect",0);
                    HAL_WAIT_MS(1000);
#endif
                    actImgValid = false;
                }

                /* Authenticate active image */
                if (actImgValid)
                {
                    if(gpSecureBoot_AuthenticateImage(GP_MM_FLASH_ALT_START, GP_DIVERSITY_FLASH_APP_START_OFFSET) == false)
                    {
#if defined(GP_DIVERSITY_LOG)
                        GP_LOG_SYSTEM_PRINTF("After install: Active Img: Authentication failed",0);
                        HAL_WAIT_MS(1000);
#endif
                        actImgValid = false;
                    }
                }

                if (actImgValid)
                {
#if defined(GP_DIVERSITY_LOG)
                    GP_LOG_SYSTEM_PRINTF("After install: upgrade succesfull, restarting",0);
                    HAL_WAIT_MS(1000);
#endif
                    // Reset copy attempts to zero
                    gpHal_FlashError_t ret;
                    copyAttempts = 0;
                    ret = gpHal_FlashProgramSector(copyAttemptsAddr, sizeof(copyAttempts), (UInt8*)&copyAttempts);
                    GP_ASSERT_SYSTEM(ret == gpHal_FlashError_Success);
                }
                else
                {
#if defined(GP_DIVERSITY_LOG)
                    GP_LOG_SYSTEM_PRINTF("After install: Active image not valid, clearing MW",0);
                    HAL_WAIT_MS(1000);
#endif
                    gpHal_FlashError_t ret;
                    UInt32 loadCompleteMW = 0;
                    ret = gpHal_FlashProgramSector(appImageLowerFlashStart + LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET, sizeof(loadCompleteMW), (UInt8*)&loadCompleteMW);
                    GP_ASSERT_SYSTEM(ret == gpHal_FlashError_Success);
                    ret = gpHal_FlashProgramSector(copyAttemptsAddr, sizeof(copyAttempts), (UInt8*)&copyAttempts);
                    GP_ASSERT_SYSTEM(ret == gpHal_FlashError_Success);
                }
            }
            else
            {
#if defined(GP_DIVERSITY_LOG)
                GP_LOG_SYSTEM_PRINTF("Active image selected",0);
                HAL_WAIT_MS(100);
#endif
            }
        }
    }
}

#if defined(GP_UPGRADE_DIVERSITY_COMPRESSION)

static void Upgrade_SecureBoot_InstallImage(UInt32 upgImageUserLicenseStart)
{
    UInt32 section1Offset, section1Size;
    UInt32 section2Offset, section2Size;
    UInt32 otaSectionAddr;
    UInt32 AppLoadCompMW = LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD;

    gpHal_FlashRead(upgImageUserLicenseStart + EXTENDED_USER_LICENSE_SECTION_1_START_ADDRESS_OFFSET_OFFSET, sizeof(UInt32), (UInt8*)&section1Offset);
    gpHal_FlashRead(upgImageUserLicenseStart + EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET, sizeof(UInt32), (UInt8*)&section1Size);
    gpHal_FlashRead(upgImageUserLicenseStart + EXTENDED_USER_LICENSE_SECTION_2_START_ADDRESS_OFFSET_OFFSET, sizeof(UInt32), (UInt8*)&section2Offset);
    gpHal_FlashRead(upgImageUserLicenseStart + EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET, sizeof(UInt32), (UInt8*)&section2Size);

    // Set addresses of compressed sections
    section1Offset += GP_MM_FLASH_ALT_START;
    if (section2Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
        section2Offset += GP_MM_FLASH_ALT_START;
    }

    // Validate the lzma image header
    GP_ASSERT_SYSTEM(lzma_IsValidInput((UInt8*)(section1Offset), section1Size) == lzma_ResultSuccess);

    // Fetch decompressed image size from the compressed image header
    UInt32 decompressedSize = lzma_GetDecompressedSize((UInt8*)(section1Offset), section1Size);

    // Basic check, make sure that the decompressed image will not overwrite the ota area with the compressed image
    if (section2Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
        otaSectionAddr = section2Offset;
    }
    else
    {
        otaSectionAddr = section1Offset - FLASH_SECTOR_SIZE;
    }
    GP_ASSERT_SYSTEM((appImageLowerFlashStart + decompressedSize) <= (otaSectionAddr));

    // Determine section sizes to erase
    UInt8 retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;
    UInt16 numSectors_section1 = ((decompressedSize) % FLASH_SECTOR_SIZE) == 0 ?
        ((decompressedSize) / FLASH_SECTOR_SIZE) :
        ((decompressedSize) / FLASH_SECTOR_SIZE) + 1;
    UInt16 numSectors_section2 = ((section2Size) % FLASH_SECTOR_SIZE) == 0 ?
        ((section2Size) / FLASH_SECTOR_SIZE) :
        ((section2Size) / FLASH_SECTOR_SIZE) + 1;

    // Validate the output buffer is aligned
    GP_ASSERT_SYSTEM(lzma_IsValidOutput((UInt8*)appImageLowerFlashStart) == lzma_ResultSuccess);

    /* Wipe all related areas */
    gpUpgrade_FlashErase(appImageLowerFlashStart, numSectors_section1);
    if (section2Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
        gpUpgrade_FlashErase(GP_UPGRADE_APP_JUMP_TABLE_ADDR(appImageLowerFlashStart), numSectors_section2);

        /* Install jump tables */
        while(retries--)
        {
            if(gpUpgrade_FlashInstallImage(GP_UPGRADE_APP_JUMP_TABLE_ADDR(appImageLowerFlashStart), section2Offset, section2Size) == gpUpgrade_StatusSuccess)
            {
                break;
            }
#if defined(GP_DIVERSITY_LOG)
            else
            {
                GP_LOG_SYSTEM_PRINTF("Retry %u/%u Install Program section 2 failed!",0, GP_UPGRADE_UPGRADE_MAX_RETRIES - retries, GP_UPGRADE_UPGRADE_MAX_RETRIES);
                HAL_WAIT_MS(100);
            }
#endif
        }
        retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;
    }

    /* Install application image with license */
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
    HAL_SET_MCU_CLOCK_SPEED(GP_WB_ENUM_CLOCK_SPEED_M64);
#endif
    while(retries--)
    {
        if(lzma_Decode((UInt8*)section1Offset, section1Size, (UInt8*)appImageLowerFlashStart) == lzma_ResultSuccess)
        {
            // Still need to set the program loaded magic word, the license got extracted with the app and the MW is therefore not yet set
            if(gpHal_FlashProgramSector(appImageLowerFlashStart + LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET, sizeof(AppLoadCompMW), (UInt8*)&AppLoadCompMW) == gpHal_FlashError_Success)
            {
                retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;
                break;
            }
        }
#if defined(GP_DIVERSITY_LOG)
        else
        {
            GP_LOG_SYSTEM_PRINTF("Retry %u/%u Extract Program section 1 failed!",0, GP_UPGRADE_UPGRADE_MAX_RETRIES - retries, GP_UPGRADE_UPGRADE_MAX_RETRIES);
            HAL_WAIT_MS(100);
        }
#endif
    }
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
    HAL_SET_MCU_CLOCK_SPEED(GP_WB_ENUM_CLOCK_SPEED_M32);
#endif

    if (retries != GP_UPGRADE_UPGRADE_MAX_RETRIES)
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("New image not installed!",0);
        HAL_WAIT_MS(100);
#endif
    }
}

#elif (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))

static void Upgrade_SecureBoot_InstallImage(UInt32 upgImageUserLicenseStart)
{
    UInt32 section1Offset;
    UInt32 section1Size;
    UInt32 section2Offset;
    UInt32 section2Size;

    gpHal_FlashRead(upgImageUserLicenseStart + EXTENDED_USER_LICENSE_SECTION_1_START_ADDRESS_OFFSET_OFFSET, sizeof(UInt32), (UInt8*)&section1Offset);
    gpHal_FlashRead(upgImageUserLicenseStart + EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET, sizeof(UInt32), (UInt8*)&section1Size);
    gpHal_FlashRead(upgImageUserLicenseStart + EXTENDED_USER_LICENSE_SECTION_2_START_ADDRESS_OFFSET_OFFSET, sizeof(UInt32), (UInt8*)&section2Offset);
    gpHal_FlashRead(upgImageUserLicenseStart + EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET, sizeof(UInt32), (UInt8*)&section2Size);

    UInt8 retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;

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
            break;
        }
#if defined(GP_DIVERSITY_LOG)
        else
        {
            GP_LOG_SYSTEM_PRINTF("Retry %u/%u Install Program section 1 failed!",0, GP_UPGRADE_UPGRADE_MAX_RETRIES - retries, GP_UPGRADE_UPGRADE_MAX_RETRIES);
            HAL_WAIT_MS(100);
        }
#endif
    }

    /* Install image part 2 */
    retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;
    while(retries--)
    {
        if(gpUpgrade_FlashInstallImage(GP_MM_FLASH_ALT_START + section2Offset, section2Offset, section2Size) == gpUpgrade_StatusSuccess)
        {
            break;
        }
#if defined(GP_DIVERSITY_LOG)
        else
        {
            GP_LOG_SYSTEM_PRINTF("Retry %u/%u Install Program section 2 failed!",0, GP_UPGRADE_UPGRADE_MAX_RETRIES - retries, GP_UPGRADE_UPGRADE_MAX_RETRIES);
            HAL_WAIT_MS(100);
        }
#endif
    }

    /* Install user license, copy from internal flash! */
    retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;
    gpUpgrade_SetFlashLoadSource(gpUpgrade_FlashLoadSourceInternal);
    while(retries--)
    {
        if(gpUpgrade_FlashInstallImage(appImageLowerFlashStart, upgImageUserLicenseStart, LOADED_USER_LICENSE_TOTAL_SIZE + EXTENDED_USER_LICENSE_TOTAL_SIZE) == gpUpgrade_StatusSuccess)
        {
            gpUpgrade_SetFlashLoadSource(gpUpgrade_FlashLoadSourceExternal);
            retries = GP_UPGRADE_UPGRADE_MAX_RETRIES;
            break;
        }
#if defined(GP_DIVERSITY_LOG)
        else
        {
            GP_LOG_SYSTEM_PRINTF("Retry %u/%u Install Licenses failed!",0, GP_UPGRADE_UPGRADE_MAX_RETRIES - retries, GP_UPGRADE_UPGRADE_MAX_RETRIES);
            HAL_WAIT_MS(100);
        }
#endif
    }

    if (retries != GP_UPGRADE_UPGRADE_MAX_RETRIES)
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("New image not installed!",0);
        HAL_WAIT_MS(100);
#endif
    }
}

#endif

#else
#error "Secure bootloader only works in combination with either flash remapping, external flash or compressed ota"
#endif

void hal__AtomicOn(void)
{

}

void hal__AtomicOff (void)
{

}
