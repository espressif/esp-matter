/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#if defined(CFG_BLE_PERIPHERAL_AUTORUN)
#include "conn.h"
#include "conn_internal.h"
#include "gatt.h"
#include "hci_core.h"
#include "bl_port.h"
#include "ble_peripheral.h"
#include "log.h"
#include "app_common.h"

#include <aos/kernel.h>
#include <aos/yloop.h>
#include <stdlib.h>

#define APP_DEBUG(fmt, ...)  printf("[app]:"fmt", %s\r\n", ##__VA_ARGS__, __func__)

static void BleInitIndHandler(void *data);
static void BleStartAdvIndHandler(void *data);
static void BleConnectedCfmHandler(void *data);
static void BleStartSecurityIndHandler(void *data);
static void BlePairingFailedCfmHandler(void *data);
static void BlePairingSucceedCfmHandler(void *data);
static void BleDisconnectCfmHandler(void *data);
static void AppHandler(input_event_t *event, void *private_data);

static struct bt_conn *BleConn = NULL;
static uint8_t BleInited = 0;

static const struct {
    u32_t  id;
    void  (*func)(void* pdata);
} BleHandler[]={
    { BLE_INIT_IND,             BleInitIndHandler},
    { BLE_START_ADV_IND,        BleStartAdvIndHandler},
    { BLE_CONNECTED_CFM,        BleConnectedCfmHandler},
    { BLE_START_SECURITY_IND,   BleStartSecurityIndHandler},
    { BLE_PAIRING_FAILED_CFM,   BlePairingFailedCfmHandler},
    { BLE_PAIRING_SUCCEED_CFM,  BlePairingSucceedCfmHandler},
    { BLE_DISCONNECT_CFM,       BleDisconnectCfmHandler},
};
static void BleConnectedCb(struct bt_conn *conn, u8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if(err || conn->type != BT_CONN_TYPE_LE){
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    if (err > 0) {
        APP_DEBUG("failed to connect to %s (%u)", addr, err);
        return;
    }

    APP_DEBUG("connected: %s", addr);
    if (!BleConn) {
        BleConn = conn;
    }

    SEND_EVENT_NOW(BLE_EVT,BLE_CONNECTED_CFM,0);
}

static void BleDisconnectedCb(struct bt_conn *conn, u8_t reason)
{
    if(conn->type != BT_CONN_TYPE_LE){
        return;
    }

    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    APP_DEBUG("disconnected: %s (reason %u)", addr, reason);

    if (BleConn == conn) {
        #if defined(CONFIG_BT_CENTRAL)
        if(conn->role == BT_HCI_ROLE_MASTER)
            bt_conn_unref(conn);
        #endif
        BleConn = NULL;
    }

    SEND_EVENT_NOW(BLE_EVT,BLE_DISCONNECT_CFM,0);
}

#if defined(CONFIG_BT_SMP)
static void BleIdentityResolvedCb(struct bt_conn *conn,
                const bt_addr_le_t *rpa,
                const bt_addr_le_t *identity)
{
    char addr_identity[BT_ADDR_LE_STR_LEN];
    char addr_rpa[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(identity, addr_identity, sizeof(addr_identity));
    bt_addr_le_to_str(rpa, addr_rpa, sizeof(addr_rpa));

    APP_DEBUG("Identity resolved %s -> %s", addr_rpa, addr_identity);
}

static void security_changed(struct bt_conn *conn,
                bt_security_t level,
                enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    APP_DEBUG("Security changed: %s level %u", addr, level);
}
#endif //CONFIG_BT_SMP

static struct bt_conn_cb BleconnCb = {
    .connected = BleConnectedCb,
    .disconnected = BleDisconnectedCb,
    #if defined(CONFIG_BT_SMP)
    .identity_resolved = BleIdentityResolvedCb,
    .security_changed = security_changed,
    #endif
};

#if defined(CONFIG_BT_SMP)
static void BlePairingConfirmCb(struct bt_conn *conn)
{
    bt_conn_auth_pairing_confirm(conn);
}

static void BlePairingFailedCb(struct bt_conn *conn,enum bt_security_err reason)
{
    APP_DEBUG("%s err(%d)",__func__,reason);
    SEND_EVENT_NOW(BLE_EVT,BLE_PAIRING_FAILED_CFM,0);
}

static void BlePairingCompleteCb(struct bt_conn *conn,bool bonded)
{
    APP_DEBUG("bonded(%d)",bonded);
    if(bonded){
        SEND_EVENT_NOW(BLE_EVT,BLE_PAIRING_SUCCEED_CFM,0);
    }else{
        SEND_EVENT_NOW(BLE_EVT,BLE_PAIRING_FAILED_CFM,0);
    }
}

static void BlePairingPasskeyDisplayCb(struct bt_conn *conn, unsigned int passkey)
{
    APP_DEBUG("");
}

static void BlePairingPasskeyEntryCb(struct bt_conn *conn)
{
    APP_DEBUG("");
}

static void BlePairingPasskeyConfirmCb(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    APP_DEBUG("Confirm passkey for %s: %06u", addr, passkey);

    bt_conn_auth_passkey_confirm(conn);
}

static void BlePairingCancelCb(struct bt_conn *conn)
{
    APP_DEBUG("conn(%p)",conn);
}

static struct bt_conn_auth_cb BleAuthCb = {
    .passkey_display = BlePairingPasskeyDisplayCb,
    .passkey_entry = BlePairingPasskeyEntryCb,
    .passkey_confirm = BlePairingPasskeyConfirmCb,
    .cancel = BlePairingCancelCb,
    .pairing_confirm = BlePairingConfirmCb,
    .pairing_failed = BlePairingFailedCb,
    .pairing_complete = BlePairingCompleteCb,
};
#endif //CONFIG_BT_SMP

static void BleInitIndHandler(void* data)
{
    if(!BleInited){
        bt_conn_cb_register(&BleconnCb);
        bt_conn_auth_cb_register(&BleAuthCb);
        BleInited = 1;
        SEND_EVENT_NOW(BLE_EVT,BLE_START_ADV_IND,0);
    }else{
        APP_DEBUG("ble already init");
    }
}

static void BleStartAdvIndHandler(void* data)
{
    const char *dev_name = bt_get_name();
    struct bt_data ad[2] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL),
        BT_DATA(BT_DATA_NAME_COMPLETE,dev_name,strlen(dev_name)),
    };
    struct bt_data sd[1] = {
        BT_DATA(BT_DATA_NAME_COMPLETE,dev_name,strlen(dev_name)),
    };

    struct bt_le_adv_param param = {
        .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),
        .id = 0,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
    };
    int err;

    (void)data;

    err = bt_le_adv_start(&param,ad,ARRAY_SIZE(ad),sd,ARRAY_SIZE(sd));
    if(err){
        APP_DEBUG("Failed to start adv. err(%d)",err);
    }
}

static void BleConnectedCfmHandler(void* data)
{
    (void)data;
    SEND_EVENT_NOW(BLE_EVT,BLE_START_SECURITY_IND,0);
}

#if defined(CONFIG_BT_SMP)
static void BleStartSecurityIndHandler(void* data)
{
    int err;

    (void)data;

    if(!BleConn){
        APP_DEBUG("Not connected");
        return;
    }
    err = bt_conn_set_security(BleConn,BT_SECURITY_L4);
    if(err){
        APP_DEBUG("Failed to start security err(%d)",err);
    }
}

static void BlePairingFailedCfmHandler(void* data)
{
    (void)data;
    APP_DEBUG("Failed to start security");
}

static void BlePairingSucceedCfmHandler(void* data)
{
    (void)data;
    APP_DEBUG("Successful pairing");
}
#endif //CONFIG_BT_SMP

static void BleDisconnectCfmHandler(void* data)
{
    SEND_EVENT_NOW(BLE_EVT,BLE_START_ADV_IND,0);
}

void BleStart(void)
{
    APP_DEBUG("ble start");
    TASK_REGISTER(AppHandler,BLE_EVT,NULL);
    SEND_EVENT_NOW(BLE_EVT,BLE_INIT_IND,0);
}

static void AppHandler(input_event_t *event, void *private_data)
{
    int i;
    uint16_t id = event->code;

    for(i=0;i<sizeof(BleHandler);i++){
        if(id == BleHandler[i].id){
            BleHandler[i].func(private_data);
            return;
        }
    }
}
#endif
