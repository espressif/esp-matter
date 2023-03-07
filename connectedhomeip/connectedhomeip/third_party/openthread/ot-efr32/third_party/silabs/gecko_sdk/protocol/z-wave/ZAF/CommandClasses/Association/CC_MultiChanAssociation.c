/**
 * @file CC_MultiChanAssociation.c
 * Handler for Command Class Multi Channel Association.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <CC_MultiChanAssociation.h>
#include <CC_Association.h>
#include <ZW_TransportLayer.h>
#include <association_plus.h>
#include <string.h>
#include "ZAF_tx_mutex.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

static received_frame_status_t
CC_MultiChannelAssociation_handler(
    RECEIVE_OPTIONS_TYPE_EX *rxOpt,
    ZW_APPLICATION_TX_BUFFER *pCmd,
    uint8_t cmdLength)
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER * pTxBuf = &(TxBuf.appTxBuf);

  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
  uint8_t txResult;

  switch (pCmd->ZW_Common.cmd)
  {
    case MULTI_CHANNEL_ASSOCIATION_GET_V2:
      {
        size_t outgoingFrameLength;

        if (true == Check_not_legal_response_job(rxOpt))
        {
          /*Do not support endpoint bit-addressing */
          return RECEIVED_FRAME_STATUS_FAIL;
        }

        memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

        AssociationGet(
            rxOpt->destNode.endpoint,
            (uint8_t *)&pCmd->ZW_MultiChannelAssociationGetV3Frame.cmdClass,
            (uint8_t *)pTxBuf,
            &outgoingFrameLength);

        RxToTxOptions(rxOpt, &pTxOptionsEx);

        // Transmit the stuff.
        txResult = Transport_SendResponseEP(
            (uint8_t *)pTxBuf,
            outgoingFrameLength,
            pTxOptionsEx,
            NULL);
        if (ZAF_ENQUEUE_STATUS_SUCCESS != txResult)
        {
          ;
        }
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case MULTI_CHANNEL_ASSOCIATION_SET_V2:
      if (E_CMD_HANDLER_RETURN_CODE_HANDLED != handleAssociationSet(
          rxOpt->destNode.endpoint,
          (ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME*)pCmd,
          cmdLength,
          COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case MULTI_CHANNEL_ASSOCIATION_REMOVE_V2:
      if (2 == cmdLength)
      {
        /*
         * According to the CC Multi Channel spec, the remove command MAY be interpreted with only
         * command class and command resulting in a command length of 2 bytes. This is interpreted as if
         * the command was [Class, Command, GroupID=0] which will remove all nodes in all groups.
         * Hence, set group ID to zero.
         */
        pCmd->ZW_MultiChannelAssociationRemove1byteV2Frame.groupingIdentifier = 0;
      }
      if (E_CMD_HANDLER_RETURN_CODE_FAIL == AssociationRemove(
          pCmd->ZW_MultiChannelAssociationRemove1byteV2Frame.groupingIdentifier,
          rxOpt->destNode.endpoint,
          (ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_1BYTE_V2_FRAME*)pCmd,
          cmdLength))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case MULTI_CHANNEL_ASSOCIATION_GROUPINGS_GET_V2:
      if (true == Check_not_legal_response_job(rxOpt))
      {
        /*Do not support endpoint bit-addressing */
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      RxToTxOptions(rxOpt, &pTxOptionsEx);
      pTxBuf->ZW_MultiChannelAssociationGroupingsReportV2Frame.cmdClass = COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2;
      pTxBuf->ZW_MultiChannelAssociationGroupingsReportV2Frame.cmd = MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V2;
      pTxBuf->ZW_MultiChannelAssociationGroupingsReportV2Frame.supportedGroupings = handleGetMaxAssociationGroups(rxOpt->destNode.endpoint);
      if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP( (uint8_t *)pTxBuf,
                                    sizeof(pTxBuf->ZW_MultiChannelAssociationGroupingsReportV2Frame),
                                    pTxOptionsEx,
                                    NULL))
      {
        /*Job failed */
        ;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;
  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}

REGISTER_CC(COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3, MULTI_CHANNEL_ASSOCIATION_VERSION_V3, CC_MultiChannelAssociation_handler);
