/**
*
* @file
* Network management utility functions
*
* @copyright 2019 Silicon Laboratories Inc.
*
*/

#include <ZAF_network_management.h>
#include <ZAF_Common_interface.h>

void ZAF_SetMaxInclusionRequestIntervals(uint32_t intervals)
{
  SZwaveCommandPackage setMaxInclusionRequestIntervals = {
    .eCommandType = EZWAVECOMMANDTYPE_ZW_SET_MAX_INCL_REQ_INTERVALS,
    .uCommandParams.SetMaxInclReqInterval.inclusionRequestInterval = intervals,
  };

  // Put the Command on queue
  SApplicationHandles * pAppHandle = ZAF_getAppHandle();
  if (EQUEUENOTIFYING_STATUS_SUCCESS != QueueNotifyingSendToBack(pAppHandle->pZwCommandQueue, (uint8_t *)&setMaxInclusionRequestIntervals, 0))
  {
    ASSERT(false);
  }
}

void ZAF_SendINIF(void (*pCallback)(uint8_t txStatus, TX_STATUS_TYPE* extendedTxStatus))
{
  SApplicationHandles* pAppHandle;

  pAppHandle = ZAF_getAppHandle();

  SZwaveTransmitPackage FramePackage;
  FramePackage.uTransmitParams.IncludedNodeInfo.Handle = pCallback;
  FramePackage.eTransmitType = EZWAVETRANSMITTYPE_INCLUDEDNODEINFORMATION;

  // Put the package on queue (and dont wait for it)
  if (EQUEUENOTIFYING_STATUS_SUCCESS != QueueNotifyingSendToBack(pAppHandle->pZwTxQueue, (uint8_t*)&FramePackage, 0))
  {
    if (NULL != pCallback)
    {
      pCallback(TRANSMIT_COMPLETE_FAIL, NULL);
    }
  }
}
