/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>
#include <esp_matter.h>

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include "esp_openthread_types.h"
#endif

#define BADGE_CLUSTER_ID 0x131BFC03

#define NAME_ATTRIBUTE_ID 0x0000
#define COMPANY_NAME_ATTRIBUTE_ID 0x0001
#define EMAIL_ATTRIBUTE_ID 0x0002
#define CONTACT_ATTRIBUTE_ID 0x0003
#define EVENT_NAME_ATTRIBUTE_ID 0x0004

#define MAX_ATTR_SIZE 33

#define DEFAULT_POWER true

typedef enum {
    NAME = 0,
    COMPANY_NAME,
    EMAIL,
    CONTACT,
    EVENT_NAME,
    MAX_VCARD_ATTR
} vcard_tag;

extern bool badge_cluster_input_exists;

typedef void *app_driver_handle_t;

/** Initialize the button driver
 *
 * This initializes the button driver associated with the selected board.
 *
 * @return Handle on success.
 * @return NULL in case of failure.
 */
app_driver_handle_t app_driver_button_init();

/** Driver Update
 *
 * This API should be called to update the driver for the attribute being updated.
 * This is usually called from the common `app_attribute_update_cb()`.
 *
 * @param[in] endpoint_id Endpoint ID of the attribute.
 * @param[in] cluster_id Cluster ID of the attribute.
 * @param[in] attribute_id Attribute ID of the attribute.
 * @param[in] val Pointer to `esp_matter_attr_val_t`. Use appropriate elements as per the value type.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val);

/** Set defaults for light driver
 *
 * Set the attribute drivers to their default values from the created data model.
 *
 * @param[in] endpoint_id Endpoint ID of the driver.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id);

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                                                  \
    {                                                                                          \
        .radio_mode = RADIO_MODE_NATIVE,                                                       \
    }

#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                                                   \
    {                                                                                          \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,                                     \
    }

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()                                                   \
    {                                                                                          \
        .storage_partition_name = "ot_storage", .netif_queue_size = 10, .task_queue_size = 10, \
    }
#endif
