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

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_console.h>
#include <esp_matter_core.h>
#include <string.h>

#include <app/util/attribute-storage.h>
#include <protocols/interaction_model/Constants.h>

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
        attr_val.val.i = val.value();
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
                .n = data_size,
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
                .n = data_size,
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
                .n = count,
                .t = (uint16_t)(data_size + data_size_len),
            },
        },
    };
    return attr_val;
}

namespace esp_matter {
namespace attribute {

static esp_matter_val_type_t get_val_type_from_attribute_type(int attribute_type);
static callback_t attribute_callback = NULL;
#if CONFIG_ENABLE_CHIP_SHELL
static esp_matter::console::engine attribute_console;

static esp_err_t console_set_handler(int argc, char **argv)
{
    if (argc < 4) {
        ESP_LOGE(TAG, "The arguments for this command is invalid");
        return ESP_ERR_INVALID_ARG;
    }

    uint16_t endpoint_id = strtol((const char *)&argv[0][2], NULL, 16);
    uint32_t cluster_id = strtol((const char *)&argv[1][2], NULL, 16);
    uint32_t attribute_id = strtol((const char *)&argv[2][2], NULL, 16);

    /* Get type from matter_attribute */
    const EmberAfAttributeMetadata *matter_attribute = emberAfLocateAttributeMetadata(endpoint_id, cluster_id,
                                                                    attribute_id);
    if (!matter_attribute) {
        ESP_LOGE(TAG, "Matter attribute not found");
        return ESP_ERR_INVALID_ARG;
    }

    /* Use the type to create the val and then update te attribute */
    esp_matter_val_type_t type = get_val_type_from_attribute_type(matter_attribute->attributeType);
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    if (type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        bool value = atoi(argv[3]);
        val = esp_matter_bool(value);
    } else if (type == ESP_MATTER_VAL_TYPE_INT8) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_int8(nullable<int8_t>());
            } else {
                int8_t value = atoi(argv[3]);
                val = esp_matter_nullable_int8(value);
            }
        } else {
            int8_t value = atoi(argv[3]);
            val = esp_matter_int8(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_UINT8) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_uint8(nullable<uint8_t>());
            } else {
                uint8_t value = atoi(argv[3]);
                val = esp_matter_nullable_uint8(value);
            }
        } else {
            uint8_t value = atoi(argv[3]);
            val = esp_matter_uint8(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_INT16) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_int16(nullable<int16_t>());
            } else {
                int16_t value = atoi(argv[3]);
                val = esp_matter_nullable_int16(value);
            }
        } else {
            int16_t value = atoi(argv[3]);
            val = esp_matter_int16(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_UINT16) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_uint16(nullable<uint16_t>());
            } else {
                uint16_t value = atoi(argv[3]);
                val = esp_matter_nullable_uint16(value);
            }
        } else {
            uint16_t value = atoi(argv[3]);
            val = esp_matter_uint16(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_INT32) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_int32(nullable<int32_t>());
            } else {
                int32_t value = atoi(argv[3]);
                val = esp_matter_nullable_int32(value);
            }
        } else {
            int32_t value = atoi(argv[3]);
            val = esp_matter_int32(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_UINT32) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_uint32(nullable<uint32_t>());
            } else {
                uint32_t value = atoi(argv[3]);
                val = esp_matter_nullable_uint32(value);
            }
        } else {
            uint32_t value = atoi(argv[3]);
            val = esp_matter_uint32(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_INT64) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_int64(nullable<int64_t>());
            } else {
                int64_t value = atoi(argv[3]);
                val = esp_matter_nullable_int64(value);
            }
        } else {
            int64_t value = atoi(argv[3]);
            val = esp_matter_int64(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_UINT64) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_uint64(nullable<uint64_t>());
            } else {
                uint64_t value = atoi(argv[3]);
                val = esp_matter_nullable_uint64(value);
            }
        } else {
            uint64_t value = atoi(argv[3]);
            val = esp_matter_uint64(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_CHAR_STRING) {
        char *value = argv[3];
        val = esp_matter_char_str(value, strlen(value));
    } else if (type == ESP_MATTER_VAL_TYPE_BITMAP8) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_bitmap8(nullable<uint8_t>());
            } else {
                uint8_t value = atoi(argv[3]);
                val = esp_matter_nullable_bitmap8(value);
            }
        } else {
            uint8_t value = atoi(argv[3]);
            val = esp_matter_bitmap8(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_BITMAP16) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_bitmap16(nullable<uint16_t>());
            } else {
                uint16_t value = atoi(argv[3]);
                val = esp_matter_nullable_bitmap16(value);
            }
        } else {
            uint16_t value = atoi(argv[3]);
            val = esp_matter_bitmap16(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_BITMAP32) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_bitmap32(nullable<uint32_t>());
            } else {
                uint32_t value = atoi(argv[3]);
                val = esp_matter_nullable_bitmap32(value);
            }
        } else {
            uint32_t value = atoi(argv[3]);
            val = esp_matter_bitmap32(value);
        }
    } else if (type == ESP_MATTER_VAL_TYPE_ENUM8) {
        if (matter_attribute->IsNullable()) {
            if (strncmp(argv[3], "null", sizeof("null")) == 0) {
                val = esp_matter_nullable_enum8(nullable<uint8_t>());
            } else {
                uint8_t value = atoi(argv[3]);
                val = esp_matter_nullable_enum8(value);
            }
        } else {
            uint8_t value = atoi(argv[3]);
            val = esp_matter_enum8(value);
        }
    } else {
        ESP_LOGE(TAG, "Type not handled: %d", type);
        return ESP_ERR_INVALID_ARG;
    }
    return update(endpoint_id, cluster_id, attribute_id, &val);
}

static esp_err_t console_get_handler(int argc, char **argv)
{
    if (argc < 3) {
        ESP_LOGE(TAG, "The arguments for this command is invalid");
        return ESP_ERR_INVALID_ARG;
    }
    uint16_t endpoint_id = strtol((const char *)&argv[0][2], NULL, 16);
    uint32_t cluster_id = strtol((const char *)&argv[1][2], NULL, 16);
    uint32_t attribute_id = strtol((const char *)&argv[2][2], NULL, 16);

    /* Get type from matter_attribute */
    const EmberAfAttributeMetadata *matter_attribute = emberAfLocateAttributeMetadata(endpoint_id, cluster_id,
                                                                    attribute_id);
    if (!matter_attribute) {
        ESP_LOGE(TAG, "Matter attribute not found");
        return ESP_ERR_INVALID_ARG;
    }

    /* Use the type to read the raw value and then print */
    esp_matter_val_type_t type = get_val_type_from_attribute_type(matter_attribute->attributeType);
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    if (type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        using Traits = chip::app::NumericAttributeTraits<bool>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        val = esp_matter_bool(Traits::StorageToWorking(value));
    } else if (type == ESP_MATTER_VAL_TYPE_INT8) {
        using Traits = chip::app::NumericAttributeTraits<int8_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_int8(nullable<int8_t>());
            } else {
                val = esp_matter_nullable_int8(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_int8(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_UINT8) {
        using Traits = chip::app::NumericAttributeTraits<uint8_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_uint8(nullable<uint8_t>());
            } else {
                val = esp_matter_nullable_uint8(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_uint8(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_INT16) {
        using Traits = chip::app::NumericAttributeTraits<int16_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_int16(nullable<int16_t>());
            } else {
                val = esp_matter_nullable_int16(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_int16(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_UINT16) {
        using Traits = chip::app::NumericAttributeTraits<uint16_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_uint16(nullable<uint16_t>());
            } else {
                val = esp_matter_nullable_uint16(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_uint16(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_INT32) {
        using Traits = chip::app::NumericAttributeTraits<int32_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_int32(nullable<int32_t>());
            } else {
                val = esp_matter_nullable_int32(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_int32(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_UINT32) {
        using Traits = chip::app::NumericAttributeTraits<uint32_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_uint32(nullable<uint32_t>());
            } else {
                val = esp_matter_nullable_uint32(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_uint32(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_INT64) {
        using Traits = chip::app::NumericAttributeTraits<int64_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_int64(nullable<int64_t>());
            } else {
                val = esp_matter_nullable_int64(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_int64(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_UINT64) {
        using Traits = chip::app::NumericAttributeTraits<uint64_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_uint64(nullable<uint64_t>());
            } else {
                val = esp_matter_nullable_uint64(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_uint64(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_CHAR_STRING) {
        /* Get raw value */
        char value[256] = {0}; /* It can go upto 256 since only 1 byte (first) is used for size */
        get_val_raw(endpoint_id, cluster_id, attribute_id, (uint8_t *)&value, sizeof(value));
        /* Get val from raw value */
        val = esp_matter_char_str(NULL, 0);
        int data_size_len = val.val.a.t - val.val.a.s;
        int data_count = 0;
        memcpy(&data_count, &value[0], data_size_len);
        val = esp_matter_char_str((char *)(value + data_size_len), data_count);
    } else if (type == ESP_MATTER_VAL_TYPE_BITMAP8) {
        using Traits = chip::app::NumericAttributeTraits<uint8_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_bitmap8(nullable<uint8_t>());
            } else {
                val = esp_matter_nullable_bitmap8(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_bitmap8(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_BITMAP16) {
        using Traits = chip::app::NumericAttributeTraits<uint16_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_bitmap16(nullable<uint16_t>());
            } else {
                val = esp_matter_nullable_bitmap16(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_bitmap16(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_BITMAP32) {
        using Traits = chip::app::NumericAttributeTraits<uint32_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_bitmap32(nullable<uint32_t>());
            } else {
                val = esp_matter_nullable_bitmap32(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_bitmap32(Traits::StorageToWorking(value));
        }
    } else if (type == ESP_MATTER_VAL_TYPE_ENUM8) {
        using Traits = chip::app::NumericAttributeTraits<uint8_t>;
        Traits::StorageType value;
        uint8_t *read_able = Traits::ToAttributeStoreRepresentation(value);
        get_val_raw(endpoint_id, cluster_id, attribute_id, read_able, sizeof(value));
        if (matter_attribute->IsNullable()) {
            if (Traits::IsNullValue(value)) {
                val = esp_matter_nullable_enum8(nullable<uint8_t>());
            } else {
                val = esp_matter_nullable_enum8(Traits::StorageToWorking(value));
            }
        } else {
            val = esp_matter_enum8(Traits::StorageToWorking(value));
        }
    } else {
        ESP_LOGE(TAG, "Type not handled: %d", type);
        return ESP_ERR_INVALID_ARG;
    }
    val_print(endpoint_id, cluster_id, attribute_id, &val);
    return ESP_OK;
}

static esp_err_t console_dispatch(int argc, char **argv)
{
    if (argc <= 0) {
        attribute_console.for_each_command(esp_matter::console::print_description, NULL);
        return ESP_OK;
    }
    return attribute_console.exec_command(argc, argv);
}

static void register_console_commands()
{
    static bool init_done = false;
    if (init_done) {
        return;
    }
    static const esp_matter::console::command_t command = {
        .name = "attribute",
        .description = "This can be used to simulate on-device control. ",
        .handler = console_dispatch,
    };

    static const esp_matter::console::command_t attribute_commands[] = {
        {
            .name = "set",
            .description = "Set an attribute value of a cluster on an endpoint. "
                           "Usage: matter esp attribute set <endpoint_id> <cluster_id> <attribute_id> <value>. "
                           "Example: matter esp attribute set 0x0001 0x0006 0x0000 1.",
            .handler = console_set_handler,
        },
        {
            .name = "get",
            .description = "Get an attribute value of a cluster on an endpoint. "
                           "Usage: matter esp attribute get <endpoint_id> <cluster_id> <attribute_id>. "
                           "Example: matter esp attribute get 0x0001 0x0006 0x0000.",
            .handler = console_get_handler,
        },
    };
    attribute_console.register_commands(attribute_commands, sizeof(attribute_commands)/sizeof(esp_matter::console::command_t));
    esp_matter::console::add_commands(&command, 1);
    init_done = true;
}
#endif // CONFIG_ENABLE_CHIP_SHELL

esp_err_t set_callback(callback_t callback)
{
    attribute_callback = callback;

    /* Other initialisations */
#if CONFIG_ENABLE_CHIP_SHELL
    register_console_commands();
#endif
    return ESP_OK;
}

static esp_err_t execute_callback(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                  uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    if (attribute_callback) {
        void *priv_data = endpoint::get_priv_data(endpoint_id);
        return attribute_callback(type, endpoint_id, cluster_id, attribute_id, val, priv_data);
    }
    return ESP_OK;
}

static esp_err_t execute_override_callback(attribute_t *attribute, callback_type_t type, uint16_t endpoint_id,
                                           uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    callback_t override_callback = attribute::get_override_callback(attribute);
    if (override_callback) {
        return override_callback(type, endpoint_id, cluster_id, attribute_id, val, NULL);
    } else {
        ESP_LOGI(TAG, "Attribute override callback not set, calling the common callback");
        return execute_callback(type, endpoint_id, cluster_id, attribute_id, val);
    }
    return ESP_OK;
}

static esp_matter_val_type_t get_val_type_from_attribute_type(int attribute_type)
{
    switch (attribute_type) {
    case ZCL_BOOLEAN_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_BOOLEAN;
        break;

    case ZCL_SINGLE_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_FLOAT;
        break;

    case ZCL_ARRAY_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_ARRAY;
        break;

    case ZCL_CHAR_STRING_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_CHAR_STRING;
        break;

    case ZCL_OCTET_STRING_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_OCTET_STRING;
        break;

    case ZCL_INT8S_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_INT8;
        break;

    case ZCL_INT8U_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_UINT8;
        break;

    case ZCL_INT16S_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_INT16;
        break;

    case ZCL_INT16U_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_UINT16;
        break;

    case ZCL_INT32S_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_INT32;
        break;

    case ZCL_INT32U_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_UINT32;
        break;

    case ZCL_INT64S_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_INT64;
        break;

    case ZCL_INT64U_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_UINT64;
        break;

    case ZCL_ENUM8_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_ENUM8;
        break;

    case ZCL_BITMAP8_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_BITMAP8;
        break;

    case ZCL_BITMAP16_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_BITMAP16;
        break;

    case ZCL_BITMAP32_ATTRIBUTE_TYPE:
        return ESP_MATTER_VAL_TYPE_BITMAP32;
        break;

    default:
        return ESP_MATTER_VAL_TYPE_INVALID;
        break;
    }
    return ESP_MATTER_VAL_TYPE_INVALID;
}

bool val_is_null(esp_matter_attr_val_t *val)
{
    switch (val->type) {
    case ESP_MATTER_VAL_TYPE_NULLABLE_INTEGER:
        return chip::app::NumericAttributeTraits<int>::IsNullValue(val->val.i);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8:
        return chip::app::NumericAttributeTraits<uint8_t>::IsNullValue(val->val.u8);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT16:
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

esp_err_t get_data_from_attr_val(esp_matter_attr_val_t *val, EmberAfAttributeType *attribute_type,
                                 uint16_t *attribute_size, uint8_t *value)
{
    switch (val->type) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        if (attribute_type) {
            *attribute_type = ZCL_BOOLEAN_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(bool);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<bool>;
            Traits::WorkingToStorage(val->val.b, *value);
        }
        break;

    case ESP_MATTER_VAL_TYPE_INTEGER:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INTEGER:
        if (attribute_type) {
            *attribute_type = ZCL_INT16U_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(int);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<int>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.i)) {
                Traits::SetNull(*(int *)value);
            } else {
                Traits::WorkingToStorage(val->val.i, *(int *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_FLOAT:
    case ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT:
        if (attribute_type) {
            *attribute_type = ZCL_SINGLE_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(float);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<float>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.f)) {
                Traits::SetNull(*(float *)value);
            } else {
                Traits::WorkingToStorage(val->val.f, *(float *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_ARRAY:
        if (attribute_type) {
            *attribute_type = ZCL_ARRAY_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = val->val.a.t;
        }
        if (value) {
            int data_size_len = val->val.a.t - val->val.a.s;
            memcpy(value, (uint8_t *)&val->val.a.s, data_size_len);
            memcpy((value + data_size_len), (uint8_t *)val->val.a.b, (*attribute_size - data_size_len));
        }
        break;

    case ESP_MATTER_VAL_TYPE_CHAR_STRING:
        if (attribute_type) {
            *attribute_type = ZCL_CHAR_STRING_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = val->val.a.t;
        }
        if (value) {
            int data_size_len = val->val.a.t - val->val.a.s;
            memcpy(value, (uint8_t *)&val->val.a.s, data_size_len);
            memcpy((value + data_size_len), (uint8_t *)val->val.a.b, (*attribute_size - data_size_len));
        }
        break;

    case ESP_MATTER_VAL_TYPE_OCTET_STRING:
        if (attribute_type) {
            *attribute_type = ZCL_OCTET_STRING_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = val->val.a.t;
        }
        if (value) {
            int data_size_len = val->val.a.t - val->val.a.s;
            memcpy(value, (uint8_t *)&val->val.a.s, data_size_len);
            memcpy((value + data_size_len), (uint8_t *)val->val.a.b, (*attribute_size - data_size_len));
        }
        break;

    case ESP_MATTER_VAL_TYPE_INT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT8:
        if (attribute_type) {
            *attribute_type = ZCL_INT8S_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(int8_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<int8_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.i8)) {
                Traits::SetNull(*(int8_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.i8, *(int8_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_UINT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT8:
        if (attribute_type) {
            *attribute_type = ZCL_INT8U_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(uint8_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<uint8_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.u8)) {
                Traits::SetNull(*(uint8_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.u8, *(uint8_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_INT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT16:
        if (attribute_type) {
            *attribute_type = ZCL_INT16S_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(int16_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<int16_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.i16)) {
                Traits::SetNull(*(int16_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.i16, *(int16_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_UINT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT16:
        if (attribute_type) {
            *attribute_type = ZCL_INT16U_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(uint16_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<uint16_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.u16)) {
                Traits::SetNull(*(uint16_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.u16, *(uint16_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_INT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT32:
        if (attribute_type) {
            *attribute_type = ZCL_INT32S_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(int32_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<int32_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.i32)) {
                Traits::SetNull(*(int32_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.i32, *(int32_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_UINT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT32:
        if (attribute_type) {
            *attribute_type = ZCL_INT32U_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(uint32_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<uint32_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.u32)) {
                Traits::SetNull(*(uint32_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.u32, *(uint32_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_INT64:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT64:
        if (attribute_type) {
            *attribute_type = ZCL_INT64S_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(int64_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<int64_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.i64)) {
                Traits::SetNull(*(int64_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.i64, *(int64_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_UINT64:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT64:
        if (attribute_type) {
            *attribute_type = ZCL_INT64U_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(uint64_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<uint64_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.u64)) {
                Traits::SetNull(*(uint64_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.u64, *(uint64_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_ENUM8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8:
        if (attribute_type) {
            *attribute_type = ZCL_ENUM8_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(uint8_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<uint8_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.u8)) {
                Traits::SetNull(*(uint8_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.u8, *(uint8_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_BITMAP8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8:
        if (attribute_type) {
            *attribute_type = ZCL_BITMAP8_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(uint8_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<uint8_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.u8)) {
                Traits::SetNull(*(uint8_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.u8, *(uint8_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_BITMAP16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16:
        if (attribute_type) {
            *attribute_type = ZCL_BITMAP16_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(uint16_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<uint16_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.u16)) {
                Traits::SetNull(*(uint16_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.u16, *(uint16_t *)value);
            }
        }
        break;

    case ESP_MATTER_VAL_TYPE_BITMAP32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32:
        if (attribute_type) {
            *attribute_type = ZCL_BITMAP32_ATTRIBUTE_TYPE;
        }
        if (attribute_size) {
            *attribute_size = sizeof(uint32_t);
        }
        if (value) {
            using Traits = chip::app::NumericAttributeTraits<uint32_t>;
            if ((val->type & ESP_MATTER_VAL_NULLANLE_BASE) && Traits::IsNullValue(val->val.u32)) {
                Traits::SetNull(*(uint32_t *)value);
            } else {
                Traits::WorkingToStorage(val->val.u32, *(uint32_t *)value);
            }
        }
        break;

    default:
        ESP_LOGE(TAG, "esp_matter_attr_val_type_t not handled: %d", val->type);
        break;
    }

    return ESP_OK;
}

static esp_err_t get_attr_val_from_data(esp_matter_attr_val_t *val, EmberAfAttributeType attribute_type,
                                        uint16_t attribute_size, uint8_t *value,
                                        const EmberAfAttributeMetadata * attribute_metadata)
{
    switch (attribute_type) {
    case ZCL_BOOLEAN_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<bool>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        *val = esp_matter_bool(attribute_value);
        break;
    }

    case ZCL_ARRAY_ATTRIBUTE_TYPE: {
        *val = esp_matter_array(NULL, 0, 0);
        int data_size_len = val->val.a.t - val->val.a.s;
        int data_count = 0;
        memcpy(&data_count, &value[0], data_size_len);
        *val = esp_matter_array((value + data_size_len), attribute_size, data_count);
        break;
    }

    case ZCL_CHAR_STRING_ATTRIBUTE_TYPE: {
        *val = esp_matter_char_str(NULL, 0);
        int data_size_len = val->val.a.t - val->val.a.s;
        int data_count = 0;
        memcpy(&data_count, &value[0], data_size_len);
        *val = esp_matter_char_str((char *)(value + data_size_len), data_count);
        break;
    }

    case ZCL_OCTET_STRING_ATTRIBUTE_TYPE: {
        *val = esp_matter_octet_str(NULL, 0);
        int data_size_len = val->val.a.t - val->val.a.s;
        int data_count = 0;
        memcpy(&data_count, &value[0], data_size_len);
        *val = esp_matter_octet_str((value + data_size_len), data_count);
        break;
    }

    case ZCL_INT8S_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<int8_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_int8(nullable<int8_t>());
            } else {
                *val = esp_matter_nullable_int8(attribute_value);
            }
        } else {
            *val = esp_matter_int8(attribute_value);
        }
        break;
    }

    case ZCL_INT8U_ATTRIBUTE_TYPE:
    case ZCL_PERCENT_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<uint8_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_uint8(nullable<uint8_t>());
            } else {
                *val = esp_matter_nullable_uint8(attribute_value);
            }
        } else {
            *val = esp_matter_uint8(attribute_value);
        }
        break;
    }

    case ZCL_INT16S_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<int16_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_int16(nullable<int16_t>());
            } else {
                *val = esp_matter_nullable_int16(attribute_value);
            }
        } else {
            *val = esp_matter_int16(attribute_value);
        }
        break;
    }

    case ZCL_INT16U_ATTRIBUTE_TYPE:
    case ZCL_PERCENT100THS_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<uint16_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_uint16(nullable<uint16_t>());
            } else {
                *val = esp_matter_nullable_uint16(attribute_value);
            }
        } else {
            *val = esp_matter_uint16(attribute_value);
        }
        break;
    }

    case ZCL_INT32S_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<int32_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_int32(nullable<int32_t>());
            } else {
                *val = esp_matter_nullable_int32(attribute_value);
            }
        } else {
            *val = esp_matter_int32(attribute_value);
        }
        break;
    }

    case ZCL_INT32U_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<uint32_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_uint32(nullable<uint32_t>());
            } else {
                *val = esp_matter_nullable_uint32(attribute_value);
            }
        } else {
            *val = esp_matter_uint32(attribute_value);
        }
        break;
    }

    case ZCL_INT64S_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<int64_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_int64(nullable<int64_t>());
            } else {
                *val = esp_matter_nullable_int64(attribute_value);
            }
        } else {
            *val = esp_matter_int64(attribute_value);
        }
        break;
    }

    case ZCL_INT64U_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<uint64_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_uint64(nullable<uint64_t>());
            } else {
                *val = esp_matter_nullable_uint64(attribute_value);
            }
        } else {
            *val = esp_matter_uint64(attribute_value);
        }
        break;
    }

    case ZCL_ENUM8_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<uint8_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_enum8(nullable<uint8_t>());
            } else {
                *val = esp_matter_nullable_enum8(attribute_value);
            }
        } else {
            *val = esp_matter_enum8(attribute_value);
        }
        break;
    }

    case ZCL_BITMAP8_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<uint8_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_bitmap8(nullable<uint8_t>());
            } else {
                *val = esp_matter_nullable_bitmap8(attribute_value);
            }
        } else {
            *val = esp_matter_bitmap8(attribute_value);
        }
        break;
    }

    case ZCL_BITMAP16_ATTRIBUTE_TYPE: {
        using Traits = chip::app::NumericAttributeTraits<uint16_t>;
        Traits::StorageType attribute_value;
        memcpy((uint8_t *)&attribute_value, value, sizeof(Traits::StorageType));
        if (attribute_metadata->IsNullable()) {
            if (Traits::IsNullValue(attribute_value)) {
                *val = esp_matter_nullable_bitmap16(nullable<uint16_t>());
            } else {
                *val = esp_matter_nullable_bitmap16(attribute_value);
            }
        } else {
            *val = esp_matter_bitmap16(attribute_value);
        }
        break;
    }

    default:
        *val = esp_matter_invalid(NULL);
        break;
    }

    return ESP_OK;
}

void val_print(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    if (val_is_null(val)) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is null **********", endpoint_id,
                 cluster_id, attribute_id);
        return;
    }

    if (val->type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val->val.b);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INTEGER || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_INTEGER) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val->val.i);
    } else if (val->type == ESP_MATTER_VAL_TYPE_FLOAT || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %f **********", endpoint_id,
                 cluster_id, attribute_id, val->val.f);
    } else if (val->type == ESP_MATTER_VAL_TYPE_UINT8 || val->type == ESP_MATTER_VAL_TYPE_BITMAP8
               || val->type == ESP_MATTER_VAL_TYPE_ENUM8 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT8
               || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val->val.u8);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INT16 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_INT16) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val->val.i16);
    } else if (val->type == ESP_MATTER_VAL_TYPE_UINT16 || val->type == ESP_MATTER_VAL_TYPE_BITMAP16
               || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT16 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val->val.u16);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INT32|| val->type == ESP_MATTER_VAL_TYPE_NULLABLE_INT32) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val->val.i32);
    } else if (val->type == ESP_MATTER_VAL_TYPE_UINT32 || val->type == ESP_MATTER_VAL_TYPE_BITMAP32
               || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT32 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val->val.u32);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INT64 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_INT64) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %lld **********", endpoint_id,
                 cluster_id, attribute_id, val->val.i64);
    } else if (val->type == ESP_MATTER_VAL_TYPE_UINT64 || val->type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT64) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %lld **********", endpoint_id,
                 cluster_id, attribute_id, val->val.u64);
    } else if (val->type == ESP_MATTER_VAL_TYPE_CHAR_STRING) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %.*s **********", endpoint_id,
                 cluster_id, attribute_id, val->val.a.s, val->val.a.b);
    } else {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is <invalid type: %d> **********",
                 endpoint_id, cluster_id, attribute_id, val->type);
    }
}

esp_err_t get_val_raw(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, uint8_t *value,
                      uint16_t attribute_size)
{
    /* Take lock if not already taken */
    lock::status_t lock_status = lock::chip_stack_lock(portMAX_DELAY);
    if (lock_status == lock::FAILED) {
        ESP_LOGE(TAG, "Could not get task context");
        return ESP_FAIL;
    }

    EmberAfStatus status = emberAfReadServerAttribute(endpoint_id, cluster_id, attribute_id, value, attribute_size);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
        ESP_LOGE(TAG, "Error getting raw value from matter: 0x%x", status);
        if (lock_status == lock::SUCCESS) {
            lock::chip_stack_unlock();
        }
        return ESP_FAIL;
    }
    if (lock_status == lock::SUCCESS) {
        lock::chip_stack_unlock();
    }
    return ESP_OK;
}

esp_err_t update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    /* Take lock if not already taken */
    lock::status_t lock_status = lock::chip_stack_lock(portMAX_DELAY);
    if (lock_status == lock::FAILED) {
        ESP_LOGE(TAG, "Could not get task context");
        return ESP_FAIL;
    }

    /* Get size */
    EmberAfAttributeType attribute_type = 0;
    uint16_t attribute_size = 0;
    get_data_from_attr_val(val, &attribute_type, &attribute_size, NULL);

    /* Get value */
    uint8_t *value = (uint8_t *)calloc(1, attribute_size);
    if (!value) {
        ESP_LOGE(TAG, "Could not allocate value buffer");
        if (lock_status == lock::SUCCESS) {
            lock::chip_stack_unlock();
        }
        return ESP_ERR_NO_MEM;
    }
    get_data_from_attr_val(val, &attribute_type, &attribute_size, value);

    /* Update matter */
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;
    if (emberAfContainsServer(endpoint_id, cluster_id)) {
        status = emberAfWriteServerAttribute(endpoint_id, cluster_id, attribute_id, value, attribute_type);
        if (status != EMBER_ZCL_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Error updating attribute to matter: 0x%X", status);
            free(value);
            if (lock_status == lock::SUCCESS) {
                lock::chip_stack_unlock();
            }
            return ESP_FAIL;
        }
    }
    free(value);
    if (lock_status == lock::SUCCESS) {
        lock::chip_stack_unlock();
    }
    return ESP_OK;
}

} /* attribute */
} /* esp_matter */

Status MatterPreAttributeChangeCallback(const chip::app::ConcreteAttributePath &path, uint8_t type,
                                        uint16_t size, uint8_t *value)
{
    uint16_t endpoint_id = path.mEndpointId;
    uint32_t cluster_id = path.mClusterId;
    uint32_t attribute_id = path.mAttributeId;
    const EmberAfAttributeMetadata *attribute_metadata = emberAfLocateAttributeMetadata(endpoint_id, cluster_id,
                                                                                      attribute_id);
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_attr_val_from_data(&val, type, size, value, attribute_metadata);

    /* Print */
    attribute::val_print(endpoint_id, cluster_id, attribute_id, &val);

    /* Callback to application */
    esp_err_t err = execute_callback(attribute::PRE_UPDATE, endpoint_id, cluster_id, attribute_id, &val);
    if (err != ESP_OK) {
        return Status::Failure;
    }
    return Status::Success;
}

void MatterPostAttributeChangeCallback(const chip::app::ConcreteAttributePath &path, uint8_t type,
                                       uint16_t size, uint8_t *value)
{
    uint16_t endpoint_id = path.mEndpointId;
    uint32_t cluster_id = path.mClusterId;
    uint32_t attribute_id = path.mAttributeId;
    const EmberAfAttributeMetadata *attribute_metadata = emberAfLocateAttributeMetadata(endpoint_id, cluster_id,
                                                                                      attribute_id);
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_attr_val_from_data(&val, type, size, value, attribute_metadata);

    /* Callback to application */
    execute_callback(attribute::POST_UPDATE, endpoint_id, cluster_id, attribute_id, &val);
}

EmberAfStatus emberAfExternalAttributeReadCallback(EndpointId endpoint_id, ClusterId cluster_id,
                                                   const EmberAfAttributeMetadata *matter_attribute, uint8_t *buffer,
                                                   uint16_t max_read_length)
{
    /* Get value */
    uint32_t attribute_id = matter_attribute->attributeId;
    node_t *node = node::get();
    if (!node) {
        return EMBER_ZCL_STATUS_FAILURE;
    }
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, cluster_id);
    attribute_t *attribute = attribute::get(cluster, attribute_id);
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    int flags = attribute::get_flags(attribute);
    if (flags & ATTRIBUTE_FLAG_OVERRIDE) {
        esp_err_t err = execute_override_callback(attribute, attribute::READ, endpoint_id, cluster_id, attribute_id,
                                                  &val);
        if (err != ESP_OK) {
            return EMBER_ZCL_STATUS_FAILURE;
        }
    } else {
        attribute::get_val(attribute, &val);
    }

    /* Print */
    attribute::val_print(endpoint_id, cluster_id, attribute_id, &val);

    /* Get size */
    uint16_t attribute_size = 0;
    attribute::get_data_from_attr_val(&val, NULL, &attribute_size, NULL);
    if (attribute_size > max_read_length) {
        ESP_LOGE(TAG, "Insufficient space for reading attribute: required: %d, max: %d", attribute_size,
                 max_read_length);
        return EMBER_ZCL_STATUS_RESOURCE_EXHAUSTED;
    }

    /* Assign value */
    attribute::get_data_from_attr_val(&val, NULL, &attribute_size, buffer);
    return EMBER_ZCL_STATUS_SUCCESS;
}

EmberAfStatus emberAfExternalAttributeWriteCallback(EndpointId endpoint_id, ClusterId cluster_id,
                                                    const EmberAfAttributeMetadata *matter_attribute, uint8_t *buffer)
{
    /* Get value */
    uint32_t attribute_id = matter_attribute->attributeId;
    node_t *node = node::get();
    if (!node) {
        return EMBER_ZCL_STATUS_FAILURE;
    }
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, cluster_id);
    attribute_t *attribute = attribute::get(cluster, attribute_id);

    /* Get val */
    /* This creates a new variable val, and stores the new attribute value in the new variable.
    The value in esp-matter data model is updated only when attribute::set_val() is called */
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_attr_val_from_data(&val, matter_attribute->attributeType, matter_attribute->size, buffer, matter_attribute);

    int flags = attribute::get_flags(attribute);
    if (flags & ATTRIBUTE_FLAG_OVERRIDE) {
        esp_err_t err = execute_override_callback(attribute, attribute::WRITE, endpoint_id, cluster_id, attribute_id,
                                                  &val);
        EmberAfStatus status = (err == ESP_OK) ? EMBER_ZCL_STATUS_SUCCESS : EMBER_ZCL_STATUS_FAILURE;
        return status;
    }

    /* Update val */
    if (val.type == ESP_MATTER_VAL_TYPE_INVALID) {
        return EMBER_ZCL_STATUS_FAILURE;
    }
    attribute::set_val(attribute, &val);
    return EMBER_ZCL_STATUS_SUCCESS;
}
