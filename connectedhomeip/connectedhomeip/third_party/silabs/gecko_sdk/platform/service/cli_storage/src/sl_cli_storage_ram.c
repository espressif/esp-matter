/***************************************************************************//**
 * @file sl_cli_storage_ram.c
 * @brief Implement storing of CLI command lines in RAM.
 * @version x.x.x
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
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
#include "sl_cli.h"
#include "sl_cli_command.h"
#include "sl_cli_storage_ram_instances.h"
#include "sl_cli_storage_ram.h"
#include "sl_string.h"
#include <string.h>

/*******************************************************************************
 *****************************   DATA TYPES   *********************************
 ******************************************************************************/

typedef void(*enum_function_t)(cli_storage_ram_handle_t handle, char *str);

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Compare two command strings. If the comparison shall be case sensitive or
 *   not can be configured with SL_CLI_IGNORE_COMMAND_CASE.
 *
 * @param[in] a         String to compare.
 *
 * @param[in] b         String to compare.
 *
 * @return              An integer greater than, or less than 0 if the strings
 *                      are not equal. 0 if the strings are equal.
 ******************************************************************************/
static int cmd_strcmp(const char *a, const char *b)
{
#if SL_CLI_IGNORE_COMMAND_CASE
  return sl_strcasecmp(a, b);
#else
  return strcmp(a, b);
#endif
}

static char *buf_adr(cli_storage_ram_handle_t handle, size_t line)
{
  return handle->ram_buffer + (line * SL_CLI_INPUT_BUFFER_SIZE);
}

static void execute_for_index(cli_storage_ram_handle_t handle, size_t i, enum_function_t func)
{
  char str[SL_CLI_INPUT_BUFFER_SIZE];

  memcpy(str, buf_adr(handle, i), SL_CLI_INPUT_BUFFER_SIZE);
  func(handle, str);
}

static void ram_enum(cli_storage_ram_handle_t handle, enum_function_t func)
{
  for (size_t i = 0; i < handle->ram_next; i++) {
    execute_for_index(handle, i, func);
  }
}

static void ram_list(cli_storage_ram_handle_t handle, char *str)
{
  (void)handle;
  printf("%s\n", str);
}

static void ram_execute(cli_storage_ram_handle_t handle, char *str)
{
  sl_cli_command_execute(handle->cli_handle, str);
}

static bool ram_add(cli_storage_ram_handle_t handle, char *str)
{
  bool status = false;

  if (handle->ram_next < handle->ram_size) {
    sl_strcpy_s(buf_adr(handle, handle->ram_next), SL_CLI_INPUT_BUFFER_SIZE, str);
    handle->ram_next++;
    status = true;
  }
  if (handle->execute_while_define) {
    ram_execute(handle, str);
  }

  return status;
}

static void ram_redirect(char *arg_str, void *user)
{
  cli_storage_ram_handle_t handle = (cli_storage_ram_handle_t)user;

  if (cmd_strcmp(arg_str, handle->end_string) == 0) {
    sl_cli_redirect_command(handle->cli_handle, NULL, NULL, NULL);
    handle->define_in_progress = false;
  } else {
    if (!ram_add(handle, arg_str)) {
      printf("ERROR: No more space to store cli commands in RAM.\n");
    }
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void sl_cli_storage_ram_clear(sl_cli_command_arg_t *arguments)
{
  cli_storage_ram_handle_t handle;

  handle = sl_cli_storage_ram_instances_convert_handle(arguments->handle);
  handle->ram_next = 0;
}

void sl_cli_storage_ram_list(sl_cli_command_arg_t *arguments)
{
  cli_storage_ram_handle_t handle;

  handle = sl_cli_storage_ram_instances_convert_handle(arguments->handle);
  ram_enum(handle, ram_list);
}

void sl_cli_storage_ram_define(sl_cli_command_arg_t *arguments)
{
  cli_storage_ram_handle_t handle;

  handle = sl_cli_storage_ram_instances_convert_handle(arguments->handle);
  if (!handle->execute_in_progress) {
    handle->define_in_progress = true;
    sl_cli_redirect_command(handle->cli_handle, ram_redirect, handle->prompt, handle);
  }
}

void sl_cli_storage_ram_execute(sl_cli_command_arg_t *arguments)
{
  cli_storage_ram_handle_t handle;

  handle = sl_cli_storage_ram_instances_convert_handle(arguments->handle);
  if (handle->execute_in_progress) {
    return;
  }
  handle->execute_in_progress = true;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  ram_enum(handle, ram_execute);
  handle->execute_in_progress = false;
#else
  handle->exec_offset = 0;
#endif
}

size_t sl_cli_storage_ram_count(sl_cli_handle_t cli_handle)
{
  cli_storage_ram_handle_t handle;

  handle = sl_cli_storage_ram_instances_convert_handle(cli_handle);
  return handle->ram_next;
}

void sl_cli_storage_ram_init(cli_storage_ram_handle_t handle)
{
  handle->ram_next = 0;
  handle->define_in_progress = false;
  handle->execute_in_progress = false;
#if !defined(SL_CATALOG_KERNEL_PRESENT)
  handle->exec_offset = 0;
#endif
}

#if !defined(SL_CATALOG_KERNEL_PRESENT)
bool sl_cli_storage_ram_tick(sl_cli_handle_t cli_handle)
{
  cli_storage_ram_handle_t handle;
  bool busy = false;

  handle = sl_cli_storage_ram_instances_convert_handle(cli_handle);
  if (handle->execute_in_progress) {
    if (handle->exec_offset < handle->ram_next) {
      execute_for_index(handle, handle->exec_offset, ram_execute);
      handle->exec_offset++;
    }
    handle->execute_in_progress = (handle->exec_offset < handle->ram_next);
    busy = handle->execute_in_progress;
  }

  return busy;
}
#endif
