/******************************************************************************

 @file  key_scan.c

 @brief This file contains the key driver for a key matrix.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2005-2022, Texas Instruments Incorporated
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

/******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <string.h>
#include <inc/hw_ints.h>
#include "icall.h"

#include "Board.h"
#include "key_scan.h"
#include "util.h"

/******************************************************************************
 * MACROS
 ******************************************************************************/

/******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define KEY_RISING_EDGE                       0
#define KEY_FALLING_EDGE                      1

// shift register stabilization period in ms
#define SHIFT_REG_STABLIZATION_PERIOD         3

/******************************************************************************
 * TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 * VARIABLES
 ******************************************************************************/
// key process function
KeyTimerEvtCBack_t keyTimerEvtFunction;
KeyEvtCBack_t keyProcessFunction;
ShiftRegTimerEvtCBack_t ShiftRegTimerEvtFunction;

static uint8 keyNumKeyEventsPerNotification;
static uint8 keyKeyEventCount;
static bool newKeyPress;
static bool shiftRegStabilizationActive;
static uint8 colCode = KEY_NUM_COLUMNS;

// Debounce timer
static Clock_Struct keyScanDebounce;

// key repeat timer
static Clock_Struct keyScanRepeat;

// Clock instance for shift register startup
static Clock_Struct shiftRegClock;

// Key pin table
static PIN_Config keyScanPinTable[] =
{
  Board_KEY_PWR   | PIN_GPIO_OUTPUT_EN | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  Board_KEY_CLK   | PIN_GPIO_OUTPUT_EN | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  Board_KEY_SCAN  | PIN_GPIO_OUTPUT_EN | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};

// Key pin table
static PIN_Config keyColPinTable[] =
{
  Board_KEY_COL1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
  Board_KEY_COL2  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
  Board_KEY_COL3  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
  PIN_TERMINATE
};

// Key pin state
static PIN_State keyScanPinState;
static PIN_State keyColPinState;

// Key Pin Handle
PIN_Handle keyScanPinHandle, keyColPinHandle;

// Key press parameters
uint8_t keys_in;

// Initialize keys
void initScanKeys(void);

/******************************************************************************
 * FUNCTIONS - Local
 ******************************************************************************/

// Keypress callbacks
static void keyFxn(PIN_Handle keyPinHandle, PIN_Id keyPinId);
static void KeyReadColumns(void);
static uint8 KeyReadRows(void);
static void clockShiftRegister(void);
static void powerDownShiftRegister(void);
static void setShiftRegisterData(uint8 data);
static void powerUpShiftRegister(void);
static void disableKeyInterrupts(void);
static void enableKeyInterrupts(void);
static void configKeyPinPullup(void);
static void configKeyPinPulldown(void);
static void keyTimerEventCB(UArg a0);
static void shiftRegClockHandlerCB(UArg arg);
static void KeyPoll(void);

/******************************************************************************
 * FUNCTIONS - API
 ******************************************************************************/
/******************************************************************************
 * @fn      keyInit
 *
 * @brief   Initialize Key Service and Key Scan Clocks. This function call must
 *          be followed with a valid call to KeyConfig().
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void KeyInit(void)
{
  // setup key process function
  keyTimerEvtFunction = KeyReadColumns;

  // setup shift register process function
  ShiftRegTimerEvtFunction = KeyPoll;

  //Initialize default debounce timer, with default 10ms
  Util_constructClock(&keyScanDebounce, keyTimerEventCB,
                      10, 0, false, NULL);

  // setup default repeat interval timer, with default 20ms
  Util_constructClock(&keyScanRepeat, keyTimerEventCB,
                      20, 0, false, NULL);

  // setup default shift register startup timer
  Util_constructClock(&shiftRegClock, shiftRegClockHandlerCB,
                      SHIFT_REG_STABLIZATION_PERIOD, 0, false, NULL);

  // Initialize scan keys
  initScanKeys();

  /* The advanced remote doesn't have the same 8X8 row/column matrix as in other
   * products. Instead, a 3x11 row/column matrix is used, with the columns
   * continuing to be utilized by GPIOs, but the rows are generated via a 16
   * bit shift register. Controls for the shift register are, however, utilized
   * with GPIOs.
   */

  // Set up initial value on output pins
  powerDownShiftRegister();
}

/******************************************************************************
 * @fn      keyConfig
 *
 * @brief   Configure the key service
 *
 * @param   key_cback - callback function to be called when a key event occurs
 *                      key_cback is called as a Util_clock object which means:
 *                      1. keep key_cback as short as possible
 *                      2. no RTOS blocking function calls are permitted
 *          initialKeyRepeatInterval - time in ms between each repeat key event
 *          debounceTime - time in ms to wait for key debounce
 *          pollRate - time in ms for repeatedly scanning the keys
 *
 * @return  None.
 ******************************************************************************/
void KeyConfig(KeyEvtCBack_t key_cback, uint16_t initialKeyRepeatInterval,
               uint16_t debounceTime, uint16_t pollRate)
{
  keyNumKeyEventsPerNotification = initialKeyRepeatInterval / pollRate;
  keyKeyEventCount = 0;

  // setup key process function
  keyProcessFunction = key_cback;

  // setup debounce timer
  Util_restartClock(&keyScanDebounce, debounceTime);

  // setup repeat interval timer
  Util_restartClock(&keyScanRepeat, pollRate);

}

/******************************************************************************
 * @fn      KeyReadColumns
 *
 * @brief   Read the current value of a key's column
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void KeyReadColumns(void)
{
  uint8 keys;

  /* Disable interrupts, as interrupts can be triggered without key press during
   * scanning process
   */
  disableKeyInterrupts();

  // Detect column first while all rows are still asserted.
  keys = PIN_getInputValue(Board_KEY_COL1)
       | PIN_getInputValue(Board_KEY_COL2) << 1
       | PIN_getInputValue(Board_KEY_COL3) << 2;

  for (colCode = 0; colCode < KEY_NUM_COLUMNS; colCode++)
  {
    if (((1 << colCode) & keys) == 0)
    {
      break;
    }
  }

  /* Start process to check the rows. Since the pin supplying power to the
   * shift register can't power all the KPa outputs, we will change the KPb
   * pins to have pull down registers, which will result in all KPb pins
   * reading 0. We will then shift 1 through the shift register, and the column
   * that is pressed will show up as a 1 on the corresponding KPb pin.
   */
  configKeyPinPulldown();
  powerUpShiftRegister();

  // Wait for shift register to settle after power up
  if (!Util_isActive(&shiftRegClock))
  {
    shiftRegStabilizationActive = TRUE;
    Util_startClock(&shiftRegClock);
  }
}

/******************************************************************************
 * @fn      KeyReadRows
 *
 * @brief   Read the current value of a key's row
 *
 * @param   None.
 *
 * @return  keys - current keys value
 ******************************************************************************/
static uint8 KeyReadRows(void)
{
  uint8 keys;
  uint8 rowCode;

  // Must first shift in a single 0 in order for KPa0 to function correctly.
  setShiftRegisterData(0);
  clockShiftRegister();

  // Check each row by shifting a 1 through the shift register
  setShiftRegisterData(1);
  clockShiftRegister();
  setShiftRegisterData(0);

  for (rowCode = 0; rowCode < KEY_NUM_ROWS; rowCode++)
  {
    // read all columns
    keys = PIN_getInputValue(Board_KEY_COL1)
         | PIN_getInputValue(Board_KEY_COL2) << 1
         | PIN_getInputValue(Board_KEY_COL3) << 2;

    // de-assert the column
    clockShiftRegister();

    // check whether the specific row was on.
    if (keys & ((uint8)1 << colCode))
    {
      break;
    }
  }

  // assert all columns for interrupt and for lower current consumption
  powerDownShiftRegister();

  if ((colCode == KEY_NUM_COLUMNS) || (rowCode == KEY_NUM_ROWS))
  {
    keys = KEY_CODE_NOKEY; // no key pressed
  }
  else
  {
    keys = (colCode << 4) | rowCode;
  }

  // change KPb pins back to pull up
  configKeyPinPullup();

  // re-enable key interrupt
  enableKeyInterrupts();

  // return key value
  return keys;
}

/******************************************************************************
 * @fn      KeyPoll
 *
 * @brief   Called by application to poll the keys and determine whether a key
 *          event should be posted to application
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void KeyPoll(void)
{
  uint8 keys = KeyReadRows();

  if (keys == KEY_CODE_NOKEY)
  {
    // if no key is press, stop timer
    if (Util_isActive(&keyScanRepeat))
    {
      Util_stopClock(&keyScanRepeat);
    }
  }

  // Increment polling counter and see if it's time to report
  keyKeyEventCount++;
  if ((keyKeyEventCount >= keyNumKeyEventsPerNotification) ||
      (keys == KEY_CODE_NOKEY) ||
      (newKeyPress == TRUE))
  {
    keyKeyEventCount = 0;
    newKeyPress = FALSE;

    // Invoke Callback if it exists
    if (keyProcessFunction)
    {
      keyProcessFunction(keys);
    }
  }
}

/******************************************************************************
 * @fn      clockShiftRegister
 *
 * @brief   Simply provides a single clock pulse to the shift register.
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void clockShiftRegister(void)
{
  PIN_setOutputValue(keyScanPinHandle, Board_KEY_CLK, 1);
  PIN_setOutputValue(keyScanPinHandle, Board_KEY_CLK, 0);
}

/******************************************************************************
 * @fn      powerDownShiftRegister
 *
 * @brief   Disables the shift register to save power.
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void powerDownShiftRegister(void)
{
  PIN_setOutputValue(keyScanPinHandle, Board_KEY_SCAN, 0);
  PIN_setOutputValue(keyScanPinHandle, Board_KEY_PWR, 0);
  PIN_setOutputValue(keyScanPinHandle, Board_KEY_CLK, 0);
}

/******************************************************************************
 * @fn      powerUpShiftRegister
 *
 * @brief   Supplies power to the shift register.
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void powerUpShiftRegister(void)
{
  PIN_setOutputValue(keyScanPinHandle, Board_KEY_PWR, 1);
}

/******************************************************************************
 * @fn      setShiftRegisterData
 *
 * @brief   Writes data to the input of the shift register.
 *
 * @param   data - input data
 *
 * @return  None.
 ******************************************************************************/
void setShiftRegisterData(uint8 data)
{
  // Data input is 1 bit, so make sure we only use LSB
  PIN_setOutputValue(keyScanPinHandle, Board_KEY_SCAN, (data & 0x01));
}

/******************************************************************************
 * @fn      disableKeyInterrupts
 *
 * @brief   Disables the column keys interrupt flags.
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void disableKeyInterrupts(void)
{
  PIN_setInterrupt(keyColPinHandle, Board_KEY_COL1|PIN_IRQ_DIS);
  PIN_setInterrupt(keyColPinHandle, Board_KEY_COL2|PIN_IRQ_DIS);
  PIN_setInterrupt(keyColPinHandle, Board_KEY_COL3|PIN_IRQ_DIS);
}

/******************************************************************************
 * @fn      enableKeyInterrupts
 *
 * @brief   Enables the column keys interrupt flags.
 *
 * @param   none
 *
 * @return
 ******************************************************************************/
void enableKeyInterrupts(void)
{
  PIN_setInterrupt(keyColPinHandle, Board_KEY_COL1|PIN_IRQ_NEGEDGE);
  PIN_setInterrupt(keyColPinHandle, Board_KEY_COL2|PIN_IRQ_NEGEDGE);
  PIN_setInterrupt(keyColPinHandle, Board_KEY_COL3|PIN_IRQ_NEGEDGE);
}

/******************************************************************************
 * @fn      configKeyPinPulldown
 *
 * @brief   Configures pull down register on column pins.
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void configKeyPinPulldown(void)
{
  PIN_setConfig(keyColPinHandle, PIN_BM_PULLING, Board_KEY_COL1|PIN_PULLDOWN);
  PIN_setConfig(keyColPinHandle, PIN_BM_PULLING, Board_KEY_COL2|PIN_PULLDOWN);
  PIN_setConfig(keyColPinHandle, PIN_BM_PULLING, Board_KEY_COL3|PIN_PULLDOWN);
}

/******************************************************************************
 * @fn      configKeyPinPullup
 *
 * @brief   Configures pull down register on column pins.
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void configKeyPinPullup(void)
{
  PIN_setConfig(keyColPinHandle, PIN_BM_PULLING, Board_KEY_COL1|PIN_PULLUP);
  PIN_setConfig(keyColPinHandle, PIN_BM_PULLING, Board_KEY_COL2|PIN_PULLUP);
  PIN_setConfig(keyColPinHandle, PIN_BM_PULLING, Board_KEY_COL3|PIN_PULLUP);
}

/******************************************************************************
 * @fn      initScanKeys
 *
 * @brief   function to enable interrupts for keys on GPIOs
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void initScanKeys(void)
{
  // Setup KEY ISR
  keyScanPinHandle = PIN_open(&keyScanPinState, keyScanPinTable);
  keyColPinHandle = PIN_open(&keyColPinState, keyColPinTable);

  // Register Callbacks
  PIN_registerIntCb(keyColPinHandle, keyFxn);
}

/*********************************************************************
 * @fn      shiftRegClockHandlerCB
 *
 * @brief   Shift register clock time-out handler function
 *
 * @param   arg - ignored
 *
 * @return  None.
 */
void shiftRegClockHandlerCB(UArg arg)
{
  shiftRegStabilizationActive = FALSE;

  if (ShiftRegTimerEvtFunction)
  {
    ShiftRegTimerEvtFunction();
  }
}

/******************************************************************************
 * @fn      keyTimerEventCB
 *
 * @brief   Key Timer event handler function
 *
 * @param   a0 - ignored
 *
 * @return  None.
 ******************************************************************************/
void keyTimerEventCB(UArg a0)
{
  /* In order to trigger a timer event at proper intervals without delay due to
     application doing key poll, we must reload the timer asap, so do it here,
     immediately after the timer event. If no more keys are found to be pressed
     during a key poll, the timer will be canceled.
  */
  if (!shiftRegStabilizationActive)
  {
    if (!Util_isActive(&keyScanRepeat))
    {
      Util_startClock(&keyScanRepeat);
    }

    // Invoke application to process timer event
    if (keyTimerEvtFunction)
    {
      keyTimerEvtFunction();
    }
  }
}

/******************************************************************************
 * @fn      keyFxn
 *
 * @brief   Interrupt handler for a Key press
 *
 * @param   keyPinHandle - ignored
 *          keyPinId - pin ID of key matrix columns
 *
 * @return  None.
 ******************************************************************************/
void keyFxn(PIN_Handle keyPinHandle, PIN_Id keyPinId)
{
  (void)keyPinHandle; // Intentionally unreferenced parameter

  // start debounce timer
  if (!Util_isActive(&keyScanDebounce))
  {
    Util_startClock(&keyScanDebounce);
  }

  newKeyPress = TRUE;
}

/******************************************************************************
 ******************************************************************************/
