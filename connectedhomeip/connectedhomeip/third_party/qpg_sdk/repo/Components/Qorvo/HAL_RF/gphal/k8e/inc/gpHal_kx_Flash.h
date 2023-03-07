/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GPHAL_KX_FLASH_H_
#define _GPHAL_KX_FLASH_H_


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt32 FlashPtr;

typedef enum {
    gpHal_FlashError_Success = 0,
    gpHal_FlashError_OutOfRange,
    gpHal_FlashError_UnalignedAddress,
    gpHal_FlashError_BlankFailure,
    gpHal_FlashError_VerifyFailure,
    gpHal_FlashError_Locked,
} gpHal_FlashError_t;


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define FLASH_WORD_SIZE             (4)
#define FLASH_PAGE_SIZE             (512U)
#define FLASH_PAGES_PER_SECTOR      (2U)
#define FLASH_SECTOR_SIZE           (FLASH_PAGES_PER_SECTOR * FLASH_PAGE_SIZE)

#define FLASH_ERASE_UNIT            FLASH_SECTOR_SIZE
#define FLASH_WRITE_UNIT            (16)

#define FLASH_ALIGN_PAGE(address)       ((address) - ((address) % FLASH_PAGE_SIZE))
#define FLASH_ALIGN_SECTOR(address)       ((address) - ((address) % FLASH_SECTOR_SIZE))

#define FLASH_CHECK_ADDRESS(address)                 (GP_MM_FLASH_LINEAR_START <= (address) && (address) < GP_MM_FLASH_LINEAR_END)
#define FLASH_CHECK_ADDRESS_RANGE(address, len)      (GP_MM_FLASH_LINEAR_START <= (address) && (len) <= GP_MM_FLASH_LINEAR_END && (address) <= GP_MM_FLASH_LINEAR_END - (len))

#define FLASH_CHECK_ALT_ADDRESS(address)             (GP_MM_FLASH_ALT_START <= (address) && (address) < GP_MM_FLASH_ALT_END)
#define FLASH_CHECK_ALT_ADDRESS_RANGE(address, len)  (GP_MM_FLASH_ALT_START <= (address) && (len) <= GP_MM_FLASH_ALT_END && (address) <= GP_MM_FLASH_ALT_END - (len))

/* Convert absolute address (in ARM memory space) to flash physical address */
#define FLASH_WB_ADDR_TO_PHYS(address)               (((address) - GP_MM_FLASH_START) / FLASH_WRITE_UNIT)
#define FLASH_WB_ALT_ADDR_TO_PHYS(address)           (((address) - GP_MM_FLASH_ALT_START) / FLASH_WRITE_UNIT)

/* Flash write lock definitions */
#define GPHAL_FLASH_WRITE_LOCK_REGION_0_2K     BM(0)
#define GPHAL_FLASH_WRITE_LOCK_REGION_2_4K     BM(1)
#define GPHAL_FLASH_WRITE_LOCK_REGION_4_6K     BM(2)
#define GPHAL_FLASH_WRITE_LOCK_REGION_6_8K     BM(3)
#define GPHAL_FLASH_WRITE_LOCK_REGION_8_10K    BM(4)
#define GPHAL_FLASH_WRITE_LOCK_REGION_10_12K   BM(5)
#define GPHAL_FLASH_WRITE_LOCK_REGION_12_14K   BM(6)
#define GPHAL_FLASH_WRITE_LOCK_REGION_14_16K   BM(7)
#define GPHAL_FLASH_WRITE_LOCK_REGION_16_18K   BM(8)
#define GPHAL_FLASH_WRITE_LOCK_REGION_18_20K   BM(9)
#define GPHAL_FLASH_WRITE_LOCK_REGION_20_22K   BM(10)
#define GPHAL_FLASH_WRITE_LOCK_REGION_22_24K   BM(11)
#define GPHAL_FLASH_WRITE_LOCK_REGION_24_26K   BM(12)
#define GPHAL_FLASH_WRITE_LOCK_REGION_26_28K   BM(13)
#define GPHAL_FLASH_WRITE_LOCK_REGION_28_30K   BM(14)
#define GPHAL_FLASH_WRITE_LOCK_REGION_30_32K   BM(15)
#define GPHAL_FLASH_WRITE_LOCK_REGION_32_64K   BM(16)
#define GPHAL_FLASH_WRITE_LOCK_REGION_64_128K  BM(17)
#define GPHAL_FLASH_WRITE_LOCK_REGION_128_256K BM(18)

#define GPHAL_FLASH_WRITE_LOCK_FULL_BLOCK     (GPHAL_FLASH_WRITE_LOCK_REGION_0_2K    | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_2_4K    | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_4_6K    | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_6_8K    | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_8_10K   | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_10_12K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_12_14K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_14_16K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_16_18K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_18_20K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_20_22K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_22_24K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_24_26K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_26_28K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_28_30K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_30_32K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_32_64K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_64_128K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_128_256K)


#define GPHAL_FLASH_WRITE_LOCK_FINE_BITMASK   (GPHAL_FLASH_WRITE_LOCK_REGION_0_2K   | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_2_4K   | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_4_6K   | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_6_8K   | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_8_10K  | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_10_12K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_12_14K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_14_16K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_16_18K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_18_20K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_20_22K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_22_24K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_24_26K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_26_28K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_28_30K | \
                                               GPHAL_FLASH_WRITE_LOCK_REGION_30_32K)

#define GPHAL_FLASH_WRITE_LOCK_COARSE_BITMASK (GPHAL_FLASH_WRITE_LOCK_REGION_32_64K | GPHAL_FLASH_WRITE_LOCK_REGION_64_128K | GPHAL_FLASH_WRITE_LOCK_REGION_128_256K)

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Read from flash.
 *
 *  @param address  Absolute address of flash data within Kx memory map (not relative to start of flash).
 *  @param length   Number of bytes to read. Must be at most FLASH_PAGE_SIZE.
 *  @param data     Pointer to buffer of "length" bytes where read data will be stored.
 */
gpHal_FlashError_t gpHal_FlashRead       (FlashPtr address, UInt16 length, UInt8* data);

/**
 *  Erase one flash sector.
 *
 *  @param address  Absolute address of flash sector within Kx memory map (not relative to start of flash).
 *                  Must be aligned to FLASH_SECTOR_SIZE.
 *  @return gpHal_FlashError_Success if erase succesful;
 *          gpHal_FlashError_BlankFailure if erase faled.
 */
gpHal_FlashError_t gpHal_FlashEraseSector(FlashPtr address);
/**
 *  Erase one flash sector and do not check that sector was erased.
 *  See @a gpHal_FlashEraseSector for a description of parameters.
 */
gpHal_FlashError_t gpHal_FlashEraseSectorNoVerify(FlashPtr address);

/**
 *  Write data to flash.
 *
 *  Note flash is written in units of FLASH_WRITE_UNIT bytes.
 *  Any write access must be aligned on FLASH_WRITE_UNIT bytes.
 *  Any write access must write a multiple of FLASH_WRITE_UNIT bytes.
 *  After writing, the same units may not be written again until the sector is erased.
 *  Write access may not cross a flash page boundary (FLASH_PAGE_SIZE bytes).
 *
 *  @param address  Absolute address of flash data within Kx memory map (not relative to start of flash).
 *                  Must be aligned to FLASH_WRITE_UNIT.
 *  @param length   Number of 32-bit words to write. Must be a multiple of (FLASH_WRITE_UNIT/4).
 *                  i.e. total number of bytes written must be a multiple of FLASH_WRITE_UNIT.
 *  @param data     Pointer to input data buffer containing "numWord" 32-bit words to write.
 *                  Buffer must be located in RAM. Writing directly from one part of flash to another is not supported.
 *  @return gpHal_FlashError_Success if write succesful;
 *          gpHal_FlashError_VerifyFailure if write failed.
 */
gpHal_FlashError_t gpHal_FlashWrite        (FlashPtr address, UInt16 numWord, UInt32* data);
/**
 *  Write data to flash and do not verify written data.
 *  See @a gpHal_FlashWrite for a description of parameters.
 */
gpHal_FlashError_t gpHal_FlashWriteNoVerify(FlashPtr address, UInt16 numWord, UInt32* data);

/**
 *  @param address Addresses are absolute - ie. according to the Kx memory map (not relative).
 *  @param length  number of bytes.
 *  @param data    RAM buffer of sufficient size where the data to be written is stored.
 *                 data needs to be a pointer into the RAM area. Writing directly from one part of flash to another is not supported.
**/
gpHal_FlashError_t gpHal_FlashProgramSector(FlashPtr address, UInt16 length, UInt8* data);

/**
 *  Check that a specified area of flash is blank (all zero bytes).
 *
 *  @param address    Absolute address of flash data within Kx memory map (not relative to start of flash).
 *                    Must be algined to 4 bytes.
 *  @param numUInt32  Number of 32-bit words to check.
 *  @return gpHal_FlashError_Success if specified area is blank;
 *          gpHal_FlashError_BlankFailure if specified area contains non-zero data.
 */
gpHal_FlashError_t gpHal_FlashBlankCheck(FlashPtr address, UInt16 numUInt32);

#if defined(GP_DIVERSITY_GPHAL_FLASH_USE_APP_ADDRESS_CHECK)
/**
 * callback which can be registered by application to decide whether or not to write to this flash address
 */
gpHal_FlashError_t gpHal_cbFlashAddressCheck(FlashPtr sectorAddress);
#endif


#if defined(GP_DIVERSITY_GPHAL_INTERN)
/**
 *  Lock parts of the flash.
 *
 *  @param block_256k_0     bitmask of first 256k of flash to be locked. 0k - 256k
 *  @param block_256k_1     bitmask of second 256k of flash to be locked. 256k - 512k
 *  @param block_256k_2     bitmask of third 256k of flash to be locked. 512k - 768k
 *  @param block_256k_3     bitmask of fourth 256k of flash to be locked. 768k - 1024k
 *
 *  For each block: first 16 bits [15:0] are 2k granular lock bits
 *                  bit [16], bit [17] and bit [18] are 32k, 64k and 128k lock bits respectively
 */
void gpHal_FlashWriteLockFineCoarse(UInt32 block256k_0, UInt32 block256k_1, UInt32 block256k_2, UInt32 block256k_3);

/**
 * Lock the redundant sectors if they are not remapped.
 */
void gpHal_FlashLockRedundantSectors(void);
#endif

/**
 * Returns the offset from the start of flash where the flash will be swapped
 *
 * Upper Flash Area --> (flash_alt_start + swap_location_offset) TO (flash_alt_start + flash_size - 1)
 * Lower Flash Area --> (flash_alt_start) TO (flash_alt_start + swap_location_offset - 1)
 * With remap off,   Lower Flash Area is mapped to (flash_start) - (flash_start + swap_location_offset - 1)
 * With remap on,  Upper Flash Area is mapped to (flash_start) - (flash_start + swap_location_offset - 1)
 */
UInt32 gpHal_FlashRemap_GetSwapLocationOffset(void);

/**
 * Swap upper and lower flash areas. After calling this function the upper half of
 * flash will be mapped to start from GP_MM_FLASH_START.
 * Not to be used from application that is using the non alternate view of Flash.
 * (usually to be used only from flash bootloader)
 */
void gpHal_FlashRemap_SwapUpperAndLowerFlashAreas(void);

/**
 * Check if the upper and lower flash areas have been swapped
 */
Bool gpHal_FlashRemap_IsFlashSwapped(void);

#ifdef __cplusplus
}
#endif
#endif //_GPHAL_KX_FLASH_H_
