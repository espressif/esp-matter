#ifndef __FILOGIC_H__
#define __FILOGIC_H__


#include <stdbool.h>
#include <stdarg.h>

#include <wifi_api_ex.h>

#include <v3/fota_download.h>

#include "fota_flash_config.h"
#include "fota_osal.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    FILOGIC_START_OK,
    FILOGIC_START_NG,
    FILOGIC_WIFI_INIT_OK,
    FILOGIC_SET_OPMODE_OK,
    FILOGIC_AP_START_OK,
    FILOGIC_AP_START_NG,
    FILOGIC_AP_STATION_CONNECTED,
    FILOGIC_AP_STATION_DISCONNECTED,
    FILOGIC_STA_CONNECTED_TO_AP,
    FILOGIC_STA_DISCONNECTED_FROM_AP,
    FILOGIC_STA_CONNECTED_FAILED,
    FILOGIC_STA_IPV4_ADDR_READY,
    FILOGIC_STA_IPV6_ADDR_READY,
    FILOGIC_SCAN_DONE,
    FILOGIC_EVENT_ID_MAX
} filogic_async_event_id_t;


typedef enum
{
    FILOGIC_WIFI_OPMODE_NONE = 0,
    FILOGIC_WIFI_OPMODE_AP   = 1,
    FILOGIC_WIFI_OPMODE_STA  = 2,
    FILOGIC_WIFI_OPMODE_DUAL = 3,
    FILOGIC_WIFI_OPMODE_MAX,
} filogic_wifi_opmode_t;

typedef enum
{
    FILOGIC_OTA_SUCCESS = 0,
    FILOGIC_OTA_INIT_FAIL,
    FILOGIC_OTA_WRITE_FAIL,
    FILOGIC_OTA_DOWNLOAD_FAIL,
    FILOGIC_OTA_APPLY_FAIL,
    FILOGIC_OTA_UNKNOWN_FAIL,
    FILOGIC_OTA_STATUS_MAX,
} filogic_ota_state_t;

#define WIFI_SSID_MAX_LEN 32
#define WIFI_PSK_MAX_LEN  64
#define WIFI_MAC_LEN      6

typedef struct filogic_wifi_prov
{
    char        ssid[ WIFI_SSID_MAX_LEN ];
    size_t      ssid_len;
    char        psk [ WIFI_PSK_MAX_LEN  ];
    uint8_t     psk_len;
    uint8_t     auth_mode;
} filogic_wifi_sta_prov_t;


// TODO: module and level are currently unused
typedef void (*filogic_logv_cbk)(int module, int level, const char * msg, va_list args);


typedef enum
{
    FILOGIC_WIFI_PORT_STA,
    FILOGIC_WIFI_PORT_AP
} filogic_wifi_port_t;


typedef struct {
    filogic_async_event_id_t        event_id;
    union {
        struct {
            filogic_wifi_port_t     port;
        } wifi_init;
        struct {
            filogic_wifi_opmode_t   opmode;
        } wifi_opmode;
        struct {
            uint8_t addr[ WIFI_MAC_LEN ];
        } wifi_mac;
        struct {
            uint8_t addr[16]; /* 111.111.111.111\0 */
        } ipv4_str;
        struct {
            uint8_t addr[40]; /* 0000:0000:0000:0000:0000:0000:0000:0000\0 */
        } ipv6_str;
        struct {
            wifi_scan_list_item_t *ap_list;
        } scan_done;
        struct {
            uint8_t *payload;
            uint32_t length;
        } wifi_event;
    } u;
} filogic_async_event_data;


typedef void (*filogic_async_cbk)(void *,
                                  filogic_async_event_id_t,
                                  filogic_async_event_data *);


typedef void (*filogic_scan_async_cbk)(wifi_scan_list_item_t * aScanResult);

/* MTK WRAP Module Maintenance API */

void * filogic_start_sync(void);

void filogic_stop_sync(void *);

/* Provide an event callback to get events from FILOGIC platform */

bool filogic_set_event_callback_sync(void *, filogic_async_cbk);

/* LOG output to application */

bool filogic_set_logv_callback_sync(void *, filogic_logv_cbk);


/****************************************************************************
 * ID TO CORRESPONDING STRINGS
 ****************************************************************************/


const char *filogic_opmode_to_name(filogic_wifi_opmode_t opmode);


const char *filogic_event_to_name(filogic_async_event_id_t event);


/****************************************************************************
 * API, JOB HANDLER, EVENT HANDLER
 ****************************************************************************/


void filogic_wifi_init_async(void *c, filogic_wifi_opmode_t);


void filogic_wifi_opmode_set_async(void *, filogic_wifi_opmode_t);


void filogic_wifi_opmode_get_sync(void *, filogic_wifi_opmode_t *);


void filogic_wifi_sta_connect_async(void *);


void filogic_wifi_sta_disconnect_async(void *);


bool filogic_wifi_sta_get_link_status_sync(void *);


void filogic_wifi_sta_prov_set_sync(void *, filogic_wifi_sta_prov_t *);


bool filogic_wifi_sta_prov_get_sync(void *, filogic_wifi_sta_prov_t *);


void filogic_wifi_ap_config_async(void *, uint8_t channel, const char *ssid, int ssid_len);


void filogic_wifi_scan(void                     *c,
                       const uint8_t            *ssid,
                       uint8_t                  ssid_len,
                       uint8_t                  max_ap_num,
                       filogic_scan_async_cbk   callback);


void filogic_wifi_mac_addr_get_sync(void *c, filogic_wifi_opmode_t opmode, uint8_t *addr);


void filogic_ota_init_sync(void *c, void *ret);


void filogic_ota_io_write_sync(void *c, const void *addr, uint32_t len, void *ret);


void filogic_ota_apply_sync(void *c, void *ret);
#ifdef __cplusplus
}
#endif

#endif /* __FILOGIC_H__ */
