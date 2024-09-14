/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <esp_event.h>
#include <esp_idf_version.h>
#include <esp_log.h>
#include <esp_rmaker_utils.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <string.h>

#include <esp_netif.h>
#include <wifi_prov_scheme_matter_ble.h>
#include <wifi_provisioning/manager.h>

#include <app_wifi.h>
#include <esp_rmaker_core.h>
#include <esp_timer.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/ESP32_custom/PlatformManagerImpl.h>
#include <platform/PlatformManager.h>
#include <qrcode.h>

ESP_EVENT_DEFINE_BASE(APP_WIFI_EVENT);

static const char *TAG = "app_wifi";

#define PROV_QR_VERSION "v1"

#define PROV_TRANSPORT_BLE "ble"
#define QRCODE_BASE_URL "https://rainmaker.espressif.com/qrcode.html"

#define CREDENTIALS_NAMESPACE "rmaker_creds"
#define RANDOM_NVS_KEY "random"

#define POP_STR_SIZE 9
static esp_timer_handle_t prov_stop_timer;
/* Timeout period in minutes */
#define APP_WIFI_PROV_TIMEOUT_PERIOD 30
/* Autofetch period in micro-seconds */
static uint64_t prov_timeout_period = (APP_WIFI_PROV_TIMEOUT_PERIOD * 60 * 1000000LL);

#define APP_PROV_STOP_ON_CREDS_MISMATCH

#define ESP_RAINMAKER_GITHUB_EXAMPLES_PATH "https://github.com/espressif/esp-rainmaker/blob/master/examples"
#define ESP_RAINMAKER_INTRO_LINK "https://rainmaker.espressif.com"
#define ESP_RMAKER_PHONE_APP_LINK "http://bit.ly/esp-rmaker"
char esp_rainmaker_ascii_art[] =
    "  ______  _____ _____    _____            _____ _   _ __  __          _  ________ _____\n"
    " |  ____|/ ____|  __ \\  |  __ \\     /\\   |_   _| \\ | |  \\/  |   /\\   | |/ /  ____|  __ \\\n"
    " | |__  | (___ | |__) | | |__) |   /  \\    | | |  \\| | \\  / |  /  \\  | ' /| |__  | |__) |\n"
    " |  __|  \\___ \\|  ___/  |  _  /   / /\\ \\   | | | . ` | |\\/| | / /\\ \\ |  < |  __| |  _  /\n"
    " | |____ ____) | |      | | \\ \\  / ____ \\ _| |_| |\\  | |  | |/ ____ \\| . \\| |____| | \\ \\\n"
    " |______|_____/|_|      |_|  \\_\\/_/    \\_\\_____|_| \\_|_|  |_/_/    \\_\\_|\\_\\______|_|  \\_\\\n";

static uint8_t *custom_mfg_data = NULL;
static size_t custom_mfg_data_len = 0;

esp_err_t app_wifi_set_custom_mfg_data(uint16_t device_type, uint8_t device_subtype)
{
    int8_t mfg_data[] = {(int8_t)MFG_DATA_HEADER, MGF_DATA_APP_ID, MFG_DATA_VERSION, MFG_DATA_CUSTOMER_ID};
    size_t mfg_data_len = sizeof(mfg_data) + 4; // 4 bytes of device type, subtype, and extra-code
    custom_mfg_data = (uint8_t *)MEM_ALLOC_EXTRAM(mfg_data_len);
    if (custom_mfg_data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory to custom mfg data");
        return ESP_ERR_NO_MEM;
    }
    memcpy(custom_mfg_data, mfg_data, sizeof(mfg_data));
    custom_mfg_data[8] = 0xff & (device_type >> 8);
    custom_mfg_data[9] = 0xff & device_type;
    custom_mfg_data[10] = device_subtype;
    custom_mfg_data[11] = 0;
    custom_mfg_data_len = mfg_data_len;
    return ESP_OK;
}

static esp_err_t qrcode_display(const char *text)
{
#define MAX_QRCODE_VERSION 5
    esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
    cfg.max_qrcode_version = MAX_QRCODE_VERSION;
    return esp_qrcode_generate(&cfg, text);
}

static void app_wifi_print_qr(const char *name, const char *pop, const char *transport)
{
    if (!name || !transport) {
        ESP_LOGW(TAG, "Cannot generate QR code payload. Data missing.");
        return;
    }
    char payload[150];
    if (pop) {
        snprintf(payload, sizeof(payload),
                 "{\"ver\":\"%s\",\"name\":\"%s\""
                 ",\"pop\":\"%s\",\"transport\":\"%s\"}",
                 PROV_QR_VERSION, name, pop, transport);
    } else {
        snprintf(payload, sizeof(payload),
                 "{\"ver\":\"%s\",\"name\":\"%s\""
                 ",\"transport\":\"%s\"}",
                 PROV_QR_VERSION, name, transport);
    }
    ESP_LOGI(TAG, "Scan this QR code from the ESP RainMaker phone app for Provisioning.");
    qrcode_display(payload);
    ESP_LOGI(TAG, "If QR code is not visible, copy paste the below URL in a browser.\n%s?data=%s", QRCODE_BASE_URL,
             payload);
    esp_event_post(APP_WIFI_EVENT, APP_WIFI_EVENT_QR_DISPLAY, payload, strlen(payload) + 1, portMAX_DELAY);
}

/* Event handler for catching system events */
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int retries = 0;
    static int failed_cnt = 0;

    if (event_base == WIFI_PROV_EVENT) {
        switch (event_id) {
        case WIFI_PROV_START:
            ESP_LOGI(TAG, "Provisioning started");
            break;
        case WIFI_PROV_CRED_RECV: {
            wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
            ESP_LOGI(TAG,
                     "Received Wi-Fi credentials"
                     "\n\tSSID     : %s\n\tPassword : %s",
                     (const char *)wifi_sta_cfg->ssid, (const char *)wifi_sta_cfg->password);
            break;
        }
        case WIFI_PROV_CRED_FAIL: {
            wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
            ESP_LOGE(TAG,
                     "Provisioning failed!\n\tReason : %s"
                     "\n\tPlease reset to factory and retry provisioning",
                     (*reason == WIFI_PROV_STA_AUTH_ERROR) ? "Wi-Fi station authentication failed"
                                                           : "Wi-Fi access-point not found");
            retries++;
            if (retries >= 5) {
                ESP_LOGI(TAG, "Failed to connect with provisioned AP, reseting provisioned credentials");
                wifi_prov_mgr_reset_sm_state_on_failure();
                esp_event_post(APP_WIFI_EVENT, APP_WIFI_EVENT_PROV_RESTART, NULL, 0, portMAX_DELAY);
                ESP_LOGW(TAG, "Failed to connect with provisioned AP, please reset to provisioning manually");
                retries = 0;
            }
            break;
        }
        case WIFI_PROV_CRED_SUCCESS:
            ESP_LOGI(TAG, "Provisioning successful");
            retries = 0;
            break;
        case WIFI_PROV_END:
            if (prov_stop_timer) {
                esp_timer_stop(prov_stop_timer);
                esp_timer_delete(prov_stop_timer);
                prov_stop_timer = NULL;
            }
            /* De-initialize manager once provisioning is finished */
            wifi_prov_mgr_deinit();
            break;
        default:
            break;
        }
    } else if (event_base == PROTOCOMM_SECURITY_SESSION_EVENT) {
        switch (event_id) {
        case PROTOCOMM_SECURITY_SESSION_SETUP_OK:
            ESP_LOGI(TAG, "Secured session established!");
            esp_rmaker_start();
            break;
        case PROTOCOMM_SECURITY_SESSION_INVALID_SECURITY_PARAMS:
            /* fall-through */
        case PROTOCOMM_SECURITY_SESSION_CREDENTIALS_MISMATCH:
            ESP_LOGE(TAG, "Received incorrect PoP or invalid security params! event: %d", (int)event_id);
            if (++failed_cnt >= 5) {
                /* stop provisioning for security reasons */
                wifi_prov_mgr_stop_provisioning();
                ESP_LOGW(TAG,
                         "Max PoP attempts reached! Provisioning disabled for security reasons. Please reboot device "
                         "to restart provisioning");
                esp_event_post(APP_WIFI_EVENT, APP_WIFI_EVENT_PROV_CRED_MISMATCH, NULL, 0, portMAX_DELAY);
            }
            break;
        default:
            break;
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
        esp_wifi_connect();
    }
}

/* Free random_bytes after use only if function returns ESP_OK */
static esp_err_t read_random_bytes_from_nvs(uint8_t **random_bytes, size_t *len)
{
    nvs_handle handle;
    esp_err_t err;
    *len = 0;

    if ((err = nvs_open_from_partition(CONFIG_ESP_RMAKER_FACTORY_PARTITION_NAME, CREDENTIALS_NAMESPACE, NVS_READONLY,
                                       &handle)) != ESP_OK) {
        ESP_LOGD(TAG, "NVS open for %s %s %s failed with error %d", CONFIG_ESP_RMAKER_FACTORY_PARTITION_NAME,
                 CREDENTIALS_NAMESPACE, RANDOM_NVS_KEY, err);
        return ESP_FAIL;
    }

    if ((err = nvs_get_blob(handle, RANDOM_NVS_KEY, NULL, len)) != ESP_OK) {
        ESP_LOGD(TAG, "Error %d. Failed to read key %s.", err, RANDOM_NVS_KEY);
        nvs_close(handle);
        return ESP_ERR_NOT_FOUND;
    }

    *random_bytes = (uint8_t *)calloc(*len, 1);
    if (*random_bytes) {
        nvs_get_blob(handle, RANDOM_NVS_KEY, *random_bytes, len);
        nvs_close(handle);
        return ESP_OK;
    }
    nvs_close(handle);
    return ESP_ERR_NO_MEM;
}

static char *custom_pop;
esp_err_t app_wifi_set_custom_pop(const char *pop)
{
    /* NULL PoP is not allowed here. Use POP_TYPE_NONE instead. */
    if (!pop) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Freeing up the PoP in case it is already allocated */
    if (custom_pop) {
        free(custom_pop);
        custom_pop = NULL;
    }

    custom_pop = strdup(pop);
    if (!custom_pop) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static esp_err_t get_device_service_name(char *service_name, size_t max)
{
    uint8_t *nvs_random = NULL;
    const char *ssid_prefix = "PROV";
    size_t nvs_random_size = 0;
    if ((read_random_bytes_from_nvs(&nvs_random, &nvs_random_size) != ESP_OK) || nvs_random_size < 3) {
        uint8_t eth_mac[6];
        esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
        snprintf(service_name, max, "%s_%02x%02x%02x", ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
    } else {
        snprintf(service_name, max, "%s_%02x%02x%02x", ssid_prefix, nvs_random[nvs_random_size - 3],
                 nvs_random[nvs_random_size - 2], nvs_random[nvs_random_size - 1]);
    }
    if (nvs_random) {
        free(nvs_random);
    }
    return ESP_OK;
}

static char *get_device_pop(app_wifi_pop_type_t pop_type)
{
    if (pop_type == POP_TYPE_NONE) {
        return NULL;
    } else if (pop_type == POP_TYPE_CUSTOM) {
        if (!custom_pop) {
            ESP_LOGE(TAG, "Custom PoP not set. Please use app_wifi_set_custom_pop().");
            return NULL;
        }
        return strdup(custom_pop);
    }
    char *pop = (char *)calloc(1, POP_STR_SIZE);
    if (!pop) {
        ESP_LOGE(TAG, "Failed to allocate memory for PoP.");
        return NULL;
    }

    if (pop_type == POP_TYPE_MAC) {
        uint8_t eth_mac[6];
        esp_err_t err = esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
        if (err == ESP_OK) {
            snprintf(pop, POP_STR_SIZE, "%02x%02x%02x%02x", eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);
            return pop;
        } else {
            ESP_LOGE(TAG, "Failed to get MAC address to generate PoP.");
            goto pop_err;
        }
    } else if (pop_type == POP_TYPE_RANDOM) {
        uint8_t *nvs_random = NULL;
        size_t nvs_random_size = 0;
        if ((read_random_bytes_from_nvs(&nvs_random, &nvs_random_size) != ESP_OK) || nvs_random_size < 4) {
            ESP_LOGE(TAG, "Failed to read random bytes from NVS to generate PoP.");
            if (nvs_random) {
                free(nvs_random);
            }
            goto pop_err;
        } else {
            snprintf(pop, POP_STR_SIZE, "%02x%02x%02x%02x", nvs_random[0], nvs_random[1], nvs_random[2], nvs_random[3]);
            free(nvs_random);
            return pop;
        }
    }
pop_err:
    free(pop);
    return NULL;
}

void app_wifi_init(void)
{
    wifi_init_config_t cfg;
    esp_err_t err = esp_netif_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize esp_netif");
        return;
    }

    // Lets not create a default station interface if already present
    if (!esp_netif_get_handle_from_ifkey("WIFI_STA_DEF")) {
        if (!esp_netif_create_default_wifi_sta()) {
            ESP_LOGE(TAG, "Failed to create the WiFi STA netif");
            return;
            ;
        }
    }

    // Initialize the ESP WiFi layer.
    cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize esp_wifi");
        return;
    }
    /* Register our event handler for Wi-Fi, IP and Provisioning related events */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#ifdef APP_PROV_STOP_ON_CREDS_MISMATCH
    ESP_ERROR_CHECK(
        esp_event_handler_register(PROTOCOMM_SECURITY_SESSION_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#endif
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
}

static void app_wifi_prov_stop(void *priv)
{
    ESP_LOGW(TAG, "Provisioning timed out. Please reboot device to restart provisioning.");
    wifi_prov_mgr_stop_provisioning();
    esp_event_post(APP_WIFI_EVENT, APP_WIFI_EVENT_PROV_TIMEOUT, NULL, 0, portMAX_DELAY);
}

esp_err_t app_wifi_start_timer(void)
{
    if (prov_timeout_period == 0) {
        return ESP_OK;
    }
    esp_timer_create_args_t prov_stop_timer_conf = {.callback = app_wifi_prov_stop,
                                                    .arg = NULL,
                                                    .dispatch_method = ESP_TIMER_TASK,
                                                    .name = "app_wifi_prov_stop_tm"};
    if (esp_timer_create(&prov_stop_timer_conf, &prov_stop_timer) == ESP_OK) {
        esp_timer_start_once(prov_stop_timer, prov_timeout_period);
        ESP_LOGI(TAG, "Provisioning will auto stop after %d minute(s).", APP_WIFI_PROV_TIMEOUT_PERIOD);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to create Provisioning auto stop timer.");
    }
    return ESP_FAIL;
}

esp_err_t app_wifi_start(app_wifi_pop_type_t pop_type)
{
    /* Configuration for the provisioning manager */
    wifi_prov_mgr_config_t config = {
        .scheme = wifi_prov_scheme_matter_ble,

        /* Any default scheme specific event handler that you would
         * like to choose. Since our example application requires
         * neither BT nor BLE, we can choose to release the associated
         * memory once provisioning is complete, or not needed
         * (in case when device is already provisioned). Choosing
         * appropriate scheme specific event handler allows the manager
         * to take care of this automatically. This can be set to
         * WIFI_PROV_EVENT_HANDLER_NONE when using wifi_prov_scheme_softap*/
        .scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE,
    };

    /* Initialize provisioning manager with the
     * configuration parameters set above */
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

    bool provisioned = false;
    /* Let's find out if the device is provisioned */
    wifi_prov_mgr_is_provisioned(&provisioned);
    /* If device is not yet provisioned start provisioning service */
    if (!provisioned) {
        ESP_LOGI(TAG, "Starting provisioning");

        /* What is the Device Service Name that we want
         * This translates to :
         *     - Wi-Fi SSID when scheme is wifi_prov_scheme_softap
         *     - device name when scheme is wifi_prov_scheme_ble
         */
        char service_name[12];
        get_device_service_name(service_name, sizeof(service_name));

        /* What is the service key (Wi-Fi password)
         * NULL = Open network
         * This is ignored when scheme is wifi_prov_scheme_ble
         */
        const char *service_key = NULL;

        /* What is the security level that we want (0 or 1):
         *      - WIFI_PROV_SECURITY_0 is simply plain text communication.
         *      - WIFI_PROV_SECURITY_1 is secure communication which consists of secure handshake
         *          using X25519 key exchange and proof of possession (pop) and AES-CTR
         *          for encryption/decryption of messages.
         */
        wifi_prov_security_t security = WIFI_PROV_SECURITY_1;

        /* Do we want a proof-of-possession (ignored if Security 0 is selected):
         *      - this should be a string with length > 0
         *      - NULL if not used
         */
        char *pop = get_device_pop(pop_type);
        if ((pop_type != POP_TYPE_NONE) && (pop == NULL)) {
            return ESP_ERR_NO_MEM;
        }

        /* This step is only useful when scheme is wifi_prov_scheme_ble. This will
         * set a custom 128 bit UUID which will be included in the BLE advertisement
         * and will correspond to the primary GATT service that provides provisioning
         * endpoints as GATT characteristics. Each GATT characteristic will be
         * formed using the primary service UUID as base, with different auto assigned
         * 12th and 13th bytes (assume counting starts from 0th byte). The client side
         * applications must identify the endpoints by reading the User Characteristic
         * Description descriptor (0x2901) for each characteristic, which contains the
         * endpoint name of the characteristic */
        uint8_t custom_service_uuid[] = {
            /* This is a random uuid. This can be modified if you want to change the BLE uuid. */
            /* 12th and 13th bit will be replaced by internal bits. */
            0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf, 0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
        };
        esp_err_t err = wifi_prov_scheme_matter_ble_set_service_uuid(custom_service_uuid, sizeof(custom_service_uuid));
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "wifi_prov_scheme_ble_set_service_uuid failed %d", err);
            return err;
        }
        if (custom_mfg_data) {
            err = wifi_prov_scheme_matter_ble_set_mfg_data(custom_mfg_data, custom_mfg_data_len);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to set mfg data, err=0x%x", err);
                return err;
            }
        }

        /* Start provisioning service */
        ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key));
        /* Print QR code for provisioning */
        app_wifi_print_qr(service_name, pop, PROV_TRANSPORT_BLE);
        ESP_LOGI(TAG, "Provisioning Started. Name : %s, POP : %s", service_name, pop ? pop : "<null>");
        if (pop) {
            free(pop);
        }
        app_wifi_start_timer();
    } else {
        ESP_LOGI(TAG, "Already provisioned, starting Wi-Fi STA");
        /* We don't need the manager as device is already provisioned,
         * so let's release it's resources */
        wifi_prov_mgr_deinit();
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
        esp_wifi_connect();
        esp_rmaker_start();
    }
    if (custom_mfg_data) {
        free(custom_mfg_data);
        custom_mfg_data = NULL;
        custom_mfg_data_len = 0;
    }
    return ESP_OK;
}
