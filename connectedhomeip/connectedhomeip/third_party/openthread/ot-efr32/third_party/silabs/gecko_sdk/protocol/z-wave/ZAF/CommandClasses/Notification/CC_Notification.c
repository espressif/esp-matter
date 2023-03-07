/**
 * @file
 * Handler for Command Class Notification.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <CC_Notification.h>
#include <ZW_basis_api.h>
#include <ZW_TransportLayer.h>

#include "config_app.h"
#include <association_plus.h>
#include <ZW_TransportMulticast.h>
#include <string.h>
#include <ZAF_TSE.h>
#include "ZAF_tx_mutex.h"

//#define DEBUGPRINT
#include "DebugPrint.h"
/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/
//static  ZW_APPLICATION_TX_BUFFER *pTxBuf;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

/*==============================   handleCommandClassNotification  ============
**
**  Function:  handler for Notification CC
**
**  Side effects: None
**
**--------------------------------------------------------------------------*/
static received_frame_status_t
CC_Notification_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt, /* IN receive options of type RECEIVE_OPTIONS_TYPE_EX  */
  ZW_APPLICATION_TX_BUFFER *pCmd, /* IN  Payload from the received frame */
  uint8_t cmdLength)               /* IN Number of command bytes including the command */
{
  size_t size;
//  UNUSED(cmdLength);
  DPRINTF("CmdClassAlarm %d\r\n", pCmd->ZW_Common.cmd);
  switch (pCmd->ZW_Common.cmd)
  {
    case NOTIFICATION_SET_V4:
      {
        uint8_t tempEndpoint =  rxOpt->destNode.endpoint;
        /*Validate endpoint! Change endpoint if it is not valid and root-endpoint*/
        if( false == FindNotificationEndpoint((notification_type_t)pCmd->ZW_NotificationSetV4Frame.notificationType, &tempEndpoint))
        {
          /* Not valid endpoint!*/
          return RECEIVED_FRAME_STATUS_FAIL;
        }

        if( 0x00 == pCmd->ZW_NotificationSetV4Frame.notificationStatus ||
            0xff == pCmd->ZW_NotificationSetV4Frame.notificationStatus)
        {
          if (E_CMD_HANDLER_RETURN_CODE_FAIL == handleAppNotificationSet(
              (notification_type_t)pCmd->ZW_NotificationSetV4Frame.notificationType,
              (NOTIFICATION_STATUS_SET)pCmd->ZW_NotificationSetV4Frame.notificationStatus,
              tempEndpoint))
          {
            return RECEIVED_FRAME_STATUS_FAIL;
          }
        }
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case NOTIFICATION_GET_V4:
      if(false == Check_not_legal_response_job(rxOpt))
      {
        ZAF_TRANSPORT_TX_BUFFER  TxBuf;
        ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
        memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

        uint8_t tempEndpoint =  rxOpt->destNode.endpoint;
        TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
        RxToTxOptions(rxOpt, &pTxOptionsEx);

        pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType = pCmd->ZW_NotificationGetV4Frame.notificationType;
        if(3 == cmdLength)
        {
          DPRINT("ZW_ALARM_GET_V1_FRAME");
          pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType = 0xFF;
        }

        DPRINT("ZW_ALARM_GET_V4_FRAME");
        /*Validate endpoint! Change endpoint if it is not valid and root-endpoint*/
        if( false == FindNotificationEndpoint((notification_type_t)pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType, &tempEndpoint))
        {
          /* Not valid endpoint!*/
          /*Job failed */
          return RECEIVED_FRAME_STATUS_FAIL;
        }

        pTxBuf->ZW_NotificationReport1byteV4Frame.cmdClass = COMMAND_CLASS_NOTIFICATION_V4;
        pTxBuf->ZW_NotificationReport1byteV4Frame.cmd = NOTIFICATION_REPORT_V4;
        pTxBuf->ZW_NotificationReport1byteV4Frame.v1AlarmType = 0; /*must be set to 0*/
        pTxBuf->ZW_NotificationReport1byteV4Frame.v1AlarmLevel = 0; /*must be set to 0*/
        pTxBuf->ZW_NotificationReport1byteV4Frame.reserved = 0; /*must be set to 0*/
        pTxBuf->ZW_NotificationReport1byteV4Frame.notificationStatus =
          CmdClassNotificationGetNotificationStatus( (notification_type_t)pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType, tempEndpoint );
        pTxBuf->ZW_NotificationReport1byteV4Frame.properties1 = 0;
        pTxBuf->ZW_NotificationReport1byteV4Frame.eventParameter1 = 0;
        pTxBuf->ZW_NotificationReport1byteV4Frame.mevent = 0;

        if(3 == cmdLength)
        {
          pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType = 0;
          size = sizeof(ZW_NOTIFICATION_REPORT_1BYTE_V4_FRAME) - 2; //Remove event-parameter and sequence number
        }
        else if(5 > cmdLength) /*CC  V2*/
        {
          DPRINT("CC V2");
          size = 0;
        }
        else{
          pTxBuf->ZW_NotificationReport1byteV4Frame.mevent = pCmd->ZW_NotificationGetV4Frame.mevent;
          size = 0;
        }


        if( 0xff == pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType && 0x00 == pTxBuf->ZW_NotificationReport1byteV4Frame.mevent)
        {
          /* In response to a Notification Get (Notification Type = 0xFF) , a responding device MUST return
             a pending notification from its internal list (Pull mode). We also do it for Push mode.*/

          notification_type_t notificationType = (notification_type_t)pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType;
          uint8_t grp = GetGroupIndex(&notificationType,tempEndpoint);

          pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType = (uint8_t)notificationType;
          if(0xff == grp)
          {
            //DPRINT(" STATUS 0xFE");
            //pTxBuf->ZW_NotificationReport1byteV4Frame.notificationStatus = 0xFE;
            //size = sizeof(ZW_NOTIFICATION_REPORT_1BYTE_V4_FRAME) - 5;
            /*We do not support alarmType!*/
            /*Job failed */
            return RECEIVED_FRAME_STATUS_FAIL;
          }
        }

        if(0 == size)
        {
          if(true == CmdClassNotificationGetNotificationEvent( (notification_type_t *)&pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType,
                                                          &pTxBuf->ZW_NotificationReport1byteV4Frame.mevent,
                                                          &(pTxBuf->ZW_NotificationReport1byteV4Frame.eventParameter1),
                                                          &(pTxBuf->ZW_NotificationReport1byteV4Frame.properties1),
                                                          tempEndpoint))
          {
            size = (sizeof(ZW_NOTIFICATION_REPORT_1BYTE_V4_FRAME) - sizeof(uint8_t) +
                (pTxBuf->ZW_NotificationReport1byteV4Frame.properties1 & NOTIFICATION_REPORT_PROPERTIES1_EVENT_PARAMETERS_LENGTH_MASK_V4)) -
                sizeof(uint8_t); /* Removed sequence number*/
          }
          else{
            /*We do not support alarmType!*/
            /*Job failed */
            return RECEIVED_FRAME_STATUS_FAIL;
          }
        }

        if (size)
        {
          if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
              (uint8_t *)pTxBuf,
              size,
              pTxOptionsEx,
              NULL))
          {
            /*Job failed */
            ;
          }
          return RECEIVED_FRAME_STATUS_SUCCESS;
        }
        else
        {
          /*Size 0 */
          ;
        }
      }
      return RECEIVED_FRAME_STATUS_FAIL;
      break;

    case NOTIFICATION_SUPPORTED_GET_V4:
      if(false == Check_not_legal_response_job(rxOpt))
      {
        ZAF_TRANSPORT_TX_BUFFER  TxBuf;
        ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
        memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

        TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
        RxToTxOptions(rxOpt, &pTxOptionsEx);
        pTxBuf->ZW_NotificationSupportedReport1byteV4Frame.cmdClass = COMMAND_CLASS_NOTIFICATION_V4;
        pTxBuf->ZW_NotificationSupportedReport1byteV4Frame.cmd = NOTIFICATION_SUPPORTED_REPORT_V4;
        handleCmdClassNotificationSupportedReport(&(pTxBuf->ZW_NotificationSupportedReport1byteV4Frame.properties1),
                                             &(pTxBuf->ZW_NotificationSupportedReport1byteV4Frame.bitMask1),
                                             NOTIFICATION_BITMASK_ARRAY_LENGTH,
                                             rxOpt->destNode.endpoint);

        /*
         * Requirement CC:0071.03.08.11.001:
         * MSb of properties1 tells whether a node supports Alarm CC V1. This implementation of
         * Notification CC does not support Alarm CC V1, hence this field is set to zero.
         */
        pTxBuf->ZW_NotificationSupportedReport1byteV4Frame.properties1 &= 0x7F;

        if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP((uint8_t *)pTxBuf,
                                         sizeof(ZW_NOTIFICATION_SUPPORTED_REPORT_1BYTE_V4_FRAME) - 1 +
                                         pTxBuf->ZW_NotificationSupportedReport1byteV4Frame.properties1,
                                         pTxOptionsEx, NULL))
        {
          /*Job failed */
          ;
        }
        return RECEIVED_FRAME_STATUS_SUCCESS;
      }
      return RECEIVED_FRAME_STATUS_FAIL;
      break;

    case EVENT_SUPPORTED_GET_V4:
      if(false == Check_not_legal_response_job(rxOpt))
      {
        ZAF_TRANSPORT_TX_BUFFER  TxBuf;
        ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
        memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

        TRANSMIT_OPTIONS_TYPE_SINGLE_EX* txOptionsEx;
        RxToTxOptions(rxOpt, &txOptionsEx);
        pTxBuf->ZW_EventSupportedReport1byteV4Frame.cmdClass = COMMAND_CLASS_NOTIFICATION_V4;
        pTxBuf->ZW_EventSupportedReport1byteV4Frame.cmd = EVENT_SUPPORTED_REPORT_V4;
        pTxBuf->ZW_EventSupportedReport1byteV4Frame.notificationType = pCmd->ZW_EventSupportedGetV4Frame.notificationType;
        pTxBuf->ZW_EventSupportedReport1byteV4Frame.properties1 = 0;
        handleCmdClassNotificationEventSupportedReport(
                            (notification_type_t)pTxBuf->ZW_EventSupportedReport1byteV4Frame.notificationType,
                            &(pTxBuf->ZW_EventSupportedReport1byteV4Frame.properties1),
                            &(pTxBuf->ZW_EventSupportedReport1byteV4Frame.bitMask1),
                            rxOpt->destNode.endpoint);

        pTxBuf->ZW_EventSupportedReport1byteV4Frame.properties1 &= 0x7F;

        if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP( (uint8_t *)pTxBuf,
                                         sizeof(ZW_EVENT_SUPPORTED_REPORT_1BYTE_V4_FRAME) - 1 +
                                         (pTxBuf->ZW_EventSupportedReport1byteV4Frame.properties1 & 0x1F), /*remove reserved bits*/
                                         txOptionsEx, NULL))
        {
          /*Job failed */
          ;
        }
        return RECEIVED_FRAME_STATUS_SUCCESS;
      }
      return RECEIVED_FRAME_STATUS_FAIL;
      break;

    default:
      break;
  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}

JOB_STATUS CmdClassNotificationReport(
  AGI_PROFILE const * const pProfile,
  uint8_t sourceEndpoint,
  notification_type_t notificationType,
  uint8_t notificationEvent,
  VOID_CALLBACKFUNC(pCallback)(TRANSMISSION_RESULT * pTransmissionResult))
{
  ZW_APPLICATION_TX_BUFFER *pTxBuf = GetRequestBuffer(pCallback);
 if( IS_NULL( pTxBuf ) )
  {
    /*Ongoing job is active.. just stop current job*/
    return JOB_STATUS_BUSY;
  }
  else
  {
    TRANSMIT_OPTIONS_TYPE_EX* pTxOptionsEx = NULL;

    pTxBuf->ZW_NotificationReport1byteV4Frame.cmdClass = COMMAND_CLASS_NOTIFICATION_V4;
    pTxBuf->ZW_NotificationReport1byteV4Frame.cmd = NOTIFICATION_REPORT_V4;
    pTxBuf->ZW_NotificationReport1byteV4Frame.v1AlarmType = 0;
    pTxBuf->ZW_NotificationReport1byteV4Frame.v1AlarmLevel = 0;
    pTxBuf->ZW_NotificationReport1byteV4Frame.reserved = 0;
    pTxBuf->ZW_NotificationReport1byteV4Frame.notificationStatus = NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED;
    notification_type_t tempNotificationType = notificationType;
    pTxBuf->ZW_NotificationReport1byteV4Frame.mevent = notificationEvent;

    CmdClassNotificationGetNotificationEvent( &tempNotificationType,
                                              &(pTxBuf->ZW_NotificationReport1byteV4Frame.mevent),
                                              &(pTxBuf->ZW_NotificationReport1byteV4Frame.eventParameter1),
                                              &(pTxBuf->ZW_NotificationReport1byteV4Frame.properties1),
                                              sourceEndpoint);

    pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType = (uint8_t)tempNotificationType;
    pTxBuf->ZW_NotificationReport1byteV4Frame.properties1 &= NOTIFICATION_REPORT_PROPERTIES1_EVENT_PARAMETERS_LENGTH_MASK_V4; /*remove sequence number and reserved flags*/

    uint8_t dataLength = sizeof(ZW_NOTIFICATION_REPORT_1BYTE_V4_FRAME) - sizeof(uint8_t) +
                         pTxBuf->ZW_NotificationReport1byteV4Frame.properties1;

    /* Remove sequence number if Sequence flag is not set*/
    if (!(NOTIFICATION_REPORT_PROPERTIES1_SEQUENCE_BIT_MASK_V4 & pTxBuf->ZW_NotificationReport1byteV4Frame.properties1))
    {
      dataLength -= (uint8_t)sizeof(uint8_t);
    }

    /*Get transmit options (node list)*/
    pTxOptionsEx = ReqNodeList( pProfile,
                          (CMD_CLASS_GRP*) &(pTxBuf->ZW_Common.cmdClass),
                          sourceEndpoint);
    if( IS_NULL( pTxOptionsEx ) )
    {
      /*Job failed, free transmit-buffer pTxBuf by cleaning mutex */
      FreeRequestBuffer();
      return JOB_STATUS_NO_DESTINATIONS;
    }

    if (ETRANSPORTMULTICAST_ADDED_TO_QUEUE != ZW_TransportMulticast_SendRequest(
        (uint8_t *)pTxBuf,
        dataLength,
        true,
        pTxOptionsEx,
        ZCB_RequestJobStatus))
    {
      /*Job failed, free transmit-buffer pTxBuf by cleaning mutex */
      FreeRequestBuffer();
      return JOB_STATUS_BUSY;
    }
  }
  return JOB_STATUS_SUCCESS;
}

void CC_Notification_report_stx(
    TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions,
    void * pData)
{
  DPRINTF("* %s() *\n"
      "\ttxOpt.src = %d\n"
      "\ttxOpt.options %#02x\n"
      "\ttxOpt.secOptions %#02x\n",
      __func__, txOptions.sourceEndpoint, txOptions.txOptions, txOptions.txSecOptions);

  /* Prepare payload for report */
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );
  s_CC_notification_data_t* pNotificationData = (s_CC_notification_data_t*)pData;

  pTxBuf->ZW_NotificationReport1byteV4Frame.cmdClass = COMMAND_CLASS_NOTIFICATION_V4;
  pTxBuf->ZW_NotificationReport1byteV4Frame.cmd = NOTIFICATION_REPORT_V4;
  pTxBuf->ZW_NotificationReport1byteV4Frame.v1AlarmType = 0;
  pTxBuf->ZW_NotificationReport1byteV4Frame.v1AlarmLevel = 0;
  pTxBuf->ZW_NotificationReport1byteV4Frame.reserved = 0;
  pTxBuf->ZW_NotificationReport1byteV4Frame.notificationStatus = NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED;

  notification_type_t tempNotificationType = pNotificationData->notificationType;
  pTxBuf->ZW_NotificationReport1byteV4Frame.mevent = pNotificationData->notificationEvent;

  CmdClassNotificationGetNotificationEvent( &tempNotificationType,
                                            &(pTxBuf->ZW_NotificationReport1byteV4Frame.mevent),
                                            &(pTxBuf->ZW_NotificationReport1byteV4Frame.eventParameter1),
                                            &(pTxBuf->ZW_NotificationReport1byteV4Frame.properties1),
                                            pNotificationData->sourceEndpoint);

  pTxBuf->ZW_NotificationReport1byteV4Frame.notificationType = (uint8_t)tempNotificationType;
  pTxBuf->ZW_NotificationReport1byteV4Frame.properties1 &= NOTIFICATION_REPORT_PROPERTIES1_EVENT_PARAMETERS_LENGTH_MASK_V4; /*remove sequence number and reserved flags*/

  size_t dataLength = sizeof(ZW_NOTIFICATION_REPORT_1BYTE_V4_FRAME) - sizeof(uint8_t) +
                       pTxBuf->ZW_NotificationReport1byteV4Frame.properties1;

  /* Remove sequence number if Sequence flag is not set*/
  if (!(NOTIFICATION_REPORT_PROPERTIES1_SEQUENCE_BIT_MASK_V4 & pTxBuf->ZW_NotificationReport1byteV4Frame.properties1))
  {
    dataLength -= sizeof(uint8_t);
  }

  if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendRequestEP((uint8_t *)pTxBuf,
                                                            dataLength,
                                                            &txOptions,
                                                            ZAF_TSE_TXCallback))
  {
    //sending request failed
    DPRINTF("%s(): Transport_SendRequestEP() failed. \n", __func__);
  }
}

static uint8_t lifeline_reporting(ccc_pair_t * p_ccc_pair)
{
  p_ccc_pair->cmdClass = COMMAND_CLASS_NOTIFICATION_V8;
  p_ccc_pair->cmd      = NOTIFICATION_REPORT_V8;
  return 1;
}

REGISTER_CC_V3(COMMAND_CLASS_NOTIFICATION_V8, NOTIFICATION_VERSION_V8, CC_Notification_handler, NULL, NULL, lifeline_reporting, 0);
