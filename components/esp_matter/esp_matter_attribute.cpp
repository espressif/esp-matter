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
#include <esp_matter_attribute.h>
#include <esp_matter_core.h>
#include <string.h>

#include <app/util/attribute-storage.h>
#include <protocols/interaction_model/Constants.h>

using chip::AttributeId;
using chip::ClusterId;
using chip::EndpointId;
using chip::Protocols::InteractionModel::Status;

static const char *TAG = "esp_matter";

static esp_matter_attribute_callback_t attribute_callback = NULL;
static void *attribute_callback_priv_data = NULL;

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

esp_matter_attr_val_t esp_matter_str(const char *val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_STRING,
        .val = {
            .s = (char *)val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_json_obj(const char *val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_JSON_OBJECT,
        .val = {
            .s = (char *)val,
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_json_array(const char *val)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_JSON_ARRAY,
        .val = {
            .s = (char *)val,
        },
    };
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

esp_matter_attr_val_t esp_matter_char_str(char *val, uint16_t total_size)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_CHAR_STRING,
        .val = {
            .a = {
                .b = (uint8_t *)val,
                .s = total_size,
                .n = 0,
            },
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_octet_str(uint8_t *val, uint16_t total_size, uint16_t count)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_OCTET_STRING,
        .val = {
            .a = {
                .b = val,
                .s = total_size,
                .n = count,
            },
        },
    };
    return attr_val;
}

esp_matter_attr_val_t esp_matter_array(uint8_t *val, uint16_t total_size, uint16_t count)
{
    esp_matter_attr_val_t attr_val = {
        .type = ESP_MATTER_VAL_TYPE_ARRAY,
        .val = {
            .a = {
                .b = val,
                .s = total_size,
                .n = count,
            },
        },
    };
    return attr_val;
}

esp_err_t esp_matter_attribute_get_type_and_val_default(esp_matter_attr_val_t *val,
                                                        EmberAfAttributeType *attribute_type, uint16_t *attribute_size,
                                                        EmberAfDefaultOrMinMaxAttributeValue *default_value)
{
    switch (val->type) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        *attribute_type = ZCL_BOOLEAN_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(bool);
        *default_value = (uint16_t)val->val.b;
        break;

    case ESP_MATTER_VAL_TYPE_INTEGER:
        *attribute_type = ZCL_INT16U_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint16_t);
        *default_value = (uint16_t)val->val.i;
        break;

    case ESP_MATTER_VAL_TYPE_FLOAT:
        *attribute_type = ZCL_SINGLE_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(float);
        *default_value = (uint8_t *)&val->val.f;
        break;

    case ESP_MATTER_VAL_TYPE_STRING:
        *attribute_type = ZCL_CHAR_STRING_ATTRIBUTE_TYPE;
        if (val->val.s) {
            *attribute_size = strlen(val->val.s);
        } else {
            *attribute_size = 0;
        }
        *default_value = (uint8_t *)val->val.s;
        break;

    case ESP_MATTER_VAL_TYPE_ARRAY:
        *attribute_type = ZCL_ARRAY_ATTRIBUTE_TYPE;
        *attribute_size = val->val.a.s;
        *default_value = (uint8_t *)val->val.a.b;
        break;

    case ESP_MATTER_VAL_TYPE_CHAR_STRING:
        *attribute_type = ZCL_CHAR_STRING_ATTRIBUTE_TYPE;
        *attribute_size = val->val.a.s;
        *default_value = (uint8_t *)val->val.a.b;
        break;

    case ESP_MATTER_VAL_TYPE_BYTE_STRING:
        ESP_LOGE(TAG, "esp_matter_attr_val_type_t not supported: %d", val->type);
        break;

    case ESP_MATTER_VAL_TYPE_OCTET_STRING:
        *attribute_type = ZCL_OCTET_STRING_ATTRIBUTE_TYPE;
        *attribute_size = val->val.a.s;
        *default_value = (uint8_t *)val->val.a.b;
        break;

    case ESP_MATTER_VAL_TYPE_INT8:
        *attribute_type = ZCL_INT8S_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(int8_t);
        *default_value = (uint16_t)val->val.i8;
        break;

    case ESP_MATTER_VAL_TYPE_UINT8:
        *attribute_type = ZCL_INT8U_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint8_t);
        *default_value = (uint16_t)val->val.u8;
        break;

    case ESP_MATTER_VAL_TYPE_INT16:
        *attribute_type = ZCL_INT16S_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(int16_t);
        *default_value = (int16_t)val->val.i16;
        break;

    case ESP_MATTER_VAL_TYPE_UINT16:
        *attribute_type = ZCL_INT16U_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint16_t);
        *default_value = (uint16_t)val->val.u16;
        break;

    case ESP_MATTER_VAL_TYPE_UINT32:
        *attribute_type = ZCL_INT32U_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint32_t);
        *default_value = (uint8_t *)&val->val.u32;
        break;

    case ESP_MATTER_VAL_TYPE_UINT64:
        *attribute_type = ZCL_INT64U_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint64_t);
        *default_value = (uint8_t *)&val->val.u64;
        break;

    case ESP_MATTER_VAL_TYPE_ENUM8:
        *attribute_type = ZCL_ENUM8_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint8_t);
        *default_value = (uint16_t)val->val.u8;
        break;

    case ESP_MATTER_VAL_TYPE_BITMAP8:
        *attribute_type = ZCL_BITMAP8_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint8_t);
        *default_value = (uint16_t)val->val.u8;
        break;

    case ESP_MATTER_VAL_TYPE_BITMAP16:
        *attribute_type = ZCL_BITMAP16_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint16_t);
        *default_value = (uint16_t)val->val.u16;
        break;

    case ESP_MATTER_VAL_TYPE_BITMAP32:
        *attribute_type = ZCL_BITMAP32_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint32_t);
        *default_value = (uint8_t *)&val->val.u32;
        break;

    default:
        ESP_LOGE(TAG, "esp_matter_attr_val_type_t not handled: %d", val->type);
        break;
    }

    return ESP_OK;
}

static esp_err_t esp_matter_attribute_get_type_and_val(esp_matter_attr_val_t *val, EmberAfAttributeType *attribute_type,
                                                       uint16_t *attribute_size, uint8_t **value)
{
    switch (val->type) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        *attribute_type = ZCL_BOOLEAN_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(bool);
        *value = (uint8_t *)&val->val.b;
        break;

    case ESP_MATTER_VAL_TYPE_INTEGER:
        *attribute_type = ZCL_INT16U_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint16_t);
        *value = (uint8_t *)&val->val.i;
        break;

    case ESP_MATTER_VAL_TYPE_FLOAT:
        *attribute_type = ZCL_SINGLE_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(float);
        *value = (uint8_t *)&val->val.f;
        break;

    case ESP_MATTER_VAL_TYPE_STRING:
        *attribute_type = ZCL_CHAR_STRING_ATTRIBUTE_TYPE;
        if (val->val.s) {
            *attribute_size = strlen(val->val.s);
        } else {
            *attribute_size = 0;
        }
        *value = (uint8_t *)val->val.s;
        break;

    case ESP_MATTER_VAL_TYPE_ARRAY:
        *attribute_type = ZCL_ARRAY_ATTRIBUTE_TYPE;
        *attribute_size = val->val.a.s;
        *value = (uint8_t *)val->val.a.b;
        break;

    case ESP_MATTER_VAL_TYPE_CHAR_STRING:
        *attribute_type = ZCL_CHAR_STRING_ATTRIBUTE_TYPE;
        *attribute_size = val->val.a.s;
        *value = (uint8_t *)val->val.a.b;
        break;

    case ESP_MATTER_VAL_TYPE_BYTE_STRING:
        ESP_LOGE(TAG, "esp_matter_attr_val_type_t not supported: %d", val->type);
        break;

    case ESP_MATTER_VAL_TYPE_OCTET_STRING:
        *attribute_type = ZCL_OCTET_STRING_ATTRIBUTE_TYPE;
        *attribute_size = val->val.a.s;
        *value = (uint8_t *)val->val.a.b;
        break;

    case ESP_MATTER_VAL_TYPE_INT8:
        *attribute_type = ZCL_INT8S_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(int8_t);
        *value = (uint8_t *)&val->val.i8;
        break;

    case ESP_MATTER_VAL_TYPE_UINT8:
        *attribute_type = ZCL_INT8U_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint8_t);
        *value = (uint8_t *)&val->val.u8;
        break;

    case ESP_MATTER_VAL_TYPE_INT16:
        *attribute_type = ZCL_INT16S_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(int16_t);
        *value = (uint8_t *)&val->val.i16;
        break;

    case ESP_MATTER_VAL_TYPE_UINT16:
        *attribute_type = ZCL_INT16U_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint16_t);
        *value = (uint8_t *)&val->val.u16;
        break;

    case ESP_MATTER_VAL_TYPE_UINT32:
        *attribute_type = ZCL_INT32U_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint32_t);
        *value = (uint8_t *)&val->val.u32;
        break;

    case ESP_MATTER_VAL_TYPE_UINT64:
        *attribute_type = ZCL_INT64U_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint64_t);
        *value = (uint8_t *)&val->val.u64;
        break;

    case ESP_MATTER_VAL_TYPE_ENUM8:
        *attribute_type = ZCL_ENUM8_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint8_t);
        *value = (uint8_t *)&val->val.u8;
        break;

    case ESP_MATTER_VAL_TYPE_BITMAP8:
        *attribute_type = ZCL_BITMAP8_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint8_t);
        *value = (uint8_t *)&val->val.u8;
        break;

    case ESP_MATTER_VAL_TYPE_BITMAP16:
        *attribute_type = ZCL_BITMAP16_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint16_t);
        *value = (uint8_t *)&val->val.u16;
        break;

    case ESP_MATTER_VAL_TYPE_BITMAP32:
        *attribute_type = ZCL_BITMAP32_ATTRIBUTE_TYPE;
        *attribute_size = sizeof(uint32_t);
        *value = (uint8_t *)&val->val.u32;
        break;

    default:
        ESP_LOGE(TAG, "esp_matter_attr_val_type_t not handled: %d", val->type);
        break;
    }

    return ESP_OK;
}

static esp_matter_attr_val_t esp_matter_attribute_get_attr_val(EmberAfAttributeType attribute_type,
                                                               uint16_t attribute_size, uint8_t *value)
{
    switch (attribute_type) {
    case ZCL_BOOLEAN_ATTRIBUTE_TYPE:
        return esp_matter_bool((bool)*value);
        break;

    case ZCL_ARRAY_ATTRIBUTE_TYPE:
        return esp_matter_array(value, attribute_size, (uint16_t)value[0]);
        break;

    case ZCL_CHAR_STRING_ATTRIBUTE_TYPE:
        return esp_matter_char_str((char *)&value[1], value[0]);
        break;

    case ZCL_OCTET_STRING_ATTRIBUTE_TYPE:
        return esp_matter_octet_str(value, attribute_size, (uint16_t)value[0]);
        break;

    case ZCL_INT8S_ATTRIBUTE_TYPE:
        return esp_matter_int8((int8_t)*value);
        break;

    case ZCL_INT8U_ATTRIBUTE_TYPE:
        return esp_matter_uint8((uint8_t)*value);
        break;

    case ZCL_INT16S_ATTRIBUTE_TYPE:
        return esp_matter_int16((int16_t)*value);
        break;

    case ZCL_INT16U_ATTRIBUTE_TYPE:
        return esp_matter_uint16((uint16_t)*value);
        break;

    case ZCL_INT32U_ATTRIBUTE_TYPE:
        return esp_matter_uint32((uint32_t)*value);
        break;

    case ZCL_INT64U_ATTRIBUTE_TYPE:
        return esp_matter_uint64((uint64_t)*value);
        break;

    case ZCL_ENUM8_ATTRIBUTE_TYPE:
        return esp_matter_enum8((uint8_t)*value);
        break;

    case ZCL_BITMAP8_ATTRIBUTE_TYPE:
        return esp_matter_bitmap8((uint8_t)*value);
        break;

    case ZCL_BITMAP16_ATTRIBUTE_TYPE:
        return esp_matter_bitmap16((uint16_t)*value);
        break;

    default:
        return esp_matter_invalid(NULL);
        break;
    }
}

void esp_matter_attribute_val_print(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t val)
{
    if (val.type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val.val.b);
    } else if (val.type == ESP_MATTER_VAL_TYPE_INTEGER) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val.val.i);
    } else if (val.type == ESP_MATTER_VAL_TYPE_FLOAT) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %f **********", endpoint_id,
                 cluster_id, attribute_id, val.val.f);
    } else if (val.type == ESP_MATTER_VAL_TYPE_STRING || val.type == ESP_MATTER_VAL_TYPE_JSON_OBJECT ||
               val.type == ESP_MATTER_VAL_TYPE_JSON_ARRAY) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %s **********", endpoint_id,
                 cluster_id, attribute_id, val.val.s);
    } else if (val.type == ESP_MATTER_VAL_TYPE_UINT8 || val.type == ESP_MATTER_VAL_TYPE_BITMAP8) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val.val.u8);
    } else if (val.type == ESP_MATTER_VAL_TYPE_INT16) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val.val.i16);
    } else if (val.type == ESP_MATTER_VAL_TYPE_UINT16) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val.val.u16);
    } else if (val.type == ESP_MATTER_VAL_TYPE_UINT32) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %d **********", endpoint_id,
                 cluster_id, attribute_id, val.val.u32);
    } else if (val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING) {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is %.*s **********", endpoint_id,
                 cluster_id, attribute_id, val.val.a.s, val.val.a.b);
    } else {
        ESP_LOGI(TAG, "********** Endpoint 0x%04X's Cluster 0x%04X's Attribute 0x%04X is <invalid type: %d> **********",
                 endpoint_id, cluster_id, attribute_id, val.type);
    }
}

esp_err_t esp_matter_attribute_callback_set(esp_matter_attribute_callback_t callback, void *priv_data)
{
    attribute_callback = callback;
    attribute_callback_priv_data = priv_data;
    return ESP_OK;
}

esp_err_t esp_matter_attribute_get_val_raw(int endpoint_id, int cluster_id, int attribute_id, uint8_t *value,
                                           uint16_t attribute_size)
{
    EmberAfStatus status = emberAfReadServerAttribute(endpoint_id, cluster_id, attribute_id, value, attribute_size);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
        ESP_LOGE(TAG, "Error getting raw value from matter");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_matter_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t val)
{
    EmberAfAttributeType attribute_type = 0;
    uint16_t attribute_size = 0;
    uint8_t *value = NULL;
    esp_matter_attribute_get_type_and_val(&val, &attribute_type, &attribute_size, &value);

    /* Update matter */
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;
    if (emberAfContainsServer(endpoint_id, cluster_id)) {
        status = emberAfWriteServerAttribute(endpoint_id, cluster_id, attribute_id, value, attribute_type);
        if (status != EMBER_ZCL_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Error updating attribute to matter");
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

Status MatterPreAttributeChangeCallback(const chip::app::ConcreteAttributePath &path, uint8_t mask, uint8_t type,
                                        uint16_t size, uint8_t *value)
{
    int endpoint_id = path.mEndpointId;
    int cluster_id = path.mClusterId;
    int attribute_id = path.mAttributeId;
    esp_matter_attr_val_t val = esp_matter_attribute_get_attr_val(type, size, value);

    /* Print */
    esp_matter_attribute_val_print(endpoint_id, cluster_id, attribute_id, val);

    /* Callback to application */
    if (attribute_callback) {
        esp_err_t err = attribute_callback(ESP_MATTER_CALLBACK_TYPE_PRE_ATTRIBUTE, endpoint_id, cluster_id,
                                           attribute_id, val, attribute_callback_priv_data);
        if (err != ESP_OK) {
            return Status::Failure;
        }
    }
    return Status::Success;
}

void MatterPostAttributeChangeCallback(const chip::app::ConcreteAttributePath &path, uint8_t mask, uint8_t type,
                                       uint16_t size, uint8_t *value)
{
    int endpoint_id = path.mEndpointId;
    int cluster_id = path.mClusterId;
    int attribute_id = path.mAttributeId;
    esp_matter_attr_val_t val = esp_matter_attribute_get_attr_val(type, size, value);

    /* Callback to application */
    if (attribute_callback) {
        attribute_callback(ESP_MATTER_CALLBACK_TYPE_POST_ATTRIBUTE, endpoint_id, cluster_id, attribute_id, val,
                           attribute_callback_priv_data);
    }
}

EmberAfStatus emberAfExternalAttributeReadCallback(EndpointId endpoint_id, ClusterId cluster_id,
                                                   EmberAfAttributeMetadata *matter_attribute, uint8_t *buffer,
                                                   uint16_t max_read_length)
{
    /* Get value */
    int attribute_id = matter_attribute->attributeId;
    esp_matter_node_t *node = esp_matter_node_get();
    if (!node) {
        return EMBER_ZCL_STATUS_FAILURE;
    }
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_get(node, endpoint_id);
    esp_matter_cluster_t *cluster = esp_matter_cluster_get(endpoint, cluster_id);
    esp_matter_attribute_t *attribute = esp_matter_attribute_get(cluster, attribute_id);
    esp_matter_attr_val_t val = esp_matter_attribute_get_val(attribute);

    /* Convert */
    EmberAfAttributeType attribute_type = 0;
    uint16_t attribute_size = 0;
    uint8_t *value = NULL;
    esp_matter_attribute_get_type_and_val(&val, &attribute_type, &attribute_size, &value);

    if (attribute_size > max_read_length) {
        return EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
    }

    /* Assign value */
    *buffer = *value;
    return EMBER_ZCL_STATUS_SUCCESS;
}

EmberAfStatus emberAfExternalAttributeWriteCallback(EndpointId endpoint_id, ClusterId cluster_id,
                                                    EmberAfAttributeMetadata *matter_attribute, uint8_t *buffer)
{
    /* Get value */
    int attribute_id = matter_attribute->attributeId;
    esp_matter_node_t *node = esp_matter_node_get();
    if (!node) {
        return EMBER_ZCL_STATUS_FAILURE;
    }
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_get(node, endpoint_id);
    esp_matter_cluster_t *cluster = esp_matter_cluster_get(endpoint, cluster_id);
    esp_matter_attribute_t *attribute = esp_matter_attribute_get(cluster, attribute_id);
    esp_matter_attr_val_t val = esp_matter_attribute_get_val(attribute);

    /* Convert */
    EmberAfAttributeType attribute_type = 0;
    uint16_t attribute_size = 0;
    uint8_t *value = NULL;
    esp_matter_attribute_get_type_and_val(&val, &attribute_type, &attribute_size, &value);

    /* Convert back */
    value = buffer;
    val = esp_matter_attribute_get_attr_val(attribute_type, attribute_size, value);

    /* Update value */
    esp_matter_attribute_set_val(attribute, val);
    return EMBER_ZCL_STATUS_SUCCESS;
}
