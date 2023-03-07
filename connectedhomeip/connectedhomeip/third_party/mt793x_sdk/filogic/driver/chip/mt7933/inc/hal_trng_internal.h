/*
 * MediaTek Inc. (C) 2020. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
#ifndef __HAL_TRNG_INTERNAL_H__
#define __HAL_TRNG_INTERNAL_H__

#include "reg_base.h"
#include "type_def.h"
#include "hal_nvic_internal.h"

#define TRNG_INIT  1
#define TRNG_DEINIT  0

#define TRNG_CHECK_AND_SET_BUSY(busy_status)  \
do{ \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    if(trng_init_status == TRNG_INIT){ \
        busy_status = HAL_TRNG_STATUS_ERROR; \
    } else { \
        trng_init_status = TRNG_INIT;  \
        busy_status = HAL_TRNG_STATUS_OK; \
    } \
    restore_interrupt_mask(saved_mask); \
}while(0)

#define TRNG_SET_IDLE()   \
do{  \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    trng_init_status= TRNG_DEINIT;  \
    restore_interrupt_mask(saved_mask); \
}while(0)

#define TRNG_CTRL_REG        (TRNG_BASE + 0x00)
#define TRNG_DATA_REG        (TRNG_BASE + 0x08)
#define TRNG_CONF_REG        (TRNG_BASE + 0x0C)

/* TRNG_CTRL_REG */
#define TRNG_RDY         (0x80000000)
#define TRNG_START       (0x00000001)

/* function delcartion */
void  trng_init(void);
void  trng_deinit(void);
uint32_t  trng_get_random_data(void);
#endif /* #ifndef __HAL_TRNG_INTERNAL_H__ */
