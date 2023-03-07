/**
 * @file
 * @brief Handler for Command Class User Code.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_TransportLayer.h>

#include "config_app.h"
#include <CC_UserCode.h>
#include <string.h>
#include "ZAF_tx_mutex.h"

//#define DEBUGPRINT
#include "DebugPrint.h"
#include <ZAF_TSE.h>

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef struct
{
  uint8_t userIdentifier;
  uint8_t userIdStatus;
  uint8_t userCode[10];
}
user_code_report_t;

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

s_CC_userCode_data_t userCodeData;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/


static received_frame_status_t
CC_UserCode_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t cmdLength)
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER * pTxBuf = &(TxBuf.appTxBuf);
  switch (pCmd->ZW_Common.cmd)
  {
    case USER_CODE_GET:
      if(true == Check_not_legal_response_job(rxOpt))
      {
        /*Get/Report do not support endpoint bit-addressing */
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      size_t len = 0;
      uint8_t maxNumberOfUsers = CC_UserCode_UsersNumberReport_handler(rxOpt->destNode.endpoint);
      TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
      RxToTxOptions(rxOpt, &pTxOptionsEx);

      pTxBuf->ZW_UserCodeReport1byteFrame.cmdClass = COMMAND_CLASS_USER_CODE;
      pTxBuf->ZW_UserCodeReport1byteFrame.cmd = USER_CODE_REPORT;
      pTxBuf->ZW_UserCodeReport1byteFrame.userIdentifier = pCmd->ZW_UserCodeGetFrame.userIdentifier;

      if((0 == pCmd->ZW_UserCodeGetFrame.userIdentifier)
         || (pCmd->ZW_UserCodeGetFrame.userIdentifier > maxNumberOfUsers))
      {
        pTxBuf->ZW_UserCodeReport1byteFrame.userIdStatus = USER_ID_NO_STATUS;
      }
      else
      {
        if(false == CC_UserCode_getId_handler(
            pCmd->ZW_UserCodeGetFrame.userIdentifier,
            (user_id_status_t*)&(pTxBuf->ZW_UserCodeReport1byteFrame.userIdStatus),
            rxOpt->destNode.endpoint))
        {
          return RECEIVED_FRAME_STATUS_FAIL;
        }

        if(false == CC_UserCode_Report_handler(
            pCmd->ZW_UserCodeGetFrame.userIdentifier,
            &(pTxBuf->ZW_UserCodeReport1byteFrame.userCode1),
            &len,
            rxOpt->destNode.endpoint))
        {
          /*Job failed */
          return RECEIVED_FRAME_STATUS_FAIL; /*failing*/
        }
      }

      if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
                  (uint8_t *)pTxBuf,
                  sizeof(ZW_USER_CODE_REPORT_1BYTE_FRAME) + len - 1,
                  pTxOptionsEx,
                  NULL))
      {
        return RECEIVED_FRAME_STATUS_FAIL; /*failing*/
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;

      break;

    case USER_CODE_SET:
      {
        bool status = true;
        uint16_t i;
        uint8_t user_code_length = cmdLength - 4;

        if (!((pCmd->ZW_UserCodeSet1byteFrame.userIdentifier <= CC_UserCode_UsersNumberReport_handler(rxOpt->destNode.endpoint)) &&
            ((USERCODE_MIN_LEN <= user_code_length) && (USERCODE_MAX_LEN >= user_code_length)))
          )
        {
          return RECEIVED_FRAME_STATUS_FAIL;
        }

        switch (pCmd->ZW_UserCodeSet1byteFrame.userIdStatus)
        {
          case USER_ID_AVAILBLE:
            for (i = 0; i < 4; i++)
            {
              *(&pCmd->ZW_UserCodeSet1byteFrame.userCode1 + i) = 0x00;
            }
            user_code_length = 4;
            break;

          case USER_ID_OCCUPIED:
          case USER_ID_RESERVED:
            // Validate user code are digits
            for(i = 0; i < user_code_length; i++)
            {
              if( ((0x30 > (uint8_t)*(&pCmd->ZW_UserCodeSet1byteFrame.userCode1 + i)) || (0x39 < (uint8_t)*(&pCmd->ZW_UserCodeSet1byteFrame.userCode1 + i))))
              {
                status = false;
                break;
              }
            }
            break;

          default:
            return RECEIVED_FRAME_STATUS_FAIL;
            break;
        }

        if (true == status)
        {
          e_cmd_handler_return_code_t return_code;
          if (0 == pCmd->ZW_UserCodeSet1byteFrame.userIdentifier)
          {
            uint8_t max_users = CC_UserCode_UsersNumberReport_handler(rxOpt->destNode.endpoint);
            for (uint8_t user_id = 1; user_id <= max_users; user_id++)
            {
              return_code = CC_UserCode_Set_handler(user_id,
                                                    pCmd->ZW_UserCodeSet1byteFrame.userIdStatus,
                                                    &(pCmd->ZW_UserCodeSet1byteFrame.userCode1),
                                                    user_code_length,
                                                    rxOpt->destNode.endpoint);

              if (E_CMD_HANDLER_RETURN_CODE_FAIL == return_code || E_CMD_HANDLER_RETURN_CODE_HANDLED == return_code)
              {
                // Build up new CC data structure
                memset(&userCodeData, 0, sizeof(s_CC_userCode_data_t));
                userCodeData.rxOptions = *rxOpt;
                userCodeData.userIdentifier = user_id;

                /* We cannot know if the same User Identifier was modified several times
                or not, so do not overwrite_previous_trigger. Also we want to know if a
                User Code was updated, even for a very short time */
                if (false == ZAF_TSE_Trigger(CC_UserCode_report_stx,
                                             (void*)&userCodeData,
                                             false))
                {
                  DPRINTF("%s(): ZAF_TSE_Trigger failed\n", __func__);
                }
              }

              if (E_CMD_HANDLER_RETURN_CODE_FAIL == return_code)
              {
                return RECEIVED_FRAME_STATUS_FAIL;
              }
            }
            return RECEIVED_FRAME_STATUS_SUCCESS;
          }
          else
          {
            return_code = CC_UserCode_Set_handler(pCmd->ZW_UserCodeSet1byteFrame.userIdentifier,
                                                  pCmd->ZW_UserCodeSet1byteFrame.userIdStatus,
                                                  &(pCmd->ZW_UserCodeSet1byteFrame.userCode1),
                                                  user_code_length,
                                                  rxOpt->destNode.endpoint);

            if (E_CMD_HANDLER_RETURN_CODE_FAIL == return_code || E_CMD_HANDLER_RETURN_CODE_HANDLED == return_code)
            {
              // Build up new CC data structure
              memset(&userCodeData, 0, sizeof(s_CC_userCode_data_t));
              userCodeData.rxOptions = *rxOpt;
              userCodeData.userIdentifier = pCmd->ZW_UserCodeGetFrame.userIdentifier;
              /* We cannot know if the same User Identifier was modified several times
                or not, so do not overwrite_previous_trigger. Also we want to know if a
                User Code was updated, even for a very short time */
              if (false == ZAF_TSE_Trigger(CC_UserCode_report_stx,
                                           (void*)&userCodeData,
                                           false))
              {
                DPRINTF("%s(): ZAF_TSE_Trigger failed\n", __func__);
              }
            }

            if (E_CMD_HANDLER_RETURN_CODE_HANDLED == return_code)
            {
              return RECEIVED_FRAME_STATUS_SUCCESS;
            }
          }
        }
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      break;

    case USERS_NUMBER_GET:
      if(false == Check_not_legal_response_job(rxOpt))
      {
    	memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

        TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
        RxToTxOptions(rxOpt, &pTxOptionsEx);
        pTxBuf->ZW_UsersNumberReportFrame.cmdClass = COMMAND_CLASS_USER_CODE;
        pTxBuf->ZW_UsersNumberReportFrame.cmd = USERS_NUMBER_REPORT;
        pTxBuf->ZW_UsersNumberReportFrame.supportedUsers =
          CC_UserCode_UsersNumberReport_handler( rxOpt->destNode.endpoint );

        if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
                      (uint8_t *)pTxBuf,
                      sizeof(ZW_USERS_NUMBER_REPORT_FRAME),
                      pTxOptionsEx,
                      NULL))
        {
          /*Job failed */
          return RECEIVED_FRAME_STATUS_FAIL;
        }
        return RECEIVED_FRAME_STATUS_SUCCESS;
      }
      return RECEIVED_FRAME_STATUS_FAIL;
      break;
  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}

JOB_STATUS CC_UserCode_SupportReport(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  uint8_t userIdentifier,
  uint8_t userIdStatus,
  uint8_t* pUserCode,
  uint8_t userCodeLen,
  VOID_CALLBACKFUNC(pCallback)(TRANSMISSION_RESULT * pTransmissionResult))
{
  CMD_CLASS_GRP cmdGrp = {COMMAND_CLASS_USER_CODE, USER_CODE_REPORT};
  user_code_report_t user_code_report;

  if ((0 == userIdentifier) || IS_NULL(pUserCode) ||
      (userCodeLen > USERCODE_MAX_LEN) || (userCodeLen < USERCODE_MIN_LEN) ||
      (2 < userIdStatus))
  {
    return JOB_STATUS_BUSY;
  }

  user_code_report.userIdentifier = userIdentifier;
  user_code_report.userIdStatus = userIdStatus;
  memcpy(user_code_report.userCode, pUserCode, userCodeLen);

  return cc_engine_multicast_request(
      pProfile,
      sourceEndpoint,
      &cmdGrp,
      (uint8_t*)&user_code_report,
      sizeof(user_code_report_t) + userCodeLen - USERCODE_MAX_LEN,
      false,
      pCallback);
}

void CC_UserCode_report_stx(
    TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions,
    void* pData)
{
  DPRINTF("* %s() *\n"
      "\ttxOpt.src = %d\n"
      "\ttxOpt.options %#02x\n"
      "\ttxOpt.secOptions %#02x\n",
      __func__, txOptions.sourceEndpoint, txOptions.txOptions, txOptions.txSecOptions);

  /* Prepare payload for report */
  size_t len;
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );
  s_CC_userCode_data_t *pUserCodeData = (s_CC_userCode_data_t*)pData;

  pTxBuf->ZW_UserCodeReport1byteFrame.cmdClass = COMMAND_CLASS_USER_CODE;
  pTxBuf->ZW_UserCodeReport1byteFrame.cmd = USER_CODE_REPORT;
  pTxBuf->ZW_UserCodeReport1byteFrame.userIdentifier = pUserCodeData->userIdentifier;

  if(false == CC_UserCode_getId_handler(
      pUserCodeData->userIdentifier,
      (user_id_status_t*)&(pTxBuf->ZW_UserCodeReport1byteFrame.userIdStatus),
      pUserCodeData->rxOptions.destNode.endpoint))
  {
    DPRINTF("%s(): CC_UserCode_getId_handler() failed. \n", __func__);
    return;

  }

  if(false == CC_UserCode_Report_handler(
      pUserCodeData->userIdentifier,
      &(pTxBuf->ZW_UserCodeReport1byteFrame.userCode1),
      &len,
      pUserCodeData->rxOptions.destNode.endpoint))
  {
    /*Job failed */
    DPRINTF("%s(): CC_UserCode_Report_handler() failed. \n", __func__);
    return;
  }

  if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendRequestEP((uint8_t *)pTxBuf,
                                                                  sizeof(ZW_USER_CODE_REPORT_1BYTE_FRAME) + len - 1,
                                                                  &txOptions,
                                                                  ZAF_TSE_TXCallback))
  {
    //sending request failed
    DPRINTF("%s(): Transport_SendRequestEP() failed. \n", __func__);
  }
}

static uint8_t lifeline_reporting(ccc_pair_t * p_ccc_pair)
{
  p_ccc_pair->cmdClass = COMMAND_CLASS_USER_CODE;
  p_ccc_pair->cmd      = USER_CODE_REPORT;
  return 1;
}

REGISTER_CC_V3(COMMAND_CLASS_USER_CODE, USER_CODE_VERSION, CC_UserCode_handler, NULL, NULL, lifeline_reporting, 0);
