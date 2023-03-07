/****************************************************************************
FILE NAME
    atvv.c

DESCRIPTION
    google voice over ble service.
    <<Google Voice over BLE spec 1.0>>

****************************************************************************/

#include <sys/errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#include "gatt.h"
#include "hci_core.h"
#include "uuid.h"

#include "atvv.h"
#include "log.h"

#define AUDIO_TRANSFER_TIMEOUT K_SECONDS(30)
struct k_delayed_work audio_timeout_work;

static struct bt_conn *atvv_conn;
static struct bt_gatt_exchange_params exchg_mtu;
static uint8_t tx_mtu_size = 20;

static void atvv_connected(struct bt_conn *conn, u8_t err);
static void atvv_disconnected(struct bt_conn *conn, u8_t reason);
static int atvv_notify(struct bt_conn *conn, uint8_t attr_index, uint8_t *buf, uint16_t len);
static struct bt_gatt_attr *get_attr(u8_t index);

static struct bt_conn_cb atvv_conn_callbacks = {
	.connected	=   atvv_connected,
	.disconnected	=   atvv_disconnected,
};

static uint8_t audio_notify_enable = 0;
static uint8_t ctl_notify_enable = 0;

/*
0x01: ADPCM 8khz/16bit;
0x02: ADPCM 16khz/16bit;
*/
static uint8_t codecs_used = 0x01;
/*
0x00: On-request model (should be supported by all remotes);
0x01: Press-to-Talk model enabled;
0x03: Hold-to-Talk model enabled.
*/
static uint8_t assis_mode = 0x00;

/*************************************************************************
* NAME:  set_codecs_mode
*/
void set_codecs_mode(uint8_t used)
{
    codecs_used = used;
}

/*************************************************************************
* NAME:  set_assis_mode
*/
void set_assis_mode(uint8_t mode)
{
    assis_mode = mode;
}

/*************************************************************************
* NAME:  audio_search
*/
int audio_search()
{
    int err = -1;
    uint8_t audio_cmd[1] = {START_SEARCH};

    if (ctl_notify_enable)
    {
        err = atvv_notify(atvv_conn, ATVV_CHAR_CTL_ATTR_VAL_INDEX, audio_cmd, sizeof(audio_cmd));
    }

    return err;
}

/*************************************************************************
* NAME:  audio_stop
*/
int  audio_start(uint8_t reason, uint8_t stream)
{
    int err = -1;
    uint8_t audio_cmd[4] = {
            AUDIO_START, 
            reason,
            codecs_used,
            stream
        };

    if (ctl_notify_enable)
    {
        err = atvv_notify(atvv_conn, ATVV_CHAR_CTL_ATTR_VAL_INDEX, audio_cmd, sizeof(audio_cmd));
        if (!err)
        {
            k_delayed_work_submit(&audio_timeout_work, AUDIO_TRANSFER_TIMEOUT);
        }
    }

    return err;
}

/*************************************************************************
* NAME:  audio_stop
*/
int audio_stop(uint8_t reason)
{
    int err = -1;
    uint8_t audio_cmd[2] = {
            AUDIO_STOP, 
            reason
        };

    if (ctl_notify_enable)
    {
        err = atvv_notify(atvv_conn, ATVV_CHAR_CTL_ATTR_VAL_INDEX, audio_cmd, sizeof(audio_cmd));
        k_delayed_work_cancel(&audio_timeout_work);
    }

    return err;
}

/*************************************************************************
* NAME:  audio_transfer
*/
int audio_transfer(uint8_t *buf, uint16_t len)
{
    int err = -1;

    if (audio_notify_enable)
    {
        err = atvv_notify(atvv_conn, ATVV_CHAR_AUDIO_ATTR_VAL_INDEX, buf, len);
    }

    return err;
}

/*************************************************************************
* NAME:  audio_timeout
*/
static void audio_timeout(struct k_work *work)
{
    BT_WARN("audio transfer timeout");

    audio_stop(STOP_REASON_TIME_OUT);

}


/*************************************************************************
* NAME:  atvv_tx_mtu_size
*/
static void atvv_tx_mtu_size(struct bt_conn *conn, u8_t err, struct bt_gatt_exchange_params *params)
{
   if(!err)
   {
        tx_mtu_size = bt_gatt_get_mtu(atvv_conn);
        BT_INFO("atvv echange mtu size success, mtu size: %d", tx_mtu_size);
   }
   else
   {
        BT_WARN("atvv echange mtu size failure, err: %d", err);
   }
}

/*************************************************************************
* NAME: atvv_connected
*/
static void atvv_connected(struct bt_conn *conn, u8_t err)
{
    if(err || conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }

    int tx_octets = 0x00fb;
    int tx_time = 0x0848;
    int ret = -1;

    BT_INFO("%s", __func__);
    atvv_conn = conn;

    /*set data length after connected.*/
    ret = bt_le_set_data_len(atvv_conn, tx_octets, tx_time);
    if(!ret)
    {
        BT_WARN("atvv set data length success.");
    }
    else
    {
        BT_WARN("atvv set data length failure, err: %d", ret);
    }

    /*exchange mtu size after connected.*/
    exchg_mtu.func = atvv_tx_mtu_size;
    ret = bt_gatt_exchange_mtu(atvv_conn, &exchg_mtu);
    if (!ret) {
        BT_INFO("atvv exchange mtu size pending.");
    } else {
        BT_WARN("atvv exchange mtu size failure, err: %d", ret);
    }

    k_delayed_work_init(&audio_timeout_work, audio_timeout);
}

/*************************************************************************
NAME: atvv_disconnected
*/
static void atvv_disconnected(struct bt_conn *conn, u8_t reason)
{ 
    if(conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }

    BT_INFO("%s", __func__);

    atvv_conn = NULL;

    k_delayed_work_del_timer(&audio_timeout_work);
}


/*************************************************************************
*  NAME: atvv_notify
*/
static int atvv_notify(struct bt_conn *conn, uint8_t attr_index, uint8_t *buf, uint16_t len)
{
    int err = -1;

    err = bt_gatt_notify(conn, get_attr(attr_index), buf, len);
    if (!err)
    {
        BT_INFO("atvv send notify success.");
    } else
    {
        BT_WARN("atvv send notify : %d", err);
    }

    return err;
}

/*************************************************************************
*  NAME: atvv_tx
*/
static int atvv_tx(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                        const void *buf, u16_t len, u16_t offset, u8_t flags)
{
    BT_INFO("recv data len=%d, offset=%d, flag=%d", len, offset, flags);

    uint8_t command = ((uint8_t *)buf)[0];
    switch(command)
    {
        case GET_CAPS:
        {
            uint8_t caps_rsp[9] = {
                CAPS_RESP, 
                0x01, 0x00,
                0x03,
                assis_mode,
                0x00, (tx_mtu_size - 3),
                0x01,
                0x00
                };

            if (ctl_notify_enable)
            {
                atvv_notify(conn, ATVV_CHAR_CTL_ATTR_VAL_INDEX, caps_rsp, sizeof(caps_rsp));
            }
        }
        break;

        case MIC_OPEN:
        {
            audio_start(START_REASON_MIC_OPEN, 0);
        }
        break;

        case MIC_CLOSE:
        {
            audio_stop(STOP_REASON_MIC_CLOSE);
        }
        break;

        case MIC_EXTEND:
        {
            k_delayed_work_cancel(&audio_timeout_work);
            k_delayed_work_submit(&audio_timeout_work, AUDIO_TRANSFER_TIMEOUT);
        }
        break;
            
        default:
            break;
    }

    return len;
}

/*************************************************************************
*  NAME: atvv_audio_ccc_changed
*/ 
static void atvv_audio_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    BT_INFO("ccc:value=[%d]", value);

    if (value == BT_GATT_CCC_NOTIFY)
    {
        audio_notify_enable = 1;
    }
    else
    {
        audio_notify_enable = 0;
    }
}

/*************************************************************************
*  NAME: atvv_ctl_ccc_changed
*/ 
static void atvv_ctl_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    BT_INFO("ccc:value=[%d]", value);

    if (value == BT_GATT_CCC_NOTIFY)
    {
        ctl_notify_enable = 1;
    }
    else
    {
        ctl_notify_enable = 0;
    }
}


/*************************************************************************
*  DEFINE : attrs 
*/
static struct bt_gatt_attr attrs[]= {
        BT_GATT_PRIMARY_SERVICE(ATVV_SERVICE_UUID),

        BT_GATT_CHARACTERISTIC(ATVV_CHAR_TX,
							BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							BT_GATT_PERM_WRITE,
							NULL,
							atvv_tx,
							NULL),

        BT_GATT_CHARACTERISTIC(ATVV_CHAR_AUDIO,
							BT_GATT_CHRC_NOTIFY,
							NULL,
							NULL,
							NULL,
							NULL),

        BT_GATT_CCC(atvv_audio_ccc_changed,
                                                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

        BT_GATT_CHARACTERISTIC(ATVV_CHAR_CTL,
							BT_GATT_CHRC_NOTIFY,
							NULL,
							NULL,
							NULL,
							NULL),

        BT_GATT_CCC(atvv_ctl_ccc_changed,
                                                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)

};

/*************************************************************************
* NAME: get_attr
*/
struct bt_gatt_attr *get_attr(u8_t index)
{
	return &attrs[index];
}

/*************************************************************************
* NAME: atvv_server
*/
struct bt_gatt_service atvv_server = BT_GATT_SERVICE(attrs);


/*************************************************************************
*  NAME: atvv_init
*/
void atvv_init()
{
    bt_conn_cb_register(&atvv_conn_callbacks);
    bt_gatt_service_register(&atvv_server);
}

