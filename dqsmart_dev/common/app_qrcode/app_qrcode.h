/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Print the QR code for commissioning
 *
 * Fetch the QR code details for commissioning from Matter and print it on the console.
 * Also print the manual code if QR code is not supported.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_qrcode_print();

#ifdef __cplusplus
}
#endif