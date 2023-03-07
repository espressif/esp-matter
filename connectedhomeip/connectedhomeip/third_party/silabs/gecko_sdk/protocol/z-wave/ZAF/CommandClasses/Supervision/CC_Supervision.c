/**
 * @file
 * Handler for Command Class Supervision.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <CC_Supervision.h>
#include <multichannel.h>
#include <ZW_TransportMulticast.h>
#include <string.h>
#include <ZAF_Common_interface.h>
#include "ZAF_tx_mutex.h"
//#define DEBUGPRINT
#include "DebugPrint.h"
#include "zpal_entropy.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

static uint8_t supervision_session_id = 0;
static uint8_t previously_receive_session_id = 0;
static uint8_t previously_rxStatus = 0;
static MULTICHAN_DEST_NODE_ID previously_received_destination = {0, 0, 0};

static uint8_t m_CommandLength = 0;

static cc_supervision_status_updates_t m_status_updates = CC_SUPERVISION_STATUS_UPDATES_NOT_SUPPORTED;
void (*m_pGetReceivedHandler)(SUPERVISION_GET_RECEIVED_HANDLER_ARGS * pArgs) = NULL;
void (*m_pReportReceivedHandler)(cc_supervision_status_t status, uint8_t duration) = NULL;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.
uint8_t GetSuperVisionSessionID(void)
{
  return supervision_session_id;
}

void SetSuperVisionSessionID(uint8_t value)
{
  supervision_session_id = value;
}
/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

void
CommandClassSupervisionInit( cc_supervision_status_updates_t status_updates,
        void (*pGetReceivedHandler)(SUPERVISION_GET_RECEIVED_HANDLER_ARGS * pArgs),
        void (*pReportReceivedHandler)(cc_supervision_status_t status, uint8_t duration))
{
  m_status_updates = status_updates;
  m_pGetReceivedHandler = pGetReceivedHandler;
  m_pReportReceivedHandler = pReportReceivedHandler;
  supervision_session_id = zpal_get_pseudo_random() % 64;
}

/**
 * Returns whether the command must be handled or not based on Node ID and session ID.
 * @param nodeID Destination Node ID including endpoint.
 * @param sessionID Received session ID.
 * @return Returns true if the command must be handled. false otherwise.
 */
static bool mustCommandBeHandled(MULTICHAN_DEST_NODE_ID nodeID, uint8_t sessionID)
{
  if (previously_receive_session_id != sessionID)
  {
    return true;
  }
  if (0 != memcmp((uint8_t *)&nodeID, (uint8_t *)&previously_received_destination, sizeof(MULTICHAN_DEST_NODE_ID)))
  {
    return true;
  }
  return false;
}

static received_frame_status_t CC_Supervision_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t cmdLength,
  ZW_APPLICATION_TX_BUFFER * pFrameOut,
  uint8_t * pLengthOut)
{

  UNUSED(cmdLength);

  switch (pCmd->ZW_Common.cmd)
  {
    case SUPERVISION_GET:
    {
      /**
       * SUPERVISION_GET handle:
       * 1. Single-cast:
       *    a. Transport_ApplicationCommandHandlerEx() is called by checking previously_receive_session_id.
       *    b. single-cast trigger supervision_report is send back.
       *
       * 2. Multi-cast:
       *    a. If multi-cast is received (rxStatus includes flag RECEIVE_STATUS_TYPE_MULTI).
       *    b. Transport_ApplicationCommandHandlerEx() is called
       *    c. Do not send supervision_report.
       *
       * 3. Multi-cast single-cast follow up:
       *    a. Transport_ApplicationCommandHandlerEx is discarded on single-cast by checking previously_receive_session_id.
       *    b. single-cast trigger supervision_report is send back.
       *
       * 4. Single-cast CC multichannel bit-adr.:
       *    CommandClassMultiChan handle bit addressing by calling each endpoint with the payload.
       *    a. If Single-cast CC multichannel bit-adr. (rxStatus includes flag RECEIVE_STATUS_TYPE_MULTI).
       *    b. Transport_ApplicationCommandHandlerEx() must be called every time. Check previously_received_destination
       *       differ from EXTRACT_SESSION_ID(pCmd->ZW_SupervisionGetFrame.sessionid)
       *    c. Do not send supervision_report.
       */
      uint8_t properties1;
      /*
       * status need to be static to handle multi-cast single-cast follow up.
       * Multi-cast get status Transport_ApplicationCommandHandlerEx() and sing-cast send Supervision report.
       */
      static cc_supervision_status_t status = CC_SUPERVISION_STATUS_NOT_SUPPORTED;
      uint8_t duration;
      TRANSMIT_OPTIONS_TYPE_SINGLE_EX * pTxOptions;

      SetFlagSupervisionEncap(true);

      if(previously_receive_session_id != CC_SUPERVISION_EXTRACT_SESSION_ID(pCmd->ZW_SupervisionGetFrame.properties1))
      {
        /*
         * Reset status session id is changed.
         */
        status = CC_SUPERVISION_STATUS_NOT_SUPPORTED;
      }

      /* Make sure encapsulated CmdClass are supported (including possible endpoints) using current keyclass */
      if (true == ZAF_CC_MultiChannel_IsCCSupported(rxOpt, (ZW_APPLICATION_TX_BUFFER *)(((uint8_t *)pCmd) + sizeof(ZW_SUPERVISION_GET_FRAME))))
      {
        if (true == mustCommandBeHandled(rxOpt->destNode, CC_SUPERVISION_EXTRACT_SESSION_ID(pCmd->ZW_SupervisionGetFrame.properties1)))
        {
          // Fill in supervision data in rxOpt
          rxOpt->bSupervisionActive = 1;
          rxOpt->sessionId = (uint8_t)CC_SUPERVISION_EXTRACT_SESSION_ID(pCmd->ZW_SupervisionGetFrame.properties1);

          /*
           * Ignore bitfield conversion warnings as there is no good solution other than stop
           * using bitfields.
           */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
          rxOpt->statusUpdate = CC_SUPERVISION_EXTRACT_STATUS_UPDATE(pCmd->ZW_SupervisionGetFrame.properties1);

          status = (cc_supervision_status_t)invoke_cc_handler(
              rxOpt,
              (ZW_APPLICATION_TX_BUFFER *)(((uint8_t *)pCmd) + sizeof(ZW_SUPERVISION_GET_FRAME)),
              (pCmd->ZW_SupervisionGetFrame.encapsulatedCommandLength),
              pFrameOut,
              pLengthOut);

          if (CC_SUPERVISION_STATUS_NOT_FOUND == status) {
            status = (cc_supervision_status_t)Transport_ApplicationCommandHandlerEx(
                    rxOpt,
                    (ZW_APPLICATION_TX_BUFFER *)(((uint8_t *)pCmd) + sizeof(ZW_SUPERVISION_GET_FRAME)),
                    (pCmd->ZW_SupervisionGetFrame.encapsulatedCommandLength));
          }
#pragma GCC diagnostic pop
        }
      }
      else
      {
        status = CC_SUPERVISION_STATUS_NOT_SUPPORTED;
      }

      /*
       * Deactivate FlagSupervisionEncap because frame is extracted. (ZF_235)
       */
      SetFlagSupervisionEncap(false);

      if (CC_SUPERVISION_STATUS_CANCEL == status)
      {
        // do nothing.
        return RECEIVED_FRAME_STATUS_SUCCESS;
      }

      /*
       * Check whether the frame was sent using multicast or broadcast. In that case we MUST NOT
       * respond to it.
       * RECEIVE_STATUS_TYPE_MULTI applies in two scenarios: non-securely and Multichannel endpoint
       * bit addressing frames.
       * RECEIVE_STATUS_TYPE_BROAD applies in the case of S2 multicast frames.
       */
      if ((rxOpt->rxStatus & RECEIVE_STATUS_TYPE_MULTI) ||
          (rxOpt->rxStatus & RECEIVE_STATUS_TYPE_BROAD))      {
        if (false == mustCommandBeHandled(rxOpt->destNode, CC_SUPERVISION_EXTRACT_SESSION_ID(pCmd->ZW_SupervisionGetFrame.properties1)))
        {
          return RECEIVED_FRAME_STATUS_FAIL;
        }

        /*update previously session-id and reset previously_received_destination [node-id + endpoint]*/
        previously_receive_session_id = CC_SUPERVISION_EXTRACT_SESSION_ID(pCmd->ZW_SupervisionGetFrame.properties1);
        memcpy((uint8_t*)&previously_received_destination, (uint8_t*)&rxOpt->destNode, sizeof(MULTICHAN_DEST_NODE_ID));
        previously_rxStatus = rxOpt->rxStatus;

        return RECEIVED_FRAME_STATUS_SUCCESS;
      }
      else
      {
        /*
         * In this case the frame is sent using singlecast.
         *
         * We cannot respond to a singlecast in the following scenarios:
         * - Session ID is unchanged from last singlecast
         */
          if ((false == mustCommandBeHandled(rxOpt->destNode, CC_SUPERVISION_EXTRACT_SESSION_ID(pCmd->ZW_SupervisionGetFrame.properties1))) &&
              (0 == previously_rxStatus))        {
          return RECEIVED_FRAME_STATUS_FAIL;
        }
      }
      /*update previously session-id and reset previously_received_destination [node-id + endpoint]*/
      previously_receive_session_id = CC_SUPERVISION_EXTRACT_SESSION_ID(pCmd->ZW_SupervisionGetFrame.properties1);
      memcpy((uint8_t*)&previously_received_destination, (uint8_t*)&rxOpt->destNode, sizeof(MULTICHAN_DEST_NODE_ID));
      previously_rxStatus = 0;

      duration = 0;
      properties1 = CC_SUPERVISION_EXTRACT_SESSION_ID(pCmd->ZW_SupervisionGetFrame.properties1);
      properties1 |= CC_SUPERVISION_ADD_MORE_STATUS_UPDATE(CC_SUPERVISION_MORE_STATUS_UPDATES_THIS_IS_LAST);
      if ((CC_SUPERVISION_STATUS_FAIL != status) &&
          (CC_SUPERVISION_STATUS_NOT_SUPPORTED != status) &&
          NON_NULL(m_pGetReceivedHandler))
      {
        // Call the assigned function.
        SUPERVISION_GET_RECEIVED_HANDLER_ARGS args;

        args.cmdClass = *(((uint8_t *)pCmd) + sizeof(ZW_SUPERVISION_GET_FRAME));
        args.cmd      = *(((uint8_t *)pCmd) + sizeof(ZW_SUPERVISION_GET_FRAME) + 1);
        args.properties1 = pCmd->ZW_SupervisionGetFrame.properties1;
        args.rxOpt = rxOpt;
        args.status = status;
        args.duration = 0;

        m_pGetReceivedHandler(&args);

        status = args.status;
        duration = args.duration;
        properties1 = args.properties1;
      }

      /*
       * When we have gotten the information, we can send a Supervision report, but only if we are
       * allowed to.
       */
      if (false == Check_not_legal_response_job(rxOpt)) {
        RxToTxOptions(rxOpt, &pTxOptions);
        CmdClassSupervisionReportSend(
                pTxOptions,
                properties1,
                status,
                duration);
      }
    }
    return RECEIVED_FRAME_STATUS_SUCCESS;
    break;

    case SUPERVISION_REPORT:
      if (supervision_session_id == pCmd->ZW_SupervisionReportFrame.properties1)
      {
        // The received session ID matches the one we sent.
        if (NON_NULL(m_pReportReceivedHandler))
        {
          m_pReportReceivedHandler(
                  pCmd->ZW_SupervisionReportFrame.status,
                  pCmd->ZW_SupervisionReportFrame.duration);
        }

        ZW_TransportMulticast_clearTimeout();
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;
  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}

JOB_STATUS
CmdClassSupervisionReportSend(
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX* pTxOptionsEx,
  uint8_t properties,
  cc_supervision_status_t status,
  uint8_t duration)
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

  pTxBuf->ZW_SupervisionReportFrame.cmdClass = COMMAND_CLASS_SUPERVISION;
  pTxBuf->ZW_SupervisionReportFrame.cmd = SUPERVISION_REPORT;
  pTxBuf->ZW_SupervisionReportFrame.properties1 = properties;
  pTxBuf->ZW_SupervisionReportFrame.status = status;
  pTxBuf->ZW_SupervisionReportFrame.duration = duration;

  if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
      (uint8_t *)pTxBuf,
      sizeof(ZW_SUPERVISION_REPORT_FRAME),
      pTxOptionsEx,
      NULL))
  {
    /*Job failed */
     return JOB_STATUS_BUSY;
  }
  return JOB_STATUS_SUCCESS;
}

void CommandClassSupervisionGetAdd(ZW_SUPERVISION_GET_FRAME* pbuf)
{
  m_CommandLength = 0;
  CommandClassSupervisionGetWrite(pbuf);
  supervision_session_id = (uint8_t)((supervision_session_id + 1) % 64); /* increment m_sessionId, wrap around if over 0x3F */
}

void CommandClassSupervisionGetWrite(ZW_SUPERVISION_GET_FRAME* pbuf)
{
  pbuf->cmdClass =  COMMAND_CLASS_SUPERVISION;
  pbuf->cmd = SUPERVISION_GET;

  pbuf->properties1 = CC_SUPERVISION_ADD_SESSION_ID(supervision_session_id);
  pbuf->properties1 |= (uint8_t)CC_SUPERVISION_ADD_STATUS_UPDATE(m_status_updates);
  pbuf->encapsulatedCommandLength = m_CommandLength;
}

void CommandClassSupervisionGetSetPayloadLength(ZW_SUPERVISION_GET_FRAME* pbuf, uint8_t payLoadlen)
{
  pbuf->encapsulatedCommandLength = payLoadlen;
  m_CommandLength = payLoadlen;
}

uint8_t CommandClassSupervisionGetGetPayloadLength(ZW_SUPERVISION_GET_FRAME* pbuf)
{
  return pbuf->encapsulatedCommandLength;
}

REGISTER_CC_V2(COMMAND_CLASS_SUPERVISION, SUPERVISION_VERSION, CC_Supervision_handler);
