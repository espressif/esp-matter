/*
 * corePKCS11 V1.1.0
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

/* This is a dummy file that contains OS layer stubs. This is needed in order to use
 * CMock to generate the necessary mocks for the mutex and memory function used
 * by the PKCS #11 mbed TLS implementation.
 */
#ifndef MOCK_OSAL_H_
#define MOCK_OSAL_H_

#include <stddef.h>
#include "threading_alt.h"

void * mock_osal_malloc( size_t size );
void * mock_osal_calloc( size_t nitems,
                         size_t size );
void mock_osal_free( void * ptr );

void mock_osal_mutex_init( mbedtls_threading_mutex_t * );
void mock_osal_mutex_free( mbedtls_threading_mutex_t * );
int mock_osal_mutex_lock( mbedtls_threading_mutex_t * );
int mock_osal_mutex_unlock( mbedtls_threading_mutex_t * );

#endif /* ifndef MOCK_OSAL_H_ */
