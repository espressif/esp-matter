/**
 * @file
 * @brief Power Management source file
 * @copyright 2019 Silicon Laboratories Inc.
 */
#include "ZAF_PM_Wrapper.h"
#include "ZAF_Common_interface.h"

void ZAF_PM_SetPowerDownCallback(void (*callback)(void))
{
  SZwaveCommandPackage FramePackage;
  EQueueNotifyingStatus status;
  FramePackage.eCommandType =  EZWAVECOMMANDTYPE_PM_SET_POWERDOWN_CALLBACK;
  FramePackage.uCommandParams.PMSetPowerDownCallback.callback = callback;
  status = QueueNotifyingSendToBack(ZAF_getAppHandle()->pZwCommandQueue,
                                    (uint8_t *)&FramePackage,
                                    0);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == status);
}
