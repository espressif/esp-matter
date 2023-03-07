/*
 *  Copyright (c) 2019, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes the initializers for supporting Security manager.
 *
 */

#ifndef SECURITY_MANAGER_H_
#define SECURITY_MANAGER_H_

#include <psa/crypto_types.h>
#include <stdbool.h>
#include <string.h>
#include "psa/crypto.h"
#include "psa/crypto_values.h"

typedef enum
{
    SL_SECURITY_MAN_SUCCESS,
    SL_SECURITY_MAN_CRYPTO_ERROR,
    SL_SECURITY_MAN_INVALID_PARAMS,
    SL_SECURITY_MAN_ERROR
} sl_sec_man_status_t;

/**
 * Initialise the security manager module.
 *
 * This API will internally initialise the PSA module
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Security Manager was successfully initialised.
 * @retval SL_SECURITY_MAN_ERROR                    There was some error during initialisation.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_init(void);

/**
 * Import a key into PSA ITS.
 *
 * The caller must Provide a valid key.
 *
 * Based on the parameters passed, the security manager will choose the storage location.
 * For persistent keys, key_id is managed by the application, and for volatiles keys,
 * PSA will create a key_id and returns the same to the application. All the keys will be wrapped
 * before storage and the key_id is further used for all the security operations.
 *
 * @param[out]  sl_psa_key_id                       Pointer to Key Id to be used for persistent keys. If the key is stored
 *                                                  in RAM, PSA will allocate the key ID and pass it to the application. For
 *                                                  non-volatile keys, key ID provided by application will be assigned as the reference.
 * @param[in]   sl_psa_key_type                     Key type encoding for the key.
 * @param[in]   sl_psa_key_algorithm                Key algorithm encoding for the key.
 * @param[in]   sl_psa_key_usage                    Key Usage encoding for the key.
 * @param[in]   sl_psa_key_persistence              What persistence level needs to be applied to the key.
 * @param[in]   sl_psa_key_literal                  Pointer to the actual key.
 * @param[in]   sl_key_literal_len                  Length of the key.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Successfully imported the key into PSA ITS.
 * @retval SL_SECURITY_MAN_ERROR                    Failed to inport key.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_import_key(psa_key_id_t *        sl_psa_key_id,
                                          psa_key_type_t        sl_psa_key_type,
                                          psa_algorithm_t       sl_psa_key_algorithm,
                                          psa_key_usage_t       sl_psa_key_usage,
                                          psa_key_persistence_t sl_psa_key_persistence,
                                          const uint8_t *       sl_psa_key_literal,
                                          size_t                sl_key_literal_len);

/**
 * Generates a key and stores the same in PSA.
 *
 * Based on the parameters passed, the security manager will choose the storage location.
 * For persistent keys, key_id is managed by the application, and for volatiles keys,
 * PSA will create a key_id and returns the same to the application. All the keys will be wrapped
 * before storage and the key_id is further used for all the security operations.
 *
 * @param[out]  sl_psa_key_id                       Pointer to Key Id to be used for persistent keys. If the key is stored
 *                                                  in RAM, PSA will allocate the key ID and pass it to the application. For
 *                                                  non-volatile keys, key ID provided by application will be assigned as the reference.
 * @param[in]   sl_psa_key_type                     Key type encoding for the key.
 * @param[in]   sl_psa_key_algorithm                Key algorithm encoding for the key.
 * @param[in]   sl_psa_key_usage                    Key Usage encoding for the key.
 * @param[in]   sl_psa_key_persistence              What persistence level needs to be applied to the key.
 * @param[in]   sl_psa_key_len                      Length of the key.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Successfully generated a key to match the parameters passed.
 * @retval SL_SECURITY_MAN_ERROR                    Failed to generate key.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_generate_key(psa_key_id_t *        sl_psa_key_id,
                                            psa_key_type_t        sl_psa_key_type,
                                            psa_algorithm_t       sl_psa_key_algorithm,
                                            psa_key_usage_t       sl_psa_key_usage,
                                            psa_key_persistence_t sl_psa_key_persistence,
                                            size_t                sl_psa_key_len);

/**
 * Export a key from PSA ITS.
 *
 * This API can be used to export the key stored in the PSA. Only keys marked as exportable can be
 * read back from the storage as plaintext.
 *
 * @param[in]   sl_psa_key_id                       Key ID used as a reference to the key. This is either passed to import key,
 *                                                  for persistent keys, or returned to the application in the key context, for
 *                                                  volatile keys.
 * @param[in]   sl_psa_key_buffer                   Pointer to the buffer to store the key.
 * @param[in]   sl_psa_key_buffer_len               Length of the buffer.
 * @param[out]  sl_psa_key_len                      Length of the exported key.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Successfully exported literal key referenced by @p sl_psa_key_id
 * @retval SL_SECURITY_MAN_ERROR                    Failed to export key.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_export_key(psa_key_id_t sl_psa_key_id,
                                          uint8_t *    sl_psa_key_buffer,
                                          size_t       sl_psa_key_buffer_len,
                                          size_t *     sl_psa_key_len);

/**
 * Get attributes for a key stored in PSA ITS.
 *
 * @param[in]   sl_psa_key_id                       Key ID used as a reference to the key.
 * @param[out]  sl_psa_key_attributes               output pointer for key attributes.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Key attributes of @p sl_psa_key_id was successfully retrieved .
 * @retval SL_SECURITY_MAN_ERROR                    Failed to get key attributes of @p sl_psa_key_id.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_get_key_attributes(psa_key_id_t          sl_psa_key_id,
                                                  psa_key_attributes_t *sl_psa_key_attributes);

/**
 * Destroy a key stored in PSA ITS.
 *
 * This API can be used to dispose a stored key from PSA.
 *
 * @param[in]   sl_psa_key_id                       Key ID used as a reference to the key.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Key referenced by @p sl_psa_key_id was successfully destroyed.
 * @retval SL_SECURITY_MAN_ERROR                    Failed to destroy key @p sl_psa_key_id.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_destroy_key(psa_key_id_t sl_psa_key_id);

/**
 * API to encrypt data (AES ECB encryption).
 *
 * This API can be used to perform AES ECB operation on given data. The user will have to pass a valid
 * Key reference in the form of a key_id to the API.
 *
 * @param[in]  sl_psa_key_id                        Key ID used as a reference to the key.
 * @param[in]  sl_psa_aes_alg                       AES Algorithm to use.
 * @param[in]  sl_psa_aes_input                     Pointer to the data to be encrypted.
 * @param[out] sl_psa_aes_output                    Pointer to buffer to hold the encrypted data.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  AES encryption was completed successfully.
 * @retval SL_SECURITY_MAN_ERROR                    AES encryption failed to complete.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_aes_encrypt(psa_key_id_t    sl_psa_key_id,
                                           psa_algorithm_t sl_psa_aes_alg,
                                           const uint8_t * sl_psa_aes_input,
                                           uint8_t *       sl_psa_aes_output);

/**
 * Start the HMAC operation.
 *
 * @param[in]   sl_psa_hmac_ctx                     Operation Context for HMAC operation.
 * @param[out]  sl_psa_key_id                       Reference to the key to be used for HMAC operation.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  HMAC operation started successfully.
 * @retval SL_SECURITY_MAN_ERROR                    HMAC operation failed to start.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_hmac_start(psa_mac_operation_t *sl_psa_hmac_ctx, psa_key_id_t sl_psa_key_id);

/**
 * Update the HMAC operation with new input.
 *
 * @param[in]  sl_psa_hmac_ctx                      Operation Context for HMAC operation.
 * @param[out] sl_psa_hmac_buffer                   A pointer to the input buffer.
 * @param[in]  sl_psa_hmac_buffer_len               The length of @p sl_psa_hmac_buffer in bytes.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  New input added successfully to HMAC operation.
 * @retval SL_SECURITY_MAN_ERROR                    New input update failed.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_hmac_update(psa_mac_operation_t *sl_psa_hmac_ctx,
                                           const uint8_t *      sl_psa_hmac_buffer,
                                           size_t               sl_psa_hmac_buffer_len);

/**
 * Complete the HMAC operation.
 *
 * @param[in]  sl_psa_hmac_ctx                      Operation Context for HMAC operation.
 * @param[out] sl_psa_hmac_buffer                   A pointer to the output buffer.
 * @param[in]  sl_psa_hmac_buffer_len               The length of @p sl_psa_hmac_buffer in bytes.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  HMAC operation completed successfully.
 * @retval SL_SECURITY_MAN_ERROR                    HMAC operation failed.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_hmac_finish(psa_mac_operation_t *sl_psa_hmac_ctx,
                                           const uint8_t *      sl_psa_hmac_buffer,
                                           size_t               sl_psa_hmac_buffer_len);

/**
 * Uninitialize the HMAC operation.
 *
 * @param[in]   sl_psa_hmac_ctx                     Operation Context for HMAC operation.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  De-initialised the HMAC successfully.
 * @retval SL_SECURITY_MAN_ERROR                    HMAC De-initialisation failed.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_hmac_deinit(psa_mac_operation_t *sl_psa_hmac_ctx);

/**
 * Perform HKDF Extract step.
 *
 * @param[in]  sl_psa_key_derivation_ctx            Operation context for key derivation operation.
 * @param[in]  sl_psa_key_derivation_algorithm      Algorithm being used for key derivation.
 * @param[in]  sl_psa_key_id                        Reference to key stored in PSA ITS.
 * @param[in]  sl_psa_key_derivation_salt           Pointer to the Salt for key derivation.
 * @param[in]  sl_psa_key_derivation_salt_length    Length of Salt.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Successfully performed HKDF extraction.
 * @retval SL_SECURITY_MAN_ERROR                    HKDF extraction failed.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_key_derivation_extract(psa_key_derivation_operation_t *sl_psa_key_derivation_ctx,
                                                      psa_algorithm_t                 sl_psa_key_derivation_algorithm,
                                                      psa_key_id_t                    sl_psa_key_id,
                                                      const uint8_t *                 sl_psa_key_derivation_salt,
                                                      uint16_t sl_psa_key_derivation_salt_length);

/**
 * Perform HKDF Expand step.
 *
 * @param[in]  sl_psa_key_derivation_ctx            Operation context for HKDF operation.
 * @param[in]  sl_psa_key_derivation_info           Pointer to the Info sequence.
 * @param[in]  sl_psa_key_derivation_info_length    Length of the Info sequence.
 * @param[out] sl_psa_key_derivation_output_key     Pointer to the output Key.
 * @param[in]  sl_psa_key_derivation_output_key_len Size of the output key buffer.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Successfully performed HKDF expansion.
 * @retval SL_SECURITY_MAN_ERROR                    HKDF expansion failed.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 *
 */
sl_sec_man_status_t sl_sec_man_key_derivation_expand(psa_key_derivation_operation_t *sl_psa_key_derivation_ctx,
                                                     const uint8_t *                 sl_psa_key_derivation_info,
                                                     uint16_t                        sl_psa_key_derivation_info_length,
                                                     uint8_t *                       sl_psa_key_derivation_output_key,
                                                     uint16_t sl_psa_key_derivation_output_key_len);

/**
 * Initialise the hashing operation.
 *
 * @param[in]  sl_psa_hash_ctx                      Context for hashing operation.
 *
 * @retval                                          Initial value for a hash operation object.
 */
psa_hash_operation_t sl_sec_man_hash_init(void);

/**
 * De-Initialise the hashing operation.
 *
 * @param[in]  sl_psa_hash_ctx                      Context for hashing operation.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Hashing operation was de-initialised successfully.
 * @retval SL_SECURITY_MAN_ERROR                    There was some error during de-initialisation.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 */
sl_sec_man_status_t sl_sec_man_hash_deinit(psa_hash_operation_t *sl_psa_hash_ctx);

/**
 * Start SHA-256 operation.
 *
 * @param[in]  sl_psa_hash_ctx                      Context for hashing operation.
 * @param[in]  sl_psa_hash_alg                      Hashing algorithm to use.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  SHA-256 Hashing was started successfully.
 * @retval SL_SECURITY_MAN_ERROR                    There was some error starting SHA-256 operation.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 */
sl_sec_man_status_t sl_sec_man_hash_start(psa_hash_operation_t *sl_psa_hash_ctx, psa_algorithm_t sl_psa_hash_alg);

/**
 * Update hashing operation with new input.
 *
 * @param[in]  sl_psa_hash_ctx                      Context for hashing operation.
 * @param[out] sl_psa_hash_buffer                   A pointer to the input buffer.
 * @param[in]  sl_psa_hash_buffer_len               The length of @p sl_psa_hash_buffer in bytes.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  New input was succesfully updated to the hash.
 * @retval SL_SECURITY_MAN_ERROR                    There was some error during updating new input.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 */
sl_sec_man_status_t sl_sec_man_hash_update(psa_hash_operation_t *sl_psa_hash_ctx,
                                           uint8_t *             sl_psa_hash_buffer,
                                           uint16_t              sl_psa_hash_buffer_len);

/**
 * Finish hashing operation.
 *
 * @param[in]  sl_psa_hash_ctx                      Context for hashing operation.
 * @param[in]  sl_psa_hash_hash                     A pointer to the output buffer, where hash needs to be stored.
 * @param[in]  sl_psa_hash_hash_size                The length of @p sl_psa_hash_hash in bytes.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Hashing operation was completed successfully.
 * @retval SL_SECURITY_MAN_ERROR                    There was some error during hashing.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 */
sl_sec_man_status_t sl_sec_man_hash_finish(psa_hash_operation_t *sl_psa_hash_ctx,
                                           uint8_t *             sl_psa_hash,
                                           uint16_t              sl_psa_hash_size,
                                           size_t *              sl_psa_hash_len);

/**
 * Generate Entropy.
 *
 * @param[in]  sl_psa_output_buffer                 Buffer to store the generated entropy.
 * @param[in]  sl_psa_output_size                   Amount of entropy needed in bytes.
 *
 * @retval SL_SECURITY_MAN_SUCCESS                  Requested entropy was successfully generated.
 * @retval SL_SECURITY_MAN_ERROR                    There was some error during entropy generation.
 * @retval SL_SECURITY_MAN_INVALID_PARAMS           There was an error in the params passed.
 */
sl_sec_man_status_t sl_sec_man_get_random( uint8_t               *sl_psa_output_buffer, 
                                           uint16_t              sl_psa_output_size);

#endif /* SECURITY_MANAGER_H_ */
