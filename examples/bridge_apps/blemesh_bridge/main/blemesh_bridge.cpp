/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_check.h>
#include <esp_log.h>

#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_bridge.h>

#include <app_bridged_device.h>
#include <blemesh_bridge.h>
#include <app_blemesh.h>

static const char *TAG = "blemesh_bridge";

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;
extern uint16_t aggregator_endpoint_id;


/** Mesh Spec 4.2.1: "The Composition Data state contains information about a node, 
 * the elements it includes, and the supported models. Composition Data Page 0 is mandatory." 
 * Composition Data Page 0 can be used to determine device type. 
 * The following Composition Data Page 0 describes the information:
 *    CID is 0x02E5; PID is 0x0000; VID is 0x0000
 *    CRPL is 0x000A
 *    Features is 0x0003 – Relay and Friend features.
 *    Loc is “unknown” – 0x0000
 *    NumS is 2; NumV is 0
 *    The Bluetooth SIG Models supported are:
 *     - 0x0000: Config Server Model
 *     - 0x1000: Generic OnOff Server Model */
static uint8_t expect_composition[] = {/* CID */0XE5, 0x02, /* PID */0x00, 0x00, /* VID */0x00, 0x00,
                                    /* CRPL */0X0A, 0x00, /* Features */0x03, 0x00, /* Loc */0x00, 0x00,
                                    /* NumS */0x02, /* NumV */0x00, /* Config Server Model */0x00, 0x00,
                                    /* Generic OnOff Server Model */0x00, 0x10};

esp_err_t blemesh_bridge_match_bridged_onoff_light(uint8_t *composition_data, uint16_t blemesh_addr)
{
    /** Compare Composition Data Page 0 to find expected device */
    if (!memcmp(composition_data, expect_composition, sizeof(expect_composition))) {
        ESP_LOGI(TAG, "This is an expected device ...");
        node_t *node = node::get();
        ESP_RETURN_ON_FALSE(node, ESP_ERR_INVALID_STATE, TAG, "Could not find esp_matter node");
        if (app_bridge_get_device_by_blemesh_addr(blemesh_addr)) {
            ESP_LOGI(TAG, "Bridged node for 0x%04x bridged device on endpoint %d has been created", blemesh_addr,
                    app_bridge_get_matter_endpointid_by_blemesh_addr(blemesh_addr));
        } else {
            app_bridged_device_t *bridged_device =
                app_bridge_create_bridged_device(node, aggregator_endpoint_id, ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID,
                                                 ESP_MATTER_BRIDGED_DEVICE_TYPE_BLEMESH,
                                                 app_bridge_blemesh_address(blemesh_addr), NULL);
            ESP_RETURN_ON_FALSE(bridged_device, ESP_FAIL, TAG, "Failed to create bridged device (on_off light)");
            ESP_LOGI(TAG, "Create/Update bridged node for 0x%04x bridged device on endpoint %d", blemesh_addr,
                    app_bridge_get_matter_endpointid_by_blemesh_addr(blemesh_addr));
        }
    } else {
        ESP_LOGW(TAG, "This isn't an unexpected device ...");
    }
    return ESP_OK;
}

esp_err_t blemesh_bridge_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                          esp_matter_attr_val_t *val, app_bridged_device_t *bridged_device)
{
    if (bridged_device && bridged_device->dev && bridged_device->dev->endpoint) {
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                ESP_LOGD(TAG, "Update Bridged Device, ep: 0x%x, cluster: 0x%lx, att: 0x%lx", endpoint_id, cluster_id,
                         attribute_id);
                app_ble_mesh_onoff_set(bridged_device->dev_addr.blemesh_addr, val->val.b);
            }
        }
    }
    else{
        ESP_LOGE(TAG, "Unable to Update Bridge Device, ep: 0x%x, cluster: 0x%lx, att: 0x%lx", endpoint_id, cluster_id,
                 attribute_id);
    }
    return ESP_OK;
}

/** ToDo: Implement some keep-alive logic in BLE mesh 
 * so that we can remove them when they are offline */
