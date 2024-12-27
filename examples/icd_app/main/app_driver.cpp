/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_log.h>
#include <esp_matter.h>
#include <iot_button.h>
#include <sdkconfig.h>

#include <app/icd/server/ICDNotifier.h>

#include <app_priv.h>

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
    button_config_t config;
    memset(&config, 0, sizeof(button_config_t));
    config.type = BUTTON_TYPE_GPIO;
    config.gpio_button_config.gpio_num = CONFIG_USER_ACTIVE_MODE_TRIGGER_BUTTON_PIN;
    config.gpio_button_config.active_level = 0;
    config.gpio_button_config.enable_power_save = true;
    button_handle_t handle = iot_button_create(&config);

    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, app_driver_button_toggle_cb, NULL);
    return (app_driver_handle_t)handle;
}

#endif // CONFIG_ENABLE_USER_ACTIVE_MODE_TRIGGER_BUTTON
