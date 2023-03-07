/**
 * Indicator command class weak functions implementation
 *
 * Allows apps to be built without cc_indicator component
 *
 * @copyright 2022 Silicon Laboratories Inc.
 */

#include "CC_Indicator.h"


ZW_WEAK void CC_Indicator_Init(cc_indicator_callback_t callback)
{
  UNUSED(callback);
}

ZW_WEAK void CC_Indicator_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions, void* pData)
{
  UNUSED(txOptions);
  UNUSED(pData);
}

ZW_WEAK void CC_Indicator_RefreshIndicatorProperties(void)
{

}
