/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>
#include <esp_matter.h>
#include <esp_openthread_types.h>

/** Standard max values (used for remapping attributes) */
#define STANDARD_BRIGHTNESS 100
#define STANDARD_HUE 360
#define STANDARD_SATURATION 100
#define STANDARD_TEMPERATURE_FACTOR 1000000

/** Matter max values (used for remapping attributes) */
#define MATTER_BRIGHTNESS 254
#define MATTER_HUE 254
#define MATTER_SATURATION 254

/** Default attribute values shared by Matter and Zigbee (off, max level) */
#define DEFAULT_POWER false
#define DEFAULT_BRIGHTNESS 254

typedef void *app_driver_handle_t;

app_driver_handle_t app_driver_light_init();
app_driver_handle_t app_driver_button_init();
esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val);
esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id);

app_driver_handle_t app_driver_light_get_handle();
esp_err_t app_driver_light_set_power(app_driver_handle_t handle, esp_matter_attr_val_t *val);
esp_err_t app_driver_light_set_brightness(app_driver_handle_t handle, esp_matter_attr_val_t *val);
esp_err_t app_driver_light_set_xy(app_driver_handle_t handle, uint16_t x, uint16_t y);

esp_err_t app_reset_button_register(void *handle);

using app_zigbee_network_joined_cb_t = void (*)();

void app_zigbee_register_network_joined_cb(app_zigbee_network_joined_cb_t cb);
bool app_zigbee_stack_is_running();
void app_zigbee_stack_start();
void app_zigbee_stack_stop();

#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG() \
    {                                         \
        .radio_mode = RADIO_MODE_NATIVE,      \
    }

#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                 \
    {                                                        \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,   \
    }

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()                                            \
    {                                                                                   \
        .storage_partition_name = "nvs", .netif_queue_size = 10, .task_queue_size = 10, \
    }
