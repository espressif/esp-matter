/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA411 AES function
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_AES_H_
#define SX_AES_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_aes_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptodma.h"
#include <stdbool.h>
#include "cryptolib_def.h"

#define AES_MAX_SIZE (256/8)

#if AES_HW_KEYS_ENABLED
/* Dummy variables to use hardware keys Key1 and Key2 */
extern uint8_t aes_hw_key2;
extern uint8_t aes_hw_key1;

/* Define blocks to access hardware keys */
#define AES_KEY1_128 block_t_convert(&aes_hw_key1, 128/8)
#define AES_KEY1_256 block_t_convert(&aes_hw_key1, 256/8)
#define AES_KEY2_128 block_t_convert(&aes_hw_key2, 128/8)
#define AES_KEY2_256 block_t_convert(&aes_hw_key2, 256/8)
#endif //AES_HW_KEYS_ENABLED

/**
* @brief Enumeration of possible mode of operation for AES algorithm.
*/
//TODO: replace with generic block cipher enum
typedef enum sx_aes_fct_e
{
    ECB  = 1,              /**< Electronic Codebook */
    CBC  = 2,              /**< Cipher Block Chaining */
    CTR  = 3,              /**< Counter Feedback */
    CFB  = 4,              /**< Cipher Feedback */
    OFB  = 5,              /**< Output Feedback */
    CCM  = 6,              /**< Counter with CBC-MAC Mode */
    GCM  = 7,              /**< Galois/Counter Mode */
    XTS  = 8,              /**< XEX-based tweaked-codebook mode with ciphertext stealing */
    CMAC  = 9,             /**< CMAC mode */
    CBCMAC = 10,           /**< CBC-MAC mode (only used by DES)*/
} sx_aes_fct_t;


/**
* @brief Enumeration of possible mode for AES algorithm.
*/
//TODO: replace with generic block cipher enum
typedef enum sx_aes_mode_e
{
    ENC = 1,            /**< Encrypt */
    DEC = 2             /**< Decrypt */
} sx_aes_mode_t;


/**
* @brief Enumeration of possible context states for AES algorithm.
*/
typedef enum sx_aes_ctx_e
{
    CTX_WHOLE = 0,            /**< No context switching (whole message) */
    CTX_BEGIN = 1,            /**< Save context (operation is not final) */
    CTX_END = 2,              /**< Load context (operation is not initial) */
    CTX_MIDDLE = 3            /**< Save & load context (operation is not initial & not final) */
} sx_aes_ctx_t;

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_aes.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_aes.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * @brief AES generic function
 *        Encrypt or decrypt \p input to \p output, using \p aes_fct mode of operation.
 *
 * @param aes_fct mode of operation for AES. See ::sx_aes_fct_t.
 * @param dir encrypt or decrypt. See ::sx_aes_mode_t.
 * @param ctx pointer to the AES context
 * @param data_in pointer to input data (plaintext or ciphertext).
 * @param data_out pointer to output data (ciphertext or plaintext).
 * @param key pointer to the AES key.
 * @param xtskey pointer to the XTS key.
 * @param iv pointer to initialization vector. Used for \p aes_fct ::CBC, ::CTR, ::CFB, ::OFB, ::XTS and ::GCM, mode of operation. 16 bytes will be read.
 * @param aad pointer to additional authenticated data. Used for \p aes_fct ::GCM mode of operation.
 * @param tag pointer to the authentication tag. Used for ::GCM mode of operation. 16 bytes will be written. in case of Encryption or Decryption
 * @param ctx_ptr pointer to the AES context (after operation)
 * @param nonce_len_blk pointer to the lenAlenC data (AES GCM context switching only) or the AES CCM Nonce
 *
 * @return 0 when execution was successful
 */
uint32_t sx_aes_blk( sx_aes_fct_t aes_fct, sx_aes_mode_t dir,sx_aes_ctx_t ctx,block_t key,block_t xtskey,block_t iv,block_t data_in,block_t data_out,block_t aad,block_t tag,block_t ctx_ptr, block_t nonce_len_blk) CHECK_RESULT;

/**
 * @brief Load LSFR with random value for AES masking
 * @param value value to be loaded in LFSR
 */
void sx_aes_load_mask(uint32_t value);

#if defined(GP_DIVERSITY_JUMPTABLES)
uint32_t sw_aes_setmode(sx_aes_fct_t aes_fct, sx_aes_mode_t dir, sx_aes_ctx_t ctx, block_t key, uint32_t *mode_out);
#if AES_CCM_ENABLED
uint32_t generate_ccm_header(block_t nonce, uint32_t aad_len, uint32_t data_len, uint32_t tag_len, block_t *header);
#endif
uint32_t get_aes_pad_len(uint32_t input_length);
uint32_t sx_aes_build_descr(  block_t *config, block_t *key, block_t *xtskey, block_t *iv, block_t *datain, block_t *dataout, block_t *aad1, block_t *aad2, block_t *tag_in, block_t *tag_out, block_t *ctx_ptr, block_t *lenAlenC_blk);
bool IsKeyLenValid(sx_aes_fct_t fct, size_t len, size_t xts_len);
bool IsIVContextLenValid(sx_aes_fct_t fct, sx_aes_ctx_t ctx, size_t len);
bool IsNextContextLenValid(sx_aes_fct_t fct, sx_aes_ctx_t ctx, size_t len);
bool IsNonceLenValid(sx_aes_fct_t fct, sx_aes_ctx_t ctx, size_t len);
bool IsPayloadLenValid(sx_aes_fct_t fct, sx_aes_ctx_t ctx, size_t len);
bool IsTagLenValid(sx_aes_fct_t fct, sx_aes_ctx_t ctx, size_t len);
uint32_t sx_aes_validate_input(sx_aes_mode_t dir, sx_aes_fct_t fct, sx_aes_ctx_t ctx, block_t key, block_t xtskey, block_t iv, block_t data_in, block_t aad, block_t tag, block_t ctx_ptr, block_t nonce_len_blk);
#endif // GP_DIVERSITY_JUMPTABLES

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
