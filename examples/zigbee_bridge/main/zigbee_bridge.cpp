/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_err.h"
#include "esp_matter_bridge.h"
#include <esp_check.h>
#include <esp_log.h>
#include <esp_matter.h>

#include <app_zigbee_bridge_device.h>
#include <app_zboss.h>
#include <esp_matter_core.h>
#include <zboss_api_buf.h>
#include <zigbee_bridge.h>

static const char *TAG = "zigbee_bridge";

using namespace esp_matter;
using namespace esp_matter::cluster;

static esp_err_t init_bridged_onoff_light(esp_matter_bridge_device_t *dev)
{
    if (!dev) {
        ESP_LOGE(TAG, "Invalid bridge device to be initialized");
        return ESP_ERR_INVALID_ARG;
    }
    on_off::config_t config;
    on_off::create(dev->endpoint, &config, CLUSTER_MASK_SERVER, ESP_MATTER_NONE_FEATURE_ID);
    endpoint::set_device_type_id(dev->endpoint, endpoint::on_off_light::get_device_type_id());
    if (endpoint::enable(dev->endpoint) != ESP_OK) {
        ESP_LOGE(TAG, "ESP Matter enable dynamic endpoint failed");
        endpoint::destroy(dev->node, dev->endpoint);
        return ESP_FAIL;
    }
    return ESP_OK;
}

void zigbee_bridge_match_bridged_onoff_light_cb(zb_bufid_t bufid)
{
    zb_zdo_match_desc_resp_t *p_resp = (zb_zdo_match_desc_resp_t *)zb_buf_begin(bufid);
    zb_apsde_data_indication_t *p_ind = ZB_BUF_GET_PARAM(bufid, zb_apsde_data_indication_t);
    zb_uint8_t *p_match_ep;
    zb_ret_t zb_err_code;
    esp_err_t ret = ESP_OK;

    if ((p_resp->status == ZB_ZDP_STATUS_SUCCESS) && (p_resp->match_len > 0)) {
        p_match_ep = (zb_uint8_t *)(p_resp + 1);
        node_t *node = node::get();
        ESP_GOTO_ON_FALSE(node, ESP_ERR_INVALID_STATE, exit, TAG, "Could not find esp_matter node");
        if (app_bridge_get_zigbee_device_by_zigbee_shortaddr(p_ind->src_addr)) {
            ESP_LOGI(TAG, "Bridged node for 0x%04x zigbee device on endpoint %d has been created", p_ind->src_addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(p_ind->src_addr));
        } else {
            app_zigbee_bridge_device_t *dev = app_bridge_create_zigbee_device(node, *p_match_ep, p_ind->src_addr);
            ESP_GOTO_ON_FALSE(dev, ESP_FAIL, exit, TAG, "Failed to create zigbee bridged device (on_off light)");
            ESP_GOTO_ON_ERROR(init_bridged_onoff_light(dev->dev), exit, TAG, "Failed to initialize the bridged node");
            ESP_LOGI(TAG, "Create/Update bridged node for 0x%04x zigbee device on endpoint %d", p_ind->src_addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(p_ind->src_addr));
        }
        zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(zigbee_bridge_match_bridged_onoff_light_timeout, ZB_ALARM_ANY_PARAM);
        if (zb_err_code != RET_OK) {
            ESP_LOGE(TAG, "Failed to cancel alarm for match_bridged_device_timeout");
        }
    }
exit:
    if (bufid) {
        zb_buf_free(bufid);
    }
}

void zigbee_bridge_match_bridged_onoff_light(zb_bufid_t bufid)
{
    zb_zdo_match_desc_param_t *p_req;

    zb_zdo_app_signal_hdr_t *p_sg_p = NULL;
    zb_get_app_signal(bufid, &p_sg_p);
    zb_zdo_signal_device_annce_params_t *dev_annce_params =
        ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_device_annce_params_t);

    zb_uint16_t shortaddr = dev_annce_params->device_short_addr;
    p_req = (zb_zdo_match_desc_param_t *)zb_buf_initial_alloc(
        bufid, sizeof(zb_zdo_match_desc_param_t) + (1) * sizeof(zb_uint16_t));
    p_req->nwk_addr = shortaddr;
    p_req->addr_of_interest = shortaddr;
    p_req->profile_id = ZB_AF_HA_PROFILE_ID;

    p_req->num_in_clusters = 1;
    p_req->num_out_clusters = 0;
    p_req->cluster_list[0] = ZB_ZCL_CLUSTER_ID_ON_OFF;

    zb_zdo_match_desc_req(bufid, zigbee_bridge_match_bridged_onoff_light_cb);
}

void zigbee_bridge_match_bridged_onoff_light_timeout(zb_bufid_t bufid)
{
    ESP_LOGE(TAG, "The device is not an onoff light");
    if (bufid) {
        zb_buf_free(bufid);
    }
}

void zigbee_bridge_send_on(zb_uint8_t buf, zb_uint16_t zigbee_shortaddr)
{
    app_zigbee_bridge_device_t *dev = app_bridge_get_zigbee_device_by_zigbee_shortaddr(zigbee_shortaddr);
    ZB_ZCL_ON_OFF_SEND_REQ(buf, zigbee_shortaddr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dev->zigbee_endpointid,
                           dev->dev->endpoint_id, ZB_AF_HA_PROFILE_ID, ZB_ZCL_DISABLE_DEFAULT_RESPONSE,
                           ZB_ZCL_CMD_ON_OFF_ON_ID, NULL);
}

void zigbee_bridge_send_off(zb_uint8_t buf, zb_uint16_t zigbee_shortaddr)
{
    app_zigbee_bridge_device_t *dev = app_bridge_get_zigbee_device_by_zigbee_shortaddr(zigbee_shortaddr);
    ZB_ZCL_ON_OFF_SEND_REQ(buf, zigbee_shortaddr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dev->zigbee_endpointid,
                           dev->dev->endpoint_id, ZB_AF_HA_PROFILE_ID, ZB_ZCL_DISABLE_DEFAULT_RESPONSE,
                           ZB_ZCL_CMD_ON_OFF_OFF_ID, NULL);
}

esp_err_t zigbee_bridge_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t *val)
{
    app_zigbee_bridge_device_t *zigbee_device = app_bridge_get_zigbee_device_by_matter_endpointid(endpoint_id);
    if (zigbee_device && zigbee_device->dev && zigbee_device->dev->endpoint) {
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                zb_buf_get_out_delayed_ext((val->val.b ? zigbee_bridge_send_on : zigbee_bridge_send_off),
                                           zigbee_device->zigbee_shortaddr, 0);
            }
        }
    }
    return ESP_OK;
}
