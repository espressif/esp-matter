/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_check.h>
#include <esp_log.h>
#include <esp_mac.h>

#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_bridge.h>

#include <espnow_bridge.h>
#include <app_bridged_device.h>

static const char *TAG = "espnow_bridge";

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;
extern uint16_t aggregator_endpoint_id;

esp_err_t espnow_bridge_match_bridged_switch(uint8_t espnow_macaddr[6], uint16_t espnow_initiator_attr, uint32_t matter_device_type_id)
{
    ESP_LOGI(TAG, "espnow switch found: " MACSTR ", initiator type: %d", MAC2STR(espnow_macaddr), espnow_initiator_attr);
    node_t *node = node::get();
    ESP_RETURN_ON_FALSE(node, ESP_ERR_INVALID_STATE, TAG, "Could not find esp_matter node");

    if (app_bridge_get_device_by_espnow_macaddr(espnow_macaddr)) {
        ESP_LOGI(TAG, "Bridged node for " MACSTR " ESP-NOW device on endpoint %d has been created", MAC2STR(espnow_macaddr),
                app_bridge_get_matter_endpointid_by_espnow_macaddr(espnow_macaddr));
    } else {
        app_bridged_device_t *bridged_device =
            app_bridge_create_bridged_device(node, aggregator_endpoint_id, matter_device_type_id,
                                             ESP_MATTER_BRIDGED_DEVICE_TYPE_ESPNOW,
                                             app_bridge_espnow_address(espnow_macaddr, espnow_initiator_attr), NULL);
        ESP_RETURN_ON_FALSE(bridged_device, ESP_FAIL, TAG, "Failed to create bridged device (espnow switch)");
        ESP_LOGI(TAG, "Create/Update bridged node for " MACSTR " bridged device on endpoint %d", MAC2STR(espnow_macaddr),
                app_bridge_get_matter_endpointid_by_espnow_macaddr(espnow_macaddr));
    }
    return ESP_OK;
}

esp_err_t espnow_bridge_remove_bridged_switch(uint8_t espnow_macaddr[6])
{
    node_t *node = node::get();
    ESP_RETURN_ON_FALSE(node, ESP_ERR_INVALID_STATE, TAG, "Could not find esp_matter node");

    app_bridged_device_t *bridged_device = app_bridge_get_device_by_espnow_macaddr(espnow_macaddr);
    if (bridged_device) {
        app_bridge_remove_device(bridged_device);
        ESP_LOGI(TAG, "Bridged ESP-NOW switch removed: " MACSTR, MAC2STR(espnow_macaddr));
    } else {
        ESP_LOGI(TAG, "Bridged ESP-NOW switch not found: " MACSTR, MAC2STR(espnow_macaddr));
    }
    return ESP_OK;
}
