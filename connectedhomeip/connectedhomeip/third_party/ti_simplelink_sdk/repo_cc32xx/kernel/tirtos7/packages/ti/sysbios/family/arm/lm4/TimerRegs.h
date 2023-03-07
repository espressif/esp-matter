/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ti_catalog_arm_peripherals_timers_timer_h
#define ti_catalog_arm_peripherals_timers_timer_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef volatile struct ti_catalog_arm_peripherals_timers_TimerRegs {
    uint32_t GPTMCFG;     /* 0x00 GPTM Config */
    uint32_t GPTMTAMR;    /* 0x04 GPTM TimerA Mode */
    uint32_t GPTMTBMR;    /* 0x08 GPTM TimerB Mode */
    uint32_t GPTMCTL;     /* 0x0C GPTM Control */
    uint32_t gap10;
    uint32_t gap14;
    uint32_t GPTMIMR;     /* 0x18 GPTM Interrupt Mask */
    uint32_t GPTMRIS;     /* 0x1C GPTM Raw Interrupt Status */
    uint32_t GPTMMIS;     /* 0x20 GPTM Masked Interrupt Status */
    uint32_t GPTMICR;     /* 0x24 GPTM Interrupt Clear */
    uint32_t GPTMTAILR;   /* 0x28 GPTM TimerA Interval Load */
    uint32_t GPTMTBILR;   /* 0x2C GPTM TimerB Interval Load */
    uint32_t GPTMTAMATCHR; /* 0x30 GPTM TimerA Match */
    uint32_t GPTMTBMATCHR; /* 0x34 GPTM TimerB Match */
    uint32_t GPTMTAPR;    /* 0x38 GPTM TimerA Prescale */
    uint32_t GPTMTBPR;    /* 0x3C GPTM TimerB Prescale */
    uint32_t GPTMTAPM;    /* 0x40 GPTM TimerA Prescale Match */
    uint32_t GPTMTBPMR;   /* 0x44 GPTM TimerB Prescale Match */
    uint32_t GPTMTAR;     /* 0x48 GPTM Timer A */
    uint32_t GPTMTBR;     /* 0x4C GPTM Timer B */
} ti_catalog_arm_peripherals_timers_TimerRegs;

typedef volatile struct ti_catalog_arm_peripherals_timers_TimerRegsM4 {
    uint32_t GPTMCFG;      /* 0x00 GPTM Config */
    uint32_t GPTMTAMR;     /* 0x04 GPTM TimerA Mode */
    uint32_t GPTMTBMR;     /* 0x08 GPTM TimerB Mode */
    uint32_t GPTMCTL;      /* 0x0C GPTM Control */
    uint32_t gap10;
    uint32_t gap14;
    uint32_t GPTMIMR;      /* 0x18 GPTM Interrupt Mask */
    uint32_t GPTMRIS;      /* 0x1C GPTM Raw Interrupt Status */
    uint32_t GPTMMIS;      /* 0x20 GPTM Masked Interrupt Status */
    uint32_t GPTMICR;      /* 0x24 GPTM Interrupt Clear */
    uint32_t GPTMTAILR;    /* 0x28 GPTM TimerA Interval Load */
    uint32_t GPTMTBILR;    /* 0x2C GPTM TimerB Interval Load */
    uint32_t GPTMTAMATCHR; /* 0x30 GPTM TimerA Match */
    uint32_t GPTMTBMATCHR; /* 0x34 GPTM TimerB Match */
    uint32_t GPTMTAPR;     /* 0x38 GPTM TimerA Prescale */
    uint32_t GPTMTBPR;     /* 0x3C GPTM TimerB Prescale */
    uint32_t GPTMTAPM;     /* 0x40 GPTM TimerA Prescale Match */
    uint32_t GPTMTBPMR;    /* 0x44 GPTM TimerB Prescale Match */
    uint32_t GPTMTAR;      /* 0x48 GPTM Timer A */
    uint32_t GPTMTBR;      /* 0x4C GPTM Timer B */
    uint32_t GPTMTAV;      /* 0x50 GPTM Timer A Value */
    uint32_t GPTMTBV;      /* 0x54 GPTM Timer B Value */
    uint32_t GPTMRTCPD;    /* 0x58 GPTM RTC Predivide */
    uint32_t GPTMTAPS;     /* 0x5C GPTM Timer A Prescale Snapshot  */
    uint32_t GPTMTBPS;     /* 0x60 GPTM Timer B Prescale Snapshot  */
    uint32_t GPTMAPV;      /* 0x64 GPTM Timer A Prescale Value */
    uint32_t GPTMBPV;      /* 0x68 GPTM Timer B Prescale Value */
    uint32_t GPTMDMAEV;    /* 0x6C GPTM DMA Event */
    uint32_t GPTMADCEV;    /* 0x70 GPTM ADC Event */
    uint32_t rsvd[979];
    uint32_t GPTMPP;       /* 0xFC0 GPTM Peripheral Properties */
    uint32_t gapFC4;
    uint32_t GPTMCC;       /* 0xFC8 GPTM clock configuration */
} ti_catalog_arm_peripherals_timers_TimerRegsM4;

#endif
