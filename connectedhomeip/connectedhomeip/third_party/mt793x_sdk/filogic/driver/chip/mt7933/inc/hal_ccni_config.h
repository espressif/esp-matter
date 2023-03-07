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

#ifndef __HAL_CCNI_CONFIG_H__
#define __HAL_CCNI_CONFIG_H__
#ifdef HAL_CCNI_MODULE_ENABLED


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

typedef enum {
    HAL_CCNI_EVENT0 = 0,
    HAL_CCNI_EVENT1 = 1,
    HAL_CCNI_EVENT2 = 2,
    HAL_CCNI_EVENT3 = 3,
    HAL_CCNI_EVENT4 = 4,
    HAL_CCNI_EVENT5 = 5,
    HAL_CCNI_EVENT6 = 6,
    HAL_CCNI_EVENT7 = 7,
    HAL_CCNI_EVENT8 = 8,
    HAL_CCNI_EVENT9 = 9,
    HAL_CCNI_EVENT10 = 10,
    HAL_CCNI_EVENT11 = 11,
    HAL_CCNI_EVENT12 = 12,
    HAL_CCNI_EVENT13 = 13,
    HAL_CCNI_EVENT14 = 14,
    HAL_CCNI_EVENT15 = 15,
    HAL_CCNI_EVENT16 = 16,
    HAL_CCNI_EVENT17 = 17,
    HAL_CCNI_EVENT18 = 18,
    HAL_CCNI_EVENT19 = 19,
    HAL_CCNI_EVENT20 = 20,
    HAL_CCNI_EVENT21 = 21,
    HAL_CCNI_EVENT22 = 22,
    HAL_CCNI_EVENT23 = 23,
    HAL_CCNI_EVENT24 = 24,
    HAL_CCNI_EVENT25 = 25,
    HAL_CCNI_EVENT26 = 26,
    HAL_CCNI_EVENT27 = 27,
    HAL_CCNI_EVENT28 = 28,
    HAL_CCNI_EVENT29 = 29,
    HAL_CCNI_EVENT30 = 30,
    HAL_CCNI_EVENT31 = 31,
    HAL_CCNI_EVENT_RESERVED = 0xFFFFFFFF,
} ccni_event_t;


#define CCNI_EVENT_MASK              (0X000000FF)
#define CCNI_SRC_MASK                (0XFF000000)
#define CCNI_DST_MASK                (0X00FF0000)

#define CCNI_EVENT_SRC_OFFSET        24

#define CCNI_EVENT_SRC_CM4           (0x1<<CCNI_EVENT_SRC_OFFSET)
#define CCNI_EVENT_SRC_DSP0          (0x2<<CCNI_EVENT_SRC_OFFSET)
#define CCNI_EVENT_SRC_DSP1          (0x3<<CCNI_EVENT_SRC_OFFSET)
#define CCNI_EVENT_SRC_N9            (0x4<<CCNI_EVENT_SRC_OFFSET)

#define CCNI_EVENT_DST_OFFSET        16

#define CCNI_EVENT_DST_CM4           (0x1<<CCNI_EVENT_DST_OFFSET)
#define CCNI_EVENT_DST_DSP0          (0x2<<CCNI_EVENT_DST_OFFSET)
#define CCNI_EVENT_DST_DSP1          (0x3<<CCNI_EVENT_DST_OFFSET)
#define CCNI_EVENT_DST_N9            (0x4<<CCNI_EVENT_DST_OFFSET)

#define MSG_LENGTH                   (8)

/**
 * @addtogroup HAL
 * @{
 * @addtogroup CCNI
 * @{
 * This section introduces how to define a CCNI event.
 */


/** @defgroup hal_ccni_define Define
 * @{
 *
 */

/*************************************************************************************
 ********************************* the next define is for CM4  ***********************
**************************************************************************************/
/* CCNI event from CM4 to DSP0 defination: Event0~7 Should be used with message. */
/**@brief Event0 had been used by AUDIO_CM4_RX. */
#define CCNI_CM4_TO_DSP0_EVENT0      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT0)

/**@brief Event1 is in default status. */
#define CCNI_CM4_TO_DSP0_EVENT1      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT1)
/**@brief Event2 is in default status. */
#define CCNI_CM4_TO_DSP0_EVENT2      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_CM4_TO_DSP0_EVENT3      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_CM4_TO_DSP0_EVENT4      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_CM4_TO_DSP0_EVENT5      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT5)
/**@brief Event6 had been used on system. */
#define CCNI_CM4_TO_DSP0_CCCI_ACK    (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT6)
/**@brief Event7 had been used on system. */
#define CCNI_CM4_TO_DSP0_CCCI        (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. */
#define CCNI_CM4_TO_DSP0_MSG_MAX     (HAL_CCNI_EVENT7+1)
/**@brief Should not change the define. */
#define CCNI_CM4_TO_DSP0_MSG_SIZE    ( MSG_LENGTH*CCNI_CM4_TO_DSP0_MSG_MAX)
/**@brief Event8-31 should be used without message.
 * Should not change the defination as the EVENT8 is reserved for exception.*/
#define IRQGEN_CM4_TO_DSP0_EXCEPTION (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT8)

/**@brief Event9 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT9    (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT9)
/**@brief Event10 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT10   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT11   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT12   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT13   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT14   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT15   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT16   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT17   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT18   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT19   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT20   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT21   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT22   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT23   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT24   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT25   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT26   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT27   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT28   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT29   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT30   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_CM4_TO_DSP0_EVENT31   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_CM4_TO_DSP0_EVENT_MAX (HAL_CCNI_EVENT31+1)
/**@brief Should not change the define. */
#define CCNI_CM4_TO_DSP0_EVENT_MAX   (IRQGEN_CM4_TO_DSP0_EVENT_MAX)


/* CCNI event from CM4 to DSP1 defination: Event0~7 Should be used with message. */
/**@brief Event0 had been used on system. */
#define CCNI_CM4_TO_DSP1_EVENT0      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT0)


/**@brief Event1 is in default status. */
#define CCNI_CM4_TO_DSP1_EVENT1      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT1)
/**@brief Event2 is in default status. */
#define CCNI_CM4_TO_DSP1_EVENT2      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_CM4_TO_DSP1_EVENT3      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_CM4_TO_DSP1_EVENT4      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_CM4_TO_DSP1_EVENT5      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT5)
/**@brief Event6 had been used on system. */
#define CCNI_CM4_TO_DSP1_CCCI_ACK    (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT6)
/**@brief Event7 had been used on system. */
#define CCNI_CM4_TO_DSP1_CCCI        (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_CM4_TO_DSP1_MSG_MAX     (HAL_CCNI_EVENT7+1)
/**@brief Should not change the define. */
#define CCNI_CM4_TO_DSP1_MSG_SIZE    (CCNI_CM4_TO_DSP0_MSG_SIZE+MSG_LENGTH*CCNI_CM4_TO_DSP1_MSG_MAX)
/**@brief Event8-31 should be used without message. \n
 * Should not change the defination as the EVENT8 is reserved for exception.*/
#define IRQGEN_CM4_TO_DSP1_EXCEPTION (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT8)

/**@brief Event9 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT9    (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT9)
/**@brief Event10 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT10   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT11   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT12   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT13   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT14   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT15   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT16   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT17   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT18   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT19   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT20   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT21   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT22   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT23   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT24   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT25   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT26   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT27   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT28   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT29   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT30   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_CM4_TO_DSP1_EVENT31   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_CM4_TO_DSP1_EVENT_MAX (HAL_CCNI_EVENT31+1)
/**@brief Should not change the define. */
#define CCNI_CM4_TO_DSP1_EVENT_MAX   (IRQGEN_CM4_TO_DSP1_EVENT_MAX)



/* CCNI event from CM4 to N9 defination: Event0~7 Should be used with message. */
/**@brief Event0 had been used by HCI_CTRL. */
#define CCNI_CM4_TO_N9_HCI_CTRL_EVENT0 (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT0)
/**@brief Event1 had been used by INFO_SYNC. */
#define CCNI_CM4_TO_N9_INFO_SYNC_EVENT1 (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT1)

/**@brief Event2 is in default status. */
#define CCNI_CM4_TO_N9_EVENT2        (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_CM4_TO_N9_EVENT3        (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_CM4_TO_N9_EVENT4        (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_CM4_TO_N9_EVENT5        (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT5)
/**@brief Event6 is in default status. */
#define CCNI_CM4_TO_N9_CCCI_ACK      (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT6)
/**@brief Event7 is in default status. */
#define CCNI_CM4_TO_N9_CCCI          (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_CM4_TO_N9_MSG_MAX     (HAL_CCNI_EVENT7+1)
/**@brief Should not change the define. */
#define CCNI_CM4_TO_N9_MSG_SIZE    (CCNI_CM4_TO_DSP1_MSG_SIZE+MSG_LENGTH*CCNI_CM4_TO_N9_MSG_MAX)
/**@brief Event8-31 should be used without message. */
/**@brief Event8 is used by HCI_CMD. */
#define IRQGEN_CM4_TO_N9_HCI_CMD     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT8)
/**@brief Event8 is used by exception. */
#define IRQGEN_CM4_TO_N9_EXCEPTION   (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT9)

/**@brief Event10 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT10     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT11     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT12     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT13     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT14     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT15     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT16     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT17     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT18     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT19     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT20     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT21     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT22     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT23     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT24     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT25     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT26     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT27     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT28     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT29     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT30     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_CM4_TO_N9_EVENT31     (CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_CM4_TO_N9_EVENT_MAX (HAL_CCNI_EVENT31+1)
/**@brief Should not change the next define. */
#define CCNI_CM4_TO_N9_EVENT_MAX   (IRQGEN_CM4_TO_N9_EVENT_MAX)



/*************************************************************************************
 ********************************* the next define is for DSP0 ***********************
**************************************************************************************/

/* CCNI event from DSP0 to CM4 defination: Event0~7 Should be used with message. */
/**@brief Event0 had been used by AUDIO_CM4_TX. */
#define AUDIO_CM4_TX_EVENT           (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT0)
/**@brief Event1 had been used by DVFS. */
#define CCNI_CM4_TO_DSP0_DVFS        (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT1)

/**@brief Event2 is in default status. */
#define CCNI_DSP0_TO_CM4_EVENT2      (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_DSP0_TO_CM4_EVENT3      (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_DSP0_TO_CM4_EVENT4      (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_DSP0_TO_CM4_EVENT5      (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT5)
/**@brief Event6 had been used on system. */
#define CCNI_DSP0_TO_CM4_CCCI_ACK    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT6)
/**@brief Event7 had been used on system. */
#define CCNI_DSP0_TO_CM4_CCCI        (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_DSP0_TO_CM4_MSG_MAX     (HAL_CCNI_EVENT7+1)
/**@brief Should not change the define. */
#define CCNI_DSP0_TO_CM4_MSG_SIZE    (CCNI_CM4_TO_N9_MSG_SIZE+MSG_LENGTH * CCNI_DSP0_TO_CM4_MSG_MAX)
/**@brief Event8-31 should be used without message.
 * Should not change the defination as the EVENT8 is reserved for exception.*/
#define IRQGEN_DSP0_TO_CM4_EXCEPTION (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT8)

/**@brief Event9 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT9    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT9)
/**@brief Event10 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT10   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT11   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT12   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT13   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT14   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT15   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT16   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT17   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT18   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT19   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT20   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT21   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT22   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT23   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT24   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT25   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT26   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT27   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT28   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT29   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT30   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_DSP0_TO_CM4_EVENT31   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_DSP0_TO_CM4_EVENT_MAX (HAL_CCNI_EVENT31+1)
/**@brief Should not change the define. */
#define CCNI_DSP0_TO_CM4_EVENT_MAX   (IRQGEN_DSP0_TO_CM4_EVENT_MAX)


/* CCNI event from DSP0 to DSP1 defination: Event0~7 Should be used with message. */
/**@brief Event0  is in default status. */
#define CCNI_DSP0_TO_DSP1_EVENT0     (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT0)
/**@brief Event1 is in default status. */
#define CCNI_DSP0_TO_DSP1_LOCK_SLEEP (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT1)
/**@brief Event2 is in default status. */
#define CCNI_DSP0_TO_DSP1_EVENT2     (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_DSP0_TO_DSP1_EVENT3     (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_DSP0_TO_DSP1_EVENT4     (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_DSP0_TO_DSP1_EVENT5     (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT5)
/**@brief Event6 had been used on system. */
#define CCNI_DSP0_TO_DSP1_CCCI_ACK   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT6)
/**@brief Event7 had been used on system. */
#define CCNI_DSP0_TO_DSP1_CCCI       (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_DSP0_TO_DSP1_MSG_MAX     (HAL_CCNI_EVENT7 + 1)
/**@brief Should not change the define. */
#define CCNI_DSP0_TO_DSP1_MSG_SIZE    (CCNI_DSP0_TO_CM4_MSG_SIZE+MSG_LENGTH*CCNI_DSP0_TO_DSP1_MSG_MAX)
/**@brief Event8-31 should be used without message.
 * Should not change the defination as the EVENT8 is reserved for exception.*/
#define IRQGEN_DSP0_TO_DSP1_EXCEPTION (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT8)

/**@brief Event9 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT9   (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT9)
/**@brief Event10 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT10  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT11  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT12  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT13  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT14  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT15  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT16  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT17  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT18  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT19  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT20  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT21  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT22  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT23  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT24  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT25  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT26  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT27  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT28  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT29  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT30  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_DSP0_TO_DSP1_EVENT31  (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_DSP0_TO_DSP1_EVENT_MAX (HAL_CCNI_EVENT31 + 1)
/**@brief Should not change the define. */
#define CCNI_DSP0_TO_DSP1_EVENT_MAX   (IRQGEN_DSP0_TO_DSP1_EVENT_MAX)



/* CCNI event from DSP0 to N9 defination: Event0~7 Should be used with message. */
/**@brief Event0 had been used by AUDIO_N9_TX. */
#define AUDIO_N9_TX_EVENT            (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT0)

/**@brief Event1 is in default status. */
#define CCNI_DSP0_TO_N9_EVENT1       (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT1)
/**@brief Event2 is in default status. */
#define CCNI_DSP0_TO_N9_EVENT2       (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_DSP0_TO_N9_EVENT3       (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_DSP0_TO_N9_EVENT4       (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_DSP0_TO_N9_EVENT5       (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT5)
/**@brief Event7 is used by CCCI ACK. */
#define CCNI_DSP0_TO_N9_CCCI_ACK     (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT6)
/**@brief Event7 is used by CCCI. */
#define CCNI_DSP0_TO_N9_CCCI         (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_DSP0_TO_N9_MSG_MAX     (HAL_CCNI_EVENT7+1)
/**@brief Should not change the define. */
#define CCNI_DSP0_TO_N9_MSG_SIZE    (CCNI_DSP0_TO_DSP1_MSG_SIZE+MSG_LENGTH*CCNI_DSP0_TO_N9_MSG_MAX)
/**@brief Event8-31 should be used without message.
 * Should not change the defination as the EVENT8 is reserved for exception.*/
#define IRQGEN_DSP0_TO_N9_VOICE_ENCODE_DONE (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT8)

/**@brief Event9 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT9     (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT9)
/**@brief Event10 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT10    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT11    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT12    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT13    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT14    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT15    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT16    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT17    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT18    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT19    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT20    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT21    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT22    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT23    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT24    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT25    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT26    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT27    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT28    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT29    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT30    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_DSP0_TO_N9_EVENT31    (CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_DSP0_TO_N9_EVENT_MAX (HAL_CCNI_EVENT31 + 1)
/**@brief Should not change the define. */
#define CCNI_DSP0_TO_N9_EVENT_MAX   (IRQGEN_DSP0_TO_N9_EVENT_MAX)



/*************************************************************************************
 ********************************* the next define is for DSP1 ***********************
**************************************************************************************/

/** CCNI event from DSP1 to CM4 defination: Event0~7 Should be used with message. */
/**@brief Event0 had been used on system. */
#define CCNI_DSP1_TO_CM4_EVENT0      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT0)

/**@brief Event1 is in default status. */
#define CCNI_DSP1_TO_CM4_EVENT1      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT1)
/**@brief Event2 is in default status. */
#define CCNI_DSP1_TO_CM4_EVENT2      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_DSP1_TO_CM4_EVENT3      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_DSP1_TO_CM4_EVENT4      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_DSP1_TO_CM4_EVENT5      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT5)
/**@brief Event6 had been used on system. */
#define CCNI_DSP1_TO_CM4_CCCI_ACK    (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT6)
/**@brief Event7 had been used on system. */
#define CCNI_DSP1_TO_CM4_CCCI        (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_DSP1_TO_CM4_MSG_MAX     (HAL_CCNI_EVENT7+1)
/**@brief Should not change the define. */
#define CCNI_DSP1_TO_CM4_MSG_SIZE    (CCNI_DSP0_TO_N9_MSG_SIZE+MSG_LENGTH*CCNI_DSP1_TO_CM4_MSG_MAX)
/**@brief Event8-31 should be used without message.
 * Should not change the defination as the EVENT8 is reserved for exception.*/
#define IRQGEN_DSP1_TO_CM4_EXCEPTION (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT8)

/**@brief Event9 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT9    (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT9)
/**@brief Event10 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT10   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT11   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT12   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT13   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT14   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT15   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT16   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT17   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT18   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT19   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT20   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT21   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT22   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT23   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT24   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT25   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT26   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT27   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT28   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT29   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT30   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_DSP1_TO_CM4_EVENT31   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_DSP1_TO_CM4_EVENT_MAX (HAL_CCNI_EVENT31+1)
/**@brief Should not change the define. */
#define CCNI_DSP1_TO_CM4_EVENT_MAX   (IRQGEN_DSP1_TO_CM4_EVENT_MAX)


/**CCNI event from DSP1 to DSP0 defination: Event0~7 Should be used with message. */
/**@brief  Event0 had been used on system. */
#define CCNI_DSP1_TO_DSP0_EVENT0      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT0)
/**@brief should not change above defines as there had been used. */

/**@brief Event1 is in default status. */
#define CCNI_DSP1_TO_DSP0_LOCK_SLEEP  (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT1)
/**@brief Event2 is in default status. */
#define CCNI_DSP1_TO_DSP0_EVENT2      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_DSP1_TO_DSP0_EVENT3      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_DSP1_TO_DSP0_EVENT4      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_DSP1_TO_DSP0_EVENT5      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT5)
/**@brief Event6 had been used on system. */
#define CCNI_DSP1_TO_DSP0_CCCI_ACK    (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT6)
/**@brief Event7 had been used on system. */
#define CCNI_DSP1_TO_DSP0_CCCI        (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_DSP1_TO_DSP0_MSG_MAX     (HAL_CCNI_EVENT7+1)
/**@brief Should not change the define. */
#define CCNI_DSP1_TO_DSP0_MSG_SIZE    (CCNI_DSP1_TO_CM4_MSG_SIZE+MSG_LENGTH*CCNI_DSP1_TO_DSP0_MSG_MAX)
/**@brief Event8-31 should be used without message.
 * Should not change the defination as the EVENT8 is reserved for exception.*/
#define IRQGEN_DSP1_TO_DSP0_EXCEPTION (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT8)

/**@brief Event9 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT9    (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT9)
/**@brief Event10 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT10   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT11   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT12   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT13   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT14   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT15   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT16   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT17   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT18   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT19   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT20   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT21   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT22   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT23   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT24   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT25   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT26   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT27   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT28   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT29   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT30   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_DSP1_TO_DSP0_EVENT31   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_DSP1_TO_DSP0_EVENT_MAX (HAL_CCNI_EVENT31+1)
/**@brief Should not change the define. */
#define CCNI_DSP1_TO_DSP0_EVENT_MAX   (IRQGEN_DSP1_TO_DSP0_EVENT_MAX)



/** CCNI event from DSP1 to N9 defination: Event0~7 Should be used with message. */
/**@brief Event0 had been used on system.*/
#define CCNI_DSP1_TO_N9_EVENT0      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT0)
/**@brief should not change above defines as there had been used. */

/**@brief Event1 is in default status. */
#define CCNI_DSP1_TO_N9_EVENT1      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT1)
/**@brief Event2 is in default status. */
#define CCNI_DSP1_TO_N9_EVENT2      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_DSP1_TO_N9_EVENT3      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_DSP1_TO_N9_EVENT4      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_DSP1_TO_N9_EVENT5      (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT5)
/**@brief Event6 had been used on system. */
#define CCNI_DSP1_TO_N9_CCCI_ACK    (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT6)
/**@brief Event7 had been used on system. */
#define CCNI_DSP1_TO_N9_CCCI        (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_DSP1_TO_N9_MSG_MAX     (HAL_CCNI_EVENT7 + 1)
/**@brief Should not change the define. */
#define CCNI_DSP1_TO_N9_MSG_SIZE    (CCNI_DSP1_TO_DSP0_MSG_SIZE+MSG_LENGTH*CCNI_DSP1_TO_N9_MSG_MAX)
/**@brief should not change above defines as there had been used. */

/**@brief Event8-31 should be used without message.*/
/**@brief Event8 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT8    (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT8)
/**@brief Event9 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT9    (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT9)
/**@brief Event10 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT10   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT11   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT12   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT13   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT14   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT15   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT16   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT17   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT18   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT19   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT20   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT21   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT22   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT23   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT24   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT25   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT26   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT27   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT28   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT29   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT30   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_DSP1_TO_N9_EVENT31   (CCNI_EVENT_SRC_DSP1|CCNI_EVENT_DST_N9|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_DSP1_TO_N9_EVENT_MAX (HAL_CCNI_EVENT31+1)
/**@brief Should not change the define. */
#define CCNI_DSP1_TO_N9_EVENT_MAX   (IRQGEN_DSP1_TO_N9_EVENT_MAX)
/************************************* DSP1 End  **************************************/



/*************************************************************************************
 ********************************* the next define is for N9   ***********************
**************************************************************************************/

/** CCNI event from N9 to CM4 defination: Event0~7 Should be used with message. */
/**@brief Event0 had been used by HCI_RESP. */
#define CCNI_N9_TO_CM4_HCI_RESP     (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT0)
/**@brief should not change above defines as there had been used. */


/**@brief Event1 is in default status. */
#define CCNI_N9_TO_CM4_EVENT1       (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT1)
/**@brief Event2 is in default status. */
#define CCNI_N9_TO_CM4_EVENT2       (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_N9_TO_CM4_EVENT3       (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_N9_TO_CM4_EVENT4       (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_N9_TO_CM4_EVENT5       (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT5)
/**@brief Event6 had been used on system. */
#define CCNI_N9_TO_CM4_CCCI_ACK     (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT6)
/**@brief Event7 had been used on system. */
#define CCNI_N9_TO_CM4_CCCI         (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_N9_TO_CM4_MSG_MAX      (HAL_CCNI_EVENT7+1)
/**@brief Should not change the define. */
#define CCNI_N9_TO_CM4_MSG_SIZE     (CCNI_DSP1_TO_N9_MSG_SIZE+MSG_LENGTH*CCNI_N9_TO_CM4_MSG_MAX)
/**@brief Event8-31 should be used without message.*/
#define IRQGEN_N9_TO_CM4_HCI_EVENT8 (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT8)
/**@brief Should not change the defines. */
#define IRQGEN_N9_TO_CM4_EXCEPTION  (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT9)
/**@brief should not change above defines as there had been used. */

/**@brief Event10 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT10    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT11    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT12    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT13    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT14    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT15    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT16    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT17    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT18    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT19    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT20    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT21    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT22    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT23    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT24    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT25    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT26    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT27    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT28    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT29    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT30    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_N9_TO_CM4_EVENT31    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_CM4|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_N9_TO_CM4_EVENT_MAX  (HAL_CCNI_EVENT31+1)
/**@brief Should not change the define. */
#define CCNI_N9_TO_CM4_EVENT_MAX    (IRQGEN_N9_TO_CM4_EVENT_MAX)



/**CCNI event from N9 to DSP0 defination:Event0~7 Should be used with message. */
/**@brief Event0 had been used by AUDIO_N9_RX.*/
#define AUDIO_N9_RX_EVENT           (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT0)

/**@brief Event1 is in default status. */
#define CCNI_N9_TO_DSP0_EVENT1      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT1)
/**@brief Event2 is in default status. */
#define CCNI_N9_TO_DSP0_EVENT2      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_N9_TO_DSP0_EVENT3      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_N9_TO_DSP0_EVENT4      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_N9_TO_DSP0_EVENT5      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT5)
/**@brief Event6 had been used on system. */
#define CCNI_N9_TO_DSP0_CCCI_ACK    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT6)
/**@brief Event7 had been used on system. */
#define CCNI_N9_TO_DSP0_CCCI        (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_N9_TO_DSP0_MSG_MAX     (HAL_CCNI_EVENT7+1)
/**@brief Should not change the define. */
#define CCNI_N9_TO_DSP0_MSG_SIZE    (CCNI_N9_TO_CM4_MSG_SIZE+MSG_LENGTH*CCNI_N9_TO_DSP0_MSG_MAX)
/**@brief Event8-31 should be used without message.
 * EVENT8 is reserved for VOICE_ENCODE. */
#define IRQGEN_N9_TO_DSP0_VOICE_ENCODE (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT8)
/**@brief EVENT9 is reserved for VOICE_DECODE. */
#define IRQGEN_N9_TO_DSP0_VOICE_DECODE (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT9)

/**@brief Event10 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT10   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT11   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT12   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT13   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT14   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT15   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT16   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT17   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT18   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT19   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT20   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT21   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT22   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT23   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT24   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT25   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT26   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT27   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT28   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT29   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT30   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_N9_TO_DSP0_EVENT31   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP0|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_N9_TO_DSP0_EVENT_MAX (HAL_CCNI_EVENT31+1)
/**@brief Should not change the define. */
#define CCNI_N9_TO_DSP0_EVENT_MAX   (IRQGEN_N9_TO_DSP0_EVENT_MAX)



/* CCNI event from \b N9 \b to \b DSP1 \b defination: Event0~7 Should be used with message. */
/**@brief Event0 had been used on system. */
#define CCNI_N9_TO_DSP1_EVENT0      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT0)

/**@brief Event1 is in default status. */
#define CCNI_N9_TO_DSP1_EVENT1      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT1)
/**@brief Event2 is in default status. */
#define CCNI_N9_TO_DSP1_EVENT2      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT2)
/**@brief Event3 is in default status. */
#define CCNI_N9_TO_DSP1_EVENT3      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT3)
/**@brief Event4 is in default status. */
#define CCNI_N9_TO_DSP1_EVENT4      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT4)
/**@brief Event5 is in default status. */
#define CCNI_N9_TO_DSP1_EVENT5      (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT5)
/**@brief Event6 had been used on system. */
#define CCNI_N9_TO_DSP1_CCCI_ACK    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT6)
/**@brief Event7 had been used on system. */
#define CCNI_N9_TO_DSP1_CCCI        (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT7)
/**@brief It defines the maximum event with message. Do not change it. */
#define CCNI_N9_TO_DSP1_MSG_MAX     (HAL_CCNI_EVENT7+1)
/**@brief Should not change the define. */
#define CCNI_N9_TO_DSP1_MSG_SIZE    (CCNI_N9_TO_DSP0_MSG_SIZE+MSG_LENGTH*CCNI_N9_TO_DSP1_MSG_MAX)

/**@brief Event8-31 should be used without message. */
/**@brief Event8 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT8    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT8)

/**@brief Event9 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT9    (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT9)
/**@brief Event10 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT10   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT10)
/**@brief Event11 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT11   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT11)
/**@brief Event12 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT12   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT12)
/**@brief Event13 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT13   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT13)
/**@brief Event14 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT14   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT14)
/**@brief Event15 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT15   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT15)
/**@brief Event16 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT16   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT16)
/**@brief Event17 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT17   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT17)
/**@brief Event18 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT18   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT18)
/**@brief Event19 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT19   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT19)
/**@brief Event20 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT20   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT20)
/**@brief Event21 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT21   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT21)
/**@brief Event22 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT22   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT22)
/**@brief Event23 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT23   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT23)
/**@brief Event24 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT24   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT24)
/**@brief Event25 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT25   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT25)
/**@brief Event26 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT26   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT26)
/**@brief Event27 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT27   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT27)
/**@brief Event28 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT28   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT28)
/**@brief Event29 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT29   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT29)
/**@brief Event30 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT30   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT30)
/**@brief Event31 is in default status. */
#define IRQGEN_N9_TO_DSP1_EVENT31   (CCNI_EVENT_SRC_N9|CCNI_EVENT_DST_DSP1|HAL_CCNI_EVENT31)
/**@brief It defines the maximum event without message. */
#define IRQGEN_N9_TO_DSP1_EVENT_MAX (HAL_CCNI_EVENT31+1)
/**@brief Should not change the define. */
#define CCNI_N9_TO_DSP1_EVENT_MAX   (IRQGEN_N9_TO_DSP1_EVENT_MAX)

/**
 * @}
 */

/**
 * @}
 * @}
 */


extern const hal_ccni_function_t ccni_cm4_to_dsp0_function_table[CCNI_CM4_TO_DSP0_EVENT_MAX];
extern const hal_ccni_function_t ccni_cm4_to_dsp1_function_table[CCNI_CM4_TO_DSP1_EVENT_MAX];
extern const hal_ccni_function_t ccni_cm4_to_n9_function_table[CCNI_CM4_TO_N9_EVENT_MAX];

extern const hal_ccni_function_t ccni_dsp0_to_cm4_function_table[CCNI_DSP0_TO_CM4_EVENT_MAX];
extern const hal_ccni_function_t ccni_dsp0_to_dsp1_function_table[CCNI_DSP0_TO_DSP1_EVENT_MAX];
extern const hal_ccni_function_t ccni_dsp0_to_n9_function_table[CCNI_DSP0_TO_N9_EVENT_MAX];

extern const hal_ccni_function_t ccni_n9_to_cm4_function_table[CCNI_N9_TO_CM4_EVENT_MAX];
extern const hal_ccni_function_t ccni_n9_to_dsp0_function_table[CCNI_N9_TO_DSP0_EVENT_MAX];
extern const hal_ccni_function_t ccni_n9_to_dsp1_function_table[CCNI_N9_TO_DSP1_EVENT_MAX];

extern const hal_ccni_function_t ccni_dsp1_to_cm4_function_table[CCNI_DSP1_TO_CM4_EVENT_MAX];
extern const hal_ccni_function_t ccni_dsp1_to_dsp0_function_table[CCNI_DSP1_TO_DSP0_EVENT_MAX];
extern const hal_ccni_function_t ccni_dsp1_to_n9_function_table[CCNI_DSP1_TO_N9_EVENT_MAX];



#endif /* #ifdef HAL_CCNI_MODULE_ENABLED */
#endif /* #ifndef __HAL_CCNI_CONFIG_H__ */

