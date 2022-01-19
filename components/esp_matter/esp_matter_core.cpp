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
#include <esp_matter_core.h>

#include <app/server/Dnssd.h>
#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/examples/DeviceAttestationCredsExample.h>
#include <platform/CHIPDeviceLayer.h>
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include <esp_matter_openthread.h>
#endif

using chip::Credentials::SetDeviceAttestationCredentialsProvider;
using chip::Credentials::Examples::GetExampleDACProvider;
using chip::DeviceLayer::ChipDeviceEvent;
using chip::DeviceLayer::ConnectivityManager;
using chip::DeviceLayer::ConnectivityMgr;
using chip::DeviceLayer::PlatformMgr;
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
using chip::DeviceLayer::ThreadStackMgr;
#endif

static const char *TAG = "esp_matter_device";

typedef struct esp_matter_attribute {
    int attribute_id;
    uint8_t flags;
    esp_matter_attr_val_t val;
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
    // const EmberAfGenericClusterFunction *function_list;
    _esp_matter_attribute_t *attribute_list;
    _esp_matter_command_t *command_list;
    struct esp_matter_cluster *next;
} _esp_matter_cluster_t;

typedef struct esp_matter_endpoint {
    int endpoint_id;
    _esp_matter_cluster_t *cluster_list;
    struct esp_matter_endpoint *next;
} _esp_matter_endpoint_t;

typedef struct esp_matter_node {
    _esp_matter_endpoint_t *endpoint_list;
} _esp_matter_node_t;

static _esp_matter_node_t *node = NULL;

static int esp_matter_cluster_get_count(_esp_matter_cluster_t *current)
{
    int count = 0;
    while (current) {
        current = current->next;
        count++;
    }
    return count;
}

static int esp_matter_attribute_get_count(_esp_matter_attribute_t *current)
{
    int count = 0;
    while (current) {
        current = current->next;
        count++;
    }
    return count;
}

extern esp_err_t esp_matter_attribute_get_type_and_val_default(esp_matter_attr_val_t *val,
                                                               EmberAfAttributeType *attribute_type,
                                                               uint16_t *attribute_size,
                                                               EmberAfDefaultOrMinMaxAttributeValue *default_value);

esp_err_t esp_matter_endpoint_enable(esp_matter_endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    _esp_matter_endpoint_t *current_endpoint = (_esp_matter_endpoint_t *)endpoint;
    static int endpoint_index = 0;

    /* Endpoint Type */
    EmberAfEndpointType *endpoint_type = (EmberAfEndpointType *)calloc(1, sizeof(EmberAfEndpointType));
    if (!endpoint_type) {
        ESP_LOGE(TAG, "Couldn't allocate endpoint_type");
        return ESP_ERR_NO_MEM;
    }

    /* Clusters */
    _esp_matter_cluster_t *cluster = current_endpoint->cluster_list;
    int cluster_count = esp_matter_cluster_get_count(cluster);
    int cluster_index = 0;
    EmberAfCluster *matter_clusters = (EmberAfCluster *)calloc(1, cluster_count * sizeof(EmberAfCluster));
    if (!matter_clusters) {
        ESP_LOGE(TAG, "Couldn't allocate matter_clusters");
        return ESP_ERR_NO_MEM;
    }

    while (cluster) {
        /* Attributes */
        _esp_matter_attribute_t *attribute = cluster->attribute_list;
        int attribute_count = esp_matter_attribute_get_count(attribute);
        int attribute_index = 0;
        EmberAfAttributeMetadata *matter_attributes = (EmberAfAttributeMetadata *)calloc(1, 
                                                       attribute_count * sizeof(EmberAfAttributeMetadata));
        if (!matter_attributes) {
            ESP_LOGE(TAG, "Couldn't allocate matter_attributes");
            return ESP_ERR_NO_MEM;
        }

        while (attribute) {
            matter_attributes[attribute_index].attributeId = attribute->attribute_id;
            matter_attributes[attribute_index].mask = attribute->flags;
            esp_matter_attribute_get_type_and_val_default(&attribute->val,
                                                          &matter_attributes[attribute_index].attributeType,
                                                          &matter_attributes[attribute_index].size,
                                                          &matter_attributes[attribute_index].defaultValue);

            matter_clusters[cluster_index].clusterSize += matter_attributes[attribute_index].size;
            attribute = attribute->next;
            attribute_index++;
        }

        matter_clusters[cluster_index].clusterId = cluster->cluster_id;
        matter_clusters[cluster_index].attributes = matter_attributes;
        matter_clusters[cluster_index].attributeCount = attribute_count;
        matter_clusters[cluster_index].mask = cluster->flags;
        // matter_clusters[cluster_index].functions = function_list;

        endpoint_type->endpointSize += matter_clusters[cluster_index].clusterSize;
        cluster = cluster->next;
        cluster_index++;
    }

    endpoint_type->cluster = matter_clusters;
    endpoint_type->clusterCount = cluster_count;

    /* Add Endpoint */
    EmberAfStatus err = emberAfSetDynamicEndpoint(endpoint_index, current_endpoint->endpoint_id, endpoint_type, 0, 1);
    if (err != EMBER_ZCL_STATUS_SUCCESS) {
        ESP_LOGE(TAG, "Error adding dynamic endpoint %d: %d", current_endpoint->endpoint_id, err);
    }
    endpoint_index++;
    return ESP_OK;
}

static esp_err_t esp_matter_endpoint_enable_all()
{
    if (!node) {
        return ESP_FAIL;
    }

    _esp_matter_endpoint_t *endpoint = node->endpoint_list;
    while (endpoint) {
        esp_matter_endpoint_enable((esp_matter_endpoint_t *)endpoint);
        endpoint = endpoint->next;
    }
    return ESP_OK;
}

static void esp_matter_chip_init_task(intptr_t context)
{
    xTaskHandle task_to_notify = reinterpret_cast<xTaskHandle>(context);
    chip::Server::GetInstance().Init();
    SetDeviceAttestationCredentialsProvider(GetExampleDACProvider());
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
    xTaskNotifyGive(task_to_notify);
}

esp_err_t esp_matter_chip_init(esp_matter_event_callback_t callback)
{
    if (PlatformMgr().InitChipStack() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to initialize CHIP stack");
        return ESP_FAIL;
    }
    ConnectivityMgr().SetBLEAdvertisingEnabled(true);
    // ConnectivityMgr().SetWiFiAPMode(ConnectivityManager::kWiFiAPMode_Enabled);
    if (chip::Platform::MemoryInit() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to initialize CHIP memory pool");
        return ESP_ERR_NO_MEM;
    }
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

esp_matter_attr_val_t esp_matter_attribute_get_val(esp_matter_attribute_t *attribute)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return esp_matter_invalid(NULL);
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;
    return current_attribute->val;
}

esp_err_t esp_matter_attribute_set_val(esp_matter_attribute_t *attribute, esp_matter_attr_val_t val)
{
    if (!attribute) {
        ESP_LOGE(TAG, "Attribute cannot be NULL");
        return ESP_FAIL;
    }
    _esp_matter_attribute_t *current_attribute = (_esp_matter_attribute_t *)attribute;
    current_attribute->val = val;
    return ESP_OK;
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
    attribute->flags |= ATTRIBUTE_MASK_EXTERNAL_STORAGE;
    attribute->val = val;

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

esp_matter_endpoint_t *esp_matter_endpoint_create_raw(esp_matter_node_t *node, int endpoint_id)
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
    endpoint->endpoint_id = endpoint_id;

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
