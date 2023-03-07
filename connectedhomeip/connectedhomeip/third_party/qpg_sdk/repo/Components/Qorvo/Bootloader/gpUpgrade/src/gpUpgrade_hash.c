/*
 *   Copyright (c) 2021, Qorvo Inc
 *
 *   Upgrade functionality
 *   Implementation of gpUpgrade hash actions
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
#include "gpUpgrade_flash.h"

#include "gpUpgrade_defs.h"

#if defined(GP_DIVERSITY_APP_LICENSE_BASED_BOOT)
#include "hal_user_license.h"
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/** @brief This function calculates a partial CRC over an address range in internal or external flash
*
*   @param crcVal     The pointer to the return value
*   @param address    The address on which to start the calculation
*   @param totalSize  Size over which to calculate
*/
void gpUpgrade_HashPartialCrc(UInt32 * crcVal, UInt32 address, UInt32 totalSize)
{
    UInt16 imgBlockLen = 0;
    UInt32 sz = 0;
    UInt32 imgAddr = address;
    UInt8 imgData[IMG_MAX_BLOCK_LEN];

    do
    {
        imgBlockLen = min(IMG_MAX_BLOCK_LEN, (totalSize - sz));
        if(gpUpgrade_StatusSuccess == gpUpgrade_FlashLoad(imgAddr, imgBlockLen, imgData))
        {
            gpUtils_CalculatePartialCrc32(crcVal, imgData, imgBlockLen);
        }
        else
        {
            *crcVal = 0xFFFFFFFF;
            break;
        }
        sz += imgBlockLen;
        imgAddr += imgBlockLen;

    }while(sz < totalSize);
}

#if defined(GP_DIVERSITY_APP_LICENSE_BASED_BOOT)
/** @brief This function calculates a CRC over an entire image
*
*   @param userLicenseAddress   The user license location with sections
*/
UInt32 gpUpgrade_HashImageCrc(UInt32 userLicenseAddress)
{
    UInt32 crcVal = 0;
    UInt32 section1Offset, section2Offset;
    UInt32 section1Size, section2Size;

    // Set licence len and declare array for adding the license to the CRC
    UInt16 licenceBlockLen = LOADED_USER_LICENSE_TOTAL_SIZE + EXTENDED_USER_LICENSE_TOTAL_SIZE - USER_LICENSE_VPP_OFFSET;
    UInt8 licData[LOADED_USER_LICENSE_TOTAL_SIZE + EXTENDED_USER_LICENSE_TOTAL_SIZE - USER_LICENSE_VPP_OFFSET];

    // Read out section sizes and address to determine crc range
    gpHal_FlashRead(userLicenseAddress + EXTENDED_USER_LICENSE_SECTION_1_START_ADDRESS_OFFSET_OFFSET, sizeof(UInt32), (UInt8*)&section1Offset);
    gpHal_FlashRead(userLicenseAddress + EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET, sizeof(UInt32), (UInt8*)&section1Size);
    gpHal_FlashRead(userLicenseAddress + EXTENDED_USER_LICENSE_SECTION_2_START_ADDRESS_OFFSET_OFFSET, sizeof(UInt32), (UInt8*)&section2Offset);
    gpHal_FlashRead(userLicenseAddress + EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET, sizeof(UInt32), (UInt8*)&section2Size);


    crcVal ^= GP_UTILS_CRC32_FINAL_XOR_VALUE;

    // Add the image section 1 to the CRC
#if (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))
    if (gpUpgrade_GetFlashLoadSource() == gpUpgrade_FlashLoadSourceExternal)
    {
        gpUpgrade_HashPartialCrc(&crcVal, section1Offset, section1Size);
    }
    else
#endif
    {
        gpUpgrade_HashPartialCrc(&crcVal, GP_MM_FLASH_ALT_START + section1Offset, section1Size);
    }

    // Add the image section 2 to the CRC
    if ((section2Size != 0x00) && (section2Size != 0xFFFFFFFF))
    {
#if (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))
    if (gpUpgrade_GetFlashLoadSource() == gpUpgrade_FlashLoadSourceExternal)
        {
            gpUpgrade_HashPartialCrc(&crcVal, section2Offset, section2Size);
        }
        else
#endif
        {
            gpUpgrade_HashPartialCrc(&crcVal, GP_MM_FLASH_ALT_START + section2Offset, section2Size);
        }
    }

    // Add user license to CRC, read from internal flash
    if(gpUpgrade_StatusSuccess == gpHal_FlashRead(userLicenseAddress + USER_LICENSE_VPP_OFFSET, licenceBlockLen, licData))
    {
        // Mask out Load Complete MW and freshness counter
        licData[USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET - USER_LICENSE_VPP_OFFSET] = 0;
        licData[USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET + 1 - USER_LICENSE_VPP_OFFSET] = 0;
        licData[USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET + 2 - USER_LICENSE_VPP_OFFSET] = 0;
        licData[USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET + 3 - USER_LICENSE_VPP_OFFSET] = 0;
        licData[USER_LICENSE_FRESHNESS_COUNTER_OFFSET - USER_LICENSE_VPP_OFFSET] = 0;

        // Calculate CRC
        gpUtils_CalculatePartialCrc32(&crcVal, licData, licenceBlockLen);
    }
    else
    {
        crcVal = 0xFFFFFFFF;
    }

    crcVal ^= GP_UTILS_CRC32_FINAL_XOR_VALUE;
    return crcVal;
}

#else
/** @brief This function calculates a CRC over an entire image
*
*   @param swUpgTab             The software upgrade table with offset, size and flags
*/
UInt32 gpUpgrade_HashImageCrc(gpUpgrade_Table_t swUpgTab)
{
    UInt32 crcVal = 0;

    crcVal ^= GP_UTILS_CRC32_FINAL_XOR_VALUE;

#if defined(GP_DIVERSITY_GPHAL_K8E)
    if ( swUpgTab.flagStatus & GP_UPGRADE_FLAG_JUMPTABLE_UPGRADE )
    {
        gpUpgrade_HashPartialCrc(&crcVal,
#if defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
                                    swUpgTab.memStartAddr - GP_UPGRADE_APP_JUMP_TABLE_SIZE,
#else
                                    GP_UPGRADE_APP_JUMP_TABLE_ADDR(swUpgTab.memStartAddr),
#endif
                                    GP_UPGRADE_APP_JUMP_TABLE_SIZE);
    }
#endif //GP_DIVERSITY_GPHAL_K8C || GP_DIVERSITY_GPHAL_K8D || GP_DIVERSITY_GPHAL_K8E

    gpUpgrade_HashPartialCrc(&crcVal, swUpgTab.memStartAddr, swUpgTab.memSz);

    crcVal ^= GP_UTILS_CRC32_FINAL_XOR_VALUE;
    return crcVal;
}
#endif
