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

#ifndef __RTC_H__
#define __RTC_H__

#ifdef HAL_RTC_MODULE_ENABLED
#include "reg_base.h"
#include "type_def.h"
#include "hal_rtc.h"


/* IO functions*/
#define __IO_WRITE32__(addr,data)   ((*(volatile unsigned int *)(addr)) = (unsigned int)data)
#define __IO_READ32__(addr)     (*(volatile unsigned int *)(addr))
#define __IO_WRITE16__(addr,data)   ((*(volatile unsigned short *)(addr)) = (unsigned short)data)
#define __IO_READ16__(addr)     (*(volatile unsigned short *)(addr))
#define __IO_WRITE8__(addr,data)    ((*(volatile unsigned char *)(addr)) = (unsigned char)data)
#define __IO_READ8__(addr)      (*(volatile unsigned char *)(addr))

#define RTC_REGISTER_CNT    60
#define RTC_WAIT_ACK_TIMES  50

#define RTC_BASE_ADDR       (0x30070000)
#define RTC_MEMORY_ADDR     (0x30078000)
#define RTC_MEMORY_END      (0x300781FF)

/* RTC mem range: 512 bytes memory, 0x3007800 ~ 0x300781FF  */
#define RTC_MEMORY_SIZE     (0x200)


/* RTC  irq id */
#define RTC_IRQ_ID      (52)

/* RTC key match */
#define RTC_PWRCHK1     (RTC_BASE_ADDR + 0x04)
#define RTC_PWRCHK2     (RTC_BASE_ADDR + 0x08)
#define RTC_KEY         (RTC_BASE_ADDR + 0x0c)
#define RTC_PROT1       (RTC_BASE_ADDR + 0x10)
#define RTC_PROT2       (RTC_BASE_ADDR + 0x14)
#define RTC_PROT3       (RTC_BASE_ADDR + 0x18)
#define RTC_PROT4       (RTC_BASE_ADDR + 0x1c)

#define RTC_CTL         (RTC_BASE_ADDR + 0x20)
#define DEBNCE_OK       (0x1 << 7)
#define INHIBIT         (0x1 << 6)
#define PROT_PASS       (0x1 << 4)
#define KEY_PASS        (0x1 << 3)
#define PWR_PASS        (0x1 << 2)
#define SIM_RTC         (0x1 << 1)
#define RC_STOP         (0x1 << 0)

#define RTC_XOSC_CFG    (RTC_BASE_ADDR + 0x28)
#define DA_EOSC_PWDB    (0x1 << 19) //0: power down   1: power-on, def
#define RG_OSC_CK_SEL   (0x1 << 6) //0:EOSC,def   1: XOSC

#define RTC_DEBNCE      (RTC_BASE_ADDR + 0x2c)
#define RTC_DEBOUNCE_MASK   (0x7)
#define DEBNCE_INITIAL      (0x0)   //less than 1ms
#define DEBNCE_DEFAULT      (0x3)   //wait 2^10-2^6 ~ 2^10 cycle of RTC clock
#define DEBNCE_DEFAULT_MAX      (0x7)

#define RTC_PMU_EN      (RTC_BASE_ADDR + 0x30)
#define RG_EXT_EVENT_STA    (0x1 << 6)
#define RG_TIME_STA     (0x1 << 5)
#define RG_ALARM_STA        (0x1 << 4)
#define RG_PMU_EN_STATE     (0x3 << 2)
#define RG_PMU_EN_EXT       (0x1 << 1)
#define RG_PMU_EN       (0x1 << 0)

#define RTC_PAD_CTL     (RTC_BASE_ADDR + 0x34)

#define RTC_EXT_EV_CTL  (RTC_BASE_ADDR + 0x38)
#define EXT_EV_POL      (0x1 << 1)
#define EXT_EV_EN       (0x1 << 0)

/* RTC TC RG */
#define RTC_TC_YEA      (RTC_BASE_ADDR + 0x40)
#define RTC_TC_MON      (RTC_BASE_ADDR + 0x44)
#define RTC_TC_DOM      (RTC_BASE_ADDR + 0x48)
#define RTC_TC_DOW      (RTC_BASE_ADDR + 0x4c)
#define RTC_TC_HOU      (RTC_BASE_ADDR + 0x50)
#define RTC_TC_MIN      (RTC_BASE_ADDR + 0x54)
#define RTC_TC_SEC      (RTC_BASE_ADDR + 0x58)

/* RTC ALARM RG */
#define RTC_AL_YEA      (RTC_BASE_ADDR + 0x60)
#define RTC_AL_MON      (RTC_BASE_ADDR + 0x64)
#define RTC_AL_DOM      (RTC_BASE_ADDR + 0x68)
#define RTC_AL_DOW      (RTC_BASE_ADDR + 0x6c)
#define RTC_AL_HOU      (RTC_BASE_ADDR + 0x70)
#define RTC_AL_MIN      (RTC_BASE_ADDR + 0x74)
#define RTC_AL_SEC      (RTC_BASE_ADDR + 0x78)

#define RTC_AL_CTL      (RTC_BASE_ADDR + 0x7c)
#define RTC_ALMEN       (0x1 << 0)
#define RTC_MASK        (0x7F << 1)
#define RTC_MASK_OFF_WEEK       (0x77 << 1)

#define RTC_RIP_CTL     (RTC_BASE_ADDR + 0x80)
#define RTC_RIP_OK      (0x1 << 1)
#define RTC_TRG_RD      (0x1 << 0)
#define TRG_RIP_CNT     (0x1)

#define RTC_RIP_CNTH    (RTC_BASE_ADDR + 0x84)
#define RIP_CNTH_MASK   (0x7F)

#define RTC_RIP_CNTL    (RTC_BASE_ADDR + 0x88)
#define RIP_CNTL_MASK   (0xFF)

#define RTC_TIMER_CTL       (RTC_BASE_ADDR + 0x90)
#define TR_INTEN            (0x1 << 0)

#define RTC_TIMER_CNT       (RTC_BASE_ADDR + 0x94)
#define TIMER_CNT           (0xFFFFFFFF)

#define RTC_PRDY_CNT        (RTC_BASE_ADDR + 0xA4)
#define PRDY_CNT_DEFAULT    (0x026F0150)

#define RTC_SPARE0			0x300700C0
#define RTC_SPARE1			0x300700C4
#define RTC_SPARE2			0x300700C8
#define RTC_SPARE3			0x300700CC
#define RTC_SPARE4			0x300700D0
#define RTC_SPARE5			0x300700D4
#define RTC_SPARE6			0x300700D8
#define RTC_SPARE7			0x300700DC
#define RTC_SPARE8			0x300700E0
#define RTC_SPARE9			0x300700E4
#define RTC_SPARE10			0x300700E8
#define RTC_SPARE11			0x300700EC
#define RTC_SPARE12			0x300700F0
#define RTC_SPARE13			0x300700F4
#define RTC_SPARE14			0x300700F8
#define RTC_SPARE15			0x300700FC

#define RTC_COREPDN         (RTC_BASE_ADDR + 0x100)
#define G_ENABLED           (0x1 << 1)
#define CORE_SHUTDOWN       (0x1 << 0)

#define RTC_BACKUP00		0x30070140
#define RTC_BACKUP01		0x30070144
#define RTC_BACKUP02		0x30070148
#define RTC_BACKUP03		0x3007014C
#define RTC_BACKUP04		0x30070150
#define RTC_BACKUP05		0x30070154
#define RTC_BACKUP06		0x30070158
#define RTC_BACKUP07		0x3007015C
#define RTC_BACKUP08		0x30070160
#define RTC_BACKUP09		0x30070164
#define RTC_BACKUP10		0x30070168
#define RTC_BACKUP11		0x3007016C
#define RTC_BACKUP12		0x30070170
#define RTC_BACKUP13		0x30070174
#define RTC_BACKUP14		0x30070178
#define RTC_BACKUP15		0x3007017C
#define RTC_BACKUP16		0x30070180
#define RTC_BACKUP17		0x30070184
#define RTC_BACKUP18		0x30070188
#define RTC_BACKUP19		0x3007018C
#define RTC_BACKUP20		0x30070190
#define RTC_BACKUP21		0x30070194
#define RTC_BACKUP22		0x30070198
#define RTC_BACKUP23		0x3007019C
#define RTC_BACKUP24		0x300701A0
#define RTC_BACKUP25		0x300701A4
#define RTC_BACKUP26		0x300701A8
#define RTC_BACKUP27		0x300701AC
#define RTC_BACKUP28		0x300701B0
#define RTC_BACKUP29		0x300701B4
#define RTC_BACKUP30		0x300701B8
#define RTC_BACKUP31		0x300701BC
#define RTC_BACKUP32		0x300701C0
#define RTC_BACKUP33		0x300701C4
#define RTC_BACKUP34		0x300701C8
#define RTC_BACKUP35		0x300701CC

/* RTC MAGIC KEY */
#define RTC_PWRCHK1_VAL     (0xc6)
#define RTC_PWRCHK2_VAL     (0x9a)
#define RTC_PROT1_VAL       (0xa3)
#define RTC_PROT2_VAL       (0x57)
#define RTC_PROT3_VAL       (0x67)
#define RTC_PROT4_VAL       (0xd2)
#define RTC_KEY_VAL     (0x59)


/** @brief RTC time structure definition.
*  base time: 2000/1/1 00:00:00 for example
*/

enum {
    NORMAL_OP = 0,
    SIMULATION_OP = 1
};

enum rtc_osc_src_t {
    RTC_OSC_SRC_EOSC = 1,
    RTC_OSC_SRC_XOSC = 2,
    RTC_OSC_SRC_END = 3,
};

enum mem_type {
    RTC_MEM_NO_SPAN = 1,
    RTC_MEM_SPAN = 2,
    RTC_MEM_SPAN_LAST = 3,
};

hal_rtc_status_t hal_rtc_osc32_sel(enum rtc_osc_src_t osc32);
hal_rtc_status_t hal_rtc_enter_rtc_mode(void);
void rtc_dump_rg(void);
void rtc_set_debounce_time(UINT32 debnce);
void rtc_set_enable(bool ucEnable);
void rtc_set_timer(UINT32 counter);
void rtc_ext_event_ctl(bool en, UINT32 pol);
void rtc_set_date(U32 front, U32 size, const char *buf, U32 buf_off, enum mem_type type);
void rtc_get_date(U32 front, U32 size, char *buf, U32 buf_off, enum mem_type type);

#endif /* #ifdef HAL_RTC_MODULE_ENABLED */
#endif /* #ifndef __RTC_H__ */
