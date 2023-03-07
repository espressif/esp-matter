/***************************************************************************//**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/

/**
   Copyright (c) Certicom Corp. 1996-2009.   All rights reserved.

   This software contains trade secrets, confidential information, and
   other intellectual property of Certicom Corp. and its licensors.
   This software cannot be used, reproduced, or distributed in whole or
   in part by any means without the explicit prior consent of Certicom
   Corp.
   Such consent must arise from a separate license agreement from
   Certicom or its licensees, as appropriate. This software implements
   Canadian, U.S., and other nations' patents, both registered and
   pending  (see the readme file included in this distribution).

   Warning: U.S. Federal and state laws and international treaties
   provide severe civil and criminal penalties for the unauthorized
   reproduction in any form of these copyrighted materials.
   Criminal copyright infringement constitutes a felony and is
   punishable by a $250,000 fine and up to 5 years in prison.

   Certicom, Certicom Security Architecture, Certicom CodeSign, Security
   Builder, Security Builder BSP, Security Builder API, Security
   Builder Crypto, Security Builder SSL, Security Builder PKI, Security
   Builder NSE and Security Builder GSE are trademarks or registered
   trademarks of Certicom Corp.

   All other trademarks or registered trademarks listed herein are
   property of their respective owners.

   Certicom Corp. has intellectual property rights relating to
   technology embodied in this product.
   In particular, and without limitation, these intellectual property
   rights may include one or more of the U.S. and non-U.S. patents listed
   at www.certicom.com/patents
   and one or more additional patents or pending patent applications in
   the U.S. and in other countries.

   Information subject to change without notice.



   Certicom Corp. Technical Support
   Email: support@certicom.com
   Vox: 1-800-511-8011
   Fax: 1-800-474-3877
 */
/*****************************************************************************
*
*       eccapi.h
*
*****************************************************************************/

#ifndef ECCAPI_H
#define ECCAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#define ZSE_MAJOR 1
#define ZSE_MINOR 0
#define ZSE_PATCH 1
#define ZSE_BUILD 25
#define ZSE_VERSION ((unsigned long)(ZSE_MAJOR & 0x0F) << 28 | (unsigned long)(ZSE_MINOR & 0x0F) << 24 | (unsigned long)(ZSE_PATCH & 0x0FFF) << 12 | (unsigned long)(ZSE_BUILD & 0x0FFF))

#if defined(__C51__) /* Keil C compiler */
#define MCE_FUNCTION_TYPE reentrant
#else
#define MCE_FUNCTION_TYPE
#endif

#define SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE (22)
#define SECT163K1_UNCOMPRESSED_PUBLIC_KEY_SIZE (43)
#define SECT163K1_PRIVATE_KEY_SIZE (21)
#define SECT163K1_CERTIFICATE_SIZE (48)
#define SECT163K1_SHARED_SECRET_SIZE (21)
#define SECT163K1_POINT_ORDER_SIZE (21)
#define SECT283K1_COMPRESSED_PUBLIC_KEY_SIZE (37)
#define SECT283K1_UNCOMPRESSED_PUBLIC_KEY_SIZE (73)
#define SECT283K1_PRIVATE_KEY_SIZE (36)
#define SECT283K1_SHARED_SECRET_SIZE (36)
#define SECT283K1_POINT_ORDER_SIZE (36)
#define AES_MMO_HASH_SIZE (16)
#define AES_MMO_MAX_DATA_BITS (0xFFFFFFFF)
#define AES_MMO_MAX_DATA_BYTES (AES_MMO_MAX_DATA_BITS / 8)

// note new 1.2 certificate format, public key is now 32 bytes into certificate
//
//Bytes Name  Description
//  1 Type  Type of certificate = 0, implicit no extensions
//  8 SerialNo  Serial Number of the certificate - from the CA
//  1 Curve Curve identifier (sect283k1 is 13 or byte value 0x0D)
//  1 Hash  Hash identifier (AES-MMO)
//  8 Issuer  8 byte identifier, 64-bit IEEE 802.15.4 address
//  5 ValidFrom 40-bit Unix time from which the certificate is valid
//  4 Lifespan  32-bit # of seconds from the ValidFrom for which time the certificate is considered valid (0xFFFFFFFF = infinite)
//  8 SubjectID 8 byte identified, 64-bit IEEE 802.15.4 address
//  1 KeyUsage  Bit flag indicating key usage (see below)
// 37 PublicKey 37-byte compressed public key value from which the public key of the Subject is reconstructed.

#define CERT_PUBKEY_OFFSET 37
#define ZSE_CERTIFICATE_SIZE_283 (SECT283K1_COMPRESSED_PUBLIC_KEY_SIZE + CERT_PUBKEY_OFFSET)

// Begin Ember modifications
#define MCE_LIBRARY_NOT_PRESENT          0xFF
#define STUB_ECC_LIBRARY                 0xFFFFFFFFUL
#define MCE_LIBRARY_NOT_PRESENT_283K1    0xFF
#define STUB_ECC_LIBRARY_283K1           0xFFFFFFFFUL
#define NO_YIELDING                      0x0
// End Ember modifications

/******************************************************************************
 * Return Codes
 *****************************************************************************/

#define MCE_SUCCESS                0x00
#define MCE_ERR_FAIL_VERIFY        0x01
#define MCE_ERR_NULL_PRIVATE_KEY   0x02
#define MCE_ERR_NULL_PUBLIC_KEY    0x03
#define MCE_ERR_NULL_INPUT_BUF     0x04
#define MCE_ERR_NULL_OUTPUT_BUF    0x05
#define MCE_ERR_NULL_FUNC_PTR      0x06
#define MCE_ERR_NULL_EPHEM_PRI_KEY 0x07
#define MCE_ERR_NULL_EPHEM_PUB_KEY 0x08
#define MCE_ERR_BAD_INPUT          0x09

/** This is a prototype of a user-provided callback function that generates
 * random seeds of the specified length.

   This function should copy <tt>sz</tt> bytes of random data into
   <tt>buffer</tt>.

   @param[out] buffer   This is an unsigned char array of size at least
                     <tt>sz</tt> to hold the random data.
   @param[in]  sz       The number of bytes of random data to compute and store.

   @retval     MCE_SUCCESS Indicates successful completion.
 */

typedef int GetRandomDataFunc (unsigned char *buffer, unsigned long sz);

/** This is a prototype of a user-provided callback function that computes an
 * AES MMO message digest of the <tt>data</tt> of given size, <tt>sz</tt>.
 *
   This function should compute the hash of the <tt>data</tt> parameter of size
   <tt>sz</tt>, and store the result in the <tt>digest</tt> buffer parameter.

   @param[out] digest  This is an unsigned char buffer to hold the message digest.
                    The length of the digest must be <tt>AES_MMO_HASH_SIZE</tt> bytes.
   @param[in] sz       The size in bytes of the message to be hashed.
   @param[in] data     This is an unsigned char buffer of data to be hashed.

   @retval MCE_SUCCESS Indicates successful completion.
 */
typedef int HashFunc (unsigned char *digest, unsigned long sz, unsigned char *data);

/** This is a prototype of a user-provided callback function to process information
   during a long computation inside the library.

   @retval - This user-provided function should return MCE_SUCCESS to indicate
          successful completion.
 */

typedef int YieldFunc (void);

/**

   This is a sample implementation of the <tt>HashFunc</tt> callback used by the
   Security Builder MCE toolkit. Please note that this is not an optimized
   implementation and is provided for testing purposes.

   This function computes the AES MMO digest of the <tt>data</tt> parameter of
   length <tt>sz</tt>, and stores the result in <tt>digest</tt>.

   @param[out] digest  This is an unsigned char buffer to hold the message digest.
                    The length of the digest must be <tt>AES_MMO_HASH_SIZE</tt> bytes.
   @param[in] sz       The size in bytes of the message to be hashed.
   @param[in] data     This is an unsigned char buffer of data to be hashed.

   @retval MCE_ERR_NULL_OUTPUT_BUF  <tt>digest</tt> is <tt>NULL</tt>
   @retval MCE_ERR_NULL_INPUT_BUF   <tt>data</tt> is <tt>NULL</tt>
   @retval MCE_SUCCESS              indicates successful completion.

 */
int aesMmoHash(unsigned char *digest, unsigned long sz, unsigned char *data);

/**

   Creates an ECDSA signature of a message digest.
   The outputs are the r and s components of the signature.

   @param[in] privateKey The private key. This is an unsigned char buffer of size
                      <tt>SECT163K1_PRIVATE_KEY_SIZE</tt>.
   @param[in] msgDigest  The hash of the message to be signed. This is an unsigned
                      char buffer of size <tt>AES_MMO_HASH_SIZE</tt>.
   @param[in] GetRandomData Pointer to a function to get random data for
                         generating ephemeral keys.
   @param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from <tt>0</tt> to <tt>10</tt>.
                      <tt>0</tt>  will never yield.
                      <tt>1</tt>  will  yield the most often.
                      <tt>10</tt> will yield the least often.
   @param[in] YieldFunc  Pointer to a function to allow user defined yielding.
   @param[out] r The r component of the signature. This is an unsigned char buffer
              of size <tt>SECT163K1_POINT_ORDER_SIZE</tt>.
   @param[out] s The s component of the signature. This is an unsigned char buffer
              of size <tt>SECT163K1_POINT_ORDER_SIZE</tt>.

   @retval MCE_ERR_NULL_PRIVATE_KEY    <tt>privateKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_OUTPUT_BUF     <tt>msgDigest</tt>, <tt>r</tt> or
                                    <tt>s</tt> are <tt>NULL</tt>.
   @retval MCE_ERR_NULL_FUNC_PTR       <tt>GetRandomData</tt> is <tt>NULL</tt> or
                                    <tt>YieldFunc</tt> is <tt>NULL</tt> and
                                    <tt>YieldLevel</tt> is not <tt>0</tt>.
   @retval MCE_ERR_BAD_INPUT           <tt>YieldLevel</tt> is greater than <tt>10</tt>.
   @retval MCE_SUCCESS                 Success.
 */

int ZSE_ECDSASign(unsigned char *privateKey,
                  unsigned char *msgDigest,
                  GetRandomDataFunc *GetRandomData,
                  unsigned char *r,
                  unsigned char *s,
                  YieldFunc *yield,
                  unsigned long yieldLevel);

/**

   Verifies an ECDSA signature created using a private signing key by using
   the associated public key, the digest and the signature components.

   @param[in] publicKey The public key. This is an unsigned char buffer of size
                     <tt>SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE</tt>.
   @param[in] msgDigest The hash of the message to be verified.  This is an
                     unsigned char buffer of size <tt>AES_MMO_HASH_SIZE</tt>.
   @param[in] r         The r component of the signature. This is an unsigned char
                     buffer of size <tt>SECT163K1_POINT_ORDER_SIZE</tt>.
   @param[in] s         The s component of the signature. This is an unsigned char
                     buffer of size <tt>SECT163K1_POINT_ORDER_SIZE</tt>.
   @param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from <tt>0</tt> to <tt>10</tt>.
                      <tt>0</tt>  will never yield.
                      <tt>1</tt>  will  yield the most often.
                      <tt>10</tt> will yield the least often.
   @param[in] YieldFunc  Pointer to a function to allow user defined yielding.
                      <tt>YieldFunc</tt> may be <tt>NULL</tt> if <tt>yieldLevel</tt> is <tt>0</tt>.

   @retval MCE_ERR_FAIL_VERIFY        The signature verification failed.
   @retval MCE_ERR_NULL_PUBLIC_KEY    <tt>publicKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_INPUT_BUF     <tt>msgDigest</tt>, <tt>r</tt> or
                                   <tt>s</tt> are <tt>NULL</tt>.
   @retval MCE_ERR_NULL_FUNC_PTR      <tt>YieldFunc</tt> is <tt>NULL</tt> and
                                   <tt>YieldLevel</tt> is not <tt>0</tt>.
   @retval MCE_ERR_BAD_INPUT          <tt>YieldLevel</tt> is greater than <tt>10</tt>.
   @retval MCE_SUCCESS                Success.
 */

int ZSE_ECDSAVerify(unsigned char *publicKey,
                    unsigned char *msgDigest,
                    unsigned char *r,
                    unsigned char *s,
                    YieldFunc *yield,
                    unsigned long yieldLevel);

/**

   Generates an ephemeral key pair using the specified random data generation
   function. Normally, the public key is sent to the remote party as part of the
   key agreement protocol.

   @param[out] privateKey   The generated private key. This is an unsigned char
                         buffer of size <tt>SECT163K1_PRIVATE_KEY_SIZE</tt>.
   @param[out] publicKey    The generated public key. This is an unsigned char
                         buffer of size
                         <tt>SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE</tt>.
   @param[in] GetRandomData Pointer to a function to get random data for
                         generating the ephemeral key pair.
   @param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from <tt>0</tt> to <tt>10</tt>.
                      <tt>0</tt>  will never yield.
                      <tt>1</tt>  will yield the most often.
                      <tt>10</tt> will yield the least often.
   @param[in] YieldFunc  Pointer to a function to allow user defined yielding.
                      <tt>YieldFunc</tt> may be <tt>NULL</tt> if <tt>yieldLevel</tt> is <tt>0</tt>.

   @retval MCE_ERR_NULL_OUTPUT_BUF    <tt>privateKey</tt> or <tt>publicKey</tt>
                                   are <tt>NULL</tt>.
   @retval MCE_ERR_NULL_FUNC_PTR      <tt>GetRandomData</tt> is <tt>NULL</tt> or
                                   <tt>YieldFunc</tt> is <tt>NULL</tt> and
                                   <tt>YieldLevel</tt> is not <tt>0</tt>.
   @retval MCE_ERR_BAD_INPUT          <tt>YieldLevel</tt> is greater than <tt>10</tt>.
   @retval MCE_SUCCESS                Success.
 */

int ZSE_ECCGenerateKey(unsigned char *privateKey,
                       unsigned char *publicKey,
                       GetRandomDataFunc *GetRandomData,
                       YieldFunc *yield,
                       unsigned long yieldLevel);

/**

   Derives a shared secret using the ECMQV algorithm. The public key of the
   remote party is reconstructed using its implicit certificate and the CA
   public key.

   @param[in] privateKey  The static private key of the local entity. This is an
                       unsigned char buffer of size
                       <tt>SECT163K1_PRIVATE_KEY_SIZE</tt>.
   @param[in] ephemeralPrivateKey   The ephemeral private key of the local entity.
                                 It should be generated using a previous call
                                 to the function <tt>ECCGenerateKey</tt>. An
                                 unsigned char buffer of size
                                 <tt>SECT163K1_PRIVATE_KEY_SIZE</tt>.
   @param[in] ephemeralPublicKey    The ephemeral public key of the local entity.
                                 It should be generated using a previous call
                                 to the function <tt>ECCGenerateKey</tt>. An
                                 unsigned char buffer of size
                                 <tt>SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE</tt>.
   @param[in] remoteCertificate     Implicit certificate of the remote party.
                                 This is an unsigned char buffer of size
                                 <tt>SECT163K1_CERTIFICATE_SIZE</tt>. The
                                 static public key of the remote party is
                                 derived from the certificate using the CA's
                                 public key.
   @param[in] remoteEphemeralPublicKey  Ephemeral public key received from the
                                     remote party. This is an unsigned char
                                     buffer of size
                                 <tt>SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE</tt>.
   @param[in] caPublicKey Public key of the certificate authority. The static
                       public key for the remote party is derived from the
                       certificate using the CA's public key.
   @param[out] keyBits    The derived shared secret. This is an unsigned char
                       buffer of size <tt>SECT163K1_SHARED_SECRET_SIZE</tt>.
   @param[in] Hash        Pointer to a function to hash the certificate data.
   @param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from <tt>0</tt> to <tt>10</tt>.
                      <tt>0</tt>  will never yield.
                      <tt>1</tt>  will  yield the most often.
                      <tt>10</tt> will yield the least often.
   @param[in] YieldFunc  Pointer to a function to allow user defined yielding.
                      <tt>YieldFunc</tt> may be <tt>NULL</tt> if <tt>yieldLevel</tt> is <tt>0</tt>.

   @retval MCE_ERR_NULL_PRIVATE_KEY      <tt>privateKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_EPHEM_PRI_KEY    <tt>ephemeralPrivateKey</tt> is
                                      <tt>NULL</tt>
   @retval MCE_ERR_NULL_EPHEM_PUB_KEY    <tt>ephemeralPublicKey</tt> or
                                      <tt>remoteEphemeralPublicKey</tt> are
                                      <tt>NULL</tt>.
   @retval MCE_ERR_NULL_INPUT_BUF        <tt>remoteCertificate</tt> is
                                      <tt>NULL</tt>.
   @retval MCE_ERR_NULL_PUBLIC_KEY       <tt>caPublicKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_OUTPUT_BUF       <tt>keyBits</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_FUNC_PTR         <tt>Hash</tt> is <tt>NULL</tt> or
                                      <tt>YieldFunc</tt> is <tt>NULL</tt> and
                                      <tt>YieldLevel</tt> is not <tt>0</tt>.
   @retval MCE_ERR_BAD_INPUT             <tt>YieldLevel</tt> is greater than <tt>10</tt>.
   @retval MCE_SUCCESS                   Success.
 */

int ZSE_ECCKeyBitGenerate(unsigned char *privateKey,
                          unsigned char *ephemeralPrivateKey,
                          unsigned char *ephemeralPublicKey,
                          unsigned char *remoteCertificate,
                          unsigned char *remoteEphemeralPublicKey,
                          unsigned char *caPublicKey,
                          unsigned char *keyBits,
                          HashFunc *Hash,
                          YieldFunc *yield,
                          unsigned long yieldLevel);

/**

   Reconstructs the remote party's public key using its implicit certificate
   and the CA public key.

   @param[in] certificate  Implicit certificate of the remote party. This is an unsigned
                        char buffer of size
                        <tt>SECT163K1_CERTIFICATE_SIZE</tt>. The static public
                        key of the remote party is derived from the certificate
                        using the CA's public key.
   @param[in] caPublicKey  Public key of the certificate authority. The static
                        public key of the remote party is derived from the
                        certificate using the CA's public key.
   @param[out] publicKey   The derived public key. This is an unsigned char buffer
                        of size
                        <tt>SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE</tt>.
   @param[in] Hash         Pointer to a function to hash the certificate data.
   @param[in] yieldLevel   The yield level determines how often the user defined
                        yield function will be called. This is a number from
                        <tt>0</tt> to <tt>10</tt>.
                        <tt>0</tt>  will never yield.
                        <tt>1</tt>  will  yield the most often.
                        <tt>10</tt> will yield the least often.
   @param[in] YieldFunc    Pointer to a function to allow user defined yielding.
                        <tt>YieldFunc</tt> may be <tt>NULL</tt> if
                        <tt>yieldLevel</tt> is <tt>0</tt>.



   @retval MCE_ERR_NULL_INPUT_BUF        <tt>certificate</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_PUBLIC_KEY       <tt>caPublicKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_OUTPUT_BUF       <tt>publicKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_FUNC_PTR         <tt>Hash</tt> is <tt>NULL</tt> or
                                      <tt>YieldFunc</tt> is <tt>NULL</tt> and
                                      <tt>YieldLevel</tt> is not <tt>0</tt>.
   @retval MCE_ERR_BAD_INPUT             <tt>YieldLevel</tt> is greater than <tt>10</tt>.
 */

int ZSE_ECQVReconstructPublicKey(unsigned char* certificate,
                                 unsigned char* caPublicKey,
                                 unsigned char* publicKey,
                                 HashFunc *Hash,
                                 YieldFunc *yield,
                                 unsigned long yieldLevel);

/**

   Returns the version number for the product.

   @retval ZSE_VERSION The product version number.
 */

unsigned long ZSE_GetVersion(void);

/**

   Creates an ECDSA signature of a message digest.
   The outputs are the r and s components of the signature.

   @param[in] privateKey The private key. This is an unsigned char buffer of size
                      <tt>SECT283K1_PRIVATE_KEY_SIZE</tt>.
   @param[in] msgDigest  The hash of the message to be signed. This is an unsigned
                      char buffer of size <tt>AES_MMO_HASH_SIZE</tt>.
   @param[in] GetRandomData Pointer to a function to get random data for
                         generating ephemeral keys.
   @param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from <tt>0</tt> to <tt>10</tt>.
                      <tt>0</tt>  will never yield.
                      <tt>1</tt>  will  yield the most often.
                      <tt>10</tt> will yield the least often.
   @param[in] YieldFunc  Pointer to a function to allow user defined yielding.
   @param[out] r The r component of the signature. This is an unsigned char buffer
              of size <tt>SECT283K1_POINT_ORDER_SIZE</tt>.
   @param[out] s The s component of the signature. This is an unsigned char buffer
              of size <tt>SECT283K1_POINT_ORDER_SIZE</tt>.

   @retval MCE_ERR_NULL_PRIVATE_KEY    <tt>privateKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_OUTPUT_BUF     <tt>msgDigest</tt>, <tt>r</tt> or
                                    <tt>s</tt> are <tt>NULL</tt>.
   @retval MCE_ERR_NULL_FUNC_PTR       <tt>GetRandomData</tt> is <tt>NULL</tt> or
                                    <tt>YieldFunc</tt> is <tt>NULL</tt> and
                                    <tt>YieldLevel</tt> is not <tt>0</tt>.
   @retval MCE_ERR_BAD_INPUT           <tt>YieldLevel</tt> is greater than <tt>10</tt>.
   @retval MCE_SUCCESS                 Success.
 */

int ZSE_ECDSASign283(unsigned char *privateKey,
                     unsigned char *msgDigest,
                     GetRandomDataFunc *GetRandomData,
                     unsigned char *r,
                     unsigned char *s,
                     YieldFunc *yield,
                     unsigned long yieldLevel);

/**

   Verifies an ECDSA signature created using a private signing key by using
   the associated public key, the digest and the signature components.

   @param[in] publicKey The public key. This is an unsigned char buffer of size
                     <tt>SECT283K1_COMPRESSED_PUBLIC_KEY_SIZE</tt>.
   @param[in] msgDigest The hash of the message to be verified.  This is an
                     unsigned char buffer of size <tt>AES_MMO_HASH_SIZE</tt>.
   @param[in] r         The r component of the signature. This is an unsigned char
                     buffer of size <tt>SECT283K1_POINT_ORDER_SIZE</tt>.
   @param[in] s         The s component of the signature. This is an unsigned char
                     buffer of size <tt>SECT283K1_POINT_ORDER_SIZE</tt>.
   @param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from <tt>0</tt> to <tt>10</tt>.
                      <tt>0</tt>  will never yield.
                      <tt>1</tt>  will  yield the most often.
                      <tt>10</tt> will yield the least often.
   @param[in] YieldFunc  Pointer to a function to allow user defined yielding.
                      <tt>YieldFunc</tt> may be <tt>NULL</tt> if <tt>yieldLevel</tt> is <tt>0</tt>.

   @retval MCE_ERR_FAIL_VERIFY        The signature verification failed.
   @retval MCE_ERR_NULL_PUBLIC_KEY    <tt>publicKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_INPUT_BUF     <tt>msgDigest</tt>, <tt>r</tt> or
                                   <tt>s</tt> are <tt>NULL</tt>.
   @retval MCE_ERR_NULL_FUNC_PTR      <tt>YieldFunc</tt> is <tt>NULL</tt> and
                                   <tt>YieldLevel</tt> is not <tt>0</tt>.
   @retval MCE_ERR_BAD_INPUT          <tt>YieldLevel</tt> is greater than <tt>10</tt>.
   @retval MCE_SUCCESS                Success.
 */

int ZSE_ECDSAVerify283(unsigned char *publicKey,
                       unsigned char *msgDigest,
                       unsigned char *r,
                       unsigned char *s,
                       YieldFunc *yield,
                       unsigned long yieldLevel);

/**

   Generates an ephemeral key pair using the specified random data generation
   function. Normally, the public key is sent to the remote party as part of the
   key agreement protocol.

   @param[out] privateKey   The generated private key. This is an unsigned char
                         buffer of size <tt>SECT283K1_PRIVATE_KEY_SIZE</tt>.
   @param[out] publicKey    The generated public key. This is an unsigned char
                         buffer of size
                         <tt>SECT283K1_COMPRESSED_PUBLIC_KEY_SIZE</tt>.
   @param[in] GetRandomData Pointer to a function to get random data for
                         generating the ephemeral key pair.
   @param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from <tt>0</tt> to <tt>10</tt>.
                      <tt>0</tt>  will never yield.
                      <tt>1</tt>  will yield the most often.
                      <tt>10</tt> will yield the least often.
   @param[in] YieldFunc  Pointer to a function to allow user defined yielding.
                      <tt>YieldFunc</tt> may be <tt>NULL</tt> if <tt>yieldLevel</tt> is <tt>0</tt>.

   @retval MCE_ERR_NULL_OUTPUT_BUF    <tt>privateKey</tt> or <tt>publicKey</tt>
                                   are <tt>NULL</tt>.
   @retval MCE_ERR_NULL_FUNC_PTR      <tt>GetRandomData</tt> is <tt>NULL</tt> or
                                   <tt>YieldFunc</tt> is <tt>NULL</tt> and
                                   <tt>YieldLevel</tt> is not <tt>0</tt>.
   @retval MCE_ERR_BAD_INPUT          <tt>YieldLevel</tt> is greater than <tt>10</tt>.
   @retval MCE_SUCCESS                Success.
 */

int ZSE_ECCGenerateKey283(unsigned char *privateKey,
                          unsigned char *publicKey,
                          GetRandomDataFunc *GetRandomData,
                          YieldFunc *yield,
                          unsigned long yieldLevel);

/**

   Derives a shared secret using the ECMQV algorithm. The public key of the
   remote party is reconstructed using its implicit certificate and the CA
   public key.

   @param[in] privateKey  The static private key of the local entity. This is an
                       unsigned char buffer of size
                       <tt>SECT283K1_PRIVATE_KEY_SIZE</tt>.
   @param[in] ephemeralPrivateKey   The ephemeral private key of the local entity.
                                 It should be generated using a previous call
                                 to the function <tt>ECCGenerateKey</tt>. An
                                 unsigned char buffer of size
                                 <tt>SECT283K1_PRIVATE_KEY_SIZE</tt>.
   @param[in] ephemeralPublicKey    The ephemeral public key of the local entity.
                                 It should be generated using a previous call
                                 to the function <tt>ECCGenerateKey</tt>. An
                                 unsigned char buffer of size
                                 <tt>SECT283K1_COMPRESSED_PUBLIC_KEY_SIZE</tt>.
   @param[in] remoteCertificate     Implicit certificate of the remote party.
                                 This is an unsigned char buffer of size
                                 <tt>ZSE_CERTIFICATE_SIZE_283</tt> which depends
                                 on the ZSE version Certificate Profile. The
                                 static public key of the remote party is
                                 derived from the certificate using the CA's
                                 public key.
   @param[in] remoteEphemeralPublicKey  Ephemeral public key received from the
                                     remote party. This is an unsigned char
                                     buffer of size
                                 <tt>SECT283K1_COMPRESSED_PUBLIC_KEY_SIZE</tt>.
   @param[in] caPublicKey Public key of the certificate authority. The static
                       public key for the remote party is derived from the
                       certificate using the CA's public key.
   @param[out] keyBits    The derived shared secret. This is an unsigned char
                       buffer of size <tt>SECT283K1_SHARED_SECRET_SIZE</tt>.
   @param[in] Hash        Pointer to a function to hash the certificate data.
   @param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from <tt>0</tt> to <tt>10</tt>.
                      <tt>0</tt>  will never yield.
                      <tt>1</tt>  will  yield the most often.
                      <tt>10</tt> will yield the least often.
   @param[in] YieldFunc  Pointer to a function to allow user defined yielding.
                      <tt>YieldFunc</tt> may be <tt>NULL</tt> if <tt>yieldLevel</tt> is <tt>0</tt>.

   @retval MCE_ERR_NULL_PRIVATE_KEY      <tt>privateKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_EPHEM_PRI_KEY    <tt>ephemeralPrivateKey</tt> is
                                      <tt>NULL</tt>
   @retval MCE_ERR_NULL_EPHEM_PUB_KEY    <tt>ephemeralPublicKey</tt> or
                                      <tt>remoteEphemeralPublicKey</tt> are
                                      <tt>NULL</tt>.
   @retval MCE_ERR_NULL_INPUT_BUF        <tt>remoteCertificate</tt> is
                                      <tt>NULL</tt>.
   @retval MCE_ERR_NULL_PUBLIC_KEY       <tt>caPublicKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_OUTPUT_BUF       <tt>keyBits</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_FUNC_PTR         <tt>Hash</tt> is <tt>NULL</tt> or
                                      <tt>YieldFunc</tt> is <tt>NULL</tt> and
                                      <tt>YieldLevel</tt> is not <tt>0</tt>.
   @retval MCE_ERR_BAD_INPUT             <tt>YieldLevel</tt> is greater than <tt>10</tt>.
   @retval MCE_SUCCESS                   Success.
 */

int ZSE_ECCKeyBitGenerate283(unsigned char *privateKey,
                             unsigned char *ephemeralPrivateKey,
                             unsigned char *ephemeralPublicKey,
                             unsigned char *remoteCertificate,
                             unsigned char *remoteEphemeralPublicKey,
                             unsigned char *caPublicKey,
                             unsigned char *keyBits,
                             HashFunc *Hash,
                             YieldFunc *yield,
                             unsigned long yieldLevel);

/**

   Reconstructs the remote party's public key using its implicit certificate
   and the CA public key.

   @param[in] certificate  Implicit certificate of the remote party. This is an unsigned
                        char buffer of size
                        <tt>ZSE_CERTIFICATE_SIZE_283</tt> which depends on
                        the ZSE Certificate Profile. The static public
                        key of the remote party is derived from the certificate
                        using the CA's public key.
   @param[in] caPublicKey  Public key of the certificate authority. The static
                        public key of the remote party is derived from the
                        certificate using the CA's public key.
   @param[out] publicKey   The derived public key. This is an unsigned char buffer
                        of size
                        <tt>SECT283K1_COMPRESSED_PUBLIC_KEY_SIZE</tt>.
   @param[in] Hash         Pointer to a function to hash the certificate data.
   @param[in] yieldLevel   The yield level determines how often the user defined
                        yield function will be called. This is a number from
                        <tt>0</tt> to <tt>10</tt>.
                        <tt>0</tt>  will never yield.
                        <tt>1</tt>  will  yield the most often.
                        <tt>10</tt> will yield the least often.
   @param[in] YieldFunc    Pointer to a function to allow user defined yielding.
                        <tt>YieldFunc</tt> may be <tt>NULL</tt> if
                        <tt>yieldLevel</tt> is <tt>0</tt>.



   @retval MCE_ERR_NULL_INPUT_BUF        <tt>certificate</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_PUBLIC_KEY       <tt>caPublicKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_OUTPUT_BUF       <tt>publicKey</tt> is <tt>NULL</tt>.
   @retval MCE_ERR_NULL_FUNC_PTR         <tt>Hash</tt> is <tt>NULL</tt> or
                                      <tt>YieldFunc</tt> is <tt>NULL</tt> and
                                      <tt>YieldLevel</tt> is not <tt>0</tt>.
   @retval MCE_ERR_BAD_INPUT             <tt>YieldLevel</tt> is greater than <tt>10</tt>.
 */

int ZSE_ECQVReconstructPublicKey283(unsigned char* certificate,
                                    unsigned char* caPublicKey,
                                    unsigned char* publicKey,
                                    HashFunc *Hash,
                                    YieldFunc *yield,
                                    unsigned long yieldLevel);

/**

   Returns the version number for the product.

   @retval ZSE_VERSION The product version number.
 */

unsigned long ZSE_GetVersion283(void);

#ifdef __cplusplus
}
#endif

#endif
