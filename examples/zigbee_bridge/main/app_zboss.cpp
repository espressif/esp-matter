/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <app_zboss.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_zigbee_api_core.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <zigbee_bridge.h>

#if (!defined(ZB_MACSPLIT_HOST) && defined(ZB_MACSPLIT_DEVICE))
#error "Zigbee host option should be enabled to use this example"
#endif

static const char *TAG = "esp_zboss";

static void bdb_start_top_level_commissioning_cb(zb_uint8_t mode_mask)
{
    ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(mode_mask));
}

/**
 * @brief Zigbee stack event handler.
 *
 * @param bufid   Reference to the Zigbee stack buffer used to pass signal.
 */

void zboss_signal_handler(zb_bufid_t bufid)
{
    // Read signal desription
    zb_zdo_app_signal_hdr_t *p_sg_p = NULL;
    zb_zdo_app_signal_type_t sig = zb_get_app_signal(bufid, &p_sg_p);
    zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(bufid);
    zb_zdo_signal_device_annce_params_t *device_annce_params = NULL;
    zb_zdo_signal_macsplit_dev_boot_params_t *rcp_version = NULL;

    switch (sig) {
    case ZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Zigbee stack initialized");
        esp_zb_bdb_start_top_level_commissioning(ZB_BDB_INITIALIZATION);
        break;

    case ZB_MACSPLIT_DEVICE_BOOT:
        ESP_LOGI(TAG, "Zigbee rcp device booted");
        rcp_version = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_macsplit_dev_boot_params_t);
        ESP_LOGI(TAG, "Zigbee rcp device version: %d.%d.%d", (rcp_version->dev_version >> 24 & 0x000000FF),
                 (rcp_version->dev_version >> 16 & 0x000000FF), (rcp_version->dev_version & 0x000000FF));
        break;

    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        if (status == RET_OK) {
            ESP_LOGI(TAG, "Start network formation");
            esp_zb_bdb_start_top_level_commissioning(ZB_BDB_NETWORK_FORMATION);
        } else {
            ESP_LOGE(TAG, "Failed to initialize Zigbee stack (status: %d)", status);
        }
        break;

    case ZB_BDB_SIGNAL_FORMATION:
        if (status == RET_OK) {
            zb_ieee_addr_t ieee_address;
            zb_get_long_address(ieee_address);
            ESP_LOGI(TAG, "Formed network successfully");
            ESP_LOGI(TAG, "ieee extended address: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx)",
                     ieee_address[7], ieee_address[6], ieee_address[5], ieee_address[4], ieee_address[3],
                     ieee_address[2], ieee_address[1], ieee_address[0], ZB_PIBCACHE_PAN_ID());
            esp_zb_bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        } else {
            ESP_LOGI(TAG, "Restart network formation (status: %d)", status);
            ZB_SCHEDULE_APP_ALARM((zb_callback_t)bdb_start_top_level_commissioning_cb, ZB_BDB_NETWORK_FORMATION,
                                  ZB_TIME_ONE_SECOND);
        }
        break;

    case ZB_BDB_SIGNAL_STEERING:
        if (status == RET_OK) {
            ESP_LOGI(TAG, "Network steering started");
        }
        break;

    case ZB_ZDO_SIGNAL_DEVICE_ANNCE:
        device_annce_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_device_annce_params_t);
        ESP_LOGI(TAG, "New device commissioned or rejoined (short: 0x%04hx)", device_annce_params->device_short_addr);
        esp_zb_zdo_match_desc_req_param_t cmd_req;
        cmd_req.dst_nwk_addr = device_annce_params->device_short_addr;
        cmd_req.addr_of_interest = device_annce_params->device_short_addr;
        esp_zb_zdo_find_on_off_light(&cmd_req, zigbee_bridge_find_bridged_on_off_light_cb);
        break;

    default:
        ESP_LOGI(TAG, "status: %d", status);
        break;
    }
    /* All callbacks should either reuse or free passed buffers. If bufid == 0, the buffer is invalid (not passed) */
    if (bufid) {
        zb_buf_free(bufid);
    }
}

static void zboss_task(void *pvParameters)
{
    /* initialize Zigbee stack with Zigbee coordinator config */
    esp_zb_cfg_t zb_nwk_cfg = ESP_ZB_ZC_CONFIG();
    esp_zb_init(&zb_nwk_cfg);
    /* initiate Zigbee Stack start without zb_send_no_autostart_signal auto-start */
    ESP_ERROR_CHECK(esp_zb_start(false));
    esp_zb_main_loop_iteration();
}

void launch_app_zboss(void)
{
    esp_zb_platform_config_t config = {
        .radio_config = ESP_ZB_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_ZB_DEFAULT_HOST_CONFIG(),
    };
    /* load Zigbee gateway platform config to initialization */
    ESP_ERROR_CHECK(esp_zb_platform_config(&config));
    xTaskCreate(zboss_task, "zboss_main", 10240, xTaskGetCurrentTaskHandle(), 5, NULL);
}
