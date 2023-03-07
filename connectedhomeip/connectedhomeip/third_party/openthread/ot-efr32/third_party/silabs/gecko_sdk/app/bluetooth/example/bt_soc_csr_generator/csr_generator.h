/***************************************************************************//**
 * @file
 * @brief SoC Certificate Generator
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef CSR_GENERATOR_H
#define CSR_GENERATOR_H

#include <stddef.h>
#include "psa/crypto.h"
#include "sl_status.h"

#define CRYPTO_EC_PUBLIC_KEY_LEN  64
#define CRYPTO_AUTH_256_LEN       32
#define CRYPTO_SHA_1_LEN          20

typedef struct subject_name_field {
  size_t name_len;
  size_t value_len;
  const char *name;
  char *value;
} subject_name_field_t;

// -----------------------------------------------------------------------------
// Public functions.

/**************************************************************************//**
 * This function generates the device EC key pair, the signing request
 * for the device certificate, and other related data.
 *****************************************************************************/
void csr_generate(void);

/**************************************************************************//**
 * This function exports the randomly generated authentication data
 * into the given buffer. Used when the CSR to be generated should
 * contain authentication data.
 *****************************************************************************/
sl_status_t export_static_auth_data(uint8_t *data,
                                    size_t data_size,
                                    size_t *data_length);

/**************************************************************************//**
 * This function exports the device public EC key into the given
 * buffer. Used when the CSR to be generated should contain
 * authentication data.
 *****************************************************************************/
sl_status_t export_public_key(mbedtls_svc_key_id_t key,
                              uint8_t *data,
                              size_t data_size,
                              size_t *data_length);

/**************************************************************************//**
 * This function calculates SHA-1 hash for given data
 *****************************************************************************/
sl_status_t calculate_sha_1(const uint8_t *ptr,
                            size_t len,
                            uint8_t *hash);

/**************************************************************************//**
 * This function converts a PSA API status code into the equivalent SL status
 *****************************************************************************/
sl_status_t psa_status_to_sl_status(psa_status_t p);

#endif // CSR_GENERATOR_H
