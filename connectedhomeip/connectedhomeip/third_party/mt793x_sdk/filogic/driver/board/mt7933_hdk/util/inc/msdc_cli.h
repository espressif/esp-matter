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

#ifndef __MSDC_CLI_H__
#define __MSDC_CLI_H__

#include "hal_sdio.h"
#include "hal_msdc.h"
#include "cli.h"





#ifdef __cplusplus
extern "C" {
#endif


extern cmd_t msdc_cli_cmds[];


#define     GPD_BUF_LEN    0x1000

#define     SDIO_BLK_SIZE     256
#define     SDIO_CHIPID_CR    0x00000000

#define     SDIO_IP_WHLPCR  0x00000004
#define     FW_OWN_REQ_CLR  0x00000200
#define     DRV_OWN_STATUS  0x00000100
#define     W_INT_EN_SET    0x00000001

#define     SDIO_IP_WHISR   0x00000010
#define     TX_DONE_INT     0x00000001
#define     RX0_DONE_INT    0X00000002
#define     RX1_DONE_INT    0X00000004

#define     SDIO_IP_WHIER   0x00000014

#define     SDIO_IP_WTSR0   0x00000028
#define     SDIO_TQ1        0x0000ff00

#define     SDIO_IP_WTDR1   0x00000034

#define     SDIO_IP_WRDR0   0x00000050
#define     SDIO_IP_WRDR1   0x00000054

#define     SDIO_IP_H2DSM0R 0x00000070
#define     SDIO_IP_WTMDR  0x000000B0

#define    SDIO_IP_CLKIOCR   0x00000100
#define    SDIO_IP_CMDIOCR   0x00000104
#define    SDIO_IP_DATA0IOCR 0x00000108
#define    SDIO_IP_DATA1IOCR   0x0000010C
#define    SDIO_IP_DATA2IOCR   0x00000110
#define    SDIO_IP_DATA3IOCR   0x00000114



#define     SDIO_IP_WRPLR   0X00000090
#define     SDIO_RX0_LEN    0x0000ffff
#define     SDIO_RX1_LEN    0xffff0000

#define     SDIO_IP_H2DSM0R 0x00000070
#define     MAGIC_CODE_SUCCESS     0x43434343
#define     MAGIC_CODE_FAIL        0x41414141
#define     MAGIC_RX0_FAIL        0x11111111
#define     MAGIC_RX1_FAIL        0x22222222
#define     MAGIC_TX1_FAIL        0x33333333
#define     MAGIC_INT_FAIL        0x44444444

#define     GPIO_DIN0       0x30404200
#define     GPIO_DIN1       0x30404210

#define     GPIO_PUPD0      0x304030D0
#define     GPIO_PUPD1      0x304030E0

#define     SLT_TRX_TIME    0x1

typedef struct SDIO_Host_Data {
    uint32_t tx_len;
    uint32_t data[1023];
} Host_Data;

#define ESTIMATE_TIME   1

#define \
    CALCULATE_TIME(start,end,duration) \
    ({ \
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end);\
        hal_gpt_get_duration_count(start, end, &duration);\
    })

#define \
    CALCULATE_TIME_WITH_AMOUNT(start,end,duration,amount) \
    ({ \
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end);\
        hal_gpt_get_duration_count(start, end, &duration);\
        amount += duration;\
    })





#define MSDC_CLI_ENTRY      {"msdc", "msdc tx/rx", NULL, msdc_cli_cmds},


#ifdef __cplusplus
}
#endif


#endif /* __GPIO_CLI_H__ */

