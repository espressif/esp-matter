/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA431 (N)DRNG
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_RNG_H_
#define SX_RNG_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_rng_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include <stdbool.h>
#include "compiler_extentions.h"
#include "cryptolib_def.h"
#include "cryptodma.h"



#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_rng.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_rng.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
* @brief RNG initialization
* @param cond_test_en When high, conditioning test is executed first
* @param clk_div Clock Divider value, determines sample frequency
* @return CRYPTOLIB_SUCCESS or CRYPTOLIB_CRYPTO_ERR
*/
uint32_t sx_rng_init(uint32_t cond_test_en, uint32_t clk_div) CHECK_RESULT;

/**
* @brief Get a random number for a block_t structure
* @param dest  Block_t with pointer to result,
*              expected length, flags
*              (constant address or not)
*/
void sx_rng_get_rand_blk(block_t dest);

/**
* @brief Get a random number for a block_t structure
* @param dst Pointer to result
* @param size Number of bytes
*/
void sx_rng_get_rand(uint8_t * dst, uint32_t size);

/**
* @brief Returns random word
* @return 32-bit random value
*/
uint32_t sx_rng_get_word(void);

/**
* @brief Generate a random number k with 1 <= k < n.
* Implementation compliant with the FIPS 186-4 guideline.
* @param dst    Pointer to store generated random number k (should be buffer)
* @param n      Pointer to upper limit n (should be buffer)
* @return       CRYPTOLIB_SUCCESS or CRYPTOLIB_INVALID_PARAM
*/
uint32_t sx_rng_get_rand_lt_n_blk(block_t dst, block_t n) CHECK_RESULT;

#if defined(GP_DIVERSITY_JUMPTABLES)
uint32_t sx_rng_conditioning_test(void);
uint32_t sx_rng_wait_startup(void);
#endif // GP_DIVERSITY_JUMPTABLES

/**
* @brief Define \p new_rng_source as the source of random for ::sx_rng_get_rand  ::sx_rng_get_rand_blk functions.
* For test purpose only, functionnal only if ::CRYPTOLIB_TEST_ENABLED is set
* @param new_rng_source set random source
*/
void sx_rng_set_alternative_rng_source(block_t *new_rng_source);
/**
* @brief Restore default random source, to revert changes from ::sx_rng_set_alternative_rng_source.
*/
void sx_rng_restore_rng_source(void);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

/**
* @brief Indicate whether conditioning or startup test failed during init.
* Only valid during bootloader and diagnostic mode!
* @return boolean
*/
bool sx_rng_startup_failed(void);

/**
* @brief to store the fail condition during init.
* @param fail set if there was a failure during startup
*/
void sx_rng_set_startup_failed(bool fail);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif /* SX_RNG_H */
