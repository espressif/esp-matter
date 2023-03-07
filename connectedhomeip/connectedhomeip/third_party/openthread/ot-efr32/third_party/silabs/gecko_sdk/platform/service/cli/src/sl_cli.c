/***************************************************************************//**
 * @file
 * @brief Backbone of CLI framework
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
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#else
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
#include "sl_cli_delay.h"
#endif
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
#include "sl_cli_storage_ram.h"
#endif
#endif
#include "sl_cli.h"
#include "sli_cli_io.h"
#include "sl_cli_input.h"
#include "sli_cli_input.h"
#include <string.h>

#if !defined(__linux__)
#include "em_device.h"
#endif

#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
#include "sl_cli_storage_nvm3.h"
#endif

#ifndef __WEAK
#define __WEAK          __attribute__((weak))
#endif

/*******************************************************************************
 ****************************   HOOK REFERENCES   ******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Notify activity on the session (data received).
 *
 * @details
 *   This function should be used to reset the session inactivity, if implemented.
 *
 * @param[in] handle
 *   A handle to a CLI instance; Use session_data pointer for accessing instance's
 *   session data.
 *
 * @return SL_STATUS_OK if user can access the CLI instance, an error code otherwise .
 ******************************************************************************/
__WEAK void sli_cli_session_activity_notification(sl_cli_handle_t handle)
{
  (void) handle;
}

/*******************************************************************************
 ****************************   STATIC VARIABLES   *****************************
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

sl_cli_handle_t sl_cli_default_handle;
sl_cli_command_group_t *sl_cli_default_command_group;

//****************************************************************************
// Local functions

/***************************************************************************//**
 * @brief
 *  Get a string that explain the status code in ascii text.
 *
 * @param[in] status
 *   A status code.
 *
 * @return
 *   A pointer to the string.
 ******************************************************************************/
static const char *status_to_string(sl_status_t status)
{
  const char *string;

  switch (status) {
    case SL_STATUS_OK:
      string = "";
      break;
    case SL_STATUS_INVALID_PARAMETER:
      string = "The argument is not formatted correctly";
      break;
    case SL_STATUS_NOT_FOUND:
      string = "No command found";
      break;
    case SL_STATUS_HAS_OVERFLOWED:
      string = "Maximum number of arguments exceeded";
      break;
    case SL_STATUS_INVALID_TYPE:
      string = "Incorrect argument type";
      break;
    case SL_STATUS_INVALID_COUNT:
      string = "Incorrect number of arguments";
      break;
    default:
      string = "Unknown error";
      break;
  }

  return string;
}

/***************************************************************************//**
 * @brief
 *   Handle sli_cli_handle_input_and_history.
 *   Updates the history buffer, executes the function and clears the input
 *   string.
 ******************************************************************************/
void sli_cli_handle_input_and_history(sl_cli_handle_t handle)
{
  handle->req_prompt = true;
  if (strlen(handle->input_buffer) > 0) {
    sli_cli_input_update_history(handle);
    sl_cli_handle_input(handle, handle->input_buffer);
    sl_cli_input_clear(handle);
  }
}

/***************************************************************************//**
 * @brief
 *   Initialize session for a CLI instance.
 *
 * @details
 *   This function is called when initializing a CLI instance. It allows a security
 *   submodule to initialize its data, retrieve information and set state before the
 *   cli instance start.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance;
 *   Should set session_data pointer for storing data which can be retrieved in the
 *   subsequent calls. The same function is called for all instances, the function must
 *   handle the different instance.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise .
 ******************************************************************************/
__WEAK sl_status_t sli_cli_session_init(sl_cli_handle_t handle)
{
  (void)handle;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Handle Session; Authentication/logging/logout/lockup, etc.
 *
 * @details
 *   This function is called everytime the CLI tick function is executed. It doesn't
 *   mean an activity occured on the session (data received). This function can write
 *   and read iostream and the default stream. The CLI takes care of switching the default
 *   stream for every instance.
 *
 * @param[in] handle
 *   A handle to a CLI instance; Use session_data pointer for accessing instance's
 *   session data.
 *
 * @return SL_STATUS_OK if user can access the CLI instance, an error code otherwise .
 ******************************************************************************/
__WEAK sl_status_t sli_cli_session_handler(sl_cli_handle_t handle)
{
  (void)handle;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Common tick function.
 *   Checks for new input, and acts according to the possible input.
 ******************************************************************************/
__WEAK bool sli_cli_tick(sl_cli_handle_t handle)
{
  int c;
  bool newline = false;
  bool no_valid_input = false;

  if (handle->tick_in_progress) {
    return false;
  }

  if (sli_cli_session_handler(handle) != SL_STATUS_OK) {
    return false;
  }

  handle->tick_in_progress = true;

  if (handle->req_prompt) {
    handle->req_prompt = false;
    sli_cli_io_printf("%s", handle->prompt_string);
  }
#if defined(SL_CLI_ACTIVE_FLAG_EN)
  handle->active = false;
#endif

  do {
#if !defined(SL_CATALOG_KERNEL_PRESENT)
    if (handle->input_char != EOF) {
      c = handle->input_char;
      handle->input_char = EOF;
    } else
#endif
    {
      c = sli_cli_io_getchar();
    }
    if (c != EOF) {
      sli_cli_session_activity_notification(handle);
      newline = sl_cli_input_char(handle, (char)c);
    } else {
      no_valid_input = true;
    }
  } while ((c != EOF) && (!newline));

  if (newline) {
    sli_cli_handle_input_and_history(handle);
#if defined(SL_CLI_ACTIVE_FLAG_EN)
    handle->req_prompt = true;
    handle->active = true;
#else
    if (handle->req_prompt) {
      sli_cli_io_printf("%s", handle->prompt_string);
      handle->req_prompt = false;
    }
#endif
  }
  handle->tick_in_progress = false;

  return no_valid_input;
}

#if defined(SL_CATALOG_KERNEL_PRESENT)
/***************************************************************************//**
 * @brief
 *   The kernel tick function.
 ******************************************************************************/
static void tick_task(void *arg)
{
  sl_cli_handle_t handle = (sl_cli_handle_t) arg;

  sl_iostream_set_default(handle->iostream_handle);

  EFM_ASSERT(osDelay(handle->start_delay_tick) == osOK);

#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
  sl_cli_storage_nvm3_tick(handle);
#endif

  while (1) {
    bool no_valid_input;

    no_valid_input = sli_cli_tick(handle);
    if (no_valid_input) {
      EFM_ASSERT(osDelay(handle->loop_delay_tick) == osOK);
    }
  }
}

/***************************************************************************//**
 * @brief
 *   The kernel create task function.
 ******************************************************************************/
static sl_status_t create_task(sl_cli_handle_t handle,
                               sl_cli_instance_parameters_t *parameters)
{
  osThreadAttr_t attr;

  attr.name = parameters->task_name;
  attr.priority = parameters->prio;
  attr.stack_mem = parameters->stack;  // Will allocate dynamically if set to NULL
  attr.stack_size = parameters->stack_size;
  attr.cb_mem = parameters->thread_cb; // Will allocate dynamically if set to NULL
  attr.cb_size = osThreadCbSize;
  attr.attr_bits = 0u;
  attr.tz_module = 0u;

  parameters->thread_id = osThreadNew(&tick_task, handle, &attr);
  EFM_ASSERT(parameters->thread_id != NULL);

  return SL_STATUS_OK;
}
#endif

/*******************************************************************************
 ****************************   GLOBAL FUNCTIONS   *****************************
 ******************************************************************************/
uint8_t *sl_cli_get_argument_hex(sl_cli_command_arg_t *a, size_t n, size_t *l)
{
  // Byte 0 and 1 contains the length, while 2..n contains the data.
  uint8_t *ptr;
  uint8_t lo, hi;

  ptr = (uint8_t *)(a->argv[a->arg_ofs + n]);
  lo = *ptr++;
  hi = *ptr++;
  *l = (size_t)lo + ((size_t)hi << 8);
  return ptr;
}

void sl_cli_clear(sl_cli_handle_t handle)
{
  // Clear and initialize handle
  memset(handle, 0, sizeof(sl_cli_t));
  handle->input_size = SL_CLI_INPUT_BUFFER_SIZE;
  handle->input_pos = 0;
  handle->input_len = 0;
  handle->last_input_type = SL_CLI_INPUT_ORDINARY;
  handle->prompt_string = SL_CLI_PROMPT_STRING;
  handle->req_prompt = true;
#if SL_CLI_NUM_HISTORY_BYTES
  handle->history_pos = 0;
#endif
  sl_slist_init(&handle->command_group);
}

void sl_cli_redirect_command(sl_cli_handle_t handle, sl_cli_command_function_t command_function, const char *prompt, void *user)
{
  if ((handle->command_function == NULL) != (command_function == NULL)) {
    handle->command_function = command_function;
    handle->aux_argument = user;
    handle->prompt_string = (prompt == NULL) ? SL_CLI_PROMPT_STRING : (char *)prompt;
  }
}

sl_status_t sl_cli_handle_input(sl_cli_handle_t handle, char *string)
{
  sl_status_t status = SL_STATUS_OK;

  if (handle->command_function == NULL) {
    status = sl_cli_command_execute(handle, string);
    if (status != SL_STATUS_OK) {
      sli_cli_io_printf("%s\n", status_to_string(status));
    }
  } else {
    handle->command_function(string, handle->aux_argument);
  }

  return status;
}

sl_status_t sl_cli_instance_init(sl_cli_handle_t handle,
                                 sl_cli_instance_parameters_t *parameters)
{
  sl_status_t status = SL_STATUS_OK;

  sl_cli_clear(handle);
  handle->iostream_handle = parameters->iostream_handle;
  sl_cli_command_add_command_group(handle, parameters->default_command_group);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  handle->start_delay_tick = ((uint64_t)osKernelGetTickFreq() * parameters->start_delay_ms) / 1000;
  handle->loop_delay_tick = ((uint64_t)osKernelGetTickFreq() * parameters->loop_delay_ms) / 1000;
  status = create_task(handle, parameters);
#else
  handle->input_char = EOF;
#endif

  if (status != SL_STATUS_OK) {
    return status;
  }

  status = sli_cli_session_init(handle);
  if (status != SL_STATUS_OK) {
    return status;
  }

#if defined(SL_CLI_ACTIVE_FLAG_EN)
  handle->active = true;
#endif

  return status;
}

#if !defined(SL_CATALOG_KERNEL_PRESENT)
bool sl_cli_is_ok_to_sleep(sl_cli_handle_t handle)
{
  if (handle->input_char == EOF) {
    handle->input_char = sli_cli_io_getchar();
  }
  if (handle->input_char != EOF) {
    return false;
  }
  if (handle->block_sleep) {
    return false;
  }

#if defined(SL_CLI_ACTIVE_FLAG_EN)
  return !handle->active;
#else
  return true;
#endif
}

void sl_cli_tick_instance(sl_cli_handle_t handle)
{
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
  if (sl_cli_delay_tick(handle)) {
    return;
  }
#endif
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
  if (sl_cli_storage_nvm3_tick(handle)) {
    return;
  }
#endif
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
  if (sl_cli_storage_ram_tick(handle)) {
    return;
  }
#endif
  sli_cli_tick(handle);
}
#endif
