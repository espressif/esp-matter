/***************************************************************************//**
 * @file
 * @brief Keyscan (KEYSCAN) peripheral API
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "peripheral_keyscan.h"
#if defined(KEYSCAN_COUNT) && (KEYSCAN_COUNT > 0)
#include "sl_assert.h"
#include "em_bus.h"

/***************************************************************************//**
 * @addtogroup keyscan KEYSCAN - Keyboard Scan
 * @brief Keyscan (KEYSCAN) Peripheral API
 * @details
 *  This module contains functions to control the KEYSCAN peripheral of Silicon
 *  Labs 32-bit MCUs and SoCs. The KEYSCAN module connects through rows and
 *  columns of GPIOs to an external mechanical keypad.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Initializes KEYSCAN module.
 ******************************************************************************/
void sl_keyscan_init(const sl_keyscan_config_t *p_config)
{
  // Wait to be ready
  sl_keyscan_wait_ready();

  if (KEYSCAN->EN == KEYSCAN_EN_EN) {
    // Disable KEYSCAN module
    KEYSCAN->EN_CLR = KEYSCAN_EN_EN;
    while (KEYSCAN->EN & _KEYSCAN_EN_DISABLING_MASK) {
      // Wait for disabling to finished
    }
  }

  // A sanity check of configuration parameters.
  EFM_ASSERT(p_config->clock_divider <= _KEYSCAN_CFG_CLKDIV_MASK);
  EFM_ASSERT(p_config->column_number <= KEYSCAN_COLNUM);
  EFM_ASSERT(p_config->row_number <= KEYSCAN_ROWNUM);
  EFM_ASSERT(p_config->scan_delay <= SL_KEYSCAN_DELAY_32MS);
  EFM_ASSERT(p_config->debounce_delay <= SL_KEYSCAN_DELAY_32MS);
  EFM_ASSERT(p_config->stable_delay <= SL_KEYSCAN_DELAY_32MS);

  // Set configuration
  KEYSCAN->CFG = ((p_config->clock_divider) << _KEYSCAN_CFG_CLKDIV_SHIFT)
                 | ((p_config->auto_start_enable) << _KEYSCAN_CFG_AUTOSTART_SHIFT)
                 | ((p_config->single_press_enable) << _KEYSCAN_CFG_SINGLEPRESS_SHIFT)
                 | ((p_config->column_number - 1) << _KEYSCAN_CFG_NUMCOLS_SHIFT)
                 | ((p_config->row_number - 1) << _KEYSCAN_CFG_NUMROWS_SHIFT);

  KEYSCAN->DELAY = ((p_config->scan_delay) << _KEYSCAN_DELAY_SCANDLY_SHIFT)
                   | ((p_config->debounce_delay) << _KEYSCAN_DELAY_DEBDLY_SHIFT)
                   | ((p_config->stable_delay) << _KEYSCAN_DELAY_STABDLY_SHIFT);
}

/***************************************************************************//**
 * Enables KEYSCAN module.
 ******************************************************************************/
void sl_keyscan_enable(void)
{
  if (KEYSCAN->EN != 0U) {
    // Wait for synchronization before writing to EN register
    sl_keyscan_wait_sync();
  }

  // Enable KEYSCAN module
  KEYSCAN->EN_SET = KEYSCAN_EN_EN;
}

/***************************************************************************//**
 * Disables KEYSCAN module.
 ******************************************************************************/
void sl_keyscan_disable(void)
{
  // Quick exit if we want to disable KEYSCAN and it's already disabled.
  if (KEYSCAN->EN == 0U) {
    return;
  }

  // Stop scan if running
  if (KEYSCAN->STATUS & _KEYSCAN_STATUS_RUNNING_MASK) {
    sl_keyscan_stop_scan();
  }

  // Wait for synchronization to complete
  sl_keyscan_wait_sync();

  // Disable module
  KEYSCAN->EN_CLR = KEYSCAN_EN_EN;
}

/***************************************************************************//**
 * Restores KEYSCAN to its reset state.
 *
 * @note
 *   The register STATUS get reset value after enabling the module because
 *   it is of type RSYNC
 ******************************************************************************/
void sl_keyscan_reset(void)
{
  // Stop scan if running
  if (KEYSCAN->STATUS & _KEYSCAN_STATUS_RUNNING_MASK) {
    sl_keyscan_stop_scan();
  }

  sl_keyscan_enable();

  // Wait for synchronization to complete
  sl_keyscan_wait_sync();

  // Software reset command
  KEYSCAN->SWRST_SET = KEYSCAN_SWRST_SWRST;
}

/** @} (end addtogroup keyscan) */
#endif /* defined(KEYSCAN_COUNT) && (KEYSCAN_COUNT > 0) */
