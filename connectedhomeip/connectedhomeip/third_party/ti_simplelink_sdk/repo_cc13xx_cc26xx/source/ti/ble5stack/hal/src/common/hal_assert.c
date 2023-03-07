/******************************************************************************

 @file  hal_assert.c

  @brief This file contains the Hardware Abstraction Layer (HAL) Assert APIs
         used to handle asserts in system software. The assert handler, and
         its behavior, depend on the build time define (please see header file).

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2006-2022, Texas Instruments Incorporated
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

#include "hal_assert.h"
#include "hal_types.h"
#include "hal_board.h"
#include "hal_defs.h"
#include "onboard.h"

#if (defined HAL_MCU_AVR)    || (defined HAL_MCU_CC2430) || \
    (defined HAL_MCU_CC2530) || (defined HAL_MCU_CC2533) || \
    (defined HAL_MCU_MSP430)
  /* for access to debug data */
#include "mac_rx.h"
#include "mac_tx.h"
#endif

/*******************************************************************************
 * MACROS
 */

// Compile Time Assertions - Integrity check of type sizes.
HAL_ASSERT_SIZE(  int8, 1);
HAL_ASSERT_SIZE( uint8, 1);
HAL_ASSERT_SIZE( int16, 2);
HAL_ASSERT_SIZE(uint16, 2);
HAL_ASSERT_SIZE( int32, 4);
HAL_ASSERT_SIZE(uint32, 4);

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

// HAL Assert callback pointer
assertCback_t halAssertCback = (assertCback_t)halAssertSpinlock;

// optional general purpose subcause - the value dpepends on assert cause.
uint8 assertSubcause = HAL_ASSERT_SUBCAUSE_NONE;

// used to decide how FALSE is handled (see halAssertHandlerExt)
uint8 legacyMode = HAL_ASSERT_LEGACY_MODE_ENABLED;


/*******************************************************************************
 * @fn          halAssertInit API
 *
 * @brief       This function is used by the Stack code to override the build-
 *              time initialization values of assert Callback and Legacy Mode,
 *              thus allowing a project to change the defaults without having
 *              to change the common (shared) header file hal_assert.h. The
 *              default values are: NULL and HAL_ASSERT_LEGACY_MODE_ENABLED.
 *
 * input parameters
 *
 * @param       initAssertCback: Pointer to HAL Assert callback.
 * @param       initLegacyMode:  HAL_ASSERT_LEGACY_MODE_ENABLED |
 *                               HAL_ASSERT_LEGACY_MODE_DISABLED
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halAssertInit( assertCback_t initAssertCback, uint8 initLegacyMode )
{
  // check if the callback pointer is invalid
  if ( initAssertCback == NULL )
  {
    // set the assert callback pointer to a valid handler
    halAssertCback = (assertCback_t)halAssertSpinlock;
  }
  else // callback pointer is valid
  {
    // set the assert callback pointer
    halAssertCback = initAssertCback;
  }

  // set the legacyMode
  legacyMode = initLegacyMode;

  return;
}


/*******************************************************************************
 * @fn          halAssertHandler API
 *
 * @brief       This function is to trap software execution. The assert action
 *              depends on the defines:
 *
 *              HAL_ASSERT_RESET  - Reset the device.
 *              HAL_ASSERT_LIGHTS - Flash the LEDs.
 *              HAL_ASSERT_SPIN   - Spinlock.
 *              Otherwise:        - Just return.
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
void halAssertHandler( void )
{
#if defined( HAL_ASSERT_RESET )
  SystemReset();
#elif defined ( HAL_ASSERT_LIGHTS )
  halAssertHazardLights();
#elif defined( HAL_ASSERT_SPIN )
  halAssertSpinlock();
#endif

  return;
}


/*******************************************************************************
 * @fn          halAssertHandlerExt API
 *
 * @brief       This function is the Extended halAssertHandler, and is backward
 *              compatible with the original halAssertHandler. The assert
 *              causes can be TRUE and FALSE, per usual, or a value from
 *              1..0xFF (see header file for possible assert cause values).
 *              When FALSE, the legacy mode (a global defined as part of the
 *              call to HAL_ASSERT_Init) will determine whether the original
 *              halAssertHandler call is used or the registered callback (if
 *              any) is used.
 *
 * input parameters
 *
 * @param       assertCause: TRUE, FALSE, 1..0xFF.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halAssertHandlerExt( uint8 assertCause )
{
  // check if there is no cause
  if ( (assertCause == FALSE) &&
       (legacyMode == HAL_ASSERT_LEGACY_MODE_ENABLED) )
  {
    halAssertHandler();

    return;
  }

  // evoke registered callback
  // Note: The pointer halAssertCback will never be NULL due to defaults.
  // Note: The value of assertSubcause, if used, should be set before the call
  //       to HAL_ASSERT. For example:
  // HAL_ASSERT_SET_SUBCAUSE( HAL_ASSERT_SUBCAUSE_FW_INERNAL_ERROR );
  // HAL_ASSERT( HAL_ASSERT_CAUSE_INTERNAL_ERROR );
  halAssertCback( assertCause, assertSubcause );

  // call base HAL Assert handler in case callback handler didn't trap
  halAssertHandler();

  return;
}

/*******************************************************************************
 * @fn          halAssertSpinlock API
 *
 * @brief       This function is to trap software execution.
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
void halAssertSpinlock( void )
{
#ifdef HAL_ASSERT_SPIN
  volatile uint8 i = 1;

  HAL_DISABLE_INTERRUPTS();
  while(i);
  HAL_ENABLE_INTERRUPTS();
#endif // HAL_ASSERT_SPIN

  return;
}

#if !defined ASSERT_WHILE
/*******************************************************************************
 * @fn          halAssertHazardLights API
 *
 * @brief       Blink LEDs to indicate an error.
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
void halAssertHazardLights(void)
{
  enum
  {
    DEBUG_DATA_RSTACK_HIGH_OFS,
    DEBUG_DATA_RSTACK_LOW_OFS,
    DEBUG_DATA_TX_ACTIVE_OFS,
    DEBUG_DATA_RX_ACTIVE_OFS,

#if (defined HAL_MCU_AVR) || (defined HAL_MCU_CC2430)
    DEBUG_DATA_INT_MASK_OFS,
#elif (defined HAL_MCU_CC2530) || (defined HAL_MCU_CC2533)
    DEBUG_DATA_INT_MASK0_OFS,
    DEBUG_DATA_INT_MASK1_OFS,
#endif

    DEBUG_DATA_SIZE
  };

  uint8 buttonHeld;
  uint8 debugData[DEBUG_DATA_SIZE];

  /* disable all interrupts before anything else */
  HAL_DISABLE_INTERRUPTS();

  /*----------------------------------------------------------------------------
   *  Initialize LEDs and turn them off.
   */
  HAL_BOARD_INIT();

  HAL_TURN_OFF_LED1();
  HAL_TURN_OFF_LED2();
  HAL_TURN_OFF_LED3();
  HAL_TURN_OFF_LED4();

  /*----------------------------------------------------------------------------
   *  Master infinite loop.
   */
  for (;;)
  {
    buttonHeld = 0;

    /*--------------------------------------------------------------------------
     *  "Hazard lights" loop.  A held keypress will exit this loop.
     */
    do
    {
      HAL_LED_BLINK_DELAY();

      /* toggle LEDS, the #ifdefs are in case HAL has logically remapped
         non-existent LEDs */
#if (HAL_NUM_LEDS >= 1)
      HAL_TOGGLE_LED1();
#if (HAL_NUM_LEDS >= 2)
      HAL_TOGGLE_LED2();
#if (HAL_NUM_LEDS >= 3)
      HAL_TOGGLE_LED3();
#if (HAL_NUM_LEDS >= 4)
      HAL_TOGGLE_LED4();
#endif
#endif
#endif
#endif

      /* escape hatch to continue execution, set escape to '1' to
         continue execution */
      {
        static uint8 escape = 0;
        if (escape)
        {
          escape = 0;
          return;
        }
      }

      /* break out of loop if button is held long enough */
      if (HAL_PUSH_BUTTON1())
      {
        buttonHeld++;
      }
      else
      {
        buttonHeld = 0;
      }
    }
    while (buttonHeld != 10); /* loop until button is held specified # times */

    /*--------------------------------------------------------------------------
     *  Just exited from "hazard lights" loop.
     */

    /* turn off all LEDs */
    HAL_TURN_OFF_LED1();
    HAL_TURN_OFF_LED2();
    HAL_TURN_OFF_LED3();
    HAL_TURN_OFF_LED4();

    /* wait for button release */
    HAL_DEBOUNCE(!HAL_PUSH_BUTTON1());

    /*--------------------------------------------------------------------------
     *  Load debug data into memory.
     */
#ifdef HAL_MCU_AVR
    {
      uint8 * pStack;
      pStack = (uint8 *) SP;
      pStack++; /* point to return address on stack */
      debugData[DEBUG_DATA_RSTACK_HIGH_OFS] = *pStack;
      pStack++;
      debugData[DEBUG_DATA_RSTACK_LOW_OFS] = *pStack;
    }
    debugData[DEBUG_DATA_INT_MASK_OFS] = EIMSK;
#endif

#if (defined HAL_MCU_CC2430)
    debugData[DEBUG_DATA_INT_MASK_OFS] = RFIM;
#elif (defined HAL_MCU_CC2530) || (defined HAL_MCU_CC2533)
    debugData[DEBUG_DATA_INT_MASK0_OFS] = RFIRQM0;
    debugData[DEBUG_DATA_INT_MASK1_OFS] = RFIRQM1;
#endif


#if (defined HAL_MCU_AVR)    || (defined HAL_MCU_CC2430) || \
    (defined HAL_MCU_CC2530) || (defined HAL_MCU_CC2533) || \
    (defined HAL_MCU_MSP430)
    debugData[DEBUG_DATA_TX_ACTIVE_OFS] = macTxActive;
    debugData[DEBUG_DATA_RX_ACTIVE_OFS] = macRxActive;
#endif

   /* initialize for data dump loop */
    {
      uint8 iBit;
      uint8 iByte;

      iBit  = 0;
      iByte = 0;

      /*------------------------------------------------------------------------
       *  Data dump loop.  A button press cycles data bits to an LED.
       */
      while (iByte < DEBUG_DATA_SIZE)
      {
        while(!HAL_PUSH_BUTTON1())
        {
            // wait for key press
        };

        /* turn on all LEDs for first bit of byte, turn on three LEDs if
          not first bit */
        HAL_TURN_ON_LED1();
        HAL_TURN_ON_LED2();
        HAL_TURN_ON_LED3();
        if (iBit == 0)
        {
          HAL_TURN_ON_LED4();
        }
        else
        {
          HAL_TURN_OFF_LED4();
        }

        /* wait for debounced key release */
        HAL_DEBOUNCE(!HAL_PUSH_BUTTON1());

        /* turn off all LEDs */
        HAL_TURN_OFF_LED1();
        HAL_TURN_OFF_LED2();
        HAL_TURN_OFF_LED3();
        HAL_TURN_OFF_LED4();

        /* output value of data bit to LED1 */
        if (debugData[iByte] & (1 << (7 - iBit)))
        {
          HAL_TURN_ON_LED1();
        }
        else
        {
          HAL_TURN_OFF_LED1();
        }

        /* advance to next bit */
        iBit++;
        if (iBit == 8)
        {
          iBit = 0;
          iByte++;
        }
      }
    }

    /*
     *  About to enter "hazard lights" loop again.  Turn off LED1 in case the
     *  last bit displayed happened to be one.  This guarantees all LEDs are
     *  off at the start of the flashing loop which uses a toggle operation to
     *  change LED states.
     */
    HAL_TURN_OFF_LED1();
  }
}
#endif // !ASSERT_WHILE

/*******************************************************************************
*/
