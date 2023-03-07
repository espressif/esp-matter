/**
 * @file
 * @brief Generate ECC key
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef ECC_KEYGEN_ALG_H_
#define ECC_KEYGEN_ALG_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_ecc_keygen_alg_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptodma.h"

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_ecc_keygen_alg.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_ecc_keygen_alg.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * validates an ECC public key
 * @param  domain  Domain of the curve to use
 * @param  pub block_t that will handle the generated priv key
 * @param  curve_flags Curve acceleration parameters
 * @param  size    size of the parameters to use
 * @return         return 1 in case of error, 0 otherwise
 */
uint32_t ecc_validate_key(block_t domain, block_t pub,uint32_t size, uint32_t curve_flags)CHECK_RESULT;
/**
 * Generates an ECC key (priv and pub)
 * @param  domain  Domain of the curve to use
 * @param  pub block_t that will handle the generated priv key
 * @param  priv block_t that will handle the generated pub key
 * @param  curve_flags Curve acceleration parameters
 * @param  size    size of the parameters to use
 * @return         return 1 in case of error, 0 otherwize
 */

 uint32_t ecc_genkey(block_t domain, block_t pub, block_t priv, uint32_t size, uint32_t curve_flags) CHECK_RESULT;

/**
 * Generates a pub key based on the curve and priv key given.
 * @param  curve   block_t to the curve parameters (domain)
 * @param  pub  block_t to the buffer for pub key
 * @param  priv block_t to the buffer for priv key
 * @param  size    size of each parameter in bytes
 * @param  curve_flags Curve acceleration parameters
 * @return         0 if no error, the error otherwize
 */

uint32_t ecc_generate_pub_key(block_t curve, block_t pub, block_t priv, uint32_t size, uint32_t curve_flags) CHECK_RESULT;

/**
 * @brief Generates a key pair for ECC montgomery curve \p curve
 * @param  domain   block_t to the curve parameters (domain)
 * @param  priv private key
 * @param  pub public key
 * @param  size    size of each parameter in bytes
 * @param  curve_flags Curve acceleration parameters
 * @return         0 if no error, the error otherwize
 */
uint32_t ecc_montgomery_genkey(block_t domain, block_t priv, block_t pub, uint32_t size, uint32_t curve_flags) CHECK_RESULT;

/**
 * Generates a pub key based on the curve and priv key given.
 * @param  curve   block_t to the curve parameters (domain)
 * @param  pk  block_t to the buffer for pub key
 * @param  sk block_t to the buffer for priv key
 * @param  size    size of each parameter in bytes
 * @param  curve_flags Curve acceleration parameters
 * @return         0 if no error, the error otherwize
 */

uint32_t ecc_montgomery_get_public_key_blk(block_t curve, block_t pk, block_t sk, uint32_t size, uint32_t curve_flags) CHECK_RESULT;

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
