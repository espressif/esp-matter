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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#include <esp_log.h>
#include <esp_matter_core.h>
#include <esp_matter.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <app-common/zap-generated/callback.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <esp_matter_delegate_callbacks.h>
#include <channel.h>
#include <channel_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 2;

static esp_err_t esp_matter_command_callback_change_channel(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                            void *opaque_ptr)
{
    chip::app::Clusters::Channel::Commands::ChangeChannel::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfChannelClusterChangeChannelCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_change_channel_by_number(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                      void *opaque_ptr)
{
    chip::app::Clusters::Channel::Commands::ChangeChannelByNumber::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfChannelClusterChangeChannelByNumberCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_skip_channel(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::Channel::Commands::SkipChannel::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfChannelClusterSkipChannelCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_program_guide(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                               void *opaque_ptr)
{
    chip::app::Clusters::Channel::Commands::GetProgramGuide::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfChannelClusterGetProgramGuideCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_record_program(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                            void *opaque_ptr)
{
    chip::app::Clusters::Channel::Commands::RecordProgram::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfChannelClusterRecordProgramCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_cancel_record_program(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::Channel::Commands::CancelRecordProgram::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfChannelClusterCancelRecordProgramCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace channel {

namespace feature {
namespace channel_list {
uint32_t get_id()
{
    return ChannelList::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_channel_list(cluster, NULL, 0, 0);
    command::create_change_channel(cluster);
    command::create_change_channel_response(cluster);

    return ESP_OK;
}
} /* channel_list */

namespace lineup_info {
uint32_t get_id()
{
    return LineupInfo::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_lineup(cluster, NULL, 0, 0);
    command::create_change_channel(cluster);
    command::create_change_channel_response(cluster);

    return ESP_OK;
}
} /* lineup_info */

namespace electronic_guide {
uint32_t get_id()
{
    return ElectronicGuide::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_get_program_guide(cluster);
    command::create_program_guide_response(cluster);
    command::create_record_program(cluster);
    command::create_cancel_record_program(cluster);

    return ESP_OK;
}
} /* electronic_guide */

namespace record_program {
uint32_t get_id()
{
    return RecordProgram::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_record_program(cluster);
    command::create_cancel_record_program(cluster);

    return ESP_OK;
}
} /* record_program */

} /* feature */

namespace attribute {
attribute_t *create_channel_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(channel_list), NULL);
    return esp_matter::attribute::create(cluster, ChannelList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_lineup(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(lineup_info), NULL);
    return esp_matter::attribute::create(cluster, Lineup::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_channel(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CurrentChannel::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_change_channel(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(channel_list)) || (has_feature(lineup_info))), NULL);
    return esp_matter::command::create(cluster, ChangeChannel::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_change_channel);
}

command_t *create_change_channel_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(channel_list)) || (has_feature(lineup_info))), NULL);
    return esp_matter::command::create(cluster, ChangeChannelResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_change_channel_by_number(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ChangeChannelByNumber::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_change_channel_by_number);
}

command_t *create_skip_channel(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SkipChannel::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_skip_channel);
}

command_t *create_get_program_guide(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(electronic_guide), NULL);
    return esp_matter::command::create(cluster, GetProgramGuide::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_get_program_guide);
}

command_t *create_program_guide_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(electronic_guide), NULL);
    return esp_matter::command::create(cluster, ProgramGuideResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_record_program(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(record_program)) && (has_feature(electronic_guide))), NULL);
    return esp_matter::command::create(cluster, RecordProgram::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_record_program);
}

command_t *create_cancel_record_program(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(record_program)) && (has_feature(electronic_guide))), NULL);
    return esp_matter::command::create(cluster, CancelRecordProgram::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_cancel_record_program);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, channel::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, channel::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ChannelDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterChannelPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        command::create_change_channel_by_number(cluster);
        command::create_skip_channel(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* channel */
} /* cluster */
} /* esp_matter */
