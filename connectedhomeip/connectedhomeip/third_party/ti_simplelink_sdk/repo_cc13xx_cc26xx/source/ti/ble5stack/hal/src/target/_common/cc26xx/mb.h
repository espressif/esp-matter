/******************************************************************************

 @file  mb.h

 @brief This file contains the CC26xx Mailbox defines, macros, and typedefs to
        be used with the Mailbox API.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
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

#ifndef MB_H
#define MB_H

/*******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include <inc/hw_types.h>
#include <inc/hw_rfc_dbell.h>

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

#define MB_BASE                        RFC_DBELL_BASE

// Registers
#define MB_CMDR_REG                    HWREG(MB_BASE + RFC_DBELL_O_CMDR)
#define MB_CMDSTA_REG                  HWREG(MB_BASE + RFC_DBELL_O_CMDSTA)
#define MB_RFHWIFG_REG                 HWREG(MB_BASE + RFC_DBELL_O_RFHWIFG)
#define MB_RFHWIEN_REG                 HWREG(MB_BASE + RFC_DBELL_O_RFHWIEN)
#define MB_RFCPEIFG_REG                HWREG(MB_BASE + RFC_DBELL_O_RFCPEIFG)
#define MB_RFCPEIEN_REG                HWREG(MB_BASE + RFC_DBELL_O_RFCPEIEN)
#define MB_RFCPEISL_REG                HWREG(MB_BASE + RFC_DBELL_O_RFCPEISL)
#define MB_RFACKIFG_REG                HWREG(MB_BASE + RFC_DBELL_O_RFACKIFG)
#define MB_RFGPOCTRL_REG               HWREG(MB_BASE + RFC_DBELL_O_SYSGPOCTL)
#define MB_RFIDLESTA_REG               HWREG(MB_BASE + RFC_DBELL_O_RFCIDLESTAT)

// Mailbox Command Status Result (CMDSTA)
#define CMDSTA_PENDING                 0x00
#define CMDSTA_DONE                    0x01
#define CMDSTA_ILLEGAL_PTR             0x81
#define CMDSTA_UNKNOWN_CMD             0x82
#define CMDSTA_UNKNOWN_DIRECT_CMD      0x83
#define CMDSTA_CONTEXT_ERR             0x85
#define CMDSTA_SCH_ERR                 0x86
#define CMDSTA_PARAM_ERR               0x87
#define CMDSTA_QUEUE_ERR               0x88
#define CMDSTA_QUEUE_BUSY              0x89
#define CMDSTA_MAILBOX_BUSY            0xFF  // Software only status.

// Mailbox Interrupts
#define MB_COMMAND_DONE_INT            BV(0)
#define MB_LAST_COMMAND_DONE_INT       BV(1)
#define MB_FG_COMMAND_DONE_INT         BV(2)
#define MB_LAST_FG_COMMAND_DONE_INT    BV(3)

#if defined( CC26XX ) || defined( CC13XX )
#define MB_CM0_SYNTH_NO_LOCK           BV(28)
#define MB_CM0_MODULES_UNLOCKED        BV(29)
#define MB_CM0_BOOT_DONE               BV(30)
#endif // CC26XX/CC13XX
#define MB_INTERNAL_ERROR              BV(31)

// Hardware Interrupts
#define UNUSED_0                       BV(0)
#define FSCA_IRQ                       BV(1)
#define MODEM_DONE_ISR                 BV(2)
#define MODEM_FIFO_INPUT_IRQ           BV(3)
#define MODEM_FIFO_OUTPUT_IRQ          BV(4)
#define MODEM_SOFT_DEFINED_IRQ         BV(5)
#define RFC_TRACER_SYSTICK_IRQ         BV(6)
#define UNUSED_7                       BV(7)
#define RFE_DONE_IRQ                   BV(8)
#define RFE_SOFT_0_DEFINED_IRQ         BV(9)
#define RFE_SOFT_1_DEFINED_IRQ         BV(10)
#define RFE_SOFT_2_DEFINED_IRQ         BV(11)
#define RAT_CHAN_0_IRQ                 BV(12)
#define RAT_CHAN_1_IRQ                 BV(13)
#define RAT_CHAN_2_IRQ                 BV(14)
#define RAT_CHAN_3_IRQ                 BV(15)
#define RAT_CHAN_4_IRQ                 BV(16)
#define RAT_CHAN_5_IRQ                 BV(17)
#define RAT_CHAN_6_IRQ                 BV(18)
#define RAT_CHAN_7_IRQ                 BV(19)
#define UNUSED_20                      BV(20)
#define UNUSED_21                      BV(21)
#define UNUSED_22                      BV(22)
#define UNUSED_23                      BV(23)
#define UNUSED_24                      BV(24)
#define UNUSED_25                      BV(25)
#define UNUSED_26                      BV(26)
#define UNUSED_27                      BV(27)
#define UNUSED_28                      BV(28)
#define UNUSED_29                      BV(29)
#define UNUSED_30                      BV(30)
#define UNUSED_31                      BV(31)

/*
** Mailbox API Related
*/

// Mailbox Interrupt Types
#define RF_CMD_ACK_INTERRUPT           0
#define RF_CPE_0_INTERRUPT             1
#define RF_CPE_1_INTERRUPT             2
#define RF_HW_INTERRUPT                3

/*******************************************************************************
 * TYPEDEFS
 */
typedef void (*mbIntCback_t)(void);

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * FUNCTIONS
 */

extern void   MB_Init( void );

extern void   MB_EnableInts( uint32, uint32, uint32 );

extern void   MB_DisableInts( void );

extern void   MB_ClearInts( void );

extern void   MB_EnableHWInts( uint32 );

extern uint32 MB_ReadMailboxStatus( void );

extern uint8  MB_SendCommand( uint32 );

extern uint8  MB_SendCommandSynch( uint32 );

extern void   MB_RegisterIsrCback( uint8, mbIntCback_t );

extern void   MB_FwDebugDump( void );

// Mailbox ISRs
extern void   mbCmdAckIsr(void);

extern void   mbCpe0Isr( void );

extern void   mbCpe1Isr( void );

extern void   mbHwIsr( void );

#endif /* MB_H */
