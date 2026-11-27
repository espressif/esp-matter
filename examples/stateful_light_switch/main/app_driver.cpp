/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "app_priv.h"
#include "button_gpio.h"
#include "esp_matter.h"
#include "esp_matter_client.h"
#include "iot_button.h"
#include "led_strip.h"
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include "bsp/esp-bsp.h"

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "app_driver";
extern uint16_t switch_endpoint_id;

// GPIO Config
#define BUTTON_GPIO_PIN GPIO_NUM_9
#define LED_GPIO_PIN GPIO_NUM_8

// Button Timings
#define BUTTON_SHORT_PRESS_TIME_MS 500
#define BUTTON_LONG_PRESS_TIME_MS 1000

// Dimming Config
#define DIMMING_RATE 42 // Levels per second (Target ~6s for 255 levels)
#define DIMMING_TIMER_PERIOD_MS 100
#define DIMMING_STEP (DIMMING_RATE / (1000 / DIMMING_TIMER_PERIOD_MS)) // ~4 levels per tick

static led_strip_handle_t s_led_strip = NULL;
static TimerHandle_t s_dimming_timer = NULL;
static int s_dimming_direction = 0; // 1 = Up, -1 = Down
static int64_t s_last_short_press_time = 0;
static bool s_is_dimming = false; // Track if dimming is in progress
static bool s_pending_dim_direction_set = false; // Flag to know if we should dim

// Command Data Helpers
struct MoveParams {
    uint8_t move_mode;
    uint8_t rate;
    uint8_t options_mask;
    uint8_t options_override;
};

static MoveParams s_move_params;

static void app_driver_update_led(uint8_t val)
{
    if (s_led_strip) {
        // Simple mapping: val is brightness. If OnOff is OFF, this function might be called with 0 or we handle it in
        // attribute_update Current implementation assumes this is called with the effective brightness
        led_strip_set_pixel(s_led_strip, 0, val, val, val); // White
        led_strip_refresh(s_led_strip);
    }
}

static void dimming_timer_callback(TimerHandle_t xTimer)
{
    // Get current level
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, switch_endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, LevelControl::Id);
    attribute_t *attribute = attribute::get(cluster, LevelControl::Attributes::CurrentLevel::Id);

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    uint8_t current_level = val.val.u8;

    int new_level = current_level + (s_dimming_direction * DIMMING_STEP);
    if (new_level > 254)
        new_level = 254;
    if (new_level < 1)
        new_level = 1;

    if (new_level != current_level) {
        val.val.u8 = (uint8_t)new_level;
        attribute::update(switch_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &val);
    }

    if (new_level == 254 || new_level == 1) {
        // Stop timer if reached limits
        xTimerStop(s_dimming_timer, 0);
    }
}

// Client Callback
static void app_driver_client_invoke_command_callback(client::peer_device_t *peer_device,
                                                      client::request_handle_t *req_handle, void *priv_data)
{
    if (req_handle->type == esp_matter::client::INVOKE_CMD) {
        char command_data_str[128];
        strcpy(command_data_str, "{}");

        if (req_handle->command_path.mClusterId == OnOff::Id) {
            // Toggle usually has no fields
        } else if (req_handle->command_path.mClusterId == LevelControl::Id) {
            if (req_handle->command_path.mCommandId == LevelControl::Commands::Move::Id) {
                MoveParams *params = (MoveParams *)req_handle->request_data;
                snprintf(command_data_str, sizeof(command_data_str),
                         "{\"0:U8\": %d, \"1:U8\": %d, \"2:U8\": %d, \"3:U8\": %d}", params->move_mode, params->rate,
                         params->options_mask, params->options_override);
            } else if (req_handle->command_path.mCommandId == LevelControl::Commands::Stop::Id) {
                snprintf(command_data_str, sizeof(command_data_str),
                         "{\"0:U8\": 0, \"1:U8\": 0}"); // OptionsMask, OptionsOverride
            }
        }

        client::interaction::invoke::send_request(NULL, peer_device, req_handle->command_path, command_data_str, NULL,
                                                  NULL, chip::NullOptional);
    }
}

static void app_driver_client_callback(client::peer_device_t *peer_device, client::request_handle_t *req_handle,
                                       void *priv_data)
{
    if (req_handle->type == esp_matter::client::INVOKE_CMD) {
        app_driver_client_invoke_command_callback(peer_device, req_handle, priv_data);
    }
}

static void app_driver_client_group_invoke_callback(uint8_t fabric_index, client::request_handle_t *req_handle,
                                                    void *priv_data)
{
    if (req_handle->type != esp_matter::client::INVOKE_CMD) {
        return;
    }
    char command_data_str[128];
    strcpy(command_data_str, "{}");

    if (req_handle->command_path.mClusterId == OnOff::Id) {
        // Toggle command has no fields
    } else if (req_handle->command_path.mClusterId == LevelControl::Id) {
        if (req_handle->command_path.mCommandId == LevelControl::Commands::Move::Id) {
            MoveParams *params = (MoveParams *)req_handle->request_data;
            snprintf(command_data_str, sizeof(command_data_str),
                     "{\"0:U8\": %d, \"1:U8\": %d, \"2:U8\": %d, \"3:U8\": %d}", params->move_mode, params->rate,
                     params->options_mask, params->options_override);
        } else if (req_handle->command_path.mCommandId == LevelControl::Commands::Stop::Id) {
            snprintf(command_data_str, sizeof(command_data_str),
                     "{\"0:U8\": 0, \"1:U8\": 0}");
        }
    } else {
        ESP_LOGW(TAG, "Unsupported cluster for group command");
        return;
    }
    client::interaction::invoke::send_group_request(fabric_index, req_handle->command_path, command_data_str);
}

// Button Callbacks
static void app_driver_button_press_down_cb(void *arg, void *data)
{
    // Determine dimming direction when button is pressed
    // If there was a recent short press (within 1.5s), we'll dim down
    int64_t now = esp_timer_get_time() / 1000;
    bool recent_short_press = (now - s_last_short_press_time) < 1500;
    s_dimming_direction = recent_short_press ? -1 : 1;
    s_pending_dim_direction_set = true;
    ESP_LOGI(TAG, "Button Press Down (dim direction: %s)", recent_short_press ? "down" : "up");
}

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Button Toggle (Short Press)");
    s_last_short_press_time = esp_timer_get_time() / 1000;
    s_pending_dim_direction_set = false; // Cancel any pending dim

    // Local Update
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute::get(switch_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id), &val);
    val.val.b = !val.val.b;
    attribute::update(switch_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);

    // Client Command
    client::request_handle_t req_handle;
    req_handle.type = esp_matter::client::INVOKE_CMD;
    req_handle.command_path.mClusterId = OnOff::Id;
    req_handle.command_path.mCommandId = OnOff::Commands::Toggle::Id;

    lock::chip_stack_lock(portMAX_DELAY);
    client::cluster_update(switch_endpoint_id, &req_handle);
    lock::chip_stack_unlock();
}

static void app_driver_button_press_hold_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Button Hold Start (direction: %s)", s_dimming_direction == -1 ? "down" : "up");
    s_is_dimming = true;

    // Start Local Timer
    if (s_dimming_timer) {
        xTimerStart(s_dimming_timer, 0);
    }

    // Client Command
    client::request_handle_t req_handle;
    req_handle.type = esp_matter::client::INVOKE_CMD;
    req_handle.command_path.mClusterId = LevelControl::Id;
    req_handle.command_path.mCommandId = LevelControl::Commands::Move::Id;

    s_move_params.move_mode = (s_dimming_direction == -1) ? 1 : 0; // 0=Up, 1=Down
    s_move_params.rate = DIMMING_RATE;
    s_move_params.options_mask = 0;
    s_move_params.options_override = 0;
    req_handle.request_data = &s_move_params;

    lock::chip_stack_lock(portMAX_DELAY);
    client::cluster_update(switch_endpoint_id, &req_handle);
    lock::chip_stack_unlock();
}

static void app_driver_button_release_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Button Release");

    // Stop Local Timer
    if (s_dimming_timer) {
        xTimerStop(s_dimming_timer, 0);
    }

    // Only send Stop command if we were actually dimming
    if (!s_is_dimming) {
        return;
    }
    s_is_dimming = false;

    client::request_handle_t req_handle;
    req_handle.type = esp_matter::client::INVOKE_CMD;
    req_handle.command_path.mClusterId = LevelControl::Id;
    req_handle.command_path.mCommandId = LevelControl::Commands::Stop::Id;

    lock::chip_stack_lock(portMAX_DELAY);
    client::cluster_update(switch_endpoint_id, &req_handle);
    lock::chip_stack_unlock();
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    if (endpoint_id == switch_endpoint_id) {
        if (cluster_id == OnOff::Id && attribute_id == OnOff::Attributes::OnOff::Id) {
            bool on = val->val.b;
            if (on) {
                // Restore brightness? Or just Max?
                // Get current level
                esp_matter_attr_val_t level_val = esp_matter_invalid(NULL);
                attribute::get_val(
                    attribute::get(endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id),
                    &level_val);
                app_driver_update_led(level_val.val.u8);
            } else {
                app_driver_update_led(0);
            }
        } else if (cluster_id == LevelControl::Id && attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
            uint8_t level = val->val.u8;
            // Check OnOff state?
            esp_matter_attr_val_t onoff_val = esp_matter_invalid(NULL);
            attribute::get_val(attribute::get(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id), &onoff_val);
            if (onoff_val.val.b) {
                app_driver_update_led(level);
            }
        }
    }
    return ESP_OK;
}

app_driver_handle_t app_driver_init()
{
    // LED Init
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO_PIN,
        .max_leds = 1,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags = {.invert_out = false}, // Correct struct initialization
    };
    // RMT backend config
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags = {.with_dma = false},
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led_strip));
    led_strip_clear(s_led_strip);

    // Button Init
    button_config_t btn_cfg = {
        .long_press_time = BUTTON_LONG_PRESS_TIME_MS,
        .short_press_time = BUTTON_SHORT_PRESS_TIME_MS,
    };
    button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = BUTTON_GPIO_PIN,
        .active_level = 0,
    };

    button_handle_t btn = NULL;
    esp_err_t err = iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &btn);
    if (err == ESP_OK) {
        iot_button_register_cb(btn, BUTTON_PRESS_DOWN, NULL, app_driver_button_press_down_cb, NULL);
        iot_button_register_cb(btn, BUTTON_SINGLE_CLICK, NULL, app_driver_button_toggle_cb, NULL);
        iot_button_register_cb(btn, BUTTON_LONG_PRESS_START, NULL, app_driver_button_press_hold_cb, NULL);
        iot_button_register_cb(btn, BUTTON_PRESS_UP, NULL, app_driver_button_release_cb, NULL);
    } else {
        ESP_LOGE(TAG, "Button create failed");
    }

    // Timer Init
    s_dimming_timer =
        xTimerCreate("DimTimer", pdMS_TO_TICKS(DIMMING_TIMER_PERIOD_MS), pdTRUE, NULL, dimming_timer_callback);

    // Client Config - register both unicast and group callbacks
    client::set_request_callback(app_driver_client_callback, app_driver_client_group_invoke_callback, NULL);

    return (app_driver_handle_t)btn;
}
