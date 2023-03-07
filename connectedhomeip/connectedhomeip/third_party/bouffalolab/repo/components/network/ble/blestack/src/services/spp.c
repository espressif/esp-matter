#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#include "gatt.h"
#include "hci_core.h"
#include "uuid.h"
#include "spp.h"

uint16_t svc1_index = 0;
uint16_t svc2_index = 0;


static uint8_t short_val_1[1] = {0x30};
static uint8_t short_val_2[1] = {0x30};
static uint8_t short_val_3[512] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
static uint8_t short_val_4[512] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
static uint8_t short_val_5[1] = {0x30};
static uint8_t short_val_6[1] = {0x01};
static uint8_t short_val_7[1] = {0x30};
static uint8_t short_val_8[1] = {0x30};
static uint8_t short_val_9[512] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
static uint8_t short_val_10[1] = {0x30};
static uint8_t short_val_11[1] = {0x30};
static uint8_t short_val_12[1] = {0x30};
static uint8_t short_val_13[1] = {0x30};

static uint8_t short_val_14[1] = {0x30};
static uint8_t short_val_15[1] = {0x30};
static uint8_t short_val_16[1] = {0x30};
static uint8_t short_val_17[1] = {0x30};


static int spp_read_data(struct bt_conn *conn,	const struct bt_gatt_attr *attr,
                void *buf, u16_t len, u16_t offset)
{
    struct attr_val *attrval = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attrval->data,attrval->len);
}

static int spp_write_data(struct bt_conn *conn,	const struct bt_gatt_attr *attr,
                const void *buf, u16_t len, u16_t offset,u8_t flags)
{
    struct attr_val *attrval = attr->user_data;

    if (offset + len > attrval->len) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    memcpy(attrval->data + offset, buf, len);
    return len;
}

/** 
 *@Dynamic spp service attribution list
 */
static struct bt_gatt_attr svc1_attr[] = {

    BT_GATT_ATTRIBUTE(SPP_UUID_0,
            BT_GATT_PERM_READ,
                spp_read_data,
                    0,
                        (void*)val_convert(short_val_1,sizeof(short_val_1))),

    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CUD,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                spp_read_data,
                    spp_write_data,
                        (void*)val_convert(short_val_2,sizeof(short_val_2))),

    BT_GATT_ATTRIBUTE(SPP_UUID_1,
            BT_GATT_PERM_READ,
                spp_read_data,
                    0,
                    (void*)val_convert(short_val_3,sizeof(short_val_3))),

    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CUD,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                spp_read_data,
                    spp_write_data,
                        (void*)val_convert(short_val_4,sizeof(short_val_4))),

    BT_GATT_ATTRIBUTE(SPP_UUID_2,
            BT_GATT_PERM_READ,
                0,
                spp_write_data,
                    (void*)val_convert(short_val_5,sizeof(short_val_5))),

    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CUD,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                spp_read_data,
                    spp_write_data,
                        (void*)val_convert(short_val_6,sizeof(short_val_6))),

    BT_GATT_ATTRIBUTE(SPP_UUID_3,
            BT_GATT_PERM_WRITE,
                0,
                spp_write_data,
                    (void*)val_convert(short_val_7,sizeof(short_val_7))),

    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CUD,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                spp_read_data,
                    spp_write_data,
                        (void*)val_convert(short_val_8,sizeof(short_val_8))),

    BT_GATT_ATTRIBUTE(SPP_UUID_4,
            BT_GATT_PERM_WRITE,
                0,
                spp_write_data,
                    (void*)val_convert(short_val_9,sizeof(short_val_9))),

    BT_GATT_ATTRIBUTE(SPP_UUID_5,
            0,
                0,
                    spp_write_data,
                        (void*)val_convert(short_val_10,sizeof(short_val_10))),

    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CCC,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                0,
                    0,
                        0),

     BT_GATT_ATTRIBUTE(SPP_UUID_6,
            0,
                0,
                    spp_write_data,
                        (void*)val_convert(short_val_11,sizeof(short_val_11))),

    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CCC,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                0,
                    0,
                        0),

    BT_GATT_ATTRIBUTE(SPP_UUID_7,
            BT_GATT_PERM_READ,
                spp_read_data,
                    0,
                        (void*)val_convert(short_val_12,sizeof(short_val_12))),

    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CUD,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                spp_read_data,
                    spp_write_data,
                        (void*)val_convert(short_val_13,sizeof(short_val_13))),
};

static struct bt_gatt_attr svc2_attr[] = {
    BT_GATT_ATTRIBUTE(SPP_UUID_8,
            BT_GATT_PERM_READ,
                spp_read_data,
                    0,
                        (void*)val_convert(short_val_14,sizeof(short_val_14))),

    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CUD,
            (BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
                spp_read_data,
                    spp_write_data,
                        (void*)val_convert(short_val_15,sizeof(short_val_15))),

   BT_GATT_ATTRIBUTE(SPP_UUID_9,
            BT_GATT_PERM_READ,
                spp_read_data,
                    0,
                        (void*)val_convert(short_val_16,sizeof(short_val_16))),

    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CUD,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                spp_read_data,
                    spp_write_data,
                        (void*)val_convert(short_val_17,sizeof(short_val_17))),
};


void bt_dyn_register_spp_srv(void)
{

    struct bt_gatt_attr *attr = svc1_attr;

    bt_gatts_add_serv_attr(spp_svc_1_uuid,1,24);
    bt_gatts_add_char(attr,BT_GATT_CHRC_READ);
    bt_gatts_add_desc(++attr);
    bt_gatts_add_char(++attr,BT_GATT_CHRC_READ);
    bt_gatts_add_desc(++attr);
    bt_gatts_add_char(++attr,BT_GATT_CHRC_WRITE);
    bt_gatts_add_desc(++attr);
    bt_gatts_add_char(++attr,BT_GATT_CHRC_WRITE_WITHOUT_RESP);
    bt_gatts_add_desc(++attr);
    bt_gatts_add_char(++attr,BT_GATT_CHRC_WRITE);

    bt_gatts_add_char(++attr,BT_GATT_CHRC_NOTIFY);
    bt_gatts_add_desc(++attr);
    
    bt_gatts_add_char(++attr,BT_GATT_CHRC_INDICATE);
    bt_gatts_add_desc(++attr);

    bt_gatts_add_char(++attr,BT_GATT_CHRC_READ);
    bt_gatts_add_desc(++attr);

    svc1_index = bt_gatts_add_service();

    attr = svc2_attr;
    bt_gatts_add_serv_attr(spp_svc_2_uuid,1,7);
    bt_gatts_add_char(attr,BT_GATT_CHRC_READ);
    bt_gatts_add_desc(++attr);
    bt_gatts_add_char(++attr,BT_GATT_CHRC_READ);
    bt_gatts_add_desc(++attr);
    svc2_index = bt_gatts_add_service();
}


void bt_dyn_unregister_spp_srv(void)
{
    bt_gatts_del_service(svc1_index);
    bt_gatts_del_service(svc2_index);
}

