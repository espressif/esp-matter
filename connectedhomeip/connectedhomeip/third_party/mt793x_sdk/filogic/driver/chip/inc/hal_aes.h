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

#ifndef __HAL_AES_H__
#define __HAL_AES_H__
#include "hal_platform.h"

#ifdef HAL_AES_MODULE_ENABLED

#ifdef HAL_AES_USE_PHYSICAL_MEMORY_ADDRESS
/**
 * @addtogroup HAL
 * @{
 * @addtogroup AES
 * @{
 * This section introduces the AES driver APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, AES function groups, enums, structures and functions.
 *
 * @section HAL_AES_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b AES                        | The Advanced Encryption Standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Advanced_Encryption_Standard"> introduction to the AES in Wikipedia</a>.|
 * |\b CBC                        | The Cipher Block Chaining(CBC), each block of plain text is XOR'ed with the previous cipher block before being encrypted. This way, each cipher text block depends on all plain text blocks processed up to that point.
 * |\b ECB                        | Electronic Codebook (ECB) mode. The message is divided into blocks, and each block is encrypted separately.
 *
 * @section HAL_AES_Features_Chapter Supported features
 *
 * - \b Support \b CBC \b and \b ECB \b and \b CTR \b and \b GCM \b modes. \n
 *   Each mode has different hardware and software configuration. Plain text date length should be multiples of 16.
 *
 * @section HAL_AES_Driver_Usage_Chapter How to use this driver
 *
 * - Use the AES in CBC mode to encrypt and decrypt. \n
 *   To use the AES driver, the caller should allocate a buffer in physical memory region, e.g. TCM, SYSRAM and PSRAM.
 *  - Step 1. Call #hal_aes_cbc_encrypt() to encrypt.
 *  - Step 2. Call #hal_aes_cbc_decrypt() to decrypt.
 *  - sample code:
 *    @code
 *
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t aes_cbc_iv[HAL_AES_CBC_IV_LENGTH] = {
 *           0x61, 0x33, 0x46, 0x68, 0x55, 0x38, 0x31, 0x43,
 *           0x77, 0x68, 0x36, 0x33, 0x50, 0x76, 0x33, 0x46
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t plain[] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
 *           66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_aes_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_aes_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t encrypted_buffer[32] = {0};
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_aes_buffer_t encrypted_text = {
 *           .buffer = encrypted_buffer,
 *           .length = sizeof(encrypted_buffer)
 *       };
 *       hal_aes_cbc_encrypt(&encrypted_text, &plain_text, &key, aes_cbc_iv);
 *
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t decrypted_buffer[32] = {0};
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_aes_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_aes_cbc_decrypt(&decrypted_text, &encrypted_text, &key, aes_cbc_iv);
 *
 *    @endcode
 * - Use the AES in ECB mode to encrypt and decrypt. \n
 *   To use the AES driver, the caller should allocate a buffer in physical memory region, e.g. TCM, SYSRAM and PSRAM.
 *  - Step 1. Call #hal_aes_ecb_encrypt() to encrypt.
 *  - Step 2. Call #hal_aes_ecb_decrypt() to decrypt.
 *  - sample code:
 *    @code
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t hardware_id[16] = {
 *           0x4d, 0x54, 0x4b, 0x30, 0x30, 0x30, 0x30, 0x30,
 *           0x32, 0x30, 0x31, 0x34, 0x30, 0x38, 0x31, 0x35
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t plain[] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
 *           66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_aes_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_aes_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t encrypted_buffer[32] = {0};
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_aes_buffer_t encrypted_text = {
 *           .buffer = encrypted_buffer,
 *           .length = sizeof(encrypted_buffer)
 *       };
 *       hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);
 *
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t decrypted_buffer[32] = {0};
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_aes_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_aes_ecb_decrypt(&decrypted_text, &encrypted_text, &key);
 *
 *    @endcode
 *
 *
 *
 */
#else /* #ifdef HAL_AES_USE_PHYSICAL_MEMORY_ADDRESS */
/**
 * @addtogroup HAL
 * @{
 * @addtogroup AES
 * @{
 * This section introduces the AES driver APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, AES function groups, enums, structures and functions.
 *
 * @section HAL_AES_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b AES                        | The Advanced Encryption Standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Advanced_Encryption_Standard"> introduction to the AES in Wikipedia</a>.|
 * |\b CBC                        | The Cipher Block Chaining(CBC), each block of plain text is XOR'ed with the previous cipher block before being encrypted. This way, each cipher text block depends on all plain text blocks processed up to that point.
 * |\b ECB                        | Electronic Codebook (ECB) mode. The message is divided into blocks, and each block is encrypted separately.
 *
 * @section HAL_AES_Features_Chapter Supported features
 *
 * - \b Support \b CBC \b and \b ECB \b and \b CTR \b and \b GCM \b modes. \n
 *   Each mode has different hardware and software configuration. Plain text date length should be multiples of 16.
 *
 * @section HAL_AES_Driver_Usage_Chapter How to use this driver
 *
 * - Use the AES in CBC mode to encrypt and decrypt. \n
 *  - Step 1. Call #hal_aes_cbc_encrypt() to encrypt.
 *  - Step 2. Call #hal_aes_cbc_decrypt() to decrypt.
 *  - sample code:
 *    @code
 *
 *       uint8_t aes_cbc_iv[HAL_AES_CBC_IV_LENGTH] = {
 *           0x61, 0x33, 0x46, 0x68, 0x55, 0x38, 0x31, 0x43,
 *           0x77, 0x68, 0x36, 0x33, 0x50, 0x76, 0x33, 0x46
 *       };
 *       uint8_t plain[] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
 *           66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       hal_aes_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       hal_aes_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       uint8_t encrypted_buffer[32] = {0};
 *       hal_aes_buffer_t encrypted_text = {
 *           .buffer = encrypted_buffer,
 *           .length = sizeof(encrypted_buffer)
 *       };
 *       hal_aes_cbc_encrypt(&encrypted_text, &plain_text, &key, aes_cbc_iv);
 *
 *       uint8_t decrypted_buffer[32] = {0};
 *       hal_aes_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_aes_cbc_decrypt(&decrypted_text, &encrypted_text, &key, aes_cbc_iv);
 *
 *    @endcode
 * - Use the AES in ECB mode to encrypt and decrypt. \n
 *  - Step 1. Call #hal_aes_ecb_encrypt() to encrypt.
 *  - Step 2. Call #hal_aes_ecb_decrypt() to decrypt.
 *  - sample code:
 *    @code
 *       uint8_t hardware_id[16] = {
 *           0x4d, 0x54, 0x4b, 0x30, 0x30, 0x30, 0x30, 0x30,
 *           0x32, 0x30, 0x31, 0x34, 0x30, 0x38, 0x31, 0x35
 *       };
 *       uint8_t plain[] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
 *           66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       hal_aes_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       hal_aes_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       uint8_t encrypted_buffer[32] = {0};
 *       hal_aes_buffer_t encrypted_text = {
 *           .buffer = encrypted_buffer,
 *           .length = sizeof(encrypted_buffer)
 *       };
 *       hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);
 *
 *       uint8_t decrypted_buffer[32] = {0};
 *       hal_aes_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_aes_ecb_decrypt(&decrypted_text, &encrypted_text, &key);
 *
 *    @endcode
 * - Use the AES in a CBC mode with key1/2 in the EFUSE to encrypt and decrypt. \n
 *  - Step1: Call #hal_aes_cbc_encrypt_ex() to encrypt.
 *  - Step2: Call #hal_aes_cbc_decrypt_ex() to decrypt.
 *  - sample code:
 *    @code
 *
 *       uint8_t aes_cbc_iv[HAL_AES_CBC_IV_LENGTH] = {
 *           0x61, 0x33, 0x46, 0x68, 0x55, 0x38, 0x31, 0x43,
 *           0x77, 0x68, 0x36, 0x33, 0x50, 0x76, 0x33, 0x46
 *       };
 *       uint8_t plain[] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
 *           66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       uint8_t hardware_id[16] = {
 *           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 *           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
 *       };
 *       hal_aes_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       hal_aes_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       uint8_t encrypted_buffer[32] = {0};
 *       hal_aes_buffer_t encrypted_text = {
 *           .buffer = encrypted_buffer,
 *           .length = sizeof(encrypted_buffer)
 *       };
 *       hal_aes_cbc_encrypt_ex(&encrypted_text, &plain_text, &key, aes_cbc_iv, HAL_AES_EFUSE_KEY1);
 *
 *       uint8_t decrypted_buffer[32] = {0};
 *       hal_aes_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_aes_cbc_decrypt_ex(&decrypted_text, &encrypted_text, &key, aes_cbc_iv, HAL_AES_EFUSE_KEY1);
 *
 *    @endcode
 * - Use the AES in a ECB mode with key1/2 in the EFUSE to encrypt and decrypt. \n
 *  - Step1: Call #hal_aes_ecb_encrypt_ex() to encrypt.
 *  - Step2: Call #hal_aes_ecb_decrypt_ex() to decrypt.
 *  - sample code:
 *    @code
 *       uint8_t hardware_id[16] = {
 *           0x4d, 0x54, 0x4b, 0x30, 0x30, 0x30, 0x30, 0x30,
 *           0x32, 0x30, 0x31, 0x34, 0x30, 0x38, 0x31, 0x35
 *       };
 *       uint8_t plain[] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
 *           66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       uint8_t hardware_id[16] = {
 *           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 *           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
 *       };
 *       hal_aes_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       hal_aes_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       uint8_t encrypted_buffer[32] = {0};
 *       hal_aes_buffer_t encrypted_text = {
 *           .buffer = encrypted_buffer,
 *           .length = sizeof(encrypted_buffer)
 *       };
 *       hal_aes_ecb_encrypt_ex(&encrypted_text, &plain_text, &key, HAL_AES_EFUSE_KEY1);
 *
 *       uint8_t decrypted_buffer[32] = {0};
 *       hal_aes_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_aes_ecb_decrypt_ex(&decrypted_text, &encrypted_text, &key, HAL_AES_EFUSE_KEY1);
 *
 *    @endcode
 *
 *
 */
#endif /* #ifdef HAL_AES_USE_PHYSICAL_MEMORY_ADDRESS */


#include "hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


/** @defgroup hal_aes_define Define
  * @{
  */

/** @brief This macro defines the block size of AES operation. The unit is in bytes.
  */
#define HAL_AES_BLOCK_SIZES       (16)

/** @brief This macro defines the key length of AES 128 bits. The unit is in bytes.
  */
#define HAL_AES_KEY_LENGTH_128    (16)

/** @brief This macro defines the key length of AES 192 bits. The unit is in bytes.
  */
#define HAL_AES_KEY_LENGTH_192    (24)

/** @brief This macro defines the key length of AES 256 bits. The unit is in bytes.
  */
#define HAL_AES_KEY_LENGTH_256    (32)

/** @brief This macro defines the size of initial vector. The unit is in bytes.
  */
#define HAL_AES_CBC_IV_LENGTH     (16)

#define HAL_AES_BUFFER_SIZE       (128)
/**
  * @}
  */


/** @defgroup hal_aes_enum Enum
  * @{
  */

/** @brief This enum defines the HAL interface return value.
  */
typedef enum {
    HAL_AES_STATUS_ERROR = -1,      /**< An error occurred. */
    HAL_AES_STATUS_OK = 0           /**< No error occurred. */
} hal_aes_status_t;

/** @brief This enum defines the EFUSE key type.
  */
typedef enum {
    HAL_AES_EFUSE_KEY1 = 0,        /**< Key1 */
    HAL_AES_EFUSE_KEY2 = 1         /**< Key2 */
} hal_aes_efuse_key_t;

/**
  * @}
  */

/** @defgroup hal_aes_struct Struct
  * @{
  */

/** @brief This structure defines the buffer used in the operation. */
typedef struct {
    uint8_t *buffer;  /**< Data buffer. */
    uint32_t length;  /**< Data length. */
} hal_aes_buffer_t;

/**
  * @}
  */



/**
 * @brief     This function provides AES encryption in CBC mode.
 * @param[out] encrypted_text is the target encrypted text.
 * @param[in]  plain_text is the source plain text.
 * @param[in]  key is the AES encryption key.
 * @param[in]  init_vector is the AES initialzation vector for encryption.
 * @return     #HAL_AES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_cbc_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH]);

/**
 * @brief     This function provides AES encryption in CBC mode.
 * @param[out] encrypted_text is the target encrypted text.
 * @param[in]  plain_text is the source plain text.
 * @param[in]  key is the AES encryption key.
 * @param[out]  init_vector is the AES initialzation vector for encryption.
 * @return     #HAL_AES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_cbc_encrypt_iteration(hal_aes_buffer_t *encrypted_text,
                                               hal_aes_buffer_t *plain_text,
                                               hal_aes_buffer_t *key,
                                               uint8_t          *init_vector);

/**
 * @brief     This function provides AES decryption in CBC mode.
 * @param[out] plain_text is the target plain text.
 * @param[in]  encrypted_text is the source encrypted text.
 * @param[in]  key is the AES encryption key.
 * @param[in]  init_vector is the AES initialization vector for encryption.
 * @return     #HAL_AES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_cbc_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH]);

/**
 * @brief     This function provides AES decryption in CBC mode.
 * @param[out] plain_text is the target plain text.
 * @param[in]  encrypted_text is the source encrypted text.
 * @param[in]  key is the AES encryption key.
 * @param[out]  init_vector is the AES initialization vector for encryption.
 * @return     #HAL_AES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_cbc_decrypt_iteration(hal_aes_buffer_t *plain_text,
                                               hal_aes_buffer_t *encrypted_text,
                                               hal_aes_buffer_t *key,
                                               uint8_t          *init_vector);


/**
 * @brief     This function provides AES encryption in CTR (enc/dec) mode.
 * @param[out] encrypted_text is the target encrypted text or target plain text.
 * @param[in]  plain_text is the source plain text to encrypt or source cipher text to decrypt.
 * @param[in]  key is the AES encryption key.
 * @param[in]  init_vector is the AES initialzation vector for encryption.
 * @return     #HAL_AES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_ctr(hal_aes_buffer_t *encrypted_text,
                             hal_aes_buffer_t *plain_text,
                             hal_aes_buffer_t *key,
                             uint8_t init_vector[HAL_AES_CBC_IV_LENGTH]);


/**
 * @brief     This function provides AES encryption in ECB mode.
 * @param[out] encrypted_text is the target encrypted text.
 * @param[in]  plain_text is the source plain text.
 * @param[in]  key is the AES encryption key.
 * @return     #HAL_AES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_ecb_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key);

/**
 * @brief     This function provides AES decryption in ECB mode.
 * @param[out] plain_text is the target plain text.
 * @param[in]  encrypted_text is the source encrypted text.
 * @param[in]  key is the AES encryption key.
 * @return     #HAL_AES_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_ecb_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key);


/**
 * @brief     This function provides an AES encryption in a CBC mode with using
 *            hardware encryption key in EFUSE.
 * @param[out] encrypted_text is the target encrypted text.
 * @param[in]  plain_text is the source plain text.
 * @param[in]  key is used to pass the key length information which is key.length and key.buffer is not used.
 * @param[in]  init_vector is the AES initialzation vector for encryption.
 * @param[in]  key_index is the index to specify which key slot is used, 0 and 1 are expected index.
 * @return     if successful, returns #HAL_AES_STATUS_OK
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_cbc_encrypt_ex(hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *key,
                                        uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
                                        hal_aes_efuse_key_t key_index);

/**
 * @brief     This function provides an AES decryption in a CBC mode with using
 *            hardware encryption key in EFUSE.
 * @param[out] plain_text is the target plain text.
 * @param[in]  encrypted_text is the source encrypted text.
 * @param[in]  key is used to pass the key length information which is key.length and key.buffer is not used.
 * @param[in]  init_vector is the AES initialization vector for encryption.
 * @param[in]  key_index is the index to specify which key slot is used, 0 and 1 are expected index.
 * @return if successful, returns #HAL_AES_STATUS_OK
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_cbc_decrypt_ex(hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *key,
                                        uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
                                        hal_aes_efuse_key_t key_index);

/**
 * @brief     This function provides an AES encryption in a ECB mode with using
 *            hardware encryption key in EFUSE.
 * @param[out] encrypted_text is the target encrypted text.
 * @param[in]  plain_text is the source plain text.
 * @param[in]  key is used to pass the key length information which is key.length and key.buffer is not used.
 * @param[in]  key_index is the index to specify which key slot is used, 0 and 1 are expected index.
 * @return if successful, returns #HAL_AES_STATUS_OK
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_ecb_encrypt_ex(hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *key,
                                        hal_aes_efuse_key_t key_index);

/**
 * @brief     This function provides an AES decryption in a ECB mode with using
 *            hardware encryption key in EFUSE.
 * @param[out] plain_text is the target plain text.
 * @param[in]  encrypted_text is the source encrypted text.
 * @param[in]  key is used to pass the key length information which is key.length and key.buffer is not used.
 * @param[in]  key_index is the index to specify which key slot is used, 0 and 1 are expected index.
 * @return if successful, returns #HAL_AES_STATUS_OK
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_ecb_decrypt_ex(hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *key,
                                        hal_aes_efuse_key_t key_index);

/**
 * @brief     This function provides AES encryption in GCM mode.
 * @param[out] encrypted_text is the target encrypted text.
 * @param[in]  plain_text is the source plain text.
 * @param[in]  iv is the initialization vector.
 * @param[in]  aad is the additional data.
 * @param[in]  key is the AES encryption key.
 * @param[out] tag is the authentication tag (16 bytes).
 * @return     #HAL_AES_STATUS_OK, if the operation completed successfully.
 *
 */
hal_aes_status_t hal_aes_gcm_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *iv,
                                     hal_aes_buffer_t *aad,
                                     hal_aes_buffer_t *key,
                                     hal_aes_buffer_t *tag);
/**
 * @brief     This function provides AES decryption in GCM mode.
 * @param[out] plain_text is the target plain text.
 * @param[in]  encrypted_text is the source encrypted text.
 * @param[in]  iv is the initialization vector.
 * @param[in]  aad is the additional data.
 * @param[in]  key is the AES encryption key.
 * @param[out] tag is the authentication tag (16 bytes).
 * @return     #HAL_AES_STATUS_OK, if the operation completed successfully.
 *
 */
hal_aes_status_t hal_aes_gcm_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *iv,
                                     hal_aes_buffer_t *aad,
                                     hal_aes_buffer_t *key,
                                     hal_aes_buffer_t *tag);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/**
* @}
* @}
*/
#endif /* #ifdef HAL_AES_MODULE_ENABLED */

#endif /* #ifndef __HAL_AES_H__ */


