/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_NSPM_H__
#define __TFM_NSPM_H__

#include <stdint.h>

#if defined(__GNUC__) && !defined(TFM_MULTI_CORE_TOPOLOGY)
/*
 * The macro cmse_nsfptr_create defined in the gcc library uses the non-standard
 * gcc C lanuage extension 'typeof'. TF-M is built with '-std=c99' so typeof
 * cannot be used in the code. As a workaround cmse_nsfptr_create is redefined
 * here to use only standard language elements.
 */
#undef cmse_nsfptr_create
#define cmse_nsfptr_create(p) ((intptr_t) (p) & ~1)

/*!
 * \def __tfm_nspm_secure_gateway_attributes__
 *
 * \brief Attributes for secure gateway functions for NSPM
 */
#ifndef __ARMCC_VERSION
/*
 * GNUARM requires noclone attribute to protect gateway function symbol from
 * being renamed and cloned
 */
#define __tfm_nspm_secure_gateway_attributes__ \
        __attribute__((cmse_nonsecure_entry, noclone))
#else
#define __tfm_nspm_secure_gateway_attributes__ \
        __attribute__((cmse_nonsecure_entry))
#endif /* !__ARMCC_VERSION */
#endif /* __GNUC__ && !TFM_MULTI_CORE_TOPOLOGY */

#ifndef TFM_PSA_API
/**
 * \brief initialise the NS context database
 */
void tfm_nspm_configure_clients(void);
#endif

/**
 * \brief Get the client ID of the current NS client
 *
 * \return The client id of the current NS client. 0 (invalid client id) is
 *         returned in case of error.
 */
int32_t tfm_nspm_get_current_client_id(void);

#ifdef TFM_PSA_API
/**
 * \brief NSPM thread main entry function
 *
 * Note: This function should not return back.
 */
void tfm_nspm_thread_entry(void);
#endif

#ifdef TFM_MULTI_CORE_TOPOLOGY
/* Unnecessary to configure Non-secure side code */
#define configure_ns_code()               do {} while (0)
#else
/*
 * \brief Configure Non-secure code, such as vector table, MSP and entry point.
 */
void configure_ns_code(void);
#endif

#endif /* __TFM_NSPM_H__ */
