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

#include <DataModelLogger.h>
#include <controller/CommissioneeDeviceProxy.h>
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#include <esp_matter_commissioner.h>
#else
#include <app/server/Server.h>
#endif
#include <esp_check.h>
#include <esp_matter_controller_cluster_command.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_mem.h>
#include <json_parser.h>

#include <crypto/CHIPCryptoPAL.h>
#include <setup_payload/ManualSetupPayloadGenerator.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>
#include <setup_payload/SetupPayload.h>

using namespace chip::app::Clusters;
static const char *TAG = "cluster_command";

namespace esp_matter {
namespace controller {

namespace clusters {

namespace on_off {

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    if (command_data->command_data_count != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    switch (command_data->command_id) {
    case OnOff::Commands::On::Id:
        return esp_matter::cluster::on_off::command::send_on(remote_device, remote_endpoint_id);
    case OnOff::Commands::Off::Id:
        return esp_matter::cluster::on_off::command::send_off(remote_device, remote_endpoint_id);
    case OnOff::Commands::Toggle::Id:
        return esp_matter::cluster::on_off::command::send_toggle(remote_device, remote_endpoint_id);
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t send_group_command(command_data_t *command_data, uint16_t group_id)
{
    if (command_data->command_data_count != 0) {
        return ESP_ERR_INVALID_ARG;
    }
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    uint8_t fabric_index = commissioner::get_device_commissioner()->GetFabricIndex();
#else
    uint8_t fabric_index = get_fabric_index();
#endif
    switch (command_data->command_id) {
    case OnOff::Commands::On::Id:
        return esp_matter::cluster::on_off::command::group_send_on(fabric_index, group_id);
        break;
    case OnOff::Commands::Off::Id:
        return esp_matter::cluster::on_off::command::group_send_off(fabric_index, group_id);
        break;
    case OnOff::Commands::Toggle::Id:
        return esp_matter::cluster::on_off::command::group_send_toggle(fabric_index, group_id);
        break;
    default:
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace on_off

namespace level_control {

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case LevelControl::Commands::Move::Id:
        if (command_data->command_data_count != 4) {
            ESP_LOGE(TAG,
                     "The command data should be specified in following order: move_mode, rate, option_mask, "
                     "option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::level_control::command::send_move(
            remote_device, remote_endpoint_id,
            /* move_mode */ string_to_uint8(command_data->command_data_str[0]),
            /* rate*/ string_to_uint8(command_data->command_data_str[1]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[2]),
            /* option_override*/ string_to_uint8(command_data->command_data_str[3]));
        break;
    case LevelControl::Commands::MoveToLevel::Id:
        if (command_data->command_data_count != 4) {
            ESP_LOGE(TAG,
                     "The command data should be specified in following order: level, transition_time, option_mask, "
                     "option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::level_control::command::send_move_to_level(
            remote_device, remote_endpoint_id,
            /* level */ string_to_uint8(command_data->command_data_str[0]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[1]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[2]),
            /* option_override */ string_to_uint8(command_data->command_data_str[3]));
        break;
    case LevelControl::Commands::Step::Id:
        if (command_data->command_data_count != 5) {
            ESP_LOGE(TAG,
                     "The command data should be specified in following order: step_mode, step_size, transition_time, "
                     "option_mask, option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::level_control::command::send_step(
            remote_device, remote_endpoint_id,
            /* step_mode */ string_to_uint8(command_data->command_data_str[0]),
            /* step_size */ string_to_uint8(command_data->command_data_str[1]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[2]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[3]),
            /* option_override */ string_to_uint8(command_data->command_data_str[4]));
        break;
    case LevelControl::Commands::Stop::Id:
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command data should be specified in following order: option_mask, option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::level_control::command::send_stop(
            remote_device, remote_endpoint_id,
            /* option_mask */ string_to_uint8(command_data->command_data_str[0]),
            /* option_override */ string_to_uint8(command_data->command_data_str[1]));
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace level_control

namespace color_control {

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case ColorControl::Commands::MoveToHue::Id:
        if (command_data->command_data_count != 5) {
            ESP_LOGE(TAG,
                     "The command data should be specified in following order: hue, direction, transition_time, "
                     "option_mask, option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::color_control::command::send_move_to_hue(
            remote_device, remote_endpoint_id,
            /* hue */ string_to_uint8(command_data->command_data_str[0]),
            /* direction */ string_to_uint8(command_data->command_data_str[1]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[2]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[3]),
            /* option_override */ string_to_uint8(command_data->command_data_str[4]));
        break;
    case ColorControl::Commands::MoveToSaturation::Id:
        if (command_data->command_data_count != 4) {
            ESP_LOGE(
                TAG,
                "The command data should be specified in following order: saturation, transition_time, option_mask, "
                "option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::color_control::command::send_move_to_saturation(
            remote_device, remote_endpoint_id,
            /* saturation */ string_to_uint8(command_data->command_data_str[0]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[1]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[2]),
            /* option_override */ string_to_uint8(command_data->command_data_str[3]));
        break;
    case ColorControl::Commands::MoveToHueAndSaturation::Id:
        if (command_data->command_data_count != 5) {
            ESP_LOGE(TAG,
                     "The command data should be specified in following order: hue, saturation, transition_time, "
                     "option_mask, "
                     "option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::color_control::command::send_move_to_hue_and_saturation(
            remote_device, remote_endpoint_id,
            /* hue */ string_to_uint8(command_data->command_data_str[0]),
            /* saturation */ string_to_uint8(command_data->command_data_str[1]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[2]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[3]),
            /* option_override */ string_to_uint8(command_data->command_data_str[4]));
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace color_control

namespace group_key_management {
using chip::app::Clusters::GroupKeyManagement::GroupKeySecurityPolicyEnum;
using cluster::group_key_management::command::group_keyset_struct;

constexpr size_t k_epoch_key_bytes_len = chip::Credentials::GroupDataProvider::EpochKey::kLengthBytes;

typedef struct epoch_keys {
    uint8_t epoch_key0_bytes[k_epoch_key_bytes_len];
    uint8_t epoch_key1_bytes[k_epoch_key_bytes_len];
    uint8_t epoch_key2_bytes[k_epoch_key_bytes_len];
} epoch_keys_t;

static bool parse_group_keyset(group_keyset_struct *keyset, char *json_str, epoch_keys_t *keys)
{
    jparse_ctx_t jctx;
    if (json_parse_start(&jctx, json_str, strlen(json_str)) != 0) {
        return false;
    }
    int int_val;
    int64_t int64_val;
    char epoch_key_oct_str[k_epoch_key_bytes_len * 2 + 1];
    // groupKeySetID
    if (json_obj_get_int(&jctx, "groupKeySetID", &int_val) != 0) {
        return false;
    }
    keyset->groupKeySetID = int_val;
    // groupKeySecurityPolicy
    if (json_obj_get_int(&jctx, "groupKeySecurityPolicy", &int_val) != 0) {
        return false;
    }
    keyset->groupKeySecurityPolicy = static_cast<GroupKeySecurityPolicyEnum>(int_val);
    // epochKey0 & epochStartTime0
    if (json_obj_get_int64(&jctx, "epochStartTime0", &int64_val) == 0 &&
        json_obj_get_string(&jctx, "epochKey0", epoch_key_oct_str, k_epoch_key_bytes_len * 2 + 1) == 0 &&
        oct_str_to_byte_arr(epoch_key_oct_str, keys->epoch_key0_bytes) == k_epoch_key_bytes_len) {
        keyset->epochKey0.SetNonNull(chip::ByteSpan(keys->epoch_key0_bytes, k_epoch_key_bytes_len));
        keyset->epochStartTime0.SetNonNull(int64_val);
    } else {
        keyset->epochKey0.SetNull();
        keyset->epochStartTime0.SetNull();
    }
    // epochKey1 & epochStartTime1
    if (json_obj_get_int64(&jctx, "epochStartTime1", &int64_val) == 0 &&
        json_obj_get_string(&jctx, "epochKey1", epoch_key_oct_str, k_epoch_key_bytes_len * 2 + 1) == 0 &&
        oct_str_to_byte_arr(epoch_key_oct_str, keys->epoch_key1_bytes) == k_epoch_key_bytes_len) {
        keyset->epochKey1.SetNonNull(chip::ByteSpan(keys->epoch_key1_bytes, k_epoch_key_bytes_len));
        keyset->epochStartTime1.SetNonNull(int64_val);
    } else {
        keyset->epochKey1.SetNull();
        keyset->epochStartTime1.SetNull();
    }
    // epochKey2 & epochStartTime2
    if (json_obj_get_int64(&jctx, "epochStartTime2", &int64_val) == 0 &&
        json_obj_get_string(&jctx, "epochKey2", epoch_key_oct_str, k_epoch_key_bytes_len * 2 + 1) == 0 &&
        oct_str_to_byte_arr(epoch_key_oct_str, keys->epoch_key2_bytes) == k_epoch_key_bytes_len) {
        keyset->epochKey2.SetNonNull(chip::ByteSpan(keys->epoch_key2_bytes, k_epoch_key_bytes_len));
        keyset->epochStartTime2.SetNonNull(int64_val);
    } else {
        keyset->epochKey2.SetNull();
        keyset->epochStartTime2.SetNull();
    }
    return true;
}

static void keyset_read_response_callback(void *ctx,
                                          const GroupKeyManagement::Commands::KeySetRead::Type::ResponseType &response)
{
    DataModelLogger::LogValue("groupKeySet", 1, response);
}

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case GroupKeyManagement::Commands::KeySetWrite::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: group_keyset");
            return ESP_ERR_INVALID_ARG;
        }
        group_keyset_struct keyset_struct;
        epoch_keys_t keys;
        if (!parse_group_keyset(&keyset_struct, command_data->command_data_str[0], &keys)) {
            ESP_LOGE(TAG, "Failed to parse the group_keyset json string");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::group_key_management::command::send_keyset_write(remote_device, remote_endpoint_id,
                                                                                     keyset_struct);
        break;
    }
    case GroupKeyManagement::Commands::KeySetRead::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: group_keyset_id");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::group_key_management::command::send_keyset_read(
            remote_device, remote_endpoint_id,
            /* group_keyset_id */ string_to_uint16(command_data->command_data_str[0]), keyset_read_response_callback);
        break;
    }
    default:
        return ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace group_key_management

namespace groups {

static void add_group_response_callback(void *ctx, const Groups::Commands::AddGroup::Type::ResponseType &response)
{
    DataModelLogger::LogValue("addGroupResponse", 1, response);
}

static void view_group_response_callback(void *ctx, const Groups::Commands::ViewGroup::Type::ResponseType &response)
{
    DataModelLogger::LogValue("viewGroupResponse", 1, response);
}

static void remove_group_response_callback(void *ctx, const Groups::Commands::RemoveGroup::Type::ResponseType &response)
{
    DataModelLogger::LogValue("removeGroupResponse", 1, response);
}

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case Groups::Commands::AddGroup::Id: {
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command data should be specified in following order: group_id group_name");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::groups::command::send_add_group(
            remote_device, remote_endpoint_id,
            /* group_id */ string_to_uint16(command_data->command_data_str[0]),
            /* group_name */ command_data->command_data_str[1], add_group_response_callback);
        break;
    }
    case Groups::Commands::ViewGroup::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: group_id");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::groups::command::send_view_group(
            remote_device, remote_endpoint_id,
            /* group_id */ string_to_uint16(command_data->command_data_str[0]), view_group_response_callback);
        break;
    }
    case Groups::Commands::RemoveGroup::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: group_id");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::groups::command::send_remove_group(
            remote_device, remote_endpoint_id,
            /* group_id */ string_to_uint16(command_data->command_data_str[0]), remove_group_response_callback);
        break;
    }
    default:
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace groups

namespace identify {

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case Identify::Commands::Identify::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: identify_time");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::identify::command::send_identify(
            remote_device, remote_endpoint_id, /* identify_time */ string_to_uint16(command_data->command_data_str[0]));
        break;
    }
    case Identify::Commands::TriggerEffect::Id: {
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command data should in be specified following order: effect_identifier effect_variant");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::identify::command::send_trigger_effect(
            remote_device, remote_endpoint_id,
            /* effect_identifier */ string_to_uint8(command_data->command_data_str[0]),
            /* effect_variant */ string_to_uint8(command_data->command_data_str[1]));
        break;
    }
    default:
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace identify

namespace scenes {

static void add_scene_success_callback(void *ctx, const Scenes::Commands::AddScene::Type::ResponseType &response)
{
    DataModelLogger::LogValue("addSceneResponse", 1, response);
}

static void view_scene_success_callback(void *ctx, const Scenes::Commands::ViewScene::Type::ResponseType &response)
{
    DataModelLogger::LogValue("viewSceneResponse", 1, response);
}

static void remove_scene_success_callback(void *ctx, const Scenes::Commands::RemoveScene::Type::ResponseType &response)
{
    DataModelLogger::LogValue("removeSceneResponse", 1, response);
}

static void remove_all_scenes_success_callback(void *ctx,
                                               const Scenes::Commands::RemoveAllScenes::Type::ResponseType &response)
{
    DataModelLogger::LogValue("removeAllScenesResponse", 1, response);
}

static void store_scene_success_callback(void *ctx, const Scenes::Commands::StoreScene::Type::ResponseType &response)
{
    DataModelLogger::LogValue("storeSceneResponse", 1, response);
}

static void get_scene_membership_success_callback(
    void *ctx, const Scenes::Commands::GetSceneMembership::Type::ResponseType &response)
{
    DataModelLogger::LogValue("getSceneMembershipResponse", 1, response);
}

using extension_field_set = chip::app::Clusters::Scenes::Structs::ExtensionFieldSet::Type;
using attribute_value_pair = chip::app::Clusters::Scenes::Structs::AttributeValuePair::Type;
static constexpr uint8_t max_cluster_per_scene = CHIP_CONFIG_SCENES_MAX_CLUSTERS_PER_SCENE;
static constexpr uint8_t max_attr_per_cluster = CHIP_CONFIG_SCENES_MAX_EXTENSION_FIELDSET_SIZE_PER_CLUSTER / 10;

static bool parse_extension_field_sets(char *json_str, extension_field_set efs_array[],
                                       attribute_value_pair avp_arrays[][max_attr_per_cluster], size_t &efs_size)
{
    jparse_ctx_t jctx;
    if (json_parse_start(&jctx, json_str, strlen(json_str)) != 0) {
        return false;
    }
    size_t index = 0;
    while (index < max_cluster_per_scene && json_arr_get_object(&jctx, index) == 0) {
        int int_val;
        int64_t int64_val;
        int attr_val_list_size = 0;
        if (json_obj_get_int(&jctx, "clusterID", &int_val) != 0) {
            json_arr_leave_object(&jctx);
            json_parse_end(&jctx);
            return false;
        }
        efs_array[index].clusterID = int_val;
        if (json_obj_get_array(&jctx, "attributeValueList", &attr_val_list_size) == 0) {
            for (size_t attr_index = 0; attr_index < attr_val_list_size; ++attr_index) {
                if (json_arr_get_object(&jctx, attr_index) == 0) {
                    if (json_obj_get_int(&jctx, "attributeID", &int_val) == 0) {
                        avp_arrays[index][attr_index].attributeID = int_val;
                    }
                    if (json_obj_get_int64(&jctx, "attributeValue", &int64_val) == 0) {
                        avp_arrays[index][attr_index].attributeValue = (uint32_t)int64_val;
                    }
                    json_arr_leave_object(&jctx);
                }
            }
            efs_array[index].attributeValueList =
                chip::app::DataModel::List<attribute_value_pair>(avp_arrays[index], attr_val_list_size);
            json_obj_leave_array(&jctx);
        } else {
            json_arr_leave_object(&jctx);
            json_parse_end(&jctx);
            return false;
        }
        json_arr_leave_object(&jctx);
        index++;
    }
    efs_size = index;
    json_parse_end(&jctx);
    return true;
}

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case Scenes::Commands::AddScene::Id: {
        if (command_data->command_data_count != 5) {
            ESP_LOGE(
                TAG,
                "The command data should be specified in following order: group_id scene_id transition_time scene_name "
                "extension_field_sets");
            return ESP_ERR_INVALID_ARG;
        }
        extension_field_set efs_array[max_cluster_per_scene];
        attribute_value_pair avp_arrays[max_cluster_per_scene][max_attr_per_cluster];
        size_t efs_size = 0;
        if (!parse_extension_field_sets(command_data->command_data_str[4], efs_array, avp_arrays, efs_size)) {
            ESP_LOGE(TAG, "Failed to Parse extension_field_sets");
            return ESP_ERR_INVALID_ARG;
        }
        cluster::scenes::command::extension_field_sets efs(efs_array, efs_size);

        return esp_matter::cluster::scenes::command::send_add_scene(
            remote_device, remote_endpoint_id,
            /* group_id */ string_to_uint16(command_data->command_data_str[0]),
            /* scene_id */ string_to_uint8(command_data->command_data_str[1]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[2]),
            /* scene_name */ command_data->command_data_str[3], efs, add_scene_success_callback);
        break;
    }
    case Scenes::Commands::ViewScene::Id: {
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command data be specified should in following order: group_id scene_id");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::scenes::command::send_view_scene(
            remote_device, remote_endpoint_id,
            /* group_id */ string_to_uint16(command_data->command_data_str[0]),
            /* scene_id */ string_to_uint8(command_data->command_data_str[1]), view_scene_success_callback);
        break;
    }
    case Scenes::Commands::RemoveScene::Id: {
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command data be specified should in following order: group_id scene_id");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::scenes::command::send_remove_scene(
            remote_device, remote_endpoint_id,
            /* group_id */ string_to_uint16(command_data->command_data_str[0]),
            /* scene_id */ string_to_uint8(command_data->command_data_str[1]), remove_scene_success_callback);
        break;
    }
    case Scenes::Commands::RemoveAllScenes::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data be specified should in following order: group_id");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::scenes::command::send_remove_all_scenes(
            remote_device, remote_endpoint_id,
            /* group_id */ string_to_uint16(command_data->command_data_str[0]), remove_all_scenes_success_callback);
        break;
    }
    case Scenes::Commands::StoreScene::Id: {
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command data should be specified in following order: group_id scene_id");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::scenes::command::send_store_scene(
            remote_device, remote_endpoint_id,
            /* group_id */ string_to_uint16(command_data->command_data_str[0]),
            /* scene_id */ string_to_uint8(command_data->command_data_str[1]), store_scene_success_callback);
        break;
    }
    case Scenes::Commands::RecallScene::Id: {
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command data should be specified in following order: group_id scene_id");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::scenes::command::send_recall_scene(
            remote_device, remote_endpoint_id,
            /* group_id */ string_to_uint16(command_data->command_data_str[0]),
            /* scene_id */ string_to_uint8(command_data->command_data_str[1]));
        break;
    }
    case Scenes::Commands::GetSceneMembership::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: group_id");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::scenes::command::send_get_scene_membership(
            remote_device, remote_endpoint_id,
            /* group_id */ string_to_uint16(command_data->command_data_str[0]), get_scene_membership_success_callback);
        break;
    }
    default:
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace scenes

namespace thermostat {

static void get_weekly_schedule_success_callback(
    void *ctx, const Thermostat::Commands::GetWeeklySchedule::Type::ResponseType &response)
{
    DataModelLogger::LogValue("getWeeklyScheduleResponse", 1, response);
}

static constexpr uint8_t max_transitions_len = 10;
using schedule_transition = chip::app::Clusters::Thermostat::Structs::WeeklyScheduleTransitionStruct::Type;

static bool parse_schedule_transition_list(char *json_str, schedule_transition *transition_array,
                                           size_t &transitions_len)
{
    jparse_ctx_t jctx;
    if (json_parse_start(&jctx, json_str, strlen(json_str)) != 0) {
        return false;
    }
    size_t index = 0;
    while (index < max_transitions_len && json_arr_get_object(&jctx, index) == 0) {
        int int_val;
        if (json_obj_get_int(&jctx, "transitionTime", &int_val) != 0) {
            json_arr_leave_object(&jctx);
            json_parse_end(&jctx);
            return false;
        }
        transition_array[index].transitionTime = int_val;

        if (json_obj_get_int(&jctx, "coolSetpoint", &int_val) == 0) {
            transition_array[index].coolSetpoint.SetNonNull((int16_t)int_val);
        } else {
            transition_array[index].coolSetpoint.SetNull();
        }

        if (json_obj_get_int(&jctx, "heatSetpoint", &int_val) == 0) {
            transition_array[index].heatSetpoint.SetNonNull((int16_t)int_val);
        } else {
            transition_array[index].heatSetpoint.SetNull();
        }

        json_arr_leave_object(&jctx);
        index++;
    }
    transitions_len = index;
    json_parse_end(&jctx);
    return true;
}

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case Thermostat::Commands::SetpointRaiseLower::Id: {
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command data should be specified in following order: mode amount");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::thermostat::command::send_setpoint_raise_lower(
            remote_device, remote_endpoint_id, /* mode */ string_to_uint8(command_data->command_data_str[0]),
            /* amount */ string_to_uint8(command_data->command_data_str[1]));
        break;
    }
    case Thermostat::Commands::SetWeeklySchedule::Id: {
        if (command_data->command_data_count != 4) {
            ESP_LOGE(TAG,
                     "The command data should be specified in following order: number_of_transitions_for_sequence "
                     "day_of_week_for_sequence mode_for_sequence transitions");
            return ESP_ERR_INVALID_ARG;
        }
        schedule_transition transition_array[max_transitions_len];
        size_t transitions_len = 0;
        if (!parse_schedule_transition_list(command_data->command_data_str[3], transition_array, transitions_len)) {
            ESP_LOGE(TAG, "Failed to parse schedule_transition_list");
            return ESP_ERR_INVALID_ARG;
        }
        cluster::thermostat::command::transitions trans(transition_array, transitions_len);
        return esp_matter::cluster::thermostat::command::send_set_weekly_schedule(
            remote_device, remote_endpoint_id,
            /* number_of_transitions_for_sequence */ string_to_uint8(command_data->command_data_str[0]),
            /* day_of_week_for_sequence */ string_to_uint8(command_data->command_data_str[1]),
            /* mode_for_sequence */ string_to_uint8(command_data->command_data_str[2]),
            /* transitions */ trans);
        break;
    }
    case Thermostat::Commands::GetWeeklySchedule::Id: {
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command data be specified should in following order: data_to_return mode_to_return");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::thermostat::command::send_get_weekly_schedule(
            remote_device, remote_endpoint_id, /* data_to_return */ string_to_uint8(command_data->command_data_str[0]),
            /* mode_to_return */ string_to_uint8(command_data->command_data_str[1]),
            get_weekly_schedule_success_callback);

        break;
    }
    case Thermostat::Commands::ClearWeeklySchedule::Id: {
        return esp_matter::cluster::thermostat::command::send_clear_weekly_schedule(remote_device, remote_endpoint_id);
        break;
    }
    default:
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace thermostat

namespace door_lock {

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case DoorLock::Commands::LockDoor::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data be specified should in following order: timed_invoke_timeout_ms");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::door_lock::command::send_lock_door(
            remote_device, remote_endpoint_id,
            /* timed_invoke_timeout_ms */ string_to_uint16(command_data->command_data_str[0]));
        break;
    }
    case DoorLock::Commands::UnlockDoor::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: timed_invoke_timeout_ms");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::door_lock::command::send_unlock_door(
            remote_device, remote_endpoint_id,
            /* timed_invoke_timeout_ms */ string_to_uint16(command_data->command_data_str[0]));
        break;
    }
    case DoorLock::Commands::UnlockWithTimeout::Id: {
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command data should be specified in following order: timeout timed_invoke_timeout_ms");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::door_lock::command::send_unlock_with_timeout(
            remote_device, remote_endpoint_id,
            /* timeout */ string_to_uint16(command_data->command_data_str[0]),
            /* timed_invoke_timeout_ms */ string_to_uint16(command_data->command_data_str[1]));
        break;
    }
    default:
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace door_lock

namespace window_covering {

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case WindowCovering::Commands::UpOrOpen::Id: {
        return esp_matter::cluster::window_covering::command::send_up_or_open(remote_device, remote_endpoint_id);
        break;
    }
    case WindowCovering::Commands::DownOrClose::Id: {
        return esp_matter::cluster::window_covering::command::send_down_or_close(remote_device, remote_endpoint_id);
        break;
    }
    case WindowCovering::Commands::StopMotion::Id: {
        return esp_matter::cluster::window_covering::command::send_stop_motion(remote_device, remote_endpoint_id);
        break;
    }
    case WindowCovering::Commands::GoToLiftValue::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: lift_value");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::window_covering::command::send_go_to_lift_value(
            remote_device, remote_endpoint_id, string_to_uint16(command_data->command_data_str[0]));
        break;
    }
    case WindowCovering::Commands::GoToLiftPercentage::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: lift_percentage");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::window_covering::command::send_go_to_lift_percentage(
            remote_device, remote_endpoint_id, string_to_uint16(command_data->command_data_str[0]));
        break;
    }
    case WindowCovering::Commands::GoToTiltValue::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: tilt_value");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::window_covering::command::send_go_to_tilt_value(
            remote_device, remote_endpoint_id, string_to_uint16(command_data->command_data_str[0]));
        break;
    }
    case WindowCovering::Commands::GoToTiltPercentage::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: tilt_percentage");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::window_covering::command::send_go_to_tilt_percentage(
            remote_device, remote_endpoint_id, string_to_uint16(command_data->command_data_str[0]));
        break;
    }
    default:
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace window_covering

namespace administrator_commissioning {

constexpr uint16_t k_default_timed_interaction_timeout = 5000;

static esp_err_t generate_pase_verifier(uint32_t iteration, uint32_t &pincode, chip::MutableByteSpan &salt,
                                        chip::Crypto::Spake2pVerifier &verifier)
{
    if (chip::Crypto::DRBG_get_bytes(salt.data(), salt.size()) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to generate salt");
        return ESP_FAIL;
    }
    if (chip::PASESession::GeneratePASEVerifier(verifier, iteration, salt, true, pincode) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to generate PASE verifier");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t print_manual_code(uint32_t pincode, uint16_t discriminator)
{
    chip::SetupPayload payload = chip::SetupPayload();
    payload.setUpPINCode = pincode;
    payload.version = 0;
    payload.discriminator.SetLongValue(discriminator);
    payload.rendezvousInformation.SetValue(chip::RendezvousInformationFlag::kOnNetwork);
    char payload_buffer[chip::QRCodeBasicSetupPayloadGenerator::kMaxQRCodeBase38RepresentationLength + 1];
    chip::MutableCharSpan manual_code(payload_buffer);
    CHIP_ERROR err = chip::ManualSetupPayloadGenerator(payload).payloadDecimalStringRepresentation(manual_code);
    if (err == CHIP_NO_ERROR) {
        ESP_LOGI(TAG, "****************Manual code:[%s]********************", payload_buffer);
    } else {
        ESP_LOGE(TAG, "Unable to generate manual code for setup payload");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case AdministratorCommissioning::Commands::OpenCommissioningWindow::Id: {
        if (command_data->command_data_count != 3) {
            ESP_LOGE(TAG,
                     "The command data should be specified in following order: commissioning_timeout iteration "
                     "discriminator");
            return ESP_ERR_INVALID_ARG;
        }
        uint16_t commissioning_timeout = string_to_uint16(command_data->command_data_str[0]);
        uint32_t iteration = string_to_uint32(command_data->command_data_str[1]);
        uint16_t discriminator = string_to_uint16(command_data->command_data_str[2]);
        uint8_t salt_buffer[chip::Crypto::kSpake2p_Max_PBKDF_Salt_Length];
        chip::MutableByteSpan salt = chip::MutableByteSpan(salt_buffer);
        uint32_t pincode = 0;
        chip::Crypto::Spake2pVerifier verifier;
        if (generate_pase_verifier(iteration, pincode, salt, verifier) != ESP_OK) {
            return ESP_ERR_INVALID_ARG;
        }
        print_manual_code(pincode, discriminator);

        chip::Spake2pVerifierSerialized serialized_verifier;
        chip::MutableByteSpan serialized_verifier_span(serialized_verifier);
        if (verifier.Serialize(serialized_verifier_span) != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Failed to serialize the verifier");
            return ESP_FAIL;
        }
        return esp_matter::cluster::administrator_commissioning::command::send_open_commissioning_window(
            remote_device, remote_endpoint_id, commissioning_timeout, serialized_verifier_span, discriminator,
            iteration, salt, k_default_timed_interaction_timeout);
    }
    case AdministratorCommissioning::Commands::OpenBasicCommissioningWindow::Id: {
        if (command_data->command_data_count != 1) {
            ESP_LOGE(TAG, "The command data should be specified in following order: commissioning_timeout");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::administrator_commissioning::command::send_open_basic_commissioning_window(
            remote_device, remote_endpoint_id,
            /* commissioning_timeout */ string_to_uint16(command_data->command_data_str[0]),
            k_default_timed_interaction_timeout);
        break;
    }
    case AdministratorCommissioning::Commands::RevokeCommissioning::Id: {
        return esp_matter::cluster::administrator_commissioning::command::send_revoke_commissioning(
            remote_device, remote_endpoint_id, k_default_timed_interaction_timeout);
        break;
    }
    default:
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace administrator_commissioning
} // namespace clusters

cluster_command_handler_t cluster_command::unsupported_cluster_command_handler = NULL;
cluster_group_command_handler_t cluster_command::unsupported_cluster_group_command_handler = NULL;

void cluster_command::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                              const SessionHandle &sessionHandle)
{
    esp_err_t err = ESP_OK;
    cluster_command *cmd = reinterpret_cast<cluster_command *>(context);
    chip::OperationalDeviceProxy device_proxy(&exchangeMgr, sessionHandle);
    switch (cmd->m_command_data->cluster_id) {
    case OnOff::Id:
        err = clusters::on_off::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case LevelControl::Id:
        err = clusters::level_control::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case ColorControl::Id:
        err = clusters::color_control::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case GroupKeyManagement::Id:
        err = clusters::group_key_management::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case Groups::Id:
        err = clusters::groups::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case WindowCovering::Id:
        err = clusters::window_covering::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case Identify::Id:
        err = clusters::identify::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case Scenes::Id:
        err = clusters::scenes::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case Thermostat::Id:
        err = clusters::thermostat::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case DoorLock::Id:
        err = clusters::door_lock::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case AdministratorCommissioning::Id:
        clusters::administrator_commissioning::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    default:
        err = ESP_ERR_NOT_SUPPORTED;
        break;
    }

    if (err == ESP_ERR_NOT_SUPPORTED && unsupported_cluster_command_handler) {
        unsupported_cluster_command_handler(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
    }
    chip::Platform::Delete(cmd);
    return;
}

void cluster_command::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    cluster_command *cmd = reinterpret_cast<cluster_command *>(context);
    chip::Platform::Delete(cmd);
    return;
}

esp_err_t cluster_command::dispatch_group_command(void *context)
{
    esp_err_t err = ESP_OK;
    cluster_command *cmd = reinterpret_cast<cluster_command *>(context);
    uint16_t group_id = cmd->m_destination_id & 0xFFFF;
    switch (cmd->m_command_data->cluster_id) {
    case OnOff::Id:
        err = clusters::on_off::send_group_command(cmd->m_command_data, group_id);
        break;
    default:
        err = ESP_ERR_NOT_SUPPORTED;
        break;
    }
    if (err == ESP_ERR_NOT_SUPPORTED && unsupported_cluster_group_command_handler) {
        err = unsupported_cluster_group_command_handler(cmd->m_command_data, group_id);
    }
    chip::Platform::Delete(cmd);
    return err;
}

esp_err_t cluster_command::send_command()
{
    if (is_group_command()) {
        return dispatch_group_command(reinterpret_cast<void *>(this));
    }
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    if (CHIP_NO_ERROR ==
        commissioner::get_device_commissioner()->GetConnectedDevice(m_destination_id, &on_device_connected_cb,
                                                                    &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#else
    chip::Server *server = &(chip::Server::GetInstance());
    server->GetCASESessionManager()->FindOrEstablishSession(ScopedNodeId(m_destination_id, get_fabric_index()),
                                                            &on_device_connected_cb, &on_device_connection_failure_cb);
    return ESP_OK;
#endif
    chip::Platform::Delete(this);
    return ESP_FAIL;
}

esp_err_t send_invoke_cluster_command(uint64_t destination_id, uint16_t endpoint_id, int cmd_data_argc,
                                      char **cmd_data_argv)
{
    command_data_t *command_data = (command_data_t *)esp_matter_mem_calloc(1, sizeof(command_data_t));
    if (!command_data) {
        ESP_LOGE(TAG, "Failed to alloc memory for command data");
        return ESP_ERR_NO_MEM;
    }
    command_data->cluster_id = string_to_uint32(cmd_data_argv[0]);
    command_data->command_id = string_to_uint32(cmd_data_argv[1]);
    command_data->command_data_count = cmd_data_argc - 2;

    for (size_t idx = 0; idx < command_data->command_data_count; idx++) {
        strncpy(command_data->command_data_str[idx], cmd_data_argv[2 + idx],
                strnlen(cmd_data_argv[2 + idx], k_max_command_data_str_len));
        command_data->command_data_str[idx][controller::k_max_command_data_str_len - 1] = 0;
    }
    cluster_command *cmd = chip::Platform::New<cluster_command>(destination_id, endpoint_id, command_data);
    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for cluster_command");
        return ESP_ERR_NO_MEM;
    }

    return cmd->send_command();
}

} // namespace controller
} // namespace esp_matter
