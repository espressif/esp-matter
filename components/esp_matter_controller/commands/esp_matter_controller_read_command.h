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
    READ_ATTRIBUTE = 0,
    READ_EVENT,
} read_command_type_t;

/** Read command class to send a read interaction command to a server **/
class read_command : public ReadClient::Callback {
public:
    /** Constructor for command with multiple paths**/
    read_command(uint64_t node_id, ScopedMemoryBufferWithSize<AttributePathParams> &&attr_paths,
                 ScopedMemoryBufferWithSize<EventPathParams> &&event_paths, attribute_report_cb_t attribute_cb,
                 read_done_cb_t read_cb_done, event_report_cb_t event_cb)
        : m_node_id(node_id)
        , m_buffered_read_cb(*this)
        , m_attr_paths(std::move(attr_paths))
        , m_event_paths(std::move(event_paths))
        , on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
        , attribute_data_cb(attribute_cb)
        , read_done_cb(read_cb_done)
        , event_data_cb(event_cb)
    {
    }
    /** Constructor for command with single path.
     * @note 0xFFFF could be used as wildcard EndpointId
     * @note 0xFFFFFFFF could be used as wildcard ClusterId/AttributeId/EventId
     */
    read_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_or_event_id,
                 read_command_type_t command_type, attribute_report_cb_t attribute_cb, read_done_cb_t read_cb_done,
                 event_report_cb_t event_cb)
        : m_node_id(node_id)
        , m_buffered_read_cb(*this)
        , on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
        , attribute_data_cb(attribute_cb)
        , read_done_cb(read_cb_done)
        , event_data_cb(event_cb)
    {
        if (command_type == READ_ATTRIBUTE) {
            m_attr_paths.Alloc(1);
            if (m_attr_paths.Get()) {
                m_attr_paths[0] = AttributePathParams(endpoint_id, cluster_id, attribute_or_event_id);
            }
        } else if (command_type == READ_EVENT) {
            m_event_paths.Alloc(1);
            if (m_event_paths.Get()) {
                m_event_paths[0] = EventPathParams(endpoint_id, cluster_id, attribute_or_event_id);
            }
        }
    }

    ~read_command() {}

    esp_err_t send_command();

    // ReadClient Callback Interface
    void OnAttributeData(const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *data,
                         const chip::app::StatusIB &status) override;

    void OnEventData(const chip::app::EventHeader &event_header, chip::TLV::TLVReader *data,
                     const chip::app::StatusIB *status) override;

    void OnError(CHIP_ERROR error) override;

    void OnDeallocatePaths(chip::app::ReadPrepareParams &&aReadPrepareParams) override;

    void OnDone(ReadClient *apReadClient) override;

private:
    uint64_t m_node_id;
    BufferedReadCallback m_buffered_read_cb;
    ScopedMemoryBufferWithSize<AttributePathParams> m_attr_paths;
    ScopedMemoryBufferWithSize<EventPathParams> m_event_paths;
    size_t m_event_path_len;

    static void on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                        const SessionHandle &sessionHandle);
    static void on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error);

    chip::Callback::Callback<chip::OnDeviceConnected> on_device_connected_cb;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> on_device_connection_failure_cb;

    attribute_report_cb_t attribute_data_cb;
    read_done_cb_t read_done_cb;
    event_report_cb_t event_data_cb;
};

/** Send read command with multiple attribute paths
 *
 * @note The three arrays should has the same size and the order of the three arrays should be the same as
 * the attribute paths.
 *
 * @param[in] node_id Remote NodeId
 * @param[in] endpoint_ids EndpointId array of the multiple attribute paths
 * @param[in] cluster_ids ClusterId array of the multiple attribute paths
 * @param[in] attribute_ids AttributeId array of the multiple attribute paths
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t send_read_attr_command(uint64_t node_id, ScopedMemoryBufferWithSize<uint16_t> &endpoint_ids,
                                 ScopedMemoryBufferWithSize<uint32_t> &cluster_ids,
                                 ScopedMemoryBufferWithSize<uint32_t> &attribute_ids);

/** Send read command with multiple event paths
 *
 * @note The three arrays should has the same size and the order of the three arrays should be the same as
 * the event paths.
 *
 * @param[in] node_id Remote NodeId
 * @param[in] endpoint_ids EndpointId array of the multiple event paths
 * @param[in] cluster_ids ClusterId array of the multiple event paths
 * @param[in] event_ids EventId array of the multiple event paths
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t send_read_event_command(uint64_t node_id, ScopedMemoryBufferWithSize<uint16_t> &endpoint_ids,
                                  ScopedMemoryBufferWithSize<uint32_t> &cluster_ids,
                                  ScopedMemoryBufferWithSize<uint32_t> &event_ids);

/** Send read command with single attribute path
 *
 * @note 0xFFFF could be used as wildcard EndpointId
 * @note 0xFFFFFFFF could be used as wildcard ClusterId/AttributeId/EventId
 *
 * @param[in] node_id Remote NodeId
 * @param[in] endpoint_id EndpointId
 * @param[in] cluster_id ClusterId
 * @param[in] attribute_id AttributeId
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t send_read_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id);

/** Send read command with single event path
 *
 * @note 0xFFFF could be used as wildcard EndpointId
 * @note 0xFFFFFFFF could be used as wildcard ClusterId/AttributeId/EventId
 *
 * @param[in] node_id Remote NodeId
 * @param[in] endpoint_id EndpointId
 * @param[in] cluster_id ClusterId
 * @param[in] event_id EventId
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t send_read_event_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t event_id);

} // namespace controller
} // namespace esp_matter
