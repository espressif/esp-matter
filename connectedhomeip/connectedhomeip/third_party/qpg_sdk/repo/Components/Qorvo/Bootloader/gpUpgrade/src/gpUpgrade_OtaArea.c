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
#ifndef GP_DIVERSITY_BOOTLOADER_BUILD
#include "gpLog.h"
#endif //"GP_DIVERSITY_BOOTLOADER_BUILD"

#include "gpUpgrade_defs.h"

#if defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)
#include "gpBsp.h"
#include "gpExtStorage.h"
#endif //GP_COMP_EXTSTORAGE

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define FLASH_ALIGN_SECTOR(address)                     ((address) - ((address) % FLASH_SECTOR_SIZE))
#define FLASH_RANGE_IN_SINGLE_SECTOR(address, length)   ((length) <= (FLASH_SECTOR_SIZE - ((address) % FLASH_SECTOR_SIZE)))
#define FLASH_TO_SECTOR_END(address)                    (FLASH_SECTOR_SIZE - ((address) % FLASH_SECTOR_SIZE))

#if defined(GP_DIVERSITY_GPHAL_K8E)
#ifdef GP_UPGRADE_DIVERSITY_COMPRESSION
// For compression use cases the position of the jumptable is fixed before the OTA area
#define GP_OTA_JT_FLASH_START                           (GP_OTA_FLASH_START - GP_UPGRADE_APP_JUMP_TABLE_SIZE)
#define GP_OTA_JT_FLASH_END                             (GP_OTA_FLASH_START) 
#else
// For other cases
#define GP_OTA_JT_FLASH_START                           (GP_UPGRADE_APP_JUMP_TABLE_ADDR(GP_OTA_FLASH_START))
#define GP_OTA_JT_FLASH_END                             (GP_UPGRADE_APP_JUMP_TABLE_ADDR(GP_OTA_FLASH_START) + GP_UPGRADE_APP_JUMP_TABLE_SIZE) 
#endif //GP_UPGRADE_DIVERSITY_COMPRESSION
#endif //defined(GP_DIVERSITY_GPHAL_K8C) || defined(GP_DIVERSITY_GPHAL_K8D) || defined(GP_DIVERSITY_GPHAL_K8E)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
#ifdef GP_DIVERSITY_APP_LICENSE_BASED_BOOT
extern const UInt32 __app_Start__;
const UInt32 appImageLowerFlashStart = ((UInt32) &__app_Start__) - GP_MM_FLASH_START + GP_MM_FLASH_ALT_START;       /* App start addr (lower flash area) from linker */
#if (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)) || defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
extern const UInt32 upgrade_image_user_license_start__;
const UInt32 upgImageUserLicenseStart = ((UInt32) &upgrade_image_user_license_start__);       /* from linker */
#endif
#endif

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
UInt32 gpUpgrade_Crc = 0;

#ifndef GP_DIVERSITY_BOOTLOADER_BUILD
//defined in linker script
extern UInt32 OTA_Start;
extern UInt32 OTA_End;
#endif

#if defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)
#if defined(GP_DIVERSITY_FLASH_APP_START_OFFSET)
const UInt32 GP_OTA_FLASH_START = GP_DIVERSITY_FLASH_APP_START_OFFSET;
#else
const UInt32 GP_OTA_FLASH_START = 0x000000;
#endif
const UInt32 GP_OTA_FLASH_END = SPI_FLASH_SIZE;
#else //GP_COMP_EXTSTORAGE
#ifndef GP_DIVERSITY_BOOTLOADER_BUILD
const UInt32 GP_OTA_FLASH_START = (UInt32)&OTA_Start;
const UInt32 GP_OTA_FLASH_END = (UInt32)&OTA_End;
#endif
#endif //GP_COMP_EXTSTORAGE
/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
#if (!defined(GP_COMP_EXTSTORAGE) || defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)) && !defined(GP_DIVERSITY_BOOTLOADER_BUILD)
static gpUpgrade_Status_t Upgrade_WriteChunk(UInt32 address, UInt16 length, UInt8* data)
{
    gpUpgrade_Status_t otaResult;
    gpHal_FlashError_t result;

    result = gpHal_FlashProgramSector(address, length, data);

    if(result != gpHal_FlashError_Success)
    {
        GP_LOG_SYSTEM_PRINTF("OTA chunk write failed (error %x)", 0 , result);
        otaResult = gpUpgrade_StatusWriteError;
    }
    else
    {
        otaResult = gpUpgrade_StatusSuccess;
    }
    return otaResult;
}
#endif // !defined(GP_COMP_EXTSTORAGE) && !defined(GP_DIVERSITY_BOOTLOADER_BUILD)

#ifndef GP_DIVERSITY_BOOTLOADER_BUILD
static Bool Upgrade_CheckValidAddressRange(UInt32 address)
{
    Bool retVal = false;

    //Address in range of defined OTA section?
    if ( (address >= GP_OTA_FLASH_START) && (address < GP_OTA_FLASH_END) )
    {
        retVal = true;
    }

#ifdef GP_DIVERSITY_JUMPTABLES
    //Address in range of defined jumptable section?
#if defined(GP_UPGRADE_DIVERSITY_COMPRESSION)
    // For builds with compressed OTA, the jumptables ota chunk is just in front of the OTA area
    else if ( address >= GP_OTA_FLASH_START - GP_UPGRADE_APP_JUMP_TABLE_SIZE &&
              address <  GP_OTA_FLASH_START )
#else
    // For other builds, the jumptables ota chunk should be at the fixed offset location
    else if ( address >= GP_UPGRADE_APP_JUMP_TABLE_ADDR(GP_OTA_FLASH_START) &&
              address < (GP_UPGRADE_APP_JUMP_TABLE_ADDR(GP_OTA_FLASH_START) + GP_UPGRADE_APP_JUMP_TABLE_SIZE))
#endif
    {
        retVal = true;
    }
#endif //GP_DIVERSITY_JUMPTABLES


    return retVal;
}
#endif // !GP_DIVERSITY_BOOTLOADER_BUILD
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
//CRC helper functions
void gpUpgrade_StartOrEndCrc(UInt32 * crcVal)
{
    if ( crcVal != NULL )
    {
        //new CRC calculation
        *crcVal ^= GP_UTILS_CRC32_FINAL_XOR_VALUE;
    }
    else
    {
#if !defined(GP_OTA_DIVERSITY_CLIENT) || defined(GP_OTA_DIVERSITY_SERIAL_TEST)//OTA manager will do CRC checking, not needed to do it in the UMB
        //use bootloader crc
        gpUpgrade_Crc ^= GP_UTILS_CRC32_FINAL_XOR_VALUE;
#else //GP_OTA_DIVERSITY_CLIENT
        gpUpgrade_Crc = 0xFFFFFFFF;
#endif //GP_OTA_DIVERSITY_CLIENT
    }

}

void gpUpgrade_CalculatePartialCrc(UInt32 * crcVal, UInt32 length, UInt8* pData)
{
    if ( crcVal != NULL )
    {
        //new CRC calculation
        gpUtils_CalculatePartialCrc32(crcVal, pData, length);
    }
    else
    {
        //use bootloader crc
        gpUtils_CalculatePartialCrc32(&gpUpgrade_Crc, pData, length);
    }
}

//OTA area functionality

#ifndef GP_DIVERSITY_BOOTLOADER_BUILD
UInt32 gpUpgrade_GetCrc(void)
{
    return gpUpgrade_Crc;
}

void gpUpgrade_ClrCrc(void)
{
    gpUpgrade_Crc = 0;
}

void gpUpgrade_SetCrc(UInt32 crcValue)
{
    gpUpgrade_Crc = crcValue;
}

void gpUpgrade_EraseOtaArea(void)
{
#if defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)
    gpExtStorage_Erase();
#else //GP_COMP_EXTSTORAGE

#endif // GP_COMP_EXTSTORAGE

}

void gpUpgrade_StartEraseOtaArea(gpUpgrade_cbEraseComplete_t cb)
{
#if defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)
    gpExtStorage_EraseNoBlock(cb);
#else // GP_COMP_EXTSTORAGE
    if(cb)
    {
        cb();
    }
#endif // GP_COMP_EXTSTORAGE
}

#ifndef GP_DIVERSITY_APP_LICENSE_BASED_BOOT
UInt32 gpUpgrade_GetOtaAreaStartAddress(void)
{
    return GP_OTA_FLASH_START;
}
#else /* GP_DIVERSITY_APP_LICENSE_BASED_BOOT */
UInt32 gpUpgrade_GetOtaAreaStartAddress(void)
{
    UInt32 otaAreaStartAddr;
    otaAreaStartAddr = GP_OTA_FLASH_START;

    return otaAreaStartAddr;
}
#endif /* GP_DIVERSITY_APP_LICENSE_BASED_BOOT */

UInt32 gpUpgrade_GetJumptableOtaAreaStartAddress(void)
{
#if defined(GP_DIVERSITY_GPHAL_K8E)
    return GP_OTA_JT_FLASH_START;
#else
    return 0;
#endif //defined(GP_DIVERSITY_GPHAL_K8C) || defined(GP_DIVERSITY_GPHAL_K8D) || defined(GP_DIVERSITY_GPHAL_K8E)
}

UInt32 gpUpgrade_GetJumptableOtaAreaSize(void)
{
#if defined(GP_DIVERSITY_GPHAL_K8E)
    return GP_OTA_JT_FLASH_END - GP_OTA_JT_FLASH_START;
#else
    return 0;
#endif //defined(GP_DIVERSITY_GPHAL_K8C) || defined(GP_DIVERSITY_GPHAL_K8D) || defined(GP_DIVERSITY_GPHAL_K8E)
}

UInt32 gpUpgrade_GetOtaAreaSize(void)
{
    return GP_OTA_FLASH_END - GP_OTA_FLASH_START;
}

void gpUpgrade_StartWrite(void)
{
#if !defined(GP_OTA_DIVERSITY_CLIENT) || defined(GP_OTA_DIVERSITY_SERIAL_TEST)//OTA manager will do CRC checking, not needed to do it in the UMB
    if(gpUpgrade_Crc != 0x0)
    {
        GP_LOG_SYSTEM_PRINTF("Overwriting already active session",0);
    }
    gpUpgrade_Crc = 0x0;

    //Init CRC according to bootloader calculation
    gpUpgrade_StartOrEndCrc(NULL);
#else //GP_OTA_DIVERSITY_CLIENT
    gpUpgrade_Crc = 0xFFFFFFFF;
#endif //GP_OTA_DIVERSITY_CLIENT
}


gpUpgrade_Status_t gpUpgrade_WriteChunk(UInt32 address, UInt16 length, UInt8* dataChunk)
{
    gpUpgrade_Status_t otaResult = gpUpgrade_StatusSuccess;

    if (length == 0)
    {
        return gpUpgrade_StatusInvalidArgument;
    }

    if (!Upgrade_CheckValidAddressRange(address) || !Upgrade_CheckValidAddressRange(address + length - 1))
    {
        return gpUpgrade_StatusInvalidAddress;
    }

    //if using external flash, call spi flash write block function
#if defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)
    otaResult = gpExtStorage_WriteBlock(address, length, dataChunk);
#else
    if(length > FLASH_SECTOR_SIZE)
    {
        return gpHal_FlashError_OutOfRange;
    }

    //split in 2 chunks
    if(!FLASH_RANGE_IN_SINGLE_SECTOR(address, length))
    {
        UInt16 chunksize = FLASH_TO_SECTOR_END(address);
        UInt16 leftsize = length - FLASH_TO_SECTOR_END(address);

        otaResult = Upgrade_WriteChunk(address, chunksize, dataChunk);
        if(otaResult == gpUpgrade_StatusSuccess)
        {
            otaResult = Upgrade_WriteChunk(address+chunksize, leftsize, &dataChunk[chunksize]);
        }
    }
    else
    {
        otaResult = Upgrade_WriteChunk(address, length, dataChunk);
    }
#endif //GP_COMP_EXTSTORAGE

#if !defined(GP_OTA_DIVERSITY_CLIENT) || defined(GP_OTA_DIVERSITY_SERIAL_TEST) //OTA manager will do CRC checking, not needed to do it in the UMB
    //Update CRC
    gpUpgrade_CalculatePartialCrc(NULL, length, dataChunk);
#endif //GP_OTA_DIVERSITY_CLIENT

    return otaResult;
}
gpUpgrade_Status_t gpUpgrade_ReadChunk(UInt32 address, UInt16 length, UInt8* dataChunk)
{
    gpUpgrade_Status_t status = gpUpgrade_StatusSuccess;

    if (!Upgrade_CheckValidAddressRange(address))
    {
        return gpUpgrade_StatusInvalidAddress;
    }

    //if using external flash, call spi flash read block function
#if defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE)
    switch(gpExtStorage_ReadBlock(address, length, dataChunk))
    {
        case gpExtStorage_Success:
            status = gpUpgrade_StatusSuccess;
            break;
        case gpExtStorage_OutOfRange:
            status = gpUpgrade_StatusOutOfRange;
            break;
        case gpExtStorage_UnalignedAddress:
            status = gpUpgrade_StatusInvalidAddress;
            break;
        case gpExtStorage_VerifyFailure:
            status = gpUpgrade_StatusFailedVerify;
            break;
#if defined(GP_EXTSTORAGE_DIVERSITY_SECURE)
        case gpExtStorage_EncryptionFailure:
        case gpExtStorage_DecryptionFailure:
        case gpExtStorage_EncryptionBlockNotAligned:
            status = gpUpgrade_StatusInvalidImage;
            break;
#endif // GP_EXTSTORAGE_DIVERSITY_SECURE
        case gpExtStorage_BlockOverFlow:
        case gpExtStorage_BlankFailure:
        default:
            GP_ASSERT_DEV_INT(false); //shall not happen on read
            break;
    }
#else // not defined(GP_COMP_EXTSTORAGE)
    switch(gpHal_FlashRead(address, length, dataChunk))
    {

        case gpHal_FlashError_Success:
            status = gpUpgrade_StatusSuccess;
            break;
        case gpHal_FlashError_OutOfRange:
            status = gpUpgrade_StatusOutOfRange;
            break;
        case gpHal_FlashError_UnalignedAddress:
            status = gpUpgrade_StatusInvalidAddress;
            break;
        case gpHal_FlashError_VerifyFailure:
            status = gpUpgrade_StatusFailedVerify;
            break;
        case gpHal_FlashError_Locked:
        case gpHal_FlashError_BlankFailure:
        default:
            GP_ASSERT_DEV_INT(false); //shall not happen on read
            break;
    }
#endif //GP_COMP_EXTSTORAGE
    return status;
}
#endif // !GP_DIVERSITY_BOOTLOADER_BUILD

