/******************************************************************************

 @file  time_config.c

 @brief Time characteristic configuration routines.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2011-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "time_clock.h"
#include "bletime.h"

/*********************************************************************
 * MACROS
 */

// Used to determine the end of Time_configList[]
#define TIME_CONFIG_MAX      (sizeof(Time_configList) / sizeof(uint8_t))

/*********************************************************************
 * CONSTANTS
 */

// Array of handle cache indexes.  This list determines the
// characteristics that are read or written during configuration.
const uint8_t Time_configList[] =
{
  HDL_CURR_TIME_CT_TIME_START,            // Current time
  HDL_CURR_TIME_CT_TIME_CCCD,             // Current time CCCD
};

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Connection handle.
uint16_t Time_connHandle = 0;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

uint8_t Time_configDone = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      Time_configNext()
 *
 * @brief   Perform the characteristic configuration read or
 *          write procedure.
 *
 * @param   state - Configuration state.
 *
 * @return  New configuration state.
 */
uint8_t Time_configNext(uint8_t state)
{
  bool read;

  // Find next non-zero cached handle of interest
  while (state < TIME_CONFIG_MAX &&
          Time_handleCache[Time_configList[state]] == 0)
  {
    state++;
  }

  // Return if reached end of list
  if (state >= TIME_CONFIG_MAX)
  {
    return TIME_CONFIG_CMPL;
  }

  // Determine what to do with characteristic
  switch (Time_configList[state])
  {
    // Read these characteristics
    case HDL_CURR_TIME_CT_TIME_START:
      read = TRUE;
      break;

    // Set notification for these characteristics
    case HDL_CURR_TIME_CT_TIME_CCCD:
      read = FALSE;
      break;

    default:
      return state;
  }

  if(Time_configDone==TRUE)
  {
    return state;
  }

  // Do a GATT read or write
  if (read)
  {
    attReadReq_t  readReq;

    readReq.handle = Time_handleCache[Time_configList[state]];

    // Send the read request
    GATT_ReadCharValue(Time_connHandle, &readReq, ICall_getEntityId());

    // Only reading time right now
    Time_configDone = TRUE;
  }
  else
  {
    attWriteReq_t writeReq;

    writeReq.pValue = GATT_bm_alloc(Time_connHandle, ATT_WRITE_REQ, 2, NULL);
    if (writeReq.pValue != NULL)
    {
      writeReq.len = 2;
      writeReq.pValue[0] = LO_UINT16(GATT_CLIENT_CFG_NOTIFY);
      writeReq.pValue[1] = HI_UINT16(GATT_CLIENT_CFG_NOTIFY);
      writeReq.sig = 0;
      writeReq.cmd = 0;

      writeReq.handle = Time_handleCache[Time_configList[state]];

      // Send the read request
      if (GATT_WriteCharValue(Time_connHandle, &writeReq,
                              ICall_getEntityId()) != SUCCESS)
      {
        GATT_bm_free((gattMsg_t *)&writeReq, ATT_WRITE_REQ);
      }
    }
  }

  return state;
}

/*********************************************************************
 * @fn      Time_configGattMsg()
   *
 * @brief   Handle GATT messages for characteristic configuration.
 *
 * @param   state - Discovery state.
 * @param   pMsg - GATT message.
 *
 * @return  New configuration state.
 */
uint8_t Time_configGattMsg(uint8_t state, gattMsgEvent_t *pMsg)
{

  if ((pMsg->method == ATT_READ_RSP || pMsg->method == ATT_WRITE_RSP) &&
       (pMsg->hdr.status == SUCCESS))
  {
    // Process response
    switch (Time_configList[state])
    {
      case HDL_CURR_TIME_CT_TIME_START:
        // Set clock to time read from time server
        Time_clockSet(pMsg->msg.readRsp.pValue);
        break;

      default:
        break;
    }
  }
  return Time_configNext(state + 1);
}


/*********************************************************************
*********************************************************************/
