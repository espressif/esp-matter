// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _IOT_BUTTON_H_
#define _IOT_BUTTON_H_

#include "button_adc.h"
#include "button_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* button_cb_t)(void *button_handle, void *usr_data);
typedef void *button_handle_t;

/**
 * @brief Button events
 *
 */
typedef enum {
    BUTTON_PRESS_DOWN = 0,
    BUTTON_PRESS_UP,
    BUTTON_PRESS_REPEAT,
    BUTTON_PRESS_REPEAT_DONE,
    BUTTON_SINGLE_CLICK,
    BUTTON_DOUBLE_CLICK,
    BUTTON_MULTIPLE_CLICK,
    BUTTON_LONG_PRESS_START,
    BUTTON_LONG_PRESS_HOLD,
    BUTTON_LONG_PRESS_UP,
    BUTTON_PRESS_END,
    BUTTON_EVENT_MAX,
} button_event_t;
/**
 * @brief Button events arg
 *
 */
typedef union {
    /**
     * @brief Long press time event data
     *
     */
    struct long_press_t {
        uint16_t press_time;    /**< press time(ms) for the corresponding callback to trigger */
    } long_press;               /**< long press struct, for event BUTTON_LONG_PRESS_START and BUTTON_LONG_PRESS_UP */

    /**
     * @brief Multiple clicks event data
     *
     */
    struct multiple_clicks_t {
        uint16_t clicks;        /**< number of clicks, to trigger the callback */
    } multiple_clicks;          /**< multiple clicks struct, for event BUTTON_MULTIPLE_CLICK */
} button_event_args_t;

/**
 * @brief Supported button type
 *
 */
typedef enum {
    BUTTON_TYPE_GPIO,
    BUTTON_TYPE_ADC,
} button_type_t;

/**
 * @brief Button configuration
 *
 */
typedef struct {
    uint16_t long_press_time;                         /**< Trigger time(ms) for long press, if 0 default to BUTTON_LONG_PRESS_TIME_MS */
    uint16_t short_press_time;                        /**< Trigger time(ms) for short press, if 0 default to BUTTON_SHORT_PRESS_TIME_MS */
} button_config_t;

/**
 * @brief Create a GPIO button
 *
 * @param config pointer of button configuration, must corresponding the button type
 *
 * @return A handle to the created button, or NULL in case of error.
 */
esp_err_t iot_button_new_gpio_device(const button_config_t *button_config, const button_gpio_config_t *gpio_cfg, button_handle_t *ret_button);

/**
 * @brief Create a GPIO button
 *
 * @param config pointer of button configuration, must corresponding the button type
 *
 * @return A handle to the created button, or NULL in case of error.
 */
esp_err_t iot_button_new_adc_device(const button_config_t *button_config, const button_adc_config_t *adc_config, button_handle_t *ret_button);

/**
 * @brief Delete a button
 *
 * @param btn_handle A button handle to delete
 *
 * @return
 *      - ESP_OK  Success
 *      - ESP_FAIL Failure
 */
esp_err_t iot_button_delete(button_handle_t btn_handle);

/**
 * @brief Register the button event callback function.
 *
 * @param btn_handle A button handle to register
 * @param event Button event
 * @param event_args Button event arguments
 * @param cb Callback function.
 * @param usr_data user data
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG   Arguments is invalid.
 */
esp_err_t iot_button_register_cb(button_handle_t btn_handle, button_event_t event, button_event_args_t *event_args, button_cb_t cb, void *usr_data);

/**
 * @brief Unregister the button event callback function.
 *
 * @param btn_handle A button handle to unregister
 * @param event Button event
 * @param event_args Button event arguments
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG   Arguments is invalid.
 */
esp_err_t iot_button_unregister_cb(button_handle_t btn_handle, button_event_t event, button_event_args_t *event_args);

/**
 * @brief how many Callbacks are still registered.
 *
 * @param btn_handle A button handle to unregister
 *
 * @return 0 if no callbacks registered, or 1 .. (BUTTON_EVENT_MAX-1) for the number of Registered Buttons.
 */
size_t iot_button_count_cb(button_handle_t btn_handle);

/**
 * @brief Get button event
 *
 * @param btn_handle Button handle
 *
 * @return Current button event. See button_event_t
 */
button_event_t iot_button_get_event(button_handle_t btn_handle);

/**
 * @brief Get button repeat times
 *
 * @param btn_handle Button handle
 *
 * @return button pressed times. For example, double-click return 2, triple-click return 3, etc.
 */
uint8_t iot_button_get_repeat(button_handle_t btn_handle);

/**
 * @brief Get button ticks time
 *
 * @param btn_handle Button handle
 *
 * @return Actual time from press down to up (ms).
 */
uint16_t iot_button_get_ticks_time(button_handle_t btn_handle);

/**
 * @brief Get button long press hold count
 *
 * @param btn_handle Button handle
 *
 * @return Count of trigger cb(BUTTON_LONG_PRESS_HOLD)
 */
uint16_t iot_button_get_long_press_hold_cnt(button_handle_t btn_handle);

#ifdef __cplusplus
}
#endif

#endif
