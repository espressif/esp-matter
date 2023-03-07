/******************************************************************************

 @file  mb.c

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
#include "icall.h"

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

#if defined( COMMON_ROM_BUILD )
// ROM Revision Number
// Note: Value based on SVN revision number.
#pragma section = ".rom_rev"
const uint16 romRev @ ".rom_rev" = 37956;

#pragma section = ".ramInitTable"
ICall_Dispatcher ICall_dispatcher @ ".ramInitTable";
ICall_EnterCS    ICall_enterCriticalSection @ ".ramInitTable";
ICall_LeaveCS    ICall_leaveCriticalSection @ ".ramInitTable";
//
uint32 R2F_FlashJT @ ".ramInitTable";
uint32 R2R_FlashJT @ ".ramInitTable";
#endif // ROM_BUILD

//
// ROM BUILD NOTE
// NOTE: This table is redefined in mb_PATCH since it was defined in ROM
//       statically. When it's symbol was imported, it was marked as Local,
//       and thus couldn't be used by the flash build. To get around this, the
//       MB_Init routine was also moved into flash, and the table was redefined
//       in RAM (a waste of 16 bytes of ROM RAM that will never be used). As
//       a result, this table needs to be commented out for FlashOnly builds
//       to prevent a duplicate error, and an extern must be used to resolve
//       any reference to this table in this file.
//
//       When a ROM respin occurs, this table should NOT be defined statically.
//
//static mbIntCback_t mbIntCbackTable[ NUM_MAILBOX_INTERRUPTS ];
extern mbIntCback_t mbIntCbackTable[];

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


#ifdef PATCH_ROM_PATCH
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

#ifdef DEBUG
  // Sanity Check:
  // When the Mailbox is ready to receive a command, the Mailbox Command
  // register should be zero.
  MB_ASSERT( MB_CMDR_REG == CMDSTA_PENDING );
#endif // DEBUG

  // clear the Mailbox interrupt callback table
  for (i=0; i<NUM_MAILBOX_INTERRUPTS; i++)
  {
    // clear the entry
    mbIntCbackTable[i] = NULL;
  }

  // register default Mailbox ISRs
  ICall_registerISR(INT_RF_CPE1,    MAP_mbCpe1Isr);
  ICall_registerISR(INT_RF_CPE0,    MAP_mbCpe0Isr);
  ICall_registerISR(INT_RF_HW,      MAP_mbHwIsr);
  ICall_registerISR(INT_RF_CMD_ACK, MAP_mbCmdAckIsr);

  return;
}
#endif // PATCH_ROM_PATCH


#ifdef PATCH_ROM_PATCH
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
  ICall_enableInt( INT_RF_CMD_ACK );
  ICall_enableInt( INT_RF_CPE0 );
  ICall_enableInt( INT_RF_CPE1 );
  ICall_enableInt( INT_RF_HW );

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return;
}
#endif // PATCH_ROM_PATCH


#ifdef PATCH_ROM_PATCH
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
  ICall_disableInt( INT_RF_CPE0 );
  ICall_disableInt( INT_RF_CPE1 );
  ICall_disableInt( INT_RF_HW );
  ICall_disableInt( INT_RF_CMD_ACK );

  // mask RF CPE interrupts
  MB_RFCPEIEN_REG = 0;

  // mask RF HW interrupts
  MB_RFHWIEN_REG = 0;

  // clear RF CPE interrupts
  MB_RFCPEIFG_REG = 0;

  // clear RF HW interrupts
  MB_RFHWIFG_REG = 0;

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return;
}
#endif // PATCH_ROM_PATCH


#ifdef PATCH_ROM_PATCH
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

  // clear all unmasked interrupts
  MB_RFCPEIFG_REG = 0;
  MB_RFHWIFG_REG  = 0;

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return;
}
#endif // PATCH_ROM_PATCH


#ifdef PATCH_ROM_PATCH
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
  ICall_disableInt( INT_RF_HW );

  // mask RF HW interrupts
  MB_RFHWIEN_REG = 0;

  // clear RF HW interrupts
  MB_RFHWIFG_REG = 0;

  // unmask all corresponding interrupts for HW
  MB_RFHWIEN_REG |= hwInts;

  // enable Doorbell HW interrupts
  ICall_enableInt( INT_RF_HW );

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return;
}
#endif // PATCH_ROM_PATCH


#ifdef PATCH_ROM_PATCH
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
  while( !MB_RFACKIFG_REG );
  MB_RFACKIFG_REG = 0;

#ifdef DEBUG
  // trap and dump problem in CM0 code
  if ( MB_RFCPEIFG_REG & (uint32)MB_INTERNAL_ERROR )
  {
    MAP_MB_FwDebugDump();
    MB_ASSERT( FALSE );
  }
#endif // DEBUG

  MB_ASSERT( (MB_CMDSTA_REG & 0xFF) == CMDSTA_DONE );

#if USE_RAW_CS
  llExitCS_Raw( primask );
#else // use RTOS CS
  HAL_EXIT_CRITICAL_SECTION( cs );
#endif // USE_RAW_CS

  return( MB_CMDSTA_REG & 0xFF );
}
#endif // PATCH_ROM_PATCH


#ifdef PATCH_ROM_PATCH
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

  // issue the command to the Mailbox interface
  MB_CMDR_REG = rfCmd;

  // wait for the the command status value to change by using the ACK interrupt
  while( !MB_RFACKIFG_REG );
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
      MB_RFCPEIFG_REG = ~(MB_COMMAND_DONE_INT | MB_LAST_COMMAND_DONE_INT);
    }
  }

  MB_ASSERT( (MB_CMDSTA_REG & 0xFF) == CMDSTA_DONE );

  // finished, so clear all unmasked interrupts
  MB_RFCPEIFG_REG = ~cpeIntMask;
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
#endif // PATCH_ROM_PATCH


/*******************************************************************************
 * @fn          MB_ReadMailboxStatus
 *
 * @brief       This function is used to return the full Mailbox command status
 *              register's contents.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      The full 32 bit contents of the command status register, where
 *              byte 0 is the result of the last command issued, and bytes 1..3
 *              are returned byte 1..3 (provided by certain Immedate commands).
 */
uint32 MB_ReadMailboxStatus( void )
{

  return( MB_CMDSTA_REG );
}


/*******************************************************************************
 * @fn          MB_RegisterIsrCback
 *
 * @brief       This routine registers a Mailbox Interrupt Callback for one
 *              of the Mailbox interrupts.
 *
 * input parameters
 *
 * @param       mbIntType  - Interrupt for associated Callback function.
 * @param       mbIntCback - Interrupt Callback function.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void MB_RegisterIsrCback( uint8 mbIntType, mbIntCback_t mbIntCback )
{
  // check the Mailbox interrupt type
  mbIntCbackTable[ mbIntType ] = mbIntCback;

  return;
}


/*******************************************************************************
 * @fn          Mailbox Firmware Debug Dump
 *
 * @brief       This routine, which is only used when debugging, dumps the
 *              stack contents of the Cortex M0 FW. It is called when
 *              bit 31 of RFCPEIFG is set to indicate an Internal Error has
 *              occurred.
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
void MB_FwDebugDump( void )
{
#ifdef DEBUG_SW_TRACE
  uint32 *cpeStackPointer = *((uint32 **)RFC_RAM_BASE);

  DBG_PRINTL1( DBGSYS, "CPE reported internal error. SP: 0x%08x. Stack contents:", (uint32) cpeStackPointer);

  DBG_PRINTL2( DBGSYS, "R0:   0x%08X  R1:   0x%08X", cpeStackPointer[0], cpeStackPointer[1]);

  DBG_PRINTL2( DBGSYS, "R2:   0x%08X  R3:   0x%08X", cpeStackPointer[2], cpeStackPointer[3]);

  DBG_PRINTL2( DBGSYS, "R4:   0x%08X  R5:   0x%08X", cpeStackPointer[-4], cpeStackPointer[-3]);

  DBG_PRINTL2( DBGSYS, "R6:   0x%08X  R7:   0x%08X", cpeStackPointer[-2], cpeStackPointer[-1]);

  DBG_PRINTL2( DBGSYS, "R12:  0x%08X  LR:   0x%08X", cpeStackPointer[4], cpeStackPointer[5]);

  DBG_PRINTL2( DBGSYS, "PC:   0x%08X  xPSR: 0x%08X", cpeStackPointer[6], cpeStackPointer[7]);
#endif // DEBUG_SW_TRACE

  return;
}


/*
** Mailbox Interrupt Service Routines
*/

#ifdef PATCH_ROM_PATCH
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
#endif // PATCH_ROM_PATCH


#ifdef PATCH_ROM_PATCH
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
    MB_RFCPEIFG_REG &= ~intClear;

    HAL_EXIT_CRITICAL_SECTION(cs);
  }
  else // there's a registered handler
  {
    // so use it
    (mbIntCbackTable[ RF_CPE_0_INTERRUPT ])();
  }

  return;
}
#endif // PATCH_ROM_PATCH


#ifdef PATCH_ROM_PATCH
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
    MB_RFCPEIFG_REG &= ~intClear;

    HAL_EXIT_CRITICAL_SECTION(cs);
  }
  else // there's a registered handler
  {
    // so use it
    (mbIntCbackTable[ RF_CPE_1_INTERRUPT ])();
  }

  return;
}
#endif // PATCH_ROM_PATCH


#ifdef PATCH_ROM_PATCH
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
    MB_RFHWIFG_REG &= ~intClear;

    HAL_EXIT_CRITICAL_SECTION(cs);
  }
  else // there's a registered handler
  {
    // so use it
    (mbIntCbackTable[ RF_HW_INTERRUPT ])();
  }

  return;
}
#endif // PATCH_ROM_PATCH

/*******************************************************************************
 */


