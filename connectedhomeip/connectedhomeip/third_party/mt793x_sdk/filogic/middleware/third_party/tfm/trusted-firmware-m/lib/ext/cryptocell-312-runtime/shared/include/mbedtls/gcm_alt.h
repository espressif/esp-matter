/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MBEDTLS_GCM_ALT_H
#define MBEDTLS_GCM_ALT_H

#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_GCM_C)

#include <stddef.h>
#include <stdint.h>
#include "mbedtls/cipher.h"

#define MBEDTLS_GCM_ENCRYPT     1
#define MBEDTLS_GCM_DECRYPT     0

#define MBEDTLS_ERR_GCM_AUTH_FAILED             -0x0012  /**< Authenticated decryption failed. */
#define MBEDTLS_ERR_GCM_BAD_INPUT               -0x0014  /**< Bad input parameters to function. */

/* hide internal implementation of the struct. Allocate enough space for it.*/
#define MBEDTLS_GCM_CONTEXT_SIZE_IN_WORDS   40

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          GCM context structure
 */
typedef struct {
    uint32_t buf[MBEDTLS_GCM_CONTEXT_SIZE_IN_WORDS];
}
mbedtls_gcm_context;

#ifdef __cplusplus
}
#endif



#endif /* MBEDTLS_GCM_C */
#endif /* MBEDTLS_GCM_ALT_H */
