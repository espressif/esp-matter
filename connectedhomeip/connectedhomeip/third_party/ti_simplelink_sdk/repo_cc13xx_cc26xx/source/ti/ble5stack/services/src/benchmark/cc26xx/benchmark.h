/******************************************************************************

 @file  benchmark.h

 @brief utility function to easily do benchmarking and output data on SWO.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef BENCHMARK_H
#define BENCHMARK_H

#ifdef __cplusplus
extern "C" {
#endif

#define BENCHMARK

// ==> READ THIS
// in order for this to work, the timestamp provider module must be included in RTOS:
// xdc.useModule('ti.sysbios.family.arm.m3.TimestampProvider');
#include <ti/sysbios/family/arm/m3/TimestampProvider.h>
#include "hal_gpio_wrapper.h"
/* ------------------------------------------------------------------------------------------------
 *                                           Typedefs
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                           Constant
 * ------------------------------------------------------------------------------------------------
 */
static int *STCSR = (int *)0xE000E010;
static int *STRVR = (int *)0xE000E014;
static int *STCVR = (int *)0xE000E018;
static int *DWT_CTRL = (int *)0xE0001000;

#define ENTER_CHAN         1
#define EXIT_CHAN          2
#define REL_CHAN           3
#define VAR1_CHAN          4

#define ID_POS             24
#define ID_MASK            0x7F

/* 128 IDs are available, list/reserved them here */
#define BM_TEST                 0x1
#define SBL_THREAD_INIT         0x2
#define BM_GENERIC_HWI          0x3
#define BM_GENERIC_SWI          0x4
#define BM_LL_RFCALLBACK        0x5
#define BM_PWR_CALIB_HWI        0x6
#define BM_RF_FSM_PWR_UP        0xA
#define BM_RF_FSM_SETUP         0xB
#define BM_RF_FSM_ACTIVE        0xC
#define BM_RF_HWI_CPE0_FSM      0xE
#define BM_RF_HWI_CPE0_ACTIVE   0xF



/* ------------------------------------------------------------------------------------------------
 *                                           Macro
 * ------------------------------------------------------------------------------------------------
 */

#define ITM_Port32(n) (*((volatile unsigned int *)(0xE0000000+4*n)))

#ifdef BENCHMARK

#define OUTPUT_VAR(chan, value) (ITM_Port32(chan) = (uint32_t) value);


// Those two Macros report the systick(cycles) value at the time it is call.
// the difference between the 'enter' value and the 'leave' value give the cycle it takes to run through the delimited 'zone'.
// note that this will include any HWI/SWI done in the meantime.
#define BM_ENTER_ZONE_ABS(id) OUTPUT_VAR(ENTER_CHAN, ((id & ID_MASK)<<ID_POS) | (*STCVR));

#define BM_LEAVE_ZONE_ABS(id)  OUTPUT_VAR(EXIT_CHAN, ((id & ID_MASK)<<ID_POS) | (*STCVR));

#define BM_ENTER_ZONE_ABS_SYNC(id) { \
                                     while(ITM_Port32(ENTER_CHAN) == 0x00); \
                                     BM_ENTER_ZONE_ABS(id); \
                                   }

#define BM_LEAVE_ZONE_ABS_SYNC(id) { \
                                     while(ITM_Port32(EXIT_CHAN) == 0x00); \
                                     BM_LEAVE_ZONE_ABS(id); \
                                   }

// Those two Macros report the difference of systick(cycles) value from the 'enter' to the leave' call.
// 'enter' and 'leave'  MUST be in the sanme function, since value are stored in the runtime stack.
#define BM_ENTER_ZONE_REL(id) { \
                                uint32_t temp1; \
                                uint32_t temp2 = 0; \
                                temp1 = *STCVR; \


#define BM_LEAVE_ZONE_REL(id) temp2 = *STCVR; \
                              OUTPUT_VAR(REL_CHAN, ((id & ID_MASK)<<ID_POS) | \
                              (temp2>temp1?temp2-temp1:temp1-temp2)); \
                              }
#define BM_LEAVE_ZONE_REL_SYNC( id) temp2 = *STCVR; \
                                while(ITM_Port32(REL_CHAN) == 0x00); \
                                OUTPUT_VAR(REL_CHAN, ((id & ID_MASK)<<ID_POS) | \
                                (temp2>temp1?temp2-temp1:temp1-temp2)); \
                              }

/* those 2 macros return a scpefici parmeter value over SWO. */
#define BM_VAR1(id, value) OUTPUT_VAR(VAR1_CHAN, ((id & ID_MASK)<<ID_POS) | value);


#define BM_VAR1_SYNC(id, value) { \
                                     while(ITM_Port32(VAR1_CHAN) == 0x00); \
                                     BM_VAR1(id, value); \
                                   }


static void BM_EnableSysTick( void )
{
    // Configure Systick
    *STRVR = 0xFFFFFE;  // max count
    *STCVR = 0;         // force a re-load of the counter value register
    *STCSR = 5;         // enable FCLK count without interrupt

    // enable SWO
#ifdef CC2650_LAUNCHXL
    // Launchpad
    //  IOCPortConfigureSet(IOID_16, IOC_PORT_MCU_SWV,
    //                    IOC_IOMODE_NORMAL);  /// ==> TDO pin for cJTAG for Launchpad
    IOCPortConfigureSet(IOID_29, IOC_PORT_MCU_SWV,
                        IOC_IOMODE_NORMAL);  /// ==> DIO_29 pin for IJET JTAG for Launchpad

#else
    // EM
    IOCPortConfigureSet(IOID_29, IOC_PORT_MCU_SWV,
                        IOC_IOMODE_NORMAL);  /// ==> RF2.14 on SRF06B
#endif

    // Output Test Pattern on SWO.
    ITM_Port32(ENTER_CHAN) = 0xAA;
    while(ITM_Port32(ENTER_CHAN) == 0x00);
    ITM_Port32(EXIT_CHAN) = 0xBB;
    while(ITM_Port32(EXIT_CHAN) == 0x00);
    ITM_Port32(REL_CHAN) = 0xCC;
    while(ITM_Port32(REL_CHAN) == 0x00);
    ITM_Port32(VAR1_CHAN) = 0xDD;
    while(ITM_Port32(VAR1_CHAN) == 0x00);

    // I/O to test accuracy. 4800000 cycles == 100ms
    HAL_GPIO_INIT();
    BM_ENTER_ZONE_REL(BM_TEST)
    {
      volatile uint32_t j = 533340; // Roughly equal to 100ms
      HWREGB( GPIO_BASE + HAL_GPIO_6 ) = 1;
      BM_ENTER_ZONE_ABS(BM_TEST);
      asm("NOP");
      while(j--)
      {
        asm("NOP");
      }
      asm("NOP");
      BM_LEAVE_ZONE_ABS_SYNC(BM_TEST);
      HWREGB( GPIO_BASE + HAL_GPIO_6 ) = 0;
    }
    BM_LEAVE_ZONE_REL(BM_TEST)

}
#else
static void BM_EnableSysTick( void ) {}

#define BM_ENTER_ZONE_ABS
#define BM_LEAVE_ZONE_ABS
#define BM_VAR1
#define BM_VAR2
#define BM_ENTER_ZONE_REL
#define BM_LEAVE_ZONE_REL

#define BM_ENTER_ZONE_ABS_SYNC
#define BM_LEAVE_ZONE_ABS_SYNC
#define BM_VAR1_SYNC
#define BM_VAR2_SYNC
#define BM_ENTER_ZONE_REL_SYNC
#define BM_LEAVE_ZONE_REL_SYNC

#endif

#ifdef __cplusplus
}
#endif

#endif /* BENCHMARK_H */
