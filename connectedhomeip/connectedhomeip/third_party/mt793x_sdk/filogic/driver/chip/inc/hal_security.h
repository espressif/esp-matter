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

#ifndef __HAL_SECUTIRY_H__
#define __HAL_SECURITY_H__
#include "hal_platform.h"

#ifdef HAL_SECURITY_MODULE_ENABLED

#include "hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @}
  */


/** @defgroup hal_security_enum Enum
  * @{
  */

/** @brief This enum defines the secure boot algorithm.
  */
typedef enum {
    HAL_SBC_NO          = 0,        /**< Secure boot is not enable */
    HAL_SBC_MAC_        = 1,        /**< mediatek propriietary MAC. */
    HAL_SBC_AES_CMAC    = 2,        /**< AES-CMAC */
    HAL_SBC_RSA         = 3,        /**< RSASSA-PSS */
} hal_sbc_algo_t;

/**
  * @}
  */




/**
 * @brief     This function to know whether secure boot is enable or not
 * @param[out]  pointer to store enable bit
 * @return  void
 * @sa     #hal_security_sbc_enable
 *
 */
void hal_security_sbc_enable(uint32_t *enable);

/**
 * @brief     This function to read RSA public key hash from eFuse
 * @param[in] pointer to buffer used to store hash value
 * @param[in]  size of hash value
 * @param[in]  system may has more than one key hash. to indicate which set of hash to read.
 * @return  void
 *
 */
void hal_security_key_hash(uint32_t *phash, uint32_t size, uint32_t set);

/**
 * @brief     the function to read the selected algorithm for secure boot in device
 * @return  #HAL_SBC_NO, secure boot is not enabled and not seclect any algorithm
 *          #HAL_SBC_MAC, mediatek proprietary MAC
 *          #HAL_SBC_AES_CMAC, AES-CMAC(RFC 4493)
 *          #HAL_SBC_RSA, RSASSA-PSS(RFC 3447)
 *
 */
hal_sbc_algo_t  hal_security_sbc_algo(void);


#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /*HAL_SECURITY_MODULE_ENABLED*/

#endif /* __HAL_SECURITY_H__ */

