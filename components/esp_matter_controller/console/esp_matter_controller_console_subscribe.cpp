// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

#include "console_utils.h"

#include <app/InteractionModelEngine.h>
#include <commands/clusters/DataModelLogger.h>
#include <esp_matter_client_subscribe_command.h>
#include <lib/support/CHIPArgParser.hpp>

namespace esp_matter::controller::console {

using chip::Platform::ScopedMemoryBufferWithSize;

class ConsoleSubscription;
static ConsoleSubscription *s_subscriptions;

class ConsoleSubscription final : public client::subscribe_command::delegate {
public:
    client::subscribe_command::subscription subscription;
    ConsoleSubscription *next = nullptr;

    void OnAttribute(const esp_matter::client::target_t &, const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *value, const chip::app::StatusIB &status) override
    {
        if (status.IsSuccess()) {
            LogErrorOnFailure(DataModelLogger::LogAttribute(path, value));
        } else {
            ESP_LOGE(TAG, "Subscription status: 0x%" PRIx32, status.ToChipError().AsInteger());
        }
    }

    void OnEvent(const esp_matter::client::target_t &, const chip::app::EventHeader &header, chip::TLV::TLVReader *value, const chip::app::StatusIB *status) override
    {
        if (status != nullptr) {
            ESP_LOGE(TAG, "Subscription event status: 0x%" PRIx32, status->ToChipError().AsInteger());
        } else {
            LogErrorOnFailure(DataModelLogger::LogEvent(header, value));
        }
    }

    void OnStateChanged(const esp_matter::client::target_t &, client::subscribe_command::state_t state, CHIP_ERROR error) override
    {
        ESP_LOGI(TAG, "Subscription state %u: 0x%" PRIx32, static_cast<unsigned>(state),
                 error.AsInteger());
        if (state == client::subscribe_command::state_t::terminated) {
            // Subscription callbacks must unwind before their owner is destroyed.
            LogErrorOnFailure(chip::DeviceLayer::PlatformMgr().ScheduleWork(Cleanup, 0));
        }
    }

    static void Reap()
    {
        ConsoleSubscription *entry = s_subscriptions;
        while (entry != nullptr) {
            ConsoleSubscription *next = entry->next;
            if (entry->subscription.state() == client::subscribe_command::state_t::terminated) {
                chip::Platform::Delete(entry);
            }
            entry = next;
        }
    }

    ~ConsoleSubscription() override
    {
        auto **entry = &s_subscriptions;
        while (*entry != nullptr && *entry != this) {
            entry = &(*entry)->next;
        }
        if (*entry == this) {
            *entry = next;
        }
    }

private:
    static void Cleanup(intptr_t)
    {
        Reap();
    }
};

template <typename Path> static esp_err_t controller_subscribe_handler(int argc, char **argv)
{
    if (argc < 6 || argc > 8) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id;
    client::subscribe_command::options_t options;
    ESP_RETURN_ON_FALSE(parse_unsigned(argv[0], node_id) && parse_unsigned(argv[4], options.min_interval) &&
                        parse_unsigned(argv[5], options.max_interval) &&
                        (argc < 7 || chip::ArgParser::ParseBoolean(argv[6], options.keep_subscriptions)) &&
                        (argc < 8 || chip::ArgParser::ParseBoolean(argv[7], options.auto_resubscribe)),
                        ESP_ERR_INVALID_ARG, TAG, "Invalid subscription options");
    ScopedMemoryBufferWithSize<Path> paths;
    ESP_RETURN_ON_ERROR(parse_path_columns(argv[1], argv[2], argv[3], paths), TAG, "Failed to parse paths");

    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    chip::ScopedNodeId target(node_id, active_fabric_index());
    auto *entry = chip::Platform::New<ConsoleSubscription>();
    ESP_RETURN_ON_FALSE(entry != nullptr, ESP_ERR_NO_MEM, TAG, "Failed to allocate subscription");
    client::subscribe_command::request_t request(paths.Span(), options);
    CHIP_ERROR err = entry->subscription.start(target, request, *entry);
    if (err != CHIP_NO_ERROR) {
        chip::Platform::Delete(entry);
        return ESP_FAIL;
    }
    entry->next = s_subscriptions;
    s_subscriptions = entry;
    return ESP_OK;
}

static esp_err_t controller_remove_subscription_handler(int argc, char **argv)
{
    if (argc < 1 || argc > 2) {
        return ESP_ERR_INVALID_ARG;
    }

    const bool all_nodes = strcmp(argv[0], "all") == 0;
    const bool all_subscriptions = all_nodes || (argc == 2 && strcmp(argv[1], "all") == 0);
    chip::NodeId node_id = chip::kUndefinedNodeId;
    chip::SubscriptionId subscription_id = 0;
    if (all_nodes) {
        if (argc != 1) {
            return ESP_ERR_INVALID_ARG;
        }
    } else {
        /* Reject signed, missing, overflowing, or malformed IDs before shutdown. */
        if (argc != 2 || !parse_unsigned(argv[0], node_id) || !chip::IsOperationalNodeId(node_id)) {
            return ESP_ERR_INVALID_ARG;
        }
        if (!all_subscriptions && !parse_unsigned(argv[1], subscription_id)) {
            return ESP_ERR_INVALID_ARG;
        }
    }

    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    const chip::ScopedNodeId peer(node_id, active_fabric_index());
    bool removed = false;
    ConsoleSubscription **current = &s_subscriptions;
    while (*current != nullptr) {
        ConsoleSubscription *entry = *current;
        const auto id = entry->subscription.id();
        if ((all_nodes || entry->subscription.peer() == peer) &&
                (all_subscriptions || (id.HasValue() && id.Value() == subscription_id))) {
            *current = entry->next;
            chip::Platform::Delete(entry);
            removed = true;
        } else {
            current = &entry->next;
        }
    }

    /* Engine shutdown may clear subscription IDs; detach console owners first. */
    auto *engine = chip::app::InteractionModelEngine::GetInstance();
    if (all_nodes) {
        engine->ShutdownAllSubscriptions();
    } else if (all_subscriptions) {
        engine->ShutdownSubscriptions(peer.GetFabricIndex(), node_id);
    } else {
        CHIP_ERROR err = engine->ShutdownSubscription(peer, subscription_id);
        return removed || err == CHIP_NO_ERROR ? ESP_OK : ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

static esp_err_t controller_list_subscriptions_handler(int argc, char **argv)
{
    if (argc != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    for (ConsoleSubscription *entry = s_subscriptions; entry != nullptr; entry = entry->next) {
        const auto peer = entry->subscription.peer();
        const auto id = entry->subscription.id();
        const char *state = "terminated";
        switch (entry->subscription.state()) {
        case client::subscribe_command::state_t::connecting:
            state = "connecting";
            break;
        case client::subscribe_command::state_t::active:
            state = "active";
            break;
        case client::subscribe_command::state_t::backoff:
            state = "backoff";
            break;
        case client::subscribe_command::state_t::terminated:
            break;
        }
        if (id.HasValue()) {
            ESP_LOGI(TAG, "fabric=%u node=0x%016" PRIx64 " subscription=%" PRIu32 " state=%s",
                     static_cast<unsigned>(peer.GetFabricIndex()), peer.GetNodeId(), id.Value(), state);
        } else {
            ESP_LOGI(TAG, "fabric=%u node=0x%016" PRIx64 " subscription=pending state=%s",
                     static_cast<unsigned>(peer.GetFabricIndex()), peer.GetNodeId(), state);
        }
    }
    if (s_subscriptions == nullptr) {
        ESP_LOGI(TAG, "No console-owned subscriptions");
    }
    return ESP_OK;
}

static esp_err_t controller_sub_handler(int argc, char **argv)
{
    ConsoleSubscription::Reap();
    if (argc >= 2 && strcmp(argv[0], "add") == 0) {
        if (strcmp(argv[1], "attr") == 0) {
            return controller_subscribe_handler<chip::app::AttributePathParams>(argc - 2, argv + 2);
        }
        if (strcmp(argv[1], "event") == 0) {
            return controller_subscribe_handler<chip::app::EventPathParams>(argc - 2, argv + 2);
        }
    } else if (argc == 1 && strcmp(argv[0], "list") == 0) {
        return controller_list_subscriptions_handler(0, nullptr);
    } else if (argc >= 2 && strcmp(argv[0], "remove") == 0) {
        return controller_remove_subscription_handler(argc - 1, argv + 1);
    }
    return ESP_ERR_INVALID_ARG;
}

esp_err_t register_subscribe_commands()
{
    static const command_t commands[] = {
        {
            .name = "sub",
            .description = "Usage: controller sub add attr|event <node> <endpoints> <clusters> <ids> <min> <max> [keep] [auto-resubscribe] OR\n"
            "\tsub list OR sub remove <node> <id|all> OR sub remove all\n"
            "\tRemoval also affects subscriptions created outside the console.",
            .handler = controller_sub_handler,
        },
    };
    return register_commands(commands, sizeof(commands) / sizeof(commands[0]));
}

} // namespace esp_matter::controller::console
