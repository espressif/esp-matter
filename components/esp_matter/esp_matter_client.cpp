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

#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_client.h>
#include <esp_matter_core.h>
#include <json_to_tlv.h>

#include <app/ConcreteAttributePath.h>
#include <app/EventHeader.h>
#include <app/MessageDef/DataVersionFilterIBs.h>
#include <app/MessageDef/StatusIB.h>
#include <app/ReadClient.h>
#include <app/ReadPrepareParams.h>
#include <app/clusters/bindings/BindingManager.h>
#include <core/Optional.h>
#include <core/TLVReader.h>
#include <core/TLVWriter.h>

#include "app/CommandPathParams.h"
#include "app/CommandSender.h"
#include "app/InteractionModelEngine.h"
#include "app/data-model/EncodableToTLV.h"

using namespace chip::app::Clusters;
using chip::DeviceProxy;
using chip::OperationalDeviceProxy;
using chip::ScopedNodeId;
using chip::SessionHandle;
using chip::Callback::Callback;
using chip::Messaging::ExchangeManager;

static const char *TAG = "esp_matter_client";

namespace esp_matter {
namespace client {

static request_callback_t client_request_callback = NULL;
static group_request_callback_t client_group_request_callback = NULL;
static void *request_callback_priv_data;
static bool initialize_binding_manager = false;

esp_err_t set_request_callback(request_callback_t callback, group_request_callback_t g_callback, void *priv_data)
{
    client_request_callback = callback;
    client_group_request_callback = g_callback;
    request_callback_priv_data = priv_data;
    return ESP_OK;
}

void esp_matter_connection_success_callback(void *context, ExchangeManager &exchangeMgr,
                                            const SessionHandle &sessionHandle)
{
    request_handle_t *req_handle = static_cast<request_handle_t *>(context);
    VerifyOrReturn(req_handle, ESP_LOGE(TAG, "Failed to call connect_success_callback since the request handle is NULL"));
    ESP_LOGI(TAG, "New connection success");
    // Only unicast binding needs to establish the connection
    if (client_request_callback) {
        OperationalDeviceProxy device(&exchangeMgr, sessionHandle);
        client_request_callback(&device, req_handle, request_callback_priv_data);
    }
    chip::Platform::Delete(req_handle);
}

void esp_matter_connection_failure_callback(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    request_handle_t *req_handle = static_cast<request_handle_t *>(context);
    ESP_LOGI(TAG, "New connection failure");
    if (req_handle) {
        chip::Platform::Delete(req_handle);
    }
}

esp_err_t connect(case_session_mgr_t *case_session_mgr, uint8_t fabric_index, uint64_t node_id,
                  request_handle_t *req_handle)
{
    VerifyOrReturnError(case_session_mgr, ESP_ERR_INVALID_ARG);
    static Callback<chip::OnDeviceConnected> success_callback(esp_matter_connection_success_callback, NULL);
    static Callback<chip::OnDeviceConnectionFailure> failure_callback(esp_matter_connection_failure_callback, NULL);

    request_handle_t *context = chip::Platform::New<request_handle_t>(req_handle);
    VerifyOrReturnError(context, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "failed to alloc memory for the command handle"));
    success_callback.mContext = static_cast<void *>(context);
    failure_callback.mContext = static_cast<void *>(context);
    case_session_mgr->FindOrEstablishSession(ScopedNodeId(node_id, fabric_index), &success_callback, &failure_callback);
    return ESP_OK;
}

esp_err_t group_request_send(uint8_t fabric_index, request_handle_t *req_handle)
{
    VerifyOrReturnError(req_handle, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "command handle is null"));
    if (client_group_request_callback) {
        client_group_request_callback(fabric_index, req_handle, request_callback_priv_data);
    }

    return ESP_OK;
}

static void esp_matter_command_client_binding_callback(const EmberBindingTableEntry &binding,
                                                       OperationalDeviceProxy *peer_device, void *context)
{
    request_handle_t *req_handle = static_cast<request_handle_t *>(context);
    VerifyOrReturn(req_handle, ESP_LOGE(TAG, "Failed to call the binding callback since command handle is NULL"));
    if (binding.type == MATTER_UNICAST_BINDING && peer_device) {
        if (client_request_callback) {
            if (req_handle->type == INVOKE_CMD) {
                req_handle->command_path.mFlags.Set(chip::app::CommandPathFlags::kEndpointIdValid);
                req_handle->command_path.mFlags.Clear(chip::app::CommandPathFlags::kGroupIdValid);
                req_handle->command_path.mEndpointId = binding.remote;
            } else if (req_handle->type == WRITE_ATTR || req_handle->type == READ_ATTR ||
                       req_handle->type == SUBSCRIBE_ATTR) {
                req_handle->attribute_path.mEndpointId = binding.remote;
            } else if (req_handle->type == READ_EVENT || req_handle->type == SUBSCRIBE_EVENT) {
                req_handle->event_path.mEndpointId = binding.remote;
            }
            client_request_callback(peer_device, req_handle, request_callback_priv_data);
        }
    } else if (binding.type == MATTER_MULTICAST_BINDING && !peer_device) {
        if (client_group_request_callback) {
            if (req_handle->type == INVOKE_CMD) {
                req_handle->command_path.mFlags.Set(chip::app::CommandPathFlags::kGroupIdValid);
                req_handle->command_path.mFlags.Clear(chip::app::CommandPathFlags::kEndpointIdValid);
                req_handle->command_path.mGroupId = binding.groupId;
            } else {
                return;
            }
            client_group_request_callback(binding.fabricIndex, req_handle, request_callback_priv_data);
        }
    }
}

static void esp_matter_binding_context_release(void *context)
{
    if (context) {
        chip::Platform::Delete(static_cast<request_handle_t *>(context));
    }
}

esp_err_t cluster_update(uint16_t local_endpoint_id, request_handle_t *req_handle)
{
    request_handle_t *context = chip::Platform::New<request_handle_t>(req_handle);
    VerifyOrReturnError(context, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "failed to alloc memory for the request handle"));
    chip::ClusterId notified_cluster_id = chip::kInvalidClusterId;
    if (req_handle->type == INVOKE_CMD) {
        notified_cluster_id = req_handle->command_path.mClusterId;
    } else if (req_handle->type == WRITE_ATTR || req_handle->type == READ_ATTR || req_handle->type == SUBSCRIBE_ATTR) {
        notified_cluster_id = req_handle->attribute_path.mClusterId;
    } else if (req_handle->type == READ_EVENT || req_handle->type == SUBSCRIBE_EVENT) {
        notified_cluster_id = req_handle->event_path.mClusterId;
    }
    VerifyOrReturnError(notified_cluster_id != chip::kInvalidClusterId, ESP_ERR_INVALID_ARG);
    if (CHIP_NO_ERROR !=
        chip::BindingManager::GetInstance().NotifyBoundClusterChanged(local_endpoint_id, notified_cluster_id,
                                                                      static_cast<void *>(context))) {
        chip::Platform::Delete(context);
        ESP_LOGE(TAG, "failed to notify the bound cluster changed");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static void __binding_manager_init(intptr_t arg)
{
    auto &server = chip::Server::GetInstance();
    struct chip::BindingManagerInitParams binding_init_params = {
        .mFabricTable = &server.GetFabricTable(),
        .mCASESessionManager = server.GetCASESessionManager(),
        .mStorage = &server.GetPersistentStorage(),
    };

    chip::BindingManager::GetInstance().Init(binding_init_params);
    chip::BindingManager::GetInstance().RegisterBoundDeviceChangedHandler(esp_matter_command_client_binding_callback);
    chip::BindingManager::GetInstance().RegisterBoundDeviceContextReleaseHandler(esp_matter_binding_context_release);
}

void binding_manager_init()
{
    if (initialize_binding_manager) {
        chip::DeviceLayer::PlatformMgr().ScheduleWork(__binding_manager_init);
        initialize_binding_manager = false;
    }
}

void binding_init()
{
    initialize_binding_manager = true;
}

namespace interaction {
using chip::app::DataModel::EncodableToTLV;

namespace invoke {

using command_data_tag = chip::app::CommandDataIB::Tag;
using chip::TLV::ContextTag;
using chip::TLV::TLVWriter;

esp_err_t send_request(void *ctx, peer_device_t *remote_device, const CommandPathParams &command_path,
                       const char *command_data_json_str, custom_command_callback::on_success_callback_t on_success,
                       custom_command_callback::on_error_callback_t on_error,
                       const Optional<uint16_t> &timed_invoke_timeout_ms, const Optional<Timeout> &response_timeout)
{
    custom_encodable_type type(command_data_json_str, custom_encodable_type::interaction_type::k_invoke_cmd);
    return send_request(ctx, remote_device, command_path, type, on_success, on_error, timed_invoke_timeout_ms, response_timeout);
}

esp_err_t send_group_request(const uint8_t fabric_index, const CommandPathParams &command_path,
                             const char *command_data_json_str)
{
    custom_encodable_type type(command_data_json_str, custom_encodable_type::interaction_type::k_invoke_cmd);
    return send_group_request(fabric_index, command_path, type);
}

esp_err_t send_request(void *ctx, peer_device_t *remote_device, const CommandPathParams &command_path,
                       const EncodableToTLV &encodable, custom_command_callback::on_success_callback_t on_success,
                       custom_command_callback::on_error_callback_t on_error,
                       const Optional<uint16_t> &timed_invoke_timeout_ms, const Optional<Timeout> &response_timeout)
{
    VerifyOrReturnError(remote_device->GetSecureSession().HasValue() && !remote_device->GetSecureSession().Value()->IsGroupSession(),
                        ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Invalid Session Type"));
    VerifyOrReturnError(!command_path.mFlags.Has(chip::app::CommandPathFlags::kGroupIdValid),
                        ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Invalid CommandPathFlags"));
    auto decoder = chip::Platform::MakeUnique<custom_command_callback>(ctx, on_success, on_error);
    VerifyOrReturnError(decoder != nullptr, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "No memory for command callback"));

    auto on_done = [raw_decoder_ptr = decoder.get()](void *context, CommandSender *command_sender) {
        chip::Platform::Delete(command_sender);
        chip::Platform::Delete(raw_decoder_ptr);
    };
    decoder->set_on_done_callback(on_done);

    auto command_sender = chip::Platform::MakeUnique<CommandSender>(decoder.get(), remote_device->GetExchangeManager(),
                                                                    timed_invoke_timeout_ms.HasValue());
    VerifyOrReturnError(command_sender != nullptr, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "No memory for command sender"));
    chip::app::CommandSender::AddRequestDataParameters add_request_data_params(timed_invoke_timeout_ms);
    command_sender->AddRequestData(command_path, encodable, add_request_data_params);
    VerifyOrReturnError(command_sender->SendCommandRequest(remote_device->GetSecureSession().Value(), response_timeout) == CHIP_NO_ERROR, ESP_FAIL, ESP_LOGE(TAG, "Failed to send command request"));
    (void)decoder.release();
    (void)command_sender.release();
    return ESP_OK;
}

esp_err_t send_group_request(const uint8_t fabric_index, const CommandPathParams &command_path,
                             const EncodableToTLV &encodeable)
{
    VerifyOrReturnError(command_path.mFlags.Has(chip::app::CommandPathFlags::kGroupIdValid), ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Invalid CommandPathFlags"));
    chip::Transport::OutgoingGroupSession session(command_path.mGroupId, fabric_index);
    chip::Messaging::ExchangeManager *exchange_mgr =
        chip::app::InteractionModelEngine::GetInstance()->GetExchangeManager();
    auto command_sender = chip::Platform::MakeUnique<chip::app::CommandSender>(nullptr, exchange_mgr);
    VerifyOrReturnError(command_sender != nullptr, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "No memory for command sender"));
    chip::app::CommandSender::AddRequestDataParameters add_request_data_params;
    command_sender->AddRequestData(command_path, encodeable, add_request_data_params);
    VerifyOrReturnError(command_sender->SendGroupCommandRequest(SessionHandle(session)) == CHIP_NO_ERROR, ESP_FAIL, ESP_LOGE(TAG, "Failed to send command request"));
    return ESP_OK;
}

} // namespace invoke

using chip::SubscriptionId;
using chip::app::ConcreteDataAttributePath;
using chip::app::EventHeader;
using chip::app::ReadPrepareParams;
using chip::app::StatusIB;
using chip::app::DataVersionFilterIBs::Builder;
using chip::TLV::TLVReader;
using chip::TLV::TLVWriter;

class client_deleter_read_callback : public ReadClient::Callback {
public:
    client_deleter_read_callback(ReadClient::Callback &callback)
        : m_callback(callback)
    {
    }

private:
    void OnReportBegin() override { m_callback.OnReportBegin(); }
    void OnReportEnd() override { m_callback.OnReportEnd(); }
    void OnError(CHIP_ERROR aError) override { m_callback.OnError(aError); }

    void OnAttributeData(const ConcreteDataAttributePath &aPath, TLVReader *apData, const StatusIB &aStatus) override
    {
        m_callback.OnAttributeData(aPath, apData, aStatus);
    }

    void OnEventData(const EventHeader &aEventHeader, TLVReader *apData, const StatusIB *apStatus) override
    {
        m_callback.OnEventData(aEventHeader, apData, apStatus);
    }

    void OnDone(ReadClient *apReadClient) override
    {
        m_callback.OnDone(apReadClient);
        chip::Platform::Delete(apReadClient);
        chip::Platform::Delete(this);
    }

    void OnSubscriptionEstablished(SubscriptionId aSubscriptionId) override
    {
        m_callback.OnSubscriptionEstablished(aSubscriptionId);
    }

    CHIP_ERROR OnResubscriptionNeeded(ReadClient *apReadClient, CHIP_ERROR aTerminationCause) override
    {
        return m_callback.OnResubscriptionNeeded(apReadClient, aTerminationCause);
    }

    void OnDeallocatePaths(chip::app::ReadPrepareParams &&aReadPrepareParams) override
    {
        m_callback.OnDeallocatePaths(std::move(aReadPrepareParams));
    }

    CHIP_ERROR OnUpdateDataVersionFilterList(Builder &aDataVersionFilterIBsBuilder,
                                             const chip::Span<AttributePathParams> &aAttributePaths,
                                             bool &aEncodedDataVersionList) override
    {
        return m_callback.OnUpdateDataVersionFilterList(aDataVersionFilterIBsBuilder, aAttributePaths,
                                                        aEncodedDataVersionList);
    }

    CHIP_ERROR GetHighestReceivedEventNumber(chip::Optional<chip::EventNumber> &aEventNumber) override
    {
        return m_callback.GetHighestReceivedEventNumber(aEventNumber);
    }

    void OnUnsolicitedMessageFromPublisher(ReadClient *apReadClient) override
    {
        m_callback.OnUnsolicitedMessageFromPublisher(apReadClient);
    }

    void OnCASESessionEstablished(const SessionHandle &aSession, ReadPrepareParams &aSubscriptionParams) override
    {
        m_callback.OnCASESessionEstablished(aSession, aSubscriptionParams);
    }

    ReadClient::Callback &m_callback;
};

namespace read {

esp_err_t send_request(client::peer_device_t *remote_device, AttributePathParams *attr_path, size_t attr_path_size,
                       EventPathParams *event_path, size_t event_path_size, ReadClient::Callback &callback)
{
    VerifyOrReturnError(remote_device->GetSecureSession().HasValue() && !remote_device->GetSecureSession().Value()->IsGroupSession(), ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Invalid Session Type"));
    VerifyOrReturnError((attr_path && attr_path_size != 0) || (event_path && event_path_size != 0),
                    ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Invalid attribute path and event path"));
    ReadPrepareParams params(remote_device->GetSecureSession().Value());
    params.mpAttributePathParamsList = attr_path;
    params.mAttributePathParamsListSize = attr_path_size;
    params.mpEventPathParamsList = event_path;
    params.mEventPathParamsListSize = event_path_size;
    params.mpDataVersionFilterList = nullptr;
    params.mDataVersionFilterListSize = 0;
    params.mIsFabricFiltered = false;

    auto client_deleter_callback = chip::Platform::MakeUnique<client_deleter_read_callback>(callback);
    VerifyOrReturnError(client_deleter_callback, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "Failed to allocate memory for client deleter callback"));
    auto client = chip::Platform::MakeUnique<ReadClient>(chip::app::InteractionModelEngine::GetInstance(),
                                                         remote_device->GetExchangeManager(), *client_deleter_callback,
                                                         ReadClient::InteractionType::Read);
    VerifyOrReturnError(client, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "Failed to allocate memory for ReadClient"));

    VerifyOrReturnError(client->SendRequest(params) == CHIP_NO_ERROR,
                    ESP_FAIL, ESP_LOGE(TAG, "Failed to send read request"));

    // The memory will be released when OnDone() is called
    client.release();
    client_deleter_callback.release();
    return ESP_OK;
}

} // namespace read

namespace subscribe {

esp_err_t send_request(client::peer_device_t *remote_device, AttributePathParams *attr_path, size_t attr_path_size,
                       EventPathParams *event_path, size_t event_path_size, uint16_t min_interval,
                       uint16_t max_interval, bool keep_subscription, bool auto_resubscribe,
                       ReadClient::Callback &callback)
{
    VerifyOrReturnError(remote_device->GetSecureSession().HasValue() && !remote_device->GetSecureSession().Value()->IsGroupSession(),
                    ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Invalid Session Type"));
    VerifyOrReturnError((attr_path && attr_path_size != 0) || (event_path && event_path_size != 0),
                    ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Invalid attribute path and event path"));
    ReadPrepareParams params(remote_device->GetSecureSession().Value());
    params.mpAttributePathParamsList = attr_path;
    params.mAttributePathParamsListSize = attr_path_size;
    params.mpEventPathParamsList = event_path;
    params.mEventPathParamsListSize = event_path_size;
    params.mpDataVersionFilterList = nullptr;
    params.mDataVersionFilterListSize = 0;
    params.mIsFabricFiltered = false;
    params.mMinIntervalFloorSeconds = min_interval;
    params.mMaxIntervalCeilingSeconds = max_interval;
    params.mKeepSubscriptions = keep_subscription;

    auto client_deleter_callback = chip::Platform::MakeUnique<client_deleter_read_callback>(callback);
    VerifyOrReturnError(client_deleter_callback, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "Failed to allocate memory for client deleter callback"));
    auto client = chip::Platform::MakeUnique<ReadClient>(chip::app::InteractionModelEngine::GetInstance(),
                                                         remote_device->GetExchangeManager(), *client_deleter_callback,
                                                         ReadClient::InteractionType::Subscribe);
    VerifyOrReturnError(client, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "Failed to allocate memory for ReadClient"));

    CHIP_ERROR err = CHIP_NO_ERROR;
    if (auto_resubscribe) {
        err = client->SendAutoResubscribeRequest(std::move(params));
    } else {
        err = client->SendRequest(params);
    }
    VerifyOrReturnError(err == CHIP_NO_ERROR,
                    ESP_FAIL, ESP_LOGE(TAG, "Failed to send subscribe request"));
    // The memory will be released when OnDone() is called
    client.release();
    client_deleter_callback.release();
    return ESP_OK;
}

} // namespace subscribe

namespace write {
static constexpr size_t k_encoded_buf_size = chip::kMaxAppMessageLen;

class client_deleter_write_callback : public WriteClient::Callback {
public:
    client_deleter_write_callback(WriteClient::Callback &callback)
        : m_callback(callback)
    {
    }
    void OnResponse(const WriteClient *apWriteClient, const ConcreteDataAttributePath &aPath,
                    StatusIB attributeStatus) override
    {
        m_callback.OnResponse(apWriteClient, aPath, attributeStatus);
    }

    void OnError(const WriteClient *apWriteClient, CHIP_ERROR aError) override
    {
        m_callback.OnError(apWriteClient, aError);
    }

    void OnDone(WriteClient *apWriteClient) override
    {
        m_callback.OnDone(apWriteClient);
        chip::Platform::Delete(apWriteClient);
        chip::Platform::Delete(this);
    }

private:
    WriteClient::Callback &m_callback;
};

static esp_err_t encode_attribute_value(uint8_t *encoded_buf, size_t encoded_buf_size, const EncodableToTLV &encodable,
                                        TLVReader &out_reader)
{
    TLVWriter writer;
    uint32_t encoded_len = 0;
    TLVReader reader;

    writer.Init(encoded_buf, encoded_buf_size);
    VerifyOrReturnError(encodable.EncodeTo(writer, chip::TLV::AnonymousTag()) == CHIP_NO_ERROR,
                    ESP_FAIL, ESP_LOGE(TAG, "Failed to encode attribute value"));
    VerifyOrReturnError(writer.Finalize() == CHIP_NO_ERROR,
                    ESP_FAIL, ESP_LOGE(TAG, "Failed to finalize TLV writer"));
    encoded_len = writer.GetLengthWritten();
    reader.Init(encoded_buf, encoded_len);
    VerifyOrReturnError(reader.Next() == CHIP_NO_ERROR,
                    ESP_FAIL, ESP_LOGE(TAG, "Failed to read next"));
    VerifyOrReturnError(reader.GetType() == chip::TLV::TLVType::kTLVType_Structure,
                    ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "The TLV type must be structure"));
    VerifyOrReturnError(reader.OpenContainer(out_reader) == CHIP_NO_ERROR,
                    ESP_FAIL, ESP_LOGE(TAG, "Failed to open container"));
    VerifyOrReturnError(out_reader.Next() == CHIP_NO_ERROR,
                    ESP_FAIL, ESP_LOGE(TAG, "Failed to read next"));
    return ESP_OK;
}

esp_err_t send_request(client::peer_device_t *remote_device, AttributePathParams &attr_path,
                       const chip::app::DataModel::EncodableToTLV &encodable, WriteClient::Callback &callback,
                       const chip::Optional<uint16_t> &timeout_ms)
{
    esp_err_t err = ESP_OK;
    VerifyOrReturnError(remote_device->GetSecureSession().HasValue() && !remote_device->GetSecureSession().Value()->IsGroupSession(),
                    ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Invalid Session Type"));
    VerifyOrReturnError(!attr_path.HasWildcardEndpointId(),
                        ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Endpoint Id Invalid"));

    ConcreteDataAttributePath path(attr_path.mEndpointId, attr_path.mClusterId, attr_path.mAttributeId);

    auto client_deleter_callback = chip::Platform::MakeUnique<client_deleter_write_callback>(callback);
    VerifyOrReturnError(client_deleter_callback, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "Failed to allocate memory for client deleter callback"));
    auto write_client = chip::Platform::MakeUnique<WriteClient>(remote_device->GetExchangeManager(),
                                                                client_deleter_callback.get(), timeout_ms, false);
    VerifyOrReturnError(write_client, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "Failed to allocate memory for WriteClient"));
    chip::Platform::ScopedMemoryBuffer<uint8_t> encoded_buf;
    encoded_buf.Alloc(k_encoded_buf_size);
    VerifyOrReturnError((encoded_buf.Get()), ESP_ERR_NO_MEM, ESP_LOGE(TAG, "Failed to alloc memory for encoded_buf"));
    TLVReader attr_val_reader;
    err = encode_attribute_value(encoded_buf.Get(), k_encoded_buf_size, encodable, attr_val_reader);
    VerifyOrReturnError(err == ESP_OK,
                    err, ESP_LOGE(TAG, "Failed to encode attribute value to a TLV reader"));
    VerifyOrReturnError(write_client->PutPreencodedAttribute(path, attr_val_reader) == CHIP_NO_ERROR,
                        ESP_FAIL, ESP_LOGE(TAG, "Failed to put pre-encoded attribute value to WriteClient"));
    VerifyOrReturnError(write_client->SendWriteRequest(remote_device->GetSecureSession().Value()) == CHIP_NO_ERROR,
                        ESP_FAIL, ESP_LOGE(TAG, "Failed to Send Write Request"));

    // Release the write_client and client deleter callback as it will be managed by the client deleter callback
    write_client.release();
    client_deleter_callback.release();
    return ESP_OK;

}

esp_err_t send_request(client::peer_device_t *remote_device, AttributePathParams &attr_path,
                       const char *attr_val_json_str, WriteClient::Callback &callback,
                       const chip::Optional<uint16_t> &timeout_ms)
{
    custom_encodable_type type(attr_val_json_str, custom_encodable_type::interaction_type::k_write_attr);
    return send_request(remote_device, attr_path, type, callback, timeout_ms);
}

esp_err_t send_request(client::peer_device_t *remote_device,
                       ScopedMemoryBufferWithSize<AttributePathParams> &attr_paths, multiple_write_encodable_type &json_encodable,
                       WriteClient::Callback &callback, const chip::Optional<uint16_t> &timeout_ms)
{
    VerifyOrReturnError(
        json_encodable.GetJsonArraySize() == attr_paths.AllocatedSize(), ESP_ERR_INVALID_ARG,
        ESP_LOGE(TAG, "The attr_values array length should be the same as the attr_paths array length"));
    VerifyOrReturnError(remote_device->GetSecureSession().HasValue() &&
                            !remote_device->GetSecureSession().Value()->IsGroupSession(),
                        ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Invalid Session Type"));
    auto client_deleter_callback = chip::Platform::MakeUnique<client_deleter_write_callback>(callback);
    VerifyOrReturnError(client_deleter_callback, ESP_ERR_NO_MEM,
                        ESP_LOGE(TAG, "Failed to allocate memory for client deleter callback"));
    auto write_client = chip::Platform::MakeUnique<WriteClient>(remote_device->GetExchangeManager(),
                                                                client_deleter_callback.get(), timeout_ms, false);
    VerifyOrReturnError(write_client, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "Failed to allocate memory for WriteClient"));

    for (size_t i = 0; i < attr_paths.AllocatedSize(); ++i) {
        ConcreteDataAttributePath path(attr_paths[i].mEndpointId, attr_paths[i].mClusterId, attr_paths[i].mAttributeId);
        chip::Platform::ScopedMemoryBuffer<uint8_t> encoded_buf;
        encoded_buf.Alloc(k_encoded_buf_size);
        VerifyOrReturnError((encoded_buf.Get()), ESP_ERR_NO_MEM,
                            ESP_LOGE(TAG, "Failed to alloc memory for encoded_buf"));
        TLVReader reader;
        TLVWriter writer;
        TLVReader attr_val_reader;
        writer.Init(encoded_buf.Get(), k_encoded_buf_size);
        VerifyOrReturnError(json_encodable.EncodeTo(writer, chip::TLV::AnonymousTag(), i) == CHIP_NO_ERROR, ESP_FAIL,
                            ESP_LOGE(TAG, "Failed to encode attribute value"));
        VerifyOrReturnError(writer.Finalize() == CHIP_NO_ERROR, ESP_FAIL,
                            ESP_LOGE(TAG, "Failed to finalize TLV writer"));
        reader.Init(encoded_buf.Get(), writer.GetLengthWritten());
        VerifyOrReturnError(reader.Next() == CHIP_NO_ERROR, ESP_FAIL, ESP_LOGE(TAG, "Failed to read next"));
        VerifyOrReturnError(reader.GetType() == chip::TLV::TLVType::kTLVType_Structure, ESP_ERR_INVALID_ARG,
                            ESP_LOGE(TAG, "The TLV type must be structure"));
        VerifyOrReturnError(reader.OpenContainer(attr_val_reader) == CHIP_NO_ERROR, ESP_FAIL,
                            ESP_LOGE(TAG, "Failed to open container"));
        VerifyOrReturnError(attr_val_reader.Next() == CHIP_NO_ERROR, ESP_FAIL, ESP_LOGE(TAG, "Failed to read next"));
        VerifyOrReturnError(write_client->PutPreencodedAttribute(path, attr_val_reader) == CHIP_NO_ERROR, ESP_FAIL,
                            ESP_LOGE(TAG, "Failed to put pre-encoded attribute value to WriteClient"));
    }

    VerifyOrReturnError(write_client->SendWriteRequest(remote_device->GetSecureSession().Value()) == CHIP_NO_ERROR,
                        ESP_FAIL, ESP_LOGE(TAG, "Failed to Send Write Request"));

    // Release the write_client and client deleter callback as it will be managed by the client deleter callback
    write_client.release();
    client_deleter_callback.release();
    return ESP_OK;
}

} // namespace write
} // namespace interaction
} // namespace client
} // namespace esp_matter
