// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_priv.h>

#include <app/util/attribute-storage.h>
#include <protocols/interaction_model/StatusCode.h>

using chip::Protocols::InteractionModel::Status;
using namespace chip;

static const char *TAG = "data_model";

namespace esp_matter {

static esp_err_t execute_override_callback(attribute_t *attribute, attribute::callback_type_t type,
                                           uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                           esp_matter_attr_val_t *val)
{
    attribute::callback_t override_callback = attribute::get_override_callback(attribute);
    void *priv_data = endpoint::get_priv_data(endpoint_id);
    if (override_callback) {
        return override_callback(type, endpoint_id, cluster_id, attribute_id, val, priv_data);
    } else {
        ESP_LOGW(TAG,
                 "Attribute override callback not set for Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32
                 "'s Attribute 0x%08" PRIX32 ", calling the common callback",
                 endpoint_id, cluster_id, attribute_id);
    }
    return ESP_OK;
}
} // namespace esp_matter

using namespace esp_matter;

Status emberAfExternalAttributeReadCallback(EndpointId endpoint_id, ClusterId cluster_id,
                                            const EmberAfAttributeMetadata *matter_attribute, uint8_t *buffer,
                                            uint16_t max_read_length)
{
    /* Get value */
    uint32_t attribute_id = matter_attribute->attributeId;
    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);
    VerifyOrReturnError(attribute, Status::Failure);
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    int flags = attribute::get_flags(attribute);
    if (flags & ATTRIBUTE_FLAG_OVERRIDE) {
        esp_err_t err =
            execute_override_callback(attribute, attribute::READ, endpoint_id, cluster_id, attribute_id, &val);
        VerifyOrReturnValue(err == ESP_OK, Status::Failure);
    } else {
        attribute::get_val(attribute, &val);
    }

    /* Here, the val_print function gets called on attribute read. */
    attribute::val_print(endpoint_id, cluster_id, attribute_id, &val, true);

    /* Get size */
    uint16_t attribute_size = 0;
    attribute::get_data_from_attr_val(&val, NULL, &attribute_size, NULL);
    VerifyOrReturnValue(attribute_size <= max_read_length, Status::ResourceExhausted,
                        ESP_LOGE(TAG,
                                 "Insufficient space for reading Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32
                                 "'s Attribute 0x%08" PRIX32 ": required: %" PRIu16 ", max: %" PRIu16 "",
                                 endpoint_id, cluster_id, attribute_id, attribute_size, max_read_length));

    /* Assign value */
    attribute::get_data_from_attr_val(&val, NULL, &attribute_size, buffer);
    return Status::Success;
}

Status emberAfExternalAttributeWriteCallback(EndpointId endpoint_id, ClusterId cluster_id,
                                             const EmberAfAttributeMetadata *matter_attribute, uint8_t *buffer)
{
    /* Get value */
    uint32_t attribute_id = matter_attribute->attributeId;
    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);
    VerifyOrReturnError(attribute, Status::Failure);

    /* Get val */
    /* This creates a new variable val, and stores the new attribute value in the new variable.
    The value in esp-matter data model is updated only when attribute::set_val() is called */
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_attr_val_from_data(&val, matter_attribute->attributeType, matter_attribute->size, buffer,
                                      matter_attribute);

    int flags = attribute::get_flags(attribute);
    if (flags & ATTRIBUTE_FLAG_OVERRIDE) {
        esp_err_t err =
            execute_override_callback(attribute, attribute::WRITE, endpoint_id, cluster_id, attribute_id, &val);
        Status status = (err == ESP_OK) ? Status::Success : Status::Failure;
        return status;
    }

    /* Update val */
    VerifyOrReturnValue(val.type != ESP_MATTER_VAL_TYPE_INVALID, Status::Failure);
    attribute::set_val(attribute, &val);
    return Status::Success;
}

// No-op function, used to force linking this file, instead of the weak functions from other files.
extern "C" void data_model_utils_impl(void) {}
