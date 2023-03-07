/**
 * @file
 * @brief Defines the procedures to make DH operations with
 *          the BA414E pub key
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_DH_ALG_H_
#define SX_DH_ALG_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_dh_alg_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptolib_def.h"
#include "cryptodma.h"

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_dh_alg.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_dh_alg.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

#if (DH_MODP_ENABLED)

/**
 * computes common key based on pub and priv key
 * @param  mod  block_t to the modulus of the domain
 * @param  priv block_t to the priv key
 * @param  pub  block_t to the pub key
 * @param  common  block_t to the common key (output)
 * @param  size    size in bytes of the parameters
 * @return         returns 0 if no error
 */
uint32_t dh_common_key_modp(block_t mod, block_t priv, block_t pub, block_t common, uint32_t size) CHECK_RESULT;

#endif


/**
 * computes common key based on pub and priv key for ECDH
 * @param  domain  block_t to the modulus of the domain
 * @param  priv block_t to the priv key
 * @param  pub  block_t to the pub key
 * @param  common  block_t to the common key (output)
 * @param  size    size in bytes of the parameters
 * @param  curve_flags Curve acceleration parameters
 * @return         returns 0 if no error
 */
uint32_t dh_common_key_ecdh(block_t domain, block_t priv, block_t pub, block_t common, uint32_t size, uint32_t curve_flags)  CHECK_RESULT;

/** @brief Compute ECDH common key based on pub and priv key, and return only the X coordinate of the common key.
 *  @param  domain  block_t to the modulus of the domain
 *  @param  priv block_t to the priv key
 *  @param  pub  block_t to the pub key
 *  @param  common  block_t to the common key (output); size should be equal to curve size
 *  @param  size    size in bytes of the parameters
 *  @param  curve_flags Curve acceleration parameters
 *  @return         returns 0 if no error
 */
uint32_t dh_common_key_ecdh_short(block_t domain, block_t priv, block_t pub, block_t common, uint32_t size, uint32_t curve_flags)  CHECK_RESULT;

/**
* computes common key based on pub and priv key for ECDH using montgomery curve
 * @param  curve  block_t to the modulus of the domain
 * @param  priv block_t to the priv key
 * @param  pub  block_t to the pub key
 * @param  common  block_t to the common key (output)
 * @param  size    size in bytes of the parameters
 * @param  curve_flags Curve acceleration parameters
 * @return         returns 0 if no error
 */
uint32_t dh_montgomery_key_ecdh(block_t curve, block_t priv, block_t pub, block_t common, uint32_t size, uint32_t curve_flags);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
