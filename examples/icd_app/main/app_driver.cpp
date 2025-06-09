/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_log.h>
#include <esp_matter.h>
#include <sdkconfig.h>

#include <app/icd/server/ICDNotifier.h>

#include <app_priv.h>
#include <iot_button.h>
#include <button_gpio.h>

#ifdef CONFIG_ENABLE_USER_ACTIVE_MODE_TRIGGER_BUTTON
using namespace chip::app::Clusters;
using namespace esp_matter;

static constexpr char *TAG = "app_driver";

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    // The device will stay active mode for Active Mode Threshold
    chip::DeviceLayer::PlatformMgr().ScheduleWork([](intptr_t) {
        chip::app::ICDNotifier::GetInstance().NotifyNetworkActivityNotification();
    });
}

app_driver_handle_t app_driver_button_init()
{
    /* Initialize button */
    button_handle_t handle = NULL;
    const button_config_t btn_cfg = {0};
    const button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = CONFIG_USER_ACTIVE_MODE_TRIGGER_BUTTON_PIN,
        .active_level = 0,
        .enable_power_save = true,
    };

    if (iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &handle) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create button device");
        return NULL;
    }

    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, NULL, app_driver_button_toggle_cb, NULL);
    return (app_driver_handle_t)handle;
}

#endif // CONFIG_ENABLE_USER_ACTIVE_MODE_TRIGGER_BUTTON
