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
#ifndef __SWLA_H__
#define __SWLA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define IRQ_START 0xAAAA0000
#define IRQ_END   0xAAAAAAAA

typedef enum SWLA_ACTION_T {
    SA_START = 0xe0,
    SA_STOP,
    SA_LABEL
} SA_ACTION_t;

typedef enum SWLA_CONTROL_T {
    SA_DISABLE = 0x0,
    SA_ENABLE = 0x1,
} SA_CONTROL_t;

typedef struct SWLA_IMAGE_HEADER {
    uint8_t xMainVer;
    uint8_t xSubVer;
    uint16_t xImageHeaderLen;
    uint16_t xMainDescLen; //?
    uint16_t xAddonDescLen; //?
    uint8_t xMode;
    uint8_t xCoreID;
    uint8_t res1[2];
    uint32_t xDumpWrapCount;
    uint8_t xPlatformName[16];
    uint8_t xFlavormName[32];
    uint32_t xStartPosition;
    uint32_t xCurPosition;
    uint32_t xBufLen;
    uint32_t xWrapCount;
} SLA_IMAGE_HEADER;


typedef struct SWLA_NODE_T {
    uint32_t xContext;
    uint32_t xTimeStamp;
} SA_NODE_t;

extern void SLA_Enable(void);
extern void SLA_CustomLogging(const char *pxCustomLabel, SA_ACTION_t xAction);
extern void SLA_RamLogging(uint32_t xContext);
extern void SLA_Control(SA_CONTROL_t xOperation);
extern void SLA_EnterDeepSleep(void *data);
extern void SLA_ExitDeepSleep(void *data);

#ifdef __cplusplus
}
#endif


#endif /* __SWLA_H__ */

