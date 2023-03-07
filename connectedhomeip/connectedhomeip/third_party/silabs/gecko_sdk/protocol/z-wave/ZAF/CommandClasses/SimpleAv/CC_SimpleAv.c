/**
 * @file
 * @brief Simple AV Command Class source file
 * @copyright 2019 Silicon Laboratories Inc.
 */
#include "config_app.h"
#include <ZW_basis_api.h>
#include <ZW_TransportLayer.h>
#include <CC_SimpleAv.h>
#include <CC_Supervision.h>
#include <string.h>
#include "ZAF_tx_mutex.h"

//#define DEBUGPRINT
#include "DebugPrint.h"

static uint8_t seqNo;

static received_frame_status_t
CC_SimpleAv_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t cmdLength)
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
  UNUSED(cmdLength);

  if(true == Check_not_legal_response_job(rxOpt))
  {
    /*Get/Report do not support endpoint bit-addressing */
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  switch (pCmd->ZW_Common.cmd)
  {
      //Must be ignored to avoid unintentional operation. Cannot be mapped to another command class.
    case SIMPLE_AV_CONTROL_GET:
      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      /* Controller wants the sensor level */
      RxToTxOptions(rxOpt, &pTxOptionsEx);
      pTxBuf->ZW_SimpleAvControlReportFrame.cmdClass = COMMAND_CLASS_SIMPLE_AV_CONTROL;
      pTxBuf->ZW_SimpleAvControlReportFrame.cmd = SIMPLE_AV_CONTROL_REPORT;
      pTxBuf->ZW_SimpleAvControlReportFrame.numberOfReports =  getApplSimpleAvReports();
      if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
          (uint8_t *)pTxBuf,
          sizeof(ZW_SIMPLE_AV_CONTROL_GET_FRAME),
          pTxOptionsEx,
          NULL))
      {
        /*Job failed */
        ;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case SIMPLE_AV_CONTROL_SUPPORTED_GET:
      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      /* Controller wants the sensor level */
      size_t len;
      RxToTxOptions(rxOpt, &pTxOptionsEx);
      len = getApplSimpleAvSupported(pCmd->ZW_SimpleAvControlSupportedReport4byteFrame.reportNo,
                                      &pTxBuf->ZW_SimpleAvControlSupportedReport1byteFrame.bitMask1);
      pTxBuf->ZW_SimpleAvControlSupportedReport1byteFrame.cmdClass = COMMAND_CLASS_SIMPLE_AV_CONTROL;
      pTxBuf->ZW_SimpleAvControlSupportedReport1byteFrame.cmd = SIMPLE_AV_CONTROL_SUPPORTED_REPORT;
      pTxBuf->ZW_SimpleAvControlSupportedReport1byteFrame.reportNo = pCmd->ZW_SimpleAvControlSupportedReport4byteFrame.reportNo;
      if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
          (uint8_t *)pTxBuf,
          sizeof(ZW_SIMPLE_AV_CONTROL_SUPPORTED_REPORT_1BYTE_FRAME) - 1 + len,
          pTxOptionsEx,
          NULL))
      {
        /*Job failed */
        ;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    default:
      break;
  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}

JOB_STATUS
CmdClassSimpleAvSet(
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX* pTxOptionsEx,
  uint16_t bCommand,
  uint8_t bKeyAttrib,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult))
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );
  CommandClassSupervisionGetAdd(&(TxBuf.supervisionGet));

  pTxBuf->ZW_SimpleAvControlSet1byteFrame.cmdClass = COMMAND_CLASS_SIMPLE_AV_CONTROL;
  pTxBuf->ZW_SimpleAvControlSet1byteFrame.cmd = SIMPLE_AV_CONTROL_SET;
  pTxBuf->ZW_SimpleAvControlSet1byteFrame.sequenceNumber = seqNo++;
  pTxBuf->ZW_SimpleAvControlSet1byteFrame.properties1 = 0x07 & bKeyAttrib;
  pTxBuf->ZW_SimpleAvControlSet1byteFrame.reserved21 = 0;
  pTxBuf->ZW_SimpleAvControlSet1byteFrame.reserved22 = 0;
  pTxBuf->ZW_SimpleAvControlSet1byteFrame.variantgroup1.command1 = (uint8_t)((bCommand&0xff00)>>8);   //Command MSB
  pTxBuf->ZW_SimpleAvControlSet1byteFrame.variantgroup1.command2 = (uint8_t)(bCommand&0xff);        //Command LSB


  if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendRequestEP(
        (uint8_t*)pTxBuf,
        sizeof(ZW_SIMPLE_AV_CONTROL_SET_1BYTE_FRAME),
        pTxOptionsEx,
        pCbFunc))
  {
    //sending request failed
    DPRINTF("%s(): Transport_SendRequestEP() BUSY. \n", __func__);
    return JOB_STATUS_BUSY;
  }
  return JOB_STATUS_SUCCESS;
}

REGISTER_CC(COMMAND_CLASS_SIMPLE_AV_CONTROL, SIMPLE_AV_CONTROL_VERSION, CC_SimpleAv_handler);
