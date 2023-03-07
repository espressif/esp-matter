/*
 * corePKCS11 V3.0.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file mbedtls_stubs.c
 * @brief Stubs for mbed TLS functions.
 */

#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/threading.h"


void mbedtls_entropy_init( mbedtls_entropy_context * ctx )
{
    __CPROVER_assert( ctx != NULL, "Received an unexpected NULL pointer." );
}

void mbedtls_entropy_free( mbedtls_entropy_context * ctx )
{
    __CPROVER_assert( ctx != NULL, "Received an unexpected NULL pointer." );
}

void mbedtls_ctr_drbg_init( mbedtls_ctr_drbg_context * ctx )
{
    __CPROVER_assert( ctx != NULL, "Received an unexpected NULL pointer." );
}

int mbedtls_ctr_drbg_seed( mbedtls_ctr_drbg_context * ctx,
                           int ( * f_entropy )( void *, unsigned char *, size_t ),
                           void * p_entropy,
                           const unsigned char * custom,
                           size_t len )
{
    __CPROVER_assert( ctx != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( f_entropy != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( p_entropy != NULL, "Received an unexpected NULL pointer." );
    return nondet_bool() ? 0 : -1;
}

int mbedtls_ctr_drbg_random( void * p_rng,
                             unsigned char * output,
                             size_t output_len )
{
    __CPROVER_assert( p_rng != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( output != NULL, "Received an unexpected NULL pointer." );
    return nondet_bool() ? 0 : -1;
}

void mbedtls_ctr_drbg_free( mbedtls_ctr_drbg_context * ctx )
{
    __CPROVER_assert( ctx != NULL, "Received an unexpected NULL pointer." );
}

int mbedtls_pk_sign( mbedtls_pk_context * ctx,
                     mbedtls_md_type_t md_alg,
                     const unsigned char * hash,
                     size_t hash_len,
                     unsigned char * sig,
                     size_t * sig_len,
                     int ( * f_rng )( void *, unsigned char *, size_t ),
                     void * p_rng )
{
    __CPROVER_assert( ctx != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( hash != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( sig != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( sig_len != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( f_rng != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( p_rng != NULL, "Received an unexpected NULL pointer." );
    return nondet_bool() ? 0 : -1;
}

int mbedtls_ecdsa_verify( mbedtls_ecp_group * grp,
                          const unsigned char * buf,
                          size_t blen,
                          const mbedtls_ecp_point * Q,
                          const mbedtls_mpi * r,
                          const mbedtls_mpi * s )
{
    __CPROVER_assert( grp != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( buf != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( Q != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( r != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( s != NULL, "Received an unexpected NULL pointer." );

    return nondet_bool() ? 0 : -1;
}

void mbedtls_sha256_init( mbedtls_sha256_context * ctx )
{
    __CPROVER_assert( ctx != NULL, "Received an unexpected NULL pointer." );
    return nondet_bool() ? 0 : -1;
}

int mbedtls_sha256_starts_ret( mbedtls_sha256_context * ctx,
                               int is224 )
{
    __CPROVER_assert( ctx != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( is224 == 0, "We are only doing sha256 currently." );
    return nondet_bool() ? 0 : -1;
}

int mbedtls_sha256_finish_ret( mbedtls_sha256_context * ctx,
                               unsigned char output[ 32 ] )
{
    __CPROVER_assert( ctx != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( output != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( __CPROVER_OBJECT_SIZE( output ) == 32UL, "SHA256 output buffers must be 32 bytes." );

    return 32;
}

int mbedtls_pk_verify( mbedtls_pk_context * ctx,
                       mbedtls_md_type_t md_alg,
                       const unsigned char * hash,
                       size_t hash_len,
                       const unsigned char * sig,
                       size_t sig_len )
{
    __CPROVER_assert( ctx != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( hash != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assert( sig != NULL, "Received an unexpected NULL pointer." );
    return nondet_bool() ? 0 : -1;
}



static void threading_mutex_init( mbedtls_threading_mutex_t * mutex )
{
    mbedtls_threading_mutex_t * m = malloc( sizeof( mbedtls_threading_mutex_t ) );

    __CPROVER_assert( mutex != NULL, "Received an unexpected NULL pointer." );
    __CPROVER_assume( m != NULL );
    mutex = m;
}

static void threading_mutex_free( mbedtls_threading_mutex_t * mutex )
{
    __CPROVER_assert( mutex != NULL, "Received an unexpected NULL pointer." );
}


static int threading_mutex_lock( mbedtls_threading_mutex_t * mutex )
{
    __CPROVER_assert( mutex != NULL, "Received an unexpected NULL pointer." );
    return nondet_bool() ? 0 : -1;
}

static int threading_mutex_unlock( mbedtls_threading_mutex_t * mutex )
{
    __CPROVER_assert( mutex != NULL, "Received an unexpected NULL pointer." );
    return nondet_bool() ? 0 : -1;
}

void (* mbedtls_mutex_init)( mbedtls_threading_mutex_t * ) = threading_mutex_init;
void (* mbedtls_mutex_free)( mbedtls_threading_mutex_t * ) = threading_mutex_free;
int (* mbedtls_mutex_lock)( mbedtls_threading_mutex_t * ) = threading_mutex_lock;
int (* mbedtls_mutex_unlock)( mbedtls_threading_mutex_t * ) = threading_mutex_unlock;
