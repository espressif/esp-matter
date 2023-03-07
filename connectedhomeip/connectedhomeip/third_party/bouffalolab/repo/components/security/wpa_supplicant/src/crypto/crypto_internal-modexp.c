/*
 * Crypto wrapper for internal crypto implementation - modexp
 * Copyright (c) 2006-2009, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */
/*
 * Hardware crypto support Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "crypto.h"
#include <blcrypto_suite/blcrypto_suite_supplicant_api.h>

int crypto_mod_exp(const uint8_t *base, size_t base_len,
	       const uint8_t *power, size_t power_len,
	       const uint8_t *modulus, size_t modulus_len,
	       uint8_t *result, size_t *result_len)
{
	struct crypto_bignum *bn_base = NULL, *bn_exp = NULL, *bn_modulus = NULL, *bn_result = NULL;
	int ret = 0;
    if ((bn_base = crypto_bignum_init_set(base, base_len)) == NULL ||
            (bn_exp = crypto_bignum_init_set(power, power_len)) == NULL ||
            (bn_modulus = crypto_bignum_init_set(modulus, modulus_len)) == NULL ||
            (bn_result = crypto_bignum_init()) == NULL) {
        ret = -1;
        goto clean;
    }

    if ((ret = crypto_bignum_exptmod(bn_base, bn_exp, bn_modulus, bn_result))) {
        goto clean;
    }

	ret = crypto_bignum_write_binary(bn_result, result, *result_len);

clean:
    crypto_bignum_deinit(bn_base, 0);
    crypto_bignum_deinit(bn_exp, 0);
    crypto_bignum_deinit(bn_modulus, 0);
    crypto_bignum_deinit(bn_result, 0);

	return ret;
}
