/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <driver/gpio.h>
#include <esp_log.h>

#include <esp_matter.h>
#include "bsp/esp-bsp.h"

#include <app_priv.h>
#include <door_lock_manager.h>
#include <platform/PlatformManager.h>

static const char *TAG = "app_driver";

using namespace chip::app::Clusters;
using namespace esp_matter;

static uint16_t s_lock_endpoint_id = 0;
static led_indicator_handle_t s_rgb_led = nullptr;

#if CONFIG_BSP_BUTTONS_NUM > 0 && defined(CONFIG_BSP_LED_TYPE_RGB_CLASSIC) && \
    (CONFIG_BSP_BUTTON_1_GPIO == CONFIG_BSP_LED_RGB_RED_GPIO || \
     CONFIG_BSP_BUTTON_1_GPIO == CONFIG_BSP_LED_RGB_GREEN_GPIO || \
     CONFIG_BSP_BUTTON_1_GPIO == CONFIG_BSP_LED_RGB_BLUE_GPIO)
#define APP_DRIVER_RGB_LED_CONFLICTS_WITH_BUTTON 1
#else
#define APP_DRIVER_RGB_LED_CONFLICTS_WITH_BUTTON 0
#endif // CONFIG_BSP_BUTTONS_NUM > 0 && CONFIG_BSP_LED_TYPE_RGB_CLASSIC

#if CONFIG_BSP_LEDS_NUM > 0 && !APP_DRIVER_RGB_LED_CONFLICTS_WITH_BUTTON && \
    (defined(CONFIG_BSP_LED_TYPE_RGB) || defined(CONFIG_BSP_LED_TYPE_RGB_CLASSIC))
#define APP_DRIVER_HAS_RGB_LED 1
#else
#define APP_DRIVER_HAS_RGB_LED 0
#endif // CONFIG_BSP_LEDS_NUM > 0 && !APP_DRIVER_RGB_LED_CONFLICTS_WITH_BUTTON

namespace {

#if APP_DRIVER_HAS_RGB_LED
constexpr uint8_t kStatusLedLevel = 0x40;
#if CONFIG_IDF_TARGET_ESP32C6
constexpr gpio_num_t kRgbPowerEnableGpio = GPIO_NUM_19;
#elif CONFIG_IDF_TARGET_ESP32H2
constexpr gpio_num_t kRgbPowerEnableGpio = GPIO_NUM_10;
#endif // CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
#endif // APP_DRIVER_HAS_RGB_LED

#if APP_DRIVER_HAS_RGB_LED
void delete_leds(led_indicator_handle_t leds[])
{
    for (size_t i = 0; i < CONFIG_BSP_LEDS_NUM; ++i) {
        if (leds[i]) {
            led_indicator_delete(leds[i]);
        }
    }
}

esp_err_t enable_rgb_power()
{
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
    gpio_config_t config = {};
    config.pin_bit_mask = 1ULL << kRgbPowerEnableGpio;
    config.mode = GPIO_MODE_OUTPUT;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    esp_err_t err = gpio_config(&config);
    if (err != ESP_OK) {
        return err;
    }
    return gpio_set_level(kRgbPowerEnableGpio, 1);
#else
    return ESP_OK;
#endif // CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
}

esp_err_t set_status_led(const esp_matter_attr_val_t *val)
{
    if (!s_rgb_led || !val) {
        return ESP_OK;
    }

    uint32_t color = SET_IRGB(0, 0, 0, 0);
    if (!val->is_null()) {
        switch (static_cast<DoorLock::DlLockState>(val->val.u8)) {
        case DoorLock::DlLockState::kLocked:
            color = SET_IRGB(0, kStatusLedLevel, 0, 0);
            break;
        case DoorLock::DlLockState::kUnlocked:
        case DoorLock::DlLockState::kUnlatched:
            color = SET_IRGB(0, 0, kStatusLedLevel, 0);
            break;
        case DoorLock::DlLockState::kNotFullyLocked:
            color = SET_IRGB(0, kStatusLedLevel, kStatusLedLevel, 0);
            break;
        default:
            break;
        }
    }
    return led_indicator_set_rgb(s_rgb_led, color);
}
#endif // APP_DRIVER_HAS_RGB_LED

#if CONFIG_BSP_BUTTONS_NUM > 0
void delete_buttons(button_handle_t buttons[])
{
    for (size_t i = 0; i < BSP_BUTTON_NUM; ++i) {
        if (buttons[i]) {
            iot_button_delete(buttons[i]);
        }
    }
}

void toggle_lock([[maybe_unused]] intptr_t context)
{
    attribute_t *lock_state_attribute = attribute::get(s_lock_endpoint_id, DoorLock::Id, DoorLock::Attributes::LockState::Id);
    esp_matter_attr_val_t lock_state;
    esp_err_t err = lock_state_attribute ? attribute::get_val(lock_state_attribute, &lock_state) : ESP_ERR_NOT_FOUND;
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read LockState: %d", err);
        return;
    }
    if (lock_state.is_null() ||
            static_cast<DoorLock::DlLockState>(lock_state.val.u8) == DoorLock::DlLockState::kUnknownEnumValue) {
        ESP_LOGW(TAG, "Ignoring button press for null or unknown LockState");
        return;
    }

    if (static_cast<DoorLock::DlLockState>(lock_state.val.u8) == DoorLock::DlLockState::kLocked) {
        ESP_LOGI(TAG, "Button pressed, unlocking door");
        BoltLockMgr().Unlock(s_lock_endpoint_id, DoorLock::OperationSourceEnum::kButton);
    } else {
        ESP_LOGI(TAG, "Button pressed, locking door");
        BoltLockMgr().Lock(s_lock_endpoint_id, DoorLock::OperationSourceEnum::kButton);
    }
}

void button_toggle_cb([[maybe_unused]] void *button_handle, [[maybe_unused]] void *usr_data)
{
    CHIP_ERROR err = chip::DeviceLayer::PlatformMgr().ScheduleWork(toggle_lock);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to schedule button action: %" CHIP_ERROR_FORMAT, err.Format());
    }
}
#endif // CONFIG_BSP_BUTTONS_NUM > 0

} // namespace

esp_err_t app_driver_attribute_update([[maybe_unused]] uint16_t updated_endpoint_id,
                                      [[maybe_unused]] uint32_t cluster_id,
                                      [[maybe_unused]] uint32_t attribute_id,
                                      [[maybe_unused]] esp_matter_attr_val_t *val)
{
#if APP_DRIVER_HAS_RGB_LED
    if (updated_endpoint_id == s_lock_endpoint_id && cluster_id == DoorLock::Id &&
            attribute_id == DoorLock::Attributes::LockState::Id) {
        return set_status_led(val);
    }
#endif
    return ESP_OK;
}

esp_err_t app_driver_init(uint16_t door_lock_endpoint_id)
{
    s_lock_endpoint_id = door_lock_endpoint_id;
    esp_err_t err = ESP_OK;

#if CONFIG_BSP_BUTTONS_NUM > 0
    button_handle_t btns[BSP_BUTTON_NUM] = {};
    err = bsp_iot_button_create(btns, nullptr, BSP_BUTTON_NUM);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize button: %d", err);
        delete_buttons(btns);
        return err;
    }
    err = iot_button_register_cb(btns[0], BUTTON_PRESS_DOWN, nullptr, button_toggle_cb, nullptr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register button callback: %d", err);
        delete_buttons(btns);
        return err;
    }
#endif // CONFIG_BSP_BUTTONS_NUM > 0

#if APP_DRIVER_HAS_RGB_LED
    err = enable_rgb_power();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable RGB LED power: %d", err);
#if CONFIG_BSP_BUTTONS_NUM > 0
        iot_button_unregister_cb(btns[0], BUTTON_PRESS_DOWN, nullptr);
        delete_buttons(btns);
#endif // CONFIG_BSP_BUTTONS_NUM > 0
        return err;
    }
    led_indicator_handle_t leds[CONFIG_BSP_LEDS_NUM] = {};
    err = bsp_led_indicator_create(leds, nullptr, CONFIG_BSP_LEDS_NUM);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize RGB LED: %d", err);
        delete_leds(leds);
#if CONFIG_BSP_BUTTONS_NUM > 0
        iot_button_unregister_cb(btns[0], BUTTON_PRESS_DOWN, nullptr);
        delete_buttons(btns);
#endif // CONFIG_BSP_BUTTONS_NUM > 0
        return err;
    }
    s_rgb_led = leds[0];
#endif // APP_DRIVER_HAS_RGB_LED

    return ESP_OK;
}

esp_err_t app_driver_set_defaults()
{
#if APP_DRIVER_HAS_RGB_LED
    attribute_t *lock_state_attribute = attribute::get(s_lock_endpoint_id, DoorLock::Id, DoorLock::Attributes::LockState::Id);
    if (!lock_state_attribute) {
        return ESP_ERR_NOT_FOUND;
    }
    esp_matter_attr_val_t val;
    esp_err_t err = attribute::get_val(lock_state_attribute, &val);
    if (err != ESP_OK) {
        return err;
    }
    return set_status_led(&val);
#else
    return ESP_OK;
#endif
}
