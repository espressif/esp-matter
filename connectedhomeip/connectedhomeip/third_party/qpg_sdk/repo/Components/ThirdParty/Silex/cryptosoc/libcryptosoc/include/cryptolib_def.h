
#ifndef CRYPTOLIB_DEF_H
#define CRYPTOLIB_DEF_H

// address definitions moved to cryptolib_internal.h
// debug hooks moved to cryptolib_internal.h

// return code
#define CRYPTOLIB_SUCCESS             0
#define CRYPTOLIB_UNSUPPORTED_ERR     1
#define CRYPTOLIB_INVALID_SIGN_ERR    3
#define CRYPTOLIB_DMA_ERR             4
#define CRYPTOLIB_INVALID_POINT       5
#define CRYPTOLIB_CRYPTO_ERR          6
#define CRYPTOLIB_INVALID_PARAM       7
#define CRYPTOLIB_RESEED_REQUIRED     8

// features
#define DEBUG_ENABLED 1
#define AES_CBC_ENABLED 0
#define AES_CCM_ENABLED 1
#define AES_OFB_ENABLED 0
#define AES_CFB_ENABLED 0
#define AES_XTS_ENABLED 0
#if defined(GP_DIVERSITY_GPHAL_K8E)
#define MD5_ENABLED 0
#define SHA1_ENABLED 0
#else
#define MD5_ENABLED 1
#define SHA1_ENABLED 1
#endif
#define SHA224_ENABLED 1
#if defined(GP_DIVERSITY_GPHAL_K8E)
#define HMAC_ENABLED 0
#else
#define HMAC_ENABLED 1
#endif

#define DH_MODP_ENABLED 1
#define SRP_ENABLED 1
#define JPAKE_ENABLED 1
#define CHACHAPOLY_ENABLED 0
#define PRIME_GEN_ENABLED 1
#define RSA_ENABLED 1
#define DSA_ENABLED 1
#define ECKCDSA_ENABLED 0
#define AIS31_ENABLED 0

// addtional feature flags added by Qorvo
#define AES_CTR_ENABLED           0   // AES-CTR mode disabled
#define AES_GCM_ENABLED           0   // AES-GCM mode disabled
#define AES_CMA_ENABLED           0   // AES-CMA mode disabled
#define ECC_BINARY_ENABLED        0   // GF(2^n) ECC curves disabled
#define CRYPTOLIB_NO_CRYPTORAM_DMA    // do not use DMA to access CryptoRAM
#define AES_PARTIAL_ENABLED       0   // do not encrypt partial messages
#define AES_MASK_EMABLED          0   // do not support AES countermeasures
#define EDDSA_ENABLED             0   // include sx_eddsa_alg.c
#define DERIVE_KEY_ENABLED        0   // include sx_derive_key_alg.c
#define LEGACY_SECUREBOOT_ENABLED 0   // include sx_legacy_secureboot.c
#define AES_HW_KEYS_ENABLED       0   // AES-HW keys disabled


#define CRYPTOLIB_TEST_ENABLED 0

#ifndef PK_CM_ENABLED
  #define PK_CM_ENABLED 0
#endif

// max supported sizes (number of bytes)
#define DH_MAX_KEY_SIZE       (4096/8)
#define SRP_MAX_KEY_SIZE      (4096/8)
#define RSA_MAX_SIZE          roundup_32(521/8 + 1)
#define DSA_MAX_SIZE_P        (3072/8)
#define DSA_MAX_SIZE_Q        (256/8)
#define PRIME_MAX_SIZE        (RSA_MAX_SIZE/2)
#define ECC_MAX_KEY_SIZE      roundup_32(521/8 + 1)
#define DERIV_MAX_SALT_SIZE   (512)
#define DERIV_MAX_INFO_SIZE   (512)

//RNG settings
#define RNG_CLKDIV            (0)
#define RNG_OFF_TIMER_VAL     (0)
#define RNG_FIFO_WAKEUP_LVL   (32)
#define RNG_INIT_WAIT_VAL     (512)
#define RNG_NB_128BIT_BLOCKS  (4)

// PK Counter-Measures
#if PK_CM_ENABLED
   // This value depends on the max operand size of PK !!
   #define PK_CM_RAND_SIZE    (8)

   #define PK_CM_RANDPROJ_ECC    1
   #define PK_CM_RANDKE_ECC      1
   #define PK_CM_RANDPROJ_MOD    1
   #define PK_CM_RANDKE_MOD      0
#endif

/** Minumum size for data to be copied by DMA */
#define BLK_MEMCPY_MIN_DMA_SIZE (0xffffffff)

#endif
