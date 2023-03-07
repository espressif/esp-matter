/***************************************************************************//**
 * @file sl_cli_storage_nvm3.c
 * @brief Implement storing of CLI command lines in NVM3.
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
#include "sl_cli_storage_nvm3_instances.h"
#include "sl_cli_storage_nvm3.h"
#include "sl_string.h"
#include <string.h>

/*******************************************************************************
 *****************************   DATA TYPES   *********************************
 ******************************************************************************/

typedef void(*enum_function_t)(cli_storage_nvm3_handle_t handle, char *str);

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

static void execute_for_key(nvm3_ObjectKey_t key, cli_storage_nvm3_handle_t handle, enum_function_t func)
{
  Ecode_t sta;
  uint32_t obj_type;
  size_t obj_len;
  char str[SL_CLI_INPUT_BUFFER_SIZE];

  sta = nvm3_getObjectInfo(nvm3_defaultHandle, key, &obj_type, &obj_len);
  if ((sta == ECODE_NVM3_OK) && (obj_len <= sizeof(str))) {
    sta = nvm3_readData(nvm3_defaultHandle, key, str, obj_len);
    if (sta == ECODE_NVM3_OK) {
      func(handle, str);
    }
  }
}

static void nvm3_enum(cli_storage_nvm3_handle_t handle, enum_function_t func)
{
  nvm3_ObjectKey_t key;

  for (size_t ofs = 0; ofs < handle->key_count; ofs++) {
    key = SL_CLI_STORAGE_NVM3_KEY_BEGIN + handle->key_offset + ofs;
    execute_for_key(key, handle, func);
  }
}

static void nvm3_content(cli_storage_nvm3_handle_t handle, char *str)
{
  (void)str;
  if (handle->key_next < (handle->key_offset + handle->key_count)) {
    handle->key_next++;
  }
}

static void content_find(cli_storage_nvm3_handle_t handle)
{
  handle->key_next = handle->key_offset;
  nvm3_enum(handle, nvm3_content);
}

static void nvm3_list(cli_storage_nvm3_handle_t handle, char *str)
{
  (void)handle;
  printf("%s\n", str);
}

static void nvm3_execute(cli_storage_nvm3_handle_t handle, char *str)
{
  sl_cli_command_execute(handle->cli_handle, str);
}

static bool nvm3_add(cli_storage_nvm3_handle_t handle, char *str)
{
  nvm3_ObjectKey_t key;
  bool status = false;

  if (handle->key_next < (handle->key_offset + handle->key_count)) {
    key = SL_CLI_STORAGE_NVM3_KEY_BEGIN + handle->key_offset + handle->key_next;
    nvm3_writeData(nvm3_defaultHandle, key, str, strlen(str) + 1);
    handle->key_next++;
    status = true;
  }
  if (handle->execute_while_define) {
    nvm3_execute(handle, str);
  }

  return status;
}

static void nvm3_redirect(char *arg_str, void *user)
{
  cli_storage_nvm3_handle_t handle = (cli_storage_nvm3_handle_t)user;

  if (cmd_strcmp(arg_str, handle->end_string) == 0) {
    sl_cli_redirect_command(handle->cli_handle, NULL, NULL, NULL);
  } else {
    if (!nvm3_add(handle, arg_str)) {
      printf("ERROR: No more space to store cli commands in FLASH.\n");
    }
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void sl_cli_storage_nvm3_clear(sl_cli_command_arg_t *arguments)
{
  nvm3_ObjectKey_t key;
  cli_storage_nvm3_handle_t handle;

  handle = sl_cli_storage_nvm3_instances_convert_handle(arguments->handle);
  for (size_t ofs = 0; ofs < handle->key_count; ofs++) {
    key = SL_CLI_STORAGE_NVM3_KEY_BEGIN + handle->key_offset + ofs;
    nvm3_deleteObject(nvm3_defaultHandle, key);
  }
  handle->key_next = handle->key_offset;
}

void sl_cli_storage_nvm3_list(sl_cli_command_arg_t *arguments)
{
  cli_storage_nvm3_handle_t handle;

  handle = sl_cli_storage_nvm3_instances_convert_handle(arguments->handle);
  nvm3_enum(handle, nvm3_list);
}

void sl_cli_storage_nvm3_define(sl_cli_command_arg_t *arguments)
{
  cli_storage_nvm3_handle_t handle;

  handle = sl_cli_storage_nvm3_instances_convert_handle(arguments->handle);
  if (!handle->execute_in_progress) {
    sl_cli_redirect_command(handle->cli_handle, nvm3_redirect, handle->prompt, handle);
  }
}

void sl_cli_storage_nvm3_execute(sl_cli_command_arg_t *arguments)
{
  cli_storage_nvm3_handle_t handle;

  handle = sl_cli_storage_nvm3_instances_convert_handle(arguments->handle);
  if (handle->execute_in_progress) {
    return;
  }
  handle->execute_in_progress = true;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  nvm3_enum(handle, nvm3_execute);
  handle->execute_in_progress = false;
#else
  handle->exec_offset = 0;
#endif
}

size_t sl_cli_storage_nvm3_count(sl_cli_handle_t cli_handle)
{
  cli_storage_nvm3_handle_t handle;

  handle = sl_cli_storage_nvm3_instances_convert_handle(cli_handle);
  return (handle->key_next - handle->key_offset);
}

void sl_cli_storage_nvm3_init(cli_storage_nvm3_handle_t handle)
{
  handle->define_in_progress = false;
  handle->execute_in_progress = false;
  handle->init_tick = false;
#if !defined(SL_CATALOG_KERNEL_PRESENT)
  handle->exec_offset = 0;
#endif
  content_find(handle);
}

bool sl_cli_storage_nvm3_tick(sl_cli_handle_t cli_handle)
{
  sl_cli_command_arg_t arguments;
  cli_storage_nvm3_handle_t handle;
  bool busy = false;

  handle = sl_cli_storage_nvm3_instances_convert_handle(cli_handle);
  if (!handle->init_tick) {
    handle->init_tick = true;
    arguments.handle = cli_handle;
    sl_cli_storage_nvm3_execute(&arguments);
#if !defined(SL_CATALOG_KERNEL_PRESENT)
    busy = handle->execute_in_progress;
  } else if (handle->execute_in_progress) {
    nvm3_ObjectKey_t key;

    if (handle->exec_offset < handle->key_count) {
      key = SL_CLI_STORAGE_NVM3_KEY_BEGIN + handle->key_offset + handle->exec_offset;
      execute_for_key(key, handle, nvm3_execute);
      handle->exec_offset++;
    }
    handle->execute_in_progress = (handle->exec_offset < handle->key_count);
    busy = handle->execute_in_progress;
#endif
  }

  return busy;
}
