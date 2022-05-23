/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/* It is recommended to copy this code in your example so that you can modify as per your application's needs,
 * especially for the indicator calbacks, button_factory_reset_pressed_cb() and button_factory_reset_released_cb().
 */

#include <esp_log.h>

#include <esp_bt.h>
#include <esp_nimble_hci.h>
#include <host/ble_hs.h>
#include <nimble/nimble_port.h>

static const char *TAG = "app_ble";

esp_err_t app_ble_disable()
{
#if CONFIG_BT_NIMBLE_ENABLED && CONFIG_DEINIT_BLE_ON_COMMISSIONING_COMPLETE
    if (!ble_hs_is_enabled()) {
        ESP_LOGI(TAG, "BLE already deinited");
        return ESP_OK;
    }

    int ret = nimble_port_stop();
    if (ret != 0) {
        ESP_LOGE(TAG, "nimble_port_stop() failed");
        return ESP_FAIL;
    }
    nimble_port_deinit();
    esp_err_t err = esp_nimble_hci_and_controller_deinit();
    err |= esp_bt_mem_release(ESP_BT_MODE_BLE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "BLE deinit failed");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "BLE deinit successful and memory reclaimed");
#endif
    return ESP_OK;
}
