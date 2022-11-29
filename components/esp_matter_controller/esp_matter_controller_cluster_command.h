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

#include <controller/CommissioneeDeviceProxy.h>
#include <esp_matter.h>

namespace esp_matter {
namespace controller {

using chip::ScopedNodeId;
using chip::SessionHandle;
using chip::Messaging::ExchangeManager;
using esp_matter::client::peer_device_t;

constexpr size_t k_max_command_data_str_len = 16;
constexpr size_t k_max_command_data_size = 8;

typedef struct command_data {
    uint32_t cluster_id;
    uint32_t command_id;
    int command_data_count;
    char command_data_str[k_max_command_data_size][k_max_command_data_str_len];
} command_data_t;

class cluster_command {
public:
    cluster_command(uint64_t node_id, uint16_t endpoint_id, command_data_t *command_data)
        : m_node_id(node_id)
        , m_endpoint_id(endpoint_id)
        , m_command_data(command_data)
        , on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
    {
    }

    ~cluster_command()
    {
        if (m_command_data) {
            free(m_command_data);
        }
    }

    esp_err_t send_command();

private:
    uint64_t m_node_id;
    uint16_t m_endpoint_id;
    command_data_t *m_command_data;

    static void on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr, SessionHandle &sessionHandle);
    static void on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error);

    chip::Callback::Callback<chip::OnDeviceConnected> on_device_connected_cb;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> on_device_connection_failure_cb;
};

esp_err_t send_invoke_cluster_command(uint64_t node_id, uint16_t endpoint_id, int cmd_data_argc, char **cmd_data_argv);

} // namespace controller
} // namespace esp_matter
