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

#include <app/server/Server.h>
#include <controller/CommissioneeDeviceProxy.h>
#include <esp_log.h>
#include <esp_matter_client.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_subscribe_command.h>

#include <commands/clusters/DataModelLogger.h>

using namespace chip::app::Clusters;
using namespace esp_matter::client;
using chip::DeviceProxy;
using chip::app::InteractionModelEngine;
using chip::app::ReadClient;
using chip::app::ReadPrepareParams;

static const char *TAG = "read_command";

static const uint8_t k_max_resubscribe_retries = 2;

namespace esp_matter {
namespace controller {

void subscribe_command::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                                const SessionHandle &sessionHandle)
{
    subscribe_command *cmd = (subscribe_command *)context;
    chip::OperationalDeviceProxy device_proxy(&exchangeMgr, sessionHandle);
    esp_err_t err = interaction::subscribe::send_request(
        &device_proxy, cmd->m_attr_paths.Get(), cmd->m_attr_paths.AllocatedSize(), cmd->m_event_paths.Get(),
        cmd->m_event_paths.AllocatedSize(), cmd->m_min_interval, cmd->m_max_interval, cmd->m_keep_subscription,
        cmd->m_auto_resubscribe, cmd->m_buffered_read_cb);
    if (err != ESP_OK) {
        chip::Platform::Delete(cmd);
    }
    return;
}

void subscribe_command::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    subscribe_command *cmd = (subscribe_command *)context;

    if (cmd->subscribe_failure_cb)
        cmd->subscribe_failure_cb((void *)cmd);

    chip::Platform::Delete(cmd);
    return;
}

esp_err_t subscribe_command::send_command()
{
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::Server *server = &(chip::Server::GetInstance());
    server->GetCASESessionManager()->FindOrEstablishSession(ScopedNodeId(m_node_id, get_fabric_index()),
                                                            &on_device_connected_cb, &on_device_connection_failure_cb);
    return ESP_OK;
#else
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    if (CHIP_NO_ERROR ==
        controller_instance.get_commissioner()->GetConnectedDevice(m_node_id, &on_device_connected_cb,
                                                                   &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#else
    if (CHIP_NO_ERROR ==
        controller_instance.get_controller()->GetConnectedDevice(m_node_id, &on_device_connected_cb,
                                                                 &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::Platform::Delete(this);
    return ESP_FAIL;
}

// ReadClient Callback Interface
void subscribe_command::OnAttributeData(const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *data,
                                        const chip::app::StatusIB &status)
{
    CHIP_ERROR error = status.ToChipError();
    if (CHIP_NO_ERROR != error) {
        ESP_LOGE(TAG, "Response Failure: %s", chip::ErrorStr(error));
        return;
    }
    if (data == nullptr) {
        ESP_LOGE(TAG, "Response Failure: No Data");
        return;
    }

    chip::TLV::TLVReader log_data;
    log_data.Init(*data);
    error = DataModelLogger::LogAttribute(path, &log_data);
    if (CHIP_NO_ERROR != error) {
        ESP_LOGE(TAG, "Response Failure: Can not decode Data");
    }

    if (attribute_data_cb) {
        attribute_data_cb(m_node_id, path, data);
    }
}

void subscribe_command::OnEventData(const chip::app::EventHeader &event_header, chip::TLV::TLVReader *data,
                                    const chip::app::StatusIB *status)
{
    CHIP_ERROR error = CHIP_NO_ERROR;
    if (status != nullptr) {
        error = status->ToChipError();
        if (CHIP_NO_ERROR != error) {
            ESP_LOGE(TAG, "Response Failure: %s", chip::ErrorStr(error));
            return;
        }
    }
    if (data == nullptr) {
        ESP_LOGE(TAG, "Response Failure: No Data");
        return;
    }

    chip::TLV::TLVReader log_data;
    log_data.Init(*data);
    error = DataModelLogger::LogEvent(event_header, &log_data);
    if (CHIP_NO_ERROR != error) {
        ESP_LOGE(TAG, "Response Failure: Can not decode Data");
    }

    if (event_data_cb) {
        event_data_cb(m_node_id, event_header, data);
    }
}

void subscribe_command::OnError(CHIP_ERROR error)
{
    ESP_LOGE(TAG, "Subscribe Error: %s", chip::ErrorStr(error));
}

void subscribe_command::OnDeallocatePaths(chip::app::ReadPrepareParams &&aReadPrepareParams)
{
    // Intentionally empty because the AttributePathParamsList or EventPathParamsList will be deleted with the
    // subscribe_command.
}

void subscribe_command::OnSubscriptionEstablished(chip::SubscriptionId subscriptionId)
{
    m_subscription_id = subscriptionId;
    m_resubscribe_retries = 0;
    ESP_LOGI(TAG, "Subscription 0x%" PRIx32 " established", subscriptionId);
}

CHIP_ERROR subscribe_command::OnResubscriptionNeeded(ReadClient *apReadClient, CHIP_ERROR aTerminationCause)
{
    m_resubscribe_retries++;
    if (m_resubscribe_retries > k_max_resubscribe_retries) {
        ESP_LOGE(TAG, "Could not find the devices in %d retries, terminate the subscription",
                 k_max_resubscribe_retries);
        return aTerminationCause;
    }
    return apReadClient->DefaultResubscribePolicy(aTerminationCause);
}

void subscribe_command::OnDone(ReadClient *apReadClient)
{
    ESP_LOGI(TAG, "Subscription 0x%" PRIx32 " Done for remote node 0x%" PRIx64, m_subscription_id, m_node_id);
    if (subscribe_done_cb) {
        // This will be called when the subscription is terminated.
        subscribe_done_cb(m_node_id, m_subscription_id);
    }
    chip::Platform::Delete(this);
}

esp_err_t send_subscribe_attr_command(uint64_t node_id, ScopedMemoryBufferWithSize<uint16_t> &endpoint_ids,
                                      ScopedMemoryBufferWithSize<uint32_t> &cluster_ids,
                                      ScopedMemoryBufferWithSize<uint32_t> &attribute_ids, uint16_t min_interval,
                                      uint16_t max_interval, bool auto_resubscribe, bool keep_subscription)
{
    if (endpoint_ids.AllocatedSize() != cluster_ids.AllocatedSize() ||
        endpoint_ids.AllocatedSize() != attribute_ids.AllocatedSize()) {
        ESP_LOGE(TAG,
                 "The endpoint_id array length should be the same as the cluster_ids array length and the "
                 "attribute_ids array length");
        return ESP_ERR_INVALID_ARG;
    }
    ScopedMemoryBufferWithSize<AttributePathParams> attr_paths;
    ScopedMemoryBufferWithSize<EventPathParams> event_paths;
    attr_paths.Alloc(endpoint_ids.AllocatedSize());
    if (!attr_paths.Get()) {
        ESP_LOGE(TAG, "Failed to alloc memory for attribute paths");
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < attr_paths.AllocatedSize(); ++i) {
        attr_paths[i] = AttributePathParams(endpoint_ids[i], cluster_ids[i], attribute_ids[i]);
    }

    subscribe_command *cmd = chip::Platform::New<subscribe_command>(
        node_id, std::move(attr_paths), std::move(event_paths), min_interval, max_interval, auto_resubscribe, nullptr,
        nullptr, nullptr, nullptr, keep_subscription);
    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for subscribe_command");
        return ESP_ERR_NO_MEM;
    }
    return cmd->send_command();
}

esp_err_t send_subscribe_event_command(uint64_t node_id, ScopedMemoryBufferWithSize<uint16_t> &endpoint_ids,
                                       ScopedMemoryBufferWithSize<uint32_t> &cluster_ids,
                                       ScopedMemoryBufferWithSize<uint32_t> &event_ids, uint16_t min_interval,
                                       uint16_t max_interval, bool auto_resubscribe, bool keep_subscription)
{
    if (endpoint_ids.AllocatedSize() != cluster_ids.AllocatedSize() ||
        endpoint_ids.AllocatedSize() != event_ids.AllocatedSize()) {
        ESP_LOGE(TAG,
                 "The endpoint_id array length should be the same as the cluster_ids array length and the "
                 "attribute_ids array length");
        return ESP_ERR_INVALID_ARG;
    }
    ScopedMemoryBufferWithSize<AttributePathParams> attr_paths;
    ScopedMemoryBufferWithSize<EventPathParams> event_paths;
    event_paths.Alloc(endpoint_ids.AllocatedSize());
    if (!event_paths.Get()) {
        ESP_LOGE(TAG, "Failed to alloc memory for attribute paths");
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < event_paths.AllocatedSize(); ++i) {
        event_paths[i] = EventPathParams(endpoint_ids[i], cluster_ids[i], event_ids[i]);
    }

    subscribe_command *cmd = chip::Platform::New<subscribe_command>(
        node_id, std::move(attr_paths), std::move(event_paths), min_interval, max_interval, auto_resubscribe, nullptr,
        nullptr, nullptr, nullptr, keep_subscription);
    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for subscribe_command");
        return ESP_ERR_NO_MEM;
    }
    return cmd->send_command();
}

esp_err_t send_subscribe_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, uint16_t min_interval, uint16_t max_interval,
                                      bool auto_resubscribe, bool keep_subscription)
{
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> attribute_ids;
    endpoint_ids.Alloc(1);
    cluster_ids.Alloc(1);
    attribute_ids.Alloc(1);
    if (!(endpoint_ids.Get() && cluster_ids.Get() && attribute_ids.Get())) {
        return ESP_ERR_NO_MEM;
    }
    return send_subscribe_attr_command(node_id, endpoint_ids, cluster_ids, attribute_ids, min_interval, max_interval,
                                       auto_resubscribe, keep_subscription);
}

esp_err_t send_subscribe_event_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t event_id,
                                       uint16_t min_interval, uint16_t max_interval, bool auto_resubscribe,
                                       bool keep_subscription)
{
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> event_ids;
    endpoint_ids.Alloc(1);
    cluster_ids.Alloc(1);
    event_ids.Alloc(1);
    if (!(endpoint_ids.Get() && cluster_ids.Get() && event_ids.Get())) {
        return ESP_ERR_NO_MEM;
    }
    return send_subscribe_event_command(node_id, endpoint_ids, cluster_ids, event_ids, min_interval, max_interval,
                                        auto_resubscribe, keep_subscription);
}

esp_err_t send_shutdown_subscription(uint64_t node_id, uint32_t subscription_id)
{
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::FabricIndex fabric_index = get_fabric_index();
#else
    chip::FabricIndex fabric_index = matter_controller_client::get_instance().get_fabric_index();
#endif
    if (CHIP_NO_ERROR !=
        InteractionModelEngine::GetInstance()->ShutdownSubscription(ScopedNodeId(node_id, fabric_index),
                                                                    subscription_id)) {
        ESP_LOGE(TAG, "Shutdown Subscription Failed");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void send_shutdown_subscriptions(uint64_t node_id)
{
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::FabricIndex fabric_index = get_fabric_index();
#else
    chip::FabricIndex fabric_index = matter_controller_client::get_instance().get_fabric_index();
#endif

    InteractionModelEngine::GetInstance()->ShutdownSubscriptions(fabric_index, node_id);
    ESP_LOGI(TAG, "Shutdown Subscriptions for node:0x%llx", node_id);
}

void send_shutdown_all_subscriptions()
{
    InteractionModelEngine::GetInstance()->ShutdownAllSubscriptions();
    ESP_LOGI(TAG, "Shutdown all Subscriptions");
}

} // namespace controller
} // namespace esp_matter
