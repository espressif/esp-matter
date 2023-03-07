/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SPM_HAL_H__
#define __TFM_SPM_HAL_H__

#include <stdint.h>
#include "tfm_secure_api.h"
#include "spm_api.h"
#ifdef TFM_MULTI_CORE_TOPOLOGY
#include "tfm_multi_core.h"
#endif
#include "tfm_plat_defs.h"

/**
 * \brief Holds peripheral specific data fields required to manage the
 *        peripherals isolation
 *
 * This structure has to be defined in the platform directory, and may have
 * different definition for each platform. The structure should contain fields
 * that describe the peripheral for the functions that are prototyped in this
 * file and are responsible for configuring the isolation of the peripherals.
 *
 * Pointers to structures of this type are managed by the SPM, and passed to the
 * necessary function on isolation request. The pointers are also defined by the
 * platform in the header file tfm_peripherals_def.h. For details on this, see
 * the documentation of that file.
 */
struct tfm_spm_partition_platform_data_t;

enum irq_target_state_t {
    TFM_IRQ_TARGET_STATE_SECURE,
    TFM_IRQ_TARGET_STATE_NON_SECURE,
};

#ifdef TFM_PSA_API
/**
 * \brief Holds SPM db fields that define the memory regions used by a
 *        partition.
 */
struct tfm_spm_partition_memory_data_t
{
    uint32_t code_start;   /*!< Start of the code memory of this partition. */
    uint32_t code_limit;   /*!< Address of the byte beyond the end of the code
                            *   memory of this partition.
                            */
    uint32_t ro_start;     /*!< Start of the read only memory of this
                            *   partition.
                            */
    uint32_t ro_limit;     /*!< Address of the byte beyond the end of the read
                            *   only memory of this partition.
                            */
    uint32_t rw_start;     /*!< Start of the data region of this partition. */
    uint32_t rw_limit;     /*!< Address of the byte beyond the end of the data
                            *   region of this partition.
                            */
    uint32_t zi_start;     /*!< Start of the zero initialised data region of
                            *   this partition.
                            */
    uint32_t zi_limit;     /*!< Address of the byte beyond the end of the zero
                            *   initialised region of this partition.
                            */
    uint32_t stack_bottom; /*!< The bottom of the stack for the partition. */
    uint32_t stack_top;    /*!< The top of the stack for the partition. */
};
#endif

/**
 * \brief This function initializes peripherals common to all platforms.
 *
 * Contrarily to SystemInit() intended for a high-priority hw initialization
 * (for example clock and power subsystems), and called on a very early boot
 * stage from startup code, this function is called from C code, hence variables
 * and other drivers data are protected from being cleared up by the C library
 * init.
 * In addition to performing initialization common to all platforms, it also
 * calls tfm_spm_hal_post_init_platform() function which implements
 * initialization of platform-specific peripherals and other hw.
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_post_init(void);

/**
 * \brief This function initializes platform-specific peripherals and hardware.
 *
 * Called from tfm_spm_hal_post_init(), this function is intended for
 * platform-specific portion of hardware initialization.
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_post_init_platform(void);

/**
 * \brief This function initialises the HW used for isolation, and sets the
 *        default configuration for them.
 *
 * This function is called during TF-M core early startup, before DB init
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_init_isolation_hw(void);

#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
/**
 * \brief This function initialises the HW used for isolation, and sets the
 *        default configuration for them.
 * This function is called during TF-M core early startup, after DB init
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_setup_isolation_hw(void);
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

/**
 * \brief Configure peripherals for a partition based on the platform data and
 *        partition index from the DB
 *
 * This function is called during partition initialisation (before calling the
 * init function for the partition)
 *
 * \param[in] partition_idx    The index of the partition that this peripheral
 *                             is assigned to.
 * \param[in] platform_data    The platform fields of the partition DB record to
 *                             be used for configuration.
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_configure_default_isolation(
                 uint32_t partition_idx,
                 const struct tfm_spm_partition_platform_data_t *platform_data);
/**
 * \brief Configures the system debug properties.
 *        The default configuration of this function should disable secure debug
 *        when either DAUTH_NONE or DAUTH_NS_ONLY define is set. It is up to the
 *        platform owner to decide if secure debug can be turned on in their
 *        system, if DAUTH_FULL define is present.
 *        The DAUTH_CHIP_DEFAULT define should not be considered a safe default
 *        option unless explicitly noted by the chip vendor.
 *        The implementation has to expect that one of those defines is going to
 *        be set. Otherwise, a compile error needs to be triggered.
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_init_debug(void);

/**
 * \brief Enables the fault handlers and sets priorities.
 *
 * Secure fault (if present) must have the highest possible priority
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_enable_fault_handlers(void);

/**
 * \brief Configures the system reset request properties
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_system_reset_cfg(void);

/**
 * \brief System reset
 */
void tfm_spm_hal_system_reset(void);

/**
 * \brief Configures all external interrupts to target the
 *        NS state, apart for the ones associated to secure
 *        peripherals (plus MPC and PPC)
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_target_state_cfg(void);

/**
 * \brief This function enable the interrupts associated
 *        to the secure peripherals (plus the isolation boundary violation
 *        interrupts)
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_enable(void);

/**
 * \brief Get the VTOR value of non-secure image
 *
 * \return Returns the address where the vector table of the non-secure image
 *         is located
 */
uint32_t tfm_spm_hal_get_ns_VTOR(void);

/**
 * \brief Get the initial address of non-secure image main stack
 *
 * \return Returns the initial non-secure MSP
 */
uint32_t tfm_spm_hal_get_ns_MSP(void);

/**
 * \brief Get the entry point of the non-secure image
 *
 * \return Returns the address of the non-secure image entry point
 */
uint32_t tfm_spm_hal_get_ns_entry_point(void);

/**
 * \brief Set the priority of a secure IRQ
 *
 * \param[in] irq_line    The IRQ to set the priority for. Might be less than 0
 * \param[in] priority    The priority to set. [0..255]
 *
 * \details This function sets the priority for the IRQ passed in the parameter.
 *          The precision of the priority value might be adjusted to match the
 *          available priority bits in the underlying target platform.
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_set_secure_irq_priority(IRQn_Type irq_line,
                                                        uint32_t priority);

/**
 * \brief Clears a pending IRQ
 *
 * \param[in] irq_line    The IRQ to clear pending for.
 */
void tfm_spm_hal_clear_pending_irq(IRQn_Type irq_line);

/**
 * \brief Enables an IRQ
 *
 * \param[in] irq_line    The IRQ to be enabled.
 */
void tfm_spm_hal_enable_irq(IRQn_Type irq_line);

/**
 * \brief Disables an IRQ
 *
 * \param[in] irq_line    The IRQ to be disabled
 */
void tfm_spm_hal_disable_irq(IRQn_Type irq_line);

/**
 * \brief Set the target state of an IRQ
 *
 * \param[in] irq_line      The IRQ to set the priority for.
 * \param[in] target_state  Target state to ret for the IRQ.
 *
 * \return                TFM_IRQ_TARGET_STATE_SECURE if interrupt is assigned
 *                        to Secure
 *                        TFM_IRQ_TARGET_STATE_NON_SECURE if interrupt is
 *                        assigned to Non-Secure
 */
enum irq_target_state_t tfm_spm_hal_set_irq_target_state(
                                          IRQn_Type irq_line,
                                          enum irq_target_state_t target_state);

/**
 * \brief This function setups the platform isolation
 *
 * This function is called after TF-M DB init
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_spm_hal_post_isolation_hw(void);

#ifdef TFM_MULTI_CORE_TOPOLOGY
/**
 * \brief Performs the necessary actions to start the non-secure CPU running
 *        the code at the specified address.
 *
 * \param[in] start_addr       The entry point address of non-secure code.
 */
void tfm_spm_hal_boot_ns_cpu(uintptr_t start_addr);

/**
 * \brief Called on the secure CPU.
 *        Flags that the secure CPU has completed its initialization
 *        Waits, if necessary, for the non-secure CPU to flag that
 *        it has completed its initialisation
 */
void tfm_spm_hal_wait_for_ns_cpu_ready(void);

/**
 * \brief Retrieve the current active security configuration information and
 *        fills the \ref security_attr_info_t.
 *
 * \param[in]  p               Base address of target memory region
 * \param[in]  s               Size of target memory region
 * \param[out] p_attr          Address of \ref security_attr_info_t to be filled
 *
 * \return void
 */
void tfm_spm_hal_get_mem_security_attr(const void *p, size_t s,
                                       struct security_attr_info_t *p_attr);

/**
 * \brief Retrieve the secure memory protection configuration information and
 *        fills the \ref mem_attr_info_t.
 *
 * \param[in]  p               Base address of target memory region
 * \param[in]  s               Size of target memory region
 * \param[out] p_attr          Address of \ref mem_attr_info_t to be filled
 *
 * \return void
 */
void tfm_spm_hal_get_secure_access_attr(const void *p, size_t s,
                                        struct mem_attr_info_t *p_attr);

/**
 * \brief Retrieve the non-secure memory protection configuration information
 *        and fills the \ref mem_attr_info_t.
 *
 * \param[in]  p               Base address of target memory region
 * \param[in]  s               Size of target memory region
 * \param[out] p_attr          Address of \ref mem_attr_info_t to be filled
 *
 * \return void
 */
void tfm_spm_hal_get_ns_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr);

#endif /*TFM_MULTI_CORE_TOPOLOGY*/

#endif /* __TFM_SPM_HAL_H__ */
