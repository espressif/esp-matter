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

#include <app/clusters/bindings/BindingManager.h>
#include <app/ConcreteAttributePath.h>
#include <app/EventHeader.h>
#include <app/MessageDef/DataVersionFilterIBs.h>
#include <app/MessageDef/StatusIB.h>
#include <app/ReadClient.h>
#include <app/ReadPrepareParams.h>
#include <core/Optional.h>
#include <core/TLVReader.h>
#include <core/TLVWriter.h>

#if CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
#include <zap-generated/CHIPClusters.h>
#include "app/CASESessionManager.h"
#include "app/CommandSender.h"
#include "app/InteractionModelEngine.h"
#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

using namespace chip::app::Clusters;
using chip::BitMask;
using chip::DeviceProxy;
using chip::FabricInfo;
using chip::kInvalidEndpointId;
using chip::OperationalDeviceProxy;
using chip::OperationalSessionSetup;
using chip::ScopedNodeId;
using chip::Server;
using chip::SessionHandle;
using chip::Callback::Callback;
using chip::Messaging::ExchangeManager;

static const char *TAG = "esp_matter_client";

namespace esp_matter {
namespace client {

static command_callback_t client_command_callback = NULL;
static group_command_callback_t client_group_command_callback = NULL;
static void *command_callback_priv_data;
static bool initialize_binding_manager = false;

esp_err_t set_command_callback(command_callback_t callback, group_command_callback_t g_callback, void *priv_data)
{
    client_command_callback = callback;
    client_group_command_callback = g_callback;
    command_callback_priv_data = priv_data;
    return ESP_OK;
}

void esp_matter_connection_success_callback(void *context, ExchangeManager &exchangeMgr,
                                            const SessionHandle &sessionHandle)
{
    command_handle_t *cmd_handle = static_cast<command_handle_t *>(context);
    if (!cmd_handle) {
        ESP_LOGE(TAG, "Failed to call connect_success_callback since the command handle is NULL");
        return;
    }
    ESP_LOGI(TAG, "New connection success");
    // Only unicast binding needs to establish the connection
    if (client_command_callback) {
        OperationalDeviceProxy device(&exchangeMgr, sessionHandle);
        client_command_callback(&device, cmd_handle, command_callback_priv_data);
    }
    chip::Platform::Delete(cmd_handle);
}

void esp_matter_connection_failure_callback(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    command_handle_t *cmd_handle = static_cast<command_handle_t *>(context);
    ESP_LOGI(TAG, "New connection failure");
    if (cmd_handle) {
        chip::Platform::Delete(cmd_handle);
    }
}

esp_err_t connect(case_session_mgr_t *case_session_mgr, uint8_t fabric_index, uint64_t node_id,
                  command_handle_t *cmd_handle)
{
    if (!case_session_mgr) {
        return ESP_ERR_INVALID_ARG;
    }
    static Callback<chip::OnDeviceConnected> success_callback(esp_matter_connection_success_callback, NULL);
    static Callback<chip::OnDeviceConnectionFailure> failure_callback(esp_matter_connection_failure_callback, NULL);

    command_handle_t *context = chip::Platform::New<command_handle_t>(cmd_handle);
    if (!context) {
        ESP_LOGE(TAG, "failed to alloc memory for the command handle");
        return ESP_ERR_NO_MEM;
    }
    success_callback.mContext = static_cast<void *>(context);
    failure_callback.mContext = static_cast<void *>(context);
    case_session_mgr->FindOrEstablishSession(ScopedNodeId(node_id, fabric_index), &success_callback,
                                             &failure_callback);
    return ESP_OK;
}

esp_err_t group_command_send(uint8_t fabric_index, command_handle_t *cmd_handle)
{
    if (!cmd_handle) {
        ESP_LOGE(TAG, "command handle is null");
        return ESP_ERR_NO_MEM;
    }

    if (client_group_command_callback) {
        client_group_command_callback(fabric_index, cmd_handle, command_callback_priv_data);
    }

    return ESP_OK;
}

static void esp_matter_command_client_binding_callback(const EmberBindingTableEntry &binding,
                                                       OperationalDeviceProxy *peer_device, void *context)
{
    command_handle_t *cmd_handle = static_cast<command_handle_t *>(context);
    if (!cmd_handle) {
        ESP_LOGE(TAG, "Failed to call the binding callback since command handle is NULL");
        return;
    }
    if (binding.type == MATTER_UNICAST_BINDING && peer_device) {
        if (client_command_callback) {
            cmd_handle->endpoint_id = binding.remote;
            client_command_callback(peer_device, cmd_handle, command_callback_priv_data);
        }
    } else if (binding.type == MATTER_MULTICAST_BINDING && !peer_device) {
        if (client_group_command_callback) {
            cmd_handle->group_id = binding.groupId;
            client_group_command_callback(binding.fabricIndex, cmd_handle, command_callback_priv_data);
        }
    }
}

static void esp_matter_binding_context_release(void *context)
{
    if (context) {
        chip::Platform::Delete(static_cast<command_handle_t *>(context));
    }
}

esp_err_t cluster_update(uint16_t local_endpoint_id, command_handle_t *cmd_handle)
{
    command_handle_t *context = chip::Platform::New<command_handle_t>(cmd_handle);
    if (!context) {
        ESP_LOGE(TAG, "failed to alloc memory for the command handle");
        return ESP_ERR_NO_MEM;
    }
    if (CHIP_NO_ERROR !=
        chip::BindingManager::GetInstance().NotifyBoundClusterChanged(local_endpoint_id, cmd_handle->cluster_id,
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
} // namespace client

#if CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
namespace cluster {
using client::peer_device_t;

static void send_command_success_callback(void *context, const chip::app::DataModel::NullObjectType &data)
{
    ESP_LOGI(TAG, "Send command success");
}

static void send_command_failure_callback(void *context, CHIP_ERROR error)
{
    ESP_LOGI(TAG, "Send command failure: err: %" CHIP_ERROR_FORMAT, error.Format());
}

namespace custom {
namespace command {

using command_data_tag = chip::app::CommandDataIB::Tag;
using chip::TLV::ContextTag;
using chip::TLV::TLVWriter;

esp_err_t send_command(void *ctx, peer_device_t *remote_device, const CommandPathParams &command_path,
                       const char *command_data_json_str, custom_command_callback::on_success_callback_t on_success,
                       custom_command_callback::on_error_callback_t on_error,
                       const Optional<uint16_t> &timed_invoke_timeout_ms, const Optional<Timeout> &response_timeout)
{
    if (!remote_device->GetSecureSession().HasValue() || remote_device->GetSecureSession().Value()->IsGroupSession()) {
        ESP_LOGE(TAG, "Invalid Session Type");
        return ESP_ERR_INVALID_ARG;
    }
    if (command_path.mFlags.Has(chip::app::CommandPathFlags::kGroupIdValid)) {
        ESP_LOGE(TAG, "Invalid CommandPathFlags");
        return ESP_ERR_INVALID_ARG;
    }
    auto decoder = chip::Platform::MakeUnique<custom_command_callback>(ctx, on_success, on_error);
    if (decoder == nullptr) {
        ESP_LOGE(TAG, "No memory for command callback");
        return ESP_ERR_NO_MEM;
    }
    auto on_done = [raw_decoder_ptr = decoder.get()](void *context, CommandSender *command_sender) {
        chip::Platform::Delete(command_sender);
        chip::Platform::Delete(raw_decoder_ptr);
    };
    decoder->set_on_done_callback(on_done);

    auto command_sender = chip::Platform::MakeUnique<CommandSender>(decoder.get(), remote_device->GetExchangeManager(),
                                                                    timed_invoke_timeout_ms.HasValue());
    if (command_sender == nullptr) {
        ESP_LOGE(TAG, "No memory for command sender");
        return ESP_ERR_NO_MEM;
    }
    chip::app::CommandSender::PrepareCommandParameters prepare_command_params;
    if (command_sender->PrepareCommand(command_path, prepare_command_params) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to prepare command");
        return ESP_FAIL;
    }
    TLVWriter *writer = command_sender->GetCommandDataIBTLVWriter();
    if (writer == nullptr) {
        ESP_LOGE(TAG, "No TLV writer in command sender");
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t err = json_to_tlv(command_data_json_str, *writer, ContextTag(command_data_tag::kFields));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to convert json string to TLV");
        return err;
    }
    chip::app::CommandSender::FinishCommandParameters finish_command_params(timed_invoke_timeout_ms);
    if (command_sender->FinishCommand(finish_command_params) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to finish command");
        return ESP_FAIL;
    }
    if (command_sender->SendCommandRequest(remote_device->GetSecureSession().Value(), response_timeout) !=
        CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to send command request");
        return ESP_FAIL;
    }
    (void)decoder.release();
    (void)command_sender.release();
    return ESP_OK;
}

esp_err_t send_group_command(const uint8_t fabric_index, const CommandPathParams &command_path,
                             const char *command_data_json_str)
{
    if (!command_path.mFlags.Has(chip::app::CommandPathFlags::kGroupIdValid)) {
        ESP_LOGE(TAG, "Invalid CommandPathFlags");
        return ESP_ERR_INVALID_ARG;
    }
    chip::Transport::OutgoingGroupSession session(command_path.mGroupId, fabric_index);
    chip::Messaging::ExchangeManager *exchange_mgr = chip::app::InteractionModelEngine::GetInstance()->GetExchangeManager();
    auto command_sender = chip::Platform::MakeUnique<chip::app::CommandSender>(nullptr, exchange_mgr);
    if (command_sender == nullptr) {
        ESP_LOGE(TAG, "No memory for command sender");
        return ESP_ERR_NO_MEM;
    }
    chip::app::CommandSender::PrepareCommandParameters prepare_command_params;
    if (command_sender->PrepareCommand(command_path, prepare_command_params) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to prepare command");
        return ESP_FAIL;
    }
    TLVWriter *writer = command_sender->GetCommandDataIBTLVWriter();
    if (writer == nullptr) {
        ESP_LOGE(TAG, "No TLV writer in command sender");
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t err = json_to_tlv(command_data_json_str, *writer, ContextTag(command_data_tag::kFields));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to convert json string to TLV");
        return err;
    }
    chip::app::CommandSender::FinishCommandParameters finish_command_params;
    if (command_sender->FinishCommand(finish_command_params) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to finish command");
        return ESP_FAIL;
    }
    if (command_sender->SendGroupCommandRequest(SessionHandle(session)) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to send command request");
        return ESP_FAIL;
    }
    return ESP_OK;
}
} // namespace command
} // namespace custom

namespace on_off {
namespace command {

esp_err_t send_on(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    OnOff::Commands::On::Type command_data;

    chip::Controller::OnOffCluster cluster(*remote_device->GetExchangeManager(),
                                           remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_on(uint8_t fabric_index, uint16_t group_id)
{
    OnOff::Commands::On::Type command_data;
    chip::Messaging::ExchangeManager *exchange_mgr = chip::app::InteractionModelEngine::GetInstance()->GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_off(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    OnOff::Commands::Off::Type command_data;

    chip::Controller::OnOffCluster cluster(*remote_device->GetExchangeManager(),
                                           remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_off(uint8_t fabric_index, uint16_t group_id)
{
    OnOff::Commands::Off::Type command_data;
    chip::Messaging::ExchangeManager *exchange_mgr = chip::app::InteractionModelEngine::GetInstance()->GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_toggle(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    OnOff::Commands::Toggle::Type command_data;

    chip::Controller::OnOffCluster cluster(*remote_device->GetExchangeManager(),
                                           remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_toggle(uint8_t fabric_index, uint16_t group_id)
{
    OnOff::Commands::Toggle::Type command_data;
    chip::Messaging::ExchangeManager *exchange_mgr = chip::app::InteractionModelEngine::GetInstance()->GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

} // namespace command
} // namespace on_off

namespace identify {
namespace command {
esp_err_t send_identify(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t identify_time)
{
    Identify::Commands::Identify::Type command_data;
    command_data.identifyTime = identify_time;

    chip::Controller::IdentifyCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_identify(uint8_t fabric_index, uint16_t group_id, uint16_t identify_time)
{
    Identify::Commands::Identify::Type command_data;
    command_data.identifyTime = identify_time;

    chip::Messaging::ExchangeManager *exchange_mgr = chip::app::InteractionModelEngine::GetInstance()->GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_trigger_effect(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t effect_identifier,
                              uint8_t effect_variant)
{
    Identify::Commands::TriggerEffect::Type command_data;
    command_data.effectIdentifier = Identify::EffectIdentifierEnum(effect_identifier);
    command_data.effectVariant = Identify::EffectVariantEnum(effect_variant);

    chip::Controller::IdentifyCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

} // namespace command
} // namespace identify

} // namespace cluster

namespace interaction {

using chip::app::ConcreteDataAttributePath;
using chip::app::EventHeader;
using chip::TLV::TLVReader;
using chip::TLV::TLVWriter;
using chip::app::StatusIB;
using chip::SubscriptionId;
using chip::app::DataVersionFilterIBs::Builder;
using chip::app::ReadPrepareParams;

class client_deleter_read_callback : public ReadClient::Callback
{
public:
    client_deleter_read_callback(ReadClient::Callback & callback) : m_callback(callback) {}
private:
    void OnReportBegin() override { m_callback.OnReportBegin(); }
    void OnReportEnd() override { m_callback.OnReportEnd(); }
    void OnError(CHIP_ERROR aError) override { m_callback.OnError(aError); }

    void OnAttributeData(const ConcreteDataAttributePath & aPath, TLVReader * apData, const StatusIB & aStatus) override
    {
        m_callback.OnAttributeData(aPath, apData, aStatus);
    }

    void OnEventData(const EventHeader & aEventHeader, TLVReader * apData, const StatusIB * apStatus) override
    {
        m_callback.OnEventData(aEventHeader, apData, apStatus);
    }

    void OnDone(ReadClient * apReadClient) override
    {
        m_callback.OnDone(apReadClient);
        chip::Platform::Delete(apReadClient);
        chip::Platform::Delete(this);
    }

    void OnSubscriptionEstablished(SubscriptionId aSubscriptionId) override
    {
        m_callback.OnSubscriptionEstablished(aSubscriptionId);
    }

    CHIP_ERROR OnResubscriptionNeeded(ReadClient * apReadClient, CHIP_ERROR aTerminationCause) override
    {
        return m_callback.OnResubscriptionNeeded(apReadClient, aTerminationCause);
    }

    void OnDeallocatePaths(chip::app::ReadPrepareParams && aReadPrepareParams) override
    {
        m_callback.OnDeallocatePaths(std::move(aReadPrepareParams));
    }

    CHIP_ERROR OnUpdateDataVersionFilterList(Builder & aDataVersionFilterIBsBuilder,
                                             const chip::Span<AttributePathParams> & aAttributePaths,
                                             bool & aEncodedDataVersionList) override
    {
        return m_callback.OnUpdateDataVersionFilterList(aDataVersionFilterIBsBuilder, aAttributePaths, aEncodedDataVersionList);
    }

    CHIP_ERROR GetHighestReceivedEventNumber(chip::Optional<chip::EventNumber> & aEventNumber) override
    {
        return m_callback.GetHighestReceivedEventNumber(aEventNumber);
    }

    void OnUnsolicitedMessageFromPublisher(ReadClient * apReadClient) override
    {
        m_callback.OnUnsolicitedMessageFromPublisher(apReadClient);
    }

    void OnCASESessionEstablished(const SessionHandle & aSession, ReadPrepareParams & aSubscriptionParams) override
    {
        m_callback.OnCASESessionEstablished(aSession, aSubscriptionParams);
    }

    ReadClient::Callback & m_callback;
};

esp_err_t send_read_command(client::peer_device_t *remote_device, AttributePathParams *attr_path, size_t attr_path_size,
                            EventPathParams *event_path, size_t event_path_size, ReadClient::Callback &callback)
{
    if (!remote_device->GetSecureSession().HasValue() || remote_device->GetSecureSession().Value()->IsGroupSession()) {
        ESP_LOGE(TAG, "Invalid Session Type");
        return ESP_ERR_INVALID_ARG;
    }
    if ((!attr_path || attr_path_size == 0) && (!event_path || event_path_size == 0)) {
        ESP_LOGE(TAG, "Invalid attribute path and event path");
        return ESP_ERR_INVALID_ARG;
    }
    ReadPrepareParams params(remote_device->GetSecureSession().Value());
    params.mpAttributePathParamsList = attr_path;
    params.mAttributePathParamsListSize = attr_path_size;
    params.mpEventPathParamsList = event_path;
    params.mEventPathParamsListSize = event_path_size;
    params.mpDataVersionFilterList = nullptr;
    params.mDataVersionFilterListSize = 0;
    params.mIsFabricFiltered = false;

    auto client_deleter_callback = chip::Platform::MakeUnique<client_deleter_read_callback>(callback);
    if (!client_deleter_callback) {
        ESP_LOGE(TAG, "Failed to allocate memory for client deleter callback");
        return ESP_ERR_NO_MEM;
    }
    auto client = chip::Platform::MakeUnique<ReadClient>(chip::app::InteractionModelEngine::GetInstance(),
                                                         remote_device->GetExchangeManager(), *client_deleter_callback,
                                                         ReadClient::InteractionType::Read);
    if (!client) {
        ESP_LOGE(TAG, "Failed to allocate memory for ReadClient");
        return ESP_ERR_NO_MEM;
    }

    if (client->SendRequest(params) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to send read request");
        return ESP_FAIL;
    }
    // The memory will be released when OnDone() is called
    client.release();
    client_deleter_callback.release();
    return ESP_OK;
}

esp_err_t send_subscribe_command(client::peer_device_t *remote_device, AttributePathParams *attr_path,
                                 size_t attr_path_size, EventPathParams *event_path, size_t event_path_size,
                                 uint16_t min_interval, uint16_t max_interval, bool keep_subscription,
                                 bool auto_resubscribe, ReadClient::Callback &callback)
{
    if (!remote_device->GetSecureSession().HasValue() || remote_device->GetSecureSession().Value()->IsGroupSession()) {
        ESP_LOGE(TAG, "Invalid Session Type");
        return ESP_ERR_INVALID_ARG;
    }
    if ((!attr_path || attr_path_size == 0) && (!event_path || event_path_size == 0)) {
        ESP_LOGE(TAG, "Invalid attribute path and event path");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!client_deleter_callback) {
        ESP_LOGE(TAG, "Failed to allocate memory for client deleter callback");
        return ESP_ERR_NO_MEM;
    }
    auto client = chip::Platform::MakeUnique<ReadClient>(chip::app::InteractionModelEngine::GetInstance(),
                                                         remote_device->GetExchangeManager(), *client_deleter_callback,
                                                         ReadClient::InteractionType::Subscribe);
    if (!client) {
        ESP_LOGE(TAG, "Failed to allocate memory for ReadClient");
        return ESP_ERR_NO_MEM;
    }

    CHIP_ERROR err = CHIP_NO_ERROR;
    if (auto_resubscribe) {
        err = client->SendAutoResubscribeRequest(std::move(params));
    } else {
        err = client->SendRequest(params);
    }
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to send subcribe request");
        return ESP_FAIL;
    }
    // The memory will be released when OnDone() is called
    client.release();
    client_deleter_callback.release();
    return ESP_OK;
}

static constexpr size_t k_encoded_buf_size = chip::kMaxAppMessageLen;

class client_deleter_write_callback : public WriteClient::Callback
{
public:
    client_deleter_write_callback(WriteClient::Callback &callback) : m_callback(callback) {}
    void OnResponse(const WriteClient * apWriteClient, const ConcreteDataAttributePath & aPath,
                    StatusIB attributeStatus) override
    {
        m_callback.OnResponse(apWriteClient, aPath, attributeStatus);
    }

    void OnError(const WriteClient * apWriteClient, CHIP_ERROR aError) override
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
    WriteClient::Callback & m_callback;
};

static esp_err_t encode_attribute_value(uint8_t *encoded_buf, size_t encoded_buf_size, const char *attr_val_json_str,
                                        TLVReader &out_reader)
{
    TLVWriter writer;
    uint32_t encoded_len = 0;
    TLVReader reader;
    esp_err_t err = ESP_OK;

    writer.Init(encoded_buf, encoded_buf_size);
    if ((err = json_to_tlv(attr_val_json_str, writer, chip::TLV::AnonymousTag())) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to parse attribute value");
        return err;
    }
    if (writer.Finalize() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to finalize tlv writer");
        return ESP_FAIL;
    }
    encoded_len = writer.GetLengthWritten();
    reader.Init(encoded_buf, encoded_len);
    if (reader.Next() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to read next");
        return ESP_FAIL;
    }
    if (reader.GetType() != chip::TLV::TLVType::kTLVType_Structure) {
        ESP_LOGE(TAG, "The TLV type must be structure");
        return ESP_ERR_INVALID_ARG;
    }
    if (reader.OpenContainer(out_reader) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to open container");
        return ESP_FAIL;
    }
    if (out_reader.Next() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to read next");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t send_write_command(client::peer_device_t *remote_device, AttributePathParams &attr_path,
                             const char *attr_val_json_str, WriteClient::Callback &callback,
                             const chip::Optional<uint16_t> &timeout_ms)
{
    esp_err_t err = ESP_OK;
    if (!remote_device->GetSecureSession().HasValue() || remote_device->GetSecureSession().Value()->IsGroupSession()) {
        ESP_LOGE(TAG, "Invalid Session Type");
        return ESP_ERR_INVALID_ARG;
    }
    if (attr_path.HasWildcardEndpointId()) {
        ESP_LOGE(TAG, "Endpoint Id Invalid");
        return ESP_ERR_INVALID_ARG;
    }

    ConcreteDataAttributePath path(attr_path.mEndpointId, attr_path.mClusterId, attr_path.mAttributeId);

    auto client_deleter_callback = chip::Platform::MakeUnique<client_deleter_write_callback>(callback);
    if (!client_deleter_callback) {
        ESP_LOGE(TAG, "Failed to allocate memory for client deleter callback");
        return ESP_ERR_NO_MEM;
    }
    auto write_client = chip::Platform::MakeUnique<WriteClient>(remote_device->GetExchangeManager(),
                                                                client_deleter_callback.get(), timeout_ms, false);
    if (!write_client) {
        ESP_LOGE(TAG, "Failed to allocate memory for WriteClient");
        return ESP_ERR_NO_MEM;
    }
    chip::Platform::ScopedMemoryBuffer<uint8_t> encoded_buf;
    encoded_buf.Alloc(k_encoded_buf_size);
    if (!encoded_buf.Get()) {
        ESP_LOGE(TAG, "Failed to alloc memory for encoded_buf");
        return ESP_ERR_NO_MEM;
    }
    TLVReader attr_val_reader;
    err = encode_attribute_value(encoded_buf.Get(), k_encoded_buf_size, attr_val_json_str, attr_val_reader);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to encode attribute value to a TLV reader");
        return err;
    }
    if (write_client->PutPreencodedAttribute(path, attr_val_reader) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to put pre-encoded attribute value to WriteClient");
        return ESP_FAIL;
    }
    if (write_client->SendWriteRequest(remote_device->GetSecureSession().Value()) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to Send Write Request");
        return ESP_FAIL;
    }
    // Release the write_client and client deleter callback as it will be managed by the client deleter callback
    write_client.release();
    client_deleter_callback.release();
    return ESP_OK;
}

} // namespace interaction
#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

} // namespace esp_matter
