/***************************************************************************//**
 * @file
 * @brief CLI for the Update TC Link Key plugin.
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

#include "app/framework/include/af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "update-tc-link-key.h"
#include "app/framework/plugin/network-steering/network-steering.h"
#include "app/framework/plugin/network-steering/network-steering-internal.h"

#ifdef UC_BUILD
#include "app/util/serial/sl_zigbee_command_interpreter.h"
void emberAfPluginSetTCLinkKeyUpdateTimerCommand(sl_cli_command_arg_t *arguments)
{
  uint32_t timeInMilliseconds = sl_cli_get_argument_uint32(arguments, 0);
  emberAfPluginSetTCLinkKeyUpdateTimerMilliSeconds(timeInMilliseconds);
}
#else //UC_BUILD
// -----------------------------------------------------------------------------
// Helper macros and functions

void emberAfPluginSetTCLinkKeyUpdateTimerCommand(void)
{
  uint32_t timeInMilliseconds = (uint32_t)emberUnsignedCommandArgument(0);
  emberAfPluginSetTCLinkKeyUpdateTimerMilliSeconds(timeInMilliseconds);
}

#endif // UC_BUILD
