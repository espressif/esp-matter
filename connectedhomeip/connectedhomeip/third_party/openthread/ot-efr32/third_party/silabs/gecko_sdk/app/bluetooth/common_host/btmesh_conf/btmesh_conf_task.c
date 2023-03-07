/***************************************************************************/ /**
 * @file
 * @brief BT Mesh Configurator Component - Task
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "btmesh_conf_task.h"

#include <stdint.h>
#include <stdlib.h>
#include "sl_status.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_capi_types.h"
#include "app_log.h"
#include "app_assert.h"
#include "btmesh_db.h"
#include "btmesh_conf.h"
#include "btmesh_conf_task.h"
#include "btmesh_conf_config.h"

/***************************************************************************//**
 * @addtogroup btmesh_conf BT Mesh Configurator Component
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_task BT Mesh Configuration Task
 * @{
 ******************************************************************************/
/// Local log new line shortcut definition
#define NL APP_LOG_NL

/***************************************************************************//**
 * Provide 16-bit unsigned value from little endian byte array
 *
 * @param[in] ptr Minimum two byte long little endian byte array
 * @returns 16-bit unsigned value read from byte array
 ******************************************************************************/
static uint16_t uint16_from_buf_le(const uint8_t *ptr)
{
  return ((uint16_t)ptr[0]) | ((uint16_t)ptr[1] << 8);
}

/***************************************************************************//**
 * Collect all 16-bit unsigned values from a bytearray and append it to the
 * passed unsigned 16-bit array by reallocating it to the proper size.
 *
 * @param[inout] buffer_ptr Pointer to the address of the existing dynamically
 *   allocated 16-bit array. The @p buffer_ptr shall have non-NULL value but
 *   the dereferenced value of @p buffer_ptr can be NULL.
 * @param[inout] buffer_len Length of @p buffer_ptr 16-bit array.
 *   The new length of the reallocated buffer is stored here.
 * @param[in] buffer_len_max Maximum length of @p buffer_ptr 16-bit array.
 * @param[in] array_data Bytearray where the 16-bit unsigned values shall be
 *   collected from.
 * @param[in] array_size Size of @p array_data bytearray.
 * @returns Status of collection.
 * @retval SL_STATUS_OK If every 16-bit value is collected from the bytearray
 *   and stored in the reallocated unsigned 16-bit array.
 * @retval SL_STATUS_NULL_POINTER If @p buffer_ptr is NULL.
 * @retval SL_STATUS_INVALID_COUNT If the @p buffer_len after reallocation would
 *   exceed @p buffer_len_max or the @p array_size is odd.
 * @retval SL_STATUS_ALLOCATION_FAILED If the reallocation of the @p buffer_ptr
 *   fails. The original buffer is not deallocated in this case.
 *
 * The @p buffer_ptr pointer shall reference the address of an existing
 * dynamically allocated (malloc, realloc, calloc) 16-bit array or the
 * dereferenced value shall be a NULL pointer. If the dereferenced @p buffer_ptr
 * is NULL then this function allocates a new buffer otherwise it reallocates an
 * existing buffer. This behavior makes it possible to build an unsigned 16-bit
 * array from multiple byte arrays by calling this function multiple times.
 ******************************************************************************/
static sl_status_t collect_uint16_le_data_from_array(uint16_t **const buffer_ptr,
                                                     uint16_t *const buffer_len,
                                                     uint16_t buffer_len_max,
                                                     const uint8_t *const array_data,
                                                     uint8_t array_size)
{
  const uint16_t elem_size = (uint16_t)sizeof(uint16_t);
  sl_status_t collect_status = SL_STATUS_FAIL;
  uint16_t current_buffer_len = *buffer_len;
  uint32_t new_buffer_len = current_buffer_len + (array_size / elem_size);
  if (NULL == buffer_ptr) {
    // The buffer_ptr shall not be NULL because the pointer to reallocated
    // unsigned 16-bit array shall be stored somewhere (avoid memory leaks)
    collect_status = SL_STATUS_NULL_POINTER;
  } else if ((0 != (array_size % elem_size))
             || (buffer_len_max < new_buffer_len)) {
    // The size of the new unsigned 16-bit array shall not exceed the maximum
    // and the input bytearray which contains 16-bit values shall have even
    // number of bytes.
    collect_status = SL_STATUS_INVALID_COUNT;
  } else {
    // If the pointer parameter of realloc is NULL then it behaves like malloc
    uint16_t *reallocated_buffer_ptr =
      (uint16_t *)realloc(*buffer_ptr,
                          new_buffer_len * sizeof(uint16_t));

    if (NULL == reallocated_buffer_ptr) {
      collect_status = SL_STATUS_ALLOCATION_FAILED;
    } else {
      // Append the new 16-bit unsigned values
      for (uint16_t data_idx = 0; data_idx < array_size; data_idx += 2) {
        reallocated_buffer_ptr[current_buffer_len++] =
          uint16_from_buf_le(&array_data[data_idx]);
      }
      *buffer_ptr = reallocated_buffer_ptr;
      *buffer_len = current_buffer_len;
      collect_status = SL_STATUS_OK;
    }
  }
  return collect_status;
}

/***************************************************************************//**
 * Process configuration request status from BT Mesh Stack API.
 *
 * @param[in] self Pointer to the configuration task
 * @param[in] request_status Status code returned from BT Mesh Stack
 *   configuration request.
 * @returns Mapped configuration request status.
 * @retval SL_STATUS_OK If request is accepted.
 * @retval SL_STATUS_BUSY If request is rejected due to busy lower layers.
 * @retval SL_STATUS_FAIL If request is rejected due to unrecoverable error.
 *
 * The @ref btmesh_conf_task_t::result is set to @p request_status.
 * The different BT Mesh Stack Configuration Client API functions have consistent
 * return values so this function maps these common return values to match the
 * interface description of @ref btmesh_conf_task_t::conf_request.
 ******************************************************************************/
static sl_status_t process_request_status_from_api(btmesh_conf_task_t *const self,
                                                   sl_status_t request_status)
{
  if (SL_STATUS_OK != request_status) {
    // If BT Mesh Stack configuration request fails then the task fails so the
    // configuration task result is set to specific BT Mesh Stack return value.
    // If the configuration request is successful then configuration task result
    // is not set because it will be determined by the BT Mesh Stack events.
    self->result = request_status;
  }

  switch (request_status) {
    case SL_STATUS_OK:
      return request_status;
    case SL_STATUS_NO_MORE_RESOURCE:
      return SL_STATUS_BUSY;
    default:
      return SL_STATUS_FAIL;
  }
}

/***************************************************************************//**
 * Process event result from BT Mesh Stack configuration client events.
 *
 * @param[in] self Pointer to the configuration task
 * @param[in] event_result Event result from the BT Mesh Stack configuration
 *   client event.
 * @returns Mapped configuration event result.
 * @retval SL_STATUS_OK If task is finished successfully.
 * @retval SL_STATUS_TIMEOUT If task timed out while it was waiting for events.
 * @retval SL_STATUS_FAIL If task failed due to server side error.
 *
 * The @ref btmesh_conf_task_t::result is set to @p event_result.
 * Note: some BT Mesh Stack configuration client events does not have result
 *   member because these events are intermediate events.
 ******************************************************************************/
static sl_status_t process_event_result_from_api(btmesh_conf_task_t *const self,
                                                 sl_status_t event_result)
{
  self->result = event_result;

  switch (event_result) {
    case SL_STATUS_OK:
    case SL_STATUS_TIMEOUT:
      return event_result;
    default:
      return SL_STATUS_FAIL;
  }
}

// -----------------------------------------------------------------------------
//                    Generic Configuration Task Functions
// -----------------------------------------------------------------------------
static sl_status_t btmesh_conf_task_request(btmesh_conf_task_t *const self,
                                            uint16_t enc_netkey_index,
                                            uint16_t server_address,
                                            uint32_t *handle);

static sl_status_t btmesh_conf_task_on_event(btmesh_conf_task_t *const self,
                                             uint16_t enc_netkey_index,
                                             uint16_t server_address,
                                             const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_to_string(btmesh_conf_task_t *const self,
                                          char *buffer,
                                          uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_create(btmesh_conf_task_id_t task_id)
{
  btmesh_conf_task_t *self;

  self = malloc(sizeof(btmesh_conf_task_t));

  if (NULL != self) {
    self->task_id = task_id;
    self->result = BTMESH_CONF_TASK_RESULT_UNKNOWN;
    self->conf_request = btmesh_conf_task_request;
    self->on_event = btmesh_conf_task_on_event;
    self->to_string = btmesh_conf_task_to_string;
    self->destroy = btmesh_conf_task_destroy;
    self->next_on_success = NULL;
    self->next_on_failure = NULL;
  }
  return self;
}

void btmesh_conf_task_destroy(btmesh_conf_task_t *self)
{
  // Destroy function shall behave as free if NULL pointer is passed
  if (NULL == self) {
    return;
  }

  if (NULL != self->next_on_failure) {
    self->next_on_failure->destroy(self->next_on_failure);
  }
  if (NULL != self->next_on_success) {
    self->next_on_success->destroy(self->next_on_success);
  }
  free(self);
}

sl_status_t btmesh_conf_task_set_next_unconditional(btmesh_conf_task_t *self,
                                                    btmesh_conf_task_t *const next_task)
{
  if ((NULL == self) || (NULL == next_task)) {
    return SL_STATUS_NULL_POINTER;
  }
  // The task tree shall be built from the root node to the leaf nodes which
  // makes it possible to avoid cycle in the graph
  if ((self == next_task)
      && (next_task->next_on_success != NULL)
      && (next_task->next_on_failure != NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  self->next_on_failure = next_task;
  self->next_on_success = next_task;
  return SL_STATUS_OK;
}

sl_status_t btmesh_conf_task_set_next_on_success(btmesh_conf_task_t *self,
                                                 btmesh_conf_task_t *const next_task)
{
  if ((NULL == self) || (NULL == next_task)) {
    return SL_STATUS_NULL_POINTER;
  }
  // The task tree shall be built from the root node to the leaf nodes which
  // makes it possible to avoid cycle in the graph
  if ((self == next_task)
      && (next_task->next_on_success != NULL)
      && (next_task->next_on_failure != NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  self->next_on_success = next_task;
  return SL_STATUS_OK;
}

sl_status_t btmesh_conf_task_set_next_on_failure(btmesh_conf_task_t *self,
                                                 btmesh_conf_task_t *const next_task)
{
  if ((NULL == self) || (NULL == next_task)) {
    return SL_STATUS_NULL_POINTER;
  }
  // The task tree shall be built from the root node to the leaf nodes which
  // makes it possible to avoid cycle in the graph
  if ((self == next_task)
      && (next_task->next_on_success != NULL)
      && (next_task->next_on_failure != NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  self->next_on_failure = next_task;
  return SL_STATUS_OK;
}

static sl_status_t btmesh_conf_task_request(btmesh_conf_task_t *const self,
                                            uint16_t enc_netkey_index,
                                            uint16_t server_address,
                                            uint32_t *handle)
{
  return SL_STATUS_FAIL;
}

static sl_status_t btmesh_conf_task_on_event(btmesh_conf_task_t *const self,
                                             uint16_t enc_netkey_index,
                                             uint16_t server_address,
                                             const sl_btmesh_msg_t *evt)
{
  return SL_STATUS_FAIL;
}

static int32_t btmesh_conf_task_to_string(btmesh_conf_task_t *const self,
                                          char *buffer,
                                          uint32_t size)
{
  return snprintf(buffer, size, "generic (id=%d)", self->task_id);
}

// -----------------------------------------------------------------------------
//                          Appkey Add Task Functions
// -----------------------------------------------------------------------------

static sl_status_t
btmesh_conf_task_appkey_add_request(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    uint32_t *handle);

static sl_status_t
btmesh_conf_task_appkey_add_on_event(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_appkey_add_to_string(btmesh_conf_task_t *const self,
                                                     char *buffer,
                                                     uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_appkey_add_create(uint16_t appkey_index,
                                                       uint16_t netkey_index)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_APPKEY_ADD);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_appkey_add_request;
    self->on_event = btmesh_conf_task_appkey_add_on_event;
    self->destroy = btmesh_conf_task_appkey_add_destroy;
    self->to_string = btmesh_conf_task_appkey_add_to_string;
    self->ext.appkey.appkey_index = appkey_index;
    self->ext.appkey.netkey_index = netkey_index;
  }
  return self;
}

void btmesh_conf_task_appkey_add_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_appkey_add_request(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint16_t appkey_index = self->ext.appkey.appkey_index;
  uint16_t netkey_index = self->ext.appkey.netkey_index;

  api_status = sl_btmesh_config_client_add_appkey(enc_netkey_index,
                                                  server_address,
                                                  appkey_index,
                                                  netkey_index,
                                                  handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_appkey_add_on_event(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_appkey_status_id:
    {
      const sl_btmesh_evt_config_client_appkey_status_t *appkey_status =
        &evt->data.evt_config_client_appkey_status;

      // If timeout is returned as event status then the upper layer might send
      // the configuration message again. If configuration server receives the
      // same Config AppKey Add message twice (same indexes with same appkey)
      // then it sends back the Config AppKey Status message with success status.
      sl_status_t event_result = appkey_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_appkey_add_to_string(btmesh_conf_task_t *const self,
                                                     char *buffer,
                                                     uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "appkey add (appkey_idx=%u,netkey_idx=%u)",
                  self->ext.appkey.appkey_index,
                  self->ext.appkey.netkey_index);
}

// -----------------------------------------------------------------------------
//                         Appkey Remove Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_appkey_remove_request(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       uint32_t *handle);

static sl_status_t
btmesh_conf_task_appkey_remove_on_event(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_appkey_remove_to_string(btmesh_conf_task_t *const self,
                                                        char *buffer,
                                                        uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_appkey_remove_create(uint16_t appkey_index,
                                                          uint16_t netkey_index)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_APPKEY_REMOVE);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_appkey_remove_request;
    self->on_event = btmesh_conf_task_appkey_remove_on_event;
    self->destroy = btmesh_conf_task_appkey_remove_destroy;
    self->to_string = btmesh_conf_task_appkey_remove_to_string;
    self->ext.appkey.appkey_index = appkey_index;
    self->ext.appkey.netkey_index = netkey_index;
  }
  return self;
}

void btmesh_conf_task_appkey_remove_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_appkey_remove_request(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint16_t appkey_index = self->ext.appkey.appkey_index;
  uint16_t netkey_index = self->ext.appkey.netkey_index;

  api_status = sl_btmesh_config_client_remove_appkey(enc_netkey_index,
                                                     server_address,
                                                     appkey_index,
                                                     netkey_index,
                                                     handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_appkey_remove_on_event(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_appkey_status_id:
    {
      const sl_btmesh_evt_config_client_appkey_status_t *appkey_status =
        &evt->data.evt_config_client_appkey_status;

      // If timeout is returned as event status then the upper layer might send
      // the configuration message again. If configuration server receives the
      // same Config AppKey Delete message twice (same indexes with same appkey)
      // then it sends back the Config AppKey Status message with success status.
      sl_status_t event_result = appkey_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_appkey_remove_to_string(btmesh_conf_task_t *const self,
                                                        char *buffer,
                                                        uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "appkey remove (appkey_idx=%u,netkey_idx=%u)",
                  self->ext.appkey.appkey_index,
                  self->ext.appkey.netkey_index);
}

// -----------------------------------------------------------------------------
//                          Appkey List Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_appkey_list_request(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     uint32_t *handle);

static sl_status_t
btmesh_conf_task_appkey_list_on_event(btmesh_conf_task_t *const self,
                                      uint16_t enc_netkey_index,
                                      uint16_t server_address,
                                      const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_appkey_list_to_string(btmesh_conf_task_t *const self,
                                                      char *buffer,
                                                      uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_appkey_list_create(uint16_t netkey_index)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_APPKEY_LIST);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_appkey_list_request;
    self->on_event = btmesh_conf_task_appkey_list_on_event;
    self->destroy = btmesh_conf_task_appkey_list_destroy;
    self->to_string = btmesh_conf_task_appkey_list_to_string;
    self->ext.appkey_list.netkey_index = netkey_index;
    self->ext.appkey_list.appkey_indexes = NULL;
    self->ext.appkey_list.appkey_count = 0;
  }
  return self;
}

void btmesh_conf_task_appkey_list_destroy(btmesh_conf_task_t *self)
{
  free(self->ext.appkey_list.appkey_indexes);
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_appkey_list_request(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint16_t netkey_index = self->ext.appkey_list.netkey_index;

  // Make sure if this is a retry then dynamic memory allocated to store appkey
  // data is deallocated in order to restore the initial state of the task.
  if (0 < self->ext.appkey_list.appkey_count) {
    free(self->ext.appkey_list.appkey_indexes);
    self->ext.appkey_list.appkey_indexes = NULL;
    self->ext.appkey_list.appkey_count = 0;
  }

  api_status = sl_btmesh_config_client_list_appkeys(enc_netkey_index,
                                                    server_address,
                                                    netkey_index,
                                                    handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_appkey_list_on_event(btmesh_conf_task_t *const self,
                                      uint16_t enc_netkey_index,
                                      uint16_t server_address,
                                      const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_appkey_list_id:
    {
      const sl_btmesh_evt_config_client_appkey_list_t *appkey_list =
        &evt->data.evt_config_client_appkey_list;
      // This event might be generated more than once so it is possible that
      // some appkeys has already been processed.

      sl_status_t collect_status =
        collect_uint16_le_data_from_array(&self->ext.appkey_list.appkey_indexes,
                                          &self->ext.appkey_list.appkey_count,
                                          BTMESH_CONF_MAX_APPKEY_COUNT,
                                          appkey_list->appkey_indices.data,
                                          appkey_list->appkey_indices.len);

      if (SL_STATUS_OK != collect_status) {
        app_log_status_error_f(collect_status,
                               "Node (netkey_idx=%d,addr=0x%04x) failed to "
                               "collect appkeys from appkey list event." NL,
                               enc_netkey_index,
                               server_address);
        event_status = SL_STATUS_ABORT;
      }
      break;
    }

    case sl_btmesh_evt_config_client_appkey_list_end_id:
    {
      const sl_btmesh_evt_config_client_appkey_list_end_t *appkey_list_end =
        &evt->data.evt_config_client_appkey_list_end;
      sl_status_t event_result = appkey_list_end->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_appkey_list_to_string(btmesh_conf_task_t *const self,
                                                      char *buffer,
                                                      uint32_t size)
{
  int32_t required_space;
  int32_t total_required_space = 0;
  uint32_t remaining_size = size;
  if (SL_STATUS_OK != self->result) {
    // Response with the appkey list is not received from the remote node yet
    return snprintf(buffer,
                    size,
                    "appkey list (netkey_idx=%u)",
                    self->ext.appkey_list.netkey_index);
  }

  required_space = snprintf(buffer,
                            size,
                            "appkey list (netkey_idx=%u,appkey_cnt=%d%s",
                            self->ext.appkey_list.netkey_index,
                            self->ext.appkey_list.appkey_count,
                            (0 == self->ext.appkey_list.appkey_count)
                            ? ")" : ",appkey_idxs=");
  if (required_space < 0) {
    return required_space;
  } else if (remaining_size <= required_space) {
    remaining_size = 0;
  } else {
    remaining_size -= required_space;
  }
  total_required_space += required_space;

  uint16_t appkey_count = self->ext.appkey_list.appkey_count;
  for (uint16_t idx = 0; idx < appkey_count; idx++) {
    required_space = snprintf(&buffer[total_required_space],
                              remaining_size,
                              (idx != (appkey_count - 1)) ? "%d," : "%d)",
                              self->ext.appkey_list.appkey_indexes[idx]);
    // The loop is not terminated if the buffer space runs out in order to
    // calculate the total required buffer size.
    // Note: return value of to_string functions shall be interpreted as snprintf
    if (required_space < 0) {
      return required_space;
    } else if (remaining_size <= required_space) {
      remaining_size = 0;
    } else {
      remaining_size -= required_space;
    }
    total_required_space += required_space;
  }
  return total_required_space;
}

// -----------------------------------------------------------------------------
//                          Model Bind Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_model_bind_request(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    uint32_t *handle);

static sl_status_t
btmesh_conf_task_model_bind_on_event(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_model_bind_to_string(btmesh_conf_task_t *const self,
                                                     char *buffer,
                                                     uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_model_bind_create(uint8_t elem_index,
                                                       uint16_t vendor_id,
                                                       uint16_t model_id,
                                                       uint16_t appkey_index)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_MODEL_BIND);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_model_bind_request;
    self->on_event = btmesh_conf_task_model_bind_on_event;
    self->destroy = btmesh_conf_task_model_bind_destroy;
    self->to_string = btmesh_conf_task_model_bind_to_string;
    self->ext.model_binding.elem_index = elem_index;
    self->ext.model_binding.vendor_id = vendor_id;
    self->ext.model_binding.model_id = model_id;
    self->ext.model_binding.appkey_index = appkey_index;
  }
  return self;
}

void btmesh_conf_task_model_bind_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_model_bind_request(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint8_t elem_index = self->ext.model_binding.elem_index;
  uint16_t vendor_id = self->ext.model_binding.vendor_id;
  uint16_t model_id = self->ext.model_binding.model_id;
  uint16_t appkey_index = self->ext.model_binding.appkey_index;

  api_status = sl_btmesh_config_client_bind_model(enc_netkey_index,
                                                  server_address,
                                                  elem_index,
                                                  vendor_id,
                                                  model_id,
                                                  appkey_index,
                                                  handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_model_bind_on_event(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_binding_status_id:
    {
      const sl_btmesh_evt_config_client_binding_status_t *binding_status =
        &evt->data.evt_config_client_binding_status;

      // If timeout is returned as event status then the upper layer might send
      // the configuration message again. If configuration server receives the
      // same Config Model App Bind message twice with same content then it
      // sends back the Config Model App Status message with success status.
      sl_status_t event_result = binding_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_model_bind_to_string(btmesh_conf_task_t *const self,
                                                     char *buffer,
                                                     uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "model bind (elem=%u,vendor=0x%04x,model=0x%04x,appkey_idx=%u)",
                  self->ext.model_binding.elem_index,
                  self->ext.model_binding.vendor_id,
                  self->ext.model_binding.model_id,
                  self->ext.model_binding.appkey_index);
}

// -----------------------------------------------------------------------------
//                         Model Unbind Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_model_unbind_request(btmesh_conf_task_t *const self,
                                      uint16_t enc_netkey_index,
                                      uint16_t server_address,
                                      uint32_t *handle);

static sl_status_t
btmesh_conf_task_model_unbind_on_event(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_model_unbind_to_string(btmesh_conf_task_t *const self,
                                                       char *buffer,
                                                       uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_model_unbind_create(uint8_t elem_index,
                                                         uint16_t vendor_id,
                                                         uint16_t model_id,
                                                         uint16_t appkey_index)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_MODEL_UNBIND);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_model_unbind_request;
    self->on_event = btmesh_conf_task_model_unbind_on_event;
    self->destroy = btmesh_conf_task_model_unbind_destroy;
    self->to_string = btmesh_conf_task_model_unbind_to_string;
    self->ext.model_binding.elem_index = elem_index;
    self->ext.model_binding.vendor_id = vendor_id;
    self->ext.model_binding.model_id = model_id;
    self->ext.model_binding.appkey_index = appkey_index;
  }
  return self;
}

void btmesh_conf_task_model_unbind_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_model_unbind_request(btmesh_conf_task_t *const self,
                                      uint16_t enc_netkey_index,
                                      uint16_t server_address,
                                      uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint8_t elem_index = self->ext.model_binding.elem_index;
  uint16_t vendor_id = self->ext.model_binding.vendor_id;
  uint16_t model_id = self->ext.model_binding.model_id;
  uint16_t appkey_index = self->ext.model_binding.appkey_index;

  api_status = sl_btmesh_config_client_unbind_model(enc_netkey_index,
                                                    server_address,
                                                    elem_index,
                                                    vendor_id,
                                                    model_id,
                                                    appkey_index,
                                                    handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_model_unbind_on_event(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_binding_status_id:
    {
      const sl_btmesh_evt_config_client_binding_status_t *binding_status =
        &evt->data.evt_config_client_binding_status;

      // If timeout is returned as event status then the upper layer might send
      // the configuration message again. If configuration server receives the
      // same Config Model App Unbind message twice with same content then it
      // sends back the Config Model App Status message with success status.
      sl_status_t event_result = binding_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_model_unbind_to_string(btmesh_conf_task_t *const self,
                                                       char *buffer,
                                                       uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "model unbind (elem=%u,vendor=0x%04x,model=0x%04x,appkey_idx=%u)",
                  self->ext.model_binding.elem_index,
                  self->ext.model_binding.vendor_id,
                  self->ext.model_binding.model_id,
                  self->ext.model_binding.appkey_index);
}

// -----------------------------------------------------------------------------
//                      Model Bindings List Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_model_bindings_list_request(btmesh_conf_task_t *const self,
                                             uint16_t enc_netkey_index,
                                             uint16_t server_address,
                                             uint32_t *handle);

static sl_status_t
btmesh_conf_task_model_bindings_list_on_event(btmesh_conf_task_t *const self,
                                              uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_model_bindings_list_to_string(btmesh_conf_task_t *const self,
                                                              char *buffer,
                                                              uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_model_bindings_list_create(uint8_t elem_index,
                                                                uint16_t vendor_id,
                                                                uint16_t model_id)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_MODEL_BINDINGS_LIST);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_model_bindings_list_request;
    self->on_event = btmesh_conf_task_model_bindings_list_on_event;
    self->destroy = btmesh_conf_task_model_bindings_list_destroy;
    self->to_string = btmesh_conf_task_model_bindings_list_to_string;
    self->ext.model_bindings_list.elem_index = elem_index;
    self->ext.model_bindings_list.vendor_id = vendor_id;
    self->ext.model_bindings_list.model_id = model_id;
    self->ext.model_bindings_list.appkey_indexes = NULL;
    self->ext.model_bindings_list.appkey_count = 0;
  }
  return self;
}

void btmesh_conf_task_model_bindings_list_destroy(btmesh_conf_task_t *self)
{
  free(self->ext.model_bindings_list.appkey_indexes);
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_model_bindings_list_request(btmesh_conf_task_t *const self,
                                             uint16_t enc_netkey_index,
                                             uint16_t server_address,
                                             uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint8_t elem_index = self->ext.model_bindings_list.elem_index;
  uint16_t vendor_id = self->ext.model_bindings_list.vendor_id;
  uint16_t model_id = self->ext.model_bindings_list.model_id;

  // Make sure if this is a retry then dynamic memory allocated to store appkey
  // data is deallocated in order to restore the initial state of the task.
  if (0 < self->ext.model_bindings_list.appkey_count) {
    free(self->ext.model_bindings_list.appkey_indexes);
    self->ext.model_bindings_list.appkey_indexes = NULL;
    self->ext.model_bindings_list.appkey_count = 0;
  }

  api_status = sl_btmesh_config_client_list_bindings(enc_netkey_index,
                                                     server_address,
                                                     elem_index,
                                                     vendor_id,
                                                     model_id,
                                                     handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_model_bindings_list_on_event(btmesh_conf_task_t *const self,
                                              uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_bindings_list_id: {
      const sl_btmesh_evt_config_client_bindings_list_t *bindings_list =
        &evt->data.evt_config_client_bindings_list;
      // This event might be generated more than once so it is possible that
      // some appkeys has already been processed.
      sl_status_t collect_status =
        collect_uint16_le_data_from_array(&self->ext.model_bindings_list.appkey_indexes,
                                          &self->ext.model_bindings_list.appkey_count,
                                          BTMESH_CONF_MAX_APPKEY_COUNT,
                                          bindings_list->appkey_indices.data,
                                          bindings_list->appkey_indices.len);

      if (SL_STATUS_OK != collect_status) {
        app_log_status_error_f(collect_status,
                               "Node (netkey_idx=%d,addr=0x%04x) failed to "
                               "collect appkeys from bindings list event." NL,
                               enc_netkey_index,
                               server_address);
        event_status = SL_STATUS_ABORT;
      }
      break;
    }

    case sl_btmesh_evt_config_client_bindings_list_end_id:
    {
      const sl_btmesh_evt_config_client_bindings_list_end_t *bindings_list_end =
        &evt->data.evt_config_client_bindings_list_end;
      sl_status_t event_result = bindings_list_end->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_model_bindings_list_to_string(btmesh_conf_task_t *const self,
                                                              char *buffer,
                                                              uint32_t size)
{
  int32_t required_space;
  int32_t total_required_space = 0;
  uint32_t remaining_size = size;
  if (SL_STATUS_OK != self->result) {
    // Response with the bindings list is not received from the remote node yet
    return snprintf(buffer,
                    size,
                    "model bindings list (elem=%u,vendor=0x%04x,model=0x%04x)",
                    self->ext.model_bindings_list.elem_index,
                    self->ext.model_bindings_list.vendor_id,
                    self->ext.model_bindings_list.model_id);
  }

  required_space = snprintf(buffer,
                            size,
                            "model bindings list (elem=%u,vendor=0x%04x,"
                            "model=0x%04x,appkey_cnt=%d%s",
                            self->ext.model_bindings_list.elem_index,
                            self->ext.model_bindings_list.vendor_id,
                            self->ext.model_bindings_list.model_id,
                            self->ext.model_bindings_list.appkey_count,
                            (0 == self->ext.model_bindings_list.appkey_count)
                            ? ")" : ",appkey_idxs=");
  if (required_space < 0) {
    return required_space;
  } else if (remaining_size <= required_space) {
    remaining_size = 0;
  } else {
    remaining_size -= required_space;
  }
  total_required_space += required_space;

  uint16_t appkey_count = self->ext.model_bindings_list.appkey_count;
  for (uint16_t idx = 0; idx < appkey_count; idx++) {
    required_space = snprintf(&buffer[total_required_space],
                              remaining_size,
                              (idx != (appkey_count - 1)) ? "%d," : "%d)",
                              self->ext.model_bindings_list.appkey_indexes[idx]);
    // The loop is not terminated if the buffer space runs out in order to
    // calculate the total required buffer size.
    // Note: return value of to_string functions shall be interpreted as snprintf
    if (required_space < 0) {
      return required_space;
    } else if (remaining_size <= required_space) {
      remaining_size = 0;
    } else {
      remaining_size -= required_space;
    }
    total_required_space += required_space;
  }
  return total_required_space;
}

// -----------------------------------------------------------------------------
//                     Model Publication Set Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_model_pub_set_request(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       uint32_t *handle);

static sl_status_t
btmesh_conf_task_model_pub_set_on_event(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_model_pub_set_to_string(btmesh_conf_task_t *const self,
                                                        char *buffer,
                                                        uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_model_pub_set_create(uint8_t elem_index,
                                                          uint16_t vendor_id,
                                                          uint16_t model_id,
                                                          uint16_t address,
                                                          uint16_t appkey_index,
                                                          uint8_t credentials,
                                                          uint8_t ttl,
                                                          uint32_t period_ms,
                                                          uint8_t retransmit_count,
                                                          uint16_t retransmit_interval_ms)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_MODEL_PUB_SET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_model_pub_set_request;
    self->on_event = btmesh_conf_task_model_pub_set_on_event;
    self->destroy = btmesh_conf_task_model_pub_set_destroy;
    self->to_string = btmesh_conf_task_model_pub_set_to_string;
    self->ext.model_pub.elem_index = elem_index;
    self->ext.model_pub.vendor_id = vendor_id;
    self->ext.model_pub.model_id = model_id;
    self->ext.model_pub.address = address;
    self->ext.model_pub.appkey_index = appkey_index;
    self->ext.model_pub.credentials = credentials;
    self->ext.model_pub.ttl = ttl;
    self->ext.model_pub.period_ms = period_ms;
    self->ext.model_pub.retransmit_count = retransmit_count;
    self->ext.model_pub.retransmit_interval_ms = retransmit_interval_ms;
  }
  return self;
}

void btmesh_conf_task_model_pub_set_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_model_pub_set_request(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       uint32_t *handle)
{
  sl_status_t request_status, api_status;
  btmesh_conf_task_model_pub_t *model_pub = &self->ext.model_pub;
  api_status = sl_btmesh_config_client_set_model_pub(enc_netkey_index,
                                                     server_address,
                                                     model_pub->elem_index,
                                                     model_pub->vendor_id,
                                                     model_pub->model_id,
                                                     model_pub->address,
                                                     model_pub->appkey_index,
                                                     model_pub->credentials,
                                                     model_pub->ttl,
                                                     model_pub->period_ms,
                                                     model_pub->retransmit_count,
                                                     model_pub->retransmit_interval_ms,
                                                     handle);
  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_model_pub_set_on_event(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_model_pub_status_id:
    {
      const sl_btmesh_evt_config_client_model_pub_status_t *model_pub_status =
        &evt->data.evt_config_client_model_pub_status;
      sl_status_t event_result = model_pub_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_model_pub_set_to_string(btmesh_conf_task_t *const self,
                                                        char *buffer,
                                                        uint32_t size)
{
  btmesh_conf_task_model_pub_t *model_pub = &self->ext.model_pub;
  return snprintf(buffer,
                  size,
                  "model pub set (elem=%u,vendor=0x%04x,model=0x%04x,addr=0x%04x,"
                  "appkey_idx=%u,cred=%u,ttl=%u,period_ms=%lums,tx_cnt=%u,tx_interval_ms=%u)",
                  model_pub->elem_index,
                  model_pub->vendor_id,
                  model_pub->model_id,
                  model_pub->address,
                  model_pub->appkey_index,
                  model_pub->credentials,
                  model_pub->ttl,
                  (unsigned long) model_pub->period_ms,
                  model_pub->retransmit_count,
                  model_pub->retransmit_interval_ms);
}
// -----------------------------------------------------------------------------
//                     Model Publication Get Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_model_pub_get_request(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       uint32_t *handle);

static sl_status_t
btmesh_conf_task_model_pub_get_on_event(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_model_pub_get_to_string(btmesh_conf_task_t *const self,
                                                        char *buffer,
                                                        uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_model_pub_get_create(uint8_t elem_index,
                                                          uint16_t vendor_id,
                                                          uint16_t model_id)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_MODEL_PUB_GET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_model_pub_get_request;
    self->on_event = btmesh_conf_task_model_pub_get_on_event;
    self->destroy = btmesh_conf_task_model_pub_get_destroy;
    self->to_string = btmesh_conf_task_model_pub_get_to_string;
    self->ext.model_pub.elem_index = elem_index;
    self->ext.model_pub.vendor_id = vendor_id;
    self->ext.model_pub.model_id = model_id;
    self->ext.model_pub.address = BTMESH_CONF_PUB_ADDRESS_UNKNOWN;
    self->ext.model_pub.appkey_index = BTMESH_CONF_PUB_APPKEY_INDEX_UNKNOWN;
    self->ext.model_pub.credentials = BTMESH_CONF_PUB_CREDENTIALS_UNKNOWN;
    self->ext.model_pub.ttl = BTMESH_CONF_PUB_TTL_UNKNOWN;
    self->ext.model_pub.period_ms = BTMESH_CONF_PUB_PERIOD_MS_UNKNOWN;
    self->ext.model_pub.retransmit_count = BTMESH_CONF_PUB_RETRANSMIT_CNT_UNKNOWN;
    self->ext.model_pub.retransmit_interval_ms =
      BTMESH_CONF_PUB_RETRANSMIT_INT_MS_UNKNOWN;
  }
  return self;
}

void btmesh_conf_task_model_pub_get_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_model_pub_get_request(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       uint32_t *handle)
{
  sl_status_t request_status, api_status;
  btmesh_conf_task_model_pub_t *model_pub = &self->ext.model_pub;
  api_status = sl_btmesh_config_client_get_model_pub(enc_netkey_index,
                                                     server_address,
                                                     model_pub->elem_index,
                                                     model_pub->vendor_id,
                                                     model_pub->model_id,
                                                     handle);
  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_model_pub_get_on_event(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_model_pub_status_id:
    {
      const sl_btmesh_evt_config_client_model_pub_status_t *model_pub_status =
        &evt->data.evt_config_client_model_pub_status;
      sl_status_t event_result = model_pub_status->result;
      event_status = process_event_result_from_api(self, event_result);
      if (SL_STATUS_OK == event_result) {
        self->ext.model_pub.address = model_pub_status->address;
        self->ext.model_pub.appkey_index = model_pub_status->appkey_index;
        self->ext.model_pub.credentials = model_pub_status->credentials;
        self->ext.model_pub.ttl = model_pub_status->ttl;
        self->ext.model_pub.period_ms = model_pub_status->period_ms;
        self->ext.model_pub.retransmit_count =
          model_pub_status->retransmit_count;
        self->ext.model_pub.retransmit_interval_ms =
          model_pub_status->retransmit_interval_ms;
      }
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_model_pub_get_to_string(btmesh_conf_task_t *const self,
                                                        char *buffer,
                                                        uint32_t size)
{
  btmesh_conf_task_model_pub_t *model_pub = &self->ext.model_pub;
  if (SL_STATUS_OK == self->result) {
    return snprintf(buffer,
                    size,
                    "model pub get (elem=%u,vendor=0x%04x,model=0x%04x,addr=0x%04x,"
                    "appkey_idx=%u,cred=%u,ttl=%u,period_ms=%lums,tx_cnt=%u,tx_interval_ms=%u)",
                    model_pub->elem_index,
                    model_pub->vendor_id,
                    model_pub->model_id,
                    model_pub->address,
                    model_pub->appkey_index,
                    model_pub->credentials,
                    model_pub->ttl,
                    (unsigned long) model_pub->period_ms,
                    model_pub->retransmit_count,
                    model_pub->retransmit_interval_ms);
  } else {
    return snprintf(buffer,
                    size,
                    "model pub get (elem=%u,vendor=0x%04x,model=0x%04x)",
                    model_pub->elem_index,
                    model_pub->vendor_id,
                    model_pub->model_id);
  }
}

// -----------------------------------------------------------------------------
//                     Model Subscription Add Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_model_sub_add_request(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       uint32_t *handle);

static sl_status_t
btmesh_conf_task_model_sub_add_on_event(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_model_sub_add_to_string(btmesh_conf_task_t *const self,
                                                        char *buffer,
                                                        uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_model_sub_add_create(uint8_t elem_index,
                                                          uint16_t vendor_id,
                                                          uint16_t model_id,
                                                          uint16_t sub_address)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_MODEL_SUB_ADD);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_model_sub_add_request;
    self->on_event = btmesh_conf_task_model_sub_add_on_event;
    self->destroy = btmesh_conf_task_model_sub_add_destroy;
    self->to_string = btmesh_conf_task_model_sub_add_to_string;
    self->ext.model_sub.elem_index = elem_index;
    self->ext.model_sub.vendor_id = vendor_id;
    self->ext.model_sub.model_id = model_id;
    self->ext.model_sub.sub_address = sub_address;
  }
  return self;
}

void btmesh_conf_task_model_sub_add_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_model_sub_add_request(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       uint32_t *handle)
{
  sl_status_t request_status, api_status;
  btmesh_conf_task_model_sub_t *model_sub = &self->ext.model_sub;
  api_status = sl_btmesh_config_client_add_model_sub(enc_netkey_index,
                                                     server_address,
                                                     model_sub->elem_index,
                                                     model_sub->vendor_id,
                                                     model_sub->model_id,
                                                     model_sub->sub_address,
                                                     handle);
  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_model_sub_add_on_event(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_model_sub_status_id:
    {
      const sl_btmesh_evt_config_client_model_sub_status_t *model_sub_status =
        &evt->data.evt_config_client_model_sub_status;
      sl_status_t event_result = model_sub_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_model_sub_add_to_string(btmesh_conf_task_t *const self,
                                                        char *buffer,
                                                        uint32_t size)
{
  btmesh_conf_task_model_sub_t *model_sub = &self->ext.model_sub;
  return snprintf(buffer,
                  size,
                  "model sub add (elem=%u,vendor=0x%04x,model=0x%04x,sub_addr=0x%04x)",
                  model_sub->elem_index,
                  model_sub->vendor_id,
                  model_sub->model_id,
                  model_sub->sub_address);
}

// -----------------------------------------------------------------------------
//                   Model Subscription Remove Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_model_sub_remove_request(btmesh_conf_task_t *const self,
                                          uint16_t enc_netkey_index,
                                          uint16_t server_address,
                                          uint32_t *handle);

static sl_status_t
btmesh_conf_task_model_sub_remove_on_event(btmesh_conf_task_t *const self,
                                           uint16_t enc_netkey_index,
                                           uint16_t server_address,
                                           const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_model_sub_remove_to_string(btmesh_conf_task_t *const self,
                                                           char *buffer,
                                                           uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_model_sub_remove_create(uint8_t elem_index,
                                                             uint16_t vendor_id,
                                                             uint16_t model_id,
                                                             uint16_t sub_address)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_MODEL_SUB_REMOVE);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_model_sub_remove_request;
    self->on_event = btmesh_conf_task_model_sub_remove_on_event;
    self->destroy = btmesh_conf_task_model_sub_remove_destroy;
    self->to_string = btmesh_conf_task_model_sub_remove_to_string;
    self->ext.model_sub.elem_index = elem_index;
    self->ext.model_sub.vendor_id = vendor_id;
    self->ext.model_sub.model_id = model_id;
    self->ext.model_sub.sub_address = sub_address;
  }
  return self;
}

void btmesh_conf_task_model_sub_remove_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_model_sub_remove_request(btmesh_conf_task_t *const self,
                                          uint16_t enc_netkey_index,
                                          uint16_t server_address,
                                          uint32_t *handle)
{
  sl_status_t request_status, api_status;
  btmesh_conf_task_model_sub_t *model_sub = &self->ext.model_sub;
  api_status = sl_btmesh_config_client_remove_model_sub(enc_netkey_index,
                                                        server_address,
                                                        model_sub->elem_index,
                                                        model_sub->vendor_id,
                                                        model_sub->model_id,
                                                        model_sub->sub_address,
                                                        handle);
  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_model_sub_remove_on_event(btmesh_conf_task_t *const self,
                                           uint16_t enc_netkey_index,
                                           uint16_t server_address,
                                           const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_model_sub_status_id:
    {
      const sl_btmesh_evt_config_client_model_sub_status_t *model_sub_status =
        &evt->data.evt_config_client_model_sub_status;
      sl_status_t event_result = model_sub_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_model_sub_remove_to_string(btmesh_conf_task_t *const self,
                                                           char *buffer,
                                                           uint32_t size)
{
  btmesh_conf_task_model_sub_t *model_sub = &self->ext.model_sub;
  return snprintf(buffer,
                  size,
                  "model sub remove (elem=%u,vendor=0x%04x,model=0x%04x,sub_addr=0x%04x)",
                  model_sub->elem_index,
                  model_sub->vendor_id,
                  model_sub->model_id,
                  model_sub->sub_address);
}

// -----------------------------------------------------------------------------
//                     Model Subscription Set Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_model_sub_set_request(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       uint32_t *handle);

static sl_status_t
btmesh_conf_task_model_sub_set_on_event(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_model_sub_set_to_string(btmesh_conf_task_t *const self,
                                                        char *buffer,
                                                        uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_model_sub_set_create(uint8_t elem_index,
                                                          uint16_t vendor_id,
                                                          uint16_t model_id,
                                                          uint16_t sub_address)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_MODEL_SUB_SET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_model_sub_set_request;
    self->on_event = btmesh_conf_task_model_sub_set_on_event;
    self->destroy = btmesh_conf_task_model_sub_set_destroy;
    self->to_string = btmesh_conf_task_model_sub_set_to_string;
    self->ext.model_sub.elem_index = elem_index;
    self->ext.model_sub.vendor_id = vendor_id;
    self->ext.model_sub.model_id = model_id;
    self->ext.model_sub.sub_address = sub_address;
  }
  return self;
}

void btmesh_conf_task_model_sub_set_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_model_sub_set_request(btmesh_conf_task_t *const self,
                                       uint16_t enc_netkey_index,
                                       uint16_t server_address,
                                       uint32_t *handle)
{
  sl_status_t request_status, api_status;
  btmesh_conf_task_model_sub_t *model_sub = &self->ext.model_sub;
  api_status = sl_btmesh_config_client_set_model_sub(enc_netkey_index,
                                                     server_address,
                                                     model_sub->elem_index,
                                                     model_sub->vendor_id,
                                                     model_sub->model_id,
                                                     model_sub->sub_address,
                                                     handle);
  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_model_sub_set_on_event(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_model_sub_status_id:
    {
      const sl_btmesh_evt_config_client_model_sub_status_t *model_sub_status =
        &evt->data.evt_config_client_model_sub_status;
      sl_status_t event_result = model_sub_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_model_sub_set_to_string(btmesh_conf_task_t *const self,
                                                        char *buffer,
                                                        uint32_t size)
{
  btmesh_conf_task_model_sub_t *model_sub = &self->ext.model_sub;
  return snprintf(buffer,
                  size,
                  "model sub set (elem=%u,vendor=0x%04x,model=0x%04x,sub_addr=0x%04x)",
                  model_sub->elem_index,
                  model_sub->vendor_id,
                  model_sub->model_id,
                  model_sub->sub_address);
}

// -----------------------------------------------------------------------------
//                   Model Subscription Clear Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_model_sub_clear_request(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         uint32_t *handle);

static sl_status_t
btmesh_conf_task_model_sub_clear_on_event(btmesh_conf_task_t *const self,
                                          uint16_t enc_netkey_index,
                                          uint16_t server_address,
                                          const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_model_sub_clear_to_string(btmesh_conf_task_t *const self,
                                                          char *buffer,
                                                          uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_model_sub_clear_create(uint8_t elem_index,
                                                            uint16_t vendor_id,
                                                            uint16_t model_id)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_MODEL_SUB_CLEAR);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_model_sub_clear_request;
    self->on_event = btmesh_conf_task_model_sub_clear_on_event;
    self->destroy = btmesh_conf_task_model_sub_clear_destroy;
    self->to_string = btmesh_conf_task_model_sub_clear_to_string;
    self->ext.model_sub_clear.elem_index = elem_index;
    self->ext.model_sub_clear.vendor_id = vendor_id;
    self->ext.model_sub_clear.model_id = model_id;
  }
  return self;
}

void btmesh_conf_task_model_sub_clear_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_model_sub_clear_request(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         uint32_t *handle)
{
  sl_status_t request_status, api_status;
  btmesh_conf_task_model_sub_clear_t *model_sub_clear = &self->ext.model_sub_clear;
  api_status = sl_btmesh_config_client_clear_model_sub(enc_netkey_index,
                                                       server_address,
                                                       model_sub_clear->elem_index,
                                                       model_sub_clear->vendor_id,
                                                       model_sub_clear->model_id,
                                                       handle);
  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_model_sub_clear_on_event(btmesh_conf_task_t *const self,
                                          uint16_t enc_netkey_index,
                                          uint16_t server_address,
                                          const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_model_sub_status_id:
    {
      const sl_btmesh_evt_config_client_model_sub_status_t *model_sub_status =
        &evt->data.evt_config_client_model_sub_status;
      sl_status_t event_result = model_sub_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_model_sub_clear_to_string(btmesh_conf_task_t *const self,
                                                          char *buffer,
                                                          uint32_t size)
{
  btmesh_conf_task_model_sub_clear_t *model_sub_clear = &self->ext.model_sub_clear;
  return snprintf(buffer,
                  size,
                  "model sub clear (elem=%u,vendor=0x%04x,model=0x%04x)",
                  model_sub_clear->elem_index,
                  model_sub_clear->vendor_id,
                  model_sub_clear->model_id);
}

// -----------------------------------------------------------------------------
//                    Model Subscription List Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_model_sub_list_request(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        uint32_t *handle);

static sl_status_t
btmesh_conf_task_model_sub_list_on_event(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_model_sub_list_to_string(btmesh_conf_task_t *const self,
                                                         char *buffer,
                                                         uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_model_sub_list_create(uint8_t elem_index,
                                                           uint16_t vendor_id,
                                                           uint16_t model_id)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_MODEL_SUB_LIST);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_model_sub_list_request;
    self->on_event = btmesh_conf_task_model_sub_list_on_event;
    self->destroy = btmesh_conf_task_model_sub_list_destroy;
    self->to_string = btmesh_conf_task_model_sub_list_to_string;
    self->ext.model_sub_list.elem_index = elem_index;
    self->ext.model_sub_list.vendor_id = vendor_id;
    self->ext.model_sub_list.model_id = model_id;
    self->ext.model_sub_list.addresses = NULL;
    self->ext.model_sub_list.address_count = 0;
  }
  return self;
}

void btmesh_conf_task_model_sub_list_destroy(btmesh_conf_task_t *self)
{
  free(self->ext.model_sub_list.addresses);
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_model_sub_list_request(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint8_t elem_index = self->ext.model_sub_list.elem_index;
  uint16_t vendor_id = self->ext.model_sub_list.vendor_id;
  uint16_t model_id = self->ext.model_sub_list.model_id;

  // Make sure if this is a retry then dynamic memory allocated to store address
  // data is deallocated in order to restore the initial state of the task.
  if (0 < self->ext.model_sub_list.address_count) {
    free(self->ext.model_sub_list.addresses);
    self->ext.model_sub_list.addresses = NULL;
    self->ext.model_sub_list.address_count = 0;
  }

  api_status = sl_btmesh_config_client_list_subs(enc_netkey_index,
                                                 server_address,
                                                 elem_index,
                                                 vendor_id,
                                                 model_id,
                                                 handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_model_sub_list_on_event(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_subs_list_id: {
      const sl_btmesh_evt_config_client_subs_list_t *subs_list =
        &evt->data.evt_config_client_subs_list;
      // This event might be generated more than once so it is possible that
      // some addresses has already been processed.
      sl_status_t collect_status =
        collect_uint16_le_data_from_array(&self->ext.model_sub_list.addresses,
                                          &self->ext.model_sub_list.address_count,
                                          BTMESH_CONF_MAX_SUB_ADDR_COUNT,
                                          subs_list->addresses.data,
                                          subs_list->addresses.len);

      if (SL_STATUS_OK != collect_status) {
        app_log_status_error_f(collect_status,
                               "Node (netkey_idx=%d,addr=0x%04x) failed to "
                               "collect addresses from subs list event." NL,
                               enc_netkey_index,
                               server_address);
        event_status = SL_STATUS_ABORT;
      }
      break;
    }

    case sl_btmesh_evt_config_client_subs_list_end_id:
    {
      const sl_btmesh_evt_config_client_subs_list_end_t *subs_list_end =
        &evt->data.evt_config_client_subs_list_end;
      sl_status_t event_result = subs_list_end->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_model_sub_list_to_string(btmesh_conf_task_t *const self,
                                                         char *buffer,
                                                         uint32_t size)
{
  int32_t required_space;
  int32_t total_required_space = 0;
  uint32_t remaining_size = size;
  if (SL_STATUS_OK != self->result) {
    // Response with the subs list is not received from the remote node yet
    return snprintf(buffer,
                    size,
                    "model subs list (elem=%u,vendor=0x%04x,model=0x%04x)",
                    self->ext.model_sub_list.elem_index,
                    self->ext.model_sub_list.vendor_id,
                    self->ext.model_sub_list.model_id);
  }

  required_space = snprintf(buffer,
                            size,
                            "model subs list (elem=%u,vendor=0x%04x,"
                            "model=0x%04x,addr_cnt=%d%s",
                            self->ext.model_sub_list.elem_index,
                            self->ext.model_sub_list.vendor_id,
                            self->ext.model_sub_list.model_id,
                            self->ext.model_sub_list.address_count,
                            (0 == self->ext.model_sub_list.address_count)
                            ? ")" : ",addrs=");
  if (required_space < 0) {
    return required_space;
  } else if (remaining_size <= required_space) {
    remaining_size = 0;
  } else {
    remaining_size -= required_space;
  }
  total_required_space += required_space;

  uint16_t addr_count = self->ext.model_sub_list.address_count;
  for (uint16_t idx = 0; idx < addr_count; idx++) {
    required_space = snprintf(&buffer[total_required_space],
                              remaining_size,
                              (idx != (addr_count - 1)) ? "0x%04x," : "0x%04x)",
                              self->ext.model_sub_list.addresses[idx]);
    // The loop is not terminated if the buffer space runs out in order to
    // calculate the total required buffer size.
    // Note: return value of to_string functions shall be interpreted as snprintf
    if (required_space < 0) {
      return required_space;
    } else if (remaining_size <= required_space) {
      remaining_size = 0;
    } else {
      remaining_size -= required_space;
    }
    total_required_space += required_space;
  }
  return total_required_space;
}

// -----------------------------------------------------------------------------
//                        Default TTL Set Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_default_ttl_set_request(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         uint32_t *handle);

static sl_status_t
btmesh_conf_task_default_ttl_set_on_event(btmesh_conf_task_t *const self,
                                          uint16_t enc_netkey_index,
                                          uint16_t server_address,
                                          const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_default_ttl_set_to_string(btmesh_conf_task_t *const self,
                                                          char *buffer,
                                                          uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_default_ttl_set_create(uint8_t default_ttl)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_DEFAULT_TTL_SET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_default_ttl_set_request;
    self->on_event = btmesh_conf_task_default_ttl_set_on_event;
    self->destroy = btmesh_conf_task_default_ttl_set_destroy;
    self->to_string = btmesh_conf_task_default_ttl_set_to_string;
    self->ext.default_ttl.ttl = default_ttl;
  }
  return self;
}

void btmesh_conf_task_default_ttl_set_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_default_ttl_set_request(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint8_t default_ttl = self->ext.default_ttl.ttl;

  api_status = sl_btmesh_config_client_set_default_ttl(enc_netkey_index,
                                                       server_address,
                                                       default_ttl,
                                                       handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_default_ttl_set_on_event(btmesh_conf_task_t *const self,
                                          uint16_t enc_netkey_index,
                                          uint16_t server_address,
                                          const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_default_ttl_status_id:
    {
      const sl_btmesh_evt_config_client_default_ttl_status_t *default_ttl_status =
        &evt->data.evt_config_client_default_ttl_status;

      sl_status_t event_result = default_ttl_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_default_ttl_set_to_string(btmesh_conf_task_t *const self,
                                                          char *buffer,
                                                          uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "default ttl set (ttl=%d)",
                  self->ext.default_ttl.ttl);
}
// -----------------------------------------------------------------------------
//                        Default TTL Get Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_default_ttl_get_request(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         uint32_t *handle);

static sl_status_t
btmesh_conf_task_default_ttl_get_on_event(btmesh_conf_task_t *const self,
                                          uint16_t enc_netkey_index,
                                          uint16_t server_address,
                                          const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_default_ttl_get_to_string(btmesh_conf_task_t *const self,
                                                          char *buffer,
                                                          uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_default_ttl_get_create(void)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_DEFAULT_TTL_GET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_default_ttl_get_request;
    self->on_event = btmesh_conf_task_default_ttl_get_on_event;
    self->destroy = btmesh_conf_task_default_ttl_get_destroy;
    self->to_string = btmesh_conf_task_default_ttl_get_to_string;
    self->ext.default_ttl.ttl = BTMESH_CONF_DEFAULT_TTL_UNKNOWN;
  }
  return self;
}

void btmesh_conf_task_default_ttl_get_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_default_ttl_get_request(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         uint32_t *handle)
{
  sl_status_t request_status, api_status;

  api_status = sl_btmesh_config_client_get_default_ttl(enc_netkey_index,
                                                       server_address,
                                                       handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_default_ttl_get_on_event(btmesh_conf_task_t *const self,
                                          uint16_t enc_netkey_index,
                                          uint16_t server_address,
                                          const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_default_ttl_status_id:
    {
      const sl_btmesh_evt_config_client_default_ttl_status_t *default_ttl_status =
        &evt->data.evt_config_client_default_ttl_status;

      sl_status_t event_result = default_ttl_status->result;
      event_status = process_event_result_from_api(self, event_result);
      if (SL_STATUS_OK == event_result) {
        self->ext.default_ttl.ttl = default_ttl_status->value;
      }
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_default_ttl_get_to_string(btmesh_conf_task_t *const self,
                                                          char *buffer,
                                                          uint32_t size)
{
  if (SL_STATUS_OK == self->result) {
    return snprintf(buffer,
                    size,
                    "default ttl get (ttl=%d)",
                    self->ext.default_ttl.ttl);
  } else {
    return snprintf(buffer,
                    size,
                    "default ttl get");
  }
}

// -----------------------------------------------------------------------------
//                        GATT Proxy Set Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_gatt_proxy_set_request(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        uint32_t *handle);

static sl_status_t
btmesh_conf_task_gatt_proxy_set_on_event(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_gatt_proxy_set_to_string(btmesh_conf_task_t *const self,
                                                         char *buffer,
                                                         uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_gatt_proxy_set_create(uint8_t value)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_GATT_PROXY_SET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_gatt_proxy_set_request;
    self->on_event = btmesh_conf_task_gatt_proxy_set_on_event;
    self->destroy = btmesh_conf_task_gatt_proxy_set_destroy;
    self->to_string = btmesh_conf_task_gatt_proxy_set_to_string;
    self->ext.gatt_proxy.value = value;
  }
  return self;
}

void btmesh_conf_task_gatt_proxy_set_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_gatt_proxy_set_request(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint8_t value = self->ext.gatt_proxy.value;

  api_status = sl_btmesh_config_client_set_gatt_proxy(enc_netkey_index,
                                                      server_address,
                                                      value,
                                                      handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_gatt_proxy_set_on_event(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_gatt_proxy_status_id:
    {
      const sl_btmesh_evt_config_client_gatt_proxy_status_t *gatt_proxy_status =
        &evt->data.evt_config_client_gatt_proxy_status;

      sl_status_t event_result = gatt_proxy_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_gatt_proxy_set_to_string(btmesh_conf_task_t *const self,
                                                         char *buffer,
                                                         uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "gatt_proxy set (val=%d)",
                  self->ext.gatt_proxy.value);
}

// -----------------------------------------------------------------------------
//                        GATT Proxy Get Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_gatt_proxy_get_request(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        uint32_t *handle);

static sl_status_t
btmesh_conf_task_gatt_proxy_get_on_event(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_gatt_proxy_get_to_string(btmesh_conf_task_t *const self,
                                                         char *buffer,
                                                         uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_gatt_proxy_get_create(void)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_GATT_PROXY_GET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_gatt_proxy_get_request;
    self->on_event = btmesh_conf_task_gatt_proxy_get_on_event;
    self->destroy = btmesh_conf_task_gatt_proxy_get_destroy;
    self->to_string = btmesh_conf_task_gatt_proxy_get_to_string;
    self->ext.gatt_proxy.value = BTMESH_CONF_PROXY_VALUE_UNKNOWN;
  }
  return self;
}

void btmesh_conf_task_gatt_proxy_get_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_gatt_proxy_get_request(btmesh_conf_task_t *const self,
                                        uint16_t enc_netkey_index,
                                        uint16_t server_address,
                                        uint32_t *handle)
{
  sl_status_t request_status, api_status;

  api_status = sl_btmesh_config_client_get_gatt_proxy(enc_netkey_index,
                                                      server_address,
                                                      handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_gatt_proxy_get_on_event(btmesh_conf_task_t *const self,
                                         uint16_t enc_netkey_index,
                                         uint16_t server_address,
                                         const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_gatt_proxy_status_id:
    {
      const sl_btmesh_evt_config_client_gatt_proxy_status_t *gatt_proxy_status =
        &evt->data.evt_config_client_gatt_proxy_status;

      sl_status_t event_result = gatt_proxy_status->result;
      event_status = process_event_result_from_api(self, event_result);
      if (SL_STATUS_OK == event_result) {
        self->ext.gatt_proxy.value = gatt_proxy_status->value;
      }
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_gatt_proxy_get_to_string(btmesh_conf_task_t *const self,
                                                         char *buffer,
                                                         uint32_t size)
{
  if (SL_STATUS_OK == self->result) {
    return snprintf(buffer,
                    size,
                    "gatt_proxy get (val=%d)",
                    self->ext.gatt_proxy.value);
  } else {
    return snprintf(buffer,
                    size,
                    "gatt_proxy get");
  }
}

// -----------------------------------------------------------------------------
//                           Relay Set Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_relay_set_request(btmesh_conf_task_t *const self,
                                   uint16_t enc_netkey_index,
                                   uint16_t server_address,
                                   uint32_t *handle);

static sl_status_t
btmesh_conf_task_relay_set_on_event(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_relay_set_to_string(btmesh_conf_task_t *const self,
                                                    char *buffer,
                                                    uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_relay_set_create(uint8_t value,
                                                      uint8_t retransmit_count,
                                                      uint16_t retransmit_interval_ms)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_RELAY_SET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_relay_set_request;
    self->on_event = btmesh_conf_task_relay_set_on_event;
    self->destroy = btmesh_conf_task_relay_set_destroy;
    self->to_string = btmesh_conf_task_relay_set_to_string;
    self->ext.relay.value = value;
    self->ext.relay.retransmit_count = retransmit_count;
    self->ext.relay.retransmit_interval_ms = retransmit_interval_ms;
  }
  return self;
}

void btmesh_conf_task_relay_set_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_relay_set_request(btmesh_conf_task_t *const self,
                                   uint16_t enc_netkey_index,
                                   uint16_t server_address,
                                   uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint8_t value = self->ext.relay.value;
  uint8_t retransmit_count = self->ext.relay.retransmit_count;
  uint16_t retransmit_interval_ms = self->ext.relay.retransmit_interval_ms;

  api_status = sl_btmesh_config_client_set_relay(enc_netkey_index,
                                                 server_address,
                                                 value,
                                                 retransmit_count,
                                                 retransmit_interval_ms,
                                                 handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_relay_set_on_event(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_relay_status_id:
    {
      const sl_btmesh_evt_config_client_relay_status_t *relay_status =
        &evt->data.evt_config_client_relay_status;

      sl_status_t event_result = relay_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_relay_set_to_string(btmesh_conf_task_t *const self,
                                                    char *buffer,
                                                    uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "relay set (val=%d,cnt=%d,interval=%dms)",
                  self->ext.relay.value,
                  self->ext.relay.retransmit_count,
                  self->ext.relay.retransmit_interval_ms);
}

// -----------------------------------------------------------------------------
//                           Relay Get Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_relay_get_request(btmesh_conf_task_t *const self,
                                   uint16_t enc_netkey_index,
                                   uint16_t server_address,
                                   uint32_t *handle);

static sl_status_t
btmesh_conf_task_relay_get_on_event(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_relay_get_to_string(btmesh_conf_task_t *const self,
                                                    char *buffer,
                                                    uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_relay_get_create(void)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_RELAY_GET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_relay_get_request;
    self->on_event = btmesh_conf_task_relay_get_on_event;
    self->destroy = btmesh_conf_task_relay_get_destroy;
    self->to_string = btmesh_conf_task_relay_get_to_string;
    self->ext.relay.value = BTMESH_CONF_RELAY_VALUE_UNKNOWN;
    self->ext.relay.retransmit_count = BTMESH_CONF_RELAY_RETRANSMIT_CNT_UNKNOWN;
    self->ext.relay.retransmit_interval_ms =
      BTMESH_CONF_RELAY_RETRANSMIT_INT_MS_UNKNOWN;
  }
  return self;
}

void btmesh_conf_task_relay_get_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_relay_get_request(btmesh_conf_task_t *const self,
                                   uint16_t enc_netkey_index,
                                   uint16_t server_address,
                                   uint32_t *handle)
{
  sl_status_t request_status, api_status;

  api_status = sl_btmesh_config_client_get_relay(enc_netkey_index,
                                                 server_address,
                                                 handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_relay_get_on_event(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_relay_status_id:
    {
      const sl_btmesh_evt_config_client_relay_status_t *relay_status =
        &evt->data.evt_config_client_relay_status;

      sl_status_t event_result = relay_status->result;
      event_status = process_event_result_from_api(self, event_result);
      if (SL_STATUS_OK == event_result) {
        self->ext.relay.value = relay_status->relay;
        self->ext.relay.retransmit_count = relay_status->retransmit_count;
        self->ext.relay.retransmit_interval_ms =
          relay_status->retransmit_interval_ms;
      }
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_relay_get_to_string(btmesh_conf_task_t *const self,
                                                    char *buffer,
                                                    uint32_t size)
{
  if (SL_STATUS_OK == self->result) {
    return snprintf(buffer,
                    size,
                    "relay get (val=%d,cnt=%d,interval=%dms)",
                    self->ext.relay.value,
                    self->ext.relay.retransmit_count,
                    self->ext.relay.retransmit_interval_ms);
  } else {
    return snprintf(buffer,
                    size,
                    "relay get");
  }
}

// -----------------------------------------------------------------------------
//                   Network Transmit Count Set Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_network_transmit_set_request(btmesh_conf_task_t *const self,
                                              uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              uint32_t *handle);

static sl_status_t
btmesh_conf_task_network_transmit_set_on_event(btmesh_conf_task_t *const self,
                                               uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_network_transmit_set_to_string(btmesh_conf_task_t *const self,
                                                               char *buffer,
                                                               uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_network_transmit_set_create(uint8_t transmit_count,
                                                                 uint16_t transmit_interval_ms)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_NETWORK_TRANSMIT_SET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_network_transmit_set_request;
    self->on_event = btmesh_conf_task_network_transmit_set_on_event;
    self->destroy = btmesh_conf_task_network_transmit_set_destroy;
    self->to_string = btmesh_conf_task_network_transmit_set_to_string;
    self->ext.network_transmit.transmit_count = transmit_count;
    self->ext.network_transmit.transmit_interval_ms = transmit_interval_ms;
  }
  return self;
}

void btmesh_conf_task_network_transmit_set_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_network_transmit_set_request(btmesh_conf_task_t *const self,
                                              uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint8_t transmit_count = self->ext.network_transmit.transmit_count;
  uint16_t transmit_interval_ms =
    self->ext.network_transmit.transmit_interval_ms;

  api_status = sl_btmesh_config_client_set_network_transmit(enc_netkey_index,
                                                            server_address,
                                                            transmit_count,
                                                            transmit_interval_ms,
                                                            handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_network_transmit_set_on_event(btmesh_conf_task_t *const self,
                                               uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_network_transmit_status_id:
    {
      const sl_btmesh_evt_config_client_network_transmit_status_t *network_transmit_status =
        &evt->data.evt_config_client_network_transmit_status;

      sl_status_t event_result = network_transmit_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_network_transmit_set_to_string(btmesh_conf_task_t *const self,
                                                               char *buffer,
                                                               uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "network transmit set (cnt=%d,interval=%dms)",
                  self->ext.network_transmit.transmit_count,
                  self->ext.network_transmit.transmit_interval_ms);
}

// -----------------------------------------------------------------------------
//                   Network Transmit Count Get Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_network_transmit_get_request(btmesh_conf_task_t *const self,
                                              uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              uint32_t *handle);

static sl_status_t
btmesh_conf_task_network_transmit_get_on_event(btmesh_conf_task_t *const self,
                                               uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_network_transmit_get_to_string(btmesh_conf_task_t *const self,
                                                               char *buffer,
                                                               uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_network_transmit_get_create(void)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_NETWORK_TRANSMIT_GET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_network_transmit_get_request;
    self->on_event = btmesh_conf_task_network_transmit_get_on_event;
    self->destroy = btmesh_conf_task_network_transmit_get_destroy;
    self->to_string = btmesh_conf_task_network_transmit_get_to_string;
    self->ext.network_transmit.transmit_count = BTMESH_CONF_NW_TRANSMIT_CNT_UNKNOWN;
    self->ext.network_transmit.transmit_interval_ms =
      BTMESH_CONF_NW_TRANSMIT_INT_MS_UNKNOWN;
  }
  return self;
}

void btmesh_conf_task_network_transmit_get_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_network_transmit_get_request(btmesh_conf_task_t *const self,
                                              uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              uint32_t *handle)
{
  sl_status_t request_status, api_status;

  api_status = sl_btmesh_config_client_get_network_transmit(enc_netkey_index,
                                                            server_address,
                                                            handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_network_transmit_get_on_event(btmesh_conf_task_t *const self,
                                               uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_network_transmit_status_id:
    {
      const sl_btmesh_evt_config_client_network_transmit_status_t *network_transmit_status =
        &evt->data.evt_config_client_network_transmit_status;

      sl_status_t event_result = network_transmit_status->result;
      event_status = process_event_result_from_api(self, event_result);
      if (SL_STATUS_OK == event_result) {
        self->ext.network_transmit.transmit_count =
          network_transmit_status->transmit_count;
        self->ext.network_transmit.transmit_interval_ms =
          network_transmit_status->transmit_interval_ms;
      }
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_network_transmit_get_to_string(btmesh_conf_task_t *const self,
                                                               char *buffer,
                                                               uint32_t size)
{
  if (SL_STATUS_OK == self->result) {
    return snprintf(buffer,
                    size,
                    "network transmit get (cnt=%d,interval=%dms)",
                    self->ext.network_transmit.transmit_count,
                    self->ext.network_transmit.transmit_interval_ms);
  } else {
    return snprintf(buffer,
                    size,
                    "network transmit get");
  }
}

// -----------------------------------------------------------------------------
//                          Friend Set Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_friend_set_request(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    uint32_t *handle);

static sl_status_t
btmesh_conf_task_friend_set_on_event(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_friend_set_to_string(btmesh_conf_task_t *const self,
                                                     char *buffer,
                                                     uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_friend_set_create(uint8_t value)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_FRIEND_SET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_friend_set_request;
    self->on_event = btmesh_conf_task_friend_set_on_event;
    self->destroy = btmesh_conf_task_friend_set_destroy;
    self->to_string = btmesh_conf_task_friend_set_to_string;
    self->ext.friend.value = value;
  }
  return self;
}

void btmesh_conf_task_friend_set_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_friend_set_request(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    uint32_t *handle)
{
  sl_status_t request_status, api_status;
  uint8_t value = self->ext.friend.value;

  api_status = sl_btmesh_config_client_set_friend(enc_netkey_index,
                                                  server_address,
                                                  value,
                                                  handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_friend_set_on_event(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_friend_status_id:
    {
      const sl_btmesh_evt_config_client_friend_status_t *friend_status =
        &evt->data.evt_config_client_friend_status;

      sl_status_t event_result = friend_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_friend_set_to_string(btmesh_conf_task_t *const self,
                                                     char *buffer,
                                                     uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "friend set (val=%d)",
                  self->ext.friend.value);
}

// -----------------------------------------------------------------------------
//                          Friend Get Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_friend_get_request(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    uint32_t *handle);

static sl_status_t
btmesh_conf_task_friend_get_on_event(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_friend_get_to_string(btmesh_conf_task_t *const self,
                                                     char *buffer,
                                                     uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_friend_get_create(void)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_FRIEND_GET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_friend_get_request;
    self->on_event = btmesh_conf_task_friend_get_on_event;
    self->destroy = btmesh_conf_task_friend_get_destroy;
    self->to_string = btmesh_conf_task_friend_get_to_string;
    self->ext.friend.value = BTMESH_CONF_FRIEND_VALUE_UNKNOWN;
  }
  return self;
}

void btmesh_conf_task_friend_get_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_friend_get_request(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    uint32_t *handle)
{
  sl_status_t request_status, api_status;

  api_status = sl_btmesh_config_client_get_friend(enc_netkey_index,
                                                  server_address,
                                                  handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_friend_get_on_event(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_friend_status_id:
    {
      const sl_btmesh_evt_config_client_friend_status_t *friend_status =
        &evt->data.evt_config_client_friend_status;

      sl_status_t event_result = friend_status->result;
      event_status = process_event_result_from_api(self, event_result);
      if (SL_STATUS_OK == event_result) {
        self->ext.friend.value = friend_status->value;
      }
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_friend_get_to_string(btmesh_conf_task_t *const self,
                                                     char *buffer,
                                                     uint32_t size)
{
  if (SL_STATUS_OK == self->result) {
    return snprintf(buffer,
                    size,
                    "friend get (val=%d)",
                    self->ext.friend.value);
  } else {
    return snprintf(buffer,
                    size,
                    "friend get");
  }
}

// -----------------------------------------------------------------------------
//                            DCD Get Task Functions
// -----------------------------------------------------------------------------

#define DCD_FEATURE_RELAY_BYTE       0x00
#define DCD_FEATURE_PROXY_BYTE       0x00
#define DCD_FEATURE_FRIEND_BYTE      0x00
#define DCD_FEATURE_LOW_POWER_BYTE   0x00

#define DCD_FEATURE_RELAY_MASK       0x01
#define DCD_FEATURE_PROXY_MASK       0x02
#define DCD_FEATURE_FRIEND_MASK      0x04
#define DCD_FEATURE_LOW_POWER_MASK   0x08

#define DCD_RAW_VENDOR_ID_SIZE 2

typedef struct btmesh_conf_dcd_raw_element_t {
  uint8_t location[2];
  uint8_t num_sig_model_ids;
  uint8_t num_vendor_model_ids;
  uint8_t models[];
} btmesh_conf_dcd_raw_element_t;

typedef struct btmesh_conf_dcd_raw_header_t {
  uint8_t cid[2];
  uint8_t pid[2];
  uint8_t vid[2];
  uint8_t crpl[2];
  uint8_t features[2];
  uint8_t elements[];
} btmesh_conf_dcd_raw_header_t;

typedef enum btmesh_conf_task_dcd_iter_cmd_t {
  BTMESH_CONF_TASK_DCD_ITER_CMD_CHECK_SIZE,
  BTMESH_CONF_TASK_DCD_ITER_CMD_UPDATE_DB
} btmesh_conf_task_dcd_iter_cmd_t;

static sl_status_t btmesh_conf_task_dcd_get_request(btmesh_conf_task_t *const self,
                                                    uint16_t enc_netkey_index,
                                                    uint16_t server_address,
                                                    uint32_t *handle);

static sl_status_t btmesh_conf_task_dcd_get_on_event(btmesh_conf_task_t *const self,
                                                     uint16_t enc_netkey_index,
                                                     uint16_t server_address,
                                                     const sl_btmesh_msg_t *evt);

static sl_status_t btmesh_conf_task_dcd_process(btmesh_conf_task_t *const self,
                                                uint16_t enc_netkey_index,
                                                uint16_t server_address);

static sl_status_t btmesh_conf_task_dcd_element_iterate(uint16_t enc_netkey_index,
                                                        uint16_t server_address,
                                                        const uint8_t *raw_dcd_data,
                                                        uint16_t raw_dcd_data_size,
                                                        btmesh_conf_task_dcd_iter_cmd_t command);

static int32_t btmesh_conf_task_dcd_get_to_string(btmesh_conf_task_t *const self,
                                                  char *buffer,
                                                  uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_dcd_get_create(uint8_t page)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_DCD_GET);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_dcd_get_request;
    self->on_event = btmesh_conf_task_dcd_get_on_event;
    self->to_string = btmesh_conf_task_dcd_get_to_string;
    self->destroy = btmesh_conf_task_dcd_get_destroy;
    self->ext.dcd.page = page;
    self->ext.dcd.raw_dcd_data = NULL;
    self->ext.dcd.raw_dcd_data_size = 0;
  }
  return self;
}

void btmesh_conf_task_dcd_get_destroy(btmesh_conf_task_t *self)
{
  free(self->ext.dcd.raw_dcd_data);
  btmesh_conf_task_destroy(self);
}

static sl_status_t btmesh_conf_task_dcd_get_request(btmesh_conf_task_t *const self,
                                                    uint16_t enc_netkey_index,
                                                    uint16_t server_address,
                                                    uint32_t *handle)
{
  sl_status_t request_status, api_status;

  app_log_debug("Config task get dcd request "
                "(nwkey=%d,addr=0x%04x,page=%d)" NL,
                enc_netkey_index,
                server_address,
                self->ext.dcd.page);
  // Make sure if this is a retry then dynamic memory allocated to store DCD data
  // is deallocated in order to restore the initial state of the task.
  if (0 != self->ext.dcd.raw_dcd_data_size) {
    free(self->ext.dcd.raw_dcd_data);
    self->ext.dcd.raw_dcd_data = NULL;
    self->ext.dcd.raw_dcd_data_size = 0;
  }

  api_status = sl_btmesh_config_client_get_dcd(enc_netkey_index,
                                               server_address,
                                               self->ext.dcd.page,
                                               handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t btmesh_conf_task_dcd_get_on_event(btmesh_conf_task_t *const self,
                                                     uint16_t enc_netkey_index,
                                                     uint16_t server_address,
                                                     const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_dcd_data_id:
    {
      const sl_btmesh_evt_config_client_dcd_data_t *evt_dcd_data =
        &evt->data.evt_config_client_dcd_data;

      const uint8_t *dcd_chunk_data = evt_dcd_data->data.data;
      uint16_t dcd_chunk_size = evt_dcd_data->data.len;
      uint16_t current_raw_dcd_data_size = self->ext.dcd.raw_dcd_data_size;
      uint16_t new_raw_dcd_data_size = current_raw_dcd_data_size + dcd_chunk_size;
      // If the pointer parameter of realloc is NULL then it behaves like malloc
      uint8_t *reallocated_dcd_data_ptr = realloc(self->ext.dcd.raw_dcd_data,
                                                  new_raw_dcd_data_size);

      if (NULL == reallocated_dcd_data_ptr) {
        // The get dcd task shall be cancelled in the stack due to memory issues
        // on the host machine. The allocated memory will be released when the
        // task object is destroyed.
        event_status = SL_STATUS_ABORT;
        self->result = SL_STATUS_ALLOCATION_FAILED;
      } else {
        self->ext.dcd.raw_dcd_data = reallocated_dcd_data_ptr;
        memcpy(&self->ext.dcd.raw_dcd_data[current_raw_dcd_data_size],
               dcd_chunk_data,
               dcd_chunk_size);
        self->ext.dcd.raw_dcd_data_size = new_raw_dcd_data_size;
      }
      break;
    }

    case sl_btmesh_evt_config_client_dcd_data_end_id:
    {
      const sl_btmesh_evt_config_client_dcd_data_end_t *dcd_data_end_evt =
        &evt->data.evt_config_client_dcd_data_end;

      sl_status_t event_result = dcd_data_end_evt->result;
      event_status = process_event_result_from_api(self, event_result);

      if (SL_STATUS_OK == event_result) {
        sl_status_t process_dcd_result;
        process_dcd_result = btmesh_conf_task_dcd_process(self,
                                                          enc_netkey_index,
                                                          server_address);
        self->result = process_dcd_result;
        event_status = (SL_STATUS_OK == process_dcd_result)
                       ? SL_STATUS_OK : SL_STATUS_FAIL;
      }
      break;
    }

    default:
      break;
  }

  return event_status;
}

static sl_status_t btmesh_conf_task_dcd_process(btmesh_conf_task_t *const self,
                                                uint16_t enc_netkey_index,
                                                uint16_t server_address)
{
  sl_status_t process_dcd_status = SL_STATUS_OK;
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(server_address);
  if (NULL == node) {
    process_dcd_status = SL_STATUS_NOT_FOUND;
  } else if (false != node->dcd_available) {
    process_dcd_status = SL_STATUS_INVALID_STATE;
  } else {
    char node_str[SL_BTMESH_CONF_MAX_LOG_MSG_FRAGMENT_SIZE_CFG_VAL];
    uint16_t remaining_size = self->ext.dcd.raw_dcd_data_size;
    btmesh_conf_dcd_raw_header_t *dcd_raw_hdr =
      (btmesh_conf_dcd_raw_header_t*) self->ext.dcd.raw_dcd_data;

    if (remaining_size < sizeof(btmesh_conf_dcd_raw_header_t)) {
      process_dcd_status = SL_STATUS_INVALID_COUNT;
    } else {
      if (btmesh_conf_any_severe_log_level(APP_LOG_LEVEL_INFO)) {
        int32_t str_retval;
        str_retval = snprintf(node_str,
                              sizeof(node_str),
                              "Node (netkey_idx=%d,addr=0x%04x)",
                              enc_netkey_index,
                              server_address);
        app_assert(0 < str_retval, "String formatting failed." NL);
      }
      node_str[sizeof(node_str) - 1] = '\0';

      uint16_t company_id = uint16_from_buf_le(&dcd_raw_hdr->cid[0]);
      uint16_t product_id = uint16_from_buf_le(&dcd_raw_hdr->pid[0]);
      uint16_t version_id = uint16_from_buf_le(&dcd_raw_hdr->vid[0]);
      uint16_t min_replay_prot_list_len = uint16_from_buf_le(&dcd_raw_hdr->crpl[0]);

      bool feature_relay = dcd_raw_hdr->features[DCD_FEATURE_RELAY_BYTE]
                           & DCD_FEATURE_RELAY_MASK;

      bool feature_proxy = dcd_raw_hdr->features[DCD_FEATURE_PROXY_BYTE]
                           & DCD_FEATURE_PROXY_MASK;

      bool feature_friend = dcd_raw_hdr->features[DCD_FEATURE_FRIEND_BYTE]
                            & DCD_FEATURE_FRIEND_MASK;

      bool feature_low_power = dcd_raw_hdr->features[DCD_FEATURE_LOW_POWER_BYTE]
                               & DCD_FEATURE_LOW_POWER_MASK;

      sl_status_t dcd_status = btmesh_db_node_dcd_set_header(server_address,
                                                             company_id,
                                                             product_id,
                                                             version_id,
                                                             min_replay_prot_list_len,
                                                             feature_relay,
                                                             feature_proxy,
                                                             feature_friend,
                                                             feature_low_power);
      app_log_info("%s DCD company id: 0x%04x" NL, node_str, company_id);
      app_log_info("%s DCD product id: 0x%04x" NL, node_str, product_id);
      app_log_info("%s DCD version id: 0x%04x" NL, node_str, version_id);
      app_log_info("%s DCD min replay prot list length: %d" NL,
                   node_str,
                   min_replay_prot_list_len);
      app_log_info("%s DCD feature relay:  %d" NL, node_str, feature_relay);
      app_log_info("%s DCD feature proxy:  %d" NL, node_str, feature_proxy);
      app_log_info("%s DCD feature friend: %d" NL, node_str, feature_friend);
      app_log_info("%s DCD feature lpn:    %d" NL, node_str, feature_low_power);
      if (SL_STATUS_OK != process_dcd_status) {
        process_dcd_status = dcd_status;
      } else {
        sl_status_t check_dcd_status =
          btmesh_conf_task_dcd_element_iterate(enc_netkey_index,
                                               server_address,
                                               self->ext.dcd.raw_dcd_data,
                                               self->ext.dcd.raw_dcd_data_size,
                                               BTMESH_CONF_TASK_DCD_ITER_CMD_CHECK_SIZE);
        if (SL_STATUS_OK != check_dcd_status) {
          process_dcd_status = dcd_status;
        } else {
          sl_status_t update_db_status =
            btmesh_conf_task_dcd_element_iterate(enc_netkey_index,
                                                 server_address,
                                                 self->ext.dcd.raw_dcd_data,
                                                 self->ext.dcd.raw_dcd_data_size,
                                                 BTMESH_CONF_TASK_DCD_ITER_CMD_UPDATE_DB);
          process_dcd_status = update_db_status;
          if (SL_STATUS_OK != update_db_status) {
            sl_status_t dcd_clear_status = btmesh_db_node_dcd_clear(server_address);
            app_assert_status_f(dcd_clear_status,
                                "Failed to clear DCD of node (addr=0x%04x)" NL,
                                server_address);
          } else {
            sl_status_t dcd_available_status =
              btmesh_db_node_dcd_set_available(server_address);
            // This call should not fail because it can fail only if the node
            // does not exists but in that case the previous calls would have
            // failed as well.
            app_assert_status_f(dcd_available_status,
                                "Failed to set DCD available of node (addr=0x%04x)" NL,
                                server_address);
          }
        }
      }
    }
  }
  return process_dcd_status;
}

/// Iterate over the elements and models of each element in the DCD and performs
/// the following operations:
///   - It checks if the size of the DCD raw data is consistent with its content
///   - If @p command is set to BTMESH_CONF_TASK_DCD_ITER_CMD_UPDATE_DB then
///     it updates the BT Mesh database with elements and models parsed from
///     the raw DCD data. The parsed elements and models are logged.
static sl_status_t btmesh_conf_task_dcd_element_iterate(uint16_t enc_netkey_index,
                                                        uint16_t server_address,
                                                        const uint8_t *raw_dcd_data,
                                                        uint16_t raw_dcd_data_size,
                                                        btmesh_conf_task_dcd_iter_cmd_t command)
{
  const uint16_t sig_model_size = 2, vendor_model_size = 4;
  uint16_t remaining_size = raw_dcd_data_size;
  uint16_t data_idx = 0;
  uint16_t element_index = 0;
  char node_str[SL_BTMESH_CONF_MAX_LOG_MSG_FRAGMENT_SIZE_CFG_VAL];

  if (remaining_size < sizeof(btmesh_conf_dcd_raw_header_t)) {
    return SL_STATUS_INVALID_COUNT;
  }
  remaining_size -= sizeof(btmesh_conf_dcd_raw_header_t);
  data_idx += sizeof(btmesh_conf_dcd_raw_header_t);

  if (btmesh_conf_any_severe_log_level(APP_LOG_LEVEL_INFO)) {
    int32_t str_retval;
    str_retval = snprintf(node_str,
                          sizeof(node_str),
                          "Node (netkey_idx=%d,addr=0x%04x)",
                          enc_netkey_index,
                          server_address);
    app_assert(0 < str_retval, "String formatting failed." NL);
  }
  node_str[sizeof(node_str) - 1] = '\0';

  while (0 < remaining_size) {
    uint16_t total_element_size;

    if (remaining_size < sizeof(btmesh_conf_dcd_raw_element_t)) {
      return SL_STATUS_INVALID_COUNT;
    }
    const btmesh_conf_dcd_raw_element_t *element =
      (const btmesh_conf_dcd_raw_element_t *)&raw_dcd_data[data_idx];

    uint8_t num_sig_model_ids = element->num_sig_model_ids;
    uint8_t num_vendor_model_ids = element->num_vendor_model_ids;
    uint16_t element_model_data_size = (num_sig_model_ids * sig_model_size)
                                       + (num_vendor_model_ids * vendor_model_size);

    total_element_size = sizeof(btmesh_conf_dcd_raw_element_t)
                         + element_model_data_size;

    if (remaining_size < total_element_size) {
      return SL_STATUS_INVALID_COUNT;
    }

    if (BTMESH_CONF_TASK_DCD_ITER_CMD_UPDATE_DB == command) {
      uint16_t location = uint16_from_buf_le(&element->location[0]);
      sl_status_t add_elem_status = btmesh_db_node_dcd_add_element(server_address,
                                                                   location);
      app_log_info("%s DCD element index %u with location 0x%04x "
                   "(sig_models=%u,vendor_models=%u)" NL,
                   node_str,
                   element_index,
                   location,
                   num_sig_model_ids,
                   num_vendor_model_ids);
      if (SL_STATUS_OK != add_elem_status) {
        return add_elem_status;
      }

      const uint8_t *models_data = element->models;
      uint16_t model_data_idx;
      btmesh_db_model_id_t model_id;

      // Process SIG models
      for (model_data_idx = 0;
           model_data_idx < (num_sig_model_ids * sig_model_size);
           model_data_idx += sig_model_size) {
        model_id.vendor = MESH_SPEC_VENDOR_ID;
        model_id.model = uint16_from_buf_le(&models_data[model_data_idx]);
        sl_status_t add_mdl_status = btmesh_db_node_dcd_add_model(server_address,
                                                                  element_index,
                                                                  model_id);
        app_log_info("%s DCD SIG model 0x%04x-%s (elem=%u)" NL,
                     node_str,
                     model_id.model,
                     btmesh_conf_sig_model_id_to_string(model_id.model),
                     element_index);
        if (SL_STATUS_OK != add_mdl_status) {
          return add_mdl_status;
        }
      }

      // Process vendor models
      for (; model_data_idx < element_model_data_size;
           model_data_idx += vendor_model_size) {
        model_id.vendor = uint16_from_buf_le(&models_data[model_data_idx]);
        model_id.model = uint16_from_buf_le(&models_data[model_data_idx + sizeof(uint16_t)]);
        sl_status_t add_mdl_status = btmesh_db_node_dcd_add_model(server_address,
                                                                  element_index,
                                                                  model_id);
        app_log_info("%s DCD vendor model 0x%04x (elem=%u,vendor=0x%04x)" NL,
                     node_str,
                     model_id.model,
                     element_index,
                     model_id.vendor);
        if (SL_STATUS_OK != add_mdl_status) {
          return add_mdl_status;
        }
      }
    }

    remaining_size -= total_element_size;
    data_idx += total_element_size;
    element_index++;
  }
  return SL_STATUS_OK;
}

static int32_t btmesh_conf_task_dcd_get_to_string(btmesh_conf_task_t *const self,
                                                  char *buffer,
                                                  uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "DCD get (page=%d)",
                  self->ext.dcd.page);
}

// -----------------------------------------------------------------------------
//                          Reset Node Task Functions
// -----------------------------------------------------------------------------
static sl_status_t
btmesh_conf_task_reset_node_request(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    uint32_t *handle);

static sl_status_t
btmesh_conf_task_reset_node_on_event(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     const sl_btmesh_msg_t *evt);

static int32_t btmesh_conf_task_reset_node_to_string(btmesh_conf_task_t *const self,
                                                     char *buffer,
                                                     uint32_t size);

btmesh_conf_task_t *btmesh_conf_task_reset_node_create(void)
{
  btmesh_conf_task_t *self;
  self = btmesh_conf_task_create(BTMESH_CONF_TASK_ID_RESET_NODE);
  if (NULL != self) {
    self->conf_request = btmesh_conf_task_reset_node_request;
    self->on_event = btmesh_conf_task_reset_node_on_event;
    self->destroy = btmesh_conf_task_reset_node_destroy;
    self->to_string = btmesh_conf_task_reset_node_to_string;
  }
  return self;
}

void btmesh_conf_task_reset_node_destroy(btmesh_conf_task_t *self)
{
  btmesh_conf_task_destroy(self);
}

static sl_status_t
btmesh_conf_task_reset_node_request(btmesh_conf_task_t *const self,
                                    uint16_t enc_netkey_index,
                                    uint16_t server_address,
                                    uint32_t *handle)
{
  sl_status_t request_status, api_status;

  api_status = sl_btmesh_config_client_reset_node(enc_netkey_index,
                                                  server_address,
                                                  handle);

  request_status = process_request_status_from_api(self, api_status);
  return request_status;
}

static sl_status_t
btmesh_conf_task_reset_node_on_event(btmesh_conf_task_t *const self,
                                     uint16_t enc_netkey_index,
                                     uint16_t server_address,
                                     const sl_btmesh_msg_t *evt)
{
  sl_status_t event_status = SL_STATUS_IN_PROGRESS;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_config_client_reset_status_id:
    {
      const sl_btmesh_evt_config_client_reset_status_t *reset_status =
        &evt->data.evt_config_client_reset_status;

      sl_status_t event_result = reset_status->result;
      event_status = process_event_result_from_api(self, event_result);
      break;
    }
  }
  return event_status;
}

static int32_t btmesh_conf_task_reset_node_to_string(btmesh_conf_task_t *const self,
                                                     char *buffer,
                                                     uint32_t size)
{
  return snprintf(buffer,
                  size,
                  "node reset");
}

/** @} (end addtogroup btmesh_conf_task) */
/** @} (end addtogroup btmesh_conf) */
