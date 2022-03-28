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
#include <esp_matter_factory.h>

#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/server/Dnssd.h>
#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/ESP32/NetworkCommissioningDriver.h>
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include <esp_matter_openthread.h>
#endif

/* TODO: Remove the examples DAC provider once we have a concrete
 * way to generate attestation credentials.
 */
#if CONFIG_ESP_MATTER_USE_ESP_DAC_PROVIDER
#include <esp_matter_dac.h>
using chip::Credentials::esp::esp_matter_dac_provider_get;
#else
#include <credentials/examples/DeviceAttestationCredsExample.h>
using chip::Credentials::Examples::GetExampleDACProvider;
#endif

using chip::CommandId;
using chip::DataVersion;
using chip::kInvalidCommandId;
using chip::kInvalidEndpointId;
using chip::Credentials::SetDeviceAttestationCredentialsProvider;
using chip::DeviceLayer::ChipDeviceEvent;
using chip::DeviceLayer::ConnectivityManager;
using chip::DeviceLayer::ConnectivityMgr;
using chip::DeviceLayer::PlatformMgr;
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
using chip::DeviceLayer::ThreadStackMgr;
#endif

static const char *TAG = "esp_matter_core";

typedef struct esp_matter_attribute {
    int attribute_id;
    uint8_t flags;
    esp_matter_attr_val_t val;
    esp_matter_attr_bounds_t *bounds;
    EmberAfDefaultOrMinMaxAttributeValue default_value;
    uint16_t default_value_size;
    struct esp_matter_attribute *next;
} _esp_matter_attribute_t;

typedef struct esp_matter_command {
    int command_id;
    uint8_t flags;
    esp_matter_command_callback_t callback;
    struct esp_matter_command *next;
} _esp_matter_command_t;

typedef struct esp_matter_cluster {
    int cluster_id;
    uint8_t flags;
    const esp_matter_cluster_function_generic_t *function_list;
    esp_matter_cluster_plugin_server_init_callback_t plugin_server_init_callback;
    esp_matter_cluster_plugin_client_init_callback_t plugin_client_init_callback;
    _esp_matter_attribute_t *attribute_list;
    _esp_matter_command_t *command_list;
    struct esp_matter_cluster *next;
} _esp_matter_cluster_t;

typedef struct esp_matter_endpoint {
    int endpoint_id;
    int device_type_id;
    uint8_t flags;
    _esp_matter_cluster_t *cluster_list;
    EmberAfEndpointType *endpoint_type;
    DataVersion *data_versions_ptr;
    struct esp_matter_endpoint *next;
} _esp_matter_endpoint_t;

typedef struct esp_matter_node {
    _esp_matter_endpoint_t *endpoint_list;
    int current_endpoint_id;
} _esp_matter_node_t;

static _esp_matter_node_t *node = NULL;

static int cluster_get_count(_esp_matter_cluster_t *current)
{
    int count = 0;
    while (current) {
        current = current->next;
        count++;
    }
    return count;
}

static int attribute_get_count(_esp_matter_attribute_t *current)
{
    int count = 0;
    while (current) {
        current = current->next;
        count++;
    }
    return count;
}

static int command_get_count(_esp_matter_command_t *current, int command_flag)
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

static int endpoint_get_next_index()
{
    int endpoint_id = 0;
    for (int index = 0; index < MAX_ENDPOINT_COUNT; index++) {
        endpoint_id = emberAfEndpointFromIndex(index);
        if (endpoint_id == kInvalidEndpointId) {
            return index;
        }
    }
    return 0xFFFF;
}

extern esp_err_t get_data_from_attr_val(esp_matter_attr_val_t *val, EmberAfAttributeType *attribute_type,
                                        uint16_t *attribute_size, uint8_t *value);

static esp_err_t attribute_free_default_value(esp_matter_attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_FAIL;
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;

    /* Free value if data is more than 2 bytes or if it is min max attribute */
    if (current_attribute->flags & ESP_MATTER_ATTRIBUTE_FLAG_MIN_MAX) {
        if (current_attribute->default_value_size > 2) {
            if (current_attribute->default_value.ptrToMinMaxValue->defaultValue.ptrToDefaultValue) {
                free((void *)current_attribute->default_value.ptrToMinMaxValue->defaultValue.ptrToDefaultValue);
            }
            if (current_attribute->default_value.ptrToMinMaxValue->minValue.ptrToDefaultValue) {
                free((void *)current_attribute->default_value.ptrToMinMaxValue->minValue.ptrToDefaultValue);
            }
            if (current_attribute->default_value.ptrToMinMaxValue->maxValue.ptrToDefaultValue) {
                free((void *)current_attribute->default_value.ptrToMinMaxValue->maxValue.ptrToDefaultValue);
            }
        }
        free((void *)current_attribute->default_value.ptrToMinMaxValue);
    } else if (current_attribute->default_value_size > 2) {
        if (current_attribute->default_value.ptrToDefaultValue) {
            free((void *)current_attribute->default_value.ptrToDefaultValue);
        }
    }
    return ESP_OK;
}

static EmberAfDefaultAttributeValue get_default_value_from_data(esp_matter_attr_val_t *val,
                                                                EmberAfAttributeType attribute_type,
                                                                uint16_t attribute_size)
{
    EmberAfDefaultAttributeValue default_value = (uint16_t)0;
    uint8_t *value = (uint8_t *)calloc(1, attribute_size);
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
        free(value);
    }
    return default_value;
}

static esp_err_t attribute_set_default_value_from_current_val(esp_matter_attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_FAIL;
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;
    esp_matter_attr_val_t *val = &current_attribute->val;

    /* Get size */
    EmberAfAttributeType attribute_type = 0;
    uint16_t attribute_size = 0;
    get_data_from_attr_val(val, &attribute_type, &attribute_size, NULL);

    /* Get and set value */
    if (current_attribute->flags & ESP_MATTER_ATTRIBUTE_FLAG_MIN_MAX) {
        EmberAfAttributeMinMaxValue *temp_value = (EmberAfAttributeMinMaxValue *)calloc(1,
                                                                                sizeof(EmberAfAttributeMinMaxValue));
        if (!temp_value) {
            ESP_LOGE(TAG, "Could not allocate ptrToMinMaxValue for default value");
            return ESP_FAIL;
        }
        temp_value->defaultValue = get_default_value_from_data(val, attribute_type, attribute_size);
        temp_value->minValue = get_default_value_from_data(&current_attribute->bounds->min,
                                                                               attribute_type, attribute_size);
        temp_value->maxValue = get_default_value_from_data(&current_attribute->bounds->max,
                                                                               attribute_type, attribute_size);
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

static esp_err_t esp_matter_endpoint_disable(esp_matter_endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* Take lock if not already taken */
    esp_matter_lock_status_t lock_status = esp_matter_chip_stack_lock(portMAX_DELAY);
    if (lock_status == ESP_MATTER_LOCK_FAILED) {
        ESP_LOGE(TAG, "Could not get task context");
        return ESP_FAIL;
    }

    /* Remove endpoint */
    _esp_matter_endpoint_t *current_endpoint = (_esp_matter_endpoint_t *)endpoint;
    int endpoint_index = emberAfGetDynamicIndexFromEndpoint(current_endpoint->endpoint_id);
    if (endpoint_index == 0xFFFF) {
        ESP_LOGE(TAG, "Could not find endpoint index");
        if (lock_status == ESP_MATTER_LOCK_SUCCESS) {
            esp_matter_chip_stack_unlock();
        }
        return ESP_FAIL;
    }
    emberAfClearDynamicEndpoint(endpoint_index);

    if (lock_status == ESP_MATTER_LOCK_SUCCESS) {
        esp_matter_chip_stack_unlock();
    }
    if (!(current_endpoint->endpoint_type)) {
        ESP_LOGE(TAG, "endpoint %d's endpoint_type is NULL", current_endpoint->endpoint_id);
        return ESP_ERR_INVALID_STATE;
    }
    /* Free all clusters */
    EmberAfEndpointType *endpoint_type = current_endpoint->endpoint_type;
    int cluster_count = endpoint_type->clusterCount;
    for (int cluster_index = 0; cluster_index < cluster_count; cluster_index++) {
        /* Free attributes */
        free((void *)endpoint_type->cluster[cluster_index].attributes);
        /* Free commands */
        if (endpoint_type->cluster[cluster_index].clientGeneratedCommandList) {
            free((void *)endpoint_type->cluster[cluster_index].clientGeneratedCommandList);
        }
        if (endpoint_type->cluster[cluster_index].serverGeneratedCommandList) {
            free((void *)endpoint_type->cluster[cluster_index].serverGeneratedCommandList);
        }
    }
    free((void *)endpoint_type->cluster);

    /* Free data versions */
    if (current_endpoint->data_versions_ptr) {
        free(current_endpoint->data_versions_ptr);
        current_endpoint->data_versions_ptr = NULL;
    }

    /* Free endpoint type */
    free(endpoint_type);
    current_endpoint->endpoint_type = NULL;
    return ESP_OK;
}

esp_err_t esp_matter_endpoint_enable(esp_matter_endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_endpoint_t *current_endpoint = (_esp_matter_endpoint_t *)endpoint;

    /* Endpoint Type */
    EmberAfEndpointType *endpoint_type = (EmberAfEndpointType *)calloc(1, sizeof(EmberAfEndpointType));
    if (!endpoint_type) {
        ESP_LOGE(TAG, "Couldn't allocate endpoint_type");
        /* goto cleanup is not used here to avoid 'crosses initialization' of data_versions below */
        return ESP_ERR_NO_MEM;
    }
    current_endpoint->endpoint_type = endpoint_type;

    /* Clusters */
    _esp_matter_cluster_t *cluster = current_endpoint->cluster_list;
    int cluster_count = cluster_get_count(cluster);
    int cluster_index = 0;

    DataVersion *data_versions_ptr = (DataVersion *)calloc(1, cluster_count * sizeof(DataVersion));
    if (!data_versions_ptr) {
        ESP_LOGE(TAG, "Couldn't allocate data_versions");
        free(endpoint_type);
        current_endpoint->endpoint_type = NULL;
        /* goto cleanup is not used here to avoid 'crosses initialization' of data_versions below */
        return ESP_ERR_NO_MEM;
    }
    chip::Span<chip::DataVersion> data_versions(data_versions_ptr, cluster_count);
    current_endpoint->data_versions_ptr = data_versions_ptr;

    /* Variables */
    /* This is needed to avoid 'crosses initialization' errors because of goto */
    esp_err_t err = ESP_OK;
    esp_matter_lock_status_t lock_status = ESP_MATTER_LOCK_FAILED;
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;
    EmberAfCluster *matter_clusters = NULL;
    _esp_matter_attribute_t *attribute = NULL;
    int attribute_count = 0;
    int attribute_index = 0;
    EmberAfAttributeMetadata *matter_attributes = NULL;
    CommandId *client_generated_command_ids = NULL;
    CommandId *server_generated_command_ids = NULL;
    _esp_matter_command_t *command = NULL;
    int command_count = 0;
    int command_index = 0;
    int command_flag = ESP_MATTER_COMMAND_FLAG_NONE;
    int endpoint_index = 0;

    matter_clusters = (EmberAfCluster *)calloc(1, cluster_count * sizeof(EmberAfCluster));
    if (!matter_clusters) {
        ESP_LOGE(TAG, "Couldn't allocate matter_clusters");
        err = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    while (cluster) {
        /* Attributes */
        attribute = cluster->attribute_list;
        attribute_count = attribute_get_count(attribute);
        attribute_index = 0;
        matter_attributes = (EmberAfAttributeMetadata *)calloc(1,
                                                       attribute_count * sizeof(EmberAfAttributeMetadata));
        if (!matter_attributes) {
            ESP_LOGE(TAG, "Couldn't allocate matter_attributes");
            err = ESP_ERR_NO_MEM;
            break;
        }

        while (attribute) {
            matter_attributes[attribute_index].attributeId = attribute->attribute_id;
            matter_attributes[attribute_index].mask = attribute->flags;
            matter_attributes[attribute_index].defaultValue = attribute->default_value;
            get_data_from_attr_val(&attribute->val, &matter_attributes[attribute_index].attributeType,
                                   &matter_attributes[attribute_index].size, NULL);

            matter_clusters[cluster_index].clusterSize += matter_attributes[attribute_index].size;
            attribute = attribute->next;
            attribute_index++;
        }

        /* Commands */
        command = NULL;
        command_count = 0;
        command_index = 0;
        command_flag = ESP_MATTER_COMMAND_FLAG_NONE;
        client_generated_command_ids = NULL;
        server_generated_command_ids = NULL;

        /* Client Generated Commands */
        command_flag = ESP_MATTER_COMMAND_FLAG_CLIENT_GENERATED;
        command = cluster->command_list;
        command_count = command_get_count(command, command_flag);
        if (command_count > 0) {
            command_index = 0;
            client_generated_command_ids = (CommandId *)calloc(1, (command_count + 1) * sizeof(CommandId));
            if (!client_generated_command_ids) {
                ESP_LOGE(TAG, "Couldn't allocate client_generated_command_ids");
                err = ESP_ERR_NO_MEM;
                break;
            }
            while (command) {
                if (command->flags & command_flag) {
                    client_generated_command_ids[command_index] = command->command_id;
                    command_index++;
                }
                command = command->next;
            }
            client_generated_command_ids[command_index] = kInvalidCommandId;
        }

        /* Server Generated Commands */
        command_flag = ESP_MATTER_COMMAND_FLAG_SERVER_GENERATED;
        command = cluster->command_list;
        command_count = command_get_count(command, command_flag);
        if (command_count > 0) {
            command_index = 0;
            server_generated_command_ids = (CommandId *)calloc(1, (command_count + 1) * sizeof(CommandId));
            if (!server_generated_command_ids) {
                ESP_LOGE(TAG, "Couldn't allocate server_generated_command_ids");
                err = ESP_ERR_NO_MEM;
                break;
            }
            while (command) {
                if (command->flags & command_flag) {
                    server_generated_command_ids[command_index] = command->command_id;
                    command_index++;
                }
                command = command->next;
            }
            server_generated_command_ids[command_index] = kInvalidCommandId;
        }

        /* Fill up the cluster */
        matter_clusters[cluster_index].clusterId = cluster->cluster_id;
        matter_clusters[cluster_index].attributes = matter_attributes;
        matter_clusters[cluster_index].attributeCount = attribute_count;
        matter_clusters[cluster_index].mask = cluster->flags;
        matter_clusters[cluster_index].functions = (EmberAfGenericClusterFunction *)cluster->function_list;
        matter_clusters[cluster_index].clientGeneratedCommandList = client_generated_command_ids;
        matter_clusters[cluster_index].serverGeneratedCommandList = server_generated_command_ids;

        /* Get next cluster */
        endpoint_type->endpointSize += matter_clusters[cluster_index].clusterSize;
        cluster = cluster->next;
        cluster_index++;

        /* This is to avoid double free in case of errors */
        matter_attributes = NULL;
        client_generated_command_ids = NULL;
        server_generated_command_ids = NULL;
    }
    if (err != ESP_OK) {
        goto cleanup;
    }

    endpoint_type->cluster = matter_clusters;
    endpoint_type->clusterCount = cluster_count;

    /* Take lock if not already taken */
    lock_status = esp_matter_chip_stack_lock(portMAX_DELAY);
    if (lock_status == ESP_MATTER_LOCK_FAILED) {
        ESP_LOGE(TAG, "Could not get task context");
        goto cleanup;
    }

    /* Add Endpoint */
    endpoint_index = endpoint_get_next_index();
    status = emberAfSetDynamicEndpoint(endpoint_index, current_endpoint->endpoint_id, endpoint_type,
                                                  current_endpoint->device_type_id, 1,
                                                  chip::Span<chip::DataVersion>(data_versions));
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
        ESP_LOGE(TAG, "Error adding dynamic endpoint %d: %d", current_endpoint->endpoint_id, err);
        err = ESP_FAIL;
        if (lock_status == ESP_MATTER_LOCK_SUCCESS) {
            esp_matter_chip_stack_unlock();
        }
        goto cleanup;
    }
    if (lock_status == ESP_MATTER_LOCK_SUCCESS) {
        esp_matter_chip_stack_unlock();
    }
    return err;

cleanup:
    if (server_generated_command_ids) {
        free(server_generated_command_ids);
    }
    if (client_generated_command_ids) {
        free(client_generated_command_ids);
    }
    if (matter_attributes) {
        free(matter_attributes);
    }
    if (matter_clusters) {
        for (int cluster_index = 0; cluster_index < cluster_count; cluster_index++) {
            /* Free attributes */
            if (matter_clusters[cluster_index].attributes) {
                free((void *)matter_clusters[cluster_index].attributes);
            }
            /* Free commands */
            if (matter_clusters[cluster_index].clientGeneratedCommandList) {
                free((void *)matter_clusters[cluster_index].clientGeneratedCommandList);
            }
            if (matter_clusters[cluster_index].serverGeneratedCommandList) {
                free((void *)matter_clusters[cluster_index].serverGeneratedCommandList);
            }
        }
        free(matter_clusters);
    }
    if (data_versions_ptr) {
        free(data_versions_ptr);
        current_endpoint->data_versions_ptr = NULL;
    }
    if (endpoint_type) {
        free(endpoint_type);
        current_endpoint->endpoint_type = NULL;
    }
    return err;
}

static esp_err_t esp_matter_endpoint_enable_all()
{
    if (!node) {
        /* Not returning error, since the node will not be initialized for application using the data model from zap */
        return ESP_OK;
    }

    _esp_matter_endpoint_t *endpoint = node->endpoint_list;
    while (endpoint) {
        esp_matter_endpoint_enable((esp_matter_endpoint_t *)endpoint);
        endpoint = endpoint->next;
    }
    return ESP_OK;
}

#define DEFAULT_TICKS (500 / portTICK_PERIOD_MS) /* 500 ms in ticks */
esp_matter_lock_status_t esp_matter_chip_stack_lock(uint32_t ticks_to_wait)
{
    if (PlatformMgr().IsChipStackLockedByCurrentThread()) {
        return ESP_MATTER_LOCK_ALREADY_TAKEN;
    }
    if (ticks_to_wait == portMAX_DELAY) {
        /* Special handling for max delay */
        PlatformMgr().LockChipStack();
        return ESP_MATTER_LOCK_SUCCESS;
    }
    uint32_t ticks_remaining = ticks_to_wait;
    uint32_t ticks = DEFAULT_TICKS;
    while (ticks_remaining > 0) {
        if (PlatformMgr().TryLockChipStack()) {
            return ESP_MATTER_LOCK_SUCCESS;
        }
        ticks = ticks_remaining < DEFAULT_TICKS ? ticks_remaining : DEFAULT_TICKS;
        ticks_remaining -= ticks;
        ESP_LOGI(TAG, "Did not get lock yet. Retrying...");
        vTaskDelay(ticks);
    }
    ESP_LOGE(TAG, "Could not get lock");
    return ESP_MATTER_LOCK_FAILED;
}

esp_err_t esp_matter_chip_stack_unlock()
{
    PlatformMgr().UnlockChipStack();
    return ESP_OK;
}

static void esp_matter_chip_init_task(intptr_t context)
{
    xTaskHandle task_to_notify = reinterpret_cast<xTaskHandle>(context);
    chip::Server::GetInstance().Init();
#if CONFIG_ESP_MATTER_USE_ESP_DAC_PROVIDER
    SetDeviceAttestationCredentialsProvider(esp_matter_dac_provider_get());
#else
    SetDeviceAttestationCredentialsProvider(GetExampleDACProvider());
#endif
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    // If Thread is Provisioned, publish the dns service
    if (chip::DeviceLayer::ConnectivityMgr().IsThreadProvisioned() &&
            (chip::Server::GetInstance().GetFabricTable().FabricCount() != 0))
    {
        chip::app::DnssdServer::Instance().StartServer();
    }
#endif
    if (esp_matter_endpoint_enable_all() != ESP_OK) {
        ESP_LOGE(TAG, "Enable all endpoints failure");
    }
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
    {
        static chip::app::Clusters::NetworkCommissioning::Instance sWiFiNetworkCommissioningInstance(0 /* Endpoint Id*/,
            &(chip::DeviceLayer::NetworkCommissioning::ESPWiFiDriver::GetInstance()));
        sWiFiNetworkCommissioningInstance.Init();
    }
#endif
    /* Initialize binding manager */
    esp_matter_binding_manager_init();
    xTaskNotifyGive(task_to_notify);
}

esp_err_t esp_matter_chip_init(esp_matter_event_callback_t callback)
{
    if (chip::Platform::MemoryInit() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to initialize CHIP memory pool");
        return ESP_ERR_NO_MEM;
    }
    if (PlatformMgr().InitChipStack() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to initialize CHIP stack");
        return ESP_FAIL;
    }
    ConnectivityMgr().SetBLEAdvertisingEnabled(true);
    // ConnectivityMgr().SetWiFiAPMode(ConnectivityManager::kWiFiAPMode_Enabled);
    if (PlatformMgr().StartEventLoopTask() != CHIP_NO_ERROR) {
        chip::Platform::MemoryShutdown();
        ESP_LOGE(TAG, "Failed to launch Matter main task");
        return ESP_FAIL;
    }
    PlatformMgr().AddEventHandler(callback, static_cast<intptr_t>(NULL));
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    esp_matter_openthread_launch_task();
    if (ThreadStackMgr().InitThreadStack() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to initialize Thread stack");
        return ESP_FAIL;
    }

    if (ThreadStackMgr().StartThreadTask() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to launch Thread task");
        return ESP_FAIL;
    }
#endif
    PlatformMgr().ScheduleWork(esp_matter_chip_init_task, reinterpret_cast<intptr_t>(xTaskGetCurrentTaskHandle()));
    // Wait for the matter stack to be initialized
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    return ESP_OK;
}

esp_err_t esp_matter_start(esp_matter_event_callback_t callback)
{
    esp_err_t err = esp_matter_chip_init(callback);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing matter");
        return err;
    }
    err = esp_matter_factory_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing factory");
    }
    return err;
}

esp_matter_node_t *esp_matter_node_get()
{
    return (esp_matter_node_t *)node;
}

esp_matter_endpoint_t *esp_matter_endpoint_get(esp_matter_node_t *node, int endpoint_id)
{
    if (!node) {
        ESP_LOGE(TAG, "Node cannot be NULL");
        return NULL;
    }
    _esp_matter_node_t *current_node = (_esp_matter_node_t *)node;
    _esp_matter_endpoint_t *current_endpoint = (_esp_matter_endpoint_t *)current_node->endpoint_list;
    while (current_endpoint) {
        if (current_endpoint->endpoint_id == endpoint_id) {
            break;
        }
        current_endpoint = current_endpoint->next;
    }
    return (esp_matter_endpoint_t *)current_endpoint;
}

esp_matter_cluster_t *esp_matter_cluster_get(esp_matter_endpoint_t *endpoint, int cluster_id)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    _esp_matter_endpoint_t *current_endpoint = (_esp_matter_endpoint_t *)endpoint;
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)current_endpoint->cluster_list;
    while (current_cluster) {
        if (current_cluster->cluster_id == cluster_id) {
            break;
        }
        current_cluster = current_cluster->next;
    }
    return (esp_matter_cluster_t *)current_cluster;
}

esp_matter_attribute_t *esp_matter_attribute_get(esp_matter_cluster_t *cluster, int attribute_id)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)current_cluster->attribute_list;
    while (current_attribute) {
        if (current_attribute->attribute_id == attribute_id) {
            break;
        }
        current_attribute = current_attribute->next;
    }
    return (esp_matter_attribute_t *)current_attribute;
}

esp_matter_command_t *esp_matter_command_get(esp_matter_cluster_t *cluster, int command_id)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    _esp_matter_command_t *current_command = (_esp_matter_command_t *)current_cluster->command_list;
    while (current_command) {
        if (current_command->command_id == command_id) {
            break;
        }
        current_command = current_command->next;
    }
    return (esp_matter_command_t *)current_command;
}

esp_matter_endpoint_t *esp_matter_endpoint_get_first(esp_matter_node_t *node)
{
    if (!node) {
        ESP_LOGE(TAG, "Node cannot be NULL");
        return NULL;
    }
    _esp_matter_node_t *current_node = (_esp_matter_node_t *)node;
    return (esp_matter_endpoint_t *)current_node->endpoint_list;
}

esp_matter_cluster_t *esp_matter_cluster_get_first(esp_matter_endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    _esp_matter_endpoint_t *current_endpoint = (_esp_matter_endpoint_t *)endpoint;
    return (esp_matter_cluster_t *)current_endpoint->cluster_list;
}

esp_matter_attribute_t *esp_matter_attribute_get_first(esp_matter_cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    return (esp_matter_attribute_t *)current_cluster->attribute_list;
}

esp_matter_command_t *esp_matter_command_get_first(esp_matter_cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    return (esp_matter_command_t *)current_cluster->command_list;
}

esp_matter_endpoint_t *esp_matter_endpoint_get_next(esp_matter_endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    _esp_matter_endpoint_t *current_endpoint = (_esp_matter_endpoint_t *)endpoint;
    return (esp_matter_endpoint_t *)current_endpoint->next;
}

esp_matter_cluster_t *esp_matter_cluster_get_next(esp_matter_cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    return (esp_matter_cluster_t *)current_cluster->next;
}

esp_matter_attribute_t *esp_matter_attribute_get_next(esp_matter_attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return NULL;
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;
    return (esp_matter_attribute_t *)current_attribute->next;
}

esp_matter_command_t *esp_matter_command_get_next(esp_matter_command_t *command)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return NULL;
    }
    _esp_matter_command_t *current_command = (_esp_matter_command_t *)command;
    return (esp_matter_command_t *)current_command->next;
}

int esp_matter_endpoint_get_id(esp_matter_endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return -1;
    }
    _esp_matter_endpoint_t *current_endpoint = (_esp_matter_endpoint_t *)endpoint;
    return current_endpoint->endpoint_id;
}

int esp_matter_cluster_get_id(esp_matter_cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return -1;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    return current_cluster->cluster_id;
}

int esp_matter_attribute_get_id(esp_matter_attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return -1;
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;
    return current_attribute->attribute_id;
}

int esp_matter_command_get_id(esp_matter_command_t *command)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return -1;
    }
    _esp_matter_command_t *current_command = (_esp_matter_command_t *)command;
    return current_command->command_id;
}

esp_err_t esp_matter_cluster_set_plugin_server_init_callback(esp_matter_cluster_t *cluster,
                                                             esp_matter_cluster_plugin_server_init_callback_t callback)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    current_cluster->plugin_server_init_callback = callback;
    return ESP_OK;
}

esp_err_t esp_matter_cluster_set_plugin_client_init_callback(esp_matter_cluster_t *cluster,
                                                             esp_matter_cluster_plugin_client_init_callback_t callback)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    current_cluster->plugin_client_init_callback = callback;
    return ESP_OK;
}

esp_matter_cluster_plugin_server_init_callback_t esp_matter_cluster_get_plugin_server_init_callback(
                                                                                        esp_matter_cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    return current_cluster->plugin_server_init_callback;
}

esp_matter_cluster_plugin_client_init_callback_t esp_matter_cluster_get_plugin_client_init_callback(
                                                                                        esp_matter_cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    return current_cluster->plugin_client_init_callback;
}

esp_err_t esp_matter_cluster_add_function_list(esp_matter_cluster_t *cluster,
                                               const esp_matter_cluster_function_generic_t *function_list,
                                               int function_flags)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;
    current_cluster->function_list = function_list;
    current_cluster->flags |= function_flags;
    return ESP_OK;
}

esp_err_t esp_matter_attribute_get_val(esp_matter_attribute_t *attribute, esp_matter_attr_val_t *val)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;
    memcpy((void *)val, (void *)&current_attribute->val, sizeof(esp_matter_attr_val_t));
    return ESP_OK;
}

esp_err_t esp_matter_attribute_set_val(esp_matter_attribute_t *attribute, esp_matter_attr_val_t *val)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_FAIL;
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;
    if (val->type == ESP_MATTER_VAL_TYPE_CHAR_STRING || val->type == ESP_MATTER_VAL_TYPE_OCTET_STRING
        || val->type == ESP_MATTER_VAL_TYPE_ARRAY) {
        /* Free old buf */
        if (current_attribute->val.val.a.b) {
            free(current_attribute->val.val.a.b);
        }
        if (val->val.a.s > 0) {
            /* Alloc new buf */
            uint8_t *new_buf = (uint8_t *)calloc(1, val->val.a.s);
            if (!new_buf) {
                ESP_LOGE(TAG, "Could not allocate new buffer");
                return ESP_ERR_NO_MEM;
            }
            /* Copy to new buf and assign */
            memcpy(new_buf, val->val.a.b, val->val.a.s);
            val->val.a.b = new_buf;
        } else {
            ESP_LOGD(TAG, "Set val called with string with size 0");
            val->val.a.b = NULL;
        }
    }
    memcpy((void *)&current_attribute->val, (void *)val, sizeof(esp_matter_attr_val_t));
    return ESP_OK;
}

esp_err_t esp_matter_attribute_add_bounds(esp_matter_attribute_t *attribute, esp_matter_attr_val_t min,
                                          esp_matter_attr_val_t max)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;

    /* Check if bounds can be set */
    if (current_attribute->val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING
        || current_attribute->val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING
        || current_attribute->val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        ESP_LOGE(TAG, "Bounds cannot be set for string/array type attributes");
        return ESP_ERR_INVALID_ARG;
    }
    if ((current_attribute->val.type != min.type) || (current_attribute->val.type != max.type)) {
        ESP_LOGE(TAG, "Cannot set bounds because of val type mismatch: expected: %d, min: %d, max: %d",
                 current_attribute->val.type, min.type, max.type);
        return ESP_ERR_INVALID_ARG;
    }

    /* Free the default value before setting the new bounds */
    attribute_free_default_value(attribute);

    /* Allocate and set */
    current_attribute->bounds = (esp_matter_attr_bounds_t *)calloc(1, sizeof(esp_matter_attr_bounds_t));
    if (!current_attribute->bounds) {
        ESP_LOGE(TAG, "Could not allocate bounds");
        return ESP_ERR_NO_MEM;
    }
    memcpy((void *)&current_attribute->bounds->min, (void *)&min, sizeof(esp_matter_attr_val_t));
    memcpy((void *)&current_attribute->bounds->max, (void *)&max, sizeof(esp_matter_attr_val_t));
    current_attribute->flags |= ESP_MATTER_ATTRIBUTE_FLAG_MIN_MAX;

    /* Set the default value again after setting the bounds and the flag */
    attribute_set_default_value_from_current_val(attribute);
    return ESP_OK;
}

esp_matter_attr_bounds_t *esp_matter_attribute_get_bounds(esp_matter_attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return NULL;
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;
    return current_attribute->bounds;
}

esp_matter_command_callback_t esp_matter_command_get_callback(esp_matter_command_t *command)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return NULL;
    }
    _esp_matter_command_t *current_command = (_esp_matter_command_t *)command;
    return current_command->callback;
}

int esp_matter_command_get_flags(esp_matter_command_t *command)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return 0;
    }
    _esp_matter_command_t *current_command = (_esp_matter_command_t *)command;
    return current_command->flags;
}

esp_err_t esp_matter_endpoint_set_device_type_id(esp_matter_endpoint_t *endpoint, int device_type_id)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_endpoint_t *current_endpoint = (_esp_matter_endpoint_t *)endpoint;
    current_endpoint->device_type_id = device_type_id;
    return ESP_OK;
}

int esp_matter_endpoint_get_device_type_id(int endpoint_id)
{
    int device_type_id = emberAfGetDeviceIdForEndpoint(endpoint_id);
    return device_type_id;
}

static esp_err_t esp_matter_attribute_delete(esp_matter_attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;

    /* Default value needs to be deleted first since it uses the current val. */
    attribute_free_default_value(attribute);

    /* Delete val here, if required */
    if (current_attribute->val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING
        || current_attribute->val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING
        || current_attribute->val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        /* Free buf */
        if (current_attribute->val.val.a.b) {
            free(current_attribute->val.val.a.b);
        }
    }

    /* Free bounds */
    if (current_attribute->bounds) {
        free(current_attribute->bounds);
    }

    /* Free */
    free(current_attribute);
    return ESP_OK;
}

esp_matter_attribute_t *esp_matter_attribute_create(esp_matter_cluster_t *cluster, int attribute_id, uint8_t flags,
                                                    esp_matter_attr_val_t val)
{
    /* Find */
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;

    /* Allocate */
    _esp_matter_attribute_t *attribute = (_esp_matter_attribute_t *)calloc(1, sizeof(_esp_matter_attribute_t));
    if (!attribute) {
        ESP_LOGE(TAG, "Couldn't allocate _esp_matter_attribute_t");
        return NULL;
    }

    /* Set */
    attribute->attribute_id = attribute_id;
    attribute->flags = flags;
    attribute->flags |= ESP_MATTER_ATTRIBUTE_FLAG_EXTERNAL_STORAGE;
    esp_matter_attribute_set_val((esp_matter_attribute_t *)attribute, &val);
    attribute_set_default_value_from_current_val((esp_matter_attribute_t *)attribute);

    /* Add */
    _esp_matter_attribute_t *previous_attribute = NULL;
    _esp_matter_attribute_t *current_attribute = current_cluster->attribute_list;
    while (current_attribute) {
        previous_attribute = current_attribute;
        current_attribute = current_attribute->next;
    }
    if (previous_attribute == NULL) {
        current_cluster->attribute_list = attribute;
    } else {
        previous_attribute->next = attribute;
    }

    return (esp_matter_attribute_t *)attribute;
}

static esp_err_t esp_matter_command_delete(esp_matter_command_t *command)
{
    if (!command) {
        ESP_LOGE(TAG, "Command cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_command_t *current_command = (_esp_matter_command_t *)command;

    /* Free */
    free(current_command);
    return ESP_OK;
}

esp_matter_command_t *esp_matter_command_create(esp_matter_cluster_t *cluster, int command_id, uint8_t flags,
                                                esp_matter_command_callback_t callback)
{
    /* Find */
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return NULL;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;

    /* Allocate */
    _esp_matter_command_t *command = (_esp_matter_command_t *)calloc(1, sizeof(_esp_matter_command_t));
    if (!command) {
        ESP_LOGE(TAG, "Couldn't allocate _esp_matter_command_t");
        return NULL;
    }

    /* Set */
    command->command_id = command_id;
    command->flags = flags;
    command->callback = callback;

    /* Add */
    _esp_matter_command_t *previous_command = NULL;
    _esp_matter_command_t *current_command = current_cluster->command_list;
    while (current_command) {
        previous_command = current_command;
        current_command = current_command->next;
    }
    if (previous_command == NULL) {
        current_cluster->command_list = command;
    } else {
        previous_command->next = command;
    }

    return (esp_matter_command_t *)command;
}

static esp_err_t esp_matter_cluster_delete(esp_matter_cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_cluster_t *current_cluster = (_esp_matter_cluster_t *)cluster;

    /* Parse and delete all commands */
    _esp_matter_command_t *command = current_cluster->command_list;
    while (command) {
        _esp_matter_command_t *next_command = command->next;
        esp_matter_command_delete((esp_matter_command_t *)command);
        command = next_command;
    }

    /* Parse and delete all attributes */
    _esp_matter_attribute_t *attribute = current_cluster->attribute_list;
    while (attribute) {
        _esp_matter_attribute_t *next_attribute = attribute->next;
        esp_matter_attribute_delete((esp_matter_attribute_t *)attribute);
        attribute = next_attribute;
    }

    /* Free */
    free(current_cluster);
    return ESP_OK;
}

esp_matter_cluster_t *esp_matter_cluster_create(esp_matter_endpoint_t *endpoint, int cluster_id, uint8_t flags)
{
    /* Find */
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    _esp_matter_endpoint_t *current_endpoint = (_esp_matter_endpoint_t *)endpoint;

    /* Allocate */
    _esp_matter_cluster_t *cluster = (_esp_matter_cluster_t *)calloc(1, sizeof(_esp_matter_cluster_t));
    if (!cluster) {
        ESP_LOGE(TAG, "Couldn't allocate _esp_matter_cluster_t");
        return NULL;
    }

    /* Set */
    cluster->cluster_id = cluster_id;
    cluster->flags = flags;

    /* Add */
    _esp_matter_cluster_t *previous_cluster = NULL;
    _esp_matter_cluster_t *current_cluster = current_endpoint->cluster_list;
    while (current_cluster) {
        previous_cluster = current_cluster;
        current_cluster = current_cluster->next;
    }
    if (previous_cluster == NULL) {
        current_endpoint->cluster_list = cluster;
    } else {
        previous_cluster->next = cluster;
    }

    return (esp_matter_cluster_t *)cluster;
}

esp_err_t esp_matter_endpoint_delete(esp_matter_node_t *node, esp_matter_endpoint_t *endpoint)
{
    if (!node || !endpoint) {
        ESP_LOGE(TAG, "Node or endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_node_t *current_node = (_esp_matter_node_t *)node;
    _esp_matter_endpoint_t *_endpoint = (_esp_matter_endpoint_t *)endpoint;

    if (!(_endpoint->flags & ESP_MATTER_ENDPOINT_FLAG_DELETABLE)) {
        ESP_LOGE(TAG, "This endpoint cannot be deleted since the ESP_MATTER_ENDPOINT_FLAG_DELETABLE is not set");
        return ESP_FAIL;
    }

    /* Find current endpoint and remove from list */
    _esp_matter_endpoint_t *current_endpoint = current_node->endpoint_list;
    _esp_matter_endpoint_t *previous_endpoint = NULL;
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
    esp_matter_endpoint_disable(endpoint);

    /* Parse and delete all clusters */
    _esp_matter_cluster_t *cluster = current_endpoint->cluster_list;
    while (cluster) {
        _esp_matter_cluster_t *next_cluster = cluster->next;
        esp_matter_cluster_delete((esp_matter_cluster_t *)cluster);
        cluster = next_cluster;
    }

    /* Free */
    free(current_endpoint);
    return ESP_OK;
}

esp_matter_endpoint_t *esp_matter_endpoint_create_raw(esp_matter_node_t *node, uint8_t flags)
{
    /* Find */
    if (!node) {
        ESP_LOGE(TAG, "Node cannot be NULL");
        return NULL;
    }
    _esp_matter_node_t *current_node = (_esp_matter_node_t *)node;

    /* Allocate */
    _esp_matter_endpoint_t *endpoint = (_esp_matter_endpoint_t *)calloc(1, sizeof(_esp_matter_endpoint_t));
    if (!endpoint) {
        ESP_LOGE(TAG, "Couldn't allocate _esp_matter_endpoint_t");
        return NULL;
    }

    /* Set */
    endpoint->endpoint_id = current_node->current_endpoint_id++;
    endpoint->device_type_id = 0xFFFF;
    endpoint->flags = flags;

    /* Add */
    _esp_matter_endpoint_t *previous_endpoint = NULL;
    _esp_matter_endpoint_t *current_endpoint = current_node->endpoint_list;
    while (current_endpoint) {
        previous_endpoint = current_endpoint;
        current_endpoint = current_endpoint->next;
    }
    if (previous_endpoint == NULL) {
        current_node->endpoint_list = endpoint;
    } else {
        previous_endpoint->next = endpoint;
    }

    return (esp_matter_endpoint_t *)endpoint;
}

esp_matter_node_t *esp_matter_node_create_raw()
{
    node = (_esp_matter_node_t *)calloc(1, sizeof(_esp_matter_node_t));
    if (!node) {
        ESP_LOGE(TAG, "Couldn't allocate _esp_matter_node_t");
        return NULL;
    }
    return (esp_matter_node_t *)node;
}
