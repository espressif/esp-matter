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
#include <messages.h>
#include <messages_ids.h>
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
constexpr uint16_t cluster_revision = 3;

static esp_err_t esp_matter_command_callback_present_messages_request(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                      void *opaque_ptr)
{
    chip::app::Clusters::Messages::Commands::PresentMessagesRequest::DecodableType command_data;
    chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;
    CHIP_ERROR error = command_data.Decode(tlv_data, command_obj->GetAccessingFabricIndex());
    if (error == CHIP_NO_ERROR) {
        emberAfMessagesClusterPresentMessagesRequestCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_cancel_messages_request(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                     void *opaque_ptr)
{
    chip::app::Clusters::Messages::Commands::CancelMessagesRequest::DecodableType command_data;
    chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;
    CHIP_ERROR error = command_data.Decode(tlv_data, command_obj->GetAccessingFabricIndex());
    if (error == CHIP_NO_ERROR) {
        emberAfMessagesClusterCancelMessagesRequestCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace messages {

namespace feature {
namespace received_confirmation {
uint32_t get_id()
{
    return ReceivedConfirmation::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* received_confirmation */

namespace confirmation_response {
uint32_t get_id()
{
    return ConfirmationResponse::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(received_confirmation), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* confirmation_response */

namespace confirmation_reply {
uint32_t get_id()
{
    return ConfirmationReply::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(received_confirmation), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* confirmation_reply */

namespace protected_messages {
uint32_t get_id()
{
    return ProtectedMessages::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* protected_messages */

} /* feature */

namespace attribute {
attribute_t *create_messages(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Messages::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_active_message_i_ds(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActiveMessageIDs::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_present_messages_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, PresentMessagesRequest::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_present_messages_request);
}

command_t *create_cancel_messages_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CancelMessagesRequest::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_cancel_messages_request);
}

} /* command */

namespace event {
event_t *create_message_queued(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, MessageQueued::Id);
}

event_t *create_message_presented(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, MessagePresented::Id);
}

event_t *create_message_complete(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, MessageComplete::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, messages::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, messages::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = MessagesDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterMessagesPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_messages(cluster, NULL, 0, 0);
        attribute::create_active_message_i_ds(cluster, NULL, 0, 0);
        command::create_present_messages_request(cluster);
        command::create_cancel_messages_request(cluster);
        /* Events */
        event::create_message_queued(cluster);
        event::create_message_presented(cluster);
        event::create_message_complete(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* messages */
} /* cluster */
} /* esp_matter */
