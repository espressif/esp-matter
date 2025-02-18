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

#include <esp_check.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_cluster_command.h>
#include <esp_matter_controller_commissioning_window_opener.h>
#include <esp_matter_controller_console.h>
#include <esp_matter_controller_group_settings.h>
#include <esp_matter_controller_pairing_command.h>
#include <esp_matter_controller_read_command.h>
#include <esp_matter_controller_subscribe_command.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_controller_write_command.h>
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

#if defined(CONFIG_ENABLE_ESP32_BLE_CONTROLLER) && defined(CONFIG_ESP_MATTER_COMMISSIONER_ENABLE)
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
#endif // defined(CONFIG_ENABLE_ESP32_BLE_CONTROLLER) && defined(CONFIG_ESP_MATTER_COMMISSIONER_ENABLE)

#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
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
        printf("pincode %ld", pincode);
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
            chip::Crypto::DRBG_get_bytes(reinterpret_cast<uint8_t *>(&gRemoteId), sizeof(gRemoteId));
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

static esp_err_t controller_invoke_command_handler(int argc, char **argv)
{
    if (argc < 4) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    uint16_t endpoint_id = string_to_uint16(argv[1]);
    uint32_t cluster_id = string_to_uint32(argv[2]);
    uint32_t command_id = string_to_uint32(argv[3]);

    if (argc > 5) {
        uint16_t timed_invoke_timeout_ms = string_to_uint16(argv[5]);
        if (timed_invoke_timeout_ms > 0) {
            return controller::send_invoke_cluster_command(node_id, endpoint_id, cluster_id, command_id, argv[4],
                                                           chip::MakeOptional(timed_invoke_timeout_ms));
        }
    }

    return controller::send_invoke_cluster_command(node_id, endpoint_id, cluster_id, command_id,
                                                   argc > 4 ? argv[4] : NULL);
}

static esp_err_t controller_read_attr_handler(int argc, char **argv)
{
    if (argc != 4) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> attribute_ids;
    ESP_RETURN_ON_ERROR(string_to_uint16_array(argv[1], endpoint_ids), TAG, "Failed to parse endpoint IDs");
    ESP_RETURN_ON_ERROR(string_to_uint32_array(argv[2], cluster_ids), TAG, "Failed to parse cluster IDs");
    ESP_RETURN_ON_ERROR(string_to_uint32_array(argv[3], attribute_ids), TAG, "Failed to parse attribute IDs");

    return controller::send_read_attr_command(node_id, endpoint_ids, cluster_ids, attribute_ids);
}

static esp_err_t controller_write_attr_handler(int argc, char **argv)
{
    if (argc < 5) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> attribute_ids;
    ESP_RETURN_ON_ERROR(string_to_uint16_array(argv[1], endpoint_ids), TAG, "Failed to parse endpoint IDs");
    ESP_RETURN_ON_ERROR(string_to_uint32_array(argv[2], cluster_ids), TAG, "Failed to parse cluster IDs");
    ESP_RETURN_ON_ERROR(string_to_uint32_array(argv[3], attribute_ids), TAG, "Failed to parse attribute IDs");

    char *attribute_val_str = argv[4];

    if (argc > 5) {
        uint16_t timed_write_timeout_ms = string_to_uint16(argv[5]);
        if (timed_write_timeout_ms > 0) {
            return controller::send_write_attr_command(node_id, endpoint_ids, cluster_ids, attribute_ids,
                                                       attribute_val_str, chip::MakeOptional(timed_write_timeout_ms));
        }
    }

    return controller::send_write_attr_command(node_id, endpoint_ids, cluster_ids, attribute_ids, attribute_val_str);
}

static esp_err_t controller_read_event_handler(int argc, char **argv)
{
    if (argc != 4) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> event_ids;
    ESP_RETURN_ON_ERROR(string_to_uint16_array(argv[1], endpoint_ids), TAG, "Failed to parse endpoint IDs");
    ESP_RETURN_ON_ERROR(string_to_uint32_array(argv[2], cluster_ids), TAG, "Failed to parse cluster IDs");
    ESP_RETURN_ON_ERROR(string_to_uint32_array(argv[3], event_ids), TAG, "Failed to parse event IDs");

    return controller::send_read_event_command(node_id, endpoint_ids, cluster_ids, event_ids);
}

static esp_err_t controller_subscribe_attr_handler(int argc, char **argv)
{
    if (argc < 6) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> attribute_ids;
    ESP_RETURN_ON_ERROR(string_to_uint16_array(argv[1], endpoint_ids), TAG, "Failed to parse endpoint IDs");
    ESP_RETURN_ON_ERROR(string_to_uint32_array(argv[2], cluster_ids), TAG, "Failed to parse cluster IDs");
    ESP_RETURN_ON_ERROR(string_to_uint32_array(argv[3], attribute_ids), TAG, "Failed to parse attribute IDs");
    uint16_t min_interval = string_to_uint16(argv[4]);
    uint16_t max_interval = string_to_uint16(argv[5]);

    bool keep_subscription = true;
    if (argc >= 7) {
        keep_subscription = string_to_bool(argv[6]);
    }

    bool auto_resubscribe = true;
    if (argc >= 8) {
        auto_resubscribe = string_to_bool(argv[7]);
    }

    return controller::send_subscribe_attr_command(node_id, endpoint_ids, cluster_ids, attribute_ids, min_interval,
                                                   max_interval, keep_subscription, auto_resubscribe);
}

static esp_err_t controller_subscribe_event_handler(int argc, char **argv)
{
    if (argc < 6) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> event_ids;
    ESP_RETURN_ON_ERROR(string_to_uint16_array(argv[1], endpoint_ids), TAG, "Failed to parse endpoint IDs");
    ESP_RETURN_ON_ERROR(string_to_uint32_array(argv[2], cluster_ids), TAG, "Failed to parse cluster IDs");
    ESP_RETURN_ON_ERROR(string_to_uint32_array(argv[3], event_ids), TAG, "Failed to parse event IDs");
    uint16_t min_interval = string_to_uint16(argv[4]);
    uint16_t max_interval = string_to_uint16(argv[5]);

    bool keep_subscription = true;
    if (argc >= 7) {
        keep_subscription = string_to_bool(argv[6]);
    }

    bool auto_resubscribe = true;
    if (argc >= 8) {
        auto_resubscribe = string_to_bool(argv[7]);
    }
    return controller::send_subscribe_event_command(node_id, endpoint_ids, cluster_ids, event_ids, min_interval,
                                                    max_interval, keep_subscription, auto_resubscribe);
}

static esp_err_t controller_shutdown_subscription_handler(int argc, char **argv)
{
    if (argc != 2) {
        return ESP_ERR_INVALID_ARG;
    }
    uint64_t node_id = string_to_uint64(argv[0]);
    uint32_t subscription_id = string_to_uint32(argv[1]);
    return controller::send_shutdown_subscription(node_id, subscription_id);
}

static esp_err_t controller_shutdown_subscriptions_handler(int argc, char **argv)
{
    if (argc != 1) {
        return ESP_ERR_INVALID_ARG;
    }
    uint64_t node_id = string_to_uint64(argv[0]);
    controller::send_shutdown_subscriptions(node_id);
    return ESP_OK;
}

static esp_err_t controller_shutdown_all_subscriptions_handler(int argc, char **argv)
{
    if (argc != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    controller::send_shutdown_all_subscriptions();
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
            .name = "group-settings",
            .description = "Managing the groups and keysets of the controller.\n"
                           "\tUsage: controller group-settings <sub-commands>",
            .handler = controller_group_settings_handler,
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
            .description = "Read attributes of the nodes.\n"
                           "\tUsage: controller read-attr <node-id> <endpoint-ids> <cluster-ids> <attr-ids>\n"
                           "\tNotes: endpoint-ids can represent a single or multiple endpoints, e.g. '0' or '0,1'. "
                           "And the same applies to cluster-ids, attr-ids, and event-ids.",
            .handler = controller_read_attr_handler,
        },
        {
            .name = "write-attr",
            .description =
                "Write attributes of the nodes.\n"
                "\tUsage: controller write-attr <node-id> <endpoint-ids> <cluster-ids> <attr-ids> <attr-value> [timed_write_timeout_ms]\n"
                "\tNotes: attr-value should be a JSON object that contains the attribute value JSON item."
                "You can get the format of the attr-value from "
                "https://docs.espressif.com/projects/esp-matter/en/latest/esp32/"
                "developing.html#write-attribute-commands",
            .handler = controller_write_attr_handler,
        },
        {
            .name = "read-event",
            .description = "Read events of the nodes.\n"
                           "\tUsage: controller read-event <node-id> <endpoint-ids> <cluster-ids> <event-ids>",
            .handler = controller_read_event_handler,
        },
        {
            .name = "subs-attr",
            .description = "Subscribe attributes of the nodes.\n"
                           "\tUsage: controller subs-attr <node-id> <endpoint-ids> <cluster-ids> <attr-ids> "
                           "<min-interval> <max-interval> [keep-subscription] [auto-resubscribe]\n"
                           "\tNotes: If 'keep-subscription' is 'false', existing subscriptions will be terminated for the node. "
                           "If 'auto-resubscribe' is 'true', controller will auto resubscribe if subscriptions timeout",
            .handler = controller_subscribe_attr_handler,
        },
        {
            .name = "subs-event",
            .description = "Subscribe events of the nodes.\n"
                           "\tUsage: controller subs-event <node-id> <endpoint-ids> <cluster-ids> <event-ids> "
                           "<min-interval> <max-interval> [keep-subscription] [auto-resubscribe]\n"
                           "\tNotes: 'keep-subscription' and 'auto-resubscribe' are the same as 'subs-attr' command",
            .handler = controller_subscribe_event_handler,
        },
        {
            .name = "shutdown-subs",
            .description = "Shutdown subscription for given node id and subscription id.\n"
                           "\tUsage: controller shutdown-subs <node-id> <subscription-id>",
            .handler = controller_shutdown_subscription_handler,
        },
        {
            .name = "shutdown-subss",
            .description = "Shutdown all subscriptions for a given node.\n"
                           "\tUsage: controller shutdown-subss <node-id>",
            .handler = controller_shutdown_subscriptions_handler,
        },
        {
            .name = "shutdown-all-subss",
            .description = "Shutdown all subscriptions to all nodes.\n"
                           "\tUsage: controller shutdown-all-subss",
            .handler = controller_shutdown_all_subscriptions_handler,
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
