/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "aes.h"

/*!
 @defgroup cryptocell_api CryptoCell runtime library

 @{
 @}
 */

/*
  ############################TOP-LEVEL APIs###################################
 */
/*!
 @defgroup cc_top Basic CryptoCell library definitions
 @brief Contains basic CryptoCell library definitions.

 @{
 @ingroup cryptocell_api
 @}
 */

/*!
 @defgroup cc_lib Basic CryptoCell library APIs
 @brief Contains basic CryptoCell library APIs.

 This module lists the basic CryptoCell library APIs.

 @{
 @ingroup cc_top
 @}
 */

  /*!
 @defgroup cc_general_defs General CryptoCell definitions
 @brief Contains general definitions of the CryptoCell runtime SW APIs.

 @{
 @ingroup cc_top
 @}
 */

/*!
 @defgroup cc_error General base error codes for CryptoCell
 @brief Contains general base-error codes for CryptoCell.


 @{
 @ingroup cc_top
 @}
 */


 /*
  ################################AES APIs#####################################
 */
/*!
  @defgroup cc_aes CryptoCell AES APIs

  @ingroup cryptocell_api

  @brief AES is a symmetric block cipher that uses a combination of both substitution
  and permutation. It is fast in both software and hardware.

  AES has a fixed block size of 128 bits, and supports the following key sizes:
  <ul><li>128 bits.</li><li>192 bits.</li><li>256 bits.</li></ul>

  For the implementation of AES, see aes.h.

 */

 /*!
  @defgroup cc_aes_hw_limit CryptoCell-312 hardware limitations for AES

  @ingroup cc_aes

  The CrytoCell-312 hardware accelerates the following AES operations:
  <ul><li>ECB.</li>
  <li>CBC.</li>
  <li>CTR.</li>
  <li>CMAC. For the implementation of CMAC, see cmac.h.</li>
  <li>OFB.</li>
  <li>CCM. For the implementation of CCM, see ccm.h.</li>
  <li>CCM star. For the implementation of CCM star, see mbedtls_cc_ccm_star.h and ccm.h.</li>
  <li>GCM. For the implementation of GCM, see gcm.h.</li></ul>

  To support the accelerated algorithms, the following conditions
  must be met:
  <ul><li>The input and output buffers must be DMA-able.</li>
  <li>The input and output buffers must be physically contingous
  blocks in memory.</li>
  <li>Buffer size must be up to 64KB.</li>
  <li>The context must also be DMA-able, as partial
  and final results are written to the context.</li>
  <li>Only integrated operations are supported for CCM, CCM star and GCM algorithms.</li></ul>
 */

 /*!
  @defgroup cc_aes_typical Typical usage of AES in CryptoCell-312

  @ingroup cc_aes

  The following is a typical AES Block operation flow:
  <ol><li>mbedtls_aes_init().</li>
  <li>mbedtls_aes_setkey_enc().</li>
  <li>mbedtls_aes_crypt_cbc().</li></ol>

*/

/*!
 @defgroup cc_aesccm_star CryptoCell AES-CCM star APIs
 @brief Contains the CryptoCell AES-CCM star APIs.

 @{
 @ingroup cc_aes
 @}
 */

/*!
 @defgroup cc_aes_defs Definitions of CryptoCell AES APIs
 @brief Contains CryptoCell AES API type definitions.

 @{
 @ingroup cc_aes
 @}
 */

/*!
 @defgroup cc_aes_defs_proj Project definitions of CryptoCell AES APIs
 @brief Contains CryptoCell AES API project type definitions.

 @{
 @ingroup cc_aes_defs
 @}
 */

 /*!
 @defgroup cc_aesccm_star_common Common definitions of the CryptoCell AES-CCM star APIs
 @brief Contains the CryptoCell AES-CCM star APIs.

 @{
 @ingroup cc_aes_defs
 @}
 */


 /*
  ################################DHM APIs#####################################
 */
 /*!
  @defgroup dhm_module CryptoCell DHM APIs

  @ingroup cryptocell_api

  @brief Diffie-Hellman-Merkle (DHM) is used to securely exchange cryptographic
  keys over a public channel.

  As described in <em>Public-Key Cryptography Standards (PKCS) #3: Diffie Hellman
  Key Agreement Standard</em>:
  "[T]wo parties, without any prior arrangements, can agree upon a secret key
  that is known only to them...This secret key can then be used, for example,
  to encrypt further communications between the parties."

  The DHM module is implemented based on the definitions in the following
  standards:
  <ul><li><em>RFC-3526: More Modular Exponential (MODP) Diffie-Hellman groups
  for Internet Key Exchange (IKE)</em>: defines a number of standardized
  Diffie-Hellman groups for IKE.</li>
  <li><em>RFC-5114: Additional Diffie-Hellman Groups for Use with IETF
  Standards</em>: defines a number of standardized Diffie-Hellman
  groups that can be used.</li></ul>

  For the implementation of DHM, see dhm.h.
 */

 /*!
  @defgroup cc_dhm_hw_limit CryptoCell-312 hardware limitations for DHM

  @ingroup dhm_module

  To support the accelerated algorithms, the following conditions
  must be met:
  <ul><li>The contexts must be DMA-able, as they might
  be used for some symmetric operations.</li></ul>
 */

 /*!
  @defgroup cc_dhm_typical Typical usage of DHM in CryptoCell-312

  @ingroup dhm_module

  The following is a typical DHM flow for one party:
  <ol><li>mbedtls_dhm_init().</li>
  <li>mbedtls_mpi_read_string().</li>
  <li>mbedtls_mpi_read_string().</li>
  <li>mbedtls_dhm_make_params().</li>
  <li>mbedtls_dhm_read_public().</li>
  <li>mbedtls_dhm_calc_secret().</li></ol>
 */

/*
  ################################ECC APIs#####################################
 */

/*!
 @defgroup cc_ecc CryptoCell Elliptic Curve APIs
 @brief Contains all CryptoCell Elliptic Curve APIs.

 Elliptic-curve cryptography (ECC) is defined in <em>Standards for Efficient
 Cryptography Group (SECG): SEC1 Elliptic Curve Cryptography</em>.

 @{
 @ingroup cryptocell_api
 @}
*/

 /*!
  @defgroup ecdh_module ECDH module overview

  @ingroup cc_ecc

  @brief Elliptic-curve Diffie–Hellman (ECDH) is an anonymous key agreement
  protocol. It allows two parties to establish a shared secret over an
  insecure channel. Each party must have an elliptic-curve public–private
  key pair.

  For more information, see <em>NIST SP 800-56A Rev. 2: Recommendation
  for Pair-Wise Key Establishment Schemes Using Discrete Logarithm
  Cryptography</em>.

  For the implementation of ECDH, see ecdh.h.
 */

 /*!
  @defgroup cc_ecdh_hw_limit CryptoCell-312 hardware limitations for ECDH

  @ingroup ecdh_module

  CryotoCell-312 does not support Brainpool curves.

  */

/*!
  @defgroup cc_ecdh_typical Typical usage of ECDH in CryptoCell-312

  @ingroup ecdh_module

  The following is a typical ECDH operation flow:
  <ol><li>mbedtls_ecp_group_init().</li>
  <li>mbedtls_mpi_init() for each group parameter.</li>
  <li>mbedtls_ecdh_gen_public().</li></ol>
 */

/*!
 @defgroup ecdh_edwards CryptoCell ECDH Edwards curve APIs
 @brief Contains the CryptoCell ECDH Edwards curve APIs.
 @{
 @ingroup ecdh_module
 @}
 */

/*!
  @defgroup ecdsa_module ECDSA module overview

  @ingroup cc_ecc

  @brief The Elliptic Curve Digital Signature Algorithm (ECDSA) is a used for
  generating and validating digital signatures.

  For the definition of ECDSA, see <em>Standards for Efficient Cryptography Group (SECG):
  SEC1 Elliptic Curve Cryptography</em>.

  For the use of ECDSA for TLS, see <em>RFC-4492: Elliptic Curve
  Cryptography (ECC) Cipher Suites for Transport Layer Security (TLS)</em>.

  For the implementation of ECDSA, see ecdsa.h.
 */

/*!
  @defgroup cc_ecdsa_hw_limit CryptoCell-312 hardware limitations for ECDSA

  @ingroup ecdsa_module

  CryotoCell-312 does not support Brainpool curves.

  \note Using hash functions with hash size greater than the EC modulus size
        is not recommended.
 */
/*!
  @defgroup cc_ecdsa_typical Typical usage of ECDSA in CryptoCell-312

  @ingroup ecdsa_module

  The following is a typical ECDSA operation flow:
  <ol><li>mbedtls_ecp_group_init().</li>
  <li>mbedtls_mpi_init() for each group parameter.</li>
  <li>mbedtls_ecp_gen_keypair().</li>
  <li>mbedtls_ecdsa_sign() or mbedtls_ecdsa_verify().</li></ol>
 */

/*!
 @defgroup eddsa CryptoCell EDDSA Edwards curve APIs
 @brief Contains the CryptoCell EDDSA Edwards curve APIs.
 @{
 @ingroup ecdsa_module
 @}
 */

/*!
 @defgroup cc_ecies CryptoCell ECIES APIs
 @brief Contains the CryptoCell Elliptic Curve Integrated Encryption Scheme (ECIES) APIs.
 @{

 @ingroup cc_ecc
 @}
*/

/*!
 @defgroup cc_ecpki CryptoCell ECPKI APIs
 @brief Contains all CryptoCell ECPKI APIs.

 This module contains all definitions relating to Elliptic Curve Public Key Infrastructure.
 @{
 @ingroup cc_ecc
 @}
*/

/*!
 @defgroup cc_ecpki_domains_defs CryptoCell ECPKI supported domains
 @brief Contains CryptoCell ECPKI domains supported by the project.

 @{
 @ingroup cc_ecpki
 @}
 */

 /*!
 @defgroup cc_ecpki_types CryptoCell ECPKI type definitions
 @brief Contains CryptoCell ECPKI API type definitions.

 @{
 @ingroup cc_ecpki
 @}
 */

/*
  ##############################EXT DMA APIs###################################
 */

/*!
 @defgroup ext_dma CryptoCell external DMA APIs
 @brief Contains all CryptoCell external DMA API definitions.

 @{
 @ingroup cryptocell_api
 @}
*/

/*!
 @defgroup aes_ext_dma CryptoCell AES external DMA APIs
 @brief Contains CryptoCell AES external DMA API definitions.

 @{
 @ingroup ext_dma
 @}
*/

/*!
 @defgroup chacha_ext_dma CryptoCell ChaCha external DMA APIs
 @brief Contains CryptoCell ChaCha external DMA APIs.

 @{
 @ingroup ext_dma
 @}
 */

/*!
 @defgroup hash_ext_dma CryptoCell hash external DMA APIs
 @brief Contains CryptoCell hash external DMA APIs.

 @{
 @ingroup ext_dma
 @}
 */

/*!
 @defgroup ext_dma_errors Specific errors of the CryptoCell external DMA APIs
 @brief Contains the CryptoCell external DMA-API error definitions.

 @{
 @ingroup ext_dma
 @}
 */


/*
  ###############################HASH APIs#####################################
 */

/*!
 @defgroup cc_hash CryptoCell hash APIs
 @brief Contains all CryptoCell hash APIs and definitions.

  The hash or Message Digest (MD) module allows you to calculate
  hash digests from data, and create signatures based on those hash digests.

  HMAC is a wrapping algorithm that uses one of the supported
  hash algorithms and a key, to generate a unique
  authentication code over the input data.

  All hash algorithms can be accessed via the generic MD layer.
  For more information, see ::mbedtls_md_setup().

  For more information on supported hash algorithms, @see cc_hash_hw_limit.

  For the implementation of hash and HMAC, see md.h.
 @{
 @ingroup cryptocell_api
 @}
 */

/*!
  @defgroup cc_hash_hw_limit CryptoCell-312 hardware limitations for hash

  @ingroup cc_hash

  The CryptoCell-312 hardware supports accelerated hash operations for
  the following modes:
  <ul><li>SHA-1</li>
  <li>SHA-224</li>
  <li>SHA-256</li></ul>

  SHA-384 and SHA-512 operations are only supported in a
  non-accelerated software mode.

  To support the accelerated algorithms, the following conditions
  must be met:
  <ul><li>The input buffer must be DMA-able.</li>
  <li>The input buffer must be physically contingous
  block in memory.</li>
  <li>Buffer size must be up to 64KB.</li>
  <li>The context must also be DMA-able, as partial
  and final results are written to the context.</li></ul>
 */

/*!
  @defgroup cc_hash_typical Typical usage of hash in CryptoCell-312

  @ingroup cc_hash

  The following is a typical hash Block operation flow
  directly using the SHA module:
  <ol><li>mbedtls_sha1_init().</li>
  <li>mbedtls_sha1_starts_ret().</li>
  <li>mbedtls_sha1_update_ret().</li>
  <li>mbedtls_sha1_finish_ret().</li></ol>

  The following is a typical HMAC Block operation flow
  using the MD module:
  <ol><li>mbedtls_md_setup().</li>
  <li>mbedtls_md_hmac_starts().</li>
  <li>mbedtls_md_hmac_update().</li>
  <li>mbedtls_md_hmac_finish().</li></ol>
 */

 /*!
 @defgroup cc_sha512_t_h CryptoCell SHA-512 truncated APIs

 @ingroup cc_hash

 @brief Contains all CryptoCell SHA-512 truncated APIs.

 */

 /*!
 @defgroup cc_hash_defs CryptoCell hash API definitions
 @brief Contains CryptoCell hash API definitions.

 @{
 @ingroup cc_hash
 @}
*/

/*!
 @defgroup cc_hash_defs_proj CryptoCell hash API project-specific definitions
 @brief Contains the project-specific hash API definitions.

 @{
 @ingroup cc_hash
 @}
 */

/*
  ###############################MGMT APIs#####################################
 */
/*!
 @defgroup cc_management CryptoCell management APIs
 @brief Contains CryptoCell Management APIs.

 @{
 @ingroup cryptocell_api
 @}
 */

/*!
 @defgroup cc_management_error Specific errors of the CryptoCell Management APIs
 @brief Contains the CryptoCell management-API error definitions.

 @{
 @ingroup cc_management
 @}
*/

/*
  ################################PAL APIs#####################################
 */
/*!
 @defgroup cc_pal CryptoCell PAL APIs
 @brief Groups all PAL APIs and definitions.

 @{
 @ingroup cryptocell_api
 @}
 */

 /*!
 @defgroup cc_pal_abort CryptoCell PAL abort operations
 @brief Contains CryptoCell PAL abort operations.

 @{
 @ingroup cc_pal
 @}
 */

/*!
 @defgroup cc_pal_apbc CryptoCell PAL APB-C APIs
 @brief Contains PAL APB-C APIs.

 @{
 @ingroup cc_pal
 @}
 */

/*!
 @defgroup cc_pal_sb_plat CryptoCell PAL definitions for Boot Services
 @brief Contains CryptoCell PAL Secure Boot definitions.

 @{
 @ingroup cc_pal
 @}
 */

/*!
 @defgroup cc_pal_init CryptoCell PAL entry or exit point APIs
 @brief Contains PAL initialization and termination APIs.

 @{
 @ingroup cc_pal
 @}
 */

/*!
 @defgroup cc_pal_log CryptoCell PAL logging APIs and definitions
 @brief Contains CryptoCell PAL layer log definitions.

 @{
 @ingroup cc_pal
 @}
 */

/*!
 @defgroup cc_pal_mem CryptoCell PAL memory operations
 @brief Contains memory-operation functions.

 @{
 @ingroup cc_pal
 @}
 */

/*!
 @defgroup cc_pal_barrier CryptoCell PAL memory Barrier APIs
 @brief Contains memory-barrier implementation definitions and APIs.

 @{
 @ingroup cc_pal_mem
 @}
*/

/*!
 @defgroup cc_pal_memmap CryptoCell PAL memory mapping APIs
 @brief Contains memory mapping functions.

 @{
 @ingroup cc_pal_mem
 @}
 */

/*!
 @defgroup cc_pal_mutex CryptoCell PAL mutex APIs
 @brief Contains resource management functions.

 @{
 @ingroup cc_pal
 @}
 */

/*!
 @defgroup cc_pal_types CryptoCell PAL platform-dependent definitions and types
 @brief Contains CryptoCell PAL platform-dependent definitions and types.

 @{
 @ingroup cc_pal
 @}
*/

/*!
 @defgroup cc_pal_compiler CryptoCell PAL platform-dependent compiler-related definitions
 @brief Contains CryptoCell PAL platform-dependent compiler-related definitions.
 @{
 @ingroup cc_pal
 @}
 */

 /*!
 @defgroup cc_pal_pm CryptoCell PAL power-management APIs
 @brief Contains PAL power-management APIs.

 @{
 @ingroup cc_pal
 @}
 */

/*!
 @defgroup cc_pal_trng CryptoCell PAL TRNG APIs
 @brief Contains APIs for retrieving TRNG user parameters.

 @{
 @ingroup cc_pal
 @}
 */

/*!
 @defgroup cc_pal_error Specific errors of the CryptoCell PAL APIs
 @brief Contains platform-dependent PAL-API error definitions.

 @{
 @ingroup cc_pal
 @}
 */

/*
  ################################PKA APIs#####################################
 */
/*!
 @defgroup cc_pka CryptoCell PKA APIs
 @brief Contains all CryptoCell PKA APIs.

 @{
 @ingroup cryptocell_api
 @}
 */

 /*!
 @defgroup cc_pka_defs_hw CryptoCell PKA-specific definitions
 @brief Contains the CryptoCell PKA API definitions.

 @{
 @ingroup cc_pka
 @}
 */

/*!
 @defgroup cc_pka_hw_plat_defs CryptoCell PKA-API platform-dependent types and definitions
 @brief Contains the platform-dependent definitions of the CryptoCell PKA APIs.

 @{
 @ingroup cc_pka_defs_hw
 @}
 */


/*
  ###############################PROD APIs#####################################
 */

/*!
 @addtogroup prod CryptoCell production-library APIs
 @brief Contains CryptoCell production-library APIs.

 @{
 @ingroup cryptocell_api
 @}
 */

 /*!
 @defgroup prod_mem CryptoCell production-library definitions
 @brief Contains CryptoCell production-library definitions.

 @{
 @ingroup prod
 @}
 */

/*!
 @defgroup cc_cmpu CryptoCell ICV production library APIs
 @brief Contains CryptoCell ICV production library APIs.

 @{
 @ingroup prod
 @}
 */

/*!
 @defgroup cc_dmpu CryptoCell OEM production library APIs
 @brief Contains CryptoCell OEM production library APIs.

 @{
 @ingroup prod
 @}
 */

/*!
 @defgroup prod_errors Specific errors of the CryptoCell production-library APIs
 @brief Contains the CryptoCell production-library-API error definitions.

 @{
 @ingroup prod
 @}
 */


/*
  ################################RNG APIs#####################################
 */
/*!
  @defgroup rng_module CryptoCell RNG APIs

  @brief The Random Number Generator (RNG) module supports random number
  generation, as defined in <em>NIST SP 800-90A: Recommendation for Random
  Number Generation Using Deterministic Random Bit Generators</em>.
  See mbedtls_ctr_drbg_random().

  The block-cipher counter-mode based deterministic random-bit
  generator (CTR_DBRG). CryptoCell provides the source of entropy.

  For the implementation of RNG, see ctr_drbg.h.
  @{
  @ingroup cryptocell_api
  @}
 */

/*!
 @defgroup cc_rnd CryptoCell random-number generation APIs.
 @brief Contains the CryptoCell random-number generation APIs.
 @{
 @ingroup rng_module
 @}
 */


 /*
  ################################RSA APIs#####################################
 */
/*!
  @defgroup rsa_module CryptoCell RSA APIs

  @ingroup cryptocell_api

  @brief RSA is an asymmetric algorithm used for secure-data transmission.

  @note As it is considered slow, it is mainly used to pass encrypted shared
  keys for symmetric key cryptography.

  The RSA module implements the standards defined in <em>Public-Key Cryptography
  Standards (PKCS) #1 v1.5: RSA Encryption</em> and <em>Public-Key
  Cryptography Standards (PKCS) #1 v2.1: RSA Cryptography Specifications</em>.

  @note CryptoCell-312 does not support blinding for RSA. If a function receives
        random pointers as input, these may be NULL.

  For the implementation of RSA, see rsa.h
 */

/*!
  @defgroup cc_rsa_hw_limit CryptoCell-312 hardware limitations for RSA

  @ingroup rsa_module

  CryptoCell-312 supports the following RSA key sizes for private-public
  operations:
  <ul><li>256 bytes (2048 bits).</li>
  <li>384 bytes (3072 bits).</li>
  <li>512 bytes (4096 bits).</li></ul>

  For key-generation, CryptoCell-312 supports the following RSA key sizes:
  <ul><li>256 bytes (2048 bits).</li>
  <li>384 bytes (3072 bits).</li></ul>
 */

/*!
  @defgroup cc_rsa_typical Typical usage of RSA in CryptoCell-312

  @ingroup rsa_module

  The following is a typical RSA operation flow:
  <ol><li>mbedtls_rsa_init().</li>
  <li>mbedtls_rsa_gen_key().</li>
  <li>mbedtls_rsa_pkcs1_encrypt().</li></ol>

  @note CryptoCell-312 requires that the same \c hash_id used for
        mbedtls_rsa_init() is used for all subsequent operations.
        Otherwise, it returns an error.
 */

 /*!
  @defgroup cc_rsa_typical_ki Typical insertion of keys in CryptoCell-312

  @ingroup rsa_module

  The following is a typical RSA key-insertion flow:
  <ol><li>mbedtls_rsa_import() or mbedtls_rsa_import_raw().</li>
  <li>mbedtls_rsa_complete().</li></ol>

  If you insert keys that are not derived by CryptoCell-312,
  the following restrictions apply:
  <ul><li>The user may insert \c N, \c D, \c E, and the complete function does
  not derive the \c P and \c Q (the CRT values).</li>
  <li>The user may insert \c P and \c Q, and the complete function derives the
  CRT values from that, but does not derive \c D.</li>
  <li>Its Illegal to insert only part of the CRT key (only \c DP for example).</li>
  <li> If all the required key parameters were inserted the function does nothing.</li></ul>
 */

/*
  #############################Secure Boot APIs################################
 */
/*!
 @defgroup cc_sb CryptoCell Secure Boot and Secure Debug APIs.
 @brief Contains all Secure Boot and Secure Debug APIs and definitions.

 @{
 @ingroup cryptocell_api
 @}
 */

 /*!
 @defgroup cc_sb_image_verifier CryptoCell Secure Boot and Secure Debug API definitions
 @brief Contains definitions used for the Secure Boot and Secure Debug APIs.

 @{
 @ingroup cc_sb
 @}
 */

 /*!
 @defgroup cc_sb_basetypes CryptoCell Secure Boot basic type definitions
 @brief Contains CryptoCell Secure Boot basic type definitions.
 @{
 @ingroup cc_sb
 @}
 */

/*!
 @defgroup cc_sbrt CryptoCell Secure Boot certificate-chain-processing APIs.
 @brief Contains CryptoCell Secure Boot certificate-chain-processing APIs.

 @{
 @ingroup cc_sb
 @}
 */

/*!
 @defgroup cc_sb_defs CryptoCell Secure Boot type definitions
 @brief Contains CryptoCell Secure Boot type definitions.
 @{
 @ingroup cc_sb
 @}
 */

/*!
 @defgroup cc_sb_gen_defs CryptoCell Secure Boot and Secure Debug definitions and structures
 @brief Contains CryptoCell Secure Boot and Secure Debug definitions and structures.

 @{
 @ingroup cc_sb_defs
 @}
 */

/*
  ##############################SRAM MAP APIs###################################
 */
/*!
 @defgroup cc_sram_map CryptoCell SRAM mapping APIs
 @brief Contains internal SRAM mapping APIs.

 @{
 @ingroup cryptocell_api
 @}
 */


/*
  ################################SRP APIs#####################################
 */
/*!
 @defgroup cc_srp CryptoCell SRP APIs
 @brief Contains CryptoCell SRP APIs.

 @{
 @ingroup cryptocell_api
 @}
 */

/*!
 @defgroup cc_srp_errors Specific errors of the CryptoCell SRP APIs
 @brief Contains the CryptoCell SRP-API error definitions.
 @{
 @ingroup cc_srp
 @}
 */


 /*
  ################################UTIL APIs####################################
 */
/*!
 @defgroup cc_utils CryptoCell utility APIs
 @brief This contains all utility APIs.
 @{
 @ingroup cryptocell_api
 @}
 */

 /*!
 @defgroup cc_util_asset_prov CryptoCell runtime-library asset-provisioning APIs
 @brief Contains CryptoCell runtime-library ICV and OEM asset-provisioning APIs
 and definitions.

 @{
 @ingroup cc_utils
 @}
 */

 /*!
 @defgroup cc_utils_defs CryptoCell utility APIs general definitions
 @brief Contains CryptoCell utility APIs general definitions.
 @{
 @ingroup cc_utils
 @}
 */

 /*!
 @defgroup cc_utils_key_defs CryptoCell utility general key definitions
 @brief Contains KDF API definitions.
 @{
 @ingroup cc_utils_key_derivation
 @}
 */

 /*!
 @defgroup cc_utils_key_derivation CryptoCell utility key-derivation APIs
 @brief Contains the CryptoCell utility key-derivation function API.

 @{
 @ingroup cc_utils
 @}
 */

/*!
 @defgroup cc_utils_errors Specific errors of the CryptoCell utility module APIs
 @brief Contains utility API error definitions.

 @{
 @ingroup cc_utils
 @}
 */

