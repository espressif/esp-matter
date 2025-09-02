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

#include "esp_check.h"
#include "esp_err.h"
#include "esp_matter_data_model.h"
#include "app/AttributePathParams.h"
#include "data_model_provider/esp_matter_data_model_provider.h"
#include <cstdint>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_console.h>
#include <esp_matter_core.h>
#include <esp_matter_mem.h>
#include <string.h>

#include <app/util/attribute-storage.h>
#include <app/util/attribute-table.h>
#include <app/reporting/reporting.h>
#include <protocols/interaction_model/Constants.h>
#include "support/CodeUtils.h"

using chip::AttributeId;
using chip::ClusterId;
using chip::EndpointId;
using chip::Protocols::InteractionModel::Status;

using namespace esp_matter;

static const char *TAG = "esp_matter_attribute";

esp_matter_attr_val_t esp_matter_invalid(void *val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_INVALID,
        .val = {
            .p = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_bool(bool val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_BOOLEAN,
        .val = {
            .b = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_int(int val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_INTEGER,
        .val = {
            .i = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_bool(nullable<bool> val)
{
    esp_matter_attr_val_t attr_val = {
	.type = ESP_MATTER_VAL_TYPE_NULLABLE_BOOLEAN,
    };
    if (val.is_null()) {
	chip::app::NumericAttributeTraits<bool>::SetNull(*(uint8_t *)(&(attr_val.val.b)));
    }
    else {
	attr_val.val.b = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_int(nullable<int> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_INTEGER,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<int>::SetNull(attr_val.val.i);
    } else {
        attr_val.val.i = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_float(float val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_FLOAT,
        .val = {
            .f = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_float(nullable<float> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<float>::SetNull(attr_val.val.f);
    } else {
        attr_val.val.f = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_int8(int8_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_INT8,
        .val = {
            .i8 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_int8(nullable<int8_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_INT8,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<int8_t>::SetNull(attr_val.val.i8);
    } else {
        attr_val.val.i8 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_uint8(uint8_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_UINT8,
        .val = {
            .u8 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_uint8(nullable<uint8_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_UINT8,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<uint8_t>::SetNull(attr_val.val.u8);
    } else {
        attr_val.val.u8 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_int16(int16_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_INT16,
        .val = {
            .i16 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_int16(nullable<int16_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_INT16,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<int16_t>::SetNull(attr_val.val.i16);
    } else {
        attr_val.val.i16 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_uint16(uint16_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_UINT16,
        .val = {
            .u16 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_uint16(nullable<uint16_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_UINT16,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<uint16_t>::SetNull(attr_val.val.u16);
    } else {
        attr_val.val.u16 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_int32(int32_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_INT32,
        .val = {
            .i32 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_int32(nullable<int32_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_INT32,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<int32_t>::SetNull(attr_val.val.i32);
    } else {
        attr_val.val.i32 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_uint32(uint32_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_UINT32,
        .val = {
            .u32 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_uint32(nullable<uint32_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_UINT32,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<uint32_t>::SetNull(attr_val.val.u32);
    } else {
        attr_val.val.u32 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_int64(int64_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_INT64,
        .val = {
            .i64 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_int64(nullable<int64_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_INT64,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<int64_t>::SetNull(attr_val.val.i64);
    } else {
        attr_val.val.i64 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_uint64(uint64_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_UINT64,
        .val = {
            .u64 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_uint64(nullable<uint64_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_UINT64,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<uint64_t>::SetNull(attr_val.val.u64);
    } else {
        attr_val.val.u64 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_enum8(uint8_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_ENUM8,
        .val = {
            .u8 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_enum8(nullable<uint8_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<uint8_t>::SetNull(attr_val.val.u8);
    } else {
        attr_val.val.u8 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_enum16(uint16_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_ENUM16,
        .val = {
            .u16 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_enum16(nullable<uint16_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_ENUM16,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<uint16_t>::SetNull(attr_val.val.u16);
    } else {
        attr_val.val.u16 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_bitmap8(uint8_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_BITMAP8,
        .val = {
            .u8 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_bitmap8(nullable<uint8_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<uint8_t>::SetNull(attr_val.val.u8);
    } else {
        attr_val.val.u8 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_bitmap16(uint16_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_BITMAP16,
        .val = {
            .u16 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_bitmap16(nullable<uint16_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<uint16_t>::SetNull(attr_val.val.u16);
    } else {
        attr_val.val.u16 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_bitmap32(uint32_t val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_BITMAP32,
        .val = {
            .u32 = val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_nullable_bitmap32(nullable<uint32_t> val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32,
    };
    if (val.is_null()) {
        chip::app::NumericAttributeTraits<uint32_t>::SetNull(attr_val.val.u32);
    } else {
        attr_val.val.u32 = val.value();
    }
    return attr_val;
}

esp_matter_attr_val_t esp_matter_char_str(char *val, uint16_t data_size)
{
    uint16_t data_size_len = 1; /* Number of bytes used to store the length */
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_CHAR_STRING,
        .val = {
            .a = {
                .b = (uint8_t *)val,
                .s = data_size,
                .t = (uint16_t)(data_size + data_size_len),
            },
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_long_char_str(char *val, uint16_t data_size)
{
    uint16_t data_size_len = 2; /* Number of bytes used to store the length */
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING,
        .val = {
            .a = {
                .b = (uint8_t *)val,
                .s = data_size,
                .t = (uint16_t)(data_size + data_size_len),
            },
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_octet_str(uint8_t *val, uint16_t data_size)
{
    uint16_t data_size_len = 1; /* Number of bytes used to store the length */
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_OCTET_STRING,
        .val = {
            .a = {
                .b = val,
                .s = data_size,
                .t = (uint16_t)(data_size + data_size_len),
            },
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_long_octet_str(uint8_t *val, uint16_t data_size)
{
    uint16_t data_size_len = 2; /* Number of bytes used to store the length */
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING,
        .val = {
            .a = {
                .b = val,
                .s = data_size,
                .t = (uint16_t)(data_size + data_size_len),
            },
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_array(uint8_t *val, uint16_t data_size, uint16_t count)
{
    uint16_t data_size_len = 2; /* Number of bytes used to store the length */
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_ARRAY,
        .val = {
            .a = {
                .b = val,
                .s = data_size,
                .t = (uint16_t)(data_size + data_size_len),
            },
        },
    };
    return attr_val;
}

namespace esp_matter {
namespace attribute {


bool val_is_null(esp_matter_attr_val_t *val)
{
    switch (val->type) {
    case ESP_MATTER_VAL_TYPE_NULLABLE_BOOLEAN:
	return chip::app::NumericAttributeTraits<bool>::IsNullValue(*(uint8_t *)(&(val->val.b)));
	break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_INTEGER:
        return chip::app::NumericAttributeTraits<int>::IsNullValue(val->val.i);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8:
        return chip::app::NumericAttributeTraits<uint8_t>::IsNullValue(val->val.u8);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16:
        return chip::app::NumericAttributeTraits<uint16_t>::IsNullValue(val->val.u16);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32:
        return chip::app::NumericAttributeTraits<uint32_t>::IsNullValue(val->val.u32);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT64:
        return chip::app::NumericAttributeTraits<uint64_t>::IsNullValue(val->val.u64);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT8:
        return chip::app::NumericAttributeTraits<int8_t>::IsNullValue(val->val.i8);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT16:
        return chip::app::NumericAttributeTraits<int16_t>::IsNullValue(val->val.i16);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT32:
        return chip::app::NumericAttributeTraits<int32_t>::IsNullValue(val->val.i32);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT64:
        return chip::app::NumericAttributeTraits<int64_t>::IsNullValue(val->val.i64);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT:
        return chip::app::NumericAttributeTraits<float>::IsNullValue(val->val.f);
        break;
    default:
        return false;
        break;
    }
    return false;
}

void val_print(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val, bool is_read)
{
    char action = (is_read) ? 'R' :'W';
    VerifyOrReturn(!val_is_null(val), ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is null **********", action,
                 endpoint_id, cluster_id, attribute_id));

    if (val->type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %d **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.b);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INTEGER || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_INTEGER) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %d **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.i);
    } else if (val->type == ESP_MATTER_VAL_TYPE_FLOAT || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %f **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.f);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INT8 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_INT8) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %i **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.i8);
    }  else if (val->type == ESP_MATTER_VAL_TYPE_UINT8 || val->type == ESP_MATTER_VAL_TYPE_BITMAP8
               || val->type == ESP_MATTER_VAL_TYPE_ENUM8 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT8
               || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %u **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.u8);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INT16 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_INT16) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %" PRIi16 " **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.i16);
    } else if (val->type == ESP_MATTER_VAL_TYPE_UINT16 || val->type == ESP_MATTER_VAL_TYPE_BITMAP16
                || val->type == ESP_MATTER_VAL_TYPE_ENUM16 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT16
                || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_ENUM16) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %" PRIu16 " **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.u16);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INT32|| val->type == ESP_MATTER_VAL_TYPE_NULLABLE_INT32) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %" PRIi32 " **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.i32);
    } else if (val->type == ESP_MATTER_VAL_TYPE_UINT32 || val->type == ESP_MATTER_VAL_TYPE_BITMAP32
               || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT32 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %" PRIu32 " **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.u32);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INT64 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_INT64) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %" PRIi64 " **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.i64);
    } else if (val->type == ESP_MATTER_VAL_TYPE_UINT64 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT64) {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %" PRIu64 " **********", action,
                 endpoint_id, cluster_id, attribute_id, val->val.u64);
    } else if (val->type == ESP_MATTER_VAL_TYPE_CHAR_STRING) {
        const char *b = val->val.a.b ? (const char *)val->val.a.b : "(empty)";
        uint16_t s = val->val.a.b ? val->val.a.s : strlen("(empty)");
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %.*s **********", action,
                 endpoint_id, cluster_id, attribute_id, s, b);
    } else if (val->type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING) {
        const char *b = val->val.a.b ? (const char *)val->val.a.b : "(empty)";
        uint16_t s = val->val.a.b ? val->val.a.s : strlen("(empty)");
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is %.*s **********", action,
                 endpoint_id, cluster_id, attribute_id, s, b);
    } else {
        ESP_LOGI(TAG, "********** %c : Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " is <invalid type: %d> **********", action,
                 endpoint_id, cluster_id, attribute_id, val->type);
    }
}

esp_err_t update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    VerifyOrReturnError(val, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "val cannot be NULL"));
    attribute_t *attr = get(endpoint_id, cluster_id, attribute_id);
    ESP_RETURN_ON_FALSE(attr, ESP_ERR_INVALID_ARG, TAG, "Failed to get attribute handle");

    /* Take lock if not already taken */
    lock::status_t lock_status = lock::chip_stack_lock(portMAX_DELAY);
    VerifyOrReturnError(lock_status != lock::FAILED, ESP_FAIL, ESP_LOGE(TAG, "Could not get task context"));
    /* Here, the val_print function gets called on attribute write.*/
    attribute::val_print(endpoint_id, cluster_id, attribute_id, val, false);

    esp_err_t err = attribute::set_val(attr, val);
    if (err == ESP_OK) {
        data_model::provider::get_instance().Temporary_ReportAttributeChanged(
            chip::app::AttributePathParams(endpoint_id, cluster_id, attribute_id));
    } else if (err == ESP_ERR_NOT_FINISHED) {
        err = ESP_OK;
    }
    if (lock_status == lock::SUCCESS) {
        lock::chip_stack_unlock();
    }
    return err;
}

esp_err_t report(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    VerifyOrReturnError(val, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "val cannot be NULL"));
    attribute_t *attr = get(endpoint_id, cluster_id, attribute_id);
    ESP_RETURN_ON_FALSE(attr, ESP_ERR_INVALID_ARG, TAG, "Failed to get attribute handle");

    /* Take lock if not already taken */
    lock::status_t lock_status = lock::chip_stack_lock(portMAX_DELAY);
    VerifyOrReturnError(lock_status != lock::FAILED, ESP_FAIL, ESP_LOGE(TAG, "Could not get task context"));

    /* Here, the val_print function gets called on attribute write.*/
    attribute::val_print(endpoint_id, cluster_id, attribute_id, val, false);

    esp_err_t err = attribute::set_val(attr, val, false);
    if (err == ESP_OK) {
        /* Report attribute */
        MatterReportingAttributeChangeCallback(endpoint_id, cluster_id, attribute_id);
    } else if (err == ESP_ERR_NOT_FINISHED) {
        err = ESP_OK;
    }

    if (lock_status == lock::SUCCESS) {
        lock::chip_stack_unlock();
    }
    return ESP_OK;
}

bool val_compare(const esp_matter_attr_val_t *val1, const esp_matter_attr_val_t *val2)
{
    if (val1 == nullptr || val2 == nullptr) {
        return val1 == val2;
    }
    if (val1->type != val2->type) {
        return false;
    }
    switch ((val1->type & (~ESP_MATTER_VAL_NULLABLE_BASE))) {
    case ESP_MATTER_VAL_TYPE_CHAR_STRING:
    case ESP_MATTER_VAL_TYPE_OCTET_STRING:
    case ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING:
    case ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING: {
        uint16_t null_len = 
            (val1->type == ESP_MATTER_VAL_TYPE_CHAR_STRING || val1->type == ESP_MATTER_VAL_TYPE_OCTET_STRING) ? UINT8_MAX : UINT16_MAX;
        if (val1->val.a.s != val2->val.a.s) {
            return false;
        }
        if (val1->val.a.s == null_len || val1->val.a.s == 0) {
            return true;
        }
        return memcmp(val1->val.a.b, val2->val.a.b, val1->val.a.s) == 0;
        break;
    }
    case ESP_MATTER_VAL_TYPE_UINT8:
    case ESP_MATTER_VAL_TYPE_ENUM8:
    case ESP_MATTER_VAL_TYPE_BITMAP8:
        return val1->val.u8 == val2->val.u8;
        break;
    case ESP_MATTER_VAL_TYPE_UINT16:
    case ESP_MATTER_VAL_TYPE_ENUM16:
    case ESP_MATTER_VAL_TYPE_BITMAP16:
        return val1->val.u16 == val2->val.u16;
        break;
    case ESP_MATTER_VAL_TYPE_UINT32:
    case ESP_MATTER_VAL_TYPE_BITMAP32:
        return val1->val.u32 == val2->val.u32;
        break;
    case ESP_MATTER_VAL_TYPE_UINT64:
        return val1->val.u64 == val2->val.u64;
        break;
    case ESP_MATTER_VAL_TYPE_INT8:
        return val1->val.i8 == val2->val.i8;
        break;
    case ESP_MATTER_VAL_TYPE_INT16:
        return val1->val.i16 == val2->val.i16;
        break;
    case ESP_MATTER_VAL_TYPE_INT32:
        return val1->val.i32 == val2->val.i32;
        break;
    case ESP_MATTER_VAL_TYPE_INT64:
        return val1->val.i64 == val2->val.i64;
        break;
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        return val1->val.b == val2->val.b;
        break;
    case ESP_MATTER_VAL_TYPE_FLOAT:
        return val1->val.f == val2->val.f;
        break;
    default:
        ESP_LOGE(TAG, "Unsupported type to compare");
    }
    return false;
}

} /* attribute */
} /* esp_matter */
