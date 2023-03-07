/******************************************************************************

 @file  icall_lite_translation.c

 @brief ICall LITE translation form icall message to stack API call..

 Group: WCS, BTS
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
#ifdef ICALL_LITE
/*********************************************************************
 * INCLUDES
 */
#include "osal.h"
#include "icall_lite_translation.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
* CONSTANTS
*/

/*********************************************************************
* TYPEDEFS
 */
#if defined(ICALL_LITE_4_PARAMS)
typedef uint32_t (*directAPIFctPtr_t)(uint32_t param1, uint32_t param2,
                                      uint32_t param3, uint32_t param4);
#elif defined(ICALL_LITE_12_PARAMS)
typedef uint32_t (*directAPIFctPtr_t)(uint32_t param1, uint32_t param2,
                                      uint32_t param3, uint32_t param4,
                                      uint32_t param5, uint32_t param6,
                                      uint32_t param7, uint32_t param8,
                                      uint32_t param9, uint32_t param10,
                                      uint32_t param11, uint32_t param12);
#else
typedef uint32_t (*directAPIFctPtr_t)(uint32_t param1, uint32_t param2,
                                      uint32_t param3, uint32_t param4,
                                      uint32_t param5, uint32_t param6,
                                      uint32_t param7, uint32_t param8);
#endif

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
#ifndef STACK_LIBRARY
static uint32_t * icallLiteJT = NULL;
#endif /* STACK_LIBRARY */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void sendLiteCmdStatus(uint8 taskId);

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 * @fn      sendLiteCmdStatus
 *
 * @brief   Send command status message to the API callee.
 *
 * @param   taskId -task Id of the API callee.
 *
 * @return  None
 */
static void sendLiteCmdStatus(uint8 taskId)
{
  ICall_LiteCmdStatus *pMsg =
    (ICall_LiteCmdStatus *)osal_msg_allocate(sizeof(ICall_LiteCmdStatus));

  if (pMsg)
  {
    pMsg->cmdId = ICALL_LITE_DIRECT_API_DONE_CMD_ID;
#ifdef ICALL_EVENTS
    osal_service_complete(taskId, (uint8 *)pMsg);
#else
    osal_msg_send(taskId, (uint8 *)pMsg);
#endif
  }
}

/*********************************************************************
 * @fn      icall_LiteTranslationInit
 *
 * @brief   Initialize the jump table value.
 *
 * @param   JT - pointer to the jump table containing the stack API to call.
 *
 * @return  None
 */
#ifndef STACK_LIBRARY
void icall_liteTranslationInit(uint32_t *JT)
{
  if (JT != NULL)
  {
    icallLiteJT = &JT[0];
  }
  else
  {
    HAL_ASSERT_FORCED();
  }
}
#endif /* STACK_LIBRARY */
/*********************************************************************
 * @fn      icall_liteTranslation
 *
 * @brief   Translate the icall direct API Message to a stack API call.
 *
 * @param   pMsg - pointer to the received message.
 *
 * @return  None
 */
void icall_liteTranslation(icall_directAPIMsg_t *pMsg)
{
  osal_msg_hdr_t *hdr;
  uint8 taskId;  //msg_ptr->hciExtCmd.srctaskid;

#ifdef STACK_LIBRARY
  pMsg->pointerStack[0] = ((directAPIFctPtr_t)(pMsg->directAPI))
#else
  pMsg->pointerStack[0] = ((directAPIFctPtr_t)(icallLiteJT[pMsg->directAPI]))
#endif  /* STACK_LIBRARY */
#if defined(ICALL_LITE_4_PARAMS)
                                                   (pMsg->pointerStack[0],
                                                    pMsg->pointerStack[1],
                                                    pMsg->pointerStack[2],
                                                    pMsg->pointerStack[3]);
#elif defined(ICALL_LITE_12_PARAMS)
                                                   (pMsg->pointerStack[0],
                                                    pMsg->pointerStack[1],
                                                    pMsg->pointerStack[2],
                                                    pMsg->pointerStack[3],
                                                    pMsg->pointerStack[4],
                                                    pMsg->pointerStack[5],
                                                    pMsg->pointerStack[6],
                                                    pMsg->pointerStack[7],
                                                    pMsg->pointerStack[8],
                                                    pMsg->pointerStack[9],
                                                    pMsg->pointerStack[10],
                                                    pMsg->pointerStack[11]);
#else
                                                   (pMsg->pointerStack[0],
                                                    pMsg->pointerStack[1],
                                                    pMsg->pointerStack[2],
                                                    pMsg->pointerStack[3],
                                                    pMsg->pointerStack[4],
                                                    pMsg->pointerStack[5],
                                                    pMsg->pointerStack[6],
                                                    pMsg->pointerStack[7]);
#endif

  hdr = (osal_msg_hdr_t *) pMsg - 1;
  taskId = osal_alien2proxy(hdr->srcentity);

  // post Message confirming the end of the API call.
  sendLiteCmdStatus(taskId);
}
#endif /* ICALL_LITE */

/*********************************************************************
*********************************************************************/
