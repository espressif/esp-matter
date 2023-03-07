#ifndef _BL_BLE_SYNC_H_
#define _BL_BLE_SYNC_H_

#include "conn.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <stream_buffer.h>
#include "transfer.h"

#define BLSYNC_BLE_VERSION "v1.1.0"

#define BLE_PROV_BUF_SIZE 256
#define BLE_PROV_TASK_STACK_SIZE 512
#define WIFI_SCAN_ITEMS_MAX 50
#define BLE_PROV_QUEUE_NUMS 2

#define BT_UUID_WIFI_PROV   BT_UUID_DECLARE_16(0xffff)

#define BT_UUID_WIFIPROV_WRITE    BT_UUID_DECLARE_16(0xff01)
#define BT_UUID_WIFIPROV_READ     BT_UUID_DECLARE_16(0xff02)

typedef struct blesync_wifi_item {
    char ssid[32];
    uint32_t ssid_len;
    uint8_t bssid[6];
    uint8_t channel;
    uint8_t auth;
    int8_t rssi;
} blesync_wifi_item_t;

struct wifi_conn {
    uint8_t ssid[32];
    uint8_t ssid_tail[1];
    uint8_t pask[64];
};

typedef void (*pfn_complete_cb_t) (void *p_arg);

struct blesync_wifi_func {
    void (*local_connect_remote_ap) (struct wifi_conn *conn_info);
    void (*local_disconnect_remote_ap) (void);
    void (*local_wifi_scan) (void(*complete)(void *));
    void (*local_wifi_state_get) (void(*state_get)(void *));
};

struct queue_buf {
    uint8_t buf[BLE_PROV_BUF_SIZE];
    uint32_t len;
};

struct wifi_state {
    char ip[16];
    char gw[16];
    char mask[16];
    char ssid[32];
    char ssid_tail[1];
    uint8_t bssid[6];
    uint8_t state;
};


#if defined(CONFIG_ZIGBEE_PROV)
struct zb_info
{
    uint16_t  panid;
    uint8_t   installcode[16];
    uint8_t   linkkey[16];
};
struct blesync_zb_func
{

    void (*zb_get_installcode) (void);
    void (*zb_join_network)(void);
    void (*zb_setlinkkey)(uint8_t*linkkey);
    void (*zb_setpanid) (uint16_t  panid);
    void (*zb_reset) (void);
};
#endif

typedef struct bl_ble_sync {
    SemaphoreHandle_t xSemaphore;
    StaticSemaphore_t xSemaphoreBuffer;
    struct wifi_state state;

    QueueHandle_t xQueue1;
    StaticQueue_t xQueueBuffer;
    struct queue_buf buf[BLE_PROV_QUEUE_NUMS];
    struct queue_buf send_buf;

    pro_handle_t pro_handle;
    pfn_complete_cb_t complete_cb;
    void *p_arg;

    struct blesync_wifi_func *wifi_func;

    uint8_t attr_read_buf[BLE_PROV_BUF_SIZE];
    uint8_t attr_write_buf[BLE_PROV_BUF_SIZE];
    uint16_t rbuf_len;

    uint8_t task_runing;
    uint8_t scaning;
    uint8_t stop_flag;

    struct wifi_conn conn_info;

    struct bt_conn *p_cur_conn;

    StackType_t stack[BLE_PROV_TASK_STACK_SIZE];
    StaticTask_t task;
    TaskHandle_t task_handle;

    struct pro_dev pro_dev;

    blesync_wifi_item_t ap_item[WIFI_SCAN_ITEMS_MAX];
    uint8_t w_ap_item;
    uint8_t r_ap_item;

#if defined(CONFIG_ZIGBEE_PROV)
    struct blesync_zb_func *zb_func;
    struct zb_info zb_info;
#endif

} bl_ble_sync_t;

int bl_ble_sync_start(bl_ble_sync_t *index,
                       struct blesync_wifi_func *func,
                       pfn_complete_cb_t cb,
                       void *cb_arg);

int bl_ble_sync_stop(bl_ble_sync_t *index);

#if defined(CONFIG_ZIGBEE_PROV)
int bl_blezb_sync_start(bl_ble_sync_t *index,
                                struct blesync_zb_func *func,
                                 pfn_complete_cb_t cb,
                                void *cb_arg);
int bl_blezb_sync_stop(bl_ble_sync_t *index);
#endif


#if defined(CONFIG_BT_MESH)
#include "include/access.h"
const struct bt_mesh_model_op vnd_sync_op[4];

#define BFL_BLE_MESH_MODEL_VND_SYNC_SRV()         \
		BT_MESH_MODEL_VND_CB(BL_COMP_ID, BT_MESH_VND_MODEL_ID_SYNC_SRV, vnd_sync_op, \
								NULL, NULL, NULL)

int bl_blemesh_sync_start(bl_ble_sync_t *index,
                       struct blesync_wifi_func *func,
                       pfn_complete_cb_t cb,
                       void *cb_arg);

int bl_blemesh_sync_stop(bl_ble_sync_t *index);
#endif/* CONFIG_BT_MESH */

#endif 
