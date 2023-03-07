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

#include <xdc/std.h>

typedef volatile struct ti_catalog_arm_peripherals_timers_TimerRegs {
    UInt32 GPTMCFG;     /* 0x00 GPTM Config */
    UInt32 GPTMTAMR;    /* 0x04 GPTM TimerA Mode */
    UInt32 GPTMTBMR;    /* 0x08 GPTM TimerB Mode */
    UInt32 GPTMCTL;     /* 0x0C GPTM Control */
    UInt32 gap10;
    UInt32 gap14;
    UInt32 GPTMIMR;     /* 0x18 GPTM Interrupt Mask */
    UInt32 GPTMRIS;     /* 0x1C GPTM Raw Interrupt Status */
    UInt32 GPTMMIS;     /* 0x20 GPTM Masked Interrupt Status */
    UInt32 GPTMICR;     /* 0x24 GPTM Interrupt Clear */
    UInt32 GPTMTAILR;   /* 0x28 GPTM TimerA Interval Load */
    UInt32 GPTMTBILR;   /* 0x2C GPTM TimerB Interval Load */
    UInt32 GPTMTAMATCHR; /* 0x30 GPTM TimerA Match */
    UInt32 GPTMTBMATCHR; /* 0x34 GPTM TimerB Match */
    UInt32 GPTMTAPR;    /* 0x38 GPTM TimerA Prescale */
    UInt32 GPTMTBPR;    /* 0x3C GPTM TimerB Prescale */
    UInt32 GPTMTAPM;    /* 0x40 GPTM TimerA Prescale Match */
    UInt32 GPTMTBPMR;   /* 0x44 GPTM TimerB Prescale Match */
    UInt32 GPTMTAR;     /* 0x48 GPTM Timer A */
    UInt32 GPTMTBR;     /* 0x4C GPTM Timer B */
} ti_catalog_arm_peripherals_timers_TimerRegs;

typedef volatile struct ti_catalog_arm_peripherals_timers_TimerRegsM4 {
    UInt32 GPTMCFG;      /* 0x00 GPTM Config */
    UInt32 GPTMTAMR;     /* 0x04 GPTM TimerA Mode */
    UInt32 GPTMTBMR;     /* 0x08 GPTM TimerB Mode */
    UInt32 GPTMCTL;      /* 0x0C GPTM Control */
    UInt32 gap10;
    UInt32 gap14;
    UInt32 GPTMIMR;      /* 0x18 GPTM Interrupt Mask */
    UInt32 GPTMRIS;      /* 0x1C GPTM Raw Interrupt Status */
    UInt32 GPTMMIS;      /* 0x20 GPTM Masked Interrupt Status */
    UInt32 GPTMICR;      /* 0x24 GPTM Interrupt Clear */
    UInt32 GPTMTAILR;    /* 0x28 GPTM TimerA Interval Load */
    UInt32 GPTMTBILR;    /* 0x2C GPTM TimerB Interval Load */
    UInt32 GPTMTAMATCHR; /* 0x30 GPTM TimerA Match */
    UInt32 GPTMTBMATCHR; /* 0x34 GPTM TimerB Match */
    UInt32 GPTMTAPR;     /* 0x38 GPTM TimerA Prescale */
    UInt32 GPTMTBPR;     /* 0x3C GPTM TimerB Prescale */
    UInt32 GPTMTAPM;     /* 0x40 GPTM TimerA Prescale Match */
    UInt32 GPTMTBPMR;    /* 0x44 GPTM TimerB Prescale Match */
    UInt32 GPTMTAR;      /* 0x48 GPTM Timer A */
    UInt32 GPTMTBR;      /* 0x4C GPTM Timer B */
    UInt32 GPTMTAV;      /* 0x50 GPTM Timer A Value */
    UInt32 GPTMTBV;      /* 0x54 GPTM Timer B Value */
    UInt32 GPTMRTCPD;    /* 0x58 GPTM RTC Predivide */
    UInt32 GPTMTAPS;     /* 0x5C GPTM Timer A Prescale Snapshot  */
    UInt32 GPTMTBPS;     /* 0x60 GPTM Timer B Prescale Snapshot  */
    UInt32 GPTMAPV;      /* 0x64 GPTM Timer A Prescale Value */
    UInt32 GPTMBPV;      /* 0x68 GPTM Timer B Prescale Value */
    UInt32 GPTMDMAEV;    /* 0x6C GPTM DMA Event */
    UInt32 GPTMADCEV;    /* 0x70 GPTM ADC Event */
    UInt32 rsvd[979];
    UInt32 GPTMPP;       /* 0xFC0 GPTM Peripheral Properties */
    UInt32 gapFC4;
    UInt32 GPTMCC;       /* 0xFC8 GPTM clock configuration */
} ti_catalog_arm_peripherals_timers_TimerRegsM4;

#endif
