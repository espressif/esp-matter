/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __PROV_H__
#define __PROV_H__

/* Add by bouffalo */
#include <conn.h>
#include <types.h>

#define PROV_ERR_NONE          0x00
#define PROV_ERR_NVAL_PDU      0x01
#define PROV_ERR_NVAL_FMT      0x02
#define PROV_ERR_UNEXP_PDU     0x03
#define PROV_ERR_CFM_FAILED    0x04
#define PROV_ERR_RESOURCES     0x05
#define PROV_ERR_DECRYPT       0x06
#define PROV_ERR_UNEXP_ERR     0x07
#define PROV_ERR_ADDR          0x08

/* Add by bouffalo */
typedef struct{
	u8_t num_ele;//Number of Elements.
	u16_t algo;//Algorithms.
	u8_t pub_key_type;//Public Key Type
	u8_t stc_oob_type;//Static OOB Type.
	u8_t op_oob_size;//Output OOB Size.
	u16_t op_oob_action;//Output OOB Action
	u8_t ip_oob_size;//Input OOB Size
	u16_t ip_oob_action;//Input OOB Action
}prov_caps_t;/*Provisioning Capabilities*/

/* Add by bouffalo */
typedef struct{
	u8_t algo;//Algorithm
	u8_t pub_key;//Public Key
	u8_t auth_method;//Authentication Method;
	u8_t auth_action;//Authentication Action;
	u8_t auth_size;//Authentication Size;
	u8_t pub_key_data[64];//Public Key data
}prov_start_t;/*Provisioning Start*/


int bt_mesh_pb_adv_open(const u8_t uuid[16], u16_t net_idx, u16_t addr,
			u8_t attention_duration);

void bt_mesh_pb_adv_recv(struct net_buf_simple *buf);

bool bt_prov_active(void);

int bt_mesh_pb_gatt_open(struct bt_conn *conn);
int bt_mesh_pb_gatt_close(struct bt_conn *conn);
int bt_mesh_pb_gatt_recv(struct bt_conn *conn, struct net_buf_simple *buf);

const struct bt_mesh_prov *bt_mesh_prov_get(void);

int bt_mesh_prov_init(const struct bt_mesh_prov *prov);

void bt_mesh_prov_complete(u16_t net_idx, u16_t addr);
void bt_mesh_prov_node_added(u16_t net_idx, u16_t addr, u8_t num_elem);
void bt_mesh_prov_reset(void);
#ifdef CONFIG_BT_MESH_MOD_BIND_CB
void bt_mesh_prov_mod_bind_cb(struct bt_mesh_model *model, u16_t net_idx, u16_t mod_app_idx);
#endif /* CONFIG_BT_MESH_MOD_BIND_CB */

#ifdef CONFIG_BT_MESH_APPKEY_ADD_CB
void bt_mesh_prov_app_key_add_cb(u16_t net_idx, u16_t mod_app_idx);
#endif /* CONFIG_BT_MESH_APPKEY_ADD_CB */

#ifdef CONFIG_BT_MESH_MOD_SUB_ADD_CB
void bt_mesh_prov_mod_sub_add_cb(struct bt_mesh_model *model, 
            u16_t elem_addr, u16_t group_addr);
#endif /* CONFIG_BT_MESH_MOD_SUB_ADD_CB */

#endif /*__PROV_H__*/
