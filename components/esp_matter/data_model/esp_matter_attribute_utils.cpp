// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include <cstdint>
#include <string.h>

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_console.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_priv.h>
#include <esp_matter_mem.h>

#include <data_model_provider/esp_matter_data_model_provider.h>

#include <app/AttributePathParams.h>
#include <app/reporting/reporting.h>
#include <app/util/attribute-storage.h>
#include <app/util/attribute-table.h>
#include <lib/support/CodeUtils.h>
#include <protocols/interaction_model/Constants.h>

using chip::AttributeId;
using chip::ClusterId;
using chip::EndpointId;

using namespace esp_matter;

static const char *TAG = "esp_matter_attribute";

esp_matter_attr_val_t esp_matter_int(int val)
{
    return esp_matter_attr_val((int32_t)val);
}

esp_matter_attr_val_t esp_matter_nullable_int(nullable<int> val)
{
    return esp_matter_attr_val(val.is_null() ? nullable<int32_t>() : nullable<int32_t>(val.value()));
}

bool esp_matter_attr_val::is_null() const
{
    if (!is_nullable()) {
        return false;
    }
    switch (get_storage_type()) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        return chip::app::NumericAttributeTraits<bool>::IsNullValue(*(uint8_t *)(&(val.b)));
    case ESP_MATTER_VAL_TYPE_UINT8:
        return chip::app::NumericAttributeTraits<uint8_t>::IsNullValue(val.u8);
    case ESP_MATTER_VAL_TYPE_UINT16:
        return chip::app::NumericAttributeTraits<uint16_t>::IsNullValue(val.u16);
    case ESP_MATTER_VAL_TYPE_UINT32:
        return chip::app::NumericAttributeTraits<uint32_t>::IsNullValue(val.u32);
    case ESP_MATTER_VAL_TYPE_UINT64:
        return chip::app::NumericAttributeTraits<uint64_t>::IsNullValue(val.u64);
    case ESP_MATTER_VAL_TYPE_INT8:
        return chip::app::NumericAttributeTraits<int8_t>::IsNullValue(val.i8);
    case ESP_MATTER_VAL_TYPE_INT16:
        return chip::app::NumericAttributeTraits<int16_t>::IsNullValue(val.i16);
    case ESP_MATTER_VAL_TYPE_INT32:
        return chip::app::NumericAttributeTraits<int32_t>::IsNullValue(val.i32);
    case ESP_MATTER_VAL_TYPE_INT64:
        return chip::app::NumericAttributeTraits<int64_t>::IsNullValue(val.i64);
    case ESP_MATTER_VAL_TYPE_FLOAT:
        return chip::app::NumericAttributeTraits<float>::IsNullValue(val.f);
    default:
        break;
    }
    return false;
}

namespace esp_matter {
namespace attribute {

void val_print(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val,
               bool is_read)
{
    char action = (is_read) ? 'R' : 'W';
    VerifyOrReturn(!val->is_null(),
                   ESP_LOGI(TAG,
                            "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32
                            "'s Attribute 0x%08" PRIX32 " is null **********",
                            action, endpoint_id, cluster_id, attribute_id));

    switch (val->get_storage_type()) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %d **********",
                 action, endpoint_id, cluster_id, attribute_id, val->val.b);
        break;
    case ESP_MATTER_VAL_TYPE_FLOAT:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %f **********",
                 action, endpoint_id, cluster_id, attribute_id, val->val.f);
        break;
    case ESP_MATTER_VAL_TYPE_INT8:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %i **********",
                 action, endpoint_id, cluster_id, attribute_id, val->val.i8);
        break;
    case ESP_MATTER_VAL_TYPE_UINT8:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %u **********",
                 action, endpoint_id, cluster_id, attribute_id, val->val.u8);
        break;
    case ESP_MATTER_VAL_TYPE_INT16:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %" PRIi16 " **********",
                 action, endpoint_id, cluster_id, attribute_id, val->val.i16);
        break;
    case ESP_MATTER_VAL_TYPE_UINT16:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %" PRIu16 " **********",
                 action, endpoint_id, cluster_id, attribute_id, val->val.u16);
        break;
    case ESP_MATTER_VAL_TYPE_INT32:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %" PRIi32 " **********",
                 action, endpoint_id, cluster_id, attribute_id, val->val.i32);
        break;
    case ESP_MATTER_VAL_TYPE_UINT32:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %" PRIu32 " **********",
                 action, endpoint_id, cluster_id, attribute_id, val->val.u32);
        break;
    case ESP_MATTER_VAL_TYPE_INT64:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %" PRIi64 " **********",
                 action, endpoint_id, cluster_id, attribute_id, val->val.i64);
        break;
    case ESP_MATTER_VAL_TYPE_UINT64:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %" PRIu64 " **********",
                 action, endpoint_id, cluster_id, attribute_id, val->val.u64);
        break;
    case ESP_MATTER_VAL_TYPE_CHAR_STRING: {
        const char *b = val->val.a.b ? (const char *)val->val.a.b : "(empty)";
        uint16_t s = val->val.a.b ? val->val.a.s : strlen("(empty)");
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %.*s **********",
                 action, endpoint_id, cluster_id, attribute_id, s, b);
    }
    break;
    case ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING: {
        const char *b = val->val.a.b ? (const char *)val->val.a.b : "(empty)";
        uint16_t s = val->val.a.b ? val->val.a.s : strlen("(empty)");
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is %.*s **********",
                 action, endpoint_id, cluster_id, attribute_id, s, b);
    }
    break;
    default:
        ESP_LOGI(TAG,
                 "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32
                 " is <invalid type: %d> **********",
                 action, endpoint_id, cluster_id, attribute_id, val->type);
        break;
    }
}

static esp_err_t update_or_report(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val, bool call_attribute_callbacks)
{
    VerifyOrReturnError(val, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "val cannot be NULL"));

    attribute_t *attr = get(endpoint_id, cluster_id, attribute_id);
    VerifyOrReturnError(attr, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Failed to get attribute handle"));

    lock::ScopedChipStackLock lock(portMAX_DELAY);

    /* Here, the val_print function gets called on attribute write.*/
    attribute::val_print(endpoint_id, cluster_id, attribute_id, val, false);

    esp_err_t err = attribute::set_val(attr, val, call_attribute_callbacks);
    if (err == ESP_OK) {
        data_model::provider::get_instance().Temporary_ReportAttributeChanged(
            chip::app::AttributePathParams(endpoint_id, cluster_id, attribute_id));
    } else if (err == ESP_ERR_NOT_FINISHED) {
        // new value is same as older value, skip reporting to IM engine
        err = ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to set attribute value for path: 0x%x/0x%" PRIx32 "/0x%" PRIX32 " err: %d",
                 endpoint_id, cluster_id, attribute_id, err);
    }
    return err;
}

esp_err_t update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    return update_or_report(endpoint_id, cluster_id, attribute_id, val, true /* call_attribute_callbacks */);
}

esp_err_t report(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    return update_or_report(endpoint_id, cluster_id, attribute_id, val, false /* call_attribute_callbacks */);
}

bool val_compare(const esp_matter_attr_val_t *val1, const esp_matter_attr_val_t *val2)
{
    if (val1 == nullptr || val2 == nullptr) {
        return val1 == val2;
    }
    if (val1->type != val2->type) {
        return false;
    }
    switch (val1->get_storage_type()) {
    case ESP_MATTER_VAL_TYPE_CHAR_STRING:
    case ESP_MATTER_VAL_TYPE_OCTET_STRING:
    case ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING:
    case ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING: {
        uint16_t null_len =
            (val1->type == ESP_MATTER_VAL_TYPE_CHAR_STRING || val1->type == ESP_MATTER_VAL_TYPE_OCTET_STRING)
            ? UINT8_MAX
            : UINT16_MAX;
        if (val1->val.a.s != val2->val.a.s) {
            return false;
        }
        if (val1->val.a.s == null_len || val1->val.a.s == 0) {
            return true;
        }
        if (val1->val.a.b == nullptr && val2->val.a.b == nullptr) {
            return true;
        }
        if (val1->val.a.b == nullptr || val2->val.a.b == nullptr) {
            return false;
        }
        return memcmp(val1->val.a.b, val2->val.a.b, val1->val.a.s) == 0;
    }
    case ESP_MATTER_VAL_TYPE_UINT8:
        return val1->val.u8 == val2->val.u8;
    case ESP_MATTER_VAL_TYPE_UINT16:
        return val1->val.u16 == val2->val.u16;
    case ESP_MATTER_VAL_TYPE_UINT32:
        return val1->val.u32 == val2->val.u32;
    case ESP_MATTER_VAL_TYPE_UINT64:
        return val1->val.u64 == val2->val.u64;
    case ESP_MATTER_VAL_TYPE_INT8:
        return val1->val.i8 == val2->val.i8;
    case ESP_MATTER_VAL_TYPE_INT16:
        return val1->val.i16 == val2->val.i16;
    case ESP_MATTER_VAL_TYPE_INT32:
        return val1->val.i32 == val2->val.i32;
    case ESP_MATTER_VAL_TYPE_INT64:
        return val1->val.i64 == val2->val.i64;
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        return val1->val.b == val2->val.b;
    case ESP_MATTER_VAL_TYPE_FLOAT:
        return val1->val.f == val2->val.f;
    default:
        ESP_LOGE(TAG, "Unsupported type to compare");
    }
    return false;
}

} // namespace attribute
} // namespace esp_matter
