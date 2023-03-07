/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA413 hash function
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef BA413_CONFIG_H
#define BA413_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "sx_hw_cfg.h"

struct ba413_regs_s {
   volatile uint32_t config;
};


/** @brief Value for ::ba413_regs_s.config, set mode to MD5 */
#define BA413_CONF_MODE_MD5      0x00000001
/** @brief Value for ::ba413_regs_s.config, set mode to SHA1 */
#define BA413_CONF_MODE_SHA1     0x00000002
/** @brief Value for ::ba413_regs_s.config, set mode to SHA224 */
#define BA413_CONF_MODE_SHA224   0x00000004
/** @brief Value for ::ba413_regs_s.config, set mode to SHA256 */
#define BA413_CONF_MODE_SHA256   0x00000008
/** @brief Value for ::ba413_regs_s.config, set mode to SHA384 */
#define BA413_CONF_MODE_SHA384   0X00000010
/** @brief Value for ::ba413_regs_s.config, set mode to SHA512 */
#define BA413_CONF_MODE_SHA512   0x00000020
/** @brief Value for ::ba413_regs_s.config, set mode to SM3 */
#define BA413_CONF_MODE_SM3      0x00000040

/** @brief Value for ::ba413_regs_s.config, enable HMAC */
#define BA413_CONF_HMAC    0x00000100
/** @brief Value for ::ba413_regs_s.config, enable padding in hardware  */
#define BA413_CONF_HWPAD   0x00000200
/** @brief Value for ::ba413_regs_s.config, set as final (return digest and not state) */
#define BA413_CONF_FINAL   0x00000400

/* Hardware configuration register. */
#define BA413_HW_CFG_MD5_SUPPORTED_LSB            0
#define BA413_HW_CFG_MD5_SUPPORTED_MASK           (1L<<BA413_HW_CFG_MD5_SUPPORTED_LSB)
#define BA413_HW_CFG_SHA1_SUPPORTED_LSB           1
#define BA413_HW_CFG_SHA1_SUPPORTED_MASK          (1L<<BA413_HW_CFG_SHA1_SUPPORTED_LSB)
#define BA413_HW_CFG_SHA224_SUPPORTED_LSB         2
#define BA413_HW_CFG_SHA224_SUPPORTED_MASK        (1L<<BA413_HW_CFG_SHA224_SUPPORTED_LSB)
#define BA413_HW_CFG_SHA256_SUPPORTED_LSB         3
#define BA413_HW_CFG_SHA256_SUPPORTED_MASK        (1L<<BA413_HW_CFG_SHA256_SUPPORTED_LSB)
#define BA413_HW_CFG_SHA384_SUPPORTED_LSB         4
#define BA413_HW_CFG_SHA384_SUPPORTED_MASK        (1L<<BA413_HW_CFG_SHA384_SUPPORTED_LSB)
#define BA413_HW_CFG_SHA512_SUPPORTED_LSB         5
#define BA413_HW_CFG_SHA512_SUPPORTED_MASK        (1L<<BA413_HW_CFG_SHA512_SUPPORTED_LSB)
#define BA413_HW_CFG_SM3_SUPPORTED_LSB            6
#define BA413_HW_CFG_SM3_SUPPORTED_MASK           (1L<<BA413_HW_CFG_SM3_SUPPORTED_LSB)
#define BA413_HW_CFG_PADDING_SUPPORTED_LSB        16
#define BA413_HW_CFG_PADDING_SUPPORTED_MASK       (1L<<BA413_HW_CFG_PADDING_SUPPORTED_LSB)
#define BA413_HW_CFG_HMAC_SUPPORTED_LSB           17
#define BA413_HW_CFG_HMAC_SUPPORTED_MASK          (1L<<BA413_HW_CFG_HMAC_SUPPORTED_LSB)
#define BA413_HW_CFG_VERIFY_DIGEST_SUPPORTED_LSB  18
#define BA413_HW_CFG_VERIFY_DIGEST_SUPPORTED_MASK (1L<<BA413_HW_CFG_VERIFY_DIGEST_SUPPORTED_LSB)

#define BA413_HW_CFG (*(const volatile uint32_t*)ADDR_BA413_HW_CFG)

#ifdef CACHED_HW_CONFIG

/** This structure hosts a copy of the HW configuration registers */
struct sx_hash_engine_config {
   bool is_config_read; /**< is set the first time we retrieve the hardware
                             configuration from the engine */
   bool is_engine_available;
   bool is_md5_enabled;
   bool is_sha1_enabled;
   bool is_sha224_enabled;
   bool is_sha256_enabled;
   bool is_sha384_enabled;
   bool is_sha512_enabled;
   bool is_sm3_enabled;
   bool is_padding_enabled;
   bool is_hmac_enabled;
   bool is_verify_digest_enabled;
};

/**
 * @brief Reads and returns the HW configuration of the hash engine.
 */
const struct sx_hash_engine_config* sx_hash_engine_read_config(void);

/**
 * @brief Simply returns the HW configuration, which should have been
 * previously read using sx_hash_engine_read_config().
 * @warning sx_hash_engine_read_config() must be previously called
 */
const struct sx_hash_engine_config* sx_hash_engine_get_config(void);

#define BA413_HW_CFG_MD5_SUPPORTED (sx_hash_engine_get_config()->is_md5_enabled)
#define BA413_HW_CFG_SHA1_SUPPORTED (sx_hash_engine_get_config()->is_sha1_enabled)
#define BA413_HW_CFG_SHA224_SUPPORTED (sx_hash_engine_get_config()->is_sha224_enabled)
#define BA413_HW_CFG_SHA256_SUPPORTED (sx_hash_engine_get_config()->is_sha256_enabled)
#define BA413_HW_CFG_SHA384_SUPPORTED (sx_hash_engine_get_config()->is_sha384_enabled)
#define BA413_HW_CFG_SHA512_SUPPORTED (sx_hash_engine_get_config()->is_sha512_enabled)
#define BA413_HW_CFG_SM3_SUPPORTED (sx_hash_engine_get_config()->is_sm3_enabled)
#define BA413_HW_CFG_PADDING_SUPPORTED (sx_hash_engine_get_config()->is_padding_enabled)
#define BA413_HW_CFG_HMAC_SUPPORTED (sx_hash_engine_get_config()->is_hmac_enabled)
#define BA413_HW_CFG_VERIFY_DIGEST_SUPPORTED (sx_hash_engine_get_config()->is_verify_digest_enabled)

#else

#define BA413_HW_CFG_MD5_SUPPORTED (BA413_HW_CFG_MD5_SUPPORTED_MASK & BA413_HW_CFG)
#define BA413_HW_CFG_SHA1_SUPPORTED (BA413_HW_CFG_SHA1_SUPPORTED_MASK & BA413_HW_CFG)
#define BA413_HW_CFG_SHA224_SUPPORTED (BA413_HW_CFG_SHA224_SUPPORTED_MASK & BA413_HW_CFG)
#define BA413_HW_CFG_SHA256_SUPPORTED (BA413_HW_CFG_SHA256_SUPPORTED_MASK & BA413_HW_CFG)
#define BA413_HW_CFG_SHA384_SUPPORTED (BA413_HW_CFG_SHA384_SUPPORTED_MASK & BA413_HW_CFG)
#define BA413_HW_CFG_SHA512_SUPPORTED (BA413_HW_CFG_SHA512_SUPPORTED_MASK & BA413_HW_CFG)
#define BA413_HW_CFG_SM3_SUPPORTED (BA413_HW_CFG_SM3_SUPPORTED_MASK & BA413_HW_CFG)
#define BA413_HW_CFG_PADDING_SUPPORTED (BA413_HW_CFG_PADDING_SUPPORTED_MASK & BA413_HW_CFG)
#define BA413_HW_CFG_HMAC_SUPPORTED (BA413_HW_CFG_HMAC_SUPPORTED_MASK & BA413_HW_CFG)
#define BA413_HW_CFG_VERIFY_DIGEST_SUPPORTED (BA413_HW_CFG_VERIFY_DIGEST_SUPPORTED_MASK & BA413_HW_CFG)

#endif

#endif
