/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "zigbee_bridge.h"
#include <app_zboss.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <zboss_api.h>

#if (!defined ZB_MACSPLIT_HOST)
#error "Zigbee host option should be enabled to use this example"
#endif

static const char *TAG = "esp_zboss";

static void bdb_start_top_level_commissioning_cb(zb_uint8_t mode_mask)
{
    if (!bdb_start_top_level_commissioning(mode_mask)) {
        ESP_LOGE(TAG, "In BDB commissioning, an error occurred (for example: the device has already been running)");
    }
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

    switch (sig) {
    case ZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Zigbee stack initialized");
        bdb_start_top_level_commissioning(ZB_BDB_INITIALIZATION);
        break;

    case ZB_MACSPLIT_DEVICE_BOOT:
        ESP_LOGI(TAG, "Zigbee rcp device booted");
        break;

    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        if (status == RET_OK) {
            ESP_LOGI(TAG, "Start network formation");
            bdb_start_top_level_commissioning(ZB_BDB_NETWORK_FORMATION);
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
            bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
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
        status =
            ZB_SCHEDULE_APP_ALARM(zigbee_bridge_match_bridged_onoff_light, bufid, MATCH_BRIDGED_DEVICE_START_DELAY);
        if (status != RET_OK) {
            ESP_LOGD(TAG, "Could not start schedule alarm for matching bridged device");
        }
        status =
            ZB_SCHEDULE_APP_ALARM(zigbee_bridge_match_bridged_onoff_light_timeout, bufid, MATCH_BRIDGED_DEVICE_TIMEOUT);
        if (status != RET_OK) {
            ESP_LOGD(TAG, "Could not start schedule alarm for matching bridged device timeout");
        }
        // this buf will be free in zboss_match_bridged_device_callback/zboss_match_bridged_device_timeout later
        bufid = 0;
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

void zboss_task()
{
    ZB_INIT("zigbee bridge");
    zb_set_network_coordinator_role(IEEE_CHANNEL_MASK);
    zb_set_nvram_erase_at_start(ERASE_PERSISTENT_CONFIG);
    zb_set_max_children(MAX_CHILDREN);
    /* initiate Zigbee Stack start without zb_send_no_autostart_signal auto-start */
    ESP_ERROR_CHECK(zboss_start_no_autostart());
    while (1) {
        zboss_main_loop_iteration();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void launch_app_zboss(void)
{
    zb_esp_platform_config_t config = {
        .radio_config = ZB_ESP_DEFAULT_RADIO_CONFIG(),
        .host_config = ZB_ESP_DEFAULT_HOST_CONFIG(),
    };
    /* load Zigbee gateway platform config to initialization */
    ESP_ERROR_CHECK(zb_esp_platform_config(&config));
    xTaskCreate(zboss_task, "zboss_main", 10240, xTaskGetCurrentTaskHandle(), 5, NULL);
}
