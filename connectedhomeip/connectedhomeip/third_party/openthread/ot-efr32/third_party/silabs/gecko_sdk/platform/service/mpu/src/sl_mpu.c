/***************************************************************************//**
 * @file
 * @brief MPU API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_device.h"

#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)

#include "sl_common.h"
#include "em_core.h"
#include "sl_mpu.h"
#include <stdlib.h>
#include <math.h>

#define MPU_RBAR_VALUE    ARM_MPU_RBAR(0u, ARM_MPU_SH_NON, 0, 1, 1)

// ARM memory map SRAM location and size.
#define MPU_ARM_SRAM_MEM_BASE          0x20000000
#define MPU_ARM_SRAM_MEM_SIZE          0x20000000

// Number of sub-regions per MPU region.
#define MPU_SUBREGION_NBR             8u

// Minimum size of a MPU region to use sub-regions disable feature.
#define MPU_SUBREGION_USE_MIN_SIZE    256u

// Pre-defined MPU regions.
#define MPU_RAM_BACKGROUND_XN_REGION_NBR  0u
#define MPU_RAM_FUNCTION_REGION_NBR       1u

#if defined(__ICCARM__)
// iccarm
#pragma section = ".textrw"
#define RAMFUNC_SECTION_BEGIN       ((uint32_t)(uint32_t *)__section_begin(".textrw"))
#define RAMFUNC_SECTION_END         ((uint32_t)(uint32_t *)__section_end(".textrw"))
#define RAMFUNC_SECTION_SIZE        __section_size(".textrw")

#elif defined(__GNUC__)
// armgcc
extern uint32_t __ram_func_section_start;
extern uint32_t __ram_func_section_end;
#define RAMFUNC_SECTION_BEGIN       (uint32_t) &__ram_func_section_start
#define RAMFUNC_SECTION_END         (uint32_t) &__ram_func_section_end
#define RAMFUNC_SECTION_SIZE        (RAMFUNC_SECTION_END - RAMFUNC_SECTION_BEGIN)

#elif defined(__CC_ARM)
// armcc
// The section name in the armcc scatter file must be "ram_code".
extern uint32_t ram_code$$Base;
extern uint32_t ram_code$$Limit;
#define RAMFUNC_SECTION_BEGIN       (uint32_t) &ram_code$$Base
#define RAMFUNC_SECTION_END         (uint32_t) &ram_code$$Limit
#define RAMFUNC_SECTION_SIZE        (RAMFUNC_SECTION_END - RAMFUNC_SECTION_BEGIN)

#endif

#ifndef ARM_MPU_ARMV8_H
static uint32_t mpu_round_up_next_pwr2(uint32_t nbr);
static uint8_t mpu_region_size_encode(uint32_t region_size);
static void mpu_compute_region_data(uint32_t section_begin,
                                    uint32_t section_end,
                                    uint32_t section_size,
                                    uint32_t *mpu_region_begin,
                                    uint32_t *mpu_region_end,
                                    uint32_t *mpu_region_size);

#endif

static uint32_t region_nbr = 0;

/**************************************************************************//**
 *****************************************************************************/
void sl_mpu_disable_execute_from_ram(void)
{
  uint32_t mpu_region_begin = 0u;
  uint32_t mpu_region_end = 0u;

  ARM_MPU_Disable();

#ifdef ARM_MPU_ARMV8_H
  uint32_t rbar;

  // Region end address LSB are always considered 1F.
  mpu_region_begin = MPU_ARM_SRAM_MEM_BASE;
  mpu_region_end = (RAMFUNC_SECTION_SIZE > 0) ? (RAMFUNC_SECTION_BEGIN & MPU_RBAR_BASE_Msk) - 32u
                   : (MPU_ARM_SRAM_MEM_BASE + MPU_ARM_SRAM_MEM_SIZE);

  ARM_MPU_SetMemAttr(0, ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1, 0, 1, 0), 0));

  if (mpu_region_begin <= mpu_region_end) {
    // A bug exists in some versions of ARM_MPU_RBAR(). Set base addr manually.
    rbar = MPU_RBAR_VALUE | (mpu_region_begin & MPU_RBAR_BASE_Msk);
    ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, 0u));
    region_nbr++;
  }

  if (RAMFUNC_SECTION_SIZE > 0u) {
    // Region end address LSB are always considered 1F.
    mpu_region_begin = (RAMFUNC_SECTION_END + 31u) & MPU_RLAR_LIMIT_Msk;
    mpu_region_end = MPU_ARM_SRAM_MEM_BASE + MPU_ARM_SRAM_MEM_SIZE - 32u;

    // A bug exists in some versions of ARM_MPU_RBAR(). Set base addr manually.
    rbar = MPU_RBAR_VALUE | (mpu_region_begin & MPU_RBAR_BASE_Msk);
    ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, 0u));
    region_nbr++;
  }
#else
  uint8_t region_size_encoded;
  uint32_t mpu_region_size;

  (void) mpu_region_begin;
  (void) mpu_region_end;

  // Set background RAM region as execute never
  region_size_encoded = mpu_region_size_encode(MPU_ARM_SRAM_MEM_SIZE);
  ARM_MPU_SetRegionEx(region_nbr,
                      MPU_ARM_SRAM_MEM_BASE,
                      ((region_size_encoded << MPU_RASR_SIZE_Pos) & MPU_RASR_SIZE_Msk)
                      | (ARM_MPU_AP_FULL << MPU_RASR_AP_Pos)
                      | MPU_RASR_B_Msk
                      | MPU_RASR_XN_Msk
                      | MPU_RASR_ENABLE_Msk);
  region_nbr++;

  // Set RAM functions region as executable
  if (RAMFUNC_SECTION_SIZE > 0u) {
    uint32_t sr_size;
    uint8_t  srd_msk;

    mpu_compute_region_data(RAMFUNC_SECTION_BEGIN,
                            RAMFUNC_SECTION_END,
                            RAMFUNC_SECTION_SIZE,
                            &mpu_region_begin,
                            &mpu_region_end,
                            &mpu_region_size);

    // Compute sub-region Disable (SRD) mask
    sr_size = mpu_region_size / MPU_SUBREGION_NBR;
    // Check if sr_size is zero to satisfy MISRA
    sr_size = (sr_size != 0) ? sr_size : MPU_SUBREGION_USE_MIN_SIZE / MPU_SUBREGION_NBR;
    srd_msk = (1u << ((mpu_region_end - RAMFUNC_SECTION_END) / sr_size)) - 1u;
    srd_msk = srd_msk << (((RAMFUNC_SECTION_END - mpu_region_begin - 1u) / sr_size) + 1u);
    srd_msk |= (1u << ((RAMFUNC_SECTION_BEGIN - mpu_region_begin) / sr_size)) - 1u;

    region_size_encoded = mpu_region_size_encode(mpu_region_size);
    ARM_MPU_SetRegionEx(region_nbr,
                        mpu_region_begin,
                        ((region_size_encoded << MPU_RASR_SIZE_Pos) & MPU_RASR_SIZE_Msk)
                        | (ARM_MPU_AP_FULL << MPU_RASR_AP_Pos)
                        | MPU_RASR_B_Msk
                        | MPU_RASR_ENABLE_Msk
                        | (srd_msk << MPU_RASR_SRD_Pos));
    region_nbr++;
  }
#endif

  // Enable MPU with default background region
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

  __DSB();
  __ISB();
}

/**************************************************************************//**
 * Enables simplified MPU driver. Configures memory address as non-executable.
 *****************************************************************************/
sl_status_t sl_mpu_disable_execute(uint32_t address_begin,
                                   uint32_t address_end,
                                   uint32_t size)
{
  uint32_t mpu_region_begin = 0u;
  uint32_t mpu_region_end = 0u;

  ARM_MPU_Disable();

#ifdef ARM_MPU_ARMV8_H
  uint32_t rbar;

  // Size of memory region must be 32 bytes or more.
  if (size >= 32u) {
    // Device memory type non Gathering, non Re-ordering, Early Write Acknowledgement
    ARM_MPU_SetMemAttr(1, ARM_MPU_ATTR_DEVICE_nGnRE);

    // Round inside the memory region, if address is not align on 32 bytes.
    mpu_region_begin = ((address_begin % 32u) == 0u) ? address_begin
                       : (address_begin + (32u - (address_begin % 32u)));

    // Round inside the memory region, if address is not align on 32 bytes.
    mpu_region_end = ((address_end % 32u) == 0u) ? address_end
                     : (address_end  - (address_end % 32u));

    // A bug exists in some versions of ARM_MPU_RBAR(). Set base addr manually.
    rbar = ARM_MPU_RBAR(0u, 0u, 0u, 1u, 1u) | (mpu_region_begin & MPU_RBAR_BASE_Msk);
    ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, 1u));
    region_nbr++;
  }
#else
  uint8_t  region_size_encoded;
  uint32_t mpu_region_size;
  uint32_t sr_size;
  uint8_t  srd_msk;

  (void) mpu_region_begin;
  (void) mpu_region_end;

  mpu_compute_region_data(address_begin,
                          address_end,
                          size,
                          &mpu_region_begin,
                          &mpu_region_end,
                          &mpu_region_size);

  // Compute sub-region Disable (SRD) mask
  sr_size = mpu_region_size / MPU_SUBREGION_NBR;
  // Check if sr_size is zero to satisfy MISRA
  sr_size = (sr_size != 0) ? sr_size : MPU_SUBREGION_USE_MIN_SIZE / MPU_SUBREGION_NBR;
  srd_msk = (1u << (((mpu_region_end - address_end) + (sr_size - 1)) / sr_size)) - 1u;
  srd_msk = srd_msk << ((address_end - mpu_region_begin - 1u) / sr_size);
  srd_msk |= (1u << (((address_begin - mpu_region_begin) + (sr_size - 1)) / sr_size)) - 1u;

  if (srd_msk == 0xFF) {
    return SL_STATUS_INVALID_RANGE;
  }

  // Set region as execute never.
  region_size_encoded = mpu_region_size_encode(mpu_region_size);
  ARM_MPU_SetRegionEx(region_nbr,
                      mpu_region_begin,
                      ((region_size_encoded << MPU_RASR_SIZE_Pos) & MPU_RASR_SIZE_Msk)
                      | (ARM_MPU_AP_FULL << MPU_RASR_AP_Pos)
                      | MPU_RASR_B_Msk
                      | MPU_RASR_XN_Msk
                      | MPU_RASR_ENABLE_Msk
                      | (srd_msk << MPU_RASR_SRD_Pos));
  region_nbr++;
#endif

  // Enable MPU with default background region
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

  __DSB();
  __ISB();

  return SL_STATUS_OK;
}

#ifndef ARM_MPU_ARMV8_H
/**************************************************************************//**
 * Rounds up given number to next power of 2.
 *
 * @param nbr Number to round-up to next power of 2.
 *****************************************************************************/
static uint32_t mpu_round_up_next_pwr2(uint32_t nbr)
{
  nbr--;
  nbr |= nbr >> 1;
  nbr |= nbr >> 2;
  nbr |= nbr >> 4;
  nbr |= nbr >> 8;
  nbr |= nbr >> 16;
  nbr++;

  return nbr;
}
#endif

#ifndef ARM_MPU_ARMV8_H
/**************************************************************************//**
 * Encode region size in RASR register REGION size field format.
 *
 * @param region_size Size of region.
 *****************************************************************************/
static uint8_t mpu_region_size_encode(uint32_t region_size)
{
  return (uint8_t)(30u - __CLZ(region_size));
}
#endif

#ifndef ARM_MPU_ARMV8_H
/**************************************************************************//**
 * Compute region size.
 *
 * @param section_begin    Section begin address.
 *
 * @param section_end      Section end address.
 *
 * @param section_size     Size of section.
 *
 * @param mpu_region_begin Pointer to a region begin variable.
 *
 * @param mpu_region_end   Pointer to a region end variable.
 *
 * @param mpu_region_size  Pointer to a region size variable.
 *****************************************************************************/
static void mpu_compute_region_data(uint32_t section_begin,
                                    uint32_t section_end,
                                    uint32_t section_size,
                                    uint32_t *mpu_region_begin,
                                    uint32_t *mpu_region_end,
                                    uint32_t *mpu_region_size)
{
  uint32_t region_begin;
  uint32_t region_end;
  uint32_t region_size;

  // Compute MPU region size and begin address
  region_size = mpu_round_up_next_pwr2(section_size);
  region_size = (region_size < MPU_SUBREGION_USE_MIN_SIZE) ? MPU_SUBREGION_USE_MIN_SIZE : region_size;

  region_begin = section_begin & ~(region_size - 1u);
  region_end = region_begin + region_size;

  while (region_end < section_end) {
    region_size *= 2u;
    region_begin = section_begin & ~(region_size - 1u);
    region_end = region_begin + region_size;
  }

  *mpu_region_begin = region_begin;
  *mpu_region_end = region_end;
  *mpu_region_size = region_size;
}
#endif

#if __CORTEX_M != (0u)
/**************************************************************************//**
 * MemManage default exception handler. Reset target.
 *****************************************************************************/
__WEAK void mpu_fault_handler(void)
{
  __NVIC_SystemReset();
}

/**************************************************************************//**
 * MemManage exception handler.
 *****************************************************************************/
void MemManage_Handler(void)
{
  mpu_fault_handler();
}
#endif

#endif /* defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U) */
