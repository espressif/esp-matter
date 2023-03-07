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

#ifndef __HAL_CORE_STATUS_H__
#define __HAL_CORE_STATUS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/** @brief This enum defines the Core ID*/
typedef enum {
    HAL_CORE_CM4  = 0,          /**< CM4 */
    HAL_CORE_DSP0 = 1,          /**< DSP0 */
    HAL_CORE_DSP1 = 2,          /**< DSP1 */
    HAL_CORE_N9   = 3,          /**< N9 */
    HAL_CORE_MAX,                /**< max ID */
    HAL_CORE_ID_NOT_EXIST = 0xffffffff
} hal_core_id_t;

/** @brief This enum defines the Core status*/
typedef enum {
    HAL_CORE_ERROR = -1,
    HAL_CORE_OFF  = 0,
    HAL_CORE_INIT = 1,
    HAL_CORE_ACTIVE = 2,
    HAL_CORE_SLEEP = 3,
    HAL_CORE_EXCEPTION = 4,
} hal_core_status_t;

hal_core_status_t hal_core_status_read(hal_core_id_t id);
hal_core_status_t hal_core_status_write(hal_core_id_t id, hal_core_status_t status);
void hal_dsp_core_reset(hal_core_id_t id, uint32_t reset_vector);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef __HAL_CORE_STATUS_H__ */
