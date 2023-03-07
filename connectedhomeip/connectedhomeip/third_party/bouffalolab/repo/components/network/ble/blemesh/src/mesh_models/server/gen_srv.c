#include <string.h>
#include <net/buf.h>
#include "mesh_model_opcode.h"
#include "gen_srv.h"
#include "log.h"

bt_mesh_model_gen_cb gen_cb;

static void send_onoff_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx)
{
    struct bt_mesh_gen_onoff_srv *srv = model->user_data;
    NET_BUF_SIMPLE_DEFINE(msg, MESH_GEN_OPCODE_SIZE_2 + GEN_ONOFF_STATUS_PAYLOAD_SIZE
                          + MESH_TRANS_MIC_SIZE_4);
    
    bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS);
    net_buf_simple_add_u8(&msg, srv->onoff_state.prest_onoff);
    /*Ignore transition delay currently, will do this later*/
    //net_buf_simple_add_u8(&msg, srv->onoff_state.target_onoff);
    //net_buf_simple_add_u8(&msg, srv->transition.trans_time);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
		BT_ERR("%s, Unable to send onoff status",  __func__);
	}
}

static void gen_onoff_get(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
    struct bt_mesh_gen_onoff_srv *srv = model->user_data;

    if(srv == NULL){
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    send_onoff_status(model, ctx);
}

static void gen_onoff_set(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
    struct bt_mesh_gen_onoff_srv *srv = model->user_data;
    u8_t onoff, tid;// trans_time, delay;
    int64_t recv_time;

    if(srv == NULL){
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    onoff = net_buf_simple_pull_u8(buf);
    
    if(onoff > MESH_STATE_ON){
        BT_ERR("%s, Invalid OnOff value 0x%02x", __func__, onoff);
        return;
    }
    
    tid = net_buf_simple_pull_u8(buf);

    if(bt_mesh_srv_check_rcvd_msg_with_last(tid, ctx->addr, ctx->recv_dst, &recv_time, &srv->last_msg)){
        return;
    }

    /*Ignore transition delay currently, will do this later*/
    if(onoff != srv->onoff_state.prest_onoff)
    {
        srv->onoff_state.prest_onoff = onoff;
        if(gen_cb)
            (gen_cb)(onoff);
    }

    bt_mesh_srv_update_last_rcvd_msg(&srv->last_msg, tid, ctx->addr,ctx->recv_dst, recv_time);
}

static void gen_onoff_set_with_ack(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
    gen_onoff_set(model, ctx, buf);
    send_onoff_status(model, ctx);
}

static void gen_onoff_set_unack(struct bt_mesh_model *model,
				struct bt_mesh_msg_ctx *ctx,
				struct net_buf_simple *buf)
{
    gen_onoff_set(model, ctx, buf);
}

void mesh_gen_srv_callback_register(bt_mesh_model_gen_cb cb)
{
    gen_cb = cb;   
}

const struct bt_mesh_model_op bt_mesh_onoff_srv_op[] = {
	{BLE_MESH_MODEL_OP_GEN_ONOFF_GET, 0, gen_onoff_get},
	{BLE_MESH_MODEL_OP_GEN_ONOFF_SET, 2, gen_onoff_set_with_ack},
	{BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK, 2, gen_onoff_set_unack },
	BT_MESH_MODEL_OP_END,
};

