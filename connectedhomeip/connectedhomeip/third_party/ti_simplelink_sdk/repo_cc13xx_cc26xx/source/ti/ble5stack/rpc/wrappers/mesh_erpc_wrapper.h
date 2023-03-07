/******************************************************************************
@file  erpc_wrapper.h

@brief Definitions and prototypes for the eRPC wrapper functions

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

#ifndef MESH_ERPC_WRAPPER_H
#define MESH_ERPC_WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "autoconf.h"
#include <bluetooth/mesh/proxy.h>
#include <bluetooth/mesh/heartbeat.h>
#include "mesh.h"

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * MACROS
 */
#ifndef BIT
#define BIT(n)  (1UL << (n))
#endif
/*********************************************************************
 * TYPEDEF
 */
/** Available Provisioning output authentication actions. */
typedef enum {
    BT_MESH_NO_OUTPUT_ERPC       = 0,
    BT_MESH_BLINK_ERPC           = BIT(0),
    BT_MESH_BEEP_ERPC            = BIT(1),
    BT_MESH_VIBRATE_ERPC         = BIT(2),
    BT_MESH_DISPLAY_NUMBER_ERPC  = BIT(3),
    BT_MESH_DISPLAY_STRING_ERPC  = BIT(4),
} bt_mesh_output_action;

/** Available Provisioning input authentication actions. */
typedef enum {
    BT_MESH_NO_INPUT_ERPC      = 0,
    BT_MESH_PUSH_ERPC          = BIT(0),
    BT_MESH_TWIST_ERPC         = BIT(1),
    BT_MESH_ENTER_NUMBER_ERPC  = BIT(2),
    BT_MESH_ENTER_STRING_ERPC  = BIT(3),
} bt_mesh_input_action;

/** Available Provisioning bearers. */
typedef enum {
    BT_MESH_PROV_ADV_ERPC   = BIT(0),
    BT_MESH_PROV_GATT_ERPC  = BIT(1),
} bt_mesh_prov_bearer;

/** Out of Band information location. */
typedef enum {
    BT_MESH_PROV_OOB_OTHER_ERPC     = BIT(0),
    BT_MESH_PROV_OOB_URI_ERPC       = BIT(1),
    BT_MESH_PROV_OOB_2D_CODE_ERPC   = BIT(2),
    BT_MESH_PROV_OOB_BAR_CODE_ERPC  = BIT(3),
    BT_MESH_PROV_OOB_NFC_ERPC       = BIT(4),
    BT_MESH_PROV_OOB_NUMBER_ERPC    = BIT(5),
    BT_MESH_PROV_OOB_STRING_ERPC    = BIT(6),
    /* 7 - 10 are reserved */
    BT_MESH_PROV_OOB_ON_BOX_ERPC    = BIT(11),
    BT_MESH_PROV_OOB_IN_BOX_ERPC    = BIT(12),
    BT_MESH_PROV_OOB_ON_PAPER_ERPC  = BIT(13),
    BT_MESH_PROV_OOB_IN_MANUAL_ERPC = BIT(14),
    BT_MESH_PROV_OOB_ON_DEV_ERPC    = BIT(15),
} bt_mesh_prov_oob_info;

// Data types declaration
typedef struct bt_mesh_prov_raw         prov_raw;
typedef struct net_buf_simple_raw       buf_simple_raw;
typedef struct bt_mesh_msg_ctx_raw      msg_ctx_raw;
typedef struct bt_mesh_comp_raw         comp_raw;
typedef struct bt_mesh_elem_raw         elem_raw;
typedef struct bt_mesh_model_raw        vnd_model_raw;
typedef struct bt_mesh_model_pub_raw    pub_raw;
typedef struct bt_mesh_model_op_raw     op_raw;
typedef struct bt_mesh_model_cb_raw     cb_raw;
typedef struct bt_mesh_hb_sub           bt_mesh_hb_sub;
typedef enum   bt_mesh_key_evt          bt_mesh_key_evt;

// Callbacks definition
typedef void        (*BLEmesh_output_number)(bt_mesh_output_action act, uint32_t num);
typedef void        (*BLEmesh_output_string)(const char *str);
typedef void        (*BLEmesh_input)(bt_mesh_input_action act, uint8_t size);
typedef void 	    (*BLEmesh_input_complete)(void);
typedef void 		(*BLEmesh_unprovisioned_beacon)(uint8_t uuid[16],
                        bt_mesh_prov_oob_info oob_info,
                        uint32_t * uri_hash);
typedef void        (*BLEmesh_link_open)(bt_mesh_prov_bearer bearer);
typedef void        (*BLEmesh_link_close)(bt_mesh_prov_bearer bearer);
typedef void        (*BLEmesh_complete)(uint16_t net_idx, uint16_t addr);
typedef void        (*BLEmesh_node_added)(uint16_t net_idx, uint8_t uuid[16], uint16_t addr, uint8_t num_elem);
typedef void        (*BLEmesh_reset)(void);
typedef void        (*BLEmesh_start_cb)(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
typedef void        (*BLEmesh_init_cb)(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
typedef void        (*BLEmesh_reset_cb)(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
typedef void        (*BLEmesh_update)(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
typedef void        (*BLEmesh_settings_set_cb)(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index, const char * name, const uint8_t * data, uint8_t data_len);
typedef void        (*BLEmesh_func)(uint32_t opcode, uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index,
                                    struct bt_mesh_msg_ctx_raw * ctx, struct net_buf_simple_raw * buf);
typedef int32_t     (*BLEmesh_fault_get_cur)(uint16_t elem_idx, uint16_t model_index, uint8_t * test_id,
                                             uint16_t * company_id, uint8_t * faults, uint8_t * fault_count);
typedef int32_t     (*BLEmesh_fault_get_reg)(uint16_t elem_idx, uint16_t model_index, uint16_t company_id,
                                             uint8_t * test_id, uint8_t * faults, uint8_t * faults_count);
typedef int         (*BLEmesh_fault_clear)(uint16_t elem_idx, uint16_t model_index, uint16_t company_id);
typedef int         (*BLEmesh_fault_test)(uint16_t elem_idx, uint16_t model_index, uint8_t test_id, uint16_t company_id);
typedef void        (*BLEmesh_attn_on)(uint16_t elem_idx, uint16_t model_index);
typedef void        (*BLEmesh_attn_off)(uint16_t elem_idx, uint16_t model_index);

// The same as bt_mesh_prov struct + uuid_len parameter
/** Provisioning properties & capabilities. */
struct bt_mesh_prov_raw {
    uint8_t *uuid;

    char *uri;

    bt_mesh_prov_oob_info oob_info;

    uint8_t         *static_val;
    uint8_t        static_val_len;

    uint8_t        output_size;
    uint16_t       output_actions;

    uint8_t        input_size;
    uint16_t       input_actions;

    int         (*output_number)(bt_mesh_output_action act, uint32_t num);
    int         (*output_string)(const char *str);
    int         (*input)(bt_mesh_input_action act, uint8_t size);
    void        (*input_complete)(void);
    void        (*unprovisioned_beacon)(uint8_t uuid[16],
                        bt_mesh_prov_oob_info oob_info,
                        uint32_t *uri_hash);
    void        (*link_open)(bt_mesh_prov_bearer bearer);
    void        (*link_close)(bt_mesh_prov_bearer bearer);
    void        (*complete)(uint16_t net_idx, uint16_t addr);
    void        (*node_added)(uint16_t net_idx, uint8_t uuid[16], uint16_t addr, uint8_t num_elem);
    void        (*reset)(void);

    uint8_t uuid_len;
};


struct net_buf_simple_raw {
    /** Pointer to the start of data in the buffer. */
    uint8_t *data;

    /** Length of the data behind the data pointer. */
    uint16_t len;

    /** Amount of data that this buffer can store. */
    uint16_t size;

    /** Start of the data storage. Not to be accessed directly
     *  (the data pointer should be used instead).
     */
    uint8_t *__buf;
};

/** Message sending context. */
struct bt_mesh_msg_ctx_raw {
    /** NetKey Index of the subnet to send the message on. */
    uint16_t net_idx;

    /** AppKey Index to encrypt the message with. */
    uint16_t app_idx;

    /** Remote address. */
    uint16_t addr;

    /** Destination address of a received message. Not used for sending. */
    uint16_t recv_dst;

    /** RSSI of received packet. Not used for sending. */
    int8_t  recv_rssi;

    /** Received TTL value. Not used for sending. */
    uint8_t  recv_ttl;

    /** Force sending reliably by using segment acknowledgement */
    bool  send_rel;

    /** TTL, or BT_MESH_TTL_DEFAULT for default TTL. */
    uint8_t  send_ttl;
};

/** Node Composition */
struct bt_mesh_comp_raw {
    uint16_t cid; /**< Company ID */
    uint16_t pid; /**< Product ID */
    uint16_t vid; /**< Version ID */

    int32_t elem_count; /**< The number of elements in this device. */
    uint32_t elem_placeholder; /**< List of elements. */
};

/** Abstraction that describes a Mesh Element */
struct bt_mesh_elem_raw {
    /** Unicast Address. Set at runtime during provisioning. */
    uint16_t addr;

    /** Location Descriptor (GATT Bluetooth Namespace Descriptors) */
    uint16_t loc;
    /** The number of SIG models in this element */
    uint8_t model_count;
    /** The number of vendor models in this element */
    uint8_t vnd_model_count;

    /** The list of SIG models in this element */
    uint32_t models_placeholder;
    /** The list of vendor models in this element */
    uint32_t vnd_models_placeholder;
};

enum
{
    SIG_MODEL = 0,
    VND_MODEL = 1
};

/** Abstraction that describes a Mesh Model instance */
struct bt_mesh_model_raw {
    int32_t model_type;
    union
    {
        uint16_t id;
        struct{
            uint16_t company;
            uint16_t vnd_id;
        };
    }model;

    /** Model Publication */
    struct bt_mesh_model_pub_raw * pub;

    /** Model callback structure. */
    struct bt_mesh_model_cb_raw * cb;

    /** Model-specific user data */
    char * user_data;
};

/** Model publication context.
 *
 *  The context should primarily be created using the
 *  BT_MESH_MODEL_PUB_DEFINE macro.
 */
struct bt_mesh_model_pub_raw {

    uint16_t addr;         /**< Publish Address. */
    uint16_t key;
    // uint16_t key:12,       /**< Publish AppKey Index. */
        //   cred:1;       /**< Friendship Credentials Flag. */

    uint8_t  ttl;          /**< Publish Time to Live. */
    uint8_t  retransmit;   /**< Retransmit Count & Interval Steps. */
    uint8_t  period;       /**< Publish Period. */
    uint8_t  period_div;
    // uint8_t  period_div:4, /**< Divisor for the Period. */
            // fast_period:1,/**< Use FastPeriodDivisor */
            // count:3;      /**< Retransmissions left. */

    uint32_t period_start; /**< Start of the current period. */

    struct net_buf_simple_raw *msg;

    void (*update)(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
};

/** Model opcode handler. */
struct bt_mesh_model_op_raw {
    uint32_t  opcode;

    size_t min_len;

    void (*func)(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index,
               struct bt_mesh_msg_ctx_raw *ctx,
               struct net_buf_simple_raw *buf);
};

struct bt_mesh_model_cb_raw {
    void (*settings_set)(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index,
            const char * name, const uint8_t * data, uint8_t data_len);
    void (*start)(uint16_t elem_idx, uint8_t is_vnd,
            uint16_t model_index);
    void (*init)(uint16_t elem_idx, uint8_t is_vnd,
            uint16_t model_index);
    void (*reset)(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
};


/** Callback function for the Health Server model */
struct bt_mesh_health_srv_cb_raw {
    int32_t (*fault_get_cur)(uint16_t elem_idx, uint16_t model_index, uint8_t * test_id,
                             uint16_t * company_id, uint8_t * faults, uint8_t * fault_count);
    int32_t (*fault_get_reg)(uint16_t elem_idx, uint16_t model_index, uint16_t company_id,
                             uint8_t * test_id, uint8_t * faults, uint8_t * faults_count);
	int  (*fault_clear)(uint16_t elem_idx, uint16_t model_index, uint16_t company_id);
	int  (*fault_test)(uint16_t elem_idx, uint16_t model_index, uint8_t test_id, uint16_t company_id);
	void (*attn_on)(uint16_t elem_idx, uint16_t model_index);
	void (*attn_off)(uint16_t elem_idx, uint16_t model_index);
};

/** Mesh Health Server Model Context */
struct bt_mesh_health_srv_raw {
	/** Composition data model entry pointer. */
	uint32_t model_placeholder;

	/** Optional callback struct */
	struct bt_mesh_health_srv_cb_raw *cb;

	/** Attention Timer state */
	uint32_t k_delayed_work_placeholder;
};

/** Model publication configuration parameters. */
struct bt_mesh_cfg_mod_pub_raw {
    uint16_t  addr;     /** Publication destination address. */
    uint16_t  app_idx;  /** Application index to publish with. */
    bool   cred_flag;   /** Friendship credential flag. */
    uint8_t   ttl;      /** Time To Live to publish with. */
    /**
     * Encoded publish period.
     * @see BT_MESH_PUB_PERIOD_100MS, BT_MESH_PUB_PERIOD_SEC,
     * BT_MESH_PUB_PERIOD_10SEC,
     * BT_MESH_PUB_PERIOD_10MIN
     */
    uint8_t   period;
    /**
     * Encoded transmit parameters.
     * @see BT_MESH_TRANSMIT
     */
    uint8_t   transmit;
};

typedef struct model_info{
    uint16_t elem_index;
    uint16_t model_index;
    uint8_t  is_vnd;
}model_info_t;

/*********************************************************************
 * FUNCTIONS
 */

// bt_mesh_init wrappers
int mesh_init(void);

int bt_mesh_init_prov_raw_init(const struct bt_mesh_prov_raw *prov_raw);

int bt_mesh_init_comp_raw_init(const struct bt_mesh_comp_raw *comp_raw);
int bt_mesh_init_elem_raw_init(uint16_t elem_index, const struct bt_mesh_elem_raw *elem_raw);
int bt_mesh_cfg_cli_raw_init(uint16_t elem_index, uint16_t model_index);
int bt_mesh_init_model_raw_init(uint16_t elem_index, uint16_t model_index, const struct bt_mesh_model_raw *model_raw,
                                const struct bt_mesh_model_op_raw *op_raw, uint16_t op_len);
int bt_mesh_cfg_srv_raw_init(uint16_t model_index);
int bt_mesh_health_srv_raw_init(uint16_t elem_index, uint16_t model_index,
                                const struct bt_mesh_health_srv_raw * health_srv, uint8_t max_faults);

// APIs to configure the node after static provisioning
int bt_mesh_cfg_app_key_add_wrapper(uint16_t net_idx, uint16_t addr, uint16_t key_net_idx,
                 uint16_t key_app_idx, const uint8_t app_key[16]);
int bt_mesh_cfg_mod_app_bind_vnd_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                 uint16_t mod_app_idx, uint16_t mod_id, uint16_t cid);
int bt_mesh_cfg_mod_app_bind_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                 uint16_t mod_app_idx, uint16_t mod_id);
int bt_mesh_cfg_mod_sub_add_vnd_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                        uint16_t sub_addr, uint16_t mod_id, uint16_t cid);
int bt_mesh_cfg_mod_sub_add_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                    uint16_t sub_addr, uint16_t mod_id);
int bt_mesh_cfg_mod_sub_del_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                    uint16_t sub_addr, uint16_t mod_id);
int bt_mesh_cfg_mod_sub_del_vnd_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                        uint16_t sub_addr, uint16_t mod_id, uint16_t cid);
int bt_mesh_cfg_mod_sub_va_add_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                       const uint8_t label[16], uint16_t mod_id);
int bt_mesh_cfg_mod_sub_va_add_vnd_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                           const uint8_t label[16], uint16_t mod_id, uint16_t cid);
int bt_mesh_cfg_mod_pub_set_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                    uint16_t mod_id, struct bt_mesh_cfg_mod_pub_raw *pub);
int bt_mesh_cfg_mod_pub_set_vnd_wrapper(uint16_t net_idx, uint16_t addr, uint16_t elem_addr,
                                        uint16_t mod_id, uint16_t cid, struct bt_mesh_cfg_mod_pub_raw *pub);

// bt_mesh_model_publish wrapper
int bt_mesh_model_publish_wrapper(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index, struct net_buf_simple_raw *msg);
// bt_mesh_model_send wrapper
int bt_mesh_model_send_data_wrapper(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index,
                                    struct bt_mesh_msg_ctx_raw *ctx, struct net_buf_simple_raw *msg);
// bt_mesh_model_find wrapper
int bt_mesh_model_find_wrapper(uint16_t elem_idx, uint16_t id);
// bt_mesh_model_find_vnd wrapper
int bt_mesh_model_find_vnd_wrapper(uint16_t elem_idx, uint16_t company, uint16_t id);
// bt_mesh_model_extend wrapper
int bt_mesh_model_extend_wrapper(uint16_t mod_elem_idx, uint8_t mod_is_vnd, uint16_t mod_idx,
                                 uint16_t base_mod_elem_idx, uint8_t base_mod_is_vnd, uint16_t base_mod_idx);
// bt_mesh_fault_update wrapper
int bt_mesh_fault_update_wrapper(uint16_t elem_idx);
int bt_mesh_prov_enable_wrapper(bt_mesh_prov_bearer bearers);
int bt_mesh_prov_disable_wrapper(bt_mesh_prov_bearer bearers);

// settings_load wrapper
int settings_load_wrapper(void);

// NV store wrapper
int store_data_wrapper(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index, const char *name, uint8_t data_len, uint8_t *data);

struct bt_mesh_model * get_model_data(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
model_info_t get_model_info(struct bt_mesh_model *model);

// Callbacks declaration - used by the eRPC
void output_number_cb(bt_mesh_output_action act, uint32_t num);
void output_string_cb(const char *str);
void input_cb(bt_mesh_input_action act, uint8_t size);
void input_complete_cb(void);
void unprovisioned_beacon_cb(uint8_t uuid[16],
					      bt_mesh_prov_oob_info oob_info,
					      uint32_t *uri_hash);
void link_open_cb(bt_mesh_prov_bearer bearer);
void link_close_cb(bt_mesh_prov_bearer bearer);
void complete_cb(uint16_t net_idx, uint16_t addr);
void node_added_cb(uint16_t net_idx, uint8_t uuid[16], uint16_t addr, uint8_t num_elem);
void reset_prov_cb(void);
void start_cb(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
void init_cb(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
void reset_cb(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
void update_cb(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index);
void settings_set_cb(uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index, const char * name, const uint8_t * data, uint8_t data_len);
void func_cb(uint32_t opcode, uint16_t elem_idx, uint8_t is_vnd, uint16_t model_index,
             struct bt_mesh_msg_ctx_raw * ctx, struct net_buf_simple_raw * buf);
int32_t fault_get_cur_cb(uint16_t elem_idx, uint16_t model_index, uint8_t * test_id,
                         uint16_t * company_id, uint8_t * faults, uint8_t * fault_count);
int32_t fault_get_reg_cb(uint16_t elem_idx, uint16_t model_index, uint16_t company_id,
                         uint8_t * test_id, uint8_t * faults, uint8_t * faults_count);
int  fault_clear_cb(uint16_t elem_idx, uint16_t model_index, uint16_t company_id);
int  fault_test_cb(uint16_t elem_idx, uint16_t model_index, uint8_t test_id, uint16_t company_id);
void attn_on_cb(uint16_t elem_idx, uint16_t model_index);
void attn_off_cb(uint16_t elem_idx, uint16_t model_index);
void hb_recv_cb(const bt_mesh_hb_sub * sub, uint8_t hops, uint16_t feat);
void hb_sub_end_cb(const bt_mesh_hb_sub * sub);
void lpn_friendship_established_cb(uint16_t net_idx, uint16_t friend_addr, uint8_t queue_size, uint8_t recv_window);
void lpn_friendship_terminated_cb(uint16_t net_idx, uint16_t friend_addr);
void lpn_polled_cb(uint16_t net_idx, uint16_t friend_addr, bool retry);
void friend_friendship_established_cb(uint16_t net_idx, uint16_t lpn_addr, uint8_t recv_delay, uint32_t polltimeout);
void friend_friendship_terminated_cb(uint16_t net_idx, uint16_t lpn_addr);
void appkey_evt_cb(uint16_t app_idx, uint16_t net_idx, bt_mesh_key_evt evt);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MESH_ERPC_WRAPPER_H */
