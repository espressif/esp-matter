/*
 * Copyright (c) 2018 ARM Limited
 * Copyright (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TARGET_CFG_H__
#define __TARGET_CFG_H__

#include "platform/ext/common/uart_stdout.h"
#include "tfm_peripherals_def.h"

#define TFM_DRIVER_STDIO    Driver_USART5
#define NS_DRIVER_STDIO     Driver_USART5

/**
 * \brief Store the addresses of memory regions
 */
struct memory_region_limits {
    uint32_t non_secure_code_start;
    uint32_t non_secure_partition_base;
    uint32_t non_secure_partition_limit;
};

/**
 * \brief Holds the data necessary to do isolation for a specific peripheral.
 */
struct tfm_spm_partition_platform_data_t
{
    uint32_t periph_start;
    uint32_t periph_limit;
    int16_t periph_ppc_bank;
    int16_t periph_ppc_loc;
};

/**
 * \brief Configures the Shared Memory Protection Units.
 */
void smpu_init_cfg(void);

/**
 * \brief Prints out the Shared Memory Protection Units config.
 */
void smpu_print_config(void);

/**
 * \brief Configures the Peripheral Protection Units.
 */
void ppu_init_cfg(void);

/**
 * \brief Configure bus masters/Protectoin Contexts.
 */
void bus_masters_cfg(void);

/**
 * \brief Performs platform specific hw initialization.
 */
void platform_init(void);

/**
 * \brief Configures all external interrupts to target the
 *        NS state, apart for the ones associated to secure
 *        peripherals.
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t nvic_interrupt_target_state_cfg(void);

/**
 * \brief This function enable the interrupts associated
 *        to the secure peripherals (plus the isolation boundary violation
 *        interrupts)
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t nvic_interrupt_enable(void);

#endif /* __TARGET_CFG_H__ */
