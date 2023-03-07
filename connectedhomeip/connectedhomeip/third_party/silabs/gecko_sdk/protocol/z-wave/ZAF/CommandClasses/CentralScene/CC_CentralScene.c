/**
 * @file CC_CentralScene.c
 * @brief Handler for Command Class Central Scene.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_TransportLayer.h>
#include "config_app.h"
#include <CC_CentralScene.h>
#include <string.h>
#include "ZAF_tx_mutex.h"

//#define DEBUGPRINT
#include "DebugPrint.h"
#include <ZAF_TSE.h>

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

struct s_CC_centralScene_data_t_ centralSceneData;
static void prepare_configuration_report(ZW_APPLICATION_TX_BUFFER *pTxBuffer);

static received_frame_status_t
CC_CentralScene_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t cmdLength)
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX * pTxOptionsEx;
  size_t numberOfBytesWithKeyAttributes;
  UNUSED(cmdLength);

  switch (pCmd->ZW_Common.cmd)
  {
    case CENTRAL_SCENE_SUPPORTED_GET_V2:
      if (false == Check_not_legal_response_job(rxOpt))
      {
        memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

        RxToTxOptions(rxOpt, &pTxOptionsEx);

        numberOfBytesWithKeyAttributes = getAppCentralSceneReportData(
            &(pTxBuf->ZW_CentralSceneSupportedReport1byteV3Frame));

        pTxBuf->ZW_CentralSceneSupportedReport1byteV3Frame.cmdClass = COMMAND_CLASS_CENTRAL_SCENE_V3;
        pTxBuf->ZW_CentralSceneSupportedReport1byteV3Frame.cmd = CENTRAL_SCENE_SUPPORTED_REPORT_V3;
        pTxBuf->ZW_CentralSceneSupportedReport1byteV3Frame.properties1 &= (uint8_t)~0xF8;

        // Forced to always support slow refresh.
        pTxBuf->ZW_CentralSceneSupportedReport1byteV3Frame.properties1 |= 0x80;

        if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
          (uint8_t *)pTxBuf,
          sizeof(ZW_CENTRAL_SCENE_SUPPORTED_REPORT_1BYTE_V3_FRAME) - 4 + numberOfBytesWithKeyAttributes,
          pTxOptionsEx,
          NULL))
        {
          /*Job failed */
          ;
        }
        return RECEIVED_FRAME_STATUS_SUCCESS;
      }
      return RECEIVED_FRAME_STATUS_FAIL;
      break;

    case CENTRAL_SCENE_CONFIGURATION_GET_V3:
      if (false == Check_not_legal_response_job(rxOpt))
      {
        RxToTxOptions(rxOpt, &pTxOptionsEx);
        prepare_configuration_report(pTxBuf);

        if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
            (uint8_t *)pTxBuf,
            sizeof(ZW_CENTRAL_SCENE_CONFIGURATION_REPORT_V3_FRAME),
            pTxOptionsEx,
            NULL))
        {
          /*Job failed */
          ;
        }
        return RECEIVED_FRAME_STATUS_SUCCESS;
      }
      return RECEIVED_FRAME_STATUS_FAIL;
      break;

    case CENTRAL_SCENE_CONFIGURATION_SET_V3:
    {
      central_scene_configuration_t configuration;
      configuration.slowRefresh = (pCmd->ZW_CentralSceneConfigurationSetV3Frame.properties1 >> 7);

      e_cmd_handler_return_code_t return_code = setAppCentralSceneConfiguration(&configuration);
      /* If handler has finished, call TSE */
      if (E_CMD_HANDLER_RETURN_CODE_FAIL == return_code || E_CMD_HANDLER_RETURN_CODE_HANDLED == return_code)
      {
        // Build up new CC data structure
        memset(&centralSceneData, 0, sizeof(s_CC_centralScene_data_t));
        centralSceneData.rxOptions = *rxOpt;

        if (false == ZAF_TSE_Trigger(CC_CentralScene_configuration_report_stx,
                                     (void *)&centralSceneData,
                                     true))
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
    break;
  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}

JOB_STATUS
CommandClassCentralSceneNotificationTransmit(
                                             AGI_PROFILE* pProfile,
                                             uint8_t sourceEndpoint,
                                             uint8_t keyAttribute,
                                             uint8_t sceneNumber,
                                             VOID_CALLBACKFUNC(pCbFunc) (TRANSMISSION_RESULT * pTransmissionResult))
{
  static uint8_t sequenceNumber = 0;
  TRANSMIT_OPTIONS_TYPE_EX* pTxOptionsEx = NULL;
  central_scene_configuration_t configuration;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = GetRequestBuffer(pCbFunc);

  if( IS_NULL( pTxBuf ) )
  {
    /*Ongoing job is active.. just stop current job*/
    return JOB_STATUS_BUSY;
  }

  pTxBuf->ZW_CentralSceneNotificationV3Frame.cmdClass = COMMAND_CLASS_CENTRAL_SCENE_V3;
  pTxBuf->ZW_CentralSceneNotificationV3Frame.cmd = CENTRAL_SCENE_NOTIFICATION_V3;
  pTxBuf->ZW_CentralSceneNotificationV3Frame.sequenceNumber = sequenceNumber++;

  getAppCentralSceneConfiguration(&configuration);
  pTxBuf->ZW_CentralSceneNotificationV3Frame.properties1 = (uint8_t)((configuration.slowRefresh << 7)
                                                                   | (keyAttribute & 0x87));
  pTxBuf->ZW_CentralSceneNotificationV3Frame.sceneNumber = sceneNumber;

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

  if(ETRANSPORTMULTICAST_ADDED_TO_QUEUE != ZW_TransportMulticast_SendRequest(
      (uint8_t *)pTxBuf,
      sizeof(ZW_CENTRAL_SCENE_NOTIFICATION_V2_FRAME),
      true, // Enable Supervision
      pTxOptionsEx,
      ZCB_RequestJobStatus))
  {
    /*Job failed, free transmit-buffer pTxBuf by cleaning mutex */
    FreeRequestBuffer();
    return JOB_STATUS_BUSY;
  }

  return JOB_STATUS_SUCCESS;
}

void CC_CentralScene_configuration_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions,
                                              void *pData)
{
  UNUSED(pData);
  DPRINTF("* %s() *\n"
      "\ttxOpt.src = %d\n"
      "\ttxOpt.options %#02x\n"
      "\ttxOpt.secOptions %#02x\n",
      __func__, txOptions.sourceEndpoint, txOptions.txOptions, txOptions.txSecOptions);

  /* Prepare payload for report */
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);

  prepare_configuration_report(pTxBuf);

  if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendRequestEP((uint8_t *)pTxBuf,
                                                                 sizeof(ZW_CENTRAL_SCENE_CONFIGURATION_REPORT_V3_FRAME),
                                                                 &txOptions,
                                                                 ZAF_TSE_TXCallback))
  {
    //sending request failed
    DPRINTF("%s(): Transport_SendRequestEP() failed. \n", __func__);
  }

}

static void prepare_configuration_report(ZW_APPLICATION_TX_BUFFER *pTxBuffer)
{
  memset((uint8_t*)pTxBuffer, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

  pTxBuffer->ZW_CentralSceneConfigurationReportV3Frame.cmdClass = COMMAND_CLASS_CENTRAL_SCENE_V3;
  pTxBuffer->ZW_CentralSceneConfigurationReportV3Frame.cmd = CENTRAL_SCENE_CONFIGURATION_REPORT_V3;

  central_scene_configuration_t configuration;
  getAppCentralSceneConfiguration(&configuration);
  pTxBuffer->ZW_CentralSceneConfigurationReportV3Frame.properties1 = (configuration.slowRefresh << 7);

}

static uint8_t lifeline_reporting(ccc_pair_t * p_ccc_pair)
{
  p_ccc_pair->cmdClass = COMMAND_CLASS_CENTRAL_SCENE;
  p_ccc_pair->cmd      = CENTRAL_SCENE_NOTIFICATION;
  p_ccc_pair++;
  p_ccc_pair->cmdClass = COMMAND_CLASS_CENTRAL_SCENE;
  p_ccc_pair->cmd      = CENTRAL_SCENE_CONFIGURATION_REPORT_V3;
  return 2;
}

/*
 * This will map this CC to Basic CC, so set commands by Basic CC can be routed to this Command Class for execution.
 */
REGISTER_CC_V3(COMMAND_CLASS_CENTRAL_SCENE, CENTRAL_SCENE_VERSION_V3, CC_CentralScene_handler, NULL, NULL, lifeline_reporting, 0);
