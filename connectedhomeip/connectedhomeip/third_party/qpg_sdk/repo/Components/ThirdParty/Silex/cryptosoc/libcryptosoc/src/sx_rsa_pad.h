/**
 * @file
 * @brief Handles RSA padding
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_RSA_PAD_H
#define SX_RSA_PAD_H

#include <stdint.h>
#include <stddef.h>
#include "compiler_extentions.h"
#include "sx_hash.h"

/**
 * Those functions are implemented using the reference document from RSA Labs:
 * (see document PKCS #1 v2.2: RSA Cryptography Standard from RSA Laboratories, sections 7, 8 and 9)
 */

/**
* @brief Enumeration of possible padding types
*/
typedef enum rsa_pad_types_e {
   ESEC_RSA_PADDING_NONE      = 0x0,
   ESEC_RSA_PADDING_OAEP      = 0x1,
   ESEC_RSA_PADDING_EME_PKCS  = 0x2,
   ESEC_RSA_PADDING_EMSA_PKCS = 0x3,
   ESEC_RSA_PADDING_PSS       = 0x4
} rsa_pad_types_t;


/**
 * Encode message of size mLen to ME of size k using algorithm defined by type
 * @param  type     RSA_pad type defines the type of padding to apply
 * @param  k        RSA param length in bytes
 * @param  EM       Pointer to the Encoded message buffer
 * @param  hashType when needed, the hash function has to be defined
 * @param  message  Pointer to the message to encode
 * @param  mLen     Len of the message to be encoded (bytes)
 * @return          0 if success
 */
uint32_t rsa_pad_encode(rsa_pad_types_t type, uint32_t k, uint8_t *EM, sx_hash_fct_t hashType, uint8_t *message, size_t mLen) CHECK_RESULT;

/**
 * Decode the EM message using algorithm defined by type and put the message in message.
 * In case of signature verification, as PSS uses hash functions, the function
 * return 0 when the message matches the encoded message.
 * @param  type     RSA_pad type defines the type of padding to apply
 * @param  k        RSA param length in bytes
 * @param  EM       Pointer to the Encoded message buffer
 * @param  hashType when needed, the hash function has to be defined
 * @param  message  Pointer to the output buffer (decoded message) or, for signature verification, the hash to compare
 * @param  mLen     Len of the message decoded (bytes)
 * @return          Returns 0 if the message is valid (decodable) or the signature matches (only for PSS)
 */
uint32_t rsa_pad_decode(rsa_pad_types_t type, uint32_t k, uint8_t *EM, sx_hash_fct_t hashType, uint8_t **message, size_t *mLen) CHECK_RESULT;

/**
 * Encode message of length mLen to the EM buffer using OAEP
 * @param  k        RSA param length in bytes
 * @param  hashType the hash function to use (default is SHA1)
 * @param  EM       Pointer to the Encoded message buffer
 * @param  message  Pointer to the message to encode
 * @param  mLen     Len of the message to be encoded (bytes)
 * @return          0 whenno error occurs
 */
uint32_t rsa_pad_eme_oaep_encode(uint32_t k, sx_hash_fct_t hashType, uint8_t *EM, uint8_t *message, size_t mLen) CHECK_RESULT;

/**
 * Decode EM message using hash function hashType and outputs encoded message to message. mLen is the length of the decoded message
 * @param  k        RSA param length in bytes
 * @param  hashType the hash function to use (default is SHA1)
 * @param  EM       Pointer to the Encoded message buffer
 * @param  message  Pointer to the decoded message buffer
 * @param  mLen     Length of the decoded message
 * @return          0 whenno error occurs
 */
uint32_t rsa_pad_eme_oaep_decode(uint32_t k, sx_hash_fct_t hashType, uint8_t *EM, uint8_t **message, size_t *mLen) CHECK_RESULT;

/**
 * Encode message of length mLen to the EM buffer using PKCS
 * @param  k        RSA param length in bytes
 * @param  EM       Pointer to the Encoded message buffer
 * @param  message  Pointer to the message to encode
 * @param  mLen     Len of the message to be encoded (bytes)
 * @return          0 when no error occurs
 */
uint32_t rsa_pad_eme_pkcs_encode(uint32_t k, uint8_t *EM, uint8_t *message, size_t mLen) CHECK_RESULT;

/**
 * Decodes encoded message using PKCS. message will point to the decoded message in EM
 * @param  k        RSA param length in bytes
 * @param  EM       Pointer to the Encoded message buffer
 * @param  message  Pointer to the decoded message in EM
 * @param  mLen     Len of the decoded message (bytes)
 * @return          0 when no error occurs
 */
uint32_t rsa_pad_eme_pkcs_decode(uint32_t k, uint8_t *EM, uint8_t **message, size_t *mLen) CHECK_RESULT;


/**
 * Encode hash using PKCS. hash has to be hashed by the function of type hash_type
 * @param  emLen     Length of Encoded Message buffer (rsa parameter length in bytes)
 * @param  hash_type Hash function used to hash the parameter hash (also used for pkcs algorithm)
 * @param  EM        Pointer to the encoded message buffer (output)
 * @param  hash      Hash to encode (input)
 * @return           0 if no error occurs
 */
uint32_t rsa_pad_emsa_pkcs_encode(uint32_t emLen, sx_hash_fct_t hash_type, uint8_t *EM, uint8_t *hash) CHECK_RESULT;


/**
 * Encode hash using PSS. hash has to be hashed by the function of type hash_type
 * This function uses a salt length equal to the hash digest length.
 * @param  emLen     Length of Encoded Message buffer (rsa parameter length in bytes)
 * @param  hashType  Hash function used to hash the parameter hash (also used for pkcs algorithm)
 * @param  EM        Pointer to the encoded message buffer (output)
 * @param  hash      Hash to encode (input)
 * @param  n0        MSB of the modulus (for masking in order to matchthe modulus size)
 * @param  sLen      intended length of the salt
 * @return           0 if no error occurs
 */
uint32_t rsa_pad_emsa_pss_encode(uint32_t emLen, sx_hash_fct_t hashType, uint8_t *EM, uint8_t *hash, uint32_t n0 ,size_t sLen) CHECK_RESULT;

/**
 * Decode encoded message using PSS and compares hash to the decoded hash
 * @param  emLen     Length of Encoded Message buffer (rsa parameter length in bytes)
 * @param  hashType  Hash function used to hash the parameter hash
 * @param  EM        Pointer to the encoded message buffer (input)
 * @param  hash      Hash to compare with
 * @param  sLen      intended length of the salt
 * @param  n0        MSB of the modulus (for masking in order to matchthe modulus size)
 * @return           0 if no error occurs and hash is valid
 */
uint32_t rsa_pad_emsa_pss_decode(uint32_t emLen, sx_hash_fct_t hashType,
                                 uint8_t *EM, uint8_t *hash, uint32_t sLen,
                                 uint32_t n0) CHECK_RESULT;

/**
 * Pads the hash of hashLen to EM of emLen. MSBs are set to 0
 * @param EM      Destination buffer (pointer)
 * @param emLen   Length of the destination buffer (bytes)
 * @param hash    Input to pad
 * @param hashLen Length of the input
 */
void rsa_pad_zeros(uint8_t *EM, size_t emLen, uint8_t *hash, size_t hashLen);

#endif
