/**
*
* @brief Battery Command Class source file
* @copyright 2019 Silicon Laboratories Inc.
*
*/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_TransportLayer.h>

#include <CC_Battery.h>
#include <ZW_TransportMulticast.h>
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

/****************************************************************************/
/*                            PUBLIC FUNCTIONS                             */
/****************************************************************************/

static received_frame_status_t
CC_Battery_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t cmdLength)
{
  UNUSED(cmdLength);
  if (pCmd->ZW_Common.cmd == BATTERY_GET)
  {
    ZAF_TRANSPORT_TX_BUFFER  TxBuf;
    ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);

    if (true == Check_not_legal_response_job(rxOpt))
    {
      // None of the following commands support endpoint bit addressing.
      return RECEIVED_FRAME_STATUS_FAIL;
    }
    memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

    TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
    RxToTxOptions(rxOpt, &pTxOptionsEx);
    pTxBuf->ZW_BatteryReportFrame.cmdClass = COMMAND_CLASS_BATTERY;
    pTxBuf->ZW_BatteryReportFrame.cmd = BATTERY_REPORT;
    pTxBuf->ZW_BatteryReportFrame.batteryLevel = CC_Battery_BatteryGet_handler(rxOpt->destNode.endpoint);

    if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
        (uint8_t *)pTxBuf,
        sizeof(ZW_BATTERY_REPORT_FRAME),
        pTxOptionsEx,
        NULL))
    {
      /*Job failed */
      ;
    }
    return RECEIVED_FRAME_STATUS_SUCCESS;
  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}

JOB_STATUS
CC_Battery_LevelReport_tx(
  const AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  uint8_t bBattLevel,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult))
{
  CMD_CLASS_GRP cmdGrp = {COMMAND_CLASS_BATTERY, BATTERY_REPORT};

  return cc_engine_multicast_request(
      pProfile,
      sourceEndpoint,
      &cmdGrp,
      &bBattLevel,
      1,
      false,
      pCbFunc);
}

static uint8_t lifeline_reporting(ccc_pair_t * p_ccc_pair)
{
  p_ccc_pair->cmdClass = COMMAND_CLASS_BATTERY;
  p_ccc_pair->cmd      = BATTERY_REPORT;
  return 1;
}

REGISTER_CC_V3(COMMAND_CLASS_BATTERY, BATTERY_VERSION, CC_Battery_handler, NULL, NULL, lifeline_reporting, 0);
