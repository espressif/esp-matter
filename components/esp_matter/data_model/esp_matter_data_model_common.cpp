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

// Cluster-independent data-model glue shared by the legacy and generated data models.

#include <cassert>

#include <esp_matter_attribute_utils.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_priv.h>
#include <app/CommandHandler.h>

static const char *TAG = "data_model";

static const char feature_policy_strs[2][16] = {"Exactly one", "At least one"};

bool validate_features(uint32_t feature_flag, feature_policy policy,
                       const char *feature_name, std::initializer_list<uint32_t> features)
{
    uint8_t count = 0;
    for (uint32_t feature : features) {
        if (feature & feature_flag) {
            count++;
        }
    }

    bool result = false;

    switch (policy) {
    case feature_policy::k_exact_one:
        result = count == 1;
        break;
    case feature_policy::k_at_least_one:
        result = count >= 1;
        break;
    }

    if (!result) {
        ESP_LOGE(TAG, "%s of the feature(s) must be supported from (%s)",
                 feature_policy_strs[static_cast<uint8_t>(policy)], feature_name);
    }

    return result;
}

namespace esp_matter {
namespace cluster {

esp_err_t update_feature_map(cluster_t *cluster, uint32_t value)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));

    /* Get the attribute */
    attribute_t *attribute = attribute::get(cluster, chip::app::Clusters::Globals::Attributes::FeatureMap::Id);

    VerifyOrReturnError(attribute, ESP_ERR_INVALID_STATE, ESP_LOGE(TAG, "Feature map attribute cannot be null"));

    /* Update the value if the attribute already exists */
    esp_matter_attr_val_t val;
    VerifyOrReturnError(attribute::get_val_internal(attribute, &val) == ESP_OK, ESP_FAIL);
    val.val.u32 |= value;
    /* Here we can't call attribute::update() since the chip stack would not have started yet, since we are
    still creating the data model. So, we are directly using attribute::set_val_internal(). */
    return attribute::set_val_internal(attribute, &val, false);
}

uint32_t get_feature_map_value(cluster_t *cluster)
{
    esp_matter_attr_val_t val;
    attribute_t *attribute = attribute::get(cluster, chip::app::Clusters::Globals::Attributes::FeatureMap::Id);

    VerifyOrReturnValue(attribute, 0, ESP_LOGE(TAG, "Feature map attribute cannot be null"));

    VerifyOrReturnValue(attribute::get_val_internal(attribute, &val) == ESP_OK, 0);
    return val.val.u32;
}

// simple API to abort cluster creation and return NULL
cluster_t *ABORT_CLUSTER_CREATE(cluster_t *cluster)
{
    esp_matter::cluster::destroy(cluster);
    assert(false);
    return NULL;
}

} // namespace cluster

namespace command {

void dispatch_single_cluster_command(const chip::app::ConcreteCommandPath &command_path,
                                     chip::TLV::TLVReader &tlv_data, void *opaque_ptr)
{
    uint16_t endpoint_id = command_path.mEndpointId;
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;
    ESP_LOGI(TAG, "Received command 0x%08" PRIX32 " for endpoint 0x%04" PRIX16 "'s cluster 0x%08" PRIX32 "", command_id,
             endpoint_id, cluster_id);

    cluster_t *cluster = cluster::get(endpoint_id, cluster_id);
    VerifyOrReturn(cluster);
    command_t *command = get(cluster, command_id, COMMAND_FLAG_ACCEPTED);
    VerifyOrReturn(command, ESP_LOGE(TAG, "Command 0x%08" PRIX32 " not found", command_id));
    esp_err_t err = ESP_OK;
    chip::TLV::TLVReader tlv_reader;
    tlv_reader.Init(tlv_data);
    callback_t callback = get_user_callback(command);
    if (callback) {
        err = callback(command_path, tlv_reader, opaque_ptr);
    }
    callback = get_callback(command);
    if ((err == ESP_OK) && callback) {
        err = callback(command_path, tlv_data, opaque_ptr);
    }
    int flags = get_flags(command);
    if (flags & COMMAND_FLAG_CUSTOM) {
        chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;
        if (!command_obj) {
            ESP_LOGE(TAG, "Command Object cannot be NULL");
            return;
        }
        command_obj->AddStatus(command_path, err == ESP_OK ? chip::Protocols::InteractionModel::Status::Success :
                               chip::Protocols::InteractionModel::Status::Failure);
    }
}

} // namespace command
} // namespace esp_matter
