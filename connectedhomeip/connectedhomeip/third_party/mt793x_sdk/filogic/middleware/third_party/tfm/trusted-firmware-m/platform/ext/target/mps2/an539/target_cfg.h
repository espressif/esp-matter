/*
 * Copyright (c) 2019-2020 Arm Limited. All rights reserved.
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

#include <stdint.h>

#define TFM_DRIVER_STDIO    Driver_USART0
#define NS_DRIVER_STDIO     Driver_USART0

enum ppc_bank_e {
    PPC_SP_DO_NOT_CONFIGURE = -1,
    PPC_SP_AHB_PPC_EXP0 = 0,
    PPC_SP_APB_PPC      = 1,
    PPC_SP_APB_PPC_EXP0 = 2,
    PPC_SP_APB_PPC_EXP1 = 3,
    PPC_SP_APB_PPC_EXP2 = 4,
    PPC_SP_APB_PPC_EXP3 = 5,
};

/**
 * \brief MPU configs
 */
#define PRIVILEGED_DEFAULT_ENABLE   1
#define HARDFAULT_NMI_ENABLE        1

/**
 * \brief Store the addresses of memory regions
 */
struct memory_region_limits {
    uint32_t non_secure_code_start;
    uint32_t non_secure_partition_base;
    uint32_t non_secure_partition_limit;
    uint32_t veneer_base;
    uint32_t veneer_limit;
#ifdef BL2
    uint32_t secondary_partition_base;
    uint32_t secondary_partition_limit;
#endif /* BL2 */
};

/**
 * \brief Holds the data necessary to do isolation for a specific peripheral.
 */
struct tfm_spm_partition_platform_data_t {
    uint32_t periph_start;
    uint32_t periph_limit;
    enum ppc_bank_e periph_ppc_bank;
    int16_t periph_ppc_loc;
};

/**
 * \brief Forward declaration
 */
struct mpu_armv8m_region_cfg_t;

/**
 * \brief Enables the fault handlers BusFault, UsageFault,
 *        MemManageFault and SecureFault.
 */
enum tfm_plat_err_t enable_fault_handlers(void);

/**
 * \brief Configures the system reset request properties
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t system_reset_cfg(void);

/**
 * \brief Configures all external interrupts to target the
 *        NS state, apart for the ones associated to secure
 *        peripherals (plus MPC and PPC)
 */
enum tfm_plat_err_t nvic_interrupt_target_state_cfg(void);

/**
 * \brief This function enable the interrupts associated
 *        to the secure peripherals (plus MPC and PPC)
 */
enum tfm_plat_err_t nvic_interrupt_enable(void);

/**
 * \brief This function enables the MPU
 */
enum mpu_armv8m_error_t mpu_enable(uint32_t privdef_en, uint32_t hfnmi_en);

/**
 * \brief This function disables the MPU
 */
enum mpu_armv8m_error_t mpu_disable(void);

/**
 * \brief This function enables the given MPU region
 */
enum mpu_armv8m_error_t mpu_region_enable(
                                    struct mpu_armv8m_region_cfg_t *region_cfg);

/**
 * \brief This function dsables the given MPU region
 */
enum mpu_armv8m_error_t mpu_region_disable(uint32_t region_nr);

/**
 * \brief This function cleans all the MPU regions configs
 */
enum mpu_armv8m_error_t mpu_clean(void);

/**
 * \brief Configures the Memory Protection Controller.
 */
enum tfm_plat_err_t mpc_init_cfg(void);

/**
 * \brief Clear MPC interrupt.
 */
void mpc_clear_irq(void);

/**
 * \brief Configures the Peripheral Protection Controller.
 */
enum tfm_plat_err_t ppc_init_cfg(void);

/**
 * \brief Restict peripheral access to secure access only
 *
 * \note The function does not configure privilege
 */
void ppc_configure_to_secure(enum ppc_bank_e bank, uint32_t pos);

/**
 * \brief Allow non-secure access to peripheral
 *
 * \note The function does not configure privilege
 */
void ppc_configure_to_non_secure(enum ppc_bank_e bank, uint32_t pos);

/**
 * \brief Restict access to peripheral to privileged in secure state
 */
void ppc_configure_to_secure_priv(enum ppc_bank_e bank, uint32_t pos);

/**
 * \brief Enable unprivileged access to peripheral in secure state
 */
void ppc_configure_to_secure_unpriv(enum ppc_bank_e bank, uint32_t pos);

/**
 * \brief Clears PPC interrupt.
 */
void ppc_clear_irq(void);

/**
 * \brief Configures SAU and IDAU.
 */
void sau_and_idau_cfg(void);

#endif /* __TARGET_CFG_H__ */
