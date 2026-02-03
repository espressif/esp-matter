/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <sdkconfig.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_idf_version.h>
#include <esp_rmaker_utils.h>
#include <app_network.h>

#ifdef CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI
#include <app_wifi_internal.h>
#include <esp_netif_types.h>
#endif /* CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI */

#include <esp_mac.h>

#include <network_provisioning/manager.h>
#ifdef CONFIG_APP_NETWORK_PROV_TRANSPORT_BLE
#include <network_provisioning/scheme_ble.h>
#else /* CONFIG_APP_NETWORK_PROV_TRANSPORT_SOFTAP */
#include <network_provisioning/scheme_softap.h>
#endif /* CONFIG_APP_NETWORK_PROV_TRANSPORT_BLE */

#ifdef CONFIG_APP_NETWORK_PROV_SHOW_QR
#include <qrcode.h>
#endif

#include <nvs.h>
#include <nvs_flash.h>
#include <esp_timer.h>
#include <app_network.h>

ESP_EVENT_DEFINE_BASE(APP_NETWORK_EVENT);
static const char *TAG = "app_network";
static const int NETWORK_CONNECTED_EVENT = BIT0;
static EventGroupHandle_t network_event_group;

#define PROV_QR_VERSION "v1"

#define PROV_TRANSPORT_SOFTAP   "softap"
#define PROV_TRANSPORT_BLE      "ble"
#define QRCODE_BASE_URL     "https://rainmaker.espressif.com/qrcode.html"

#define CREDENTIALS_NAMESPACE   "rmaker_creds"
#define RANDOM_NVS_KEY          "random"

#define POP_STR_SIZE    9
static esp_timer_handle_t prov_stop_timer;
/* Timeout period in minutes */
#define APP_NETWORK_PROV_TIMEOUT_PERIOD   CONFIG_APP_NETWORK_PROV_TIMEOUT_PERIOD
/* Autofetch period in micro-seconds */
static uint64_t prov_timeout_period = (APP_NETWORK_PROV_TIMEOUT_PERIOD * 60 * 1000000LL);

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 3)
#define APP_PROV_STOP_ON_CREDS_MISMATCH
#elif (CONFIG_APP_NETWORK_PROV_MAX_RETRY_CNT > 0)
#warning "Provisioning window stop on max credentials failures, needs IDF version >= 5.1.3"
#endif

#ifdef CONFIG_APP_NETWORK_SHOW_DEMO_INTRO_TEXT

#define ESP_RAINMAKER_GITHUB_EXAMPLES_PATH  "https://github.com/espressif/esp-rainmaker/blob/master/examples"
#define ESP_RAINMAKER_INTRO_LINK    "https://rainmaker.espressif.com"
#define ESP_RMAKER_PHONE_APP_LINK   "http://bit.ly/esp-rmaker"
char esp_rainmaker_ascii_art[] = \
                                 "  ______  _____ _____    _____            _____ _   _ __  __          _  ________ _____\n"\
                                 " |  ____|/ ____|  __ \\  |  __ \\     /\\   |_   _| \\ | |  \\/  |   /\\   | |/ /  ____|  __ \\\n"\
                                 " | |__  | (___ | |__) | | |__) |   /  \\    | | |  \\| | \\  / |  /  \\  | ' /| |__  | |__) |\n"\
                                 " |  __|  \\___ \\|  ___/  |  _  /   / /\\ \\   | | | . ` | |\\/| | / /\\ \\ |  < |  __| |  _  /\n"\
                                 " | |____ ____) | |      | | \\ \\  / ____ \\ _| |_| |\\  | |  | |/ ____ \\| . \\| |____| | \\ \\\n"\
                                 " |______|_____/|_|      |_|  \\_\\/_/    \\_\\_____|_| \\_|_|  |_/_/    \\_\\_|\\_\\______|_|  \\_\\\n";

static void intro_print(bool provisioned)
{
    printf("####################################################################################################\n");
    printf("%s\n", esp_rainmaker_ascii_art);
    printf("Welcome to ESP RainMaker %s demo application!\n", RMAKER_DEMO_PROJECT_NAME);
    if (!provisioned) {
        printf("Follow these steps to get started:\n");
        printf("1. Download the ESP RainMaker phone app by visiting this link from your phone's browser:\n\n");
        printf("   %s\n\n", ESP_RMAKER_PHONE_APP_LINK);
        printf("2. Sign up and follow the steps on screen to add the device to your Wi-Fi/Thread network.\n");
        printf("3. You are now ready to use the device and control it locally as well as remotely.\n");
        printf("   You can also use the Boot button on the board to control your device.\n");
    }
    printf("\nIf you want to reset network credentials, or reset to factory, press and hold the Boot button.\n");
    printf("\nThis application uses ESP RainMaker, which is based on ESP IDF.\n");
    printf("Check out the source code for this application here:\n   %s/%s\n",
           ESP_RAINMAKER_GITHUB_EXAMPLES_PATH, RMAKER_DEMO_PROJECT_NAME);
    printf("\nPlease visit %s for additional information.\n\n", ESP_RAINMAKER_INTRO_LINK);
    printf("####################################################################################################\n");
}

#else

static void intro_print(bool provisioned)
{
    /* Do nothing */
}

#endif /* !APP_NETWORK_SHOW_DEMO_INTRO_TEXT */

#ifdef CONFIG_APP_NETWORK_PROV_SHOW_QR
static esp_err_t qrcode_display(const char *text)
{
#define MAX_QRCODE_VERSION 5
    esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
    cfg.max_qrcode_version = MAX_QRCODE_VERSION;
    return esp_qrcode_generate(&cfg, text);
}
#endif

static uint8_t *custom_mfg_data = NULL;
static size_t custom_mfg_data_len = 0;

esp_err_t app_network_set_custom_mfg_data(uint16_t device_type, uint8_t device_subtype)
{
    int8_t mfg_data[] = {MFG_DATA_HEADER, MGF_DATA_APP_ID, MFG_DATA_VERSION, MFG_DATA_CUSTOMER_ID};
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
    ESP_LOG_BUFFER_HEXDUMP("tag", custom_mfg_data, mfg_data_len, 3);
    return ESP_OK;
}

static void app_network_print_qr(const char *name, const char *pop, const char *transport)
{
    if (!name || !transport) {
        ESP_LOGW(TAG, "Cannot generate QR code payload. Data missing.");
        return;
    }
    char payload[150];
    if (pop) {
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                 ",\"pop\":\"%s\",\"transport\":\"%s\"}",
                 PROV_QR_VERSION, name, pop, transport);
    } else {
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                 ",\"transport\":\"%s\"}",
                 PROV_QR_VERSION, name, transport);
    }
#ifdef CONFIG_APP_NETWORK_PROV_SHOW_QR
    ESP_LOGI(TAG, "Scan this QR code from the ESP RainMaker phone app for Provisioning.");
    qrcode_display(payload);
#endif /* CONFIG_APP_NETWORK_PROV_SHOW_QR */
    ESP_LOGI(TAG, "If QR code is not visible, copy paste the below URL in a browser.\n%s?data=%s", QRCODE_BASE_URL, payload);
    esp_event_post(APP_NETWORK_EVENT, APP_NETWORK_EVENT_QR_DISPLAY, payload, strlen(payload) + 1, portMAX_DELAY);
}

/* Free random_bytes after use only if function returns ESP_OK */
static esp_err_t read_random_bytes_from_nvs(uint8_t **random_bytes, size_t *len)
{
    nvs_handle handle;
    esp_err_t err;
    *len = 0;

    if ((err = nvs_open_from_partition(CONFIG_ESP_RMAKER_FACTORY_PARTITION_NAME, CREDENTIALS_NAMESPACE,
                                       NVS_READONLY, &handle)) != ESP_OK) {
        ESP_LOGD(TAG, "NVS open for %s %s %s failed with error %d", CONFIG_ESP_RMAKER_FACTORY_PARTITION_NAME, CREDENTIALS_NAMESPACE, RANDOM_NVS_KEY, err);
        return ESP_FAIL;
    }

    if ((err = nvs_get_blob(handle, RANDOM_NVS_KEY, NULL, len)) != ESP_OK) {
        ESP_LOGD(TAG, "Error %d. Failed to read key %s.", err, RANDOM_NVS_KEY);
        nvs_close(handle);
        return ESP_ERR_NOT_FOUND;
    }

    *random_bytes = calloc(*len, 1);
    if (*random_bytes) {
        nvs_get_blob(handle, RANDOM_NVS_KEY, *random_bytes, len);
        nvs_close(handle);
        return ESP_OK;
    }
    nvs_close(handle);
    return ESP_ERR_NO_MEM;
}

static char *custom_pop;
esp_err_t app_network_set_custom_pop(const char *pop)
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
    const char *ssid_prefix = CONFIG_APP_NETWORK_PROV_NAME_PREFIX;
    size_t nvs_random_size = 0;
    if ((read_random_bytes_from_nvs(&nvs_random, &nvs_random_size) != ESP_OK) || nvs_random_size < 3) {
        uint8_t mac_addr[6];
        esp_read_mac(mac_addr, ESP_MAC_BASE);
        snprintf(service_name, max, "%s_%02x%02x%02x", ssid_prefix, mac_addr[3], mac_addr[4], mac_addr[5]);
    } else {
        snprintf(service_name, max, "%s_%02x%02x%02x", ssid_prefix, nvs_random[nvs_random_size - 3],
                 nvs_random[nvs_random_size - 2], nvs_random[nvs_random_size - 1]);
    }
    if (nvs_random) {
        free(nvs_random);
    }
    return ESP_OK;
}

static char *get_device_pop(app_network_pop_type_t pop_type)
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
    char *pop = calloc(1, POP_STR_SIZE);
    if (!pop) {
        ESP_LOGE(TAG, "Failed to allocate memory for PoP.");
        return NULL;
    }

    if (pop_type == POP_TYPE_MAC) {
        uint8_t mac_addr[6];
        esp_err_t err = esp_read_mac(mac_addr, ESP_MAC_BASE);
        if (err == ESP_OK) {
            snprintf(pop, POP_STR_SIZE, "%02x%02x%02x%02x", mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
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

static void network_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{

#ifdef APP_PROV_STOP_ON_CREDS_MISMATCH
    static int failed_cnt = 0;
#endif
#ifdef APP_PROV_STOP_ON_CREDS_MISMATCH
    if (event_base == PROTOCOMM_SECURITY_SESSION_EVENT) {
        switch (event_id) {
        case PROTOCOMM_SECURITY_SESSION_SETUP_OK:
            ESP_LOGI(TAG, "Secured session established!");
            break;
        case PROTOCOMM_SECURITY_SESSION_INVALID_SECURITY_PARAMS:
        /* fall-through */
        case PROTOCOMM_SECURITY_SESSION_CREDENTIALS_MISMATCH:
            ESP_LOGE(TAG, "Received incorrect PoP or invalid security params! event: %d", (int) event_id);
            if (CONFIG_APP_NETWORK_PROV_MAX_POP_MISMATCH &&
                    (++failed_cnt >= CONFIG_APP_NETWORK_PROV_MAX_POP_MISMATCH)) {
                /* stop provisioning for security reasons */
                network_prov_mgr_stop_provisioning();
                ESP_LOGW(TAG, "Max PoP attempts reached! Provisioning disabled for security reasons. Please reboot device to restart provisioning");
                esp_event_post(APP_NETWORK_EVENT, APP_NETWORK_EVENT_PROV_CRED_MISMATCH, NULL, 0, portMAX_DELAY);
            }
            break;
        default:
            break;
        }
    }
#endif /* APP_PROV_STOP_ON_CREDS_MISMATCH */
#ifdef CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        /* Signal main application to continue execution */
        xEventGroupSetBits(network_event_group, NETWORK_CONNECTED_EVENT);
    }
#endif /* CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI */
    if (event_base == NETWORK_PROV_EVENT && event_id == NETWORK_PROV_END) {
        if (prov_stop_timer) {
            esp_timer_stop(prov_stop_timer);
            esp_timer_delete(prov_stop_timer);
            prov_stop_timer = NULL;
        }
        network_prov_mgr_deinit();
    }
}

void app_network_init()
{
    /* Initialize the event loop, if not done already. */
    esp_err_t err = esp_event_loop_create_default();
    /* If the default event loop is already initialized, we get ESP_ERR_INVALID_STATE */
    if (err != ESP_OK) {
        if (err == ESP_ERR_INVALID_STATE) {
            ESP_LOGW(TAG, "Event loop creation failed with ESP_ERR_INVALID_STATE. Proceeding since it must have been created elsewhere.");
        } else {
            ESP_LOGE(TAG, "Failed to create default event loop, err = %x", err);
            return;
        }
    }
#ifdef CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI
    ESP_ERROR_CHECK(wifi_init());
#endif
    network_event_group = xEventGroupCreate();
#ifdef APP_PROV_STOP_ON_CREDS_MISMATCH
    ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_SECURITY_SESSION_EVENT, ESP_EVENT_ANY_ID, &network_event_handler, NULL));
#endif
#ifdef CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &network_event_handler, NULL));
#endif
    ESP_ERROR_CHECK(esp_event_handler_register(NETWORK_PROV_EVENT, NETWORK_PROV_END, &network_event_handler, NULL));
}

static void app_network_prov_stop(void *priv)
{
    ESP_LOGW(TAG, "Provisioning timed out. Please reboot device to restart provisioning.");
    network_prov_mgr_stop_provisioning();
    esp_event_post(APP_NETWORK_EVENT, APP_NETWORK_EVENT_PROV_TIMEOUT, NULL, 0, portMAX_DELAY);
}

esp_err_t app_network_start_timer(void)
{
    if (prov_timeout_period == 0) {
        return ESP_OK;
    }
    esp_timer_create_args_t prov_stop_timer_conf = {
        .callback = app_network_prov_stop,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "app_wifi_prov_stop_tm"
    };
    if (esp_timer_create(&prov_stop_timer_conf, &prov_stop_timer) == ESP_OK) {
        esp_timer_start_once(prov_stop_timer, prov_timeout_period);
        ESP_LOGI(TAG, "Provisioning will auto stop after %d minute(s).",
                 APP_NETWORK_PROV_TIMEOUT_PERIOD);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to create Provisioning auto stop timer.");
    }
    return ESP_FAIL;
}

esp_err_t app_network_start(app_network_pop_type_t pop_type)
{
    /* Do we want a proof-of-possession (ignored if Security 0 is selected):
     *      - this should be a string with length > 0
     *      - NULL if not used
     */
    char *pop = get_device_pop(pop_type);
    if ((pop_type != POP_TYPE_NONE) && (pop == NULL)) {
        return ESP_ERR_NO_MEM;
    }
    /* What is the Device Service Name that we want
     * This translates to :
     *     - device name when scheme is network_prov_scheme_ble/wifi_prov_scheme_ble
     */
    char service_name[12];
    get_device_service_name(service_name, sizeof(service_name));
    /* What is the service key (Wi-Fi password)
     * NULL = Open network
     * This is ignored when scheme is network_prov_scheme_ble/wifi_prov_scheme_ble
     */
    const char *service_key = NULL;
    esp_err_t err = ESP_OK;
    bool provisioned = false;
#ifdef CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI
    err = wifi_start(pop, service_name, service_key, custom_mfg_data, custom_mfg_data_len, &provisioned);
#endif
    if (err != ESP_OK) {
        free(pop);
        return err;
    }
    if (!provisioned) {
#ifdef CONFIG_APP_NETWORK_PROV_TRANSPORT_BLE
        app_network_print_qr(service_name, pop, PROV_TRANSPORT_BLE);
#else /* CONFIG_APP_NETWORK_PROV_TRANSPORT_SOFTAP */
        app_network_print_qr(service_name, pop, PROV_TRANSPORT_SOFTAP);
#endif /* CONFIG_APP_NETWORK_PROV_TRANSPORT_BLE */
        app_network_start_timer();
    }
    free(pop);
    intro_print(provisioned);
    if (custom_mfg_data) {
        free(custom_mfg_data);
        custom_mfg_data = NULL;
        custom_mfg_data_len = 0;
    }
    /* Wait for Network connection */
    xEventGroupWaitBits(network_event_group, NETWORK_CONNECTED_EVENT, false, true, portMAX_DELAY);
    return err;
}
