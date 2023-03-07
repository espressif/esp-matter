#ifndef BL_WPS_H_BSO9J1QR
#define BL_WPS_H_BSO9J1QR

#include <stdint.h>

typedef enum {
    BL_WPS_EVENT_COMPLETE,
    BL_WPS_EVENT_PIN,
    BL_WPS_EVENT_FAILURE,
    BL_WPS_EVENT_TIMEOUT,
    BL_WPS_EVENT_SESSION_OVERLAP,
    BL_WPS_EVENT_SCAN_ERROR,
} bl_wps_event_t;

typedef struct {
    uint8_t ssid[32];
    uint8_t ssid_len;
    uint8_t bssid[6];
    char passphrase[64];
} bl_wps_ap_credential_t;

typedef struct {
    char pin[8 + 1];
} bl_wps_pin_t;

typedef void (*wps_event_callback_t)(bl_wps_event_t event, void *payload, void *cb_arg);

typedef enum wps_type {
    WPS_TYPE_DISABLE = 0,
    WPS_TYPE_PBC,
    WPS_TYPE_PIN,
    WPS_TYPE_MAX,
} wps_type_t;

#define WPS_MAX_MANUFACTURER_LEN 65
#define WPS_MAX_MODEL_NUMBER_LEN 33
#define WPS_MAX_MODEL_NAME_LEN   33
#define WPS_MAX_DEVICE_NAME_LEN  33

typedef struct {
    char manufacturer[WPS_MAX_MANUFACTURER_LEN]; /*!< Manufacturer, null-terminated string. The default manufcturer is used if the string is empty */
    char model_number[WPS_MAX_MODEL_NUMBER_LEN]; /*!< Model number, null-terminated string. The default model number is used if the string is empty */
    char model_name[WPS_MAX_MODEL_NAME_LEN];     /*!< Model name, null-terminated string. The default model name is used if the string is empty */
    char device_name[WPS_MAX_DEVICE_NAME_LEN];   /*!< Device name, null-terminated string. The default device name is used if the string is empty */
} wps_factory_information_t;

typedef struct bl_wps_config {
    wps_type_t type;
    wps_factory_information_t factory_info;
    wps_event_callback_t event_cb;
    void *event_cb_arg;
} bl_wps_config_t;

typedef enum {
    BL_WPS_ERR_OK,
    BL_WPS_ERR_DUPLICATE_INSTANCE,
    BL_WPS_ERR_MEMORY,
    BL_WPS_ERR_WIFI_STATE,
} bl_wps_err_t;

bl_wps_err_t bl_wifi_wps_start(const struct bl_wps_config *config);

#endif /* end of include guard: BL_WPS_H_BSO9J1QR */
