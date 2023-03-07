/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "cc_pal_types.h"
#include "cc_pal_log.h"

/* Implementation that should never be optimized out by the compiler */

void mbedtls_zeroize_internal( void *v, size_t n )
{
    volatile unsigned char *p = NULL;
    if( NULL == v )
    {
        CC_PAL_LOG_ERR( "input is NULL\n" );
        return;
    }
    p = (unsigned char*)v; while( n-- ) *p++ = 0;
}

