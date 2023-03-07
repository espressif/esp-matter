/******************************************************************************

 @file  mb_patch.c

 @brief This file contains the data structures and APIs for CC26xx doorbell.

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

/*******************************************************************************
 * INCLUDES
 */

#include "hal_mcu.h"
#include "hal_assert.h"
#include "mb.h"
//
#include "r2f_common_flash_jt.h"
#include "r2r_common_flash_jt.h"

#ifdef USE_ICALL
#include <icall.h>
#else /* USE_ICALL */
#include <ti/sysbios/hal/Hwi.h>
#endif /* USE_ICALL */

// SW Tracer
#ifdef DEBUG_SW_TRACE
#define DBG_ENABLE
#include "dbgid_sys_mst.h"
#endif // DEBUG_SW_TRACE

/*******************************************************************************
 * MACROS
 */

//#define USE_RAW_CS 1

/*******************************************************************************
 * CONSTANTS
 */

// Mailbox RF Interrupt Types
#define NUM_MAILBOX_INTERRUPTS         4

// Miscellaneous
#define INVALID_TASK_ID                0xFF

#ifdef DEBUG
#define MB_ASSERT(cond) {volatile uint8 i = (cond); while(!i);}
#else // !DEBUG
// Note: Use HALNODEBUG to eliminate HAL assert handling (i.e. no assert).
// Note: If HALNODEBUG is not used, use ASSERT_RESET to reset system on assert.
//       Otherwise, evaluation board hazard lights are used.
// Note: Unused input parameter possible when HALNODEBUG; PC-Lint error 715.
#define MB_ASSERT(cond) HAL_ASSERT(cond)
#endif // DEBUG

/*******************************************************************************
 * TYPEDEFS
 */

typedef struct
{
  uint8  mbFree;                       // True: MB is free, False: MB busy.
  uint8  taskID;                       // Task ID to notify (if used).
  uint32 eventID;                      // Task Event ID of notification (if used).
} mbState_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

// Note: for FLASH_ROM_BUILD, forced to use RAM already allocated for CommonROM.
#if defined(__IAR_SYSTEMS_ICC__)

#if defined( FLASH_ONLY_BUILD )
__no_init mbIntCback_t mbIntCbackTable[ NUM_MAILBOX_INTERRUPTS ];
#else // FLASH_ROM_BUILD
__no_init mbIntCback_t mbIntCbackTable[ NUM_MAILBOX_INTERRUPTS ] @ 0x20004FEC;
#endif // FLASH_ONLY_BUILD

#elif defined(__TI_COMPILER_VERSION) || defined (__TI_COMPILER_VERSION__)

#if defined( FLASH_ROM_BUILD )
#pragma location = 0x20004FEC;
#endif // FLASH_ROM_BUILD

mbIntCback_t mbIntCbackTable[ NUM_MAILBOX_INTERRUPTS ];

#endif // __IAR_SYSTEMS_ICC__

/*******************************************************************************
 * PROTOTYPES
 */

void MB_DisableInts( void );

/******************************************************************************/

/*
** Mailbox Local Functions
*/

/*
** Mailbox Application Programming Interface
*/

#if defined( USE_RAW_CS )
uint32 llEnterCS_Raw( void )
{
  uint32 primask;

  // R0-R3 are scratch registers!
  __asm("mrs r0, PRIMASK");                   // get current PRIMASK
  __asm("mov %0, r0" : "=r"(primask) );       // save it locally
  __asm("cpsid i");                           // disable interrupts

  return( primask );
}

void llExitCS_Raw( uint32 primask )
{
  // R0-R3 are scratch registers!
  __asm("mov r0, %0" : : "r"(primask) );      // copy parameter to register
  __asm("msr PRIMASK, r0");                   // and restore PRIMASK

  return;
}
#endif // USE_RAW_CS


/*******************************************************************************
 * @fn          MB_Init
 *
 * @brief       This function is used to initialize the Mailbox interface. It
 *              masks and clears all interrupts, clears the CPE and HW jump
 *              table, and disables the mailbox command acknowledge interrupt.
 *
 *              Note: If the ACK interrupt is needed, it can be enabled and
 *                    a user ISR can be registered for processing.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void MB_Init( void )
{
  uint8 i;

  // disable and clear all interrupts
  MAP_MB_DisableInts();

  // clear the Mailbox interrupt callback table
  for (i=0; i<NUM_MAILBOX_INTERRUPTS; i++)
  {
    // clear the entry
    mbIntCbackTable[i] = NULL;
  }

  // register default Mailbox ISRs
#ifdef USE_ICALL
  ICall_registerISR_Ext(INT_RFC_CPE_1,    MAP_mbCpe1Isr, 0x80);
  ICall_registerISR_Ext(INT_RFC_CPE_0,    MAP_mbCpe0Isr, 0x80);
  ICall_registerISR_Ext(INT_RFC_HW_COMB,      MAP_mbHwIsr, 0x80);
  ICall_registerISR_Ext(INT_RFC_CMD_ACK, MAP_mbCmdAckIsr, 0x80);
#else /* USE_ICALL */
  Hwi_create(INT_RFC_CPE1, (Hwi_FuncPtr) MAP_mbCpe1Isr, NULL, NULL);
  Hwi_create(INT_RFC_CPE0, (Hwi_FuncPtr) MAP_mbCpe0Isr, NULL, NULL);
  Hwi_create(INT_RFC_HW_COMB, (Hwi_FuncPtr) MAP_mbHwIsr, NULL, NULL);
  Hwi_create(INT_RFC_CMD_ACK, (Hwi_FuncPtr) MAP_mbCmdAckIsr, NULL, NULL);
#endif /* USE_ICALL */

  return;
}


/*******************************************************************************
 * @fn          MB_EnableInts
 *
 * @brief       This function is used to map CPE 0 and 1, and HW interrupts,
 *              and clears/unmasks them. These interrupts are then enabled.
 *
 * input parameters
 *
 * @param       cpe0Ints - Bit map of CPE 0 interrupts.
 * @param       cpe1Ints - Bit map of CPE 1 interrupts.
 * @param       hwInts   - Bit map of HW interrupts.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void MB_EnableInts( uint32 cpe0Ints, uint32 cpe1Ints, uint32 hwInts )
{
#if USE_RAW_CS
  uint32 primask;

  primask = llEnterCS_Raw();
#else // use RTOS CS
  halIntState_t cs;

  HAL_ENTER_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  // disable all Mailbox interrupts
  MAP_MB_DisableInts();

  // map bit map of all corresponding interrupts to CPE 0
  MB_RFCPEISL_REG &= ~cpe0Ints;

  // map bit map of all corresponding interrupts to CPE 1
  MB_RFCPEISL_REG |= cpe1Ints;

  // unmask all corresponding interrupts for either CPE 0 or 1
  MB_RFCPEIEN_REG |= (cpe0Ints | cpe1Ints);

  // unmask all corresponding interrupts for HW
  MB_RFHWIEN_REG |= hwInts;

  // enable Doorbell CPE interrupts
#ifdef USE_ICALL
  ICall_enableInt( INT_RFC_CMD_ACK );
  ICall_enableInt( INT_RFC_CPE_0 );
  ICall_enableInt( INT_RFC_CPE_1 );
  ICall_enableInt( INT_RFC_HW_COMB );
#else /* USE_ICALL */
  Hwi_enableInterrupt( INT_RFC_CMD_ACK );
  Hwi_enableInterrupt( INT_RFC_CPE_0 );
  Hwi_enableInterrupt( INT_RFC_CPE_1 );
  Hwi_enableInterrupt( INT_RFC_HW_COMB );
#endif /* USE_ICALL */

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return;
}


/*******************************************************************************
 * @fn          MB_DisableInts
 *
 * @brief       This function is used to disable and clear all CPE 0 and 1,
 *              and HW interrupts.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void MB_DisableInts( void )
{
#if USE_RAW_CS
  uint32 primask;

  primask = llEnterCS_Raw();
#else // use RTOS CS
  halIntState_t cs;

  HAL_ENTER_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  // disable Doorbell CPE and HW interrupts
#ifdef USE_ICALL
  ICall_disableInt( INT_RFC_CPE_0 );
  ICall_disableInt( INT_RFC_CPE_1 );
  ICall_disableInt( INT_RFC_HW_COMB );
  ICall_disableInt( INT_RFC_CMD_ACK );
#else /* USE_ICALL */
  Hwi_disableInterrupt( INT_RFC_CPE_0 );
  Hwi_disableInterrupt( INT_RFC_CPE_1 );
  Hwi_disableInterrupt( INT_RFC_HW_COMB );
  Hwi_disableInterrupt( INT_RFC_CMD_ACK );
#endif /* USE_ICALL */

  // mask RF CPE interrupts
  MB_RFCPEIEN_REG = 0;

  // mask RF HW interrupts
  MB_RFHWIEN_REG = 0;

  // clear RF CPE interrupts
  // Note: Bug in PG1 and PG2 where clear may not take place on first write.
  MB_RFCPEIFG_REG = 0;
  MB_RFCPEIFG_REG = 0;

  // clear RF HW interrupts
  // Note: Bug in PG1 and PG2 where clear may not take place on first write.
  MB_RFHWIFG_REG = 0;
  MB_RFHWIFG_REG = 0;

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return;
}


/*******************************************************************************
 * @fn          MB_ClearInts
 *
 * @brief       This function is used to clear all unmasked CPE/HW interrupts.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void MB_ClearInts( void )
{
#if USE_RAW_CS
  uint32 primask;

  primask = llEnterCS_Raw();
#else // use RTOS CS
  halIntState_t cs;

  HAL_ENTER_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  // clear RF CPE interrupts
  // Note: Bug in PG1 and PG2 where clear may not take place on first write.
  MB_RFCPEIFG_REG = 0;
  MB_RFCPEIFG_REG = 0;

  // clear RF HW interrupts
  // Note: Bug in PG1 and PG2 where clear may not take place on first write.
  MB_RFHWIFG_REG  = 0;
  MB_RFHWIFG_REG  = 0;

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return;
}
/*******************************************************************************
 * @fn          MB_EnableHWInts
 *
 * @brief       This function is used to map only HW interrupts, and
 *              clears/unmasks them. These interrupts are then enabled.
 *
 * input parameters
 *
 * @param       hwInts - Bit map of HW interrupts.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void MB_EnableHWInts( uint32 hwInts )
{
#if USE_RAW_CS
  uint32 primask;

  primask = llEnterCS_Raw();
#else // use RTOS CS
  halIntState_t cs;

  HAL_ENTER_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  // disable Doorbell HW interrupts
#ifdef USE_ICALL
  ICall_disableInt( INT_RFC_HW_COMB );
#else /* USE_ICALL */
  Hwi_disableInterrupt( INT_RFC_HW_COMB );
#endif /* USE_ICALL */

  // mask RF HW interrupts
  MB_RFHWIEN_REG = 0;

  // clear RF HW interrupts
  MB_RFHWIFG_REG = 0;
  MB_RFHWIFG_REG = 0;

  // clear RF HW interrupts
  // Note: Bug in PG1 and PG2 where clear may not take place on first write.
  MB_RFHWIEN_REG |= hwInts;

  // enable Doorbell HW interrupts
#ifdef USE_ICALL
  ICall_enableInt( INT_RFC_HW_COMB );
#else /* USE_ICALL */
  Hwi_enableInterrupt( INT_RFC_HW_COMB );
#endif /* USE_ICALL */

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return;
}


/*******************************************************************************
 * @fn          MB_SendCommand
 *
 * @brief       This function is used to send a command to the Mailbox
 *              interface, then poll for the the command status to change
 *              indicating that the command has completed (in the case of an
 *              Immediate or Direct command), or the command has been accepted
 *              for execution (in the case of a Radio command).
 *
 *              Note that this routine only returns the command status result.
 *              If the command status return bytes are required, they can be
 *              obtained using MB_ReadMailboxStatus.
 *
 *              Note: This command can safely execute from a critical section.
 *
 * input parameters
 *
 * @param       radioCmd - Immediate, Direct, or Radio command.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      A CMDSTA value, or CMDSTA_MAILBOX_BUSY.
 */
uint8 MB_SendCommand( uint32 radioCmd )
{
  // check that the Mailbox is not busy
  if ( MB_CMDR_REG != CMDSTA_PENDING )
  {
    return( CMDSTA_MAILBOX_BUSY );
  }

#if USE_RAW_CS
  uint32 primask;

  primask = llEnterCS_Raw();
#else // use RTOS CS
  halIntState_t cs;

  HAL_ENTER_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  // issue the command to the Mailbox interface
  MB_CMDR_REG = radioCmd;

  // wait for the the command status value to change by using the ACK interrupt
  // Note: Bug in PG1 and PG2 where clear may not take place on first write.
  while( !MB_RFACKIFG_REG );
  MB_RFACKIFG_REG = 0;
  MB_RFACKIFG_REG = 0;

#ifdef DEBUG
  // trap and dump problem in CM0 code
  if ( MB_RFCPEIFG_REG & (uint32)MB_INTERNAL_ERROR )
  {
    MAP_MB_FwDebugDump();
    MB_ASSERT( FALSE );
  }
#endif // DEBUG

  // check for any unexpected Mailbox error
  MB_ASSERT( (MB_CMDSTA_REG & 0xFF) == CMDSTA_DONE );

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return( MB_CMDSTA_REG & 0xFF );
}


/*******************************************************************************
 * @fn          MB_SendCommandSynch
 *
 * @brief       This function can be used to issue an RFHAL command via the
 *              Mailbox interface. This function allows the user to issue a
 *              RFHAL command completely synchronously, without concern for
 *              interrupt processing. This function therefore assumes, in the
 *              case of Radio Commands, that no other radio command is pending.
 *
 *              Before the command is issued, all unmasked CPE/HW interrupts
 *              are saved, and all interrupts are masked. After the command is
 *              issued, the command status is polled until a status change has
 *              occurred, indicating that the command has either completed in
 *              the case of an Immediate or Direct command, or the command has
 *              been accepted for execution in the case of a Radio command.
 *              If the command is a Radio Command, and the Mailbox command
 *              status completed successfully, the Last Command Done interrupt
 *              is polled, and the Last Command and Last Command Done interrupts
 *              are cleared (whether or not they were unmasked by the user).
 *              In all cases, all unmasked interrupts are cleared, the user's
 *              mask register is restored, and the Mailbox command status is
 *              returned.
 *
 *              WARNING: Radio commands MUST have an end trigger or timeout
 *                       trigger that will eventually end or this command will
 *                       hang waiting for a Command Done or Last Command Done
 *                       interrupt that never occurs.
 *
 *              WARNING: This routine assumes there is no other radio command
 *                       pending.
 *
 *              Note: This routine only returns the command status result.
 *                    If the command status return bytes are required, they
 *                    can be obtained using MB_ReadMailboxStatus.
 *
 *              Note: This command can safely execute from a critical section.
 *
 * input parameters
 *
 * @param       rfCmd - Immediate, Direct, or Radio command.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      A CMDSTA value, or CMDSTA_MAILBOX_BUSY.
 */
uint8 MB_SendCommandSynch( uint32 rfCmd )
{
  uint32 cpeIntMask;
  uint32 hwIntMask;

  // check that the Mailbox is not busy
  if ( MB_CMDR_REG != CMDSTA_PENDING )
  {
    return( CMDSTA_MAILBOX_BUSY );
  }

#if USE_RAW_CS
  uint32 primask;

  primask = llEnterCS_Raw();
#else // use RTOS CS
  halIntState_t cs;

  HAL_ENTER_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  // this routine assumes there is no other command pending
  MB_ASSERT( (MB_RFCPEIFG_REG &
             (MB_COMMAND_DONE_INT | MB_LAST_COMMAND_DONE_INT)) == 0);

  // get the currently unmasked CPE and HW interrupts
  cpeIntMask = MB_RFCPEIEN_REG;
  hwIntMask  = MB_RFHWIEN_REG;

  // mask the interrupts
  MB_RFCPEIEN_REG = 0;
  MB_RFHWIEN_REG  = 0;

  // ensure an ACK isn't already pending
  // Note: If the ACK were pending, we end up trapping an assert below as the
  //       we zip past the checks and end up with a CMDSTA of zero because the
  //       command actually isn't finished yet.
  MB_ASSERT( !MB_RFACKIFG_REG );

  // issue the command to the Mailbox interface
  MB_CMDR_REG = rfCmd;

  // wait for the the command status value to change by using the ACK interrupt
  // Note: Bug in PG1 and PG2 where clear may not take place on first write.
  while( !MB_RFACKIFG_REG );
  MB_RFACKIFG_REG = 0;
  MB_RFACKIFG_REG = 0;

  // check if this was a radio command
  // Note: If the least significant bit is not set, then radioCmd is a pointer
  //       to a command structure, the first halfword of which is the opcode.
  //       Bits 11..9 specify the type of command, where Radio commands are
  //       indicated by b1XX. See rfHal.h for command format.
  if ( !(rfCmd & 0x01) && (*((uint16 *)rfCmd) & 0x0800) )
  {
    // ensure that the Mailbox command completed successfully
    // Note: A radio command will never start if the Mailbox command failed.
    if ( MB_CMDSTA_REG == CMDSTA_DONE )
    {
      // poll last command done interrupt
      // Note: A radio command was issued, and there wasn't a Mailbox error,
      //       so wait for the command to end. It is assumed here that the
      //       user has NOT started a command that will never end!
      while( !(MB_RFCPEIFG_REG & MB_LAST_COMMAND_DONE_INT) );

      // clear Last Command and Last Command Done interrupts
      // Note: This is done whether the user had these interrupts unmasked!
      // Note: Bug in PG1 and PG2 where clear may not take place on first write.
      MB_RFCPEIFG_REG = ~(MB_COMMAND_DONE_INT | MB_LAST_COMMAND_DONE_INT);
      MB_RFCPEIFG_REG = ~(MB_COMMAND_DONE_INT | MB_LAST_COMMAND_DONE_INT);
    }
  }

  // check for any unexpected Mailbox error
  MB_ASSERT( (MB_CMDSTA_REG & 0xFF) == CMDSTA_DONE );

  // finished, so clear all unmasked interrupts
  // Note: Bug in PG1 and PG2 where clear may not take place on first write.
  MB_RFCPEIFG_REG = ~cpeIntMask;
  MB_RFCPEIFG_REG = ~cpeIntMask;
  MB_RFHWIFG_REG  = ~hwIntMask;
  MB_RFHWIFG_REG  = ~hwIntMask;

  // restore the interrupt mask
  MB_RFCPEIEN_REG = cpeIntMask;
  MB_RFHWIEN_REG  = hwIntMask;

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return( MB_CMDSTA_REG & 0xFF );
}


/*
** Mailbox Interrupt Service Routines
*/


/*******************************************************************************
 * @fn          Mailbox Command Processor Engine 0 ISR
 *
 * @brief       This ISR handles the Mailbox Command Processing Engine 0
 *              interrupt, or transfers control to a registered Callback to
 *              handle it.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void mbCpe0Isr( void )
{
  CLEAR_SLEEP_MODE();

  // check if there's a registered handler for this interrupt
  if ( mbIntCbackTable[ RF_CPE_0_INTERRUPT ] == NULL )
  {
    halIntState_t cs;
    uint32        intClear;

    HAL_ENTER_CRITICAL_SECTION(cs);

    // build mask to clear CPE0 interrupts
    // Note: The local variable and order of operations are required to avoid
    //       IAR compiler warning Pa082 that the order of volatile access is
    //       undefined.
    intClear  =  MB_RFCPEIEN_REG;
    intClear &= ~MB_RFCPEISL_REG;

    // clear any originally unmasked CPE0 interrupts that may now be pending
    // Note: Bug in PG1 and PG2 where clear may not take place on first write.
    MB_RFCPEIFG_REG = ~intClear;
    MB_RFCPEIFG_REG = ~intClear;

    HAL_EXIT_CRITICAL_SECTION(cs);
  }
  else // there's a registered handler
  {
    // so use it
    (mbIntCbackTable[ RF_CPE_0_INTERRUPT ])();
  }

  return;
}


/*******************************************************************************
 * @fn          Mailbox Command Processor Engine 1 ISR
 *
 * @brief       This ISR handles the Mailbox Command Processing Engine 1
 *              interrupt, or transfers control to a registered Callback to
 *              handle it.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void mbCpe1Isr( void )
{
  CLEAR_SLEEP_MODE();

  // check if there's a registered handler for this interrupt
  if ( mbIntCbackTable[ RF_CPE_1_INTERRUPT ] == NULL )
  {
    halIntState_t cs;
    uint32        intClear;

    HAL_ENTER_CRITICAL_SECTION(cs);

    // build mask to clear CPE0 interrupts
    // Note: The local variable and order of operations are required to avoid
    //       IAR compiler warning Pa082 that the order of volatile access is
    //       undefined.
    intClear  = MB_RFCPEIEN_REG;
    intClear &= MB_RFCPEISL_REG;

    // clear any originally unmasked CPE1 interrupts that may now be pending
    // Note: Bug in PG1 and PG2 where clear may not take place on first write.
    MB_RFCPEIFG_REG = ~intClear;
    MB_RFCPEIFG_REG = ~intClear;

    HAL_EXIT_CRITICAL_SECTION(cs);
  }
  else // there's a registered handler
  {
    // so use it
    (mbIntCbackTable[ RF_CPE_1_INTERRUPT ])();
  }

  return;
}


/*******************************************************************************
 * @fn          Mailbox Hardware ISR
 *
 * @brief       This ISR handles the Mailbox Hardware interrupt, or transfers
 *              control to a registered Callback to handle it.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void mbHwIsr( void )
{
  CLEAR_SLEEP_MODE();

  // check if there's a registered handler for this interrupt
  if ( mbIntCbackTable[ RF_HW_INTERRUPT ] == NULL )
  {
    halIntState_t cs;
    uint32        intClear;

    HAL_ENTER_CRITICAL_SECTION(cs);

    // build mask to clear CPE0 interrupts
    // Note: The local variable and order of operations are required to avoid
    //       IAR compiler warning Pa082 that the order of volatile access is
    //       undefined.
    intClear = MB_RFHWIEN_REG;

    // clear any originally unmasked HW interrupts that may now be pending
    // Note: Bug in PG1 and PG2 where clear may not take place on first write.
    MB_RFHWIFG_REG = ~intClear;
    MB_RFHWIFG_REG = ~intClear;

    HAL_EXIT_CRITICAL_SECTION(cs);
  }
  else // there's a registered handler
  {
    // so use it
    (mbIntCbackTable[ RF_HW_INTERRUPT ])();
  }

  return;
}


/*******************************************************************************
 * @fn          Mailbox Command Acknowledge ISR
 *
 * @brief       This ISR handles the Mailbox Command Acknowledge interrupt, or
 *              transfers control to a registered Callback to handle it.
 *
 *              Note: The Mailbox Command ACK interrupt can not be masked.
 *              Note: There may be additional radio interrupts that may result.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void mbCmdAckIsr( void )
{
  CLEAR_SLEEP_MODE();

  // always clear the interrupt
  // Note: Bug in PG1 and PG2 where clear may not take place on first write.
  MB_RFACKIFG_REG = 0;
  MB_RFACKIFG_REG = 0;

  // check if there's a registered handler for this interrupt
  if ( mbIntCbackTable[ RF_CMD_ACK_INTERRUPT ] == NULL )
  {
    // nothing to clear, nothing to do
    return;
  }
  else // there's a registered handler
  {
    // so use it
    (mbIntCbackTable[ RF_CMD_ACK_INTERRUPT ])();
  }

  return;
}

/*******************************************************************************
 */


