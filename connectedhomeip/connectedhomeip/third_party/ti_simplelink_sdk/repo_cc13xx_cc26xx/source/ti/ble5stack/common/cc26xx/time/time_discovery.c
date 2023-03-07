/******************************************************************************

 @file  time_discovery.c

 @brief Time  service and characteristic discovery routines.

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
#include <string.h>
#include "bcomdef.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gatt_profile_uuid.h"
#include "bletime.h"

/*********************************************************************
 * MACROS
 */

// Length of Characteristic declaration + handle with 16 bit UUID
#define CHAR_DESC_HDL_UUID16_LEN        7

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

// Attribute handle cache
uint16_t Time_handleCache[HDL_CACHE_LEN];

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Attribute handles used during discovery
static uint16_t Time_svcStartHdl;
static uint16_t Time_svcEndHdl;
static uint8_t Time_endHdlIdx;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static uint8_t Time_discCurrTime(uint8_t state, gattMsgEvent_t *pMsg);

/*********************************************************************
 * @fn      Time_discStart()
 *
 * @brief   Start service discovery.
 *
 * @return  New discovery state.
 */
uint8_t Time_discStart(void)
{
  // Clear handle cache
  memset(Time_handleCache, 0, sizeof(Time_handleCache));

  // Start discovery with first service
  return Time_discGattMsg(DISC_CURR_TIME_START, NULL);
}

/*********************************************************************
 * @fn      Time_discGattMsg()
 *
 * @brief   Handle GATT messages for characteristic discovery.
 *
 * @param   state - Discovery state.
 * @param   pMsg - GATT message.
 *
 * @return  New discovery state.
 */
uint8_t Time_discGattMsg(uint8_t state, gattMsgEvent_t *pMsg)
{
  // Execute discovery function for service
  do
  {
    switch (state & 0xF0)
    {
      // Current time service
      case DISC_CURR_TIME_START:
        state = Time_discCurrTime(state, pMsg);
        if (state == DISC_FAILED)
        {
          state = DISC_FAILED;
        }
        else if (state == DISC_IDLE)
        {
          state = DISC_IDLE;
        }
        break;

      default:
        break;
    }

  } while ((state != 0) && ((state & 0x0F) == 0));

  return state;
}

/*********************************************************************
 * @fn      Time_discCurrTime()
 *
 * @brief   Current time service and characteristic discovery.
 *
 * @param   state - Discovery state.
 * @param   pMsg - GATT message.
 *
 * @return  New discovery state.
 */
static uint8_t Time_discCurrTime(uint8_t state, gattMsgEvent_t *pMsg)
{
  uint8_t newState = state;

  switch (state)
  {
    case DISC_CURR_TIME_START:
      {
        uint8_t uuid[ATT_BT_UUID_SIZE] = { LO_UINT16(CURRENT_TIME_SERV_UUID),
                                         HI_UINT16(CURRENT_TIME_SERV_UUID) };

        // Initialize service discovery variables
        Time_svcStartHdl = Time_svcEndHdl = 0;
        Time_endHdlIdx = 0;

        // Discover service by UUID
        GATT_DiscPrimaryServiceByUUID(Time_connHandle, uuid,
                                       ATT_BT_UUID_SIZE, ICall_getEntityId());

        newState = DISC_CURR_TIME_SVC;
      }
      break;

    case DISC_CURR_TIME_SVC:
      // Service found, store handles
      if (pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
           pMsg->msg.findByTypeValueRsp.numInfo > 0)
      {
        Time_svcStartHdl =
          ATT_ATTR_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0);
        Time_svcEndHdl =
          ATT_GRP_END_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0);
      }

      // If procedure complete
      if ((pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP  &&
             pMsg->hdr.status == bleProcedureComplete) ||
           (pMsg->method == ATT_ERROR_RSP))
      {
        // If service found
        if (Time_svcStartHdl != 0)
        {
          // Discover all characteristics
          GATT_DiscAllChars(Time_connHandle, Time_svcStartHdl,
                             Time_svcEndHdl, ICall_getEntityId());

          newState = DISC_CURR_TIME_CHAR;
        }
        else
        {
          // Service not found
          newState = DISC_FAILED;
        }
      }
      break;

    case DISC_CURR_TIME_CHAR:
      {
         // Characteristics found
        if (pMsg->method == ATT_READ_BY_TYPE_RSP &&
             pMsg->msg.readByTypeRsp.numPairs > 0 &&
             pMsg->msg.readByTypeRsp.len == CHAR_DESC_HDL_UUID16_LEN)
        {
          uint8_t   i;
          uint8_t   *p;
          uint16_t  handle;
          uint16_t  uuid;
          // For each characteristic declaration
          p = pMsg->msg.readByTypeRsp.pDataList;
          for (i = pMsg->msg.readByTypeRsp.numPairs; i > 0; i--)
          {
            // Parse characteristic declaration
            handle = BUILD_UINT16(p[3], p[4]);
            uuid = BUILD_UINT16(p[5], p[6]);

            // If looking for end handle
            if (Time_endHdlIdx != 0)
            {
              // End handle is one less than handle of characteristic
              // declaration.
              Time_handleCache[Time_endHdlIdx] = BUILD_UINT16(p[0], p[1]) - 1;

              Time_endHdlIdx = 0;
            }

            // If UUID is of interest, store handle
            switch (uuid)
            {
              case CURRENT_TIME_UUID:
                Time_handleCache[HDL_CURR_TIME_CT_TIME_START] = handle;
                Time_endHdlIdx = HDL_CURR_TIME_CT_TIME_END;
                break;

              default:
                break;
            }

            p += CHAR_DESC_HDL_UUID16_LEN;
          }
        }

        // If procedure complete
        if ((pMsg->method == ATT_READ_BY_TYPE_RSP  &&
               pMsg->hdr.status == bleProcedureComplete) ||
             (pMsg->method == ATT_ERROR_RSP))
        {
          // Special case of end handle at end of service
          if (Time_endHdlIdx != 0)
          {
            Time_handleCache[Time_endHdlIdx] = Time_svcEndHdl;
            Time_endHdlIdx = 0;
          }

          // If didn't find time characteristic
          if (Time_handleCache[HDL_CURR_TIME_CT_TIME_START] == 0)
          {
            newState = DISC_FAILED;
          }

          else if (Time_handleCache[HDL_CURR_TIME_CT_TIME_START] <
                   Time_handleCache[HDL_CURR_TIME_CT_TIME_END])
          {
            // Discover characteristic descriptors
            GATT_DiscAllCharDescs(Time_connHandle,
                                  Time_handleCache[HDL_CURR_TIME_CT_TIME_START] + 1,
                                  Time_handleCache[HDL_CURR_TIME_CT_TIME_END],
                                  ICall_getEntityId());

            newState = DISC_CURR_TIME_CT_TIME_CCCD;
          }
          else
          {
            newState = DISC_IDLE;
          }
        }
      }
      break;

    case DISC_CURR_TIME_CT_TIME_CCCD:
      {
        // Characteristic descriptors found
        if (pMsg->method == ATT_FIND_INFO_RSP &&
             pMsg->msg.findInfoRsp.numInfo > 0 &&
             pMsg->msg.findInfoRsp.format == ATT_HANDLE_BT_UUID_TYPE)
        {
          uint8_t i;

          // For each handle/uuid pair
          for (i = 0; i < pMsg->msg.findInfoRsp.numInfo; i++)
          {
            // Look for CCCD
            if (ATT_BT_PAIR_UUID(pMsg->msg.findInfoRsp.pInfo, i) ==
                                                      GATT_CLIENT_CHAR_CFG_UUID)
            {
              // CCCD found
              Time_handleCache[HDL_CURR_TIME_CT_TIME_CCCD] =
                ATT_BT_PAIR_HANDLE(pMsg->msg.findInfoRsp.pInfo, i);
              break;
            }
          }
        }

        // If procedure complete
        if ((pMsg->method == ATT_FIND_INFO_RSP  &&
               pMsg->hdr.status == bleProcedureComplete) ||
             (pMsg->method == ATT_ERROR_RSP))
        {
          newState = DISC_IDLE;
        }
      }
      break;

    default:
      break;
  }

  return newState;
}


/*********************************************************************
*********************************************************************/
