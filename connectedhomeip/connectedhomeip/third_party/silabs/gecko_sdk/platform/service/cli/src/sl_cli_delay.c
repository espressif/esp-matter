/***************************************************************************//**
 * @file
 * @brief A non-blocking delay for the CLI.
 * @version x.y.z
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_status.h"
#include "sl_cli.h"
#include "sli_cli_arguments.h"
#include "sl_cli_delay.h"
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#else
#include "sl_sleeptimer.h"
#include <string.h>
#endif

//****************************************************************************

#if !defined(SL_CATALOG_KERNEL_PRESENT)
static bool is_busy(sl_cli_handle_t handle)
{
  sl_status_t status;
  bool busy;

  status = sl_sleeptimer_is_timer_running(&handle->cli_delay.sleeptimer, &busy);
  busy = (status != SL_STATUS_OK) ? false : busy;

  return busy;
}
#endif

//****************************************************************************

#if defined(SL_CATALOG_KERNEL_PRESENT)

void sl_cli_delay_command(sl_cli_command_arg_t *arguments)
{
  uint32_t delay_ms = 0;

  if (sl_cli_get_argument_count(arguments) > 0) {
    delay_ms = sl_cli_get_argument_uint32(arguments, 0);
  }
  osDelay(((uint64_t)osKernelGetTickFreq() * delay_ms) / 1000);
}

#else // defined(SL_CATALOG_KERNEL_PRESENT)

void sl_cli_delay_command(sl_cli_command_arg_t *arguments)
{
  uint32_t delay_ms = 0;
  sl_cli_handle_t handle = arguments->handle;

  if (sl_cli_get_argument_count(arguments) > 0) {
    delay_ms = sl_cli_get_argument_uint32(arguments, 0);
  }
  sl_sleeptimer_start_timer_ms(&handle->cli_delay.sleeptimer, delay_ms, NULL, NULL, 0, 0);
}

bool sl_cli_delay_tick(sl_cli_handle_t handle)
{
  return is_busy(handle);
}

#endif // defined(SL_CATALOG_KERNEL_PRESENT)
