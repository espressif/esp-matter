/*  Bluetooth Mesh */

#include <string.h>
#include <errno.h>

#include "mesh.h"
#include "lpn.h"
#include "crypto.h"
#include "access.h"
#include "foundation.h"
#include "transport.h"
#include "include/main.h"
#include "mesh_settings.h"
#include "include/mesh.h"

#include "log.h"
#define CID_NVAL 0xFFFF

static struct bt_mesh_model *find_model(u16_t elem_addr, u16_t cid, u16_t mod_id)
{
    struct bt_mesh_elem *elem = NULL;

    if (!BT_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Not a unicast address 0x%04x", __func__, elem_addr);
        return NULL;
    }

    elem = bt_mesh_elem_find(elem_addr);
    if (elem == NULL) {
        BT_ERR("%s, No element found, addr 0x%04x", __func__, elem_addr);
        return NULL;
    }

    if (cid == CID_NVAL) {
        return bt_mesh_model_find(elem, mod_id);
    } else {
        return bt_mesh_model_find_vnd(elem, cid, mod_id);
    }

    return NULL;
}

int bt_mesh_model_subscribe_group_addr(u16_t elem_addr, u16_t cid,
                                       u16_t mod_id, u16_t group_addr)
{
    struct bt_mesh_model *model = NULL;
    int i;

    model = find_model(elem_addr, cid, mod_id);
    if (model == NULL) {
        BT_ERR("Subscribe, model not found, cid 0x%04x, mod_id 0x%04x", cid, mod_id);
        return -ENODEV;
    }

    if (!BT_MESH_ADDR_IS_GROUP(group_addr)) {
        BT_ERR("Subscribe, not a group address 0x%04x", group_addr);
        return -EINVAL;
    }

    if (bt_mesh_model_find_group(&model, group_addr)) {
        BT_INFO("Group address 0x%04x already exists", group_addr);
        return 0;
    }

    for (i = 0; i < ARRAY_SIZE(model->groups); i++) {
        if (model->groups[i] == BT_MESH_ADDR_UNASSIGNED) {
            model->groups[i] = group_addr;

            if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
                bt_mesh_store_mod_sub(model);
            }

            if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
                bt_mesh_lpn_group_add(group_addr);
            }

            BT_INFO("Subscribe group address 0x%04x", group_addr);
            return 0;
        }
    }

    BT_ERR("Subscribe, model sub is full!");
    return -ENOMEM;
}

int bt_mesh_model_unsubscribe_group_addr(u16_t elem_addr, u16_t cid,
                                         u16_t mod_id, u16_t group_addr)
{
    struct bt_mesh_model *model = NULL;
    u16_t *match = NULL;

    model = find_model(elem_addr, cid, mod_id);
    if (model == NULL) {
        BT_ERR("Unsubscribe, model not found, cid 0x%04x, mod_id 0x%04x", cid, mod_id);
        return -ENODEV;
    }

    if (!BT_MESH_ADDR_IS_GROUP(group_addr)) {
        BT_ERR("Unsubscribe, not a group address 0x%04x", group_addr);
        return -EINVAL;
    }

    match = bt_mesh_model_find_group(&model, group_addr);
    if (match == NULL) {
        BT_WARN("Group address 0x%04x not exists", group_addr);
        return -EEXIST;
    }

    *match = BT_MESH_ADDR_UNASSIGNED;

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        bt_mesh_store_mod_sub(model);
    }

    if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
        bt_mesh_lpn_group_del(&group_addr, 1);
    }

    BT_INFO("Unsubscribe group address 0x%04x", group_addr);
    return 0;
}

int bt_mesh_local_model_bind(u16_t net_idx, u16_t mod_app_idx)
{
	int i, j, err = 0;
	const struct bt_mesh_comp* dev_comp = bt_mesh_comp_get();
	u16_t addr = bt_mesh_primary_addr();
	u8_t status;
	
	for (i = 0; i < dev_comp->elem_count; i++) {
		struct bt_mesh_elem *elem = &dev_comp->elem[i];

		for (j = 0; j < elem->model_count; j++) {
			struct bt_mesh_model *model = &elem->models[j];
			
			if(model->id == BT_MESH_MODEL_ID_CFG_CLI
				|| model->id == BT_MESH_MODEL_ID_CFG_SRV){
				continue;
			}
			err = bt_mesh_cfg_mod_app_bind(net_idx, addr, addr+i,
								mod_app_idx, model->id, &status);
		}

		for (j = 0; j < elem->vnd_model_count; j++) {
			struct bt_mesh_model *model = &elem->vnd_models[j];

			err = bt_mesh_cfg_mod_app_bind_vnd(net_idx, addr,
						   addr+i, mod_app_idx,
						   model->vnd.id, model->vnd.company, &status);
		}
	}
	return err;
}

extern u8_t local_mod_bind(struct bt_mesh_model *model, u16_t key_idx);
int bt_mesh_local_model_bind_direct(u16_t net_idx, u16_t mod_app_idx)
{
	int i, j, err = 0;
	const struct bt_mesh_comp* dev_comp = bt_mesh_comp_get();
	u16_t addr = bt_mesh_primary_addr();
	u8_t status;

	for (i = 0; i < dev_comp->elem_count; i++) {
		struct bt_mesh_elem *elem = &dev_comp->elem[i];

		for (j = 0; j < elem->model_count; j++) {
			struct bt_mesh_model *model = &elem->models[j];

			if(model->id == BT_MESH_MODEL_ID_CFG_CLI
				|| model->id == BT_MESH_MODEL_ID_CFG_SRV){
				continue;
			}
			err = local_mod_bind(model, mod_app_idx);
			if(err == STATUS_SUCCESS){
				BT_WARN("model bind status[%d]", err);
			}
		}

		for (j = 0; j < elem->vnd_model_count; j++) {
			struct bt_mesh_model *model = &elem->vnd_models[j];
			err = local_mod_bind(model, mod_app_idx);
			if(err == STATUS_SUCCESS){
				BT_WARN("vendor bind status[%d]", err);
			}
		}
	}
	return err;
}
