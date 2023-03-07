/***************************************************************************//**
 * @file ccm-star.h
 * @brief provides the api definition for aes-ccm* encryption
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef CCM_STAR_HEADER
#define CCM_STAR_HEADER

/**
 * @addtogroup security
 *
 * This file describes the functions for performing an AES-CCM* encryption on
 * a block of data.
 *
 * @{
 */

#define MESSAGE_LENGTH_FIELD_SIZE 2
#define NONCE_LENGTH 15 - MESSAGE_LENGTH_FIELD_SIZE

#ifdef USE_8_BYTE_MIC
  #define MIC_LENGTH 8
#else
  #define MIC_LENGTH 4
#endif

// A nonce contains the following fields:
#define STANDALONE_FLAGS_INDEX                   0
#define STANDALONE_NONCE_INDEX                   1
#define STANDALONE_VARIABLE_FIELD_INDEX_HIGH    14
#define STANDALONE_VARIABLE_FIELD_INDEX_LOW     15

/**
 * @brief  performs aes-ccm encryption and tagging, producing an encrypted
 *         message and a MIC
 * @param  nonce       the initial vector to the ccm encryption
 * @param  authData    additional authentication data used in the calculation of
 *                     the MIC, but does not get encrypted in the result
 * @param  authlen     length in bytes of the authentication data
 * @param  encryptData the data to be encrypted
 * @param  encryptlen  length in bytes of the data to be encrypted
 * @param  micResult   an array that will containing the resulting MIC
 * @return             EMBER_SUCCESS or other EmberStatus indicating a failure
 */
EmberStatus emberCcmEncryptAndTag(uint8_t *nonce,
                                  uint8_t *authData, uint16_t authlen,
                                  uint8_t *encryptData, uint16_t encryptlen,
                                  uint8_t *micResult);

/**
 * @brief  performs an authenticated decryption on an encrypted message,
 *         doing the decryption in place
 * @param  nonce       the initial vector for the ccm decryption
 * @param  authData    additional authentication data
 * @param  authlen     length in bytes of the authenticated data
 * @param  encryptData the encrypted data
 * @param  encryptlen  length in bytes of the encrypted data
 * @param  mic         the message integrity code to be checked against
 *                     for authentication
 * @return             EMBER_SUCCESS on successful authentication and
 *                     decryption, EMBER_AUTH_FAILURE if the calculated
 *                     MIC does not match what was passed
 */
EmberStatus emberCcmAuthDecrypt(uint8_t *nonce,
                                uint8_t *authData, uint16_t authlen,
                                uint8_t *encryptData, uint16_t encryptlen,
                                uint8_t *mic);

/**
 * @brief  calculates a message integrity code and performs ccm encryption
 *         on a flat packet.  Assumes the packet is formatted
 *         AUTH_DATA | ENCRYPT_DATA.  Performs encryption in place
 * @param  nonce      initial vector to the ccm encryption
 * @param  packet     flat packet containing the authData followed by the
 *                    data to be encrypted
 * @param  authlen    length in bytes of the authenticated data
 * @param  encryptlen length in bytes of the encrypted data
 * @param  micResult  the resulting MIC
 * @return            EMBER_SUCCESS or other EmberStatus indicating a failure
 *
 * @note   packet must be large enough to append the resulting MIC to
 *         the end of the buffer (len(packet) >= authlen + encryptlen + miclen)
 */
EmberStatus emberCcmEncryptAndTagPacket(uint8_t *nonce,
                                        uint8_t *packet, uint16_t authlen,
                                        uint16_t encryptlen);

/**
 * @brief  performs an authenticated decryption on a flat packet, assuming the
 *         format AUTH_DATA | ENCRYPT_DATA' | MIC, where ENCRYPT_DATA' is the
 *         message payload before decryption
 * @param  nonce       the initial vector to the ccm decryption
 * @param  packet      flat packet containing the authentication data, followed
 *                     by the data to be decrypted, followed by the MIC
 * @param  authlen     length in bytes of the authentication data
 * @param  encryptData pointer to the beginning of the encrypted data, will
 *                     store the resulting decrypted message
 * @param  encryptlen  length in bytes of the encrypted data
 * @return             EMBER_SUCCESS on successful authentication and
 *                     decryption, EMBER_AUTH_FAILURE if the calculated
 *                     MIC does not match what was passed
 */
EmberStatus emberCcmAuthDecryptPacket(uint8_t *nonce,
                                      uint8_t *packet, uint16_t authlen,
                                      uint16_t encryptlen);

/**
 * @brief  calculates the CCM* MIC of a given message without outputting
 *         the ciphertext.
 * @param  nonce       initial vector as input to the CCM* algorithm
 * @param  authData    additional authentication data
 * @param  authlen     length in bytes of authentication data
 * @param  payload     the message payload
 * @param  payloadlen  length of the payload in bytes
 * @param  micResult   the resulting MIC
 * @return             EMBER_SUCCESS or other EmberStatus indicating a failure
 */
void emberCcmCalculateAndEncryptMic(uint8_t *nonce,
                                    uint8_t *authData, uint16_t authlen,
                                    uint8_t *payload, uint16_t payloadlen,
                                    uint8_t *micResult);
/**
 * @brief  performs a CCM* encryption in place without tag (tag length zero).
 *         This operation basically reduces to AES-CTR with an IV constructed
 *         from @p nonce, and is thus symmetric for both encryption and
 *         decryption.
 *
 * @note   this function previously had an input parameter 'blockCount', which
 *         got removed since the function didn't adhere to its intent when
 *         setting blockCount to any other value then 1.
 * @param  bytes      bytes to be encrypted/decrypted
 * @param  length     length of the input/output
 * @param  nonce      nonce (of length NONCE_LENGTH) to use for the operation
 * @return            EMBER_SUCCESS or other EmberStatus indicating a failure
 */
void emberCcmEncryptBytes(uint8_t *bytes,
                          uint16_t length,
                          uint8_t* nonce);

// @} END addtogroup

// Alias legacy names for application compatibility
#define emberEncryptPayload emberCcmEncryptBytes

#endif // CCM_STAR_HEADER
