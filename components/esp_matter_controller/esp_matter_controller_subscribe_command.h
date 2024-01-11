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

#include <app/BufferedReadCallback.h>
#include <controller/CommissioneeDeviceProxy.h>
#include <esp_matter.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_mem.h>

namespace esp_matter {
namespace controller {

using chip::ScopedNodeId;
using chip::SessionHandle;
using chip::app::AttributePathParams;
using chip::app::BufferedReadCallback;
using chip::app::EventPathParams;
using chip::app::ReadClient;
using chip::Messaging::ExchangeManager;
using chip::Platform::ScopedMemoryBufferWithSize;
using esp_matter::client::peer_device_t;

typedef enum {
    SUBSCRIBE_ATTRIBUTE = 0,
    SUBSCRIBE_EVENT,
} subscribe_command_type_t;

class subscribe_command : public ReadClient::Callback {
public:
    subscribe_command(uint64_t node_id, ScopedMemoryBufferWithSize<AttributePathParams> &&attr_paths,
                      ScopedMemoryBufferWithSize<EventPathParams> &&event_paths, uint16_t min_interval,
                      uint16_t max_interval, bool auto_resubscribe = true, attribute_report_cb_t attribute_cb = nullptr,
                      event_report_cb_t event_cb = nullptr, subscribe_done_cb_t done_cb = nullptr,
                      subscribe_failure_cb_t connect_failure_cb = nullptr)
        : m_node_id(node_id)
        , m_min_interval(min_interval)
        , m_max_interval(max_interval)
        , m_auto_resubscribe(auto_resubscribe)
        , m_buffered_read_cb(*this)
        , m_attr_paths(std::move(attr_paths))
        , m_event_paths(std::move(event_paths))
        , on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
        , attribute_data_cb(attribute_cb)
        , event_data_cb(event_cb)
        , subscribe_done_cb(done_cb)
        , subscribe_failure_cb(connect_failure_cb)
    {
    }

    subscribe_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_or_event_id,
                      subscribe_command_type_t command_type, uint16_t min_interval, uint16_t max_interval,
                      bool auto_resubscribe = true, attribute_report_cb_t attribute_cb = nullptr,
                      event_report_cb_t event_cb = nullptr, subscribe_done_cb_t done_cb = nullptr,
                      subscribe_failure_cb_t connect_failure_cb = nullptr)
        : m_node_id(node_id)
        , m_min_interval(min_interval)
        , m_max_interval(max_interval)
        , m_auto_resubscribe(auto_resubscribe)
        , m_buffered_read_cb(*this)
        , on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
        , attribute_data_cb(attribute_cb)
        , event_data_cb(event_cb)
        , subscribe_done_cb(done_cb)
        , subscribe_failure_cb(connect_failure_cb)
    {
        if (command_type == SUBSCRIBE_ATTRIBUTE) {
            m_attr_paths.Alloc(1);
            if (m_attr_paths.Get()) {
                m_attr_paths[0] = AttributePathParams(endpoint_id, cluster_id, attribute_or_event_id);
            }
        } else if (command_type == SUBSCRIBE_EVENT) {
            m_event_paths.Alloc(1);
            if (m_event_paths.Get()) {
                m_event_paths[0] = EventPathParams(endpoint_id, cluster_id, attribute_or_event_id);
            }
        }
    }

    ~subscribe_command() {}

    esp_err_t send_command();

    // ReadClient Callback Interface
    void OnAttributeData(const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *data,
                         const chip::app::StatusIB &status) override;

    void OnEventData(const chip::app::EventHeader &event_header, chip::TLV::TLVReader *data,
                     const chip::app::StatusIB *status) override;

    void OnError(CHIP_ERROR error) override;

    void OnDeallocatePaths(chip::app::ReadPrepareParams &&aReadPrepareParams) override;

    void OnDone(ReadClient *apReadClient) override;

    void OnSubscriptionEstablished(chip::SubscriptionId subscriptionId) override;

    CHIP_ERROR OnResubscriptionNeeded(ReadClient *apReadClient, CHIP_ERROR aTerminationCause) override;

private:
    uint64_t m_node_id;
    uint16_t m_min_interval;
    uint16_t m_max_interval;
    bool m_auto_resubscribe;
    BufferedReadCallback m_buffered_read_cb;
    uint32_t m_subscription_id = 0;
    uint8_t m_resubscribe_retries = 0;
    ScopedMemoryBufferWithSize<AttributePathParams> m_attr_paths;
    ScopedMemoryBufferWithSize<EventPathParams> m_event_paths;

    static void on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                        const SessionHandle &sessionHandle);
    static void on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error);

    chip::Callback::Callback<chip::OnDeviceConnected> on_device_connected_cb;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> on_device_connection_failure_cb;
    attribute_report_cb_t attribute_data_cb;
    event_report_cb_t event_data_cb;
    subscribe_done_cb_t subscribe_done_cb;
    subscribe_failure_cb_t subscribe_failure_cb;
};

esp_err_t send_subscribe_attr_command(uint64_t node_id, ScopedMemoryBufferWithSize<uint16_t> &endpoint_ids,
                                      ScopedMemoryBufferWithSize<uint32_t> &cluster_ids,
                                      ScopedMemoryBufferWithSize<uint32_t> &attribute_ids, uint16_t min_interval,
                                      uint16_t max_interval, bool auto_resubscribe = true);

esp_err_t send_subscribe_event_command(uint64_t node_id, ScopedMemoryBufferWithSize<uint16_t> &endpoint_ids,
                                       ScopedMemoryBufferWithSize<uint32_t> &cluster_ids,
                                       ScopedMemoryBufferWithSize<uint32_t> &event_ids, uint16_t min_interval,
                                       uint16_t max_interval, bool auto_resubscribe = true);

esp_err_t send_subscribe_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, uint16_t min_interval, uint16_t max_interval,
                                      bool auto_resubscribe = true);

esp_err_t send_subscribe_event_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t event_id,
                                       uint16_t min_interval, uint16_t max_interval, bool auto_resubscribe = true);

esp_err_t send_shutdown_subscription(uint64_t node_id, uint32_t subscription_id);

} // namespace controller
} // namespace esp_matter
