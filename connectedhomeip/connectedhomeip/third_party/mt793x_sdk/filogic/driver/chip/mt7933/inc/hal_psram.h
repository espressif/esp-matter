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


#ifndef _HAL_PSRAM_H_
#define _HAL_PSRAM_H_

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
 * @addtogroup HAL
 * @{
 * @addtogroup PSRAM
 * @{
 * This section describes the programming interfaces of the PSRAM driver.
 *
 * @section HAL_PSRAM_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the PSRAM driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b Psram                        | psram is one type of dram . |
 *
 * @section HAL_PSRAM_Features_Chapter Supported features
 * - \b PSRAM \b hsleep \n
 * Function #hal_psram_power_hsleep() is used to get the psram device to be half sleep status.
 * - \b PSRAM \b wake up \n
 * Function #hal_psram_power_wakeup() is used to get the psram device to be wake up status from half sleep status.
 * @section HAL_Psram_Driver_Usage_Chapter How to use this driver
 * - \b PSRAM \b hsleep \b wakeup
 *   - call #hal_psram_init() to initialize the psram module.
 * The psram is one kind of semiconductor memory, which is used to to store data. The features supported by this module are listed below:
 *   @endcode
 *
 */

/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_psram_enum Enum
  * @{
  */
/** @brief This enum defines the PSRAM status. */
typedef enum {
    HAL_PSRAM_STATUS_FAIL       = -1,           /**< Error occurred during the function call. */
    HAL_PSRAM_STATUS_SUCCESS    = 0             /**< No error occurred during the function call.*/
} hal_psram_status_t;


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
 * @brief     This function initializes the psram base enironment. Call this function if psram is required.
 * @return    #HAL_PSRAM_STATUS_SUCCESS, if the operation is successful.\n
 *            #HAL_PSRAM_STATUS_FAIL, if the operation is failed.
 */
extern hal_psram_status_t hal_psram_init(void);

/**
 * @brief     This function gets psram power to half sleep status.
 * @return    #HAL_PSRAM_STATUS_SUCCESS, if the operation is successful.\n
 *            #HAL_PSRAM_STATUS_FAIL, if the operation is failed.
 */
extern hal_psram_status_t hal_psram_power_hsleep(void);

/**
 * @brief     This function gets psram power to wakeup status.
 * @return    #HAL_PSRAM_STATUS_SUCCESS, if the operation is successful.\n
 *            #HAL_PSRAM_STATUS_FAIL, if the operation is failed.
 */
extern hal_psram_status_t hal_psram_power_wakeup(void);

/**
 * @brief     This function power off the psram, including slp memory cell.
 * @return    #HAL_PSRAM_STATUS_SUCCESS, if the operation is successful.\n
 *            #HAL_PSRAM_STATUS_FAIL, if the operation is failed.
 */
hal_psram_status_t hal_psram_off(void);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


/**
 * @}
 * @}
*/

#endif /* #ifndef _HAL_PSRAM_H_ */


