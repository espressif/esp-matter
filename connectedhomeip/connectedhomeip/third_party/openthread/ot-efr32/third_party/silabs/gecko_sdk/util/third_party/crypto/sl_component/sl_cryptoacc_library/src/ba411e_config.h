/**
 * @file
 * @brief Defines macros to be used for the configuration of the BA411E AES
 * @copyright Copyright (c) 2016-2020 Silex Insight. All Rights reserved
 */


#ifndef BA411E_CONFIG_H
#define BA411E_CONFIG_H
#include <stdint.h>
#include <stdbool.h>

#include "sx_blk_cipher_common.h"
#include "sx_blk_cipher.h"

/** @brief BA411E Mode Register mask for hardware key 1 & 2*/
#define AES_MODEID_KEYX_MASK     0x000000C0

/* AES hardware configuration - register 1*/
#define AES_HW_CFG_ECB_SUPPORTED_LSB      0
#define AES_HW_CFG_ECB_SUPPORTED_MASK     (1L<<AES_HW_CFG_ECB_SUPPORTED_LSB)
#define AES_HW_CFG_CBC_SUPPORTED_LSB      1
#define AES_HW_CFG_CBC_SUPPORTED_MASK     (1L<<AES_HW_CFG_CBC_SUPPORTED_LSB)
#define AES_HW_CFG_CTR_SUPPORTED_LSB      2
#define AES_HW_CFG_CTR_SUPPORTED_MASK     (1L<<AES_HW_CFG_CTR_SUPPORTED_LSB)
#define AES_HW_CFG_CFB_SUPPORTED_LSB      3
#define AES_HW_CFG_CFB_SUPPORTED_MASK     (1L<<AES_HW_CFG_CFB_SUPPORTED_LSB)
#define AES_HW_CFG_OFB_SUPPORTED_LSB      4
#define AES_HW_CFG_OFB_SUPPORTED_MASK     (1L<<AES_HW_CFG_OFB_SUPPORTED_LSB)
#define AES_HW_CFG_CCM_SUPPORTED_LSB      5
#define AES_HW_CFG_CCM_SUPPORTED_MASK     (1L<<AES_HW_CFG_CCM_SUPPORTED_LSB)
#define AES_HW_CFG_GCM_SUPPORTED_LSB      6
#define AES_HW_CFG_GCM_SUPPORTED_MASK     (1L<<AES_HW_CFG_GCM_SUPPORTED_LSB)
#define AES_HW_CFG_XTS_SUPPORTED_LSB      7
#define AES_HW_CFG_XTS_SUPPORTED_MASK     (1L<<AES_HW_CFG_XTS_SUPPORTED_LSB)
#define AES_HW_CFG_CMAC_SUPPORTED_LSB     8
#define AES_HW_CFG_CMAC_SUPPORTED_MASK    (1L<<AES_HW_CFG_CMAC_SUPPORTED_LSB)
#define AES_HW_CFG_CS_EN_LSB              16 /* Ciphertext Stealing */
#define AES_HW_CFG_CS_EN_MASK             (1L<<AES_HW_CFG_CS_EN_LSB)
#define AES_HW_CFG_CM_EN_LSB              17 /* Countermeasures */
#define AES_HW_CFG_CM_EN_MASK             (1L<<AES_HW_CFG_CM_EN_LSB)
#define AES_HW_CFG_KEY_SIZE_LSB           24
#define AES_HW_CFG_KEY_SIZE_MASK          (0x7<<AES_HW_CFG_KEY_SIZE_LSB)
#define AES_HW_CFG_KEY_SIZE_128_SUPPORTED_LSB 24
#define AES_HW_CFG_KEY_SIZE_128_SUPPORTED_MASK (1L<<AES_HW_CFG_KEY_SIZE_128_SUPPORTED_LSB)
#define AES_HW_CFG_KEY_SIZE_192_SUPPORTED_LSB 25
#define AES_HW_CFG_KEY_SIZE_192_SUPPORTED_MASK (1L<<AES_HW_CFG_KEY_SIZE_192_SUPPORTED_LSB)
#define AES_HW_CFG_KEY_SIZE_256_SUPPORTED_LSB 26
#define AES_HW_CFG_KEY_SIZE_256_SUPPORTED_MASK (1L<<AES_HW_CFG_KEY_SIZE_256_SUPPORTED_LSB)

#define BA411E_HW_CFG_1 (*(const volatile uint32_t*)ADDR_BA411E_HW_CFG_1)

/* AES hardware configuration - register 2*/
#define AES_HW_CFG_MAX_CTR_SIZE_LSB       0
#define AES_HW_CFG_MAX_CTR_SIZE_MASK      (0xFFFF<<AES_HW_CFG_MAX_CTR_SIZE_LSB)

#define BA411E_HW_CFG_2 (*(const volatile uint32_t*)ADDR_BA411E_HW_CFG_2)

#ifdef CACHED_HW_CONFIG

/** This structure hosts a copy of the HW configuration registers */
struct sx_aes_engine_config {
   bool is_config_read; /**< is set the first time we retrieve the hardware
                             configuration from the engine */
   bool is_engine_available;
   bool is_ecb_supported; /**< Indicates if the ECB mode is enabled */
   bool is_cbc_supported; /**< Indicates if the CBC mode is enabled */
   bool is_ctr_supported; /**< Indicates if the CTR mode is enabled */
   bool is_cfb_supported; /**< Indicates if the CFB mode is enabled */
   bool is_ofb_supported; /**< Indicates if the OFB mode is enabled */
   bool is_gcm_supported; /**< Indicates if the GCM mode is enabled */
   bool is_ccm_supported; /**< Indicates if the CCM mode is enabled */
   bool is_xts_supported; /**< Indicates if the XTS mode is enabled */
   bool is_cmac_supported; /**< Indicates if the CMAC mode is enabled */
   bool is_cipher_stealing_enabled; /**< Indicates if cipher stealing is
                                         enabled for CBC and XTS, allowing last
                                         data block to not be aligned on 128b */
   bool is_coutermeasure_enabled; /**< Indicates if countermeasure is enabled */
   bool is_key_128b_supported; /**< Indicates if 128b keys are supported */
   bool is_key_192b_supported; /**< Indicates if 192b key are supported */
   bool is_key_256b_supported; /**< Indicates if 256b key are supported */
   uint32_t n_bits_counter_modes; /**< size in bits of counter used in CTR/CCM */
};

/** @brief Reads and returns the HW configuration of the AES engine. */
const struct sx_aes_engine_config* sx_aes_engine_read_config(void);
/** @brief Returns the HW configuration, assuming it is already read
 * @warning sx_aes_engine_read_config() must be previously called
 */
const struct sx_aes_engine_config* sx_aes_engine_get_config(void);

#define AES_HW_CFG_ECB_SUPPORTED (sx_aes_engine_get_config()->is_ecb_supported)
#define AES_HW_CFG_CBC_SUPPORTED (sx_aes_engine_get_config()->is_cbc_supported)
#define AES_HW_CFG_CTR_SUPPORTED (sx_aes_engine_get_config()->is_ctr_supported)
#define AES_HW_CFG_CFB_SUPPORTED (sx_aes_engine_get_config()->is_cfb_supported)
#define AES_HW_CFG_OFB_SUPPORTED (sx_aes_engine_get_config()->is_ofb_supported)
#define AES_HW_CFG_CCM_SUPPORTED (sx_aes_engine_get_config()->is_ccm_supported)
#define AES_HW_CFG_GCM_SUPPORTED (sx_aes_engine_get_config()->is_gcm_supported)
#define AES_HW_CFG_XTS_SUPPORTED (sx_aes_engine_get_config()->is_xts_supported)
#define AES_HW_CFG_CMAC_SUPPORTED (sx_aes_engine_get_config()->is_cmac_supported)
#define AES_HW_CFG_CS_EN (sx_aes_engine_get_config()->is_cipher_stealing_enabled)
#define AES_HW_CFG_CM_EN (sx_aes_engine_get_config()->is_coutermeasure_enabled)
#define AES_HW_CFG_KEY_SIZE_128_SUPPORTED (sx_aes_engine_get_config()->is_key_128b_supported)
#define AES_HW_CFG_KEY_SIZE_192_SUPPORTED (sx_aes_engine_get_config()->is_key_192b_supported)
#define AES_HW_CFG_KEY_SIZE_256_SUPPORTED (sx_aes_engine_get_config()->is_key_256b_supported)

#else

#define AES_HW_CFG_ECB_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_ECB_SUPPORTED_MASK)
#define AES_HW_CFG_CBC_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_CBC_SUPPORTED_MASK)
#define AES_HW_CFG_CTR_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_CTR_SUPPORTED_MASK)
#define AES_HW_CFG_CFB_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_CFB_SUPPORTED_MASK)
#define AES_HW_CFG_OFB_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_OFB_SUPPORTED_MASK)
#define AES_HW_CFG_CCM_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
#define AES_HW_CFG_GCM_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
#define AES_HW_CFG_XTS_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_XTS_SUPPORTED_MASK)
#define AES_HW_CFG_CMAC_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_CMAC_SUPPORTED_MASK)
#define AES_HW_CFG_CS_EN (BA411E_HW_CFG_1 & AES_HW_CFG_CS_EN_MASK)
#define AES_HW_CFG_CM_EN (BA411E_HW_CFG_1 & AES_HW_CFG_CM_EN_MASK)
#define AES_HW_CFG_KEY_SIZE_128_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_KEY_SIZE_128_SUPPORTED_MASK)
#define AES_HW_CFG_KEY_SIZE_192_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_KEY_SIZE_192_SUPPORTED_MASK)
#define AES_HW_CFG_KEY_SIZE_256_SUPPORTED (BA411E_HW_CFG_1 & AES_HW_CFG_KEY_SIZE_256_SUPPORTED_MASK)

#endif
#endif