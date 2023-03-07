/**
 * TSE : Allows building applications without TSE module being included.
 * @copyright 2022 Silicon Laboratories Inc.
 */

#include "ZAF_TSE.h"


ZW_WEAK bool ZAF_TSE_Trigger(zaf_tse_callback_t pCallback,
                     void* pData,
                     bool overwrite_previous_trigger)
{
  UNUSED(pCallback);
  UNUSED(pData);
  UNUSED(overwrite_previous_trigger);
  return true;
}


ZW_WEAK bool ZAF_TSE_Init()
{
  return true;
}


ZW_WEAK void ZAF_TSE_TXCallback(transmission_result_t * pTransmissionResult)
{
  UNUSED(pTransmissionResult);
}
