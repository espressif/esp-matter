/**
 * @file
 * Handles SECURITY_COMMANDS_SUPPORTED_GET and SECURITY_2_COMMANDS_SUPPORTED_GET for endpoints.
 *
 * The Z-Wave stack will handle the above commands for the root device.
 * @copyright 2022 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_basis_api.h>
#include <ZW_TransportLayer.h>
#include <ZW_TransportMulticast.h>
#include "config_app.h"
#include <string.h>
#include <ZAF_Common_interface.h>
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



/*==============================   handleCommandClassBinarySwitch  ============
**
**  Function:  handler for Binary Switch Info CC
**
**  Side effects: None
**
**--------------------------------------------------------------------------*/
static received_frame_status_t
CC_Security_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt, /* IN receive options of type RECEIVE_OPTIONS_TYPE_EX  */
  ZW_APPLICATION_TX_BUFFER *pCmd, /* IN  Payload from the received frame */
  uint8_t cmdLength)               /* IN Number of command bytes including the command */
{
  size_t length;
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *txOptionsEx = NULL;
  received_frame_status_t frame_status = RECEIVED_FRAME_STATUS_FAIL;
  UNUSED(cmdLength);

  if(true == Check_not_legal_response_job(rxOpt))
  {
    /*Get/Report do not support endpoint bit-addressing */
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  if ((SECURITY_COMMANDS_SUPPORTED_GET != pCmd->ZW_Common.cmd) &&
      (SECURITY_2_COMMANDS_SUPPORTED_GET != pCmd->ZW_Common.cmd))
  {
    return RECEIVED_FRAME_STATUS_NO_SUPPORT;
  }

  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

  uint8_t* pPayload = NULL;
  security_key_t secureLevel;
  CMD_CLASS_LIST *pCmdClassList  = NULL;
  RxToTxOptions(rxOpt, &txOptionsEx);

  if ((SECURITY_KEY_S2_UNAUTHENTICATED <= rxOpt->securityKey) &&  // We must respond to "S0 SUPPORTED GET" also when included at higher S2 level
      (SECURITY_KEY_S0 >= rxOpt->securityKey) &&
	  (SECURITY_COMMANDS_SUPPORTED_GET == pCmd->ZW_Common.cmd) && (COMMAND_CLASS_SECURITY == pCmd->ZW_Common.cmdClass))
  {
    frame_status = RECEIVED_FRAME_STATUS_SUCCESS;
    pTxBuf->ZW_SecurityCommandsSupportedReport1byteFrame.cmdClass = COMMAND_CLASS_SECURITY;
    pTxBuf->ZW_SecurityCommandsSupportedReport1byteFrame.cmd = SECURITY_COMMANDS_SUPPORTED_REPORT;
    pTxBuf->ZW_SecurityCommandsSupportedReport1byteFrame.reportsToFollow = 0;
    length = sizeof(pTxBuf->ZW_SecurityCommandsSupportedReport1byteFrame) - 3;
    secureLevel = SECURITY_KEY_S0;
    pPayload = &pTxBuf->ZW_SecurityCommandsSupportedReport1byteFrame.commandClassSupport1;
  }
  else if ((SECURITY_KEY_S2_UNAUTHENTICATED <= rxOpt->securityKey) && (SECURITY_KEY_S2_ACCESS >= rxOpt->securityKey) &&
		   (SECURITY_2_COMMANDS_SUPPORTED_GET == pCmd->ZW_Common.cmd) && (COMMAND_CLASS_SECURITY_2 == pCmd->ZW_Common.cmdClass))
  {
    frame_status = RECEIVED_FRAME_STATUS_SUCCESS;
    pTxBuf->ZW_SecurityCommandsSupportedReport1byteFrame.cmdClass = COMMAND_CLASS_SECURITY_2;
    pTxBuf->ZW_SecurityCommandsSupportedReport1byteFrame.cmd = SECURITY_2_COMMANDS_SUPPORTED_REPORT;
    length = 2; /*SECURITY_2_COMMANDS_SUPPORTED_REPORT = 2*/
    secureLevel = rxOpt->securityKey;//SECURITY_KEY_S2_UNAUTHENTICATED;
    pPayload = &pTxBuf->ZW_SecurityCommandsSupportedReport1byteFrame.reportsToFollow;
  }
  else
  {
    /*Job failed */
    return RECEIVED_FRAME_STATUS_NO_SUPPORT;
  }

  pCmdClassList = GetCommandClassList((0 != ZAF_GetNodeID()), secureLevel, rxOpt->destNode.endpoint);
  if(NULL != pCmdClassList)
  {
    length += pCmdClassList->size;
    memcpy(pPayload, pCmdClassList->pList, pCmdClassList->size);
  }

  if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
      (uint8_t *)pTxBuf,
      length,
      txOptionsEx,
      NULL))
  {
    /*Job failed, free transmit-buffer pTxBuf by cleaning mutex */
    ;
  }
  return frame_status;
}

/*
 * Register for both S0 and S2. Version is set to zero because the version is set by and fetched
 * from the Z-Wave protocol.
 */
REGISTER_CC(COMMAND_CLASS_SECURITY,   0x00, CC_Security_handler);
REGISTER_CC(COMMAND_CLASS_SECURITY_2, 0x00, CC_Security_handler);
