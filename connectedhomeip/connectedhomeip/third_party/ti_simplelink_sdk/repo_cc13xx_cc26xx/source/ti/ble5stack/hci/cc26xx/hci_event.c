/******************************************************************************

 @file  hci_event.c

 @brief This file send HCI events for the controller. It implements all the
        LL event callback and HCI events send.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "hci_event.h"
#include "ble.h"

#if defined( CC26XX ) || defined( CC13XX )
#include "rom_jt.h"
#endif // CC26XX/CC13XX

#include "rom_jt.h"

/*******************************************************************************
 * MACROS
 */
#define LL_PHY_UPDATE_TYPE_CODED 3
// Note 7.7.65.12 - LE Phy Update Complete Event - LE Coded PHY type defined in TX_PHY and RX_PHY should be 0x03 
#define LL_ConvertPhy(phy) ((phy == LL_PHY_CODED)? LL_PHY_UPDATE_TYPE_CODED : phy)
/*******************************************************************************
 * CONSTANTS
 */

// CTE report event samples indexing
#define HCI_CTE_SAMPLES_COUNT_REF_PERIOD         (8)    //number of samples in referece period according to spec
#define HCI_CTE_FIRST_SAMPLE_IDX_REF_PERIOD      (1)    //reference period start samples index for sample rate 1Mhz,2Mhz and 3Mhz
#define HCI_CTE_FIRST_SAMPLE_IDX_REF_PERIOD_4MHZ (0)    //reference period start samples index for sample rate 4Mhz
#define HCI_CTE_FIRST_SAMPLE_IDX_SLOT_1US        (37)   //1us start samples index for sample rate 1Mhz,2Mhz and 3Mhz
#define HCI_CTE_FIRST_SAMPLE_IDX_SLOT_2US        (45)   //2us start samples index for sample rate 1Mhz,2Mhz and 3Mhz
#define HCI_CTE_FIRST_SAMPLE_IDX_SLOT_1US_4MHZ   (36)   //1us start samples index for sample rate 4Mhz
#define HCI_CTE_FIRST_SAMPLE_IDX_SLOT_2US_4MHZ   (44)   //2us start samples index for sample rate 4Mhz

// CTE report event samples offsets
#define HCI_CTE_SAMPLE_JUMP_REF_PERIOD           (4)    //peek 1 sample every 4 samples
#define HCI_CTE_SAMPLE_JUMP_SLOT_1US             (8)    //peek 1 sample every 8 samples
#define HCI_CTE_SAMPLE_JUMP_SLOT_2US             (16)   //peek 1 sample every 16 samples

#define HCI_CTE_MAX_SAMPLES_PER_EVENT            (96)   //max samples data length in one event
#define HCI_CTE_MAX_RF_BUFFER_SIZE               (512)  //first buffer size (MCE RAM)
#define HCI_CTE_MAX_RF_EXT_BUFFER_SIZE           (512)  //second buffer size (RFE RAM)
#define HCI_CTE_SAMPLE_RATE_4MHZ                 (4)
#define HCI_CTE_SAMPLE_RATE_1MHZ                 (1)

#define HCI_PERIODIC_ADV_REPORT_MAX_DATA         (0xFF - HCI_PERIODIC_ADV_REPORT_EVENT_LEN)
#define HCI_PERIODIC_ADV_REPORT_DATA_INCOMPLETE  (1)

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

uint8 pBleEvtMask[B_EVENT_MASK_LEN];
uint8 pHciEvtMask[B_EVENT_MASK_LEN];
uint8 pHciEvtMask2[B_EVENT_MASK_LEN];

char *BLEEventCode_BleLogStrings[] = {
  "HCI_BLE_CONNECTION_COMPLETE_EVENT                 ",
  "HCI_BLE_ADV_REPORT_EVENT                          ",
  "HCI_BLE_CONN_UPDATE_COMPLETE_EVENT                ",
  "HCI_BLE_READ_REMOTE_FEATURE_COMPLETE_EVENT        ",
  "HCI_BLE_LTK_REQUESTED_EVENT                       ",
  "HCI_BLE_REMOTE_CONN_PARAM_REQUEST_EVENT           ",
  "HCI_BLE_DATA_LENGTH_CHANGE_EVENT                  ",
  "HCI_BLE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT ",
  "HCI_BLE_GENERATE_DHKEY_COMPLETE_EVENT             ",
  "HCI_BLE_ENHANCED_CONNECTION_COMPLETE_EVENT        ",
  "HCI_BLE_DIRECT_ADVERTISING_REPORT_EVENT           ",
  "HCI_BLE_PHY_UPDATE_COMPLETE_EVENT                 ",
  "HCI_BLE_EXTENDED_ADV_REPORT_EVENT                 ",
  "HCI_BLE_PERIODIC_ADV_SYNCH_ESTABLISHED_EVENT      ",
  "HCI_BLE_PERIODIC_ADV_REPORT_EVENT                 ",
  "HCI_BLE_PERIODIC_ADV_SYNCH_LOST_EVENT             ",
  "HCI_BLE_SCAN_TIMEOUT_EVENT                        ",
  "HCI_BLE_ADV_SET_TERMINATED_EVENT                  ",
  "HCI_BLE_SCAN_REQUEST_RECEIVED_EVENT               ",
  "HCI_BLE_CHANNEL_SELECTION_ALGORITHM_EVENT         ",
  "HCI_BLE_CONNECTIONLESS_IQ_REPORT_EVENT            ",
  "HCI_BLE_CONNECTION_IQ_REPORT_EVENT                ",
  "HCI_BLE_CTE_REQUEST_FAILED_EVENT                  ",
  "HCI_BLE_SCAN_REQ_REPORT_EVENT                     ",
  "HCI_BLE_EXT_CONNECTION_IQ_REPORT_EVENT            ",
  "HCI_BLE_CHANNEL_MAP_UPDATE_EVENT                  ",
};

/*******************************************************************************
 * EXTERNS
 */

extern uint8 hciPTMenabled;

/*
** Internal Functions
*/

/*******************************************************************************
 * @fn          hciInitEventMasks
 *
 * @brief       This routine initializes Bluetooth and BLE event makss to their
 *              default values.
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
void hciInitEventMasks( void )
{
  // set default Bluetooth event mask bits
  pHciEvtMask[0] = BT_EVT_MASK_BYTE0;
  pHciEvtMask[1] = BT_EVT_MASK_BYTE1;
  pHciEvtMask[2] = BT_EVT_MASK_BYTE2;
  pHciEvtMask[3] = BT_EVT_MASK_BYTE3;
  pHciEvtMask[4] = BT_EVT_MASK_BYTE4;
  pHciEvtMask[5] = BT_EVT_MASK_BYTE5;
  pHciEvtMask[6] = BT_EVT_MASK_BYTE6;
  pHciEvtMask[7] = BT_EVT_MASK_BYTE7;

  // set default Bluetooth event mask page 2 bits
  pHciEvtMask2[0] = BT_EVT_MASK2_BYTE0;
  pHciEvtMask2[1] = BT_EVT_MASK2_BYTE1;
  pHciEvtMask2[2] = BT_EVT_MASK2_BYTE2;
  pHciEvtMask2[3] = BT_EVT_MASK2_BYTE3;
  pHciEvtMask2[4] = BT_EVT_MASK2_BYTE4;
  pHciEvtMask2[5] = BT_EVT_MASK2_BYTE5;
  pHciEvtMask2[6] = BT_EVT_MASK2_BYTE6;
  pHciEvtMask2[7] = BT_EVT_MASK2_BYTE7;

  // set default BLE event mask bits
  pBleEvtMask[0] = LE_EVT_MASK_BYTE0;
  pBleEvtMask[1] = LE_EVT_MASK_BYTE1;
  pBleEvtMask[2] = LE_EVT_MASK_BYTE2;
  pBleEvtMask[3] = LE_EVT_MASK_NONE;
  pBleEvtMask[4] = LE_EVT_MASK_NONE;
  pBleEvtMask[5] = LE_EVT_MASK_NONE;
  pBleEvtMask[6] = LE_EVT_MASK_NONE;
  pBleEvtMask[7] = LE_EVT_MASK_NONE;

  return;
}


/*
** HCI Events
*/

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This function sends the Data Buffer Overflow Event to the Host.
 *
 * Public function defined in hci_c_event.h.
 */
void HCI_DataBufferOverflowEvent( uint8 linkType )
{
  hciPacket_t *msg;
  uint8       totalLength;

  // OSAL message header + HCI event header + parameters
  totalLength = sizeof( hciPacket_t ) +
                HCI_EVENT_MIN_LENGTH +
                HCI_BUFFER_OVERFLOW_EVENT_LEN;

  msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

  if (msg)
  {
    // create message header
    msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
    msg->hdr.status = 0xFF;

    // create event header
    msg->pData    = (uint8*)(msg+1);
    msg->pData[0] = HCI_EVENT_PACKET;
    msg->pData[1] = HCI_DATA_BUFFER_OVERFLOW_EVENT;
    msg->pData[2] = HCI_BUFFER_OVERFLOW_EVENT_LEN;

    // Link Type
    msg->pData[3] = linkType;

    // send message
    (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
  }
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This function sends the Number of Completed Packets Event to the Host.
 *
 * Public function defined in hci_c_event.h.
 */
void HCI_NumOfCompletedPacketsEvent( uint8   numHandles,
                                     uint16 *handles,
                                     uint16 *numCompletedPkts )
{
  // check if this is for the Host
  if ( hciL2capTaskID != 0 )
  {
    hciEvt_NumCompletedPkt_t *pkt =
      (hciEvt_NumCompletedPkt_t *)MAP_osal_msg_allocate( sizeof(hciEvt_NumCompletedPkt_t) +
                                                     (numHandles * 2 * sizeof(uint16)) );
    if ( pkt )
    {
      pkt->hdr.event = HCI_DATA_EVENT; // packet type
      pkt->hdr.status = HCI_NUM_OF_COMPLETED_PACKETS_EVENT_CODE; // event code
      pkt->numHandles = numHandles;
      pkt->pConnectionHandle = (uint16 *)(pkt+1);
      pkt->pNumCompletedPackets = (uint16 *)( (uint8 *)(pkt+1) + ( numHandles * sizeof(uint16) ) );

      // for each handle, there's a handle number and a number of
      // completed packets for that handle
      for ( uint8 i = 0; i < numHandles; i++ )
      {
        pkt->pConnectionHandle[i] = handles[i];
        pkt->pNumCompletedPackets[i] = numCompletedPkts[i];
      }

      (void)MAP_osal_msg_send( hciL2capTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // the data length
    // ALT: Use 1+(numHandles * HCI_NUM_COMPLETED_PACKET_EVENT_LEN-1).
    dataLength = HCI_NUM_COMPLETED_PACKET_EVENT_LEN +
      ((numHandles-1) * (HCI_NUM_COMPLETED_PACKET_EVENT_LEN-1));

    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

    // allocate memory for OSAL hdr + packet
    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if ( msg )
    {
      uint8 i;

      // OSAL header
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // build event packet
      msg->pData    = (uint8 *)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;                        // packet type
      msg->pData[1] = HCI_NUM_OF_COMPLETED_PACKETS_EVENT_CODE; // event code
      msg->pData[2] = dataLength;
      msg->pData[3] = numHandles;

      // for each handle, there's a number handle number and a number of
      // completed packets for that handle
      for (i=0; i<numHandles; i++)
      {
        msg->pData[4+(4*i)] = LO_UINT16(handles[i]);
        msg->pData[5+(4*i)] = HI_UINT16(handles[i]);
        msg->pData[6+(4*i)] = LO_UINT16(numCompletedPkts[i]);
        msg->pData[7+(4*i)] = HI_UINT16(numCompletedPkts[i]);
      }

      // send message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * This function sends a Command Complete Event to the Host.
 *
 * Public function defined in hci_c_event.h.
 */
void HCI_CommandCompleteEvent( uint16 opcode,
                               uint8  numParam,
                               uint8  *param )
{
  // check if this is for the Host
  if ( ((hciGapTaskID != 0) || (hciSmpTaskID != 0)) && (hciPTMenabled == FALSE) )
  {
    hciEvt_CmdComplete_t *pkt =
      (hciEvt_CmdComplete_t *)MAP_osal_msg_allocate( sizeof(hciEvt_CmdComplete_t) +
                                                 numParam );

    if ( pkt )
    {
      uint8 taskID;

      if ( (opcode == HCI_LE_RAND || opcode == HCI_LE_ENCRYPT) && (hciSmpTaskID) )
      {
        taskID         = hciSmpTaskID;
        pkt->hdr.event = HCI_SMP_EVENT_EVENT;
      }
      else
      {
        taskID         = hciGapTaskID;
        pkt->hdr.event = HCI_GAP_EVENT_EVENT;
      }
      pkt->hdr.status   = HCI_COMMAND_COMPLETE_EVENT_CODE;
      pkt->numHciCmdPkt = 1;
      pkt->cmdOpcode    = opcode;
      pkt->pReturnParam = (uint8 *)(pkt+1);

      (void)MAP_osal_memcpy( pkt->pReturnParam, param, numParam );

      (void)MAP_osal_msg_send( taskID, (uint8 *)pkt );
    }
  }
  else
  {
    MAP_HCI_SendCommandCompleteEvent( HCI_COMMAND_COMPLETE_EVENT_CODE,
                                      opcode,
                                      numParam,
                                      param );
  }
}


#if defined(BLE_VS_FEATURES) && (BLE_VS_FEATURES & SCAN_REQ_RPT_CFG) &&        \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
/*******************************************************************************
 * @fn          LL_EXT_ScanReqReportCback Callback
 *
 * @brief       This LL callback is used to generate a vendor specific Scan
 *              Request Report meta event when an Scan Request is received by
 *              an Advertiser.
 *
 * input parameters
 *
 * @param       peerAddrType - Peer address type.
 * @param       peerAddr     - Peer address.
 * @param       chan         - BLE channel of report.
 * @param       rssi         - The RSSI of received packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_EXT_ScanReqReportCback( uint8  peerAddrType,
                                uint8 *peerAddr,
                                uint8  chan,
                                int8   rssi )
{
  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEScanReqReport_t *pkt;

    pkt = (hciEvt_BLEScanReqReport_t *)MAP_osal_msg_allocate( sizeof(hciEvt_BLEScanReqReport_t) );

    if ( pkt )
    {
      pkt->hdr.event  = HCI_GAP_EVENT_EVENT;
      pkt->hdr.status = HCI_LE_EVENT_CODE;

      pkt->BLEEventCode = HCI_BLE_SCAN_REQ_REPORT_EVENT;
      pkt->eventType    = 0; // Scan Request is the only packet supported

      pkt->peerAddrType = peerAddrType;
      (void)MAP_osal_memcpy( pkt->peerAddr, peerAddr, B_ADDR_LEN );

      pkt->bleChan = chan;
      pkt->rssi    = rssi;

      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    hciPacket_t *msg;

    // OSAL message header + HCI event header + data
    msg = (hciPacket_t *)MAP_osal_msg_allocate( sizeof(hciPacket_t)  +
                                                HCI_EVENT_MIN_LENGTH +
                                                HCI_SCAN_REQ_REPORT_EVENT_LEN );
    if (msg)
    {
      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData    = (uint8*)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;
      msg->pData[1] = HCI_LE_EVENT_CODE;
      msg->pData[2] = HCI_SCAN_REQ_REPORT_EVENT_LEN;

      // populate event
      msg->pData[3]  = HCI_BLE_SCAN_REQ_REPORT_EVENT;                 // event code
      msg->pData[4]  = LL_ADV_RPT_SCAN_REQ;                           // advertisement event type
      msg->pData[5]  = peerAddrType;                                  // peer address type
      (void)MAP_osal_memcpy (&msg->pData[6], peerAddr, B_ADDR_LEN);   // peer address
      msg->pData[12] = chan;                                          // channel
      msg->pData[13] = rssi;                                          // RSSI

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}
#endif // SCAN_REQ_RPT_CFG & (ADV_NCONN_CFG | ADV_CONN_CFG)

/*******************************************************************************
 * @fn          LL_EXT_ChanMapUpdateCback Callback
 *
 * @brief       This LL callback is used to generate a vendor specific channel map
 *              update event
 *
 * input parameters
 *
 * @param       connHandle - connection for which channel map was updated
 * @param       newChanMap - new channel map
 * @param       nextDataChan - the next channel we will be using
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_EXT_ChanMapUpdateCback(uint16 connHandle, uint8 *newChanMap, uint8 nextDataChan)
{
  // check if this is for the Host
  if (hciGapTaskID != 0)
  {
    hciEvt_BLEChanMapUpdate_t *pkt;

    pkt = (hciEvt_BLEChanMapUpdate_t *)MAP_osal_msg_allocate(sizeof(hciEvt_BLEChanMapUpdate_t));

    if ( pkt )
    {
      pkt->hdr.event  = HCI_GAP_EVENT_EVENT;
      pkt->hdr.status = HCI_LE_EVENT_CODE;
      pkt->BLEEventCode = HCI_BLE_CHANNEL_MAP_UPDATE_EVENT;

      pkt->connHandle = connHandle;
      pkt->nextDataChan = nextDataChan;

      (void)MAP_osal_memcpy(pkt->newChanMap, newChanMap, LL_NUM_BYTES_FOR_CHAN_MAP);

      (void)MAP_osal_msg_send(hciGapTaskID, (uint8 *)pkt);
    }
  }
  else
  {
    hciPacket_t *msg;

    // OSAL message header + HCI event header + data
    msg = (hciPacket_t *)MAP_osal_msg_allocate( sizeof(hciPacket_t)  +
                                                HCI_EVENT_MIN_LENGTH +
                                                HCI_BLE_CHANNEL_MAP_UPDATE_EVENT_LEN );
    if (msg)
    {
      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData    = (uint8*)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;
      msg->pData[1] = HCI_LE_EVENT_CODE;
      msg->pData[2] = HCI_BLE_CHANNEL_MAP_UPDATE_EVENT_LEN;

      // populate event
      msg->pData[3]  = HCI_BLE_CHANNEL_MAP_UPDATE_EVENT;                             // event code
      (void)MAP_osal_memcpy (&msg->pData[4], &connHandle, sizeof(connHandle));       // connHandle
      msg->pData[6] = nextDataChan;                                                  // next data channel
      (void)MAP_osal_memcpy (&msg->pData[7], newChanMap, LL_NUM_BYTES_FOR_CHAN_MAP); // new channel map

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}

/*******************************************************************************
 * This function sends a Vendor Specific Command Complete Event to the Host.
 *
 * Public function defined in hci_c_event.h.
 */
void HCI_VendorSpecifcCommandCompleteEvent( uint16  opcode,
                                            uint8   numParam,
                                            uint8  *param )
{
  // check if this is for the Host
  if ( (hciGapTaskID != 0) && (hciPTMenabled == FALSE) )
  {
    hciEvt_VSCmdComplete_t *pkt =
      (hciEvt_VSCmdComplete_t *)MAP_osal_msg_allocate( sizeof(hciEvt_VSCmdComplete_t) +
                                                   numParam );

    if ( pkt )
    {
      pkt->hdr.event   = HCI_GAP_EVENT_EVENT;
      pkt->hdr.status  = HCI_VE_EVENT_CODE;
      pkt->length      = numParam;
      pkt->cmdOpcode   = opcode;
      pkt->pEventParam = (uint8 *)(pkt+1);

      (void)MAP_osal_memcpy( pkt->pEventParam, param, numParam );

      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    MAP_HCI_SendCommandCompleteEvent( HCI_VE_EVENT_CODE,
                                      opcode,
                                      numParam,
                                      param );
  }
}


/*******************************************************************************
 * This function sends a Command Status Event to the Host.
 *
 * Public function defined in hci_c_event.h.
 */
void HCI_CommandStatusEvent( hciStatus_t status,
                             uint16      opcode )
{
  // check if this is for the Host
  if ( (hciGapTaskID != 0) && (hciPTMenabled == FALSE) )
  {
    hciEvt_CommandStatus_t *pMsg;
    uint8 totalLength;

    totalLength = sizeof(hciEvt_CommandStatus_t);

    pMsg = (hciEvt_CommandStatus_t *)MAP_osal_msg_allocate( totalLength );

    if ( pMsg )
    {
      // message type, HCI event type
      pMsg->hdr.event = HCI_GAP_EVENT_EVENT;

      // use the OSAL status field for HCI event code
      pMsg->hdr.status   = HCI_COMMAND_STATUS_EVENT_CODE;
      pMsg->cmdStatus    = status;
      pMsg->numHciCmdPkt = 1;
      pMsg->cmdOpcode    = opcode;

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)pMsg );
    }
  }
  else
  {
    MAP_HCI_SendCommandStatusEvent( HCI_COMMAND_STATUS_EVENT_CODE,
                                    status,
                                    opcode );
  }
}


/*******************************************************************************
 * This function sends a Hardware Error Event to the Host.
 *
 * Public function defined in hci_c_event.h.
 */
void HCI_HardwareErrorEvent( uint8 hwErrorCode )
{
  // check the event mask to see if this event has been masked by Host
  if ( (BT_EVT_MASK_HARDWARE_ERROR & pHciEvtMask[BT_EVT_INDEX_HARDWARE_ERROR]) == 0 )
  {
    return;
  }

  // check if this is for the Host
  if ( (hciGapTaskID != 0) && (hciPTMenabled == FALSE) )
  {
    hciEvt_HardwareError_t *pMsg;
    uint8 totalLength;

    totalLength = sizeof(hciEvt_HardwareError_t);

    pMsg = (hciEvt_HardwareError_t *)MAP_osal_msg_allocate( totalLength );

    if ( pMsg )
    {
      // message type, HCI event type
      pMsg->hdr.event = HCI_GAP_EVENT_EVENT;

      // use the OSAL status field for HCI event code
      pMsg->hdr.status   = HCI_BLE_HARDWARE_ERROR_EVENT_CODE;
      pMsg->hardwareCode = hwErrorCode;

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)pMsg );
    }
  }
  else // TL present
  {
    hciPacket_t *msg;

    // OSAL message header + HCI event header + data length of one
    msg = (hciPacket_t *)MAP_osal_msg_allocate( sizeof(hciPacket_t)  +
                                            HCI_EVENT_MIN_LENGTH +
                                            1 );

    // send event (if we have the memory)
    if (msg)
    {
      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData    = (uint8*)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;
      msg->pData[1] = HCI_BLE_HARDWARE_ERROR_EVENT_CODE;
      msg->pData[2] = 1; // data length

      // error code
      msg->pData[3] = hwErrorCode;

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }

  return;
}


/*******************************************************************************
 * This generic function sends a Command Complete or a Vendor Specific Command
 * Complete Event to the Host.
 *
 * Public function defined in hci_c_event.h.
 */
void HCI_SendCommandStatusEvent ( uint8  eventCode,
                                  uint16 status,
                                  uint16 opcode )
{
  uint8 data[4];

  data[0] = status;
  data[1] = 1;                 // number of HCI command packets
  data[2] = LO_UINT16(opcode); // opcode (LSB)
  data[3] = HI_UINT16(opcode); // opcode (MSB)

  MAP_HCI_SendControllerToHostEvent( eventCode, 4, data );
}


/*******************************************************************************
 * This generic function sends a Command Complete or a Vendor Specific Command
 * Complete Event to the Host.
 *
 * Public function defined in hci_c_event.h.
 */
void HCI_SendCommandCompleteEvent ( uint8  eventCode,
                                    uint16 opcode,
                                    uint8  numParam,
                                    uint8  *param )
{
  hciPacket_t *msg;
  uint8        totalLength;

  // The initial length will be:
  // OSAL message header(4) - not part of packet sent to HCI Host!
  // Minimum Event Data: Packet Type(1) + Event Code(1) + Length(1)
  // Return Parameters (0..N)
  totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + numParam;

  // adjust the size of the event packet based on event code
  // Note: If not a vendor specific event, then the event includes:
  //       Command Complete Data: Number of HCI Commands Allowed(1) + Command Opcode(2)
  // Note: If a vendor specific event, then the event includes:
  //       Vendor Specific Command Complete Data: Vendor Specific Event Opcode(2)
  totalLength += ( (eventCode != HCI_VE_EVENT_CODE)  ?
                   HCI_CMD_COMPLETE_EVENT_LEN        :
                   HCI_CMD_VS_COMPLETE_EVENT_LEN );

  // allocate memory for OSAL hdr + packet
  msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

  if ( msg )
  {
    // OSAL message event, status, and pointer to packet
    msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
    msg->hdr.status = 0xFF;
    msg->pData      = (uint8*)(msg+1);

    // fill in Command Complete Event data
    msg->pData[0] = HCI_EVENT_PACKET;
    msg->pData[1] = eventCode;

    // check if this isn't a vendor specific event
    if ( eventCode != HCI_VE_EVENT_CODE )
    {
      msg->pData[2] = numParam + HCI_CMD_COMPLETE_EVENT_LEN;
      msg->pData[3] = 1;                   // event parameter 1
      msg->pData[4] = LO_UINT16( opcode ); // event parameter 2
      msg->pData[5] = HI_UINT16( opcode ); // event parameter 2

      // remaining event parameters
      (void)MAP_osal_memcpy (&msg->pData[6], param, numParam);
    }
    else // it is a vendor specific event
    {
      // less one byte as number of complete packets not used in vendor specific event
      msg->pData[2] = numParam + HCI_CMD_VS_COMPLETE_EVENT_LEN;
      msg->pData[3] = param[0];            // event parameter 0: event opcode LSB
      msg->pData[4] = param[1];            // event parameter 1: event opcode MSB
      msg->pData[5] = param[2];            // event parameter 2: status
      msg->pData[6] = LO_UINT16( opcode ); // event parameter 3: command opcode LSB
      msg->pData[7] = HI_UINT16( opcode ); // event parameter 3: command opcode MSB

      // remaining event parameters
      // Note: The event opcode and status were already placed in the msg packet.
      (void)MAP_osal_memcpy (&msg->pData[8], &param[3], numParam-HCI_EVENT_MIN_LENGTH);
    }

    // send the message
    (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
  }
}


/*******************************************************************************
 * This is a generic function used to send events from the Controller to the
 * Host.
 *
 * Public function defined in hci_c_event.h.
 */
void HCI_SendControllerToHostEvent( uint8 eventCode,
                                    uint8 dataLen,
                                    uint8 *pData )
{
  hciPacket_t *msg;
  uint8 totalLength;

  // OSAL message header + HCI event header + data
  totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLen;

  // allocate memory for OSAL hdr + packet
  msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

  if ( msg )
  {
    // message type, HCI event type
    msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
    msg->hdr.status = 0xFF;

    // packet
    msg->pData    = (uint8*)(msg+1);
    msg->pData[0] = HCI_EVENT_PACKET;
    msg->pData[1] = eventCode;
    msg->pData[2] = dataLen;

    // copy data
    if ( dataLen )
    {
      (void)MAP_osal_memcpy( &(msg->pData[3]), pData, dataLen );
    }

    // send message
    (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
  }
}


/*******************************************************************************
 * @fn          LL_AuthPayloadTimeoutExpiredCback Callback
 *
 * @brief       This LL callback is used to generate an Authenticated Payload
 *              Timeout event when the APTO expires.
 *
 * input parameters
 *
 * @param       connHandle - Connection handle.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_AuthPayloadTimeoutExpiredCback( uint16 connHandle )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pHciEvtMask2[BT_EVT_INDEX2_APTO_EXPIRED] & BT_EVT_MASK2_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED) == 0) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_AptoExpired_t *pkt =
      (hciEvt_AptoExpired_t *)MAP_osal_msg_allocate( sizeof(hciEvt_AptoExpired_t) +
                                                     sizeof(uint16) );
    if ( pkt )
    {
      pkt->hdr.event  = HCI_DATA_EVENT;              // packet type
      pkt->hdr.status = HCI_APTO_EXPIRED_EVENT_CODE; // event code
      pkt->connHandle = connHandle;

      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    hciPacket_t *msg;

    // OSAL message header + HCI event header + data

    // allocate memory for OSAL hdr + packet
    msg = (hciPacket_t *)MAP_osal_msg_allocate( sizeof(hciPacket_t)  +
                                            HCI_EVENT_MIN_LENGTH +
                                            HCI_APTO_EXPIRED_EVENT_LEN );

    if ( msg )
    {
      // OSAL header
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // build event packet
      msg->pData    = (uint8 *)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;            // packet type
      msg->pData[1] = HCI_APTO_EXPIRED_EVENT_CODE; // event code
      msg->pData[2] = HCI_APTO_EXPIRED_EVENT_LEN;
      msg->pData[3] = LO_UINT16( connHandle );
      msg->pData[4] = HI_UINT16( connHandle );

      // send message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          LL_RemoteConnParamReqCback Callback
 *
 * @brief       This LL callback is used to generate a Remote Connection
 *              Parameter Request meta event to provide to the Host the peer's
 *              connection parameter request parameters (min connection
 *              interval, max connection interval, slave latency, and connection
 *              timeout), and to request the Host's acceptance or rejection of
 *              this parameters.
 *
 * input parameters
 *
 * @param       connHandle   - Connection handle.
 * @param       Interval_Min - Lower limit for connection interval.
 * @param       Interval_Max - Upper limit for connection interval.
 * @param       Latency      - Slave latency.
 * @param       Timeout      - Connection timeout.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_RemoteConnParamReqCback( uint16 connHandle,
                                 uint16 Interval_Min,
                                 uint16 Interval_Max,
                                 uint16 Latency,
                                 uint16 Timeout )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pBleEvtMask[LE_EVT_INDEX_REMOTE_CONN_PARAM_REQUEST] & LE_EVT_MASK_REMOTE_CONN_PARAM_REQUEST) == 0) )
  {
    // the event mask is not set for this event, so accept as is
    MAP_LL_RemoteConnParamReqReply( connHandle,
                                    Interval_Min,
                                    Interval_Max,
                                    Latency,
                                    Timeout,
                                    0,
                                    0 );

    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLERemoteConnParamReq_t *msg;
    uint8 totalLength;

    totalLength = sizeof( hciEvt_BLERemoteConnParamReq_t );

    msg = (hciEvt_BLERemoteConnParamReq_t *)MAP_osal_msg_allocate(totalLength);

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_GAP_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

      // event packet
      msg->BLEEventCode = HCI_BLE_REMOTE_CONN_PARAM_REQUEST_EVENT;
      msg->status       = HCI_SUCCESS;
      msg->connHandle   = connHandle;
      msg->Interval_Min = Interval_Min;
      msg->Interval_Max = Interval_Max;
      msg->Latency      = Latency;
      msg->Timeout      = Timeout;

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_REMOTE_CONNECTION_PARAMETER_REQUEST_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_REMOTE_CONN_PARAM_REQUEST_EVENT;  // event code
      *pBuf++ = LO_UINT16(connHandle);           // connection handle (LSB)
      *pBuf++ = HI_UINT16(connHandle);           // connection handle (MSB)
      *pBuf++ = LO_UINT16(Interval_Min);         // connection interval (LSB)
      *pBuf++ = HI_UINT16(Interval_Min);         // connection interval (MSB)
      *pBuf++ = LO_UINT16(Interval_Max);         // connection interval (LSB)
      *pBuf++ = HI_UINT16(Interval_Max);         // connection interval (MSB)
      *pBuf++ = LO_UINT16(Latency);              // slave latency (LSB)
      *pBuf++ = HI_UINT16(Latency);              // slave latency (MSB)
      *pBuf++ = LO_UINT16(Timeout);              // connection timeout (LSB)
      *pBuf++ = HI_UINT16(Timeout);              // connection timeout (MSB)

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          LL_PhyUpdateCompleteEventCback Callback
 *
 * @brief       This LL callback is used to indicate that the Controller has
 *              has changed the transmitter or receiver PHY in use or that the
 *              LL_SetPhy command has failed.
 *
 * input parameters
 *
 * @param       status     - Status of LL_SetPhy command.
 * @param       connHandle - Connection handle.
 * @param       txPhy      - Bit map of PHY used for Tx.
 * @param       rxPhy      - Bit map of PHY used for Rx.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_PhyUpdateCompleteEventCback( llStatus_t status,
                                     uint16     connHandle,
                                     uint8      txPhy,
                                     uint8      rxPhy )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pBleEvtMask[LE_EVT_INDEX_PHY_UPDATE_COMPLETE] & LE_EVT_MASK_PHY_UPDATE_COMPLETE) == 0) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // Qualification fix - Bit map of PHY could not be 0
  if ((status != LL_STATUS_SUCCESS) && (txPhy == 0) && (rxPhy == 0))
  {
    txPhy = LL_PHY_1_MBPS;
    rxPhy = LL_PHY_1_MBPS;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEPhyUpdateComplete_t *msg =
      (hciEvt_BLEPhyUpdateComplete_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLEPhyUpdateComplete_t ));

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_GAP_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

      // event packet
      msg->BLEEventCode = HCI_BLE_PHY_UPDATE_COMPLETE_EVENT;
      msg->status       = status;
      msg->connHandle   = connHandle;
      msg->txPhy        = LL_ConvertPhy(txPhy);
      msg->rxPhy        = LL_ConvertPhy(rxPhy);

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_PHY_UPDATE_COMPLETE_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_PHY_UPDATE_COMPLETE_EVENT;  // event code
      *pBuf++ = status;                             // status
      *pBuf++ = LO_UINT16(connHandle);              // connection handle (LSB)
      *pBuf++ = HI_UINT16(connHandle);              // connection handle (MSB)
      *pBuf++ = LL_ConvertPhy(txPhy);               // TX PHY
      *pBuf++ = LL_ConvertPhy(rxPhy);               // RX PHY

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}
#endif // ADV_CONN_CFG | INIT_CFG
#endif // PHY_2MBPS_CFG | PHY_LR_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          LL_DataLengthChangeEventCback Callback
 *
 * @brief       This LL callback is used to indicate that the Controller has
 *              has changed the either the maximum payload length or the
 *              maximum transmit time of data channel PDUs in either direction.
 *              The values reported are the maximum taht will actually be used
 *              on the connection following the change.
 *
 * input parameters
 *
 * @param       connHandle  - Connection handle.
 * @param       maxTxOctets - Maximum number of transmit payload bytes.
 * @param       maxTxTime   - Maximum transmit time.
 * @param       maxRxOctets - Maximum number of receive payload bytes.
 * @param       maxRxTime   - Maximum receive time.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_DataLengthChangeEventCback( uint16 connHandle,
                                    uint16 maxTxOctets,
                                    uint16 maxTxTime,
                                    uint16 maxRxOctets,
                                    uint16 maxRxTime )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pBleEvtMask[LE_EVT_INDEX_DATA_LENGTH_CHANGE] & LE_EVT_MASK_DATA_LENGTH_CHANGE) == 0) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEDataLengthChange_t *msg =
      (hciEvt_BLEDataLengthChange_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLEDataLengthChange_t ));

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_GAP_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

      // event packet
      msg->BLEEventCode = HCI_BLE_DATA_LENGTH_CHANGE_EVENT;
      msg->connHandle   = connHandle;
      msg->maxTxOctets  = maxTxOctets;
      msg->maxTxTime    = maxTxTime;
      msg->maxRxOctets  = maxRxOctets;
      msg->maxRxTime    = maxRxTime;

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_DATA_LENGTH_CHANGE_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_DATA_LENGTH_CHANGE_EVENT;   // event code
      *pBuf++ = LO_UINT16(connHandle);              // connection handle (LSB)
      *pBuf++ = HI_UINT16(connHandle);              // connection handle (MSB)
      *pBuf++ = LO_UINT16(maxTxOctets);             // max Tx bytes (LSB)
      *pBuf++ = HI_UINT16(maxTxOctets);             // max Tx bytes (MSB)
      *pBuf++ = LO_UINT16(maxTxTime);               // max Tx time (LSB)
      *pBuf++ = HI_UINT16(maxTxTime);               // max Tx time (MSB)
      *pBuf++ = LO_UINT16(maxRxOctets);             // max Rx bytes (LSB)
      *pBuf++ = HI_UINT16(maxRxOctets);             // max Rx bytes (MSB)
      *pBuf++ = LO_UINT16(maxRxTime);               // max Rx time (LSB)
      *pBuf++ = HI_UINT16(maxRxTime);               // max Rx time (MSB)

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * @fn          LL_ReadLocalP256PublicKeyCompleteEventCback Callback
 *
 * @brief       This LL callback is used to indicate the Controller has
 *              completed the generation of the P256 public key.
 *
 * @param       None.
 *
 * input parameters
 *
 * output parameters
 *
 * @param       status   - Operation status.
 * @param       p256KeyX - P256 public key (first 32 bytes X=0..31).
 * @param       p256KeyY - P256 public key (second 32 bytes Y=32..63).
 *
 * @return      None.
 */
void LL_ReadLocalP256PublicKeyCompleteEventCback( uint8  status,
                                                  uint8 *p256KeyX,
                                                  uint8 *p256KeyY )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pBleEvtMask[LE_EVT_INDEX_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE] & LE_EVT_MASK_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE) == 0) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // check if this is for the Host
  if ( hciSmpTaskID != 0 )
  {
    hciEvt_BLEReadP256PublicKeyComplete_t *msg =
      (hciEvt_BLEReadP256PublicKeyComplete_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLEReadP256PublicKeyComplete_t ));

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_SMP_META_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE;

      // event packet
      msg->BLEEventCode = HCI_BLE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT;
      msg->status       = status;

      if ( status == HCI_SUCCESS )
      {
        // copy the P256 key
        (void)MAP_osal_memcpy( &msg->p256Key[0],  p256KeyX, LL_SC_P256_KEY_LEN/2 );
        (void)MAP_osal_memcpy( &msg->p256Key[32], p256KeyY, LL_SC_P256_KEY_LEN/2 );
      }
      else // clear the key to avoid confusion
      {
        (void)MAP_osal_memset( msg->p256Key, 0, LL_SC_P256_KEY_LEN );
      }

      // send the message
      (void)MAP_osal_msg_send( hciSmpTaskID, (uint8 *)msg );
    }
  }
  else // Host not present
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT;   // event code
      *pBuf++ = status;

      if ( status == HCI_SUCCESS )
      {
        // copy the P256 key
        (void)MAP_osal_memcpy( &pBuf[0],  p256KeyX, LL_SC_P256_KEY_LEN/2 );
        (void)MAP_osal_memcpy( &pBuf[32], p256KeyY, LL_SC_P256_KEY_LEN/2 );
      }
      else // clear the key to avoid confusion
      {
        (void)MAP_osal_memset( pBuf, 0, LL_SC_P256_KEY_LEN );
      }

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
    else // out of heap!
    {
      MAP_HCI_HardwareErrorEvent( HCI_ERROR_CODE_MEM_CAP_EXCEEDED );
    }
  }
}


/*******************************************************************************
 * @fn          LL_GenerateDHKeyCompleteEventCback Callback
 *
 * @brief       This LL callback is used to indicate teh Controller has
 *              completed the generation of the Diffie Hellman key.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       status - Operation status.
 * @param       dhKey  - Diffie Hellman key (32 bytes).
 *
 * @return      None.
 */
void LL_GenerateDHKeyCompleteEventCback( uint8  status,
                                         uint8 *dhKey )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pBleEvtMask[LE_EVT_INDEX_GENERATE_DHKEY_COMPLETE] & LE_EVT_MASK_GENERATE_DHKEY_COMPLETE) == 0) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // check if this is for the Host
  if ( hciSmpTaskID != 0 )
  {
    hciEvt_BLEGenDHKeyComplete_t *msg =
      (hciEvt_BLEGenDHKeyComplete_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLEGenDHKeyComplete_t ));

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_SMP_META_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE;

      // event packet
      msg->BLEEventCode = HCI_BLE_GENERATE_DHKEY_COMPLETE_EVENT;
      msg->status       = status;

      if ( status == HCI_SUCCESS )
      {
        // copy the DH key
        (void)MAP_osal_memcpy( &msg->dhKey, dhKey, LL_SC_DHKEY_LEN );
      }
      else // clear the key to avoid confusion
      {
        (void)MAP_osal_memset( &msg->dhKey, 0, LL_SC_DHKEY_LEN );
      }

      // send the message
      (void)MAP_osal_msg_send( hciSmpTaskID, (uint8 *)msg );
    }
  }
  else // Host not present
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_GENERATE_DHKEY_COMPLETE_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_GENERATE_DHKEY_COMPLETE_EVENT;   // event code
      *pBuf++ = status;

      if ( status == HCI_SUCCESS )
      {
        // copy the DH key
        (void)MAP_osal_memcpy( pBuf, dhKey, LL_SC_DHKEY_LEN );
      }
      else // clear the key to avoid confusion
      {
        (void)MAP_osal_memset( pBuf, 0, LL_SC_DHKEY_LEN );
      }

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
    else // out of heap!
    {
      MAP_HCI_HardwareErrorEvent( HCI_ERROR_CODE_MEM_CAP_EXCEEDED );
    }
  }
}


/*******************************************************************************
 * @fn          LL_EnhancedConnectionCompleteCback Callback
 *
 * @brief       This LL callback is used to generate an Enhanced Connection
 *              Complete meta event when a connection is established by either
 *              an Advertiser or an Initiator.
 *
 * input parameters
 *
 * @param       reasonCode    - Status of connection complete.
 * @param       connHandle    - Connection handle.
 * @param       role          - Connection formed as Master or Slave.
 * @param       peerAddrType  - Peer address as Public or Random.
 * @param       peerAddr      - Pointer to peer device address.
 * @param       localRPA      - Pointer to local RPA.
 * @param       peerRPA       - Pointer to peer RPA.
 * @param       connInterval  - Connection interval.
 * @param       slaveLatency  - Slave latency.
 * @param       connTimeout   - Connection timeout.
 * @param       clockAccuracy - Sleep clock accuracy (from Master only).
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_EnhancedConnectionCompleteCback( uint8   reasonCode,
                                         uint16  connHandle,
                                         uint8   role,
                                         uint8   peerAddrType,
                                         uint8  *peerAddr,
                                         uint8  *localRPA,
                                         uint8  *peerRPA,
                                         uint16  connInterval,
                                         uint16  slaveLatency,
                                         uint16  connTimeout,
                                         uint8   clockAccuracy )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( !(pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) ||
       (!(pBleEvtMask[LE_EVT_INDEX_CONN_COMPLETE] & LE_EVT_MASK_CONN_COMPLETE) &&
        !(pBleEvtMask[LE_EVT_INDEX_ENH_CONN_COMPLETE] & LE_EVT_MASK_ENH_CONN_COMPLETE)) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEEnhConnComplete_t *pkt;

    pkt = (hciEvt_BLEEnhConnComplete_t *)MAP_osal_msg_allocate( sizeof(hciEvt_BLEEnhConnComplete_t) );

    if ( pkt )
    {
      pkt->hdr.event  = HCI_GAP_EVENT_EVENT;
      pkt->hdr.status = HCI_LE_EVENT_CODE;

      if ( reasonCode == LL_STATUS_SUCCESS )
      {
        pkt->status = HCI_SUCCESS;
        (void)MAP_osal_memcpy( pkt->peerAddr, peerAddr, B_ADDR_LEN );
      }
      else
      {
        pkt->status = bleGAPConnNotAcceptable;
        (void)MAP_osal_memset( pkt->peerAddr, 0, B_ADDR_LEN );
      }

      if ( pBleEvtMask[LE_EVT_INDEX_ENH_CONN_COMPLETE] & LE_EVT_MASK_ENH_CONN_COMPLETE )
      {
        pkt->BLEEventCode = HCI_BLE_ENHANCED_CONNECTION_COMPLETE_EVENT;

        // local and peer RPA
        (localRPA != NULL) ? MAP_osal_memcpy( pkt->localRPA, localRPA, B_ADDR_LEN ) :
                             MAP_osal_memset( pkt->localRPA, 0, B_ADDR_LEN );

        (peerRPA != NULL)  ? MAP_osal_memcpy( pkt->peerRPA, peerRPA, B_ADDR_LEN ) :
                             MAP_osal_memset( pkt->peerRPA, 0, B_ADDR_LEN );

        pkt->peerAddrType = peerAddrType;
      }
      else // LE_EVT_MASK_CONN_COMPLETE
      {
        pkt->BLEEventCode = HCI_BLE_CONNECTION_COMPLETE_EVENT;
        MAP_osal_memset( pkt->localRPA, 0, B_ADDR_LEN );
        MAP_osal_memset( pkt->peerRPA, 0, B_ADDR_LEN );
        pkt->peerAddrType = peerAddrType & LL_DEV_ADDR_TYPE_MASK;
      }

      pkt->connectionHandle = connHandle;
      pkt->role             = role;
      pkt->connInterval     = connInterval;
      pkt->connLatency      = slaveLatency;
      pkt->connTimeout      = connTimeout;
      pkt->clockAccuracy    = clockAccuracy;

      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    if ( pBleEvtMask[LE_EVT_INDEX_ENH_CONN_COMPLETE] & LE_EVT_MASK_ENH_CONN_COMPLETE )
    {
      dataLength = HCI_ENH_CONNECTION_COMPLETE_EVENT_LEN;
    }
    else // LE_EVT_MASK_CONN_COMPLETE
    {
      dataLength = HCI_CONNECTION_COMPLETE_EVENT_LEN;
    }

    // OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 i = 0;

      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData    = (uint8*)(msg+1);
      msg->pData[i++] = HCI_EVENT_PACKET;
      msg->pData[i++] = HCI_LE_EVENT_CODE;
      msg->pData[i++] = dataLength;

      // populate event code
      if ( pBleEvtMask[LE_EVT_INDEX_ENH_CONN_COMPLETE] & LE_EVT_MASK_ENH_CONN_COMPLETE )
      {
        msg->pData[i++] = HCI_BLE_ENHANCED_CONNECTION_COMPLETE_EVENT;
      }
      else // LE_EVT_MASK_CONN_COMPLETE
      {
        msg->pData[i++] = HCI_BLE_CONNECTION_COMPLETE_EVENT;
      }

      msg->pData[i++] = reasonCode;                                 // reason code
      msg->pData[i++] = LO_UINT16 (connHandle);                     // connection handle (LSB)
      msg->pData[i++] = HI_UINT16 (connHandle);                     // connection handle (MSB)
      msg->pData[i++] = role;                                       // role (master/slave)

      if ( pBleEvtMask[LE_EVT_INDEX_ENH_CONN_COMPLETE] & LE_EVT_MASK_ENH_CONN_COMPLETE )
      {
        msg->pData[i++] = peerAddrType;                               // address type
      }
      else // Connection Complete
      {
        msg->pData[i++] = peerAddrType & LL_DEV_ADDR_TYPE_MASK;       // address type
      }

      // copy address
      (peerAddr != NULL) ? MAP_osal_memcpy( &msg->pData[i], peerAddr, B_ADDR_LEN ) :
      MAP_osal_memset( &msg->pData[i], 0, B_ADDR_LEN );

      i += B_ADDR_LEN;

      // populate event code
      if ( pBleEvtMask[LE_EVT_INDEX_ENH_CONN_COMPLETE] & LE_EVT_MASK_ENH_CONN_COMPLETE )
      {
        // local and peer RPA
        (localRPA != NULL) ? MAP_osal_memcpy( &msg->pData[i], localRPA, B_ADDR_LEN ) :
                             MAP_osal_memset( &msg->pData[i], 0, B_ADDR_LEN );

        i += B_ADDR_LEN;

        (peerRPA != NULL)  ? MAP_osal_memcpy( &msg->pData[i], peerRPA, B_ADDR_LEN ) :
                             MAP_osal_memset( &msg->pData[i], 0, B_ADDR_LEN );

        i += B_ADDR_LEN;
      }

      msg->pData[i++] = LO_UINT16 (connInterval);                  // connection interval (LSB)
      msg->pData[i++] = HI_UINT16 (connInterval);                  // connection interval (MSB)
      msg->pData[i++] = LO_UINT16 (slaveLatency);                  // slave latency (LSB)
      msg->pData[i++] = HI_UINT16 (slaveLatency);                  // slave latency (LSB)
      msg->pData[i++] = LO_UINT16 (connTimeout);                   // connectin timeout (LSB)
      msg->pData[i++] = HI_UINT16 (connTimeout);                   // connection timeout (MSB)
      msg->pData[i++] = clockAccuracy;                             // clock accuracy

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}


/*******************************************************************************
 * @fn          LL_ChannelSelectionAlgorithmCback Callback
 *
 * @brief       This LL callback is used to indicate which channel selection
 *              algorithm is used on a data channel connection.
 *
 * input parameters
 *
 * @param       connHandle - Connection handle.
 * @param       chSelAlgo  - LL_CHANNEL_SELECT_ALGO_1 | LL_CHANNEL_SELECT_ALGO_2
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_ChannelSelectionAlgorithmCback( uint16 connHandle,
                                        uint8  chSelAlgo )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pBleEvtMask[LE_EVT_INDEX_CHANNEL_SELECTION_ALGORITHM] & LE_EVT_MASK_CHANNEL_SELECTION_ALGORITHM) == 0) )
  {
    // the event mask is not enabled for this event
    return;
  }

    // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEChanSelAlgo_t *msg =
      (hciEvt_BLEChanSelAlgo_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLEChanSelAlgo_t ));

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_GAP_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

      // event packet
      msg->BLEEventCode = HCI_BLE_CHANNEL_SELECTION_ALGORITHM_EVENT;
      msg->connHandle   = connHandle;
      msg->chSelAlgo    = chSelAlgo;

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_CHANNEL_SELECTION_ALGORITHM_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_CHANNEL_SELECTION_ALGORITHM_EVENT;   // event code
      *pBuf++ = LO_UINT16(connHandle);                       // connection handle (LSB)
      *pBuf++ = HI_UINT16(connHandle);                       // connection handle (MSB)
      *pBuf++ = chSelAlgo;                                   // chan selection algo

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}


/*
** LL Callbacks for LE Meta-Events
*/

/*******************************************************************************
 * @fn          LL_AdvReportCback Callback
 *
 * @brief       This LL callback is used to generate a Advertisment Report meta
 *              event when an Advertisment or Scan Response is received by a
 *              Scanner.
 *
 * input parameters
 *
 * @param       advEvt      - Advertise event type, or Scan Response event type.
 * @param       advAddrType - Public or Random address type.
 * @param       advAddr     - Pointer to device address.
 * @param       dataLen     - Length of data in bytes.
 * @param       advData     - Pointer to data.
 * @param       rssi        - The RSSI of received packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_AdvReportCback( uint8 advEvt,
                        uint8 advAddrType,
                        uint8 *advAddr,
                        uint8 dataLen,
                        uint8 *advData,
                        int8  rssi )
{
  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEAdvPktReport_t *pkt;
    hciEvt_DevInfo_t *devInfo;
    uint8 x;

    pkt = (hciEvt_BLEAdvPktReport_t *)MAP_osal_msg_allocate( sizeof( hciEvt_BLEAdvPktReport_t ) +
                                                             sizeof( hciEvt_DevInfo_t ) );

    if ( pkt )
    {
      pkt->hdr.event = HCI_GAP_EVENT_EVENT;
      pkt->hdr.status = HCI_LE_EVENT_CODE;
      pkt->BLEEventCode = HCI_BLE_ADV_REPORT_EVENT;
      pkt->numDevices = 1;  // assume one device for now
      pkt->devInfo = devInfo = (hciEvt_DevInfo_t *)(pkt+1);

      for ( x = 0; x < pkt->numDevices; x++, devInfo++ )
      {
        /* Fill in the device info */
        devInfo->eventType = advEvt;
        devInfo->addrType = advAddrType;
        (void)MAP_osal_memcpy( devInfo->addr, advAddr, B_ADDR_LEN );
        devInfo->dataLen = dataLen;
        (void)MAP_osal_memcpy( devInfo->rspData, advData, dataLen );
        devInfo->rssi = rssi;
      }

      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 totalLength;
    uint8 dataLength;

    // check if LE Meta-Events are enabled and this event is enabled
    if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
         (((pBleEvtMask[LE_EVT_INDEX_ADV_REPORT] & LE_EVT_MASK_ADV_REPORT) == 0 )) )
    {
      // the event mask is not set for this event
      return;
    }

    // data length
    dataLength = HCI_ADV_REPORT_EVENT_LEN + dataLen;

    // OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData    = (uint8*)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;
      msg->pData[1] = HCI_LE_EVENT_CODE;
      msg->pData[2] = dataLength;

      // populate event
      msg->pData[3] = HCI_BLE_ADV_REPORT_EVENT;                // event code
      msg->pData[4] = 1;                                       // number of devices; assume 1 for now
      msg->pData[5] = advEvt;                                  // advertisement event type
      msg->pData[6] = advAddrType;                             // address type
      (void)MAP_osal_memcpy (&msg->pData[7], advAddr, B_ADDR_LEN); // address
      msg->pData[13] = dataLen;                                // data length
      (void)MAP_osal_memcpy (&msg->pData[14], advData, dataLen);   // data
      msg->pData[14 + dataLen] = rssi;                         // RSSI

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}


/*******************************************************************************
 * @fn          LL_ConnectionCompleteCback Callback
 *
 * @brief       This LL callback is used to generate a Connection Complete meta
 *              event when a connection is established by either an Advertiser
 *              or an Initiator.
 *
 * input parameters
 *
 * @param       reasonCode    - Status of connection complete.
 * @param       connHandle    - Connection handle.
 * @param       role          - Connection formed as Master or Slave.
 * @param       peerAddrType  - Peer address as Public or Random.
 * @param       peerAddr      - Pointer to peer device address.
 * @param       connInterval  - Connection interval.
 * @param       slaveLatency  - Slave latency.
 * @param       connTimeout   - Connection timeout.
 * @param       clockAccuracy - Sleep clock accuracy (from Master only).
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_ConnectionCompleteCback( uint8  reasonCode,
                                 uint16 connHandle,
                                 uint8  role,
                                 uint8  peerAddrType,
                                 uint8  *peerAddr,
                                 uint16 connInterval,
                                 uint16 slaveLatency,
                                 uint16 connTimeout,
                                 uint8  clockAccuracy )
{
  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEConnComplete_t *pkt;

    pkt = (hciEvt_BLEConnComplete_t *)MAP_osal_msg_allocate( sizeof( hciEvt_BLEConnComplete_t ) );
    if ( pkt )
    {
      pkt->hdr.event    = HCI_GAP_EVENT_EVENT;
      pkt->hdr.status   = HCI_LE_EVENT_CODE;
      pkt->BLEEventCode = HCI_BLE_CONNECTION_COMPLETE_EVENT;

      if ( reasonCode == LL_STATUS_SUCCESS )
      {
        pkt->status = HCI_SUCCESS;
        (void)MAP_osal_memcpy( pkt->peerAddr, peerAddr, B_ADDR_LEN );
      }
      else
      {
        pkt->status = bleGAPConnNotAcceptable;
        (void)MAP_osal_memset( pkt->peerAddr, 0, B_ADDR_LEN );
      }
      pkt->connectionHandle = connHandle;
      pkt->role             = role;
      pkt->peerAddrType     = peerAddrType;
      pkt->connInterval     = connInterval;
      pkt->connLatency      = slaveLatency;
      pkt->connTimeout      = connTimeout;
      pkt->clockAccuracy    = clockAccuracy;

      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // check if LE Meta-Events are enabled and this event is enabled
    if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
         (((pBleEvtMask[LE_EVT_INDEX_CONN_COMPLETE] & LE_EVT_MASK_CONN_COMPLETE) == 0 )) )
    {
      // the event mask is not enabled for this event
      return;
    }

    // data length
    dataLength = HCI_CONNECTION_COMPLETE_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData    = (uint8*)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;
      msg->pData[1] = HCI_LE_EVENT_CODE;
      msg->pData[2] = dataLength;

      // populate event
      msg->pData[3] = HCI_BLE_CONNECTION_COMPLETE_EVENT;          // event code
      msg->pData[4] = reasonCode;                                 // reason code
      msg->pData[5] = LO_UINT16 (connHandle);                     // connection handle (LSB)
      msg->pData[6] = HI_UINT16 (connHandle);                     // connection handle (MSB)
      msg->pData[7] = role;                                       // role (master/slave)
      msg->pData[8] = peerAddrType;                               // address type

      // copy address
      (void)MAP_osal_memcpy (&msg->pData[9], peerAddr, B_ADDR_LEN);

      msg->pData[15] = LO_UINT16 (connInterval);                  // connection interval (LSB)
      msg->pData[16] = HI_UINT16 (connInterval);                  // connection interval (MSB)
      msg->pData[17] = LO_UINT16 (slaveLatency);                  // slave latency (LSB)
      msg->pData[18] = HI_UINT16 (slaveLatency);                  // slave latency (LSB)
      msg->pData[19] = LO_UINT16 (connTimeout);                   // connectin timeout (LSB)
      msg->pData[20] = HI_UINT16 (connTimeout);                   // connection timeout (MSB)
      msg->pData[21] = clockAccuracy;                             // clock accuracy

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}


/*******************************************************************************
 * @fn          LL_DisconnectCback Callback
 *
 * @brief       This LL callback is used to generate a Disconnect Complete meta
 *              event when a connection is disconnected by either a Master or
 *              a Slave.
 *
 * input parameters
 *
 * @param       connHandle - Connection handle.
 * @param       reasonCode - Status of connection complete.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_DisconnectCback( uint16 connHandle,
                         uint8  reasonCode )
{
  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_DisconnComplete_t *pkt;

    pkt = (hciEvt_DisconnComplete_t *)MAP_osal_msg_allocate( sizeof( hciEvt_DisconnComplete_t ) );
    if ( pkt )
    {
      pkt->hdr.event  = HCI_GAP_EVENT_EVENT;
      pkt->hdr.status = HCI_DISCONNECTION_COMPLETE_EVENT_CODE;
      pkt->status     = HCI_SUCCESS;
      pkt->connHandle = connHandle;
      pkt->reason     = reasonCode;

      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // check the event mask
    if( ( BT_EVT_MASK_DISCONNECTION_COMPLETE &
          pHciEvtMask[BT_EVT_INDEX_DISCONNECT_COMPLETE]) == 0 )
    {
      // event mask is not set for this event, do not send to host
      return;
    }

    // data length
    dataLength = HCI_DISCONNECTION_COMPLETE_LEN;

    // the length will be OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData    = (uint8*)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;
      msg->pData[1] = HCI_DISCONNECTION_COMPLETE_EVENT_CODE;
      msg->pData[2] = dataLength;

      msg->pData[3] = HCI_SUCCESS;
      msg->pData[4] = LO_UINT16(connHandle);      // connection handle (LSB)
      msg->pData[5] = HI_UINT16(connHandle);      // connection handle (MSB)
      msg->pData[6] = reasonCode;                 // reason code

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}


/*******************************************************************************
 * @fn          LL_ConnParamUpdateCback Callback
 *
 * @brief       This LL callback is used to generate a Connection Update
 *              Complete meta event when a connection's parameters are updated
 *              by the Master, or if an error occurs (e.g. during the
 *              Connection Parameter Request control procedure).
 *
 * input parameters
 *
 * @param       status       - Status of update complete event.
 * @param       connHandle   - Connection handle.
 * @param       connInterval - Connection interval.
 * @param       slaveLatency - Slave latency.
 * @param       connTimeout  - Connection timeout.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_ConnParamUpdateCback( llStatus_t status,
                              uint16     connHandle,
                              uint16     connInterval,
                              uint16     connLatency,
                              uint16     connTimeout )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       (((pBleEvtMask[LE_EVT_INDEX_CONN_UPDATE_COMPLETE] & LE_EVT_MASK_CONN_UPDATE_COMPLETE) == 0 )) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEConnUpdateComplete_t *msg;
    uint8 totalLength;

    totalLength = sizeof( hciEvt_BLEConnUpdateComplete_t );

    msg = (hciEvt_BLEConnUpdateComplete_t *)MAP_osal_msg_allocate(totalLength);

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_GAP_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

      // event packet
      msg->BLEEventCode     = HCI_BLE_CONN_UPDATE_COMPLETE_EVENT;
      msg->status           = status;
      msg->connectionHandle = connHandle;
      msg->connInterval     = connInterval;
      msg->connLatency      = connLatency;
      msg->connTimeout      = connTimeout;

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_CONN_UPDATE_COMPLETE_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_CONN_UPDATE_COMPLETE_EVENT;  // event code
      *pBuf++ = status;                              // status
      *pBuf++ = LO_UINT16(connHandle);               // connection handle (LSB)
      *pBuf++ = HI_UINT16(connHandle);               // connection handle (MSB)
      *pBuf++ = LO_UINT16(connInterval);             // connection interval (LSB)
      *pBuf++ = HI_UINT16(connInterval);             // connection interval (MSB)
      *pBuf++ = LO_UINT16(connLatency);              // slave latency (LSB)
      *pBuf++ = HI_UINT16(connLatency);              // slave latency (MSB)
      *pBuf++ = LO_UINT16(connTimeout);              // connection timeout (LSB)
      *pBuf++ = HI_UINT16(connTimeout);              // connection timeout (MSB)

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}

/*******************************************************************************
 * @fn          LL_ConnParamUpdateRejectCback Callback
 *
 * @brief       This function will send an event to the GAP after the device
 *              has rejected a peer device connection parameter update request
 *
 *
 * input parameters
 *
 * @param       connHandle    - Connection handle.
 * @param       status        - Link Layer error code.
 *
 * @param       connInterval  - Connection interval.
 * @param       connLatency   - Slave latency.
 * @param       connTimeout   - Connection timeout.
 *
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_ConnParamUpdateRejectCback( llStatus_t   status,
                                    uint16  connHandle,
                                    uint16  connInterval,
                                    uint16  connLatency,
                                    uint16  connTimeout )

{
  // check if the feature was enabled by the application.
  if ( (hciVsEvtMask & PEER_PARAM_REJECT_ENABLED ) == 0 )
  {
    // the feature was not enabled
    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
      hciEvt_BLEConnUpdateComplete_t *msg;
      uint8 totalLength;

      totalLength = sizeof( hciEvt_BLEConnUpdateComplete_t );

      msg = (hciEvt_BLEConnUpdateComplete_t *)MAP_osal_msg_allocate(totalLength);

      if( msg )
      {
        // message header
        msg->hdr.event  = HCI_GAP_EVENT_EVENT;
        msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

        // event packet
        msg->BLEEventCode     = HCI_BLE_CONN_UPDATE_REJECT_EVENT;
        msg->status           = status;
        msg->connectionHandle = connHandle;
        msg->connInterval     = connInterval;
        msg->connLatency      = connLatency;
        msg->connTimeout      = connTimeout;

        // send the message
        (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
     }
  }
/** else
 {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_CONN_UPDATE_COMPLETE_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_CONN_UPDATE_REJECT_EVENT;  // event code
      *pBuf++ = status;                              // status
      *pBuf++ = LO_UINT16(connHandle);               // connection handle (LSB)
      *pBuf++ = HI_UINT16(connHandle);               // connection handle (MSB)
      *pBuf++ = LO_UINT16(connInterval);             // connection interval (LSB)
      *pBuf++ = HI_UINT16(connInterval);             // connection interval (MSB)
      *pBuf++ = LO_UINT16(connLatency);              // slave latency (LSB)
      *pBuf++ = HI_UINT16(connLatency);              // slave latency (MSB)
      *pBuf++ = LO_UINT16(connTimeout);              // connection timeout (LSB)
      *pBuf++ = HI_UINT16(connTimeout);              // connection timeout (MSB)

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  } **/
}

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          LL_ReadRemoteUsedFeaturesCompleteCback Callback
 *
 * @brief       This LL callback is used to generate a Read Remote Used Features
 *              Complete meta event when a Master makes this request of a Slave.
 *
 * input parameters
 *
 * @param       status     - HCI status.
 * @param       connHandle - Connection handle.
 * @param       featureSet - Pointer to eight byte bit mask of LE features.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_ReadRemoteUsedFeaturesCompleteCback( hciStatus_t  status,
                                             uint16       connHandle,
                                             uint8       *featureSet )
{
  hciPacket_t *msg;
  uint8 dataLength;
  uint8 totalLength;

  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       (((pBleEvtMask[LE_EVT_INDEX_READ_REMOTE_FEATURE] & LE_EVT_MASK_READ_REMOTE_FEATURE) == 0 )) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // data length
  dataLength = HCI_READ_REMOTE_FEATURE_COMPLETE_EVENT_LEN;

  // OSAL message header + HCI event header + data
  totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

  msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

  if (msg)
  {
    // message type, length
    msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
    msg->hdr.status = 0xFF;

    // create message
    msg->pData    = (uint8*)(msg+1);
    msg->pData[0] = HCI_EVENT_PACKET;
    msg->pData[1] = HCI_LE_EVENT_CODE;
    msg->pData[2] = dataLength;

    // event code
    msg->pData[3] = HCI_BLE_READ_REMOTE_FEATURE_COMPLETE_EVENT;
    msg->pData[4] = status;
    msg->pData[5] = LO_UINT16(connHandle);  // connection handle (LSB)
    msg->pData[6] = HI_UINT16(connHandle);  // connection handle (MSB)

    // feature set
#ifdef QUAL_TEST
    (void)MAP_osal_memcpy (&msg->pData[7], remoteFeatureSet[connHandle].featureSet, B_FEATURE_SUPPORT_LENGTH);
#else
    (void)MAP_osal_memcpy (&msg->pData[7], featureSet, B_FEATURE_SUPPORT_LENGTH);
#endif
    // send the message
    (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
  }
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * @fn          LL_ReadRemoteVersionInfoCback Callback
 *
 * @brief       This LL callback is used to generate a Read Remote Version
 *              Information Complete meta event when a Master makes this request
 *              of a Slave.
 *
 * input parameters
 *
 * @param       status     - Status of callback.
 * @param       verNum     - Version of the Bluetooth Controller specification.
 * @param       connHandle - Company identifier of the manufacturer of the
 *                           Bluetooth Controller.
 * @param       subverNum  - A unique value for each implementation or revision
 *                           of an implementation of the Bluetooth Controller.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_ReadRemoteVersionInfoCback( hciStatus_t status,
                                    uint16      connHandle,
                                    uint8       verNum,
                                    uint16      comId,
                                    uint16      subverNum )
{
  hciPacket_t *msg;
  uint8 dataLength;
  uint8 totalLength;

  // check the event mask
  if( ( BT_EVT_MASK_READ_REMOTE_VERSION_INFORMATION_COMPLETE &
        pHciEvtMask[BT_EVT_INDEX_READ_REMOTE_VERSION_INFO]) == 0 )
  {
    // event mask is not set for this event, do not send to host
    return;
  }

  // data length
  dataLength = HCI_REMOTE_VERSION_INFO_EVENT_LEN;

  // OSAL message header + HCI event header + data
  totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

  msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

  if (msg)
  {
    // message type, length
    msg->hdr.event   = HCI_CTRL_TO_HOST_EVENT;
    msg->hdr.status = 0xFF;

    // create message
    msg->pData     = (uint8*)(msg+1);
    msg->pData[0]  = HCI_EVENT_PACKET;
    msg->pData[1]  = HCI_READ_REMOTE_INFO_COMPLETE_EVENT_CODE;
    msg->pData[2]  = dataLength;
    msg->pData[3]  = status;
    msg->pData[4]  = LO_UINT16( connHandle );
    msg->pData[5]  = HI_UINT16( connHandle );
    msg->pData[6]  = verNum;
    msg->pData[7]  = LO_UINT16( comId );       // company ID (LSB)
    msg->pData[8]  = HI_UINT16( comId );       // company ID (MSB)
    msg->pData[9]  = LO_UINT16( subverNum );
    msg->pData[10] = HI_UINT16( subverNum );

    // send the message
    (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
  }
}


/*******************************************************************************
 * @fn          LL_EncLtkReqCback Callback
 *
 * @brief       This LL callback is used to generate a Encryption LTK Request
 *              meta event to provide to the Host the Master's random number
 *              and encryption diversifier, and to request the Host's Long Term
 *              Key (LTK).
 *
 * input parameters
 *
 * @param       connHandle - The LL connection ID for new connection.
 * @param       randNum    - Random vector used in device identification.
 * @param       encDiv     - Encrypted diversifier.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_EncLtkReqCback( uint16  connHandle,
                        uint8  *randNum,
                        uint8  *encDiv )
{
  // check if this is for the Host
  if ( hciSmpTaskID != 0 )
  {
    hciEvt_BLELTKReq_t *pkt;

    pkt = (hciEvt_BLELTKReq_t *)MAP_osal_msg_allocate( sizeof( hciEvt_BLELTKReq_t ) );
    if ( pkt )
    {
      pkt->hdr.event    = HCI_SMP_EVENT_EVENT;
      pkt->hdr.status   = HCI_LE_EVENT_CODE;
      pkt->BLEEventCode = HCI_BLE_LTK_REQUESTED_EVENT;
      pkt->connHandle   = connHandle;

      (void)MAP_osal_memcpy( pkt->random, randNum, B_RANDOM_NUM_SIZE );
      pkt->encryptedDiversifier = BUILD_UINT16( encDiv[0], encDiv[1] );

      (void)MAP_osal_msg_send( hciSmpTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // check if LE Meta-Events are enabled and this event is enabled
    if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
         (((pBleEvtMask[LE_EVT_INDEX_LTK_REQUEST] & LE_EVT_MASK_LTK_REQUEST) == 0 )) )
    {
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
      // the event mask is not set for this event, so reject immediately
      MAP_LL_EncLtkNegReply( connHandle );
#endif // CTRL_CONFIG=ADV_CONN_CFG

      return;
    }

    // data length
    dataLength = HCI_LTK_REQUESTED_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;
      *pBuf++ = HCI_BLE_LTK_REQUESTED_EVENT;                 // event code
      *pBuf++ = LO_UINT16(connHandle);                       // connection handle (LSB)
      *pBuf++ = HI_UINT16(connHandle);                       // connection handle (MSB)
      (void)MAP_osal_memcpy (pBuf, randNum, B_RANDOM_NUM_SIZE);  // random number
      pBuf += B_RANDOM_NUM_SIZE;                             // size of random number
      *pBuf++ = *encDiv++;                                   // encryption diversifier (LSB)
      *pBuf   = *encDiv;                                     // encryption diversifier (MSB)

      /* Send the message */
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          LL_EncChangeCback Callback
 *
 * @brief       This Callback is used by the LL to indicate to the Host that
 *              an encryption change has taken place. This results when
 *              the host performs a LL_StartEncrypt when encryption is not
 *              already enabled.
 *
 *              Note: If the key request was rejected, then encryption will
 *                    remain off.
 *
 * input parameters
 *
 * @param       connHandle - The LL connection ID for new connection.
 * @param       reason     - LL_ENC_KEY_REQ_ACCEPTED or LL_ENC_KEY_REQ_REJECTED.
 * @param       encEnab    - LL_ENCRYPTION_OFF or LL_ENCRYPTION_ON.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_EncChangeCback( uint16 connHandle,
                        uint8  reason,
                        uint8  encEnab )
{
  // check if this is for the Host
  if ( hciSmpTaskID != 0 )
  {
    hciEvt_EncryptChange_t *pkt;

    pkt = (hciEvt_EncryptChange_t *)MAP_osal_msg_allocate( sizeof( hciEvt_EncryptChange_t ) );
    if ( pkt )
    {
      pkt->hdr.event    = HCI_SMP_EVENT_EVENT;
      pkt->hdr.status   = HCI_LE_EVENT_CODE;
      pkt->BLEEventCode = HCI_ENCRYPTION_CHANGE_EVENT_CODE;
      pkt->connHandle   = connHandle;
      pkt->reason       = reason;
      pkt->encEnable    = encEnab;

      (void)MAP_osal_msg_send( hciSmpTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // check the event mask
    if( ( BT_EVT_MASK_ENCRYPTION_CHANGE &
          pHciEvtMask[BT_EVT_INDEX_ENCRYPTION_CHANGE]) == 0 )
    {
      // event mask is not set for this event, do not send to host
      return;
    }

    // data length
    dataLength = HCI_ENCRYPTION_CHANGE_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      // populate event
      *pBuf++    = HCI_EVENT_PACKET;
      *pBuf++    = HCI_ENCRYPTION_CHANGE_EVENT_CODE;
      *pBuf++    = dataLength;
      *pBuf++    = reason;
      *pBuf++    = LO_UINT16(connHandle);
      *pBuf++    = HI_UINT16(connHandle);
      *pBuf      = encEnab;

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * @fn          LL_EncKeyRefreshCback Callback
 *
 * @brief       This Callback is used by the LL to indicate to the Host that
 *              an encryption key refresh has taken place. This results when
 *              the host performs a LL_StartEncrypt when encryption is already
 *              enabled.
 *
 * input parameters
 *
 * @param       connHandle - The LL connection ID for new connection.
 * @param       reason    - LL_ENC_KEY_REQ_ACCEPTED, LL_CTRL_PKT_TIMEOUT_TERM
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_EncKeyRefreshCback( uint16 connHandle,
                            uint8  reason )
{
  // check if this is for the Host
  if ( hciSmpTaskID != 0 )
  {
    hciEvt_EncryptChange_t *pkt;

    pkt = (hciEvt_EncryptChange_t *)MAP_osal_msg_allocate( sizeof( hciEvt_EncryptChange_t ) );
    if ( pkt )
    {
      pkt->hdr.event    = HCI_SMP_EVENT_EVENT;
      pkt->hdr.status   = HCI_LE_EVENT_CODE;
      pkt->BLEEventCode = HCI_ENCRYPTION_CHANGE_EVENT_CODE;
      pkt->connHandle   = connHandle;
      pkt->reason       = reason;
      pkt->encEnable    = TRUE;

      (void)MAP_osal_msg_send( hciSmpTaskID, (uint8 *)pkt );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // check the event mask
    if( ( BT_EVT_MASK_ENCRYPTION_KEY_REFRESH_COMPLETE &
          pHciEvtMask[BT_EVT_INDEX_KEY_REFRESH_COMPLETE]) == 0 )
    {
      /* Event mask is not set for this event, do not send to host */
      return;
    }

    // data length
    dataLength = HCI_KEY_REFRESH_COMPLETE_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      // populate event
      *pBuf++    = HCI_EVENT_PACKET;
      *pBuf++    = HCI_KEY_REFRESH_COMPLETE_EVENT_CODE;
      *pBuf++    = dataLength;
      *pBuf++    = reason;
      *pBuf++    = LO_UINT16(connHandle);
      *pBuf++    = HI_UINT16(connHandle);

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * @fn          LL_DirectTestEndDone Callback
 *
 * @brief       This Callback is used by the LL to notify the HCI that the
 *              Direct Test End command has completed.
 *
 *
 * input parameters
 *
 * @param       numPackets - The number of packets received. Zero for transmit.
 * @param       mode       - LL_DIRECT_TEST_MODE_TX or LL_DIRECT_TEST_MODE_RX.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      LL_STATUS_SUCCESS
 */
void LL_DirectTestEndDoneCback( uint16 numPackets,
                                uint8  mode )
{
  uint8 rtnParam[3];

  // unused input parameter; PC-Lint error 715.
  (void)mode;

  rtnParam[0] = HCI_SUCCESS;

  rtnParam[1] = LO_UINT16( numPackets );
  rtnParam[2] = HI_UINT16( numPackets );

  MAP_HCI_CommandCompleteEvent( HCI_LE_TEST_END, 3, rtnParam );
}

/*******************************************************************************
 * @fn          LL_SetCteSamples
 *
 * @brief       This function is used to truncate and copy CTE samples
 *
 * @design      /ref did_202754181
 *
 * input parameters
 *
 * @param       sampleCount - number of samples
 * @param       sampleSlot - 1 us or 2 us
 * @param       sampleRate - 1Mhz to 4Mhz
 * @param       sampleSize - 8 bits or 16 bits
 * @param       sampleCtrl - default filtering or RAW_RF(no_filtering)
 * @param       samplesOffset - samples offset in source buffer 
 * @param       src - source which keep the samples in 16 bits per I sample and
 *                    16 bits per Q sample (32 bits per sample)
 * @param       iqSamples - destination buffer which will keep the IQ samples
 *
 *              each destination buffer size should be (sizeof (int8) * sampleCount)
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      updated samples offset
 */
uint16 LL_SetCteSamples( uint16  sampleCount,
                        uint8  sampleSlot,
                        uint8  sampleRate,
                        uint8  sampleSize,
                        uint8  sampleCtrl,
                        uint16  samplesOffset,
                        uint32 *src,
                        int8   *iqSamples)
{
  int16 sample[2];  //divide the sample for I sample and Q sample
  uint16 maxVal = 0;
  int32  absValQ = 0;
  int32  absValI = 0;
  uint16 sampleNum;  // antenna sample number
  uint16 sampleIdx;  // sample index in the src buffer
  uint8  shift = 0;
  uint8  firstSampleIndex;
  uint8  firstRefSampleIndex;
  uint8  iterate,j;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///// RAW_RF mode - copy samples without filtering, sampleRate/sampleSize/slotDuration are forced to 4/2/1  /////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // handle RF_RAW mode
  if (sampleCtrl & CTE_SAMPLING_CONTROL_RF_RAW_NO_FILTERING)
  {
    int16 *pIQ = (int16 *)iqSamples;

    // go over the antenna samples (divide count by 2 to compensate for 16bit size)
    for (sampleNum = 0; sampleNum < sampleCount/2; sampleNum++)
    {
      // find sample index in src buffer according to the samples offset without skipping
      sampleIdx = sampleNum + samplesOffset;

      // find in which RAM the sample located and get it in 16 bits
      *(uint32 *)sample = src[sampleIdx];

      pIQ[sampleNum * 2] = sample[1];
      pIQ[sampleNum  * 2 + 1] = sample[0];
    }
    // return updated samples offset
    return (samplesOffset + sampleCount/2);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///// Not RAW mode - switching period/idle period samples are filtered out according to the BT5.1 Spec /////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // normalize the number of samples
  // each sample can consist of up to 4 samples and 2 byes size
  // sample count should be aligned to sample rate and sample size
  sampleCount /= (sampleRate * sampleSize);

  //find the first sample index
  if (sampleRate == HCI_CTE_SAMPLE_RATE_4MHZ)
  {
    firstRefSampleIndex = HCI_CTE_FIRST_SAMPLE_IDX_REF_PERIOD_4MHZ;
    firstSampleIndex = (sampleSlot == LL_CTE_SAMPLE_SLOT_1US)?
                        HCI_CTE_FIRST_SAMPLE_IDX_SLOT_1US_4MHZ:
                        HCI_CTE_FIRST_SAMPLE_IDX_SLOT_2US_4MHZ;
  }
  else
  {
    firstRefSampleIndex = HCI_CTE_FIRST_SAMPLE_IDX_REF_PERIOD;
    firstSampleIndex = (sampleSlot == LL_CTE_SAMPLE_SLOT_1US)?
                        HCI_CTE_FIRST_SAMPLE_IDX_SLOT_1US:
                        HCI_CTE_FIRST_SAMPLE_IDX_SLOT_2US;
  }

  // in case sample size is 8 bits - execute the loop twice:
  // 1) find the max value
  // 2) normalize the samples and copy
  // in case the sample size is 16 bits - run only the second iteration, no need to normalize (RF samples are represented as 16 bits)
  for (iterate = (sampleSize - 1); iterate < 2; iterate++)
  {
    // go over the antenna samples
    for (sampleNum = 0; sampleNum < sampleCount ; sampleNum++)
    {
      // find sample index in src buffer according to the samples offset
      if ((sampleNum + samplesOffset) < HCI_CTE_SAMPLES_COUNT_REF_PERIOD)
      {
        sampleIdx = ((sampleNum + samplesOffset) * HCI_CTE_SAMPLE_JUMP_REF_PERIOD) + firstRefSampleIndex;
      }
      else
      {
        if (sampleSlot == LL_CTE_SAMPLE_SLOT_1US)
        {
          sampleIdx = (((sampleNum + samplesOffset) - HCI_CTE_SAMPLES_COUNT_REF_PERIOD) * HCI_CTE_SAMPLE_JUMP_SLOT_1US) + firstSampleIndex;
        }
        else
        {
          sampleIdx = (((sampleNum + samplesOffset) - HCI_CTE_SAMPLES_COUNT_REF_PERIOD) * HCI_CTE_SAMPLE_JUMP_SLOT_2US) + firstSampleIndex;
        }
      }

      // each antenna sample can consist of up to 4 samples - depend on the sample rate
      for (j = 0; j < sampleRate; j++)
      {        
        // find in which RAM the sample located and get it in 16 bits
        *(uint32 *)sample = src[sampleIdx + j];

        // first iteration - find the max value
        // relevant only when sample size is 8 bits
        if (iterate == 0)
        {
          // Get abs of Q
          absValQ = sample[0];

          if (absValQ < 0)
          {
            absValQ = (-1)*absValQ;
          }

          // Get abs of I
          absValI = sample[1];

          if (absValI < 0)
          {
            absValI = (-1)*absValI;
          }

          // Check if abs of I is bigger than the max value we found
          if (absValI > maxVal)
          {
            maxVal = absValI;
          }

          // Check if abs of Q is bigger than the max value we found
          if (absValQ > maxVal)
          {
            maxVal = absValQ;
          }
        }
        else // normalize the sample and copy it
        {
          // case of 8 bits sample size as 5.1 spec definition
          if (sampleSize == LL_CTE_SAMPLE_SIZE_8BITS)
          {
            // copy the Q sample
            iqSamples[(sampleNum * sampleRate * 2) + j + 1] = (int8)(sample[0] / (1<<shift));

            // copy the I sample
            iqSamples[(sampleNum * sampleRate * 2) + j] = (int8)(sample[1] / (1<<shift));
          }
          else // case of 16 bits sample size as defined by VS command
          {
            int16 *pIQ = (int16 *)iqSamples;

            // copy Q sample
            pIQ[(sampleNum * sampleRate * 2) + (2 * j) + 1] = sample[0];

            // copy I sample
            pIQ[(sampleNum * sampleRate * 2) + (2 * j)] = sample[1];
          }
        }
      }
    }

    // first iteration - find the shift value
    // relevant only when sample size is 8 bits
    if (iterate == 0)
    {
      // find shifting value according to the max value sample
      if (maxVal < 0x80)
      {
        shift = 0;
      }
      else
      {
        shift = 8;
        maxVal >>= 8;
        if (maxVal < 0x08)
        {
          shift -= 4;
        }
        else
        {
          maxVal >>= 4;
        }
        if (maxVal < 0x02)
        {
          shift -= 2;
        }
        else
        {
          maxVal >>= 2;
        }
        if (maxVal < 0x01)
        {
          shift -= 1;
        } 
      }
    }
  }
  // return updated samples offset
  return (samplesOffset + sampleCount);
}

/*******************************************************************************
 * @fn          HCI_ConnectionIqReportEvent Callback
 *
 * @brief       This function is used to generate a I/Q CTE report event
 *              after receiving a CTE response control packet with CTE.
 *
 * @design      /ref did_202754181
 *
 * input parameters
 *
 * @param       connHandle    - Connection handle.
 * @param       phy           - current phy 1M or 2M
 * @param       dataChIndex   - index of the data channel 
 * @param       rssi          - RSSI value of the packet
 * @param       rssiAntenna   - ID of the antenna on which the RSSI was measured
 * @param       cteType       - CTE type (0-AoA, 1-AoD with 1us, 2-AoD with 2us)
 * @param       slotDuration  - Switching and sampling slots (1 - 1us, 2 - 2us) 
 * @param       status        - packet status:
 *                              0 - CRC was correct
 *                              1 - CRC was incorrect 
 * @param       connEvent     - current connection event counter
 * @param       sampleCount   - number of samples including the 8 reference period
 * @param       cteData       - RF buffer which hold the samples
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HCI_ConnectionIqReportEvent(uint16 connHandle,
                                 uint8  phy,
                                 uint8  dataChIndex,
                                 uint16 rssi,
                                 uint8  rssiAntenna,
                                 uint8  cteType,
                                 uint8  slotDuration,
                                 uint8  status,
                                 uint16 connEvent,
                                 uint8  sampleCount,
                                 uint32 *cteData)
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
   (((pBleEvtMask[LE_EVT_INDEX_CONNECTION_IQ_REPORT] & LE_EVT_MASK_CONNECTION_IQ_REPORT) == 0 )) )
  {
    // the event mask is not enabled for this event
    return;
  }
  // case the source buffers are NULLs - set the samples count to proper value
  if (cteData == NULL)
  {
    sampleCount = 0;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLECteConnectionIqReport_t *msg;

    msg = (hciEvt_BLECteConnectionIqReport_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLECteConnectionIqReport_t ) + (2 * sampleCount));

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_GAP_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

      // event packet
      msg->BLEEventCode = HCI_BLE_CONNECTION_IQ_REPORT_EVENT;
      msg->connHandle   = connHandle;
      msg->phy          = phy;
      msg->dataChIndex  = dataChIndex;
      msg->rssi         = rssi;
      msg->rssiAntenna  = rssiAntenna;
      msg->cteType      = cteType;
      msg->slotDuration = slotDuration;
      msg->status       = status;
      msg->connEvent    = connEvent;
      msg->sampleCount  = sampleCount;
      msg->iqSamples    = (int8 *)((uint8 *)msg + sizeof( hciEvt_BLECteConnectionIqReport_t ));

      // copy IQ samples
      LL_SetCteSamples(sampleCount,
                       slotDuration,
                       HCI_CTE_SAMPLE_RATE_1MHZ,
                       LL_CTE_SAMPLE_SIZE_8BITS,
                       0,
                       0,
                       cteData,
                       msg->iqSamples);

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length includes I samples and Q samples
    dataLength = HCI_CONNECTION_IQ_REPORT_EVENT_LEN + (sampleCount * 2);

    // OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData    = (uint8*)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;
      msg->pData[1] = HCI_LE_EVENT_CODE;
      msg->pData[2] = dataLength;

      // populate event
      msg->pData[3] = HCI_BLE_CONNECTION_IQ_REPORT_EVENT;         // event code
      msg->pData[4] = LO_UINT16 (connHandle);                     // connection handle (LSB)
      msg->pData[5] = HI_UINT16 (connHandle);                     // connection handle (MSB)
      msg->pData[6] = phy;                                        // current phy
      msg->pData[7] = dataChIndex;                                // index of data channel
      msg->pData[8] = LO_UINT16 (rssi);                           // rssi (LSB)
      msg->pData[9] = HI_UINT16 (rssi);                           // rssi (MSB)
      msg->pData[10] = rssiAntenna;                               // antenna ID
      msg->pData[11] = cteType;                                   // cte type
      msg->pData[12] = slotDuration;                              // sampling slot 1us or 2us
      msg->pData[13] = status;                                    // packet status
      msg->pData[14] = LO_UINT16 (connEvent);                     // connection event (LSB)
      msg->pData[15] = HI_UINT16 (connEvent);                     // connection event (MSB)
      msg->pData[16] = sampleCount;                               // number of samples

      // copy IQ samples
      LL_SetCteSamples(sampleCount,
                       slotDuration,
                       HCI_CTE_SAMPLE_RATE_1MHZ,
                       LL_CTE_SAMPLE_SIZE_8BITS,
                       0,
                       0,
                       cteData,
                       (int8 *)&(msg->pData[HCI_EVENT_MIN_LENGTH+HCI_CONNECTION_IQ_REPORT_EVENT_LEN]));

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}

/*******************************************************************************
 * @fn          HCI_CteRequestFailedEvent Callback
 *
 * @brief       This function is used to generate a I/Q CTE report event
 *              after receiving a CTE response control packet with CTE data.
 *
 * @design      /ref did_202754181
 *
 * input parameters
 *
 * @param       status        - Status of IQ report.
 * @param       connHandle    - Connection handle.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HCI_CteRequestFailedEvent( uint8  status,
                                uint16 connHandle)
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pBleEvtMask[LE_EVT_INDEX_CTE_REQUEST_FAILED] & LE_EVT_MASK_CTE_REQUEST_FAILED) == 0) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLECteRequestFailed_t *msg =
      (hciEvt_BLECteRequestFailed_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLECteRequestFailed_t ));

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_GAP_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

      // event packet
      msg->BLEEventCode = HCI_BLE_CTE_REQUEST_FAILED_EVENT;
      msg->status       = status;
      msg->connHandle   = connHandle;

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_CTE_REQUEST_FAILED_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_CTE_REQUEST_FAILED_EVENT;   // event code
      *pBuf++ = status;
      *pBuf++ = LO_UINT16(connHandle);              // connection handle (LSB)
      *pBuf++ = HI_UINT16(connHandle);              // connection handle (MSB)

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}

/*******************************************************************************
 * @fn          HCI_ExtConnectionIqReportEvent Callback
 *
 * @brief       This function is used to generate an Extended I/Q CTE (Oversampling)
 *              report event after receiving packet with CTE.
 *
 * @design      /ref did_202754181
 *
 * input parameters
 *
 * @param       connHandle    - Connection handle.
 * @param       phy           - current phy 1M or 2M
 * @param       dataChIndex   - index of the data channel
 * @param       rssi          - RSSI value of the packet
 * @param       rssiAntenna   - ID of the antenna on which the RSSI was measured
 * @param       cteType       - CTE type (0-AoA, 1-AoD with 1us, 2-AoD with 2us)
 * @param       slotDuration  - Switching and sampling slots (1 - 1us, 2 - 2us)
 * @param       status        - packet status:
 *                              0 - CRC was correct
 *                              1 - CRC was incorrect
 * @param       connEvent     - current connection event counter
 * @param       sampleCount   - number of samples including the 8 reference period
 * @param       sampleRate    - number of samples per 1us represent CTE accuracy
 *                              range : 1 - least accuracy (as in 5.1 spec) to 4 - most accuracy
 * @param       sampleSize    - sample size represent CTE accuracy
 *                              range : 1 - 8 bit (as in 5.1 spec) or 2 - 16 bits (most accurate)
 * @param       sampleCtrl    - 1 : RF RAW mode 2: Filtered mode (switching period omitted)
 * @param       cteData       - RF buffer which hold the samples
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HCI_ExtConnectionIqReportEvent(uint16 connHandle,
                                    uint8  phy,
                                    uint8  dataChIndex,
                                    uint16 rssi,
                                    uint8  rssiAntenna,
                                    uint8  cteType,
                                    uint8  slotDuration,
                                    uint8  status,
                                    uint16 connEvent,
                                    uint16 sampleCount,
                                    uint8  sampleRate,
                                    uint8  sampleSize,
                                    uint8  sampleCtrl,
                                    uint32 *cteData)
{
  uint8 numEvents = 0;
  uint8 samplesPerEvent;
  uint16 samplesOffset = 0;
  uint8 i;
  uint16 totalDataLen;

  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
   (((pBleEvtMask[LE_EVT_INDEX_CONNECTION_IQ_REPORT] & LE_EVT_MASK_CONNECTION_IQ_REPORT) == 0 )) )
  {
    // the event mask is not enabled for this event
    return;
  }
  // case the source buffers are NULLs - set the samples count to proper value
  if (cteData == NULL)
  {
    sampleCount = 0;
  }

  // set the total samples includes over sampling (in case the host enable it by VS command)
  sampleCount *= (sampleRate * sampleSize);

  // set the number of host event to send
  numEvents += ((sampleCount / HCI_CTE_MAX_SAMPLES_PER_EVENT) + (((sampleCount % HCI_CTE_MAX_SAMPLES_PER_EVENT) != 0)?1:0));
  totalDataLen = sampleCount;

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEExtCteConnectionIqReport_t *msg;

    // compose each event
    for (i=0; i < numEvents; i++)
    {
      // set number of sample in current event
      // should be aligned to sample rate and size
      samplesPerEvent = (sampleCount < HCI_CTE_MAX_SAMPLES_PER_EVENT)?sampleCount:HCI_CTE_MAX_SAMPLES_PER_EVENT;

      // update the total samples value
      sampleCount -= samplesPerEvent;

      msg = (hciEvt_BLEExtCteConnectionIqReport_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLEExtCteConnectionIqReport_t ) + (2 * samplesPerEvent));

      if( msg )
      {
        // message header
        msg->hdr.event  = HCI_GAP_EVENT_EVENT;
        msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

        // event packet
        msg->BLEEventCode = HCI_BLE_EXT_CONNECTION_IQ_REPORT_EVENT;
        msg->totalDataLen = totalDataLen;
        msg->eventIndex   = i;
        msg->connHandle   = connHandle;
        msg->phy          = phy;
        msg->dataChIndex  = dataChIndex;
        msg->rssi         = rssi;
        msg->rssiAntenna  = rssiAntenna;
        msg->cteType      = cteType;
        msg->slotDuration = slotDuration;
        msg->status       = status;
        msg->connEvent    = connEvent;
        msg->dataLen      = samplesPerEvent;
        msg->sampleRate   = sampleRate;
        msg->sampleSize   = sampleSize;
        msg->sampleCtrl   = sampleCtrl;
        msg->iqSamples    = (int8 *)((uint8 *)msg + sizeof( hciEvt_BLEExtCteConnectionIqReport_t ));

        // copy IQ samples
        samplesOffset = LL_SetCteSamples(samplesPerEvent,
                                         slotDuration,
                                         sampleRate,
                                         sampleSize,
                                         sampleCtrl,
                                         samplesOffset,
                                         cteData,
                                         msg->iqSamples);

        // send the message
        (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
      }
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // compose each event
    for (i=0; i < numEvents; i++)
    {
      // set number of sample in current event
      // should be aligned to sample rate
      samplesPerEvent = (sampleCount < HCI_CTE_MAX_SAMPLES_PER_EVENT)?sampleCount:HCI_CTE_MAX_SAMPLES_PER_EVENT;

      // update the total samples value
      sampleCount -= samplesPerEvent;

      // data length includes I samples and Q samples
      dataLength = HCI_EXT_CONNECTION_IQ_REPORT_EVENT_LEN + (samplesPerEvent * 2);

      // OSAL message header + HCI event header + data
      totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

      msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

      if (msg)
      {
        // message type, length
        msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
        msg->hdr.status = 0xFF;

        // create message
        msg->pData    = (uint8*)(msg+1);
        msg->pData[0] = HCI_EVENT_PACKET;
        msg->pData[1] = HCI_LE_EVENT_CODE;
        msg->pData[2] = dataLength;

        // populate event
        msg->pData[3] = HCI_BLE_EXT_CONNECTION_IQ_REPORT_EVENT;     // event code
        msg->pData[4] = LO_UINT16 (totalDataLen);                   // total samples data length (LSB)
        msg->pData[5] = HI_UINT16 (totalDataLen);                   // total samples data length (MSB)
        msg->pData[6] = i;                                          // event number
        msg->pData[7] = LO_UINT16 (connHandle);                     // connection handle (LSB)
        msg->pData[8] = HI_UINT16 (connHandle);                     // connection handle (MSB)
        msg->pData[9] = phy;                                        // current phy
        msg->pData[10] = dataChIndex;                               // index of data channel
        msg->pData[11] = LO_UINT16 (rssi);                          // rssi (LSB)
        msg->pData[12] = HI_UINT16 (rssi);                          // rssi (MSB)
        msg->pData[13] = rssiAntenna;                               // antenna ID
        msg->pData[14] = cteType;                                   // cte type
        msg->pData[15] = slotDuration;                              // sampling slot 1us or 2us
        msg->pData[16] = status;                                    // packet status
        msg->pData[17] = LO_UINT16 (connEvent);                     // connection event (LSB)
        msg->pData[18] = HI_UINT16 (connEvent);                     // connection event (MSB)
        msg->pData[19] = samplesPerEvent;                           // number of samples
        msg->pData[20] = sampleRate;                                // sample rate
        msg->pData[21] = sampleSize;                                // sample size
        msg->pData[22] = sampleCtrl;                                // sample control flags

        // copy IQ samples
        samplesOffset = LL_SetCteSamples(samplesPerEvent,
                                         slotDuration,
                                         sampleRate,
                                         sampleSize,
                                         sampleCtrl,
                                         samplesOffset,
                                         cteData,
                                         (int8 *)&(msg->pData[HCI_EVENT_MIN_LENGTH+HCI_EXT_CONNECTION_IQ_REPORT_EVENT_LEN]));

        // send the message
        (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
      }
    }
  }
}

/*******************************************************************************
 * @fn          HCI_ConnectionlessIqReportEvent Callback
 *
 * @brief       This function is used to generate a I/Q CTE report event
 *              after receiving advertise or generic rx packet with CTE.
 *
 * input parameters
 *
 * @param       syncHandle    - periodic advertisment sync handle.
 * @param       channelIndex  - index of the data channel
 * @param       rssi          - RSSI value of the packet
 * @param       rssiAntenna   - ID of the antenna on which the RSSI was measured
 * @param       cteType       - CTE type (0-AoA, 1-AoD with 1us, 2-AoD with 2us)
 * @param       slotDuration  - Switching and sampling slots (1 - 1us, 2 - 2us)
 * @param       status        - packet status:
 *                              0 - CRC was correct
 *                              1 - CRC was incorrect
 * @param       eventCounter  - current periodic adv event counter
 * @param       sampleCount   - number of samples including the 8 reference period
 *                              range : bit0=0 - Default filtering, bit0=1 - RAW_RF(no filtering), , bit1..7=0 - spare
 * @param       cteData       - RF buffer which hold the samples
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HCI_ConnectionlessIqReportEvent(uint16 syncHandle,
                                     uint8  channelIndex,
                                     uint16 rssi,
                                     uint8  rssiAntenna,
                                     uint8  cteType,
                                     uint8  slotDuration,
                                     uint8  status,
                                     uint16 eventCounter,
                                     uint8  sampleCount,
                                     uint32 *cteData)
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
   (((pBleEvtMask[LE_EVT_INDEX_CONNECTIONLESS_IQ_REPORT] & LE_EVT_MASK_CONNECTIONLESS_IQ_REPORT) == 0 )) )
  {
    // the event mask is not enabled for this event
    return;
  }
  // case the main source buffer is NULL - do not send the report
  if (cteData == NULL)
  {
    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    if (llCteTest.testMode == TRUE)
    {
      // this section need to be used when the event destination is host_test
      hciPacket_t *msg;
      uint8 dataLength;
      uint8 totalLength;

      // data length includes I samples and Q samples
      dataLength = HCI_CONNECTIONLESS_IQ_REPORT_EVENT_LEN + (sampleCount * 2);

      // OSAL message header + HCI event header + data
      totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

      msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

      if (msg)
      {
        // message type, length
        msg->hdr.event	= HCI_GAP_EVENT_EVENT;
        msg->hdr.status = HCI_TEST_EVENT_CODE;

        // create message
        msg->pData	  = (uint8*)(msg+1);

        // populate event
        msg->pData[0] = HCI_BLE_CONNECTIONLESS_IQ_REPORT_EVENT; 	// event code
        msg->pData[1] = LO_UINT16 (syncHandle); 					        // periodic adv sync handle (LSB)
        msg->pData[2] = HI_UINT16 (syncHandle); 					        // periodic adv sync handle (MSB)
        msg->pData[3] = channelIndex;								              // index of data channel
        msg->pData[4] = LO_UINT16 (rssi);							            // rssi (LSB)
        msg->pData[5] = HI_UINT16 (rssi);							            // rssi (MSB)
        msg->pData[6] = rssiAntenna;								              // antenna ID
        msg->pData[7] = cteType;									                // cte type
        msg->pData[8] = slotDuration;								              // sampling slot 1us or 2us
        msg->pData[9] = status;									                  // packet status
        msg->pData[10] = LO_UINT16 (eventCounter);					      // periodic adv event (LSB)
        msg->pData[11] = HI_UINT16 (eventCounter);					      // periodic adv event (MSB)
        msg->pData[12] = sampleCount;								              // number of samples

        // copy IQ samples
        LL_SetCteSamples(sampleCount,
                         slotDuration,
                         HCI_CTE_SAMPLE_RATE_1MHZ,
                         LL_CTE_SAMPLE_SIZE_8BITS,
                         0,
                         0,
                         cteData,
                         (int8 *)&(msg->pData[HCI_CONNECTIONLESS_IQ_REPORT_EVENT_LEN]));

        // send the message
        (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
       }
     }
     else
     {// this section is used to send event to gap_task

	   hciEvt_BLECteConnectionlessIqReport_t *msg;

	   msg = (hciEvt_BLECteConnectionlessIqReport_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLECteConnectionlessIqReport_t ) + (2 * sampleCount));

       if( msg )
       {
         // message header
         msg->hdr.event	= HCI_GAP_EVENT_EVENT;
         msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

         // event packet
         msg->BLEEventCode = HCI_BLE_CONNECTIONLESS_IQ_REPORT_EVENT;
         msg->syncHandle   = syncHandle;
         msg->channelIndex = channelIndex;
         msg->rssi		  = rssi;
         msg->rssiAntenna  = rssiAntenna;
         msg->cteType	  = cteType;
         msg->slotDuration = slotDuration;
         msg->status 	  = status;
         msg->eventCounter = eventCounter;
         msg->sampleCount  = sampleCount;
         msg->iqSamples	  = (int8 *)((uint8 *)msg + sizeof( hciEvt_BLECteConnectionlessIqReport_t ));

         // copy IQ samples
         LL_SetCteSamples(sampleCount,
                          slotDuration,
                          HCI_CTE_SAMPLE_RATE_1MHZ,
                          LL_CTE_SAMPLE_SIZE_8BITS,
                          0,
                          0,
                          cteData,
                          msg->iqSamples);

         // send the message
         (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );

       }
     }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length includes I samples and Q samples
    dataLength = HCI_CONNECTIONLESS_IQ_REPORT_EVENT_LEN + (sampleCount * 2);

    // OSAL message header + HCI event header + data
    totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      // message type, length
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // create message
      msg->pData    = (uint8*)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;
      msg->pData[1] = HCI_LE_EVENT_CODE;
      msg->pData[2] = dataLength;

      // populate event
      msg->pData[3] = HCI_BLE_CONNECTIONLESS_IQ_REPORT_EVENT;     // event code
      msg->pData[4] = LO_UINT16 (syncHandle);                     // periodic adv sync handle (LSB)
      msg->pData[5] = HI_UINT16 (syncHandle);                     // periodic adv sync handle (MSB)
      msg->pData[6] = channelIndex;                               // index of data channel
      msg->pData[7] = LO_UINT16 (rssi);                           // rssi (LSB)
      msg->pData[8] = HI_UINT16 (rssi);                           // rssi (MSB)
      msg->pData[9] = rssiAntenna;                                // antenna ID
      msg->pData[10] = cteType;                                   // cte type
      msg->pData[11] = slotDuration;                              // sampling slot 1us or 2us
      msg->pData[12] = status;                                    // packet status
      msg->pData[13] = LO_UINT16 (eventCounter);                  // periodic adv event (LSB)
      msg->pData[14] = HI_UINT16 (eventCounter);                  // periodic adv event (MSB)
      msg->pData[15] = sampleCount;                               // number of samples

      // copy IQ samples
      LL_SetCteSamples(sampleCount,
                       slotDuration,
                       HCI_CTE_SAMPLE_RATE_1MHZ,
                       LL_CTE_SAMPLE_SIZE_8BITS,
                       0,
                       0,
                       cteData,
                       (int8 *)&(msg->pData[HCI_EVENT_MIN_LENGTH+HCI_CONNECTIONLESS_IQ_REPORT_EVENT_LEN]));

      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}

/*******************************************************************************
 * @fn          HCI_ExtConnectionlessIqReportEvent Callback
 *
 * @brief       This function is used to generate an Extended I/Q CTE (Oversampling)
 *              report event after receiving periodic advertise packet with CTE.
 *
 * input parameters
 *
 * @param       syncHandle    - periodic advertisment sync handle.
 * @param       channelIndex  - index of the data channel
 * @param       rssi          - RSSI value of the packet
 * @param       rssiAntenna   - ID of the antenna on which the RSSI was measured
 * @param       cteType       - CTE type (0-AoA, 1-AoD with 1us, 2-AoD with 2us)
 * @param       slotDuration  - Switching and sampling slots (1 - 1us, 2 - 2us)
 * @param       status        - packet status:
 *                              0 - CRC was correct
 *                              1 - CRC was incorrect
 * @param       eventCounter  - current periodic adv event counter
 * @param       sampleCount   - number of samples including the 8 reference period
 * @param       sampleRate    - number of samples per 1us represent CTE accuracy
 *                              range : 1 - least accuracy (as in 5.1 spec) to 4 - most accuracy
 * @param       sampleSize    - sample size represent CTE accuracy
 *                              range : 1 - 8 bit (as in 5.1 spec) or 2 - 16 bits (most accurate)
 * @param       sampleCtrl    - sample control flags
 *                              range : bit0=0 - Default filtering, bit0=1 - RAW_RF(no filtering), , bit1..7=0 - spare
 * @param       cteData       - RF buffer which hold the samples
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HCI_ExtConnectionlessIqReportEvent(uint16 syncHandle,
                                        uint8  channelIndex,
                                        uint16 rssi,
                                        uint8  rssiAntenna,
                                        uint8  cteType,
                                        uint8  slotDuration,
                                        uint8  status,
                                        uint16 eventCounter,
                                        uint16 sampleCount,
                                        uint8  sampleRate,
                                        uint8  sampleSize,
                                        uint8  sampleCtrl,
                                        uint32 *cteData)
{
  uint8 numEvents = 0;
  uint8 samplesPerEvent;
  uint16 samplesOffset = 0;
  uint8 i;
  uint16 totalDataLen;

  // check if LE Meta-Events are enabled and this event is enabled
  // Note: the bit number is the same as Connectionless Iq Report Event bit
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
   (((pBleEvtMask[LE_EVT_INDEX_CONNECTIONLESS_IQ_REPORT] & LE_EVT_MASK_CONNECTIONLESS_IQ_REPORT) == 0 )) )
  {
    // the event mask is not enabled for this event
    return;
  }
   // case the main source buffer is NULL - do not send the report
  if (cteData == NULL)
  {
    return;
  }
  // set the total samples includes over sampling (in case the host enable it by VS command)
  sampleCount *= (sampleRate * sampleSize);

  // set the number of host event to send
  numEvents += ((sampleCount / HCI_CTE_MAX_SAMPLES_PER_EVENT) + (((sampleCount % HCI_CTE_MAX_SAMPLES_PER_EVENT) != 0)?1:0));
  totalDataLen = sampleCount;

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEExtCteConnectionlessIqReport_t *msg;

    // compose each event
    for (i=0; i < numEvents; i++)
    {
      // set number of sample in current event
      // should be aligned to sample rate and size
      samplesPerEvent = (sampleCount < HCI_CTE_MAX_SAMPLES_PER_EVENT)?sampleCount:HCI_CTE_MAX_SAMPLES_PER_EVENT;

      // update the total samples value
      sampleCount -= samplesPerEvent;

      msg = (hciEvt_BLEExtCteConnectionlessIqReport_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLEExtCteConnectionlessIqReport_t ) + (2 * samplesPerEvent));

      if( msg )
      {
        // message header
        msg->hdr.event  = HCI_GAP_EVENT_EVENT;
        msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

        // event packet
        msg->BLEEventCode = HCI_BLE_EXT_CONNECTIONLESS_IQ_REPORT_EVENT;
        msg->totalDataLen = totalDataLen;
        msg->eventIndex   = i;
        msg->syncHandle   = syncHandle;
        msg->channelIndex = channelIndex;
        msg->rssi         = rssi;
        msg->rssiAntenna  = rssiAntenna;
        msg->cteType      = cteType;
        msg->slotDuration = slotDuration;
        msg->status       = status;
        msg->eventCounter = eventCounter;
        msg->dataLen      = samplesPerEvent;
        msg->sampleRate   = sampleRate;
        msg->sampleSize   = sampleSize;
        msg->sampleCtrl   = sampleCtrl;
        msg->iqSamples    = (int8 *)((uint8 *)msg + sizeof( hciEvt_BLEExtCteConnectionlessIqReport_t ));

        // copy IQ samples
        samplesOffset = LL_SetCteSamples(samplesPerEvent,
                                         slotDuration,
                                         sampleRate,
                                         sampleSize,
                                         sampleCtrl,
                                         samplesOffset,
                                         cteData,
                                         msg->iqSamples);

        // send the message
        (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
      }
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // compose each event
    for (i=0; i < numEvents; i++)
    {
      // set number of sample in current event
      // should be aligned to sample rate
      samplesPerEvent = (sampleCount < HCI_CTE_MAX_SAMPLES_PER_EVENT)?sampleCount:HCI_CTE_MAX_SAMPLES_PER_EVENT;

      // update the total samples value
      sampleCount -= samplesPerEvent;

      // data length includes I samples and Q samples
      dataLength = HCI_EXT_CONNECTIONLESS_IQ_REPORT_EVENT_LEN + (samplesPerEvent * 2);

      // OSAL message header + HCI event header + data
      totalLength = sizeof( hciPacket_t ) + HCI_EVENT_MIN_LENGTH + dataLength;

      msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

      if (msg)
      {
        // message type, length
        msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
        msg->hdr.status = 0xFF;

        // create message
        msg->pData    = (uint8*)(msg+1);
        msg->pData[0] = HCI_EVENT_PACKET;
        msg->pData[1] = HCI_LE_EVENT_CODE;
        msg->pData[2] = dataLength;

        // populate event
        msg->pData[3] = HCI_BLE_EXT_CONNECTIONLESS_IQ_REPORT_EVENT; // event code
        msg->pData[4] = LO_UINT16 (totalDataLen);                   // total samples data length (LSB)
        msg->pData[5] = HI_UINT16 (totalDataLen);                   // total samples data length (MSB)
        msg->pData[6] = i;                                          // event number
        msg->pData[7] = LO_UINT16 (syncHandle);                     // periodic handle (LSB)
        msg->pData[8] = HI_UINT16 (syncHandle);                     // periodic handle (MSB)
        msg->pData[9] = channelIndex;                              // index of data channel
        msg->pData[10] = LO_UINT16 (rssi);                          // rssi (LSB)
        msg->pData[11] = HI_UINT16 (rssi);                          // rssi (MSB)
        msg->pData[12] = rssiAntenna;                               // antenna ID
        msg->pData[13] = cteType;                                   // cte type
        msg->pData[14] = slotDuration;                              // sampling slot 1us or 2us
        msg->pData[15] = status;                                    // packet status
        msg->pData[16] = LO_UINT16 (eventCounter);                  // periodic adv event counter (LSB)
        msg->pData[17] = HI_UINT16 (eventCounter);                  // periodic adv event counter (MSB)
        msg->pData[18] = samplesPerEvent;                           // number of samples
        msg->pData[19] = sampleRate;                                // sample rate
        msg->pData[20] = sampleSize;                                // sample size
        msg->pData[21] = sampleCtrl;                                // sample control flags

        // copy IQ samples
        samplesOffset = LL_SetCteSamples(samplesPerEvent,
                                         slotDuration,
                                         sampleRate,
                                         sampleSize,
                                         sampleCtrl,
                                         samplesOffset,
                                         cteData,
                                         (int8 *)&(msg->pData[HCI_EVENT_MIN_LENGTH+HCI_EXT_CONNECTIONLESS_IQ_REPORT_EVENT_LEN]));

        // send the message
        (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
      }
    }
  }
}

/*********************************************************************
 * @fn      HCI_PeriodicAdvSyncEstablishedEvent
 *
 * @brief   This event indicates the scanner that the Controller has received
 *          the first periodic advertising packet from an advertiser after the
 *          HCI_LE_Periodic_Advertising_Create_Sync command has been sent to the Controller.
 *
 * @design  /ref did_286039104
 *
 * input parameters
 *
 * @param   status           - Periodic advertising sync HCI status
 * @param   syncHandle       - Handle identifying the periodic advertising train assigned by the Controller
 *                             (Range: 0x0000 to 0x0EFF)
 * @param   advSid           - Value of the Advertising SID subfield in the ADI field of the PDU
 * @param   advAddrType      - Advertiser address type
 *                             0x00 - Public
 *                             0x01 - Random
 *                             0x02 - Public Identity Address
 *                             0x03 - Random Identity Address
 * @param   advAddress       - Advertiser address
 * @param   advPhy           - Advertiser PHY
 *                             0x01 - LE 1M
 *                             0x02 - LE 2M
 *                             0x03 - LE Coded
 * @param   periodicAdvInt   - Periodic advertising interval Range: 0x0006 to 0xFFFF
 *                             Time = N * 1.25 ms (Time Range: 7.5 ms to 81.91875 s)
 * @param   advClockAccuracy - Accuracy of the periodic advertiser's clock
 *                             0x00 - 500 ppm
 *                             0x01 - 250 ppm
 *                             0x02 - 150 ppm
 *                             0x03 - 100 ppm
 *                             0x04 - 75 ppm
 *                             0x05 - 50 ppm
 *                             0x06 - 30 ppm
 *                             0x07 - 20 ppm
 *
 * @return  void
 */
void HCI_PeriodicAdvSyncEstablishedEvent( uint8  status,
                                          uint16 syncHandle,
                                          uint8  advSid,
                                          uint8  advAddrType,
                                          uint8  *advAddress,
                                          uint8  advPhy,
                                          uint16 periodicAdvInt,
                                          uint8  advClockAccuracy )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pBleEvtMask[LE_EVT_INDEX_PERIODIC_ADV_SYNC_ESTABLISHED] & LE_EVT_MASK_PERIODIC_ADV_SYNC_ESTABLISHED) == 0) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEPeriodicAdvSyncEstablished_t *msg =
      (hciEvt_BLEPeriodicAdvSyncEstablished_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLEPeriodicAdvSyncEstablished_t ));

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_GAP_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

      // event packet
      msg->BLEEventCode = HCI_BLE_PERIODIC_ADV_SYNCH_ESTABLISHED_EVENT;
      msg->status       = status;
      msg->syncHandle   = syncHandle;
      msg->sid          = advSid;
      msg->addrType     = advAddrType;
      if (advAddress != NULL)
      {
        (void)MAP_osal_memcpy( msg->address, advAddress, B_ADDR_LEN );
      }
      msg->phy          = advPhy;
      msg->periodicInterval = periodicAdvInt;
      msg->clockAccuracy = advClockAccuracy;

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_PERIODIC_ADV_SYNCH_ESTABLISHED_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_PERIODIC_ADV_SYNCH_ESTABLISHED_EVENT;   // event code
      *pBuf++ = status;
      *pBuf++ = LO_UINT16(syncHandle);              // sync handle (LSB)
      *pBuf++ = HI_UINT16(syncHandle);              // sync handle (MSB)
      *pBuf++ = advSid;
      *pBuf++ = advAddrType;
      if (advAddress != NULL)
      {
        (void)MAP_osal_memcpy( pBuf, advAddress, B_ADDR_LEN );
      }
      pBuf += B_ADDR_LEN;
      *pBuf++ = advPhy;
      *pBuf++ = LO_UINT16(periodicAdvInt);          // periodic interval (LSB)
      *pBuf++ = HI_UINT16(periodicAdvInt);          // periodic interval (MSB)
      *pBuf = advClockAccuracy;
      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}

/*********************************************************************
 * @fn      HCI_LE_PeriodicAdvertisingReportEvent
 *
 * @brief   This event indicates the scanner that the Controller has
 *          received a Periodic Advertising packet.
 *
 * @design  /ref did_286039104
 *
 * @param   syncHandle - Handle identifying the periodic advertising train
 * @param   txPower    - Tx Power information (Range: -127 to +20)
 * @param   rssi       - RSSI value for the received packet (Range: -127 to +20)
 *                       If the packet contains CTE, this value is not available
 * @param   cteType    - Constant Tone Extension type
 *                       0x00 - AoA Constant Tone Extension
 *                       0x01 - AoD Constant Tone Extension with 1us slots
 *                       0x02 - AoD Constant Tone Extension with 2us slots
 *                       0xFF - No Constant Tone Extension
 * @param   dataStatus - Data status
 *                       0x00 - Data complete
 *                       0x01 - Data incomplete, more data to come
 *                       0x02 - Data incomplete, data truncated, no more to come
 * @param   dataLen    - Length of the Data field (Range: 0 to 247)
 * @param   data       - Data received from a Periodic Advertising packet
 *
 * @return  void
 */
void HCI_PeriodicAdvReportEvent( uint16 syncHandle,
                                 int8   txPower,
                                 int8   rssi,
                                 uint8  cteType,
                                 uint8  dataStatus,
                                 uint8  dataLen,
                                 uint8  *data )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pBleEvtMask[LE_EVT_INDEX_PERIODIC_ADV_REPORT] & LE_EVT_MASK_PERIODIC_ADV_REPORT) == 0) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEPeriodicAdvReport_t *msg =
      (hciEvt_BLEPeriodicAdvReport_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLEPeriodicAdvReport_t ) + dataLen);

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_GAP_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

      // event packet
      msg->BLEEventCode = HCI_BLE_PERIODIC_ADV_REPORT_EVENT;
      msg->syncHandle   = syncHandle;
      msg->txPower      = txPower;
      msg->rssi         = rssi;
      msg->cteType      = cteType;
      msg->dataStatus   = dataStatus;
      msg->dataLen      = dataLen;
      if ((data != NULL) && (dataLen > 0))
      {
        msg->data = ((uint8 *)(msg)) + sizeof( hciEvt_BLEPeriodicAdvReport_t );
        MAP_osal_memcpy( msg->data, data, dataLen );
      }

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 eventLength;
    uint8 dataOffset = 0;
    uint16 totalLength;

    do
    {
      // data length
      dataLength = MIN( dataLen, HCI_PERIODIC_ADV_REPORT_MAX_DATA );
      dataLen -= dataLength;
      eventLength = HCI_PERIODIC_ADV_REPORT_EVENT_LEN + dataLength;
      // OSAL message header + HCI event header + data
      totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + eventLength;

      msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

      if (msg)
      {
        uint8 *pBuf;

        // message type
        msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
        msg->hdr.status = 0xFF;

        // point to the byte following the hciPacket_t structure
        msg->pData = (uint8*)(msg+1);
        pBuf       = msg->pData;

        *pBuf++ = HCI_EVENT_PACKET;
        *pBuf++ = HCI_LE_EVENT_CODE;
        *pBuf++ = eventLength;

        // populate event
        *pBuf++ = HCI_BLE_PERIODIC_ADV_REPORT_EVENT;   // event code
        *pBuf++ = LO_UINT16(syncHandle);              // sync handle (LSB)
        *pBuf++ = HI_UINT16(syncHandle);              // sync handle (MSB)
        *pBuf++ = txPower;
        *pBuf++ = rssi;
        *pBuf++ = cteType;
        *pBuf++ = (dataLen > 0)?HCI_PERIODIC_ADV_REPORT_DATA_INCOMPLETE:dataStatus;
        *pBuf++ = dataLength;
        if ((data != NULL) && (dataLength > 0))
        {
          MAP_osal_memcpy( pBuf, data + dataOffset, dataLength );
          dataOffset += dataLength;
        }

        // send the message
        (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
      }
    }
    while (dataLen > 0);
  }
}

/*********************************************************************
 * @fn      HCI_PeriodicAdvSyncLostEvent
 *
 * @brief   This event indicates the scanner that the Controller has not
 *          received a Periodic Advertising packet from the train identified
 *          by syncHandle within the timeout period.
 *
 * @design  /ref did_286039104
 *
 * @param   syncHandle - Handle identifying the periodic advertising train
 *
 * @return  void
 */
void HCI_PeriodicAdvSyncLostEvent( uint16 syncHandle )
{
  // check if LE Meta-Events are enabled and this event is enabled
  if ( ((pHciEvtMask[BT_EVT_INDEX_LE_META_EVENT] & BT_EVT_MASK_LE_META_EVENT) == 0) ||
       ((pBleEvtMask[LE_EVT_INDEX_PERIODIC_ADV_SYNC_LOST] & LE_EVT_MASK_PERIODIC_ADV_SYNC_LOST) == 0) )
  {
    // the event mask is not enabled for this event
    return;
  }

  // check if this is for the Host
  if ( hciGapTaskID != 0 )
  {
    hciEvt_BLEPeriodicAdvSyncLost_t *msg =
      (hciEvt_BLEPeriodicAdvSyncLost_t *)MAP_osal_msg_allocate(sizeof( hciEvt_BLEPeriodicAdvSyncLost_t ));

    if( msg )
    {
      // message header
      msg->hdr.event  = HCI_GAP_EVENT_EVENT;
      msg->hdr.status = HCI_LE_EVENT_CODE; // use status field to pass the HCI Event code

      // event packet
      msg->BLEEventCode = HCI_BLE_PERIODIC_ADV_SYNCH_LOST_EVENT;
      msg->syncHandle   = syncHandle;

      // send the message
      (void)MAP_osal_msg_send( hciGapTaskID, (uint8 *)msg );
    }
  }
  else
  {
    hciPacket_t *msg;
    uint8 dataLength;
    uint8 totalLength;

    // data length
    dataLength = HCI_PERIODIC_ADV_SYNCH_LOST_EVENT_LEN;

    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

    msg = (hciPacket_t *)MAP_osal_msg_allocate(totalLength);

    if (msg)
    {
      uint8 *pBuf;

      // message type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // point to the byte following the hciPacket_t structure
      msg->pData = (uint8*)(msg+1);
      pBuf       = msg->pData;

      *pBuf++ = HCI_EVENT_PACKET;
      *pBuf++ = HCI_LE_EVENT_CODE;
      *pBuf++ = dataLength;

      // populate event
      *pBuf++ = HCI_BLE_PERIODIC_ADV_SYNCH_LOST_EVENT;   // event code
      *pBuf++ = LO_UINT16(syncHandle);              // sync handle (LSB)
      *pBuf++ = HI_UINT16(syncHandle);              // sync handle (MSB)
      // send the message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
  }
}

/*******************************************************************************
 */
