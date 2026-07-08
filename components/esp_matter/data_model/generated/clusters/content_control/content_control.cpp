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
#include <content_control.h>
#include <content_control_ids.h>
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
constexpr uint16_t cluster_revision = 1;

static esp_err_t esp_matter_command_callback_update_pin(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::UpdatePIN::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterUpdatePINCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_reset_pin(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::ResetPIN::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterResetPINCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enable(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                    void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::Enable::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterEnableCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_disable(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                     void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::Disable::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterDisableCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_bonus_time(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                            void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::AddBonusTime::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterAddBonusTimeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_screen_daily_time(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::SetScreenDailyTime::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterSetScreenDailyTimeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_block_unrated_content(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::BlockUnratedContent::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterBlockUnratedContentCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_unblock_unrated_content(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                     void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::UnblockUnratedContent::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterUnblockUnratedContentCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_on_demand_rating_threshold(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                            void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::SetOnDemandRatingThreshold::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterSetOnDemandRatingThresholdCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_scheduled_content_rating_threshold(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                                    void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::SetScheduledContentRatingThreshold::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterSetScheduledContentRatingThresholdCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_block_channels(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::AddBlockChannels::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterAddBlockChannelsCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_block_channels(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::RemoveBlockChannels::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterRemoveBlockChannelsCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_block_applications(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                    void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::AddBlockApplications::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterAddBlockApplicationsCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_block_applications(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                       void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::RemoveBlockApplications::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterRemoveBlockApplicationsCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_block_content_time_window(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                           void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::SetBlockContentTimeWindow::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterSetBlockContentTimeWindowCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_block_content_time_window(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                              void *opaque_ptr)
{
    chip::app::Clusters::ContentControl::Commands::RemoveBlockContentTimeWindow::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfContentControlClusterRemoveBlockContentTimeWindowCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace content_control {

namespace feature {
namespace screen_time {
uint32_t get_id()
{
    return ScreenTime::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_screen_daily_time(cluster, config->screen_daily_time);
    attribute::create_remaining_screen_time(cluster, config->remaining_screen_time);
    command::create_add_bonus_time(cluster);
    command::create_set_screen_daily_time(cluster);
    event::create_remaining_screen_time_expired(cluster);

    return ESP_OK;
}
} /* screen_time */

namespace pin_management {
uint32_t get_id()
{
    return PINManagement::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_update_pin(cluster);
    command::create_reset_pin(cluster);
    command::create_reset_pin_response(cluster);

    return ESP_OK;
}
} /* pin_management */

namespace block_unrated {
uint32_t get_id()
{
    return BlockUnrated::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_block_unrated(cluster, config->block_unrated);
    command::create_block_unrated_content(cluster);
    command::create_unblock_unrated_content(cluster);

    return ESP_OK;
}
} /* block_unrated */

namespace on_demand_content_rating {
uint32_t get_id()
{
    return OnDemandContentRating::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_on_demand_rating_threshold(cluster, config->on_demand_rating_threshold, sizeof(config->on_demand_rating_threshold));
    attribute::create_on_demand_ratings(cluster, NULL, 0, 0);
    command::create_set_on_demand_rating_threshold(cluster);

    return ESP_OK;
}
} /* on_demand_content_rating */

namespace scheduled_content_rating {
uint32_t get_id()
{
    return ScheduledContentRating::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_scheduled_content_rating_threshold(cluster, config->scheduled_content_rating_threshold, sizeof(config->scheduled_content_rating_threshold));
    attribute::create_scheduled_content_ratings(cluster, NULL, 0, 0);
    command::create_set_scheduled_content_rating_threshold(cluster);

    return ESP_OK;
}
} /* scheduled_content_rating */

namespace block_channels {
uint32_t get_id()
{
    return BlockChannels::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_block_channel_list(cluster, NULL, 0, 0);
    command::create_add_block_channels(cluster);
    command::create_remove_block_channels(cluster);

    return ESP_OK;
}
} /* block_channels */

namespace block_applications {
uint32_t get_id()
{
    return BlockApplications::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_block_application_list(cluster, NULL, 0, 0);
    command::create_add_block_applications(cluster);
    command::create_remove_block_applications(cluster);

    return ESP_OK;
}
} /* block_applications */

namespace block_content_time_window {
uint32_t get_id()
{
    return BlockContentTimeWindow::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_block_content_time_window(cluster, NULL, 0, 0);
    command::create_set_block_content_time_window(cluster);
    command::create_remove_block_content_time_window(cluster);
    event::create_entering_block_content_time_window(cluster);

    return ESP_OK;
}
} /* block_content_time_window */

} /* feature */

namespace attribute {
attribute_t *create_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, Enabled::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_on_demand_ratings(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(on_demand_content_rating), NULL);
    return esp_matter::attribute::create(cluster, OnDemandRatings::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_on_demand_rating_threshold(cluster_t *cluster, char *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(on_demand_content_rating), NULL);
    VerifyOrReturnValue(length <= k_max_on_demand_rating_threshold_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound. cluster_id: 0x%08" PRIX32, content_control::Id));
    return esp_matter::attribute::create(cluster, OnDemandRatingThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_on_demand_rating_threshold_length + 1);
}

attribute_t *create_scheduled_content_ratings(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(scheduled_content_rating), NULL);
    return esp_matter::attribute::create(cluster, ScheduledContentRatings::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_scheduled_content_rating_threshold(cluster_t *cluster, char *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(scheduled_content_rating), NULL);
    VerifyOrReturnValue(length <= k_max_scheduled_content_rating_threshold_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound. cluster_id: 0x%08" PRIX32, content_control::Id));
    return esp_matter::attribute::create(cluster, ScheduledContentRatingThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_scheduled_content_rating_threshold_length + 1);
}

attribute_t *create_screen_daily_time(cluster_t *cluster, uint32_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(screen_time), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ScreenDailyTime::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(86400)));
    return attribute;
}

attribute_t *create_remaining_screen_time(cluster_t *cluster, uint32_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(screen_time), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, RemainingScreenTime::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(86400)));
    return attribute;
}

attribute_t *create_block_unrated(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_unrated), NULL);
    return esp_matter::attribute::create(cluster, BlockUnrated::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_block_channel_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_channels), NULL);
    return esp_matter::attribute::create(cluster, BlockChannelList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_block_application_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_applications), NULL);
    return esp_matter::attribute::create(cluster, BlockApplicationList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_block_content_time_window(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_content_time_window), NULL);
    return esp_matter::attribute::create(cluster, BlockContentTimeWindow::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_update_pin(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pin_management), NULL);
    return esp_matter::command::create(cluster, UpdatePIN::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_update_pin);
}

command_t *create_reset_pin(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pin_management), NULL);
    return esp_matter::command::create(cluster, ResetPIN::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_reset_pin);
}

command_t *create_reset_pin_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pin_management), NULL);
    return esp_matter::command::create(cluster, ResetPINResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_enable(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Enable::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_enable);
}

command_t *create_disable(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Disable::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_disable);
}

command_t *create_add_bonus_time(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(screen_time), NULL);
    return esp_matter::command::create(cluster, AddBonusTime::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_bonus_time);
}

command_t *create_set_screen_daily_time(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(screen_time), NULL);
    return esp_matter::command::create(cluster, SetScreenDailyTime::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_screen_daily_time);
}

command_t *create_block_unrated_content(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_unrated), NULL);
    return esp_matter::command::create(cluster, BlockUnratedContent::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_block_unrated_content);
}

command_t *create_unblock_unrated_content(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_unrated), NULL);
    return esp_matter::command::create(cluster, UnblockUnratedContent::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_unblock_unrated_content);
}

command_t *create_set_on_demand_rating_threshold(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(on_demand_content_rating), NULL);
    return esp_matter::command::create(cluster, SetOnDemandRatingThreshold::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_on_demand_rating_threshold);
}

command_t *create_set_scheduled_content_rating_threshold(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(scheduled_content_rating), NULL);
    return esp_matter::command::create(cluster, SetScheduledContentRatingThreshold::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_scheduled_content_rating_threshold);
}

command_t *create_add_block_channels(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_channels), NULL);
    return esp_matter::command::create(cluster, AddBlockChannels::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_block_channels);
}

command_t *create_remove_block_channels(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_channels), NULL);
    return esp_matter::command::create(cluster, RemoveBlockChannels::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_block_channels);
}

command_t *create_add_block_applications(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_applications), NULL);
    return esp_matter::command::create(cluster, AddBlockApplications::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_block_applications);
}

command_t *create_remove_block_applications(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_applications), NULL);
    return esp_matter::command::create(cluster, RemoveBlockApplications::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_block_applications);
}

command_t *create_set_block_content_time_window(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_content_time_window), NULL);
    return esp_matter::command::create(cluster, SetBlockContentTimeWindow::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_block_content_time_window);
}

command_t *create_remove_block_content_time_window(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_content_time_window), NULL);
    return esp_matter::command::create(cluster, RemoveBlockContentTimeWindow::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_block_content_time_window);
}

} /* command */

namespace event {
event_t *create_remaining_screen_time_expired(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(screen_time), NULL);
    return esp_matter::event::create(cluster, RemainingScreenTimeExpired::Id);
}

event_t *create_entering_block_content_time_window(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(block_content_time_window), NULL);
    return esp_matter::event::create(cluster, EnteringBlockContentTimeWindow::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, content_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, content_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ContentControlDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterContentControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_enabled(cluster, config->enabled);
        command::create_enable(cluster);
        command::create_disable(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* content_control */
} /* cluster */
} /* esp_matter */
