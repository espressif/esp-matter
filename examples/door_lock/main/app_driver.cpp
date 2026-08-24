/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <driver/gpio.h>
#include <esp_log.h>

#include "bsp/esp-bsp.h"

#include <app_priv.h>
#include <bolt_lock_manager.h>
#include <platform/PlatformManager.h>

using namespace chip::app::Clusters;
using namespace chip::DeviceLayer;

static const char *TAG = "app_driver";

static button_handle_t s_button = nullptr;
static led_indicator_handle_t s_status_led = nullptr;

static_assert(BSP_BUTTON_NUM <= 1, "The door lock supports at most one BSP button");
static_assert(BSP_LED_NUM <= 1, "The door lock supports at most one BSP status LED");

static void app_driver_button_deinit()
{
    if (s_button) {
        iot_button_delete(s_button);
        s_button = nullptr;
    }
}

static void app_driver_toggle_lock([[maybe_unused]] intptr_t arg)
{
    BoltLockManager  &manager = BoltLockManager::Instance();
    switch (manager.GetState()) {
    case BoltLockManager::State::kLockingInitiated:
    case BoltLockManager::State::kLockingCompleted:
        manager.Unlock(DoorLock::OperationSourceEnum::kButton);
        break;
    case BoltLockManager::State::kUnlockingInitiated:
    case BoltLockManager::State::kUnlockingCompleted:
        manager.Lock(DoorLock::OperationSourceEnum::kButton);
        break;
    }
}

static void app_driver_button_toggle_cb([[maybe_unused]] void * button_handle, [[maybe_unused]] void * usr_data)
{
    CHIP_ERROR err = PlatformMgr().ScheduleWork(app_driver_toggle_lock);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to schedule button action: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

static esp_err_t app_driver_button_init()
{
    if (BSP_BUTTON_NUM == 0) {
        ESP_LOGW(TAG, "No lock button configured");
        return ESP_OK;
    }

    esp_err_t err = bsp_iot_button_create(&s_button, nullptr, 1);
    if (err != ESP_OK) {
        s_button = nullptr;
        return err;
    }

    err = iot_button_register_cb(s_button, BUTTON_PRESS_DOWN, nullptr, app_driver_button_toggle_cb, nullptr);
    if (err != ESP_OK) {
        app_driver_button_deinit();
    }
    return err;
}

#ifdef CONFIG_M5NANO_BOARD
static constexpr gpio_num_t status_led_power_gpio =
    static_cast<gpio_num_t>(CONFIG_M5NANO_STATUS_LED_POWER_GPIO);

static esp_err_t app_driver_status_led_power_set(bool enabled)
{
    gpio_config_t config = {};
    config.pin_bit_mask = 1ULL << static_cast<uint32_t>(status_led_power_gpio);
    config.mode = GPIO_MODE_OUTPUT;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    esp_err_t err = gpio_config(&config);
    if (err != ESP_OK) {
        return err;
    }
    return gpio_set_level(status_led_power_gpio, enabled);
}
#else
static esp_err_t app_driver_status_led_power_set([[maybe_unused]] bool enabled)
{
    return ESP_OK;
}
#endif // CONFIG_M5NANO_BOARD

static void app_driver_status_led_deinit()
{
    if (s_status_led) {
        led_indicator_delete(s_status_led);
        s_status_led = nullptr;
    }

    esp_err_t err = app_driver_status_led_power_set(false);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to disable status LED power: %d", err);
    }
}

static esp_err_t app_driver_status_led_init()
{
    if (BSP_LED_NUM == 0) {
        ESP_LOGW(TAG, "No status LED configured");
        return ESP_OK;
    }

    esp_err_t err = app_driver_status_led_power_set(true);
    if (err != ESP_OK) {
        return err;
    }

    err = bsp_led_indicator_create(&s_status_led, nullptr, 1);
    if (err != ESP_OK || !s_status_led) {
        app_driver_status_led_deinit();
        return err != ESP_OK ? err : ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

#if defined(CONFIG_BSP_LED_TYPE_RGB) || defined(CONFIG_BSP_LED_TYPE_RGB_CLASSIC)
static esp_err_t app_driver_status_led_set(BoltLockManager::State state)
{
    if (BSP_LED_NUM == 0) {
        return ESP_OK;
    }

    static constexpr uint8_t kStatusLedLevel = 0x40;
    uint32_t color = SET_IRGB(0, 0, 0, 0);
    switch (state) {
    case BoltLockManager::State::kLockingCompleted:
        color = SET_IRGB(0, kStatusLedLevel, 0, 0);
        break;
    case BoltLockManager::State::kUnlockingCompleted:
        color = SET_IRGB(0, 0, kStatusLedLevel, 0);
        break;
    case BoltLockManager::State::kLockingInitiated:
    case BoltLockManager::State::kUnlockingInitiated:
        color = SET_IRGB(0, kStatusLedLevel, kStatusLedLevel, 0);
        break;
    }
    return led_indicator_set_rgb(s_status_led, color);
}
#else
static esp_err_t app_driver_status_led_set(BoltLockManager::State state)
{
    if (BSP_LED_NUM == 0) {
        return ESP_OK;
    }

    bool locked = false;
    switch (state) {
    case BoltLockManager::State::kLockingInitiated:
    case BoltLockManager::State::kLockingCompleted:
        locked = true;
        break;
    case BoltLockManager::State::kUnlockingInitiated:
    case BoltLockManager::State::kUnlockingCompleted:
        break;
    }
    return led_indicator_set_on_off(s_status_led, locked);
}
#endif // CONFIG_BSP_LED_TYPE_RGB || CONFIG_BSP_LED_TYPE_RGB_CLASSIC

esp_err_t app_driver_set_lock_state(BoltLockManager::State state)
{
    ESP_LOGI(TAG, "Setting lock state to %u", static_cast<unsigned>(state));
    return app_driver_status_led_set(state);
}

esp_err_t app_driver_init()
{
    esp_err_t err = app_driver_button_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize lock button: %d", err);
        return err;
    }

    err = app_driver_status_led_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize status LED: %d", err);
        app_driver_button_deinit();
        return err;
    }

    return ESP_OK;
}
