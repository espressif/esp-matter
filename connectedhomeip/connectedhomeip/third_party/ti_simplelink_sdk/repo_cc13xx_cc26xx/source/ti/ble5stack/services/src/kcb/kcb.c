/******************************************************************************

 @file  kcb.c

 @brief Key Callback Services

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
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
 */
#include "hal_types.h"
#include "hal_board.h"
#include "kcb.h"
#include "key_scan.h"

/******************************************************************************
 * MACROS
 */

/******************************************************************************
 * CONSTANTS
 */

// Remote keymap
#define KCB_NUM_RC_KEYS 48
static const rcKeyId_t kcbKeyMap[KCB_NUM_RC_KEYS] =
{
  // 0b00 <column-number> <row-number>
  // row mapped    to P0
  // column mapped to P1

  // column 1
  /*0b00 0000*/ RC_SW_PWR,
  /*0b00 0001*/ RC_SW_VOL_UP,
  /*0b00 0010*/ RC_SW_MENU,
  /*0b00 0011*/ RC_SW_UP_ARW,
  /*0b00 0100*/ RC_SW_RT_ARW,
  /*0b00 0101*/ RC_SW_PAIR,
  /*0b00 0110*/ RC_SW_DIG_3,
  /*0b00 0111*/ RC_SW_DIG_6,
  /*0b00 1000*/ RC_SW_DIG_9,
  /*0b00 1001*/ RC_SW_RECORD,
  /*0b00 1010*/ RC_SW_NEXT,
  /*0b00 1011*/ RC_SW_NA,
  /*0b00 1100*/ RC_SW_NA,
  /*0b00 1101*/ RC_SW_NA,
  /*0b00 1110*/ RC_SW_NA,
  /*0b00 1111*/ RC_SW_NA,

  // column 2
  /*0b01 0000*/ RC_SW_TV,
  /*0b01 0001*/ RC_SW_MUTE,
  /*0b01 0010*/ RC_SW_HOME,
  /*0b01 0011*/ RC_SW_ENTER,
  /*0b01 0100*/ RC_SW_DN_ARW,
  /*0b01 0101*/ RC_SW_INFINITY,
  /*0b01 0110*/ RC_SW_DIG_2,
  /*0b01 0111*/ RC_SW_DIG_5,
  /*0b01 1000*/ RC_SW_DIG_8,
  /*0b01 1001*/ RC_SW_DIG_0,
  /*0b01 1010*/ RC_SW_PLAY,
  /*0b01 1011*/ RC_SW_NA,
  /*0b01 1100*/ RC_SW_NA,
  /*0b01 1101*/ RC_SW_NA,
  /*0b01 1110*/ RC_SW_NA,
  /*0b01 1111*/ RC_SW_NA,

  // column 3
  /*0b10 0000*/ RC_SW_STB,
  /*0b10 0001*/ RC_SW_VOL_DN,
  /*0b10 0010*/ RC_SW_BACK,
  /*0b10 0011*/ RC_SW_LF_ARW,
  /*0b10 0100*/ RC_SW_VOICE,
  /*0b10 0101*/ RC_SW_DIG_1,
  /*0b10 0110*/ RC_SW_DIG_4,
  /*0b10 0111*/ RC_SW_DIG_7,
  /*0b10 1000*/ RC_SW_AV,
  /*0b10 1001*/ RC_SW_PREV,
  /*0b10 1010*/ RC_SW_NA,
  /*0b10 1011*/ RC_SW_NA,
  /*0b10 1100*/ RC_SW_NA,
  /*0b10 1101*/ RC_SW_NA,
  /*0b10 1110*/ RC_SW_NA,
  /*0b10 1111*/ RC_SW_NA
};

/******************************************************************************
 * TYPEDEFS
 */

/******************************************************************************
 * GLOBAL VARIABLES
 */
KcbEvtCBack_t kcbProcessKeyFunction;

/******************************************************************************
 * EXTERNAL VARIABLES
 */

/******************************************************************************
 * LOCAL VARIABLES
 */
static uint8 kcbCurrentKeyState;
static uint8 kcbCurrentKeyPressed;
static uint8 kcbNumberKeys;
static uint16 kcbCurrentKeyPressedCount;

/******************************************************************************
 * LOCAL FUNCTIONS
 */

/******************************************************************************
 * PUBLIC FUNCTIONS
 */

/******************************************************************************
*
* @fn      kcb_Init
*
* @brief   Initialize the Key Callback module
*
* @param   processKeyCB - application callback function to process keys
*          numKeys - number of key presses supported at the same time
*
* @return  none
*/
void kcb_Init(KcbEvtCBack_t processKeyCB, uint8_t numKeys)
{
  kcbCurrentKeyState = RC_KEY_STATE_NOT_PRESSED;
  kcbCurrentKeyPressed = RC_SW_NA;
  kcbCurrentKeyPressedCount = 0;
  kcbProcessKeyFunction = processKeyCB;
  kcbNumberKeys = numKeys;
}

/******************************************************************************
 *
 * @fn      kcb_ProcessKey
 *
 * @brief   Processes key that is being pressed
 *
 * @param   keyId - current key being pressed
 *
 * @return  none
 */
void kcb_ProcessKey(uint8 keyId)
{
  if(kcbNumberKeys == KCB_ONE_KEYS_SUPPORTED)
  {
    if (kcbCurrentKeyState == RC_KEY_STATE_NOT_PRESSED)
    {
      if ((keyId != KEY_CODE_NOKEY) && (keyId < KCB_NUM_RC_KEYS))
      {
        // single key press detected
        kcbCurrentKeyState = RC_KEY_STATE_PRESSED;
        kcbCurrentKeyPressedCount = 0;
        kcbCurrentKeyPressed = (rcKeyId_t)keyId;
      }
    }
    else
    {
      if ((keyId != KEY_CODE_NOKEY) && (keyId == kcbCurrentKeyPressed))
      {
        // Same key still being pressed
        if (kcbCurrentKeyPressedCount < KEY_REPEATED_THRESHOLD)
        {
          kcbCurrentKeyState = RC_KEY_STATE_PRESSED_NOT_YET_REPEATED;
          kcbCurrentKeyPressedCount++;
        }
        else
        {
          kcbCurrentKeyState = RC_KEY_STATE_REPEATED;
        }
      }
      else if (keyId == KEY_CODE_NOKEY)
      {
        kcbCurrentKeyState = RC_KEY_STATE_NOT_PRESSED;
      }
    }

    // Go ahead and notify key handler of key press event
    if (kcbProcessKeyFunction)
    {
      kcbProcessKeyFunction(kcbKeyMap[kcbCurrentKeyPressed],
                            (rcKeyState_t) kcbCurrentKeyState );
    }
  }
}

/******************************************************************************
 ******************************************************************************/
