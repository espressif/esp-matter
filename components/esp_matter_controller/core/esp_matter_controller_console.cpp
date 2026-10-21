/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <commands/clusters/DataModelLogger.h>
#include <app/InteractionModelEngine.h>
#include <esp_check.h>
#include <esp_matter_client.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_commissioning_window_opener.h>
#include <esp_matter_controller_console.h>
#include <esp_matter_controller_group_settings.h>
#include <esp_matter_controller_icd_client.h>
#include <esp_matter_controller_pairing_command.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_core.h>
#include <inttypes.h>
#include <lib/core/CHIPCore.h>
#include <lib/shell/Commands.h>
#include <lib/shell/Engine.h>
#include <lib/shell/commands/Help.h>
#include <lib/shell/streamer.h>
#include <lib/support/CHIPArgParser.hpp>
#include <lib/support/CHIPMem.h>
#include <lib/support/CodeUtils.h>
#include <platform/CHIPDeviceLayer.h>
#include <protocols/secure_channel/RendezvousParameters.h>
#include <protocols/user_directed_commissioning/UserDirectedCommissioning.h>
#include <tlv_to_json.h>

using chip::NodeId;
using chip::Inet::IPAddress;
using chip::Platform::ScopedMemoryBufferWithSize;
using chip::Transport::PeerAddress;

const static char *TAG = "controller_console";

namespace esp_matter {

static size_t get_array_size(const char *str)
{
    if (!str) {
        return 0;
    }
    size_t ret = 1;
    for (size_t i = 0; i < strlen(str); ++i) {
        if (str[i] == ',') {
            ret++;
        }
    }
    return ret;
}

static esp_err_t string_to_uint32_array(const char *str, ScopedMemoryBufferWithSize<uint32_t> &uint32_array)
{
    size_t array_len = get_array_size(str);
    if (array_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    uint32_array.Calloc(array_len);
    if (!uint32_array.Get()) {
        return ESP_ERR_NO_MEM;
    }
    char number[11]; // max(strlen("0xFFFFFFFF"), strlen("4294967295")) + 1
    const char *next_number_start = str;
    char *next_number_end = NULL;
    size_t next_number_len = 0;
    for (size_t i = 0; i < array_len; ++i) {
        next_number_end = strchr(next_number_start, ',');
        if (next_number_end > next_number_start) {
            next_number_len = std::min((size_t)(next_number_end - next_number_start), sizeof(number) - 1);
        } else if (i == array_len - 1) {
            next_number_len = strnlen(next_number_start, sizeof(number) - 1);
        } else {
            return ESP_ERR_INVALID_ARG;
        }
        strncpy(number, next_number_start, next_number_len);
        number[next_number_len] = 0;
        uint32_array[i] = string_to_uint32(number);
        if (next_number_end > next_number_start) {
            next_number_start = next_number_end + 1;
        }
    }
    return ESP_OK;
}

esp_err_t string_to_uint16_array(const char *str, ScopedMemoryBufferWithSize<uint16_t> &uint16_array)
{
    size_t array_len = get_array_size(str);
    if (array_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    uint16_array.Calloc(array_len);
    if (!uint16_array.Get()) {
        return ESP_ERR_NO_MEM;
    }
    char number[7]; // max(strlen(0xFFFF), strlen(65535)) + 1
    const char *next_number_start = str;
    char *next_number_end = NULL;
    size_t next_number_len = 0;
    for (size_t i = 0; i < array_len; ++i) {
        next_number_end = strchr(next_number_start, ',');
        if (next_number_end > next_number_start) {
            next_number_len = std::min((size_t)(next_number_end - next_number_start), sizeof(number) - 1);
        } else if (i == array_len - 1) {
            next_number_len = strnlen(next_number_start, sizeof(number) - 1);
        } else {
            return ESP_ERR_INVALID_ARG;
        }
        strncpy(number, next_number_start, next_number_len);
        number[next_number_len] = 0;
        uint16_array[i] = string_to_uint16(number);
        if (next_number_end > next_number_start) {
            next_number_start = next_number_end + 1;
        }
    }
    return ESP_OK;
}

namespace console {
static engine controller_console;

static esp_err_t controller_help_handler(int argc, char **argv)
{
    controller_console.for_each_command(print_description, nullptr);
    return ESP_OK;
}

#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
static int char_to_int(char ch)
{
    if ('A' <= ch && ch <= 'F') {
        return 10 + ch - 'A';
    } else if ('a' <= ch && ch <= 'f') {
        return 10 + ch - 'a';
    } else if ('0' <= ch && ch <= '9') {
        return ch - '0';
    }
    return -1;
}

static bool convert_hex_str_to_bytes(const char *hex_str, uint8_t *bytes, uint8_t &bytes_len)
{
    if (!hex_str) {
        return false;
    }
    size_t hex_str_len = strlen(hex_str);
    if (hex_str_len == 0 || hex_str_len % 2 != 0 || hex_str_len / 2 > bytes_len) {
        return false;
    }
    bytes_len = hex_str_len / 2;
    for (size_t i = 0; i < bytes_len; ++i) {
        int byte_h = char_to_int(hex_str[2 * i]);
        int byte_l = char_to_int(hex_str[2 * i + 1]);
        if (byte_h < 0 || byte_l < 0) {
            return false;
        }
        bytes[i] = (byte_h << 4) + byte_l;
    }
    return true;
}

static esp_err_t controller_pairing_handler(int argc, char **argv)
{
    VerifyOrReturnError(argc >= 2 && argc <= 6, ESP_ERR_INVALID_ARG);
    esp_err_t result = ESP_ERR_INVALID_ARG;

    if (strncmp(argv[0], "onnetwork", sizeof("onnetwork")) == 0) {
        VerifyOrReturnError(argc == 3, ESP_ERR_INVALID_ARG);

        uint64_t nodeId = string_to_uint64(argv[1]);
        uint32_t pincode = string_to_uint32(argv[2]);
        return controller::pairing_on_network(nodeId, pincode);

#if CONFIG_ENABLE_ESP32_BLE_CONTROLLER
    } else if (strncmp(argv[0], "ble-wifi", sizeof("ble-wifi")) == 0) {
        VerifyOrReturnError(argc == 6, ESP_ERR_INVALID_ARG);

        uint64_t nodeId = string_to_uint64(argv[1]);
        uint32_t pincode = string_to_uint32(argv[4]);
        uint16_t disc = string_to_uint16(argv[5]);

        result = controller::pairing_ble_wifi(nodeId, pincode, disc, argv[2], argv[3]);
    } else if (strncmp(argv[0], "ble-thread", sizeof("ble-thread")) == 0) {
        VerifyOrReturnError(argc == 5, ESP_ERR_INVALID_ARG);

        uint8_t dataset_tlvs_buf[254];
        uint8_t dataset_tlvs_len = sizeof(dataset_tlvs_buf);
        if (!convert_hex_str_to_bytes(argv[2], dataset_tlvs_buf, dataset_tlvs_len)) {
            return ESP_ERR_INVALID_ARG;
        }
        uint64_t node_id = string_to_uint64(argv[1]);
        uint32_t pincode = string_to_uint32(argv[3]);
        uint16_t disc = string_to_uint16(argv[4]);

        result = controller::pairing_ble_thread(node_id, pincode, disc, dataset_tlvs_buf, dataset_tlvs_len);
#else // if !CONFIG_ENABLE_ESP32_BLE_CONTROLLER
    } else if (strncmp(argv[0], "ble-wifi", sizeof("ble-wifi")) == 0 ||
               strncmp(argv[0], "ble-thread", sizeof("ble-thread")) == 0) {
        ESP_LOGE(TAG, "Please enable ENABLE_ESP32_BLE_CONTROLLER to use pairing %s command", argv[0]);
        return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_ENABLE_ESP32_BLE_CONTROLLER
    } else if (strncmp(argv[0], "code", sizeof("code")) == 0) {
        VerifyOrReturnError(argc == 3, ESP_ERR_INVALID_ARG);

        uint64_t nodeId = string_to_uint64(argv[1]);
        const char *payload = argv[2];

        result = controller::pairing_code(nodeId, payload);
    } else if (strncmp(argv[0], "code-thread", sizeof("code-thread")) == 0) {
        VerifyOrReturnError(argc == 4, ESP_ERR_INVALID_ARG);

        uint64_t nodeId = string_to_uint64(argv[1]);
        const char *payload = argv[3];

        uint8_t dataset_tlvs_buf[254];
        uint8_t dataset_tlvs_len = sizeof(dataset_tlvs_buf);
        if (!convert_hex_str_to_bytes(argv[2], dataset_tlvs_buf, dataset_tlvs_len)) {
            return ESP_ERR_INVALID_ARG;
        }

        result = controller::pairing_code_thread(nodeId, payload, dataset_tlvs_buf, dataset_tlvs_len);
    } else if (strncmp(argv[0], "code-wifi", sizeof("code-wifi")) == 0) {
        VerifyOrReturnError(argc == 5, ESP_ERR_INVALID_ARG);

        uint64_t nodeId = string_to_uint64(argv[1]);
        const char *ssid = argv[2];
        const char *password = argv[3];
        const char *payload = argv[4];

        result = controller::pairing_code_wifi(nodeId, ssid, password, payload);
    } else if (strncmp(argv[0], "code-wifi-thread", sizeof("code-wifi-thread")) == 0) {
        VerifyOrReturnError(argc == 6, ESP_ERR_INVALID_ARG);

        uint64_t nodeId = string_to_uint64(argv[1]);
        const char *ssid = argv[2];
        const char *password = argv[3];
        const char *payload = argv[4];

        uint8_t dataset_tlvs_buf[254];
        uint8_t dataset_tlvs_len = sizeof(dataset_tlvs_buf);
        if (!convert_hex_str_to_bytes(argv[5], dataset_tlvs_buf, dataset_tlvs_len)) {
            return ESP_ERR_INVALID_ARG;
        }

        result = controller::pairing_code_wifi_thread(nodeId, ssid, password, payload, dataset_tlvs_buf,
                                                      dataset_tlvs_len);
    } else if (strncmp(argv[0], "unpair", sizeof("unpair")) == 0) {
        VerifyOrReturnError(argc == 2, ESP_ERR_INVALID_ARG);
        uint64_t node_id = string_to_uint64(argv[1]);
        result = controller::unpair_device(node_id);
    }

    if (result != ESP_OK) {
        ESP_LOGE(TAG, "Pairing over code failed");
    }
    return result;
}

#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
static esp_err_t controller_udc_handler(int argc, char **argv)
{
    if (argc < 1 || argc > 3) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strncmp(argv[0], "reset", sizeof("reset")) == 0) {
        if (argc != 1) {
            return ESP_ERR_INVALID_ARG;
        }
        controller::matter_controller_client::get_instance()
        .get_commissioner()
        ->GetUserDirectedCommissioningServer()
        ->ResetUDCClientProcessingStates();
    } else if (strncmp(argv[0], "print", sizeof("print")) == 0) {
        if (argc != 1) {
            return ESP_ERR_INVALID_ARG;
        }
        controller::matter_controller_client::get_instance()
        .get_commissioner()
        ->GetUserDirectedCommissioningServer()
        ->PrintUDCClients();
    } else if (strncmp(argv[0], "commission", sizeof("commission")) == 0) {
        if (argc != 3) {
            return ESP_ERR_INVALID_ARG;
        }
        uint32_t pincode = string_to_uint32(argv[1]);
        printf("pincode %" PRIu32, pincode);
        size_t index = (size_t)string_to_uint32(argv[2]);
        controller::matter_controller_client &instance = controller::matter_controller_client::get_instance();
        UDCClientState *state =
            instance.get_commissioner()->GetUserDirectedCommissioningServer()->GetUDCClients().GetUDCClientState(index);
        ESP_RETURN_ON_FALSE(state != nullptr, ESP_FAIL, TAG, "UDC client not found");
        state->SetUDCClientProcessingState(chip::Protocols::UserDirectedCommissioning::UDCClientProcessingState::kCommissioningNode);

        chip::NodeId gRemoteId = chip::kTestDeviceNodeId;
        chip::RendezvousParameters params = chip::RendezvousParameters()
                                            .SetSetupPINCode(pincode)
                                            .SetDiscriminator(state->GetLongDiscriminator())
                                            .SetPeerAddress(state->GetPeerAddress());
        do {
            LogErrorOnFailure(chip::Crypto::DRBG_get_bytes(reinterpret_cast<uint8_t *>(&gRemoteId), sizeof(gRemoteId)));
        } while (!chip::IsOperationalNodeId(gRemoteId));

        ESP_RETURN_ON_FALSE(instance.get_commissioner()->PairDevice(gRemoteId, params) == CHIP_NO_ERROR, ESP_FAIL, TAG,
                            "Failed to commission udc");
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}
#endif // CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE

#ifndef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
static esp_err_t controller_group_settings_handler(int argc, char **argv)
{
    if (argc >= 1) {
        if (strncmp(argv[0], "show-groups", sizeof("show-groups")) == 0) {
            return controller::group_settings::show_groups();
        } else if (strncmp(argv[0], "add-group", sizeof("add-group")) == 0) {
            if (argc != 3) {
                return ESP_ERR_INVALID_ARG;
            }
            uint16_t group_id = string_to_uint16(argv[1]);
            char *group_name = argv[2];
            return controller::group_settings::add_group(group_name, group_id);
        } else if (strncmp(argv[0], "remove-group", sizeof("remove-group")) == 0) {
            if (argc != 2) {
                return ESP_ERR_INVALID_ARG;
            }
            uint16_t group_id = string_to_uint16(argv[1]);
            return controller::group_settings::remove_group(group_id);
        } else if (strncmp(argv[0], "show-keysets", sizeof("show-keysets")) == 0) {
            return controller::group_settings::show_keysets();
        } else if (strncmp(argv[0], "add-keyset", sizeof("add-keyset")) == 0) {
            if (argc != 5) {
                return ESP_ERR_INVALID_ARG;
            }
            uint16_t keyset_id = string_to_uint16(argv[1]);
            uint8_t key_policy = string_to_uint8(argv[2]);
            uint64_t validity_time = string_to_uint64(argv[3]);
            char *epoch_key_oct_str = argv[4];
            return controller::group_settings::add_keyset(keyset_id, key_policy, validity_time, epoch_key_oct_str);
        } else if (strncmp(argv[0], "remove-keyset", sizeof("remove_keyset")) == 0) {
            if (argc != 2) {
                return ESP_ERR_INVALID_ARG;
            }
            uint16_t keyset_id = string_to_uint16(argv[1]);
            return controller::group_settings::remove_keyset(keyset_id);
        } else if (strncmp(argv[0], "bind-keyset", sizeof("bind_keyset")) == 0) {
            if (argc != 3) {
                return ESP_ERR_INVALID_ARG;
            }
            uint16_t group_id = string_to_uint16(argv[1]);
            uint16_t keyset_id = string_to_uint16(argv[2]);
            return controller::group_settings::bind_keyset(group_id, keyset_id);
        } else if (strncmp(argv[0], "unbind-keyset", sizeof("unbind_keyset")) == 0) {
            if (argc != 3) {
                return ESP_ERR_INVALID_ARG;
            }
            uint16_t group_id = string_to_uint16(argv[1]);
            uint16_t keyset_id = string_to_uint16(argv[2]);
            return controller::group_settings::unbind_keyset(group_id, keyset_id);
        }
    }
    ESP_LOGI(TAG, "Subcommands of group-settings:");
    ESP_LOGI(TAG, "Show groups   : controller group-settings show-groups");
    ESP_LOGI(TAG, "Add group     : controller group-settings add-group <group_id> <group_name>");
    ESP_LOGI(TAG, "Remove group  : controller group-settings remove-group <group_id>");
    ESP_LOGI(TAG, "Show keysets  : controller group-settings show-keysets");
    ESP_LOGI(TAG,
             "Add keyset    : controller group-settings add-keyset <ketset_id> <policy> <validity_time> "
             "<epoch_key_oct_str>");
    ESP_LOGI(TAG, "Remove keyset : controller group-settings remove-keyset <ketset_id>");
    ESP_LOGI(TAG, "Bind keyset   : controller group-settings bind-keyset <group_id> <ketset_id>");
    ESP_LOGI(TAG, "Unbind keyset : controller group-settings unbind-keyset <group_id> <ketset_id>");
    return ESP_OK;
}
#endif

static void print_manual_code(const char *manual_code)
{
    ESP_LOGI(TAG,
             "*************************************Manual Code: [%s]**********************************************",
             manual_code);
}

static esp_err_t open_commissioning_window_handler(int argc, char **argv)
{
    if (argc != 5) {
        return ESP_ERR_INVALID_ARG;
    }
    uint64_t node_id = string_to_uint64(argv[0]);
    uint8_t option = string_to_uint8(argv[1]);
    bool is_enhanced = option == 1;
    uint16_t window_timeout = string_to_uint16(argv[2]);
    uint32_t iteration = string_to_uint32(argv[3]);
    uint16_t discriminator = string_to_uint16(argv[4]);

    controller::commissioning_window_opener::get_instance().set_callback(print_manual_code);
    return controller::commissioning_window_opener::get_instance().send_open_commissioning_window_command(
               node_id, is_enhanced, window_timeout, iteration, discriminator, 10000 /* timed_invoke_timeout_ms */);
}

static chip::FabricIndex active_fabric_index()
{
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    return controller::get_fabric_index();
#else
    return controller::matter_controller_client::get_instance().get_fabric_index();
#endif
}

static CHIP_ERROR resolve_console_target(chip::NodeId node_id, client::node_target_t &target)
{
    return client::resolve_target(chip::ScopedNodeId(node_id, active_fabric_index()), target);
}

// The console owns each outstanding command independently. Cleanup runs after
// callbacks unwind; if the work queue is full, the next command/completion also
// drains this list. Queued cleanup never stores a command pointer.
class ConsoleCommand {
public:
    virtual ~ConsoleCommand() = default;

    static void Reap()
    {
        while (sCompleted != nullptr) {
            ConsoleCommand *entry = sCompleted;
            sCompleted = entry->mNext;
            chip::Platform::Delete(entry);
        }
    }

protected:
    void Complete()
    {
        mNext = sCompleted;
        sCompleted = this;
        LogErrorOnFailure(chip::DeviceLayer::PlatformMgr().ScheduleWork(Cleanup, 0));
    }

private:
    static void Cleanup(intptr_t)
    {
        Reap();
    }
    static ConsoleCommand *sCompleted;
    ConsoleCommand *mNext = nullptr;
};

ConsoleCommand *ConsoleCommand::sCompleted = nullptr;

class ConsoleReadDelegate final : public ConsoleCommand, public client::read_command::delegate {
public:
    client::read_command::interaction operation;
    void OnAttribute(const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *value, const chip::app::StatusIB &status) override
    {
        CHIP_ERROR err = status.ToChipError();
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Read status: 0x%" PRIx32, err.AsInteger());
        } else {
            LogErrorOnFailure(DataModelLogger::LogAttribute(path, value));
        }
    }

    void OnEvent(const chip::app::EventHeader &header, chip::TLV::TLVReader *value, const chip::app::StatusIB *status) override
    {
        if (status != nullptr) {
            ESP_LOGE(TAG, "Read event status: 0x%" PRIx32,
                     status->ToChipError().AsInteger());
        } else {
            LogErrorOnFailure(DataModelLogger::LogEvent(header, value));
        }
    }

    void OnDone(CHIP_ERROR err) override
    {
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Read failed: 0x%" PRIx32, err.AsInteger());
        }
        Complete();
    }
};

class ConsoleWriteDelegate final : public ConsoleCommand, public client::write_command::delegate {
public:
    client::write_command::interaction operation;
    void OnResponse(const chip::app::ConcreteDataAttributePath &, const chip::app::StatusIB &status) override
    {
        CHIP_ERROR err = status.ToChipError();
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Write status: 0x%" PRIx32, err.AsInteger());
        }
    }

    void OnDone(CHIP_ERROR err) override
    {
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Write failed: 0x%" PRIx32, err.AsInteger());
        }
        Complete();
    }
};

class ConsoleInvokeDelegate final
    : public ConsoleCommand, public client::invoke_command::delegate {
public:
    client::invoke_command::interaction operation;
    void OnResponse(const chip::app::ConcreteCommandPath &, const chip::app::StatusIB &status, chip::TLV::TLVReader *response) override
    {
        CHIP_ERROR err = status.ToChipError();
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Invoke status: 0x%" PRIx32, err.AsInteger());
            return;
        }
        if (response == nullptr) {
            ESP_LOGI(TAG, "Invoke completed without response data");
            return;
        }
        cJSON *json = nullptr;
        if (tlv_to_json(*response, &json, tlv_to_json_options{.human_readable_bytes = true}) == ESP_OK) {
            char *text = cJSON_Print(json);
            if (text != nullptr) {
                ESP_LOGI(TAG, "Response JSON:\n%s", text);
                cJSON_free(text);
            }
        }
        cJSON_Delete(json);
    }

    void OnDone(CHIP_ERROR err) override
    {
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Invoke failed: 0x%" PRIx32, err.AsInteger());
        }
        Complete();
    }
};

class ConsoleSubscription;
static ConsoleSubscription *s_subscriptions;

class ConsoleSubscription final : public client::subscribe_command::delegate {
public:
    client::subscribe_command::subscription subscription;
    ConsoleSubscription *next = nullptr;

    void OnAttribute(const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *value, const chip::app::StatusIB &status) override
    {
        if (status.IsSuccess()) {
            LogErrorOnFailure(DataModelLogger::LogAttribute(path, value));
        } else {
            ESP_LOGE(TAG, "Subscription status: 0x%" PRIx32, status.ToChipError().AsInteger());
        }
    }

    void OnEvent(const chip::app::EventHeader &header, chip::TLV::TLVReader *value, const chip::app::StatusIB *status) override
    {
        if (status != nullptr) {
            ESP_LOGE(TAG, "Subscription event status: 0x%" PRIx32, status->ToChipError().AsInteger());
        } else {
            LogErrorOnFailure(DataModelLogger::LogEvent(header, value));
        }
    }

    void OnStateChanged(client::subscribe_command::state_t state, CHIP_ERROR cause) override
    {
        ESP_LOGI(TAG, "Subscription state %u: 0x%" PRIx32, static_cast<unsigned>(state),
                 cause.AsInteger());
        if (state == client::subscribe_command::state_t::terminated && !cleanup_scheduled) {
            CHIP_ERROR err = chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::kZero, Cleanup, this);
            if (err == CHIP_NO_ERROR) {
                cleanup_scheduled = true;
            } else {
                ESP_LOGE(TAG, "Failed to schedule subscription cleanup: 0x%" PRIx32, err.AsInteger());
            }
        }
    }

    ~ConsoleSubscription()
    {
        chip::DeviceLayer::SystemLayer().CancelTimer(Cleanup, this);
    }

private:
    static void Cleanup(chip::System::Layer *, void *context)
    {
        auto *entry = static_cast<ConsoleSubscription *>(context);
        ConsoleSubscription **current = &s_subscriptions;
        while (*current != nullptr) {
            if (*current == entry) {
                *current = entry->next;
                chip::Platform::Delete(entry);
                return;
            }
            current = &(*current)->next;
        }
    }

    bool cleanup_scheduled = false;
};

static esp_err_t controller_invoke_command_handler(int argc, char **argv)
{
    if (argc < 4) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    uint16_t endpoint_id = string_to_uint16(argv[1]);
    uint32_t cluster_id = string_to_uint32(argv[2]);
    uint32_t command_id = string_to_uint32(argv[3]);
    client::json_encodable fields(argc > 4 ? argv[4] : nullptr);
    client::invoke_command::request_t request = {
        .endpoint_id = endpoint_id,
        .cluster_id = cluster_id,
        .command_id = command_id,
        .fields = argc > 4 ? &fields : nullptr,
    };
    if (argc > 5) {
        uint16_t timeout = string_to_uint16(argv[5]);
        if (timeout > 0) {
            request.options.timed_timeout_ms.SetValue(timeout);
        }
    }

    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    ConsoleCommand::Reap();
    auto *command = chip::Platform::New<ConsoleInvokeDelegate>();
    ESP_RETURN_ON_FALSE(command != nullptr, ESP_ERR_NO_MEM, TAG, "Failed to allocate Invoke");
    CHIP_ERROR err;
    if (chip::IsGroupId(node_id)) {
        client::group_target_t target = {active_fabric_index(), static_cast<chip::GroupId>(node_id)};
        err = command->operation.start(target, request, command);
    } else {
        client::node_target_t target;
        err = resolve_console_target(node_id, target);
        if (err == CHIP_NO_ERROR) {
            err = command->operation.start(target, request, command);
        }
    }
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Invoke submission failed: 0x%" PRIx32, err.AsInteger());
        chip::Platform::Delete(command);
        return ESP_FAIL;
    }
    return ESP_OK;
}

template <typename Path>
static esp_err_t parse_path_columns(const char *endpoint_arg, const char *cluster_arg, const char *path_arg,
                                    ScopedMemoryBufferWithSize<Path> &paths)
{
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> path_ids;
    esp_err_t err = string_to_uint16_array(endpoint_arg, endpoint_ids);
    if (err != ESP_OK) {
        return err;
    }
    err = string_to_uint32_array(cluster_arg, cluster_ids);
    if (err != ESP_OK) {
        return err;
    }
    err = string_to_uint32_array(path_arg, path_ids);
    if (err != ESP_OK) {
        return err;
    }
    ESP_RETURN_ON_FALSE(endpoint_ids.AllocatedSize() == cluster_ids.AllocatedSize() &&
                        endpoint_ids.AllocatedSize() == path_ids.AllocatedSize(),
                        ESP_ERR_INVALID_ARG, TAG, "Path array sizes differ");
    ESP_RETURN_ON_FALSE(paths.Alloc(endpoint_ids.AllocatedSize()), ESP_ERR_NO_MEM, TAG, "Failed to allocate paths");
    for (size_t index = 0; index < paths.AllocatedSize(); ++index) {
        paths[index] = Path(endpoint_ids[index], cluster_ids[index], path_ids[index]);
    }
    return ESP_OK;
}

template <typename Path> static esp_err_t controller_read_handler(int argc, char **argv)
{
    if (argc != 4) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    ScopedMemoryBufferWithSize<Path> paths;
    ESP_RETURN_ON_ERROR(parse_path_columns(argv[1], argv[2], argv[3], paths), TAG, "Failed to parse paths");
    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    client::node_target_t target;
    CHIP_ERROR err = resolve_console_target(node_id, target);
    ESP_RETURN_ON_FALSE(err == CHIP_NO_ERROR, ESP_FAIL, TAG, "Target resolution failed");
    client::read_command::request_t request(paths.Span());
    ConsoleCommand::Reap();
    auto *command = chip::Platform::New<ConsoleReadDelegate>();
    ESP_RETURN_ON_FALSE(command != nullptr, ESP_ERR_NO_MEM, TAG, "Failed to allocate Read");
    err = command->operation.start(target, request, *command);
    if (err != CHIP_NO_ERROR) {
        chip::Platform::Delete(command);
    }
    return err == CHIP_NO_ERROR ? ESP_OK : ESP_FAIL;
}

static esp_err_t controller_write_attr_handler(int argc, char **argv)
{
    if (argc < 5 || argc > 6) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    ScopedMemoryBufferWithSize<chip::app::ConcreteDataAttributePath> paths;
    ESP_RETURN_ON_ERROR(parse_path_columns(argv[1], argv[2], argv[3], paths), TAG, "Failed to parse attribute paths");
    ESP_RETURN_ON_FALSE(paths.AllocatedSize() == 1, ESP_ERR_INVALID_ARG, TAG, "Write requires one attribute path");
    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    client::node_target_t target;
    CHIP_ERROR err = resolve_console_target(node_id, target);
    ESP_RETURN_ON_FALSE(err == CHIP_NO_ERROR, ESP_FAIL, TAG, "Target resolution failed");
    client::json_encodable value(argv[4]);
    client::write_command::attribute_write_t attribute = {
        .path = paths[0],
        .value = &value,
    };
    client::write_command::request_t request = {
        .attributes = chip::Span<const client::write_command::attribute_write_t>(&attribute, 1),
    };
    if (argc == 6) {
        uint16_t timeout = string_to_uint16(argv[5]);
        if (timeout > 0) {
            request.options.timed_timeout_ms.SetValue(timeout);
        }
    }
    ConsoleCommand::Reap();
    auto *command = chip::Platform::New<ConsoleWriteDelegate>();
    ESP_RETURN_ON_FALSE(command != nullptr, ESP_ERR_NO_MEM, TAG, "Failed to allocate Write");
    err = command->operation.start(target, request, command);
    if (err != CHIP_NO_ERROR) {
        chip::Platform::Delete(command);
    }
    return err == CHIP_NO_ERROR ? ESP_OK : ESP_FAIL;
}

template <typename Path> static esp_err_t controller_subscribe_handler(int argc, char **argv)
{
    if (argc < 6 || argc > 8) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    ScopedMemoryBufferWithSize<Path> paths;
    ESP_RETURN_ON_ERROR(parse_path_columns(argv[1], argv[2], argv[3], paths), TAG, "Failed to parse paths");
    client::subscribe_command::options_t options = {
        .min_interval = string_to_uint16(argv[4]),
        .max_interval = string_to_uint16(argv[5]),
    };
    if (argc >= 7) {
        options.keep_subscriptions = string_to_bool(argv[6]);
    }
    if (argc >= 8) {
        options.auto_resubscribe = string_to_bool(argv[7]);
    }

    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    client::node_target_t target;
    CHIP_ERROR err = resolve_console_target(node_id, target);
    ESP_RETURN_ON_FALSE(err == CHIP_NO_ERROR, ESP_FAIL, TAG, "Target resolution failed");
    ESP_RETURN_ON_FALSE(target.execution == client::execution_t::remote, ESP_ERR_NOT_SUPPORTED, TAG,
                        "Subscriptions require a remote target");
    auto *entry = chip::Platform::New<ConsoleSubscription>();
    ESP_RETURN_ON_FALSE(entry != nullptr, ESP_ERR_NO_MEM, TAG, "Failed to allocate subscription");
    chip::ScopedNodeId remote_target = target.node;
    client::subscribe_command::request_t request(paths.Span(), options);
    err = entry->subscription.start(remote_target, request, *entry);
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
        // Reject signs, missing IDs, overflow and trailing text before any shutdown.
        if (argc != 2 || argv[0][0] < '0' || argv[0][0] > '9' ||
                !chip::ArgParser::ParseInt(argv[0], node_id) || !chip::IsOperationalNodeId(node_id)) {
            return ESP_ERR_INVALID_ARG;
        }
        if (!all_subscriptions && (argv[1][0] < '0' || argv[1][0] > '9' ||
                                   !chip::ArgParser::ParseInt(argv[1], subscription_id))) {
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

    // Detach console owners first; engine shutdown can clear their subscription IDs.
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

static esp_err_t controller_icd_list_handler(int argc, char **argv)
{
    if (argc != 1 || strncmp(argv[0], "list", sizeof("list")) != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    controller::list_registered_icd();
    return ESP_OK;
}

static esp_err_t controller_dispatch(int argc, char **argv)
{
    if (argc == 0) {
        return controller_help_handler(argc, argv);
    }
    return controller_console.exec_command(argc, argv);
}

esp_err_t controller_register_commands()
{
    // Subcommands for root command: `controller <subcommand>`
    const static command_t controller_sub_commands[] = {
        {
            .name = "help",
            .description = "print this page",
            .handler = controller_help_handler,
        },
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
        {
            .name = "pairing",
            .description = "Commands for commissioning/unpair nodes.\n"
            "\tUsage: controller pairing onnetwork <nodeid> <pincode> OR\n"
            "\tcontroller pairing ble-wifi <nodeid> <ssid> <password> <pincode> <discriminator> OR\n"
            "\tcontroller pairing ble-thread <nodeid> <dataset> <pincode> <discriminator> OR\n"
            "\tcontroller pairing ble-thread <nodeid> <dataset> <pincode> <discriminator> OR\n"
            "\tcontroller pairing code <nodeid> <payload> OR\n"
            "\tcontroller pairing code-wifi <nodeid> <ssid> <password> <payload> OR\n"
            "\tcontroller pairing code-thread <nodeid> <dataset> <payload> OR\n"
            "\tcontroller pairing code-wifi-thread <nodeid> <ssid> <password> <dataset> <payload> OR\n"
            "\tcontroller pairing unpair <nodeid>",
            .handler = controller_pairing_handler,
        },
        {
            .name = "icd",
            .description = "icd client management.\n"
            "\tUsage: controller icd list",
            .handler = controller_icd_list_handler,
        },
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
        {
            .name = "udc",
            .description = "UDC command.\n"
            "\tUsage: controller udc reset OR\n"
            "\tcontroller udc print OR\n"
            "\tcontroller udc commission <pincode> <udc-entry>",
            .handler = controller_udc_handler,
        },
#endif
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#ifndef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
        {
            .name = "group-settings",
            .description = "Managing the groups and keysets of the controller.\n"
            "\tUsage: controller group-settings <sub-commands>",
            .handler = controller_group_settings_handler,
        },
#endif // !CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
        {
            .name = "open-commissioning-window",
            .description =
            "Send command to open basic/enhanced commissioning window\n"
            "\tUsage: controller open-commissioning-window <node-id> <option> <window-timeout> <iteration> "
            "<discriminator>\n"
            "\toption: 1 to use enhanced commissioning window. 0 to use basic commissioning window.\n"
            "\titeration: Number of PBKDF iterations to use to derive the verifier. Ignored if 'option' is 0.\n"
            "\tdiscriminator: Discriminator to use for advertising.  Ignored if 'option' is 0.",
            .handler = open_commissioning_window_handler,
        },
        {
            .name = "invoke-cmd",
            .description =
            "Send command to the nodes.\n"
            "\tUsage: controller invoke-cmd <node-id|group-id> <endpoint-id> <cluster-id> <command-id> "
            "[command_data] [timed_invoke_timeout_ms]\n"
            "\tNotes: group-id should start with prefix '0xFFFFFFFFFFFF', endpoint-id will be ignored if the fist "
            "parameter is group-id.\n"
            "\tNotes: The command_data should be a JSON object that includes all the command data fields defined "
            "in the "
            "SPEC. You can get the format of the payload from "
            "https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#cluster-commands\n"
            "\tNotes: The timed_invoke_timeout_ms should be used with command_data. If the command has no command "
            "data field, please use '\"{}\"' as the command_data ",
            .handler = controller_invoke_command_handler,
        },
        {
            .name = "read-attr",
            .description =
            "Read attributes of the nodes.\n"
            "\tUsage: controller read-attr <node-id> <endpoint-ids> <cluster-ids> <attr-ids>\n"
            "\tNotes: endpoint-ids can represent a single or multiple endpoints, e.g. '0' or '0,1'. "
            "And the same applies to cluster-ids, attr-ids, and event-ids.",
            .handler = controller_read_handler<chip::app::AttributePathParams>,
        },
        {
            .name = "write-attr",
            .description =
            "Write attributes of the nodes.\n"
            "\tUsage: controller write-attr <node-id> <endpoint-ids> <cluster-ids> <attr-ids> <attr-value> [timed_write_timeout_ms]\n"
            "\tNotes: attr-value should be a JSON object that contains the attribute value JSON item."
            " For multiple paths, use a JSON array with one object per path. "
            "You can get the format of the attr-value from "
            "https://docs.espressif.com/projects/esp-matter/en/latest/esp32/"
            "developing.html#write-attribute-commands",
            .handler = controller_write_attr_handler,
        },
        {
            .name = "read-event",
            .description = "Read events of the nodes.\n"
            "\tUsage: controller read-event <node-id> <endpoint-ids> <cluster-ids> <event-ids>",
            .handler = controller_read_handler<chip::app::EventPathParams>,
        },
        {
            .name = "sub-attr",
            .description =
            "Subscribe attributes of the nodes.\n"
            "\tUsage: controller sub-attr <node-id> <endpoint-ids> <cluster-ids> <attr-ids> "
            "<min-interval> <max-interval> [keep-subscription] [auto-resubscribe]\n"
            "\tNotes: If 'keep-subscription' is 'false', existing subscriptions will be terminated for the node. "
            "If 'auto-resubscribe' is 'true', controller will auto resubscribe if subscriptions timeout",
            .handler = controller_subscribe_handler<chip::app::AttributePathParams>,
        },
        {
            .name = "sub-event",
            .description = "Subscribe events of the nodes.\n"
            "\tUsage: controller sub-event <node-id> <endpoint-ids> <cluster-ids> <event-ids> "
            "<min-interval> <max-interval> [keep-subscription] [auto-resubscribe]\n"
            "\tNotes: 'keep-subscription' and 'auto-resubscribe' are the same as 'sub-attr' command",
            .handler = controller_subscribe_handler<chip::app::EventPathParams>,
        },
        {
            .name = "sub-list",
            .description = "List console-owned subscriptions, including connecting and retrying entries.\n"
            "\tUsage: controller sub-list",
            .handler = controller_list_subscriptions_handler,
        },
        {
            .name = "sub-remove",
            .description = "Remove one subscription, all for a node, or all globally.\n"
            "\tUsage: controller sub-remove <node-id> <subscription-id> OR\n"
            "\tcontroller sub-remove <node-id> all OR\n"
            "\tcontroller sub-remove all\n"
            "\tNotes: Node-specific removal uses the active fabric. Global removal covers all fabrics.\n"
            "\tRemoval also affects subscriptions created outside the console.",
            .handler = controller_remove_subscription_handler,
        },
    };

    const static command_t controller_command = {
        .name = "controller",
        .description = "Controller commands. Usage: matter esp controller <command_name>",
        .handler = controller_dispatch,
    };
    // Register the controller commands
    controller_console.register_commands(controller_sub_commands, sizeof(controller_sub_commands) / sizeof(command_t));
    return add_commands(&controller_command, 1);
}

} // namespace console
} // namespace esp_matter
