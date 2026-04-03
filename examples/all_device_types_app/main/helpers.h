/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

namespace esp_matter {

namespace data_model {

int create(uint8_t device_type_index);

/** Change the device type.
 *
 * Persists the new device type selection to NVS and reboots.
 * On reboot, the new device type is created with stable endpoint IDs.
 *
 * @param[in] device_type_index Index into the device type list.
 *
 * @return 0 on success, non-zero on failure. Does not return on success (reboots).
 */
int change(uint8_t device_type_index);
} /* namespace data_model */

namespace nvs_helpers {

esp_err_t set_device_type_in_nvs(uint8_t device_type_index);

esp_err_t get_device_type_from_nvs(uint8_t *device_type_index);
} /* namespace nvs_helpers */

} /* namespace esp_matter */

namespace example {
namespace console {

/** Add Help Commands
 *
 * Adds the help commands.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t register_help_commands();

/** Add Device Commands
 *
 * Adds the create device commands.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t register_create_device_commands();

/** Add Change Device Commands
 *
 * Registers the 'change' CLI command that switches the device type at runtime.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t register_change_device_commands();

void init(void);

void deinit(void);
} /* namespace console */
} /* namespace example */
