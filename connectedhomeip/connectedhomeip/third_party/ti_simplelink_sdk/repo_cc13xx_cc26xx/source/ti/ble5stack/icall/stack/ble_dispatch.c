/******************************************************************************

 @file  ble_dispatch.c

 @brief ICall BLE Stack Dispatcher for embedded and NP/Serial messages.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
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
#include "osal_snv.h"
#include "osal_bufmgr.h"

#include "sm.h"
#include "gap.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "hci_tl.h"
#include "linkdb.h"
#include "npi.h"

#include "gattservapp.h"
#include "gapgattserver.h"
#include "gapbondmgr.h"

#if defined ( GATT_TEST ) || defined ( GATT_QUAL )
  #include "gatttest.h"
#endif

#include "hci_ext.h"
#include "ble_dispatch.h"
#include "icall_apimsg.h"
#include "trng_api.h"
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>

#include "ll_common.h"
#include "rom_jt.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#if !defined(HCI_TL_FULL) && !defined(HCI_TL_NONE)
  #define HCI_TL_NONE
#endif // !HCI_TL_FULL && !HCI_TL_NONE

#define RSP_PAYLOAD_IDX                  6
#define MAX_RSP_DATA_LEN                 50
#define MAX_RSP_BUF                      (RSP_PAYLOAD_IDX + MAX_RSP_DATA_LEN)

#if !defined(HCI_EXT_APP_OUT_BUF)
  #define HCI_EXT_APP_OUT_BUF            44
#endif // !HCI_EXT_APP_OUT_BUF

#define KEYDIST_SENC                     0x01
#define KEYDIST_SID                      0x02
#define KEYDIST_SSIGN                    0x04
#define KEYDIST_SLINK                    0x08
#define KEYDIST_MENC                     0x10
#define KEYDIST_MID                      0x20
#define KEYDIST_MSIGN                    0x40
#define KEYDIST_MLINK                    0x80

// Maximum number of reliable writes supported by Attribute Client
#define GATT_MAX_NUM_RELIABLE_WRITES     5

#if !defined(STACK_REVISION)
  #define STACK_REVISION                 0x010100
#endif // STACK_REVISION

extern const uint16 ll_buildRevision;

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
  uint8  pktType;
  uint16 opCode;
  uint8  len;
  uint8  *pData;
} hciExtCmd_t;

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

// Task ID for internal task/event processing
uint8 bleDispatch_TaskID;

// Outgoing response
uint8 rspBuf[MAX_RSP_BUF];

#if defined(HCI_TL_FULL) && defined(HOST_CONFIG)
  // The device's local keys
  static uint8 IRK[KEYLEN] = {0};
  static uint8 SRK[KEYLEN] = {0};

  // Outgoing event
  static uint8 out_msg[HCI_EXT_APP_OUT_BUF];
#endif // HCI_TL_FULL && HOST_CONFIG

/*********************************************************************
 * LOCAL FUNCTIONS
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

/*** For ICall messages ***/
static uint8 processICallMsg(ICall_CmdMsg *msg_ptr);
static uint8 processICallHost(uint16 opCode, ICall_CmdMsg *msg_ptr,
                              uint8 *pRspDataLen, uint8 *pSendCS);
static uint8 processICallLL(uint16 opCode, ICall_CmdMsg *msg_ptr,
                            uint8 *pRspDataLen, uint8 *pSendCS);
static uint8 processICallUTIL(uint8 cmdID, ICall_CmdMsg *msg_ptr,
                              uint8 *pRspDataLen, uint8 *pSendCS);
static uint8 buildRevision(ICall_BuildRevision *pBuildRev);

#if defined(HOST_CONFIG)
#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
static uint8 processICallL2CAP(uint8 cmdID, ICall_CmdMsg *msg_ptr,
                               uint8 *pSendCS);
static uint8 processICallATT(uint8 cmdID, ICall_CmdMsg *msg_ptr,
                             uint8 *pSendCS);
static uint8 processICallGATT(uint8 cmdID, ICall_CmdMsg *msg_ptr,
                              uint8 *pSendCS);
#endif // (CENTRAL_CFG | PERIPHERAL_CFG)
static uint8 processICallGAP(uint8 cmdID, ICall_CmdMsg *msg_ptr,
                             uint8 *pRspDataLen, uint8 *pSendCS);
#endif // HOST_CONFIG

/*** For Dispatch messages ***/
static uint8 processDispMsg(ICall_CmdMsg *msg_ptr);
static uint8 processDispGeneral(ICall_CmdMsg *msg_ptr, uint8 *pSendCS);

#if defined(HOST_CONFIG)
static uint8 processDispGAPProfile(ICall_CmdMsg *msg_ptr, uint8 *pRspDataLen,
                                   uint8 *pSendCS);

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
static uint8 processDispGATTProfile(ICall_CmdMsg *msg_ptr, uint8 *pSendCS);

#if !defined(GATT_DB_OFF_CHIP)
static uint8 processDispGGS(ICall_CmdMsg *msg_ptr, uint8 *pSendCS);
static uint8 processDispGSA(ICall_CmdMsg *msg_ptr, uint8 *pSendCS);
#endif // !GATT_DB_OFF_CHIP
#endif // (CENTRAL_CFG | PERIPHERAL_CFG)
#endif // HOST_CONFIG

// Do not use SNV drivers with FPGA. Unverified behavior
#ifndef USE_FPGA
static uint8 checkNVLen(osalSnvId_t id, osalSnvLen_t len);
#endif // USE_FPGA

static void sendGapCmdStatus(uint8 taskId, uint16 opCode, uint8 cmdId,
                             uint8 status, uint8 len, uint8 *pData);

/*** For HCI Extension messages ***/
#if defined(HCI_TL_FULL)
static uint8 processExtMsg(hciPacket_t *pMsg);
static uint8 processExtMsgUTIL(uint8 cmdID, hciExtCmd_t *pCmd,
                               uint8 *pRspDataLen);
#if defined(HOST_CONFIG)
static uint8 processExtMsgL2CAP(uint8 cmdID, hciExtCmd_t *pCmd,
                                uint8 *pRspDataLen);
static uint8 processExtMsgATT(uint8 cmdID, hciExtCmd_t *pCmd);
static uint8 processExtMsgGATT(uint8 cmdID, hciExtCmd_t *pCmd,
                               uint8 *pRspDataLen);
static uint8 processExtMsgGAP(uint8 cmdID, hciExtCmd_t *pCmd,
                              uint8 *pRspDataLen);

static uint8 processEvents(osal_event_hdr_t *pMsg );
static uint8 *processEventsGAP(gapEventHdr_t *pMsg, uint8 *pOutMsg,
                               uint8 *pMsgLen,uint8 *pAllocated,
                               uint8 *pDeallocate);
static uint8 *processEventsL2CAP(l2capSignalEvent_t *pPkt, uint8 *pOutMsg,
                                 uint8 *pMsgLen);
static uint8 *processEventsGATT(gattMsgEvent_t *pPkt, uint8 *pOutMsg,
                                uint8 *pMsgLen, uint8 *pAllocated);
static uint8 *processDataL2CAP( l2capDataEvent_t *pPkt, uint8 *pOutMsg,
                                uint8 *pMsgLen, uint8 *pAllocated );
static uint8 *processEventsSM( smEventHdr_t *pMsg, uint8 *pOutMsg,
                               uint8 *pMsgLen, uint8 *pAllocated );
#if !defined(GATT_DB_OFF_CHIP)
static uint8 *processEventsGATTServ(gattEventHdr_t *pPkt, uint8 *pMsg,
                                    uint8 *pMsgLen);
#else // GATT_DB_OFF_CHIP
static uint8 addAttrRec(gattService_t *pServ, uint8 *pUUID, uint8 len,
                        uint8 permissions, uint16 *pTotalAttrs,
                        uint8 *pRspDataLen);
static void freeAttrRecs(gattService_t *pServ);
static const uint8 *findUUIDRec(uint8 *pUUID, uint8 len);
#endif // !GATT_DB_OFF_CHIP

#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
static uint8 buildCoChannelInfo( uint16 CID, l2capCoCInfo_t *pInfo, uint8 *pRspBuf );
static uint16 l2capVerifySecCB( uint16 connHandle, uint8 id,
                                l2capConnectReq_t *pReq );
#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)

static uint8 buildHCIExtHeader(uint8 *pBuf, uint16 event, uint8 status,
                               uint16 connHandle);
static uint8 mapATT2BLEStatus(uint8 status);
static uint8 *createMsgPayload( uint8 *pBuf, uint16 len );
static uint8 *createSignedMsgPayload( uint8 sig, uint8 cmd, uint8 *pBuf, uint16 len );
static uint8 *createPayload( uint8 *pBuf, uint16 len, uint8 sigLen );
#endif // HOST_CONFIG
#endif // HCI_TL_FULL

/*********************************************************************
 * @fn      bleDispatch_Init
 *
 * @brief   Initialization function for the ICall BLE Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification ...).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void bleDispatch_Init(uint8_t task_id)
{
  bleDispatch_TaskID = task_id;

#if defined(HCI_TL_FULL) && defined(HOST_CONFIG)
  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs( bleDispatch_TaskID );

  // Register to receive incoming ATT Indications/Notifications
  GATT_RegisterForInd( bleDispatch_TaskID );

  #if !defined(GATT_DB_OFF_CHIP)
    // Register with GATT Server App for event messages
    GATTServApp_RegisterForMsg( bleDispatch_TaskID );
  #else
    // Register with GATT Server for GATT messages
    GATT_RegisterForReq( bleDispatch_TaskID );
  #endif // GATT_DB_OFF_CHIP

  // Do not use SNV drivers with FPGA. Unverified behavior
#ifndef USE_FPGA
  VOID osal_snv_read( BLE_NVID_IRK, KEYLEN, IRK );
  VOID osal_snv_read( BLE_NVID_CSRK, KEYLEN, SRK );
#endif // USE_FPGA

#endif // HCI_TL_FULL && HOST_CONFIG
}

/*********************************************************************
 * @fn      bleDispatch_ProcessEvent
 *
 * @brief   BLE Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16_t bleDispatch_ProcessEvent(uint8_t task_id, uint16_t events)
{
  if (events & SYS_EVENT_MSG)
  {
    ICall_CmdMsg *pMsg;

    if ((pMsg = (ICall_CmdMsg *)osal_msg_receive(bleDispatch_TaskID))!= NULL)
    {
      uint8 dealloc = TRUE;

      // check if there was a boot message to be sent
      if ( (SysBootMsg != NULL) && (SysBootMsg != INVALID_SYSBOOTMSG) )
      {
        // send it on now that we're done booting
        (void)osal_msg_send( hciTaskID, SysBootMsg );

        // invalidate the pointer
        SysBootMsg = INVALID_SYSBOOTMSG;
      }

      // Process incoming messages
      switch (pMsg->hdr.event)
      {
        case ICALL_CMD_EVENT:
          // Incoming ICall message
          dealloc = processICallMsg(pMsg);
          break;

        case DISPATCH_CMD_EVENT:
          // Incoming Dispatch message
          dealloc = processDispMsg(pMsg);
          break;

#if !defined(HCI_TL_NONE)
        case HCI_HOST_TO_CTRL_DATA_EVENT:
          {
            hciDataPacket_t *pHciData = (hciDataPacket_t *)pMsg;
            uint8 *pData = pHciData->pData;

            // Replace data with bm data
            pHciData->pData = HCI_bm_alloc(pHciData->pktLen);

            if (pHciData->pData)
            {
              VOID osal_memcpy(pHciData->pData, pData, pHciData->pktLen);

              // Send it to the HCI handler
              (void)osal_msg_send(hciTaskID, (uint8*)(pMsg));

              dealloc = FALSE;
            }

            osal_mem_free(pData);
          }
          break;

        case HCI_HOST_TO_CTRL_CMD_EVENT:
           // Send it to the HCI handler
          (void)osal_msg_send(hciTaskID, (uint8*)(pMsg));
          dealloc = FALSE;
          break;
#endif // !HCI_TL_NONE

#if defined(HCI_TL_FULL)
        case HCI_EXT_CMD_EVENT:
          // Incoming HCI extension message
          dealloc = processExtMsg((hciPacket_t *)pMsg);
          break;

        default:
#if defined(HOST_CONFIG)
          dealloc = processEvents((osal_event_hdr_t *)pMsg);
#endif // HOST_CONFIG
          break;
#else // !HCI_TL_FULL
        default:
          break;
#endif // HCI_TL_FULL
      }

      // Release the OSAL message
      if (dealloc)
      {
        VOID osal_msg_deallocate((uint8 *)pMsg);
      }
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn      processICallMsg
 *
 * @brief   Parse and process incoming ICall message
 *
 * @param   msg_ptr - pointer to incoming ICall Command message
 *
 * @return  true
 */
static uint8 processICallMsg(ICall_CmdMsg *msg_ptr)
{
  uint8 rspDataLen = 0;
  uint8 sendCmdStatus = TRUE;
  uint16 opCode = msg_ptr->hciExtCmd.opCode;
  uint8 cmdId = msg_ptr->hciExtCmd.cmdId;
  uint8 taskId = msg_ptr->hciExtCmd.srctaskid;
  bStatus_t stat;

  // Check if a Controller Link Layer command (Spec & Vendor Specific)
  if (((opCode >> 10) == VENDOR_SPECIFIC_OGF) &&
      (((opCode >> 7) & 0x07) != HCI_OPCODE_CSG_LINK_LAYER))
  {
    stat = processICallHost(opCode, msg_ptr, &rspDataLen, &sendCmdStatus);
  }
  else
  {
    stat = processICallLL(opCode, msg_ptr, &rspDataLen, &sendCmdStatus);
  }

  // Deallocate here to free up heap space for the serial message set out HCI.
  VOID osal_msg_deallocate((uint8 *)msg_ptr);

  if (sendCmdStatus)
  {
    // Send back an immediate response
    sendGapCmdStatus(taskId, opCode, cmdId, stat, rspDataLen,
                     &rspBuf[RSP_PAYLOAD_IDX]);
  }

  return (FALSE);
}

/*********************************************************************
 * @fn      processICallHost
 *
 * @brief   Parse and process incoming ICall Host messages
 *
 * @param   opCode - incoming HCI extension opcode.
 * @param   msg_ptr - pointer to ICall message
 * @param   pRspDataLen - response data length to be returned.
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  none
 */
static uint8 processICallHost(uint16 opCode, ICall_CmdMsg *msg_ptr,
                              uint8 *pRspDataLen, uint8 *pSendCS)
{
  bStatus_t stat;

  switch((opCode >> 7) & 0x07)
  {
#if defined(HOST_CONFIG)
#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
    case HCI_EXT_L2CAP_SUBGRP:
      stat = processICallL2CAP((opCode & 0x007F), msg_ptr, pSendCS);
      break;

    case HCI_EXT_ATT_SUBGRP:
      stat = processICallATT((opCode & 0x007F), msg_ptr, pSendCS);
      break;

    case HCI_EXT_GATT_SUBGRP:
      stat = processICallGATT((opCode & 0x007F), msg_ptr, pSendCS);
      break;
#endif // (CENTRAL_CFG | PERIPHERAL_CFG)

    case HCI_EXT_GAP_SUBGRP:
      stat = processICallGAP((opCode & 0x007F), msg_ptr, pRspDataLen, pSendCS);
      break;
#endif // HOST_CONFIG

    case HCI_EXT_UTIL_SUBGRP:
      stat = processICallUTIL((opCode & 0x007F), msg_ptr, pRspDataLen, pSendCS);
      break;

    default:
      stat = FAILURE;
      break;
  }

  return (stat);
}

/*********************************************************************
 * @fn      processICallLL
 *
 * @brief   Parse and process incoming ICall Link Layer (LL) messages
 *
 * @param   opCode - incoming HCI extension opcode.
 * @param   msg_ptr - pointer to ICall message
 * @param   pRspDataLen - response data length to be returned.
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  SUCCESS or FAILURE
 */
static uint8 processICallLL(uint16 opCode, ICall_CmdMsg *msg_ptr,
                            uint8 *pRspDataLen, uint8 *pSendCS)
{
  bStatus_t stat;

  // Note: All LL Command definitions include OGF (63) and CSG (0): 0xFC00
  switch(opCode)
  {
    /*
     * BT/LE HCI Commands: Link Layer
     */
#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
    case HCI_READ_REMOTE_VERSION_INFO:
      stat = HCI_ReadRemoteVersionInfoCmd(msg_ptr->hciParams.param1);
      break;
#endif // ADV_CONN_CFG | INIT_CFG

    case HCI_SET_EVENT_MASK:
      stat = HCI_SetEventMaskCmd(msg_ptr->hciPtrParams.pParam1);
      break;

    case HCI_SET_EVENT_MASK_PAGE_2:
      stat = HCI_SetEventMaskPage2Cmd(msg_ptr->hciPtrParams.pParam1);
      break;

    case HCI_RESET:
      stat = HCI_ResetCmd();
      break;

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
    case HCI_READ_TRANSMIT_POWER:
      stat = HCI_ReadTransmitPowerLevelCmd(msg_ptr->hciParams.param1,
                                           msg_ptr->hciParams.param2);
      break;
#endif // ADV_CONN_CFG | INIT_CFG

    case HCI_READ_LOCAL_VERSION_INFO:
      stat = HCI_ReadLocalVersionInfoCmd();
      break;

    case HCI_READ_LOCAL_SUPPORTED_COMMANDS:
      stat = HCI_ReadLocalSupportedCommandsCmd();
      break;

    case HCI_READ_LOCAL_SUPPORTED_FEATURES:
      stat = HCI_ReadLocalSupportedFeaturesCmd();
      break;

    case HCI_READ_BDADDR:
      stat = HCI_ReadBDADDRCmd();
      break;

    case HCI_READ_RSSI:
      stat = HCI_ReadRssiCmd(msg_ptr->hciParams.param1);
      break;

    case HCI_LE_SET_EVENT_MASK:
      stat = HCI_LE_SetEventMaskCmd(msg_ptr->hciPtrParams.pParam1);
      break;

    case HCI_LE_READ_LOCAL_SUPPORTED_FEATURES:
      stat = HCI_LE_ReadLocalSupportedFeaturesCmd();
      break;

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
    case HCI_LE_READ_ADV_CHANNEL_TX_POWER:
      stat = HCI_LE_ReadAdvChanTxPowerCmd();
      break;
#endif // ADV_NCONN_CFG | ADV_CONN_CFG

    case HCI_LE_READ_WHITE_LIST_SIZE:
      stat = HCI_LE_ReadWhiteListSizeCmd();
      break;

    case HCI_LE_CLEAR_WHITE_LIST:
      stat = HCI_LE_ClearWhiteListCmd();
      break;

    case HCI_LE_ADD_WHITE_LIST:
      stat = HCI_LE_AddWhiteListCmd(msg_ptr->hciParamAndPtr.param,
                                    msg_ptr->hciParamAndPtr.pParam);
      break;

    case HCI_LE_REMOVE_WHITE_LIST:
      stat = HCI_LE_RemoveWhiteListCmd(msg_ptr->hciParamAndPtr.param,
                                       msg_ptr->hciParamAndPtr.pParam);
      break;

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
    case HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION:
      stat = HCI_LE_SetHostChanClassificationCmd(msg_ptr->hciPtrParams.pParam1);
      break;
#endif // INIT_CFG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
    case HCI_LE_READ_CHANNEL_MAP:
      stat = HCI_LE_ReadChannelMapCmd(msg_ptr->hciParams.param1);
      break;

    case HCI_LE_READ_REMOTE_USED_FEATURES:
      stat = HCI_LE_ReadRemoteUsedFeaturesCmd(msg_ptr->hciParams.param1);
      break;
#endif // ADV_CONN_CFG | INIT_CFG

    case HCI_LE_ENCRYPT:
      stat = HCI_LE_EncryptCmd(msg_ptr->hciPtrParams.pParam1,
                               msg_ptr->hciPtrParams.pParam2);
      break;

    case HCI_LE_READ_SUPPORTED_STATES:
      stat = HCI_LE_ReadSupportedStatesCmd();
      break;

    case HCI_LE_TRANSMITTER_TEST:
      stat = HCI_LE_TransmitterTestCmd(msg_ptr->hciLeTxTest.txChan,
                                       msg_ptr->hciLeTxTest.dataLen,
                                       msg_ptr->hciLeTxTest.payloadType);
      break;

    case HCI_LE_RECEIVER_TEST:
      stat = HCI_LE_ReceiverTestCmd(msg_ptr->hciParams.param1);
      break;

    case HCI_LE_TEST_END:
      stat = HCI_LE_TestEndCmd();
      break;

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
    case HCI_LE_READ_MAX_DATA_LENGTH:
      stat = HCI_LE_ReadMaxDataLenCmd();
      break;

    case HCI_LE_SET_DATA_LENGTH:
      stat = HCI_LE_SetDataLenCmd(msg_ptr->hciParams.param1,
                                  msg_ptr->hciParams.param2,
                                  msg_ptr->hciParams.param3);
      break;

    case HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH:
      stat = HCI_LE_WriteSuggestedDefaultDataLenCmd(msg_ptr->hciParams.param1,
                                                    msg_ptr->hciParams.param2);
      break;

    case HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH:
      stat = HCI_LE_ReadSuggestedDefaultDataLenCmd();
      break;
#endif // (ADV_CONN_CFG | INIT_CFG)

    /*
     * HCI Vendor Specific Commands: Link Layer Extensions
     */
    case HCI_EXT_SET_TX_POWER:
      stat = HCI_EXT_SetTxPowerCmd(msg_ptr->hciParams.param1);
      break;

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
    case HCI_EXT_ONE_PKT_PER_EVT:
      //stat = HCI_EXT_OnePktPerEvtCmd(msg_ptr->hciExtOnePktPerEvt.control);
      stat = HCI_EXT_OnePktPerEvtCmd(msg_ptr->hciParams.param1);
      break;
#endif // ADV_CONN_CFG | INIT_CFG

    case HCI_EXT_DECRYPT:
      stat = HCI_EXT_DecryptCmd(msg_ptr->hciPtrParams.pParam1,
                                msg_ptr->hciPtrParams.pParam2);
      break;

    case HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES:
      stat = HCI_EXT_SetLocalSupportedFeaturesCmd(msg_ptr->hciPtrParams.pParam1);
      break;

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
    case HCI_EXT_SET_FAST_TX_RESP_TIME:
      stat = HCI_EXT_SetFastTxResponseTimeCmd(msg_ptr->hciParams.param1);
      break;

    case HCI_EXT_OVERRIDE_SL:
      stat = HCI_EXT_SetSlaveLatencyOverrideCmd(msg_ptr->hciParams.param1);
      break;
#endif // ADV_CONN_CFG

    case HCI_EXT_MODEM_TEST_TX:
      stat = HCI_EXT_ModemTestTxCmd(msg_ptr->hciParams.param1,
                                    msg_ptr->hciParams.param2);
      break;

    case HCI_EXT_MODEM_HOP_TEST_TX:
      stat = HCI_EXT_ModemHopTestTxCmd();
      break;

    case HCI_EXT_MODEM_TEST_RX:
      stat = HCI_EXT_ModemTestRxCmd(msg_ptr->hciParams.param1);
      break;

    case HCI_EXT_END_MODEM_TEST:
      stat = HCI_EXT_EndModemTestCmd();
      break;

    case HCI_EXT_SET_BDADDR:
      stat = HCI_EXT_SetBDADDRCmd(msg_ptr->hciPtrParams.pParam1);
      break;

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
    case HCI_EXT_SET_SCA:
      stat = HCI_EXT_SetSCACmd(msg_ptr->hciParams.param1);
      break;
#endif // ADV_CONN_CFG | INIT_CFG

    case HCI_EXT_ENABLE_PTM:
      stat = HCI_EXT_EnablePTMCmd();
      break;

    case HCI_EXT_SET_MAX_DTM_TX_POWER:
      stat = HCI_EXT_SetMaxDtmTxPowerCmd(msg_ptr->hciParams.param1);
      break;

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
    case HCI_EXT_DISCONNECT_IMMED:
      stat = HCI_EXT_DisconnectImmedCmd(msg_ptr->hciParams.param1);
      break;

    case HCI_EXT_PER:
      stat = HCI_EXT_PacketErrorRateCmd(msg_ptr->hciParams.param1,
                                        msg_ptr->hciParams.param2);
      break;

    case HCI_EXT_PER_BY_CHAN:
      stat = HCI_EXT_PERbyChanCmd(msg_ptr->hciParamAndPtr.param,
                                  (perByChan_t *)msg_ptr->hciParamAndPtr.pParam);
      break;
#endif // ADV_CONN_CFG | INIT_CFG

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
    case HCI_EXT_ADV_EVENT_NOTICE:
      stat = HCI_EXT_AdvEventNoticeCmd(msg_ptr->hciParams.param1,
                                       msg_ptr->hciParams.param2);
      break;
#endif // ADV_NCONN_CFG | ADV_CONN_CFG

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
    case HCI_EXT_CONN_EVENT_NOTICE:
      stat = HCI_EXT_ConnEventNoticeCmd(msg_ptr->hciParams.param1,
                                        msg_ptr->hciParams.param2,
                                        msg_ptr->hciParams.param3);
      break;
#endif // ADV_CONN_CFG | INIT_CFG

    case HCI_EXT_BUILD_REVISION:
      stat = HCI_EXT_BuildRevisionCmd(msg_ptr->hciParams.param1,
                                      msg_ptr->hciParams.param2);
      break;

    case HCI_EXT_DELAY_SLEEP:
      stat = HCI_EXT_DelaySleepCmd(msg_ptr->hciParams.param1);
      break;

    case HCI_EXT_RESET_SYSTEM:
      stat = HCI_EXT_ResetSystemCmd(msg_ptr->hciParams.param1);
      break;

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
    case HCI_EXT_NUM_COMPLETED_PKTS_LIMIT:
      stat = HCI_EXT_NumComplPktsLimitCmd(msg_ptr->hciParams.param1,
                                          msg_ptr->hciParams.param2);
      break;

    case HCI_EXT_GET_CONNECTION_INFO:
      stat = HCI_EXT_GetConnInfoCmd(msg_ptr->hciPtrParams.pParam1,
                                    msg_ptr->hciPtrParams.pParam2,
                                    (hciConnInfo_t *)msg_ptr->hciPtrParams.pParam3);
      break;
#endif // ADV_CONN_CFG | INIT_CFG

    default:
      stat = FAILURE;
      break;
  }

  return (stat);
}

/*********************************************************************
 * @fn      sendGapCmdStatus
 *
 * @brief   Send GAP Command Status event.
 *
 * @param   taskId - task to send message to.
 * @param   opCode - command opcode.
 * @param   cmdId - command id.
 * @param   status - status.
 * @param   len - length of outgoing data.
 * @param   pData - outgoing data.
 *
 * @return  none
 */
static void sendGapCmdStatus(uint8 taskId, uint16 opCode, uint8 cmdId,
                             uint8 status, uint8 len, uint8 *pData)
{
  ICall_GapCmdStatus *pMsg =
    (ICall_GapCmdStatus *)osal_msg_allocate(sizeof(ICall_GapCmdStatus) + len);

  if (pMsg)
  {
    pMsg->hdr.hdr.event = ICALL_EVENT_EVENT;
    pMsg->hdr.hdr.status = status;
    pMsg->hdr.eventOpcode = HCI_EXT_GAP_CMD_STATUS_EVENT;
    pMsg->opCode = opCode;
    pMsg->cmdId = cmdId;
    pMsg->len = len;
    pMsg->pValue = (uint8 *)(pMsg+1);

    VOID osal_memcpy(pMsg->pValue, pData, len);

    VOID osal_msg_send(taskId, (uint8 *)pMsg);
  }
}

// Do not use SNV drivers with FPGA. Unverified behavior
#ifndef USE_FPGA
/*********************************************************************
 * @fn      checkNVLen
 *
 * @brief   Checks the size of NV items.
 *
 * @param   id - NV ID.
 * @param   len - lengths in bytes of item.
 *
 * @return  SUCCESS, INVALIDPARAMETER or FAILURE
 */
static uint8 checkNVLen(osalSnvId_t id, osalSnvLen_t len)
{
  uint8 stat = SUCCESS;

  switch (id)
  {
    case BLE_NVID_CSRK:
    case BLE_NVID_IRK:
      if (len != KEYLEN)
      {
        stat = INVALIDPARAMETER;
      }
      break;

    default:
#if defined(GAP_BOND_MGR)
      if ((id >= BLE_NVID_GAP_BOND_START) && (id <= BLE_NVID_GAP_BOND_END))
      {
        stat = gapBondMgr_CheckNVLen(id, len);
      }
      else
#endif
#if !defined(NO_OSAL_SNV) && !(defined(OSAL_SNV) && (OSAL_SNV == 0))
      if ((id >= BLE_NVID_CUST_START) && (id <= BLE_NVID_CUST_END))
      {
        stat = SUCCESS;
      }
      else
#endif
      {
        stat = INVALIDPARAMETER;  // Initialize status to failure
      }
      break;
  }

  return (stat);
}
#endif // USE_FPGA

/*********************************************************************
 * @fn      processICallUTIL
 *
 * @brief   Parse and process incoming ICall UTIL messages.
 *
 * @param   cmdID - incoming HCI extension command ID.
 * @param   msg_ptr - pointer to ICall message
 * @param   pRspDataLen - response data length to be returned.
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  SUCCESS, INVALIDPARAMETER and FAILURE.
 */
static uint8 processICallUTIL(uint8 cmdID, ICall_CmdMsg *msg_ptr,
                              uint8 *pRspDataLen, uint8 *pSendCS)
{
  bStatus_t stat;

  switch(cmdID)
  {
  // Do not use SNV drivers with FPGA. Unverified behavior
#ifndef USE_FPGA
    case HCI_EXT_UTIL_NV_READ:
      {
        osalSnvId_t id = msg_ptr->utilNvRead.id;
        osalSnvLen_t len = msg_ptr->utilNvRead.len;

        if (checkNVLen(id, len) == SUCCESS)
        {
          stat = osal_snv_read(id, len, msg_ptr->utilNvRead.pBuf);
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case HCI_EXT_UTIL_NV_WRITE:
      {
        osalSnvId_t id = msg_ptr->utilNvWrite.id;
        osalSnvLen_t len = msg_ptr->utilNvWrite.len;

        if (checkNVLen(id, len) == SUCCESS)
        {
          stat = osal_snv_write(id, len, msg_ptr->utilNvWrite.pBuf);
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;
#endif // USE_FPGA

    case HCI_EXT_UTIL_FORCE_BOOT:
      {
        extern void appForceBoot(void);
        appForceBoot();

        // Should never get here if SBL is present
        stat = INVALIDPARAMETER;
      }
      break;

    case HCI_EXT_UTIL_BUILD_REV:
        stat = buildRevision(msg_ptr->utilBuildRev.pBuildRev);
      break;

    case HCI_EXT_UTIL_GET_TRNG:
      {
        uint32    trngVal;
        CryptoKey entropy;
 
        // Initialise the CryptoKey
        CryptoKeyPlaintext_initKey(&entropy, (uint8 *)(&trngVal), 4); 

        TRNG_generateEntropy(trngHandle, &entropy);

        rspBuf[RSP_PAYLOAD_IDX]   = BREAK_UINT32( trngVal, 0 );
        rspBuf[RSP_PAYLOAD_IDX+1] = BREAK_UINT32( trngVal, 1 );
        rspBuf[RSP_PAYLOAD_IDX+2] = BREAK_UINT32( trngVal, 2 );
        rspBuf[RSP_PAYLOAD_IDX+3] = BREAK_UINT32( trngVal, 3 );

        *pRspDataLen = sizeof( uint32 );

        stat = SUCCESS;
      }
      break;

#if (HOST_CONFIG & (PERIPHERAL_CFG))
    case UTIL_EXT_GATT_GET_NEXT_HANDLE: // GATT Write Characteristic Descriptor
      {
          uint16 paramValue = 0xFFFF;

          paramValue = GATT_GetNextHandle();

          *pRspDataLen = sizeof(uint16);
          VOID osal_memcpy(&rspBuf[RSP_PAYLOAD_IDX], &paramValue, sizeof(uint16));

          stat = SUCCESS;

        break;
      }
#endif
  default:
      stat = FAILURE;
      break;
  }

  return (stat);
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
static uint8 buildRevision(ICall_BuildRevision *pBuildRev)
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
#if defined(HOST_CONFIG) && (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))

/*********************************************************************
 * @fn      processICallL2CAP
 *
 * @brief   Parse and process incoming ICall L2CAP messages
 *
 * @param   cmdID - incoming message command ID
 * @param   msg_ptr - pointer to incoming ICall message
 * @param   pSendCS - whether to send Command Status response back.
  *
 * @return  SUCCESS or FAILURE
 */
static uint8 processICallL2CAP(uint8 cmdID, ICall_CmdMsg *msg_ptr,
                               uint8 *pSendCS)
{
  bStatus_t stat;

  switch(cmdID)
  {
#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
    case HCI_EXT_L2CAP_DATA:
      stat = L2CAP_SendSDU(msg_ptr->l2capSendSDU.pPkt);
      break;

    case HCI_EXT_L2CAP_REGISTER_PSM:
      stat = L2CAP_RegisterPsm(msg_ptr->l2capRegisterPsm.pPsm);
      break;

    case HCI_EXT_L2CAP_DEREGISTER_PSM:
      stat = L2CAP_DeregisterPsm(msg_ptr->l2capDeregisterPsm.taskId,
                                 msg_ptr->l2capDeregisterPsm.psm);
      break;

    case HCI_EXT_L2CAP_PSM_INFO:
      stat = L2CAP_PsmInfo(msg_ptr->l2capPsmInfo.psm,
                           msg_ptr->l2capPsmInfo.pInfo);
      break;

    case HCI_EXT_L2CAP_PSM_CHANNELS:
      stat = L2CAP_PsmChannels(msg_ptr->l2capPsmChannels.psm,
                               msg_ptr->l2capPsmChannels.numCIDs,
                               msg_ptr->l2capPsmChannels.pCIDs);
      break;

    case HCI_EXT_L2CAP_CHANNEL_INFO:
      stat = L2CAP_ChannelInfo(msg_ptr->l2capChannelInfo.CID,
                               msg_ptr->l2capChannelInfo.pInfo);
      break;

    case L2CAP_CONNECT_REQ:
      stat = L2CAP_ConnectReq(msg_ptr->l2capConnectReq.connHandle,
                              msg_ptr->l2capConnectReq.psm,
                              msg_ptr->l2capConnectReq.peerPsm);
      break;

    case L2CAP_CONNECT_RSP:
      stat = L2CAP_ConnectRsp(msg_ptr->l2capConnectRsp.connHandle,
                              msg_ptr->l2capConnectRsp.id,
                              msg_ptr->l2capConnectRsp.result);
      break;

    case L2CAP_DISCONNECT_REQ:
      stat = L2CAP_DisconnectReq(msg_ptr->l2capDisconnectReq.CID);
      break;

    case L2CAP_FLOW_CTRL_CREDIT:
      stat = L2CAP_FlowCtrlCredit(msg_ptr->l2capFlowCtrlCredit.CID,
                                  msg_ptr->l2capFlowCtrlCredit.peerCredits);
      break;
#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)

    case L2CAP_PARAM_UPDATE_REQ:
      stat =  L2CAP_ConnParamUpdateReq(msg_ptr->l2capParamUpdateReq.connHandle,
                                       msg_ptr->l2capParamUpdateReq.pUpdateReq,
                                       msg_ptr->l2capParamUpdateReq.taskId);
      break;

    default:
      stat = FAILURE;
      break;
  }

  return (stat);
}

/*********************************************************************
 * @fn      processICallATT
 *
 * @brief   Parse and process incoming ICall ATT message
 *
 * @param   cmdID - incoming message command ID
 * @param   msg_ptr - pointer to incoming ICall message
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE,
 *          bleInvalidPDU or bleMemAllocError
 */
static uint8 processICallATT(uint8 cmdID, ICall_CmdMsg *msg_ptr, uint8 *pSendCS)
{
  uint16 connHandle = msg_ptr->attParamAndPtr.connHandle;
  attMsg_t *pMsg = msg_ptr->attParamAndPtr.pMsg;
  bStatus_t stat;

  switch(cmdID)
  {
    case ATT_ERROR_RSP:
      stat = ATT_ErrorRsp(connHandle, &pMsg->errorRsp);
      break;

    case ATT_READ_RSP:
      stat = ATT_ReadRsp(connHandle, &pMsg->readRsp);
      break;

    case ATT_READ_BLOB_RSP:
      stat = ATT_ReadBlobRsp(connHandle, &pMsg->readBlobRsp);
      break;

    case ATT_WRITE_RSP:
      stat = ATT_WriteRsp(connHandle);
      break;

    case ATT_EXECUTE_WRITE_RSP:
      stat = ATT_ExecuteWriteRsp(connHandle);
      break;

#if !defined(GATT_NO_CLIENT)
    case ATT_HANDLE_VALUE_CFM:
      stat = ATT_HandleValueCfm(connHandle);
      break;
#endif // !GATT_NO_CLIENT

    default:
      /*
       * For all other ATT commands, the corresponding GATT procedure should
       * be used instead.
       */
      stat = FAILURE;
      break;
  }

  return (stat);
}

/*********************************************************************
 * @fn      processICallGATT
 *
 * @brief   Parse and process incoming ICall GATT message
 *
 * @param   cmdID - incoming message command ID
 * @param   msg_ptr - pointer to incoming ICall message
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE,
 *          bleInvalidPDU or bleMemAllocError
 */
static uint8 processICallGATT(uint8 cmdID, ICall_CmdMsg *msg_ptr,
                              uint8 *pSendCS)
{
#if !defined(GATT_NO_CLIENT)
  attMsg_t *pReq = msg_ptr->gattReq.pReq;
  uint8 taskId = msg_ptr->gattReq.taskId;
#endif // !GATT_NO_CLIENT
  uint16 connHandle = msg_ptr->gattReq.connHandle;
  bStatus_t stat;

  switch(cmdID)
  {
#if !defined(GATT_NO_CLIENT)
    case ATT_EXCHANGE_MTU_REQ: // GATT Exchange MTU
      stat = GATT_ExchangeMTU(connHandle, &pReq->exchangeMTUReq, taskId);
      break;

    case ATT_READ_BY_GRP_TYPE_REQ: // GATT Discover All Primary Services
      {
        // GATT_DiscAllPrimaryServices in icall_api.c does not call standard
        // gattRequest(), meaning msg_ptr is not of type gattReq. Local
        // connHandle and taskId values cannot be assumed correct. Must cast
        // msg_ptr to proper type in union to get these fields
        ICall_GattDiscAllPrimaryServ *pDisc = &msg_ptr->gattDiscAllPrimaryServ;

        stat = GATT_DiscAllPrimaryServices(pDisc->connHandle, pDisc->taskId);
      }
      break;

    case ATT_FIND_BY_TYPE_VALUE_REQ: // GATT Discover Primary Service By UUID
      {
        ICall_GattDiscPrimServByUUID *pUUID = &msg_ptr->gattDiscPrimServByUUID;

        stat = GATT_DiscPrimaryServiceByUUID(connHandle, pUUID->pValue,
                                             pUUID->len, pUUID->taskId);
      }
      break;

    case GATT_FIND_INCLUDED_SERVICES: // GATT Find Included Services
    case GATT_DISC_ALL_CHARS: // GATT Discover All Characteristics
      {
        // First requested handle number
        uint16 startHandle = msg_ptr->gattDiscAllChars.startHandle;

        // Last requested handle number
        uint16 endHandle = msg_ptr->gattDiscAllChars.endHandle;

        if (cmdID == GATT_FIND_INCLUDED_SERVICES)
        {
          stat = GATT_FindIncludedServices(connHandle, startHandle, endHandle,
                                           msg_ptr->gattDiscAllChars.taskId);
        }
        else
        {
          stat = GATT_DiscAllChars(connHandle, startHandle, endHandle,
                                   msg_ptr->gattDiscAllChars.taskId);
        }
      }
      break;

    case ATT_READ_BY_TYPE_REQ: // GATT Discover Characteristics by UUID
      stat = GATT_DiscCharsByUUID(connHandle, &pReq->readByTypeReq, taskId);
      break;

    case ATT_FIND_INFO_REQ: // GATT Discover All Characteristic Descriptors
      stat = GATT_DiscAllCharDescs(connHandle, pReq->findInfoReq.startHandle,
                                   pReq->findInfoReq.endHandle, taskId);
      break;

    case ATT_READ_REQ: // GATT Read Characteristic Value
      stat = GATT_ReadCharValue(connHandle, &pReq->readReq, taskId);
      break;

    case GATT_READ_USING_CHAR_UUID: // GATT Read Using Characteristic UUID
      stat = GATT_ReadUsingCharUUID(connHandle, &pReq->readByTypeReq, taskId);
      break;

    case ATT_READ_BLOB_REQ: // GATT Read Long Characteristic Value
      stat = GATT_ReadLongCharValue(connHandle, &pReq->readBlobReq, taskId);
      break;

    case ATT_READ_MULTI_REQ: // GATT Read Multiple Characteristic Values
      stat = GATT_ReadMultiCharValues(connHandle, &pReq->readMultiReq, taskId);
      break;

    case GATT_WRITE_NO_RSP: // GATT Write Without Response
      stat = GATT_WriteNoRsp(connHandle, &pReq->writeReq);
      break;

    case GATT_SIGNED_WRITE_NO_RSP: // GATT Signed Write Without Response
      stat = GATT_SignedWriteNoRsp(connHandle, &pReq->writeReq);
      break;

    case ATT_WRITE_REQ: // GATT Write Characteristic Value
      stat = GATT_WriteCharValue(connHandle, &pReq->writeReq, taskId);
      break;

    case ATT_PREPARE_WRITE_REQ: // GATT Write Long Characteristic Value
    case GATT_WRITE_LONG_CHAR_DESC: // GATT Write Long Characteristic Descriptor
      {
        attPrepareWriteReq_t *pReq = msg_ptr->gattWriteLong.pReq;

        if (cmdID == ATT_PREPARE_WRITE_REQ)
        {
          stat = GATT_WriteLongCharValue(connHandle, pReq, taskId);
        }
        else
        {
          stat = GATT_WriteLongCharDesc(connHandle, pReq, taskId);
        }
      }
      break;

    case GATT_RELIABLE_WRITES: // GATT Reliable Writes
      {
        uint8 numReqs = msg_ptr->gattReliableWrite.numReqs;

        if ((numReqs > 0) && (numReqs <= GATT_MAX_NUM_RELIABLE_WRITES))
        {
          attPrepareWriteReq_t *pReqs = msg_ptr->gattReliableWrite.pReqs;

          // Send all saved Prepare Write Requests
          stat = GATT_ReliableWrites(connHandle, pReqs, numReqs,
                                     ATT_WRITE_PREPARED_VALUES,
                                     msg_ptr->gattReliableWrite.taskId);
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case GATT_READ_CHAR_DESC: // GATT Read Characteristic Descriptor
      stat = GATT_ReadCharDesc(connHandle, &pReq->readReq, taskId);
      break;

    case GATT_READ_LONG_CHAR_DESC: // GATT Read Long Characteristic Descriptor
      stat = GATT_ReadLongCharDesc(connHandle, &pReq->readBlobReq, taskId);
      break;

    case GATT_WRITE_CHAR_DESC: // GATT Write Characteristic Descriptor
      stat = GATT_WriteCharDesc(connHandle, &pReq->writeReq, taskId);
      break;
#endif // !GATT_NO_CLIENT

    case ATT_HANDLE_VALUE_NOTI:
      {
        attMsg_t *pIndNoti = msg_ptr->gattInd.pIndNoti;

        stat = GATT_Notification(connHandle, &pIndNoti->handleValueNoti,
                                 msg_ptr->gattInd.authenticated);
      }
      break;

    case ATT_HANDLE_VALUE_IND:
      {
        attMsg_t *pIndNoti = msg_ptr->gattInd.pIndNoti;

        stat = GATT_Indication(connHandle, &pIndNoti->handleValueInd,
                               msg_ptr->gattInd.authenticated,
                               msg_ptr->gattInd.taskId);
      }
      break;

    case GATT_CCC_UPDATE:
      {
        ICall_cccUpdateMsg pMsg = msg_ptr->cccUpdateMsg;

        GATTServApp_SendCCCUpdatedEvent(pMsg.connHandle, pMsg.attrHandle,
                                        pMsg.value);

        stat = SUCCESS;
      }
      break;

    default:
      stat = FAILURE;
      break;
  }

  return (stat);
}
#endif /* defined(HOST_CONFIG) && (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG)) */

#if defined(HOST_CONFIG)
/*********************************************************************
 * @fn      processICallGAP
 *
 * @brief   Parse and process incoming ICall GAP message
 *
 * @param   cmdID - incoming message command ID
 * @param   msg_ptr - pointer to incoming ICall message
 * @param   pRspDataLen - response data length to be returned.
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE,
 *          or bleMemAllocError
 */
static uint8 processICallGAP(uint8 cmdID, ICall_CmdMsg *msg_ptr,
                             uint8 *pRspDataLen, uint8 *pSendCS)
{
  bStatus_t stat = SUCCESS;

  switch(cmdID)
  {
    case HCI_EXT_GAP_DEVICE_INIT:
      {
        ICall_GapDeviceInit *pDevInit = &msg_ptr->gapDeviceInit;

        stat = GAP_DeviceInit(pDevInit->profileRole,
                              pDevInit->taskID,
                              pDevInit->addrType,
                              pDevInit->pRandomAddr);
      }
      break;

#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
    case HCI_EXT_GAP_TERMINATE_LINK:
      stat = GAP_TerminateLinkReq(msg_ptr->gapTerminateLink.connHandle,
                                  msg_ptr->gapTerminateLink.reason);
      break;

    case HCI_EXT_GAP_UPDATE_LINK_PARAM_REQ:
      stat = GAP_UpdateLinkParamReq(
               (gapUpdateLinkParamReq_t *)msg_ptr->gapPtrParams.pParam1);
      break;

    case HCI_EXT_GAP_UPDATE_LINK_PARAM_REQ_REPLY:
      stat = GAP_UpdateLinkParamReqReply(
            (gapUpdateLinkParamReqReply_t *)msg_ptr->gapPtrParams.pParam1);
      break;

    case HCI_EXT_GAP_AUTHENTICATE:
      stat = GAP_Authenticate((gapAuthParams_t *)msg_ptr->gapPtrParams.pParam1,
                              (gapPairingReq_t *)msg_ptr->gapPtrParams.pParam2);
      break;

    case HCI_EXT_GAP_TERMINATE_AUTH:
      stat = GAP_TerminateAuth(msg_ptr->gapParams.connHandle,
                               msg_ptr->gapParams.param);
      break;

    case HCI_EXT_GAP_BOND:
      stat = GAP_Bond(msg_ptr->gapBondParams.connHandle,
                      msg_ptr->gapBondParams.authenticated,
                      msg_ptr->gapBondParams.secureConnections,
                      msg_ptr->gapBondParams.pParams,
                      msg_ptr->gapBondParams.startEncryption);
      break;

    case HCI_EXT_GAP_SIGNABLE:
      stat = GAP_Signable(msg_ptr->gapEnableSignedData.connHandle,
                          msg_ptr->gapEnableSignedData.authenticated,
                          msg_ptr->gapEnableSignedData.pParams);
      break;

    case HCI_EXT_GAP_PASSKEY_UPDATE:
      stat = GAP_PasskeyUpdate(msg_ptr->gapPassKeyUpdateParams.pPasskey,
                               msg_ptr->gapPassKeyUpdateParams.connHandle);
      break;
#endif // CENTRAL_CFG | PERIPHERAL_CFG

    case HCI_EXT_GAP_SET_PARAM:
      {
        uint16 id = msg_ptr->gapSetParam.paramID;
        uint16 param = msg_ptr->gapSetParam.paramValue;

        if ((id != GAP_PARAM_AUTH_TASK_ID) && (id < GAP_PARAMID_MAX))
        {
          stat = GAP_SetParamValue(id, param);
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case HCI_EXT_GAP_GET_PARAM:
      {
        uint16 paramValue = 0xFFFF;
        uint16 param = msg_ptr->gapSetParam.paramID;

        if (param < 0x00FF)
        {
          if ((param != GAP_PARAM_AUTH_TASK_ID) && (param < GAP_PARAMID_MAX))
          {
            paramValue = GAP_GetParamValue(param);
          }
        }

        if (paramValue != 0xFFFF)
        {
          *pRspDataLen = sizeof(uint16);
          VOID osal_memcpy(&rspBuf[RSP_PAYLOAD_IDX], &paramValue, sizeof(uint16));

          stat = SUCCESS;
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

#if (HOST_CONFIG & PERIPHERAL_CFG)
    case HCI_EXT_GAP_SLAVE_SECURITY_REQ_UPDATE:
      stat = GAP_SendSlaveSecurityRequest(msg_ptr->gapParams.connHandle,
                                          msg_ptr->gapParams.param);
      break;
#endif // PERIPHERAL_CFG

    case HCI_EXT_GAP_BOND_SET_PARAM:
#if defined(GAP_BOND_MGR)
      {
        ICall_paramIdLenVal *pParam = &msg_ptr->profileSetParam.paramIdLenVal;

        stat = GAPBondMgr_SetParameter(pParam->paramId, pParam->len,
                                       pParam->pValue);
      }
#else
      stat = INVALIDPARAMETER;
#endif
      break;

    case HCI_EXT_GAP_BOND_GET_PARAM:
      {
        uint8 len = 0;
#if defined(GAP_BOND_MGR)
        uint16 id = msg_ptr->gapGetParam.paramID;

        stat = GAPBondMgr_GetParameter(id, &rspBuf[RSP_PAYLOAD_IDX]);

        switch (id)
        {
          case GAPBOND_INITIATE_WAIT:
            len = 2;
            break;

          case GAPBOND_OOB_DATA:
            len = KEYLEN;
            break;

         case GAPBOND_DEFAULT_PASSCODE:
           len = 4;
           break;

         default:
           len = 1;
           break;
        }
#else
        stat = INVALIDPARAMETER;
#endif
        *pRspDataLen = len;
      }
      break;

#if !defined(GATT_NO_SERVICE_CHANGED)
    case HCI_EXT_GAP_BOND_SERVICE_CHANGE:
#if defined(GAP_BOND_MGR)
      stat = GAPBondMgr_ServiceChangeInd(msg_ptr->gapParams.connHandle,
                                         msg_ptr->gapParams.param);
#else
      stat = INVALIDPARAMETER;
#endif
      break;
#endif // !GATT_NO_SERVICE_CHANGED

    case HCI_EXT_SM_GET_CONFIRM_OOB:
      {
        stat = SM_GetScConfirmOob(msg_ptr->smGetSCConfirmOob.publicKey,
                                  msg_ptr->smGetSCConfirmOob.oob,
                                  &rspBuf[RSP_PAYLOAD_IDX]);

        *pRspDataLen = 16;
      }
      break;

    default:
      stat = FAILURE;
      break;
  }

  return stat;
}
#endif // HOST_CONFIG

/*********************************************************************
 * @fn      processDispMsg
 *
 * @brief   Parse and process incoming Dispatch message
 *
 * @param   msg_ptr - pointer to incoming ICall Command message
 *
 * @return  true
 */
static uint8 processDispMsg(ICall_CmdMsg *msg_ptr)
{
  bStatus_t stat;
  uint8 rspDataLen = 0;
  uint8 sendCmdStatus = TRUE;
  uint16 subGroup = msg_ptr->hciExtCmd.opCode;
  uint8 cmdId = msg_ptr->hciExtCmd.cmdId;
  uint8 taskId = msg_ptr->hciExtCmd.srctaskid;

  switch(subGroup)
  {
    case DISPATCH_GENERAL:
      stat = processDispGeneral(msg_ptr, &sendCmdStatus);
      break;

#if defined(HOST_CONFIG)
    case DISPATCH_GAP_PROFILE:
      stat = processDispGAPProfile(msg_ptr, &rspDataLen, &sendCmdStatus);
      break;

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
    case DISPATCH_GATT_PROFILE:
      stat = processDispGATTProfile(msg_ptr, &sendCmdStatus);
      break;

#if !defined(GATT_DB_OFF_CHIP)
    case DISPATCH_GAP_GATT_SERV:
      stat = processDispGGS(msg_ptr, &sendCmdStatus);
      break;

    case DISPATCH_GATT_SERV_APP:
      stat = processDispGSA(msg_ptr, &sendCmdStatus);
      break;
#endif // !GATT_DB_OFF_CHIP
#endif // CENTRAL_CFG | PERIPHERAL_CFG
#endif // HOST_CONFIG

    default:
      stat = FAILURE;
      break;
  }

  // Deallocate here to free up heap space for the serial message set out HCI.
  VOID osal_msg_deallocate((uint8 *)msg_ptr);

  if (sendCmdStatus)
  {
    // Send back an immediate response
    sendGapCmdStatus(taskId, subGroup, cmdId, stat, rspDataLen,
                     &rspBuf[RSP_PAYLOAD_IDX]);
  }

  return (FALSE);
}

/*********************************************************************
 * @fn      processDispGeneral
 *
 * @brief   Parse and process incoming Dispatch General message
 *
 * @param   msg_ptr - pointer to incoming ICall message
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  SUCCESS, INVALIDPARAMETER or FAILURE
 */
static uint8 processDispGeneral(ICall_CmdMsg *msg_ptr, uint8 *pSendCS)
{
  bStatus_t stat = SUCCESS;
  uint16 cmdID = msg_ptr->hciExtCmd.cmdId;

  switch(cmdID)
  {
#if !defined(HCI_TL_NONE)
    case DISPATCH_GENERAL_REG_NPI:
      NPI_RegisterTask(msg_ptr->registerTaskMsg.taskID);
      *pSendCS = FALSE;
      break;
#endif // !HCI_TL_NONE

#if defined(HOST_CONFIG) & (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
    case DISPATCH_GENERAL_REG_L2CAP_FC:
      L2CAP_RegisterFlowCtrlTask(msg_ptr->registerTaskMsg.taskID);
      *pSendCS = FALSE;
      break;
#endif // HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG )

    default:
      stat = FAILURE;
      break;
  }

  return (stat);
}

#if defined(HOST_CONFIG)
/*********************************************************************
 * @fn      processDispGAPProfile
 *
 * @brief   Parse and process incoming Dispatch GAP Profile message
 *
 * @param   msg_ptr - pointer to incoming ICall message.
 * @param   pRspDataLen - response data length to be returned.
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  SUCCESS, INVALIDPARAMETER or FAILURE
 */
static uint8 processDispGAPProfile(ICall_CmdMsg *msg_ptr, uint8 *pRspDataLen,
                                   uint8 *pSendCS)
{
  bStatus_t stat = SUCCESS;
  uint16 cmdID = msg_ptr->hciExtCmd.cmdId;

  switch(cmdID)
  {
    case DISPATCH_GAP_REG_FOR_MSG:
      GAP_RegisterForMsgs(msg_ptr->registerTaskMsg.taskID);
      *pSendCS = FALSE;
      break;

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
    case DISPATCH_GAP_LINKDB_STATE:
      rspBuf[RSP_PAYLOAD_IDX] = linkDB_State(msg_ptr->linkDBState.connHandle,
                                             msg_ptr->linkDBState.state);
      *pRspDataLen = sizeof(uint8);
      break;

    case DISPATCH_GAP_LINKDB_NUM_CONNS:
      rspBuf[RSP_PAYLOAD_IDX] = linkDB_NumConns();
      *pRspDataLen = sizeof(uint8);
      break;

    case DISPATCH_GAP_LINKDB_NUM_ACTIVE:
      rspBuf[RSP_PAYLOAD_IDX] = linkDB_NumActive();
      *pRspDataLen = sizeof(uint8);
      break;

    case DISPATCH_GAP_LINKDB_GET_INFO:
      {
        ICall_LinkDBGetInfo *pMsg = &msg_ptr->linkDBGetInfo;

        stat = linkDB_GetInfo( pMsg->connHandle, pMsg->pInfo );
      }
      break;
#endif // CENTRAL_CFG | PERIPHERAL_CFG

    case DISPATCH_PROFILE_REG_CB:
#if defined(GAP_BOND_MGR)
      GAPBondMgr_Register(msg_ptr->gapBondMgrRegister.pCB);
#endif
      *pSendCS = FALSE;
      break;

    case DISPATCH_GAP_BOND_PASSCODE_RSP:
#if defined(GAP_BOND_MGR)
      {
        ICall_BondMgrPasscodeRsp *pRsp = &msg_ptr->gapBondMgrPasscodeRsp;

        stat = GAPBondMgr_PasscodeRsp(pRsp->connHandle, pRsp->status,
                                      pRsp->passcode);
      }
#else
      stat = INVALIDPARAMETER;
#endif
      break;

    case DISPATCH_GAP_BOND_FIND_ADDR:
#if defined(GAP_BOND_MGR)
      {
        ICall_BondMgrFindAddr *pReq = &msg_ptr->gapbondMgrResolveAddr;

        rspBuf[RSP_PAYLOAD_IDX] = GAPBondMgr_FindAddr(pReq->addrType,
        							                                pReq->pDevAddr,
                                                      pReq->pResolvedAddr);
      }
#else
      rspBuf[RSP_PAYLOAD_IDX] = GAP_BONDINGS_MAX;
#endif
      *pRspDataLen = sizeof(uint8);
      break;

    default:
      stat = FAILURE;
      break;
  }

  return (stat);
}

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
/*********************************************************************
 * @fn      processDispGATTProfile
 *
 * @brief   Parse and process incoming Dispatch GATT Bond Manager
 *          message
 *
 * @param   msg_ptr - pointer to incoming ICall message
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  SUCCESS, INVALIDPARAMETER or FAILURE
 */
static uint8 processDispGATTProfile(ICall_CmdMsg *msg_ptr, uint8 *pSendCS)
{
  bStatus_t stat = SUCCESS;
  uint16 cmdID = msg_ptr->hciExtCmd.cmdId;

  switch(cmdID)
  {
    case DISPATCH_GATT_REG_FOR_MSG:
      GATT_RegisterForMsgs(msg_ptr->registerTaskMsg.taskID);
      *pSendCS = FALSE;
      break;

#if !defined(GATT_NO_CLIENT)
    case DISPATCH_GATT_INIT_CLIENT:
      // Initialize GATT Client
      stat = GATT_InitClient();
      break;

    case DISPATCH_GATT_REG_4_IND:
      // Register to receive incoming ATT Indications/Notifications
      GATT_RegisterForInd(msg_ptr->gattRegisterForInd.taskId);
      *pSendCS = FALSE;
      break;
#endif // !GATT_NO_CLIENT

    case DISPATCH_GATT_HTA_FLOW_CTRL:
      // Set Host to App Flow Control
      GATT_SetHostToAppFlowCtrl(msg_ptr->gattHtaFlowCtrl.heapSize,
                                msg_ptr->gattHtaFlowCtrl.flowCtrlMode);
      *pSendCS = FALSE;
      break;

    case DISPATCH_GATT_APP_COMPL_MSG:
      // Notify GATT of App Completed Message
      GATT_AppCompletedMsg(msg_ptr->gattAppComplMsg.pMsg);
      *pSendCS = FALSE;
      break;

    case DISPATCH_GATT_SEND_RSP:
      stat = GATT_SendRsp(msg_ptr->gattSendRsp.connHandle,
                          msg_ptr->gattSendRsp.method,
                          msg_ptr->gattSendRsp.pRsp);
      break;

    default:
      stat = FAILURE;
      break;
  }

  return (stat);
}

#if !defined(GATT_DB_OFF_CHIP)
/*********************************************************************
 * @fn      processDispGGS
 *
 * @brief   Parse and process incoming Dispatch GAP GATT Service
 *
 * @param   msg_ptr - pointer to incoming ICall message
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  SUCCESS, INVALIDPARAMETER or FAILURE
 */
static uint8 processDispGGS(ICall_CmdMsg *msg_ptr, uint8 *pSendCS)
{
  bStatus_t stat = SUCCESS;
  uint16 cmdID = msg_ptr->hciExtCmd.cmdId;

  switch(cmdID)
  {
#if !defined ( GATT_QUAL )
    case DISPATCH_PROFILE_ADD_SERVICE:
      stat = GGS_AddService(msg_ptr->profileAddService.services);
      break;
#endif // GATT_QUAL

    case DISPATCH_PROFILE_SET_PARAM:
      {
        ICall_paramIdLenVal *pParam = &msg_ptr->profileSetParam.paramIdLenVal;

        // Set the GAP GATT Characteristics
        stat = GGS_SetParameter(pParam->paramId, pParam->len, pParam->pValue);
      }
      break;

    case DISPATCH_PROFILE_GET_PARAM:
      {
        ICall_paramIdVal *pParam = &msg_ptr->profileGetParam.paramIdVal;

        // Get the GAP GATT Characteristics
        stat = GGS_GetParameter(pParam->paramId, pParam->pValue);
      }
      break;

    case DISPATCH_PROFILE_REG_CB:
      {
#if ( HOST_CONFIG & PERIPHERAL_CFG ) & defined ( GAP_PRIVACY_RECONNECT )
        // GAP Bond Manager is given priority to register for callbacks
        // to the privacy reconnect address attribute
        stat = FAILURE;
#else
        GGS_RegisterAppCBs(msg_ptr->ggsRegister.pCB);
#endif // PERIPERAL_CFG & GAP_PRIVACY_RECONNECT
      }
      break;

    default:
      stat = FAILURE;
      break;
  }

  return (stat);
}

/*********************************************************************
 * @fn      processDispGSA
 *
 * @brief   Parse and process incoming Dispatch GATT Server App
 *
 * @param   msg_ptr - pointer to incoming ICall message
 * @param   pSendCS - whether to send Command Status response back.
 *
 * @return  SUCCESS, INVALIDPARAMETER or FAILURE
 */
static uint8 processDispGSA(ICall_CmdMsg *msg_ptr, uint8 *pSendCS)
{
  bStatus_t stat = SUCCESS;
  uint16 cmdID = msg_ptr->hciExtCmd.cmdId;

  switch(cmdID)
  {
#if defined ( GATT_QUAL )
    case DISPATCH_GSA_ADD_QUAL_SERVICE:
      stat = GATTQual_AddService(msg_ptr->profileAddService.services);
      break;
#else
    case DISPATCH_PROFILE_ADD_SERVICE:
      stat = GATTServApp_AddService(msg_ptr->profileAddService.services);
      break;

#if defined ( GATT_TEST )
    case DISPATCH_GSA_ADD_TEST_SERVICE:
      stat = GATTTest_AddService(msg_ptr->profileAddService.services);
      break;
#endif // GATT_TEST
#endif // GATT_QUAL

    case DISPATCH_PROFILE_REG_SERVICE:
      {
        ICall_GSA_RegService *pServ = &msg_ptr->gsaRegService;

        stat = GATTServApp_RegisterService(pServ->pAttrs, pServ->numAttrs,
                                           pServ->encKeySize, pServ->pServiceCBs);
      }
      break;

    case DISPATCH_PROFILE_DEREG_SERVICE:
      stat = GATTServApp_DeregisterService(msg_ptr->gsaDeregService.handle,
                                           msg_ptr->gsaDeregService.p2pAttrs);
      break;

    case DISPATCH_PROFILE_SET_PARAM:
      {
        ICall_paramIdLenVal *pParam = &msg_ptr->profileSetParam.paramIdLenVal;

        // Set the GAP GATT Characteristics
        stat = GATTServApp_SetParameter(pParam->paramId, pParam->len,
                                        pParam->pValue);
      }
      break;

    case DISPATCH_PROFILE_GET_PARAM:
      {
        ICall_paramIdVal *pParam = &msg_ptr->profileGetParam.paramIdVal;

        // Get the GAP GATT Characteristics
        stat = GATTServApp_GetParameter(pParam->paramId, pParam->pValue);
      }
      break;

#if !defined(GATT_NO_SERVICE_CHANGED)
    case DISPATCH_GSA_SERVICE_CHANGE_IND:
      {
        ICall_GSA_ServiceChangeInd *pInd = &msg_ptr->gsaServiceChangeInd;

        stat = GATTServApp_SendServiceChangedInd(pInd->connHandle, pInd->taskId);
      }
      break;
#endif // !GATT_NO_SERVICE_CHANGED

#ifdef ATT_DELAYED_REQ
    case DISPATCH_PROFILE_DELAYED_RSP:
      {
        ICall_GSA_ReadRsp *pRsp = &msg_ptr->gsaReadRsp;
        stat = GATTServApp_ReadRsp(pRsp->connHandle, pRsp->pAttrValue,
                                   pRsp->attrLen, pRsp->attrHandle);
      }
      break;
#endif // ATT_DELAYED_REQ

    default:
      stat = FAILURE;
      break;
  }

  return (stat);
}
#endif // !GATT_DB_OFF_CHIP
#endif // (CENTRAL_CFG | PERIPHERAL_CFG)
#endif // HOST_CONFIG

#if defined(HCI_TL_FULL)
/*********************************************************************
 * @fn      processExtMsg
 *
 * @brief   Parse and process incoming HCI extension messages.
 *
 * @param   pMsg - incoming HCI extension message.
 *
 * @return  none
 */
static uint8 processExtMsg( hciPacket_t *pMsg )
{
  uint8 deallocateIncoming;
  bStatus_t stat = SUCCESS;
  uint8 rspDataLen = 0;
  hciExtCmd_t msg;
  uint8 *pBuf = pMsg->pData;

  // Parse the header
  msg.pktType = *pBuf++;
  msg.opCode = BUILD_UINT16( pBuf[0], pBuf[1] );
  pBuf += 2;

  msg.len = *pBuf++;
  msg.pData = pBuf;

  switch( msg.opCode >> 7 )
  {
#if defined(HOST_CONFIG)
    case HCI_EXT_L2CAP_SUBGRP:
      stat = processExtMsgL2CAP( (msg.opCode & 0x007F), &msg, &rspDataLen );
      break;

    case HCI_EXT_ATT_SUBGRP:
      stat = processExtMsgATT( (msg.opCode & 0x007F), &msg );
      break;

    case HCI_EXT_GATT_SUBGRP:
      stat = processExtMsgGATT( (msg.opCode & 0x007F), &msg, &rspDataLen );
      break;

    case HCI_EXT_GAP_SUBGRP:
      stat = processExtMsgGAP( (msg.opCode & 0x007F), &msg, &rspDataLen );
      break;
#endif // HOST_CONFIG

    case HCI_EXT_UTIL_SUBGRP:
      stat = processExtMsgUTIL( (msg.opCode & 0x007F), &msg, &rspDataLen );
      break;

    default:
      stat = FAILURE;
      break;
  }

  // Deallocate here to free up heap space for the serial message set out HCI.
  VOID osal_msg_deallocate( (uint8 *)pMsg );
  deallocateIncoming = FALSE;

  // Send back an immediate response
  rspBuf[0] = LO_UINT16( HCI_EXT_GAP_CMD_STATUS_EVENT );
  rspBuf[1] = HI_UINT16( HCI_EXT_GAP_CMD_STATUS_EVENT );
  rspBuf[2] = stat;
  rspBuf[3] = LO_UINT16( 0xFC00 | msg.opCode );
  rspBuf[4] = HI_UINT16( 0xFC00 | msg.opCode );
  rspBuf[5] = rspDataLen;

  // IMPORTANT!! Fill in Payload (if needed) in case statement

  HCI_SendControllerToHostEvent( HCI_VE_EVENT_CODE, (6 + rspDataLen), rspBuf );

  return ( deallocateIncoming );
}

/*********************************************************************
 * @fn      processExtMsgUTIL
 *
 * @brief   Parse and process incoming HCI extension UTIL messages.
 *
 * @param   cmdID - incoming HCI extension command ID.
 * @param   pCmd - incoming HCI extension message.
 * @param   pRspDataLen - response data length to be returned.
 *
 * @return  SUCCESS, INVALIDPARAMETER and FAILURE.
 */
static uint8 processExtMsgUTIL( uint8 cmdID, hciExtCmd_t *pCmd, uint8 *pRspDataLen )
{
  bStatus_t stat = SUCCESS;

  *pRspDataLen = 0;

  switch( cmdID )
  {
  // Do not use SNV drivers with FPGA. Unverified behavior
#ifndef USE_FPGA
    case HCI_EXT_UTIL_NV_READ:
      {
        uint8 *pBuf = pCmd->pData;
        osalSnvId_t id  = pBuf[0];
        osalSnvLen_t len = pBuf[1];

        // This has a limitation of only allowing a max data length because of the fixed buffer.
        if ( (len < MAX_RSP_DATA_LEN) && (checkNVLen( id, len ) == SUCCESS) )
        {
          stat = osal_snv_read( id, len, &rspBuf[RSP_PAYLOAD_IDX] );
          if ( stat == SUCCESS )
          {
            *pRspDataLen = pBuf[1];
          }
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case HCI_EXT_UTIL_NV_WRITE:
      {
        uint8 *pBuf = pCmd->pData;
        osalSnvId_t id  = pBuf[0];
        osalSnvLen_t len = pBuf[1];
        if ( checkNVLen( id, len ) == SUCCESS )
        {
          stat = osal_snv_write( id, len, &pBuf[2] );
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;
#endif // USE_FPGA

    case HCI_EXT_UTIL_FORCE_BOOT:
      {
        extern void appForceBoot(void);
        appForceBoot();

        // Should never get here if SBL is present
        stat = INVALIDPARAMETER;
      }
      break;

    case HCI_EXT_UTIL_BUILD_REV:
      {
        ICall_BuildRevision buildRev;

        VOID buildRevision(&buildRev);

        // Stack revision
        //  Byte 0: Major
        //  Byte 1: Minor
        //  Byte 2: Patch
        rspBuf[RSP_PAYLOAD_IDX]   = BREAK_UINT32( buildRev.stackVersion, 0 );
        rspBuf[RSP_PAYLOAD_IDX+1] = BREAK_UINT32( buildRev.stackVersion, 1 );
        rspBuf[RSP_PAYLOAD_IDX+2] = BREAK_UINT32( buildRev.stackVersion, 2 );

        // Build revision
        rspBuf[RSP_PAYLOAD_IDX+3] = LO_UINT16( buildRev.buildVersion );
        rspBuf[RSP_PAYLOAD_IDX+4] = HI_UINT16( buildRev.buildVersion );

        // Stack info (Byte 5)
        rspBuf[RSP_PAYLOAD_IDX+5] = buildRev.stackInfo;

        // Controller info - part 1 (Byte 6)
        rspBuf[RSP_PAYLOAD_IDX+6] = LO_UINT16( buildRev.ctrlInfo );

        // Controller info - part 2 (Byte 7)
        rspBuf[RSP_PAYLOAD_IDX+7] = 0; // reserved

        // Host info - part 1 (Byte 8)
        rspBuf[RSP_PAYLOAD_IDX+8] = LO_UINT16( buildRev.hostInfo );

        // Host info - part 2 (Byte 9)
        rspBuf[RSP_PAYLOAD_IDX+9] = 0; // reserved

        *pRspDataLen = 10;
      }
      break;

    default:
      stat = FAILURE;
      break;
  }

  return ( stat );
}

#if defined(HOST_CONFIG)
/*********************************************************************
 * @fn      processExtMsgL2CAP
 *
 * @brief   Parse and process incoming HCI extension L2CAP messages.
 *
 * @param   pCmd - incoming HCI extension message.
 *
 * @return  SUCCESS or FAILURE
 */
static uint8 processExtMsgL2CAP( uint8 cmdID, hciExtCmd_t *pCmd, uint8 *pRspDataLen )
{
  uint8 *pBuf = pCmd->pData;
  uint16 connHandle = BUILD_UINT16( pBuf[0], pBuf[1] ); // connHandle, CID or PSM
  l2capSignalCmd_t cmd;
  bStatus_t stat;

  switch( cmdID )
  {
#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
    case HCI_EXT_L2CAP_DATA:
      if ( pCmd->len > 2 )
      {
        uint8 *pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
        if ( pPayload != NULL )
        {
          l2capPacket_t pkt;

          pkt.CID = connHandle; // connHandle is CID here
          pkt.pPayload = pPayload;
          pkt.len = pCmd->len-2;

          // Send SDU over dynamic channel
          stat = L2CAP_SendSDU( &pkt );
          if ( stat != SUCCESS )
          {
            VOID osal_mem_free( pPayload );
          }
        }
        else
        {
          stat = bleMemAllocError;
        }
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case HCI_EXT_L2CAP_REGISTER_PSM:
      if ( pCmd->len == 10 )
      {
        l2capPsm_t psm;

        psm.psm = connHandle; // connHandle is PSM here
        psm.mtu = BUILD_UINT16( pBuf[2], pBuf[3] );
        psm.initPeerCredits = BUILD_UINT16( pBuf[4], pBuf[5] );
        psm.peerCreditThreshold = BUILD_UINT16( pBuf[6], pBuf[7] );
        psm.maxNumChannels = pBuf[8];
        psm.pfnVerifySecCB = pBuf[9] ? l2capVerifySecCB : NULL;
        psm.taskId = bleDispatch_TaskID;

        stat = L2CAP_RegisterPsm( &psm );
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case HCI_EXT_L2CAP_DEREGISTER_PSM:
      if ( pCmd->len == 2 )
      {
        stat = L2CAP_DeregisterPsm( bleDispatch_TaskID, connHandle ); // connHandle is PSM here
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case HCI_EXT_L2CAP_PSM_INFO:
      if ( pCmd->len == 2 )
      {
        l2capPsmInfo_t info;

        stat = L2CAP_PsmInfo( connHandle, &info ); // connHandle is PSM here
        if ( stat == SUCCESS )
        {
          *pRspDataLen = 10;
          rspBuf[RSP_PAYLOAD_IDX]   = LO_UINT16( info.mtu );
          rspBuf[RSP_PAYLOAD_IDX+1] = HI_UINT16( info.mtu );

          rspBuf[RSP_PAYLOAD_IDX+2] = LO_UINT16( info.mps );
          rspBuf[RSP_PAYLOAD_IDX+3] = HI_UINT16( info.mps );

          rspBuf[RSP_PAYLOAD_IDX+4] = LO_UINT16( info.initPeerCredits );
          rspBuf[RSP_PAYLOAD_IDX+5] = HI_UINT16( info.initPeerCredits );

          rspBuf[RSP_PAYLOAD_IDX+6] = LO_UINT16( info.peerCreditThreshold );
          rspBuf[RSP_PAYLOAD_IDX+7] = HI_UINT16( info.peerCreditThreshold );

          rspBuf[RSP_PAYLOAD_IDX+8] = info.maxNumChannels;
          rspBuf[RSP_PAYLOAD_IDX+9] = info.numActiveChannels;
        }
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case HCI_EXT_L2CAP_PSM_CHANNELS:
      if ( pCmd->len == 2 )
      {
        l2capPsmInfo_t info;

        stat = L2CAP_PsmInfo( connHandle, &info ); // connHandle is PSM here
        if ( ( stat == SUCCESS ) && ( info.numActiveChannels > 0 ) )
        {
          uint8 numCIDs = info.numActiveChannels;

          uint16 *pCIDs = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * numCIDs );
          if ( pCIDs != NULL )
          {
            stat = L2CAP_PsmChannels( connHandle, numCIDs, pCIDs );
            if ( stat == SUCCESS )
            {
              uint8 *pRspBuf = &rspBuf[RSP_PAYLOAD_IDX];
              uint8 i;

              for ( i = 0; i < numCIDs; i++ )
              {
                *pRspBuf++ = LO_UINT16( pCIDs[i] );
                *pRspBuf++ = HI_UINT16( pCIDs[i] );
              }

              *pRspDataLen = numCIDs * 2;
            }

            VOID osal_mem_free( pCIDs );
          }
          else
          {
            stat = bleMemAllocError;
          }
        }
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case HCI_EXT_L2CAP_CHANNEL_INFO:
      if ( pCmd->len == 2 )
      {
        l2capChannelInfo_t channelInfo;

        stat = L2CAP_ChannelInfo( connHandle, &channelInfo ); // connHandle is CID here
        if ( stat == SUCCESS )
        {
          rspBuf[RSP_PAYLOAD_IDX] = channelInfo.state;

          *pRspDataLen = 1 + buildCoChannelInfo( connHandle, &channelInfo.info,
                                                 &rspBuf[RSP_PAYLOAD_IDX+1] );
        }
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case L2CAP_CONNECT_REQ:
      if ( pCmd->len == 6 )
      {
        uint16 psm = BUILD_UINT16( pBuf[2], pBuf[3] );
        uint16 peerPsm = BUILD_UINT16( pBuf[4], pBuf[5] );

        stat = L2CAP_ConnectReq( connHandle, psm, peerPsm );
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case L2CAP_CONNECT_RSP:
      if ( pCmd->len == 5 )
      {
        uint16 result = BUILD_UINT16( pBuf[3], pBuf[4] );

        stat = L2CAP_ConnectRsp( connHandle, pBuf[2], result );
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case L2CAP_DISCONNECT_REQ:
      if ( pCmd->len == 2 )
      {
        stat = L2CAP_DisconnectReq( connHandle ); // connHandle is CID here
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case L2CAP_FLOW_CTRL_CREDIT:
      stat = L2CAP_ParseFlowCtrlCredit( &cmd, pBuf, pCmd->len );
      if ( stat == SUCCESS )
      {
        stat = L2CAP_FlowCtrlCredit( cmd.credit.CID, cmd.credit.credits );
      }
      break;
#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)

    case L2CAP_PARAM_UPDATE_REQ:
      stat = L2CAP_ParseParamUpdateReq( &cmd, &pBuf[2], pCmd->len-2 );
      if ( stat == SUCCESS )
      {
        stat =  L2CAP_ConnParamUpdateReq( connHandle, &cmd.updateReq, bleDispatch_TaskID );
      }
      break;

    case L2CAP_INFO_REQ:
      stat = L2CAP_ParseInfoReq( &cmd, &pBuf[2], pCmd->len-2 );
      if ( stat == SUCCESS )
      {
        stat = L2CAP_InfoReq( connHandle, &cmd.infoReq, bleDispatch_TaskID );
      }
      break;

    default:
      stat = FAILURE;
      break;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      processExtMsgATT
 *
 * @brief   Parse and process incoming HCI extension ATT messages.
 *
 * @param   pCmd - incoming HCI extension message.
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE,
 *          bleInvalidPDU, bleInsufficientAuthen,
 *          bleInsufficientKeySize, bleInsufficientEncrypt or bleMemAllocError
 */
static uint8 processExtMsgATT( uint8 cmdID, hciExtCmd_t *pCmd )
{
  static uint8 numPrepareWrites = 0;
  static attPrepareWriteReq_t *pPrepareWrites = NULL;
  uint8 *pBuf, *pPayload = NULL, safeToDealloc = TRUE;
  uint16 connHandle;
  attMsg_t msg;
  bStatus_t stat = bleInvalidPDU;

  // Make sure received buffer contains at lease connection handle (2 otects)
  if ( pCmd->len < 2 )
  {
    return ( stat );
  }

  pBuf = pCmd->pData;
  connHandle = BUILD_UINT16( pBuf[0], pBuf[1] );

  switch( cmdID )
  {
    case ATT_ERROR_RSP:
      if ( ATT_ParseErrorRsp( &pBuf[2], pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = ATT_ErrorRsp( connHandle, &msg.errorRsp );
      }
      break;

    case ATT_EXCHANGE_MTU_REQ:
      if ( ATT_ParseExchangeMTUReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                    pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = GATT_ExchangeMTU( connHandle, &msg.exchangeMTUReq, bleDispatch_TaskID );
      }
      break;

    case ATT_EXCHANGE_MTU_RSP:
      if ( ATT_ParseExchangeMTURsp( &pBuf[2], pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = ATT_ExchangeMTURsp( connHandle, &msg.exchangeMTURsp );
      }
      break;

    case ATT_FIND_INFO_REQ:
      if ( ATT_ParseFindInfoReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                 pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = GATT_DiscAllCharDescs( connHandle, msg.findInfoReq.startHandle,
                                      msg.findInfoReq.endHandle, bleDispatch_TaskID );
      }
      break;

    case ATT_FIND_INFO_RSP:
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseFindInfoRsp( pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          stat = ATT_FindInfoRsp( connHandle, &msg.findInfoRsp );
          if ( stat == SUCCESS )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_FIND_BY_TYPE_VALUE_REQ:
      if ( ATT_ParseFindByTypeValueReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                        pCmd->len-2, &msg ) == SUCCESS )
      {
        attFindByTypeValueReq_t *pReq = &msg.findByTypeValueReq;

        // Find out what's been requested
        if ( gattPrimaryServiceType( pReq->type )     &&
             ( pReq->startHandle == GATT_MIN_HANDLE ) &&
             ( pReq->endHandle   == GATT_MAX_HANDLE ) )
        {
          // Discover primary service by service UUID
          stat = GATT_DiscPrimaryServiceByUUID( connHandle, pReq->pValue,
                                                pReq->len, bleDispatch_TaskID );
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case ATT_FIND_BY_TYPE_VALUE_RSP:
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseFindByTypeValueRsp( pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          stat = ATT_FindByTypeValueRsp( connHandle, &msg.findByTypeValueRsp );
          if ( stat == SUCCESS )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_BY_TYPE_REQ:
      if ( ATT_ParseReadByTypeReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                   pCmd->len-2, &msg ) == SUCCESS )
      {
        attReadByTypeReq_t *pReq = &msg.readByTypeReq;

        // Find out what's been requested
        if ( gattIncludeType( pReq->type ) )
        {
          // Find included services
          stat = GATT_FindIncludedServices( connHandle, pReq->startHandle,
                                            pReq->endHandle, bleDispatch_TaskID );
        }
        else if ( gattCharacterType( pReq->type ) )
        {
          // Discover all characteristics of a service
          stat = GATT_DiscAllChars( connHandle, pReq->startHandle,
                                    pReq->endHandle, bleDispatch_TaskID );
        }
        else
        {
          // Read using characteristic UUID
          stat = GATT_ReadUsingCharUUID( connHandle, pReq, bleDispatch_TaskID );
        }
      }
      break;

    case ATT_READ_BY_TYPE_RSP:
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseReadByTypeRsp( pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          stat = ATT_ReadByTypeRsp( connHandle, &msg.readByTypeRsp );
          if ( stat == SUCCESS )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_REQ:
      if ( ATT_ParseReadReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                             pCmd->len-2, &msg ) == SUCCESS )
      {
        // Read Characteristic Value or Read Characteristic Descriptor
        stat = GATT_ReadCharValue( connHandle, &msg.readReq, bleDispatch_TaskID );
      }
      break;

    case ATT_READ_RSP:
      // Create payload if attribute value is present
      if ( pCmd->len > 2 )
      {
        pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      }

      if ( ( pCmd->len == 2 ) || ( pPayload != NULL ) )
      {
        if ( ATT_ParseReadRsp( pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          stat = ATT_ReadRsp( connHandle, &msg.readRsp );
          if  ( stat == SUCCESS )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_BLOB_REQ:
      if ( ATT_ParseReadBlobReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                 pCmd->len-2, &msg ) == SUCCESS )
      {
        // Read long characteristic value
        stat = GATT_ReadLongCharValue( connHandle, &msg.readBlobReq, bleDispatch_TaskID );
      }
      break;

    case ATT_READ_BLOB_RSP:
      // Create payload if attribute value is present
      if ( pCmd->len > 2 )
      {
        pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      }

      if ( ( pCmd->len == 2 ) || ( pPayload != NULL ) )
      {
        if ( ATT_ParseReadBlobRsp( pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          stat = ATT_ReadBlobRsp( connHandle, &msg.readBlobRsp );
          if ( stat == SUCCESS )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_MULTI_REQ:
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseReadMultiReq( ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                    pCmd->len-2, &msg ) == SUCCESS )
        {
          stat = GATT_ReadMultiCharValues( connHandle, &msg.readMultiReq, bleDispatch_TaskID );
          if ( stat == SUCCESS )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_MULTI_RSP:
      // Create payload if attribute value is present
      if ( pCmd->len > 2 )
      {
        pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      }

      if ( ( pCmd->len == 2 ) || ( pPayload != NULL ) )
      {
        if ( ATT_ParseReadMultiRsp( pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          stat = ATT_ReadMultiRsp( connHandle, &msg.readMultiRsp );
          if ( stat == SUCCESS )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_BY_GRP_TYPE_REQ:
      if ( ATT_ParseReadByTypeReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                   pCmd->len-2, &msg ) == SUCCESS )
      {
        attReadByGrpTypeReq_t *pReq = &msg.readByGrpTypeReq;

        // Find out what's been requested
        if ( gattPrimaryServiceType( pReq->type )     &&
             ( pReq->startHandle == GATT_MIN_HANDLE ) &&
             ( pReq->endHandle   == GATT_MAX_HANDLE ) )
        {
          // Discover all primary services
          stat = GATT_DiscAllPrimaryServices( connHandle, bleDispatch_TaskID );
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case ATT_READ_BY_GRP_TYPE_RSP:
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseReadByGrpTypeRsp( pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          stat = ATT_ReadByGrpTypeRsp( connHandle, &msg.readByGrpTypeRsp );
          if ( stat == SUCCESS )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_WRITE_REQ:
      pPayload = createSignedMsgPayload( pBuf[2], pBuf[3], &pBuf[4], pCmd->len-4 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseWriteReq( pBuf[2], pBuf[3], pPayload,
                                pCmd->len-4, &msg ) == SUCCESS )
        {
          attWriteReq_t *pReq = &msg.writeReq;

          if ( pReq->cmd == FALSE )
          {
            // Write Characteristic Value or Write Characteristic Descriptor
            stat = GATT_WriteCharValue( connHandle, pReq, bleDispatch_TaskID );
          }
          else
          {
            if ( pReq->sig == FALSE )
            {
              // Write Without Response
              stat = GATT_WriteNoRsp( connHandle, pReq );
            }
            else
            {
              // Signed Write Without Response
              stat = GATT_SignedWriteNoRsp( connHandle, pReq );
            }
          }

          if ( ( stat == SUCCESS ) && ( pReq->pValue != NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_WRITE_RSP:
      if ( ATT_ParseWriteRsp( &pBuf[2], pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = ATT_WriteRsp( connHandle );
      }
      break;

    case ATT_PREPARE_WRITE_REQ:
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParsePrepareWriteReq( ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                       pCmd->len-2, &msg ) == SUCCESS )
        {
          attPrepareWriteReq_t *pReq = &msg.prepareWriteReq;

#if !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES)
          if ( GATTServApp_GetParamValue() == GATT_TESTMODE_PREPARE_WRITE )
          {
            // Send the Prepare Write Request right away - needed for GATT testing
            stat = GATT_PrepareWriteReq( connHandle, pReq, bleDispatch_TaskID );
            if ( ( stat == SUCCESS ) && ( pReq->pValue != NULL ) )
            {
              safeToDealloc = FALSE; // payload passed to GATT
            }
          }
          else
#endif // !GATT_DB_OFF_CHIP && TESTMODE
          {
            // GATT Reliable Writes
            if ( pPrepareWrites == NULL )
            {
              // First allocated buffer for the Prepare Write Requests
              pPrepareWrites = osal_mem_alloc( GATT_MAX_NUM_RELIABLE_WRITES * sizeof( attPrepareWriteReq_t ) );
            }

            if ( pPrepareWrites != NULL )
            {
              if ( numPrepareWrites < GATT_MAX_NUM_RELIABLE_WRITES )
              {
                // Save the Prepare Write Request for now
                VOID osal_memcpy( &(pPrepareWrites[numPrepareWrites++]),
                                  pReq, sizeof( attPrepareWriteReq_t ) );

                // Request was parsed and saved successfully
                stat = SUCCESS;
                safeToDealloc = FALSE; // payload saved for later
              }
              else
              {
                stat = INVALIDPARAMETER;
              }
            }
            else
            {
              stat = bleMemAllocError;
            }
          }
        }
      }
      break;

    case ATT_PREPARE_WRITE_RSP:
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParsePrepareWriteRsp( pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          attPrepareWriteRsp_t *pRsp = &msg.prepareWriteRsp;

          stat = ATT_PrepareWriteRsp( connHandle, pRsp );
          if ( ( stat == SUCCESS ) && ( pRsp->pValue != NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_EXECUTE_WRITE_REQ:
      if ( ATT_ParseExecuteWriteReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                     pCmd->len-2, &msg ) == SUCCESS )
      {
#if !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES)
        if ( GATTServApp_GetParamValue() == GATT_TESTMODE_PREPARE_WRITE )
        {
          // Send the Execute Write Request right away - needed for GATT testing
          stat = GATT_ExecuteWriteReq( connHandle, &msg.executeWriteReq, bleDispatch_TaskID );
        }
        else
#endif // !GATT_DB_OFF_CHIP && TESTMODE
        if ( pPrepareWrites != NULL )
        {
          // GATT Reliable Writes - send all saved Prepare Write Requests
          stat = GATT_ReliableWrites( connHandle, pPrepareWrites, numPrepareWrites,
                                      msg.executeWriteReq.flags, bleDispatch_TaskID );
          if ( stat != SUCCESS )
          {
        	uint8 i;

            // Free saved payload buffers
            for ( i = 0; i < numPrepareWrites; i++ )
            {
              if ( pPrepareWrites[i].pValue != NULL )
              {
                osal_bm_free( pPrepareWrites[i].pValue );
              }
            }

            osal_mem_free( pPrepareWrites );
          }
          // else pPrepareWrites will be freed by GATT Client

          // Reset GATT Reliable Writes variables
          pPrepareWrites = NULL;
          numPrepareWrites = 0;
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case ATT_EXECUTE_WRITE_RSP:
      if ( ATT_ParseExecuteWriteRsp( &pBuf[2], pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = ATT_ExecuteWriteRsp( connHandle );
      }
      break;

    case ATT_HANDLE_VALUE_NOTI:
      pPayload = createMsgPayload( &pBuf[3], pCmd->len-3 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseHandleValueInd( ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                      pCmd->len-3, &msg ) == SUCCESS )
        {
          attHandleValueNoti_t *pNoti = &msg.handleValueNoti;

          stat = GATT_Notification( connHandle, pNoti, pBuf[2] );
          if ( ( stat == SUCCESS ) && ( pNoti->pValue!= NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_HANDLE_VALUE_IND:
      pPayload = createMsgPayload( &pBuf[3], pCmd->len-3 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseHandleValueInd( ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                      pCmd->len-3, &msg ) == SUCCESS )
        {
          attHandleValueInd_t *pInd = &msg.handleValueInd;

          stat = GATT_Indication( connHandle, pInd, pBuf[2], bleDispatch_TaskID );
          if ( ( stat == SUCCESS ) && ( pInd->pValue != NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_HANDLE_VALUE_CFM:
      if ( ATT_ParseHandleValueCfm( &pBuf[2], pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = ATT_HandleValueCfm( connHandle );
      }
      break;

    default:
      stat = FAILURE;
      break;
  }

  if ( ( pPayload != NULL ) && safeToDealloc )
  {
    // Free payload
    osal_bm_free( pPayload );
  }

  return ( mapATT2BLEStatus( stat ) );
}

/*********************************************************************
 * @fn      processExtMsgGATT
 *
 * @brief   Parse and process incoming HCI extension GATT messages.
 *
 * @param   cmdID - incoming HCI extension command ID.
 * @param   pCmd - incoming HCI extension message.
 * @param   pRspDataLen - response data length to be returned.
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE,
 *          bleInvalidPDU or bleMemAllocError
 */
static uint8 processExtMsgGATT( uint8 cmdID, hciExtCmd_t *pCmd, uint8 *pRspDataLen )
{
#if defined(GATT_DB_OFF_CHIP)
  static uint16 totalAttrs = 0;
  static gattService_t service = { 0, NULL };
#endif // GATT_DB_OFF_CHIP
  uint8 *pBuf, *pPayload = NULL, safeToDealloc = TRUE;
  uint16 connHandle;
  attMsg_t msg;
  bStatus_t stat = bleInvalidPDU;

  // Make sure received buffer is at lease 2-otect long
  if ( pCmd->len < 2 )
  {
    return ( stat );
  }

  pBuf = pCmd->pData;
  connHandle = BUILD_UINT16( pBuf[0], pBuf[1] );

  switch( cmdID )
  {
    case ATT_EXCHANGE_MTU_REQ: // GATT Exchange MTU
      if ( ATT_ParseExchangeMTUReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                    pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = GATT_ExchangeMTU( connHandle, &msg.exchangeMTUReq, bleDispatch_TaskID );
      }
      break;

    case ATT_READ_BY_GRP_TYPE_REQ: // GATT Discover All Primary Services
      stat = GATT_DiscAllPrimaryServices( connHandle, bleDispatch_TaskID );
      break;

    case ATT_FIND_BY_TYPE_VALUE_REQ: // GATT Discover Primary Service By UUID
      stat = GATT_DiscPrimaryServiceByUUID( connHandle, &pBuf[2],
                                            pCmd->len-2, bleDispatch_TaskID );
      break;

    case GATT_FIND_INCLUDED_SERVICES: // GATT Find Included Services
    case GATT_DISC_ALL_CHARS: // GATT Discover All Characteristics
      if ( ( pCmd->len-2 ) == ATT_READ_BY_TYPE_REQ_FIXED_SIZE )
      {
        // First requested handle number
        uint16 startHandle = BUILD_UINT16( pBuf[2], pBuf[3] );

        // Last requested handle number
        uint16 endHandle = BUILD_UINT16( pBuf[4], pBuf[5] );

        if ( cmdID == GATT_FIND_INCLUDED_SERVICES )
        {
          stat = GATT_FindIncludedServices( connHandle, startHandle,
                                            endHandle, bleDispatch_TaskID );
        }
        else
        {
          stat = GATT_DiscAllChars( connHandle, startHandle,
                                    endHandle, bleDispatch_TaskID );
        }
      }
      break;

    case ATT_READ_BY_TYPE_REQ: // GATT Discover Characteristics by UUID
      if ( ATT_ParseReadByTypeReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                   pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = GATT_DiscCharsByUUID( connHandle, &msg.readByTypeReq, bleDispatch_TaskID );
      }
      break;

    case ATT_FIND_INFO_REQ: // GATT Discover All Characteristic Descriptors
      if ( ATT_ParseFindInfoReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                 pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = GATT_DiscAllCharDescs( connHandle, msg.findInfoReq.startHandle,
                                      msg.findInfoReq.endHandle, bleDispatch_TaskID );
      }
      break;

    case ATT_READ_REQ: // GATT Read Characteristic Value
      if ( ATT_ParseReadReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                             pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = GATT_ReadCharValue( connHandle, &msg.readReq, bleDispatch_TaskID );
      }
      break;

    case GATT_READ_USING_CHAR_UUID: // GATT Read Using Characteristic UUID
      if ( ATT_ParseReadByTypeReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                   pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = GATT_ReadUsingCharUUID( connHandle, &msg.readByTypeReq, bleDispatch_TaskID );
      }
      break;

    case ATT_READ_BLOB_REQ: // GATT Read Long Characteristic Value
      if ( ATT_ParseReadBlobReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                 pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = GATT_ReadLongCharValue( connHandle, &msg.readBlobReq, bleDispatch_TaskID );
      }
      break;

    case ATT_READ_MULTI_REQ: // GATT Read Multiple Characteristic Values
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseReadMultiReq( ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                    pCmd->len-2, &msg ) == SUCCESS )
        {
          stat = GATT_ReadMultiCharValues( connHandle, &msg.readMultiReq, bleDispatch_TaskID );
          if ( stat == SUCCESS )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case GATT_WRITE_NO_RSP: // GATT Write Without Response
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseWriteReq( FALSE, TRUE, pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          attWriteReq_t *pReq = &msg.writeReq;

          stat = GATT_WriteNoRsp( connHandle, pReq );
          if ( ( stat == SUCCESS ) && ( pReq->pValue != NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case GATT_SIGNED_WRITE_NO_RSP: // GATT Signed Write Without Response
      pPayload = createSignedMsgPayload( TRUE, TRUE, &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseWriteReq( TRUE, TRUE, pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          attWriteReq_t *pReq = &msg.writeReq;

          stat = GATT_SignedWriteNoRsp( connHandle, pReq );
          if ( ( stat == SUCCESS ) && ( pReq->pValue != NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_WRITE_REQ: // GATT Write Characteristic Value
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseWriteReq( FALSE, FALSE, pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          attWriteReq_t *pReq = &msg.writeReq;

          stat = GATT_WriteCharValue( connHandle, pReq, bleDispatch_TaskID );
          if ( ( stat == SUCCESS ) && ( pReq->pValue != NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_PREPARE_WRITE_REQ: // GATT Write Long Characteristic Value
    case GATT_WRITE_LONG_CHAR_DESC: // GATT Write Long Characteristic Descriptor
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParsePrepareWriteReq( ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                       pCmd->len-2, &msg ) == SUCCESS )
        {
          attPrepareWriteReq_t *pReq = &msg.prepareWriteReq;
          if ( cmdID == ATT_PREPARE_WRITE_REQ )
          {
            stat = GATT_WriteLongCharValue( connHandle, pReq, bleDispatch_TaskID );
          }
          else
          {
            stat = GATT_WriteLongCharDesc( connHandle, pReq, bleDispatch_TaskID );
          }

          if ( ( stat == SUCCESS ) && ( pReq->pValue != NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case GATT_RELIABLE_WRITES: // GATT Reliable Writes
      if ( pCmd->len-2 > 0 )
      {
        uint8 numReqs = pBuf[2];

        if ( ( numReqs > 0 ) && ( numReqs <= GATT_MAX_NUM_RELIABLE_WRITES ) )
        {
          // First allocated buffer for the Prepare Write Requests
          attPrepareWriteReq_t *pReqs = osal_mem_alloc( numReqs * sizeof( attPrepareWriteReq_t ) );
          if ( pReqs != NULL )
          {
            uint8 i;

            VOID osal_memset( pReqs, 0, numReqs * sizeof( attPrepareWriteReq_t ) );

            pBuf += 3; // pass connHandle and numReqs

            // Create payload buffer for each Prepare Write Request
            for ( i = 0; i < numReqs; i++ )
            {
              // length of request is length of attribute value plus fixed fields.
              // request format: length (1) + handle (2) + offset (2) + attribute value
              uint8 reqLen = ATT_PREPARE_WRITE_REQ_FIXED_SIZE;

              if ( *pBuf > 0 )
              {
                reqLen += *pBuf++;
                pPayload = createMsgPayload( pBuf, reqLen );
                if ( pPayload == NULL )
                {
                  stat = bleMemAllocError;
                  break;
                }

                VOID ATT_ParsePrepareWriteReq( ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                               reqLen, (attMsg_t *)&(pReqs[i]) );
              }
              else // no attribute value
              {
                pBuf++;
                VOID ATT_ParsePrepareWriteReq( ATT_SIG_NOT_INCLUDED, FALSE, pBuf,
                                               reqLen, (attMsg_t *)&(pReqs[i]) );
              }

              // Next request
              pBuf += reqLen;
            }

            // See if all requests were parsed successfully
            if ( i == numReqs )
            {
              // Send all saved Prepare Write Requests
              stat = GATT_ReliableWrites( connHandle, pReqs, numReqs,
                                          ATT_WRITE_PREPARED_VALUES, bleDispatch_TaskID );
            }

            if ( stat != SUCCESS )
            {
              // Free payload buffers first
              for ( i = 0; i < numReqs; i++ )
              {
                if ( pReqs[i].pValue != NULL )
                {
                  osal_bm_free( pReqs[i].pValue );
                }
              }

              osal_mem_free( pReqs );
            }
            // else pReqs will be freed by GATT Client
          }
          else
          {
            stat = bleMemAllocError;
          }
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case GATT_READ_CHAR_DESC: // GATT Read Characteristic Descriptor
      if ( ATT_ParseReadReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                             pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = GATT_ReadCharDesc( connHandle, &msg.readReq, bleDispatch_TaskID );
      }
      break;

    case GATT_READ_LONG_CHAR_DESC: // GATT Read Long Characteristic Descriptor
      if ( ATT_ParseReadBlobReq( ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                 pCmd->len-2, &msg ) == SUCCESS )
      {
        stat = GATT_ReadLongCharDesc( connHandle, &msg.readBlobReq, bleDispatch_TaskID );
      }
      break;

    case GATT_WRITE_CHAR_DESC: // GATT Write Characteristic Descriptor
      pPayload = createMsgPayload( &pBuf[2], pCmd->len-2 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseWriteReq( FALSE, FALSE, pPayload, pCmd->len-2, &msg ) == SUCCESS )
        {
          attWriteReq_t *pReq = &msg.writeReq;

          stat = GATT_WriteCharDesc( connHandle, pReq, bleDispatch_TaskID );
          if ( ( stat == SUCCESS ) && ( pReq->pValue != NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_HANDLE_VALUE_NOTI:
      pPayload = createMsgPayload( &pBuf[3], pCmd->len-3 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseHandleValueInd( ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                      pCmd->len-3, &msg ) == SUCCESS )
        {
          attHandleValueNoti_t *pNoti = &msg.handleValueNoti;

          stat = GATT_Notification( connHandle, pNoti, pBuf[2] );
          if ( ( stat == SUCCESS ) && ( pNoti->pValue != NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_HANDLE_VALUE_IND:
      pPayload = createMsgPayload( &pBuf[3], pCmd->len-3 );
      if ( pPayload != NULL )
      {
        if ( ATT_ParseHandleValueInd( ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                      pCmd->len-3, &msg ) == SUCCESS )
        {
          attHandleValueInd_t *pInd = &msg.handleValueInd;

          stat = GATT_Indication( connHandle, pInd, pBuf[2], bleDispatch_TaskID );
          if ( ( stat == SUCCESS ) && ( pInd->pValue != NULL ) )
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

#if defined(GATT_DB_OFF_CHIP) // These GATT commands don't include connHandle field
    case HCI_EXT_GATT_ADD_SERVICE:
      if ( service.attrs == NULL )
      {
        // Service type must be 2 octets (Primary or Secondary)
        if ( pCmd->len-3 == ATT_BT_UUID_SIZE )
        {
          uint16 uuid = BUILD_UINT16( pBuf[0], pBuf[1] );
          uint16 numAttrs = BUILD_UINT16( pBuf[2], pBuf[3] );

          if ( ( ( uuid == GATT_PRIMARY_SERVICE_UUID )     ||
                 ( uuid == GATT_SECONDARY_SERVICE_UUID ) ) &&
               ( numAttrs > 0 ) )
          {
            uint8 encKeySize = pBuf[4];

            if ( ( encKeySize >= GATT_MIN_ENCRYPT_KEY_SIZE ) &&
                 ( encKeySize <= GATT_MAX_ENCRYPT_KEY_SIZE ) )
            {
              // Allocate buffer for the attribute table
              service.attrs = osal_mem_alloc( numAttrs * sizeof( gattAttribute_t ) );
              if ( service.attrs != NULL )
              {
                // Zero out all attribute fields
                VOID osal_memset( service.attrs, 0, numAttrs * sizeof( gattAttribute_t ) );

                totalAttrs = numAttrs;
                service.encKeySize = encKeySize;

                // Set up service record
                stat = addAttrRec( &service, pBuf, ATT_BT_UUID_SIZE,
                                   GATT_PERMIT_READ, &totalAttrs, pRspDataLen );
              }
              else
              {
                stat = bleMemAllocError;
              }
            }
            else
            {
              stat = bleInvalidRange;
            }
          }
          else
          {
            stat = INVALIDPARAMETER;
          }
        }
      }
      else
      {
        stat = blePending;
      }
      break;

    case HCI_EXT_GATT_DEL_SERVICE:
      {
        uint16 handle = BUILD_UINT16( pBuf[0], pBuf[1] );

        if ( handle == 0x0000 )
        {
          // Service is not registered with GATT yet
          freeAttrRecs( &service );

          totalAttrs = 0;
        }
        else
        {
          gattService_t serv;

          // Service is already registered with the GATT Server
          stat = GATT_DeregisterService( handle, &serv );
          if ( stat == SUCCESS )
          {
            freeAttrRecs( &serv );
          }
        }

        stat = SUCCESS;
      }
      break;

    case HCI_EXT_GATT_ADD_ATTRIBUTE:
      if ( service.attrs != NULL )
      {
        if ( ( pCmd->len-1 == ATT_UUID_SIZE ) ||
             ( pCmd->len-1 == ATT_BT_UUID_SIZE ) )
        {
          // Add attribute record to the service being added
          stat = addAttrRec( &service, pBuf, pCmd->len-1,
                             pBuf[pCmd->len-1], &totalAttrs, pRspDataLen );
        }
      }
      else // no corresponding service
      {
        stat = INVALIDPARAMETER;
      }
      break;
#endif // GATT_DB_OFF_CHIP

    default:
      stat = FAILURE;
      break;
  }

  if ( ( pPayload != NULL ) && safeToDealloc )
  {
    // Free payload
    osal_bm_free( pPayload );
  }

  return ( mapATT2BLEStatus( stat ) );
}

/*********************************************************************
 * @fn      processExtMsgGAP
 *
 * @brief   Parse and process incoming HCI extension GAP messages.
 *
 * @param   cmdID - incoming HCI extension command ID.
 * @param   pCmd - incoming HCI extension message.
 * @param   pRspDataLen - response data length to be returned.
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE,
 *          or bleMemAllocError
 */
static uint8 processExtMsgGAP( uint8 cmdID, hciExtCmd_t *pCmd, uint8 *pRspDataLen )
{
  uint8 *pBuf = pCmd->pData;
  bStatus_t stat = SUCCESS;

  switch( cmdID )
  {
    case HCI_EXT_GAP_DEVICE_INIT:
      {
        uint8 profileRole = pBuf[0];
        stat = GAP_DeviceInit( profileRole, bleDispatch_TaskID,
                               pBuf[1], &pBuf[2]);

        // Take over the processing of Authentication messages
        VOID GAP_SetParamValue( GAP_PARAM_AUTH_TASK_ID, bleDispatch_TaskID );
      }
      break;

#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
    case HCI_EXT_GAP_TERMINATE_LINK:
      stat = GAP_TerminateLinkReq( BUILD_UINT16( pBuf[0], pBuf[1] ), pBuf[2] );
      break;

    case HCI_EXT_GAP_UPDATE_LINK_PARAM_REQ:
      {
        gapUpdateLinkParamReq_t updateLinkReq;

        updateLinkReq.connectionHandle = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        updateLinkReq.intervalMin = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        updateLinkReq.intervalMax = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        updateLinkReq.connLatency = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        updateLinkReq.connTimeout = BUILD_UINT16( pBuf[0], pBuf[1] );

        stat = GAP_UpdateLinkParamReq( &updateLinkReq );
      }
      break;

    case HCI_EXT_GAP_UPDATE_LINK_PARAM_REQ_REPLY:
      {
        gapUpdateLinkParamReqReply_t updateLinkReply;

        updateLinkReply.connectionHandle = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        updateLinkReply.intervalMin = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        updateLinkReply.intervalMax = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        updateLinkReply.connLatency = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        updateLinkReply.connTimeout = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        updateLinkReply.signalIdentifier = *pBuf++
        updateLinkReply.accepted    = *pBuf;

        stat = GAP_UpdateLinkParamReqReply(&updateLinkReply);
      }
      break;

    case HCI_EXT_GAP_AUTHENTICATE:
      {
        uint8 tmp;
        gapAuthParams_t params;
        gapPairingReq_t pairReq;
        gapPairingReq_t *pPairReq = NULL;

#if ( OSALMEM_METRICS )
        uint16 memUsed = osal_heap_mem_used();
#endif

        VOID osal_memset( &params, 0, sizeof ( gapAuthParams_t ) );

        params.connectionHandle = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;

        params.secReqs.ioCaps = *pBuf++;
        params.secReqs.oobAvailable = *pBuf++;
        VOID osal_memcpy( params.secReqs.oob, pBuf, KEYLEN );
        pBuf += KEYLEN;

        VOID osal_memcpy( params.secReqs.oobConfirm, pBuf, KEYLEN);
        pBuf += KEYLEN;
        params.secReqs.localOobAvailable = *pBuf++;
        VOID osal_memcpy( params.secReqs.localOob, pBuf, KEYLEN);
        pBuf += KEYLEN;
        params.secReqs.isSCOnlyMode = *pBuf++;
        params.secReqs.eccKeys.isUsed = *pBuf++;
        VOID osal_memcpy( params.secReqs.eccKeys.sK, pBuf, SM_ECC_KEY_LEN);
        pBuf += SM_ECC_KEY_LEN;
        VOID osal_memcpy( params.secReqs.eccKeys.pK_x, pBuf, SM_ECC_KEY_LEN);
        pBuf += SM_ECC_KEY_LEN;
        VOID osal_memcpy( params.secReqs.eccKeys.pK_y, pBuf, SM_ECC_KEY_LEN);
        pBuf += SM_ECC_KEY_LEN;

        params.secReqs.authReq = *pBuf++;
        params.secReqs.maxEncKeySize = *pBuf++;

        tmp = *pBuf++; // key distribution
        params.secReqs.keyDist.sEncKey = ( tmp & KEYDIST_SENC ) ? TRUE : FALSE;
        params.secReqs.keyDist.sIdKey = ( tmp & KEYDIST_SID ) ? TRUE : FALSE;
        params.secReqs.keyDist.sSign = ( tmp & KEYDIST_SSIGN ) ? TRUE : FALSE;
        params.secReqs.keyDist.sLinkKey = ( tmp & KEYDIST_SLINK ) ? TRUE : FALSE;
        params.secReqs.keyDist.sReserved = 0;
        params.secReqs.keyDist.mEncKey = ( tmp & KEYDIST_MENC ) ? TRUE : FALSE;
        params.secReqs.keyDist.mIdKey = ( tmp & KEYDIST_MID ) ? TRUE : FALSE;
        params.secReqs.keyDist.mSign = ( tmp & KEYDIST_MSIGN ) ? TRUE : FALSE;
        params.secReqs.keyDist.mLinkKey = ( tmp & KEYDIST_MLINK ) ? TRUE : FALSE;
        params.secReqs.keyDist.mReserved = 0;

        tmp = *pBuf++; // pairing request enable
        if ( tmp )
        {
          pairReq.ioCap = *pBuf++;
          pairReq.oobDataFlag = *pBuf++;
          pairReq.authReq = *pBuf++;
          pairReq.maxEncKeySize = *pBuf++;
          tmp = *pBuf++;
          pairReq.keyDist.sEncKey = ( tmp & KEYDIST_SENC ) ? TRUE : FALSE;
          pairReq.keyDist.sIdKey = ( tmp & KEYDIST_SID ) ? TRUE : FALSE;
          pairReq.keyDist.sSign = ( tmp & KEYDIST_SSIGN ) ? TRUE : FALSE;
          pairReq.keyDist.sLinkKey = ( tmp & KEYDIST_SLINK ) ? TRUE : FALSE;
          pairReq.keyDist.sReserved = 0;
          pairReq.keyDist.mEncKey = ( tmp & KEYDIST_MENC ) ? TRUE : FALSE;
          pairReq.keyDist.mIdKey = ( tmp & KEYDIST_MID ) ? TRUE : FALSE;
          pairReq.keyDist.mSign = ( tmp & KEYDIST_MSIGN ) ? TRUE : FALSE;
          pairReq.keyDist.mLinkKey = ( tmp & KEYDIST_MLINK ) ? TRUE : FALSE;
          pairReq.keyDist.mReserved = 0;
          pPairReq = &pairReq;
        }

        stat = GAP_Authenticate( &params, pPairReq );

#if ( OSALMEM_METRICS )
        *pRspDataLen = 2;
        rspBuf[RSP_PAYLOAD_IDX] = LO_UINT16( memUsed );
        rspBuf[RSP_PAYLOAD_IDX+1] = HI_UINT16( memUsed );
#endif
      }
      break;

    case HCI_EXT_GAP_TERMINATE_AUTH:
      stat = GAP_TerminateAuth( BUILD_UINT16( pBuf[0], pBuf[1] ), pBuf[2] );
      break;

    case HCI_EXT_GAP_BOND:
      {
        uint16 connectionHandle;
        smSecurityInfo_t securityInfo;
        uint8 authenticated;
        uint8 secureConnections;

        // Do Security Information part
        connectionHandle = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        authenticated = *pBuf++;
        secureConnections = *pBuf++;
        VOID osal_memcpy( securityInfo.ltk, pBuf, KEYLEN );
        pBuf += KEYLEN;
        securityInfo.div = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        VOID osal_memcpy( securityInfo.rand, pBuf, B_RANDOM_NUM_SIZE );
        pBuf += B_RANDOM_NUM_SIZE;
        securityInfo.keySize = *pBuf++;

        stat = GAP_Bond( connectionHandle, authenticated, secureConnections,
                         &securityInfo, TRUE );
      }
      break;

    case HCI_EXT_GAP_SIGNABLE:
      {
        uint16 connectionHandle;
        uint8 authenticated;
        smSigningInfo_t signing;

        connectionHandle = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;

        authenticated = *pBuf++;

        VOID osal_memcpy( signing.srk, pBuf, KEYLEN );
        pBuf += KEYLEN;

        signing.signCounter = BUILD_UINT32( pBuf[0], pBuf[1], pBuf[2], pBuf[3] );

        stat = GAP_Signable( connectionHandle, authenticated, &signing );
      }
      break;

    case HCI_EXT_GAP_PASSKEY_UPDATE:
      stat = GAP_PasskeyUpdate( &pBuf[2], BUILD_UINT16( pBuf[0], pBuf[1] ) );
      break;

#endif // CENTRAL_CFG | PERIPHERAL_CFG

    case HCI_EXT_GAP_SET_PARAM:
      {
        uint16 id = (uint16)pBuf[0];
        uint16 value = BUILD_UINT16( pBuf[1], pBuf[2] );

        if ( ( id != GAP_PARAM_AUTH_TASK_ID ) && ( id < GAP_PARAMID_MAX ) )
        {
          stat = GAP_SetParamValue( id, value );
        }
#if !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES)
        else if ( id == GAP_PARAM_GATT_TESTCODE )
        {
          GATTServApp_SetParamValue( value );
        }
        else if ( id == GAP_PARAM_ATT_TESTCODE )
        {
          ATT_SetParamValue( value );
        }
        else if ( id == TGAP_GGS_TESTCODE )
        {
          GGS_SetParamValue( value );
        }
        else if ( id == GAP_PARAM_L2CAP_TESTCODE )
        {
          L2CAP_SetParamValue( value );
        }
#endif // !GATT_DB_OFF_CHIP && TESTMODES
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case HCI_EXT_GAP_GET_PARAM:
      {
        uint16 paramValue = 0xFFFF;
        uint16 param = (uint16)pBuf[0];

        if ( param < 0x00FF )
        {
          if ( ( param != GAP_PARAM_AUTH_TASK_ID ) && ( param < GAP_PARAMID_MAX ) )
          {
            paramValue = GAP_GetParamValue( param );
          }
#if !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES)
          else if ( param == GAP_PARAM_GATT_TESTCODE )
          {
            paramValue = GATTServApp_GetParamValue();
          }
          else if ( param == GAP_PARAM_ATT_TESTCODE )
          {
            paramValue = ATT_GetParamValue();
          }
          else if ( param == TGAP_GGS_TESTCODE )
          {
            paramValue = GGS_GetParamValue();
          }
          else if ( param == GAP_PARAM_L2CAP_TESTCODE )
          {
            paramValue = L2CAP_GetParamValue();
          }
#endif // !GATT_DB_OFF_CHIP && TESTMODES
        }
#if ( OSALMEM_METRICS )
        else
        {
          paramValue = osal_heap_mem_used();
        }
#endif
        if ( paramValue != 0xFFFF )
        {
          stat = SUCCESS;
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
        *pRspDataLen = 2;
        rspBuf[RSP_PAYLOAD_IDX] = LO_UINT16( paramValue );
        rspBuf[RSP_PAYLOAD_IDX+1] = HI_UINT16( paramValue );
      }
      break;

#if ( HOST_CONFIG & PERIPHERAL_CFG )
    case HCI_EXT_GAP_SLAVE_SECURITY_REQ_UPDATE:
      stat = GAP_SendSlaveSecurityRequest( BUILD_UINT16( pBuf[0], pBuf[1] ), pBuf[2] );
      break;
#endif // PERIPHERAL_CFG

    case HCI_EXT_GAP_BOND_SET_PARAM:
      {
#if defined(GAP_BOND_MGR)
        uint16 id = BUILD_UINT16( pBuf[0], pBuf[1] );
        uint8 *pValue;
        uint32 passcode;

        switch ( id )
        {
          case GAPBOND_DEFAULT_PASSCODE:
            // First build passcode
            passcode = osal_build_uint32( &pBuf[3], pBuf[2] );
            pValue = (uint8 *)&passcode;
            break;

          default:
            pValue = &pBuf[3];
            break;
        }

        if ( stat == SUCCESS )
        {
          stat = GAPBondMgr_SetParameter( id, pBuf[2],  pValue );
        }
#else
        stat = INVALIDPARAMETER;
#endif
      }
      break;

    case HCI_EXT_GAP_BOND_GET_PARAM:
      {
        uint8 len = 0;
#if defined(GAP_BOND_MGR)
        uint16 id = BUILD_UINT16( pBuf[0], pBuf[1] );

        stat = GAPBondMgr_GetParameter( id, &rspBuf[RSP_PAYLOAD_IDX] );

        switch ( id )
        {
          case GAPBOND_INITIATE_WAIT:
            len = 2;
            break;

          case GAPBOND_OOB_DATA:
            len = KEYLEN;
            break;

          case GAPBOND_DEFAULT_PASSCODE:
            len = 4;
            break;

          default:
            len = 1;
            break;
        }
#else
        stat = INVALIDPARAMETER;
#endif
        *pRspDataLen = len;
      }
      break;

#if !defined(GATT_NO_SERVICE_CHANGED)
    case HCI_EXT_GAP_BOND_SERVICE_CHANGE:
      {
#if defined(GAP_BOND_MGR)
        stat = GAPBondMgr_ServiceChangeInd( BUILD_UINT16( pBuf[0], pBuf[1] ), pBuf[2] );
#else
        stat = INVALIDPARAMETER;
#endif
      }
      break;
#endif // !GATT_NO_SERVICE_CHANGED

    case HCI_EXT_SM_REGISTER_TASK:
      SM_RegisterTask(bleDispatch_TaskID);
      break;

    case HCI_EXT_SM_GET_ECCKEYS:
      stat = SM_GetEccKeys();
      break;

    case HCI_EXT_SM_GET_DHKEY:
      {
        // each field is 32 bytes
        stat = SM_GetDHKey(pBuf, &pBuf[32], &pBuf[64]);
      }
      break;

    case HCI_EXT_SM_GET_CONFIRM_OOB:
      stat = SM_GetScConfirmOob(pBuf,
                                &pBuf[32],
                                &rspBuf[RSP_PAYLOAD_IDX]);

      *pRspDataLen = 16;
      break;

    default:
      stat = FAILURE;
      break;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      processEvents
 *
 * @brief   Process an incoming Event messages.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static uint8 processEvents( osal_event_hdr_t *pMsg )
{
  uint8 msgLen = 0;
  uint8 *pBuf = NULL;
  uint8 allocated = FALSE;
  uint8 deallocateIncoming = TRUE;

  VOID osal_memset( out_msg, 0, sizeof ( out_msg ) );

  switch ( pMsg->event )
  {
    case GAP_MSG_EVENT:
      pBuf = processEventsGAP( (gapEventHdr_t *)pMsg, out_msg, &msgLen, &allocated, &deallocateIncoming );
      break;

    case SM_MSG_EVENT:
      pBuf = processEventsSM( (smEventHdr_t *)pMsg, out_msg, &msgLen, &allocated );
      break;

    case L2CAP_SIGNAL_EVENT:
      pBuf = processEventsL2CAP( (l2capSignalEvent_t *)pMsg, out_msg, &msgLen );
      break;

    case L2CAP_DATA_EVENT:
      pBuf = processDataL2CAP( (l2capDataEvent_t *)pMsg, out_msg, &msgLen, &allocated );
      break;

    case GATT_MSG_EVENT:
      pBuf = processEventsGATT( (gattMsgEvent_t *)pMsg, out_msg, &msgLen, &allocated );
      break;
#if !defined(GATT_DB_OFF_CHIP)
    case GATT_SERV_MSG_EVENT:
      pBuf = processEventsGATTServ( (gattEventHdr_t *)pMsg, out_msg, &msgLen );
      break;
#endif
    default:
      break; // ignore
  }

  // Deallocate here to free up heap space for the serial message set out HCI.
  if ( deallocateIncoming )
  {
    VOID osal_msg_deallocate( (uint8 *)pMsg );
  }

  if ( msgLen )
  {
    HCI_SendControllerToHostEvent( HCI_VE_EVENT_CODE,  msgLen, pBuf );
  }

  if ( (pBuf != NULL) && (allocated == TRUE) )
  {
    osal_mem_free( pBuf );
  }

  return ( FALSE );
}

/*********************************************************************
 * @fn      processEventsGAP
 *
 * @brief   Process an incoming GAP Event messages.
 *
 * @param   pMsg - message to process
 * @param   pOutMsg - outgoing message to be built
 * @param   pMsgLen - length of outgoing message
 * @param   pAllocated - whether outgoing message is locally allocated
 * @param   pDeallocate - whether it's safe to deallocate incoming message
 *
 * @return  outgoing message
 */
static uint8 *processEventsGAP( gapEventHdr_t *pMsg, uint8 *pOutMsg, uint8 *pMsgLen,
                                uint8 *pAllocated, uint8 *pDeallocate )
{
  uint8 msgLen = 0;
  uint8 *pBuf = NULL;

  *pDeallocate = TRUE;

  switch ( pMsg->opcode )
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
      {
        gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

        pOutMsg[0] = LO_UINT16( HCI_EXT_GAP_DEVICE_INIT_DONE_EVENT );
        pOutMsg[1] = HI_UINT16( HCI_EXT_GAP_DEVICE_INIT_DONE_EVENT );
        pOutMsg[2] = pPkt->hdr.status;
        VOID osal_memcpy( &pOutMsg[3], pPkt->devAddr, B_ADDR_LEN );
        pOutMsg[9]  = LO_UINT16( pPkt->dataPktLen );
        pOutMsg[10] = HI_UINT16( pPkt->dataPktLen );
        pOutMsg[11] = pPkt->numDataPkts;

        // Copy the Device's local keys
        VOID osal_memcpy( &pOutMsg[12], IRK, KEYLEN );
        VOID osal_memcpy( &pOutMsg[12+KEYLEN], SRK, KEYLEN );

        if ( pPkt->hdr.status == SUCCESS )
        {
          VOID osal_snv_write( BLE_NVID_IRK, KEYLEN, IRK );
          VOID osal_snv_write( BLE_NVID_CSRK, KEYLEN, SRK );
        }

        pBuf = pOutMsg;
        msgLen = 44;
      }
      break;

    case GAP_LINK_ESTABLISHED_EVENT:
      {
        gapEstLinkReqEvent_t *pPkt = (gapEstLinkReqEvent_t *)pMsg;

#if defined(GAP_BOND_MGR)
       if ( pPkt->hdr.status == SUCCESS )
       {
          // Notify the Bond Manager to the connection
          gapBondMgr_LinkEst( pPkt->devAddrType, pPkt->devAddr, pPkt->connectionHandle, pPkt->connRole );
       }
#endif

        pOutMsg[0] = LO_UINT16( HCI_EXT_GAP_LINK_ESTABLISHED_EVENT );
        pOutMsg[1] = HI_UINT16( HCI_EXT_GAP_LINK_ESTABLISHED_EVENT );
        pOutMsg[2] = pPkt->hdr.status;
        pOutMsg[3] = pPkt->devAddrType;
        VOID osal_memcpy( &(pOutMsg[4]), pPkt->devAddr, B_ADDR_LEN );
        pOutMsg[10] = LO_UINT16( pPkt->connectionHandle );
        pOutMsg[11] = HI_UINT16( pPkt->connectionHandle );
        pOutMsg[12] = pPkt->connRole;
        pOutMsg[13] = LO_UINT16( pPkt->connInterval );
        pOutMsg[14] = HI_UINT16( pPkt->connInterval );
        pOutMsg[15] = LO_UINT16( pPkt->connLatency );
        pOutMsg[16] = HI_UINT16( pPkt->connLatency );
        pOutMsg[17] = LO_UINT16( pPkt->connTimeout );
        pOutMsg[18] = HI_UINT16( pPkt->connTimeout );
        pOutMsg[19] = pPkt->clockAccuracy;
        pBuf = pOutMsg;
        msgLen = 20;
      }
      break;

    case GAP_LINK_TERMINATED_EVENT:
      {
        gapTerminateLinkEvent_t *pPkt = (gapTerminateLinkEvent_t *)pMsg;

#if defined(GAP_BOND_MGR)
        VOID GAPBondMgr_ProcessGAPMsg( (gapEventHdr_t *)pMsg );
#endif

        pOutMsg[0] = LO_UINT16( HCI_EXT_GAP_LINK_TERMINATED_EVENT );
        pOutMsg[1] = HI_UINT16( HCI_EXT_GAP_LINK_TERMINATED_EVENT );
        pOutMsg[2] = pPkt->hdr.status;
        pOutMsg[3] = LO_UINT16( pPkt->connectionHandle );
        pOutMsg[4] = HI_UINT16( pPkt->connectionHandle );
        pOutMsg[5] = pPkt->reason;
        pBuf = pOutMsg;
        msgLen = 6;
      }
      break;

    case GAP_UPDATE_LINK_PARAM_REQ_EVENT:
      {
        gapUpdateLinkParamReqEvent_t *pPkt =
                                           (gapUpdateLinkParamReqEvent_t *)pMsg;

        pOutMsg[0]  = LO_UINT16( HCI_EXT_GAP_LINK_PARAM_UPDATE_REQ_EVENT );
        pOutMsg[1]  = HI_UINT16( HCI_EXT_GAP_LINK_PARAM_UPDATE_REQ_EVENT );
        pOutMsg[2]  = pPkt->hdr.status;
        pOutMsg[3]  = LO_UINT16( pPkt->req.connectionHandle );
        pOutMsg[4]  = HI_UINT16( pPkt->req.connectionHandle );
        pOutMsg[5]  = LO_UINT16( pPkt->req.intervalMin );
        pOutMsg[6]  = HI_UINT16( pPkt->req.intervalMin );
        pOutMsg[7]  = LO_UINT16( pPkt->req.intervalMax );
        pOutMsg[8]  = HI_UINT16( pPkt->req.intervalMax );
        pOutMsg[9]  = LO_UINT16( pPkt->req.connLatency );
        pOutMsg[10] = HI_UINT16( pPkt->req.connLatency );
        pOutMsg[11] = LO_UINT16( pPkt->req.connTimeout );
        pOutMsg[12] = HI_UINT16( pPkt->req.connTimeout );
        pOutMsg[13] = pPkt->req.signalIdentifier;
        pBuf = pOutMsg;
        msgLen = 14;
      }
      break;

    case GAP_LINK_PARAM_UPDATE_EVENT:
      {
        gapLinkUpdateEvent_t *pPkt = (gapLinkUpdateEvent_t *)pMsg;

        pOutMsg[0]  = LO_UINT16( HCI_EXT_GAP_LINK_PARAM_UPDATE_EVENT );
        pOutMsg[1]  = HI_UINT16( HCI_EXT_GAP_LINK_PARAM_UPDATE_EVENT );
        pOutMsg[2]  = pPkt->hdr.status;
        pOutMsg[3]  = LO_UINT16( pPkt->connectionHandle );
        pOutMsg[4]  = HI_UINT16( pPkt->connectionHandle );
        pOutMsg[5]  = LO_UINT16( pPkt->connInterval );
        pOutMsg[6]  = HI_UINT16( pPkt->connInterval );
        pOutMsg[7]  = LO_UINT16( pPkt->connLatency );
        pOutMsg[8]  = HI_UINT16( pPkt->connLatency );
        pOutMsg[9]  = LO_UINT16( pPkt->connTimeout );
        pOutMsg[10] = HI_UINT16( pPkt->connTimeout );
        pBuf = pOutMsg;
        msgLen = 11;
      }
      break;

    case GAP_SIGNATURE_UPDATED_EVENT:
      {
        gapSignUpdateEvent_t *pPkt = (gapSignUpdateEvent_t *)pMsg;

#if defined(GAP_BOND_MGR)
        VOID GAPBondMgr_ProcessGAPMsg( (gapEventHdr_t *)pMsg );
#endif

        pOutMsg[0]  = LO_UINT16( HCI_EXT_GAP_SIGNATURE_UPDATED_EVENT );
        pOutMsg[1]  = HI_UINT16( HCI_EXT_GAP_SIGNATURE_UPDATED_EVENT );
        pOutMsg[2]  = pPkt->hdr.status;
        pOutMsg[3]  = pPkt->addrType;
        VOID osal_memcpy( &(pOutMsg[4]), pPkt->devAddr, B_ADDR_LEN );
        pOutMsg[10] = BREAK_UINT32( pPkt->signCounter, 0 );
        pOutMsg[11] = BREAK_UINT32( pPkt->signCounter, 1 );
        pOutMsg[12] = BREAK_UINT32( pPkt->signCounter, 2 );
        pOutMsg[13] = BREAK_UINT32( pPkt->signCounter, 3 );
        pBuf = pOutMsg;
        msgLen = 14;
      }
      break;

    case GAP_PASSKEY_NEEDED_EVENT:
      {
        gapPasskeyNeededEvent_t *pPkt = (gapPasskeyNeededEvent_t *)pMsg;

#if defined(GAP_BOND_MGR)
        VOID GAPBondMgr_ProcessGAPMsg( (gapEventHdr_t *)pMsg );
#endif
        pOutMsg[0]  = LO_UINT16( HCI_EXT_GAP_PASSKEY_NEEDED_EVENT );
        pOutMsg[1]  = HI_UINT16( HCI_EXT_GAP_PASSKEY_NEEDED_EVENT );
        pOutMsg[2]  = pPkt->hdr.status;
        VOID osal_memcpy( &(pOutMsg[3]), pPkt->deviceAddr, B_ADDR_LEN );
        pOutMsg[9] = LO_UINT16( pPkt->connectionHandle );
        pOutMsg[10] = HI_UINT16( pPkt->connectionHandle );
        pOutMsg[11] = pPkt->uiInputs;
        pOutMsg[12] = pPkt->uiOutputs;
        VOID osal_memcpy( &(pOutMsg[13]), &pPkt->numComparison, 4 );
        pBuf = pOutMsg;
        msgLen = 17;
      }
      break;

    case GAP_AUTHENTICATION_COMPLETE_EVENT:
      {
        gapAuthCompleteEvent_t *pPkt = (gapAuthCompleteEvent_t *)pMsg;

#if defined(GAP_BOND_MGR)
        *pDeallocate = GAPBondMgr_ProcessGAPMsg( (gapEventHdr_t *)pMsg );
#endif

        msgLen = 107;

        pBuf = osal_mem_alloc( msgLen );
        if ( pBuf )
        {
          uint8 *buf = pBuf;

          *pAllocated = TRUE;

          VOID osal_memset( buf, 0, msgLen );

          *buf++  = LO_UINT16( HCI_EXT_GAP_AUTH_COMPLETE_EVENT );
          *buf++  = HI_UINT16( HCI_EXT_GAP_AUTH_COMPLETE_EVENT );
          *buf++  = pPkt->hdr.status;
          *buf++  = LO_UINT16( pPkt->connectionHandle );
          *buf++  = HI_UINT16( pPkt->connectionHandle );
          *buf++  = pPkt->authState;

          if ( pPkt->pSecurityInfo )
          {
            *buf++ = TRUE;
            *buf++ = pPkt->pSecurityInfo->keySize;
            VOID osal_memcpy( buf, pPkt->pSecurityInfo->ltk, KEYLEN );
            buf += KEYLEN;
            *buf++ = LO_UINT16( pPkt->pSecurityInfo->div );
            *buf++ = HI_UINT16( pPkt->pSecurityInfo->div );
            VOID osal_memcpy( buf, pPkt->pSecurityInfo->rand, B_RANDOM_NUM_SIZE );
            buf += B_RANDOM_NUM_SIZE;
          }
          else
          {
            // Skip securityInfo
            buf += 1 + KEYLEN + B_RANDOM_NUM_SIZE + 2 + 1;
          }

          if ( pPkt->pDevSecInfo )
          {
            *buf++ = TRUE;
            *buf++ = pPkt->pDevSecInfo->keySize;
            VOID osal_memcpy( buf, pPkt->pDevSecInfo->ltk, KEYLEN );
            buf += KEYLEN;
            *buf++ = LO_UINT16( pPkt->pDevSecInfo->div );
            *buf++ = HI_UINT16( pPkt->pDevSecInfo->div );
            VOID osal_memcpy( buf, pPkt->pDevSecInfo->rand, B_RANDOM_NUM_SIZE );
            buf += B_RANDOM_NUM_SIZE;
          }
          else
          {
            // Skip securityInfo
            buf += 1 + KEYLEN + B_RANDOM_NUM_SIZE + 2 + 1;
          }

          if ( pPkt->pIdentityInfo )
          {
            *buf++ = TRUE;
            VOID osal_memcpy( buf, pPkt->pIdentityInfo->irk, KEYLEN );
            buf += KEYLEN;
            VOID osal_memcpy( buf, pPkt->pIdentityInfo->bd_addr, B_ADDR_LEN );
            buf += B_ADDR_LEN;
            *buf++ = pPkt->pIdentityInfo->addrType;
          }
          else
          {
            // Skip identityInfo
            buf += KEYLEN + B_ADDR_LEN + 2;
          }

          if ( pPkt->pSigningInfo )
          {
            *buf++ = TRUE;
            VOID osal_memcpy( buf, pPkt->pSigningInfo->srk, KEYLEN );
            buf += KEYLEN;

            *buf++ = BREAK_UINT32( pPkt->pSigningInfo->signCounter, 0 );
            *buf++ = BREAK_UINT32( pPkt->pSigningInfo->signCounter, 1 );
            *buf++ = BREAK_UINT32( pPkt->pSigningInfo->signCounter, 2 );
            *buf = BREAK_UINT32( pPkt->pSigningInfo->signCounter, 3 );
          }
        }
        else
        {
          pOutMsg[0]  = LO_UINT16( HCI_EXT_GAP_AUTH_COMPLETE_EVENT );
          pOutMsg[1]  = HI_UINT16( HCI_EXT_GAP_AUTH_COMPLETE_EVENT );
          pOutMsg[2]  = bleMemAllocError;
          pOutMsg[3]  = LO_UINT16( pPkt->connectionHandle );
          pOutMsg[4]  = HI_UINT16( pPkt->connectionHandle );
          pBuf = pOutMsg;
          msgLen = 5;
        }
      }
      break;

    case GAP_BOND_COMPLETE_EVENT:
      {
        gapBondCompleteEvent_t *pPkt = (gapBondCompleteEvent_t *)pMsg;

#if defined(GAP_BOND_MGR)
        VOID GAPBondMgr_ProcessGAPMsg( (gapEventHdr_t *)pMsg );
#endif

        pOutMsg[0] = LO_UINT16( HCI_EXT_GAP_BOND_COMPLETE_EVENT );
        pOutMsg[1] = HI_UINT16( HCI_EXT_GAP_BOND_COMPLETE_EVENT );
        pOutMsg[2] = pPkt->hdr.status;
        pOutMsg[3] = LO_UINT16( pPkt->connectionHandle );
        pOutMsg[4] = HI_UINT16( pPkt->connectionHandle );
        pBuf = pOutMsg;
        msgLen = 5;
      }
      break;

    case GAP_PAIRING_REQ_EVENT:
      {
        gapPairingReqEvent_t *pPkt = (gapPairingReqEvent_t *)pMsg;
        uint8 tmp = 0;

#if defined(GAP_BOND_MGR)
        VOID GAPBondMgr_ProcessGAPMsg( (gapEventHdr_t *)pMsg );
#endif

        pOutMsg[0] = LO_UINT16( HCI_EXT_GAP_PAIRING_REQ_EVENT );
        pOutMsg[1] = HI_UINT16( HCI_EXT_GAP_PAIRING_REQ_EVENT );
        pOutMsg[2] = pPkt->hdr.status;
        pOutMsg[3] = LO_UINT16( pPkt->connectionHandle );
        pOutMsg[4] = HI_UINT16( pPkt->connectionHandle );
        pOutMsg[5] = pPkt->pairReq.ioCap;
        pOutMsg[6] = pPkt->pairReq.oobDataFlag;
        pOutMsg[7] = pPkt->pairReq.authReq;
        pOutMsg[8] = pPkt->pairReq.maxEncKeySize;

        tmp |= ( pPkt->pairReq.keyDist.sEncKey ) ? KEYDIST_SENC : 0;
        tmp |= ( pPkt->pairReq.keyDist.sIdKey ) ? KEYDIST_SID : 0;
        tmp |= ( pPkt->pairReq.keyDist.sSign ) ? KEYDIST_SSIGN : 0;
        tmp |= ( pPkt->pairReq.keyDist.sLinkKey ) ? KEYDIST_SLINK : 0;
        tmp |= ( pPkt->pairReq.keyDist.mEncKey ) ? KEYDIST_MENC : 0;
        tmp |= ( pPkt->pairReq.keyDist.mIdKey ) ? KEYDIST_MID : 0;
        tmp |= ( pPkt->pairReq.keyDist.mSign ) ? KEYDIST_MSIGN : 0;
        tmp |= ( pPkt->pairReq.keyDist.mLinkKey ) ? KEYDIST_MLINK : 0;
        pOutMsg[9] = tmp;

        pBuf = pOutMsg;
        msgLen = 10;
      }
      break;

    case GAP_SLAVE_REQUESTED_SECURITY_EVENT:
      {
        gapSlaveSecurityReqEvent_t *pPkt = (gapSlaveSecurityReqEvent_t *)pMsg;

#if defined(GAP_BOND_MGR)
        VOID GAPBondMgr_ProcessGAPMsg( (gapEventHdr_t *)pMsg );
#endif

        pOutMsg[0]  = LO_UINT16( HCI_EXT_GAP_SLAVE_REQUESTED_SECURITY_EVENT );
        pOutMsg[1]  = HI_UINT16( HCI_EXT_GAP_SLAVE_REQUESTED_SECURITY_EVENT );
        pOutMsg[2]  = pPkt->hdr.status;
        pOutMsg[3]  = LO_UINT16( pPkt->connectionHandle );
        pOutMsg[4]  = HI_UINT16( pPkt->connectionHandle );
        VOID osal_memcpy( &(pOutMsg[5]), pPkt->deviceAddr, B_ADDR_LEN );
        pOutMsg[11] = pPkt->authReq;
        pBuf = pOutMsg;
        msgLen = 12;
      }
      break;

    case GAP_LINK_PARAM_UPDATE_REJECT_EVENT:
      {
        gapLinkUpdateEvent_t *pPkt = (gapLinkUpdateEvent_t *)pMsg;

        pOutMsg[0]  = LO_UINT16( GAP_LINK_PARAM_UPDATE_REJECT_EVENT );
        pOutMsg[1]  = HI_UINT16( GAP_LINK_PARAM_UPDATE_REJECT_EVENT );
        pOutMsg[2]  = pPkt->hdr.status;
        pOutMsg[3]  = LO_UINT16( pPkt->connectionHandle );
        pOutMsg[4]  = HI_UINT16( pPkt->connectionHandle );
        pOutMsg[5]  = LO_UINT16( pPkt->connInterval );
        pOutMsg[6]  = HI_UINT16( pPkt->connInterval );
        pOutMsg[7]  = LO_UINT16( pPkt->connLatency );
        pOutMsg[8]  = HI_UINT16( pPkt->connLatency );
        pOutMsg[9]  = LO_UINT16( pPkt->connTimeout );
        pOutMsg[10] = HI_UINT16( pPkt->connTimeout );
        pBuf = pOutMsg;
        msgLen = 11;
      }
      break;

    default:
      // Unknown command
      break;
  }

  *pMsgLen = msgLen;

  return ( pBuf );
}

/*********************************************************************
 * @fn      processEventsSM
 *
 * @brief   Process an incoming SM Event messages.
 *
 * @param   pMsg       - packet to process
 * @param   pOutMsg    - outgoing message to be built
 * @param   pMsgLen    - length of outgoing message
 * @param   pAllocated - whether outgoing message is locally allocated
 *
 * @return  outgoing message
 */
static uint8 *processEventsSM( smEventHdr_t *pMsg, uint8 *pOutMsg,
                               uint8 *pMsgLen, uint8 *pAllocated )
{
  uint8 msgLen = 0;
  uint8 *pBuf = NULL;

  switch (pMsg->opcode)
  {
    case SM_ECC_KEYS_EVENT:
      {
        smEccKeysEvt_t *keys = (smEccKeysEvt_t *)pMsg;

        msgLen = (SM_ECC_KEY_LEN * 3) + 3;

        pBuf = osal_mem_alloc( msgLen );
        if ( pBuf )
        {
          uint8 *buf = pBuf;

          *buf++ = LO_UINT16( HCI_EXT_SM_GET_ECC_KEYS_EVENT );
          *buf++ = HI_UINT16( HCI_EXT_SM_GET_ECC_KEYS_EVENT );
          *buf++ = keys->hdr.status;
          osal_memcpy(buf, keys->privateKey, SM_ECC_KEY_LEN);
          buf+=32;
          osal_memcpy(buf, keys->publicKeyX, SM_ECC_KEY_LEN);
          buf+=32;
          osal_memcpy(buf, keys->publicKeyY, SM_ECC_KEY_LEN);

          *pAllocated = TRUE;
        }
        else
        {
          keys->hdr.status = bleMemAllocError;
        }
      }
      break;

    case SM_DH_KEY_EVENT:
      {
        smDhKeyEvt_t *dhKey = (smDhKeyEvt_t *)pMsg;

        msgLen = SM_ECC_KEY_LEN + 3;

        pBuf = osal_mem_alloc( msgLen );
        if ( pBuf )
        {
          uint8 *buf = pBuf;
          *buf++ = LO_UINT16( HCI_EXT_SM_GET_DH_KEY_EVENT );
          *buf++ = HI_UINT16( HCI_EXT_SM_GET_DH_KEY_EVENT );
          *buf++ = dhKey->hdr.status;
          osal_memcpy(buf, dhKey->dhKey, SM_ECC_KEY_LEN);

          *pAllocated = TRUE;
        }
        else
        {
          dhKey->hdr.status = bleMemAllocError;
        }
      }
      break;

    default:
      break;
  }

  *pMsgLen = msgLen;

  return ( pBuf );
}


/*********************************************************************
 * @fn      processEventsL2CAP
 *
 * @brief   Process an incoming L2CAP Event messages.
 *
 * @param   pPkt - packet to process
 * @param   pOutMsg - outgoing message to be built
 * @param   pMsgLen - length of outgoing message
 *
 * @return  outgoing message
 */
static uint8 *processEventsL2CAP( l2capSignalEvent_t *pPkt, uint8 *pOutMsg, uint8 *pMsgLen )
{
  uint8 msgLen;

  // Build the message header first
  msgLen = buildHCIExtHeader( pOutMsg, (HCI_EXT_L2CAP_EVENT | pPkt->opcode),
                              pPkt->hdr.status, pPkt->connHandle );
#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
  // Build Send SDU Done event regardless of status
  if ( pPkt->opcode == L2CAP_SEND_SDU_DONE_EVT )
  {
    l2capSendSduDoneEvt_t *pSduEvt = &(pPkt->cmd.sendSduDoneEvt);

    pOutMsg[msgLen++] = LO_UINT16( pSduEvt->CID );
    pOutMsg[msgLen++] = HI_UINT16( pSduEvt->CID );

    pOutMsg[msgLen++] = LO_UINT16( pSduEvt->credits );
    pOutMsg[msgLen++] = HI_UINT16( pSduEvt->credits );

    pOutMsg[msgLen++] = LO_UINT16( pSduEvt->peerCID );
    pOutMsg[msgLen++] = HI_UINT16( pSduEvt->peerCID );

    pOutMsg[msgLen++] = LO_UINT16( pSduEvt->peerCredits );
    pOutMsg[msgLen++] = HI_UINT16( pSduEvt->peerCredits );

    pOutMsg[msgLen++] = LO_UINT16( pSduEvt->totalLen );
    pOutMsg[msgLen++] = HI_UINT16( pSduEvt->totalLen );

    pOutMsg[msgLen++] = LO_UINT16( pSduEvt->txLen );
    pOutMsg[msgLen++] = HI_UINT16( pSduEvt->txLen );
  }
  else
#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)
  if ( pPkt->hdr.status == SUCCESS ) // Build all other messages if status is success
  {
    switch ( pPkt->opcode )
    {
      case L2CAP_CMD_REJECT:
        msgLen += L2CAP_BuildCmdReject( &pOutMsg[msgLen], (uint8 *)&(pPkt->cmd.cmdReject) );
        break;

      case L2CAP_PARAM_UPDATE_RSP:
        msgLen += L2CAP_BuildParamUpdateRsp( &pOutMsg[msgLen], (uint8 *)&(pPkt->cmd.updateRsp) );
        break;

      case L2CAP_INFO_RSP:
        msgLen += L2CAP_BuildInfoRsp( &pOutMsg[msgLen], (uint8 *)&(pPkt->cmd.infoRsp) );
        break;

#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
      case L2CAP_CONNECT_REQ:
        pOutMsg[msgLen++] = pPkt->id;
        msgLen += L2CAP_BuildConnectReq( &pOutMsg[msgLen], (uint8 *)&(pPkt->cmd.connectReq) );
        break;

      case L2CAP_CHANNEL_ESTABLISHED_EVT:
        {
          l2capChannelEstEvt_t *pEstEvt = &(pPkt->cmd.channelEstEvt);

          pOutMsg[msgLen++] = LO_UINT16( pEstEvt->result );
          pOutMsg[msgLen++] = HI_UINT16( pEstEvt->result );

          if ( pEstEvt->result == L2CAP_CONN_SUCCESS )
          {
            msgLen += buildCoChannelInfo( pEstEvt->CID, &pEstEvt->info, &pOutMsg[msgLen] );
          }
        }
        break;

       case L2CAP_CHANNEL_TERMINATED_EVT:
        {
          l2capChannelTermEvt_t *pTermEvt = &(pPkt->cmd.channelTermEvt);

          pOutMsg[msgLen++] = LO_UINT16( pTermEvt->CID );
          pOutMsg[msgLen++] = HI_UINT16( pTermEvt->CID );

          pOutMsg[msgLen++] = LO_UINT16( pTermEvt->peerCID );
          pOutMsg[msgLen++] = HI_UINT16( pTermEvt->peerCID );

          pOutMsg[msgLen++] = LO_UINT16( pTermEvt->reason );
          pOutMsg[msgLen++] = HI_UINT16( pTermEvt->reason );
        }
        break;

      case L2CAP_OUT_OF_CREDIT_EVT:
      case L2CAP_PEER_CREDIT_THRESHOLD_EVT:
        {
          l2capCreditEvt_t *pCreditEvt = &(pPkt->cmd.creditEvt);

          pOutMsg[msgLen++] = LO_UINT16( pCreditEvt->CID );
          pOutMsg[msgLen++] = HI_UINT16( pCreditEvt->CID );

          pOutMsg[msgLen++] = LO_UINT16( pCreditEvt->peerCID );
          pOutMsg[msgLen++] = HI_UINT16( pCreditEvt->peerCID );

          pOutMsg[msgLen++] = LO_UINT16( pCreditEvt->credits );
          pOutMsg[msgLen++] = HI_UINT16( pCreditEvt->credits );
        }
        break;
#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)

      default:
        // Unknown command
        break;
    }
  }

  *pMsgLen = msgLen;

  return ( pOutMsg );
}

/*********************************************************************
 * @fn      processDataL2CAP
 *
 * @brief   Process an incoming L2CAP Data message.
 *
 * @param   pPkt - packet to process
 * @param   pOutMsg - outgoing message to be built
 * @param   pMsgLen - length of outgoing message
 *
 * @return  outgoing message
 */
static uint8 *processDataL2CAP( l2capDataEvent_t *pPkt, uint8 *pOutMsg,
                                uint8 *pMsgLen, uint8 *pAllocated )
{
  uint8 *pBuf;
  uint16 msgLen = HCI_EXT_HDR_LEN + 2 + 2; // hdr + CID + packet length;
  uint8 status = pPkt->hdr.status;

  *pAllocated = FALSE;

  msgLen += pPkt->pkt.len;
  if ( msgLen > HCI_EXT_APP_OUT_BUF )
  {
    pBuf = osal_mem_alloc( msgLen );
    if ( pBuf )
    {
      *pAllocated = TRUE;
    }
    else
    {
      pBuf = pOutMsg;
      msgLen -= pPkt->pkt.len;

      status = bleMemAllocError;
    }
  }
  else
  {
    pBuf = pOutMsg;
  }

  // Build the message header first
  VOID buildHCIExtHeader( pBuf, (HCI_EXT_L2CAP_EVENT | HCI_EXT_L2CAP_DATA),
                          status, pPkt->connHandle );
  // Add CID
  pBuf[HCI_EXT_HDR_LEN] = LO_UINT16( pPkt->pkt.CID );
  pBuf[HCI_EXT_HDR_LEN+1] = HI_UINT16( pPkt->pkt.CID );

  // Add data length
  pBuf[HCI_EXT_HDR_LEN+2] = LO_UINT16( pPkt->pkt.len );
  pBuf[HCI_EXT_HDR_LEN+3] = HI_UINT16( pPkt->pkt.len );

  // Add payload
  if ( pPkt->pkt.pPayload != NULL )
  {
    if ( status == SUCCESS )
    {
      VOID osal_memcpy( &pBuf[HCI_EXT_HDR_LEN+4], pPkt->pkt.pPayload, pPkt->pkt.len );
    }

    // Received buffer is processed so it's safe to free it
    osal_bm_free( pPkt->pkt.pPayload );
  }

  *pMsgLen = msgLen;

  return ( pBuf );
}

/*********************************************************************
 * @fn      processEventsGATT
 *
 * @brief   Process an incoming GATT Event messages.
 *
 * @param   pPkt - packet to process
 * @param   pOutMsg - outgoing message to be built
 * @param   pMsgLen - length of outgoing message
 * @param   pAllocated - whether outgoing message is locally allocated
 *
 * @return  outgoing message
 */
static uint8 *processEventsGATT( gattMsgEvent_t *pPkt, uint8 *pOutMsg,
                                 uint8 *pMsgLen, uint8 *pAllocated )
{
  uint8 msgLen = 0, attHdrLen = 0, hdrLen = HCI_EXT_HDR_LEN + 1; // hdr + event length
  uint8 *pBuf, *pPayload = NULL;
  uint8 status = pPkt->hdr.status;

  *pAllocated = FALSE;

  if ( ( status == SUCCESS ) || ( status == blePending ) )
  {
    // Build the ATT header first
    switch ( pPkt->method )
    {
      case ATT_ERROR_RSP:
        attHdrLen = ATT_BuildErrorRsp( &pOutMsg[hdrLen], (uint8 *)&(pPkt->msg.errorRsp) );
        break;

      case ATT_EXCHANGE_MTU_REQ:
      case ATT_MTU_UPDATED_EVENT:
        attHdrLen = ATT_BuildExchangeMTUReq( &pOutMsg[hdrLen], (uint8 *)&(pPkt->msg.exchangeMTUReq) );
        break;

      case ATT_EXCHANGE_MTU_RSP:
        attHdrLen = ATT_BuildExchangeMTURsp( &pOutMsg[hdrLen], (uint8 *)&(pPkt->msg.exchangeMTURsp) );
        break;

      case ATT_FIND_INFO_REQ:
        attHdrLen = ATT_BuildFindInfoReq( &pOutMsg[hdrLen], (uint8 *)&(pPkt->msg.findInfoReq) );
        break;

      case ATT_FIND_INFO_RSP:
        {
          attFindInfoRsp_t *pRsp = &pPkt->msg.findInfoRsp;

          attHdrLen = ATT_FIND_INFO_RSP_FIXED_SIZE;

          // Copy response header over
          msgLen = ATT_BuildFindInfoRsp( &pOutMsg[hdrLen], (uint8 *)pRsp ) - attHdrLen;
          pPayload = pRsp->pInfo;
        }
        break;

      case ATT_FIND_BY_TYPE_VALUE_REQ:
        {
          attFindByTypeValueReq_t *pReq = &pPkt->msg.findByTypeValueReq;

          attHdrLen = ATT_FIND_BY_TYPE_VALUE_REQ_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildFindByTypeValueReq( &pOutMsg[hdrLen], (uint8 *)pReq ) - attHdrLen;
          pPayload = pReq->pValue;
        }
        break;

      case ATT_FIND_BY_TYPE_VALUE_RSP:
        {
          attFindByTypeValueRsp_t *pRsp = &pPkt->msg.findByTypeValueRsp;

          msgLen = ATT_BuildFindByTypeValueRsp( &pOutMsg[hdrLen], (uint8 *)pRsp );
          pPayload = pRsp->pHandlesInfo;
        }
        break;

      case ATT_READ_BY_TYPE_REQ:
        attHdrLen = ATT_BuildReadByTypeReq( &pOutMsg[hdrLen], (uint8 *)&(pPkt->msg.readByTypeReq) );
        break;

      case ATT_READ_BY_TYPE_RSP:
        {
          attReadByTypeRsp_t *pRsp = &pPkt->msg.readByTypeRsp;

          attHdrLen = ATT_READ_BY_TYPE_RSP_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildReadByTypeRsp( &pOutMsg[hdrLen], (uint8 *)pRsp ) - attHdrLen;
          pPayload = pRsp->pDataList;
        }
        break;

      case ATT_READ_REQ:
        attHdrLen = ATT_BuildReadReq( &pOutMsg[hdrLen], (uint8 *)&(pPkt->msg.readReq) );
        break;

      case ATT_READ_RSP:
        {
          attReadRsp_t *pRsp = &pPkt->msg.readRsp;

          msgLen = ATT_BuildReadRsp( &pOutMsg[hdrLen], (uint8 *)pRsp );
          pPayload = pRsp->pValue;
        }
        break;

      case ATT_READ_BLOB_REQ:
        attHdrLen = ATT_BuildReadBlobReq( &pOutMsg[hdrLen], (uint8 *)&(pPkt->msg.readBlobReq) );
        break;

      case ATT_READ_BLOB_RSP:
        {
          attReadBlobRsp_t *pRsp = &pPkt->msg.readBlobRsp;

          msgLen = ATT_BuildReadBlobRsp( &pOutMsg[hdrLen], (uint8 *)pRsp );
          pPayload = pRsp->pValue;
        }
        break;

      case ATT_READ_MULTI_REQ:
        {
          attReadMultiReq_t *pReq = &pPkt->msg.readMultiReq;

          msgLen = ATT_BuildReadMultiReq( &pOutMsg[hdrLen], (uint8 *)pReq );
          pPayload = pReq->pHandles;
        }
        break;

      case ATT_READ_MULTI_RSP:
        {
          attReadMultiRsp_t *pRsp = &pPkt->msg.readMultiRsp;

          msgLen = ATT_BuildReadMultiRsp( &pOutMsg[hdrLen], (uint8 *)pRsp );
          pPayload = pRsp->pValues;
        }
        break;

      case ATT_READ_BY_GRP_TYPE_REQ:
        attHdrLen = ATT_BuildReadByTypeReq( &pOutMsg[hdrLen], (uint8 *)&(pPkt->msg.readByGrpTypeReq) );
        break;

      case ATT_READ_BY_GRP_TYPE_RSP:
        {
          attReadByGrpTypeRsp_t *pRsp = &pPkt->msg.readByGrpTypeRsp;

          attHdrLen = ATT_READ_BY_GRP_TYPE_RSP_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildReadByGrpTypeRsp( &pOutMsg[hdrLen], (uint8 *)pRsp ) - attHdrLen;
          pPayload = pRsp->pDataList;
        }
        break;

      case ATT_WRITE_REQ:
        {
          attWriteReq_t *pReq = &pPkt->msg.writeReq;

          pOutMsg[hdrLen] = pReq->sig;
          pOutMsg[hdrLen+1] = pReq->cmd;

          attHdrLen = ATT_WRITE_REQ_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildWriteReq( &pOutMsg[hdrLen+2], (uint8 *)pReq ) - attHdrLen;
          pPayload = pReq->pValue;

          attHdrLen += 2; // sig + cmd
        }
        break;

      case ATT_PREPARE_WRITE_REQ:
        {
          attPrepareWriteReq_t *pReq = &pPkt->msg.prepareWriteReq;

          attHdrLen = ATT_PREPARE_WRITE_REQ_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildPrepareWriteReq( &pOutMsg[hdrLen], (uint8 *)pReq ) - attHdrLen;
          pPayload = pReq->pValue;
        }
        break;

      case ATT_PREPARE_WRITE_RSP:
        {
          attPrepareWriteRsp_t *pRsp = &pPkt->msg.prepareWriteRsp;

          attHdrLen = ATT_PREPARE_WRITE_RSP_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildPrepareWriteRsp( &pOutMsg[hdrLen], (uint8 *)pRsp ) - attHdrLen;
          pPayload = pRsp->pValue;
        }
        break;

      case ATT_EXECUTE_WRITE_REQ:
        attHdrLen = ATT_BuildExecuteWriteReq( &pOutMsg[hdrLen], (uint8 *)&(pPkt->msg.executeWriteReq) );
        break;

      case ATT_HANDLE_VALUE_IND:
      case ATT_HANDLE_VALUE_NOTI:
        {
          attHandleValueInd_t *pInd = &pPkt->msg.handleValueInd;

          attHdrLen = ATT_HANDLE_VALUE_IND_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildHandleValueInd( &pOutMsg[hdrLen], (uint8 *)pInd ) - attHdrLen;
          pPayload = pInd->pValue;
        }
        break;

      case ATT_FLOW_CTRL_VIOLATED_EVENT:
        {
          attFlowCtrlViolatedEvt_t *pEvt = &pPkt->msg.flowCtrlEvt;

          pOutMsg[hdrLen]   = pEvt->opcode;
          pOutMsg[hdrLen+1] = pEvt->pendingOpcode;

          attHdrLen = 2;
        }
        break;

      default:
        // Unknown command
        break;
    }
  }

  // Event format: HCI Ext hdr + event len + ATT hdr + ATT PDU
  if ( ( hdrLen + attHdrLen + msgLen ) > HCI_EXT_APP_OUT_BUF )
  {
    pBuf = osal_mem_alloc( hdrLen + attHdrLen + msgLen );
    if ( pBuf )
    {
      *pAllocated = TRUE;

      // Copy the ATT header over
      if ( attHdrLen > 0 )
      {
        VOID osal_memcpy( &pBuf[hdrLen], &pOutMsg[hdrLen], attHdrLen );
      }
    }
    else
    {
      pBuf = pOutMsg;
      msgLen = 0;

      status = bleMemAllocError;
    }
  }
  else
  {
    pBuf = pOutMsg;
  }

  // Build the message PDU
  if ( pPayload != NULL )
  {
    if ( msgLen > 0 )
    {
      // Copy the message payload over
      VOID osal_memcpy( &pBuf[hdrLen+attHdrLen], pPayload, msgLen );
    }

    // Free the payload buffer
    osal_bm_free( pPayload );
  }

  // Build the message header
  VOID buildHCIExtHeader( pBuf, (HCI_EXT_ATT_EVENT | pPkt->method), status, pPkt->connHandle );

  // Add the event (PDU) length for GATT events for now!
  pBuf[HCI_EXT_HDR_LEN] = attHdrLen + msgLen;

  *pMsgLen = hdrLen + attHdrLen + msgLen;

  return ( pBuf );
}

#if !defined(GATT_DB_OFF_CHIP)
/*********************************************************************
 * @fn      processEventsGATTServ
 *
 * @brief   Process an incoming GATT Server Event messages.
 *
 * @param   pPkt - packet to process
 * @param   pMsg - outgoing message to be built
 * @param   pMsgLen - length of outgoing message
 *
 * @return  outgoing message
 */
static uint8 *processEventsGATTServ( gattEventHdr_t *pPkt, uint8 *pMsg, uint8 *pMsgLen )
{
  uint8 hdrLen = HCI_EXT_HDR_LEN + 1; // hdr + event length
  uint8 msgLen = 0;
  uint8 *pBuf = pMsg;

  if ( pPkt->hdr.status == SUCCESS )
  {
    // Build the message first
    switch ( pPkt->method )
    {
      case GATT_CLIENT_CHAR_CFG_UPDATED_EVENT:
        {
          gattClientCharCfgUpdatedEvent_t *pEvent = (gattClientCharCfgUpdatedEvent_t *)pPkt;

#if defined(GAP_BOND_MGR)
          VOID gapBondMgr_UpdateCharCfg( pEvent->connHandle, pEvent->attrHandle, pEvent->value );
#endif
          // Attribute handle
          pMsg[hdrLen]   = LO_UINT16( pEvent->attrHandle );
          pMsg[hdrLen+1] = HI_UINT16( pEvent->attrHandle );

          // Attribute value
          pMsg[hdrLen+2] = LO_UINT16( pEvent->value );
          pMsg[hdrLen+3] = HI_UINT16( pEvent->value );

          msgLen = 4;
        }
        break;

      default:
        // Unknown command
        break;
    }
  }

  // Build the message header
  VOID buildHCIExtHeader( pBuf, (HCI_EXT_GATT_EVENT | pPkt->method), pPkt->hdr.status, pPkt->connHandle );

  // Add the event (PDU) length for GATT events for now!
  pBuf[HCI_EXT_HDR_LEN] = msgLen;

  *pMsgLen = hdrLen + msgLen;

  return ( pBuf );
}
#endif // !GATT_DB_OFF_CHIP

#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
/*********************************************************************
 * @fn      buildCoChannelInfo
 *
 * @brief   Build Connection Oriented Channel info.
 *
 * @param   CID - local CID
 * @param   pInfo - pointer to CoC info
 * @param   pRspBuf - buffer to copy CoC info into
 *
 * @return  length of data copied
 */
static uint8 buildCoChannelInfo( uint16 CID, l2capCoCInfo_t *pInfo, uint8 *pRspBuf )
{
  uint8 msgLen = 0;

  pRspBuf[msgLen++] = LO_UINT16( pInfo->psm );
  pRspBuf[msgLen++] = HI_UINT16( pInfo->psm );

  pRspBuf[msgLen++] = LO_UINT16( CID );
  pRspBuf[msgLen++] = HI_UINT16( CID );

  pRspBuf[msgLen++] = LO_UINT16( pInfo->mtu );
  pRspBuf[msgLen++] = HI_UINT16( pInfo->mtu );

  pRspBuf[msgLen++] = LO_UINT16( pInfo->mps );
  pRspBuf[msgLen++] = HI_UINT16( pInfo->mps );

  pRspBuf[msgLen++] = LO_UINT16( pInfo->credits );
  pRspBuf[msgLen++] = HI_UINT16( pInfo->credits );

  pRspBuf[msgLen++] = LO_UINT16( pInfo->peerCID );
  pRspBuf[msgLen++] = HI_UINT16( pInfo->peerCID );

  pRspBuf[msgLen++] = LO_UINT16( pInfo->peerMtu );
  pRspBuf[msgLen++] = HI_UINT16( pInfo->peerMtu );

  pRspBuf[msgLen++] = LO_UINT16( pInfo->peerMps );
  pRspBuf[msgLen++] = HI_UINT16( pInfo->peerMps );

  pRspBuf[msgLen++] = LO_UINT16( pInfo->peerCredits );
  pRspBuf[msgLen++] = HI_UINT16( pInfo->peerCredits );

  pRspBuf[msgLen++] = LO_UINT16( pInfo->peerCreditThreshold );
  pRspBuf[msgLen++] = HI_UINT16( pInfo->peerCreditThreshold );

  return ( msgLen );
}

/*********************************************************************
  * @fn      l2capVerifySecCB
 *
 * @brief   Callback function to verify security when a Connection
 *          Request is received.
 *
 * @param   connHandle - connection handle request was received on
 * @param   id - identifier matches responses with requests
 * @param   pReq - received connection request
 *
 * @return  See L2CAP Connection Response: Result values
 */
static uint16 l2capVerifySecCB( uint16 connHandle, uint8 id, l2capConnectReq_t *pReq )
{
  uint8 msgLen = 0;

  // Build the message header first
  msgLen = buildHCIExtHeader( out_msg, (HCI_EXT_L2CAP_EVENT | L2CAP_CONNECT_REQ),
                              SUCCESS, connHandle );

  out_msg[msgLen++] = id;
  msgLen += L2CAP_BuildConnectReq( &out_msg[msgLen], (uint8 *)pReq );

  // Send out the Connection Request
  HCI_SendControllerToHostEvent( HCI_VE_EVENT_CODE,  msgLen, out_msg );

  return ( L2CAP_CONN_PENDING_SEC_VERIFY );
}
#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)

/*********************************************************************
 * @fn      buildHCIExtHeader
 *
 * @brief   Build an HCI Extension header.
 *
 * @param   pBuf - header to be built
 * @param   event - event id
 * @param   status - event status
 * @param   connHandle - connection handle
 *
 * @return  header length
 */
static uint8 buildHCIExtHeader( uint8 *pBuf, uint16 event, uint8 status, uint16 connHandle )
{
  pBuf[0] = LO_UINT16( event );
  pBuf[1] = HI_UINT16( event );
  pBuf[2] = status;
  pBuf[3] = LO_UINT16( connHandle );
  pBuf[4] = HI_UINT16( connHandle );

  return ( HCI_EXT_HDR_LEN );
}

/*********************************************************************
 * @fn      mapATT2BLEStatus
 *
 * @brief   Map ATT error code to BLE Generic status code.
 *
 * @param   status - ATT status
 *
 * @return  BLE Generic status
 */
static uint8  mapATT2BLEStatus( uint8 status )
{
  uint8 stat;

  switch ( status )
  {
    case ATT_ERR_INSUFFICIENT_AUTHEN:
      // Returned from Send routines
      stat = bleInsufficientAuthen;
      break;

    case ATT_ERR_INSUFFICIENT_ENCRYPT:
      // Returned from Send routines
      stat = bleInsufficientEncrypt;
      break;

    case ATT_ERR_INSUFFICIENT_KEY_SIZE:
      // Returned from Send routines
      stat = bleInsufficientKeySize;
      break;

    default:
      stat = status;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      createMsgPayload
 *
 * @brief   Create payload buffer for OTA message to be sent.
 *
 * @param   pBuf - pointer to message data
 * @param   len - length of message data
 *
 * @return  pointer to payload. NULL if alloc fails.
 */
static uint8 *createMsgPayload( uint8 *pBuf, uint16 len )
{
  return ( createPayload( pBuf, len, 0 ) );
}

/*********************************************************************
 * @fn      createMsgPayload
 *
 * @brief   Create payload buffer for OTA message to be sent.
 *
 * @param   sig - whether to include authentication signature
 * @param   cmd - whether it's write command
 * @param   pBuf - pointer to message data
 * @param   len - length of message data
 *
 * @return  pointer to payload. NULL if alloc fails.
 */
static uint8 *createSignedMsgPayload( uint8 sig, uint8 cmd, uint8 *pBuf, uint16 len )
{
  if ( sig && cmd )
  {
    // Need to allocate space for Authentication Signature
    return ( createPayload( pBuf, len, ATT_AUTHEN_SIG_LEN ) );
  }

  return ( createPayload( pBuf, len, 0 ) );
}

/*********************************************************************
 * @fn      createPayload
 *
 * @brief   Create payload buffer for OTA message plus authentication signature
 *          to be sent.
 *
 * @param   pBuf - pointer to message data
 * @param   len - length of message data
 * @param   sigLen - length of authentication signature
 *
 * @return  pointer to payload. NULL if alloc fails.
 */
static uint8 *createPayload( uint8 *pBuf, uint16 len, uint8 sigLen )
{
  // Create payload buffer for OTA message to be sent
  uint8 *pPayload = L2CAP_bm_alloc( len + ATT_OPCODE_SIZE + sigLen );
  if ( pPayload != NULL )
  {
    pPayload = osal_bm_adjust_header( pPayload, -ATT_OPCODE_SIZE );

    // Copy received data over
    VOID osal_memcpy( pPayload, pBuf, len );

    return ( pPayload );
  }

  return ( NULL );
}

#if defined(GATT_DB_OFF_CHIP)
/*********************************************************************
 * @fn      addAttrRec
 *
 * @brief   Add attribute record to its service.
 *
 * @param   pServ - GATT service
 * @param   pUUID - attribute UUID
 * @param   len - length of UUID
 * @param   permissions - attribute permissions
 * @param   pTotalAttrs - total number of attributes
 * @param   pRspDataLen - response data length to be returned
 *
 * @return  status
 */
static uint8 addAttrRec( gattService_t *pServ, uint8 *pUUID, uint8 len,
                         uint8 permissions, uint16 *pTotalAttrs, uint8 *pRspDataLen )
{
  gattAttribute_t *pAttr = &(pServ->attrs[pServ->numAttrs]);
  uint8 stat = SUCCESS;

  // Set up attribute record
  pAttr->type.uuid = findUUIDRec( pUUID, len );
  if ( pAttr->type.uuid != NULL )
  {
    pAttr->type.len = len;
    pAttr->permissions = permissions;

    // Are all attributes added to the service yet?
    if ( ++pServ->numAttrs == *pTotalAttrs )
    {
      // Register the service with the GATT Server
      stat = GATT_RegisterService( pServ );
      if ( stat == SUCCESS )
      {
        *pRspDataLen = 4;

        // Service startHandle
        uint16 handle = pServ->attrs[0].handle;
        rspBuf[RSP_PAYLOAD_IDX] = LO_UINT16( handle );
        rspBuf[RSP_PAYLOAD_IDX+1] = HI_UINT16( handle );

        // Service endHandle
        handle = pServ->attrs[pServ->numAttrs-1].handle;
        rspBuf[RSP_PAYLOAD_IDX+2] = LO_UINT16( handle );
        rspBuf[RSP_PAYLOAD_IDX+3] = HI_UINT16( handle );

        // Service is registered with GATT; clear its info
        pServ->attrs = NULL;
        pServ->numAttrs = 0;
      }
      else
      {
        freeAttrRecs( pServ );
      }

      // We're done with this service
      *pTotalAttrs = 0;
    }
  }
  else
  {
    stat = INVALIDPARAMETER;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      freeAttrRecs
 *
 * @brief   Free attribute records. Also, free UUIDs that were
 *          allocated dynamically.
 *
 * @param   pServ - GATT service
 *
 * @return  none
 */
static void freeAttrRecs( gattService_t *pServ )
{
  if ( pServ->attrs != NULL )
  {
    for ( uint8 i = 0; i < pServ->numAttrs; i++ )
    {
      gattAttrType_t *pType = &pServ->attrs[i].type;
      if ( pType->uuid != NULL )
      {
        if ( GATT_FindUUIDRec( (uint8 *)pType->uuid, pType->len ) == NULL )
        {
          // UUID was dynamically allocated; free it
          osal_mem_free( (uint8 *)pType->uuid );
        }
      }
    }

    osal_mem_free( pServ->attrs );

    pServ->attrs = NULL;
    pServ->numAttrs = 0;
  }
}

/*********************************************************************
 * @fn      findUUIDRec
 *
 * @brief   Find UUID record. If the record is not found, create one
 *          dynamically.
 *
 * @param   pUUID - UUID to look for
 * @param   len - length of UUID
 *
 * @return  UUID record
 */
static const uint8 *findUUIDRec( uint8 *pUUID, uint8 len )
{
  const uint8 *pUuid = GATT_FindUUIDRec( pUUID, len );
  if ( pUuid == NULL )
  {
    // UUID not found; allocate space for it
    pUuid = osal_mem_alloc( len );
    if ( pUuid != NULL )
    {
      VOID osal_memcpy( (uint8 *)pUuid, pUUID, len );
    }
  }

  return ( pUuid );
}
#endif // GATT_DB_OFF_CHIP
#endif // HOST_CONFIG
#endif // HCI_TL_FULL

/*********************************************************************
*********************************************************************/
