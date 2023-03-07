// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef BL_WPA_H
#define BL_WPA_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup WPA_APIs  WPS APIs
  * @brief ESP32 Supplicant APIs
  *
  */

/** @addtogroup WPA_APIs
  * @{
  */
/* Crypto callback functions */
/* const wpa_crypto_funcs_t g_wifi_default_wpa_crypto_funcs; */
/* Mesh crypto callback functions */
/* const mesh_crypto_funcs_t g_wifi_default_mesh_crypto_funcs; */

/**
  * @brief     Supplicant initialization
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_NO_MEM : out of memory
  */
int bl_supplicant_init(void);

/**
  * @brief     Supplicant deinitialization
  *
  * @return
  *          - ESP_OK : succeed
  *          - others: failed
  */
int bl_supplicant_deinit(void);

int wpa_parse_wpa_ie_wrapper(const uint8_t *wpa_ie, size_t wpa_ie_len, wifi_wpa_ie_t *data);
int pbkdf2_sha1(const char *passphrase, const char *ssid, size_t ssid_len,
		int iterations, uint8_t *buf, size_t buflen);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* BL_WPA_H */
