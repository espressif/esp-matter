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
/* MediaTek restricted information */

#ifndef __BT_SYSTEM_INTERNAL_H__
#define __BT_SYSTEM_INTERNAL_H__

#include "bt_type.h"

BT_EXTERN_C_BEGIN

#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct {
    uint8_t    bdr_init_tx_power_level;  /*Initial level of the BT(BDR) connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. */
    uint8_t    reserved;                 /*Reserved.*/
    uint8_t    fixed_tx_power_enable;    /*Level of bt/le connection radio transmission power is fixed to the #fixed_tx_power_level when enable is 1. Disable this funtion when enable is 0. Only available in AB155x.*/
    uint8_t    fixed_tx_power_level;     /*Fixed level of bt/le connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. Only available in AB155x.*/
    uint8_t    le_init_tx_power_level;   /*Initial level of le connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. */
    uint8_t    bt_max_tx_power_level;    /*Maximum level of bt connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. */
    uint8_t    bdr_tx_power_level_offset; /*BDR tx power level offset select(1dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    bdr_fine_tx_power_level_offset;/*BDR fine tx power level offset select(0.25dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    edr_tx_power_level_offset; /*EDR tx power level offset select(1dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    edr_fine_tx_power_level_offset; /*EDR fine tx power level offset select(0.25dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    ble_tx_power_level_offset;/*BLE tx power level offset select(1dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    ble_fine_tx_power_level_offset;/*BLE fine tx power level offset select(0.25dbm reduced/step, step:0~3). Only available in AB155x.*/
    uint8_t    edr_init_tx_power_level;  /*Initial level of the BT(EDR) connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power. Only available in AB155x*/
} bt_config_tx_power_ext1_t;

bt_status_t bt_config_tx_power_level_ext1(const bt_config_tx_power_ext1_t *tx_power_info);

#define BT_DUT_MODE_ACTIVE_IND                  (BT_MODULE_SYSTEM | 0x0007)    /**< Buletooth device enter dut mode event with last mode. */

#endif/* DOXYGEN_SHOULD_SKIP_THIS */

BT_EXTERN_C_END

#endif

