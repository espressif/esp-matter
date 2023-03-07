/***************************************************************************//**
 * @file
 * @brief Power Manager Internal API definition.
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

#include "sl_power_manager.h"
#include "sl_slist.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_EMLIB_CORE_DEBUG_CONFIG_PRESENT)
#include "emlib_core_debug_config.h"
#endif

#if !defined(SL_EMLIB_CORE_ENABLE_INTERRUPT_DISABLED_TIMING)
#define SL_EMLIB_CORE_ENABLE_INTERRUPT_DISABLED_TIMING   0
#endif

#if (SL_EMLIB_CORE_ENABLE_INTERRUPT_DISABLED_TIMING == 1)
#include "sl_cycle_counter.h"
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define SLI_POWER_MANAGER_EM_TABLE_SIZE  2

/*******************************************************************************
 *****************************   DATA TYPES   *********************************
 ******************************************************************************/

// Debug entry
typedef struct {
  sl_slist_node_t node;
  const char *module_name;
} sli_power_debug_requirement_entry_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void sli_power_manager_init_hardware(void);

void sli_power_manager_apply_em(sl_power_manager_em_t em);

void sli_power_manager_debug_init(void);

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
void sli_power_manager_save_states(void);

void sli_power_manager_handle_pre_deepsleep_operations(void);

void sli_power_manager_restore_high_freq_accuracy_clk(void);

bool sli_power_manager_is_high_freq_accuracy_clk_ready(bool wait);

void sli_power_manager_restore_states(void);

/*******************************************************************************
 * Converts microseconds time in sleeptimer ticks.
 ******************************************************************************/
uint32_t sli_power_manager_convert_delay_us_to_tick(uint32_t time_us);

/*******************************************************************************
 * Returns the default minimum offtime for xtal high frequency oscillator.
 ******************************************************************************/
uint32_t sli_power_manager_get_default_high_frequency_minimum_offtime(void);

#if defined(EMU_VSCALE_PRESENT)
/***************************************************************************//**
 * Enable or disable fast wake-up in EM2 and EM3
 *
 * @note Will also update the wake up time from EM2 to EM0.
 ******************************************************************************/
void sli_power_manager_em23_voltage_scaling_enable_fast_wakeup(bool enable);
#endif

/*******************************************************************************
 * Restores the Low Frequency clocks according to which LF oscillators are used.
 ******************************************************************************/
void sli_power_manager_low_frequency_restore(void);
#endif

/*******************************************************************************
 * Gets the delay associated the wake-up process from EM23.
 *
 * @return Delay for the complete wake-up process with full restore.
 ******************************************************************************/
uint32_t sli_power_manager_get_wakeup_process_time_overhead(void);
