/* Copyright Statement:
 *
 * (C) 2005-2022  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_DES_H__
#define __HAL_DES_H__
#include "hal_platform.h"

#ifdef HAL_DES_MODULE_ENABLED

#ifdef HAL_DES_USE_PHYSICAL_MEMORY_ADDRESS
/**
 * @addtogroup HAL
 * @{
 * @addtogroup DES
 * @{
 * This section introduces the DES driver APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, DES function groups, enums, structures and functions.
 *
 * @section HAL_DES_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DES                        | The Data Encryption Standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Data_Encryption_Standard"> introduction to the DES in Wikipedia </a>.|
 * |\b CBC                        | The Cipher Block Chaining. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation"> introduction to the CBC in Wikipedia </a>.|
 * |\b ECB                        | The Electronic Codebook. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation"> introduction to the ECB in Wikipedia </a>.|
 *
 * @section HAL_DES_Features_Chapter Supported features
 * MediaTek MT7933 has two encryption modes in DES, CBC mode and ECB mode. In the CBC mode, each block of encryption mode is XOR'ed with the previous cipher block before being encrypted,
 * Each cipher text block depends on all plain text blocks processed up to that point. In ECB mode, the message is divided into blocks, and each block is encrypted separately.
 * - \b Support \b CBC \b and \b ECB \b modes. \n
 *   Each mode has different hardware and software configuration. Plain text date length should be multiples of 16.
 *
 * @section HAL_DES_Driver_Usage_Chapter How to use this driver
 *
 * - Use DES in the CBC mode to encrypt and decrypt. \n
 *   To use the DES driver, allocate a buffer in physical memory region, e.g. TCM, SYSRAM and PSRAM.
 *  - Step 1. Call #hal_des_cbc_encrypt() to encrypt.
 *  - Step 2. Call #hal_des_cbc_decrypt() to decrypt.
 *  - sample code:
 *    @code
 *
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t des_cbc_iv[HAL_DES_CBC_IV_LENGTH] = {
 *           0x61, 0x33, 0x46, 0x68, 0x55, 0x38, 0x31, 0x43
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t encrypted[32] = {0};
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t plain[30] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66,
 *           77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t encrypted_text = {
 *           .buffer = encrypted,
 *           .length = sizeof(encrypted)
 *       };
 *       hal_des_cbc_encrypt(&encrypted_text, &plain_text, &key, des_cbc_iv);
 *
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t decrypted_buffer[32] = {0};
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_aes_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_des_cbc_decrypt(&decrypted_text, &encrypted_text, &key, des_cbc_iv);
 *
 *    @endcode
 * - Use DES in the ECB mode to encrypt and decrypt. \n
 *   To use the DES driver, allocate a buffer in physical memory region, e.g. TCM, SYSRAM and PSRAM.
 *  - Step 1. call #hal_des_ecb_encrypt() to encrypt.
 *  - Step 2. call #hal_des_ecb_decrypt() to decrypt.
 *  - sample code:
 *    @code
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t hardware_id[8] = {
 *           0x4d, 0x54, 0x4b, 0x30, 0x30, 0x30, 0x30, 0x30
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t plain[] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
 *           66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t encrypted_buffer[32] = {0};
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t encrypted_text = {
 *           .buffer = encrypted_buffer,
 *           .length = sizeof(encrypted_buffer)
 *       };
 *       hal_des_ecb_encrypt(&encrypted_text, &plain_text, &key);
 *
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t decrypted_buffer[32] = {0};
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_des_ecb_decrypt(&decrypted_text, &encrypted_text, &key);
 *
 *    @endcode
 *
 *
 *
 */
#else /* #ifdef HAL_DES_USE_PHYSICAL_MEMORY_ADDRESS */
/**
 * @addtogroup HAL
 * @{
 * @addtogroup DES
 * @{
 * This section introduces the DES driver APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, DES function groups, enums, structures and functions.
 *
 * @section HAL_DES_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DES                        | The Data Encryption Standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Data_Encryption_Standard"> introduction to the DES in Wikipedia </a>.|
 * |\b CBC                        | The Cipher Block Chaining. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation"> introduction to the CBC in Wikipedia </a>.|
 * |\b ECB                        | The Electronic Codebook. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation"> introduction to the ECB in Wikipedia </a>.|
 *
 * @section HAL_DES_Features_Chapter Supported features
 * MediaTek MT7933 has two encryption modes in DES, CBC mode and ECB mode. In the CBC mode, each block of encryption mode is XOR'ed with the previous cipher block before being encrypted,
 * Each cipher text block depends on all plain text blocks processed up to that point. In ECB mode, the message is divided into blocks, and each block is encrypted separately.
 * - \b Support \b CBC \b and \b ECB \b modes. \n
 *   Each mode has different hardware and software configuration. Plain text date length should be multiples of 16.
 *
 * @section HAL_DES_Driver_Usage_Chapter How to use this driver
 *
 * - Use DES in the CBC mode to encrypt and decrypt. \n
 *  - Step 1. Call #hal_des_cbc_encrypt() to encrypt.
 *  - Step 2. Call #hal_des_cbc_decrypt() to decrypt.
 *  - sample code:
 *    @code
 *
 *       uint8_t des_cbc_iv[HAL_DES_CBC_IV_LENGTH] = {
 *           0x61, 0x33, 0x46, 0x68, 0x55, 0x38, 0x31, 0x43
 *       };
 *       uint8_t encrypted[32] = {0};
 *       uint8_t plain[30] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66,
 *           77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       hal_des_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       hal_des_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       hal_des_buffer_t encrypted_text = {
 *           .buffer = encrypted,
 *           .length = sizeof(encrypted)
 *       };
 *       hal_des_cbc_encrypt(&encrypted_text, &plain_text, &key, des_cbc_iv);
 *
 *       uint8_t decrypted_buffer[32] = {0};
 *       hal_aes_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_des_cbc_decrypt(&decrypted_text, &encrypted_text, &key, des_cbc_iv);
 *
 *    @endcode
 * - Use DES in the ECB mode to encrypt and decrypt. \n
 *  - Step 1. call #hal_des_ecb_encrypt() to encrypt.
 *  - Step 2. call #hal_des_ecb_decrypt() to decrypt.
 *  - sample code:
 *    @code
 *       uint8_t hardware_id[8] = {
 *           0x4d, 0x54, 0x4b, 0x30, 0x30, 0x30, 0x30, 0x30
 *       };
 *       uint8_t plain[] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
 *           66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       hal_des_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       hal_des_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       uint8_t encrypted_buffer[32] = {0};
 *       hal_des_buffer_t encrypted_text = {
 *           .buffer = encrypted_buffer,
 *           .length = sizeof(encrypted_buffer)
 *       };
 *       hal_des_ecb_encrypt(&encrypted_text, &plain_text, &key);
 *
 *       uint8_t decrypted_buffer[32] = {0};
 *       hal_des_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_des_ecb_decrypt(&decrypted_text, &encrypted_text, &key);
 *
 *    @endcode
 *
 *
 *
 */
#endif /* #ifdef HAL_DES_USE_PHYSICAL_MEMORY_ADDRESS */

#include "hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/** @defgroup hal_des_define Define
  * @{
  */

/** @brief This macro defines the block size of DES operation. The unit is in bytes.
  */
#define HAL_DES_BLOCK_SIZES      (8)

/** @brief This macro defines the key length of DES 64 bits. The unit is in bytes.
  */
#define HAL_DES_KEY_LENGTH_64    (8)

/** @brief This macro defines the key length of DES 128 bits. The unit is in bytes.
  */
#define HAL_DES_KEY_LENGTH_128   (16)

/** @brief This macro defines the key length of DES 192 bits. The unit is in bytes.
  */
#define HAL_DES_KEY_LENGTH_192   (24)

/** @brief This macro defines the size of initial vector. The unit is in bytes.
  */
#define HAL_DES_CBC_IV_LENGTH    (8)

/**
  * @}
  */


/** @defgroup hal_des_enum Enum
  * @{
  */

/** @brief This enum defines the HAL interface return value.
  */
typedef enum {
    HAL_DES_STATUS_ERROR = -1,      /**< An error occurred. */
    HAL_DES_STATUS_OK = 0           /**< No error occurred. */
} hal_des_status_t;

/**
  * @}
  */

/** @defgroup hal_des_struct Struct
  * @{
  */

/** @brief This structure defines the buffer used in the operation. */
typedef struct {
    uint8_t *buffer;  /**< Data buffer. */
    uint32_t length;  /**< Data length. */
} hal_des_buffer_t;

/**
  * @}
  */



/**
 * @brief     DES encryption in the CBC mode.
 * @param[out] encrypted_text is the target encrypted text.
 * @param[in]  plain_text is the source plain text.
 * @param[in]  key is the DES key for encryption.
 * @param[in]  init_vector is the DES initialization vector for encryption.
 * @return     #HAL_DES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_DES_Driver_Usage_Chapter
 *
 */
hal_des_status_t hal_des_cbc_encrypt(hal_des_buffer_t *encrypted_text,
                                     hal_des_buffer_t *plain_text,
                                     hal_des_buffer_t *key,
                                     uint8_t init_vector[HAL_DES_CBC_IV_LENGTH]);

/**
 * @brief     DES decryption in the CBC mode.
 * @param[out] plain_text is the target plain text.
 * @param[in]  encrypted_text is the source encrypted text.
 * @param[in]  key is the DES key for encryption.
 * @param[in]  init_vector is the DES initialization vector for encryption.
 * @return     #HAL_DES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_DES_Driver_Usage_Chapter
 *
 */
hal_des_status_t hal_des_cbc_decrypt(hal_des_buffer_t *plain_text,
                                     hal_des_buffer_t *encrypted_text,
                                     hal_des_buffer_t *key,
                                     uint8_t init_vector[HAL_DES_CBC_IV_LENGTH]);


/**
 * @brief     DES encryption in the ECB mode.
 * @param[out] encrypted_text is the target encrypted text.
 * @param[in]  plain_text is the source plain text.
 * @param[in]  key is the DES key for encryption.
 * @return     #HAL_DES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_DES_Driver_Usage_Chapter
 *
 */
hal_des_status_t hal_des_ecb_encrypt(hal_des_buffer_t *encrypted_text,
                                     hal_des_buffer_t *plain_text,
                                     hal_des_buffer_t *key);

/**
 * @brief     DES decryption in the ECB mode.
 * @param[out] plain_text is the target plain text.
 * @param[in]  encrypted_text is the source encrypted text.
 * @param[in]  key is the DES key for encryption.
 * @return     #HAL_DES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_DES_Driver_Usage_Chapter
 *
 */
hal_des_status_t hal_des_ecb_decrypt(hal_des_buffer_t *plain_text,
                                     hal_des_buffer_t *encrypted_text,
                                     hal_des_buffer_t *key);



#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/**
* @}
* @}
*/
#endif /* #ifdef HAL_DES_MODULE_ENABLED */
#endif /* #ifndef __HAL_DES_H__ */
