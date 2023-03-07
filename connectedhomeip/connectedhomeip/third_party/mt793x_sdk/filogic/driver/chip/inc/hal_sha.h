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

#ifndef __HAL_SHA_H__
#define __HAL_SHA_H__

#include "hal_platform.h"

#ifdef HAL_SHA_MODULE_ENABLED

#ifdef HAL_SHA_USE_PHYSICAL_MEMORY_ADDRESS
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SHA
 * @{
 * This section introduces the SHA driver APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, SHA function groups, enums, structures and functions.
 *
 * @section HAL_SHA_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SHA                        |Secure Hash Algorithm. For more information, please refer to <a href="https://zh.wikipedia.org/wiki/SHA%E5%AE%B6%E6%97%8F"> introduction to the SHA in Wikipedia </a>.|
 *
 * @section HAL_SHA_Features_Chapter Supported features
 * - \b Support \b five \b mode \b encryption.
 *   The hardware has five different behaviors according to the software mode configuration:
 *  - \b SHA1 \b mode: In this mode, hardware will encrypt in sha1.
 *  - \b SHA224 \b mode: In this mode, hardware will encrypt in sha224.
 *  - \b SHA256 \b mode: In this mode, hardware will encrypt in sha256.
 *
 * @section HAL_SHA_Driver_Usage_Chapter How to use this driver
 *
 * - Use SHA1 to encrypt. \n
 *  To use the SHA1 driver, the caller should allocate a buffer in physical memory region, e.g. TCM, SYSRAM and PSRAM.
 *  - Step 1. Call #hal_sha1_init() to initialize the module.
 *  - Step 2. Call #hal_sha1_append() to encrypt.
 *  - Step 3. Call #hal_sha1_end() to get data.
 *  - sample code:
 *    @code
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t *data = "abcdefghijklmnopqrstwxyz1234567";
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t digest[HAL_SHA1_DIGEST_SIZE] = {0};
 *       hal_sha1_context_t context = {0};
 *       hal_sha1_init(&context);
 *       hal_sha1_append(&context, data, strlen(data));
 *       hal_sha1_end(&context, digest);
 *    @endcode
 *
 * - Other encryption modes are implemented similar to SHA1 mode.
 *
 */
#else /* #ifdef HAL_SHA_USE_PHYSICAL_MEMORY_ADDRESS */
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SHA
 * @{
 * This section introduces the SHA driver APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, SHA function groups, enums, structures and functions.
 *
 * @section HAL_SHA_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SHA                        |Secure Hash Algorithm. For more information, please refer to <a href="https://zh.wikipedia.org/wiki/SHA%E5%AE%B6%E6%97%8F"> introduction to the SHA in Wikipedia </a>.|
 *
 * @section HAL_SHA_Features_Chapter Supported features
 * - \b Support \b five \b mode \b encryption.
 *   The hardware has five different behaviors according to the software mode configuration:
 *  - \b SHA1 \b mode: In this mode, hardware will encrypt in sha1.
 *  - \b SHA224 \b mode: In this mode, hardware will encrypt in sha224.
 *  - \b SHA256 \b mode: In this mode, hardware will encrypt in sha256.
 *
 * @section HAL_SHA_Driver_Usage_Chapter How to use this driver
 *
 * - Use SHA1 to encrypt. \n
 *  - Step 1. Call #hal_sha1_init() to initialize the module.
 *  - Step 2. Call #hal_sha1_append() to encrypt.
 *  - Step 3. Call #hal_sha1_end() to get data.
 *  - sample code:
 *    @code
 *       uint8_t *data = "abcdefghijklmnopqrstwxyz1234567";
 *       uint8_t digest[HAL_SHA1_DIGEST_SIZE] = {0};
 *       hal_sha1_context_t context = {0};
 *       hal_sha1_init(&context);
 *       hal_sha1_append(&context, data, strlen(data));
 *       hal_sha1_end(&context, digest);
 *    @endcode
 *
 * - The others encrypt steps are just like SHA1.
 *
 */
#endif /* #ifdef HAL_SHA_USE_PHYSICAL_MEMORY_ADDRESS */


#include "hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/** @defgroup hal_sha_define Define
  * @{
  */

#define HAL_SHA1_BLOCK_SIZE      (64)  /**<  512 bits = 64  bytes */
#define HAL_SHA1_DIGEST_SIZE     (20)  /**<  160 bits = 20  bytes */
#define HAL_SHA224_BLOCK_SIZE    (64)  /**<  512 bits = 64  bytes */
#define HAL_SHA224_DIGEST_SIZE   (28)  /**<  224 bits = 28  bytes */
#define HAL_SHA256_BLOCK_SIZE    (64)  /**<  512 bits = 64  bytes */
#define HAL_SHA256_DIGEST_SIZE   (32)  /**<  256 bits = 32  bytes */

/**
  * @}
  */


/** @defgroup hal_sha_enum Enum
  * @{
  */

/** @brief This enum defines the HAL interface return value. */
typedef enum {
    HAL_SHA_STATUS_ERROR = -1,      /**< An error occurred. */
    HAL_SHA_STATUS_OK = 0           /**< No error occurred. */
} hal_sha_status_t;

/**
  * @}
  */


/** @defgroup hal_sha_struct Struct
  * @{
  */

/** @brief SHA context - SHA1/224/256 */
typedef struct {
    uint32_t hash_value[16];    /**< Hash value. */
    uint32_t message_length;    /**< Total size. */
    uint8_t  buffer[HAL_SHA256_BLOCK_SIZE];   /**< Data buffer. */
    uint8_t  *block;            /**< A pointer to the start of the buffer. */
    uint32_t block_length;      /**< Block length for each operation. */
} hal_sha_context_t;

/** @brief This defines SHA1 context structure. */
#define hal_sha1_context_t hal_sha_context_t
/** @brief This defines SHA224 context structure. */
#define hal_sha224_context_t hal_sha_context_t
/** @brief This defines SHA256 context structure. */
#define hal_sha256_context_t hal_sha_context_t

/**
  * @}
  */




/**
 * @brief     This function initializes the SHA1 engine.
 * @param[in]  context is the paramter of this API, see #hal_sha_context_t.
 * @return #HAL_SHA_STATUS_OK, if the operation completed successfully.
 * @sa     #hal_sha1_init
 * @par    Example
 * Sample code, please refer to @ref HAL_SHA_Driver_Usage_Chapter.
 *
 */
hal_sha_status_t hal_sha1_init(hal_sha1_context_t *context);

/**
 * @brief     This function appends data for SHA1 operation.
 * @param[in]  context is the paramter of this API, see #hal_sha_context_t.
 * @param[in]  message is the source data buffer.
 * @param[in]  length is the data length.
 * @return #HAL_SHA_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_SHA_Driver_Usage_Chapter.
 *
 */
hal_sha_status_t hal_sha1_append(hal_sha1_context_t *context, const uint8_t *message, uint32_t length);

/**
 * @brief     This function ends the SHA1 operation and gets the SHA1 result.
 * @param[in]  context is the paramter of this API, see #hal_sha_context_t.
 * @param[out]  digest_message is the SHA1 result.
 * @return #HAL_SHA_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_SHA_Driver_Usage_Chapter.
 *
 */
hal_sha_status_t hal_sha1_end(hal_sha1_context_t *context, uint8_t digest_message[HAL_SHA1_DIGEST_SIZE]);


/**
 * @brief     This function initializes the SHA224 engine.
 * @param[in]  context is the paramter of this API, see #hal_sha_context_t.
 * @return #HAL_SHA_STATUS_OK, if the operation completed successfully.
 * @sa    #hal_sha224_init
 * @par    Example
 * Sample code, please refer to @ref HAL_SHA_Driver_Usage_Chapter.
 *
 */
hal_sha_status_t hal_sha224_init(hal_sha224_context_t *context);


/**
 * @brief     This function appends data for SHA224 operation.
 * @param[in]  context is the paramter of this API, see #hal_sha_context_t.
 * @param[in]  message is the source data buffer.
 * @param[in]  length is the data length.
 * @return #HAL_SHA_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_SHA_Driver_Usage_Chapter.
 *
 */
hal_sha_status_t hal_sha224_append(hal_sha224_context_t *context, const uint8_t *message, uint32_t length);


/**
 * @brief     This function ends the SHA224 operation and gets the SHA224 result.
 * @param[in]  context is the paramter of this API, see #hal_sha_context_t.
 * @param[out]  digest_message is the SHA224 result.
 * @return #HAL_SHA_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_SHA_Driver_Usage_Chapter.
 *
 */
hal_sha_status_t hal_sha224_end(hal_sha224_context_t *context, uint8_t digest_message[HAL_SHA224_DIGEST_SIZE]);


/**
 * @brief     This function initializes the SHA256 engine.
 * @param[in]  context is the paramter of this API, see #hal_sha_context_t.
 * @return #HAL_SHA_STATUS_OK, if the operation completed successfully.
 * @sa     #hal_sha256_init
 * @par    Example
 * Sample code, please refer to @ref HAL_SHA_Driver_Usage_Chapter.
 *
 */
hal_sha_status_t hal_sha256_init(hal_sha256_context_t *context);

/**
 * @brief     This function appends data for SHA256 operation.
 * @param[in]  context is the paramter of this API, see #hal_sha_context_t.
 * @param[in]  message is the source data buffer.
 * @param[in]  length is the data length.
 * @return #HAL_SHA_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_SHA_Driver_Usage_Chapter.
 *
 */
hal_sha_status_t hal_sha256_append(hal_sha256_context_t *context, const uint8_t *message, uint32_t length);

/**
 * @brief     This function ends the SHA256 operation and gets the SHA256 result.
 * @param[in]  context is the paramter of this API, see #hal_sha_context_t.
 * @param[out]  digest_message is the SHA256 result.
 * @return #HAL_SHA_STATUS_OK, if the operation completed successfully.
 * @par    Example
 * Sample code, please refer to @ref HAL_SHA_Driver_Usage_Chapter.
 *
 */
hal_sha_status_t hal_sha256_end(hal_sha256_context_t *context, uint8_t digest_message[HAL_SHA256_DIGEST_SIZE]);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/**
* @}
* @}
*/
#endif /* #ifdef HAL_SHA_MODULE_ENABLED */
#endif /* #ifndef __HAL_SHA_H__ */

