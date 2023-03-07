/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
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

#ifndef __HAL_FLASH_H__
#define __HAL_FLASH_H__
#include "hal_platform.h"

#ifdef HAL_FLASH_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup FLASH
 * @{
 * This section describes the programming interfaces of the FLASH driver.
 *
 * @section HAL_FLASH_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the FLASH driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b XIP                        | eXecute In Place: A method of executing programs directly from long term storage rather than copying it into the RAM |
 *
 * @section HAL_FLASH_Features_Chapter Supported features
 * The general purpose of FLASH is to save data into the embedded FLASH storage and read them back. The features supported by this module are listed below:
 * - \b FLASH \b erase \n
 * It has to be erased before data can be saved into the FLASH storage, function  #hal_flash_erase() implements that operation.
 * - \b FLASH \b write \n
 * Function #hal_flash_write() is called to save data into the FLASH storage after it's erased.
 * - \b FLASH \b read \n
 * Function #hal_flash_read() is called to read data from the FLASH storage.
 * @section HAL_FLASH_Driver_Usage_Chapter How to use this driver
 * - \b FLASH \b read \b and \b Write
 *   - step1: call #hal_flash_init() to initialize the FLASH module.
 *   - step2: call #hal_flash_erase() to erase the target block of the FLASH storage
 *   - step3: call #hal_flash_write() to write data to the FLASH storage
 *   - step4: call #hal_flash_read() to read data from the FLASH storage
 *   - Sample code:
 *   @code
 *  if (HAL_FLASH_STATUS_OK != hal_flash_init()) {
 *      //error handling
 *  }
 *  start_address = 0xAC000;// Start_address should be within flash size.
 *  if (HAL_FLASH_STATUS_OK != hal_flash_erase(start_address, HAL_FLASH_BLOCK_4K)) {
 *      //error handling
 *  }
 *  #define MAX_DATA (16)
 *  uint8_t data_to_write[MAX_DATA] = {0};
 *  if (HAL_FLASH_STATUS_OK != hal_flash_write(start_address, data_to_write, sizeof(data_to_write))) {
 *      //error handling
 *  }
 *  uint8_t data_read[MAX_DATA] = {0};
 *  if (HAL_FLASH_STATUS_OK != hal_flash_read(start_address, data_read, sizeof(data_read))) {
 *      //error handling
 *  }
 *  //data handling
 *   @endcode
 *
 */

#include "hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_flash_enum Enum
 *  @{
 */

typedef enum hal_flash_lock_size_t {
    HAL_FLASH_LOCK_SIZE_256K,
    HAL_FLASH_LOCK_SIZE_512K,
    HAL_FLASH_LOCK_SIZE_1M,
    HAL_FLASH_LOCK_SIZE_2M,
    HAL_FLASH_LOCK_SIZE_4M,
    HAL_FLASH_LOCK_SIZE_ALL,
} hal_flash_lock_size_t;



/** @brief flash API return status definition */
typedef enum {
    HAL_FLASH_STATUS_ERROR_ERASE_FAIL = -7,       /**< flash erase fail */
    HAL_FLASH_STATUS_ERROR_PROG_FAIL = -6,        /**< flash program fail */
    HAL_FLASH_STATUS_ERROR_NO_INIT = -5,          /**< flash driver don't initiate */
    HAL_FLASH_STATUS_ERROR_NO_SPACE = -4,         /**< flash has no space to write */
    HAL_FLASH_STATUS_ERROR_WRONG_ADDRESS = -3,    /**< flash invalid access address */
    HAL_FLASH_STATUS_ERROR_LOCKED = -2,           /**< flash had locked */
    HAL_FLASH_STATUS_ERROR = -1,                  /**< flash function error */
    HAL_FLASH_STATUS_OK = 0                       /**< flash function ok */
} hal_flash_status_t;


/** @brief flash block definition */
typedef enum {
    HAL_FLASH_BLOCK_4K  = 0,          /**< flash erase block size 4k   */
    HAL_FLASH_BLOCK_32K = 1,          /**< flash erase block size 32k */
    HAL_FLASH_BLOCK_64K = 2,          /**< flash erase block size 64k */
    HAL_FLASH_CHIP_ERASE = 3
} hal_flash_block_t;

/**
  * @}
 */


/*****************************************************************************
 * Structures
 *****************************************************************************/



/*****************************************************************************
 * Functions
 *****************************************************************************/
/**
 * @brief     flash init
 * @return
 * #HAL_FLASH_STATUS_OK on success\n
 * #HAL_FLASH_STATUS_ERROR_NO_INIT on fail
 */
hal_flash_status_t hal_flash_init(void);


/**
 * @brief     flash deinit
 * @return
 * #HAL_FLASH_STATUS_OK on success\n
 */
hal_flash_status_t hal_flash_deinit(void);


/**
 * @brief     flash erase
 * @param[in]  start_address is starting address to erase from
 * @param[in]  block_type is the size of block to be erased
 * @return
 * #HAL_FLASH_STATUS_OK on success\n
 * #HAL_FLASH_STATUS_ERROR_ERASE_FAIL on fail
 * @note
 *  The start_address should be align with the block_type
 */
hal_flash_status_t hal_flash_erase(uint32_t start_address,  hal_flash_block_t block_type);


/**
 * @brief     flash read
 * @param[in]  start_address is starting address to read the data from
 * @param[out]  buffer is place to hold the incoming data
 * @param[in]  length is the length of the data content
 * @return
 * #HAL_FLASH_STATUS_OK on success\n
 * #HAL_FLASH_STATUS_ERROR on fail
 */
hal_flash_status_t hal_flash_read(uint32_t start_address, uint8_t *buffer, uint32_t length);


/**
 * @brief     flash write
 * @param[in]  address is starting address to write from.Before the address can be written to for the first time,
 *            the address located sector or block must first be erased.
 * @param[in]  length is data length
 * @param[in]  data is source data to be written
 * @return
 * #HAL_FLASH_STATUS_OK on success\n
 * #HAL_FLASH_STATUS_ERROR_PROG_FAIL on fail
 */
hal_flash_status_t hal_flash_write(uint32_t address, const uint8_t *data, uint32_t length);


/**
 * @brief     modify base address of flash
 * @param[in]  base_address: 0x1800_0000 or 0x9000_0000
 * @return
 */
void hal_flash_set_base_address(uint32_t base_address) ;


/**
 * @brief     flash otp read
 * @param[in]  address is starting address to read from.
 * @param[in]  length is data length
 * @param[in]  data is source data to be read
 * @return
 * #HAL_FLASH_STATUS_OK on success\n
 * #HAL_FLASH_STATUS_ERROR on fail
 */
hal_flash_status_t hal_flash_otp_read(uint32_t addr, uint32_t len, uint8_t *buffer);


/**
 * @brief     flash otp write
 * @param[in]  address is starting address to write from.
 * @param[in]  length is data length
 * @param[in]  data is source data to be written
 * @return
 * #HAL_FLASH_STATUS_OK on success\n
 * #HAL_FLASH_STATUS_ERROR on fail
 */
hal_flash_status_t hal_flash_otp_write(uint32_t addr, uint32_t len, uint8_t *buffer);


/**
 * @brief     flash otp lock
 * @return
 * #HAL_FLASH_STATUS_OK on success\n
 * #HAL_FLASH_STATUS_ERROR on fail
 */
hal_flash_status_t hal_flash_otp_lock(void);


/**
 * @brief    check flash otp lock status
 * @param[in]  otp lock status
 * @return
 * #HAL_FLASH_STATUS_OK on success\n
 * #HAL_FLASH_STATUS_ERROR on fail
 */
hal_flash_status_t hal_flash_otp_lockstatus(uint8_t *lockstatus);


/**
 * @brief    when function return, flash in ready state
 */
hal_flash_status_t Flash_ReturnReady(void);



#ifdef __cplusplus
}
#endif


/**
 * @}
 * @}
*/
#endif /*HAL_FLASH_MODULE_ENABLED*/
#endif /* __HAL_FLASH_H__ */


