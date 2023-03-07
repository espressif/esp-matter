/******************************************************************************

@file  erpc_wrapper.c

@brief This file contains some wrapper function which simplify access to
       the BLE Host API through eRPC.

Group: WCS, BTS
Target Device: cc13xx_cc26xx

******************************************************************************

 Copyright (c) 2019-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************


*****************************************************************************/

/*********************************************************************
* INCLUDES
*/
#include <string.h>
#include <stdlib.h>

#include <icall.h>
#include "mesh_erpc_wrapper.h"

#include <zephyr.h>
#include <sys/printk.h>
#include <settings/settings.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>

#include "ti_ble_mesh_prov_data.h"
#include "ti_device_composition.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define UUID_MAX_LEN 16

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
#ifdef MESH_ERPC
struct bt_mesh_comp *comp_data;
struct bt_mesh_prov *prov_data;
#else
struct bt_mesh_comp *comp_data = &comp;
struct bt_mesh_prov *prov_data = &prov;
#endif

#ifdef MESH_ERPC
/* Register Zephyr's Callbacks */
BT_MESH_HB_CB_DEFINE(eRPC_hb_cb) = {
  .recv    = hb_recv_cb,
  .sub_end = hb_sub_end_cb,
};

#ifdef CONFIG_BT_MESH_FRIEND
BT_MESH_FRIEND_CB_DEFINE(eRPC_friend_cb) = {
  .established = friend_friendship_established_cb,
  .terminated  = friend_friendship_terminated_cb,
};
#endif /* CONFIG_BT_MESH_FRIEND */

#ifdef CONFIG_BT_MESH_LOW_POWER
BT_MESH_LPN_CB_DEFINE(eRPC_lpn_cb) = {
  .established = lpn_friendship_established_cb,
  .terminated  = lpn_friendship_terminated_cb,
  .polled      = lpn_polled_cb,
};
#endif /* CONFIG_BT_MESH_LOW_POWER */

BT_MESH_APP_KEY_CB_DEFINE(appkey_evt_cb);
#endif /* MESH_ERPC */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
struct bt_mesh_model * get_model_data(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
model_info_t get_model_info(struct bt_mesh_model *model);

// Callbacks wrappers
int output_number_callback(bt_mesh_output_action_t act, uint32_t num);
int output_string_callback(const char *str);
void unprovisioned_beacon_callback(uint8_t uuid[16],
                    bt_mesh_prov_oob_info_t oob_info,
                    uint32_t *uri_hash);
void link_open_callback(bt_mesh_prov_bearer_t bearer);
void link_close_callback(bt_mesh_prov_bearer_t bearer);
int input_callback(bt_mesh_input_action_t act, uint8_t size);
void func_callback(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct net_buf_simple *buf);
int update_callback(struct bt_mesh_model *model);
int init_callback(struct bt_mesh_model *model);
void reset_callback(struct bt_mesh_model *model);
int start_callback(struct bt_mesh_model *model);
int settings_set_callback(struct bt_mesh_model *model, const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg);
int fault_get_cur_callback(struct bt_mesh_model *model, uint8_t *test_id,
                           uint16_t *company_id, uint8_t *faults,
                           uint8_t *fault_count);
int fault_get_reg_callback(struct bt_mesh_model *model, uint16_t company_id,
                           uint8_t *test_id, uint8_t *faults,
                           uint8_t *fault_count);
int fault_clear_callback(struct bt_mesh_model *model, uint16_t company_id);
int fault_test_callback(struct bt_mesh_model *model, uint8_t test_id, uint16_t company_id);
void attn_on_callback(struct bt_mesh_model *model);
void attn_off_callback(struct bt_mesh_model *model);

/*********************************************************************
 * LOCAL VARIABLES
 */
/*********************************************************************
 * FUNCTIONS
 */
extern void mesh_erpc_register(void);
/*********************************************************************
 * @fn      mesh_init
 *
 * @brief   Calls the bt_mesh_init API.
 *
 * @param   None.
 *
 * @return  None.
 */
 int mesh_init(void)
 {
     int err = 0;

#ifdef MESH_ERPC
     int i = 0;
     // Register to the ICALL from the eRPC context
     mesh_erpc_register();

     // Return an error if there is an element with no models
     for(i = 0; i < comp_data->elem_count; i++)
     {
         if((comp_data->elem + i)->model_count == 0 && (comp_data->elem + i)->vnd_model_count == 0)
         {
             return -1;
         }
     }
#endif

     err = bt_mesh_init(prov_data, comp_data);
     if (err != 0) {
         return -1;
     }
     return 0;
 }

 /*********************************************************************
 * @fn      bt_mesh_init_prov_raw_init
 *
 * @brief   Config bt_mesh_prov parameters.
 *
 * @param   None.
 *
 * @return  None.
 */
 int bt_mesh_init_prov_raw_init(const struct bt_mesh_prov_raw *prov_raw)
{
    // Allocate the memory for the pointers inside prov_data
    uint8_t *uuid = (uint8_t *)ICall_malloc(sizeof(uint8_t) * UUID_MAX_LEN);
    char *uri = (char *)ICall_malloc(sizeof(char) * (strlen(prov_raw->uri) + 1));
    uint8_t *static_val = NULL;

    // Allocate the prov_data memory
    prov_data = (struct bt_mesh_prov *)ICall_malloc(sizeof(struct bt_mesh_prov));
    if(!prov_data)
    {
        return -1;
    }

    // Reset the uuid content and copy the uuid provided in prov_raw
    memset(uuid, 0, sizeof(uint8_t) * UUID_MAX_LEN);
    memcpy(uuid, prov_raw->uuid, sizeof(uint8_t) * prov_raw->uuid_len);
    // Copy the uri provided in prov_raw
    memcpy(uri, prov_raw->uri, strlen(prov_raw->uri));
    // If prov_raw->static_val is not NULL, allocate the memory for it and copy the data
    if(prov_raw->static_val)
    {
        static_val = (uint8_t *)ICall_malloc(sizeof(uint8_t) * prov_raw->static_val_len);
        memcpy(static_val, prov_raw->static_val, sizeof(uint8_t) * prov_raw->static_val_len);
    }

    struct bt_mesh_prov curr_prov =
    {
     .uuid = uuid,
     .uri = uri,
     .oob_info = (bt_mesh_prov_oob_info_t)prov_raw->oob_info,
     .static_val = static_val,
     .static_val_len = prov_raw->static_val_len,
     .output_size = prov_raw->output_size,
     .output_actions = prov_raw->output_actions,
     .input_size = prov_raw->input_size,
     .input_actions = prov_raw->input_actions,
     .output_number = prov_raw->output_number? output_number_callback : NULL,
     .output_string = prov_raw->output_string? output_string_callback : NULL,
     .input = prov_raw->input? input_callback : NULL,
     .input_complete = prov_raw->input_complete,
     .unprovisioned_beacon = prov_raw->unprovisioned_beacon? unprovisioned_beacon_callback : NULL,
     .link_open = prov_raw->link_open? link_open_callback : NULL,
     .link_close = prov_raw->link_close? link_close_callback : NULL,
     .complete = prov_raw->complete,
     .node_added = prov_raw->node_added,
     .reset = prov_raw->reset,
    };

    // If prov_raw->static_val is NULL, set the length to 0
    if(!prov_raw->static_val)
    {
        curr_prov.static_val_len = 0;
    }

    // Copy the data from curr_prov to prov_data
    memcpy(prov_data, &curr_prov, sizeof(struct bt_mesh_prov));

    return 0;
}

/*********************************************************************
* @fn      bt_mesh_init_comp_raw_init
*
* @brief   Config bt_mesh_comp parameters.
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_init_comp_raw_init(const struct bt_mesh_comp_raw *comp_raw)
{
    // Allocate the comp_data memory
    comp_data = (struct bt_mesh_comp*)ICall_malloc(sizeof(struct bt_mesh_comp));
    if(!comp_data)
    {
      return -1;
    }

    // Copy the data from comp_raw to comp_data
    memcpy(comp_data, comp_raw, sizeof(struct bt_mesh_comp));

    // Allocate the elem memory
    comp_data->elem = (struct bt_mesh_elem*)ICall_malloc(sizeof(struct bt_mesh_elem) * comp_data->elem_count);
    if(!comp_data->elem)
    {
      return -1;
    }

    // Reset the elements data
    memset(comp_data->elem, 0, sizeof(struct bt_mesh_elem) * comp_data->elem_count);

    return 0;
}

/*********************************************************************
* @fn      bt_mesh_init_elem_raw_init
*
* @brief   Config bt_mesh_elem parameters.
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_init_elem_raw_init(uint16_t elem_index, const struct bt_mesh_elem_raw *elem_raw)
{
    struct bt_mesh_model * curr_models = NULL;
    struct bt_mesh_model * curr_vnd_models = NULL;

    // Allocate the element models memory
    if(elem_raw->model_count > 0)
    {
        curr_models = (struct bt_mesh_model *)ICall_malloc(sizeof(struct bt_mesh_model) * (elem_raw->model_count));
        if(!curr_models)
        {
            return -1;
        }
    }

    // Allocate the element vnd models memory
    if(elem_raw->vnd_model_count > 0)
    {
        curr_vnd_models = (struct bt_mesh_model *)ICall_malloc(sizeof(struct bt_mesh_model) * (elem_raw->vnd_model_count));
        if(!curr_vnd_models)
        {
            return -1;
        }
    }

    struct bt_mesh_elem curr_elem =
    {
     .loc               = elem_raw->loc,
     .model_count       = elem_raw->model_count,
     .vnd_model_count   = elem_raw->vnd_model_count,
     .models            = curr_models,
     .vnd_models        = curr_vnd_models
    };

    memcpy((comp_data->elem + elem_index), &curr_elem, sizeof(struct bt_mesh_elem));

    return 0;
}

/*********************************************************************
* @fn      bt_mesh_cfg_cli_raw_init
*
* @brief   Config the bt_mesh_cfg_cli.
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_cli_raw_init(uint16_t elem_index, uint16_t model_index)
{
    // Return an error in case the model_index is >= from the model_count of the element
    if(model_index >= (comp_data->elem + elem_index)->model_count)
    {
        return -1;
    }

    struct bt_mesh_cfg_cli * cfg_cli = (struct bt_mesh_cfg_cli *)ICall_malloc(sizeof(struct bt_mesh_cfg_cli));
    if(!cfg_cli)
    {
      return -1;
    }

    struct bt_mesh_model cfg_cli_model = BT_MESH_MODEL_CFG_CLI(cfg_cli);

    memset(cfg_cli, 0, sizeof(struct bt_mesh_cfg_cli));
    memcpy(((comp_data->elem + elem_index)->models + model_index), &cfg_cli_model, sizeof(struct bt_mesh_model));
    return 0;
}

/*********************************************************************
* @fn      bt_mesh_cfg_srv_raw_init
*
* @brief   Config the bt_mesh_cfg_srv parameters.
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_srv_raw_init(uint16_t model_index)
{
    // Return an error in case the model_index is >= from the model_count of element 0
    if(model_index >= comp_data->elem->model_count)
    {
        return -1;
    }

    struct bt_mesh_model cfg_srv_model = BT_MESH_MODEL_CFG_SRV;

    memcpy(((comp_data->elem)->models + model_index), &cfg_srv_model, sizeof(struct bt_mesh_model));
    return 0;
}

/*********************************************************************
* @fn      bt_mesh_health_srv_raw_init
*
* @brief   Config the bt_mesh_health_srv parameters.
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_health_srv_raw_init(uint16_t elem_index, uint16_t model_index,
                                const struct bt_mesh_health_srv_raw * health_srv, uint8_t max_faults)
{
    // Return an error in case the model_index is >= from the model_count of the elem_index
    if(model_index >= ((comp_data->elem + elem_index)->model_count) )
    {
        return -1;
    }
    struct bt_mesh_health_srv * curr_health_srv = (struct bt_mesh_health_srv *)ICall_malloc(sizeof(struct bt_mesh_health_srv));
    if(!curr_health_srv)
    {
      return -1;
    }
    struct bt_mesh_health_srv_cb * curr_health_srv_cb = (struct bt_mesh_health_srv_cb *)ICall_malloc(sizeof(struct bt_mesh_health_srv_cb));
    if(!curr_health_srv_cb)
    {
      return -1;
    }

    // For each callback in the health_srv->cb struct, check if it's NULL or not.
    // If it's not NULL, assign it's wrapper callback function to the corresponding
    // curr_health_srv_cb callback, else, set it to NULL.
    curr_health_srv_cb->fault_get_cur = health_srv->cb->fault_get_cur? fault_get_cur_callback : NULL;
    curr_health_srv_cb->fault_get_reg = health_srv->cb->fault_get_reg? fault_get_reg_callback : NULL;
    curr_health_srv_cb->fault_clear = health_srv->cb->fault_clear? fault_clear_callback : NULL;
    curr_health_srv_cb->fault_test = health_srv->cb->fault_test? fault_test_callback : NULL;
    curr_health_srv_cb->attn_on = health_srv->cb->attn_on? attn_on_callback : NULL;
    curr_health_srv_cb->attn_off = health_srv->cb->attn_off? attn_off_callback : NULL;

    // Assign the curr_health_srv_cb pointer to the curr_health_srv cb parameter
    curr_health_srv->cb = curr_health_srv_cb;

    // Create the health server's model publication
    BT_MESH_HEALTH_PUB_DEFINE(health_pub, 0);

    // Create the health server model
    struct bt_mesh_model health_srv_model = BT_MESH_MODEL_HEALTH_SRV(curr_health_srv, &health_pub);

    // Copy the health server model to the models list
    memcpy(((comp_data->elem + elem_index)->models + model_index), &health_srv_model, sizeof(struct bt_mesh_model));
    return 0;
}

/*********************************************************************
* @fn      bt_mesh_init_model_raw_init
*
* @brief   Config bt_mesh_model parameters.
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_init_model_raw_init(uint16_t elem_index, uint16_t model_index, const struct bt_mesh_model_raw *model_raw,
                                const struct bt_mesh_model_op_raw *op_raw, uint16_t op_len)
{
    // Return an error in case the model_index is >= from the model_count of the elem_index
    if((model_raw->model_type == VND_MODEL && model_index >= (comp_data->elem + elem_index)->vnd_model_count) ||
            (model_raw->model_type == SIG_MODEL && model_index >= (comp_data->elem + elem_index)->model_count))
    {
        return -1;
    }
    int i;
    // Allocate the memory of the pointers in bt_mesh_model
    struct bt_mesh_model_pub * curr_pub = NULL;
    struct bt_mesh_model_cb * curr_cb   = NULL;
    char * user_data                    = NULL;
    // Allocating space for op_len+1 opcodes, sicne last opcode must be a terminating opcode (all fields are 0).
    struct bt_mesh_model_op * curr_ops  = (struct bt_mesh_model_op *)ICall_malloc(sizeof(struct bt_mesh_model_op) * (op_len + 1));
    if(!curr_ops)
    {
        return -1;
    }

    if(model_raw->pub)
    {
        curr_pub = (struct bt_mesh_model_pub *)ICall_malloc(sizeof(struct bt_mesh_model_pub));
        if(!curr_pub)
        {
            return -1;
        }

        // Copy the bt_mesh_model_pub data
        memcpy(&(curr_pub->addr), model_raw->pub, sizeof(struct bt_mesh_model_pub_raw));
        // replace the eRPC named callback with the real callback
        if(curr_pub->update)
        {
            curr_pub->update = update_callback;
        }
        // Copy the net_buf_simple data
        curr_pub->msg = (struct net_buf_simple *)ICall_malloc(sizeof(struct net_buf_simple));
        if(!curr_pub->msg)
        {
            return -1;
        }
        memcpy(curr_pub->msg, (struct net_buf_simple *)model_raw->pub->msg, sizeof(struct net_buf_simple));
        // Copy the net_buf_simple data
        curr_pub->msg->data = (uint8_t *)ICall_malloc(sizeof(uint8_t)*curr_pub->msg->size);
        if(!curr_pub->msg->data)
        {
            return -1;
        }
        curr_pub->msg->__buf = curr_pub->msg->data;
        memcpy(curr_pub->msg->data, model_raw->pub->msg->data, (sizeof(uint8_t) * curr_pub->msg->len));
    }

    // Copy the bt_mesh_model_op parameters and set the func callback to be the callback wrapper
    for(i = 0; i < op_len; i++)
    {
        struct bt_mesh_model_op temp_op =
        {
         .opcode = (op_raw + i)->opcode,
         .min_len = (op_raw + i)->min_len,
         .func = func_callback
        };

        // Copy the bt_mesh_model_op data
        memcpy((curr_ops + i), &temp_op, (sizeof(struct bt_mesh_model_op)));
    }

    // Terminating ops list.
    // ops list is terminated by an OP which all fields are set to 0
    memset(curr_ops + op_len, 0, sizeof(struct bt_mesh_model_op));

    // In case the bt_mesh_model_cb pointer is not NULL
    // Do malloc and set the callback wrappers
    if(model_raw->cb)
    {
        curr_cb   = (struct bt_mesh_model_cb *)ICall_malloc(sizeof(struct bt_mesh_model_cb));
        if(!curr_cb)
        {
            return -1;
        }
        // Update the bt_mesh_model_cb with the wrappers callbacks
        struct bt_mesh_model_cb cb_wrappers =
        {
         .settings_set = model_raw->cb->settings_set ? settings_set_callback: NULL,
         .start = model_raw->cb->start ? start_callback: NULL,
         .init = model_raw->cb->init ? init_callback: NULL,
         .reset = model_raw->cb->reset ? reset_callback: NULL
        };

        // Copy the bt_mesh_model_cb data
        memcpy(curr_cb, &cb_wrappers, sizeof(struct bt_mesh_model_cb));
    }

    if(model_raw->user_data)
    {
        user_data = (char *)ICall_malloc(sizeof(char) * (strlen(model_raw->user_data) + 1));
        if(!user_data)
        {
            return -1;
        }
        // Copy the usre_data
        memcpy(user_data, model_raw->user_data, strlen(model_raw->user_data));
    }

    struct bt_mesh_model curr_model =
    {
     .pub = curr_pub,
     .id = model_raw->model.id,
     .op = curr_ops,
     .cb = curr_cb,
     .user_data = user_data
    };

    if(model_raw->model_type == VND_MODEL)
    {
        curr_model.vnd.company = model_raw->model.company;
        curr_model.vnd.id = model_raw->model.vnd_id;
    }

    model_raw->model_type == VND_MODEL ?
    memcpy(((comp_data->elem + elem_index)->vnd_models + model_index), &curr_model, sizeof(struct bt_mesh_model)) :
    memcpy(((comp_data->elem + elem_index)->models + model_index), &curr_model, sizeof(struct bt_mesh_model));

    return 0;
}

/*********************************************************************
* @fn      bt_mesh_cfg_app_key_add_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_app_key_add_wrapper(uint16_t net_idx, uint16_t addr, uint16_t key_net_idx,
                uint16_t key_app_idx, const uint8_t app_key[16])
{
    return bt_mesh_cfg_app_key_add(net_idx, addr, key_net_idx, key_app_idx, app_key, NULL);
}

/*********************************************************************
* @fn      bt_mesh_cfg_mod_app_bind_vnd_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_mod_app_bind_vnd_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                 uint16_t mod_app_idx, uint16_t mod_id, uint16_t cid)
{
    return bt_mesh_cfg_mod_app_bind_vnd(net_idx, addr, elem_addr, mod_app_idx, mod_id, cid, NULL);
}

/*********************************************************************
* @fn      bt_mesh_cfg_mod_app_bind_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_mod_app_bind_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                     uint16_t mod_app_idx, uint16_t mod_id)
{
    return bt_mesh_cfg_mod_app_bind(net_idx, addr, elem_addr, mod_app_idx, mod_id, NULL);
}

/*********************************************************************
* @fn      bt_mesh_cfg_mod_sub_add_vnd_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_mod_sub_add_vnd_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                        uint16_t sub_addr, uint16_t mod_id, uint16_t cid)
{
    return bt_mesh_cfg_mod_sub_add_vnd(net_idx, addr, elem_addr, sub_addr, mod_id, cid, NULL);
}

/*********************************************************************
* @fn      bt_mesh_cfg_mod_sub_add_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_mod_sub_add_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                    uint16_t sub_addr, uint16_t mod_id)
{
    return bt_mesh_cfg_mod_sub_add(net_idx, addr, elem_addr, sub_addr, mod_id, NULL);
}

/*********************************************************************
* @fn      bt_mesh_cfg_mod_sub_del_vnd_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_mod_sub_del_vnd_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                        uint16_t sub_addr, uint16_t mod_id, uint16_t cid)
{
    return bt_mesh_cfg_mod_sub_del_vnd(net_idx, addr, elem_addr, sub_addr, mod_id, cid, NULL);
}

/*********************************************************************
* @fn      bt_mesh_cfg_mod_sub_del_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_mod_sub_del_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                    uint16_t sub_addr, uint16_t mod_id)
{
    return bt_mesh_cfg_mod_sub_del(net_idx, addr, elem_addr, sub_addr, mod_id, NULL);
}

/*********************************************************************
* @fn      bt_mesh_cfg_mod_sub_va_add_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_mod_sub_va_add_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr, const uint8_t label[16], uint16_t mod_id)
{
    return bt_mesh_cfg_mod_sub_va_add(net_idx, addr, elem_addr, label, mod_id, NULL, NULL);
}

/*********************************************************************
* @fn      bt_mesh_cfg_mod_sub_va_add_vnd_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_mod_sub_va_add_vnd_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr, const uint8_t label[16], uint16_t mod_id, uint16_t cid)
{
    return bt_mesh_cfg_mod_sub_va_add_vnd(net_idx, addr, elem_addr, label, mod_id, cid, NULL, NULL);
}

/*********************************************************************
* @fn      bt_mesh_cfg_mod_pub_set_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_mod_pub_set_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr, uint16_t mod_id, struct bt_mesh_cfg_mod_pub_raw *pub)
{
    return bt_mesh_cfg_mod_pub_set(net_idx, addr, elem_addr, mod_id, (struct bt_mesh_cfg_mod_pub *)pub, NULL);
}

/*********************************************************************
* @fn      bt_mesh_cfg_mod_pub_set_vnd_wrapper
*
* @brief
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_cfg_mod_pub_set_vnd_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr, uint16_t mod_id, uint16_t cid, struct bt_mesh_cfg_mod_pub_raw *pub)
{
    return bt_mesh_cfg_mod_pub_set_vnd(net_idx, addr, elem_addr, mod_id, cid, (struct bt_mesh_cfg_mod_pub *)pub, NULL);
}

/*********************************************************************
* @fn      bt_mesh_model_publish_wrapper function
*
* @brief   gets the model info and calls the mesh profile
*          bt_mesh_model_publish() API
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_model_publish_wrapper(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index, struct net_buf_simple_raw *msg)
{
    struct bt_mesh_model *model = get_model_data(elem_idx, is_vnd, model_index);
    struct net_buf_simple *pub_msg = model->pub->msg;
    int i;

    // Set the publication msg len to 0 before copying the data
    pub_msg->len = 0;

    // Verify that the msg length is not bigger then the publication size
    if(msg->len > (pub_msg->size))
    {
        return -1;
    }

    // Copy the msg data that was sent from the application
    for(i = 0; i < msg->len; i++)
    {
        net_buf_simple_add_u8(pub_msg, (uint8_t)*(msg->data + i));
    }

    return bt_mesh_model_publish(model);
}

/*********************************************************************
* @fn      bt_mesh_model_send_data_wrapper function
*
* @brief   gets the model info, ctx and msg and calls the mesh profile
*          bt_mesh_model_send() API
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_model_send_data_wrapper(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index,
                                    struct bt_mesh_msg_ctx_raw *ctx, struct net_buf_simple_raw *msg)
{
    int i, err;
    struct bt_mesh_model * model = get_model_data(elem_idx, is_vnd, model_index);
    // Create the buffer
    const uint16_t temp = msg->size;
    NET_BUF_SIMPLE_DEFINE(temp_msg, temp);

    // Copy the msg data that was sent from the application
    for(i = 0; i < msg->len; i++)
    {
        net_buf_simple_add_u8(&temp_msg, (uint8_t)*(msg->data + i));
    }
    err = bt_mesh_model_send(model, (struct bt_mesh_msg_ctx *)ctx, &temp_msg, NULL, NULL);

    return err;

}

/*********************************************************************
* @fn      bt_mesh_model_find_wrapper function
*
* @brief   gets the elem_idx and model id, calls bt_mesh_model_find()
*          API with the elem to search for the model in and the model
*          id
*
* @param   None.
*
* @return  model index.
*/
int bt_mesh_model_find_wrapper(uint16_t elem_idx, uint16_t id)
{
    // Get the model
    struct bt_mesh_model * model = bt_mesh_model_find(&(comp_data->elem[elem_idx]), id);

    // If the model id exist in the given element return the model index
    if(model)
    {
        return model->mod_idx;
    }
    // In case that the model id doesn't exist in the given element return indication
    return -1;
}

/*********************************************************************
* @fn      bt_mesh_model_find_wrapper function
*
* @brief   gets the elem_idx, company id and model id,
*          calls bt_mesh_model_find_vnd() API with the elem to search
*          for the model in, the company id and the model id
*
* @param   None.
*
* @return  model index.
*/
int bt_mesh_model_find_vnd_wrapper(uint16_t elem_idx, uint16_t company, uint16_t id)
{
    // Get the model
    struct bt_mesh_model * model = bt_mesh_model_find_vnd(&(comp_data->elem[elem_idx]), company, id);

    // If the model id exist in the given element return the model index
    if(model)
    {
        return model->mod_idx;
    }
    // In case that the model id doesn't exist in the given element return indication
    return -1;
}

/*********************************************************************
* @fn      store_data_wrapper
*
* @brief   call to bt_mesh_model_data_store API which stores a data
*          item to NV.
*
* @param   elem_idx - the index number of the element
* @param   is_vnd - is it a vendor model or not (generic model)
* @param   model_index - the index number of the model
* @param   name - name/key of the data item to be stored
* @param   data_len - the length of the data to be stored
* @param   data - the data to be stored
*
* @return  0 when data stored successfully, otherwise a nonzero
*          value.
*/
int store_data_wrapper(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index, const char *name, uint8_t data_len, uint8_t *data)
{
    // Get the model
    struct bt_mesh_model *model = get_model_data(elem_idx, is_vnd, model_index);

    return bt_mesh_model_data_store(model, is_vnd, name, (void *)data, data_len);
}

#ifdef CONFIG_BT_MESH_MODEL_EXTENSIONS
/*********************************************************************
* @fn      bt_mesh_model_extend_wrapper function
*
* @brief   gets the mod_elem_idx, mod_is_vnd, mod_idx,base_mod_elem_idx,
*          base_mod_is_vnd and base_mod_idx
*          calls bt_mesh_model_extend() API with the mod and base_mod
*          pointers
*
* @param   None.
*
* @return  model index.
*/
int bt_mesh_model_extend_wrapper(uint16_t mod_elem_idx, uint8_t mod_is_vnd, uint16_t mod_idx,
                                 uint16_t base_mod_elem_idx, uint8_t base_mod_is_vnd, uint16_t base_mod_idx)
{
    // Get the mod and base_mod
    struct bt_mesh_model * mod = get_model_data(mod_elem_idx, mod_is_vnd, mod_idx);
    struct bt_mesh_model * base_mod = get_model_data(base_mod_elem_idx, base_mod_is_vnd, base_mod_idx);

    if (!mod || !base_mod)
    {
        return -1;
    }
    // Call bt_mesh_model_extend with the mod and base_mod pointers
    return bt_mesh_model_extend(mod, base_mod);
}
#endif

/*********************************************************************
* @fn      bt_mesh_fault_update_wrapper function
*
* @brief   gets the element info and calls the mesh profile
*          bt_mesh_fault_update() API
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_fault_update_wrapper(uint16_t elem_idx)
{
    int err;
    err = bt_mesh_fault_update(&(comp_data->elem[elem_idx]));

    return err;
}

/*********************************************************************
* @fn      bt_mesh_prov_enable_wrapper function
*
* @brief   gets the bearers and calls the mesh profile
*          bt_mesh_prov_enable() API
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_prov_enable_wrapper(bt_mesh_prov_bearer bearers)
{
    return bt_mesh_prov_enable((bt_mesh_prov_bearer_t)bearers);
}

/*********************************************************************
* @fn      bt_mesh_prov_disable_wrapper function
*
* @brief   gets the bearers and calls the mesh profile
*          bt_mesh_prov_disable() API
*
* @param   None.
*
* @return  None.
*/
int bt_mesh_prov_disable_wrapper(bt_mesh_prov_bearer bearers)
{
    return bt_mesh_prov_disable((bt_mesh_prov_bearer_t)bearers);
}

/*********************************************************************
* @fn      settings_load_wrapper function
*
* @brief   calls the Zephyr settings module settings_load() API
*
* @param   None.
*
* @return  None.
*/
int settings_load_wrapper(void)
{
    return settings_load();
}

/*********************************************************************
* @fn      get_model_data helper function
*
* @brief   gets a model info and returns its pointer
*
* @param   None.
*
* @return  None.
*/
struct bt_mesh_model * get_model_data(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index)
{
    if(elem_idx < comp_data->elem_count)
    {
        if(is_vnd && model_index < comp_data->elem[elem_idx].vnd_model_count)
        {
            return (comp_data->elem +elem_idx)->vnd_models + model_index;
        }
        if(!is_vnd && model_index < comp_data->elem[elem_idx].model_count)
        {
            return (comp_data->elem +elem_idx)->models + model_index;
        }
    }
    return NULL;
}

/*********************************************************************
* @fn      get_model_info helper function
*
* @brief   gets a pointer to a model and returns its
*          model_info_t details
*
* @param   None.
*
* @return  None.
*/
model_info_t get_model_info(struct bt_mesh_model *model)
{
    model_info_t curr_model_info;
    uint8_t i;
    curr_model_info.elem_index = model->elem_idx;
    for(i = 0; i < comp_data->elem[model->elem_idx].model_count; i++){
        if(&comp_data->elem[model->elem_idx].models[i] == model)
        {
            curr_model_info.model_index = i;
            curr_model_info.is_vnd = 0;
        }
    }
    for(i = 0; i < comp_data->elem[model->elem_idx].vnd_model_count; i++){
        if(&comp_data->elem[model->elem_idx].vnd_models[i] == model)
        {
            curr_model_info.model_index = i;
            curr_model_info.is_vnd = 1;
        }
    }
    return curr_model_info;
}

/*********************************************************************
* @fn      output_number callback wrapper
*
* @brief   Calls the bt_mesh_prov output_number callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
int output_number_callback(bt_mesh_output_action_t act, uint32_t num)
{
    // Call the eRPC named callback
    output_number_cb((bt_mesh_output_action)act, num);
    return 0;
}

/*********************************************************************
* @fn      output_string callback wrapper
*
* @brief   Calls the bt_mesh_prov output_string callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
int output_string_callback(const char *str)
{
    // Call the eRPC named callback
    output_string_cb(str);
    return 0;
}

/*********************************************************************
* @fn      unprovisioned_beacon callback wrapper
*
* @brief   Calls the bt_mesh_prov unprovisioned_beacon callback defined
*          in the application side.
*
* @param   None.
*
* @return  None.
*/
void unprovisioned_beacon_callback(uint8_t uuid[16],
                    bt_mesh_prov_oob_info_t oob_info,
                    uint32_t *uri_hash)
{
    unprovisioned_beacon_cb(uuid, (bt_mesh_prov_oob_info) oob_info, uri_hash);
}

/*********************************************************************
* @fn      link_open callback wrapper
*
* @brief   Calls the bt_mesh_prov link_open callback defined
*          in the application side.
*
* @param   None.
*
* @return  None.
*/
void link_open_callback(bt_mesh_prov_bearer_t bearer)
{
    link_open_cb((bt_mesh_prov_bearer) bearer);
}

/*********************************************************************
* @fn      link_close callback wrapper
*
* @brief   Calls the bt_mesh_prov link_close callback defined
*          in the application side.
*
* @param   None.
*
* @return  None.
*/
void link_close_callback(bt_mesh_prov_bearer_t bearer)
{
    link_close_cb((bt_mesh_prov_bearer) bearer);
}

/*********************************************************************
* @fn      input callback wrapper
*
* @brief   Calls the bt_mesh_prov input callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
int input_callback(bt_mesh_input_action_t act, uint8_t size)
{
    // Call the eRPC named callback
    input_cb((bt_mesh_input_action)act, size);
    return 0;
}

/*********************************************************************
* @fn      func callback wrapper
*
* @brief   Calls the bt_mesh_model_op func callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
void func_callback(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct net_buf_simple *buf)
{
    uint32_t opcode = 0;
    uint8_t op_len;
    model_info_t curr_model_info = get_model_info(model);

    // Get the opcode length.
    // since the opcode is located in the beginning of the buffer
    // and the data starts right after it, check how many bytes the
    // buffer contains before the data starts
    op_len = buf->data - buf->__buf;

    // vnd model contains only 3-octet opcode
    if(curr_model_info.is_vnd && op_len == 3)
    {
        opcode |= (uint8_t)*buf->__buf;
        opcode = opcode << 8 | (uint8_t)*(buf->__buf + 2);
        opcode = opcode << 8 | (uint8_t)*(buf->__buf + 1);
    }
    // SIG model contains only 1-octet or 2-octet opcode
    else if(op_len == 2)
    {
        opcode |= (uint16_t)*buf->__buf << 8 | (uint8_t)*(buf->__buf + 1);
    }
    else
    {
        opcode |= (uint8_t)*buf->__buf;
    }

    // Need to add opcode to the Cbk in eRPC
    func_cb(opcode, curr_model_info.elem_index, curr_model_info.is_vnd, curr_model_info.model_index,
            (struct bt_mesh_msg_ctx_raw *)ctx, (struct net_buf_simple_raw *)buf);
}

/*********************************************************************
* @fn      update callback wrapper
*
* @brief   Calls the bt_mesh_model_pub update callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
int update_callback(struct bt_mesh_model *model)
{
    model_info_t curr_model_info = get_model_info(model);

    // Call the eRPC named callback
    update_cb(curr_model_info.elem_index, curr_model_info.is_vnd, curr_model_info.model_index);

    return 0;
}

/*********************************************************************
* @fn      init callback wrapper
*
* @brief   Calls the bt_mesh_model_cb init callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
int init_callback(struct bt_mesh_model *model)
{
    model_info_t curr_model_info = get_model_info(model);

    // Call the eRPC named callback
    init_cb(curr_model_info.elem_index, curr_model_info.is_vnd, curr_model_info.model_index);

    return 0;
}

/*********************************************************************
* @fn      reset callback wrapper
*
* @brief   Calls the bt_mesh_model_cb reset callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
void reset_callback(struct bt_mesh_model *model)
{
    model_info_t curr_model_info = get_model_info(model);

    // Call the eRPC named callback
    reset_cb(curr_model_info.elem_index, curr_model_info.is_vnd, curr_model_info.model_index);
}

/*********************************************************************
* @fn      start callback wrapper
*
* @brief   Calls the bt_mesh_model_cb start callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
int start_callback(struct bt_mesh_model *model)
{

    model_info_t curr_model_info = get_model_info(model);

    // Call the eRPC named callback
    start_cb(curr_model_info.elem_index, curr_model_info.is_vnd, curr_model_info.model_index);

    return 0;
}

/*********************************************************************
* @fn      settings_set_callback wrapper
*
* @brief   reads data from NV and calls the settings_set_cb
*
* @param   model - model to read the persistent data of
* @param   name - name/key of the data to be read
* @param   len_rd - the size of the data to be read
* @param   read_cb - function provided to read the data from the Mesh
*                    stack.
* @param   cb_arg - arguments for the read function provided by the
*                   Mesh stack.
*
* @return  0 if the read succeeded, otherwise -EINVAL value
*/
int settings_set_callback(struct bt_mesh_model *model, const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    model_info_t curr_model_info = get_model_info(model);

    // Allocate memory for the data pointer
    uint8_t *data = (uint8_t *)ICall_malloc(len_rd);

    // Calling read_cb to get the data saved in NV
    if (read_cb(cb_arg, data, len_rd) != len_rd)
    {
        return -EINVAL;
    }

    // Call the eRPC named callback
    settings_set_cb(curr_model_info.elem_index, curr_model_info.is_vnd, curr_model_info.model_index, name, data, len_rd);

    ICall_free(data);
    return 0;
}

/*********************************************************************
* @fn      fault_get_cur callback wrapper
*
* @brief   Calls the bt_mesh_health_srv_cb fault_get_cur callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
int fault_get_cur_callback(struct bt_mesh_model *model, uint8_t *test_id,
                           uint16_t *company_id, uint8_t *faults,
                           uint8_t *fault_count)
{
    model_info_t curr_model_info = get_model_info(model);

    // Call the eRPC named callback
    return fault_get_cur_cb(curr_model_info.elem_index, curr_model_info.model_index, test_id, company_id, faults, fault_count);
}


/*********************************************************************
* @fn      fault_get_reg callback wrapper
*
* @brief   Calls the bt_mesh_health_srv_cb fault_get_reg callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
int fault_get_reg_callback(struct bt_mesh_model *model, uint16_t company_id,
                           uint8_t *test_id, uint8_t *faults,
                           uint8_t *fault_count)
{

    model_info_t curr_model_info = get_model_info(model);

    // Call the eRPC named callback
    return fault_get_reg_cb(curr_model_info.elem_index, curr_model_info.model_index, company_id, test_id, faults, fault_count);
}


/*********************************************************************
* @fn      fault_clear callback wrapper
*
* @brief   Calls the bt_mesh_health_srv_cb fault_clear callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
int fault_clear_callback(struct bt_mesh_model *model, uint16_t company_id)
{

    model_info_t curr_model_info = get_model_info(model);

    // Call the eRPC named callback
    return fault_clear_cb(curr_model_info.elem_index, curr_model_info.model_index, company_id);
}


/*********************************************************************
* @fn      fault_test callback wrapper
*
* @brief   Calls the bt_mesh_health_srv_cb fault_test callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
int fault_test_callback(struct bt_mesh_model *model, uint8_t test_id, uint16_t company_id)
{
    model_info_t curr_model_info = get_model_info(model);

    // Call the eRPC named callback
    return fault_test_cb(curr_model_info.elem_index, curr_model_info.model_index, test_id, company_id);
}


/*********************************************************************
* @fn      attn_on callback wrapper
*
* @brief   Calls the bt_mesh_health_srv_cb attn_on callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
void attn_on_callback(struct bt_mesh_model *model)
{
    model_info_t curr_model_info = get_model_info(model);

    // Call the eRPC named callback
    attn_on_cb(curr_model_info.elem_index, curr_model_info.model_index);
}


/*********************************************************************
* @fn      attn_off callback wrapper
*
* @brief   Calls the bt_mesh_health_srv_cb attn_off callback defined in the
*          application side.
*
* @param   None.
*
* @return  None.
*/
void attn_off_callback(struct bt_mesh_model *model)
{
    model_info_t curr_model_info = get_model_info(model);

    // Call the eRPC named callback
    attn_off_cb(curr_model_info.elem_index, curr_model_info.model_index);
}

/*********************************************************************
*********************************************************************/
