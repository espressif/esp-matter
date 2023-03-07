/**
 * @file
 * @brief Network learn source file
 * @copyright 2019 Silicon Laboratories Inc.
 */


#include "ZAF_network_learn.h"
#include "ZW_application_transport_interface.h"
#include "ZAF_Common_interface.h"

void ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_ACTION bMode)
{
  SApplicationHandles* pAppHandle;

  pAppHandle = ZAF_getAppHandle();

  SZwaveCommandPackage CommandPackage;
  CommandPackage.eCommandType = EZWAVECOMMANDTYPE_NETWORK_LEARN_MODE_START;
  CommandPackage.uCommandParams.SetSmartStartLearnMode.eLearnMode = bMode;
  QueueNotifyingSendToBack(pAppHandle->pZwCommandQueue, (uint8_t*)&CommandPackage, 0);
}
