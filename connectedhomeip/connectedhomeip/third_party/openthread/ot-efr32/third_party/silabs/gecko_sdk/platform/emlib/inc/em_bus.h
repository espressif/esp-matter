/***************************************************************************//**
 * @file
 * @brief RAM and peripheral bit-field set and clear API
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef EM_BUS_H
#define EM_BUS_H

#include "sl_assert.h"
#include "em_core.h"
#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup bus BUS - Bitfield Read/Write
 * @brief BUS register and RAM bit/field read/write API
 * @details
 *  API to perform bit-band and field set/clear access to RAM and peripherals.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Perform a single-bit write operation on a 32-bit word in RAM.
 *
 * @details
 *   This function uses Cortex-M bit-banding hardware to perform an atomic
 *   read-modify-write operation on a single bit write on a 32-bit word in RAM.
 *   See the reference manual for more details about bit-banding.
 *
 * @note
 *   This function is atomic on Cortex-M cores with bit-banding support. Bit-
 *   banding is a multi cycle read-modify-write bus operation. RAM bit-banding is
 *   performed using the memory alias region at BITBAND_RAM_BASE.
 *
 * @param[in] addr An ddress of a 32-bit word in RAM.
 *
 * @param[in] bit A bit position to write, 0-31.
 *
 * @param[in] val A value to set bit to, 0 or 1.
 ******************************************************************************/
__STATIC_INLINE void BUS_RamBitWrite(volatile uint32_t *addr,
                                     unsigned int bit,
                                     unsigned int val)
{
#if defined(BITBAND_RAM_BASE)
  uint32_t aliasAddr =
    BITBAND_RAM_BASE + (((uint32_t)addr - SRAM_BASE) * (uint32_t) 32) + (bit * (uint32_t) 4);

  *(volatile uint32_t *)aliasAddr = (uint32_t)val;
#else
  uint32_t tmp = *addr;

  /* Make sure val is not more than 1 because only one bit needs to be set. */
  *addr = (tmp & ~(1UL << bit)) | ((val & 1UL) << bit);
#endif
}

/***************************************************************************//**
 * @brief
 *   Perform a single-bit read operation on a 32-bit word in RAM.
 *
 * @details
 *   This function uses Cortex-M bit-banding hardware to perform an atomic
 *   read operation on a single register bit. See the
 *   reference manual for more details about bit-banding.
 *
 * @note
 *   This function is atomic on Cortex-M cores with bit-banding support.
 *   RAM bit-banding is performed using the memory alias region
 *   at BITBAND_RAM_BASE.
 *
 * @param[in] addr RAM address.
 *
 * @param[in] bit A bit position to read, 0-31.
 *
 * @return
 *     The requested bit shifted to bit position 0 in the return value.
 ******************************************************************************/
__STATIC_INLINE unsigned int BUS_RamBitRead(volatile const uint32_t *addr,
                                            unsigned int bit)
{
#if defined(BITBAND_RAM_BASE)
  uint32_t aliasAddr =
    BITBAND_RAM_BASE + (((uint32_t)addr - SRAM_BASE) * (uint32_t) 32) + (bit * (uint32_t) 4);

  return *(volatile uint32_t *)aliasAddr;
#else
  return ((*addr) >> bit) & 1UL;
#endif
}

/***************************************************************************//**
 * @brief
 *   Perform a single-bit write operation on a peripheral register.
 *
 * @details
 *   This function uses Cortex-M bit-banding hardware to perform an atomic
 *   read-modify-write operation on a single register bit. See the
 *   reference manual for more details about bit-banding.
 *
 * @note
 *   This function is atomic on Cortex-M cores with bit-banding support. Bit-
 *   banding is a multi cycle read-modify-write bus operation. Peripheral register
 *   bit-banding is performed using the memory alias region at BITBAND_PER_BASE.
 *
 * @param[in] addr A peripheral register address.
 *
 * @param[in] bit A bit position to write, 0-31.
 *
 * @param[in] val A value to set bit to, 0 or 1.
 ******************************************************************************/
__STATIC_INLINE void BUS_RegBitWrite(volatile uint32_t *addr,
                                     unsigned int bit,
                                     unsigned int val)
{
  EFM_ASSERT(bit < 32U);
#if defined(PER_REG_BLOCK_SET_OFFSET) && defined(PER_REG_BLOCK_CLR_OFFSET)
  uint32_t aliasAddr;
  if (val != 0U) {
    aliasAddr = (uint32_t)addr + PER_REG_BLOCK_SET_OFFSET;
  } else {
    aliasAddr = (uint32_t)addr + PER_REG_BLOCK_CLR_OFFSET;
  }
  *(volatile uint32_t *)aliasAddr = 1UL << bit;
#elif defined(BITBAND_PER_BASE)
  uint32_t aliasAddr =
    BITBAND_PER_BASE + (((uint32_t)addr - PER_MEM_BASE) * (uint32_t) 32) + (bit * (uint32_t) 4);

  *(volatile uint32_t *)aliasAddr = (uint32_t)val;
#else
  uint32_t tmp = *addr;

  /* Make sure val is not more than 1 because only one bit needs to be set. */
  *addr = (tmp & ~(1 << bit)) | ((val & 1) << bit);
#endif
}

/***************************************************************************//**
 * @brief
 *   Perform a single-bit read operation on a peripheral register.
 *
 * @details
 *   This function uses Cortex-M bit-banding hardware to perform an atomic
 *   read operation on a single register bit. See the
 *   reference manual for more details about bit-banding.
 *
 * @note
 *   This function is atomic on Cortex-M cores with bit-banding support.
 *   Peripheral register bit-banding is performed using the memory alias
 *   region at BITBAND_PER_BASE.
 *
 * @param[in] addr A peripheral register address.
 *
 * @param[in] bit A bit position to read, 0-31.
 *
 * @return
 *     The requested bit shifted to bit position 0 in the return value.
 ******************************************************************************/
__STATIC_INLINE unsigned int BUS_RegBitRead(volatile const uint32_t *addr,
                                            unsigned int bit)
{
#if defined(BITBAND_PER_BASE)
  uint32_t aliasAddr =
    BITBAND_PER_BASE + (((uint32_t)addr - PER_MEM_BASE) * (uint32_t)32) + (bit * (uint32_t) 4);

  return *(volatile uint32_t *)aliasAddr;
#else
  return ((*addr) >> bit) & 1UL;
#endif
}

/***************************************************************************//**
 * @brief
 *   Perform a masked set operation on a peripheral register address.
 *
 * @details
 *   A peripheral register masked set provides a single-cycle and atomic set
 *   operation of a bit-mask in a peripheral register. All 1s in the mask are
 *   set to 1 in the register. All 0s in the mask are not changed in the
 *   register.
 *   RAMs and special peripherals are not supported. See the
 *   reference manual for more details about the peripheral register field set.
 *
 * @note
 *   This function is single-cycle and atomic on cores with peripheral bit set
 *   and clear support. It uses the memory alias region at PER_BITSET_MEM_BASE.
 *
 * @param[in] addr A peripheral register address.
 *
 * @param[in] mask A mask to set.
 ******************************************************************************/
__STATIC_INLINE void BUS_RegMaskedSet(volatile uint32_t *addr,
                                      uint32_t mask)
{
#if defined(PER_REG_BLOCK_SET_OFFSET)
  uint32_t aliasAddr = (uint32_t)addr + PER_REG_BLOCK_SET_OFFSET;
  *(volatile uint32_t *)aliasAddr = mask;
#elif defined(PER_BITSET_MEM_BASE)
  uint32_t aliasAddr = PER_BITSET_MEM_BASE + ((uint32_t)addr - PER_MEM_BASE);
  *(volatile uint32_t *)aliasAddr = mask;
#else
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  *addr |= mask;
  CORE_EXIT_CRITICAL();
#endif
}

/***************************************************************************//**
 * @brief
 *   Perform a masked clear operation on the peripheral register address.
 *
 * @details
 *   A peripheral register masked clear provides a single-cycle and atomic clear
 *   operation of a bit-mask in a peripheral register. All 1s in the mask are
 *   set to 0 in the register.
 *   All 0s in the mask are not changed in the register.
 *   RAMs and special peripherals are not supported. See the
 *   reference manual for more details about the peripheral register field clear.
 *
 * @note
 *   This function is single-cycle and atomic on cores with peripheral bit set
 *   and clear support. It uses the memory alias region at PER_BITCLR_MEM_BASE.
 *
 * @param[in] addr A peripheral register address.
 *
 * @param[in] mask A mask to clear.
 ******************************************************************************/
__STATIC_INLINE void BUS_RegMaskedClear(volatile uint32_t *addr,
                                        uint32_t mask)
{
#if defined(PER_REG_BLOCK_CLR_OFFSET)
  uint32_t aliasAddr = (uint32_t)addr + PER_REG_BLOCK_CLR_OFFSET;
  *(volatile uint32_t *)aliasAddr = mask;
#elif defined(PER_BITCLR_MEM_BASE)
  uint32_t aliasAddr = PER_BITCLR_MEM_BASE + ((uint32_t)addr - PER_MEM_BASE);
  *(volatile uint32_t *)aliasAddr = mask;
#else
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  *addr &= ~mask;
  CORE_EXIT_CRITICAL();
#endif
}

/***************************************************************************//**
 * @brief
 *   Perform peripheral register masked write.
 *
 * @details
 *   This function first reads the peripheral register and updates only bits
 *   that are set in the mask with content of val. Typically, the mask is a
 *   bit-field in the register and the value val is within the mask.
 *
 * @note
 *   The read-modify-write operation is executed in a critical section to
 *   guarantee atomicity. Note that atomicity can only be guaranteed if register
 *   is modified only by the core, and not by other peripherals (like DMA).
 *
 * @param[in] addr A peripheral register address.
 *
 * @param[in] mask A peripheral register mask.
 *
 * @param[in] val  A peripheral register value. The value must be shifted to the
                  correct bit position in the register corresponding to the field
                  defined by the mask parameter. The register value must be
                  contained in the field defined by the mask parameter. The
                  register value is masked to prevent involuntary spillage.
 ******************************************************************************/
__STATIC_INLINE void BUS_RegMaskedWrite(volatile uint32_t *addr,
                                        uint32_t mask,
                                        uint32_t val)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  *addr = (*addr & ~mask) | (val & mask);
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *   Perform a peripheral register masked read.
 *
 * @details
 *   Read an unshifted and masked value from a peripheral register.
 *
 * @note
 *   This operation is not hardware accelerated.
 *
 * @param[in] addr A peripheral register address.
 *
 * @param[in] mask A peripheral register mask.
 *
 * @return
 *   An unshifted and masked register value.
 ******************************************************************************/
__STATIC_INLINE uint32_t BUS_RegMaskedRead(volatile const uint32_t *addr,
                                           uint32_t mask)
{
  return *addr & mask;
}

/** @} (end addtogroup bus) */

#ifdef __cplusplus
}
#endif

#endif /* EM_BUS_H */
