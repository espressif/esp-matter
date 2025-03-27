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

#include <esp_check.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_group_settings.h>
#include <esp_matter_controller_utils.h>

using chip::FabricIndex;
using chip::KeysetId;
using chip::Credentials::GroupDataProvider;

constexpr char TAG[] = "groupsettings";

namespace esp_matter {
namespace controller {
namespace group_settings {

static bool find_keyset_id(FabricIndex fabric_index, uint16_t group_id, KeysetId &keyset_id)
{
    GroupDataProvider *group_data_provider = chip::Credentials::GetGroupDataProvider();
    auto iter = group_data_provider->IterateGroupKeys(fabric_index);
    GroupDataProvider::GroupKey group_key;
    while (iter->Next(group_key)) {
        if (group_key.group_id == group_id) {
            keyset_id = group_key.keyset_id;
            iter->Release();
            return true;
        }
    }
    iter->Release();
    return false;
}

esp_err_t show_groups()
{
    ESP_LOGI(TAG, "  +-------------------------------------------------------------------------------------+");
    ESP_LOGI(TAG, "  | Available Groups :                                                                  |");
    ESP_LOGI(TAG, "  +-------------------------------------------------------------------------------------+");
    ESP_LOGI(TAG, "  | Group Id   |  KeySet Id     |   Group Name                                          |");
    FabricIndex fabric_index = esp_matter::controller::matter_controller_client::get_instance().get_fabric_index();
    GroupDataProvider *group_data_provider = chip::Credentials::GetGroupDataProvider();
    auto iter = group_data_provider->IterateGroupInfo(fabric_index);
    GroupDataProvider::GroupInfo group_info;
    if (iter) {
        while (iter->Next(group_info)) {
            chip::KeysetId keyset_id;
            if (find_keyset_id(fabric_index, group_info.group_id, keyset_id)) {
                ESP_LOGI(TAG, "  | 0x%-12x  0x%-13x  %-50s |", group_info.group_id, keyset_id, group_info.name);
            } else {
                ESP_LOGI(TAG, "  | 0x%-12x  %-15s  %-50s |", group_info.group_id, "None", group_info.name);
            }
        }
        iter->Release();
    }
    ESP_LOGI(TAG, "  +-------------------------------------------------------------------------------------+");
    return ESP_OK;
}

esp_err_t add_group(char *group_name, uint16_t group_id)
{
    if (strlen(group_name) > CHIP_CONFIG_MAX_GROUP_NAME_LENGTH || group_id == chip::kUndefinedGroupId) {
        return ESP_ERR_INVALID_ARG;
    }

    FabricIndex fabric_index = esp_matter::controller::matter_controller_client::get_instance().get_fabric_index();
    GroupDataProvider *group_data_provider = chip::Credentials::GetGroupDataProvider();
    GroupDataProvider::GroupInfo group_info;

    group_info.SetName(group_name);
    group_info.group_id = group_id;
    ESP_RETURN_ON_FALSE(CHIP_NO_ERROR == group_data_provider->SetGroupInfo(fabric_index, group_info), ESP_FAIL, TAG,
                        "Failed to set the group info");
    return ESP_OK;
}

esp_err_t remove_group(uint16_t group_id)
{
    if (group_id == chip::kUndefinedGroupId) {
        return ESP_ERR_INVALID_ARG;
    }

    FabricIndex fabric_index = esp_matter::controller::matter_controller_client::get_instance().get_fabric_index();
    GroupDataProvider *group_data_provider = chip::Credentials::GetGroupDataProvider();
    ESP_RETURN_ON_FALSE(CHIP_NO_ERROR == group_data_provider->RemoveGroupInfo(fabric_index, group_id), ESP_FAIL, TAG,
                        "Failed to remove the group info");
    return ESP_OK;
}

esp_err_t show_keysets()
{
    FabricIndex fabric_index = esp_matter::controller::matter_controller_client::get_instance().get_fabric_index();
    GroupDataProvider *group_data_provider = chip::Credentials::GetGroupDataProvider();
    GroupDataProvider::KeySet keyset;

    ESP_LOGI(TAG, "  +-------------------------------------------------------------------------------------+");
    ESP_LOGI(TAG, "  | Available KeySets :                                                                 |");
    ESP_LOGI(TAG, "  +-------------------------------------------------------------------------------------+");
    ESP_LOGI(TAG, "  | KeySet Id   |   Key Policy                                                          |");

    auto iter = group_data_provider->IterateKeySets(fabric_index);
    if (iter) {
        while (iter->Next(keyset)) {
            ESP_LOGI(TAG, "  | 0x%-12x  %-66s  |", keyset.keyset_id,
                     (keyset.policy == GroupDataProvider::SecurityPolicy::kCacheAndSync) ? "Cache and Sync"
                                                                                         : "Trust First");
        }
        iter->Release();
    }
    ESP_LOGI(TAG, "  +-------------------------------------------------------------------------------------+");
    return ESP_OK;
}

esp_err_t bind_keyset(uint16_t group_id, uint16_t keyset_id)
{
    size_t current_count = 0;
    FabricIndex fabric_index = esp_matter::controller::matter_controller_client::get_instance().get_fabric_index();
    GroupDataProvider *group_data_provider = chip::Credentials::GetGroupDataProvider();

    auto iter = group_data_provider->IterateGroupKeys(fabric_index);
    current_count = iter->Count();
    iter->Release();

    if (CHIP_NO_ERROR !=
        group_data_provider->SetGroupKeyAt(fabric_index, current_count,
                                           GroupDataProvider::GroupKey(group_id, keyset_id))) {
        ESP_LOGE(TAG, "Failed to bind keyset");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t unbind_keyset(uint16_t group_id, uint16_t keyset_id)
{
    size_t index = 0;
    FabricIndex fabric_index = esp_matter::controller::matter_controller_client::get_instance().get_fabric_index();
    GroupDataProvider *group_data_provider = chip::Credentials::GetGroupDataProvider();

    auto iter = group_data_provider->IterateGroupKeys(fabric_index);
    size_t max_count = iter->Count();
    GroupDataProvider::GroupKey group_key;
    while (iter->Next(group_key)) {
        if (group_key.group_id == group_id && group_key.keyset_id == keyset_id) {
            break;
        }
        index++;
    }
    iter->Release();
    ESP_RETURN_ON_FALSE(index < max_count, ESP_ERR_NOT_FOUND, TAG, "Failed to find the group key");
    ESP_RETURN_ON_FALSE(CHIP_NO_ERROR == group_data_provider->RemoveGroupKeyAt(fabric_index, index), ESP_FAIL, TAG,
                        "Failed to remove the group key");
    return ESP_OK;
}

esp_err_t add_keyset(uint16_t keyset_id, uint8_t key_policy, uint64_t validity_time, char *epoch_key_oct_str)
{
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    FabricIndex fabric_index = controller_instance.get_fabric_index();

    GroupDataProvider *group_data_provider = chip::Credentials::GetGroupDataProvider();
    uint8_t compressed_fabric_id[sizeof(uint64_t)];
    chip::MutableByteSpan compressed_fabric_id_span(compressed_fabric_id);
    if (CHIP_NO_ERROR !=
#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
        controller_instance.get_commissioner()->GetCompressedFabricIdBytes(compressed_fabric_id_span)) {
#else
        controller_instance.get_controller()->GetCompressedFabricIdBytes(compressed_fabric_id_span)) {
#endif
        ESP_LOGE(TAG, "Failed to get the compressed fabric_id");
        return ESP_FAIL;
    }
    if (key_policy != 0 && key_policy != 1) {
        return ESP_ERR_INVALID_ARG;
    }
    GroupDataProvider::KeySet keyset(keyset_id, GroupDataProvider::SecurityPolicy(key_policy), 1);
    GroupDataProvider::EpochKey epoch_key;
    epoch_key.start_time = validity_time;
    uint8_t epoch_key_buf[GroupDataProvider::EpochKey::kLengthBytes];
    if (oct_str_to_byte_arr(epoch_key_oct_str, epoch_key_buf) != GroupDataProvider::EpochKey::kLengthBytes) {
        ESP_LOGE(TAG, "The epoch key octstring is wrong");
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(epoch_key.key, epoch_key_buf, GroupDataProvider::EpochKey::kLengthBytes);
    memcpy(keyset.epoch_keys, &epoch_key, sizeof(GroupDataProvider::EpochKey));
    if (CHIP_NO_ERROR != group_data_provider->SetKeySet(fabric_index, compressed_fabric_id_span, keyset)) {
        ESP_LOGE(TAG, "Failed to set keyset");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t remove_keyset(uint16_t keyset_id)
{
    FabricIndex fabric_index = esp_matter::controller::matter_controller_client::get_instance().get_fabric_index();
    GroupDataProvider *group_data_provider = chip::Credentials::GetGroupDataProvider();

    size_t index = 0;
    CHIP_ERROR err = CHIP_NO_ERROR;
    auto iter = group_data_provider->IterateGroupKeys(fabric_index);
    GroupDataProvider::GroupKey group_key;
    while (iter->Next(group_key)) {
        if (group_key.keyset_id == keyset_id) {
            err = group_data_provider->RemoveGroupKeyAt(fabric_index, index);
            if (err != CHIP_NO_ERROR) {
                break;
            }
        }
        index++;
    }
    iter->Release();

    if (err == CHIP_NO_ERROR) {
        return ESP_OK;
    }
    ESP_RETURN_ON_FALSE(CHIP_NO_ERROR == group_data_provider->RemoveKeySet(fabric_index, keyset_id), ESP_FAIL, TAG,
                        "Failed to remove the keyset");

    return ESP_OK;
}

} // namespace group_settings
} // namespace controller
} // namespace esp_matter
