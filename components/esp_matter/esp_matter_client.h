// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <json_to_tlv.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app/data-model/EncodableToTLV.h>
#include <cassert>
#include <esp_err.h>
#include <esp_matter_core.h>
#include <string.h>

namespace esp_matter {
/* Client APIs */
namespace client {

/** Client request types
 */
typedef enum {
    INVOKE_CMD = 0,
    READ_ATTR = 1,
    READ_EVENT = 2,
    WRITE_ATTR = 3,
    SUBSCRIBE_ATTR = 4,
    SUBSCRIBE_EVENT = 5,
} request_type_t;

/** Request handle as the input when calling `connect()` or `cluster_update()`
 *
 */

typedef struct request_handle {
    request_type_t type;
    union {
        chip::app::AttributePathParams attribute_path;
        chip::app::EventPathParams event_path;
        chip::app::CommandPathParams command_path;
    };
    /* This could be the command data field when the request type is INVOKE_CMD,
     * or the attribute value data when the request type is WRITE_ATTR.
     */
    void *request_data;
    request_handle() : type(INVOKE_CMD), request_data(NULL) {}
    request_handle(struct request_handle& req) : type(req.type), request_data(req.request_data)
    {
        if (req.type == INVOKE_CMD) {
            command_path = req.command_path;
        } else if (req.type == WRITE_ATTR || req.type == READ_ATTR || req.type == SUBSCRIBE_ATTR) {
            attribute_path = req.attribute_path;
        } else if (req.type == READ_EVENT || req.type == SUBSCRIBE_EVENT) {
            event_path = req.event_path;
        }
    }
} request_handle_t;

/** Peer device handle */
typedef chip::DeviceProxy peer_device_t;

/** CASE Session Manager */
typedef chip::CASESessionManager case_session_mgr_t;

/** Request send callback
 *
 * This callback will be called when `connect()` or `cluster_update()` is called and the connection completes. The
 * send_request APIs can then be called from the callback.
 *
 * @param[in] peer_device Peer device handle. This can be passed to the send_command APIs.
 * @param[in] req_handle Request handle used by `connect()` or `cluster_update()`.
 * @param[in] priv_data (Optional) Private data associated with the callback. This will be passed to callback. It
 * should stay allocated throughout the lifetime of the device.
 */
typedef void (*request_callback_t)(peer_device_t *peer_device, request_handle_t *req_handle, void *priv_data);

/** Group request send callback
 *
 * This callback will be called when `cluster_update()` is called and the group request is triggered for binding cluster.
 *
 * @note: The request type should be INVOKE_CMD and the command should not expect a response.
 *
 * @param[in] fabric_index The index of the fabric that the group command is sent to.
 * @param[in] req_handle Request handle used by `cluster_update()`.
 * @param[in] priv_data (Optional) Private data associated with the callback. This will be passed to callback. It
 * should stay allocated throughout the lifetime of the device.
 */
typedef void (*group_request_callback_t)(uint8_t fabric_index, request_handle_t *req_handle, void *priv_data);

#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
/** Initialize binding
 *
 * This should be called if the Binding cluster has been created. It just sets a flag for the binding manager to be
 * initialized.
 * If the cluster::binding::create() is being used, this is called internally.
 */
void binding_init();

/** Initialize binding manager
 *
 * This initializes the binding manager. It is called after the matter thread has been started.
 */
void binding_manager_init();
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

/** Connect
 *
 * Connect to another device on the same fabric to send a request.
 *
 * @param[in] case_session_mgr CASE Session Manager to find or establish the session
 * @param[in] fabric_index Fabric index.
 * @param[in] node_id Node ID of the other device.
 * @param[in] req_handle Request to be sent to the remote device.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t connect(case_session_mgr_t *case_session_mgr, uint8_t fabric_index, uint64_t node_id,
                  request_handle_t *req_handle);

/** group_request_send
 *
 * on the same fabric to send a group request.
 *
 * @param[in] fabric_index Fabric index.
 * @param[in] req_handle Request to be sent to the group.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t group_request_send(uint8_t fabric_index, request_handle_t *req_handle);

/** Set request send callback
 *
 * Set the common request send callback and the group request send callback. The common callback will be called
 * when `connect()` or `cluster_update()` is called and the connection completes. The group callback will be called
 * when `cluster_update()` is called and the group request is triggered.
 *
 * @param[in] callback request send callback.
 * @param[in] g_callback group request send callback
 * @param[in] priv_data (Optional) Private data associated with the callback. This will be passed to callback. It
 * should stay allocated throughout the lifetime of the device.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_request_callback(request_callback_t callback, group_request_callback_t g_callback, void *priv_data);

/** Cluster update
 *
 * For an already binded device, this API can be used to get the request send callback, and the send_request APIs can
 * then be called from the callback.
 *
 * @param[in] local_endpoint_id The ID of the local endpoint with a binding cluster.
 * @param[in] req_handle Request information to notify the bound cluster changed.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t cluster_update(uint16_t local_endpoint_id, request_handle_t *req_handle);

namespace interaction {

using chip::Optional;
using chip::app::AttributePathParams;
using chip::app::CommandPathParams;
using chip::app::CommandSender;
using chip::app::ConcreteCommandPath;
using chip::app::EventPathParams;
using chip::app::ReadClient;
using chip::app::StatusIB;
using chip::app::WriteClient;
using chip::Messaging::ExchangeManager;
using chip::Platform::ScopedMemoryBufferWithSize;
using chip::System::Clock::Timeout;
using chip::TLV::TLVReader;
using client::peer_device_t;
using chip::app::DataModel::EncodableToTLV;

class custom_encodable_type : public EncodableToTLV
{
public:
    const char *k_empty_command_data = "{}";
    const char *k_null_attribute_data = "null";
    enum interaction_type {
        k_invoke_cmd = 0,
        k_write_attr,
    };

    custom_encodable_type(const char *json_str, interaction_type usage)
    {
        if (json_str) {
            m_json_str = strdup(json_str);
        } else {
            if (usage == k_invoke_cmd) {
                m_json_str = strdup("{}");
            } else if (usage == k_write_attr) {
                m_json_str = strdup("null");
            }
        }
        assert(m_json_str);
    }

    ~custom_encodable_type() { free(m_json_str); }

    CHIP_ERROR EncodeTo(chip::TLV::TLVWriter & writer, chip::TLV::Tag tag) const override
    {
        if (json_to_tlv(m_json_str, writer, tag) != ESP_OK) {
            return CHIP_ERROR_INTERNAL;
        }
        return CHIP_NO_ERROR;
    }
private:
    char *m_json_str = NULL;
};

class multiple_write_encodable_type
{
public:
    multiple_write_encodable_type(const char *json_str)
    {
        json = cJSON_Parse(json_str);
    }

    ~multiple_write_encodable_type() { cJSON_Delete(json); }

    CHIP_ERROR EncodeTo(chip::TLV::TLVWriter &writer, chip::TLV::Tag tag, size_t index)
    {
        cJSON *json_at_index = NULL;
        if (!json) {
            return CHIP_ERROR_INVALID_ARGUMENT;
        }
        if (json->type != cJSON_Array) {
            json_at_index = json;
        } else {
            json_at_index = cJSON_GetArrayItem(json, index);
        }
        if (json_to_tlv(json_at_index, writer, tag) != ESP_OK) {
            return CHIP_ERROR_INTERNAL;
        }
        return CHIP_NO_ERROR;
    }

    size_t GetJsonArraySize() { return static_cast<size_t>(cJSON_GetArraySize(json)); }

private:
    cJSON *json = NULL;
};

/** Command invoke APIs
 *
 * They can be used for all the commands of all the clusters, including the custom clusters.
 */
namespace invoke {

class custom_command_callback final : public chip::app::CommandSender::Callback {
public:
    using on_success_callback_t =
        std::function<void(void *, const ConcreteCommandPath &, const StatusIB &, TLVReader *)>;
    using on_error_callback_t = std::function<void(void *, CHIP_ERROR error)>;
    using on_done_callback_t = std::function<void(void *, CommandSender *command_sender)>;

    custom_command_callback(void *ctx, on_success_callback_t on_success, on_error_callback_t on_error,
                            on_done_callback_t on_done = {})
        : on_success_cb(on_success)
        , on_error_cb(on_error)
        , on_done_cb(on_done)
        , context(ctx)
    {
    }
    void set_on_done_callback(on_done_callback_t on_done) { on_done_cb = on_done; }

private:
    void OnResponse(CommandSender *command_sender, const ConcreteCommandPath &command_path, const StatusIB &status,
                    TLVReader *response_data) override
    {
        if (called_callback) {
            return;
        }
        called_callback = true;
        if (on_success_cb) {
            on_success_cb(context, command_path, status, response_data);
        }
    }

    void OnError(const CommandSender *command_sender, CHIP_ERROR error) override
    {
        if (called_callback) {
            return;
        }
        called_callback = true;
        if (on_error_cb) {
            on_error_cb(context, error);
        }
    }

    void OnDone(CommandSender *command_sender) override
    {
        if (!called_callback) {
            OnError(command_sender, CHIP_END_OF_TLV);
        }
        if (on_done_cb) {
            on_done_cb(context, command_sender);
        }
    }

    on_success_callback_t on_success_cb;
    on_error_callback_t on_error_cb;
    on_done_callback_t on_done_cb;
    bool called_callback = false;
    void *context;
};

esp_err_t send_request(void *ctx, peer_device_t *remote_device, const CommandPathParams &command_path,
                       const char *command_data_json_str, custom_command_callback::on_success_callback_t on_success,
                       custom_command_callback::on_error_callback_t on_error,
                       const Optional<uint16_t> &timed_invoke_timeout_ms,
                       const Optional<Timeout> &response_timeout = chip::NullOptional);

esp_err_t send_group_request(const uint8_t fabric_index, const CommandPathParams &command_path,
                             const char *command_data_json_str);

esp_err_t send_request(void *ctx, peer_device_t *remote_device, const CommandPathParams &command_path,
                       const chip::app::DataModel::EncodableToTLV &encodable,
                       custom_command_callback::on_success_callback_t on_success,
                       custom_command_callback::on_error_callback_t on_error,
                       const Optional<uint16_t> &timed_invoke_timeout_ms,
                       const Optional<Timeout> &response_timeout = chip::NullOptional);


esp_err_t send_group_request(const uint8_t fabric_index, const CommandPathParams &command_path,
                             const chip::app::DataModel::EncodableToTLV &encodable);

} // namespace invoke

/** Attribute/event read API
 *
 * It can be used for reading all the attributes/events of all the clusters, including the custom clusters.
 */
namespace read {
esp_err_t send_request(client::peer_device_t *remote_device, AttributePathParams *attr_path, size_t attr_path_size,
                       EventPathParams *event_path, size_t event_path_size, ReadClient::Callback &callback);
} // namespace read

/** Attribute write API
 *
 * It can be used for writing all the attributes of all the clusters, including the custom clusters.
 */
namespace write {
esp_err_t send_request(client::peer_device_t *remote_device, AttributePathParams &attr_path,
                       const char *attr_val_json_str, WriteClient::Callback &callback,
                       const chip::Optional<uint16_t> &timeout_ms);

esp_err_t send_request(client::peer_device_t *remote_device, AttributePathParams &attr_path,
                       const chip::app::DataModel::EncodableToTLV &encodable, WriteClient::Callback &callback,
                       const chip::Optional<uint16_t> &timeout_ms);

esp_err_t send_request(client::peer_device_t *remote_device, ScopedMemoryBufferWithSize<AttributePathParams> &attr_paths,
                       multiple_write_encodable_type &json_encodable, WriteClient::Callback &callback,
                       const chip::Optional<uint16_t> &timeout_ms);
} // namespace write

namespace subscribe {
esp_err_t send_request(client::peer_device_t *remote_device, AttributePathParams *attr_path, size_t attr_path_size,
                       EventPathParams *event_path, size_t event_path_size, uint16_t min_interval,
                       uint16_t max_interval, bool keep_subscription, bool auto_resubscribe,
                       ReadClient::Callback &callback);
} // namespace subscribe

} // namespace interaction
} // namespace client
} // namespace esp_matter
