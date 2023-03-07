/***************************************************************************//**
 * @file
 * @brief CLI implementations for the scripting features of the multiprotocol
 *   app. These features allow a user to upload a series of commands to the
 *   chip to be executed consecutively without waiting for further CLI input
 *   until the uploaded script is completed. This is useful in testing
 *   scenarios where they delay related to entering commands one-by-one
 *   is too great.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rail.h"
#include "app_common.h"
#include "response_print.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
  #include "sl_cli_storage_ram_instances.h"
#endif
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
  #include "sl_cli_storage_nvm3_instances.h"
#endif
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
  #include "sl_cli_delay.h"
#endif

static bool flashCommandScriptExists;
static bool ramCommandScriptExists;

// non-blocking wait
void wait(sl_cli_command_arg_t *args)
{
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
  // Relative time by default
  RAIL_TimeMode_t timeMode = RAIL_TIME_DELAY;

  if (sl_cli_get_argument_count(args) >= 2 && !parseTimeModeFromString(sl_cli_get_argument_string(args, 1), &timeMode)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x3, "Invalid time mode");
    return;
  }

  RAIL_Time_t timeStart = RAIL_GetTime();
  RAIL_Time_t timeDurationUs = sl_cli_get_argument_uint32(args, 0);

  // In the code, everything is handled as relative time, so convert this
  // to a relative value
  if (timeMode == RAIL_TIME_ABSOLUTE) {
    timeDurationUs -= timeStart;
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "currentTime:%u,delay:%u,resumeTime:%u",
                timeStart,
                timeDurationUs,
                timeDurationUs + timeStart);

  // Take into account the time it took to printf above.
  uint32_t extraOffset = RAIL_GetTime() - timeStart;
  if (extraOffset < timeDurationUs) {
    timeDurationUs -= extraOffset;
  } else {
    timeDurationUs = 0;
  }

  // Remove the optional 2nd CLI argument so sl_cli_delay_command works.
  // Also, convert microsecond time delay into milliseconds.
  uint32_t timeDurationMs = timeDurationUs / 1000; // convert us to ms
  args->argc = 2; // pass only cmd and delay (and not optional argument)
  args->argv[1] = (void *)&timeDurationMs;

  // Call the non-blocking wait function (with a relative delay in ms).
  sl_cli_delay_command(args);
#else
  (void)args;
  responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                     "CLI Delay support not enabled");
#endif //SL_CATALOG_CLI_DELAY_PRESENT
}

void enterScript(sl_cli_command_arg_t *args)
{
  bool useFlash = (sl_cli_get_argument_count(args) >= 1)
                  && !!sl_cli_get_argument_uint8(args, 0);
  bool success = true;

  // Don't pass along the optional input argument.
  args->argc = 1; // pass only cmd (and not optional argument)

  if (useFlash) {
    // Use flash.
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
    if (flashCommandScriptExists) {
      sl_cli_storage_nvm3_clear(args);
    }
    flashCommandScriptExists = true;
    sl_cli_storage_nvm3_define(args);
#else
    (void)flashCommandScriptExists;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "Flash support not enabled");
    return;
#endif //SL_CATALOG_CLI_STORAGE_NVM3_PRESENT
  } else {
    // Use RAM.
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
    if (ramCommandScriptExists) {
      sl_cli_storage_ram_clear(args);
    }
    ramCommandScriptExists = true;
    sl_cli_storage_ram_define(args);
#else
    (void)ramCommandScriptExists;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "RAM support not enabled");
    return;
#endif //SL_CATALOG_CLI_STORAGE_RAM_PRESENT
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "location:%s,status:%s",
                useFlash ? "flash" : "RAM",
                success ? "Success" : "Failure");
}

void clearScript(sl_cli_command_arg_t *args)
{
  bool useFlash = (sl_cli_get_argument_count(args) >= 1)
                  && !!sl_cli_get_argument_uint8(args, 0);
  bool success = true;

  // Don't pass along the optional input argument.
  args->argc = 1; // pass only cmd (and not optional argument)

  if (useFlash) {
    // Use flash.
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
    flashCommandScriptExists = false;
    sl_cli_storage_nvm3_clear(args);
#else
    (void)flashCommandScriptExists;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "Flash support not enabled");
    return;
#endif //SL_CATALOG_CLI_STORAGE_NVM3_PRESENT
  } else {
    // Use RAM.
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
    ramCommandScriptExists = false;
    sl_cli_storage_ram_clear(args);
#else
    (void)ramCommandScriptExists;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "RAM support not enabled");
    return;
#endif //SL_CATALOG_CLI_STORAGE_RAM_PRESENT
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "location:%s,status:%s",
                useFlash ? "flash" : "RAM",
                success ? "Success" : "Failure");
}

void printScript(sl_cli_command_arg_t *args)
{
  bool useFlash = (sl_cli_get_argument_count(args) >= 1)
                  && !!sl_cli_get_argument_uint8(args, 0);
  bool success = false;
  uint32_t scriptCount = 0U;

  // Don't pass along the optional input argument.
  args->argc = 1; // pass only cmd (and not optional argument)

  if (useFlash) {
    // Use flash.
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
    scriptCount = sl_cli_storage_nvm3_count(args->handle);
    if (scriptCount > 0U) {
      sl_cli_storage_nvm3_list(args);
      success = true;
    }
#else
    (void)scriptCount;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "Flash support not enabled");
    return;
#endif //SL_CATALOG_CLI_STORAGE_NVM3_PRESENT
  } else {
    // Use RAM.
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
    scriptCount = sl_cli_storage_ram_count(args->handle);
    if (scriptCount > 0U) {
      sl_cli_storage_ram_list(args);
      success = true;
    }
#else
    (void)scriptCount;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "RAM support not enabled");
    return;
#endif //SL_CATALOG_CLI_STORAGE_RAM_PRESENT
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "location:%s,status:%s,scriptCount:%u",
                useFlash ? "flash" : "RAM",
                success ? "Success" : "Failure",
                scriptCount);
}

void runScript(sl_cli_command_arg_t *args)
{
  bool useFlash = (sl_cli_get_argument_count(args) >= 1)
                  && !!sl_cli_get_argument_uint8(args, 0);
  bool success = false;
  uint32_t scriptCount = 0U;

  // Don't pass along the optional input argument.
  args->argc = 1; // pass only cmd (and not optional argument)

  if (useFlash) {
    // Use flash.
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
    scriptCount = sl_cli_storage_nvm3_count(args->handle);
    if (scriptCount > 0U) {
      sl_cli_storage_nvm3_execute(args);
      success = true;
    }
#else
    (void)scriptCount;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "Flash support not enabled");
    return;
#endif //SL_CATALOG_CLI_STORAGE_NVM3_PRESENT
  } else {
    // Use RAM.
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
    scriptCount = sl_cli_storage_ram_count(args->handle);
    if (scriptCount > 0U) {
      sl_cli_storage_ram_execute(args);
      success = true;
    }
#else
    (void)scriptCount;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "RAM support not enabled");
    return;
#endif //SL_CATALOG_CLI_STORAGE_RAM_PRESENT
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "location:%s,status:%s",
                useFlash ? "flash" : "RAM",
                success ? "Success" : "Failure");
}
