/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_INTERNAL_H__
#define __TFM_INTERNAL_H__

#include "secure_utilities.h"
#include "tfm_arch.h"

#ifndef TFM_PSA_API
/*
 * This function pointer is meant to only hold non secure function pointers.
 * It will be turned into a non-secure one (LSB cleared) before being called
 * whatever happens anyway (unless cast to another function pointer type).
 * Registers will be cleared before branching so that no information leaks
 * from secure to non-secure world.
 */
typedef void (*nsfptr_t) (void) __attribute__((cmse_nonsecure_call));

extern nsfptr_t ns_entry;

/**
 * \brief Jumps to non-secure code.
 */
void jump_to_ns_code(void);
#else /* !defined(TFM_PSA_API) */

/**
 * \brief Move to handler mode by a SVC for specific purpose
 */
void tfm_core_handler_mode(void);
#endif /* !defined(TFM_PSA_API) */

/**
 * \brief Retrieve secure partition related data from shared memory area, which
 *        stores shared data between bootloader and runtime firmware.
 *
 * \param[in] args  Pointer to stack frame, which carries input parameters.
 */
void tfm_core_get_boot_data_handler(uint32_t args[]);

/**
 * \brief Validate the content of shared memory area, which stores the shared
 *        data between bootloader and runtime firmware.
 */
void tfm_core_validate_boot_data(void);

#endif /* __TFM_INTERNAL_H__ */
