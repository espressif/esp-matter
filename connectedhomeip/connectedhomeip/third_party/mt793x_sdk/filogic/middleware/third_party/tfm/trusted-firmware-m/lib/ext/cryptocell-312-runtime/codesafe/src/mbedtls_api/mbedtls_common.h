/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MBEDTLS_COMMON_H
#define MBEDTLS_COMMON_H
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \brief           Implementation of memset to zero
 *
 * \param v         adrress to set
 * \param n         size
 */
void mbedtls_zeroize_internal( void *v, size_t n );

#ifdef __cplusplus
}
#endif

#endif  /* MBEDTLS_COMMON_H */
