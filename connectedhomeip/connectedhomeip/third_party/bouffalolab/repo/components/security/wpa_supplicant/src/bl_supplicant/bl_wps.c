#include "bl_wps.h"
#include <wps/wps.h>
#include <wps/wps_i.h>
#include <bl_wifi.h>
#include <bl_supplicant/bl_wifi_driver.h>
#include <rsn_supp/wpa.h>
#include <rsn_supp/wpa_i.h>
#include <bl_supplicant/bl_wpas_glue.h>
#include <wps/wps_defs.h>
#include <rsn_supp/wpa.h>
#include <eap_peer/eap_defs.h>
#include <eap_peer/eap_common.h>
#include <common/eapol_common.h>
#include <crypto/dh_group5.h>
#include <wifi_mgmr_ext.h>
#include <semphr.h>
#include <aos/yloop.h>

#define WPS_SM_EAPOL_VERSION 1

static int wps_tx_start_(void);
static int wifi_station_wps_deinit_(void);
static int wps_dev_deinit_(struct wps_device_data *dev);

static struct wps_sm *gWpsSm;

struct wps_sm *wps_sm_get(void)
{
    return gWpsSm;
}

int wps_get_status(void)
{
    return bl_wifi_get_wps_status_internal();
}

int wps_set_status(uint32_t status)
{
    bl_wifi_set_wps_status_internal(status);
    return 0;
}

int wps_credential_save(u8 idx, u8 *ssid, u8 ssid_len, char *key, u8 key_len)
{
    struct wps_sm *sm = gWpsSm;

    if (!ssid || !sm || idx > 2) {
        return -1;
    }

    // Drop credential that does not match negotiation AP
    if (!(ssid_len == sm->ssid_neg_len && !memcmp(ssid, sm->ssid_neg, ssid_len))) {
        return 0;
    }

    memset(sm->ssid[idx], 0x00, sizeof(sm->ssid[idx]));
    memcpy(sm->ssid[idx], ssid, ssid_len);
    sm->ssid_len[idx] = ssid_len;
    memset(sm->key[idx], 0x00, sizeof(sm->key[idx]));
    memcpy(sm->key[idx], key, key_len);
    sm->key_len[idx] = key_len;

    // We only save the last matched credential
    sm->ap_cred_cnt = 1;

    return 0;
}

wps_type_t wps_get_type()
{
    struct wps_sm *wps = gWpsSm;
    return wps->cfg.type;
}

static void wps_build_ic_appie_wps_pr_(void)
{
    struct wpabuf *extra_ie = NULL;
    struct wpabuf *wps_ie;
    struct wps_sm *sm = gWpsSm;

    wpa_printf(MSG_DEBUG, "wps build: wps pr\n");

    if (wps_get_type() == WPS_TYPE_PBC) {
        wps_ie = (struct wpabuf *)wps_build_probe_req_ie(DEV_PW_PUSHBUTTON,
                 sm->dev,
                 sm->uuid, WPS_REQ_ENROLLEE,
                 0, NULL);
    } else {
        wps_ie = (struct wpabuf *)wps_build_probe_req_ie(DEV_PW_DEFAULT,
                 sm->dev,
                 sm->uuid, WPS_REQ_ENROLLEE,
                 0, NULL);
    }

    if (wps_ie) {
        if (wpabuf_resize(&extra_ie, wpabuf_len(wps_ie)) == 0) {
            wpabuf_put_buf(extra_ie, wps_ie);
        } else {
            wpabuf_free(wps_ie);
            return;
        }
        wpabuf_free(wps_ie);
    }

    memcpy(sm->wps_ie_probe_request, wpabuf_head(extra_ie), extra_ie->used);
    sm->wps_ie_probe_request_len = extra_ie->used;
    wpabuf_free(extra_ie);
    bl_wifi_set_appie_internal(0xff, WIFI_APPIE_WPS_PR, sm->wps_ie_probe_request, sm->wps_ie_probe_request_len, true);
}

static void wps_build_ic_appie_wps_ar_(void)
{
    struct wps_sm *sm = gWpsSm;
    struct wpabuf *buf = (struct wpabuf *)wps_build_assoc_req_ie(WPS_REQ_ENROLLEE);

    wpa_printf(MSG_DEBUG, "wps build: wps ar\n");

    if (buf) {
        memcpy(sm->wps_ie_association_request, wpabuf_head(buf), buf->used);
        sm->wps_ie_association_request_len = buf->used;
        wpabuf_free(buf);
        bl_wifi_set_appie_internal(0xff, WIFI_APPIE_WPS_AR, sm->wps_ie_association_request, sm->wps_ie_association_request_len, true);
    }
}

typedef enum {
    WPS_SCAN_TARGET_FOUND,
    WPS_SCAN_SESSION_OVERLAP,
    WPS_SCAN_TIMEOUT,
    WPS_SCAN_ERROR,
} wps_scan_result_t;

static void wps_scan_complete_(void *data, void *param)
{
    SemaphoreHandle_t scan_sem = (SemaphoreHandle_t)data;
    xSemaphoreGive(scan_sem);
}

static wps_scan_result_t wps_scan_(void)
{
    struct wps_sm *sm = gWpsSm;
    wps_scan_result_t result = WPS_SCAN_TIMEOUT;
    SemaphoreHandle_t scan_sem = NULL;

    if (NULL == (scan_sem = xSemaphoreCreateBinary())) {
        result = WPS_SCAN_ERROR;
        goto exit;
    }

    sm->is_wps_scan = true;
    wps_set_status(WPS_STATUS_SCANNING);
    while (xTaskGetTickCount() - sm->start_tick < pdMS_TO_TICKS(WPS_TIMEOUT_MS)) {
        wifi_mgmr_scan(scan_sem, wps_scan_complete_);
        xSemaphoreTake(scan_sem, portMAX_DELAY);

        if (sm->discover_ssid_cnt == 1) {
            result = WPS_SCAN_TARGET_FOUND;
            goto exit;
        } else if (sm->discover_ssid_cnt > 1) {
            result = WPS_SCAN_SESSION_OVERLAP;
            goto exit;
        }
    }
exit:
    if (scan_sem) {
        vSemaphoreDelete(scan_sem);
    }
    sm->is_wps_scan = false;
    return result;
}

static void notify_user_(bl_wps_event_t event, void *payload)
{
    struct wps_sm *sm = gWpsSm;

    if (sm->cfg.event_cb) {
        sm->cfg.event_cb(event, payload, sm->cfg.event_cb_arg);
    }
}

static void wifi_event_cb_(input_event_t *event, void *private_data)
{
    struct wps_sm *sm = (struct wps_sm *)private_data;
    wps_evq_msg_t msg = {};

    if (event->code == CODE_WIFI_ON_DISCONNECT) {
        msg.event = WPS_EVQ_DISCONNECTED;
        xQueueSend(sm->event_queue, &msg, portMAX_DELAY);
    }
}

static void wps_timeout_cb_(TimerHandle_t xTimer)
{
    struct wps_sm *sm = gWpsSm;
    wps_evq_msg_t msg = { .event = WPS_EVQ_TIMEOUT };
    xQueueSend(sm->event_queue, &msg, portMAX_DELAY);
}

static void wps_eapol_start_timer_cb_(void *arg)
{
    wps_tx_start_();
}

static void wps_success_timer_cb_(TimerHandle_t xTimer)
{
    wifi_mgmr_sta_disconnect();
}

static void connect_ap_wps_neg_(u8 *ssid, u8 ssid_len, u8 *bssid)
{
    char ssid_str[33] = {};
    wifi_interface_t wifi_interface = wifi_mgmr_sta_enable();
    struct ap_connect_adv ext_param;
    memcpy(ssid_str, ssid, ssid_len);

    wifi_mgmr_sta_autoconnect_disable();

    memset(&ext_param, 0, sizeof(ext_param));
    ext_param.ap_info.type = AP_INFO_TYPE_SUGGEST;
    ext_param.ap_info.time_to_live = 5;
    ext_param.ap_info.bssid = bssid;
    // Disabling DHCP is required
    // ext_param.ap_info.use_dhcp = 0;
    wifi_mgmr_sta_connect_ext(wifi_interface, ssid_str, NULL, &ext_param);
}

static void prepare_stop_(void)
{
    wps_set_status(WPS_STATUS_DISABLE);

    bl_wifi_set_appie_internal(0xff, WIFI_APPIE_WPS_PR, NULL, 0, true);
    bl_wifi_set_appie_internal(0xff, WIFI_APPIE_WPS_AR, NULL, 0, true);
    bl_wifi_set_wps_cb_internal(NULL);

    wifi_mgmr_sta_disconnect();
    vTaskDelay(1000);
    wifi_mgmr_sta_disable(NULL);
    vTaskDelay(100);
    wifi_mgmr_sta_autoconnect_enable();
}

static void wps_task_(void *pvParameters)
{
    struct wps_sm *sm = gWpsSm;

    sm->start_tick = xTaskGetTickCount();

    if (wps_get_type() == WPS_TYPE_PIN) {
        // only pin & ap_cred is allocated directly by calling FreeRTOS API, as they are exported
        bl_wps_pin_t *pin = pvPortMalloc(sizeof(*pin));
        if (pin) {
            memcpy(pin->pin, sm->wps->dev_password, 8);
            notify_user_(BL_WPS_EVENT_PIN, pin);
        }
    }

    wps_build_ic_appie_wps_pr_();
    wps_build_ic_appie_wps_ar_();

    wps_build_public_key(sm->wps, NULL, WPS_CALC_KEY_PRE_CALC);

    aos_register_event_filter(EV_WIFI, wifi_event_cb_, sm);

    wps_scan_result_t scan_result = wps_scan_();

    switch (scan_result) {
    case WPS_SCAN_TARGET_FOUND:
        wps_set_status(WPS_STATUS_PENDING);
        connect_ap_wps_neg_(sm->ssid_neg, sm->ssid_neg_len, sm->bssid);
        break;
    case WPS_SCAN_SESSION_OVERLAP:
        notify_user_(BL_WPS_EVENT_SESSION_OVERLAP, 0);
        goto error_out;
    case WPS_SCAN_TIMEOUT:
        notify_user_(BL_WPS_EVENT_TIMEOUT, 0);
        goto error_out;
    default:
        notify_user_(BL_WPS_EVENT_SCAN_ERROR, 0);
        goto error_out;
    }

    bl_wifi_timer_setfn(&sm->wps_eapol_start_timer, wps_eapol_start_timer_cb_, sm);

    TickType_t time_elapsed = xTaskGetTickCount() - sm->start_tick;
    if (time_elapsed >= WPS_TIMEOUT_MS) {
        notify_user_(BL_WPS_EVENT_TIMEOUT, 0);
        goto error_out;
    }
    TickType_t timeout_timer_period = pdMS_TO_TICKS(WPS_TIMEOUT_MS) - time_elapsed;
    sm->timeout_timer = xTimerCreateStatic("wps timeout", timeout_timer_period, pdFALSE, 0, wps_timeout_cb_, &sm->timeout_timer_buffer);
    xTimerStart(sm->timeout_timer, portMAX_DELAY);

    for (;;) {
        wps_evq_msg_t msg;
        bool exit_loop = false;
        if (pdPASS != xQueueReceive(sm->event_queue, &msg, portMAX_DELAY)) {
            break;
        }

        switch (msg.event) {
        case WPS_EVQ_SUCCESS:
            sm->success_cb_timer = xTimerCreateStatic("wps success", pdMS_TO_TICKS(1000), pdFALSE, 0, wps_success_timer_cb_, &sm->success_cb_timer_buffer);
            xTimerStart(sm->success_cb_timer, portMAX_DELAY);
            break;
        case WPS_EVQ_FAILURE:
            prepare_stop_();
            break;
        case WPS_EVQ_DISCONNECTED:
            if (sm->timeout_timer) {
                xTimerStop(sm->timeout_timer, portMAX_DELAY);
                xTimerDelete(sm->timeout_timer, portMAX_DELAY);
                sm->timeout_timer = NULL;
            }
            if (sm->success_cb_timer) {
                xTimerStop(sm->success_cb_timer, portMAX_DELAY);
                xTimerDelete(sm->success_cb_timer, portMAX_DELAY);
                sm->success_cb_timer = NULL;
            }
            if (sm->wps->state == WPS_FINISHED && sm->ap_cred_cnt == 1) {
                bl_wps_ap_credential_t *cred;
                prepare_stop_();
                if ((cred = pvPortMalloc(sizeof(*cred)))) {
                    memset(cred, 0, sizeof(*cred));
                    memcpy(cred->bssid, sm->bssid, ETH_ALEN);
                    memcpy(cred->ssid, sm->ssid[0], sm->ssid_len[0]);
                    memcpy(cred->passphrase, sm->key[0], sm->key_len[0]);
                    notify_user_(BL_WPS_EVENT_COMPLETE, cred);
                } else {
                    notify_user_(BL_WPS_EVENT_FAILURE, 0);
                }
            } else {
                prepare_stop_();
                notify_user_(BL_WPS_EVENT_FAILURE, 0);
            }
            exit_loop = true;
            break;
        case WPS_EVQ_TIMEOUT:
            prepare_stop_();
            notify_user_(BL_WPS_EVENT_TIMEOUT, 0);
            exit_loop = true;
            break;
        }
        if (exit_loop) {
            break;
        }
    }
error_out:
    wifi_station_wps_deinit_();
    vTaskDelete(NULL);
}

static void wps_set_default_factory_(void)
{
    struct wps_sm *wps = gWpsSm;
    wps_factory_information_t *factory_info = &wps->factory_info;

    sprintf(factory_info->manufacturer, "Bouffalo Lab");
    sprintf(factory_info->model_name, "BL60X");
    sprintf(factory_info->model_number, "BL60X");
    sprintf(factory_info->device_name, "BL60X STATION");
}

static int wps_set_factory_info_(const struct bl_wps_config *config)
{
    struct wps_sm *wps = gWpsSm;
    wps_factory_information_t *factory_info = &wps->factory_info;

    wps_set_default_factory_();

    if (config->factory_info.manufacturer[0] != 0) {
        memcpy(factory_info->manufacturer, config->factory_info.manufacturer, WPS_MAX_MANUFACTURER_LEN - 1);
    }

    if (config->factory_info.model_number[0] != 0) {
        memcpy(factory_info->model_number, config->factory_info.model_number, WPS_MAX_MODEL_NUMBER_LEN - 1);
    }

    if (config->factory_info.model_name[0] != 0) {
        memcpy(factory_info->model_name, config->factory_info.model_name, WPS_MAX_MODEL_NAME_LEN - 1);
    }

    if (config->factory_info.device_name[0] != 0) {
        memcpy(factory_info->device_name, config->factory_info.device_name, WPS_MAX_DEVICE_NAME_LEN - 1);
    }

    wpa_printf(MSG_INFO, "manufacturer: %s, model number: %s, model name: %s, device name: %s\n", factory_info->manufacturer,
               factory_info->model_number, factory_info->model_name, factory_info->device_name);

    return 0;
}

static int wps_dev_init_(void)
{
    int ret = 0;
    struct wps_sm *sm = gWpsSm;
    struct wps_device_data *dev = NULL;
    wps_factory_information_t *factory_info;

    dev = &sm->wps_ctx->dev;
    sm->dev = dev;
    factory_info = &sm->factory_info;

    dev->config_methods = WPS_CONFIG_VIRT_PUSHBUTTON | WPS_CONFIG_PHY_DISPLAY;
    dev->rf_bands = WPS_RF_24GHZ;

    WPA_PUT_BE16(dev->pri_dev_type, WPS_DEV_COMPUTER);
    WPA_PUT_BE32(dev->pri_dev_type + 2, WPS_DEV_OUI_WFA);
    WPA_PUT_BE16(dev->pri_dev_type + 6, WPS_DEV_COMPUTER_PC);

    dev->manufacturer = (char *)os_zalloc(WPS_MAX_MANUFACTURER_LEN);
    if (!dev->manufacturer) {
        ret = -1;
        goto _out;
    }
    sprintf(dev->manufacturer, factory_info->manufacturer);

    dev->model_name = (char *)os_zalloc(WPS_MAX_MODEL_NAME_LEN);
    if (!dev->model_name) {
        ret = -1;
        goto _out;
    }
    sprintf(dev->model_name, factory_info->model_name);

    dev->model_number = (char *)os_zalloc(WPS_MAX_MODEL_NAME_LEN);
    if (!dev->model_number) {
        ret = -1;
        goto _out;
    }
    sprintf(dev->model_number, factory_info->model_number);

    dev->device_name = (char *)os_zalloc(WPS_MAX_DEVICE_NAME_LEN);
    if (!dev->device_name) {
        ret = -1;
        goto _out;
    }
    sprintf(dev->device_name, factory_info->device_name);

    dev->serial_number = (char *)os_zalloc(16);
    if (!dev->serial_number) {
        ret = -1;
        goto _out;
    }
    sprintf(dev->serial_number, "%02x%02x%02x%02x%02x%02x",
            sm->ownaddr[0], sm->ownaddr[1], sm->ownaddr[2],
            sm->ownaddr[3], sm->ownaddr[4], sm->ownaddr[5]);

    uuid_gen_mac_addr(sm->ownaddr, sm->uuid);
    memcpy(dev->mac_addr, sm->ownaddr, ETH_ALEN);

    return 0;

_out:
    wps_dev_deinit_(dev);

    return ret;
}

static int wps_dev_deinit_(struct wps_device_data *dev)
{
    int ret = 0;

    if (!dev) {
        return -1;
    }

    if (dev->manufacturer) {
        os_free(dev->manufacturer);
        dev->manufacturer = NULL;
    }
    if (dev->model_name) {
        os_free(dev->model_name);
        dev->model_name = NULL;
    }
    if (dev->model_number) {
        os_free(dev->model_number);
        dev->model_number = NULL;
    }
    if (dev->device_name) {
        os_free(dev->device_name);
        dev->device_name = NULL;
    }
    if (dev->serial_number) {
        os_free(dev->serial_number);
        dev->serial_number = NULL;
    }

    return ret;
}

/**
 * wps_init_ - Initialize WPS Registration protocol data
 * @cfg: WPS configuration
 * Returns: Pointer to allocated data or %NULL on failure
 *
 * This function is used to initialize WPS data for a registration protocol
 * instance (i.e., each run of registration protocol as a Registrar of
 * Enrollee. The caller is responsible for freeing this data after the
 * registration run has been completed by calling wps_deinit().
 */
static struct wps_data *wps_init_(void)
{
    struct wps_sm *sm = gWpsSm;
    struct wps_data *data = (struct wps_data *)os_zalloc(sizeof(*data));
    const char *all_zero_pin = "00000000";

    if (data == NULL) {
        return NULL;
    }

    data->wps = sm->wps_ctx;

    data->registrar = 0; /* currently, we force to support enrollee only */

    memcpy(data->mac_addr_e, sm->dev->mac_addr, ETH_ALEN);
    memcpy(data->uuid_e, sm->uuid, WPS_UUID_LEN);

    if (wps_get_type() == WPS_TYPE_PIN) {
        u32 spin = 0;
        data->dev_pw_id = DEV_PW_DEFAULT;
        data->dev_password_len = 8;
        data->dev_password = (u8 *) os_zalloc(data->dev_password_len + 1);
        if (data->dev_password == NULL) {
            os_free(data);
            return NULL;
        }

        spin = wps_generate_pin();
        sprintf((char *)data->dev_password, "%08lu", spin);
        wpa_hexdump_key(MSG_DEBUG, "WPS: AP PIN dev_password",
                        data->dev_password, data->dev_password_len);
        do {
            char tmpp[9];
            os_bzero(tmpp, 9);
            memcpy(tmpp, data->dev_password, 8);
            wpa_printf(MSG_DEBUG, "WPS PIN [%s]\n", tmpp);
        } while (0);
    } else if (wps_get_type() == WPS_TYPE_PBC) {
        data->pbc = 1;
        /* Use special PIN '00000000' for PBC */
        data->dev_pw_id = DEV_PW_PUSHBUTTON;
        if (data->dev_password) {
            os_free(data->dev_password);
        }
        data->dev_password = (u8 *) os_zalloc(9);
        if (data->dev_password == NULL) {
            os_free(data);
            return NULL;
        } else {
            strncpy((char *)data->dev_password, all_zero_pin, 9);
        }
        data->dev_password_len = 8;
    }

    data->wps->config_methods = WPS_CONFIG_PUSHBUTTON | WPS_CONFIG_DISPLAY;
#ifdef CONFIG_WPS2
    data->wps->config_methods |= (WPS_CONFIG_VIRT_PUSHBUTTON | WPS_CONFIG_PHY_DISPLAY);
#endif

    data->state = data->registrar ? RECV_M1 : SEND_M1;

    return data;
}

// TODO: add ability to detect session overlap
static bool wps_parse_scan_result(struct wps_scan_ie *scan)
{
    struct wps_sm *sm = gWpsSm;
#ifdef WPS_DEBUG
    char tmp[33];

    os_bzero(tmp, sizeof(tmp));
    strncpy(tmp, (char *)scan->ssid, scan->ssid_len);
    wpa_printf(MSG_DEBUG, "wps parse scan: %s\n", tmp);
#endif

    if (wps_get_type() == WPS_TYPE_DISABLE || wps_get_status() != WPS_STATUS_SCANNING) {
        return false;
    }

    struct wpabuf *buf = wpabuf_alloc_copy(scan->wps + 6, scan->wps[1] - 4);

    // TODO: check if AP mode matches current setting
    if (wps_is_selected_pbc_registrar(buf, scan->bssid)
            || wps_is_selected_pin_registrar(buf, scan->bssid)) {
        wpabuf_free(buf);

        if (sm->is_wps_scan == false) {
            return false;
        }

        if (memcmp(sm->bssid, scan->bssid, ETH_ALEN)) {
            sm->discover_ssid_cnt++;
        }
        if (!scan->rsn && !scan->wpa && (scan->capinfo & WIFI_CAPINFO_PRIVACY)) {
            wpa_printf(MSG_ERROR, "WEP not suppported in WPS");

            return false;
        }

        memcpy(sm->ssid_neg, scan->ssid, scan->ssid_len);
        sm->ssid_neg_len = scan->ssid_len;
        if (scan->bssid) {
            memcpy(sm->bssid, scan->bssid, ETH_ALEN);
        } else {
        }
        wpa_printf(MSG_DEBUG, "wps discover [%s]", (char *)sm->ssid_neg);
        sm->scan_cnt = 0;

        sm->channel = scan->chan;

        return true;
    }
    wpabuf_free(buf);

    return false;
}

static int wps_stop_process(void)
{
    struct wps_sm *sm = gWpsSm;

    if (!gWpsSm) {
        return -1;
    }

    wps_set_status(WPS_STATUS_DISABLE);
    sm->scan_cnt = 0;
    sm->discover_ssid_cnt = 0;
    sm->wps->state = SEND_M1;
    os_bzero(sm->bssid, ETH_ALEN);
    os_bzero(sm->ssid, sizeof(sm->ssid));
    os_bzero(sm->ssid_len, sizeof(sm->ssid_len));
    sm->ap_cred_cnt = 0;

#if 0
    esp_wifi_disarm_sta_connection_timer_internal();
    ets_timer_disarm(&sm->wps_msg_timeout_timer);
    ets_timer_disarm(&sm->wps_success_cb_timer);

    esp_wifi_disconnect();
#endif

    wpa_printf(MSG_DEBUG, "Write wps_fail_information");

#if 0
    esp_event_send_internal(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_FAILED, &reason_code, sizeof(reason_code), portMAX_DELAY);
#endif

    return 0;
}

static int wps_finish(void)
{
    struct wps_sm *sm = gWpsSm;
    int ret = -1;

    if (!gWpsSm) {
        return -1;
    }

    if (sm->wps->state == WPS_FINISHED) {
#if 0
        wifi_config_t *config = (wifi_config_t *)os_zalloc(sizeof(wifi_config_t));

        if (config == NULL) {
            wifi_event_sta_wps_fail_reason_t reason_code = WPS_FAIL_REASON_NORMAL;
            esp_event_send_internal(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_FAILED, &reason_code, sizeof(reason_code), portMAX_DELAY);
            return -1;
        }
#endif

        wpa_printf(MSG_DEBUG, "wps finished------>");
        wps_set_status(WPS_STATUS_SUCCESS);
        /* ets_timer_disarm(&sm->wps_timeout_timer); */
        /* ets_timer_disarm(&sm->wps_msg_timeout_timer); */

        if (sm->ap_cred_cnt == 1) {
            wps_evq_msg_t msg = { .event = WPS_EVQ_SUCCESS };
            xQueueSend(sm->event_queue, &msg, portMAX_DELAY);
#if 0
            memset(config, 0x00, sizeof(wifi_sta_config_t));
            memcpy(config->sta.ssid, sm->ssid[0], sm->ssid_len[0]);
            memcpy(config->sta.password, sm->key[0], sm->key_len[0]);
            memcpy(config->sta.bssid, sm->bssid, ETH_ALEN);
            config->sta.bssid_set = 0;
            esp_wifi_set_config(0, config);

            os_free(config);
            config = NULL;
#endif
        }
#if 0
        ets_timer_disarm(&sm->wps_success_cb_timer);
        ets_timer_arm(&sm->wps_success_cb_timer, 1000, 0);
#endif

        ret = 0;
    } else {
        wpa_printf(MSG_ERROR, "wps failed----->");

        ret = wps_stop_process();
    }

    return ret;
}

static int wps_send_eap_identity_rsp_(u8 id)
{
    struct wps_sm *sm = gWpsSm;
    struct wpabuf *eap_buf = NULL;
    u8 bssid[6];
    u8 *buf = NULL;
    int len;
    int ret = 0;

    wpa_printf(MSG_DEBUG, "wps send eapol id rsp");
    eap_buf = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_IDENTITY, sm->identity_len,
                            EAP_CODE_RESPONSE, id);
    if (!eap_buf) {
        ret = -1;
        goto _err;
    }

    ret = bl_wifi_get_assoc_bssid_internal(sm->vif_idx, bssid);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "bssid is empty!");
        return -1;
    }

    wpabuf_put_data(eap_buf, sm->identity, sm->identity_len);

    buf = wpa_sm_alloc_eapol(WPS_SM_EAPOL_VERSION, IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head_u8(eap_buf), wpabuf_len(eap_buf), (size_t *)&len, NULL);
    if (!buf) {
        ret = -1;
        goto _err;
    }

    ret = wpa_sm_ether_send(sm->ownaddr, bssid, ETH_P_EAPOL, buf, len);
    if (ret) {
        ret = -1;
        goto _err;
    }

_err:
    wpa_sm_free_eapol(buf);
    wpabuf_free(eap_buf);
    return ret;
}

static int wps_send_frag_ack_(u8 id)
{
    struct wps_sm *sm = gWpsSm;
    struct wpabuf *eap_buf = NULL;
    u8 bssid[6];
    u8 *buf;
    int len;
    int ret = 0;
    enum wsc_op_code opcode = WSC_FRAG_ACK;

    wpa_printf(MSG_DEBUG, "send frag ack id:%d", id);

    if (!sm) {
        return -1;
    }

    ret = bl_wifi_get_assoc_bssid_internal(sm->vif_idx, bssid);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "bssid is empty!");
        return ret;
    }

    eap_buf = eap_msg_alloc(EAP_VENDOR_WFA, 0x00000001, 2, EAP_CODE_RESPONSE, id);
    if (!eap_buf) {
        ret = -1;
        goto _err;
    }

    wpabuf_put_u8(eap_buf, opcode);
    wpabuf_put_u8(eap_buf, 0x00); /* flags */

    buf = wpa_sm_alloc_eapol(WPS_SM_EAPOL_VERSION, IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head_u8(eap_buf), wpabuf_len(eap_buf), (size_t *)&len, NULL);
    if (!buf) {
        ret = -1;
        goto _err;
    }

    ret = wpa_sm_ether_send(sm->ownaddr, bssid, ETH_P_EAPOL, buf, len);
    wpa_sm_free_eapol(buf);
    if (ret) {
        ret = -1;
        goto _err;
    }

_err:
    wpabuf_free(eap_buf);
    return ret;
}

static int wps_enrollee_process_msg_frag_(struct wpabuf **buf, int tot_len, u8 *frag_data, int frag_len, u8 flag)
{
    struct wps_sm *sm = gWpsSm;
    u8 identifier;

    if (!sm) {
        return -1;
    }

    identifier = sm->current_identifier;

    if (buf == NULL || frag_data == NULL) {
        wpa_printf(MSG_ERROR, "fun:%s. line:%d, frag buf or frag data is null", __FUNCTION__, __LINE__);
        return -1;
    }

    if (*buf == NULL) {
        if (0 == (flag & WPS_MSG_FLAG_LEN) || tot_len < frag_len) {
            wpa_printf(MSG_ERROR, "fun:%s. line:%d, flag error:%02x", __FUNCTION__, __LINE__, flag);
            return -1;
        }

        *buf = wpabuf_alloc(tot_len);
        if (*buf == NULL) {
            return -1;
        }

        wpabuf_put_data(*buf, frag_data, frag_len);
        return wps_send_frag_ack_(identifier);
    }

    if (flag & WPS_MSG_FLAG_LEN) {
        wpa_printf(MSG_ERROR, "fun:%s. line:%d, flag error:%02x", __FUNCTION__, __LINE__, flag);
        return -1;
    }

    wpabuf_put_data(*buf, frag_data, frag_len);

    if (flag & WPS_MSG_FLAG_MORE) {
        return wps_send_frag_ack_(identifier);
    }

    return 0;
}

static int wps_process_wps_mX_req_(u8 *ubuf, int len, enum wps_process_res *res)
{
    struct wps_sm *sm = gWpsSm;
    static struct wpabuf *wps_buf = NULL;
    struct eap_expand *expd;
    int tlen = 0;
    u8 *tbuf;
    u8 flag;
    int frag_len;
    u16 be_tot_len = 0;

    if (!sm) {
        return -1;
    }

    expd = (struct eap_expand *) ubuf;
    wpa_printf(MSG_DEBUG, "wps process mX req: len %d, tlen %d", len, tlen);

    flag = *(u8 *)(ubuf + sizeof(struct eap_expand));
    if (flag & WPS_MSG_FLAG_LEN) {
        tbuf = ubuf + sizeof(struct eap_expand) + 1 + 2;//two bytes total length
        frag_len = len - (sizeof(struct eap_expand) + 1 + 2);
        be_tot_len = *(u16 *)(ubuf + sizeof(struct eap_expand) + 1);
        tlen = ((be_tot_len & 0xff) << 8) | ((be_tot_len >> 8) & 0xff);
    } else {
        tbuf = ubuf + sizeof(struct eap_expand) + 1;
        frag_len = len - (sizeof(struct eap_expand) + 1);
        tlen = frag_len;
    }

    if ((flag & WPS_MSG_FLAG_MORE) || wps_buf != NULL) {//frag msg
        wpa_printf(MSG_DEBUG, "rx frag msg id:%d, flag:%d, frag_len: %d, tot_len: %d, be_tot_len:%d", sm->current_identifier, flag, frag_len, tlen, be_tot_len);
        if (0 != wps_enrollee_process_msg_frag_(&wps_buf, tlen, tbuf, frag_len, flag)) {
            if (wps_buf) {
                wpabuf_free(wps_buf);
                wps_buf = NULL;
            }
            return -1;
        }
        if (flag & WPS_MSG_FLAG_MORE) {
            if (res) {
                *res = WPS_FRAGMENT;
            }
            return 0;
        }
    } else { //not frag msg
        if (wps_buf) {//if something wrong, frag msg buf is not freed, free first
            wpa_printf(MSG_ERROR, "something is wrong, frag buf is not freed");
            wpabuf_free(wps_buf);
            wps_buf = NULL;
        }
        wps_buf = wpabuf_alloc_copy(tbuf, tlen);
    }

    if (!wps_buf) {
        return -1;
    }

    /* ets_timer_disarm(&sm->wps_msg_timeout_timer); */

    if (res) {
        *res = wps_enrollee_process_msg(sm->wps, expd->opcode, wps_buf);
    } else {
        wps_enrollee_process_msg(sm->wps, expd->opcode, wps_buf);
    }

    if (wps_buf) {
        wpabuf_free(wps_buf);
        wps_buf = NULL;
    }
    return 0;
}

static int wps_send_wps_mX_rsp_(u8 id)
{
    struct wps_sm *sm = gWpsSm;
    struct wpabuf *eap_buf = NULL;
    struct wpabuf *wps_buf = NULL;
    u8 bssid[6];
    u8 *buf;
    int len;
    int ret = 0;
    enum wsc_op_code opcode;

    wpa_printf(MSG_DEBUG, "wps send wps mX rsp");

    if (!sm) {
        return -1;
    }

    ret = bl_wifi_get_assoc_bssid_internal(sm->vif_idx, bssid);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "bssid is empty!");
        return ret;
    }

    wps_buf = (struct wpabuf *)wps_enrollee_get_msg(sm->wps, &opcode);
    if (!wps_buf) {
        ret = -1;
        goto _err;
    }

    eap_buf = eap_msg_alloc(EAP_VENDOR_WFA, 0x00000001, wpabuf_len(wps_buf) + 2, EAP_CODE_RESPONSE, id);
    if (!eap_buf) {
        ret = -1;
        goto _err;
    }

    wpabuf_put_u8(eap_buf, opcode);
    wpabuf_put_u8(eap_buf, 0x00); /* flags */
    wpabuf_put_data(eap_buf, wpabuf_head_u8(wps_buf), wpabuf_len(wps_buf));


    wpabuf_free(wps_buf);

    buf = wpa_sm_alloc_eapol(WPS_SM_EAPOL_VERSION, IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head_u8(eap_buf), wpabuf_len(eap_buf), (size_t *)&len, NULL);
    if (!buf) {
        ret = -1;
        goto _err;
    }

    ret = wpa_sm_ether_send(sm->ownaddr, bssid, ETH_P_EAPOL, buf, len);
    wpa_sm_free_eapol(buf);
    if (ret) {
        ret = -1;
        goto _err;
    }

_err:
    wpabuf_free(eap_buf);
    return ret;
}

static int wps_start_msg_timer_(void)
{
#if 0
    struct wps_sm *sm = gWpsSm;
    uint32_t msg_timeout;
    int ret = -1;

    if (!gWpsSm) {
        return -1;
    }

    if (sm->wps->state == WPS_FINISHED) {
        msg_timeout = 100;
        wpa_printf(MSG_DEBUG, "start msg timer WPS_FINISHED %d ms", msg_timeout);
        ets_timer_disarm(&sm->wps_msg_timeout_timer);
        ets_timer_arm(&sm->wps_msg_timeout_timer, msg_timeout, 0);
        ret = 0;
    } else if (sm->wps->state == RECV_M2) {
        msg_timeout = 5000;
        wpa_printf(MSG_DEBUG, "start msg timer RECV_M2 %d ms", msg_timeout);
        ets_timer_disarm(&sm->wps_msg_timeout_timer);
        ets_timer_arm(&sm->wps_msg_timeout_timer, msg_timeout, 0);
        ret = 0;
    }
    return ret;
#endif
    return 0;
}

static int wps_sm_rx_eapol_(u8 *src_addr, u8 *buf, u32 len)
{
    struct wps_sm *sm = gWpsSm;
    u32 plen, data_len, eap_len;
    struct ieee802_1x_hdr *hdr;
    struct eap_hdr *ehdr;
    u8 *tmp;
    u8 eap_code;
    u8 eap_type;
    int ret = -1;
    enum wps_process_res res = WPS_DONE;

    if (!gWpsSm) {
        return -1;
    }

    if (len < sizeof(*hdr) + sizeof(*ehdr)) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG,  "WPA: EAPOL frame too short to be a WPA "
                   "EAPOL-Key (len %lu, expecting at least %lu)",
                   (unsigned long) len,
                   (unsigned long) sizeof(*hdr) + sizeof(*ehdr));
#endif
        return 0;
    }

    tmp = buf;

    hdr = (struct ieee802_1x_hdr *) tmp;
    ehdr = (struct eap_hdr *) (hdr + 1);
    plen = be_to_host16(hdr->length);
    data_len = plen + sizeof(*hdr);
    eap_len = be_to_host16(ehdr->length);

#ifdef DEBUG_PRINT
    wpa_printf(MSG_DEBUG, "IEEE 802.1X RX: version=%d type=%d length=%ld",
               hdr->version, hdr->type, plen);
#endif

    if (hdr->version < EAPOL_VERSION) {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAP_PACKET) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPS: EAP frame (type %u) discarded, "
                   "not a EAP PACKET frame", hdr->type);
#endif
        ret = 0;
        goto out;
    }
    if (plen > len - sizeof(*hdr) || plen < sizeof(*ehdr)) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame payload size %lu "
                   "invalid (frame size %lu)",
                   (unsigned long) plen, (unsigned long) len);
#endif
        ret = 0;
        goto out;
    }

    wpa_hexdump(MSG_MSGDUMP, "WPA: RX EAPOL-EAP PACKET", tmp, len);

    if (data_len < len) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPA: ignoring %lu bytes after the IEEE "
                   "802.1X data", (unsigned long) len - data_len);
#endif
    }

    if (eap_len != plen) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPA: EAPOL length %lu "
                   "invalid (eapol length %lu)",
                   (unsigned long) eap_len, (unsigned long) plen);
#endif
        ret = 0;
        goto out;
    }

    eap_code = ehdr->code;
    switch (eap_code) {
    case EAP_CODE_SUCCESS:
        wpa_printf(MSG_DEBUG, "error: receive eapol success frame!");
        ret = 0;
        break;
    case EAP_CODE_FAILURE:
        wpa_printf(MSG_DEBUG, "receive eap code failure!");
        ret = wps_finish();
        break;
    case EAP_CODE_RESPONSE:
        wpa_printf(MSG_DEBUG, "error: receive eapol response frame!");
        ret = 0;
        break;
    case EAP_CODE_REQUEST: {
        eap_type = ((u8 *)ehdr)[sizeof(*ehdr)];
        switch (eap_type) {
        case EAP_TYPE_IDENTITY:
            wpa_printf(MSG_DEBUG, "=========identity===========");
            sm->current_identifier = ehdr->identifier;
            bl_wifi_timer_disarm(&sm->wps_eapol_start_timer);
            wpa_printf(MSG_DEBUG,  "WPS: Build EAP Identity.");
            ret = wps_send_eap_identity_rsp_(ehdr->identifier);
            bl_wifi_timer_arm(&sm->wps_eapol_start_timer, 3000, 0);
            break;
        case EAP_TYPE_EXPANDED:
            wpa_printf(MSG_DEBUG, "=========expanded plen[%ld], %d===========", plen, sizeof(*ehdr));
            if (ehdr->identifier == sm->current_identifier) {
                ret = 0;
                wpa_printf(MSG_DEBUG, "wps: ignore overlap identifier");
                goto out;
            }
            sm->current_identifier = ehdr->identifier;

            tmp = (u8 *)(ehdr + 1) + 1;
            ret = wps_process_wps_mX_req_(tmp, plen - sizeof(*ehdr) - 1, &res);
            if (ret == 0 && res != WPS_FAILURE && res != WPS_IGNORE && res != WPS_FRAGMENT) {
                ret = wps_send_wps_mX_rsp_(ehdr->identifier);
                if (ret == 0) {
                    wpa_printf(MSG_DEBUG, "sm->wps->state = %d", sm->wps->state);
                    wps_start_msg_timer_();
                }
            } else if (ret == 0 && res == WPS_FRAGMENT) {
                wpa_printf(MSG_DEBUG, "wps frag, continue...");
                ret = 0;
            } else if (res == WPS_IGNORE) {
                wpa_printf(MSG_DEBUG, "IGNORE overlap Mx");
                ret = 0; /* IGNORE the overlap */
            } else {
                ret = -1;
            }
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
out:
    if (ret != 0 || res == WPS_FAILURE) {
#if 0
        wifi_event_sta_wps_fail_reason_t reason_code = WPS_FAIL_REASON_NORMAL;
        wpa_printf(MSG_DEBUG, "wpa rx eapol internal: fail ret=%d", ret);
        wps_set_status(WPS_STATUS_DISABLE);
        esp_wifi_disarm_sta_connection_timer_internal();
        ets_timer_disarm(&sm->wps_timeout_timer);

        esp_event_send_internal(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_FAILED, &reason_code, sizeof(reason_code), portMAX_DELAY);
#endif
        wpa_printf(MSG_DEBUG, "wpa rx eapol internal: fail ret=%d", ret);
        wps_set_status(WPS_STATUS_DISABLE);
        wps_evq_msg_t msg = { .event = WPS_EVQ_FAILURE };
        xQueueSend(sm->event_queue, &msg, portMAX_DELAY);
        return ret;
    }

    return ret;
}

static int wps_tx_start_(void)
{
    struct wps_sm *sm = gWpsSm;
    u8 bssid[6];
    u8 *buf;
    int len;
    int ret = 0;

    if (!sm) {
        return -1;
    }

    ret = bl_wifi_get_assoc_bssid_internal(sm->vif_idx, bssid);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "bssid is empty!");
        return ret;
    }

    wpa_printf(MSG_DEBUG,  "WPS: Send EAPOL START.");
    buf = wpa_sm_alloc_eapol(WPS_SM_EAPOL_VERSION, IEEE802_1X_TYPE_EAPOL_START, (u8 *)"", 0, (size_t *)&len, NULL);
    if (!buf) {
        return -1;
    }

    wpa_sm_ether_send(sm->ownaddr, bssid, ETH_P_EAPOL, buf, len);
    wpa_sm_free_eapol(buf);

    bl_wifi_timer_arm(&sm->wps_eapol_start_timer, 3000, 0);

    return 0;
}

static int wps_config_(uint8_t vif_idx, uint8_t sta_idx)
{
    struct wps_sm *sm = gWpsSm;
    if (!sm) {
        return -1;
    }

    sm->vif_idx = vif_idx;
    sm->sta_idx = sta_idx;

    return 0;
}

static int wps_start_pending_(void)
{
    struct wps_sm *sm = gWpsSm;
    if (!sm) {
        return -1;
    }

    return wps_tx_start_();
}

static const struct wps_funcs wps_cb = {
    .wps_parse_scan_result = wps_parse_scan_result,
    .wps_sm_rx_eapol       = wps_sm_rx_eapol_,
    .wps_config            = wps_config_,
    .wps_start_pending     = wps_start_pending_,
};

static void wps_deinit_(void)
{
    struct wps_data *data = gWpsSm->wps;

    if (data->wps_pin_revealed) {
        wpa_printf(MSG_DEBUG,  "WPS: Full PIN information revealed and "
                   "negotiation failed");
    } else if (data->registrar)
        wpa_printf(MSG_DEBUG,  "WPS: register information revealed and "
                   "negotiation failed");
    wpabuf_free(data->dh_privkey);

    data->dh_privkey = NULL;

    wpabuf_free(data->dh_pubkey_e);
    wpabuf_free(data->dh_pubkey_r);
    wpabuf_free(data->last_msg);
    os_free(data->dev_password);
    dh5_free(data->dh_ctx);
    wps_dev_deinit_(&data->peer_dev);
    os_free(data);
}

static int wifi_station_wps_init_(void)
{
    struct wps_sm *sm = gWpsSm;

    wpa_printf(MSG_DEBUG, "wifi sta wps init");

    bl_wifi_mac_addr_get(sm->ownaddr);

    sm->discover_ssid_cnt = 0;
    sm->ignore_sel_reg = false;
    sm->discard_ap_cnt = 0;
    memset(&sm->dis_ap_list, 0, WPS_MAX_DIS_AP_NUM * sizeof(struct discard_ap_list_t));
    sm->identity_len = 29;
    memcpy(sm->identity, WPS_EAP_EXT_VENDOR_TYPE, sm->identity_len);

    sm->is_wps_scan = false;

    sm->wps_ctx = (struct wps_context *)os_zalloc(sizeof(struct wps_context));
    if (!sm->wps_ctx) {
        goto _err;
    }

    if (wps_dev_init_() != 0) {
        goto _err;
    }

    if ((sm->wps = wps_init_()) == NULL) {
        goto _err;
    }

    sm->scan_cnt = 0;

    bl_wifi_set_wps_cb_internal(&wps_cb);

    return 0;

_err:
    if (sm->dev) {
        wps_dev_deinit_(sm->dev);
        sm->dev = NULL;
    }
    if (sm->wps) {
        wps_deinit_();
        sm->wps = NULL;
    }
    return -1;
}

static int wifi_station_wps_deinit_(void)
{
    struct wps_sm *sm = gWpsSm;

    if (gWpsSm == NULL) {
        return -1;
    }

    aos_unregister_event_filter(EV_WIFI, wifi_event_cb_, sm);

    if (sm->event_queue) {
        vQueueDelete(sm->event_queue);
    }

    if (sm->dev) {
        wps_dev_deinit_(sm->dev);
        sm->dev = NULL;
    }
    if (sm->wps_ctx) {
        os_free(sm->wps_ctx);
        sm->wps_ctx = NULL;
    }
    if (sm->wps) {
        wps_deinit_();
        sm->wps = NULL;
    }
    if (sm) {
        os_free(gWpsSm);
        gWpsSm = NULL;
    }

    return 0;
}

bl_wps_err_t bl_wifi_wps_start(const struct bl_wps_config *config)
{
    struct wps_sm *sm = gWpsSm;
    bl_wps_err_t ret = BL_WPS_ERR_OK;
    int wifi_state = 0;

    if (sm) {
        wpa_printf(MSG_ERROR, "wps context already created\n");
        ret = BL_WPS_ERR_DUPLICATE_INSTANCE;
        goto ret;
    }

    wifi_mgmr_state_get(&wifi_state);
    if (!(wifi_state == WIFI_STATE_IDLE || wifi_state == WIFI_STATE_WITH_AP_IDLE)) {
        wpa_printf(MSG_ERROR, "wps is not allowed in current wifi state");
        ret = BL_WPS_ERR_WIFI_STATE;
        goto ret;
    }
    if ((sm = os_zalloc(sizeof(struct wps_sm))) == NULL) {
        ret = BL_WPS_ERR_MEMORY;
        goto ret;
    }

    if (!(sm->event_queue = xQueueCreate(WPS_EVENT_QUEUE_CAPACITY, sizeof(wps_evq_msg_t)))) {
        ret = BL_WPS_ERR_MEMORY;
        goto ret;
    }
    sm->cfg.type = config->type;
    sm->cfg.event_cb = config->event_cb;
    sm->cfg.event_cb_arg = config->event_cb_arg;

    gWpsSm = sm;

    wps_set_factory_info_(config);
    if (wifi_station_wps_init_()) {
        ret = BL_WPS_ERR_MEMORY;
        goto err;
    }
    if (pdPASS != xTaskCreate(wps_task_, "wps", WPS_TASK_STACK_DEPTH, NULL, 10, &sm->wps_task_hdl)) {
        ret = BL_WPS_ERR_MEMORY;
        goto err;
    }

    goto ret;

err:
    wps_set_status(WPS_STATUS_DISABLE);
    vPortFree(sm);
    gWpsSm = NULL;
ret:
    return ret;
}
