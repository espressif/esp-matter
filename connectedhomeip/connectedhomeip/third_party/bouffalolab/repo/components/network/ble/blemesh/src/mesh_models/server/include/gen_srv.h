#ifndef  _GENERIC_SERVER_H_
#define  _GENERIC_SERVER_H_

#include "common_srv.h"
#include "include/access.h"

struct bt_mesh_gen_onoff_state{
    u8_t prest_onoff;
    u8_t target_onoff;
};

/*Mesh Onoff Server Model Context */
struct bt_mesh_gen_onoff_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_gen_onoff_state onoff_state;
    struct bt_mesh_state_transition transition;
    struct bt_mesh_last_rcvd_msg last_msg;
};

#define GEN_ONOFF_STATUS_PAYLOAD_SIZE  3

extern const struct bt_mesh_model_op bt_mesh_onoff_srv_op[];

#define BT_MESH_MODEL_GEN_ONOFF(srv_data)                                      \
		BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV,                      \
			      bt_mesh_onoff_srv_op, NULL, srv_data)

typedef void (*bt_mesh_model_gen_cb)(uint8_t value);
void mesh_gen_srv_callback_register(bt_mesh_model_gen_cb cb);
#endif
