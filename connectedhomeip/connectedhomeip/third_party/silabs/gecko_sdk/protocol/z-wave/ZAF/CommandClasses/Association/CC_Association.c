/**
 * @file CC_Association.c
 * Handler for Command Class Association.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <association_plus.h>
#include <CC_Association.h>
#include <string.h>
#include "ZAF_tx_mutex.h"
//#define DEBUGPRINT
#include "DebugPrint.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

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

static received_frame_status_t
CC_Association_handler(
    RECEIVE_OPTIONS_TYPE_EX *rxOpt,
    ZW_APPLICATION_TX_BUFFER *pCmd,
    uint8_t cmdLength
)
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER * pTxBuf = &(TxBuf.appTxBuf);
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
  uint8_t txResult;

  switch (pCmd->ZW_Common.cmd)
  {
    case ASSOCIATION_GET_V2:
      {
        size_t outgoingFrameLength;

        if (true == Check_not_legal_response_job(rxOpt))
        {
          // Get/Report do not support endpoint bit addressing.
          return RECEIVED_FRAME_STATUS_FAIL;
        }

        memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

        AssociationGet(
            rxOpt->destNode.endpoint,
            (uint8_t *)&pCmd->ZW_AssociationGetFrame.cmdClass,
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
          return RECEIVED_FRAME_STATUS_FAIL;
        }
        return RECEIVED_FRAME_STATUS_SUCCESS;
      }
      break;

    case ASSOCIATION_SET_V2:
      if (E_CMD_HANDLER_RETURN_CODE_FAIL == handleAssociationSet(
          rxOpt->destNode.endpoint,
          (ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME*)pCmd,
          cmdLength,
          COMMAND_CLASS_ASSOCIATION))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;

    case ASSOCIATION_REMOVE_V2:
      if (3 > cmdLength)
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      if (E_CMD_HANDLER_RETURN_CODE_FAIL == AssociationRemove(
          pCmd->ZW_AssociationRemove1byteV2Frame.groupingIdentifier,
          rxOpt->destNode.endpoint,
          (ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_1BYTE_V2_FRAME*)pCmd,
          cmdLength))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case ASSOCIATION_GROUPINGS_GET_V2:
        DPRINT("ASSOCIATION_GROUPINGS_GET_V2\r\n");

        if(false == Check_not_legal_response_job(rxOpt))
        {
          memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

          TRANSMIT_OPTIONS_TYPE_SINGLE_EX *txOptionsEx;
          RxToTxOptions(rxOpt, &txOptionsEx);
          pTxBuf->ZW_AssociationGroupingsReportV2Frame.cmdClass = COMMAND_CLASS_ASSOCIATION;
          pTxBuf->ZW_AssociationGroupingsReportV2Frame.cmd = ASSOCIATION_GROUPINGS_REPORT_V2;
          pTxBuf->ZW_AssociationGroupingsReportV2Frame.supportedGroupings = handleGetMaxAssociationGroups(rxOpt->destNode.endpoint);
          if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP( (uint8_t *)pTxBuf,
                                            sizeof(pTxBuf->ZW_AssociationGroupingsReportV2Frame),
                                            txOptionsEx,
                                            NULL))
          {
            /*Job failed */
            ;
          }
          return RECEIVED_FRAME_STATUS_SUCCESS;
        }
        return RECEIVED_FRAME_STATUS_FAIL;
        break;
    case ASSOCIATION_SPECIFIC_GROUP_GET_V2:
        DPRINT("ASSOCIATION_GROUPINGS_GET_V2\r\n");

        if(false== Check_not_legal_response_job(rxOpt))
        {
          memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

          TRANSMIT_OPTIONS_TYPE_SINGLE_EX *txOptionsEx;
          RxToTxOptions(rxOpt, &txOptionsEx);
          pTxBuf->ZW_AssociationSpecificGroupReportV2Frame.cmdClass = COMMAND_CLASS_ASSOCIATION;
          pTxBuf->ZW_AssociationSpecificGroupReportV2Frame.cmd = ASSOCIATION_SPECIFIC_GROUP_REPORT_V2;
          pTxBuf->ZW_AssociationSpecificGroupReportV2Frame.group = ApplicationGetLastActiveGroupId();
          if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP( (uint8_t *)pTxBuf,
                                            sizeof(pTxBuf->ZW_AssociationSpecificGroupReportV2Frame),
                                            txOptionsEx,
                                            NULL))
          {
            /*Job failed */
            ;
          }
          return RECEIVED_FRAME_STATUS_SUCCESS;
        }
        return RECEIVED_FRAME_STATUS_FAIL;
        break;
  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}

REGISTER_CC(COMMAND_CLASS_ASSOCIATION, ASSOCIATION_VERSION_V2, CC_Association_handler);
