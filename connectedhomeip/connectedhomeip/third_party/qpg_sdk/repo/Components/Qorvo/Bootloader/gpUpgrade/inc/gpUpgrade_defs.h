/*
 *   Copyright (c) 2018, Qorvo Inc
 *
 *   Upgrade functionality
 *   Declarations of the public functions and enumerations of gpUpgrade.
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


#ifndef _GPUPGRADE_DEFS_H_
#define _GPUPGRADE_DEFS_H_

/// @file "gpUpgrade_defs.h"
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define IMG_MAX_BLOCK_LEN                       256

#if !defined(GP_DIVERSITY_APP_LICENSE_BASED_BOOT)
extern UInt32 umBoot_SwTable_Start__;
#define Upgrade_SwTableRes ((UInt32)&umBoot_SwTable_Start__ + (gpUpgrade_ImageIdRescueImage * sizeof(gpUpgrade_Table_t)))
#define Upgrade_SwTableUpg ((UInt32)&umBoot_SwTable_Start__ + (gpUpgrade_ImageIdPendingImage * sizeof(gpUpgrade_Table_t)))
#define Upgrade_SwTableAct ((UInt32)&umBoot_SwTable_Start__ + (gpUpgrade_ImageIdActiveImage * sizeof(gpUpgrade_Table_t)))
#endif

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/
#define UPGRADE_TAB_OFFSET_FLAG              (4 * FLASH_WORD_SIZE)

//Flag masks/shifts for the status flag
#define FLAG_UPGRADE_HANDLED_SHIFT 0
#define FLAG_UPGRADE_HANDLED_MASK 0xFF      /* use 1 byte for handled */
#define FLAG_STATUS_SHIFT 8
#define FLAG_STATUS_MASK 0xFFFFFF00        /* Use 3 of 4 bytes for status */
#define UPGRADE_FLAG_GET_FLAG(flagStatus)    (((flagStatus) >> FLAG_UPGRADE_HANDLED_SHIFT) & FLAG_UPGRADE_HANDLED_MASK)
#define UPGRADE_FLAG_GET_STATUS(flagStatus)  (((flagStatus) >> FLAG_STATUS_SHIFT) & FLAG_UPGRADE_HANDLED_MASK)

#define GP_UPGRADE_UPGRADE_MAX_RETRIES       (4)

#define FLASH_IN_PAGE(address, length)          ((length) <= (FLASH_PAGE_SIZE - ((address) % FLASH_PAGE_SIZE)))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#if !defined(GP_DIVERSITY_APP_LICENSE_BASED_BOOT)
/** @struct gpUpgrade_Table_t
 *  @brief Info table holding information about pending images.
*/
typedef struct {
    gpUpgrade_ImageId_t         id;
    UInt32                      swVer;
    UInt32                      hwVer;
    UInt32                      crc;
    /** @brief 4 byte field: NULL|NULL|status|flag */
    UInt32                      flagStatus;
    UInt32                      memType;
    UInt32                      memStartAddr;
    UInt32                      memSz;
} gpUpgrade_Table_t;
#endif /* !GP_DIVERSITY_APP_LICENSE_BASED_BOOT*/

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GP_DIVERSITY_BOOTLOADER_BUILD
/** @brief Get current value of gpUpgrade_Crc
*   @return crcValue
*/
UInt32 gpUpgrade_GetCrc(void);

/** @brief Reset the value of gpUpgrade_Crc.
*/
void gpUpgrade_ClrCrc(void);

#endif //GP_DIVERSITY_BOOTLOADER_BUILD

/** @brief Initialize or perform final step of CRC calculation.
*
*   @param pCrcValue                 Pointer to CRC value to be initialised or finalized.
*/
void gpUpgrade_StartOrEndCrc(UInt32* pCrcValue);

/** @brief Calculate the CRC over the image data. Used both bootloader and user application writing the data.
                 Using CRC32 (CRC-32 IEEE or ISO-3309 (polynomial 0xedb88320 (MSB first: 0x04C11DB7)))
*
*   @param pCrcValue                 Pointer to CRC value to be used for calculation.
*   @param length                    Length of the data.
*   @param pData                     Data over which the CRC will be calculated.
*/
void gpUpgrade_CalculatePartialCrc(UInt32* pCrcValue, UInt32 length, UInt8* pData);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPUPGRADE_DEFS_H_

