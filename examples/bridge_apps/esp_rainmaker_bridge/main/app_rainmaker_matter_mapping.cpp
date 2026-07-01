/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <app_bridged_device.h>
#include <app_rainmaker_bridged_device.h>
#include <app_rainmaker_matter_mapping.h>
#include <common_macros.h>
#include <rainmaker_controller.h>

#include <cJSON.h>
#include <esp_check.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_bridge.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_standard_params.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "rainmaker_controller_sync";

using namespace chip;
using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

extern uint16_t aggregator_endpoint_id;

static uint32_t app_rainmaker_get_matter_device_type(const char *config, size_t config_len,
                                                     char *node_name, size_t node_name_len);
static esp_err_t app_map_rainmaker_params_to_matter_attributes(uint16_t endpoint_id, const char *node_name,
                                                               const char *params, size_t params_len);

static rainmaker_device_addr_t app_rainmaker_matter_address(const char *node_id, const char *node_name = nullptr)
{
    rainmaker_device_addr_t addr = {};
    if (node_id) {
        strlcpy(addr.rainmaker_node_id, node_id, sizeof(addr.rainmaker_node_id));
    }
    if (node_name) {
        strlcpy(addr.rainmaker_node_name, node_name, sizeof(addr.rainmaker_node_name));
    }
    return addr;
}

static const rainmaker_device_addr_t *app_rainmaker_matter_get_address_by_endpoint_id(uint16_t endpoint_id)
{
    app_bridged_device_t *bridged_device = app_bridge_get_device(endpoint_id);
    if (!bridged_device || !bridged_device->get_dev_addr()) {
        return nullptr;
    }
    return static_cast<const rainmaker_device_addr_t *>(bridged_device->get_dev_addr());
}

static uint16_t app_rainmaker_matter_get_matter_endpoint_id_by_node_id(const char *node_id)
{
    rainmaker_device_addr_t addr = app_rainmaker_matter_address(node_id);
    return app_bridge_get_endpoint(&addr);
}

static const char *app_rainmaker_matter_get_node_id_by_matter_endpoint_id(uint16_t endpoint_id)
{
    const rainmaker_device_addr_t *addr = app_rainmaker_matter_get_address_by_endpoint_id(endpoint_id);
    return addr ? addr->rainmaker_node_id : nullptr;
}

static const char *app_rainmaker_matter_get_node_name_by_matter_endpoint_id(uint16_t endpoint_id)
{
    const rainmaker_device_addr_t *addr = app_rainmaker_matter_get_address_by_endpoint_id(endpoint_id);
    return addr ? addr->rainmaker_node_name : nullptr;
}

static app_bridged_device_t *app_rainmaker_matter_get_device_by_node_id(const char *node_id)
{
    rainmaker_device_addr_t addr = app_rainmaker_matter_address(node_id);
    return app_bridge_get_device(&addr);
}

static esp_err_t rainmaker_controller_match_device(char *node_id, const char *node_name, uint32_t device_type_id)
{
    node_t *node = node::get();

    if (app_rainmaker_matter_get_matter_endpoint_id_by_node_id(node_id) != chip::kInvalidEndpointId) {
        ESP_LOGI(TAG, "Bridged node for %s rainmaker device has been created", node_id);
        return ESP_OK;
    }

    rainmaker_device_addr_t addr = app_rainmaker_matter_address(node_id, node_name);
    return app_bridge_create_new_device(node, aggregator_endpoint_id, device_type_id, &addr, nullptr);
}

static esp_err_t rainmaker_controller_set_device_group_id(const char *node_id, const char *group_id)
{
    ESP_RETURN_ON_FALSE(node_id && node_id[0] != 0 && group_id, ESP_ERR_INVALID_ARG, TAG,
                        "Invalid RainMaker Groups.GroupID update: node_id=%p group_id=%p", node_id, group_id);

    ESP_LOGI(TAG, "Set RainMaker Groups.GroupID for node %s: %s", node_id, group_id);
    rainmaker_controller_param_update_t update = {
        .name = ESP_RMAKER_DEF_GROUP_ID_NAME,
        .type = RAINMAKER_CONTROLLER_PARAM_TYPE_STRING,
        .value = {.s = group_id},
    };
    esp_err_t err = rainmaker_controller_set_node_params(node_id, "Groups", &update, 1);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to set RainMaker Groups.GroupID for node %s: %d", node_id, err);
    }
    return err;
}

static esp_err_t rainmaker_controller_get_param_from_device(const char *node_id, uint16_t endpoint_id)
{
    node_t *node = node::get();
    endpoint_t *dev_endpoint = endpoint::get(node, endpoint_id);
    ESP_RETURN_ON_FALSE(dev_endpoint, ESP_ERR_NOT_FOUND, TAG, "Matter endpoint %u not found", endpoint_id);
    uint8_t device_type_count = endpoint::get_device_type_count(dev_endpoint);
    uint32_t dev_type_id = endpoint::bridged_node::get_device_type_id();
    uint8_t dev_type_ver;

    for (uint8_t i = 0; i < device_type_count; ++i) {
        if ((ESP_OK == endpoint::get_device_type_at_index(dev_endpoint, i, dev_type_id, dev_type_ver))) {
            ESP_LOGI(TAG, "Endpoint Id: %d--Node Id: %s Device Type: %ld\n", endpoint_id, node_id, dev_type_id);
            if (dev_type_id != endpoint::bridged_node::get_device_type_id()) {
                break;
            }
        }
    }

    switch (dev_type_id) {
    case ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID:
    case ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID:
    case ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID:
    case ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID: {
        char *receive_buffer = nullptr;
        if (rainmaker_controller_get_node_params(node_id, &receive_buffer) == ESP_OK && receive_buffer) {
            app_map_rainmaker_params_to_matter_attributes(
                endpoint_id, app_rainmaker_matter_get_node_name_by_matter_endpoint_id(endpoint_id), receive_buffer,
                strlen(receive_buffer));
            free(receive_buffer);
        }
    }
    break;
    /* TODO: add other device types */
    default:
        break;
    }
    return ESP_OK;
}

static esp_err_t rainmaker_controller_update_online_state(const char *node_id, uint16_t endpoint_id)
{
    bool connection_status = false;
    attribute_t *attribute = attribute::get(endpoint_id, BridgedDeviceBasicInformation::Id,
                                            BridgedDeviceBasicInformation::Attributes::Reachable::Id);
    ESP_RETURN_ON_FALSE(attribute, ESP_ERR_NOT_FOUND, TAG, "Matter Reachable attribute not found for endpoint %u",
                        endpoint_id);
    esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
    ESP_RETURN_ON_ERROR(attribute::get_val(attribute, &val), TAG,
                        "Failed to read Matter Reachable attribute for endpoint %u", endpoint_id);

    ESP_RETURN_ON_ERROR(rainmaker_controller_get_node_connection_status(node_id, &connection_status), TAG,
                        "Failed to get node %s connection status", node_id);

    ESP_LOGI(TAG, "node %s connection status: %d", node_id, connection_status);
    if (val.val.b != connection_status) {
        val.val.b = connection_status;
        ESP_RETURN_ON_ERROR(attribute::update(endpoint_id, BridgedDeviceBasicInformation::Id,
                                              BridgedDeviceBasicInformation::Attributes::Reachable::Id, &val),
                            TAG, "Failed to update Matter Reachable attribute for endpoint %u", endpoint_id);
    }

    return ESP_OK;
}

static esp_err_t app_rainmaker_matter_set_online_state(uint16_t endpoint_id, bool connection_status)
{
    attribute_t *attribute = attribute::get(endpoint_id, BridgedDeviceBasicInformation::Id,
                                            BridgedDeviceBasicInformation::Attributes::Reachable::Id);
    ESP_RETURN_ON_FALSE(attribute, ESP_ERR_NOT_FOUND, TAG, "Matter Reachable attribute not found for endpoint %u",
                        endpoint_id);

    esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
    ESP_RETURN_ON_ERROR(attribute::get_val(attribute, &val), TAG,
                        "Failed to read Matter Reachable attribute for endpoint %u", endpoint_id);
    if (val.val.b == connection_status) {
        return ESP_OK;
    }

    val.val.b = connection_status;
    return attribute::update(endpoint_id, BridgedDeviceBasicInformation::Id,
                             BridgedDeviceBasicInformation::Attributes::Reachable::Id, &val);
}

static esp_err_t rainmaker_controller_add_new_device(char *node_id)
{
    esp_err_t ret = ESP_OK;
    uint32_t matter_device_type = 0;
    char node_name[32] = {0};
    char *receive_buffer = nullptr;

    ESP_RETURN_ON_FALSE(node_id, ESP_ERR_INVALID_ARG, TAG, "RainMaker node id is null");

    ESP_GOTO_ON_FALSE(rainmaker_controller_get_node_config(node_id, &receive_buffer) == ESP_OK && receive_buffer,
                      ESP_FAIL, cleanup, TAG, "Get Node %s config failed\n", node_id);

    matter_device_type = app_rainmaker_get_matter_device_type(receive_buffer, strlen(receive_buffer), node_name,
                                                              sizeof(node_name));
    ESP_GOTO_ON_FALSE((matter_device_type != APP_RAINMAKER_INVALID_MATTER_DEVICE_TYPE) && (node_name[0] != 0),
                      ESP_ERR_NOT_FOUND, cleanup, TAG, "Node %s device type 0x%lx is invalid", node_id,
                      matter_device_type);

    ESP_GOTO_ON_ERROR(rainmaker_controller_match_device(node_id, node_name, matter_device_type), cleanup, TAG,
                      "Failed to create Matter bridged endpoint for RainMaker node %s", node_id);

cleanup:
    free(receive_buffer);
    return ret;
}

static esp_err_t rainmaker_controller_delete_device(uint16_t endpoint_id)
{
    const char *node_id = app_rainmaker_matter_get_node_id_by_matter_endpoint_id(endpoint_id);
    ESP_RETURN_ON_FALSE(node_id, ESP_ERR_NOT_FOUND, TAG, "RainMaker node not found for Matter endpoint %d", endpoint_id);
    char node_id_buf[sizeof(rainmaker_device_addr_t::rainmaker_node_id)] = {0};
    strlcpy(node_id_buf, node_id, sizeof(node_id_buf));

    app_bridged_device_t *bridged_device = app_rainmaker_matter_get_device_by_node_id(node_id);
    if (!bridged_device) {
        ESP_LOGW(TAG, "Bridged RainMaker device not found: %s", node_id_buf);
        return ESP_OK;
    }

    rainmaker_controller_set_device_group_id(node_id_buf, "");
    app_bridge_remove_device(bridged_device);
    ESP_LOGI(TAG, "Bridged rainmaker device removed: %s", node_id_buf);
    return ESP_OK;
}

static bool rainmaker_group_nodes_has(cJSON *group_nodes, const char *node_id)
{
    if (!cJSON_IsArray(group_nodes) || !node_id) {
        return false;
    }

    cJSON *node = nullptr;
    cJSON_ArrayForEach(node, group_nodes) {
        if (cJSON_IsString(node) && node->valuestring && strcmp(node->valuestring, node_id) == 0) {
            return true;
        }
    }
    return false;
}

static void matter_remove_devices_not_in_group(cJSON *group_nodes)
{
    uint16_t matter_endpoint_id_array[MAX_BRIDGED_DEVICE_COUNT];
    esp_matter_bridge::get_bridged_endpoint_ids(matter_endpoint_id_array);

    for (int i = 0; i < MAX_BRIDGED_DEVICE_COUNT; i++) {
        if (matter_endpoint_id_array[i] == chip::kInvalidEndpointId) {
            continue;
        }

        const char *node_id = app_rainmaker_matter_get_node_id_by_matter_endpoint_id(matter_endpoint_id_array[i]);
        if (!node_id || rainmaker_group_nodes_has(group_nodes, node_id)) {
            continue;
        }

        ESP_LOGI(TAG, "Remove RainMaker device outside bridge group Node: %s Endpoint: %d\n", node_id,
                 matter_endpoint_id_array[i]);
        rainmaker_controller_delete_device(matter_endpoint_id_array[i]);
    }
}

esp_err_t app_map_rainmaker_nodes_to_matter(char *nodes_json, size_t nodes_json_len, const char *group_id)
{
    esp_err_t ret = ESP_OK;
    cJSON *node = nullptr;
    cJSON *group_nodes = cJSON_ParseWithLength(nodes_json, nodes_json_len);
    ESP_GOTO_ON_FALSE(cJSON_IsArray(group_nodes), ESP_FAIL, cleanup, TAG, "Bridge group nodes json is not an array");

    cJSON_ArrayForEach(node, group_nodes) {
        if (!cJSON_IsString(node) || !node->valuestring || node->valuestring[0] == 0) {
            continue;
        }
        if (strcmp(node->valuestring, esp_rmaker_get_node_id()) == 0) {
            continue;
        }

        bool created = false;
        uint16_t endpoint_id = app_rainmaker_matter_get_matter_endpoint_id_by_node_id(node->valuestring);
        if (endpoint_id == chip::kInvalidEndpointId) {
            char node_id[32] = {0};
            snprintf(node_id, sizeof(node_id), "%s", node->valuestring);
            if (rainmaker_controller_add_new_device(node_id) == ESP_OK) {
                endpoint_id = app_rainmaker_matter_get_matter_endpoint_id_by_node_id(node->valuestring);
                created = true;
            }
        }

        ESP_LOGI(TAG, "Exist node: %s--endpoint id: %d\n", node->valuestring, endpoint_id);
        if (endpoint_id == chip::kInvalidEndpointId) {
            continue;
        }

        if (created && group_id) {
            rainmaker_controller_set_device_group_id(node->valuestring, group_id);
        }
        rainmaker_controller_get_param_from_device(node->valuestring, endpoint_id);
        rainmaker_controller_update_online_state(node->valuestring, endpoint_id);
    }

    matter_remove_devices_not_in_group(group_nodes);

cleanup:
    cJSON_Delete(group_nodes);
    return ret;
}

esp_err_t app_map_rainmaker_node_params_to_matter(const char *node_id, const char *payload, size_t payload_len)
{
    ESP_RETURN_ON_FALSE(node_id && payload && payload_len > 0, ESP_ERR_INVALID_ARG, TAG,
                        "Invalid RainMaker params update: node=%p payload=%p len=%u", node_id, payload, payload_len);

    char node_id_buf[32] = {0};
    snprintf(node_id_buf, sizeof(node_id_buf), "%s", node_id);
    uint16_t endpoint_id = app_rainmaker_matter_get_matter_endpoint_id_by_node_id(node_id_buf);
    ESP_RETURN_ON_FALSE(endpoint_id != chip::kInvalidEndpointId, ESP_ERR_NOT_FOUND, TAG,
                        "Matter endpoint not found for RainMaker node %s", node_id);

    const char *node_name = app_rainmaker_matter_get_node_name_by_matter_endpoint_id(endpoint_id);
    ESP_LOGI(TAG, "RainMaker update maps to Matter endpoint=%u device=%s", endpoint_id, node_name ? node_name : "");
    esp_err_t err = app_map_rainmaker_params_to_matter_attributes(endpoint_id, node_name, payload, payload_len);
    return err == ESP_ERR_NOT_FOUND ? ESP_OK : err;
}

esp_err_t app_map_rainmaker_node_connectivity_to_matter(const char *node_id, bool connected)
{
    ESP_RETURN_ON_FALSE(node_id, ESP_ERR_INVALID_ARG, TAG, "RainMaker connectivity update missing node id");

    char node_id_buf[32] = {0};
    snprintf(node_id_buf, sizeof(node_id_buf), "%s", node_id);
    uint16_t endpoint_id = app_rainmaker_matter_get_matter_endpoint_id_by_node_id(node_id_buf);
    ESP_RETURN_ON_FALSE(endpoint_id != chip::kInvalidEndpointId, ESP_ERR_NOT_FOUND, TAG,
                        "Matter endpoint not found for RainMaker node %s", node_id);

    ESP_LOGI(TAG, "RainMaker connectivity update maps to Matter endpoint=%u connected=%d", endpoint_id, connected);
    return app_rainmaker_matter_set_online_state(endpoint_id, connected);
}

static esp_err_t attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, int value)
{
    esp_err_t err = ESP_OK;
    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);
    ESP_RETURN_ON_FALSE(attribute, ESP_ERR_NOT_FOUND, TAG,
                        "RainMaker->Matter attribute missing: endpoint=%u cluster=0x%08lx attribute=0x%08lx value=%d",
                        endpoint_id, cluster_id, attribute_id, value);

    esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
    ESP_RETURN_ON_ERROR(attribute::get_val(attribute, &val), TAG,
                        "Failed to read Matter attribute: endpoint=%u cluster=0x%08lx attribute=0x%08lx",
                        endpoint_id, cluster_id, attribute_id);

    if (cluster_id == OnOff::Id && attribute_id == OnOff::Attributes::OnOff::Id) {
        if (val.val.b == (bool)value) {
            ESP_LOGI(TAG, "RainMaker->Matter OnOff unchanged: endpoint=%u value=%d", endpoint_id, val.val.b);
            return err;
        }
        ESP_LOGI(TAG, "RainMaker->Matter OnOff update: endpoint=%u old=%d new=%d", endpoint_id, val.val.b, (bool)value);
        val.val.b = (bool)value;
    } else if (cluster_id == LevelControl::Id && attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
        uint8_t matter_value = REMAP_TO_RANGE(value, RMAKER_LEVEL_MAX_VALUE, MATTER_LEVEL_MAX_VALUE);
        if (val.val.u8 == matter_value) {
            ESP_LOGI(TAG, "RainMaker->Matter Level unchanged: endpoint=%u rmaker=%d matter=%u",
                     endpoint_id, value, val.val.u8);
            return err;
        }
        ESP_LOGI(TAG, "RainMaker->Matter Level update: endpoint=%u rmaker=%d old=%u new=%u",
                 endpoint_id, value, val.val.u8, matter_value);
        val.val.u8 = matter_value;
    } else if (cluster_id == ColorControl::Id && attribute_id == ColorControl::Attributes::CurrentHue::Id) {
        uint8_t matter_value = REMAP_TO_RANGE(value, RMAKER_HUE_MAX_VALUE, MATTER_HUE_MAX_VALUE);
        if (val.val.u8 == matter_value) {
            ESP_LOGI(TAG, "RainMaker->Matter Hue unchanged: endpoint=%u rmaker=%d matter=%u",
                     endpoint_id, value, val.val.u8);
            return err;
        }
        ESP_LOGI(TAG, "RainMaker->Matter Hue update: endpoint=%u rmaker=%d old=%u new=%u",
                 endpoint_id, value, val.val.u8, matter_value);
        val.val.u8 = matter_value;
    } else if (cluster_id == ColorControl::Id && attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
        uint8_t matter_value = REMAP_TO_RANGE(value, RMAKER_SATURATION_MAX_VALUE, MATTER_SATURATION_MAX_VALUE);
        if (val.val.u8 == matter_value) {
            ESP_LOGI(TAG, "RainMaker->Matter Saturation unchanged: endpoint=%u rmaker=%d matter=%u",
                     endpoint_id, value, val.val.u8);
            return err;
        }
        ESP_LOGI(TAG, "RainMaker->Matter Saturation update: endpoint=%u rmaker=%d old=%u new=%u",
                 endpoint_id, value, val.val.u8, matter_value);
        val.val.u8 = matter_value;
    } else if (cluster_id == ColorControl::Id && attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
        uint16_t matter_value = REMAP_TO_RANGE_INVERSE(value, STANDARD_TEMPERATURE_FACTOR);
        if (val.val.u16 == matter_value) {
            ESP_LOGI(TAG, "RainMaker->Matter CCT unchanged: endpoint=%u rmaker=%d matter=%u",
                     endpoint_id, value, val.val.u16);
            return err;
        }
        ESP_LOGI(TAG, "RainMaker->Matter CCT update: endpoint=%u rmaker=%d old=%u new=%u",
                 endpoint_id, value, val.val.u16, matter_value);
        val.val.u16 = matter_value;
    } else if (cluster_id == WindowCovering::Id) {
        /* TODO: windowcovering was not supported yet */
        return err;
    }

    err = attribute::report(endpoint_id, cluster_id, attribute_id, &val);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "RainMaker->Matter attribute report: endpoint=%u cluster=0x%08lx attribute=0x%08lx err=%d",
                 endpoint_id, cluster_id, attribute_id, err);
    } else {
        ESP_LOGE(TAG, "RainMaker->Matter attribute report failed: endpoint=%u cluster=0x%08lx attribute=0x%08lx err=%d",
                 endpoint_id, cluster_id, attribute_id, err);
    }

    return err;
}

esp_err_t app_map_matter_attribute_to_rainmaker(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                                esp_matter_attr_val_t *val)
{
    const char *node_id = app_rainmaker_matter_get_node_id_by_matter_endpoint_id(endpoint_id);
    const char *node_name = app_rainmaker_matter_get_node_name_by_matter_endpoint_id(endpoint_id);
    if (node_id == nullptr || node_name == nullptr) {
        return ESP_OK;
    }

    rainmaker_controller_param_update_t update = {};
    if (cluster_id == OnOff::Id) {
        if (attribute_id == OnOff::Attributes::OnOff::Id) {
            update.name = "Power";
            update.type = RAINMAKER_CONTROLLER_PARAM_TYPE_BOOL;
            update.value.b = val->val.b;
        }
    } else if (cluster_id == LevelControl::Id) {
        if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
            attribute_t *attribute = attribute::get(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id);
            esp_matter_attr_val_t val_onoff = esp_matter_invalid(nullptr);
            ESP_RETURN_ON_FALSE(attribute, ESP_ERR_NOT_FOUND, TAG, "Matter OnOff attribute not found for endpoint=%u",
                                endpoint_id);
            ESP_RETURN_ON_ERROR(attribute::get_val(attribute, &val_onoff), TAG,
                                "Failed to read Matter OnOff attribute for endpoint=%u", endpoint_id);
            if (val_onoff.val.b == false) {
                return ESP_OK;
            }
            update.name = "Brightness";
            update.type = RAINMAKER_CONTROLLER_PARAM_TYPE_INT;
            update.value.i = REMAP_TO_RANGE(val->val.u8, MATTER_LEVEL_MAX_VALUE, RMAKER_LEVEL_MAX_VALUE);
        }
    } else if (cluster_id == ColorControl::Id) {
        if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
            update.name = "Hue";
            update.type = RAINMAKER_CONTROLLER_PARAM_TYPE_INT;
            update.value.i = REMAP_TO_RANGE(val->val.u8, MATTER_HUE_MAX_VALUE, RMAKER_HUE_MAX_VALUE);
        } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
            update.name = "Saturation";
            update.type = RAINMAKER_CONTROLLER_PARAM_TYPE_INT;
            update.value.i = REMAP_TO_RANGE(val->val.u8, MATTER_SATURATION_MAX_VALUE, RMAKER_SATURATION_MAX_VALUE);
        } else if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
            update.name = "CCT";
            update.type = RAINMAKER_CONTROLLER_PARAM_TYPE_INT;
            update.value.i = REMAP_TO_RANGE_INVERSE(val->val.u16, STANDARD_TEMPERATURE_FACTOR);
        }
    }

    if (!update.name) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Matter write maps to RainMaker node %s device %s param %s", node_id, node_name, update.name);
    return rainmaker_controller_set_node_params(node_id, node_name, &update, 1);
}

static esp_err_t app_map_rainmaker_params_to_matter_attributes(uint16_t endpoint_id, const char *node_name,
                                                               const char *params, size_t params_len)
{
    esp_err_t err = ESP_FAIL;
    cJSON *root = nullptr;
    cJSON *device = nullptr;
    cJSON *item = nullptr;

    ESP_RETURN_ON_FALSE(params, ESP_ERR_INVALID_ARG, TAG, "RainMaker params payload is null");
    root = cJSON_ParseWithLength(params, params_len);
    ESP_RETURN_ON_FALSE(root, ESP_FAIL, TAG, "Failed to parse RainMaker params payload");

    if (!node_name || node_name[0] == 0) {
        /* TODO: add other device types later */
        err = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    device = cJSON_GetObjectItem(root, node_name);
    if (!cJSON_IsObject(device)) {
        ESP_LOGI(TAG, "RainMaker update has no %s service params; ignore non-device update", node_name);
        err = ESP_ERR_NOT_FOUND;
        goto cleanup;
    }

    ESP_LOGI(TAG, "RainMaker->Matter parse params: endpoint=%u device=%s payload=%.*s",
             endpoint_id, node_name, (int)params_len, params);
    item = cJSON_GetObjectItem(device, "Brightness");
    if (cJSON_IsNumber(item)) {
        int attribute_value = item->valueint;
        if (attribute_value > 0) {
            ESP_LOGI(TAG, "RainMaker param Brightness=%d", attribute_value);
            attribute_update(endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, attribute_value);
        } else {
            ESP_LOGI(TAG, "RainMaker param Brightness=%d ignored because value is not positive", attribute_value);
        }
    }

    item = cJSON_GetObjectItem(device, "Hue");
    if (cJSON_IsNumber(item)) {
        int attribute_value = item->valueint;
        ESP_LOGI(TAG, "RainMaker param Hue=%d", attribute_value);
        attribute_update(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentHue::Id, attribute_value);
    }

    item = cJSON_GetObjectItem(device, "Power");
    if (cJSON_IsBool(item)) {
        int attribute_value = cJSON_IsTrue(item);
        ESP_LOGI(TAG, "RainMaker param Power=%d", attribute_value);
        attribute_update(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, attribute_value);
    }

    item = cJSON_GetObjectItem(device, "Saturation");
    if (cJSON_IsNumber(item)) {
        int attribute_value = item->valueint;
        ESP_LOGI(TAG, "RainMaker param Saturation=%d", attribute_value);
        attribute_update(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentSaturation::Id, attribute_value);
    }

    item = cJSON_GetObjectItem(device, "CCT");
    if (cJSON_IsNumber(item)) {
        int attribute_value = item->valueint;
        ESP_LOGI(TAG, "RainMaker param CCT=%d", attribute_value);
        attribute_update(endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id, attribute_value);
    }

    err = ESP_OK;

cleanup:
    cJSON_Delete(root);

    return err;
}

static uint32_t app_rainmaker_get_matter_device_type(const char *config, size_t config_len,
                                                     char *node_name, size_t node_name_len)
{
    esp_err_t ret = ESP_OK;
    cJSON *root = nullptr;
    cJSON *devices = nullptr;
    cJSON *device = nullptr;
    cJSON *type = nullptr;
    cJSON *params = nullptr;
    cJSON *param = nullptr;
    cJSON *name = nullptr;
    uint32_t matter_device_type_id = APP_RAINMAKER_INVALID_MATTER_DEVICE_TYPE;

    root = cJSON_ParseWithLength(config, config_len);
    ESP_RETURN_ON_FALSE(root, APP_RAINMAKER_INVALID_MATTER_DEVICE_TYPE, TAG,
                        "Failed to parse RainMaker node config");

    devices = cJSON_GetObjectItem(root, "devices");
    ESP_GOTO_ON_FALSE(cJSON_IsArray(devices), ESP_FAIL, cleanup, TAG, "No devices found in RainMaker node config");

    device = cJSON_GetArrayItem(devices, 0);
    ESP_GOTO_ON_FALSE(cJSON_IsObject(device), ESP_FAIL, cleanup, TAG, "No device object found in RainMaker node config");

    type = cJSON_GetObjectItem(device, "type");
    ESP_GOTO_ON_FALSE(cJSON_IsString(type) && type->valuestring, ESP_FAIL, read_node_name, TAG,
                      "No device type found in RainMaker node config");

    if ((strcmp(type->valuestring, ESP_RMAKER_DEVICE_LIGHTBULB) != 0) &&
            (strcmp(type->valuestring, ESP_RMAKER_DEVICE_LIGHT) != 0)) {
        /* TODO: add other device types */
        ESP_LOGW(TAG, "Unsupported RainMaker device type: %s", type->valuestring);
        goto read_node_name;
    }

    matter_device_type_id = ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID;
    params = cJSON_GetObjectItem(device, "params");
    ESP_GOTO_ON_FALSE(cJSON_IsArray(params), ESP_FAIL, read_node_name, TAG,
                      "No params found in RainMaker node config");

    cJSON_ArrayForEach(param, params) {
        cJSON *param_type = cJSON_IsObject(param) ? cJSON_GetObjectItem(param, "type") : nullptr;
        if (!cJSON_IsString(param_type) || !param_type->valuestring) {
            continue;
        }
        if (((strcmp(param_type->valuestring, ESP_RMAKER_PARAM_HUE) == 0) ||
                (strcmp(param_type->valuestring, ESP_RMAKER_PARAM_SATURATION) == 0)) &&
                (matter_device_type_id < ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID)) {
            matter_device_type_id = ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID;
        } else if ((strcmp(param_type->valuestring, ESP_RMAKER_PARAM_CCT) == 0) &&
                   (matter_device_type_id < ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID)) {
            matter_device_type_id = ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID;
        } else if (((strcmp(param_type->valuestring, ESP_RMAKER_PARAM_BRIGHTNESS) == 0) ||
                    (strcmp(param_type->valuestring, ESP_RMAKER_PARAM_INTENSITY) == 0)) &&
                   (matter_device_type_id < ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID)) {
            matter_device_type_id = ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID;
        }
    }

read_node_name:
    name = cJSON_GetObjectItem(device, "name");
    if (!cJSON_IsString(name) || !name->valuestring) {
        ESP_LOGE(TAG, "Failed to get RainMaker node name");
    } else {
        snprintf(node_name, node_name_len, "%s", name->valuestring);
    }

cleanup:
    cJSON_Delete(root);

    (void)ret;
    return matter_device_type_id;
}
