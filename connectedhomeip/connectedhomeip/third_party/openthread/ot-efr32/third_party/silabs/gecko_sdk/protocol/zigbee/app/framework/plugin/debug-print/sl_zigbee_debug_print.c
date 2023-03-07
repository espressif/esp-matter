/***************************************************************************//**
 * @file
 * @brief ZigBee specific debug print API definitions.
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

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "sl_component_catalog.h"
#include "sl_zigbee_debug_print.h"
#include "sl_zigbee_debug_print_config.h"

//------------------------------------------------------------------------------
// Forward declarations

static sl_status_t check_group_type(uint32_t group_type);

//------------------------------------------------------------------------------
// Static variables

static uint32_t print_group_bitmask = (0
#if ((SL_ZIGBEE_DEBUG_STACK_GROUP_ENABLED == 1) && (SL_ZIGBEE_DEBUG_STACK_GROUP_RUNTIME_DEFAULT == 1))
                                       | SL_ZIGBEE_DEBUG_PRINT_TYPE_STACK
#endif
#if ((SL_ZIGBEE_DEBUG_CORE_GROUP_ENABLED == 1) && (SL_ZIGBEE_DEBUG_CORE_GROUP_RUNTIME_DEFAULT == 1))
                                       | SL_ZIGBEE_DEBUG_PRINT_TYPE_CORE
#endif
#if ((SL_ZIGBEE_DEBUG_APP_GROUP_ENABLED == 1) && (SL_ZIGBEE_DEBUG_APP_GROUP_RUNTIME_DEFAULT == 1))
                                       | SL_ZIGBEE_DEBUG_PRINT_TYPE_APP
#endif
#if ((SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED == 1) && (SL_ZIGBEE_DEBUG_ZCL_GROUP_RUNTIME_DEFAULT == 1))
                                       | SL_ZIGBEE_DEBUG_PRINT_TYPE_ZCL
#endif
#if ((SL_ZIGBEE_DEBUG_PRINTS_ZCL_LEGACY_AF_DEBUG_ENABLED == 1) && (SL_ZIGBEE_DEBUG_PRINTS_ZCL_LEGACY_AF_DEBUG_RUNTIME_DEFAULT == 1))
                                       | SL_ZIGBEE_DEBUG_PRINT_TYPE_LEGACY_AF_DEBUG
#endif
                                       );

//------------------------------------------------------------------------------
// Public APIs

sl_status_t sl_zigbee_debug_print_enable_group(uint32_t group_type, bool enable)
{
  sl_status_t status = check_group_type(group_type);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (enable) {
    print_group_bitmask |= group_type;
  } else {
    print_group_bitmask &= ~group_type;
  }

  return SL_STATUS_OK;
}

//------------------------------------------------------------------------------
// Internal APIs

void sli_zigbee_debug_print(uint32_t group_type, const char* format, ...)
{
  sl_status_t status = check_group_type(group_type);
  va_list args;

  if (status != SL_STATUS_OK
      || (print_group_bitmask & group_type) == 0) {
    return;
  }

  va_start(args, format);
  local_vprintf(format, args);
  va_end(args);
}

void sli_zigbee_debug_print_buffer(uint32_t group_type,
                                   const uint8_t *buffer,
                                   uint16_t buffer_length,
                                   bool with_space)
{
  uint16_t i;

  for (i = 0; i < buffer_length; i++) {
    sli_zigbee_debug_print(group_type, "%02X", buffer[i]);
    if (with_space) {
      sli_zigbee_debug_print(group_type, " ");
    }
  }
}

//------------------------------------------------------------------------------
// Static functions

static sl_status_t check_group_type(uint32_t group_type)
{
  switch (group_type) {
    case SL_ZIGBEE_DEBUG_PRINT_TYPE_STACK:
#if (SL_ZIGBEE_DEBUG_STACK_GROUP_ENABLED == 0)
      return SL_STATUS_INVALID_PARAMETER;
#endif // SL_ZIGBEE_DEBUG_STACK_GROUP_ENABLED==0
      break;
    case SL_ZIGBEE_DEBUG_PRINT_TYPE_CORE:
#if (SL_ZIGBEE_DEBUG_CORE_GROUP_ENABLED == 0)
      return SL_STATUS_INVALID_PARAMETER;
#endif // SL_ZIGBEE_DEBUG_CORE_GROUP_ENABLED==0
      break;
    case SL_ZIGBEE_DEBUG_PRINT_TYPE_APP:
#if (SL_ZIGBEE_DEBUG_APP_GROUP_ENABLED == 0)
      return SL_STATUS_INVALID_PARAMETER;
#endif // SL_ZIGBEE_DEBUG_APP_GROUP_ENABLED==0
      break;
    case SL_ZIGBEE_DEBUG_PRINT_TYPE_ZCL:
#if (SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED == 0)
      return SL_STATUS_INVALID_PARAMETER;
#endif // SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED==0
      break;
    case SL_ZIGBEE_DEBUG_PRINT_TYPE_LEGACY_AF_DEBUG:
#if (SL_ZIGBEE_DEBUG_PRINTS_ZCL_LEGACY_AF_DEBUG_ENABLED == 0)
      return SL_STATUS_INVALID_PARAMETER;
#endif // SL_ZIGBEE_DEBUG_PRINTS_ZCL_LEGACY_AF_DEBUG_ENABLED==0
      break;
    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

//------------------------------------------------------------------------------
// CLI commands

#ifdef SL_CATALOG_CLI_PRESENT

#include "sl_cli.h"

static void enable_group_command_common(sl_cli_command_arg_t *arguments, uint8_t group_type)
{
  bool enable = (sl_cli_get_argument_uint8(arguments, 0) > 0);

  if (sl_zigbee_debug_print_enable_group(group_type, enable) == SL_STATUS_OK) {
    // Notice, this will produce output only if the core group is still enabled.
    sl_zigbee_core_debug_print("Debug print group %s\n", (enable ? "enabled" : "disabled"));
  } else {
    sl_zigbee_core_debug_print("Wrong parameter\n");
  }
}

void sli_zigbee_debug_print_enable_stack_type_command(sl_cli_command_arg_t *arguments)
{
  enable_group_command_common(arguments, SL_ZIGBEE_DEBUG_PRINT_TYPE_STACK);
}

void sli_zigbee_debug_print_enable_core_type_command(sl_cli_command_arg_t *arguments)
{
  enable_group_command_common(arguments, SL_ZIGBEE_DEBUG_PRINT_TYPE_CORE);
}

void sli_zigbee_debug_print_enable_app_type_command(sl_cli_command_arg_t *arguments)
{
  enable_group_command_common(arguments, SL_ZIGBEE_DEBUG_PRINT_TYPE_APP);
}

void sli_zigbee_debug_print_enable_zcl_type_command(sl_cli_command_arg_t *arguments)
{
  enable_group_command_common(arguments, SL_ZIGBEE_DEBUG_PRINT_TYPE_ZCL);
}

void sli_zigbee_debug_print_enable_legacy_af_debug_type_command(sl_cli_command_arg_t *arguments)
{
  enable_group_command_common(arguments, SL_ZIGBEE_DEBUG_PRINT_TYPE_LEGACY_AF_DEBUG);
}

#endif // SL_CATALOG_CLI_PRESENT
