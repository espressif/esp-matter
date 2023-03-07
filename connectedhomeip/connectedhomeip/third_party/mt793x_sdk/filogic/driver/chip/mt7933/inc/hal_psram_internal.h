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


#ifndef _HAL_PSRAM_INTERNAL_H_
#define _HAL_PSRAM_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#include "hal_psram.h"

typedef enum {
    PSRAM_MODE_PSRAM_4MB_APM,       /** 0 APM_NON_UHS_4MB*/
    PSRAM_MODE_PSRAM_8MB_APM,       /** 1 APM_NON_UHS_8MB*/
    PSRAM_MODE_UHS_PSRAM_8MB,       /** 2 RDL_UHS_8MB*/
    PSRAM_MODE_UHS_PSRAM_16MB,      /** 3 RDL_UHS_16MB*/
    PSRAM_MODE_PSRAM_4MB_WB,        /** 4 WB_NON_UHS_4MB*/
    PSRAM_MODE_PSRAM_8MB_WB,        /** 5 WB_NON_UHS_8MB*/
    PSRAM_MODE_RESERVED,
    PSRAM_MODE_NONE,
} PSRAM_MODE_ENUM;

typedef void (*p_asic_mpu_config_callbak)(void);

void uhs_psram_S0_to_S1(void);
void uhs_psram_S1_to_S0(void);
void uhs_psram_S1_to_Sidle_HSLEEP(void);
void uhs_psram_Sidle_to_S1_HSLEEP(void);
void uhs_psram_Sidle_to_S1_SREF(void);
void uhs_psram_S1_to_Sidle_SREF(void);
void nonuhs_psram_low_pwr_wake_up(PSRAM_MODE_ENUM mode);
int nonuhs_psram_low_pwr_half_slp(PSRAM_MODE_ENUM mode);
PSRAM_MODE_ENUM psram_type_get_from_AO_RG(void);
U32 psram_size_get(void);
hal_psram_status_t hal_psram_security_restore_after_wakeup(void);
hal_psram_status_t hal_psram_ls_backup_before_hsleep(void);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


/**
 * @}
 * @}
*/

#endif /* #ifndef _HAL_PSRAM_INTERNAL_H_ */


