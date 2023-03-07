/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MBEDTLS_HASH_COMMON_H
#define MBEDTLS_HASH_COMMON_H
#include "mbedtls_common.h"
#ifdef __cplusplus
extern "C"
{
#endif


uint32_t   mbedtls_hashUpdate(void   *pHashUserCtx,
                        uint8_t     *pDataIn,
                        size_t      dataInSize);

void mbedtls_sha_init_internal( void *ctx );

int mbedtls_sha_process_internal( void *ctx, const unsigned char *data );

int mbedtls_sha_finish_internal( void *ctx );

int mbedtls_sha_update_internal( void *ctx, const unsigned char *input, size_t ilen );

int mbedtls_sha_starts_internal( void *ctx, hashMode_t mode);




#ifdef __cplusplus
}
#endif

#endif  /* MBEDTLS_HASH_COMMON_H */
