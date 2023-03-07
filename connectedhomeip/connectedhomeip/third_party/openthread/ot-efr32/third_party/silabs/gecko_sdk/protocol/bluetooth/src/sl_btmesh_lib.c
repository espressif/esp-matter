/***************************************************************************//**
 * @brief Mesh helper library
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

#include <stdint.h>
#include <stdlib.h>

#include "sl_btmesh_api.h"

#include "sl_status.h"
#include "sl_btmesh_generic_model_capi_types.h"
#include "sl_btmesh_lib.h"
#include "sl_btmesh_serdeser.h"
#include "sl_malloc.h"

/* Enough room for Property ID (2 bytes), access flags (1 byte), and
   longest currently defined property characteristic (36 bytes) */
#define LONGEST_STATE 39
#define LONGEST_REQUEST 39

uint32_t mesh_lib_transition_time_to_ms(uint8_t t)
{
  uint32_t res_ms[4] = { 100, 1000, 10000, 600000 };
  uint32_t unit = (t >> 6) & 0x03;
  uint32_t count = t & 0x3f;
  return res_ms[unit] * count;
}

struct reg {
  uint16_t model_id;
  uint16_t elem_index;
  union {
    struct {
      mesh_lib_generic_server_client_request_cb client_request_cb;
      mesh_lib_generic_server_change_cb state_changed_cb;
      mesh_lib_generic_server_recall_cb state_recall_cb;
    } server;
    struct {
      mesh_lib_generic_client_server_response_cb server_response_cb;
    } client;
  };
};

/**
 * @brief Struct containing the registry descriptor values
 */
struct registry_s {
/// Pointer to array allocated to store registered handlers
  struct reg *entries;
/// Length of the allocated storage
  size_t length;
/// Index of the next empty element
  size_t index;
/// Allocation increment
  size_t increment;
};
static struct registry_s registry = { .entries = NULL,
                                      .length = 0,
                                      .index = 0,
                                      .increment = 0 };

static struct reg *find_reg(uint16_t model_id, uint16_t elem_index)
{
  size_t r;
  for (r = 0; r < registry.index; r++) {
    if (registry.entries[r].model_id == model_id
        && registry.entries[r].elem_index == elem_index) {
      return &registry.entries[r];
    }
  }
  return NULL;
}

static struct reg *find_free(void)
{
  struct reg *tmp;
  if (registry.index == registry.length) {
    // if full and no increment set
    if (registry.increment == 0) {
      return NULL;
    }
    // if no empty slot has been found, reallocate
    tmp = registry.entries;
    registry.entries = sl_realloc(registry.entries,
                                  (registry.length
                                   + registry.increment)
                                  * sizeof(struct reg));
    if (NULL == registry.entries) {
      registry.entries = tmp;
      return NULL;
    }
    registry.length += registry.increment;
  }
  return &registry.entries[registry.index++];
}

sl_status_t mesh_lib_init(size_t initial_num, size_t increment)
{
  registry.length = initial_num;
  if (registry.length > 0) {
    registry.entries = sl_calloc(registry.length, sizeof(struct reg));
    if (registry.entries == NULL) {
      return SL_STATUS_NO_MORE_RESOURCE;
    }
  } else {
    registry.entries = NULL;
  }
  registry.index = 0;
  registry.increment = increment;
  return SL_STATUS_OK;
}

void mesh_lib_deinit(void)
{
  if (registry.entries != NULL) {
    sl_free(registry.entries);
    registry.entries = NULL;
    registry.length = 0;
    registry.index = 0;
    registry.increment = 0;
  }
}

sl_status_t
mesh_lib_generic_server_register_handler(uint16_t model_id,
                                         uint16_t elem_index,
                                         mesh_lib_generic_server_client_request_cb request,
                                         mesh_lib_generic_server_change_cb change,
                                         mesh_lib_generic_server_recall_cb recall)
{
  struct reg *entry = NULL;

  if ((request == NULL) && (change == NULL) && (recall == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  entry = find_reg(model_id, elem_index);
  if (entry != NULL) {
    return SL_STATUS_INVALID_STATE; // already exists
  }

  entry = find_free();
  if (entry == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  entry->model_id = model_id;
  entry->elem_index = elem_index;
  entry->server.client_request_cb = request;
  entry->server.state_changed_cb = change;
  entry->server.state_recall_cb = recall;
  return SL_STATUS_OK;
}

sl_status_t
mesh_lib_generic_client_register_handler(uint16_t model_id,
                                         uint16_t elem_index,
                                         mesh_lib_generic_client_server_response_cb response)
{
  struct reg *entry = NULL;

  if (response == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  entry = find_reg(model_id, elem_index);
  if (entry != NULL) {
    return SL_STATUS_INVALID_STATE; // already exists
  }

  entry = find_free();
  if (entry == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  entry->model_id = model_id;
  entry->elem_index = elem_index;
  entry->client.server_response_cb = response;
  return SL_STATUS_OK;
}

void mesh_lib_generic_server_event_handler(sl_btmesh_msg_t *evt)
{
  sl_btmesh_evt_generic_server_client_request_t *req = NULL;
  sl_btmesh_evt_generic_server_state_changed_t *chg = NULL;
  sl_btmesh_evt_generic_server_state_recall_t *recall = NULL;
  struct mesh_generic_request request;
  struct mesh_generic_state current;
  struct mesh_generic_state target;
  int has_target;
  struct reg *entry;

  if (evt == NULL) {
    return;
  }

  switch (SL_BGAPI_MSG_ID(evt->header)) {
    case sl_btmesh_evt_generic_server_client_request_id:
      req = &(evt->data.evt_generic_server_client_request);
      entry = find_reg(req->model_id, req->elem_index);
      if (entry != NULL) {
        if (mesh_lib_deserialize_request(&request,
                                         (mesh_generic_request_t)req->type,
                                         req->parameters.data,
                                         req->parameters.len)
            == SL_STATUS_OK) {
          if (entry->server.client_request_cb != NULL) {
            (entry->server.client_request_cb)(req->model_id,
                                              req->elem_index,
                                              req->client_address,
                                              req->server_address,
                                              req->appkey_index,
                                              &request,
                                              req->transition_ms,
                                              req->delay_ms,
                                              req->flags);
          }
        }
      }
      break;
    case sl_btmesh_evt_generic_server_state_changed_id:
      chg = &(evt->data.evt_generic_server_state_changed);
      entry = find_reg(chg->model_id, chg->elem_index);
      if (entry != NULL) {
        if (mesh_lib_deserialize_state(&current,
                                       &target,
                                       &has_target,
                                       (mesh_generic_state_t)chg->type,
                                       chg->parameters.data,
                                       chg->parameters.len)
            == SL_STATUS_OK) {
          if (entry->server.state_changed_cb != NULL) {
            (entry->server.state_changed_cb)(chg->model_id,
                                             chg->elem_index,
                                             &current,
                                             has_target ? &target : NULL,
                                             chg->remaining_ms);
          }
        }
      }
      break;
    case sl_btmesh_evt_generic_server_state_recall_id:
      recall = &(evt->data.evt_generic_server_state_recall);
      entry = find_reg(recall->model_id, recall->elem_index);
      if (entry != NULL) {
        if (mesh_lib_deserialize_state(&current,
                                       &target,
                                       &has_target,
                                       (mesh_generic_state_t)recall->type,
                                       recall->parameters.data,
                                       recall->parameters.len)
            == SL_STATUS_OK) {
          if (entry->server.state_recall_cb != NULL) {
            (entry->server.state_recall_cb)(recall->model_id,
                                            recall->elem_index,
                                            &current,
                                            has_target ? &target : NULL,
                                            recall->transition_time_ms);
          }
        }
      }
      break;
  }
}

void mesh_lib_generic_client_event_handler(sl_btmesh_msg_t *evt)
{
  sl_btmesh_evt_generic_client_server_status_t *res = NULL;
  struct mesh_generic_state current;
  struct mesh_generic_state target;
  int has_target;
  struct reg *entry;

  if (evt == NULL) {
    return;
  }

  switch (SL_BGAPI_MSG_ID(evt->header)) {
    case sl_btmesh_evt_generic_client_server_status_id:
      res = &(evt->data.evt_generic_client_server_status);
      entry = find_reg(res->model_id, res->elem_index);
      if (entry != NULL) {
        if (mesh_lib_deserialize_state(&current,
                                       &target,
                                       &has_target,
                                       (mesh_generic_state_t)res->type,
                                       res->parameters.data,
                                       res->parameters.len)
            == SL_STATUS_OK) {
          (entry->client.server_response_cb)(res->model_id,
                                             res->elem_index,
                                             res->client_address,
                                             res->server_address,
                                             &current,
                                             has_target ? &target : NULL,
                                             res->remaining_ms,
                                             res->flags);
        }
      }
      break;
  }
}

sl_status_t
mesh_lib_generic_server_respond(uint16_t model_id,
                                uint16_t element_index,
                                uint16_t client_addr,
                                uint16_t appkey_index,
                                const struct mesh_generic_state *current,
                                const struct mesh_generic_state *target,
                                uint32_t remaining_ms,
                                uint8_t response_flags)
{
  uint8_t buf[LONGEST_STATE];
  size_t len;

  if (mesh_lib_serialize_state(current, target, buf, sizeof(buf), &len) != SL_STATUS_OK) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  return sl_btmesh_generic_server_respond(client_addr,
                                          element_index,
                                          model_id,
                                          appkey_index,
                                          remaining_ms,
                                          response_flags,
                                          current->kind,
                                          len,
                                          buf);
}

sl_status_t
mesh_lib_generic_server_update(uint16_t model_id,
                               uint16_t element_index,
                               const struct mesh_generic_state *current,
                               const struct mesh_generic_state *target,
                               uint32_t remaining_ms)
{
  uint8_t buf[LONGEST_STATE];
  size_t len;

  if (mesh_lib_serialize_state(current, target, buf, sizeof(buf), &len) != SL_STATUS_OK) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  return sl_btmesh_generic_server_update(element_index,
                                         model_id,
                                         remaining_ms,
                                         current->kind,
                                         len,
                                         buf);
}

sl_status_t
mesh_lib_generic_server_publish(uint16_t model_id,
                                uint16_t element_index,
                                mesh_generic_state_t kind)
{
  return sl_btmesh_generic_server_publish(element_index,
                                          model_id,
                                          kind);
}

sl_status_t mesh_lib_generic_client_get(uint16_t model_id,
                                        uint16_t element_index,
                                        uint16_t server_addr,
                                        uint16_t appkey_index,
                                        mesh_generic_state_t kind)
{
  return sl_btmesh_generic_client_get(server_addr,
                                      element_index,
                                      model_id,
                                      appkey_index,
                                      kind);
}

sl_status_t mesh_lib_generic_client_set(uint16_t model_id,
                                        uint16_t element_index,
                                        uint16_t server_addr,
                                        uint16_t appkey_index,
                                        uint8_t transaction_id,
                                        const struct mesh_generic_request *request,
                                        uint32_t transition_ms,
                                        uint16_t delay_ms,
                                        uint8_t flags)
{
  uint8_t buf[LONGEST_REQUEST];
  size_t len;

  if (mesh_lib_serialize_request(request, buf, sizeof(buf), &len) != SL_STATUS_OK) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  return sl_btmesh_generic_client_set(server_addr,
                                      element_index,
                                      model_id,
                                      appkey_index,
                                      transaction_id,
                                      transition_ms,
                                      delay_ms,
                                      flags,
                                      request->kind,
                                      len,
                                      buf);
}

sl_status_t
mesh_lib_generic_client_publish(uint16_t model_id,
                                uint16_t element_index,
                                uint8_t transaction_id,
                                const struct mesh_generic_request *request,
                                uint32_t transition_ms,
                                uint16_t delay_ms,
                                uint8_t request_flags)
{
  uint8_t buf[LONGEST_REQUEST];
  size_t len;

  if (mesh_lib_serialize_request(request, buf, sizeof(buf), &len) != SL_STATUS_OK) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  return sl_btmesh_generic_client_publish(element_index,
                                          model_id,
                                          transaction_id,
                                          transition_ms,
                                          delay_ms,
                                          request_flags,
                                          request->kind,
                                          len,
                                          buf);
}
