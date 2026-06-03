/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <app_bridged_device.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_bridge.h>

#include <app_zigbee_bridged_device.h>
#include <zigbee_bridge.h>

static const char *TAG = "zigbee_bridge";

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

extern uint16_t aggregator_endpoint_id;

void zigbee_bridge_find_bridged_on_off_light_cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx)
{
    ESP_LOGI(TAG, "on_off_light found: address:0x%" PRIx16 ", endpoint:%" PRId8 ", response_status:%d", addr, endpoint, zdo_status);
    if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
        node_t *node = node::get();
        if (!node) {
            ESP_LOGE(TAG, "Could not find esp_matter node");
            return;
        }
        zigbee_device_addr_t zigbee_addr = {
            .endpoint_id = endpoint,
            .shortaddr = addr,
        };
        if (app_bridge_get_device(&zigbee_addr)) {
            ESP_LOGI(TAG, "Bridged node for 0x%04" PRIx16 " zigbee device on endpoint %" PRId16 " has been created", addr,
                     app_bridge_get_endpoint(&zigbee_addr));
        } else {
            VerifyOrReturn(app_bridge_create_new_device(node, aggregator_endpoint_id, ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID, &zigbee_addr, nullptr) == ESP_OK);
            ESP_LOGI(TAG, "Create/Update bridged node for 0x%04" PRIx16 " zigbee device on endpoint %" PRId16 "", addr,
                     app_bridge_get_endpoint(&zigbee_addr));
        }
    }
}

esp_err_t zigbee_bridge_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                         esp_matter_attr_val_t *val, app_bridged_device_t *zigbee_device)
{
    esp_matter_bridge::device_t *matter_device = zigbee_device ? zigbee_device->get_matter_device() : nullptr;
    if (matter_device && matter_device->endpoint) {
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                ESP_LOGD(TAG, "Update Bridged Device, ep: %" PRId16 ", cluster: %" PRId32 ", att: %" PRId32 "", endpoint_id, cluster_id,
                         attribute_id);
                esp_zb_zcl_on_off_cmd_t cmd_req;
                zigbee_device_addr_t *zigbee_device_addr = (zigbee_device_addr_t *)zigbee_device->get_dev_addr();
                cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = zigbee_device_addr->shortaddr;
                cmd_req.zcl_basic_cmd.dst_endpoint = zigbee_device_addr->endpoint_id;
                cmd_req.zcl_basic_cmd.src_endpoint = esp_matter::endpoint::get_id(matter_device->endpoint);
                cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
                cmd_req.on_off_cmd_id = val->val.b ? ESP_ZB_ZCL_CMD_ON_OFF_ON_ID : ESP_ZB_ZCL_CMD_ON_OFF_OFF_ID;
                if (esp_zb_lock_acquire(portMAX_DELAY)) {
                    esp_zb_zcl_on_off_cmd_req(&cmd_req);
                    esp_zb_lock_release();
                }
            }
        }
    } else {
        ESP_LOGE(TAG, "Unable to Update Bridge Device, ep: %" PRId16 ", cluster: %" PRId32 ", att: %" PRId32 "", endpoint_id, cluster_id, attribute_id);
    }
    return ESP_OK;
}
