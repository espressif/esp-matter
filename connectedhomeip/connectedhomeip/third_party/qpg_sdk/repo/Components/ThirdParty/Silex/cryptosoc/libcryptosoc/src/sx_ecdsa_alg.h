/**
 * @file
 * @brief Defines the procedures to make ECDSA operations with
 *          the BA414E pub key
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_ECDSA_ALG_H_
#define SX_ECDSA_ALG_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_ecdsa_alg_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "cryptodma.h"
#include "sx_ecc_curves.h"
#include "sx_hash.h"

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_ecdsa_alg.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_ecdsa_alg.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * @brief Validate Curve parameters when given by the host \p curve
 * @param curve the ECC curve
* @return ::CRYPTOLIB_SUCCESS if successful
          ::CRYPTOLIB_CRYPTO_ERR if there is an issue with the parameters

 */
uint32_t ecdsa_domain_parameter_validation (const sx_ecc_curve_t *curve) ;

/**
 * @brief Verify ECDSA signature from a \p formatted_digest
 * @param curve the ECC curve
 * @param formatted_digest digest to verify against \p signature. Expected to be formatted as required by ECDSA specification.
 * @param key ECC public key
 * @param signature verify that
* @return ::CRYPTOLIB_SUCCESS if successful
 */
uint32_t ecdsa_signature_verification_digest(const sx_ecc_curve_t *curve, block_t formatted_digest, block_t key, block_t signature);

/**
 * @brief Configure hardware for signature generation.
 * @details Load the curve parameters, the key and formatted_digest. Those are
 * needed once before attempting to generate signatures.
 * @param curve the ECC curve
 * @param formatted_digest digest to generate the signature. Formatted as required by ECDSA specification.
 * @param key ECC private key
 * @return ::CRYPTOLIB_SUCCESS if successful
 */
uint32_t sx_ecdsa_signature_configure(const sx_ecc_curve_t *curve, block_t formatted_digest, block_t key);

/**
* @brief Verify ECDSA signature from a \p message
* @param curve selects the ECC curve
* @param message data to hash and verigy agains \p signature
* @param key ECC public key
* @param signature to verify
* @param hash_fct select hash algorithm to use on \p message
* @return ::CRYPTOLIB_SUCCESS if successful
*/
uint32_t ecdsa_signature_verification(const sx_ecc_curve_t *curve, block_t message, block_t key, block_t signature, sx_hash_fct_t hash_fct);


/**
* @brief Generate ECDSA signature from a \p formatted_digest
* @param curve selects the ECC curve
* @param formatted_digest digest to use to generate \p signature. Expected to be formatted as required by ECDSA specification.
* @param key ECC private key
* @param signature location to store the signature
* @return ::CRYPTOLIB_SUCCESS if successful
*/
uint32_t ecdsa_signature_generation_digest(const sx_ecc_curve_t *curve, block_t formatted_digest, block_t key, block_t signature);

/**
* @brief Generate ECDSA signature from a \p message
* @param curve selects the ECC curve
* @param message data to hash and sign
* @param key ECC private key
* @param signature location to store the signature
* @param hash_fct select hash algorithm to use on \p message
* @return ::CRYPTOLIB_SUCCESS if successful
*/
uint32_t ecdsa_signature_generation(const sx_ecc_curve_t *curve, block_t message, block_t key, block_t signature, sx_hash_fct_t hash_fct);

#if defined(GP_DIVERSITY_JUMPTABLES)
void sx_bitshift(uint8_t *array, uint8_t len, uint8_t shift);

uint32_t ecdsa_operation(const sx_ecc_curve_t *curve, block_t message, block_t key, block_t signature, sx_hash_fct_t hash_fct, uint8_t verify );
#endif // GP_DIVERSITY_JUMPTABLES

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
