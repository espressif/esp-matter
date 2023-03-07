/**
 * @file
 * Handler for Command Class Multi Channel.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <CC_MultiChan.h>
#include "multichannel.h"
#include <string.h>
#include <ZW_TransportSecProtocol.h>
#include <ZAF_command_class_utils.h>
#include <ZAF_Common_interface.h>
#include "ZAF_tx_mutex.h"
#include "ZAF_CC_Invoker.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

// Add debug defines here if necessary. Remember UART api.

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

static void CmdClassMultiChannelEncapsulateCmd(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_MULTI_CHANNEL_CMD_ENCAP_V2_FRAME *pCmd,
  uint8_t cmdLength,
  ZW_APPLICATION_TX_BUFFER * pFrameOut,
  uint8_t * pLengthOut);

/****************************************************************************/
/*                              FUNCTIONS                                   */
/****************************************************************************/

static received_frame_status_t CC_MultiChannel_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t cmdLength,
  ZW_APPLICATION_TX_BUFFER * pFrameOut,
  uint8_t * pLengthOut)
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER * pTxBuf = &(TxBuf.appTxBuf);
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX * pTxOptionsEx;

  switch(pCmd->ZW_Common.cmd)
  {
    case MULTI_CHANNEL_END_POINT_GET_V4:
      if (true == Check_not_legal_response_job(rxOpt))
      {
        // None of the following commands support endpoint bit addressing.
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      RxToTxOptions(rxOpt, &pTxOptionsEx);

      pTxBuf->ZW_MultiChannelEndPointReportV4Frame.cmdClass      = COMMAND_CLASS_MULTI_CHANNEL_V4;
      pTxBuf->ZW_MultiChannelEndPointReportV4Frame.cmd           = MULTI_CHANNEL_END_POINT_REPORT_V4;
      GetMultiChannelEndPointFunctionality((EP_FUNCTIONALITY_DATA*) &pTxBuf->ZW_MultiChannelEndPointReportV4Frame.properties1);

      if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
          (uint8_t *)pTxBuf,
          sizeof(ZW_MULTI_CHANNEL_END_POINT_REPORT_V4_FRAME),
          pTxOptionsEx,
          NULL))
      {
        /*Job failed */
        ;
      }
      break;

    case MULTI_CHANNEL_CAPABILITY_GET_V4:
      if (true == Check_not_legal_response_job(rxOpt))
      {
        // None of the following commands support endpoint bit addressing.
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      if ((0 == (pCmd->ZW_MultiChannelCapabilityGetV4Frame.properties1 & 0x7F)) ||
          (NUMBER_OF_ENDPOINTS < (pCmd->ZW_MultiChannelCapabilityGetV4Frame.properties1 & 0x7F)))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      {
        ZW_MULTI_CHANNEL_CAPABILITY_GET_V4_FRAME * pCmdCap = (ZW_MULTI_CHANNEL_CAPABILITY_GET_V4_FRAME *)pCmd;
        DEVICE_CLASS* pDevClass = GetEndpointDeviceClass(pCmdCap->properties1 & 0x7F);
        CMD_CLASS_LIST* pCmdClassList = GetCommandClassList((0 != ZAF_GetNodeID()), SECURITY_KEY_NONE, (pCmdCap->properties1 & 0x7F));
        size_t ccListSize;

        RxToTxOptions(rxOpt, &pTxOptionsEx);

        pTxBuf->ZW_MultiChannelCapabilityReport4byteV4Frame.cmdClass    = COMMAND_CLASS_MULTI_CHANNEL_V4;
        pTxBuf->ZW_MultiChannelCapabilityReport4byteV4Frame.cmd         = MULTI_CHANNEL_CAPABILITY_REPORT_V4;
        pTxBuf->ZW_MultiChannelCapabilityReport4byteV4Frame.properties1 = (pCmdCap->properties1 & 0x7f);
        if (IS_NULL(pCmdClassList) || IS_NULL(pCmdClassList->pList) || (0 == pCmdClassList->size) || IS_NULL(pDevClass) || (0 == (pCmdCap->properties1 & 0x7f)))
        {
          return RECEIVED_FRAME_STATUS_FAIL;
        }
        else
        {
          pTxBuf->ZW_MultiChannelCapabilityReport4byteV4Frame.genericDeviceClass  = pDevClass->genericDeviceClass;
          pTxBuf->ZW_MultiChannelCapabilityReport4byteV4Frame.specificDeviceClass = pDevClass->specificDeviceClass;
          memcpy( &(pTxBuf->ZW_MultiChannelCapabilityReport4byteV4Frame.commandClass1), pCmdClassList->pList, pCmdClassList->size);
          ccListSize = pCmdClassList->size;
        }

        if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
            (uint8_t *)pTxBuf,
            sizeof(ZW_MULTI_CHANNEL_CAPABILITY_REPORT_1BYTE_V4_FRAME) + ccListSize - 1,
            pTxOptionsEx,
            NULL))
        {
          /*Job failed */
          ;
        }
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case MULTI_CHANNEL_END_POINT_FIND_V4:
      if (true == Check_not_legal_response_job(rxOpt))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      RxToTxOptions(rxOpt, &pTxOptionsEx);

      {
        size_t bufLen;
        ZW_MULTI_CHANNEL_END_POINT_FIND_V4_FRAME* pcmdEpfind = (ZW_MULTI_CHANNEL_END_POINT_FIND_V4_FRAME *)pCmd;

        pTxBuf->ZW_MultiChannelEndPointFindReport1byteV4Frame.cmdClass = COMMAND_CLASS_MULTI_CHANNEL_V4;
        pTxBuf->ZW_MultiChannelEndPointFindReport1byteV4Frame.cmd = MULTI_CHANNEL_END_POINT_FIND_REPORT_V4;
        pTxBuf->ZW_MultiChannelEndPointFindReport1byteV4Frame.reportsToFollow = 0;
        pTxBuf->ZW_MultiChannelEndPointFindReport1byteV4Frame.genericDeviceClass = pcmdEpfind->genericDeviceClass;
        pTxBuf->ZW_MultiChannelEndPointFindReport1byteV4Frame.specificDeviceClass = pcmdEpfind->specificDeviceClass;
        bufLen = FindEndPoints(pcmdEpfind->genericDeviceClass,
                               pcmdEpfind->specificDeviceClass,
                               &pTxBuf->ZW_MultiChannelEndPointFindReport1byteV4Frame.variantgroup1.properties1);

        if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
            (uint8_t *)pTxBuf,
            sizeof(ZW_MULTI_CHANNEL_END_POINT_FIND_REPORT_1BYTE_V4_FRAME) + bufLen - 1 ,
            pTxOptionsEx,
            NULL))
        {
          /*Job failed */
          ;
        }
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;
    case MULTI_CHANNEL_CMD_ENCAP_V4:
      CmdClassMultiChannelEncapsulateCmd(rxOpt,(ZW_MULTI_CHANNEL_CMD_ENCAP_V2_FRAME*) pCmd, cmdLength, pFrameOut, pLengthOut);
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;
    case MULTI_CHANNEL_AGGREGATED_MEMBERS_GET_V4:
      if (true == Check_not_legal_response_job(rxOpt))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      RxToTxOptions(rxOpt, &pTxOptionsEx);

      {
        ZW_MULTI_CHANNEL_AGGREGATED_MEMBERS_GET_V4_FRAME* pcmdEpAgg = (ZW_MULTI_CHANNEL_AGGREGATED_MEMBERS_GET_V4_FRAME *)pCmd;
        pTxBuf->ZW_MultiChannelAggregatedMembersReport1byteV4Frame.cmdClass = COMMAND_CLASS_MULTI_CHANNEL_V4;
        pTxBuf->ZW_MultiChannelAggregatedMembersReport1byteV4Frame.cmd = MULTI_CHANNEL_AGGREGATED_MEMBERS_REPORT_V4;
        pTxBuf->ZW_MultiChannelAggregatedMembersReport1byteV4Frame.properties1 = pcmdEpAgg->properties1;
        pTxBuf->ZW_MultiChannelAggregatedMembersReport1byteV4Frame.numberOfBitMasks =
          ReadAggregatedEndpointGroup( pcmdEpAgg->properties1,
                                       &pTxBuf->ZW_MultiChannelAggregatedMembersReport1byteV4Frame.aggregatedMembersBitMask1);

        if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
            (uint8_t *)pTxBuf,
            sizeof(ZW_MULTI_CHANNEL_AGGREGATED_MEMBERS_REPORT_1BYTE_V4_FRAME) + pTxBuf->ZW_MultiChannelAggregatedMembersReport1byteV4Frame.numberOfBitMasks - 1 ,
            pTxOptionsEx,
            NULL))
        {
          /*Job failed */
        }
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}


/**
 * Extracts the content of a multichannel frame and call the application command handler.
 * @param rxOpt Receive options.
 * @param pCmd Given command.
 * @param cmdLength Length of the command.
 */
static void CmdClassMultiChannelEncapsulateCmd(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_MULTI_CHANNEL_CMD_ENCAP_V2_FRAME *pCmd,
  uint8_t cmdLength,
  ZW_APPLICATION_TX_BUFFER * pFrameOut,
  uint8_t * pLengthOut)
{
  uint8_t bitNo;

  //Setup source node endpoint
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
  rxOpt->sourceNode.endpoint = pCmd->properties1;

  if( 0x00 == (pCmd->properties2 & 0x80))
  {
    /*Non bit addressing */
    rxOpt->destNode.endpoint = pCmd->properties2 & 0x7f;
  }
  else
  {
    /* Need to extract endpoint bits. Just reset it*/
    rxOpt->destNode.endpoint = 0;
  }
#pragma GCC diagnostic pop

  bitNo = 0;

  do /*BitAdress = 0 -> run only through once*/
  {
    /*BitAdress = 0 -> Destination address 1-127 */
    /*BitAdress = 1 -> Destination address is mask for End-point 1-7 */
    if(0 == (pCmd->properties2 & 0x80))/**End-point address**/
    {
      bitNo = 7;
    }
    else /**End-point bit mask address**/
    {
      while(0 == ((pCmd->properties2 & 0x7f) & (1<<bitNo)))/* Search through bitfied endpoint*/
      {
        bitNo++;
        /*validate bitNo is legal endpoint*/
        if((7 == bitNo) || NULL == GetEndpointcmdClassList(false, bitNo)){
          return;
        }
      }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      rxOpt->destNode.endpoint = bitNo + 1;/*endpoint = bit x + 1*/
#pragma GCC diagnostic pop

      rxOpt->destNode.BitAddress = 1; /*Tell Command Class that it is bit-addres. Get do not support bit-address!!*/
    }

    if (true == ZAF_CC_MultiChannel_IsCCSupported(rxOpt, (ZW_APPLICATION_TX_BUFFER*)&pCmd->encapFrame))
    {
      /* Command class supported */
      received_frame_status_t status;
      status = invoke_cc_handler(rxOpt, (ZW_APPLICATION_TX_BUFFER*)&pCmd->encapFrame, cmdLength - 4, pFrameOut, pLengthOut);
      if (RECEIVED_FRAME_STATUS_CC_NOT_FOUND == status) {
        Transport_ApplicationCommandHandlerEx(rxOpt, (ZW_APPLICATION_TX_BUFFER*)&pCmd->encapFrame, cmdLength - 4);
      }
    }
    if (0 == rxOpt->destNode.endpoint)
    {
      return; /* No endpoint addressing! just return */
    }
  }while(7 > ++bitNo);
}


REGISTER_CC_V2(COMMAND_CLASS_MULTI_CHANNEL_V4, MULTI_CHANNEL_VERSION_V4, CC_MultiChannel_handler);
