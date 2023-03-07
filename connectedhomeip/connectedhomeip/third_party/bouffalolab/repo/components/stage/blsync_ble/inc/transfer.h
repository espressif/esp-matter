/** @file
 *  @brief protocol
 */
#ifndef __TRANS_LAYER_H__
#define __TRANS_LAYER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include "bitops.h"
#include "encrypt_layer.h"
#include "pro_config.h"

#define PRO_CTRL_RETRY_BIT         (0)
#define PRO_CTRL_FRAG_BIT          (1)
#define PRO_CTRL_PORTE_BIT         (2)
#define PRO_CTRL_ACK_BIT           (3)
#define PRO_CTRL_TYPE_BIT          (4)
#define PRO_CTRL_VERSION_BIT       (6)

#define PACK_FRAG_END_BIT  (15)

#define PRO_RETRY         (1)
#define PRO_NOT_RETRY     (0)

#define PRO_VERSION00     (0)
#define PRO_VERSION01     (1)

#define PRO_TYPE_CMD      (0)
#define PRO_TYPE_DATA     (1)

#define PRO_ACK_PACK      (1)
#define PRO_NOT_ACK_PACK  (0)

#define PRO_PORTECTE      (1)
#define PRO_NOT_PORTECTE  (0)

typedef enum {
    CMD_ACK = 0x00,
    CMD_SET_SEC_MODE,
    CMD_SET_OP_MODE,
    CMD_CONNECT_WIFI,
    CMD_DISCONNECT_WIFI,
    CMD_GET_WIFI_STATUS,
    CMD_DEAUTHENTICATE,
    CMD_GET_VERSION,
    CMD_CLOSE_CONNECTION,
    CMD_WIFI_SCAN,
    CMD_DATA_GET_PREPARE,
    CMD_WIFI_DATA_GET,
    CMD_PROV_STOP,
    CMD_WIFI_STATE_GET,

#if defined(CONFIG_ZIGBEE_PROV)
    CMD_ZB_SCAN=0x20,
    CMD_ZB_JOIN,
    CMD_ZB_GET_INSTALLCODE,
    CMD_ZB_RESET,
#endif

} cmd_id_t;

typedef enum {
    DATA_ACK = 0x00,
    DATA_STA_WIFI_BSSID,
    DATA_STA_WIFI_SSID,
    DATA_STA_WIFI_PASSWORD,
    DATA_SOFTAP_WIFI_SSID,
    DATA_SOFTAP_WIFI_PASSWORD,
    DATA_SOFTAP_MAX_CONNECTION_COUNT,
    DATA_SOFTAP_AUTH_MODE,
    DATA_SOFTAP_CHANNEL,
    DATA_USERNAME,
    DATA_CA_CERTIFICATION,
    DATA_CLIENT_CERTIFICATION,
    DATA_SERVER_CERTIFICATION,
    DATA_CLIENT_PRIVATE_KEY,
    DATA_SERVER_PRIVATE_KEY,
    DATA_WIFI_CONNECTION_STATE,
    DATA_VERSION,
    DATA_WIFI_LIST,
    DATA_ERROR,
    DATA_CUSTOM_DATA,

#if defined(CONFIG_ZIGBEE_PROV)
    DATA_ZB_LINKKEY = 0x20,
    DATA_ZB_PANID,
    DATA_ZB_INSTALLCODE,
    DATA_ZB_SCAN_INFO,
#endif

} data_id_t;

typedef enum  {
    PROTOCOL_EVENT_CMD,
    PROTOCOL_EVENT_DATA
} event_type_t;

struct pro_event {
    event_type_t type;
    int event_id;
    void *p_buf;
    uint32_t length;
};

struct pro_func {
    int (*pfu_bytes_send)(void *p_drv, const void *buf, size_t bytes);

    int (*pfn_recv_event)(void *p_drv, struct pro_event *p_event);
};

struct pro_dev {

    enc_handle_t enc_handle;
    struct encrypt enc;

    uint8_t  old_seq;
    uint8_t *pyld_buf;
    struct pro_event ev;
    uint16_t tol_len_now;
    uint16_t total_length;

    uint16_t mtu;
    uint16_t ack_len;
    uint8_t *ack_buf;
    long long seq_start_ms;

    SemaphoreHandle_t xSemaphore;
    StaticSemaphore_t xSemaphoreBuffer;

    const struct pro_func *p_func;
    void *p_drv;
};

typedef struct pro_dev * pro_handle_t;

struct general_head {
    uint8_t ctrl;
    uint8_t seq;
    uint16_t frag_ctrl;
};

int pro_trans_write (pro_handle_t handle,
                     const void *p_data,
                     uint16_t length);

int pro_trans_read (pro_handle_t handle, const void *buf,
                     size_t bytes, uint16_t mtu);

int pro_trans_ack (pro_handle_t handle);

int pro_trans_layer_ack_read (pro_handle_t handle,
                                const void *ack_buf,
                                size_t bytes);

pro_handle_t pro_trans_init (struct pro_dev  *p_dev,
                             const struct pro_func *p_func,
                             void *p_drv);

void pro_trans_reset (pro_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif

