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
#if defined(CFG_BLE_ENABLE)
#include <FreeRTOS.h>
#include <task.h>

#include "bluetooth.h"
#include "ble_cli_cmds.h"
#if defined(CONFIG_BT_MESH)
#include "mesh_cli_cmds.h"
#if defined(CONFIG_BT_MESH_MODEL)
#if (defined(CONFIG_BT_MESH_MODEL_GEN_SRV) || defined(CONFIG_BT_MESH_MODEL_GEN_CLI))
#include "bfl_ble_mesh_generic_model_api.h"
#include "hal_gpio.h"
#endif
#if (defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV) || defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI))
#include "bfl_ble_mesh_lighting_model_api.h"
#endif
#include "bfl_ble_mesh_local_data_operation_api.h"
#include "bfl_ble_mesh_networking_api.h"
#else
#if (defined(CONFIG_BT_MESH_MODEL_GEN_SRV) || defined(CONFIG_BT_MESH_MODEL_GEN_CLI))
#include "gen_srv.h"
#include "hal_gpio.h"
#endif
#endif /* CONFIG_BT_MESH_MODEL */

#endif
#include "hci_driver.h"
#include "hci_core.h"
#include "ble_lib_api.h"
#include "log.h"

#if defined(CONFIG_BLE_TP_SERVER)
#include "ble_tp_svc.h"
#endif

#if defined(CONFIG_BT_OAD_SERVER)
#include "oad_main.h"
#include "oad_service.h"
#endif

#if defined(CONFIG_BT_OAD_CLIENT)
#include "oad_client.h"

#endif

#if defined(CONFIG_HOGP_SERVER)
#include "hog.h"
#endif

#if defined(CONFIG_BT_BAS_SERVER)
#include "bas.h"
#endif

#if defined(CONFIG_BT_SCPS_SERVER)
#include "scps.h"
#endif

#if defined(CONFIG_BT_DIS_SERVER)
#include "dis.h"
#endif

#if defined(CONFIG_BT_MESH)
#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
//#define LED_PIN         (21)
#define LED_PIN         (5)
#define LED_PIN_PULLUP  (0)
#define LED_PIN_PULDONW (0)

void model_gen_cb(uint8_t value)
{
    //bl_gpio_output_set(LED_PIN, value); 
    if(value)
        hal_gpio_led_on();
    else
        hal_gpio_led_off();
}


#if defined(CONFIG_BT_MESH_MODEL)
static void example_handle_gen_onoff_msg(bfl_ble_mesh_model_t *model,
										 bfl_ble_mesh_msg_ctx_t *ctx,
										 bfl_ble_mesh_server_recv_gen_onoff_set_t *set)
{
	bfl_ble_mesh_gen_onoff_srv_t *srv = model->user_data;

	switch (ctx->recv_op) {
	case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_GET:
		bfl_ble_mesh_server_model_send_msg(model, ctx,
			BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS, sizeof(srv->state.onoff), &srv->state.onoff);
		break;
	case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
	case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK:
		if (set->op_en == false) {
			srv->state.onoff = set->onoff;
		} else {
			/* TODO: Delay and state transition */
			srv->state.onoff = set->onoff;
		}
        model_gen_cb(set->onoff);
		if (ctx->recv_op == BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
			bfl_ble_mesh_server_model_send_msg(model, ctx,
				BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS, sizeof(srv->state.onoff), &srv->state.onoff);
		}
		bfl_ble_mesh_model_publish(model, BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS,
			sizeof(srv->state.onoff), &srv->state.onoff, ROLE_NODE);
		break;
	default:
		break;
	}
}


static void example_ble_mesh_generic_server_cb(bfl_ble_mesh_generic_server_cb_event_t event,
                                               bfl_ble_mesh_generic_server_cb_param_t *param)
{
    bfl_ble_mesh_gen_onoff_srv_t *srv;
    printf("event 0x%02x, opcode 0x%04x, src 0x%04x, dst 0x%04x\n",
        event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);

    switch (event) {
    case BFL_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:
        printf("BFL_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT\n");
        if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
            printf("onoff 0x%02x\n", param->value.state_change.onoff_set.onoff);
            model_gen_cb(param->value.state_change.onoff_set.onoff);
        }
        break;
    case BFL_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT:
        printf("BFL_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT\n");
        if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_GET) {
            srv = param->model->user_data;
            printf("onoff 0x%02x\n", srv->state.onoff);
            example_handle_gen_onoff_msg(param->model, &param->ctx, NULL);
        }
        break;
    case BFL_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT:
        printf("BFL_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT\n");
        if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
            printf("onoff 0x%02x, tid 0x%02x\n", param->value.set.onoff.onoff, param->value.set.onoff.tid);
            if (param->value.set.onoff.op_en) {
                printf("trans_time 0x%02x, delay 0x%02x\n",
                    param->value.set.onoff.trans_time, param->value.set.onoff.delay);
            }
            example_handle_gen_onoff_msg(param->model, &param->ctx, &param->value.set.onoff);
        }
        break;
    default:
        printf( "Unknown Generic Server event 0x%02x\n", event);
        break;
    }
}
#endif/*CONFIG_BT_MESH_MODEL_GEN_SRV*/

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
static void example_handle_light_lgn_msg(bfl_ble_mesh_model_t *model,
										 bfl_ble_mesh_msg_ctx_t *ctx,
										 bfl_ble_mesh_server_recv_light_lightness_set_t *set)
{
	bfl_ble_mesh_light_lightness_srv_t *srv = model->user_data;

	switch (ctx->recv_op) {
	case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
		bfl_ble_mesh_server_model_send_msg(model, ctx,
			BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS, sizeof(srv->state->lightness_actual), (uint8_t*)&srv->state->lightness_actual);
		break;
	case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
	case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK:
		if (set->op_en == false) {
			srv->state->lightness_actual = set->lightness;
		} else {
			/* TODO: Delay and state transition */
			srv->state->lightness_actual = set->lightness;
		}
		if (ctx->recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET) {
			bfl_ble_mesh_server_model_send_msg(model, ctx,
				BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS, sizeof(srv->state->lightness_actual), (uint8_t*)&srv->state->lightness_actual);
		}
		bfl_ble_mesh_model_publish(model, BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS,
			sizeof(srv->state->lightness_actual), (uint8_t*)&srv->state->lightness_actual, ROLE_NODE);
		break;
	default:
		break;
	}
}


static void example_ble_mesh_lighting_server_cb(bfl_ble_mesh_lighting_server_cb_event_t event,
			bfl_ble_mesh_lighting_server_cb_param_t *param)
{
    printf("event 0x%02x, opcode 0x%04x, src 0x%04x, dst 0x%04x\n",
        event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);

    switch (event) {
    case BFL_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT:
        printf("BFL_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT\n");
        if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK) {
            printf("Light lightness [%x]\n", param->value.state_change.lightness_set.lightness);
        }
		else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK) {
            printf("Light ctl ln[%x]tp[%x]uv[%x]\n", 
				param->value.state_change.ctl_set.lightness,
				param->value.state_change.ctl_set.temperature,
				param->value.state_change.ctl_set.delta_uv);
        }
		else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET ||
	        param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK) {
	        printf("Light hsl l[%x]h[%x]s[%x]\n", 
				param->value.state_change.hsl_set.lightness,
				param->value.state_change.hsl_set.hue,
				param->value.state_change.hsl_set.saturation);
        }
        break;
    case BFL_BLE_MESH_LIGHTING_SERVER_RECV_GET_MSG_EVT:
        printf("BFL_BLE_MESH_LIGHTING_SERVER_RECV_GET_MSG_EVT\n");
        if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET) {
            bfl_ble_mesh_light_lightness_srv_t *srv = param->model->user_data;
            printf("onoff 0x%02x\n", srv->state->lightness_actual);
            example_handle_light_lgn_msg(param->model, &param->ctx, NULL);
        }
		else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_GET) {
            bfl_ble_mesh_light_ctl_srv_t *srv = param->model->user_data;
            printf("Light ctl ln[%x]ln_t[%x] tp[%x]tp_t[%x] uv[%x]uv_t[%x]\n", 
            		srv->state->lightness, srv->state->target_lightness,
            		srv->state->temperature, srv->state->target_temperature,
            		srv->state->delta_uv, srv->state->target_delta_uv);
            //example_handle_gen_onoff_msg(param->model, &param->ctx, NULL);
        }
		else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_GET) {
            bfl_ble_mesh_light_hsl_srv_t *srv = param->model->user_data;
            printf("Light ctl l[%x]l_t[%x] h[%x]h_t[%x] s[%x]s_t[%x]\n", 
            		srv->state->lightness, srv->state->target_lightness,
            		srv->state->hue, srv->state->target_hue,
            		srv->state->saturation, srv->state->target_saturation);
            //example_handle_gen_onoff_msg(param->model, &param->ctx, NULL);
        }
        break;
    case BFL_BLE_MESH_LIGHTING_SERVER_RECV_SET_MSG_EVT:
        printf("BFL_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT\n");
        if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK) {
            printf("Light lightness [%x], tid[%x]\n", param->value.set.lightness.lightness, param->value.set.lightness.tid);
            if (param->value.set.lightness.op_en) {
                printf("trans_time [%x], delay [%x]\n",
                    param->value.set.lightness.trans_time, param->value.set.lightness.delay);
            }
            example_handle_light_lgn_msg(param->model, &param->ctx, &param->value.set.lightness);
        }
		else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK) {
            printf("Light ctl ln[%x] tp[%d] uv[%x] tid[%x]\n", 
				param->value.set.ctl.lightness,
				param->value.set.ctl.temperature,
				param->value.set.ctl.delta_uv,
				param->value.set.ctl.tid);
            if (param->value.set.ctl.op_en) {
                printf("trans_time [%x], delay [%x]\n",
                    param->value.set.ctl.trans_time, param->value.set.ctl.delay);
            }
            //example_handle_gen_onoff_msg(param->model, &param->ctx, &param->value.set.onoff);
        }
		else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK) {
            printf("Light hsl l[%x] h[%d] s[%x] tid[%x]\n", 
				param->value.set.hsl.lightness,
				param->value.set.hsl.hue,
				param->value.set.hsl.saturation,
				param->value.set.hsl.tid);
            if (param->value.set.hsl.op_en) {
                printf("trans_time 0x%02x, delay 0x%02x\n",
                    param->value.set.hsl.trans_time, param->value.set.hsl.delay);
            }
            //example_handle_gen_onoff_msg(param->model, &param->ctx, &param->value.set.onoff);
        }
        break;
    default:
        printf( "Unknown Server event opcode[%x] 0x%02x", param->ctx.recv_op, event);
        break;
    }
}
#endif /*CONFIG_BT_MESH_MODEL_LIGHT_SRV*/
#endif /* CONFIG_BT_MESH_MODEL */
#endif /*CONFIG_BT_MESH*/

#if defined(CONFIG_BT_OAD_SERVER)
bool app_check_oad(u32_t cur_file_ver, u32_t new_file_ver)
{
    //App layer decides whether to do oad according to file version
    /*if(new_file_ver > cur_file_ver)
        return true;
    else
        return false;*/
    return true;
}
#endif

void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \n",
            bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);

#ifdef CONFIG_BT_STACK_CLI 
        ble_cli_register();
#if defined(CONFIG_BT_STACK_PTS)
        pts_cli_register();
#endif
#endif /* CONFIG_BT_STACK_CLI */

#if defined(CONFIG_BT_MESH)
        blemesh_cli_register();
#if defined(CONFIG_BT_MESH_MODEL)
#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
        //bl_gpio_enable_output(LED_PIN, LED_PIN_PULLUP, LED_PIN_PULDONW);	
		bfl_ble_mesh_register_generic_server_callback(example_ble_mesh_generic_server_cb);
#endif
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
		bfl_ble_mesh_register_lighting_server_callback(example_ble_mesh_lighting_server_cb);
#endif
#else
		//bl_gpio_enable_output(LED_PIN, LED_PIN_PULLUP, LED_PIN_PULDONW);
		mesh_gen_srv_callback_register(model_gen_cb);
#endif /* CONFIG_BT_MESH_MODEL */
#endif

#if defined(CONFIG_BLE_TP_SERVER)
        ble_tp_init();
#endif
#if defined(CONFIG_BT_OAD_SERVER)
        oad_service_enable(app_check_oad);
#endif

#if defined(CONFIG_BT_OAD_CLIENT)
        oad_cli_register();
#endif

#if defined(CONFIG_BT_BAS_SERVER)
        bas_init();
#endif

#if defined(CONFIG_BT_DIS_SERVER)
        dis_init(USB_IMPL_VID, AR_VENDOR_ID, AR_PRODUCT_ID, 0);
#endif

#if defined(CONFIG_BT_SCPS_SERVER)
        scps_init(BT_GAP_SCAN_FAST_INTERVAL, BT_GAP_SCAN_FAST_WINDOW);
#endif

#if defined(CONFIG_HOGP_SERVER)
        hog_init();
#endif

    }
}

void ble_stack_start(void)
{
     // Initialize BLE controller
    ble_controller_init(configMAX_PRIORITIES - 1);
    // Initialize BLE Host stack
    hci_driver_init();
    bt_enable(bt_enable_cb);
}
#endif
