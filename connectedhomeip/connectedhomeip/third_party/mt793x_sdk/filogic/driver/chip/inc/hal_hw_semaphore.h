/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#ifndef __HAL_SMPH_H__
#define __HAL_SMPH_H__

#include "hal_platform.h"

#ifdef HAL_HW_SEMAPHORE_MODULE_ENABLED


/**
 * @addtogroup HAL
 * @{
 * @addtogroup HW_SEMAPHORE
 * @{
 * This section describes the programming interfaces of the SEMAPHORE HAL driver.
 *
 * @section HAL_HW_SEMAPHORE_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the SEMAPHORE driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b HW_SEMAPHORE         | A hardware mechanism used to control access to a common resource by multiple processes in a concurrent system such as a multiprogramming operating system |
 *
 * @section HAL_HW_SEMAPHORE_Features_Chapter Supported features
 * - \b Take \b a \b hardware \b semaphore. \n
 *   Call hal_hw_semaphore_take() function to take a hardware semaphore.
 * - \b Give \b a \b hardware \b semaphore. \n
 *   Call hal_hw_semaphore_give() function to give a hardware semaphore.
 * - \b Query \b the \b channel \b of \b a \b specific \b hardware \b semaphore. \n
 *   Call hal_hw_semaphore_query_channel() function to query which channel occupies the semaphore.
 *
 * @}
 * @}
 */


/**
 * @addtogroup HAL
 * @{
 * @addtogroup HW_SEMAPHORE
 * @{
 * @section HAL_HW_SEMAPHORE_Driver_Usage_Chapter How to use this driver
 * - \b Use \b HW_SEMAPHORE \b to \b lock \b the \b critical \b resources, \b e.g. \b memory \b or \b IO, \b cross \b MCUs. \n
 *   - Step 1: Call hal_nvic_save_and_set_interrupt_mask() to disable irq before taking a semaphore.
 *   - Step 2: Call hal_hw_semaphore_take() to take a semaphore. You can use a busy loop to do it more than one time, just as in the example code shown below.
 *   - Step 3: Do something for the critical resource.
 *   - Step 4: Call hal_hw_semaphore_give() to give the semaphore.
 *   - Step 5: Call hal_nvic_restore_interrupt_mask() to restore the previous irq status.
 *   - Sample code:
 *   @code
 *
 *     #define MAX_TIMES  10000
 *     uint32_t int_mask;
 *     uint32_t take_times=0;
 *
 *     hal_nvic_save_and_set_interrupt_mask(&int_mask);
 *
 *     while(++take_times)
 *     {
 *         if (HAL_HW_SEMAPHORE_STATUS_OK==hal_hw_semaphore_take(HAL_HW_SEMAPHORE_ID_0))
 *         {
 *             break;
 *         }
 *         if (take_times>MAX_TIMES)
 *         {
 *              hal_nvic_restore_interrupt_mask(int_mask);
 *              //error handling
 *         }
 *     }
 *
 *      //Put your code here. If the semaphore is taken, do something for the critical resource.
 *
 *      if (HAL_HW_SEMAPHORE_STATUS_OK==hal_hw_semaphore_give(HAL_HW_SEMAPHORE_ID_0))
 *      {
 *          hal_nvic_restore_interrupt_mask(int_mask);
 *      } else {
 *          hal_nvic_restore_interrupt_mask(int_mask);
 *          //error handling
 *      }
 *
 *   @endcode
 *
 *
 * - \b Use \b HW_SEMAPHORE \b to \b check \b whether \b the \b shared \b resource \b is \b avaliable \b cross \b MCUs. \n
 *   - Step 1: Call hal_hw_semaphore_take() to take a semaphore.
 *   - Step 2: Do something for the critical resource.
 *   - Step 3: Call hal_hw_semaphore_give() to give the semaphore.
 *   - Sample code:
 *   @code
 *
 *     hal_i2c_config_t i2c_config;
 *     uint8_t slave_address = 0X50;
 *     const uint8_t send_data[8] = {0x00, 0x00, 0xFF, 0xAA, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19}; //the data that used to send
 *
 *     i2c_config.frequency = HAL_I2C_FREQUENCY_400K;
 *     uint32_t slave_address = 0x20;
 *
 *    if (HAL_HW_SEMAPHORE_STATUS_OK == hal_hw_semaphore_take(HAL_HW_SEMAPHORE_ID_0)) {
 *        //If the semaphore is used for sharing I2C port 0 cross MCUs,do something when the semaphore taken successfully.
 *        hal_i2c_init(HAL_I2C_MASTER_0, &i2c_config);
 *        hal_i2c_master_send_polling(HAL_I2C_MASTER_0, slave_address, send_data, sizeof(send_data))
 *        hal_i2c_deinit(HAL_I2C_MASTER_0);
 *
 *        if(HAL_HW_SEMAPHORE_STATUS_OK !=hal_hw_semaphore_give(HAL_I2C_MASTER_0)) {
 *            //error handing
 *        }
 *     } else {
 *      //error handing
 *     }
 *
 *   @endcode
 */


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_hw_semaphore_enum Enum
 *  @{
 */

/** @brief This enum defines the SEMAPHORE channel*/
typedef enum {
    HAL_HW_SEMAPHORE_CHANNEL_CM4  = 0,          /**< HW semaphore channel used by CM4 */
    HAL_HW_SEMAPHORE_CHANNEL_DSP0 = 1,          /**< HW semaphore channel used by DSP0 */
    HAL_HW_SEMAPHORE_CHANNEL_DSP1 = 2,          /**< HW semaphore channel used by DSO1 */
    HAL_HW_SEMAPHORE_CHANNEL_N9   = 3,          /**< HW semaphore channel used by N9 */
    HAL_HW_SEMAPHORE_CHANNEL_NOT_EXIST = 0xFF   /**< HW semaphore is not used by any core */
} hal_hw_semaphore_channel_t;


/** @brief This enum defines the SEMAPHORE ID. \n
*   @note  In order to make sure the ID is dedicated for a specific core/feature, the user should use the predefined \n
*   MACRO that represents the SEMAPHORE ID. The MACRO is defined in the following lines: \n
*   mcu\driver\board\ab155x_evk\hw_resource_assignment\inc\hal_resource_assignment.h \n
*   dsp\driver\board\ab155x_evk\hw_resource_assignment\inc\hal_resource_assignment.h \n
*   Taking #HW_SEMAPHORE_SYSLOG as an example, the user should define the same MACRO with the same ID in the \n
*   two header files above and use the MACRO when using the semaphore ID.
*/
typedef enum {
    HAL_HW_SEMAPHORE_ID_0  = 0,                 /**< HW semaphore id 0 */
    HAL_HW_SEMAPHORE_ID_1  = 1,
    HAL_HW_SEMAPHORE_ID_2  = 2,
    HAL_HW_SEMAPHORE_ID_3  = 3,
    HAL_HW_SEMAPHORE_ID_4  = 4,
    HAL_HW_SEMAPHORE_ID_5  = 5,
    HAL_HW_SEMAPHORE_ID_6  = 6,
    HAL_HW_SEMAPHORE_ID_7  = 7,
    HAL_HW_SEMAPHORE_ID_8  = 8,
    HAL_HW_SEMAPHORE_ID_9  = 9,
    HAL_HW_SEMAPHORE_ID_10 = 10,
    HAL_HW_SEMAPHORE_ID_11 = 11,
    HAL_HW_SEMAPHORE_ID_12 = 12,
    HAL_HW_SEMAPHORE_ID_13 = 13,
    HAL_HW_SEMAPHORE_ID_14 = 14,
    HAL_HW_SEMAPHORE_ID_15 = 15,
    HAL_HW_SEMAPHORE_ID_16 = 16,
    HAL_HW_SEMAPHORE_ID_17 = 17,
    HAL_HW_SEMAPHORE_ID_18 = 18,
    HAL_HW_SEMAPHORE_ID_19 = 19,
    HAL_HW_SEMAPHORE_ID_20 = 20,
    HAL_HW_SEMAPHORE_ID_21 = 21,
    HAL_HW_SEMAPHORE_ID_22 = 22,
    HAL_HW_SEMAPHORE_ID_23 = 23,
    HAL_HW_SEMAPHORE_ID_24 = 24,
    HAL_HW_SEMAPHORE_ID_25 = 25,
    HAL_HW_SEMAPHORE_ID_26 = 26,
    HAL_HW_SEMAPHORE_ID_27 = 27,
    HAL_HW_SEMAPHORE_ID_28 = 28,
    HAL_HW_SEMAPHORE_ID_29 = 29,
    HAL_HW_SEMAPHORE_ID_30 = 30,
    HAL_HW_SEMAPHORE_ID_31 = 31,
    HAL_HW_SEMAPHORE_ID_32 = 32,
    HAL_HW_SEMAPHORE_ID_33 = 33,
    HAL_HW_SEMAPHORE_ID_34 = 34,
    HAL_HW_SEMAPHORE_ID_35 = 35,
    HAL_HW_SEMAPHORE_ID_36 = 36,
    HAL_HW_SEMAPHORE_ID_37 = 37,
    HAL_HW_SEMAPHORE_ID_38 = 38,
    HAL_HW_SEMAPHORE_ID_39 = 39,
    HAL_HW_SEMAPHORE_ID_40 = 40,
    HAL_HW_SEMAPHORE_ID_41 = 41,
    HAL_HW_SEMAPHORE_ID_42 = 42,
    HAL_HW_SEMAPHORE_ID_43 = 43,
    HAL_HW_SEMAPHORE_ID_44 = 44,
    HAL_HW_SEMAPHORE_ID_45 = 45,
    HAL_HW_SEMAPHORE_ID_46 = 46,
    HAL_HW_SEMAPHORE_ID_47 = 47,
    HAL_HW_SEMAPHORE_ID_48 = 48,
    HAL_HW_SEMAPHORE_ID_49 = 49,
    HAL_HW_SEMAPHORE_ID_50 = 50,
    HAL_HW_SEMAPHORE_ID_51 = 51,
    HAL_HW_SEMAPHORE_ID_52 = 52,
    HAL_HW_SEMAPHORE_ID_53 = 53,
    HAL_HW_SEMAPHORE_ID_54 = 54,
    HAL_HW_SEMAPHORE_ID_55 = 55,
    HAL_HW_SEMAPHORE_ID_56 = 56,
    HAL_HW_SEMAPHORE_ID_57 = 57,
    HAL_HW_SEMAPHORE_ID_58 = 58,
    HAL_HW_SEMAPHORE_ID_59 = 59,
    HAL_HW_SEMAPHORE_ID_60 = 60,
    HAL_HW_SEMAPHORE_ID_61 = 61,
    HAL_HW_SEMAPHORE_ID_62 = 62,
    HAL_HW_SEMAPHORE_ID_63 = 63,
    HAL_HW_SEMAPHORE_ID_MAX
} hal_hw_semaphore_id_t;


/** @brief This enum defines the SEMAPHORE API return status*/
typedef enum {
    HAL_HW_SEMAPHORE_STATUS_INVALID_PARAMETER = -3, /**< Invalid input ID */
    HAL_HW_SEMAPHORE_STATUS_GIVE_ERROR = -2,    /**< semaphore gives error, it is already released before giving */
    HAL_HW_SEMAPHORE_STATUS_TAKE_ERROR = -1,    /**< semaphore takes error, it is already taken by someone else */
    HAL_HW_SEMAPHORE_STATUS_OK = 0              /**< semaphore takes and gives successfully */
} hal_hw_semaphore_status_t;

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
 * @brief  Take a specified semaphore
 * @param[in] id is the semaphore id with the type #hal_hw_semaphore_id_t
 * @return
 * #HAL_HW_SEMAPHORE_STATUS_INVALID_PARAMETER, invalid parameter.
 * #HAL_HW_SEMAPHORE_STATUS_TAKE_ERROR, semaphore take failed.
 * #HAL_HW_SEMAPHORE_STATUS_OK, semaphore take sucessfully.
 */
hal_hw_semaphore_status_t hal_hw_semaphore_take(hal_hw_semaphore_id_t id);

/**
 * @brief Give a specified semaphore
 * @param[in] id is the semaphore id with the type #hal_hw_semaphore_id_t
 * @return
 * #HAL_HW_SEMAPHORE_STATUS_INVALID_PARAMETER, invalid parameter.
 * #HAL_HW_SEMAPHORE_STATUS_GIVE_ERROR, semaphore give failed.
 * #HAL_HW_SEMAPHORE_STATUS_OK, semaphore give success.
 */
hal_hw_semaphore_status_t hal_hw_semaphore_give(hal_hw_semaphore_id_t id);

/**
 * @brief   Query semaphore is taken by which channel(core)
 * @param[in] id is the semaphore id with the type #hal_hw_semaphore_id_t
 * @return
 * #HAL_HW_SEMAPHORE_STATUS_INVALID_PARAMETER, invalid parameter.
 * #HAL_HW_SEMAPHORE_CHANNEL_CM4, semaphore is taken by CM4.
 * #HAL_HW_SEMAPHORE_CHANNEL_DSP0, semaphore is taken by DSP0.
 * #HAL_HW_SEMAPHORE_CHANNEL_DSP1, semaphore is taken by DSP1.
 * #HAL_HW_SEMAPHORE_CHANNEL_N9, semaphore is taken by N9.
 * #HAL_HW_SEMAPHORE_CHANNEL_NOT_EXIST, semaphore is not taken by any channel.
 */
hal_hw_semaphore_channel_t hal_hw_semaphore_query_channel(hal_hw_semaphore_id_t id);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


/**
 * @}
 * @}
*/
#endif /* #ifdef HAL_HW_SEMAPHORE_MODULE_ENABLED */
#endif /* #ifndef __HAL_SMPH_H__ */



