/* Copyright Statement:
 *
 * (C) 2005-2020  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek Inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE.
 */

/**
 * @file wdt.h
 *
 * This header file exposes the API for WDT
 */

#ifndef __WDT_H__
#define __WDT_H__

#include "reg_base.h"
#include "type_def.h"

#define WDT_RESTART_KEY         (0x1971)
#define WDT_LENGTH_KEY          (0x08)
#define WDT_MODE_KEY            (0x22)
#define WDT_SWRST_KEY           (0x1209)

/* WDT_MODE     0x0030 */
#define WDT_MODE_KEY_OFFSET            (8)
#define WDT_MODE_KEY_MASK              BITS(8, 15)
#define WDT_MODE_IRQ_OTHER_EN_OFFSET   (4)
#define WDT_MODE_IRQ_OFFSET            (3)
#define WDT_MODE_EXTEN_OFFSET          (2)
#define WDT_MODE_EXTPOL_OFFSET         (1)
#define WDT_MODE_ENABLE_OFFSET         (0)

/* WDT_LENGTH       0x0034 */
#define WDT_LENGTH_TIMEOUT_OFFSET     (5)
#define WDT_LENGTH_TIMEOUT_MASK       BITS(5, 15)
#define WDT_LENGTH_KEY_OFFSET         (0)
#define WDT_LENGTH_KEY_MASK           BITS(0, 4)

/* WDT_STA          0x003C */
#define WDT_STA_WDT_OFFSET          (15)
#define WDT_STA_SW_WDT_OFFSET       (14)
#define WDT_STA_RSV3_STA_OFFSET     (13)

/* WDT_SW_STA          0x300B0004 */
#define WDT_SW_STA_WDT_OFFSET       (1)
#define WDT_SW_STA_SW_WDT_OFFSET    (0)

/* WDT_INTERVAL     0x0040 */
#define WDT_INTERVAL_LENGTH_OFFSET  (0)
#define WDT_INTERVAL_LENGTH_MASK    BITS(0, 11)
#define WDT_INTERVAL_MIN            (5)

/* WDT_SWINT    0x0080 */
#define WDT_SW_INT_OFFSET          (31)
#define WDT_SW_INT_CLR_OFFSET      (30)
#define WDT_RSV_OFFSET             (0)

#define WDT_REG_BASE               (WDT_BASE)
#define WDT_SW_STA_BASE            (PMU_CTRL_BASE)
#define WDT_SW_STA_OFFSET          (0x04)

#define WDT_MODE_OFFSET            (0x30)
#define WDT_SWINT_ADDR             (WDT_REG_BASE + 0x80)


/** @defgroup _WDT_TypeDef Struct
  * @{
  */
// WDT
/** @brief This structure defines the watchdog timer control registers.*/
typedef struct _WDT_TypeDef {
    volatile uint32_t WDT_MODE;     //0x0030
    volatile uint32_t WDT_LENGTH;   //0x0034
    volatile uint32_t WDT_RESTART;  //0x0038
    volatile uint32_t WDT_STA;      //0x003C
    volatile uint32_t WDT_INTERVAL; //0x0040
    volatile uint32_t WDT_SWRST;    //0x0044
} WDT_TypeDef, *P_WDT_TypeDef;


void wdt_set_counter(uint32_t value);
void wdt_set_interval(uint32_t value);
/**
 * @brief config watchdog mode, watchdog has two modes: \n
 *        reset mode: hardware reset will happen in this mode if the timer has expired or software reset register is set.
 *        interrupt mode: An interrupt will trigger instead of hardware reset. \n
 *        when watchdog reset/interrupt happened, watchdog timer also can trigger interrupts to other cores(Todo).
 *
 * @param irq:
 * @param    0: reset mode
 * @param    1: interrupt mode
 *
 * @return none
 */
void wdt_set_mode(uint32_t irq);
/**
 * @brief This function is used to enable/disable watchdog timer, then the watchdog timer will start/stop counting down.
 * @param ucEnable: enable or disable watchdog timer.
 * @return    none
 */
void wdt_set_enable(uint32_t ucEnable);
/**
 * @brief restart watchdog timer, watchdog timer will restart counting down from the initial value.
 *
 * @param none
 * @return none
 */
extern void wdt_restart(void);
/**
 * @brief generate watchdog reset/interrupt by register setting.
 *        After this function is called, watchdog reset/interrupt will happen.
 *
 * @param none
 * @return none
 */
void wdt_swrst(void);
/**
 * @brief get watchdog timer status of last reset/interrupt. If watchdog timer failed to fed and
 *        has expired or the software register is set by #wdt_swrst() function, the reset/interrupt
 *        will hanpped.
 *
 * @param none
 * @return high value means reset/interrupt occurred
 * @return low value means no reset/interrupt occurred
 *
 */
uint32_t wdt_get_status(void);
void wdt_dump_info(void);
void wdt_sw_set_hw_reboot(void);
void wdt_sw_clr_sw_reboot(void);
#endif /* #ifndef __WDT_H__ */
