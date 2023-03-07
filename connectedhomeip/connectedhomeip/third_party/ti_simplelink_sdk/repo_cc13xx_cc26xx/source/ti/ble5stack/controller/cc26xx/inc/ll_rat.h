/******************************************************************************

 @file  ll_rat.h

 @brief This file contains the Link Layer (LL) types, constants,
        API's etc. for the Bluetooth Low Energy (BLE) Controller
        Radio Access Timer (RAT).

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR.
 *
 */

#ifndef LL_RAT_H
#define LL_RAT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include <inc/hw_rfc_rat.h>

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

#define RAT_BASE                       RFC_RAT_BASE

// Registers
#define RAT_CONFIG_REG                 HWREG(RAT_BASE + RFC_RAT_O_RATCFG)
#define RAT_COUNT_REG                  HWREG(RAT_BASE + RFC_RAT_O_RATCNT)
#define RAT_ADJUST_REG                 HWREG(RAT_BASE + RFC_RAT_O_RATADJ)
#define RAT_ARM_CAPTURE_REG            HWREG(RAT_BASE + RFC_RAT_O_RATARMSET)
#define RAT_ARM_CLEAR_REG              HWREG(RAT_BASE + RFC_RAT_O_RATARMCLR)
#define RAT_OUT_EVT_REG                HWREG(RAT_BASE + RFC_RAT_O_RATEV)
#define RAT_OUT_MODE_CONFIG_REG        HWREG(RAT_BASE + RFC_RAT_O_RATOUT)
#define RAT_CHAN_0_CONFIG_REG          HWREG(RAT_BASE + RFC_RAT_O_RATCH0CFG)
#define RAT_CHAN_1_CONFIG_REG          HWREG(RAT_BASE + RFC_RAT_O_RATCH1CFG)
#define RAT_CHAN_2_CONFIG_REG          HWREG(RAT_BASE + RFC_RAT_O_RATCH2CFG)
#define RAT_CHAN_3_CONFIG_REG          HWREG(RAT_BASE + RFC_RAT_O_RATCH3CFG)
#define RAT_CHAN_4_CONFIG_REG          HWREG(RAT_BASE + RFC_RAT_O_RATCH4CFG)
#define RAT_CHAN_5_CONFIG_REG          HWREG(RAT_BASE + RFC_RAT_O_RATCH5CFG)
#define RAT_CHAN_6_CONFIG_REG          HWREG(RAT_BASE + RFC_RAT_O_RATCH6CFG)
#define RAT_CHAN_7_CONFIG_REG          HWREG(RAT_BASE + RFC_RAT_O_RATCH7CFG)
#define RAT_CHAN_0_CAPT_COMP_REG       HWREG(RAT_BASE + RFC_RAT_O_RATCH0VAL)
#define RAT_CHAN_1_CAPT_COMP_REG       HWREG(RAT_BASE + RFC_RAT_O_RATCH1VAL)
#define RAT_CHAN_2_CAPT_COMP_REG       HWREG(RAT_BASE + RFC_RAT_O_RATCH2VAL)
#define RAT_CHAN_3_CAPT_COMP_REG       HWREG(RAT_BASE + RFC_RAT_O_RATCH3VAL)
#define RAT_CHAN_4_CAPT_COMP_REG       HWREG(RAT_BASE + RFC_RAT_O_RATCH4VAL)
#define RAT_CHAN_5_CAPT_COMP_REG       HWREG(RAT_BASE + RFC_RAT_O_RATCH5VAL)
#define RAT_CHAN_6_CAPT_COMP_REG       HWREG(RAT_BASE + RFC_RAT_O_RATCH6VAL)
#define RAT_CHAN_7_CAPT_COMP_REG       HWREG(RAT_BASE + RFC_RAT_O_RATCH7VAL)

#define LL_MAX_32BIT_TIME_IN_625US     0x07A12000  // 32s in 625us ticks (LSTO limit)
#define LL_MAX_OVERLAP_TIME_LIMIT      0x7270E000  // 8 minutes in 625us ticks. chosen to be half of 17 minutes RF overlap.
#define LL_MAX_32BIT_TIME              0xFFFFFFFF

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * Functions
 */

extern uint32 llGetCurrentTime( void );

extern uint8 llTimeCompare( uint32 time1, uint32 time2 );

extern uint32 llTimeDelta( uint32 time1, uint32 time2 );

#ifdef __cplusplus
}
#endif

#endif /* LL_RAT_H */
