/**
 * @file
 * Source file of helper module for CC Multi Channel.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#include "multichannel.h"
#include <ZW_TransportSecProtocol.h>
#include <ZAF_command_class_utils.h>

bool ZAF_CC_MultiChannel_IsCCSupported(
  RECEIVE_OPTIONS_TYPE_EX * pRxOpt,
  ZW_APPLICATION_TX_BUFFER * pCmd)
{
  if (0 == pRxOpt->destNode.endpoint)
  {
    return (TransportCmdClassSupported(pCmd->ZW_Common.cmdClass, pCmd->ZW_Common.cmd, pRxOpt->securityKey));
  }
  else
  {
    CMD_CLASS_LIST* pNonSec = GetEndpointcmdClassList(false, pRxOpt->destNode.endpoint);
    CMD_CLASS_LIST* pSec    = GetEndpointcmdClassList(true, pRxOpt->destNode.endpoint);
    if ((NULL != pNonSec) && (NULL != pSec))
    {
      return (CmdClassSupported(pRxOpt->securityKey,
                                pCmd->ZW_Common.cmdClass,
                                pCmd->ZW_Common.cmd,
                                pSec->pList, pSec->size,
                                pNonSec->pList, pNonSec->size));
    }
  }
  return false;
}

void CmdClassMultiChannelEncapsulate(
  uint8_t **ppData,
  size_t  *dataLength,
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx)
{
  ZW_APPLICATION_TX_BUFFER *pTxBuf;
  size_t sizeCmdFrameHeader;

  if (IS_NULL(*ppData)
   || ((0 == pTxOptionsEx->sourceEndpoint)
   && (0 == pTxOptionsEx->pDestNode->node.endpoint)))
  {
    return;
  }

  sizeCmdFrameHeader = sizeof(ZW_MULTI_CHANNEL_CMD_ENCAP_V2_FRAME) - sizeof(ALL_EXCEPT_ENCAP);
  *ppData -= sizeCmdFrameHeader;
  pTxBuf = (ZW_APPLICATION_TX_BUFFER *)*ppData;


  *dataLength += sizeCmdFrameHeader;

  pTxBuf->ZW_MultiChannelCmdEncapV2Frame.cmdClass = COMMAND_CLASS_MULTI_CHANNEL_V4;
  pTxBuf->ZW_MultiChannelCmdEncapV2Frame.cmd = MULTI_CHANNEL_CMD_ENCAP_V4;
  pTxBuf->ZW_MultiChannelCmdEncapV2Frame.properties1 = pTxOptionsEx->sourceEndpoint;
  pTxBuf->ZW_MultiChannelCmdEncapV2Frame.properties2 = (uint8_t)(pTxOptionsEx->pDestNode->node.endpoint | (pTxOptionsEx->pDestNode->node.BitAddress << 0x07));
}

