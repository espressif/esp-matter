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

#include <cstdint>
#include <cstring>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_priv.h>
#include <esp_matter_data_model_provider.h>
#include <esp_matter_attr_data_buffer.h>
#include <esp_matter_mem.h>
#include <esp_matter_nvs.h>
#include <esp_random.h>
#include <nvs_flash.h>
#include <singly_linked_list.h>

#include <access/SubjectDescriptor.h>
#include <app/clusters/identify-server/identify-server.h>
#include <app/data-model-provider/MetadataTypes.h>
#include <app/data-model-provider/Provider.h>
#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#include <app/util/endpoint-config-api.h>
#include <credentials/GroupDataProviderImpl.h>
#include <lib/core/DataModelTypes.h>
#include <lib/core/TLV.h>
#include <lib/support/ScopedBuffer.h>

#define ESP_MATTER_MAX_DEVICE_TYPE_COUNT CONFIG_ESP_MATTER_MAX_DEVICE_TYPE_COUNT
#define ESP_MATTER_MAX_SEMANTIC_TAG_COUNT 3

static const char *TAG = "data_model";

using chip::CommandId;
using chip::DataVersion;
using chip::EventId;
using chip::kInvalidAttributeId;
using chip::kInvalidClusterId;
using chip::kInvalidCommandId;
using chip::kInvalidEndpointId;
using chip::app::DataModel::EndpointCompositionPattern;

namespace esp_matter {

struct _attribute_base_t {
    uint16_t flags; // This struct is for attributes managed internally.
    esp_matter_val_type_t attribute_val_type;
    uint32_t attribute_id;
    struct _attribute_base_t *next;
};

struct _attribute_t : public _attribute_base_t {
    esp_matter_val_t attribute_val;
    esp_matter_attr_bounds_t *bounds;
    uint16_t endpoint_id;
    uint32_t cluster_id;
    attribute::callback_t override_callback;
};

typedef struct _command {
    uint32_t command_id;
    uint16_t flags;
    command::callback_t callback;
    command::callback_t user_callback;
    struct _command *next;
} _command_t;

typedef struct _event {
    uint32_t event_id;
    struct _event *next;
} _event_t;

typedef struct _cluster {
    uint32_t cluster_id;
    uint16_t endpoint_id;
    uint8_t flags;
    const cluster::function_generic_t *functions;
    cluster::plugin_server_init_callback_t plugin_server_init_callback;
    cluster::delegate_init_callback_t delegate_init_callback;
    void *delegate_pointer;
    cluster::add_bounds_callback_t add_bounds_callback;
    cluster::initialization_callback_t init_callback;
    cluster::shutdown_callback_t shutdown_callback;
    chip::DataVersion data_version;
    _attribute_base_t *attribute_list; /* If attribute is managed internally, the actual pointer type is
                                     _internal_attribute_t. When operating attribute_list, do check the flags first! */
    _command_t *command_list;
    _event_t *event_list;
    struct _cluster *next;
} _cluster_t;

typedef struct device_type {
    uint8_t version;
    uint32_t id;
} device_type_t;

typedef struct _endpoint {
    uint16_t endpoint_id;
    bool enabled;
    uint8_t device_type_count;
    device_type_t device_types[ESP_MATTER_MAX_DEVICE_TYPE_COUNT];
    uint16_t flags;
    uint16_t parent_endpoint_id;
    void *priv_data;
    Identify *identify;
    EndpointCompositionPattern composition_pattern;
    uint8_t semantic_tag_count;
    chip::app::DataModel::Provider::SemanticTag semantic_tags[ESP_MATTER_MAX_SEMANTIC_TAG_COUNT];
    _cluster_t *cluster_list;
    struct _endpoint *next;
} _endpoint_t;

typedef struct _node {
    _endpoint_t *endpoint_list;
    uint16_t min_unused_endpoint_id;
} _node_t;

namespace {
// Treat 0xFFFF'FFFF as wildcard cluster
inline bool is_wildcard_cluster_id(uint32_t cluster_id)
{
    return cluster_id == chip::kInvalidClusterId;
}

// Treat 0xFFFF as wildcard endpoint
inline bool is_wildcard_endpoint_id(uint16_t endpoint_id)
{
    return endpoint_id == chip::kInvalidEndpointId;
}

// We allocate this large scoped buffer to read the attribute as the TLV data
// This is enough to store the TLV encoded buffer for a primitive type of data
const uint32_t k_max_tlv_size_to_read_attribute_value = 512;
const uint32_t k_max_tlv_size_to_write_attribute_value = 512;

} // namespace

namespace node {

static _node_t *node = NULL;

// If Matter server or ESP-Matter data model is not enabled. we will never use minimum unused endpoint id.
esp_err_t store_min_unused_endpoint_id()
{
    VerifyOrReturnError((node && esp_matter::is_started()), ESP_ERR_INVALID_STATE,
                        ESP_LOGE(TAG, "Node does not exist or esp_matter does not start"));
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, ESP_MATTER_KVS_NAMESPACE, NVS_READWRITE, &handle);

    VerifyOrReturnError(err == ESP_OK, err, ESP_LOGE(TAG, "Failed to open the node nvs_namespace"));
    err = nvs_set_u16(handle, "min_uu_ep_id", node->min_unused_endpoint_id);
    nvs_commit(handle);
    nvs_close(handle);
    return err;
}

esp_err_t read_min_unused_endpoint_id()
{
    VerifyOrReturnError((node && esp_matter::is_started()), ESP_ERR_INVALID_STATE,
                        ESP_LOGE(TAG, "Node does not exist or esp_matter does not start"));

    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, ESP_MATTER_KVS_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_OK) {
        err = nvs_get_u16(handle, "min_uu_ep_id", &node->min_unused_endpoint_id);
        nvs_close(handle);
    }

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Cannot find minimum unused endpoint_id, try to find in the previous namespace");
        // Try to read the minimum unused endpoint_id from the previous node namespace.
        err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, "node", NVS_READONLY, &handle);
        VerifyOrReturnError(err == ESP_OK, err, ESP_LOGI(TAG, "Failed to open node namespace"));
        err = nvs_get_u16(handle, "min_uu_ep_id", &node->min_unused_endpoint_id);
        nvs_close(handle);
        if (err == ESP_OK) {
            // If the minimum unused endpoint_id is got, we will erase it from the previous namespace
            // and store it to the new namespace.
            if (nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, "node", NVS_READWRITE, &handle) == ESP_OK) {
                if (nvs_erase_key(handle, "min_uu_ep_id") != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to erase minimum unused endpoint_id");
                } else {
                    nvs_commit(handle);
                }
                nvs_close(handle);
            }
            return store_min_unused_endpoint_id();
        }
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get minimum unused endpoint_id in the %s nvs_namespace", ESP_MATTER_KVS_NAMESPACE);
    }
    return err;
}

} // namespace node

namespace endpoint {

esp_err_t disable(endpoint_t *endpoint)
{
    VerifyOrReturnError(endpoint, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    current_endpoint->enabled = false;
    return ESP_OK;
}

static esp_err_t init_identification(endpoint_t *endpoint)
{
    VerifyOrReturnError(endpoint, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    cluster_t *identify_cluster = cluster::get(endpoint, chip::app::Clusters::Identify::Id);
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    /* Init identify if exists and not initialized */
    if (identify_cluster != nullptr && current_endpoint->identify == NULL) {
        _attribute_t *identify_type_attr = (_attribute_t *)attribute::get(
            identify_cluster, chip::app::Clusters::Identify::Attributes::IdentifyType::Id);
        if (identify_type_attr) {
            return identification::init(current_endpoint->endpoint_id, identify_type_attr->attribute_val.u8);
        } else {
            ESP_LOGE(TAG, "Can't get IdentifyType attribute in Identify cluster");
            return ESP_ERR_INVALID_STATE;
        }
    }
    // Return ESP_OK when identify cluster is not on the endpoint.
    return ESP_OK;
}

esp_err_t enable(endpoint_t *endpoint)
{
    VerifyOrReturnError(endpoint, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    current_endpoint->enabled = true;
    init_identification(endpoint);
    esp_matter::cluster::delegate_init_callback_common(endpoint);
    return ESP_OK;
}

esp_err_t enable_all()
{
    node_t *node = node::get();
    /* Not returning error, since the node will not be initialized for application using the data model from zap */
    VerifyOrReturnError(node, ESP_OK);

    endpoint_t *endpoint = get_first(node);
    while (endpoint) {
        enable(endpoint);
        endpoint = get_next(endpoint);
    }
    return ESP_OK;
}

bool is_attribute_enabled(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id)
{
    attribute_t *attr = attribute::get(endpoint_id, cluster_id, attribute_id);
    return (attr != nullptr);
}

bool is_command_enabled(uint16_t endpoint_id, uint32_t cluster_id, uint32_t command_id)
{
    command_t *cmd = command::get(endpoint_id, cluster_id, command_id);
    return (cmd != nullptr);
}

bool is_enabled(endpoint_t *endpoint)
{
    VerifyOrReturnError(endpoint, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return current_endpoint->enabled;
}
} /* endpoint */

namespace attribute {

static callback_t attribute_callback = NULL;
esp_err_t set_callback(callback_t callback)
{
    attribute_callback = callback;
    return ESP_OK;
}

static esp_err_t execute_callback(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                  uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    if (attribute_callback) {
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
        void *priv_data = endpoint::get_priv_data(endpoint_id);
#else
        void *priv_data = nullptr;
#endif
        return attribute_callback(type, endpoint_id, cluster_id, attribute_id, val, priv_data);
    }
    return ESP_OK;
}

/**
 * Check whether the attribute value is in the range of attribute bounds
 *
 * @param[in] val Attribute value.
 * @param[in] bounds Attribute bounds.
 *
 * @return 0 if val is in the range of attribute bounds.
 * @return 1 if val is more than bounds.max
 * @return -1 if val is less than bounds.min
 * @return -2 if val type is wrong
 */
static int compare_attr_val_with_bounds(esp_matter_attr_val_t val, esp_matter_attr_bounds_t bounds)
{
    switch (val.type) {
    case ESP_MATTER_VAL_TYPE_UINT8:
    case ESP_MATTER_VAL_TYPE_ENUM8:
    case ESP_MATTER_VAL_TYPE_BITMAP8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8: {
        using Traits = chip::app::NumericAttributeTraits<uint8_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.u8)) {
            return 0;
        }
        if (val.val.u8 < bounds.min.val.u8) {
            return -1;
        } else if (val.val.u8 > bounds.max.val.u8) {
            return 1;
        } else {
            return 0;
        }
        break;
    }
    case ESP_MATTER_VAL_TYPE_UINT16:
    case ESP_MATTER_VAL_TYPE_ENUM16:
    case ESP_MATTER_VAL_TYPE_BITMAP16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16: {
        using Traits = chip::app::NumericAttributeTraits<uint16_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.u16)) {
            return 0;
        }
        if (val.val.u16 < bounds.min.val.u16) {
            return -1;
        } else if (val.val.u16 > bounds.max.val.u16) {
            return 1;
        } else {
            return 0;
        }
        break;
    }
    case ESP_MATTER_VAL_TYPE_UINT32:
    case ESP_MATTER_VAL_TYPE_BITMAP32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32: {
        using Traits = chip::app::NumericAttributeTraits<uint32_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.u32)) {
            return 0;
        }
        if (val.val.u32 < bounds.min.val.u32) {
            return -1;
        } else if (val.val.u32 > bounds.max.val.u32) {
            return 1;
        } else {
            return 0;
        }
        break;
    }
    case ESP_MATTER_VAL_TYPE_UINT64:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT64: {
        using Traits = chip::app::NumericAttributeTraits<uint64_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.u64)) {
            return 0;
        }
        if (val.val.u64 < bounds.min.val.u64) {
            return -1;
        } else if (val.val.u64 > bounds.max.val.u64) {
            return 1;
        } else {
            return 0;
        }
        break;
    }
    case ESP_MATTER_VAL_TYPE_INT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT8: {
        using Traits = chip::app::NumericAttributeTraits<int8_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.i8)) {
            return 0;
        }
        if (val.val.i8 < bounds.min.val.i8) {
            return -1;
        } else if (val.val.i8 > bounds.max.val.i8) {
            return 1;
        } else {
            return 0;
        }
        break;
    }
    case ESP_MATTER_VAL_TYPE_INT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT16: {
        using Traits = chip::app::NumericAttributeTraits<int16_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.i16)) {
            return 0;
        }
        if (val.val.i16 < bounds.min.val.i16) {
            return -1;
        } else if (val.val.i16 > bounds.max.val.i16) {
            return 1;
        } else {
            return 0;
        }
        break;
    }
    case ESP_MATTER_VAL_TYPE_INT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT32: {
        using Traits = chip::app::NumericAttributeTraits<int32_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.i32)) {
            return 0;
        }
        if (val.val.i32 < bounds.min.val.i32) {
            return -1;
        } else if (val.val.i32 > bounds.max.val.i32) {
            return 1;
        } else {
            return 0;
        }
        break;
    }
    case ESP_MATTER_VAL_TYPE_INT64:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT64: {
        using Traits = chip::app::NumericAttributeTraits<int64_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.i64)) {
            return 0;
        }
        if (val.val.i64 < bounds.min.val.i64) {
            return -1;
        } else if (val.val.i64 > bounds.max.val.i64) {
            return 1;
        } else {
            return 0;
        }
        break;
    }
    case ESP_MATTER_VAL_TYPE_FLOAT:
    case ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT: {
        using Traits = chip::app::NumericAttributeTraits<float>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.f)) {
            return 0;
        }
        if (val.val.f < bounds.min.val.f) {
            return -1;
        } else if (val.val.f > bounds.max.val.f) {
            return 1;
        } else {
            return 0;
        }
        break;
    }
    default:
        ESP_LOGE(TAG, "Failed to compare_attr_val_with_bounds as the attribute value type is wrong");
        break;
    }
    return -2;
}

static esp_err_t bound_attribute_val(attribute_t *attribute)
{
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    esp_matter_attr_val_t temp_val;
    temp_val.type = current_attribute->attribute_val_type;
    temp_val.val = current_attribute->attribute_val;
    int compare_result = compare_attr_val_with_bounds(temp_val, *current_attribute->bounds);
    if (compare_result == 1) {
        current_attribute->attribute_val = current_attribute->bounds->max.val;
    } else if (compare_result == -1) {
        current_attribute->attribute_val = current_attribute->bounds->min.val;
    } else if (compare_result != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

attribute_t *create(cluster_t *cluster, uint32_t attribute_id, uint16_t flags, esp_matter_attr_val_t val,
                    uint16_t max_val_size)
{
    /* Find */
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    attribute_t *existing_attribute = get(cluster, attribute_id);
    if (existing_attribute) {
        ESP_LOGW(TAG, "Attribute 0x%08" PRIX32 " on cluster 0x%08" PRIX32 " already exists. Not creating again.",
                 attribute_id, cluster::get_id(cluster));
        return existing_attribute;
    }
    _attribute_t *attribute = NULL;

    if (flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY) {
        /* Create */
        attribute = (_attribute_t *)esp_matter_mem_calloc(1, sizeof(_attribute_base_t));
        if (!attribute) {
            return nullptr;
        }

        /* Set */
        attribute->flags = flags;
        attribute->attribute_val_type = val.type;
        attribute->attribute_id = attribute_id;
    } else {
        attribute = (_attribute_t *)esp_matter_mem_calloc(1, sizeof(_attribute_t));
        if (!attribute) {
            return nullptr;
        }

        /* Set */
        attribute->flags = flags;
        attribute->attribute_id = attribute_id;
        attribute->override_callback = nullptr;
        attribute->cluster_id = current_cluster->cluster_id;
        attribute->endpoint_id = current_cluster->endpoint_id;
        attribute->attribute_val_type = val.type;
        if (val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING || val.type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
            val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING || val.type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING) {
            attribute->attribute_val.a.max = max_val_size;
            val.val.a.max = max_val_size;
        }
        bool attribute_updated = false;
        if (flags & ATTRIBUTE_FLAG_NONVOLATILE) {
            // read from the NVS and store in the attribute's storage
            esp_matter_attr_val_t temp_val;
            temp_val.type = attribute->attribute_val_type;
            esp_err_t err =
                get_val_from_nvs(attribute->endpoint_id, attribute->cluster_id, attribute_id, temp_val);
            if (err == ESP_OK) {
                attribute->attribute_val = temp_val.val;
                attribute_updated = true;
            }
        }
        if (!attribute_updated) {
            set_val_internal((attribute_t *)attribute, &val, false);
        }
    }

    /* Add */
    SinglyLinkedList<_attribute_base_t>::append(&current_cluster->attribute_list, attribute);
    return (attribute_t *)attribute;
}

static esp_err_t destroy(attribute_t *attribute)
{
    VerifyOrReturnError(attribute, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Attribute cannot be NULL"));
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    if (current_attribute->flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY) {
        // For attribute managed internally, free as the _attribute_base_t pointer.
        esp_matter_mem_free((_attribute_base_t *)attribute);
        return ESP_OK;
    }

    /* Delete val here, if required */
    if (current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_ARRAY) {
        /* Free buf */
        esp_matter_mem_free(current_attribute->attribute_val.a.b);
    }

    /* Erase the persistent data */
    if (attribute::get_flags(attribute) & ATTRIBUTE_FLAG_NONVOLATILE) {
        erase_val_in_nvs(current_attribute->endpoint_id, current_attribute->cluster_id,
                         current_attribute->attribute_id);
    }

    /* Free */
    esp_matter_mem_free(current_attribute);
    return ESP_OK;
}

attribute_t *get(cluster_t *cluster, uint32_t attribute_id)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    _attribute_base_t *current_attribute = current_cluster->attribute_list;
    while (current_attribute) {
        if (current_attribute->attribute_id == attribute_id) {
            break;
        }
        current_attribute = current_attribute->next;
    }

    return (attribute_t *)current_attribute;
}

attribute_t *get(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id)
{
    cluster_t *cluster = cluster::get(endpoint_id, cluster_id);
    return get(cluster, attribute_id);
}

attribute_t *get_first(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return (attribute_t *)current_cluster->attribute_list;
}

attribute_t *get_next(attribute_t *attribute)
{
    VerifyOrReturnValue(attribute, NULL, ESP_LOGE(TAG, "Attribute cannot be NULL"));
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    return (attribute_t *)current_attribute->next;
}

uint32_t get_id(attribute_t *attribute)
{
    VerifyOrReturnValue(attribute, kInvalidAttributeId, ESP_LOGE(TAG, "Attribute cannot be NULL"));
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    return current_attribute->attribute_id;
}

constexpr uint16_t k_deferred_attribute_persistence_time_ms = CONFIG_ESP_MATTER_DEFERRED_ATTR_PERSISTENCE_TIME_MS;

static void deferred_attribute_write(chip::System::Layer *layer, void *attribute_ptr)
{
    _attribute_t *current_attribute = (_attribute_t *)attribute_ptr;
    ESP_LOGI(TAG, "Store the deferred attribute 0x%" PRIx32 " of cluster 0x%" PRIX32 " on endpoint 0x%" PRIx16,
             current_attribute->attribute_id, current_attribute->cluster_id, current_attribute->endpoint_id);
    store_val_in_nvs(current_attribute->endpoint_id, current_attribute->cluster_id, current_attribute->attribute_id,
                     {current_attribute->attribute_val_type, current_attribute->attribute_val});
}

esp_err_t set_val_internal(attribute_t *attribute, esp_matter_attr_val_t *val, bool call_callbacks)
{
    VerifyOrReturnError(attribute && val, ESP_ERR_INVALID_ARG);
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    ESP_RETURN_ON_FALSE(!(current_attribute->flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY), ESP_ERR_NOT_SUPPORTED, TAG,
                        "Attribute is not managed by esp matter data model");

    // As we know that this is esp-matter managed attribute, we can safely log the path
    ESP_LOGD(TAG, "setting attribute value for: 0x%x:0x%" PRIx32 ":0x%" PRIx32, current_attribute->endpoint_id,
             current_attribute->cluster_id, current_attribute->attribute_id);

    VerifyOrReturnError(current_attribute->attribute_val_type == val->type, ESP_ERR_INVALID_ARG,
                        ESP_LOGE(TAG, "Different value type : Expected Type : %u Attempted Type: %u",
                            current_attribute->attribute_val_type, val->type));

    if ((current_attribute->flags & ATTRIBUTE_FLAG_MIN_MAX) && current_attribute->bounds) {
        if (compare_attr_val_with_bounds(*val, *current_attribute->bounds) != 0) {
            return ESP_ERR_INVALID_ARG;
        }
    }
    esp_matter_attr_val_t temp_val;
    temp_val.type = current_attribute->attribute_val_type;
    temp_val.val = current_attribute->attribute_val;
    if (val_compare(val, &temp_val)) {
        // If the value is not changed, return ESP_ERR_NOT_FINISHED directly.
        return ESP_ERR_NOT_FINISHED;
    }
    /* Callback to application */
    if (call_callbacks) {
        ESP_RETURN_ON_ERROR(execute_callback(attribute::PRE_UPDATE, current_attribute->endpoint_id,
                                         current_attribute->cluster_id, current_attribute->attribute_id, val),
                            TAG, "Failed to execute pre update callback");
    }
    // TODO: call pre attribute change function is the cluster has the flag
    if (val->type == ESP_MATTER_VAL_TYPE_CHAR_STRING || val->type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        val->type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING || val->type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING) {
        uint16_t null_len =
            (val->type == ESP_MATTER_VAL_TYPE_CHAR_STRING || val->type == ESP_MATTER_VAL_TYPE_OCTET_STRING)
            ? UINT8_MAX
            : UINT16_MAX;
        if (val->val.a.s > 0) {
            uint8_t *new_buf = nullptr;
            if (val->val.a.s != null_len) {
                if (val->val.a.s > current_attribute->attribute_val.a.max) {
                    return ESP_ERR_NO_MEM;
                }
                /* Free old buf */
                esp_matter_mem_free(current_attribute->attribute_val.a.b);
                current_attribute->attribute_val.a.b = nullptr;

                bool null_reserve =
                    val->type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING || val->type == ESP_MATTER_VAL_TYPE_CHAR_STRING;
                /* Alloc new buf */
                new_buf = (uint8_t *)esp_matter_mem_calloc(1, val->val.a.s + (null_reserve ? 1 : 0));
                VerifyOrReturnError(new_buf, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "Could not allocate new buffer"));
                /* Copy to new buf and assign */
                memcpy(new_buf, val->val.a.b, val->val.a.s);
            }
            current_attribute->attribute_val.a.b = new_buf;
            current_attribute->attribute_val.a.s = val->val.a.s;
            current_attribute->attribute_val.a.t = val->val.a.t;
        } else {
            ESP_LOGD(TAG, "Set val called with string with size 0");
        }
    } else {
        memcpy((void *)&current_attribute->attribute_val, (void *)&val->val, sizeof(esp_matter_val_t));
    }
    /* Callback to application */
    if (call_callbacks) {
        execute_callback(attribute::POST_UPDATE, current_attribute->endpoint_id, current_attribute->cluster_id,
                         current_attribute->attribute_id, val);

        cluster_t *cluster = cluster::get(current_attribute->endpoint_id, current_attribute->cluster_id);
        cluster::function_attribute_change_t attr_change_function =
            (cluster::function_attribute_change_t)cluster::get_function(cluster, CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION);

        if (attr_change_function) {
            attr_change_function(chip::app::ConcreteAttributePath(
                current_attribute->endpoint_id, current_attribute->cluster_id, current_attribute->attribute_id));
        }
    }
    if (current_attribute->flags & ATTRIBUTE_FLAG_NONVOLATILE) {
        if (current_attribute->flags & ATTRIBUTE_FLAG_DEFERRED) {
            if (!chip::DeviceLayer::SystemLayer().IsTimerActive(deferred_attribute_write, current_attribute)) {
                auto &system_layer = chip::DeviceLayer::SystemLayer();
                system_layer.StartTimer(chip::System::Clock::Milliseconds16(k_deferred_attribute_persistence_time_ms),
                                        deferred_attribute_write, current_attribute);
            }
        } else {
            esp_matter_attr_val_t temp_val;
            temp_val.type = current_attribute->attribute_val_type;
            temp_val.val = current_attribute->attribute_val;
            store_val_in_nvs(current_attribute->endpoint_id, current_attribute->cluster_id,
                             current_attribute->attribute_id, temp_val);
        }
    }

    return ESP_OK;
}

// parsing the following structure into esp_matter_attr_val_t
// Array
//   Structure
//     Structure(ContextTag:1)
//       Structure(ContextTag:0)
//       Structure(ContextTag:1)
//       Structure(ContextTag:2) -> Actual data value that we are interested in
//     End of Structure
//   End of Structure
// End of Array

// this function drills down on the reader and moves the reader to the point where the actual data value is present
static CHIP_ERROR move_tlv_reader_to_attribute_data(chip::TLV::TLVReader &reader)
{
    // Navigate: Array (Anonymous)
    ReturnErrorOnFailure(reader.Next(chip::TLV::kTLVType_Array, chip::TLV::AnonymousTag()));

    chip::TLV::TLVType outerContainer;
    ReturnErrorOnFailure(reader.EnterContainer(outerContainer));

    // Navigate: Structure (Anonymous) - AttributeReportIB
    ReturnErrorOnFailure(reader.Next(chip::TLV::kTLVType_Structure, chip::TLV::AnonymousTag()));

    chip::TLV::TLVType reportContainer;
    ReturnErrorOnFailure(reader.EnterContainer(reportContainer));

    // Navigate: Structure(ContextTag:1)
    ReturnErrorOnFailure(reader.Next(chip::TLV::kTLVType_Structure, chip::TLV::ContextTag(1)));

    chip::TLV::TLVType dataContainer;
    ReturnErrorOnFailure(reader.EnterContainer(dataContainer));

    // we know that there are 3 elements so avoid having while(true), just trying to handle infinite loop scenario
    for (uint8_t i = 0; i < 3; i++) {
        ReturnErrorOnFailure(reader.Next());

        if (reader.GetTag() == chip::TLV::ContextTag(2)) {
            ESP_LOGD(TAG, "Found context tag 2");
            break;
        }
    }

    return CHIP_NO_ERROR;
}

static esp_err_t get_val_from_tlv_data(const uint8_t *tlv_data, uint32_t tlv_len, esp_matter_attr_val_t *val)
{
    chip::TLV::TLVReader reader;
    reader.Init(tlv_data, tlv_len);

    VerifyOrReturnError(move_tlv_reader_to_attribute_data(reader) == CHIP_NO_ERROR, ESP_FAIL,
                        ESP_LOGE(TAG, "Failed to get TLV reader for attribute data"));

    esp_matter::data_model::attribute_data_decode_buffer data_buffer(val->type);
    VerifyOrReturnError(data_buffer.Decode(reader) == CHIP_NO_ERROR, ESP_FAIL, ESP_LOGE(TAG, "Failed to decode TLV data"));

    *val = data_buffer.get_attr_val();

    // attribute_data_decode_buffer's destructor frees the buffer so we need to copy the buffer to a new buffer
    // and pass on to the user. This is only required for string and octet string types

    bool is_type_string = (val->type == ESP_MATTER_VAL_TYPE_CHAR_STRING
                            || val->type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING);
    bool is_type_octet_string = (val->type == ESP_MATTER_VAL_TYPE_OCTET_STRING
                                    || val->type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING);

    if (is_type_string || is_type_octet_string) {
        // for empty strings, we need to copy at least null terminator
        uint32_t bytes_to_copy = (is_type_string ? val->val.a.s + 1 : val->val.a.s);

        if (val->val.a.b && bytes_to_copy > 0) {
            uint8_t *new_buf = (uint8_t *)esp_matter_mem_calloc(sizeof(uint8_t), bytes_to_copy);
            VerifyOrReturnError(new_buf != nullptr, ESP_ERR_NO_MEM);
            memcpy(new_buf, val->val.a.b, bytes_to_copy);
            val->val.a.b = new_buf; // new buffer is now owned by the caller
        }
        // else it's of type nullable string, so we don't need to do anything
    }

    return ESP_OK;
}

// Helper function to find the cluster_id and attribute_id for internally managed attributes
static esp_err_t find_cluster_and_endpoint_id_for_internally_managed_attribute(const _attribute_base_t *attribute_base,
                                                                                uint16_t &out_endpoint_id,
                                                                                uint32_t &out_cluster_id)
{
    VerifyOrReturnError(attribute_base, ESP_ERR_INVALID_ARG);
    _node_t *node = (_node_t *)node::get();
    VerifyOrReturnError(node, ESP_ERR_INVALID_STATE);

    // This is a linear search and it's not efficient, but we don't have a better way to do it right now.
    for (_endpoint_t *endpoint = node->endpoint_list; endpoint != nullptr; endpoint = endpoint->next) {
        for (_cluster_t *cluster = endpoint->cluster_list; cluster != nullptr; cluster = cluster->next) {
            for (_attribute_base_t *attr = cluster->attribute_list; attr != nullptr; attr = attr->next) {
                if (attr == attribute_base) {
                    out_endpoint_id = endpoint->endpoint_id;
                    out_cluster_id = cluster::get_id((cluster_t *)cluster);
                    return ESP_OK;
                }
            }
        }
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t get_val(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    VerifyOrReturnError(val, ESP_ERR_INVALID_ARG);
    esp_matter_val_type_t val_type = get_val_type(endpoint_id, cluster_id, attribute_id);
    VerifyOrReturnError(val_type != ESP_MATTER_VAL_TYPE_INVALID, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(val_type != ESP_MATTER_VAL_TYPE_ARRAY, ESP_ERR_NOT_SUPPORTED);

    chip::Platform::ScopedMemoryBuffer<uint8_t> scoped_buf;
    scoped_buf.Calloc(k_max_tlv_size_to_read_attribute_value);
    if (scoped_buf.IsNull()) {
        ESP_LOGE(TAG, "Failed to allocate memory for scoped buffer");
        return ESP_ERR_NO_MEM;
    }

    chip::TLV::TLVWriter writer;
    writer.Init(scoped_buf.Get(), k_max_tlv_size_to_read_attribute_value);

    chip::app::AttributeReportIBs::Builder reportBuilder = chip::app::AttributeReportIBs::Builder();
    reportBuilder.Init(&writer);

    chip::Access::SubjectDescriptor subjectDescriptor;
    auto concrete_path = chip::app::ConcreteAttributePath(endpoint_id, cluster_id, attribute_id);
    chip::DataVersion version = chip::DataVersion();
    chip::app::AttributeValueEncoder encoder(reportBuilder, subjectDescriptor, concrete_path, version);

    chip::app::DataModel::ReadAttributeRequest request;
    request.path = concrete_path;
    request.readFlags = chip::app::DataModel::ReadFlags::kFabricFiltered;

    chip::app::DataModel::ActionReturnStatus action_return_status = esp_matter::data_model::provider::get_instance().ReadAttribute(request, encoder);

    if (action_return_status.IsError()) {
        chip::app::DataModel::ActionReturnStatus::StringStorage storage;
        ESP_LOGE(TAG, "ReadAttribute failed with error:%s", action_return_status.c_str(storage));
        return ESP_FAIL;
    }

    ESP_LOG_BUFFER_HEX_LEVEL("TLV data", scoped_buf.Get(), writer.GetLengthWritten(), ESP_LOG_DEBUG);

    val->type = val_type;
    return get_val_from_tlv_data(scoped_buf.Get(), writer.GetLengthWritten(), val);
}

static esp_err_t get_path_from_attribute_handle(const _attribute_t *attribute, uint16_t &endpoint_id,
                                                    uint32_t &cluster_id, uint32_t &attribute_id)
{
    attribute_id = attribute->attribute_id;
    if (attribute->flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY) {
        // for connectedhomeip managed attributes, we need to find the cluster and endpoint id
        return find_cluster_and_endpoint_id_for_internally_managed_attribute(attribute, endpoint_id, cluster_id);
    }

    // in case of esp-matter managed attributes, we can directly use the endpoint and cluster id
    endpoint_id = attribute->endpoint_id;
    cluster_id = attribute->cluster_id;
    return ESP_OK;
}

esp_err_t get_val(attribute_t *attribute, esp_matter_attr_val_t *val)
{
    VerifyOrReturnError(attribute && val, ESP_ERR_INVALID_ARG);
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    uint16_t endpoint_id = chip::kInvalidEndpointId;
    uint32_t cluster_id = chip::kInvalidClusterId;
    uint32_t attribute_id = chip::kInvalidAttributeId;

    esp_err_t err = get_path_from_attribute_handle(current_attribute, endpoint_id, cluster_id, attribute_id);
    VerifyOrReturnError(err == ESP_OK, err);

    return get_val(endpoint_id, cluster_id, attribute_id, val);
}

esp_err_t get_val_internal(attribute_t *attribute, esp_matter_attr_val_t *val)
{
    VerifyOrReturnError(attribute && val, ESP_ERR_INVALID_ARG);
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    ESP_RETURN_ON_FALSE(!(current_attribute->flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY), ESP_ERR_NOT_SUPPORTED, TAG,
                        "Attribute is not managed by esp matter data model");
    memcpy((void *)&val->type, (void *)&current_attribute->attribute_val_type, sizeof(esp_matter_val_type_t));
    memcpy((void *)&val->val, (void *)&current_attribute->attribute_val, sizeof(esp_matter_val_t));
    return ESP_OK;
}

esp_matter_val_type_t get_val_type(attribute_t *attribute)
{
    VerifyOrReturnValue(attribute, ESP_MATTER_VAL_TYPE_INVALID, ESP_LOGE(TAG, "Attribute cannot be NULL"));
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    return current_attribute->attribute_val_type;
}

esp_matter_val_type_t get_val_type(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id)
{
    return get_val_type(get(endpoint_id, cluster_id, attribute_id));
}

// Helper function: Set value via WriteAttribute with kInternal flag (for SCI/AAI writable attributes)
static esp_err_t set_val_via_write_attribute(uint16_t endpoint_id, uint32_t cluster_id,
                                              uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    chip::Platform::ScopedMemoryBuffer<uint8_t> tlv_buffer;
    tlv_buffer.Calloc(k_max_tlv_size_to_write_attribute_value);
    VerifyOrReturnError(!tlv_buffer.IsNull(), ESP_ERR_NO_MEM);

    chip::TLV::TLVWriter writer;
    writer.Init(tlv_buffer.Get(), k_max_tlv_size_to_write_attribute_value);

    // Encoding is within a structure:
    //   - BEGIN_STRUCT
    //     - 1: <encoded value>
    //   - END_STRUCT
    chip::TLV::TLVType outerContainer;
    VerifyOrReturnError(writer.StartContainer(chip::TLV::AnonymousTag(), chip::TLV::kTLVType_Structure, outerContainer) == CHIP_NO_ERROR, ESP_FAIL);
    data_model::attribute_data_encode_buffer encode_buffer(*val);
    VerifyOrReturnError(encode_buffer.Encode(writer, chip::TLV::ContextTag(1)) == CHIP_NO_ERROR, ESP_FAIL);
    VerifyOrReturnError(writer.EndContainer(outerContainer) == CHIP_NO_ERROR, ESP_FAIL);
    VerifyOrReturnError(writer.Finalize() == CHIP_NO_ERROR, ESP_FAIL);

    // position the reader inside the buffer, on the encoded value
    chip::TLV::TLVReader reader;
    reader.Init(tlv_buffer.Get(), writer.GetLengthWritten());
    VerifyOrReturnError(reader.Next() == CHIP_NO_ERROR, ESP_FAIL);
    VerifyOrReturnError(reader.EnterContainer(outerContainer) == CHIP_NO_ERROR, ESP_FAIL);
    VerifyOrReturnError(reader.Next() == CHIP_NO_ERROR, ESP_FAIL);

    // create write request and call WriteAttribute through the provider
    chip::app::DataModel::WriteAttributeRequest request;
    request.path = chip::app::ConcreteDataAttributePath(endpoint_id, cluster_id, attribute_id);
    request.operationFlags.Set(chip::app::DataModel::OperationFlags::kInternal);

    chip::Access::SubjectDescriptor subjectDescriptor;
    chip::app::AttributeValueDecoder decoder(reader, subjectDescriptor);

    // we need to ensure locks are in place to avoid assertion errors
    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);

    chip::app::DataModel::ActionReturnStatus status =
        esp_matter::data_model::provider::get_instance().WriteAttribute(request, decoder);

    if (status.IsError()) {
        chip::app::DataModel::ActionReturnStatus::StringStorage storage;
        ESP_LOGE(TAG, "WriteAttribute failed with status: %s", status.c_str(storage));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t set_val(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val, bool call_callbacks)
{
    VerifyOrReturnError(val && val->type != ESP_MATTER_VAL_TYPE_INVALID, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(val->type != ESP_MATTER_VAL_TYPE_ARRAY, ESP_ERR_NOT_SUPPORTED);

    attribute_t *attr = get(endpoint_id, cluster_id, attribute_id);
    VerifyOrReturnError(attr, ESP_ERR_NOT_FOUND);
    VerifyOrReturnError(get_val_type(attr) == val->type, ESP_ERR_INVALID_ARG);

    uint16_t flags = get_flags(attr);

    if (!(flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY)) {
        // this updates the value of attribute in the esp-matter storage
        return attribute::set_val_internal(attr, val, call_callbacks);
    }

    // we can use DataModelProvider::WriteAttribute API for writable attributes
    if (flags & ATTRIBUTE_FLAG_WRITABLE) {
        return set_val_via_write_attribute(endpoint_id, cluster_id, attribute_id, val);
    }

    // TODO: If not writable, we could use the cluster-specific setter API to update the value
    //       with the code-driven effort, we can get the cluster object and call the setter API
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t set_val(attribute_t *attribute, esp_matter_attr_val_t *val, bool call_callbacks)
{
    VerifyOrReturnError(attribute && val, ESP_ERR_INVALID_ARG);
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    uint16_t endpoint_id = chip::kInvalidEndpointId;
    uint32_t cluster_id = chip::kInvalidClusterId;
    uint32_t attribute_id = chip::kInvalidAttributeId;

    esp_err_t err = get_path_from_attribute_handle(current_attribute, endpoint_id, cluster_id, attribute_id);
    VerifyOrReturnError(err == ESP_OK, err);

    return set_val(endpoint_id, cluster_id, attribute_id, val, call_callbacks);
}

esp_err_t add_bounds(attribute_t *attribute, esp_matter_attr_val_t min, esp_matter_attr_val_t max)
{
    VerifyOrReturnError(attribute, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Attribute cannot be NULL"));
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    ESP_RETURN_ON_FALSE(!(current_attribute->flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY), ESP_ERR_NOT_SUPPORTED, TAG,
                        "Attribute is not managed by esp matter data model");

    /* Check if bounds can be set */
    if (current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_ARRAY ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        ESP_LOGE(TAG, "Bounds cannot be set for string/array/boolean type attributes");
        return ESP_ERR_INVALID_ARG;
    }
    VerifyOrReturnError(((current_attribute->attribute_val_type == min.type) && (current_attribute->attribute_val_type == max.type)),
                        ESP_ERR_INVALID_ARG,
                        ESP_LOGE(TAG, "Cannot set bounds because of val type mismatch: expected: %d, min: %d, max: %d",
                                 current_attribute->attribute_val_type, min.type, max.type));
    current_attribute->bounds = (esp_matter_attr_bounds_t *)esp_matter_mem_calloc(1, sizeof(esp_matter_attr_bounds_t));
    if (!current_attribute->bounds) {
        ESP_LOGE(TAG, "Failed to allocate bounds for attribute");
        return ESP_ERR_NO_MEM;
    }
    current_attribute->flags |= ATTRIBUTE_FLAG_MIN_MAX;
    current_attribute->bounds->min = min;
    current_attribute->bounds->max = max;
    return bound_attribute_val(attribute);
}

esp_err_t get_bounds(attribute_t *attribute, esp_matter_attr_bounds_t *bounds)
{
    if (!attribute || !bounds) {
        ESP_LOGE(TAG, "Attribute or bounds cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    if (current_attribute->flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY) {
        ESP_LOGE(TAG, "Cannot get bounds for attribute managed internally");
        return ESP_ERR_INVALID_ARG;
    }

    if (!(current_attribute->flags & ATTRIBUTE_FLAG_MIN_MAX)) {
        ESP_LOGW(TAG,
                 "Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32 " has not set bounds",
                 current_attribute->endpoint_id, current_attribute->cluster_id, current_attribute->attribute_id);
        return ESP_ERR_INVALID_ARG;
    }
    *bounds = *current_attribute->bounds;
    return ESP_OK;
}

uint16_t get_flags(attribute_t *attribute)
{
    VerifyOrReturnValue(attribute, 0, ESP_LOGE(TAG, "Attribute cannot be NULL"));
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    return current_attribute->flags;
}

esp_err_t set_override_callback(attribute_t *attribute, callback_t callback)
{
    VerifyOrReturnError(attribute, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Attribute cannot be NULL"));
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    ESP_RETURN_ON_FALSE(!(current_attribute->flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY), ESP_ERR_NOT_SUPPORTED, TAG,
                        "Attribute is not managed by esp matter data model");

    if (current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_ARRAY ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        current_attribute->attribute_val_type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING) {
        // The override callback might allocate memory and we have no way to free the memory
        // TODO: Add memory-safe override callback for these attribute types
        ESP_LOGE(TAG, "Cannot set override callback for attribute 0x%" PRIX32, current_attribute->attribute_id);
        return ESP_ERR_NOT_SUPPORTED;
    }
    current_attribute->override_callback = callback;
    current_attribute->flags |= ATTRIBUTE_FLAG_OVERRIDE;
    return ESP_OK;
}

callback_t get_override_callback(attribute_t *attribute)
{
    VerifyOrReturnValue(attribute, NULL, ESP_LOGE(TAG, "Attribute cannot be NULL"));
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    VerifyOrReturnValue(!(current_attribute->flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY), NULL,
                        ESP_LOGE(TAG, "Attribute is not managed by esp matter data model"));

    return current_attribute->override_callback;
}

esp_err_t set_deferred_persistence(attribute_t *attribute)
{
    VerifyOrReturnError(attribute, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Attribute cannot be NULL"));
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    ESP_RETURN_ON_FALSE(!(current_attribute->flags & ATTRIBUTE_FLAG_MANAGED_INTERNALLY), ESP_ERR_NOT_SUPPORTED, TAG,
                        "Attribute is not managed by esp matter data model");

    if (!(current_attribute->flags & ATTRIBUTE_FLAG_NONVOLATILE)) {
        ESP_LOGE(TAG, "Attribute should be non-volatile to set a deferred persistence time");
        return ESP_ERR_INVALID_ARG;
    }
    current_attribute->flags |= ATTRIBUTE_FLAG_DEFERRED;
    return ESP_OK;
}

} // namespace attribute

namespace command {
command_t *create(cluster_t *cluster, uint32_t command_id, uint8_t flags, callback_t callback)
{
    /* Find */
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    command_t *existing_command = get(cluster, command_id, flags);
    if (existing_command) {
        ESP_LOGW(TAG, "Command 0x%08" PRIX32 " on cluster 0x%08" PRIX32 " already exists. Not creating again.",
                 command_id, cluster::get_id(cluster));
        return existing_command;
    }

    /* Allocate */
    _command_t *command = (_command_t *)esp_matter_mem_calloc(1, sizeof(_command_t));
    VerifyOrReturnValue(command, NULL, ESP_LOGE(TAG, "Couldn't allocate _command_t"));

    /* Set */
    command->command_id = command_id;
    command->flags = flags;
    command->callback = callback;
    command->user_callback = NULL;

    /* Add */
    SinglyLinkedList<_command_t>::append(&current_cluster->command_list, command);
    return (command_t *)command;
}

command_t *get(uint16_t endpoint_id, uint32_t cluster_id, uint32_t command_id)
{
    _cluster_t *current_cluster = (_cluster_t *)cluster::get(endpoint_id, cluster_id);
    VerifyOrReturnValue(current_cluster, NULL);
    _command_t *command = (_command_t *)current_cluster->command_list;

    while (command) {
        if (command->command_id == command_id) {
            break;
        }
        command = command->next;
    }
    return (command_t *)command;
}

command_t *get(cluster_t *cluster, uint32_t command_id, uint16_t flags)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    _command_t *current_command = (_command_t *)current_cluster->command_list;
    while (current_command) {
        if ((current_command->command_id == command_id) && (current_command->flags & flags)) {
            break;
        }
        current_command = current_command->next;
    }
    return (command_t *)current_command;
}

command_t *get_first(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return (command_t *)current_cluster->command_list;
}

command_t *get_next(command_t *command)
{
    VerifyOrReturnValue(command, NULL, ESP_LOGE(TAG, "Command cannot be NULL"));
    _command_t *current_command = (_command_t *)command;
    return (command_t *)current_command->next;
}

uint32_t get_id(command_t *command)
{
    VerifyOrReturnValue(command, kInvalidCommandId, ESP_LOGE(TAG, "Command cannot be NULL"));
    _command_t *current_command = (_command_t *)command;
    return current_command->command_id;
}

callback_t get_callback(command_t *command)
{
    VerifyOrReturnValue(command, NULL, ESP_LOGE(TAG, "Command cannot be NULL"));
    _command_t *current_command = (_command_t *)command;
    return current_command->callback;
}

callback_t get_user_callback(command_t *command)
{
    VerifyOrReturnValue(command, NULL, ESP_LOGE(TAG, "Command cannot be NULL"));
    _command_t *current_command = (_command_t *)command;
    return current_command->user_callback;
}

void set_user_callback(command_t *command, callback_t user_callback)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
    }
    _command_t *current_command = (_command_t *)command;
    current_command->user_callback = user_callback;
}

uint16_t get_flags(command_t *command)
{
    VerifyOrReturnValue(command, 0, ESP_LOGE(TAG, "Command cannot be NULL"));
    _command_t *current_command = (_command_t *)command;
    return current_command->flags;
}

} // namespace command

namespace event {

event_t *create(cluster_t *cluster, uint32_t event_id)
{
    /* Find */
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    event_t *existing_event = get(cluster, event_id);
    if (existing_event) {
        ESP_LOGW(TAG, "Event 0x%08" PRIX32 " on cluster 0x%08" PRIX32 " already exists. Not creating again.", event_id,
                 cluster::get_id(cluster));
        return existing_event;
    }

    /* Allocate */
    _event_t *event = (_event_t *)esp_matter_mem_calloc(1, sizeof(_event_t));
    VerifyOrReturnValue(event, NULL, ESP_LOGE(TAG, "Couldn't allocate _event_t"));

    /* Set */
    event->event_id = event_id;

    /* Add */
    SinglyLinkedList<_event_t>::append(&current_cluster->event_list, event);
    return (event_t *)event;
}

event_t *get(cluster_t *cluster, uint32_t event_id)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    _event_t *current_event = (_event_t *)current_cluster->event_list;
    while (current_event) {
        if (current_event->event_id == event_id) {
            break;
        }
        current_event = current_event->next;
    }
    return (event_t *)current_event;
}

event_t *get_first(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return (event_t *)current_cluster->event_list;
}

event_t *get_next(event_t *event)
{
    VerifyOrReturnValue(event, NULL, ESP_LOGE(TAG, "Event cannot be NULL"));
    _event_t *current_event = (_event_t *)event;
    return (event_t *)current_event->next;
}

uint32_t get_id(event_t *event)
{
    VerifyOrReturnValue(event, chip::kInvalidEventId, ESP_LOGE(TAG, "Event cannot be NULL"));
    _event_t *current_event = (_event_t *)event;
    return current_event->event_id;
}

} // namespace event

namespace cluster {

cluster_t *create(endpoint_t *endpoint, uint32_t cluster_id, uint8_t flags)
{
    /* Find */
    VerifyOrReturnValue(endpoint, NULL, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    VerifyOrReturnValue(((flags & CLUSTER_FLAG_SERVER) || (flags & CLUSTER_FLAG_CLIENT)), NULL,
                        ESP_LOGE(TAG, "Server or client cluster flag not set"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    cluster_t *existing_cluster = get(endpoint, cluster_id);
    if (existing_cluster) {
        _cluster_t *_existing_cluster = (_cluster_t *)existing_cluster;
        _existing_cluster->flags |= flags;
        return existing_cluster;
    }

    /* Allocate */
    _cluster_t *cluster = (_cluster_t *)esp_matter_mem_calloc(1, sizeof(_cluster_t));
    if (!cluster) {
        ESP_LOGE(TAG, "Couldn't allocate _cluster_t");
        return NULL;
    }

    /* Set */
    cluster->cluster_id = cluster_id;
    cluster->endpoint_id = current_endpoint->endpoint_id;
    cluster->flags = flags;
    cluster->add_bounds_callback = nullptr;
    cluster->attribute_list = nullptr;
    cluster->delegate_init_callback = nullptr;
    cluster->data_version = esp_random(); // Generate a random 32-bit number
    cluster->functions = nullptr;
    cluster->plugin_server_init_callback = nullptr;
    cluster->init_callback = nullptr;
    cluster->shutdown_callback = nullptr;

    /* Add */
    SinglyLinkedList<_cluster_t>::append(&current_endpoint->cluster_list, cluster);
    return (cluster_t *)cluster;
}

esp_err_t destroy(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;

    /* Parse and delete all commands */
    SinglyLinkedList<_command_t>::delete_list(&current_cluster->command_list);

    /* Parse and delete all attributes */
    _attribute_base_t *attribute = current_cluster->attribute_list;
    while (attribute) {
        _attribute_base_t *next_attribute = attribute->next;
        attribute::destroy((attribute_t *)attribute);
        attribute = next_attribute;
    }

    /* Parse and delete all events */
    SinglyLinkedList<_event_t>::delete_list(&current_cluster->event_list);

    /* Free */
    esp_matter_mem_free(current_cluster);
    return ESP_OK;
}

cluster_t *get(endpoint_t *endpoint, uint32_t cluster_id)
{
    VerifyOrReturnValue(endpoint, NULL, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    _cluster_t *current_cluster = (_cluster_t *)current_endpoint->cluster_list;

    while (current_cluster) {
        if (current_cluster->cluster_id == cluster_id) {
            break;
        }
        current_cluster = current_cluster->next;
    }
    return (cluster_t *)current_cluster;
}

cluster_t *get(uint16_t endpoint_id, uint32_t cluster_id)
{
    endpoint_t *endpoint = endpoint::get(endpoint_id);
    return get(endpoint, cluster_id);
}

cluster_t *get_first(endpoint_t *endpoint)
{
    VerifyOrReturnValue(endpoint, NULL, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return (cluster_t *)current_endpoint->cluster_list;
}

cluster_t *get_next(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return (cluster_t *)current_cluster->next;
}

uint32_t get_id(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, kInvalidClusterId, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return current_cluster->cluster_id;
}

uint8_t get_flags(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, 0, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return current_cluster->flags;
}

esp_err_t get_data_version(cluster_t *cluster, chip::DataVersion &data_version)
{
    VerifyOrReturnValue(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    data_version = current_cluster->data_version;
    return ESP_OK;
}

esp_err_t increase_data_version(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    current_cluster->data_version++;
    return ESP_OK;
}

void *get_delegate_impl(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return current_cluster->delegate_pointer;
}

esp_err_t set_plugin_server_init_callback(cluster_t *cluster, plugin_server_init_callback_t callback)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    current_cluster->plugin_server_init_callback = callback;
    return ESP_OK;
}

plugin_server_init_callback_t get_plugin_server_init_callback(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return current_cluster->plugin_server_init_callback;
}

esp_err_t set_delegate_and_init_callback(cluster_t *cluster, delegate_init_callback_t callback, void *delegate)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    current_cluster->delegate_init_callback = callback;
    current_cluster->delegate_pointer = delegate;
    return ESP_OK;
}

delegate_init_callback_t get_delegate_init_callback(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL."));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return current_cluster->delegate_init_callback;
}

esp_err_t set_add_bounds_callback(cluster_t *cluster, add_bounds_callback_t callback)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    current_cluster->add_bounds_callback = callback;
    return ESP_OK;
}

add_bounds_callback_t get_add_bounds_callback(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return current_cluster->add_bounds_callback;
}

esp_err_t add_function_list(cluster_t *cluster, const function_generic_t *function_list, int function_flags)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    current_cluster->flags |= function_flags;
    current_cluster->functions = function_list;
    return ESP_OK;
}

function_generic_t get_function(cluster_t *cluster, uint8_t function_flag)
{
    VerifyOrReturnValue(cluster, nullptr, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    if ((current_cluster->flags & function_flag) == 0) {
        return nullptr;
    }
    uint8_t flag = 0x01;
    uint8_t index = 0;
    while (flag < function_flag) {
        if ((current_cluster->flags & flag) != 0) {
            index++;
        }
        flag = flag << 1;
    }
    return current_cluster->functions[index];
}

esp_err_t set_init_and_shutdown_callbacks(cluster_t *cluster, initialization_callback_t init_callback,
                                          shutdown_callback_t shutdown_callback)
{
    VerifyOrReturnValue(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    current_cluster->init_callback = init_callback;
    current_cluster->shutdown_callback = shutdown_callback;
    return ESP_OK;
}

initialization_callback_t get_init_callback(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, nullptr, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return current_cluster->init_callback;
}

shutdown_callback_t get_shutdown_callback(cluster_t *cluster)
{
    VerifyOrReturnValue(cluster, nullptr, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return current_cluster->shutdown_callback;
}

} // namespace cluster

namespace endpoint {

endpoint_t *create(node_t *node, uint8_t flags, void *priv_data)
{
    /* Find */
    VerifyOrReturnValue(node, NULL, ESP_LOGE(TAG, "Node cannot be NULL"));
    _node_t *current_node = (_node_t *)node;

    VerifyOrReturnValue(
        get_count(node) < CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT, NULL,
        ESP_LOGE(TAG, "Dynamic endpoint count cannot be greater than CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT:%u",
                 CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT));

    /* Allocate */
    _endpoint_t *endpoint = (_endpoint_t *)esp_matter_mem_calloc(1, sizeof(_endpoint_t));
    VerifyOrReturnValue(endpoint, NULL, ESP_LOGE(TAG, "Couldn't allocate _endpoint_t"));

    /* Set */
    endpoint->endpoint_id = current_node->min_unused_endpoint_id++;
    endpoint->device_type_count = 0;
    endpoint->parent_endpoint_id = chip::kInvalidEndpointId;
    endpoint->flags = flags;
    endpoint->priv_data = priv_data;
    endpoint->composition_pattern = EndpointCompositionPattern::kFullFamily;
    endpoint->semantic_tag_count = 0;
    endpoint->enabled = true;
    /* Store */
    if (esp_matter::is_started()) {
        node::store_min_unused_endpoint_id();
    }

    /* Add */
    SinglyLinkedList<_endpoint_t>::append(&current_node->endpoint_list, endpoint);

    return (endpoint_t *)endpoint;
}

endpoint_t *resume(node_t *node, uint8_t flags, uint16_t endpoint_id, void *priv_data)
{
    /* Find */
    VerifyOrReturnValue(node, NULL, ESP_LOGE(TAG, "Node cannot be NULL"));
    _node_t *current_node = (_node_t *)node;
    _endpoint_t *previous_endpoint = NULL;
    _endpoint_t *current_endpoint = current_node->endpoint_list;
    while (current_endpoint) {
        VerifyOrReturnValue(current_endpoint->endpoint_id != endpoint_id, NULL,
                            ESP_LOGE(TAG, "Could not resume an endpoint that has been added to the node"));
        previous_endpoint = current_endpoint;
        current_endpoint = current_endpoint->next;
    }

    /* Check */
    VerifyOrReturnError(endpoint_id < current_node->min_unused_endpoint_id, NULL,
                        ESP_LOGE(TAG, "The endpoint_id of the resumed endpoint should have been used"));

    /* Allocate */
    _endpoint_t *endpoint = (_endpoint_t *)esp_matter_mem_calloc(1, sizeof(_endpoint_t));
    VerifyOrReturnValue(endpoint, NULL, ESP_LOGE(TAG, "Couldn't allocate _endpoint_t"));

    /* Set */
    endpoint->endpoint_id = endpoint_id;
    endpoint->device_type_count = 0;
    endpoint->flags = flags;
    endpoint->priv_data = priv_data;
    endpoint->composition_pattern = EndpointCompositionPattern::kFullFamily;

    /* Add */
    if (previous_endpoint == NULL) {
        current_node->endpoint_list = endpoint;
    } else {
        previous_endpoint->next = endpoint;
    }

    return (endpoint_t *)endpoint;
}

esp_err_t destroy(node_t *node, endpoint_t *endpoint)
{
    VerifyOrReturnError((node && endpoint), ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Node or endpoint cannot be NULL"));
    _node_t *current_node = (_node_t *)node;
    _endpoint_t *_endpoint = (_endpoint_t *)endpoint;

    VerifyOrReturnError(
        (_endpoint->flags & ENDPOINT_FLAG_DESTROYABLE), ESP_FAIL,
        ESP_LOGE(TAG, "This endpoint cannot be deleted since the ENDPOINT_FLAG_DESTROYABLE is not set"));

    /* Disable */
    disable(endpoint);

    /* Find current endpoint */
    _endpoint_t *current_endpoint = current_node->endpoint_list;
    _endpoint_t *previous_endpoint = NULL;
    while (current_endpoint) {
        if (current_endpoint == _endpoint) {
            break;
        }
        previous_endpoint = current_endpoint;
        current_endpoint = current_endpoint->next;
    }
    VerifyOrReturnError(current_endpoint != NULL, ESP_FAIL, ESP_LOGE(TAG, "Could not find the endpoint to delete"));

    /* Parse and delete all clusters */
    _cluster_t *cluster = current_endpoint->cluster_list;
    while (cluster) {
        _cluster_t *next_cluster = cluster->next;
        cluster::destroy((cluster_t *)cluster);
        cluster = next_cluster;
        /* Move cluster_list to find the remain cluster */
        current_endpoint->cluster_list = cluster;
    }

    /* Remove from list */
    if (previous_endpoint == NULL) {
        current_node->endpoint_list = current_endpoint->next;
    } else {
        previous_endpoint->next = current_endpoint->next;
    }

    /* Free */
    if (current_endpoint->identify != NULL) {
        chip::Platform::Delete(current_endpoint->identify);
        current_endpoint->identify = NULL;
    }
    esp_matter_mem_free(current_endpoint);

    return ESP_OK;
}

endpoint_t *get(node_t *node, uint16_t endpoint_id)
{
    VerifyOrReturnValue(node, NULL, ESP_LOGE(TAG, "Node cannot be NULL"));
    _node_t *current_node = (_node_t *)node;
    _endpoint_t *current_endpoint = (_endpoint_t *)current_node->endpoint_list;
    while (current_endpoint) {
        if (current_endpoint->endpoint_id == endpoint_id) {
            break;
        }
        current_endpoint = current_endpoint->next;
    }
    return (endpoint_t *)current_endpoint;
}

endpoint_t *get(uint16_t endpoint_id)
{
    node_t *node = node::get();
    return get(node, endpoint_id);
}

endpoint_t *get_first(node_t *node)
{
    VerifyOrReturnValue(node, NULL, ESP_LOGE(TAG, "Node cannot be NULL"));
    _node_t *current_node = (_node_t *)node;
    return (endpoint_t *)current_node->endpoint_list;
}

endpoint_t *get_next(endpoint_t *endpoint)
{
    VerifyOrReturnValue(endpoint, NULL, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return (endpoint_t *)current_endpoint->next;
}

uint16_t get_count(node_t *node)
{
    VerifyOrReturnValue(node, 0, ESP_LOGE(TAG, "Node cannot be NULL"));
    uint16_t count = 0;
    endpoint_t *endpoint = get_first(node);
    while (endpoint) {
        count++;
        endpoint = get_next(endpoint);
    }
    return count;
}

uint16_t get_id(endpoint_t *endpoint)
{
    VerifyOrReturnValue(endpoint, kInvalidEndpointId, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return current_endpoint->endpoint_id;
}

esp_err_t add_device_type(endpoint_t *endpoint, uint32_t device_type_id, uint8_t device_type_version)
{
    VerifyOrReturnError(endpoint, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    VerifyOrReturnError(current_endpoint->device_type_count < ESP_MATTER_MAX_DEVICE_TYPE_COUNT, ESP_FAIL,
                        ESP_LOGE(TAG, "Could not add a new device-type:%" PRIu32 " to the endpoint", device_type_id));
    current_endpoint->device_types[current_endpoint->device_type_count].id = device_type_id;
    current_endpoint->device_types[current_endpoint->device_type_count].version = device_type_version;
    current_endpoint->device_type_count++;
    return ESP_OK;
}

size_t get_device_type_count(endpoint_t *endpoint)
{
    VerifyOrReturnValue(endpoint, 0, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return current_endpoint->device_type_count;
}

esp_err_t get_device_type_at_index(endpoint_t *endpoint, size_t index, uint32_t &out_device_type_id,
                                   uint8_t &out_device_type_version)
{
    VerifyOrReturnValue(endpoint, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    VerifyOrReturnValue(index < current_endpoint->device_type_count, ESP_ERR_INVALID_ARG,
                        ESP_LOGE(TAG, "Index should be less than device_type_count"));
    out_device_type_id = current_endpoint->device_types[index].id;
    out_device_type_version = current_endpoint->device_types[index].version;
    return ESP_OK;
}

esp_err_t set_parent_endpoint(endpoint_t *endpoint, endpoint_t *parent_endpoint)
{
    VerifyOrReturnError((endpoint && parent_endpoint), ESP_ERR_INVALID_ARG,
                        ESP_LOGE(TAG, "Endpoint or parent_endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    _endpoint_t *current_parent_endpoint = (_endpoint_t *)parent_endpoint;
    current_endpoint->parent_endpoint_id = current_parent_endpoint->endpoint_id;
    return ESP_OK;
}

uint16_t get_parent_endpoint_id(endpoint_t *endpoint)
{
    VerifyOrReturnValue(endpoint, 0xFFFF, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return current_endpoint->parent_endpoint_id;
}

EndpointCompositionPattern get_composition_pattern(endpoint_t *endpoint)
{
    VerifyOrReturnValue(endpoint, EndpointCompositionPattern::kFullFamily, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return current_endpoint->composition_pattern;
}

esp_err_t set_semantic_tags(endpoint_t *endpoint, const chip::app::DataModel::Provider::SemanticTag *tags, size_t tag_count)
{
    VerifyOrReturnValue(endpoint, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    VerifyOrReturnValue(tag_count <= ESP_MATTER_MAX_SEMANTIC_TAG_COUNT, ESP_ERR_INVALID_ARG,
                        ESP_LOGE(TAG, "Tag count should be no more than %d", ESP_MATTER_MAX_SEMANTIC_TAG_COUNT));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    if (tags != nullptr) {
        for (size_t i = 0; i < tag_count; ++i) {
            current_endpoint->semantic_tags[i] = tags[i];
        }
    }
    current_endpoint->semantic_tag_count = tags ? tag_count : 0;
    return ESP_OK;
}

int get_semantic_tag_count(endpoint_t *endpoint)
{
    VerifyOrReturnValue(endpoint, -1, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return current_endpoint->semantic_tag_count;
}

esp_err_t get_semantic_tag_at_index(endpoint_t *endpoint, size_t index,
                                    chip::app::DataModel::Provider::SemanticTag &tag)
{
    VerifyOrReturnValue(endpoint, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Endpoint cannot be NULL"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    if (index >= current_endpoint->semantic_tag_count) {
        return ESP_ERR_NOT_FOUND;
    }
    tag = current_endpoint->semantic_tags[index];
    return ESP_OK;
}

/**
 * @brief Get the number of clusters that match the given flags
 *
 * @param endpoint_id: The endpoint ID to check, 0xFFFF is treated as wildcard endpoint id
 * @param cluster_id: The cluster ID to check, 0xFFFF is treated as wildcard cluster id
 * @param cluster_flags: The flags to check
 * @return The number of clusters that match the given flags
 */
uint32_t get_cluster_count(uint32_t endpoint_id, uint32_t cluster_id, uint8_t cluster_flags)
{
    uint32_t count = 0;
    node_t *node = node::get();
    VerifyOrReturnValue(node, count, ESP_LOGE(TAG, "Node cannot be NULL"));

    // lambda to check if cluster matches flags and return 1 if it does, 0 otherwise
    auto check_cluster_flags = [cluster_flags](const cluster_t *cluster) -> uint32_t {
        if (cluster) {
            const _cluster_t *_cluster = (_cluster_t *)cluster;
            return (_cluster->flags & cluster_flags) ? 1 : 0;
        }
        return 0;
    };

    // lambda to count all matching clusters for an endpoint
    auto get_count_on_all_clusters = [&check_cluster_flags](endpoint_t *endpoint) -> uint32_t {
        uint32_t result = 0;
        if (!endpoint)
            return result;

        cluster_t *cluster = cluster::get_first(endpoint);
        while (cluster) {
            result += check_cluster_flags(cluster);
            cluster = cluster::get_next(cluster);
        }
        return result;
    };

    // lambda to find and count a specific cluster
    auto get_count_on_specific_cluster = [&check_cluster_flags](const endpoint_t *endpoint,
                                                                uint32_t cluster_id) -> uint32_t {
        if (!endpoint)
            return 0;
        cluster_t *cluster = cluster::get((endpoint_t *)endpoint, cluster_id);
        return check_cluster_flags(cluster);
    };

    // Case 1: Wildcard endpoint
    if (is_wildcard_endpoint_id(endpoint_id)) {
        endpoint_t *endpoint = endpoint::get_first(node);
        while (endpoint) {
            if (endpoint::is_enabled(endpoint)) {
                // Case 1.1: Wildcard cluster - count all clusters with matching flags
                if (is_wildcard_cluster_id(cluster_id)) {
                    count += get_count_on_all_clusters(endpoint);
                }
                // Case 1.2: Specific cluster - count if it exists and has matching flags
                else {
                    count += get_count_on_specific_cluster(endpoint, cluster_id);
                }
            }
            endpoint = endpoint::get_next(endpoint);
        }
    }
    // Case 2: Specific endpoint
    else {
        endpoint_t *endpoint = endpoint::get(endpoint_id);
        if (!endpoint || !endpoint::is_enabled(endpoint)) {
            return count;
        }

        // Case 2.1: Wildcard cluster - count all clusters with matching flags
        if (is_wildcard_cluster_id(cluster_id)) {
            count += get_count_on_all_clusters(endpoint);
        }
        // Case 2.2: Specific cluster - count if it exists and has matching flags
        else {
            count += get_count_on_specific_cluster(endpoint, cluster_id);
        }
    }

    return count;
}

void *get_priv_data(uint16_t endpoint_id)
{
    endpoint_t *endpoint = get(endpoint_id);
    VerifyOrReturnValue(endpoint, NULL, ESP_LOGE(TAG, "Endpoint not found"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return current_endpoint->priv_data;
}

esp_err_t set_priv_data(uint16_t endpoint_id, void *priv_data)
{
    endpoint_t *endpoint = get(endpoint_id);
    VerifyOrReturnError(endpoint, ESP_ERR_NOT_FOUND, ESP_LOGE(TAG, "Endpoint not found"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    current_endpoint->priv_data = priv_data;
    return ESP_OK;
}

esp_err_t set_identify(uint16_t endpoint_id, void *identify)
{
    endpoint_t *endpoint = get(endpoint_id);
    VerifyOrReturnError(endpoint, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Endpoint not found"));
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    current_endpoint->identify = (Identify *)identify;
    return ESP_OK;
}

} // namespace endpoint

namespace node {

node_t *create_raw()
{
    VerifyOrReturnValue(!node, (node_t *)node, ESP_LOGE(TAG, "Node already exists"));
    node = (_node_t *)esp_matter_mem_calloc(1, sizeof(_node_t));
    VerifyOrReturnValue(node, NULL, ESP_LOGE(TAG, "Couldn't allocate _node_t"));
    return (node_t *)node;
}

esp_err_t destroy_raw()
{
    VerifyOrReturnError(node, ESP_ERR_INVALID_STATE, ESP_LOGE(TAG, "NULL node cannot be destroyed"));
    _node_t *current_node = (_node_t *)node;
    esp_matter_mem_free(current_node);
    node = NULL;
    return ESP_OK;
}

node_t *get()
{
    return (node_t *)node;
}

esp_err_t destroy()
{
    esp_err_t err = ESP_OK;
    node_t *current_node = get();
    VerifyOrReturnError(current_node, ESP_ERR_INVALID_STATE, ESP_LOGE(TAG, "Node cannot be NULL"));

    attribute::set_callback(nullptr);
    identification::set_callback(nullptr);

    endpoint_t *current_endpoint = endpoint::get_first(current_node);
    endpoint_t *next_endpoint = nullptr;
    while (current_endpoint != nullptr) {
        next_endpoint = endpoint::get_next(current_endpoint);
        // Endpoints should have destroyable flag set to true before destroying
        ((_endpoint_t *)current_endpoint)->flags |= ENDPOINT_FLAG_DESTROYABLE;
        err = endpoint::destroy((node_t *)current_node, current_endpoint);
        VerifyOrDo(err == ESP_OK, ESP_LOGE(TAG, "Failed to destroy endpoint"));

        current_endpoint = next_endpoint;
    }

    return destroy_raw();
}

uint32_t get_server_cluster_endpoint_count(uint32_t cluster_id)
{
    return endpoint::get_cluster_count(chip::kInvalidEndpointId, cluster_id, CLUSTER_FLAG_SERVER);
}

uint32_t get_client_cluster_endpoint_count(uint32_t cluster_id)
{
    return endpoint::get_cluster_count(chip::kInvalidEndpointId, cluster_id, CLUSTER_FLAG_CLIENT);
}

} // namespace node
} // namespace esp_matter
