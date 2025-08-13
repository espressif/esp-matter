/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once
#include <esp_err.h>
#include <esp_event.h>
#include <esp_idf_version.h>
#include "app_network.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize Wi-Fi
 *
 * This initializes Wi-Fi and the network/wifi provisioning manager
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t wifi_init();

/** Start Wi-Fi
 *
 * This will start provisioning if the node is not provisioned and will connect to Wi-Fi
 * if node is provisioned. Function will return successfully only after Wi-Fi is connect
 *
 * @param[in] pop The Proof of Possession (PoP) pin
 * @param[in] service_name The service name of network/wifi provisioning. This translates to
 *              - Wi-Fi SSID when scheme is network_prov_scheme_softap/wifi_prov_scheme_softap
 *              - device name when scheme is network_prov_scheme_ble/wifi_prov_scheme_ble
 * @param[in] service_key The service key of network/wifi provisioning. This translates to
 *              - Wi-Fi password when scheme is network_prov_scheme_softap/wifi_prov_scheme_softap (NULL = Open network)
 * @param[in] mfg_data The manufacture specific data of network/wifi provisioning.
 * @param[in] mfg_data The manufacture specific data length of network/wifi provisioning.
 * @param[out] provisioned Whether the device is provisioned.
 *
 * @return ESP_OK on success (Wi-Fi connected).
 * @return error in case of failure.
 */
esp_err_t wifi_start(const char *pop, const char *service_name, const char *service_key, uint8_t *mfg_data,
                     size_t mfg_data_len, bool *provisioned);

#ifdef __cplusplus
}
#endif
