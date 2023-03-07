/**
 * \file threading_alt.h
 *
 * \brief Threading abstraction layer
 */
/*
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

#ifndef __THREADING_ALT_H__
#define __THREADING_ALT_H__

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_THREADING_C)

#if defined(MBEDTLS_THREADING_ALT)
#include "FreeRTOS.h"
#include "semphr.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	SemaphoreHandle_t mutex;
	char is_valid;
} mbedtls_threading_mutex_t;


/**
 * \brief           An implementation of mutex init function based on
 *                  FreeRTOS.
 *
 * \param mutex    mutex handler.
 *
 * \note           Call mbedtls_threading_set_alt() to set it to the 
 *                  implementation of mutex_init().
 *
 * \note           It does not return a status code. If it fails, it will leave
 *                  its argument (the mutex) in a state such that
 *                  mutex_lock() will fail when called with this argument.
 */
void threading_mutex_init_freertos( mbedtls_threading_mutex_t *mutex );


/**
 * \brief           An implementation of mutex free function based on
 *                  FreeRTOS.
 *
 * \param mutex    mutex handler.
 *
 * \note           Call mbedtls_threading_set_alt() to set it to the 
 *                  implementation of mutex_free().
 *
 * \note           It does not return a status code.
 */
void threading_mutex_free_freertos( mbedtls_threading_mutex_t *mutex );

/**
 * \brief           An implementation of mutex lock function based on
 *                  FreeRTOS.
 *
 * \param mutex    mutex handler.
 *
 * \note           Call mbedtls_threading_set_alt() to set it to the 
 *                  implementation of mutex_lock().
 *
 * \return          0 if successful,
 *                  or a specific MBEDTLS_ERR_XXX error code
 */
int threading_mutex_lock_freertos( mbedtls_threading_mutex_t *mutex );

/**
 * \brief           An implementation of mutex unlock function based on
 *                  FreeRTOS.
 *
 * \param mutex    mutex handler.
 *
 * \note           Call mbedtls_threading_set_alt() to set it to the 
 *                  implementation of mutex_unlock().
 *
 * \return          0 if successful,
 *                  or a specific MBEDTLS_ERR_XXX error code
 */
int threading_mutex_unlock_freertos( mbedtls_threading_mutex_t *mutex );


/**
 * \brief           Set your alternate threading implementation function
 *                  pointers and initialize global mutexes. If used, this
 *                  function must be called once in the main thread before any
 *                  other mbed TLS function is called, and
 *                  mbedtls_threading_free_alt_freertos() must be called once
 *                  in the main thread after all other mbed TLS functions.
 */
void mbedtls_threading_set_alt_freertos( void );


/**
 * \brief               Free global mutexes.
 */
void mbedtls_threading_free_alt_freertos( void );


#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_THREADING_ALT */

#endif /* MBEDTLS_THREADING_C */
#endif /* __THREADING_ALT_H__ */
 
