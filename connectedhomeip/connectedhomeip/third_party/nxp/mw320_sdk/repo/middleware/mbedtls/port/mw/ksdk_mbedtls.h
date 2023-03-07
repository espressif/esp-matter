/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef KSDK_MBEDTLS_H
#define KSDK_MBEDTLS_H

#ifdef __cplusplus
extern "C" {
#endif

void CRYPTO_InitHardware(void);

void mbedtls_hardware_init_hash(uint8_t *entropy, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* KSDK_MBEDTLS_H */
