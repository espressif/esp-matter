/***************************************************************************//**
 * @brief Zigbee Application Framework sleep code.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_component_catalog.h"
#include PLATFORM_HEADER

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_ZIGBEE_PRO_COMPLIANCE_PRESENT)

#include PLATFORM_HEADER
#include "sl_cli.h"
#include "zigbee_app_framework_common.h"
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#include "sl_zigbee_debug_print.h"
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

extern bool emAfStayAwakeWhenNotJoined;
extern bool emAfForceEndDeviceToStayAwake;
extern void emberAfForceEndDeviceToStayAwake(bool stayAwake);

void emberAfPluginIdleSleepStatusCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  sl_zigbee_core_debug_print("Stay awake when not joined: %s\n",
                             (emAfStayAwakeWhenNotJoined ? "yes" : "no"));
  sl_zigbee_core_debug_print("Forced stay awake:          %s\n",
                             (emAfForceEndDeviceToStayAwake ? "yes" : "no"));
}

void emberAfPluginIdleSleepStayAwakeCommand(sl_cli_command_arg_t *arguments)
{
  bool stayAwake = (bool)sl_cli_get_argument_uint8(arguments, 0);
  if (stayAwake) {
    sl_zigbee_core_debug_print("Forcing device to stay awake\n");
  } else {
    sl_zigbee_core_debug_print("Allowing device to go to sleep\n");
  }
  emberAfForceEndDeviceToStayAwake(stayAwake);
}

void emberAfPluginIdleSleepAwakeWhenNotJoinedCommand(sl_cli_command_arg_t *arguments)
{
  emAfStayAwakeWhenNotJoined = (bool)sl_cli_get_argument_uint8(arguments, 0);
}

#endif //#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_ZIGBEE_PRO_COMPLIANCE_PRESENT)
