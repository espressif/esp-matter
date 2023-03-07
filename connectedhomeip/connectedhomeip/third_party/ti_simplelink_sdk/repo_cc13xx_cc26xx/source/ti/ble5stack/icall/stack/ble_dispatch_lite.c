/******************************************************************************

 @file  ble_dispatch_lite.c

 @brief ICall BLE Stack Dispatcher for embedded and NP/Serial messages.

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
#include "osal_snv.h"
#include "osal_bufmgr.h"

#include "ble_dispatch.h"
#include "ble_dispatch_lite.h"

#include "rom_jt.h"

#include <icall_lite_translation.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#if !defined(STACK_REVISION)
  #define STACK_REVISION                 0x010200
#endif // STACK_REVISION

extern const uint16 ll_buildRevision;

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
uint8_t icall_liteTaskId;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

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
 * @fn      bleDispatch_BMAlloc
 *
 * @brief   Implementation of the BM allocator functionality.
 *
 *          Note: This function should only be called by the upper
 *                layer protocol/application.
 *
 * @param   type - type of the message to allocate.
 * @param   size - number of bytes to allocate from the heap.
 * @param   connHandle - connection that GATT message is to be sent on
 *                       (applicable only to BM_MSG_GATT type).
 * @param   opcode - opcode of GATT message that buffer to be allocated for
 *                   (applicable only to BM_MSG_GATT type).
 * @param   pSizeAlloc - number of bytes allocated for the caller from the heap
 *                       (applicable only to BM_MSG_GATT type).
 *
 * @return  pointer to the heap allocation; NULL if error or failure.
 */
void *bleDispatch_BMAlloc(uint8_t type, uint16_t size, uint16_t connHandle,
                          uint8_t opcode, uint16_t *pSizeAlloc)
{
  void *pBuf;

  switch (type)
  {
#if defined(HOST_CONFIG) && (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
    case BM_MSG_GATT:
      pBuf = GATT_bm_alloc(connHandle, opcode, size, pSizeAlloc);
      break;

    case BM_MSG_L2CAP:
      pBuf = L2CAP_bm_alloc(size);
      break;

#endif /* defined(HOST_CONFIG) && (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG)) */
    default:
      pBuf = BM_alloc(size);
      break;
  }

  return (pBuf);
}

/*********************************************************************
 * @fn      bleDispatch_BMFree
 *
 * @brief   Implementation of the BM de-allocator functionality.
 *
 * @param   type - type of the message to free.
 * @param   pBuf - pointer to the memory to free.
 * @param   opcode - opcode of GATT message (applicable only to BM_MSG_GATT
 *                   type).
 *
 * @return  none
 */
void bleDispatch_BMFree(uint8_t type, void *pBuf, uint8_t opcode)
{
  switch (type)
  {
#if defined(HOST_CONFIG) && (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
    case BM_MSG_GATT:
      GATT_bm_free((gattMsg_t *)pBuf, opcode);
      break;

    case BM_MSG_L2CAP:
      /*lint --fallthrough */
#endif /* defined(HOST_CONFIG) && (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG)) */
    default:
      BM_free(pBuf);
      break;
  }
}

/*********************************************************************
 * @fn      buildRevision
 *
 * @brief   Read the Build Revision used to build the BLE stack.
 *
 * @param   pBuildRev - pointer to variable to copy build revision into
 *
 * @return  SUCCESS: Operation was successfully.
 *          INVALIDPARAMETER: Invalid parameter.
 */
uint8 buildRevision(ICall_BuildRevision *pBuildRev)
{
  if (pBuildRev!= NULL)
  {
    pBuildRev->stackVersion = (uint32)STACK_REVISION;
    pBuildRev->buildVersion = (uint16)ll_buildRevision;

    // Stack info (Byte 5)
    //  Bit    0: IAR used to build stack project (0: no, 1: yes)
    //  Bit    1: CCS used to build stack project (0: no, 1: yes)
    //  Bits 2-3: Reserved
    //  Bit    4: IAR used to build stack library (0: no, 1: yes)
    //  Bits 5-6: Reserved
    //  Bit    7: ROM build (0: no, 1: yes)
    pBuildRev->stackInfo =
#if defined(__IAR_SYSTEMS_ICC__)
      BLDREV_STK_IAR_PROJ |
#endif // __IAR_SYSTEMS_ICC__
#if defined(__TI_COMPILER_VERSION__)
      BLDREV_STK_CCS_PROJ |
#endif // __TI_COMPILER_VERSION__
#if defined(FLASH_ROM_BUILD)
      BLDREV_STK_IAR_LIB  |
      BLDREV_STK_ROM_BLD  |
#endif // FLASH_ROM_BUILD
      0;

    // Controller info - part 1 (Byte 6)
    //  Bit  0: ADV_NCONN_CFG (0: not included, 1: included)
    //  Bit  1: ADV_CONN_CFG (0: not included, 1: included)
    //  Bit  2: SCAN_CFG (0: not included, 1: included)
    //  Bit  3: INIT_CFG (0: not included, 1: included)
    //  Bit  4: PING_CFG (0: not included, 1: included)
    //  Bit  5: SLV_FEAT_EXCHG_CFG (0: not included, 1: included)
    //  Bit  6: CONN_PARAM_REQ_CFG (0: not included, 1: included)
    //  Bit  7: Reserved
    pBuildRev->ctrlInfo =
#if defined(CTRL_CONFIG)
      CTRL_CONFIG                    |
#endif // CTRL_CONFIG
      BLDREV_CTRL_PING_CFG           |
      BLDREV_CTRL_SLV_FEAT_EXCHG_CFG |
      BLDREV_CTRL_CONN_PARAM_REQ_CFG |
      0;

    // Host info - part 1 (Byte 8)
    //  Bit    0: BROADCASTER_CFG (0: not included, 1: included)
    //  Bit    1: OBSERVER_CFG (0: not included, 1: included)
    //  Bit    2: PERIPHERAL_CFG (0: not included, 1: included)
    //  Bit    3: CENTRAL_CFG (0: not included, 1: included)
    //  Bit    4: GAP_BOND_MGR (0: not included, 1: included)
    //  Bit    5: L2CAP_CO_CHANNELS (0: not included, 1: included)
    //  Bits 6-7: Reserved
    pBuildRev->hostInfo =
#if defined(HOST_CONFIG)
      HOST_CONFIG                   |
#endif // HOST_CONFIG
#if defined(GAP_BOND_MGR)
      BLDREV_HOST_GAP_BOND_MGR      |
#endif // GAP_BOND_MGR
#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
      BLDREV_HOST_L2CAP_CO_CHANNELS |
#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)
      0;

    return (SUCCESS);
  }

  return (INVALIDPARAMETER);
}

/*********************************************************************
 * @fn      icall_liteMsgParser
 *
 * @brief   parsing of the ble specific icall direct API message.
 *
 * @param   *msg - pointer to message receive
 *
 * @return  None
*/
void icall_liteMsgParser(void * msg)
{
  osal_msg_hdr_t *hdr;

  hdr = (osal_msg_hdr_t *) msg - 1;

  if (hdr->format == ICALL_MSG_FORMAT_DIRECT_API_ID)
  {
      osal_msg_send(icall_liteTaskId, msg);
  }
}

/*********************************************************************
 * @fn      ble_dispatch_liteProcess
 *
 * @brief   Process osal message send to the icall lite task. This allows icall
 *          lite direct API messages to be parse with the lowest priority, and
 *          not diverge from the previous implementation.
 *
 * @param   taskId.
 *          events
 *
 * @return  None
 */
uint16 ble_dispatch_liteProcess(uint8_t  taskId, uint16_t events)
{
  uint8 *pMsg;
  VOID taskId;
  if ( events & SYS_EVENT_MSG )
  {
    if ( (pMsg = MAP_osal_msg_receive( icall_liteTaskId )) != NULL )
    {
      icall_directAPIMsg_t* appMsg = (icall_directAPIMsg_t*) pMsg;
      icall_liteTranslation(appMsg);
    }

    // Return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }
  return 0;
}

/*********************************************************************
 * @fn      ble_dispatch_liteInit
 *
 * @brief   initialize the icall lite osal task.
 *
 * @param   taskId.
 *
 * @return  None
 */
void ble_dispatch_liteInit(uint8_t  taskId)
{
  icall_liteTaskId = taskId;
}

/*********************************************************************
 * @fn      icall_liteErrorFunction
 *
 * @brief   call assert. this function is used to replace non-existant function
 *          in the jump table.
 *
 * @param   None
 *
 * @return  None
*/
#ifndef STACK_LIBRARY
void icall_liteErrorFunction (void)
{
  HAL_ASSERT( HAL_ASSERT_CAUSE_INTERNAL_ERROR );
  return;
}
#endif /* STACK_LIBRARY */

#endif /* ICALL_LITE */

/*********************************************************************
*********************************************************************/

