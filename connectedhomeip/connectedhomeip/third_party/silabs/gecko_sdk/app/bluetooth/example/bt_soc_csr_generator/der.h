/***************************************************************************//**
 * @file
 * @brief Certificate signing request structure generation.
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

#ifndef DER_H
#define DER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "psa/crypto.h"
#include "csr_generator.h"

/**
 * @brief Create a DER-encoded CSR
 *
 * Create a DER-encoded CSR for the given parameters.
 *
 * @param subject_name_ptr Array of Subject DN components
 * @param subject_name_len Length of the array of Subject DN components
 * @param key Device EC key; public key will be presented in the CSR and
 * the private key will be used in signing the CSR
 * @param use_static_auth Whether to include static authentication data
 * into CSR or not
 * @param buffer DER output buffer
 * @param buffer_len Length of the DER output buffer
 * @param write_len Number of bytes written to the DER output buffer (length of
 * constructed structure)
 *
 * @return SL_STATUS_OK if the CSR could be generated and encoded; an error otherwise
 */
sl_status_t der_encode_csr(const struct subject_name_field *subject_name_ptr,
                           size_t subject_name_len,
                           mbedtls_svc_key_id_t key,
                           bool use_static_auth,
                           uint8_t *buffer,
                           size_t buffer_len,
                           size_t *write_len);

#endif // DER_H
