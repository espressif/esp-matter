/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes definitions for performing CCM of HW security engine
 */

#include <mbedtls/cipher.h>

#ifndef MBEDTLS_CCM_ALT_H
#define MBEDTLS_CCM_ALT_H

#define HW_AES_MODE_CTR 2
#define HW_AES_MODE_CBCMAC 3
#define HW_AES_MODE_CCM 4

#define HW_AES_ENC 1
#define HW_AES_DEC 0

#define HW_SECENG_POLLING 1
#define HW_SECENG_INTERRUPT 0

typedef struct mbedtls_ccm_context
{
    uint8_t key[MBEDTLS_MAX_KEY_LENGTH];
    uint8_t key_len;
    uint8_t decrypt_tag[16];
} mbedtls_ccm_context;

typedef enum
{
    HW_AES_FREE = 0,      // 0
    HW_AES_BUSY,          // 1
    HW_AES_PARAM_ERROR,   // 2
    HW_AES_PARAM_CORRECT, // 3
    HW_AES_MIC_ERROR,     // 4
    HW_AES_CFG_DONE,      // 5
    HW_AES_COMPLETE,      // 6
} tHW_AES_STATUS;

typedef struct _tHW_AES tHW_AES;
typedef void (*tHW_AES_CALLBACK)(tHW_AES *pst_hw_aes);

/**
 * \brief    The HW CCM context-type definition. The CCM context is passed
 *           to security engine.
 */
typedef struct _tHW_AES
{
    uint8_t polling_flag; /* Polling (HW_SECENG_POLLING) or Interrupt (HW_SECENG_INTERRUPT) */
    uint8_t enc_flag;     /* Encrypt (HW_AES_ENC) or Decrypt (HW_AES_DEC)     */
    uint8_t aes_mode;     /* 2: AES-CTR mode(only message encryption/decryption).     */
    /* 3: AES-CBC-MAC mode(only mic, no message encryption/decryption).                                */
    /* 4: AES-CCM mode.(mic + message encryption/decryption)                                           */
    uint8_t  nonce_len;   /* The length of nonce in bytes. 11-14.   */
    uint8_t  key_len;     /* The length of keys in byte: 16(128 bits), 24(192 bits), 32(256 bits)     */
    uint8_t  mic_len;     /* The length of mic  in byte: 0, 4(32 bits), 8(64 bits), 12(96 bits), 16(128 bits)     */
    uint16_t auth_len;    /* The length of authentication bits in bytes. 1 - 2047 bytes.    */
    uint16_t msg_len;     /* The length of message to be encrypted/decrypted in bytes. 0 - 2047 bytes.     */
    uint8_t *nonce_ptr;   /* The pointer points to nonce.   */
    uint8_t *key_ptr;     /* The pointer points to keys.     */
    uint8_t *in_auth_ptr; /* The pointer points to input auth data. */
    /* ENC: a data; DEC: a data.                                                                       */
    uint8_t *in_msg_ptr; /* The pointer points to input message to be encrypted/decrypted. */
    /*           AES-CTR               AES-CBC-MAC              AES-CCM                                */
    /* ENC:         m                      m                      m                                    */
    /* DEC:         c                    m + mic                c + mic                                */
    uint8_t *out_ptr; /* The pointer points to output data after encrypted/decrypted. */
    /*           AES-CTR               AES-CBC-MAC              AES-CCM                                */
    /* ENC:       a + c                a + m + mic             a + c + mic                             */
    /* DEC:       a + m                   a + m                  a + m                                 */
    uint8_t          hw_aes_result; /* The result of HW AES. FREE, BUSY, ERROR */
    tHW_AES_CALLBACK callback;      /* Call back function for HW AES if using interrupt.      */
    struct _tHW_AES *hw_aes_next;   /* The pointer points to the next data structure in the waithing line.   */
} tHW_AES;

/**
 * \brief           This function initializes the specified CCM context,
 *                  to make references valid, and prepare the context
 *                  for mbedtls_ccm_setkey() or mbedtls_ccm_free().
 *
 * \param hw_aes    The HW CCM context.
 */
extern uint8_t hw_aes_engine(tHW_AES *hw_aes);

/**
 * \brief           This function initializes the specified CCM context,
 *                  to make references valid, and prepare the context
 *                  for mbedtls_ccm_setkey() or mbedtls_ccm_free().
 *
 * \param ctx       The CCM context to initialize.
 */
void mbedtls_ccm_init(mbedtls_ccm_context *ctx);

/**
 * \brief           This function initializes the CCM context set in the
 *                  \p ctx parameter and sets the encryption key.
 *
 * \param ctx       The CCM context to initialize.
 * \param cipher    The 128-bit block cipher to use.
 * \param key       The encryption key.
 * \param keybits   The key size in bits. This must be acceptable by the cipher.
 *
 * \return          \c 0 on success.
 * \return          A CCM or cipher-specific error code on failure.
 */
int mbedtls_ccm_setkey(mbedtls_ccm_context *ctx,
                       mbedtls_cipher_id_t  cipher,
                       const unsigned char *key,
                       unsigned int         keybits);

/**
 * \brief   This function releases and clears the specified CCM context
 *          and underlying cipher sub-context.
 *
 * \param ctx       The CCM context to clear.
 */
void mbedtls_ccm_free(mbedtls_ccm_context *ctx);

/**
 * \brief           This function encrypts a buffer using CCM.
 *
 * \note            The tag is written to a separate buffer. To concatenate
 *                  the \p tag with the \p output, as done in <em>RFC-3610:
 *                  Counter with CBC-MAC (CCM)</em>, use
 *                  \p tag = \p output + \p length, and make sure that the
 *                  output buffer is at least \p length + \p tag_len wide.
 *
 * \param ctx       The CCM context to use for encryption.
 * \param length    The length of the input data in Bytes.
 * \param iv        Initialization vector (nonce).
 * \param iv_len    The length of the nonce in Bytes: 7, 8, 9, 10, 11, 12,
 *                  or 13. The length L of the message length field is
 *                  15 - \p iv_len.
 * \param add       The additional data field.
 * \param add_len   The length of additional data in Bytes.
 *                  Must be less than 2^16 - 2^8.
 * \param input     The buffer holding the input data.
 * \param output    The buffer holding the output data.
 *                  Must be at least \p length Bytes wide.
 * \param tag       The buffer holding the authentication field.
 * \param tag_len   The length of the authentication field to generate in Bytes:
 *                  4, 6, 8, 10, 12, 14 or 16.
 *
 * \return          \c 0 on success.
 * \return          A CCM or cipher-specific error code on failure.
 */
int mbedtls_ccm_encrypt_and_tag(mbedtls_ccm_context *ctx,
                                size_t               length,
                                const unsigned char *iv,
                                size_t               iv_len,
                                const unsigned char *add,
                                size_t               add_len,
                                const unsigned char *input,
                                unsigned char *      output,
                                unsigned char *      tag,
                                size_t               tag_len);

/**
 * \brief           This function encrypts a buffer using CCM*.
 *
 * \note            The tag is written to a separate buffer. To concatenate
 *                  the \p tag with the \p output, as done in <em>RFC-3610:
 *                  Counter with CBC-MAC (CCM)</em>, use
 *                  \p tag = \p output + \p length, and make sure that the
 *                  output buffer is at least \p length + \p tag_len wide.
 *
 * \note            When using this function in a variable tag length context,
 *                  the tag length has to be encoded into the \p iv passed to
 *                  this function.
 *
 * \param ctx       The CCM context to use for encryption.
 * \param length    The length of the input data in Bytes.
 * \param iv        Initialization vector (nonce).
 * \param iv_len    The length of the nonce in Bytes: 7, 8, 9, 10, 11, 12,
 *                  or 13. The length L of the message length field is
 *                  15 - \p iv_len.
 * \param add       The additional data field.
 * \param add_len   The length of additional data in Bytes.
 *                  Must be less than 2^16 - 2^8.
 * \param input     The buffer holding the input data.
 * \param output    The buffer holding the output data.
 *                  Must be at least \p length Bytes wide.
 * \param tag       The buffer holding the authentication field.
 * \param tag_len   The length of the authentication field to generate in Bytes:
 *                  0, 4, 6, 8, 10, 12, 14 or 16.
 *
 * \warning         Passing 0 as \p tag_len means that the message is no
 *                  longer authenticated.
 *
 * \return          \c 0 on success.
 * \return          A CCM or cipher-specific error code on failure.
 */
int mbedtls_ccm_star_encrypt_and_tag(mbedtls_ccm_context *ctx,
                                     size_t               length,
                                     const unsigned char *iv,
                                     size_t               iv_len,
                                     const unsigned char *add,
                                     size_t               add_len,
                                     const unsigned char *input,
                                     unsigned char *      output,
                                     unsigned char *      tag,
                                     size_t               tag_len);

/**
 * \brief           This function performs a CCM authenticated decryption of a
 *                  buffer.
 *
 * \param ctx       The CCM context to use for decryption.
 * \param length    The length of the input data in Bytes.
 * \param iv        Initialization vector (nonce).
 * \param iv_len    The length of the nonce in Bytes: 7, 8, 9, 10, 11, 12,
 *                  or 13. The length L of the message length field is
 *                  15 - \p iv_len.
 * \param add       The additional data field.
 * \param add_len   The length of additional data in Bytes.
 *                  Must be less than 2^16 - 2^8.
 * \param input     The buffer holding the input data.
 * \param output    The buffer holding the output data.
 *                  Must be at least \p length Bytes wide.
 * \param tag       The buffer holding the authentication field.
 * \param tag_len   The length of the authentication field in Bytes.
 *                  4, 6, 8, 10, 12, 14 or 16.
 *
 * \return          \c 0 on success. This indicates that the message is authentic.
 * \return          #MBEDTLS_ERR_CCM_AUTH_FAILED if the tag does not match.
 * \return          A cipher-specific error code on calculation failure.
 */
int mbedtls_ccm_auth_decrypt(mbedtls_ccm_context *ctx,
                             size_t               length,
                             const unsigned char *iv,
                             size_t               iv_len,
                             const unsigned char *add,
                             size_t               add_len,
                             const unsigned char *input,
                             unsigned char *      output,
                             const unsigned char *tag,
                             size_t               tag_len);

/**
 * \brief           This function performs a CCM* authenticated decryption of a
 *                  buffer.
 *
 * \note            When using this function in a variable tag length context,
 *                  the tag length has to be decoded from \p iv and passed to
 *                  this function as \p tag_len. (\p tag needs to be adjusted
 *                  accordingly.)
 *
 * \param ctx       The CCM context to use for decryption.
 * \param length    The length of the input data in Bytes.
 * \param iv        Initialization vector (nonce).
 * \param iv_len    The length of the nonce in Bytes: 7, 8, 9, 10, 11, 12,
 *                  or 13. The length L of the message length field is
 *                  15 - \p iv_len.
 * \param add       The additional data field.
 * \param add_len   The length of additional data in Bytes.
 *                  Must be less than 2^16 - 2^8.
 * \param input     The buffer holding the input data.
 * \param output    The buffer holding the output data.
 *                  Must be at least \p length Bytes wide.
 * \param tag       The buffer holding the authentication field.
 * \param tag_len   The length of the authentication field in Bytes.
 *                  0, 4, 6, 8, 10, 12, 14 or 16.
 *
 * \warning         Passing 0 as \p tag_len means that the message is no
 *                  longer authenticated.
 *
 * \return          \c 0 on success.
 * \return          #MBEDTLS_ERR_CCM_AUTH_FAILED if the tag does not match.
 * \return          A cipher-specific error code on calculation failure.
 */
int mbedtls_ccm_star_auth_decrypt(mbedtls_ccm_context *ctx,
                                  size_t               length,
                                  const unsigned char *iv,
                                  size_t               iv_len,
                                  const unsigned char *add,
                                  size_t               add_len,
                                  const unsigned char *input,
                                  unsigned char *      output,
                                  const unsigned char *tag,
                                  size_t               tag_len);

/**
 * \brief           In decrypt part, this function gets the tag calculated from input data
 *
 * \param ctx       The CCM context to use for decryption.
 * \param tag       The buffer returns calculated tag value
 * \param tag_len   The length of the authentication field in Bytes.
 *                  0, 4, 6, 8, 10, 12, 14 or 16.
 */
void mbedtls_ccm_get_decrypt_tag(mbedtls_ccm_context *ctx, const unsigned char *tag, size_t tag_len);

#endif /* MBEDTLS_CCM_ALT_H */
