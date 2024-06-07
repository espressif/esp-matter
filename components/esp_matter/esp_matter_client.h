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

#include <app-common/zap-generated/cluster-objects.h>
#include <esp_err.h>
#include <esp_matter_core.h>

namespace esp_matter {
namespace client {
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
using chip::System::Clock::Timeout;
using chip::TLV::TLVReader;
using client::peer_device_t;

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
