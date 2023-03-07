/*
 *  RFC 1321 compliant MD5 implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 *  The MD5 algorithm was designed by Ron Rivest in 1991.
 *
 *  http://www.ietf.org/rfc/rfc1321.txt
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_MD5_C)

#include "mbedtls/md5.h"
#include <string.h>

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#endif /* MBEDTLS_PLATFORM_C */

#if defined(MBEDTLS_MD5_ALT)

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
#include "top.h"
#endif

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

void mbedtls_md5_init( mbedtls_md5_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_md5_context ) );
}

void mbedtls_md5_free( mbedtls_md5_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_md5_context ) );
}

void mbedtls_md5_clone( mbedtls_md5_context *dst,
                        const mbedtls_md5_context *src )
{
    *dst = *src;
}

/*
 * MD5 context setup
 */
int mbedtls_md5_starts_ret( mbedtls_md5_context *ctx )
{
    hal_md5_status_t status = HAL_MD5_STATUS_ERROR;

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo96M();
#endif

    do {
        status = hal_md5_init(ctx);
        if( status == -100 )
            hal_gpt_delay_ms(1);
    } while( status == -100 );
    
#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo192M();
#endif
    return HAL_MD5_STATUS_OK == status ? 0 : (int)status;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_md5_starts( mbedtls_md5_context *ctx )
{
    mbedtls_md5_starts_ret( ctx );
}
#endif

/*
 * MD5 process buffer
 */
int mbedtls_md5_update_ret( mbedtls_md5_context *ctx,
                            const unsigned char *input,
                            size_t ilen )

{
    hal_md5_status_t status = HAL_MD5_STATUS_ERROR;

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo96M();
#endif

    do {
        status = hal_md5_append( ctx, (uint8_t*)input, (uint32_t)ilen);
        if( status == -100 )
            hal_gpt_delay_ms(1);
    } while( status == -100 );

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo192M();
#endif
    return HAL_MD5_STATUS_OK == status ? 0 : (int)status;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_md5_update( mbedtls_md5_context *ctx,
                         const unsigned char *input,
                         size_t ilen )
{
    mbedtls_md5_update_ret( ctx, input, ilen );
}
#endif

/*
 * MD5 final digest
 */
int mbedtls_md5_finish_ret( mbedtls_md5_context *ctx,
                            unsigned char output[16] )

{
    hal_md5_status_t status = HAL_MD5_STATUS_ERROR;

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo96M();
#endif

    do {
        status = hal_md5_end( ctx, (uint8_t*)output);
        if( status == -100 )
            hal_gpt_delay_ms(1);
    } while( status == -100 );

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo192M();
#endif
    return HAL_MD5_STATUS_OK == status ? 0 : (int)status;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_md5_finish( mbedtls_md5_context *ctx,
                         unsigned char output[16] )
{
    mbedtls_md5_finish_ret( ctx, output );
}
#endif

int mbedtls_internal_md5_process( mbedtls_md5_context *ctx,
                                  const unsigned char data[64] )

{
    unsigned char output[16] = {0};
    int ret = -1;

    ret = mbedtls_md5_starts_ret(ctx);
    if (0 != ret)
    {
        return ret;
    }

    ret = mbedtls_md5_update_ret(ctx, data, 64);
    if (0 != ret)
    {
        return ret;
    }

    ret = mbedtls_md5_finish_ret(ctx, output);
    return ret;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_md5_process( mbedtls_md5_context *ctx,
                          const unsigned char data[64] )
{
    mbedtls_internal_md5_process( ctx, data );
}
#endif

#endif /* MBEDTLS_MD5_ALT */

#endif /* MBEDTLS_MD5_C */

