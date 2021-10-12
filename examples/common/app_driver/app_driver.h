/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <esp_err.h>

/** Initialize the board and the drivers
 *
 * This initializes the selected board, which then initializes the respective drivers associated with it.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_driver_init(void);

/** CLI handler for driver
 *
 * This API can be added as the callback for shell commands
 *
 * @return 0 on success.
 * @return -1 in case of failure.
 */
int app_driver_cli_handler(int argc, char** argv);

#ifdef __cplusplus
}
#endif
