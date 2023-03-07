
/**
 * @file
 * Handler for Command Class Association Group Info.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <AppTimer.h>
#include <SwTimer.h>
#include <CC_AssociationGroupInfo.h>
#include <string.h>
#include "ZAF_tx_mutex.h"
#include "ZW_TransportEndpoint.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

#define REPORT_ONE_GROUP 1
#define REPORT_ALL_GROUPS 2

typedef enum
{
  REPORT_STATUS_INACTIVE,
  REPORT_STATUS_ONE_GROUP,
  REPORT_STATUS_ALL_GROUPS
}
report_status_t;

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

static uint8_t currentGroupId;
static uint8_t associationGroupInfoGetEndpoint = 0;
static report_status_t reportStatus = REPORT_STATUS_INACTIVE;
static RECEIVE_OPTIONS_TYPE_EX rxOptions;
static SSwTimer AGIReportSendTimer;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

void ZCB_AGIReport(uint8_t txStatus);
void ZCB_AGIReportSendTimer(SSwTimer* pTimer);

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

static bool SendAssoGroupInfoReport(void)
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);

  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *txOptionsEx;

  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

  /*Check pTxBuf is free*/
  if (IS_NULL( pTxBuf ))
  {
    return false;
  }

  RxToTxOptions(&rxOptions, &txOptionsEx);

  pTxBuf->ZW_AssociationGroupInfoReport1byteFrame.cmdClass = COMMAND_CLASS_ASSOCIATION_GRP_INFO;
  pTxBuf->ZW_AssociationGroupInfoReport1byteFrame.cmd      = ASSOCIATION_GROUP_INFO_REPORT;
  // If thelist mode bit is set in the get frame it should be also set in the report frame.
  pTxBuf->ZW_AssociationGroupInfoReport1byteFrame.properties1 = ((REPORT_STATUS_ALL_GROUPS == reportStatus) ? ASSOCIATION_GROUP_INFO_REPORT_PROPERTIES1_LIST_MODE_BIT_MASK : 0x00) & 0xFF; // we send one report per group
  pTxBuf->ZW_AssociationGroupInfoReport1byteFrame.properties1 |= 0x01; // One report pr. group.

  CC_AGI_groupInfoGet_handler(
      currentGroupId,
      rxOptions.destNode.endpoint,
      &pTxBuf->ZW_AssociationGroupInfoReport1byteFrame.variantgroup1);

  if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP( (uint8_t *)pTxBuf,
              sizeof(ZW_ASSOCIATION_GROUP_INFO_REPORT_1BYTE_FRAME),
              txOptionsEx,
			  ZCB_AGIReport))
  {
    return false;
  }
  return true;
}


/*
Use this timer to delay the sending of the next AGI report after the mutex is released
Since we cannot get a new tx buffer in the call back because the mutex is reserved
*/
void
ZCB_AGIReportSendTimer(SSwTimer* pTimer)
{
  SendAssoGroupInfoReport();
  UNUSED(pTimer);
}


/*The AGI report call back we will send a report per association group
  if we seed to send AGI fro all the groups*/
void
ZCB_AGIReport(uint8_t txStatus)
{
  UNUSED(txStatus);
  if (reportStatus == REPORT_STATUS_ALL_GROUPS)
  {
    if (currentGroupId++ < CC_AGI_groupCount_handler(associationGroupInfoGetEndpoint))
    {
      AppTimerRegister(&AGIReportSendTimer, false, ZCB_AGIReportSendTimer);
      TimerStart(&AGIReportSendTimer, 10);
      return;
    }
  }
  reportStatus = REPORT_STATUS_INACTIVE;
}

static received_frame_status_t
CC_AGI_handler(
    RECEIVE_OPTIONS_TYPE_EX *rxOpt,
    ZW_APPLICATION_TX_BUFFER *pCmd,
    uint8_t cmdLength,
    ZW_APPLICATION_TX_BUFFER *pFrameOut,
    uint8_t * pFrameOutLength)
{
  size_t  length;
  uint8_t groupID;
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *txOptionsEx;
  size_t  groupNameLength;
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);

  UNUSED(cmdLength);

  if (true == Check_not_legal_response_job(rxOpt))
  {
    // Get/Report do not support bit addressing.
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  switch (pCmd->ZW_Common.cmd)
    {
      case ASSOCIATION_GROUP_NAME_GET:
        memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

        if (3 != cmdLength)
        {
          return RECEIVED_FRAME_STATUS_FAIL;
        }

        pTxBuf->ZW_AssociationGroupNameReport1byteFrame.cmdClass = COMMAND_CLASS_ASSOCIATION_GRP_INFO;
        pTxBuf->ZW_AssociationGroupNameReport1byteFrame.cmd      = ASSOCIATION_GROUP_NAME_REPORT;

        groupID = pCmd->ZW_AssociationGroupNameGetFrame.groupingIdentifier;
        pTxBuf->ZW_AssociationGroupNameReport1byteFrame.groupingIdentifier = groupID;

        groupNameLength = CC_AGI_groupNameGet_handler(
            (char *)&(pTxBuf->ZW_AssociationGroupNameReport1byteFrame.name1),
            groupID,
            rxOpt->destNode.endpoint);

        pTxBuf->ZW_AssociationGroupNameReport1byteFrame.lengthOfName = (uint8_t)groupNameLength;

        RxToTxOptions(rxOpt, &txOptionsEx);
        if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
            (uint8_t *)pTxBuf,
            sizeof(ZW_ASSOCIATION_GROUP_NAME_REPORT_1BYTE_FRAME) - sizeof(uint8_t) + groupNameLength,
            txOptionsEx,
            NULL))
        {
          /*Job failed*/
          ;
        }
        return RECEIVED_FRAME_STATUS_SUCCESS;
        break;

    case ASSOCIATION_GROUP_INFO_GET:
      // Ignore command if we are already transmitting.
      if (REPORT_STATUS_INACTIVE != reportStatus)
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      if (pCmd->ZW_AssociationGroupInfoGetFrame.properties1 &
          ASSOCIATION_GROUP_INFO_GET_PROPERTIES1_LIST_MODE_BIT_MASK)
      {
        /*if list mode is one then ignore groupid and report information about all the asscoication group
         one group at a time*/
         reportStatus = REPORT_STATUS_ALL_GROUPS;
         currentGroupId = 1;
         associationGroupInfoGetEndpoint = rxOpt->destNode.endpoint;
      }
      else
      {
        /*if list mode is zero and group id is not then report the association group info for the specific group*/
        reportStatus = REPORT_STATUS_ONE_GROUP;
        currentGroupId = pCmd->ZW_AssociationGroupInfoGetFrame.groupingIdentifier;
        associationGroupInfoGetEndpoint = rxOpt->destNode.endpoint;
      }

      if (REPORT_STATUS_INACTIVE != reportStatus)
      {
        memcpy((uint8_t*)&rxOptions, (uint8_t*)rxOpt, sizeof(RECEIVE_OPTIONS_TYPE_EX));
        if (true != SendAssoGroupInfoReport())
        {
          reportStatus = REPORT_STATUS_INACTIVE;
          return RECEIVED_FRAME_STATUS_FAIL;
        }
        else
        {
          return RECEIVED_FRAME_STATUS_SUCCESS;
        }
      }
      return RECEIVED_FRAME_STATUS_FAIL;
      break;

    case ASSOCIATION_GROUP_COMMAND_LIST_GET:
      groupID = pCmd->ZW_AssociationGroupCommandListGetFrame.groupingIdentifier;

      ZAF_CC_AGI_CorrectGroupIdIfInvalid(rxOpt->destNode.endpoint, &groupID);

      bool status = GetApplGroupCommandList(&pFrameOut->ZW_AssociationGroupCommandListReport1byteFrame.command1, &length, groupID, rxOpt->destNode.endpoint);

      if ((true != status) || (0 == length))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      pFrameOut->ZW_AssociationGroupCommandListReport1byteFrame.cmdClass = COMMAND_CLASS_ASSOCIATION_GRP_INFO;
      pFrameOut->ZW_AssociationGroupCommandListReport1byteFrame.cmd      = ASSOCIATION_GROUP_COMMAND_LIST_REPORT;
      pFrameOut->ZW_AssociationGroupCommandListReport1byteFrame.groupingIdentifier = groupID;
      pFrameOut->ZW_AssociationGroupCommandListReport1byteFrame.listLength = (uint8_t)length;

      *pFrameOutLength = (uint8_t)(sizeof(ZW_ASSOCIATION_GROUP_COMMAND_LIST_REPORT_1BYTE_FRAME)
                       - sizeof(uint8_t)
                       + length);

      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    default:
      return RECEIVED_FRAME_STATUS_NO_SUPPORT;
      break;
  }
  return RECEIVED_FRAME_STATUS_SUCCESS;
}

REGISTER_CC_V3(COMMAND_CLASS_ASSOCIATION_GRP_INFO, ASSOCIATION_GRP_INFO_VERSION_V3, CC_AGI_handler, NULL, NULL, NULL, 0);
