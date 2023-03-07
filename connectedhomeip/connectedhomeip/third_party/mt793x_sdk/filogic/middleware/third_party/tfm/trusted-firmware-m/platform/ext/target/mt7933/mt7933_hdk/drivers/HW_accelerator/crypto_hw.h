/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CRYPTO_HW_H__
#define __CRYPTO_HW_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AES_256_BIT_KEY_SIZE            32

/* The context data-base used by the AES functions on the low level */
typedef struct AesContext {
    /* AES Key: fixed size is 128 bit = 512/2*/
    uint8_t     keyBuf[AES_256_BIT_KEY_SIZE];
    /* keySize: 128, 192, 256 */
    uint32_t    keySizeByte;
} AesContext_t;


/**
 * \brief Initialize the GCPU crypto accelerator
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_init(void);

/**
 * \brief Deallocate the GCPU crypto accelerator
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_finish(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CRYPTO_HW_H__ */
