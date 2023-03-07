/***************************************************************************//**
 * @file
 * @brief Implements secure EZSP stubs.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "app/util/ezsp/ezsp-protocol.h"

EzspStatus emSecureEzspInit(void)
{
  return EZSP_ERROR_INVALID_CALL;
}
EzspStatus emSecureEzspDeInit(void)
{
  return EZSP_ERROR_INVALID_CALL;
}
bool emSecureEzspIsOn(void)
{
  return false;
}
EzspStatus emSecureEzspEncode(void)
{
  return EZSP_ERROR_INVALID_CALL;
}
EzspStatus emSecureEzspDecode(void)
{
  return EZSP_ERROR_INVALID_CALL;
}
void emSecureEzspSetState(uint8_t state)
{
  (void)state;
}
bool emSecureEzspParametersArePending(void)
{
  return false;
}
