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

#pragma once

/*
This is a common include file which includes all the other esp_matter component files which would be required by the
application.
*/

#include <esp_matter_attribute.h>
#include <esp_matter_cluster.h>
#include <esp_matter_command.h>
#include <esp_matter_client.h>
#include <esp_matter_core.h>
#include <esp_matter_endpoint.h>
#include <esp_matter_event.h>

#include <app-common/zap-generated/att-storage.h>
#include <app-common/zap-generated/attribute-id.h>
#include <app-common/zap-generated/cluster-id.h>
#include <app-common/zap-generated/command-id.h>

#include <app/server/Dnssd.h>
#include <platform/CHIPDeviceLayer.h>

typedef enum esp_matter_endpoint_flags {
    ESP_MATTER_ENDPOINT_FLAG_NONE = 0x00,
    ESP_MATTER_ENDPOINT_FLAG_DELETABLE = 0x01,
} esp_matter_endpoint_flags_t;

typedef enum esp_matter_cluster_flags {
    ESP_MATTER_CLUSTER_FLAG_NONE = 0x00,
    ESP_MATTER_CLUSTER_FLAG_INIT_FUNCTION = CLUSTER_MASK_INIT_FUNCTION, /* 0x01 */
    ESP_MATTER_CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION = CLUSTER_MASK_ATTRIBUTE_CHANGED_FUNCTION, /* 0x02 */
    ESP_MATTER_CLUSTER_FLAG_DEFAULT_RESPONSE_FUNCTION = CLUSTER_MASK_DEFAULT_RESPONSE_FUNCTION, /* 0x04 */
    ESP_MATTER_CLUSTER_FLAG_MESSAGE_SENT_FUNCTION = CLUSTER_MASK_MESSAGE_SENT_FUNCTION, /* 0x08 */
    ESP_MATTER_CLUSTER_FLAG_MANUFACTURER_SPECIFIC_ATTRIBUTE_CHANGED_FUNCTION = CLUSTER_MASK_MANUFACTURER_SPECIFIC_ATTRIBUTE_CHANGED_FUNCTION, /* 0x10 */
    ESP_MATTER_CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION = CLUSTER_MASK_PRE_ATTRIBUTE_CHANGED_FUNCTION, /* 0x20 */
    ESP_MATTER_CLUSTER_FLAG_SERVER = CLUSTER_MASK_SERVER, /* 0x40 */
    ESP_MATTER_CLUSTER_FLAG_CLIENT = CLUSTER_MASK_CLIENT, /* 0x80 */
} esp_matter_cluster_flags_t;

typedef enum esp_matter_attribute_flags {
    ESP_MATTER_ATTRIBUTE_FLAG_NONE = 0x00,
    ESP_MATTER_ATTRIBUTE_FLAG_WRITABLE = ATTRIBUTE_MASK_WRITABLE, /* 0x01 */
    ESP_MATTER_ATTRIBUTE_FLAG_NONVOLATILE = ATTRIBUTE_MASK_NONVOLATILE, /* 0x02 */
    ESP_MATTER_ATTRIBUTE_FLAG_MIN_MAX = ATTRIBUTE_MASK_MIN_MAX, /* 0x04 */
    ESP_MATTER_ATTRIBUTE_FLAG_MUST_USE_TIMED_WRITE = ATTRIBUTE_MASK_MUST_USE_TIMED_WRITE, /* 0x08 */
    ESP_MATTER_ATTRIBUTE_FLAG_EXTERNAL_STORAGE = ATTRIBUTE_MASK_EXTERNAL_STORAGE, /* 0x10 */
    ESP_MATTER_ATTRIBUTE_FLAG_SINGLETON = ATTRIBUTE_MASK_SINGLETON, /* 0x20 */
    ESP_MATTER_ATTRIBUTE_FLAG_CLIENT = ATTRIBUTE_MASK_CLIENT, /* 0x40 */
    ESP_MATTER_ATTRIBUTE_FLAG_NULLABLE = ATTRIBUTE_MASK_NULLABLE, /* 0x80 */
} esp_matter_attribute_flags_t;

typedef enum esp_matter_command_flags {
    ESP_MATTER_COMMAND_FLAG_NONE = 0x00,
    ESP_MATTER_COMMAND_FLAG_CUSTOM = 0x01,
    ESP_MATTER_COMMAND_FLAG_CLIENT_GENERATED = 0x02,
    ESP_MATTER_COMMAND_FLAG_SERVER_GENERATED = 0x04,
} esp_matter_command_flags_t;
