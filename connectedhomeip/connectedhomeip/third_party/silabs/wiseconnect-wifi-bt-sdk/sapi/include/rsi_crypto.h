/*******************************************************************************
* @file  rsi_crypto.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#ifndef RSI_SHA_H
#define RSI_SHA_H
/******************************************************
 * *                      Macros
 * ******************************************************/

#define FIRST_CHUNK  BIT(0)
#define MIDDLE_CHUNK BIT(1)
#define LAST_CHUNK   BIT(2)

#define SHA_1   1
#define SHA_256 2
#define SHA_384 3
#define SHA_512 4

#define SHAKE_128 21
#define SHAKE_256 17
#define SHA3_224  18
#define SHA3_256  17
#define SHA3_384  13
#define SHA3_512  9

#define AES        2
#define SHA        4
#define HMAC_SHA   5
#define DH         14
#define ECDH       15
#define SHA3       16
#define CHACHAPOLY 17
#define GCM        18
#define SHAKE      19

#define ECDH_PM  1
#define ECDH_PA  2
#define ECDH_PS  3
#define ECDH_PD  4
#define ECDH_PAF 5

#define SHA_1_DIGEST_LEN     20
#define SHA_256_DIGEST_LEN   32
#define SHA_384_DIGEST_LEN   48
#define SHA_512_DIGEST_LEN   64
#define SHA_224_DIGEST_LEN   28
#define SHAKE_128_DIGEST_LEN 16

#define HMAC_SHA_1    1
#define HMAC_SHA_256  2
#define HMAC_SHAA_384 3
#define HMAC_SHA_512  4
#define IV_SIZE       16

#define ECDH_VECTOR_192_SIZE 24
#define ECDH_VECTOR_224_SIZE 28
#define ECDH_VECTOR_256_SIZE 32

#define ECDH_MAX_OUTPUT_SIZE 96
#define ECDH_MAX_VECTOR_SIZE 32
#define DH_MAX_OUTPUT_SIZE   512

#define HMAC_SHA_1_DIGEST_LEN   20
#define HMAC_SHA_256_DIGEST_LEN 32
#define HMAC_SHA_384_DIGEST_LEN 48
#define HMAC_SHA_512_DIGEST_LEN 64

#define CBC_MODE 1
#define ECB_MODE 2
#define CTR_MODE 3

#define AES_ENCRYPTION 1
#define AES_DECRYPTION 2

#define GCM_ENCRYPTION 0
#define GCM_DECRYPTION 1

#define AES_KEY_SIZE_128 16
#define AES_KEY_SIZE_192 24
#define AES_KEY_SIZE_256 32

#define GCM_KEY_SIZE_128     128
#define GCM_KEY_SIZE_192     192
#define GCM_KEY_SIZE_256     256
#define GCM_IV_SIZE_IN_BYTES 12
#define ECDH_192             1
#define ECDH_224             2
#define ECDH_256             4

//! ECDH curve types
#define ECDH_CURVE_P 0
#define ECDH_CURVE_K 1
#define ECDH_CURVE_B 2

//!DMA Support
#define DMA_ENABLE  1
#define DMA_DISABLE 0

//! PAD Char
#define PAD_CHAR_SHA3                      0x06
#define PAD_CHAR_SHAKE                     0x1F
#define CHACHA20POLY1305                   0
#define CHACHA20                           1
#define CHACHAPOLY_MODE_POLY1305_KEYR_KEYS 2
#define POLY1305_MODE                      3

#define TAG_SIZE 16

#define CHACHAPOLY_ENCRYPTION 0
#define CHACHAPOLY_DECRYPTION 1

#define KEY_CHACHA_SIZE                    32
#define KEYR_SIZE                          16
#define KEYS_SIZE                          16
#define NONCE_SIZE                         16
#define MAX_DATA_SIZE_BYTES                1400 /*Data size*/
#define MAX_DATA_SIZE_BYTES_FOR_CHACHAPOLY 1200 /*Data size for chachapoly*/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// SHA Request Frames Structures

typedef struct rsi_sha_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t sha_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint8_t msg[1400];
} rsi_sha_req_t;

typedef struct rsi_chachapoly_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t chachapoly_flags;
  uint16_t total_msg_length;
  uint16_t header_length;
  uint16_t current_chunk_length;
  uint16_t encrypt_decryption;
  uint32_t dma_use;
  uint8_t key_chacha[32];
  uint8_t nonce[16];
  uint8_t keyr_in[16];
  uint8_t keys_in[16];
  uint8_t header_input[128];
  uint8_t msg[1200];
} rsi_chachapoly_req_t;
typedef struct rsi_hmac_sha_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t hmac_sha_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint32_t key_length;
  uint8_t hmac_data[1400];
} rsi_hmac_sha_req_t;

typedef struct rsi_aes_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t aes_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint16_t encrypt_decryption;
  uint16_t key_length;
  uint8_t key[32];
  uint8_t IV[16];
  uint8_t msg[1400];
} rsi_aes_req_t;

typedef struct rsi_gcm_req_s {
  uint16_t algorithm_type;
  uint8_t gcm_flags;
  uint8_t encrypt_decryption;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint16_t key_length;
  uint16_t header_length;
  uint32_t dma_use;
  uint8_t key[32];
  uint8_t IV[12];
  uint8_t header[128];
  uint8_t msg[1400];
} rsi_gcm_req_t;
typedef struct rsi_exp_req_s {
  uint32_t algorithm_type;
  uint32_t prime_length;
  uint32_t base_length;
  uint32_t exponent_length;
  uint8_t prime[512];
  uint8_t base[512];
  uint8_t exponent[512];
} rsi_exp_req_t;

typedef struct rsi_ecdh_pm_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t ecdh_curve_type;
  uint8_t d[32];
  uint8_t sx[32];
  uint8_t sy[32];
  uint8_t sz[32];
} rsi_ecdh_pm_req_t;

typedef struct rsi_ecdh_pa_ps_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t sx[32];
  uint8_t sy[32];
  uint8_t sz[32];
  uint8_t tx[32];
  uint8_t ty[32];
  uint8_t tz[32];
} rsi_ecdh_pa_ps_req_t;

typedef struct rsi_ecdh_pd_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t sx[32];
  uint8_t sy[32];
  uint8_t sz[32];
} rsi_ecdh_pd_req_t;

typedef struct rsi_ecdh_affine_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t ecdh_curve_type;
  uint8_t sx[32];
  uint8_t sy[32];
  uint8_t sz[32];
} rsi_ecdh_affine_req_t;
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
int32_t rsi_sha_pen(uint8_t sha_mode,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_len,
                    uint8_t pending_flag,
                    uint8_t *digest);
int32_t rsi_hmac_sha(uint8_t hmac_sha_mode,
                     uint8_t *msg,
                     uint32_t msg_length,
                     uint8_t *key,
                     uint32_t key_length,
                     uint8_t *digest,
                     uint8_t *hmac_buffer);
int32_t rsi_hmac_sha_pen(uint8_t hmac_sha_mode,
                         uint8_t *data,
                         uint16_t total_length,
                         uint16_t chunk_length,
                         uint32_t key_length,
                         uint8_t pending_flag,
                         uint8_t *digest);
int32_t rsi_aes_pen(uint16_t aes_mode,
                    uint16_t enc_dec,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    uint8_t *key,
                    uint16_t key_length,
                    uint8_t *iv,
                    uint8_t aes_flags,
                    uint8_t *output);
int32_t rsi_aes(uint16_t aes_mode,
                uint16_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint8_t *output);
int32_t rsi_exponentiation(uint8_t *prime,
                           uint32_t prime_length,
                           uint8_t *base,
                           uint32_t base_length,
                           uint8_t *exponent,
                           uint32_t exponent_length,
                           uint8_t *exp_result);
int32_t rsi_sha3(uint8_t mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
int32_t rsi_shake(uint8_t mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
int32_t rsi_sha3_shake_pen(uint8_t pad_char,
                           uint8_t mode,
                           uint8_t *msg,
                           uint16_t msg_length,
                           uint16_t chunk_len,
                           uint8_t pending_flag,
                           uint8_t *digest);
int32_t rsi_chachapoly(uint16_t chachapoly_mode,
                       uint16_t enc_dec,
                       uint16_t dma_use,
                       uint8_t *msg,
                       uint16_t msg_length,
                       uint8_t *key_chacha,
                       uint8_t *keyr_in,
                       uint8_t *keys_in,
                       uint8_t *nonce,
                       uint8_t *header_input,
                       uint16_t header_length,
                       uint8_t *output);
int32_t rsi_chachapoly_pen(uint16_t chachapoly_mode,
                           uint16_t enc_dec,
                           uint16_t dma_use,
                           uint8_t *msg,
                           uint16_t msg_length,
                           uint16_t chunk_len,
                           uint8_t *key_chacha,
                           uint8_t *keyr_in,
                           uint8_t *keys_in,
                           uint8_t *nonce,
                           uint8_t *header_input,
                           uint16_t header_length,
                           uint8_t chachapoly_flags,
                           uint8_t *output);
int32_t rsi_gcm_pen(uint8_t enc_dec,
                    uint16_t dma_use,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    uint8_t *key,
                    uint16_t key_length,
                    uint8_t *iv,
                    uint16_t iv_sz,
                    uint8_t *header,
                    uint16_t header_length,
                    uint8_t gcm_flags,
                    uint8_t *output);
int32_t rsi_gcm(uint8_t enc_dec,
                uint16_t dma_use,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint16_t iv_sz,
                uint8_t *header,
                uint16_t header_length,
                uint8_t *output);

#endif
