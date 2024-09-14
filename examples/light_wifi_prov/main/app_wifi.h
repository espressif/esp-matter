/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once
#include <esp_err.h>
#include <esp_event.h>


#define MFG_DATA_HEADER                     0xe5, 0x02
#define MGF_DATA_APP_ID                     'N', 'o', 'v'
#define MFG_DATA_VERSION                    'a'
#define MFG_DATA_CUSTOMER_ID                0x00, 0x01

#define MGF_DATA_DEVICE_TYPE_LIGHT          0x0005
#define MGF_DATA_DEVICE_TYPE_SWITCH         0x0080

#define MFG_DATA_DEVICE_SUBTYPE_SWITCH      0x01
#define MFG_DATA_DEVICE_SUBTYPE_LIGHT       0x01

#define MFG_DATA_DEVICE_EXTRA_CODE          0x00

/** ESP RainMaker Event Base */
ESP_EVENT_DECLARE_BASE(APP_WIFI_EVENT);

/** App Wi-Fir Events */
typedef enum {
    /** QR code available for display. Associated data is the NULL terminated QR payload. */
    APP_WIFI_EVENT_QR_DISPLAY = 1,
    /** Provisioning timed out */
    APP_WIFI_EVENT_PROV_TIMEOUT,
    /** Provisioning has restarted due to failures (Invalid SSID/Passphrase) */
    APP_WIFI_EVENT_PROV_RESTART,
    /** Provisioning closed due to invalid credentials */
    APP_WIFI_EVENT_PROV_CRED_MISMATCH,
} app_wifi_event_t;

/** Types of Proof of Possession */
typedef enum {
    /** Use MAC address to generate PoP */
    POP_TYPE_MAC,
    /** Use random stream generated and stored in fctry partition during claiming process as PoP */
    POP_TYPE_RANDOM,
    /** Do not use any PoP.
     * Use this option with caution. Consider using `CONFIG_APP_WIFI_PROV_TIMEOUT_PERIOD` with this.
     */
    POP_TYPE_NONE,
    /** Use a custom PoP.
     * Set a custom PoP using app_wifi_set_custom_pop() first.
     */
    POP_TYPE_CUSTOM
} app_wifi_pop_type_t;

/** Initialize Wi-Fi
 *
 * This initializes Wi-Fi and the Wi-Fi provisioning manager
 */
void app_wifi_init();

/** Start Wi-Fi
 *
 * This will start provisioning if the node is not provisioned and will connect to Wi-Fi
 * if node is provisioned. Function will return successfully only after Wi-Fi is connect
 *
 * @param[in] pop_type The type for Proof of Possession (PoP) pin
 * 
 * @return ESP_OK on success (Wi-Fi connected).
 * @return error in case of failure.
 */
esp_err_t app_wifi_start(app_wifi_pop_type_t pop_type);

/** Set custom manufacturing data
 *
 * This can be used to add some custom manufacturing data in BLE advertisements during
 * provisioning. This can be used by apps to filter the scanned BLE devices and show
 * only the relevant one. Supported by Nova Home app for light and switch
 *
 * @param[in] device_type Type of the device, like light or switch
 * @param[in] device_subtype Sub Type of the device (application specific)
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_wifi_set_custom_mfg_data(uint16_t device_type, uint8_t device_subtype);

/** Set custom PoP
 *
 * This can be used to set a custom Proof of Possession (PoP) pin for provisioning.
 * Applicable only if POP_TYPE_CUSTOM is used for app_wifi_start().
 *
 * @param[in] pop A NULL terminated PoP string (typically 8 characters alphanumeric)
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_wifi_set_custom_pop(const char *pop);
