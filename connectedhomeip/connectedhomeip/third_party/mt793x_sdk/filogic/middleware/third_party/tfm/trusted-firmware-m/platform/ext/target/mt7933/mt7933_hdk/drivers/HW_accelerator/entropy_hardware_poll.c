/*
 *  Hardware entropy collector for the K64F, using Freescale's RNGA
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
 * WARNING: this is temporary!
 * This should be in a separate yotta module which would be a target
 * dependency of mbedtls (see IOTSSL-313)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
#include <string.h>
#include "mbedtls/entropy_poll.h"
#include "hal_trng.h"


int mbedtls_hardware_poll( void *data,
        unsigned char *output, size_t len, size_t *olen )
{
    uint32_t random_seed;
    hal_trng_status_t ret = HAL_TRNG_STATUS_ERROR;

    ((void) data);

    if( len < sizeof(uint32_t) ) {
        return( -1 );
    }

    ret = hal_trng_init();
    if (HAL_TRNG_STATUS_OK != ret) {
        return( -2 );
    }

    ret = hal_trng_get_generated_random_number(&random_seed);
    hal_trng_deinit();
    if (HAL_TRNG_STATUS_OK != ret) {
        return( -3 );
    }

    memcpy( output, &random_seed, sizeof(uint32_t) );
    *olen = sizeof(uint32_t);

    return( 0 );
}
#endif /* MBEDTLS_ENTROPY_HARDWARE_ALT */

