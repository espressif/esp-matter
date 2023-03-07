/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *  The file contains functionality to program Kx flash
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

// #define GP_LOCAL_LOG

#include "global.h"
#include "gpBsp.h"
#include "hal.h"

#include "gpHal.h"
#include "gpLog.h"

#include "gpHal_kx_Flash.h"
#include "gpHal_kx_Ipc.h"

#ifdef GP_COMP_CHIPEMU
#include "gpChipEmu_Flash.h"
#endif

#include "hal_ROM.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/


#define FLASH_CHECK_WORD_ALIGNED(address)               (((address) & 0x3) == 0)
#define FLASH_RANGE_IN_SINGLE_SECTOR(address, length)   ((length) <= (FLASH_SECTOR_SIZE - ((address) % FLASH_SECTOR_SIZE)))


#ifdef GP_COMP_CHIPEMU
#define FLASH_READ_UINT32(address)  ({ UInt32 ret; flash_read_rom(address, (UInt8*)&ret, 4); ret;})
#else
#define FLASH_READ_UINT32(address)  *((UInt32*)(address))
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Dependencies on external functions
 *****************************************************************************/

#ifdef GP_COMP_CHIPEMU
extern UInt16 flash_read_rom(UInt32 address, UInt8* data, UInt16 length_8);
extern UInt16 flash_read_rom_info_page(UInt32 address, UInt8* data, UInt16 length_8);
#endif //GP_COMP_CHIPEMU

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static gpHal_FlashError_t gpHal_FlashVerify(FlashPtr address, UInt32* data, UInt16 length_32)
{
    gpHal_FlashError_t ret;
#if !defined(GP_COMP_CHIPEMU)
    ret = MEMCMP((UInt32*)address,data,length_32*4) ? gpHal_FlashError_VerifyFailure : gpHal_FlashError_Success;
#else
    {
        UInt32 rb[FLASH_PAGE_SIZE/4];
        UInt8  *bp, *dp;
        bp = (UInt8 *)rb;
        dp = (UInt8 *)data;

        //Need to give the lenght in bytes to flash_read_rom
        flash_read_rom(address, bp, length_32*4);

        if (!MEMCMP(bp, dp, length_32*4))    // need to compare ALL bytes !! (lenght in bytes)
        {
            ret = gpHal_FlashError_Success;
        }
        else
        {
            UInt16 t;
            for (t = 0; t < length_32; t++)
            {
                GP_LOG_PRINTF("Unexp. %lx",0, (unsigned long)rb[t]);
            }
            ret = gpHal_FlashError_VerifyFailure;
        }
    }
#endif
    return ret;
}

static void gpHal_FlashLock(void)
{
        /* Lock Flash */
        GP_WB_WRITE_STANDBY_FLASH_GLOBAL_WRITE_LOCK(1);
}

static gpHal_FlashError_t gpHal_FlashUnlock(FlashPtr sectorAddress)
{
    /* Unlock flash */
    GP_WB_WRITE_STANDBY_FLASH_GLOBAL_WRITE_LOCK(0);

#if defined(GP_DIVERSITY_GPHAL_FLASH_USE_APP_ADDRESS_CHECK)
    if(gpHal_cbFlashAddressCheck(sectorAddress) != gpHal_FlashError_Success)
    {
        gpHal_FlashLock();
        return gpHal_FlashError_OutOfRange;
    }
#endif

    return gpHal_FlashError_Success;

}


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

gpHal_FlashError_t gpHal_FlashBlankCheck(FlashPtr address, UInt16 numUInt32)
{
    UInt16 i;

    if (!FLASH_CHECK_WORD_ALIGNED(address))
    {
        GP_LOG_PRINTF("FlashBlankCheck unaligned address 0x%lx", 0, (unsigned long)address);
        return gpHal_FlashError_UnalignedAddress;
    }

    for (i = 0; i < numUInt32; ++i)
    {
        if (FLASH_READ_UINT32((address + (4*i))) != 0)
        {
            GP_LOG_PRINTF("Failing erasure address %lx, data = %lx",0,(unsigned long)(address + (4*i)),
                                                                      (unsigned long)FLASH_READ_UINT32((address + (4*i))));
            return gpHal_FlashError_BlankFailure;
        }
    }

    return gpHal_FlashError_Success;
}


gpHal_FlashError_t gpHal_FlashRead(FlashPtr address, UInt16 numBytes, UInt8* data)
{

#ifdef GP_COMP_CHIPEMU
    return flash_read_rom(address, data, numBytes);
#else
    MEMCPY(data, (const void*)address, numBytes);
    return gpHal_FlashError_Success;
#endif // GP_COMP_CHIPEMU
}

gpHal_FlashError_t gpHal_FlashEraseSectorNoVerify(FlashPtr sectorAddress)
{
#if !defined(GP_COMP_CHIPEMU)
    gpHal_IpcBackupRestoreFlags_t flags;
#endif // !GP_COMP_CHIPEMU

    if(gpHal_FlashUnlock(sectorAddress) != gpHal_FlashError_Success)
    {
        return gpHal_FlashError_Locked;
    }

    if (!FLASH_CHECK_ADDRESS(sectorAddress) && !FLASH_CHECK_ALT_ADDRESS(sectorAddress))
    {
        GP_LOG_PRINTF("FlashErase addres out of range 0x%lx", 0, (unsigned long)sectorAddress);
        return gpHal_FlashError_OutOfRange;
    }

    if (sectorAddress % FLASH_SECTOR_SIZE != 0)
    {
        GP_LOG_PRINTF("FlashErase unaligned addres 0x%lx", 0, (unsigned long)sectorAddress);
        return gpHal_FlashError_UnalignedAddress;
    }

#if !defined(GP_COMP_CHIPEMU)
    gpHal_IpcStop(&flags);
#endif // !GP_COMP_CHIPEMU

    HAL_DISABLE_GLOBAL_INT();


    if (FLASH_CHECK_ADDRESS(sectorAddress))
    {
#if defined(GP_COMP_CHIPEMU)
        //Chipemu expects full addres, not Physical
        ROM_flash_sector_erase(sectorAddress);
#else
        ROM_flash_sector_erase(FLASH_WB_ADDR_TO_PHYS(sectorAddress));
#endif
    }
    else if (FLASH_CHECK_ALT_ADDRESS(sectorAddress))
    {
        /* Make sure that the uC has access the alternate view */
        GP_ASSERT_SYSTEM(GP_WB_READ_STANDBY_BLOCK_ALTERNATE_CODE_FLASH_VIEW() == false);

#if defined(GP_COMP_CHIPEMU)
        //Chipemu expects full addres, not Physical
        ROM_flash_sector_erase(sectorAddress);
#else
        ROM_flash_sector_erase(FLASH_WB_ALT_ADDR_TO_PHYS(sectorAddress));
#endif
    }
    else
    {
        // should already be captured by check above
        GP_ASSERT_SYSTEM(false);
    }

    // Invalidate flash data buffers.
    GP_WB_WRITE_MM_FLASH_CONV_BUFFER_INVALIDATE(0x1f);
    __DMB();

    HAL_ENABLE_GLOBAL_INT();

    gpHal_FlashLock();

#if !defined(GP_COMP_CHIPEMU)
    gpHal_IpcRestart(&flags);
#endif // !GP_COMP_CHIPEMU

    return gpHal_FlashError_Success;
}

/*
 * For now, make sure the INF page is not erase by accident.
 * Cannot really do an ASSERT here, as we cannot execute from FLASH.
 */
gpHal_FlashError_t gpHal_FlashEraseSector(FlashPtr sectorAddress)
{
    gpHal_FlashError_t ret = gpHal_FlashEraseSectorNoVerify(sectorAddress);
    if (ret == gpHal_FlashError_Success)
    {
        ret = gpHal_FlashBlankCheck(sectorAddress, FLASH_SECTOR_SIZE / 4);
    }
    return ret;
}

gpHal_FlashError_t gpHal_FlashWriteNoVerify(FlashPtr address, UInt16 numWord, UInt32 *data)
{
#if !defined(GP_COMP_CHIPEMU)
    gpHal_IpcBackupRestoreFlags_t flags;
#endif // !GP_COMP_CHIPEMU

    if(gpHal_FlashUnlock(address) != gpHal_FlashError_Success)
    {
        return gpHal_FlashError_Locked;
    }

    if (!FLASH_CHECK_ADDRESS_RANGE(address, ((UInt32)numWord) * 4) && !FLASH_CHECK_ALT_ADDRESS_RANGE(address, ((UInt32)numWord) * 4))
    {
        GP_LOG_PRINTF("FlashWrite addres out of range 0x%lx", 0, (unsigned long)address);
        return gpHal_FlashError_OutOfRange;
    }

    if (address % FLASH_WRITE_UNIT != 0)
    {
        GP_LOG_PRINTF("FlashWrite unaligned address 0x%lx", 0, (unsigned long)address);
        return gpHal_FlashError_UnalignedAddress;
    }

    if (numWord % (FLASH_WRITE_UNIT / 4) != 0)
    {
        GP_LOG_PRINTF("FlashWrite unaligned length 0x%x", 0, numWord);
        return gpHal_FlashError_UnalignedAddress;
    }

    if (numWord > (FLASH_PAGE_SIZE - (address % FLASH_PAGE_SIZE)) / 4)
    {
        GP_LOG_PRINTF("FlashWrite crossing page boundary 0x%lx 0x%x", 0, (unsigned long)address, numWord);
        return gpHal_FlashError_UnalignedAddress;
    }

#if !defined(GP_COMP_CHIPEMU)
    gpHal_IpcStop(&flags);
#endif // !GP_COMP_CHIPEMU

    HAL_DISABLE_GLOBAL_INT();

    if (FLASH_CHECK_ADDRESS(address))
    {
#if defined(GP_COMP_CHIPEMU)
        ROM_flash_write(address, data, numWord);
#else
        ROM_flash_write(FLASH_WB_ADDR_TO_PHYS(address), data, (UInt8)(numWord / (FLASH_WRITE_UNIT / sizeof(data[0]))));
#endif
    }
    else if(FLASH_CHECK_ALT_ADDRESS(address))
    {
        /* Make sure that the uC has access the alternate view */
        GP_ASSERT_SYSTEM(GP_WB_READ_STANDBY_BLOCK_ALTERNATE_CODE_FLASH_VIEW() == false);

#if defined(GP_COMP_CHIPEMU)
        ROM_flash_write(address, data, numWord);
#else
        ROM_flash_write(FLASH_WB_ALT_ADDR_TO_PHYS(address), data, (UInt8)(numWord / (FLASH_WRITE_UNIT / sizeof(data[0]))));
#endif
    }
    else
    {
        GP_ASSERT_SYSTEM(false);
    }

    // Invalidate flash data buffers.
    GP_WB_WRITE_MM_FLASH_CONV_BUFFER_INVALIDATE(0x1f);
    __DMB();

    HAL_ENABLE_GLOBAL_INT();

    gpHal_FlashLock();

#if !defined(GP_COMP_CHIPEMU)
    gpHal_IpcRestart(&flags);
#endif // !GP_COMP_CHIPEMU

    return gpHal_FlashError_Success;
}

gpHal_FlashError_t gpHal_FlashWrite(FlashPtr address, UInt16 numWord, UInt32* data)
{
    gpHal_FlashError_t ret = gpHal_FlashWriteNoVerify(address, numWord, data);
    if (ret == gpHal_FlashError_Success)
    {
        ret = gpHal_FlashVerify(address, data, numWord);
    }
    return ret;
}

gpHal_FlashError_t gpHal_FlashProgramSector(FlashPtr address, UInt16 length, UInt8* data)
{
    gpHal_FlashError_t result;
    FlashPtr           sectorAligned = FLASH_ALIGN_SECTOR(address);
    UInt32             buffer[FLASH_SECTOR_SIZE / sizeof(UInt32)];

    if(!FLASH_RANGE_IN_SINGLE_SECTOR(address, length))
    {
        return gpHal_FlashError_OutOfRange;
    }

    /* read complete sector*/
    result = gpHal_FlashRead(sectorAligned, FLASH_SECTOR_SIZE, (UInt8*)buffer);

    if(result != gpHal_FlashError_Success)
    {
        return result;
    }

    /* modify */
    MEMCPY(((UInt8*)buffer) + (address - sectorAligned), data, length);
    /* erase sector */
    result = gpHal_FlashEraseSectorNoVerify(sectorAligned);

    if(result != gpHal_FlashError_Success)
    {
        return result;
    }

    /* Write Page 0 */
    result = gpHal_FlashWrite(sectorAligned, FLASH_PAGE_SIZE/4, (UInt32*)&buffer[0]);
    if(result == gpHal_FlashError_Success)
    {
        /* Write Page 1 */
        result = gpHal_FlashWrite(sectorAligned + FLASH_PAGE_SIZE , FLASH_PAGE_SIZE/4, (UInt32*)&buffer[FLASH_PAGE_SIZE/4]);
    }

    return result;
}


#if defined(GP_DIVERSITY_GPHAL_INTERN)
// NOTE: writes to the lock bits are one-shot, so calls to this function will set incremental lock bits!
void gpHal_FlashWriteLockFineCoarse(UInt32 block256k_0, UInt32 block256k_1, UInt32 block256k_2, UInt32 block256k_3)
{

    GP_ASSERT_SYSTEM((block256k_0 <= GPHAL_FLASH_WRITE_LOCK_FULL_BLOCK) &
                     (block256k_1 <= GPHAL_FLASH_WRITE_LOCK_FULL_BLOCK) &
                     (block256k_2 <= GPHAL_FLASH_WRITE_LOCK_FULL_BLOCK) &
                     (block256k_3 <= GPHAL_FLASH_WRITE_LOCK_FULL_BLOCK));


    // Write fine lock bits into the right format
    GP_WB_WRITE_STANDBY_WRITE_LOCK_FINE(((UInt64)(block256k_0 & GPHAL_FLASH_WRITE_LOCK_FINE_BITMASK)) |
                                        (((UInt64)(block256k_1 & GPHAL_FLASH_WRITE_LOCK_FINE_BITMASK)) << 16) |
                                        (((UInt64)(block256k_2 & GPHAL_FLASH_WRITE_LOCK_FINE_BITMASK)) << 32) |
                                        ((UInt64)(block256k_3 & GPHAL_FLASH_WRITE_LOCK_FINE_BITMASK)) << 48);


    // Write coarse lock bits into the right format
    GP_WB_WRITE_STANDBY_WRITE_LOCK_COARSE((((block256k_0 & GPHAL_FLASH_WRITE_LOCK_COARSE_BITMASK) >> 16)) |
                                          (((block256k_1 & GPHAL_FLASH_WRITE_LOCK_COARSE_BITMASK) >> 16) << 3) |
                                          (((block256k_2 & GPHAL_FLASH_WRITE_LOCK_COARSE_BITMASK) >> 16) << 6) |
                                          (((block256k_3 & GPHAL_FLASH_WRITE_LOCK_COARSE_BITMASK) >> 16) << 9));

}

void gpHal_FlashLockRedundantSectors(void)
{
    // Lock the redundant sector, if it has not been remapped
    GP_WB_WRITE_STANDBY_LOCK_NVM_REDUNDANCY_0(0x01);
    GP_WB_WRITE_STANDBY_LOCK_NVM_REDUNDANCY_1(0x01);
    GP_WB_WRITE_STANDBY_LOCK_NVM_REDUNDANCY_2(0x01);
    GP_WB_WRITE_STANDBY_LOCK_NVM_REDUNDANCY_3(0x01);
}
#endif

UInt32 gpHal_FlashRemap_GetSwapLocationOffset(void)
{
    UInt32 flashsizeKB = 0;
    UInt32 offsetBytes;

    if(GP_WB_READ_STANDBY_NVM_SIZE() == GP_WB_ENUM_NVM_SIZE_KB1024)
    {
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
        flashsizeKB = 1024; //KB
#else
        /* FPGA has smaller flash size then silicon (576KB) */
        flashsizeKB = 512; //KB
#endif

    } else if(GP_WB_READ_STANDBY_NVM_SIZE() == GP_WB_ENUM_NVM_SIZE_KB512)
    {
        flashsizeKB = 512; //KB

    } else
    {
        GP_LOG_SYSTEM_PRINTF("Flash remapping not implemented for this flash size!", 0);
        GP_ASSERT_SYSTEM(0);
    }

    /* Flash is swapped at half of the Flash */
    offsetBytes = flashsizeKB << 9; //(flashsizeKB * 1024) / 2;
    return offsetBytes;
}

void gpHal_FlashRemap_SwapUpperAndLowerFlashAreas(void)
{
    const UInt8 block0_256KB = 0, block1_256KB = 1;
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
    const UInt8 block2_256KB = 2, block3_256KB = 3;
#endif //!defined(GP_HAL_EXPECTED_CHIP_EMULATED)
    if(GP_WB_READ_STANDBY_NVM_SIZE() == GP_WB_ENUM_NVM_SIZE_KB1024)
    {

#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
        // swap the first 512k with the second half
        GP_WB_WRITE_CORTEXM4_ICODE_DCODE_BLOCK_0_REMAP(block2_256KB);
        GP_WB_WRITE_CORTEXM4_ICODE_DCODE_BLOCK_1_REMAP(block3_256KB);

        // here the new app resides at offset 0x1000, just as the original one
        GP_WB_WRITE_CORTEXM4_ICODE_DCODE_BLOCK_2_REMAP(block0_256KB);
        GP_WB_WRITE_CORTEXM4_ICODE_DCODE_BLOCK_3_REMAP(block1_256KB);
#else
        /* FPGA has smaller flash size then silicon (576KB) */
        // only swap first first two 256k blocks
        GP_WB_WRITE_CORTEXM4_ICODE_DCODE_BLOCK_0_REMAP(block1_256KB);
        GP_WB_WRITE_CORTEXM4_ICODE_DCODE_BLOCK_1_REMAP(block0_256KB);
#endif
    }
    else if(GP_WB_READ_STANDBY_NVM_SIZE() == GP_WB_ENUM_NVM_SIZE_KB512)
    {
        // only swap first first two 256k blocks
        GP_WB_WRITE_CORTEXM4_ICODE_DCODE_BLOCK_0_REMAP(block1_256KB);
        GP_WB_WRITE_CORTEXM4_ICODE_DCODE_BLOCK_1_REMAP(block0_256KB);
    } else
    {
        GP_LOG_SYSTEM_PRINTF("Flash remapping not implemented for this flash size!", 0);
        GP_ASSERT_SYSTEM(0);
    }

    __DMB();
    __DSB();
}

Bool gpHal_FlashRemap_IsFlashSwapped(void)
{
    Bool swapped = false;
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
    const UInt8 block0_256KB = 0, block1_256KB = 1, block2_256KB = 2, block3_256KB = 3;
    UInt8 block0_remap, block1_remap, block2_remap, block3_remap;
#else
    /* FPGA has smaller flash size then silicon (576KB) */
    const UInt8 block0_256KB = 0, block1_256KB = 1;//, block2_256KB = 2, block3_256KB = 3;
    UInt8 block0_remap, block1_remap;//, block2_remap, block3_remap;
#endif

    block0_remap = GP_WB_READ_CORTEXM4_ICODE_DCODE_BLOCK_0_REMAP();
    block1_remap = GP_WB_READ_CORTEXM4_ICODE_DCODE_BLOCK_1_REMAP();
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
    block2_remap = GP_WB_READ_CORTEXM4_ICODE_DCODE_BLOCK_2_REMAP();
    block3_remap = GP_WB_READ_CORTEXM4_ICODE_DCODE_BLOCK_3_REMAP();
#endif

    if(GP_WB_READ_STANDBY_NVM_SIZE() == GP_WB_ENUM_NVM_SIZE_KB1024)
    {
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
        // check if the first 512k is swapped with the second half
        swapped = (block0_remap == block2_256KB && block1_remap == block3_256KB &&
                block2_remap == block0_256KB && block3_remap == block1_256KB);
#else
        swapped = ( block0_remap == block1_256KB && block1_remap == block0_256KB);
#endif
    }
    else if(GP_WB_READ_STANDBY_NVM_SIZE() == GP_WB_ENUM_NVM_SIZE_KB512)
    {
        // check if the first 256k is swapped with the second half
        swapped = ( block0_remap == block1_256KB && block1_remap == block0_256KB);
    } else
    {
        GP_LOG_SYSTEM_PRINTF("Flash remapping not implemented for this flash size!", 0);
        GP_ASSERT_SYSTEM(0);
    }

    return swapped;
}

