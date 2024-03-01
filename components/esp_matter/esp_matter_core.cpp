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
#include <esp_matter_core.h>
#include <nvs.h>
#if CONFIG_BT_ENABLED
#include <esp_bt.h>
#if CONFIG_BT_NIMBLE_ENABLED
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
#include <esp_nimble_hci.h>
#endif
#include <host/ble_hs.h>
#include <nimble/nimble_port.h>
#endif /* CONFIG_BT_NIMBLE_ENABLED */
#endif /* CONFIG_BT_ENABLED */

#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/clusters/general-diagnostics-server/general-diagnostics-server.h>
#include <app/server/Dnssd.h>
#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/FabricTable.h>
#include <lib/core/DataModelTypes.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/DeviceInfoProvider.h>
#include <platform/DiagnosticDataProvider.h>
#include <platform/ESP32/ESP32Utils.h>
#include <platform/ESP32/NetworkCommissioningDriver.h>
#include <esp_matter_ota.h>
#include <esp_matter_mem.h>
#include <esp_matter_providers.h>

#include <esp_matter_nvs.h>

using chip::CommandId;
using chip::DataVersion;
using chip::EventId;
using chip::kInvalidAttributeId;
using chip::kInvalidCommandId;
using chip::kInvalidClusterId;
using chip::kInvalidEndpointId;
using chip::Credentials::SetDeviceAttestationCredentialsProvider;
using chip::DeviceLayer::ChipDeviceEvent;
using chip::DeviceLayer::ConfigurationMgr;
using chip::DeviceLayer::ConnectivityManager;
using chip::DeviceLayer::ConnectivityMgr;
using chip::DeviceLayer::PlatformMgr;
using chip::DeviceLayer::DiagnosticDataProvider;
using chip::DeviceLayer::GetDiagnosticDataProvider;
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
using chip::DeviceLayer::ThreadStackMgr;
#endif

#define ESP_MATTER_NVS_PART_NAME CONFIG_ESP_MATTER_NVS_PART_NAME
#define ESP_MATTER_MAX_DEVICE_TYPE_COUNT CONFIG_ESP_MATTER_MAX_DEVICE_TYPE_COUNT

static const char *TAG = "esp_matter_core";
static bool esp_matter_started = false;

#ifndef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
// If Matter Server is disabled, we should have an empty InitDataModelHandler()
void InitDataModelHandler() {}
#endif

namespace esp_matter {

namespace {

void PostEvent(uint16_t eventType)
{
    chip::DeviceLayer::ChipDeviceEvent event;
    event.Type = eventType;
    CHIP_ERROR error = chip::DeviceLayer::PlatformMgr().PostEvent(&event);
    if (error != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "Failed to post event for event type:%" PRIu16 ", err:%" CHIP_ERROR_FORMAT, eventType, error.Format());
    }
}

class AppDelegateImpl : public AppDelegate
{
public:
    void OnCommissioningSessionStarted()
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted);
    }

    void OnCommissioningSessionStopped()
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped);
    }

    void OnCommissioningWindowOpened()
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened);
    }

    void OnCommissioningWindowClosed()
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed);
    }
};

class FabricDelegateImpl : public chip::FabricTable::Delegate
{
public:
    void FabricWillBeRemoved(const chip::FabricTable & fabricTable,chip::FabricIndex fabricIndex)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kFabricWillBeRemoved);
    }

    void OnFabricRemoved(const chip::FabricTable & fabricTable,chip::FabricIndex fabricIndex)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kFabricRemoved);
    }

    void OnFabricCommitted(const chip::FabricTable & fabricTable, chip::FabricIndex fabricIndex)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kFabricCommitted);
    }

    void OnFabricUpdated(const chip::FabricTable & fabricTable, chip::FabricIndex fabricIndex)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kFabricUpdated);
    }
};

AppDelegateImpl s_app_delegate;

FabricDelegateImpl s_fabric_delegate;

}  // namespace

typedef struct _attribute {
    uint32_t attribute_id;
    uint32_t cluster_id;
    uint16_t endpoint_id;
    uint16_t flags;
    esp_matter_attr_val_t val;
    esp_matter_attr_bounds_t *bounds;
    EmberAfDefaultOrMinMaxAttributeValue default_value;
    uint16_t default_value_size;
    attribute::callback_t override_callback;
    struct _attribute *next;
} _attribute_t;

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
    uint16_t flags;
    const cluster::function_generic_t *function_list;
    cluster::plugin_server_init_callback_t plugin_server_init_callback;
    _attribute_t *attribute_list;
    _command_t *command_list;
    _event_t *event_list;
    struct _cluster *next;
} _cluster_t;

typedef struct _endpoint {
    uint16_t endpoint_id;
    uint8_t device_type_count;
    uint32_t device_type_ids[ESP_MATTER_MAX_DEVICE_TYPE_COUNT];
    uint8_t device_type_versions[ESP_MATTER_MAX_DEVICE_TYPE_COUNT];
    uint16_t flags;
    _cluster_t *cluster_list;
    EmberAfEndpointType *endpoint_type;
    DataVersion *data_versions_ptr;
    EmberAfDeviceType *device_types_ptr;
    uint16_t parent_endpoint_id;
    void *priv_data;
    struct _endpoint *next;
} _endpoint_t;

typedef struct _node {
    _endpoint_t *endpoint_list;
    uint16_t min_unused_endpoint_id;
} _node_t;

namespace node {

static _node_t *node = NULL;

static esp_err_t store_min_unused_endpoint_id()
{
    if (!node || !esp_matter_started) {
        ESP_LOGE(TAG, "Node does not exist or esp_matter does not start");
        return ESP_ERR_INVALID_STATE;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, ESP_MATTER_KVS_NAMESPACE,
                                            NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open the node nvs_namespace");
        return err;
    }
    err = nvs_set_u16(handle, "min_uu_ep_id", node->min_unused_endpoint_id);
    nvs_commit(handle);
    nvs_close(handle);
    return err;
}

static esp_err_t read_min_unused_endpoint_id()
{
    if (!node || !esp_matter_started) {
        ESP_LOGE(TAG, "Node does not exist or esp_matter does not start");
        return ESP_ERR_INVALID_STATE;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, ESP_MATTER_KVS_NAMESPACE,
                                            NVS_READONLY, &handle);
    if (err == ESP_OK) {
        err = nvs_get_u16(handle, "min_uu_ep_id", &node->min_unused_endpoint_id);
        nvs_close(handle);
    }

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Cannot find minimum unused endpoint_id, try to find in the previous namespace");
        // Try to read the minimum unused endpoint_id from the previous node namespace.
        err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, "node", NVS_READONLY, &handle);
        if (err != ESP_OK) {
            ESP_LOGI(TAG, "Failed to open node namespace");
            return err;
        }
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

} /* node */

namespace cluster {
static int get_count(_cluster_t *current)
{
    int count = 0;
    while (current) {
        current = current->next;
        count++;
    }
    return count;
}
} /* cluster */

namespace command {
static int get_count(_command_t *current, int command_flag)
{
    int count = 0;
    while (current) {
        if (current->flags & command_flag) {
            count++;
        }
        current = current->next;
    }
    return count;
}
} /* command */

namespace event {
static int get_count(_event_t *current)
{
    int count = 0;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}
}

namespace attribute {

extern esp_err_t get_data_from_attr_val(esp_matter_attr_val_t *val, EmberAfAttributeType *attribute_type,
                                        uint16_t *attribute_size, uint8_t *value);

static int get_count(_attribute_t *current)
{
    int count = 0;
    while (current) {
        current = current->next;
        count++;
    }
    return count;
}

static esp_err_t free_default_value(attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_FAIL;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    /* Free value if data is more than 2 bytes or if it is min max attribute */
    if (current_attribute->flags & ATTRIBUTE_FLAG_MIN_MAX) {
        if (current_attribute->default_value_size > 2) {
            if (current_attribute->default_value.ptrToMinMaxValue->defaultValue.ptrToDefaultValue) {
                esp_matter_mem_free((void *)current_attribute->default_value.ptrToMinMaxValue->defaultValue.ptrToDefaultValue);
            }
            if (current_attribute->default_value.ptrToMinMaxValue->minValue.ptrToDefaultValue) {
                esp_matter_mem_free((void *)current_attribute->default_value.ptrToMinMaxValue->minValue.ptrToDefaultValue);
            }
            if (current_attribute->default_value.ptrToMinMaxValue->maxValue.ptrToDefaultValue) {
                esp_matter_mem_free((void *)current_attribute->default_value.ptrToMinMaxValue->maxValue.ptrToDefaultValue);
            }
        }
        esp_matter_mem_free((void *)current_attribute->default_value.ptrToMinMaxValue);
    } else if (current_attribute->default_value_size > 2) {
        if (current_attribute->default_value.ptrToDefaultValue) {
            esp_matter_mem_free((void *)current_attribute->default_value.ptrToDefaultValue);
        }
    }
    return ESP_OK;
}

static EmberAfDefaultAttributeValue get_default_value_from_data(esp_matter_attr_val_t *val,
                                                                EmberAfAttributeType attribute_type,
                                                                uint16_t attribute_size)
{
    EmberAfDefaultAttributeValue default_value = (uint16_t)0;
    uint8_t *value = (uint8_t *)esp_matter_mem_calloc(1, attribute_size);
    if (!value) {
        ESP_LOGE(TAG, "Could not allocate value buffer for default value");
        return default_value;
    }
    get_data_from_attr_val(val, &attribute_type, &attribute_size, value);

    if (attribute_size > 2) {
        /* Directly set the pointer */
        default_value = value;
    } else {
        /* This data is 2 bytes or less. This should be represented as uint16. Copy the bytes appropriately
        for 0 or 1 or 2 bytes to be converted to uint16. Then free the allocated buffer. */
        uint16_t int_value = 0;
        if (attribute_size == 2) {
            memcpy(&int_value, value, attribute_size);
        } else if (attribute_size == 1) {
            int_value = (uint16_t)*value;
        }
        default_value = int_value;
        esp_matter_mem_free(value);
    }
    return default_value;
}

static esp_err_t set_default_value_from_current_val(attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_FAIL;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    esp_matter_attr_val_t *val = &current_attribute->val;

    /* Get size */
    EmberAfAttributeType attribute_type = 0;
    uint16_t attribute_size = 0;
    get_data_from_attr_val(val, &attribute_type, &attribute_size, NULL);

    /* Get and set value */
    if (current_attribute->flags & ATTRIBUTE_FLAG_MIN_MAX) {
        EmberAfAttributeMinMaxValue *temp_value = (EmberAfAttributeMinMaxValue *)esp_matter_mem_calloc(1,
                                                                                sizeof(EmberAfAttributeMinMaxValue));
        if (!temp_value) {
            ESP_LOGE(TAG, "Could not allocate ptrToMinMaxValue for default value");
            return ESP_FAIL;
        }
        temp_value->defaultValue = get_default_value_from_data(val, attribute_type, attribute_size);
        temp_value->minValue = get_default_value_from_data(&current_attribute->bounds->min, attribute_type,
                                                           attribute_size);
        temp_value->maxValue = get_default_value_from_data(&current_attribute->bounds->max, attribute_type,
                                                           attribute_size);
        current_attribute->default_value.ptrToMinMaxValue = temp_value;
    } else if (attribute_size > 2) {
        EmberAfDefaultAttributeValue temp_value = get_default_value_from_data(val, attribute_type, attribute_size);
        current_attribute->default_value.ptrToDefaultValue = temp_value.ptrToDefaultValue;
    } else {
        EmberAfDefaultAttributeValue temp_value = get_default_value_from_data(val, attribute_type, attribute_size);
        current_attribute->default_value.defaultValue = temp_value.defaultValue;
    }
    current_attribute->default_value_size = attribute_size;
    return ESP_OK;
}
} /* attribute */

namespace endpoint {

static int get_next_index()
{
    uint16_t endpoint_id = 0;
    for (int index = 0; index < MAX_ENDPOINT_COUNT; index++) {
        endpoint_id = emberAfEndpointFromIndex(index);
        if (endpoint_id == kInvalidEndpointId) {
            return index;
        }
    }
    return 0xFFFF;
}

static esp_err_t disable(endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* Take lock if not already taken */
    lock::status_t lock_status = lock::chip_stack_lock(portMAX_DELAY);
    if (lock_status == lock::FAILED) {
        ESP_LOGE(TAG, "Could not get task context");
        return ESP_FAIL;
    }

    /* Remove endpoint */
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    int endpoint_index = emberAfGetDynamicIndexFromEndpoint(current_endpoint->endpoint_id);
    if (endpoint_index == 0xFFFF) {
        ESP_LOGE(TAG, "Could not find endpoint index");
        if (lock_status == lock::SUCCESS) {
            lock::chip_stack_unlock();
        }
        return ESP_FAIL;
    }
    emberAfClearDynamicEndpoint(endpoint_index);

    if (lock_status == lock::SUCCESS) {
        lock::chip_stack_unlock();
    }
    if (!(current_endpoint->endpoint_type)) {
        ESP_LOGE(TAG, "endpoint %" PRIu16 "'s endpoint_type is NULL", current_endpoint->endpoint_id);
        return ESP_ERR_INVALID_STATE;
    }
    /* Free all clusters */
    EmberAfEndpointType *endpoint_type = current_endpoint->endpoint_type;
    int cluster_count = endpoint_type->clusterCount;
    for (int cluster_index = 0; cluster_index < cluster_count; cluster_index++) {
        /* Free attributes */
        esp_matter_mem_free((void *)endpoint_type->cluster[cluster_index].attributes);
        /* Free commands */
        if (endpoint_type->cluster[cluster_index].acceptedCommandList) {
            esp_matter_mem_free((void *)endpoint_type->cluster[cluster_index].acceptedCommandList);
        }
        if (endpoint_type->cluster[cluster_index].generatedCommandList) {
            esp_matter_mem_free((void *)endpoint_type->cluster[cluster_index].generatedCommandList);
        }
        /* Free events */
        if (endpoint_type->cluster[cluster_index].eventList) {
            esp_matter_mem_free((void *)endpoint_type->cluster[cluster_index].eventList);
        }
    }
    esp_matter_mem_free((void *)endpoint_type->cluster);

    /* Free data versions */
    if (current_endpoint->data_versions_ptr) {
        esp_matter_mem_free(current_endpoint->data_versions_ptr);
        current_endpoint->data_versions_ptr = NULL;
    }

    /* Free device types */
    if (current_endpoint->device_types_ptr) {
        esp_matter_mem_free(current_endpoint->device_types_ptr);
        current_endpoint->device_types_ptr = NULL;
    }

    /* Free endpoint type */
    esp_matter_mem_free(endpoint_type);
    current_endpoint->endpoint_type = NULL;

    return ESP_OK;
}

esp_err_t enable(endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;

    /* Endpoint Type */
    EmberAfEndpointType *endpoint_type = (EmberAfEndpointType *)esp_matter_mem_calloc(1, sizeof(EmberAfEndpointType));
    if (!endpoint_type) {
        ESP_LOGE(TAG, "Couldn't allocate endpoint_type");
        /* goto cleanup is not used here to avoid 'crosses initialization' of data_versions below */
        return ESP_ERR_NO_MEM;
    }
    current_endpoint->endpoint_type = endpoint_type;

    /* Device types */
    EmberAfDeviceType *device_types_ptr = (EmberAfDeviceType *)esp_matter_mem_calloc(current_endpoint->device_type_count, sizeof(EmberAfDeviceType));
    if (!device_types_ptr) {
        ESP_LOGE(TAG, "Couldn't allocate device_types");
        esp_matter_mem_free(endpoint_type);
        current_endpoint->endpoint_type = NULL;
        /* goto cleanup is not used here to avoid 'crosses initialization' of device_types below */
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < current_endpoint->device_type_count; ++i) {
        device_types_ptr[i].deviceId = current_endpoint->device_type_ids[i];
        device_types_ptr[i].deviceVersion = current_endpoint->device_type_versions[i];
    }
    chip::Span<EmberAfDeviceType> device_types(device_types_ptr, current_endpoint->device_type_count);
    current_endpoint->device_types_ptr = device_types_ptr;

    /* Clusters */
    _cluster_t *cluster = current_endpoint->cluster_list;
    int cluster_count = cluster::get_count(cluster);
    int cluster_index = 0;

    DataVersion *data_versions_ptr = (DataVersion *)esp_matter_mem_calloc(1, cluster_count * sizeof(DataVersion));
    if (!data_versions_ptr) {
        ESP_LOGE(TAG, "Couldn't allocate data_versions");
        esp_matter_mem_free(data_versions_ptr);
        esp_matter_mem_free(endpoint_type);
        current_endpoint->data_versions_ptr = NULL;
        current_endpoint->endpoint_type = NULL;
        /* goto cleanup is not used here to avoid 'crosses initialization' of data_versions below */
        return ESP_ERR_NO_MEM;
    }
    chip::Span<chip::DataVersion> data_versions(data_versions_ptr, cluster_count);
    current_endpoint->data_versions_ptr = data_versions_ptr;

    /* Variables */
    /* This is needed to avoid 'crosses initialization' errors because of goto */
    esp_err_t err = ESP_OK;
    lock::status_t lock_status = lock::FAILED;
    CHIP_ERROR status = CHIP_NO_ERROR;
    EmberAfCluster *matter_clusters = NULL;
    _attribute_t *attribute = NULL;
    int attribute_count = 0;
    int attribute_index = 0;
    EmberAfAttributeMetadata *matter_attributes = NULL;
    CommandId *accepted_command_ids = NULL;
    CommandId *generated_command_ids = NULL;
    _command_t *command = NULL;
    int command_count = 0;
    int command_index = 0;
    int command_flag = COMMAND_FLAG_NONE;
    EventId *event_ids = NULL;
    _event_t *event = NULL;
    int event_count = 0;
    int event_index = 0;
    int endpoint_index = 0;

    matter_clusters = (EmberAfCluster *)esp_matter_mem_calloc(1, cluster_count * sizeof(EmberAfCluster));
    if (!matter_clusters) {
        ESP_LOGE(TAG, "Couldn't allocate matter_clusters");
        err = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    while (cluster) {
        /* Attributes */
        attribute = cluster->attribute_list;
        attribute_count = attribute::get_count(attribute);
        attribute_index = 0;
        matter_attributes = (EmberAfAttributeMetadata *)esp_matter_mem_calloc(1, attribute_count * sizeof(EmberAfAttributeMetadata));
        if (!matter_attributes) {
            if (attribute_count != 0) {
                ESP_LOGE(TAG, "Couldn't allocate matter_attributes");
                err = ESP_ERR_NO_MEM;
                break;
            }
        }

        while (attribute) {
            matter_attributes[attribute_index].attributeId = attribute->attribute_id;
            matter_attributes[attribute_index].mask = attribute->flags;
            matter_attributes[attribute_index].defaultValue = attribute->default_value;
            attribute::get_data_from_attr_val(&attribute->val, &matter_attributes[attribute_index].attributeType,
                                              &matter_attributes[attribute_index].size, NULL);

            /* The length is not fixed for string attribute, so set it to the max size (32) to avoid overflow issue 
             * when writing a longer string.
             */
            if (attribute->val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
                attribute->val.type  == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING) {
                matter_attributes[attribute_index].size = attribute->val.val.a.t;
            }

            matter_clusters[cluster_index].clusterSize += matter_attributes[attribute_index].size;
            attribute = attribute->next;
            attribute_index++;
        }

        /* Commands */
        command = NULL;
        command_count = 0;
        command_index = 0;
        command_flag = COMMAND_FLAG_NONE;
        accepted_command_ids = NULL;
        generated_command_ids = NULL;

        /* Client Generated Commands */
        command_flag = COMMAND_FLAG_ACCEPTED;
        command = cluster->command_list;
        command_count = command::get_count(command, command_flag);
        if (command_count > 0) {
            command_index = 0;
            accepted_command_ids = (CommandId *)esp_matter_mem_calloc(1, (command_count + 1) * sizeof(CommandId));
            if (!accepted_command_ids) {
                ESP_LOGE(TAG, "Couldn't allocate accepted_command_ids");
                err = ESP_ERR_NO_MEM;
                break;
            }
            while (command) {
                if (command->flags & command_flag) {
                    accepted_command_ids[command_index] = command->command_id;
                    command_index++;
                }
                command = command->next;
            }
            accepted_command_ids[command_index] = kInvalidCommandId;
        }

        /* Server Generated Commands */
        command_flag = COMMAND_FLAG_GENERATED;
        command = cluster->command_list;
        command_count = command::get_count(command, command_flag);
        if (command_count > 0) {
            command_index = 0;
            generated_command_ids = (CommandId *)esp_matter_mem_calloc(1, (command_count + 1) * sizeof(CommandId));
            if (!generated_command_ids) {
                ESP_LOGE(TAG, "Couldn't allocate generated_command_ids");
                err = ESP_ERR_NO_MEM;
                break;
            }
            while (command) {
                if (command->flags & command_flag) {
                    generated_command_ids[command_index] = command->command_id;
                    command_index++;
                }
                command = command->next;
            }
            generated_command_ids[command_index] = kInvalidCommandId;
        }

        /* Event */
        event = cluster->event_list;
        event_count = event::get_count(event);
        if (event_count > 0) {
            event_index = 0;
            event_ids = (EventId *)esp_matter_mem_calloc(1, (event_count + 1) * sizeof(EventId));
            if (!event_ids) {
                ESP_LOGE(TAG, "Couldn't allocate event_ids");
                err = ESP_ERR_NO_MEM;
                break;
            }
            while (event) {
                event_ids[event_index] = event->event_id;
                event_index++;
                event = event->next;
            }
            event_ids[event_index] = chip::kInvalidEventId;
        }

        /* Fill up the cluster */
        matter_clusters[cluster_index].clusterId = cluster->cluster_id;
        matter_clusters[cluster_index].attributes = matter_attributes;
        matter_clusters[cluster_index].attributeCount = attribute_count;
        matter_clusters[cluster_index].mask = cluster->flags;
        matter_clusters[cluster_index].functions = (EmberAfGenericClusterFunction *)cluster->function_list;
        matter_clusters[cluster_index].acceptedCommandList = accepted_command_ids;
        matter_clusters[cluster_index].generatedCommandList = generated_command_ids;
        matter_clusters[cluster_index].eventList = event_ids;
        matter_clusters[cluster_index].eventCount = event_count;

        /* Get next cluster */
        endpoint_type->endpointSize += matter_clusters[cluster_index].clusterSize;
        cluster = cluster->next;
        cluster_index++;

        /* This is to avoid double free in case of errors */
        matter_attributes = NULL;
        accepted_command_ids = NULL;
        generated_command_ids = NULL;
        event_ids = NULL;
    }
    if (err != ESP_OK) {
        goto cleanup;
    }

    endpoint_type->cluster = matter_clusters;
    endpoint_type->clusterCount = cluster_count;

    /* Take lock if not already taken */
    lock_status = lock::chip_stack_lock(portMAX_DELAY);
    if (lock_status == lock::FAILED) {
        ESP_LOGE(TAG, "Could not get task context");
        goto cleanup;
    }

    /* Add Endpoint */
    endpoint_index = endpoint::get_next_index();
    status = emberAfSetDynamicEndpoint(endpoint_index, current_endpoint->endpoint_id, endpoint_type, data_versions,
                                       device_types, current_endpoint->parent_endpoint_id);
    if (status != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Error adding dynamic endpoint %" PRIu16 ": %" CHIP_ERROR_FORMAT, current_endpoint->endpoint_id, status.Format());
        err = ESP_FAIL;
        if (lock_status == lock::SUCCESS) {
            lock::chip_stack_unlock();
        }
        goto cleanup;
    }
    if (lock_status == lock::SUCCESS) {
        lock::chip_stack_unlock();
    }
    ESP_LOGI(TAG, "Dynamic endpoint %" PRIu16 " added", current_endpoint->endpoint_id);
    return err;

cleanup:
    if (generated_command_ids) {
        esp_matter_mem_free(generated_command_ids);
    }
    if (accepted_command_ids) {
        esp_matter_mem_free(accepted_command_ids);
    }
    if (event_ids) {
        esp_matter_mem_free(event_ids);
    }
    if (matter_attributes) {
        esp_matter_mem_free(matter_attributes);
    }
    if (matter_clusters) {
        for (int cluster_index = 0; cluster_index < cluster_count; cluster_index++) {
            /* Free attributes */
            if (matter_clusters[cluster_index].attributes) {
                esp_matter_mem_free((void *)matter_clusters[cluster_index].attributes);
            }
            /* Free commands */
            if (matter_clusters[cluster_index].acceptedCommandList) {
                esp_matter_mem_free((void *)matter_clusters[cluster_index].acceptedCommandList);
            }
            if (matter_clusters[cluster_index].generatedCommandList) {
                esp_matter_mem_free((void *)matter_clusters[cluster_index].generatedCommandList);
            }
            /* Free events */
            if (matter_clusters[cluster_index].eventList) {
                esp_matter_mem_free((void *)matter_clusters[cluster_index].eventList);
            }
        }
        esp_matter_mem_free(matter_clusters);
    }
    if (data_versions_ptr) {
        esp_matter_mem_free(data_versions_ptr);
        current_endpoint->data_versions_ptr = NULL;
    }
    if (device_types_ptr) {
        esp_matter_mem_free(device_types_ptr);
        current_endpoint->device_types_ptr = NULL;
    }
    if (endpoint_type) {
        esp_matter_mem_free(endpoint_type);
        current_endpoint->endpoint_type = NULL;
    }
    return err;
}

static esp_err_t enable_all()
{
    node_t *node = node::get();
    if (!node) {
        /* Not returning error, since the node will not be initialized for application using the data model from zap */
        return ESP_OK;
    }

    endpoint_t *endpoint = get_first(node);
    while (endpoint) {
        enable(endpoint);
        endpoint = get_next(endpoint);
    }
    return ESP_OK;
}
} /* endpoint */

namespace lock {
#define DEFAULT_TICKS (500 / portTICK_PERIOD_MS) /* 500 ms in ticks */
status_t chip_stack_lock(uint32_t ticks_to_wait)
{
#if CHIP_STACK_LOCK_TRACKING_ENABLED
    if (PlatformMgr().IsChipStackLockedByCurrentThread()) {
        return ALREADY_TAKEN;
    }
#endif
    if (ticks_to_wait == portMAX_DELAY) {
        /* Special handling for max delay */
        PlatformMgr().LockChipStack();
        return SUCCESS;
    }
    uint32_t ticks_remaining = ticks_to_wait;
    uint32_t ticks = DEFAULT_TICKS;
    while (ticks_remaining > 0) {
        if (PlatformMgr().TryLockChipStack()) {
            return SUCCESS;
        }
        ticks = ticks_remaining < DEFAULT_TICKS ? ticks_remaining : DEFAULT_TICKS;
        ticks_remaining -= ticks;
        ESP_LOGI(TAG, "Did not get lock yet. Retrying...");
        vTaskDelay(ticks);
    }
    ESP_LOGE(TAG, "Could not get lock");
    return FAILED;
}

esp_err_t chip_stack_unlock()
{
    PlatformMgr().UnlockChipStack();
    return ESP_OK;
}
} /* lock */

#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
static void deinit_ble_if_commissioned(void)
{
#if CONFIG_BT_ENABLED && CONFIG_USE_BLE_ONLY_FOR_COMMISSIONING
        if(chip::Server::GetInstance().GetFabricTable().FabricCount() > 0) {
            esp_err_t err = ESP_OK;
#if CONFIG_BT_NIMBLE_ENABLED
            if (!ble_hs_is_enabled()) {
                ESP_LOGI(TAG, "BLE already deinited");
                return;
            }

            if (nimble_port_stop() != 0) {
                ESP_LOGE(TAG, "nimble_port_stop() failed");
                return;
            }
            nimble_port_deinit();
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
            err = esp_nimble_hci_and_controller_deinit();
#endif
#endif /* CONFIG_BT_NIMBLE_ENABLED */
#if CONFIG_IDF_TARGET_ESP32
            err |= esp_bt_mem_release(ESP_BT_MODE_BTDM);
#elif CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32H2 \
       || CONFIG_IDF_TARGET_ESP32C6
            err |= esp_bt_mem_release(ESP_BT_MODE_BLE);
#endif
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "BLE deinit failed");
                return;
            }
            ESP_LOGI(TAG, "BLE deinit successful and memory reclaimed");
            PostEvent(chip::DeviceLayer::DeviceEventType::kBLEDeinitialized);
        }
#endif /* CONFIG_BT_ENABLED && CONFIG_USE_BLE_ONLY_FOR_COMMISSIONING */
}

static void esp_matter_chip_init_task(intptr_t context)
{
    TaskHandle_t task_to_notify = reinterpret_cast<TaskHandle_t>(context);
    static chip::CommonCaseDeviceServerInitParams initParams;
    initParams.InitializeStaticResourcesBeforeServerInit();
    initParams.appDelegate = &s_app_delegate;
    CHIP_ERROR ret = chip::Server::GetInstance().GetFabricTable().AddFabricDelegate(&s_fabric_delegate);
    if (ret != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "Failed to add fabric delegate, err:%" CHIP_ERROR_FORMAT, ret.Format());
    }
    chip::Server::GetInstance().Init(initParams);

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    // If Thread is Provisioned, publish the dns service
    if (chip::DeviceLayer::ConnectivityMgr().IsThreadProvisioned() &&
        (chip::Server::GetInstance().GetFabricTable().FabricCount() != 0)) {
        chip::app::DnssdServer::Instance().StartServer();
    }
#endif
    if (endpoint::enable_all() != ESP_OK) {
        ESP_LOGE(TAG, "Enable all endpoints failure");
    }
    // The following two events can't be recorded when we start the server because the endpoints are not enabled.
    // TODO: Find a better way to record the events which should be recorded in matter server init
    // Record start up event in basic information cluster.
    PlatformMgr().HandleServerStarted();
    // Record boot reason evnet in general diagnostics cluster.
    chip::app::Clusters::GeneralDiagnostics::BootReasonEnum bootReason;
    if (GetDiagnosticDataProvider().GetBootReason(bootReason) == CHIP_NO_ERROR) {
        chip::app::Clusters::GeneralDiagnosticsServer::Instance().OnDeviceReboot(bootReason);
    }
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
    {
        static chip::app::Clusters::NetworkCommissioning::Instance sWiFiNetworkCommissioningInstance(0,
                                            &(chip::DeviceLayer::NetworkCommissioning::ESPWiFiDriver::GetInstance()));
        sWiFiNetworkCommissioningInstance.Init();
    }
#endif
#if CHIP_DEVICE_CONFIG_ENABLE_ETHERNET
    {
        static chip::app::Clusters::NetworkCommissioning::Instance sEthernetNetworkCommissioningInstance(0,
                                            &(chip::DeviceLayer::NetworkCommissioning::ESPEthernetDriver::GetInstance()));
        sEthernetNetworkCommissioningInstance.Init();
    }
#endif
    deinit_ble_if_commissioned();
    xTaskNotifyGive(task_to_notify);
}
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

static void device_callback_internal(const ChipDeviceEvent * event, intptr_t arg)
{
    switch (event->Type)
    {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI || CHIP_DEVICE_CONFIG_ENABLE_ETHERNET
        if (event->InterfaceIpAddressChanged.Type == chip::DeviceLayer::InterfaceIpChangeType::kIpV6_Assigned ||
                event->InterfaceIpAddressChanged.Type == chip::DeviceLayer::InterfaceIpChangeType::kIpV4_Assigned) {
            chip::app::DnssdServer::Instance().StartServer();
        }
#endif
        break;
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    case chip::DeviceLayer::DeviceEventType::kDnssdInitialized:
        esp_matter_ota_requestor_start();
        /* Initialize binding manager */
        client::binding_manager_init();
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning Complete");
        break;

    case chip::DeviceLayer::DeviceEventType::kCHIPoBLEConnectionClosed:
        ESP_LOGI(TAG, "BLE Disconnected");
        deinit_ble_if_commissioned();
        break;
#endif
    default:
        break;
    }
}

static esp_err_t chip_init(event_callback_t callback, intptr_t callback_arg)
{
    if (chip::Platform::MemoryInit() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to initialize CHIP memory pool");
        return ESP_ERR_NO_MEM;
    }
    if (PlatformMgr().InitChipStack() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to initialize CHIP stack");
        return ESP_FAIL;
    }

    setup_providers();
    // ConnectivityMgr().SetWiFiAPMode(ConnectivityManager::kWiFiAPMode_Enabled);
    if (PlatformMgr().StartEventLoopTask() != CHIP_NO_ERROR) {
        chip::Platform::MemoryShutdown();
        ESP_LOGE(TAG, "Failed to launch Matter main task");
        return ESP_FAIL;
    }
    PlatformMgr().AddEventHandler(device_callback_internal, static_cast<intptr_t>(NULL));
    if(callback) {
       PlatformMgr().AddEventHandler(callback, callback_arg);
    }
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    if (ThreadStackMgr().InitThreadStack() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to initialize Thread stack");
        return ESP_FAIL;
    }
#if CHIP_CONFIG_ENABLE_ICD_SERVER
    if (ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_SleepyEndDevice) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to set the Thread device type");
        return ESP_FAIL;
    }

#elif CHIP_DEVICE_CONFIG_THREAD_FTD
    if (ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_Router) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to set the Thread device type");
        return ESP_FAIL;
    }
#else
    if (ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_MinimalEndDevice) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to set the Thread device type");
        return ESP_FAIL;
    }
#endif
    if (ThreadStackMgr().StartThreadTask() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to launch Thread task");
        return ESP_FAIL;
    }
#endif // CHIP_DEVICE_CONFIG_ENABLE_THREAD

#if CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    PlatformMgr().ScheduleWork(esp_matter_chip_init_task, reinterpret_cast<intptr_t>(xTaskGetCurrentTaskHandle()));
    // Wait for the matter stack to be initialized
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    return ESP_OK;
}

esp_err_t start(event_callback_t callback, intptr_t callback_arg)
{
    if (esp_matter_started) {
        ESP_LOGE(TAG, "esp_matter has started");
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t err = esp_event_loop_create_default();

    // In case create event loop returns ESP_ERR_INVALID_STATE it is not necessary to fail startup
    // as of it means that default event loop is already initialized and no additional actions should be done.
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Error create default event loop");
        return err;
    }
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
    if (chip::DeviceLayer::Internal::ESP32Utils::InitWiFiStack() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Error initializing Wi-Fi stack");
        return ESP_FAIL;
    }
#endif
    esp_matter_ota_requestor_init();

    err = chip_init(callback, callback_arg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing matter");
        return err;
    }
    esp_matter_started = true;
    err = node::read_min_unused_endpoint_id();
    // If the min_unused_endpoint_id is not found, we will write the current min_unused_endpoint_id in nvs.
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        err = node::store_min_unused_endpoint_id();
    }
    return err;
}

esp_err_t factory_reset()
{
    esp_err_t err = ESP_OK;
    node_t *node = node::get();
    if (node) {
        /* ESP Matter data model is used. Erase all the data that we have added in nvs. */
        nvs_handle_t handle;
        err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, ESP_MATTER_KVS_NAMESPACE, NVS_READWRITE, &handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open esp_matter nvs partition ");
        } else {
            err = nvs_erase_all(handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to erase esp_matter nvs namespace");
            } else {
                nvs_commit(handle);
            }
            nvs_close(handle);
        }
    }

    /* Submodule factory reset. This also restarts after completion. */
    ConfigurationMgr().InitiateFactoryReset();
    return err;
}

namespace attribute {
attribute_t *create(cluster_t *cluster, uint32_t attribute_id, uint8_t flags, esp_matter_attr_val_t val)
{
    /* Find */
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    attribute_t *existing_attribute = get(cluster, attribute_id);
    if (existing_attribute) {
        ESP_LOGW(TAG, "Attribute 0x%08" PRIX32 " on cluster 0x%08" PRIX32 " already exists. Not creating again.", attribute_id,
                 current_cluster->cluster_id);
        return existing_attribute;
    }

    /* Allocate */
    _attribute_t *attribute = (_attribute_t *)esp_matter_mem_calloc(1, sizeof(_attribute_t));
    if (!attribute) {
        ESP_LOGE(TAG, "Couldn't allocate _attribute_t");
        return NULL;
    }

    /* Set */
    attribute->attribute_id = attribute_id;
    attribute->cluster_id = current_cluster->cluster_id;
    attribute->endpoint_id = current_cluster->endpoint_id;
    attribute->flags = flags;
    attribute->flags |= ATTRIBUTE_FLAG_EXTERNAL_STORAGE;

    // After reboot, string and array are treated as Invalid. So need to store val.type and size of attribute value.
    attribute->val.type = val.type;
    if (val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        attribute->val.val.a.s = val.val.a.s;
        attribute->val.val.a.n = val.val.a.n;
        attribute->val.val.a.t = val.val.a.t;
    }

    bool attribute_updated = false;
    if (attribute->flags & ATTRIBUTE_FLAG_NONVOLATILE) {
        // Lets directly read into attribute->val so that we don't have to set the attribute value again.
        esp_err_t err = get_val_from_nvs(attribute->endpoint_id, attribute->cluster_id, attribute->attribute_id,
                                            attribute->val);
        if (err == ESP_OK) {
            attribute_updated = true;
        }
    }
    if (!attribute_updated) {
        set_val((attribute_t *)attribute, &val);
    }

    set_default_value_from_current_val((attribute_t *)attribute);

    /* Add */
    _attribute_t *previous_attribute = NULL;
    _attribute_t *current_attribute = current_cluster->attribute_list;
    while (current_attribute) {
        previous_attribute = current_attribute;
        current_attribute = current_attribute->next;
    }
    if (previous_attribute == NULL) {
        current_cluster->attribute_list = attribute;
    } else {
        previous_attribute->next = attribute;
    }

    return (attribute_t *)attribute;
}

static esp_err_t destroy(attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    /* Default value needs to be deleted first since it uses the current val. */
    free_default_value(attribute);

    /* Delete val here, if required */
    if (current_attribute->val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        /* Free buf */
        if (current_attribute->val.val.a.b) {
            esp_matter_mem_free(current_attribute->val.val.a.b);
        }
    }

    /* Free bounds */
    if (current_attribute->bounds) {
        esp_matter_mem_free(current_attribute->bounds);
    }

    /* Erase the persistent data */
    if (current_attribute->flags & ATTRIBUTE_FLAG_NONVOLATILE) {
        erase_val_in_nvs(current_attribute->endpoint_id, current_attribute->cluster_id, current_attribute->attribute_id);
    }

    /* Free */
    esp_matter_mem_free(current_attribute);
    return ESP_OK;
}

attribute_t *get(cluster_t *cluster, uint32_t attribute_id)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    _attribute_t *current_attribute = (_attribute_t *)current_cluster->attribute_list;
    while (current_attribute) {
        if (current_attribute->attribute_id == attribute_id) {
            break;
        }
        current_attribute = current_attribute->next;
    }
    return (attribute_t *)current_attribute;
}

attribute_t *get_first(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return (attribute_t *)current_cluster->attribute_list;
}

attribute_t *get_next(attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return NULL;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    return (attribute_t *)current_attribute->next;
}

uint32_t get_id(attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return kInvalidAttributeId;
    }
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
                     current_attribute->val);
}

esp_err_t set_val(attribute_t *attribute, esp_matter_attr_val_t *val)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_FAIL;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    if (val->type == ESP_MATTER_VAL_TYPE_CHAR_STRING || val->type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        val->type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING || val->type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
        val->type == ESP_MATTER_VAL_TYPE_ARRAY) {
        /* Free old buf */
        if (current_attribute->val.val.a.b) {
            esp_matter_mem_free(current_attribute->val.val.a.b);
            current_attribute->val.val.a.b = NULL;
        }
        if (val->val.a.s > 0) {
            /* Alloc new buf */
            uint8_t *new_buf = (uint8_t *)esp_matter_mem_calloc(1, val->val.a.s);
            if (!new_buf) {
                ESP_LOGE(TAG, "Could not allocate new buffer");
                return ESP_ERR_NO_MEM;
            }
            /* Copy to new buf and assign */
            memcpy(new_buf, val->val.a.b, val->val.a.s);
            current_attribute->val.val.a.b = new_buf;
            current_attribute->val.val.a.s = val->val.a.s;
            current_attribute->val.val.a.n = val->val.a.n;
            current_attribute->val.val.a.t = val->val.a.t;
        } else {
            ESP_LOGD(TAG, "Set val called with string with size 0");
        }
    } else {
        memcpy((void *)&current_attribute->val, (void *)val, sizeof(esp_matter_attr_val_t));
    }

    if (current_attribute->flags & ATTRIBUTE_FLAG_NONVOLATILE) {
        if (current_attribute->flags & ATTRIBUTE_FLAG_DEFERRED) {
            if (!chip::DeviceLayer::SystemLayer().IsTimerActive(deferred_attribute_write, current_attribute)) {
                auto & system_layer = chip::DeviceLayer::SystemLayer();
                system_layer.StartTimer(chip::System::Clock::Milliseconds16(k_deferred_attribute_persistence_time_ms),
                                        deferred_attribute_write, current_attribute);
            }
        } else {
            store_val_in_nvs(current_attribute->endpoint_id, current_attribute->cluster_id,
                             current_attribute->attribute_id, current_attribute->val);
        }
    }
    return ESP_OK;
}

esp_err_t get_val(attribute_t *attribute, esp_matter_attr_val_t *val)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    memcpy((void *)val, (void *)&current_attribute->val, sizeof(esp_matter_attr_val_t));
    return ESP_OK;
}

esp_err_t add_bounds(attribute_t *attribute, esp_matter_attr_val_t min, esp_matter_attr_val_t max)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;

    /* Check if bounds can be set */
    if (current_attribute->val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        ESP_LOGE(TAG, "Bounds cannot be set for string/array type attributes");
        return ESP_ERR_INVALID_ARG;
    }
    if ((current_attribute->val.type != min.type) || (current_attribute->val.type != max.type)) {
        ESP_LOGE(TAG, "Cannot set bounds because of val type mismatch: expected: %d, min: %d, max: %d",
                 current_attribute->val.type, min.type, max.type);
        return ESP_ERR_INVALID_ARG;
    }

    /* Free the default value before setting the new bounds */
    free_default_value(attribute);

    /* Allocate and set */
    current_attribute->bounds = (esp_matter_attr_bounds_t *)esp_matter_mem_calloc(1, sizeof(esp_matter_attr_bounds_t));
    if (!current_attribute->bounds) {
        ESP_LOGE(TAG, "Could not allocate bounds");
        return ESP_ERR_NO_MEM;
    }
    memcpy((void *)&current_attribute->bounds->min, (void *)&min, sizeof(esp_matter_attr_val_t));
    memcpy((void *)&current_attribute->bounds->max, (void *)&max, sizeof(esp_matter_attr_val_t));
    current_attribute->flags |= ATTRIBUTE_FLAG_MIN_MAX;

    /* Set the default value again after setting the bounds and the flag */
    set_default_value_from_current_val(attribute);
    return ESP_OK;
}

esp_matter_attr_bounds_t *get_bounds(attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return NULL;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    return current_attribute->bounds;
}

uint16_t get_flags(attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return 0;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    return current_attribute->flags;
}

esp_err_t set_override_callback(attribute_t *attribute, callback_t callback)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    if (current_attribute->val.type == ESP_MATTER_VAL_TYPE_ARRAY ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        current_attribute->val.type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING) {
        // The override callback might allocate memory and we have no way to free the memory
        // TODO: Add memory-safe override callback for these attribute types
        ESP_LOGE(TAG, "Cannot set override callback for attribute 0x%" PRIX32 " on cluster 0x%" PRIX32,
                 current_attribute->attribute_id, current_attribute->cluster_id);
        return ESP_ERR_NOT_SUPPORTED;
    }
    current_attribute->override_callback = callback;
    current_attribute->flags |= ATTRIBUTE_FLAG_OVERRIDE;
    return ESP_OK;
}

callback_t get_override_callback(attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return NULL;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    return current_attribute->override_callback;
}

esp_err_t set_deferred_persistence(attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _attribute_t *current_attribute = (_attribute_t *)attribute;
    if (!(current_attribute->flags & ATTRIBUTE_FLAG_NONVOLATILE)) {
        ESP_LOGE(TAG, "Attribute should be non-volatile to set a deferred persistence time");
        return ESP_ERR_INVALID_ARG;
    }
    current_attribute->flags |= ATTRIBUTE_FLAG_DEFERRED;
    return ESP_OK;
}

} /* attribute */

namespace command {
command_t *create(cluster_t *cluster, uint32_t command_id, uint8_t flags, callback_t callback)
{
    /* Find */
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    command_t *existing_command = get(cluster, command_id, flags);
    if (existing_command) {
        ESP_LOGW(TAG, "Command 0x%08" PRIX32 " on cluster 0x%08" PRIX32 " already exists. Not creating again.", command_id,
                 current_cluster->cluster_id);
        return existing_command;
    }

    /* Allocate */
    _command_t *command = (_command_t *)esp_matter_mem_calloc(1, sizeof(_command_t));
    if (!command) {
        ESP_LOGE(TAG, "Couldn't allocate _command_t");
        return NULL;
    }

    /* Set */
    command->command_id = command_id;
    command->flags = flags;
    command->callback = callback;
    command->user_callback = NULL;

    /* Add */
    _command_t *previous_command = NULL;
    _command_t *current_command = current_cluster->command_list;
    while (current_command) {
        previous_command = current_command;
        current_command = current_command->next;
    }
    if (previous_command == NULL) {
        current_cluster->command_list = command;
    } else {
        previous_command->next = command;
    }

    return (command_t *)command;
}

static esp_err_t destroy(command_t *command)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _command_t *current_command = (_command_t *)command;

    /* Free */
    esp_matter_mem_free(current_command);
    return ESP_OK;
}

command_t *get(cluster_t *cluster, uint32_t command_id, uint16_t flags)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return (command_t *)current_cluster->command_list;
}

command_t *get_next(command_t *command)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return NULL;
    }
    _command_t *current_command = (_command_t *)command;
    return (command_t *)current_command->next;
}

uint32_t get_id(command_t *command)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return kInvalidCommandId;
    }
    _command_t *current_command = (_command_t *)command;
    return current_command->command_id;
}

callback_t get_callback(command_t *command)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return NULL;
    }
    _command_t *current_command = (_command_t *)command;
    return current_command->callback;
}

callback_t get_user_callback(command_t *command)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return NULL;
    }
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
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return 0;
    }
    _command_t *current_command = (_command_t *)command;
    return current_command->flags;
}

} /* command */

namespace event {

event_t *create(cluster_t *cluster, uint32_t event_id)
{
    /* Find */
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    event_t *existing_event = get(cluster, event_id);
    if (existing_event) {
        ESP_LOGW(TAG, "Event 0x%08" PRIX32 " on cluster 0x%08" PRIX32 " already exists. Not creating again.", event_id,
                 current_cluster->cluster_id);
        return existing_event;
    }

    /* Allocate */
    _event_t *event = (_event_t *)esp_matter_mem_calloc(1, sizeof(_event_t));
    if (!event) {
        ESP_LOGE(TAG, "Couldn't allocate _event_t");
        return NULL;
    }

    /* Set */
    event->event_id = event_id;

    /* Add */
    _event_t *previous_event = NULL;
    _event_t *current_event = current_cluster->event_list;
    while (current_event) {
        previous_event = current_event;
        current_event = current_event->next;
    }
    if (previous_event == NULL) {
        current_cluster->event_list = event;
    } else {
        previous_event->next = event;
    }

    return (event_t *)event;
}

static esp_err_t destroy(event_t *event)
{
    if (!event) {
        ESP_LOGE(TAG, "Event cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _event_t *current_event = (_event_t *)event;

    /* Free */
    esp_matter_mem_free(current_event);
    return ESP_OK;
}

event_t *get(cluster_t *cluster, uint32_t event_id)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return (event_t *)current_cluster->event_list;
}

event_t *get_next(event_t *event)
{
    if (!event) {
        ESP_LOGE(TAG, "Event cannot be NULL");
        return NULL;
    }
    _event_t *current_event = (_event_t *)event;
    return (event_t *)current_event->next;
}

uint32_t get_id(event_t *event)
{
    if (!event) {
        ESP_LOGE(TAG, "Event cannot be NULL");
        return chip::kInvalidEventId;
    }
    _event_t *current_event = (_event_t *)event;
    return current_event->event_id;
}

} /* event */


namespace cluster {

cluster_t *create(endpoint_t *endpoint, uint32_t cluster_id, uint8_t flags)
{
    /* Find */
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    if (!(flags & CLUSTER_FLAG_SERVER) && !(flags & CLUSTER_FLAG_CLIENT)) {
        ESP_LOGE(TAG, "Server or client cluster flag not set");
        return NULL;
    }
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    cluster_t *existing_cluster = get(endpoint, cluster_id);
    if (existing_cluster) {
        /* If a server already exists, do not create it again */
        _cluster_t *_existing_cluster = (_cluster_t *)existing_cluster;
        if ((_existing_cluster->flags & CLUSTER_FLAG_SERVER) && (flags & CLUSTER_FLAG_SERVER)) {
            ESP_LOGW(TAG, "Server Cluster 0x%08" PRIX32 " on endpoint 0x%04" PRIx16 " already exists. Not creating again.", cluster_id,
                     current_endpoint->endpoint_id);
            return existing_cluster;
        }

        /* If a client already exists, do not create it again */
        if ((_existing_cluster->flags & CLUSTER_FLAG_CLIENT) && (flags & CLUSTER_FLAG_CLIENT)) {
            ESP_LOGW(TAG, "Client Cluster 0x%08" PRIX32 " on endpoint 0x%04" PRIx16 " already exists. Not creating again.", cluster_id,
                     current_endpoint->endpoint_id);
            return existing_cluster;
        }

        /* The cluster already exists, but is of a different type. Just update the 'Set' part from below. */
        ESP_LOGI(TAG, "Cluster 0x%08" PRIX32 " on endpoint 0x%04" PRIx16 " already exists. Updating values.", cluster_id,
                 current_endpoint->endpoint_id);
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

    /* Add */
    _cluster_t *previous_cluster = NULL;
    _cluster_t *current_cluster = current_endpoint->cluster_list;
    while (current_cluster) {
        previous_cluster = current_cluster;
        current_cluster = current_cluster->next;
    }
    if (previous_cluster == NULL) {
        current_endpoint->cluster_list = cluster;
    } else {
        previous_cluster->next = cluster;
    }

    return (cluster_t *)cluster;
}

static esp_err_t destroy(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;

    /* Parse and delete all commands */
    _command_t *command = current_cluster->command_list;
    while (command) {
        _command_t *next_command = command->next;
        command::destroy((command_t *)command);
        command = next_command;
    }

    /* Parse and delete all attributes */
    _attribute_t *attribute = current_cluster->attribute_list;
    while (attribute) {
        _attribute_t *next_attribute = attribute->next;
        attribute::destroy((attribute_t *)attribute);
        attribute = next_attribute;
    }

    /* Parse and delete all events */
    _event_t *event = current_cluster->event_list;
    while (event) {
        _event_t *next_event = event->next;
        event::destroy((event_t *)event);
        event = next_event;
    }

    /* Free */
    esp_matter_mem_free(current_cluster);
    return ESP_OK;
}

cluster_t *get(endpoint_t *endpoint, uint32_t cluster_id)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
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

cluster_t *get_first(endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return (cluster_t *)current_endpoint->cluster_list;
}

cluster_t *get_next(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return (cluster_t *)current_cluster->next;
}

uint32_t get_id(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return kInvalidClusterId;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return current_cluster->cluster_id;
}

esp_err_t set_plugin_server_init_callback(cluster_t *cluster, plugin_server_init_callback_t callback)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    current_cluster->plugin_server_init_callback = callback;
    return ESP_OK;
}

plugin_server_init_callback_t get_plugin_server_init_callback(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    return current_cluster->plugin_server_init_callback;
}

esp_err_t add_function_list(cluster_t *cluster, const function_generic_t *function_list, int function_flags)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _cluster_t *current_cluster = (_cluster_t *)cluster;
    current_cluster->function_list = function_list;
    current_cluster->flags |= function_flags;
    return ESP_OK;
}

} /* cluster */

namespace endpoint {

endpoint_t *create(node_t *node, uint8_t flags, void *priv_data)
{
    /* Find */
    if (!node) {
        ESP_LOGE(TAG, "Node cannot be NULL");
        return NULL;
    }
    _node_t *current_node = (_node_t *)node;

    if (get_count(node) >= CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT) {
        ESP_LOGE(TAG, "Dynamic endpoint count cannot be greater than CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT:%u",
                CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT);
        return NULL;
    }

    /* Allocate */
    _endpoint_t *endpoint = (_endpoint_t *)esp_matter_mem_calloc(1, sizeof(_endpoint_t));
    if (!endpoint) {
        ESP_LOGE(TAG, "Couldn't allocate _endpoint_t");
        return NULL;
    }

    /* Set */
    endpoint->endpoint_id = current_node->min_unused_endpoint_id++;
    endpoint->device_type_count = 0;
    endpoint->parent_endpoint_id = chip::kInvalidEndpointId;
    endpoint->flags = flags;
    endpoint->priv_data = priv_data;

    /* Store */
    if (esp_matter_started) {
        node::store_min_unused_endpoint_id();
    }

    /* Add */
    _endpoint_t *previous_endpoint = NULL;
    _endpoint_t *current_endpoint = current_node->endpoint_list;
    while (current_endpoint) {
        previous_endpoint = current_endpoint;
        current_endpoint = current_endpoint->next;
    }
    if (previous_endpoint == NULL) {
        current_node->endpoint_list = endpoint;
    } else {
        previous_endpoint->next = endpoint;
    }

    return (endpoint_t *)endpoint;
}

endpoint_t *resume(node_t *node, uint8_t flags, uint16_t endpoint_id, void *priv_data)
{
    /* Find */
    if (!node) {
        ESP_LOGE(TAG, "Node cannot be NULL");
        return NULL;
    }
    _node_t *current_node = (_node_t *)node;
    _endpoint_t *previous_endpoint = NULL;
    _endpoint_t *current_endpoint = current_node->endpoint_list;
    while (current_endpoint) {
        if (current_endpoint->endpoint_id == endpoint_id) {
            ESP_LOGE(TAG, "Could not resume an endpoint that has been added to the node");
            return NULL;
        }
        previous_endpoint = current_endpoint;
        current_endpoint = current_endpoint->next;
    }

    /* Check */
    if (endpoint_id >= current_node->min_unused_endpoint_id) {
        ESP_LOGE(TAG, "The endpoint_id of the resumed endpoint should have been used");
        return NULL;
    }

     /* Allocate */
     _endpoint_t *endpoint = (_endpoint_t *)esp_matter_mem_calloc(1, sizeof(_endpoint_t));
     if (!endpoint) {
         ESP_LOGE(TAG, "Couldn't allocate _endpoint_t");
         return NULL;
     }

     /* Set */
     endpoint->endpoint_id = endpoint_id;
     endpoint->device_type_count = 0;
     endpoint->flags = flags;
     endpoint->priv_data = priv_data;

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
    if (!node || !endpoint) {
        ESP_LOGE(TAG, "Node or endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _node_t *current_node = (_node_t *)node;
    _endpoint_t *_endpoint = (_endpoint_t *)endpoint;

    if (!(_endpoint->flags & ENDPOINT_FLAG_DESTROYABLE)) {
        ESP_LOGE(TAG, "This endpoint cannot be deleted since the ENDPOINT_FLAG_DESTROYABLE is not set");
        return ESP_FAIL;
    }

    /* Find current endpoint and remove from list */
    _endpoint_t *current_endpoint = current_node->endpoint_list;
    _endpoint_t *previous_endpoint = NULL;
    while (current_endpoint) {
        if (current_endpoint == _endpoint) {
            break;
        }
        previous_endpoint = current_endpoint;
        current_endpoint = current_endpoint->next;
    }
    if (current_endpoint == NULL) {
        ESP_LOGE(TAG, "Could not find the endpoint to delete");
        return ESP_FAIL;
    }
    if (previous_endpoint == NULL) {
        current_node->endpoint_list = current_endpoint->next;
    } else {
        previous_endpoint->next = current_endpoint->next;
    }

    /* Disable */
    disable(endpoint);

    /* Parse and delete all clusters */
    _cluster_t *cluster = current_endpoint->cluster_list;
    while (cluster) {
        _cluster_t *next_cluster = cluster->next;
        cluster::destroy((cluster_t *)cluster);
        cluster = next_cluster;
    }

    /* Free */
    esp_matter_mem_free(current_endpoint);
    return ESP_OK;
}

endpoint_t *get(node_t *node, uint16_t endpoint_id)
{
    if (!node) {
        ESP_LOGE(TAG, "Node cannot be NULL");
        return NULL;
    }
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

endpoint_t *get_first(node_t *node)
{
    if (!node) {
        ESP_LOGE(TAG, "Node cannot be NULL");
        return NULL;
    }
    _node_t *current_node = (_node_t *)node;
    return (endpoint_t *)current_node->endpoint_list;
}

endpoint_t *get_next(endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return (endpoint_t *)current_endpoint->next;
}

uint16_t get_count(node_t *node)
{
    if (!node) {
        ESP_LOGE(TAG, "Node cannot be NULL");
        return 0;
    }
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
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return kInvalidEndpointId;
    }
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return current_endpoint->endpoint_id;
}

esp_err_t add_device_type(endpoint_t *endpoint, uint32_t device_type_id, uint8_t device_type_version)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    if (current_endpoint->device_type_count >= ESP_MATTER_MAX_DEVICE_TYPE_COUNT) {
        ESP_LOGE(TAG, "Could not add a new device type to the endpoint");
        return ESP_FAIL;
    }
    current_endpoint->device_type_ids[current_endpoint->device_type_count] = device_type_id;
    current_endpoint->device_type_versions[current_endpoint->device_type_count] = device_type_version;
    current_endpoint->device_type_count++;
    return ESP_OK;
}

uint32_t *get_device_type_ids(endpoint_t *endpoint, uint8_t *device_type_count_ptr)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    if (!device_type_count_ptr) {
        ESP_LOGE(TAG, "device type count pointer cannot be NULL");
        return NULL;
    }
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    *device_type_count_ptr = current_endpoint->device_type_count;
    return current_endpoint->device_type_ids;
}

uint8_t *get_device_type_versions(endpoint_t *endpoint, uint8_t *device_type_count_ptr)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    if (!device_type_count_ptr) {
        ESP_LOGE(TAG, "device type count pointer cannot be NULL");
        return NULL;
    }
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    *device_type_count_ptr = current_endpoint->device_type_count;
    return current_endpoint->device_type_versions;
}

esp_err_t set_parent_endpoint(endpoint_t *endpoint, endpoint_t *parent_endpoint)
{
    if (!endpoint || !parent_endpoint) {
        ESP_LOGE(TAG, "Endpoint or parent_endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    _endpoint_t *current_parent_endpoint = (_endpoint_t *)parent_endpoint;
    current_endpoint->parent_endpoint_id = current_parent_endpoint->endpoint_id;
    return ESP_OK;
}

void *get_priv_data(uint16_t endpoint_id)
{
    node_t *node = node::get();
    if (!node) {
        /* This is not an error, since the node will not be initialized for application using the data model from zap */
        return NULL;
    }
    endpoint_t *endpoint = get(node, endpoint_id);
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint not found");
        return NULL;
    }
    _endpoint_t *current_endpoint = (_endpoint_t *)endpoint;
    return current_endpoint->priv_data;
}

} /* endpoint */

namespace node {

node_t *create_raw()
{
    if (node) {
        ESP_LOGE(TAG, "Node already exists");
        return (node_t *)node;
    }
    node = (_node_t *)esp_matter_mem_calloc(1, sizeof(_node_t));
    if (!node) {
        ESP_LOGE(TAG, "Couldn't allocate _node_t");
        return NULL;
    }
    return (node_t *)node;
}

node_t *get()
{
    return (node_t *)node;
}

} /* node */
} /* esp_matter */
