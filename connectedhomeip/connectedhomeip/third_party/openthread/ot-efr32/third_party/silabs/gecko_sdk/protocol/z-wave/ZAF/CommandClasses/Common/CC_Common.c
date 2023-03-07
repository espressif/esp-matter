/**
 * @file CC_Common.c
 * @brief Common types and definitions for all command classes.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#include <ZW_TransportMulticast.h>
#include <CC_Common.h>
#include <agi.h>
#include <string.h>
#include "ZAF_tx_mutex.h"

#define MAX_PAYLOAD 150

typedef struct _ZW_ENGINE_FRAME_
{
  CMD_CLASS_GRP grp;
   uint8_t payload[MAX_PAYLOAD];
} ZW_ENGINE_FRAME;



JOB_STATUS cc_engine_multicast_request(
    const AGI_PROFILE* pProfile,
    uint8_t endpoint,
    CMD_CLASS_GRP *pcmdGrp,
    uint8_t* pPayload,
    uint8_t size,
    uint8_t fSupervisionEnable,
    VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult))
{
  uint8_t frame_len = sizeof(CMD_CLASS_GRP);
  ZW_ENGINE_FRAME * pTxBuf = (ZW_ENGINE_FRAME *)GetRequestBuffer(pCbFunc);

  if( IS_NULL( pTxBuf ) )
  {
    /*Ongoing job is active.. just stop current job*/
    return JOB_STATUS_BUSY;
  }
  else
  {
    TRANSMIT_OPTIONS_TYPE_EX* pTxOptionsEx = NULL;

    pTxBuf->grp.cmdClass = pcmdGrp->cmdClass;
    pTxBuf->grp.cmd = pcmdGrp->cmd;

    if( 0 != size )
    {
      memcpy(pTxBuf->payload, pPayload, size);
      frame_len += size;
    }

    /*Get transmit options (node list)*/
    pTxOptionsEx = ReqNodeList( pProfile,
                          &(pTxBuf->grp),
                          endpoint);

    if( IS_NULL( pTxOptionsEx ) )
    {
      /*Job failed, free transmit-buffer pTxBuf by cleaning mutex */
      FreeRequestBuffer();
      return JOB_STATUS_NO_DESTINATIONS;
    }

    if(ETRANSPORTMULTICAST_ADDED_TO_QUEUE !=  ZW_TransportMulticast_SendRequest(
        (uint8_t *)pTxBuf,
        frame_len,
        fSupervisionEnable,
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
