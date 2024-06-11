// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_matter.h>
#include <esp_matter_client.h>
#include <esp_matter_mem.h>
#include <lib/core/ScopedNodeId.h>
#include <messaging/ExchangeMgr.h>
#include <transport/Session.h>

using chip::ScopedNodeId;
using chip::SessionHandle;
using chip::Messaging::ExchangeManager;

namespace esp_matter {
namespace controller {

class commissioning_window_opener {
public:
    typedef void (*commissioning_window_open_callback_t)(const char *manual_code);

    static commissioning_window_opener &get_instance()
    {
        static commissioning_window_opener instance;
        return instance;
    }

    void set_callback(commissioning_window_open_callback_t callback) { m_callback = callback; }

    esp_err_t send_open_commissioning_window_command(uint64_t node_id, bool is_enhanced, uint16_t timeout,
                                                     uint32_t iteration, uint16_t discriminator,
                                                     uint16_t timed_invoke_timeout_ms);

    uint16_t m_default_remote_endpoint_id = 0;

private:
    commissioning_window_opener()
        : on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
    {
    }
    ~commissioning_window_opener() {}

    static void on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                        const SessionHandle &sessionHandle);
    static void on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error);

    static void send_command_success_callback(void *context, const chip::app::DataModel::NullObjectType &data);

    static void send_command_failure_callback(void *context, CHIP_ERROR error);

    chip::Callback::Callback<chip::OnDeviceConnected> on_device_connected_cb;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> on_device_connection_failure_cb;

    uint32_t m_discriminator = 0;
    bool m_is_enhanced = false;
    uint32_t m_pincode = 0;
    uint16_t m_timout = 0;
    uint32_t m_iteration = 0;
    uint16_t m_timed_invoke_timeout_ms = 0;
    commissioning_window_open_callback_t m_callback = nullptr;
};

} // namespace controller
} // namespace esp_matter
