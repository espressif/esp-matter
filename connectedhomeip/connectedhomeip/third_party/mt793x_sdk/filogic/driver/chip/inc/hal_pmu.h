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

#ifndef __HAL_PMU_H__
#define __HAL_PMU_H__

#ifdef HAL_PMU_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup PMU
 * @{
 * This section describes the programming interfaces of the Power Management Unit(PMU) HAL driver.
 *
 * @section HAL_PMU_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the PMU driver and how to use its various functions.
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b PMU            | The power managment unit(PMU) manages the power supply fo the entire chip, include processor, memory and more.|
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <reg_base.h>
#include "hal_platform.h"

/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_pmu_enum Enum
 *  @{
 */

/** @brief This enum defines the PMU API return status*/
typedef enum {
    HAL_PMU_STATUS_ERROR = -1,      /**< pmu function error */
    HAL_PMU_STATUS_OK = 0       /**< pmu function ok */
} hal_pmu_status_t;


/** @brief This enum defines the output voltage settings of vcore*/
typedef enum {
    HAL_PMU_VCORE_0p65V = 0,    /**< vcore 0.65V*/
    HAL_PMU_VCORE_0p7V,     /**< vcore 0.7V*/
    HAL_PMU_VCORE_0p8V,     /**< vcore 0.8V*/
} hal_pmu_vcore_vosel_t;

/** @brief This enum defines the output voltage settings of mldo*/
typedef enum {
    HAL_PMU_MLDO_0p65V = 0, /**< vcore 0.65V*/
    HAL_PMU_MLDO_0p7V,          /**< vcore 0.7V*/
    HAL_PMU_MLDO_0p8V,          /**< vcore 0.8V*/
    HAL_PMU_MLDO_0p85V,     /**< vcore 0.85V*/
} hal_pmu_mldo_vosel_t;

/** @brief This enum defines the PMU stage flow */
typedef enum {
    HAL_SLEEP_NORMAL = 0,
    HAL_RETENTION_NORMAL,
} hal_pmu_stage_flow;

/** @brief PMU force mode*/
typedef enum {
    HAL_PMU_FORCE_PFM = 0,
    HAL_PMU_FORCE_PWM = 1,
} hal_pmu_force_mode_t;

/** @brief PMU control mode*/
typedef enum {
    HAL_PMU_CONTROL_OFF = 0,
    HAL_PMU_CONTROL_ON,
} hal_pmu_control_t;

/** @brief PMU retention status*/
typedef enum {
    HAL_PMU_RETENTION_LDO_OFF = 0,
    HAL_PMU_RETENTION_LDO_ON,
} hal_pmu_retention_status_t;

/**
 * @}
 */

/*****************************************************************************
 * Functions
 *****************************************************************************/
/**
 * @brief     PMU init function.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *      // initial
 *  hal_pmu_init();
 *
 * @endcode
 */
hal_pmu_status_t hal_pmu_init(void);

/**
 * @brief     This function sets vcore power to the specific voltage.
 * @param[in] vol specified vcore voltage.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *  // set vcore to 0.7V
 *   hal_pmu_set_vcore_voltage(HAL_PMU_VCORE_0p7V);
 * @endcode
 */
hal_pmu_status_t hal_pmu_set_vcore_voltage(hal_pmu_vcore_vosel_t vol);

/**
 * @brief     This function sets mldo power to the specific voltage.
 * @param[in] vol specified mldo voltage.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *  // set mldo to 0.7V
 *   hal_pmu_set_mldo_voltage(HAL_PMU_MLDO_0p7V);
 * @endcode
 */
hal_pmu_status_t hal_pmu_set_mldo_voltage(hal_pmu_mldo_vosel_t vol);

/**
 * \ifnot MT7933
 * @brief     This function set power mode of pmu.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *      // initial power mode
 *  hal_pmu_init_power_mode();
 *
 * @endcode
 * \else
 * @private
 * @brief     This function is not supported.
 * \endif
 */

hal_pmu_status_t hal_pmu_init_power_mode(void);
/**
 * \ifnot MT7933
 * @brief     This function set the pmu into sleep mode.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *      // enter sleep mode
 *  hal_pmu_enter_sleep_mode();
 *
 * @endcode
 * \else
 * @private
 * @brief     This function is not supported.
 * \endif
 */
hal_pmu_status_t hal_pmu_enter_sleep_mode(void);

/**
 * \ifnot MT7933
 * @brief     This function set the pmu into retention mode.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *      //retention mode
 *  hal_pmu_enter_retention_mode();
 *
 * @endcode
 * \else
 * @private
 * @brief     This function is not supported.
 * \endif
 */
hal_pmu_status_t hal_pmu_enter_retention_mode(hal_pmu_retention_status_t status);

/**
 * \ifnot MT7933
 * @brief     This function resume the pmu into normal mode.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *      // resume to normal mode
 *  hal_pmu_resume_to_normal();
 *
 * @endcode
 * \else
 * @private
 * @brief     This function is not supported.
 * \endif
 */
hal_pmu_status_t hal_pmu_resume_to_normal(hal_pmu_stage_flow mode);

/**
 * \ifnot MT7933
 * @brief     This function sets cldo power to the specific voltage.
 * @param[in] index specified of cldo voltage.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *  // set cldo voltage by index
 *   hal_pmu_set_cldo_voltage(index);
 * @endcode
 * \else
 * @private
 * @brief     This function is not supported.
 * \endif
 */
hal_pmu_status_t hal_pmu_set_cldo_voltage(int index);

/**
 * \ifnot MT7933
 * @brief     This function sets retention status of ldo.
 * @param[in] retention status specified of ldo.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *  // set reteion status of ldo
 *   hal_pmu_retention_ldo_control(status);
 * @endcode
 * \else
 * @private
 * @brief     This function is not supported.
 * \endif
 */
hal_pmu_status_t hal_pmu_retention_ldo_control(hal_pmu_control_t status);

/**
 * \ifnot MT7933
 * @brief     This function switch the power of sram.
 * @param[in] power status specified of sram.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *  // switch sram power
 *   hal_pmu_sram_power_switch_control(status);
 * @endcode
 * \else
 * @private
 * @brief     This function is not supported.
 * \endif
 */
hal_pmu_status_t hal_pmu_sram_power_switch_control(hal_pmu_control_t status);

/**
 * \ifnot MT7933
 * @brief     This function sets retention voltage of ldo.
 * @param[in] retention voltage specified of ldo.
 * @return    To indicate whether this function call is successful or not. \n
 *                #HAL_PMU_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_PMU_STATUS_ERROR, if the operation is failed. \n
 * @par       Example
 * @code
 *  // set reteion voltage of ldo
 *   hal_pmu_set_retention_ldo_voltage(adjust_value);
 * @endcode
 * \else
 * @private
 * @brief     This function is not supported.
 * \endif
 */
hal_pmu_status_t hal_pmu_set_retention_ldo_voltage(int adjust_value);

/**
 * @}
 * @}
*/

#endif /* HAL_PMU_MODULE_ENABLED */
#endif /* __HAL_PMU_H__ */
