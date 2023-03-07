/*
 * Copyright (c) 2020 Bouffblfolab.
 *
 * This file is part of
 *     *** Bouffblfolab Software Dev Kit ***
 *      (see www.bouffblfolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materiblfs provided with the distribution.
 *   3. Neither the name of Bouffblfo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHblfL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTblf, SPECIblf, EXEMPLARY, OR CONSEQUENTIblf
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ble_app.h"
#include <FreeRTOS.h>
#include <task.h>
#include "bluetooth.h"
#include "ble_cli_cmds.h"
#include "hci_driver.h"
#include "ble_lib_api.h"
#include <errno.h>
#include <aos/kernel.h>
#include <aos/yloop.h>

#include "conn.h"
#include "gatt.h"
#include "hci_core.h"
#include "uuid.h"
//#include "oad_api.h"
#include "bl_gpio.h"
#include <vfs.h>



static struct bt_conn *default_conn;
static struct bt_uuid_16 uuid = BT_UUID_INIT_16(0);
static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;
static u16_t bt_gatt_write_without_hanle=0;

static uint8_t notify_func(struct bt_conn *conn,
			   struct bt_gatt_subscribe_params *params,
			   const void *data, uint16_t length)
{
 	
	if(length!=0){
	
		uint8_t *recv_buffer;
		recv_buffer=pvPortMalloc(sizeof(uint8_t)*length);
		memcpy(recv_buffer, data, length);
		printf("ble notification=");
		for (size_t i = 0; i < length; i++)
		{
			printf("0x%x ",recv_buffer[i]);
		}
		vPortFree(recv_buffer);
		printf("\n");	

	}
    
	return BT_GATT_ITER_CONTINUE;
}
void bl_gatt_write_without_rsp(void)
{
 	char data[10] = {0x00,0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    if(default_conn!=NULL){

	bt_gatt_write_without_response(default_conn, bt_gatt_write_without_hanle, data, 10, 0);	

	}

}


void ble_appc_disconnected(void)
{
    if(default_conn!=NULL)
    {
        
        printf("user disconnect\n");
        bt_conn_disconnect(default_conn,BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        aos_post_event(EV_BLE_TEST,BLE_DEV_DISCONN,NULL);
    }
}
void ble_appc_stopscan(void)
{

	printf("user stop scan\n");
	bt_le_scan_stop();
	aos_post_event(EV_BLE_TEST,BLE_SCAN_STOP,NULL);
    
}
static uint8_t discover_func(struct bt_conn *conn,
			     const struct bt_gatt_attr *attr,
			     struct bt_gatt_discover_params *params)
{
	int err;

	if (!attr) {
		printf("Discover complete\n");
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

	printf("[ATTRIBUTE] handle %d\n", attr->handle);

	if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_TEST)) {

		memcpy(&uuid, BT_UUID_TEST_RX, sizeof(uuid));
		discover_params.uuid = &uuid.uuid;
		discover_params.start_handle = attr->handle + 1;
		discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

		err = bt_gatt_discover(conn, &discover_params);
		if (err) {
			printf("Discover failed (err %d)\n", err);
		}
	} else if (!bt_uuid_cmp(discover_params.uuid,
				BT_UUID_TEST_RX)) {
		memcpy(&uuid, BT_UUID_GATT_CCC, sizeof(uuid));
		discover_params.uuid = &uuid.uuid;
		discover_params.start_handle = attr->handle + 2;
		discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
		subscribe_params.value_handle = bt_gatt_attr_value_handle(attr);

		err = bt_gatt_discover(conn, &discover_params);
		if (err) {
			printf("Discover failed (err %d)\n", err);
		}
	} else if(!bt_uuid_cmp(discover_params.uuid,
				BT_UUID_GATT_CCC)) {
		subscribe_params.notify = notify_func;
		subscribe_params.value = BT_GATT_CCC_NOTIFY;
		subscribe_params.ccc_handle = attr->handle;
		err = bt_gatt_subscribe(conn, &subscribe_params);
		if (err && err != -EALREADY) {
			printf("Subscribe failed (err %d)\n", err);
		} else {
			printf("[SUBSCRIBED]\n");
			bt_gatt_write_without_hanle=subscribe_params.ccc_handle+2;
		}

	
		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_STOP;
}

static bool data_cb(struct bt_data *data, void *user_data)
{
    char *name = user_data;
    u8_t len;

    switch (data->type) {
    case BT_DATA_NAME_SHORTENED:
    case BT_DATA_NAME_COMPLETE:
        len = (data->data_len > NAME_LEN - 1)?(NAME_LEN - 1):(data->data_len);
        memcpy(name, data->data, len);
        return false;
    default:
        return true;
    }
}


static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
	char dev[BT_ADDR_LE_STR_LEN];
    char name[NAME_LEN];

	struct bt_le_conn_param param = {
		.interval_min =  BT_GAP_INIT_CONN_INT_MIN,
		.interval_max =  BT_GAP_INIT_CONN_INT_MAX,
		.latency = 0,
		.timeout = 400,
	};
    //struct bt_le_conn_param param;
	bt_addr_le_to_str(addr, dev, sizeof(dev));

    if (rssi < -20) 
    {
        return;
    }
	/* We're only interested in connectable events */
	if (type == BT_LE_ADV_IND) {

		bt_le_scan_stop();
		printf("device_found ok\r\n");
        (void)memset(name, 0, sizeof(name));
        bt_data_parse(ad, data_cb, name);
        printf("[DEVICE]: %s, AD evt type %u, AD data len %u, RSSI %i NAME=%s\n",
	       dev, type, ad->len, rssi,name);

        default_conn = bt_conn_create_le(addr, &param);

        if(!default_conn){
        printf("Connection failed\r\n");
        }else{
        printf("Connection pending\r\n");
        }

	}
    
}

void start_scan(void)
{
	int err;
 	struct bt_le_scan_param scan_param;

	scan_param.type=BT_LE_SCAN_TYPE_PASSIVE;
	scan_param.filter_dup=0;
	scan_param.interval=0x80;
	scan_param.window=0x40;
	err = bt_le_scan_start(&scan_param, device_found);
	if (err) {
		printf("Scanning failed to start (err %d)\n", err);
		return;
	}
	aos_post_event(EV_BLE_TEST,BLE_SCAN_START,NULL);
	printf("Scanning successfully started\n");
}

static void connected(struct bt_conn *conn, uint8_t conn_err)
{
	char addr[BT_ADDR_LE_STR_LEN];
	int err;

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (conn_err) {
		printf("Failed to connect to %s (%u)\n", addr, conn_err);

		bt_conn_unref(default_conn);
		default_conn = NULL;

		
		return;
	}

	printf("Connected: %s\n", addr);
	aos_post_event(EV_BLE_TEST,BLE_DEV_CONN,NULL);
	if (conn == default_conn) {
		memcpy(&uuid, BT_UUID_TEST, sizeof(uuid));
		discover_params.uuid = &uuid.uuid;
		discover_params.func = discover_func;
		discover_params.start_handle = 0x0001;
		discover_params.end_handle = 0xffff;
		discover_params.type = BT_GATT_DISCOVER_PRIMARY;

		err = bt_gatt_discover(default_conn, &discover_params);
		if (err) {
			printf("Discover failed(err %d)\n", err);
			return;
		}
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Disconnected: %s (reason 0x%02x)\n", addr, reason);

	if (default_conn != conn) {
		return;
	}

	bt_conn_unref(default_conn);
	default_conn = NULL;
	aos_post_event(EV_BLE_TEST,BLE_DEV_DISCONN,NULL);
}

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
};

static void bt_ready(void)
{
	start_scan();
}

static void ble_app_init(int err)
{
    if (err != 0) {
        printf("BT FAILED started\n");
    }else{
        printf("BT SUCCESS started\n");
        bt_ready();  
    }
   
}

void ble_appc_start(void)
{
	int err;
	ble_controller_init(configMAX_PRIORITIES - 1);
    // Initiblfize BLE Host stack
    hci_driver_init();
	err = bt_enable(ble_app_init);

	if (err) {
		printf("Bluetooth init failed (err %d)\n", err);
		return;
	}
	bt_conn_cb_register(&conn_callbacks);

}

