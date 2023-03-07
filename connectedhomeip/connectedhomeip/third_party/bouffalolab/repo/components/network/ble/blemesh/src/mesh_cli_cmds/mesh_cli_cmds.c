#include <stdlib.h>
#include "conn.h"
#include "gatt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "mesh_cli_cmds.h"
#include "include/mesh.h"
#include "errno.h"

#include "mesh.h"
#include "net.h"
#include "transport.h"
#include "foundation.h"
#include "mesh_settings.h"
#include "adv.h"
#include "beacon.h"
#include "hci_core.h"
#include "log.h"
#if defined(CONFIG_BT_MESH_MODEL)
#if (defined(CONFIG_BT_MESH_MODEL_GEN_SRV) || defined(CONFIG_BT_MESH_MODEL_GEN_CLI))
#include "bfl_ble_mesh_generic_model_api.h"
#endif
#if (defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV) || defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI))
#include "bfl_ble_mesh_lighting_model_api.h"
#endif
#include "bfl_ble_mesh_local_data_operation_api.h"
#include "bfl_ble_mesh_networking_api.h"
#else
#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
#include "gen_srv.h"
#endif
#endif /* CONFIG_BT_MESH_MODEL */

#if defined(CONFIG_BT_MESH_SYNC)
#include "blsync_ble_app.h" // added blemesh sync 
#endif

#if defined(CONFIG_BT_SETTINGS)
#include "easyflash.h"
#endif
#include <../../blestack/src/include/bluetooth/crypto.h>
#include "local_operation.h"

#define CUR_FAULTS_MAX 4

bool blemesh_inited = false;
#if defined(CONFIG_BT_MESH_LOW_POWER)
//below value is for Tmall Genie
u8_t dev_uuid[16] = {0xA8,0x01,0x71,0x5e,0x1c,0x00,0x00,0xe4,0x46,0x46,0x63,0xa7,0xf8,0x02,0x00,0x00};
u8_t auth_value[16] = {0x78,0x8A,0xE3,0xEE,0x0F,0x2A,0x7E,0xFA,0xD3,0x67,0x35,0x81,0x41,0xFE,0x1B,0x06};
#else
u8_t dev_uuid[16] = {0xA8,0x01,0x71,0xe0,0x1a,0x00,0x00,0x0f,0x7e,0x35,0x63,0xa7,0xf8,0x02,0x00,0x00};
u8_t auth_value[16] = {0x7f,0x80,0x1a,0xf4,0xa0,0x8c,0x50,0x39,0xae,0x7d,0x7b,0x44,0xa0,0x92,0xd9,0xc2};
#endif

static bt_mesh_input_action_t input_act;
static u8_t input_size;
static u8_t cur_faults[CUR_FAULTS_MAX];
static u8_t reg_faults[CUR_FAULTS_MAX * 2];

static struct {
	u16_t local;
	u16_t dst;
	u16_t net_idx;
	u16_t app_idx;
} net = {
	.local = BT_MESH_ADDR_UNASSIGNED,
	.dst = BT_MESH_ADDR_UNASSIGNED,
};

#if defined(BL602) || defined(BL702) || defined(BL606P)
#define vOutputString(...)  printf(__VA_ARGS__)
#else
#define vOutputString(...)  bl_print(SYSTEM_UART_ID, PRINT_MODULE_CLI, __VA_ARGS__)
#endif

static void prov_reset(void);
static void gen_dev_uuid(void);
static void link_open(bt_mesh_prov_bearer_t bearer);
static void link_close(bt_mesh_prov_bearer_t bearer);
static int output_number(bt_mesh_output_action_t action, u32_t number);
#ifdef CONFIG_BT_MESH_PROVISIONER
static u8_t capabilities(prov_caps_t* prv_caps, prov_start_t* prv_start);
#endif
static int output_string(const char *str);
static void prov_input_complete(void);
static void prov_complete(u16_t net_idx, u16_t addr);
static void prov_reset(void);
static int input(bt_mesh_input_action_t act, u8_t size);
static void get_faults(u8_t *faults, u8_t faults_size, u8_t *dst, u8_t *count);
static void health_current_status(struct bt_mesh_health_cli *cli, u16_t addr,
				  u8_t test_id, u16_t cid, u8_t *faults, size_t fault_count);
static int fault_get_cur(struct bt_mesh_model *model, u8_t *test_id,
			 u16_t *company_id, u8_t *faults, u8_t *fault_count);
static int fault_get_reg(struct bt_mesh_model *model, u16_t cid,
			 u8_t *test_id, u8_t *faults, u8_t *fault_count);
static int fault_clear(struct bt_mesh_model *model, uint16_t cid);
static int fault_test(struct bt_mesh_model *model, uint8_t test_id, uint16_t cid);
static void attn_on(struct bt_mesh_model *model);
static void attn_off(struct bt_mesh_model *model);

static void blemeshcli_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#if defined(CONFIG_BT_MESH_PROVISIONER)
static void blemeshcli_pvnr_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif
static void blemeshcli_set_dev_uuid(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void blemeshcli_input_num(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void blemeshcli_input_str(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#if defined(CONFIG_BT_MESH_MODEL)
#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
static void blemeshcli_gen_oo_cli(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
static void blemeshcli_light_lgn_cli(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void blemeshcli_light_ctl_cli(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void blemeshcli_light_hsl_cli(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
static void blemeshcli_vendor_cli(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif
#endif /* CONFIG_BT_MESH_MODEL */
static void blemeshcli_pb(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#if defined(CONFIG_BT_MESH_PB_ADV)
#if defined(CONFIG_BT_MESH_PROVISIONER)
static void blemeshcli_provision_adv(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif /* CONFIG_BT_MESH_PROVISIONER */
#endif /* CONFIG_BT_MESH_PB_ADV */
static void blemeshcli_reset(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void blemeshcli_net_send(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void blemeshcli_seg_send(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void blemeshcli_rpl_clr(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void blemeshcli_ivu_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void blemeshcli_iv_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void blemeshcli_fault_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);

#if defined(CONFIG_BT_MESH_LOW_POWER)
static void blemeshcli_lpn_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif

#if defined(CONFIG_BT_MESH_CDB)
static void blemeshcli_cdb_create(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cdb_clear(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cdb_show(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cdb_node_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cdb_node_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cdb_subnet_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cdb_subnet_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cdb_app_key_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cdb_app_key_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
#endif /* CONFIG_BT_MESH_CDB */
#if defined(CONFIG_BT_MESH_PROVISIONER)
static void blemeshcli_beacon_listen(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_provision(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_node_cfg(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
#endif /* CONFIG_BT_MESH_PROVISIONER */
#if defined(CONFIG_BT_MESH_PROVISIONER)
static void blemeshcli_get_comp(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_set_dst(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_krp_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_krp_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cfg_bcn_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cfg_bcn_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cfg_dttl_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cfg_dttl_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cfg_gpxy_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_cfg_gpxy_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_friend(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_relay(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_node_identify(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_node_reset(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_network_trans(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_lpn_timeout_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_net_key_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
/* Added by bouffalo */
static void blemeshcli_net_key_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_net_key_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_net_key_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_app_key_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
/* Added by bouffalo */
static void blemeshcli_app_key_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_app_key_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_app_key_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
/* Added by bouffalo */
static void blemeshcli_kr_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_app_bind(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_app_unbind(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_app_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_sub_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_sub_ow(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_sub_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_sub_del_all(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_sub_add_va(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_sub_ow_va(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_sub_del_va(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_sub_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_mod_pub(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
/* Added by bouffalo */
static void blemeshcli_mod_pub_va(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_hb_sub(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_hb_pub(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);

static void blemeshcli_clhm_fault(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_clhm_period(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_clhm_ats(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
#endif
#if defined(CFG_NODE_SEND_CFGCLI_MSG) && defined(CONFIG_BT_MESH_CDB)
static void blemeshcli_pvnr_devkey(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
#endif
#if defined(CONFIG_BT_MESH_SYNC)
static void blemeshcli_sync_start(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
static void blemeshcli_sync_stop(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
#endif

#if defined(CONFIG_BT_MESH_TEST)
static int nodelist_check(uint16_t addr);
static int nodelist_check_clear();
static void blemeshcli_nodelist_op(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[]);
#endif /* CONFIG_BT_MESH_TEST */

#if defined(CONFIG_BT_MESH_PROVISIONER)
static void print_node_added(u16_t net_idx, u8_t uuid[16], u16_t addr, u8_t num_elem);
#endif

static struct bt_mesh_prov prov = {
	.uuid = dev_uuid,
    .oob_info = 0,
	.link_open = link_open,
	.link_close = link_close,
	.complete = prov_complete,
	.reset = prov_reset,
	.static_val = auth_value,
	.static_val_len = 16,
#ifdef CONFIG_BT_MESH_PTS
	.output_size = 8,
#else
	.output_size = 6,
#endif
#ifdef CONFIG_BT_MESH_PTS
	.output_actions = (BT_MESH_BLINK | BT_MESH_BEEP | BT_MESH_VIBRATE | BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
#else
	.output_actions = (BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
#endif
#ifdef CONFIG_BT_MESH_PROVISIONER
	.capabilities = capabilities,
	.node_added = print_node_added,
#endif
	.output_number = output_number,
	.output_string = output_string,
#ifdef CONFIG_BT_MESH_PTS
	.input_size = 8,
#else
	.input_size = 6,
#endif
#ifdef CONFIG_BT_MESH_PTS
	.input_actions = (BT_MESH_PUSH | BT_MESH_TWIST | BT_MESH_ENTER_NUMBER | BT_MESH_ENTER_STRING),
#else
	.input_actions = (BT_MESH_ENTER_NUMBER | BT_MESH_ENTER_STRING),
#endif
	.input = input,
	.input_complete = prov_input_complete,
};

static const struct bt_mesh_health_srv_cb health_srv_cb = {
	.fault_get_cur = fault_get_cur,
	.fault_get_reg = fault_get_reg,
	.fault_clear = fault_clear,
	.fault_test = fault_test,
	.attn_on = attn_on,
	.attn_off = attn_off,
};

static struct bt_mesh_health_srv health_srv = {
	.cb = &health_srv_cb,
};

static struct bt_mesh_health_cli health_cli = {
	.current_status = health_current_status,
};

BT_MESH_HEALTH_PUB_DEFINE(health_pub, CUR_FAULTS_MAX);

static struct bt_mesh_cfg_cli cfg_cli = {
};

static struct bt_mesh_cfg_srv cfg_srv = {
	.relay = BT_MESH_RELAY_ENABLED,
	.beacon = BT_MESH_BEACON_ENABLED,//BT_MESH_BEACON_DISABLED,
#if defined(CONFIG_BT_MESH_FRIEND)
	.frnd = BT_MESH_FRIEND_DISABLED,
#else
	.frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BT_MESH_GATT_PROXY)
	.gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
#else
	.gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif

	.default_ttl = 7,

	/* 6 transmissions with 20ms interval */
	.net_transmit = BT_MESH_TRANSMIT(5, 20),
	/* 3 transmissions with 20ms interval */
	.relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

#if defined(CONFIG_BT_MESH_MODEL)
#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
BFL_BLE_MESH_MODEL_PUB_DEFINE(onoff_pub, 2 + 3, ROLE_NODE);
static bfl_ble_mesh_gen_onoff_srv_t onoff_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
};

#endif

#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
BFL_BLE_MESH_MODEL_PUB_DEFINE(onoff_cli_pub, 2 + 1, ROLE_NODE);
static bfl_ble_mesh_client_t onoff_client;
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
BFL_BLE_MESH_MODEL_PUB_DEFINE(lightness_pub, 2 + 3, ROLE_NODE);
static bfl_ble_mesh_light_lightness_state_t lightness_state;
static bfl_ble_mesh_light_lightness_srv_t lightness_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_RSP_BY_APP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_RSP_BY_APP,
    .state = &lightness_state,
};
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
BFL_BLE_MESH_MODEL_PUB_DEFINE(lightness_cli_pub, 2 + 1, ROLE_NODE);
static bfl_ble_mesh_client_t lightness_client;
#endif
	
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
BFL_BLE_MESH_MODEL_PUB_DEFINE(ctl_pub, 2 + 3, ROLE_NODE);
static bfl_ble_mesh_light_ctl_state_t ctl_state;
static bfl_ble_mesh_light_ctl_srv_t ctl_server = {
	.rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
	.rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
	.state = &ctl_state,
};
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
BFL_BLE_MESH_MODEL_PUB_DEFINE(ctl_cli_pub, 2 + 1, ROLE_NODE);
static bfl_ble_mesh_client_t ctl_client;
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
BFL_BLE_MESH_MODEL_PUB_DEFINE(hsl_pub, 2 + 3, ROLE_NODE);
static bfl_ble_mesh_light_hsl_state_t hsl_state;
static bfl_ble_mesh_light_hsl_srv_t hsl_server = {
	.rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
	.rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
	.state = &hsl_state,
};
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
BFL_BLE_MESH_MODEL_PUB_DEFINE(hsl_cli_pub, 2 + 1, ROLE_NODE);
static bfl_ble_mesh_client_t hsl_client;
#endif

#else
#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
struct bt_mesh_gen_onoff_srv onoff_srv = {
};
#endif
#endif /* CONFIG_BT_MESH_MODEL */

static struct bt_mesh_model sig_models[] = {
	BT_MESH_MODEL_CFG_SRV(&cfg_srv),
	BT_MESH_MODEL_CFG_CLI(&cfg_cli),
	BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
	BT_MESH_MODEL_HEALTH_CLI(&health_cli),
#if !defined(CONFIG_BT_MESH_MODEL)
	#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
	BT_MESH_MODEL_GEN_ONOFF(&onoff_srv),
	#endif
#else
	#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
	BFL_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub, &onoff_server),
	#endif
	#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
	BFL_BLE_MESH_MODEL_GEN_ONOFF_CLI(&onoff_cli_pub, &onoff_client),
	#endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
	BFL_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SRV(&lightness_pub, &lightness_server),
    #endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
	BFL_BLE_MESH_MODEL_LIGHT_LIGHTNESS_CLI(&lightness_cli_pub, &lightness_client),
    #endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
	BFL_BLE_MESH_MODEL_LIGHT_CTL_SRV(&ctl_pub, &ctl_server),
    #endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
	BFL_BLE_MESH_MODEL_LIGHT_CTL_CLI(&ctl_cli_pub, &ctl_client),
    #endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
	BFL_BLE_MESH_MODEL_LIGHT_HSL_SRV(&hsl_pub, &hsl_server),
    #endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
	BFL_BLE_MESH_MODEL_LIGHT_HSL_CLI(&hsl_cli_pub, &hsl_client),
    #endif
#endif /* CONFIG_BT_MESH_MODEL */
};

struct vendor_data_t{
	uint8_t data[BT_MESH_TX_VND_SDU_MAX_SHORT];
};
static struct vendor_data_t vendor_data;

#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
static struct vendor_data_t vendor_data_cli;
#endif /*CONFIG_BT_MESH_MODEL_VENDOR_CLI*/

static void vendor_data_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, BT_MESH_TX_SDU_MAX);

    vOutputString("data[%s]\n", bt_hex(buf->data, buf->len));

    if (buf == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }
    bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_VND_OP_DATA_STATUS);
	net_buf_simple_add_mem(&msg, buf->data, buf->len);

    bt_mesh_model_send(model, ctx, &msg, NULL, NULL);
}

#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
static void vendor_data_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    vOutputString("Vendor status[%s]\n", bt_hex(buf->data, buf->len));
}
#endif /*CONFIG_BT_MESH_MODEL_VENDOR_CLI*/

/* Mapping of message handlers for Generic Power OnOff Server (0x1006) */
const struct bt_mesh_model_op vendor_data_op[] = {
    { BLE_MESH_MODEL_VND_OP_DATA_SET, 1, vendor_data_set },
    { BLE_MESH_MODEL_VND_OP_DATA_SET_UNACK, 1, vendor_data_set },
    BT_MESH_MODEL_OP_END,
};
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
const struct bt_mesh_model_op vendor_data_op_cli[] = {
    { BLE_MESH_MODEL_VND_OP_DATA_STATUS, 1, vendor_data_status },
    BT_MESH_MODEL_OP_END,
};
#endif /*CONFIG_BT_MESH_MODEL_VENDOR_CLI*/


static struct bt_mesh_model vendor_models[] = {
	BT_MESH_MODEL_VND(BL_COMP_ID, BT_MESH_VND_MODEL_ID_DATA_SRV, vendor_data_op, NULL, &vendor_data),
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
	BT_MESH_MODEL_VND(BL_COMP_ID, BT_MESH_VND_MODEL_ID_DATA_CLI, vendor_data_op_cli, NULL, &vendor_data_cli),
#endif /*CONFIG_BT_MESH_MODEL_VENDOR_CLI*/
#if defined(CONFIG_BT_MESH_SYNC)
	BFL_BLE_MESH_MODEL_VND_SYNC_SRV(),
#endif
};

static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(0, sig_models, vendor_models),
};

static const struct bt_mesh_comp comp = {
	.cid = BL_COMP_ID,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};

#if defined(BL602) || defined(BL702) || defined(BL606P)
const struct cli_command btMeshCmdSet[] STATIC_CLI_CMD_ATTRIBUTE = {
#else
const struct cli_command btMeshCmdSet[] = {
#endif
    {"blemesh_init", "blemesh_init:[Initialize]\r\n Parameter[Null]", blemeshcli_init},
#if defined(CONFIG_BT_MESH_PROVISIONER)
    {"blemesh_pvnr_init", "blemesh_pvnr_init:[Initialize]\r\n Parameter[Null]", blemeshcli_pvnr_init},
#endif
	{"blemesh_set_dev_uuid", "blemesh_input_num:[input number in provisionging procedure]\r\n\
     [Size:16 Octets, e.g.112233445566778899AA]", blemeshcli_set_dev_uuid},
    {"blemesh_pb", "blemesh_pb:[Enable or disable provisioning]\r\n\
     [bear, 1:adv bear, 2:gatt bear]\r\n\
     [enable, 0:disable provisioning, 1:enable provisioning]", blemeshcli_pb},
#if defined(CONFIG_BT_MESH_PB_ADV)
#if defined(CONFIG_BT_MESH_PROVISIONER)
	{"blemesh_provision_adv", "blemesh_pb:[Enable or disable provisioning]\r\n\
		 [bear, 1:adv bear, 2:gatt bear]\r\n\
		 [enable, 0:disable provisioning, 1:enable provisioning]", blemeshcli_provision_adv},
#endif /* CONFIG_BT_MESH_PROVISIONER */
#endif /* CONFIG_BT_MESH_PB_ADV */
    
    {"blemesh_reset", "blemesh_reset:[Reset the state of the local mesh node]\r\n Parameter[Null]", blemeshcli_reset},
    {"blemesh_net_send", "blemesh_net_send:[Send a network packet]\r\n Parameter[TTL CTL SRC DST]", blemeshcli_net_send},
    {"blemesh_seg_send", "blemesh_seg_send:[Send a segmented message]\r\n Parameter[SRC DST]", blemeshcli_seg_send},
    {"blemesh_rpl_clr", "blemesh_rpl_clr:[Clear replay protection list]\r\n Parameter[Null]", blemeshcli_rpl_clr},
    {"blemesh_ivu_test", "blemesh_ivu_test:[Enable or disable iv update test mode]\r\n\
     [enable, 0:disable, 1:enable]", blemeshcli_ivu_test},
    {"blemesh_iv_update", "blemesh_iv_update:[Enable or disable iv update procedure]\r\n\
     [enable, 0:disable, 1:enable by sending secure network beacons]", blemeshcli_iv_update},
    {"blemesh_fault_set", "blemesh_fault_set:[Set current fault or registered fault values]\r\n\
     [type, 0:current fault, 1:registered fault]\r\n\
     [fault, fault array in hex format]", blemeshcli_fault_set},
    #if defined(CONFIG_BT_MESH_LOW_POWER)
    {"blemesh_lpn_set", "blemesh_lpn_set:[Enable or disable low power node]\r\n\
     [enable, 0:disable lpn, 1:enable lpn]", blemeshcli_lpn_set},
    #endif
    {"blemesh_input_num", "blemesh_input_num:[input number in provisionging procedure]\r\n\
     [Max Size:16 Octets, e.g.112233445566778899AA]", blemeshcli_input_num},
     
    {"blemesh_input_str", "blemesh_input_str:[input Alphanumeric in provisionging procedure]\r\n\
     [Max Size:16 Characters, e.g.123ABC]", blemeshcli_input_str},
#if defined(CONFIG_BT_MESH_MODEL)
#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
	{"blemesh_gen_oo_cli", "blemesh_gen_oo_cli:[cmd op app_idx opcode msg_role addr net_idx op_en_t onoff tid trans_time delay]\r\n\
     []", blemeshcli_gen_oo_cli},
#endif
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
     {"blemesh_light_lgn_cli", "blemesh_light_lgn_cli:[cmd op app_idx opcode msg_role addr net_idx op_en_t lightness tid trans_time delay]", blemeshcli_light_lgn_cli},
     {"blemesh_light_ctl_cli", "blemesh_light_ctl_cli:[cmd op app_idx opcode msg_role addr net_idx op_en_t ctl_lightness ctl_temperatrue ctl_delta_uv tid trans_time delay]", blemeshcli_light_ctl_cli},
     {"blemesh_light_hsl_cli", "blemesh_light_hsl_cli:[cmd op app_idx opcode msg_role addr net_idx op_en lightness hue saturation tid trans_time delay]", blemeshcli_light_hsl_cli},
#endif
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
	{"blemesh_vendor_cli", "blemesh_vendor_cli:[cmd op app_idx opcode msg_role addr net_idx]", blemeshcli_vendor_cli},
#endif
#endif /* CONFIG_BT_MESH_MODEL */
#if defined(CONFIG_BT_MESH_CDB)
	{"blemesh_cdb_create", "", blemeshcli_cdb_create},
	{"blemesh_cdb_clear", "", blemeshcli_cdb_clear},
	{"blemesh_cdb_show", "", blemeshcli_cdb_show},
	{"blemesh_cdb_node_add", "", blemeshcli_cdb_node_add},
	{"blemesh_cdb_node_del", "", blemeshcli_cdb_node_del},
	{"blemesh_cdb_subnet_add", "", blemeshcli_cdb_subnet_add},
	{"blemesh_cdb_subnet_del", "", blemeshcli_cdb_subnet_del},
	{"blemesh_cdb_app_key_add", "", blemeshcli_cdb_app_key_add},
	{"blemesh_cdb_app_key_del", "", blemeshcli_cdb_app_key_del},
#endif /* CONFIG_BT_MESH_CDB */
#if defined(CONFIG_BT_MESH_PROVISIONER)
	{"blemesh_beacon_listen", "", blemeshcli_beacon_listen},
	{"blemesh_provision", "", blemeshcli_provision},
	{"blemesh_node_cfg", "", blemeshcli_node_cfg},
#endif /* CONFIG_BT_MESH_PROVISIONER */
#if defined(CONFIG_BT_MESH_PROVISIONER)
	{"blemesh_get_comp", "", blemeshcli_get_comp},
	{"blemesh_set_dst", "", blemeshcli_set_dst},
	{"blemesh_net_key_add", "", blemeshcli_net_key_add},
	/* Added by bouffalo */
	{"blemesh_net_key_update", "", blemeshcli_net_key_update},
	{"blemesh_net_key_get", "", blemeshcli_net_key_get},
	{"blemesh_net_key_del", "", blemeshcli_net_key_del},
	{"blemesh_app_key_add", "", blemeshcli_app_key_add},
	/* Added by bouffalo */
	{"blemesh_app_key_update", "", blemeshcli_app_key_update},
	{"blemesh_app_key_get", "", blemeshcli_app_key_get},
	{"blemesh_app_key_del", "", blemeshcli_app_key_del},
	/* Added by bouffalo */
	{"blemesh_kr_update", "", blemeshcli_kr_update},
	{"blemesh_mod_app_bind", "", blemeshcli_mod_app_bind},
	{"blemesh_mod_app_unbind", "", blemeshcli_mod_app_unbind},
	{"blemesh_mod_app_get", "", blemeshcli_mod_app_get},
	{"blemesh_mod_sub_add", "", blemeshcli_mod_sub_add},
	{"blemesh_mod_sub_ow", "", blemeshcli_mod_sub_ow},
	{"blemesh_mod_sub_del", "", blemeshcli_mod_sub_del},
	{"blemesh_mod_sub_del_all", "", blemeshcli_mod_sub_del_all},
	{"blemesh_mod_sub_add_va", "", blemeshcli_mod_sub_add_va},
	{"blemesh_mod_sub_ow_va", "", blemeshcli_mod_sub_ow_va},
	{"blemesh_mod_sub_del_va", "", blemeshcli_mod_sub_del_va},
	{"blemesh_mod_sub_get", "", blemeshcli_mod_sub_get},
	{"blemesh_mod_pub", "", blemeshcli_mod_pub},
	{"blemesh_mod_pub_va", "", blemeshcli_mod_pub_va},
	{"blemesh_hb_sub", "", blemeshcli_hb_sub},
	{"blemesh_hb_pub", "", blemeshcli_hb_pub},
	{"blemesh_krp_get", "", blemeshcli_krp_get},
	{"blemesh_krp_set", "", blemeshcli_krp_set},
	{"blemesh_cfg_bcn_get", "", blemeshcli_cfg_bcn_get},
	{"blemesh_cfg_bcn_set", "", blemeshcli_cfg_bcn_set},
	{"blemesh_cfg_dttl_get", "", blemeshcli_cfg_dttl_get},
	{"blemesh_cfg_dttl_set", "", blemeshcli_cfg_dttl_set},
	{"blemesh_cfg_gpxy_get", "", blemeshcli_cfg_gpxy_get},
	{"blemesh_cfg_gpxy_set", "", blemeshcli_cfg_gpxy_set},
	{"blemesh_friend", "", blemeshcli_friend},
	{"blemesh_relay", "", blemeshcli_relay},
	{"blemesh_node_identify", "", blemeshcli_node_identify},
	{"blemesh_node_reset", "[0]net_idx; [1]dst addr", blemeshcli_node_reset},
	{"blemesh_network_trans", "", blemeshcli_network_trans},
	{"blemesh_lpn_timeout_get", "", blemeshcli_lpn_timeout_get},
	{"blemesh_clhm_fault", "", blemeshcli_clhm_fault},
	{"blemesh_clhm_period", "", blemeshcli_clhm_period},
	{"blemesh_clhm_ats", "", blemeshcli_clhm_ats},
#endif
#if defined(CFG_NODE_SEND_CFGCLI_MSG) && defined(CONFIG_BT_MESH_CDB)
	{"blemesh_pvnr_devkey", "", blemeshcli_pvnr_devkey},
#endif
#if defined(CONFIG_BT_MESH_SYNC)
	{"blemesh_sync_start", "No parameter", blemeshcli_sync_start},
	{"blemesh_sync_stop", "No parameter", blemeshcli_sync_stop},
#endif
#if defined(CONFIG_BT_MESH_TEST)
    {"blemesh_nodelist_op", "", blemeshcli_nodelist_op},
#endif /* CONFIG_BT_MESH_TEST */
    #if defined(BL70X)
    {NULL, NULL, "No handler / Invalid command", NULL}
    #endif
};

/* Read string from uart */
static void read_str(char* str, u8_t size)
{
	extern int cli_getchar(char *inbuf);
	char* str_s = str;
	while(str - str_s <= size){
		while(!cli_getchar(str));
		if(*str == '\r' || *str == '\n'){
			*str = 0;
			break;
		}
		vOutputString("%c", *str);
		str++;
	}
}

#if defined(CONFIG_BT_MESH_LOW_POWER)
static void blemeshcli_lpn_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    static bool lpn_enabled;
    u8_t enable;
    int err;


    get_uint8_from_string(&argv[1], &enable);
    
    if(enable){
        if(lpn_enabled){
            vOutputString("LPN already enabled\r\n");
            return;
        }
        
        err = bt_mesh_lpn_set(true);
        if(err){
            vOutputString("Failed to enable LPN\r\n");
        }else{
            lpn_enabled = true;
            vOutputString("Enable LPN successfully\r\n");
        }     
    }else{
        if(!lpn_enabled){
            vOutputString("LPN already disabled\r\n");
            return;
        }

        err = bt_mesh_lpn_set(false);
        if(err){
            vOutputString("Failed to disable LPN\r\n");
        }else{
            lpn_enabled = false;
            vOutputString("Disable LPN successfully\r\n");
        }
    }
}

static void lpn_cb(u16_t friend_addr, bool established)
{
	if (established) {
		vOutputString("Friendship (as LPN) established to Friend 0x%04x\r\n", friend_addr);
	} else {
		vOutputString("Friendship (as LPN) lost with Friend 0x%04x\r\n", friend_addr);
	}
}
#endif

static void blemeshcli_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int err;

    if(blemesh_inited){
        vOutputString("Has initialized \r\n");
        return;
    }

    // auto generate device uuid
    gen_dev_uuid();
#ifdef CONFIG_BT_MESH_PROVISIONER
	/* set role type as node */
	prov.role = 0;
#endif
    err = bt_mesh_init(&prov, &comp);
    if(err){
        vOutputString("Failed to init \r\n");
        return;
    }

    blemesh_inited = true;
    vOutputString("Init successfully \r\n");

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        mesh_set();
        mesh_commit();
    }

    if (bt_mesh_is_provisioned()) {
		vOutputString("Mesh network restored from flash\r\n");
	} else {
		vOutputString("Use pb-adv or pb-gatt to enable advertising\r\n");
	}

#ifdef CONFIG_BT_MESH_PTS
    int i;

    // used for almost all test cases
    vOutputString("[PTS] TSPX_bd_addr_iut: %s\n", 
    				bt_hex(bt_dev.id_addr[0].a.val, 6));

    // used for almost all test cases
    vOutputString("[PTS] TSPX_device_uuid: %s\n", bt_hex(dev_uuid, 16));

    // used for test case MESH/NODE/RLY/BV-02-C
    vOutputString("[PTS] TSPX_maximum_network_message_cache_entries: %d\r\n", 
    				CONFIG_BT_MESH_MSG_CACHE_SIZE);

    // used for test case MESH/NODE/CFG/CFGR/BV-01-C
    vOutputString("[PTS] TSPX_iut_supports_relay: %s\r\n", 
    				cfg_srv.relay == BT_MESH_RELAY_ENABLED ? "TRUE" : "FALSE");

    // used for test case MESH/NODE/CFG/SL/BV-03-C
    vOutputString("[PTS] TSPX_vendor_model_id: %04X%04X\r\n", 
    				vendor_models[0].vnd.company, vendor_models[0].vnd.id);
#endif

#if IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)
	bt_mesh_lpn_set_cb(lpn_cb);
#endif
}
static const u16_t net_idx = BT_MESH_NET_PRIMARY;
static const u16_t app_idx = BT_MESH_APP_PRIMARY;

#if defined(CONFIG_BT_MESH_PROVISIONER)
static u16_t self_addr = 1;
static void setup_cdb(void)
{
	struct bt_mesh_cdb_app_key *key;

	key = bt_mesh_cdb_app_key_alloc(net_idx, app_idx);
	if (key == NULL) {
		vOutputString("Failed to allocate app-key 0x%04x\n");
		return;
	}

	bt_rand(key->keys[0].app_key, 16);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_app_key_store(key);
	}
}

static void blemeshcli_pvnr_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	u8_t net_key[16], dev_key[16];
	int err;

	if(blemesh_inited){
        vOutputString("Has initialized \r\n");
        return;
    }

#ifdef CONFIG_BT_MESH_PROVISIONER
	/* set role type as provisioner */
	prov.role = 1;
#endif

	err = bt_mesh_init(&prov, &comp);
	if (err) {
		vOutputString("Initializing mesh failed (err %d)\n");
		return;
	}
	blemesh_inited = true;
	printk("Mesh initialized\n");

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		vOutputString("Loading stored settings\n");
		mesh_set();
		mesh_commit();
	}

	bt_rand(net_key, 16);

	err = bt_mesh_cdb_create(net_key);
	if (err == -EALREADY) {
		vOutputString("Using stored CDB\n");
	} else if (err) {
		vOutputString("Failed to create CDB (err %d)\n", err);
		return;
	} else {
		vOutputString("Created CDB\n");
		setup_cdb();
	}

	bt_rand(dev_key, 16);

	err = bt_mesh_provision(net_key, BT_MESH_NET_PRIMARY, 0, 0, self_addr,
				dev_key);
	if (err == -EALREADY) {
		vOutputString("Using stored settings\n");
	} else if (err) {
		vOutputString("Provisioning failed (err %d)\n", err);
		return;
	} else {
		vOutputString("Network key:%s\n", bt_hex(net_key, 16));
		vOutputString("Dev key:%s\n", bt_hex(dev_key, 16));
		vOutputString("Provisioning completed\n");

		//Do local APP KEY Add
		if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
			struct bt_mesh_cdb_app_key *app_key;
			
			app_key = bt_mesh_cdb_app_key_get(app_idx);
			if (!app_key) {
				return;
			}
			u8_t status;
			err = bt_mesh_cfg_app_key_add(net_idx, self_addr, net_idx,
						app_idx, app_key->keys[0].app_key, &status);
			if (err) {
				vOutputString("Unable to send App Key Add (err %d)", err);
				return;
			}
		}
		//Do local model bound.
		bt_mesh_local_model_bind(net_idx, app_idx);
	}

	return;
}
#endif

static const char *bearer2str(bt_mesh_prov_bearer_t bearer)
{
	switch (bearer) {
	case BT_MESH_PROV_ADV:
		return "PB-ADV";
	case BT_MESH_PROV_GATT:
		return "PB-GATT";
	default:
		return "unknown";
	}
}

#if defined(CONFIG_BT_MESH_PROV)
static void blemeshcli_pb(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int err;
    uint8_t bearer;
    uint8_t enable;

    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &bearer);
    get_uint8_from_string(&argv[2], &enable);
    
	if (enable) {
		err = bt_mesh_prov_enable(bearer);
		if (err) {
			vOutputString("Failed to enable %s (err %d)\r\n", bearer2str(bearer), err);
		} else {
			vOutputString("%s enabled\r\n", bearer2str(bearer));
		}
	} else {
		err = bt_mesh_prov_disable(bearer);
		if (err) {
			vOutputString("Failed to disable %s (err %d)\r\n",
				    bearer2str(bearer), err);
		} else {
			vOutputString("%s disabled\r\n", bearer2str(bearer));
		}
	}
}
#endif

#if defined(CONFIG_BT_MESH_PB_ADV)
#if defined(CONFIG_BT_MESH_PROVISIONER)
static void blemeshcli_provision_adv(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	u8_t uuid[16];
	u8_t attention_duration;
	u16_t net_idx;
	u16_t addr;
	size_t len;
	int err;
	/* Get UUID Form arg[1] */
	len = hex2bin(argv[1], strlen(argv[1]), uuid, sizeof(uuid));
	(void)memset(uuid + len, 0, sizeof(uuid) - len);

	/* Get Net Idx Form arg[2] */
	net_idx = strtoul(argv[2], NULL, 0);
	/* Get Unicast address Form arg[3] */
	addr = strtoul(argv[3], NULL, 0);
	/* Get Unicast address Form arg[4] */
	attention_duration = strtoul(argv[4], NULL, 0);

	err = bt_mesh_provision_adv(uuid, net_idx, addr, attention_duration);
	if (err) {
		vOutputString("Provisioning failed (err %d)\n", err);
	}

	return;
}
#endif /* CONFIG_BT_MESH_PROVISIONER */
#endif /* CONFIG_BT_MESH_PB_ADV */

static void link_open(bt_mesh_prov_bearer_t bearer)
{
	vOutputString("Provisioning link opened on %s\r\n", bearer2str(bearer));
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
	vOutputString("Provisioning link closed on %s\r\n", bearer2str(bearer));
}
#ifdef CONFIG_BT_MESH_PROVISIONER
const static char* output_oob_action[5]= {
"Blink","Beep","Vibrate","Output Numeric","Output Alphanumeric",
};

const static char* input_oob_action[4]= {
"Push","Twist","Input Numeric","Input Alphanumeric",
};

static u8_t capabilities(prov_caps_t* prv_caps, prov_start_t* prv_start)
{
	vOutputString(">>>num_ele[%x]\n", prv_caps->num_ele);

	prv_start->algo = 0;
	vOutputString(">>>Provisionee algo[%x], Provisioner algo[%x]\n"
					, prv_caps->algo, prv_start->algo);

	prv_start->pub_key = prv_caps->pub_key_type;
	vOutputString(">>>Provisionee pub_key_type[%x], Provisioner pub_key_type[%x]\n"
					, prv_caps->pub_key_type, prv_start->pub_key);
	if(prv_start->pub_key){
		//vOutputString(">>>Enter Provisionee's public key:\n");
		//char str[64+64+4];
		//read_str(str, sizeof(str)-1);
		//char* pstr = str;
		//get_bytearray_from_string(&pstr, prv_start->pub_key_data, 64);
		
		const u8_t prvee_key[] = {
			0xF4,0x65,0xE4,0x3F,0xF2,0x3D,0x3F,0x1B,0x9D,0xC7,0xDF,0xC0,0x4D,0xA8,0x75,0x81,
			0x84,0xDB,0xC9,0x66,0x20,0x47,0x96,0xEC,0xCF,0x0D,0x6C,0xF5,0xE1,0x65,0x00,0xCC,
			0x02,0x01,0xD0,0x48,0xBC,0xBB,0xD8,0x99,0xEE,0xEF,0xC4,0x24,0x16,0x4E,0x33,0xC2,
			0x01,0xC2,0xB0,0x10,0xCA,0x6B,0x4D,0x43,0xA8,0xA1,0x55,0xCA,0xD8,0xEC,0xB2,0x79
			};
		memcpy(&prv_start->pub_key_data, prvee_key, sizeof(prv_start->pub_key_data));
		vOutputString(">>>Recved Provisionee's public key:%s\n"
					, bt_hex(prv_start->pub_key_data, sizeof(prv_start->pub_key_data)));
	}

	vOutputString(">>>Provisionee stc_oob_type: %x\n", prv_caps->stc_oob_type);
	if(prv_caps->stc_oob_type){
		prv_start->auth_method = 0x01;
		prv_start->auth_action = 0;
		prv_start->auth_size = 0;
		return 0;
	}
	if(prv_caps->op_oob_size == 0
		&& prv_caps->ip_oob_size == 0){
		prv_start->auth_method = 0x00;
		prv_start->auth_action = 0;
		prv_start->auth_size = 0;
		return 0;
	}
	
	if(prv_caps->op_oob_size){
		vOutputString(">>>Provisionee op_oob_size: %x\n", prv_caps->op_oob_size);
		vOutputString(">>>Provisionee op_oob_action: %x\n", prv_caps->op_oob_action);
		vOutputString(">>>Enter a number within:");
		int cnt = 0, index = 0;
		for(int i = 0; i < 5; ++i){
			if(prv_caps->op_oob_action&(1<<i)){
				vOutputString("[%d %s] ", i, output_oob_action[i]);
				cnt++;
				index = i;
			}
		}
		vOutputString(":\n");
		if(cnt > 1){
			char str[20];
			read_str(str, sizeof(str)-1);
			prv_start->auth_action = strtoul(str, NULL, 10);
		}
		else{
			prv_start->auth_action = index;
		}
		prv_start->auth_size = prv_caps->op_oob_size;
		prv_start->auth_method = 0x02;
	}

	if(prv_caps->ip_oob_size){
		vOutputString(">>>Provisionee ip_oob_size: %x\n", prv_caps->ip_oob_size);
		vOutputString(">>>Provisionee ip_oob_action: %x\n", prv_caps->ip_oob_action);
		vOutputString(">>>Provisionee Enter a number within:");
		int cnt = 0, index = 0;
		for(int i = 0; i < 4; ++i){
			if(prv_caps->ip_oob_action&(1<<i)){
				vOutputString("[%d %s] ", i, input_oob_action[i]);
				cnt++;
				index = i;
			}
		}
		vOutputString(":\n");
		if(cnt>1){
			char str[20];
			read_str(str, sizeof(str)-1);
			prv_start->auth_action = strtoul(str, NULL, 10);
		}else{
			prv_start->auth_action = index;
		}
		prv_start->auth_size = prv_caps->ip_oob_size;
		prv_start->auth_method = 0x03;
	}
	
	vOutputString(">>>Provisioner auth method[%x] action[%x] size[%x]:\n"
					, prv_start->auth_method, prv_start->auth_action
					, prv_start->auth_size);
	return 0;
}
#endif /*CONFIG_BT_MESH_PROVISIONER*/
static int output_number(bt_mesh_output_action_t action, u32_t number)
{
	vOutputString("OOB Number: %u\r\n", number);
	return 0;
}

static int output_string(const char *str)
{
	vOutputString("OOB String: %s\r\n", str);
	return 0;
}

static void prov_input_complete(void)
{
	vOutputString("Input complete\r\n");
}

static void prov_complete(u16_t net_idx, u16_t addr)
{
	vOutputString("Local node provisioned, net_idx 0x%04x address 0x%04x\r\n", net_idx, addr);
	net.net_idx = net_idx,
	net.local = addr;
	net.dst = addr;
}

static void prov_reset(void)
{
	vOutputString("The local node has been reset and needs reprovisioning\r\n");
}

static void gen_dev_uuid(void)
{
//device uuid: 07af0000-1111-2222-3333-mac address(6 bytes)

	bt_addr_le_t adv_addr;
	bt_get_local_public_address(&adv_addr);
	u8_t uuid[16] = {0x07,0xaf,0x00,0x00,0x11,0x11,0x22,0x22,0x33,0x33,
	                        adv_addr.a.val[5],
	                        adv_addr.a.val[4],
	                        adv_addr.a.val[3],
	                        adv_addr.a.val[2],
	                        adv_addr.a.val[1],
	                        adv_addr.a.val[0]};
	memcpy(dev_uuid, uuid, 16);

}

static void blemeshcli_set_dev_uuid(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    vOutputString("device uuid is %s\r\n", argv[1]);
    get_bytearray_from_string(&argv[1], dev_uuid,16);
}

static void blemeshcli_input_num(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err;
    uint32_t num;

    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
     
    num = strtoul(argv[1], NULL, 10);

	if (input_act != BT_MESH_ENTER_NUMBER) {
		vOutputString("A number hasn't been requested!\r\n");
		return;
	}

	if (strlen(argv[1]) < input_size) {
		vOutputString("Too short input (%u digits required)\r\n", input_size);
		return;
	}

	err = bt_mesh_input_number(num);
	if (err) {
		vOutputString("Numeric input failed (err %d)\r\n", err);
		return;
	}

	input_act = BT_MESH_NO_INPUT;
}

static void blemeshcli_input_str(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err;

    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

	if (input_act != BT_MESH_ENTER_STRING) {
		vOutputString("A string hasn't been requested!\r\n");
		return;
	}

	if (strlen(argv[1]) < input_size) {
		vOutputString("Too short input (%u characters required)\r\n", input_size);
		return;
	}

	err = bt_mesh_input_string(argv[1]);
	if (err) {
		vOutputString("String input failed (err %d)\r\n", err);
		return;
	}

	input_act = BT_MESH_NO_INPUT;
}

#if defined(CONFIG_BT_MESH_MODEL)
#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
void ble_mesh_generic_onoff_client_model_cb(bfl_ble_mesh_generic_client_cb_event_t event,
        bfl_ble_mesh_generic_client_cb_param_t *param)
{
    uint32_t opcode = param->params->opcode;

    //vOutputString("enter %s: event is %d, error code is %d, opcode is 0x%x\n",
    //         __func__, event, param->error_code, opcode);

    switch (event) {
    case BFL_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT: {
        switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_GET:
            if (param->error_code == BFL_OK) {
                vOutputString("GenOnOffClient:GetStatus,OK[%x]\n", param->status_cb.onoff_status.present_onoff);
            } else {
                vOutputString("GenOnOffClient:GetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BFL_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT: {
        switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
            if (param->error_code == BFL_OK) {
				/* Add protection for packet loss rate test*/
				unsigned int key = irq_lock();
                vOutputString("GenOnOffClient:SetStatus,OK[%x]\n", param->status_cb.onoff_status.present_onoff);
				irq_unlock(key);
            } else {
                vOutputString("GenOnOffClient:SetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK:
            if (param->error_code == BFL_OK) {
                vOutputString("GenOnOffClient:SetUNACK,OK, opcode[%x] raddr[%x]\n", 
                                opcode, param->params->ctx.addr);
            } else {
                vOutputString("GenOnOffClient:SetUNACK,Fail[%x]\n", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BFL_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT: {
        if (param->error_code == BFL_OK) {
            switch (opcode) {
                case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS:
                    vOutputString("Recv onoff status, raddr[%x]\n", 
                                        param->params->ctx.addr);
					#if defined(CONFIG_BT_MESH_TEST)
                    if(nodelist_check(param->params->ctx.addr)){
                        vOutputString("Recv all response\r\n");
                    }
					#endif /* CONFIG_BT_MESH_TEST */
                break;
                }
        } else {
            vOutputString("GenOnOffClient:Publish,Fail[%x]\n", param->error_code);
        }
        break;
    }
    case BFL_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        vOutputString("GenOnOffClient:TimeOut[%x]\n", param->error_code);
        break;
    case BFL_BLE_MESH_GENERIC_CLIENT_EVT_MAX:
        vOutputString("GenONOFFClient:InvalidEvt[%x]\n", param->error_code);
        break;
    default:
        break;
    }
    //vOutputString("exit %s \n", __func__);
}


static void blemeshcli_gen_oo_cli(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err = BFL_OK;

    if(argc < 1){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    bfl_ble_mesh_generic_client_set_state_t gen_client_set;
    bfl_ble_mesh_generic_client_get_state_t gen_client_get;
    bfl_ble_mesh_client_common_param_t onoff_common = {
        .msg_timeout = 0,
        .ctx.send_ttl = 7,
        .model = NULL,
    };

    vOutputString("enter %s\n", __func__);

	/*Get client model*/
	for(int i = 0; i < sizeof(sig_models)/sizeof(struct bt_mesh_model); ++i){
		if(sig_models[i].id == BFL_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI){
			 onoff_common.model = &sig_models[i];
		}
	}
	if(onoff_common.model == NULL){
		vOutputString("Can't find gen onoff cli %s\n", __func__);
		return;
	}

	if(argc == 7 || argc == 12){
	    /* appkey_index */
	    get_uint16_from_string(&argv[2], &onoff_common.ctx.app_idx);
	    /* opcode */
	    get_uint32_from_string(&argv[3], &onoff_common.opcode);
	    /* role */
	    get_uint8_from_string(&argv[4], &onoff_common.msg_role);
	    /* address */
	    get_uint16_from_string(&argv[5], &onoff_common.ctx.addr);
	    /* network key index */
	    get_uint16_from_string(&argv[6], &onoff_common.ctx.net_idx);

		if(argc == 12){
			/* op_en */
			uint8_t op_en_t;
			get_uint8_from_string(&argv[7], &op_en_t);
			gen_client_set.onoff_set.op_en = op_en_t?true:false;

			/* onoff */
			get_uint8_from_string(&argv[8], &gen_client_set.onoff_set.onoff);
			/* tid */
			get_uint8_from_string(&argv[9], &gen_client_set.onoff_set.tid);
			/* trans_time */
			get_uint8_from_string(&argv[10], &gen_client_set.onoff_set.trans_time);
			/* delay */
			get_uint8_from_string(&argv[11], &gen_client_set.onoff_set.delay);
		}
	}
	else{
		if(argc != 2){
			vOutputString("Number of Parameters is not correct\r\n");
			return;
		}
	}

    if (strcmp(argv[1], "get") == 0) {
        err = bfl_ble_mesh_generic_client_get_state(&onoff_common, &gen_client_get);
    } else if (strcmp(argv[1], "set") == 0) {
		/* Add protection for packet loss rate test*/
		unsigned int key = irq_lock();
		vOutputString("%sSetStart\n", __func__);
		irq_unlock(key);

#if defined(CONFIG_BT_MESH_TEST)
        if(onoff_common.opcode == BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET
            && BT_MESH_ADDR_IS_GROUP(onoff_common.ctx.addr)){
            nodelist_check_clear();
        }
#endif /* CONFIG_BT_MESH_TEST */
        err = bfl_ble_mesh_generic_client_set_state(&onoff_common, &gen_client_set);
    } else if (strcmp(argv[1], "reg") == 0) {
        err = bfl_ble_mesh_register_generic_client_callback(ble_mesh_generic_onoff_client_model_cb);
        if (err == BFL_OK) {
            vOutputString("GenONOFFClient:Reg,OK\n");
        }
    }
    vOutputString("exit %s\n", __func__);
}
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
void ble_mesh_light_client_model_cb(bfl_ble_mesh_light_client_cb_event_t event,
        bfl_ble_mesh_light_client_cb_param_t *param)
{
	uint32_t opcode = param->params->opcode;

    vOutputString("enter %s: event is %d, error code is %d, opcode is 0x%x\n",
             __func__, event, param->error_code, opcode);

    switch (event) {
    case BFL_BLE_MESH_LIGHT_CLIENT_GET_STATE_EVT: {
        switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light lightness Client:GetStatus,OK, ln[%x]ln_t[%x]\n"
								, param->status_cb.lightness_status.present_lightness
								, param->status_cb.lightness_status.target_lightness);
            } else {
                vOutputString("Light lightness Client:GetStatus,Fail[%x]\n", param->error_code);
            }
            break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_GET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light ctl Client:GetStatus,OK,ln[%x]ln_t[%x] tp[%x]tp_t[%x]\n",
					param->status_cb.ctl_status.present_ctl_lightness,
					param->status_cb.ctl_status.target_ctl_lightness,
					param->status_cb.ctl_status.present_ctl_temperature,
					param->status_cb.ctl_status.target_ctl_temperature);
            } else {
                vOutputString("Light ctl Client:GetStatus,Fail[%x]\n", param->error_code);
            }
            break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_GET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light hsl Client:GetStatus,OK,l[%x] h[%x] s[%x]\n",
					param->status_cb.hsl_status.hsl_lightness,
					param->status_cb.hsl_status.hsl_hue,
					param->status_cb.hsl_status.hsl_saturation);
            } else {
                vOutputString("Light hsl Client:GetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BFL_BLE_MESH_LIGHT_CLIENT_SET_STATE_EVT: {
        switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light lightness Client:SetStatus,OK, ln[%x]ln_t[%x]\n"
								, param->status_cb.lightness_status.present_lightness
								, param->status_cb.lightness_status.target_lightness);
            } else {
                vOutputString("Light lightness Client:SetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK:
            if (param->error_code == BFL_OK) {
                vOutputString("Light lightness Client:SetUNACK,OK\n");
            } else {
                vOutputString("Light lightness Client:SetUNACK,Fail[%x]\n", param->error_code);
            }
            break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light ctl Client:SetStatus,OK, ln[%x]ln_t[%x] tp[%x]tp_t[%x]\n", 
					param->status_cb.ctl_status.present_ctl_lightness,
					param->status_cb.ctl_status.target_ctl_lightness,
					param->status_cb.ctl_status.present_ctl_temperature,
					param->status_cb.ctl_status.target_ctl_temperature);
            } else {
                vOutputString("Light ctl Client:SetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK:
            if (param->error_code == BFL_OK) {
                vOutputString("Light ctl Client:SetUNACK,OK\n");
            } else {
                vOutputString("Light ctl Client:SetUNACK,Fail[%x]\n", param->error_code);
            }
            break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light hsl Client:SetStatus,OK, l[%x] h[%x] s[%x]\n", 
					param->status_cb.hsl_status.hsl_lightness,
					param->status_cb.hsl_status.hsl_hue,
					param->status_cb.hsl_status.hsl_saturation);
            } else {
                vOutputString("Light ctl Client:SetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK:
            if (param->error_code == BFL_OK) {
                vOutputString("Light hsl Client:SetUNACK,OK\n");
            } else {
                vOutputString("Light hsl Client:SetUNACK,Fail[%x]\n", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BFL_BLE_MESH_LIGHT_CLIENT_PUBLISH_EVT: {
		switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
	        if (param->error_code == BFL_OK) {
	            vOutputString("Light lightness Client:Publish,OK\n");
	        } else {
	            vOutputString("Light lightness Client:Publish,Fail[%x]\n", param->error_code);
	        }
			break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_GET:
	        if (param->error_code == BFL_OK) {
	            vOutputString("Light ctl Client:Publish,OK\n");
	        } else {
	            vOutputString("Light ctl Client:Publish,Fail[%x]\n", param->error_code);
	        }
			break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_GET:
	        if (param->error_code == BFL_OK) {
	            vOutputString("Light hsl Client:Publish,OK\n");
	        } else {
	            vOutputString("Light hsl Client:Publish,Fail[%x]\n", param->error_code);
	        }
			break;
		default:
        	break;
		}
		break;
    }
    case BFL_BLE_MESH_LIGHT_CLIENT_TIMEOUT_EVT:{
		switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK:
        	vOutputString("Light lightness Client:TimeOut[%x]\n", param->error_code);
			break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_GET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK:
        	vOutputString("Light ctl Client:TimeOut[%x]\n", param->error_code);
			break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_GET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK:
        	vOutputString("Light hsl Client:TimeOut[%x]\n", param->error_code);
			break;
		default:
			break;
		}
        break;
    }
    case BFL_BLE_MESH_LIGHT_CLIENT_EVT_MAX:{
        vOutputString("InvalidEvt, Opcode[%x] [%x]\n", opcode, param->error_code);
        break;
    }
    default:
        break;
    }
    vOutputString("exit %s \n", __func__);
}

static void blemeshcli_light_lgn_cli(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err = BFL_OK;

    if(argc < 1){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    bfl_ble_mesh_light_client_set_state_t light_client_set;
    bfl_ble_mesh_light_client_get_state_t light_client_get;
    bfl_ble_mesh_client_common_param_t lgn_common = {
        .msg_timeout = 0,
        .ctx.send_ttl = 7,
        .model = NULL,
    };

    vOutputString("enter %s\n", __func__);

	/*Get client model*/
	for(int i = 0; i < sizeof(sig_models)/sizeof(struct bt_mesh_model); ++i){
		if(sig_models[i].id == BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI){
			 lgn_common.model = &sig_models[i];
		}
	}
	if(lgn_common.model == NULL){
		vOutputString("Can't find light ligntness cli %s\n", __func__);
		return;
	}

	if(argc > 6){
	    /* appkey_index */
	    get_uint16_from_string(&argv[2], &lgn_common.ctx.app_idx);
	    /* opcode */
	    get_uint32_from_string(&argv[3], &lgn_common.opcode);
	    /* role */
	    get_uint8_from_string(&argv[4], &lgn_common.msg_role);
	    /* address */
	    get_uint16_from_string(&argv[5], &lgn_common.ctx.addr);
	    /* network key index */
	    get_uint16_from_string(&argv[6], &lgn_common.ctx.net_idx);
	}
	if(argc > 11){
	    /* op_en */
	    uint8_t op_en_t;
	    get_uint8_from_string(&argv[7], &op_en_t);
		light_client_set.lightness_set.op_en = op_en_t?true:false;

	    /* lightness */
	    get_uint16_from_string(&argv[8], &light_client_set.lightness_set.lightness);
	    /* tid */
	    get_uint8_from_string(&argv[9], &light_client_set.lightness_set.tid);
	    /* trans_time */
	    get_uint8_from_string(&argv[10], &light_client_set.lightness_set.trans_time);
	    /* delay */
	    get_uint8_from_string(&argv[11], &light_client_set.lightness_set.delay);
	}

    if (strcmp(argv[1], "get") == 0) {
        err = bfl_ble_mesh_light_client_get_state(&lgn_common, &light_client_get);
    } else if (strcmp(argv[1], "set") == 0) {
        err = bfl_ble_mesh_light_client_set_state(&lgn_common, &light_client_set);
    } else if (strcmp(argv[1], "reg") == 0) {
        err = bfl_ble_mesh_register_light_client_callback(ble_mesh_light_client_model_cb);
        if (err == BFL_OK) {
            vOutputString("Light lightness client:Reg,OK\n");
        }
    }
    vOutputString("exit %s\n", __func__);
}

static void blemeshcli_light_ctl_cli(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err = BFL_OK;

	if(argc < 1){
		vOutputString("Number of Parameters is not correct\r\n");
		return;
	}

	bfl_ble_mesh_light_client_set_state_t light_client_set;
	bfl_ble_mesh_light_client_get_state_t light_client_get;
	bfl_ble_mesh_client_common_param_t ctl_common = {
		.msg_timeout = 0,
		.ctx.send_ttl = 7,
		.model = NULL,
	};

	vOutputString("enter %s\n", __func__);

	/*Get client model*/
	for(int i = 0; i < sizeof(sig_models)/sizeof(struct bt_mesh_model); ++i){
		if(sig_models[i].id == BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI){
			 ctl_common.model = &sig_models[i];
		}
	}
	if(ctl_common.model == NULL){
		vOutputString("Can't find light ctl cli %s\n", __func__);
		return;
	}

	if(argc > 6){
		/* appkey_index */
		get_uint16_from_string(&argv[2], &ctl_common.ctx.app_idx);
		/* opcode */
		get_uint32_from_string(&argv[3], &ctl_common.opcode);
		/* role */
		get_uint8_from_string(&argv[4], &ctl_common.msg_role);
		/* address */
		get_uint16_from_string(&argv[5], &ctl_common.ctx.addr);
		/* network key index */
		get_uint16_from_string(&argv[6], &ctl_common.ctx.net_idx);
	}
	if(argc > 13){
		/* op_en */
		uint8_t op_en_t;
		get_uint8_from_string(&argv[7], &op_en_t);
		light_client_set.ctl_set.op_en = op_en_t?true:false;

		/* onoff */
		get_uint16_from_string(&argv[8], &light_client_set.ctl_set.ctl_lightness);

		get_uint16_from_string(&argv[9], &light_client_set.ctl_set.ctl_temperatrue);

		get_uint16_from_string(&argv[10], (uint16_t*)&light_client_set.ctl_set.ctl_delta_uv);
		/* tid */
		get_uint8_from_string(&argv[11], &light_client_set.ctl_set.tid);
		/* trans_time */
		get_uint8_from_string(&argv[12], &light_client_set.ctl_set.trans_time);
		/* delay */
		get_uint8_from_string(&argv[13], &light_client_set.ctl_set.delay);
	}

	if (strcmp(argv[1], "get") == 0) {
		err = bfl_ble_mesh_light_client_get_state(&ctl_common, &light_client_get);
	} else if (strcmp(argv[1], "set") == 0) {
		err = bfl_ble_mesh_light_client_set_state(&ctl_common, &light_client_set);
	} else if (strcmp(argv[1], "reg") == 0) {
		err = bfl_ble_mesh_register_light_client_callback(ble_mesh_light_client_model_cb);
		if (err == BFL_OK) {
			vOutputString("Light lightness client:Reg,OK\n");
		}
	}
	vOutputString("exit %s\n", __func__);
}

static void blemeshcli_light_hsl_cli(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err = BFL_OK;

	if(argc < 1){
		vOutputString("Number of Parameters is not correct\n");
		return;
	}

	bfl_ble_mesh_light_client_set_state_t light_client_set;
	bfl_ble_mesh_light_client_get_state_t light_client_get;
	bfl_ble_mesh_client_common_param_t hsl_common = {
		.msg_timeout = 0,
		.ctx.send_ttl = 7,
		.model = NULL,
	};

	vOutputString("enter %s\n", __func__);

	/*Get client model*/
	for(int i = 0; i < sizeof(sig_models)/sizeof(struct bt_mesh_model); ++i){
		if(sig_models[i].id == BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI){
			 hsl_common.model = &sig_models[i];
		}
	}
	if(hsl_common.model == NULL){
		vOutputString("Can't find light ligntness cli %s\n", __func__);
		return;
	}

	if(argc > 6){
		/* appkey_index */
		get_uint16_from_string(&argv[2], &hsl_common.ctx.app_idx);
		/* opcode */
		get_uint32_from_string(&argv[3], &hsl_common.opcode);
		/* role */
		get_uint8_from_string(&argv[4], &hsl_common.msg_role);
		/* address */
		get_uint16_from_string(&argv[5], &hsl_common.ctx.addr);
		/* network key index */
		get_uint16_from_string(&argv[6], &hsl_common.ctx.net_idx);
	}
	if(argc > 13){
		/* op_en */
		uint8_t op_en_t;
		get_uint8_from_string(&argv[7], &op_en_t);
		light_client_set.hsl_set.op_en = op_en_t?true:false;

		/* onoff */
		get_uint16_from_string(&argv[8], &light_client_set.hsl_set.hsl_lightness);
		get_uint16_from_string(&argv[9], &light_client_set.hsl_set.hsl_hue);
		get_uint16_from_string(&argv[10], &light_client_set.hsl_set.hsl_saturation);
		/* tid */
		get_uint8_from_string(&argv[11], &light_client_set.hsl_set.tid);
		/* trans_time */
		get_uint8_from_string(&argv[12], &light_client_set.hsl_set.trans_time);
		/* delay */
		get_uint8_from_string(&argv[13], &light_client_set.hsl_set.delay);
	}

	if (strcmp(argv[1], "get") == 0) {
		err = bfl_ble_mesh_light_client_get_state(&hsl_common, &light_client_get);
	} else if (strcmp(argv[1], "set") == 0) {
		err = bfl_ble_mesh_light_client_set_state(&hsl_common, &light_client_set);
	} else if (strcmp(argv[1], "reg") == 0) {
		err = bfl_ble_mesh_register_light_client_callback(ble_mesh_light_client_model_cb);
		if (err == BFL_OK) {
			vOutputString("Light lightness client:Reg,OK\n");
		}
	}
	vOutputString("exit %s\n", __func__);
}
#endif
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
static void blemeshcli_vendor_cli(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    u16_t id;
    struct bt_mesh_msg_ctx ctx = {.send_ttl = 3};

    if(argc != 6 && argc != 7){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    vOutputString("enter %s\n", __func__);

    BT_MESH_MODEL_BUF_DEFINE(msg, BT_MESH_MODEL_OP_3(0x0000, BL_COMP_ID), 40);

    get_uint16_from_string(&argv[1], &ctx.app_idx);
    get_uint16_from_string(&argv[2], &id);
    get_uint16_from_string(&argv[4], &ctx.addr);
    get_uint16_from_string(&argv[5], &ctx.net_idx);

    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_3(id, BL_COMP_ID));
    
    if(argc == 7){
        uint8_t len = strlen(argv[6])>>1;
        uint8_t val[40];
        get_bytearray_from_string(&argv[6], val, len);
        net_buf_simple_add_mem(&msg, val, len);
        vOutputString("send data %s\r\n", bt_hex(msg.data, msg.len));
    }

    

    struct bt_mesh_model* model_t;
    model_t = bt_mesh_model_find_vnd(elements, BL_COMP_ID, BT_MESH_VND_MODEL_ID_DATA_CLI);
    if(model_t == NULL){
        BT_ERR("Unable to found vendor model");
    }

    if (bt_mesh_model_send(model_t, &ctx, &msg, NULL, NULL)){
		BT_ERR("Unable to send vendor cli command");
	}
}
#endif /* CONFIG_BT_MESH_MODEL_VENDOR_CLI */
#endif /* CONFIG_BT_MESH_MODEL */

static int input(bt_mesh_input_action_t act, u8_t size)
{
	char str[20];
	switch (act) {
	case BT_MESH_ENTER_NUMBER:{
		vOutputString("Enter a number (max %u digits) with: input-num <num>:", size);
		read_str(str, sizeof(str)-1);
		u32_t num = strtoul(str, NULL, 10);
		vOutputString("Recved num[%d]\n", num);
		int err = bt_mesh_input_number(num);
		if (err) {
			vOutputString("Numeric input failed (err %d)\r\n", err);
		}
	}break;
	case BT_MESH_ENTER_STRING:{
		vOutputString("Enter a string (max %u chars) with: input-str <str>\r\n", size);
		read_str(str, sizeof(str)-1);
		vOutputString("Recved string[%d]\n", str);
		int err = bt_mesh_input_string(str);
		if (err) {
			vOutputString("String input failed (err %d)\r\n", err);
		}
	}break;
	default:
		vOutputString("Unknown input action %u (size %u) requested!\r\n", act, size);
		return -EINVAL;
	}

	input_act = act;
	input_size = size;
	return 0;
}

static void blemeshcli_reset(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	if(!blemesh_inited){
		vOutputString("blemesh not init\n");
		return;
	}
	bt_mesh_reset();
	vOutputString("Local node reset complete\r\n");
#if defined(CONFIG_BT_MESH_PROVISIONER)
	bt_mesh_cdb_clear();
	vOutputString("Cleared CDB\n");
#endif
}

static void blemeshcli_net_send(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t ttl;
    uint8_t ctl;
    uint16_t src;
    uint16_t dst;
    uint8_t payload[16] = {0x00};
    
    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    if (!bt_mesh_is_provisioned()) {
        vOutputString("Local node is not yet provisioned\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &ttl);
    get_uint8_from_string(&argv[2], &ctl);
    get_uint16_from_string(&argv[3], &src);
    get_uint16_from_string(&argv[4], &dst);
    
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net.net_idx,
        .app_idx = ctl ? BT_MESH_KEY_UNUSED : BT_MESH_KEY_DEV,
        .addr = dst,
        .send_rel = 0,
        .send_ttl = ttl,
    };
    
    struct bt_mesh_net_tx tx = {
        .sub = bt_mesh_subnet_get(ctx.net_idx),
        .ctx = &ctx,
        .src = src,
        .xmit = bt_mesh_net_transmit_get(),
        .friend_cred = 0,
    };
    
    struct net_buf *buf = bt_mesh_adv_create(BT_MESH_ADV_DATA, tx.xmit, K_NO_WAIT);
    if (!buf) {
        vOutputString("Out of network buffers\r\n");
        return;
    }
    
    vOutputString("Sending network packet\r\n");
    
    net_buf_reserve(buf, BT_MESH_NET_HDR_LEN);
    net_buf_add_mem(buf, payload, sizeof(payload));
    
    bt_mesh_net_send(&tx, buf, NULL, NULL);
}

static uint16_t get_app_idx(void)
{
    int i;
    
    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];
        
        if (key->net_idx != BT_MESH_KEY_UNUSED) {
            return key->app_idx;
        }
    }
    
    return BT_MESH_KEY_UNUSED;
}

static void blemeshcli_seg_send(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    NET_BUF_SIMPLE_DEFINE(sdu, BT_MESH_TX_SDU_MAX);
    uint16_t src;
    uint16_t dst;
    uint8_t payload[12] = {0xFF};
    
    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    if (!bt_mesh_is_provisioned()) {
        vOutputString("Local node is not yet provisioned\r\n");
        return;
    }
    
    get_uint16_from_string(&argv[1], &src);
    get_uint16_from_string(&argv[2], &dst);
    
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net.net_idx,
        .app_idx = get_app_idx(),
        .addr = dst,
        .send_rel = 1,
        .send_ttl = 0,
    };
    
    struct bt_mesh_net_tx tx = {
        .sub = bt_mesh_subnet_get(ctx.net_idx),
        .ctx = &ctx,
        .src = src,
        .xmit = bt_mesh_net_transmit_get(),
        .friend_cred = 0,
    };
    
    vOutputString("Sending segmented message\r\n");
    
    net_buf_simple_add_mem(&sdu, payload, sizeof(payload));
    
    bt_mesh_trans_send(&tx, &sdu, NULL, NULL);
}

static void blemeshcli_rpl_clr(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
#if defined(CONFIG_BT_SETTINGS)

	bt_mesh_clear_rpl();

	vOutputString("Replay protection list (size: %d) cleared\r\n", CONFIG_BT_MESH_CRPL);
#endif
    
    memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));
}

static void blemeshcli_ivu_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t enable;
    
    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &enable);
    
#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
    bt_mesh_iv_update_test(enable);
#endif
    
    if (enable) {
        vOutputString("IV Update test mode enabled\r\n");
    } else {
        vOutputString("IV Update test mode disabled\r\n");
    }
}

static void blemeshcli_iv_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t enable;
    
    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    if (!bt_mesh_is_provisioned()) {
        vOutputString("Local node is not yet provisioned\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &enable);
    
    if (enable) {
        vOutputString("IV Update procedure started\r\n");
        
#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
        bt_mesh_iv_update();
#endif
        
        if (!atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR)) {
            bt_mesh_beacon_ivu_initiator(1);
        }
    } else {
        vOutputString("IV Update procedure stopped\r\n");
        
        if (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR)) {
            bt_mesh_beacon_ivu_initiator(0);
        }
    }
}

static void blemeshcli_fault_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t type;
    int i;
    
    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &type);
    
    if (type == 0) {
        if(strlen(argv[2])/2 >= sizeof(cur_faults)) {
            get_bytearray_from_string(&argv[2], cur_faults,sizeof(cur_faults));
        } else {
            memset(cur_faults, 0x00, sizeof(cur_faults));
            get_bytearray_from_string(&argv[2], cur_faults,strlen(argv[2])/2);
        }
        
        vOutputString("Current Fault: ");
        for(i=0; i<sizeof(cur_faults); i++) {
            vOutputString("%02X", cur_faults[i]);
        }
        vOutputString("\r\n");
    } else {
        if(strlen(argv[2])/2 >= sizeof(reg_faults)) {
            get_bytearray_from_string(&argv[2], reg_faults,sizeof(reg_faults));
        } else {
            memset(reg_faults, 0x00, sizeof(reg_faults));
            get_bytearray_from_string(&argv[2], reg_faults,strlen(argv[2])/2);
        }
        
        vOutputString("Registered Fault: ");
        for(i=0; i<sizeof(reg_faults); i++) {
            vOutputString("%02X", reg_faults[i]);
        }
        vOutputString("\r\n");
    }
}

static void get_faults(u8_t *faults, u8_t faults_size, u8_t *dst, u8_t *count)
{
	u8_t i, limit = *count;

	for (i = 0U, *count = 0U; i < faults_size && *count < limit; i++) {
		if (faults[i]) {
			*dst++ = faults[i];
			(*count)++;
		}
	}
}

void show_faults(u8_t test_id, u16_t cid, u8_t *faults, size_t fault_count)
{
	size_t i;

	if (!fault_count) {
		vOutputString("Health Test ID 0x%02x Company ID 0x%04x: no faults\r\n", test_id, cid);
		return;
	}

	vOutputString("Health Test ID 0x%02x Company ID 0x%04x Fault Count %zu:\r\n", test_id, cid, fault_count);

	for (i = 0; i < fault_count; i++) {
		vOutputString("\t0x%02x", faults[i]);
	}
}

static void health_current_status(struct bt_mesh_health_cli *cli, u16_t addr,
				  u8_t test_id, u16_t cid, u8_t *faults,
				  size_t fault_count)
{
	vOutputString("Health Current Status from 0x%04x\r\n", addr);
	show_faults(test_id, cid, faults, fault_count);
}

static int fault_get_cur(struct bt_mesh_model *model, u8_t *test_id,
			 u16_t *company_id, u8_t *faults, u8_t *fault_count)
{
	vOutputString("Sending current faults\r\n");

	*test_id = 0x00;
	*company_id = BT_COMP_ID_LF;

	get_faults(cur_faults, sizeof(cur_faults), faults, fault_count);

	return 0;
}

static int fault_get_reg(struct bt_mesh_model *model, u16_t cid,
			 u8_t *test_id, u8_t *faults, u8_t *fault_count)
{
	if (cid != BT_COMP_ID_LF) {
		vOutputString("Faults requested for unknown Company ID 0x%04x\r\n", cid);
		return -EINVAL;
	}

	vOutputString("Sending registered faults\r\n");

	*test_id = 0x00;

	get_faults(reg_faults, sizeof(reg_faults), faults, fault_count);

	return 0;
}

static int fault_clear(struct bt_mesh_model *model, uint16_t cid)
{
	if (cid != BT_COMP_ID_LF) {
		return -EINVAL;
	}

	(void)memset(reg_faults, 0, sizeof(reg_faults));

	return 0;
}

static int fault_test(struct bt_mesh_model *model, uint8_t test_id, uint16_t cid)
{
	if (cid != BT_COMP_ID_LF) {
		return -EINVAL;
	}

	if (test_id != 0x00) {
		return -EINVAL;
	}

	return 0;
}

static void attn_on(struct bt_mesh_model *model)
{
#ifdef CONFIG_BT_MESH_PTS
	vOutputString("[PTS] Attention timer on\r\n");
#endif
}

static void attn_off(struct bt_mesh_model *model)
{
#ifdef CONFIG_BT_MESH_PTS
	vOutputString("[PTS] Attention timer off\r\n");
#endif
}

#if defined(CONFIG_BT_MESH_GATT_PROXY)
static void __attribute__((unused)) blemeshcli_ident(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err;

	err = bt_mesh_proxy_identity_enable();
	if (err) {
		vOutputString("Failed advertise using Node Identity (err ""%d)\r\n", err);
	}
}
#endif /* MESH_GATT_PROXY */

#if defined(CONFIG_BT_MESH_CDB)
static void blemeshcli_cdb_create(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t net_key[16];
	size_t len;
	int err;

	if (argc < 2) {
		bt_rand(net_key, 16);
	} else {
		len = hex2bin(argv[1], strlen(argv[1]), net_key,
			      sizeof(net_key));
		memset(net_key + len, 0, sizeof(net_key) - len);
	}

	err = bt_mesh_cdb_create(net_key);
	if (err < 0) {
		vOutputString("Failed to create CDB (err %d)\n", err);
	}
	vOutputString("Net key:%s\n", bt_hex(net_key, 16));
}

static void blemeshcli_cdb_clear(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	if(!blemesh_inited){
		vOutputString("blemesh not init\n");
		return;
	}
	bt_mesh_cdb_clear();

	vOutputString("Cleared CDB\n");
}

static void cdb_print_nodes(void)
{
	char key_hex_str[32 + 1], uuid_hex_str[32 + 1];
	struct bt_mesh_cdb_node *node;
	int i, total = 0;
	bool configured;

	vOutputString("Address  Elements  Flags  %-32s  DevKey\n", "UUID");

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes); ++i) {
		node = &bt_mesh_cdb.nodes[i];
		if (node->addr == BT_MESH_ADDR_UNASSIGNED) {
			continue;
		}

		configured = atomic_test_bit(node->flags,
					     BT_MESH_CDB_NODE_CONFIGURED);

		total++;
		bin2hex(node->uuid, 16, uuid_hex_str, sizeof(uuid_hex_str));
		bin2hex(node->dev_key, 16, key_hex_str, sizeof(key_hex_str));
		vOutputString("0x%04x   %-8d  %-5s  %s  %s\n", node->addr,
			    node->num_elem, configured ? "C" : "-",
			    uuid_hex_str, key_hex_str);
	}

	vOutputString("> Total nodes: %d\n", total);
}

static void cdb_print_subnets(void)
{
	struct bt_mesh_cdb_subnet *subnet;
	char key_hex_str[32 + 1];
	int i, total = 0;

	vOutputString("NetIdx  NetKey\n");

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.subnets); ++i) {
		subnet = &bt_mesh_cdb.subnets[i];
		if (subnet->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		total++;
		bin2hex(subnet->keys[0].net_key, 16, key_hex_str,
			sizeof(key_hex_str));
		vOutputString("0x%03x   %s\n", subnet->net_idx,
			    key_hex_str);
	}

	vOutputString("> Total subnets: %d\n", total);
}

static void cdb_print_app_keys(void)
{
	struct bt_mesh_cdb_app_key *app_key;
	char key_hex_str[32 + 1];
	int i, total = 0;

	vOutputString("NetIdx  AppIdx  AppKey\n");

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.app_keys); ++i) {
		app_key = &bt_mesh_cdb.app_keys[i];
		if (app_key->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		total++;
		bin2hex(app_key->keys[0].app_key, 16, key_hex_str,
			sizeof(key_hex_str));
		vOutputString("0x%03x   0x%03x   %s\n",
			    app_key->net_idx, app_key->app_idx, key_hex_str);
	}

	vOutputString("> Total app-keys: %d\n", total);
}

static void blemeshcli_cdb_show(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	if (!atomic_test_bit(bt_mesh_cdb.flags, BT_MESH_CDB_VALID)) {
		vOutputString("No valid networks\n");
		return;
	}

	vOutputString("Mesh Network Information\n");
	vOutputString("========================\n");

	cdb_print_nodes();
	vOutputString("---\n");
	cdb_print_subnets();
	vOutputString("---\n");
	cdb_print_app_keys();
}

static void blemeshcli_cdb_node_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	struct bt_mesh_cdb_node *node;
	u8_t uuid[16], dev_key[16];
	u16_t addr, net_idx;
	u8_t num_elem;
	size_t len;

	len = hex2bin(argv[1], strlen(argv[1]), uuid, sizeof(uuid));
	memset(uuid + len, 0, sizeof(uuid) - len);

	addr = strtoul(argv[2], NULL, 0);
	num_elem = strtoul(argv[3], NULL, 0);
	net_idx = strtoul(argv[4], NULL, 0);

	if (argc < 6) {
		bt_rand(dev_key, 16);
	} else {
		len = hex2bin(argv[5], strlen(argv[5]), dev_key,
			      sizeof(dev_key));
		memset(dev_key + len, 0, sizeof(dev_key) - len);
	}

	node = bt_mesh_cdb_node_alloc(uuid, addr, num_elem, net_idx);
	if (node == NULL) {
		vOutputString("Failed to allocate node\n");
		return;
	}

	memcpy(node->dev_key, dev_key, 16);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_cdb_node(node);
	}

	vOutputString("Added node 0x%04x\n", addr);
}

static void blemeshcli_cdb_node_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	struct bt_mesh_cdb_node *node;
	u16_t addr;

	addr = strtoul(argv[1], NULL, 0);

	node = bt_mesh_cdb_node_get(addr);
	if (node == NULL) {
		vOutputString("No node with address 0x%04x\n", addr);
		return;
	}

	bt_mesh_cdb_node_del(node, true);

	vOutputString("Deleted node 0x%04x\n", addr);
}

static void blemeshcli_cdb_subnet_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	struct bt_mesh_cdb_subnet *sub;
	u8_t net_key[16];
	u16_t net_idx;
	size_t len;

	net_idx = strtoul(argv[1], NULL, 0);

	if (argc < 3) {
		bt_rand(net_key, 16);
	} else {
		len = hex2bin(argv[2], strlen(argv[2]), net_key,
			      sizeof(net_key));
		memset(net_key + len, 0, sizeof(net_key) - len);
	}

	sub = bt_mesh_cdb_subnet_alloc(net_idx);
	if (sub == NULL) {
		vOutputString("Could not add subnet\n");
		return;
	}

	memcpy(sub->keys[0].net_key, net_key, 16);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_cdb_subnet(sub);
	}

	vOutputString("Added Subnet 0x%03x\n", net_idx);
}

static void blemeshcli_cdb_subnet_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	struct bt_mesh_cdb_subnet *sub;
	u16_t net_idx;

	net_idx = strtoul(argv[1], NULL, 0);

	sub = bt_mesh_cdb_subnet_get(net_idx);
	if (sub == NULL) {
		vOutputString("No subnet with NetIdx 0x%03x\n", net_idx);
		return;
	}

	bt_mesh_cdb_subnet_del(sub, true);

	vOutputString("Deleted subnet 0x%03x\n", net_idx);
}

static void blemeshcli_cdb_app_key_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	struct bt_mesh_cdb_app_key *key;
	u16_t net_idx, app_idx;
	u8_t app_key[16];
	size_t len;

	net_idx = strtoul(argv[1], NULL, 0);
	app_idx = strtoul(argv[2], NULL, 0);

	if (argc < 4) {
		bt_rand(app_key, 16);
	} else {
		len = hex2bin(argv[3], strlen(argv[3]), app_key,
			      sizeof(app_key));
		memset(app_key + len, 0, sizeof(app_key) - len);
	}

	key = bt_mesh_cdb_app_key_alloc(net_idx, app_idx);
	if (key == NULL) {
		vOutputString("Could not add AppKey\n");
		return;
	}

	memcpy(key->keys[0].app_key, app_key, 16);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_cdb_app_key(key);
	}

	vOutputString("Added AppKey 0x%03x\n", app_idx);
}

static void blemeshcli_cdb_app_key_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	struct bt_mesh_cdb_app_key *key;
	u16_t app_idx;

	app_idx = strtoul(argv[1], NULL, 0);

	key = bt_mesh_cdb_app_key_get(app_idx);
	if (key == NULL) {
		vOutputString("No AppKey 0x%03x\n", app_idx);
		return;
	}

	bt_mesh_cdb_app_key_del(key, true);

	vOutputString("Deleted AppKey 0x%03x\n", app_idx);
}
#endif /* CONFIG_BT_MESH_CDB */

#if defined(CONFIG_BT_MESH_PROVISIONER)
static void print_unprovisioned_beacon(u8_t uuid[16],
				bt_mesh_prov_oob_info_t oob_info,
				u32_t *uri_hash)
{
	vOutputString("Received unprovisioned beacon info:\n");
	vOutputString("uuid:[%s]\n", bt_hex(uuid, 16));
	vOutputString("oob_info:[%x]\n", (u16_t)oob_info);
	if(uri_hash)
		vOutputString("uri_hash:[%x]\n", *uri_hash);
	else
		vOutputString("uri_hash:[NULL]\n");
}

static void print_node_added(u16_t net_idx, u8_t uuid[16], u16_t addr, u8_t num_elem)
{
	vOutputString("Node added:\n");
	vOutputString("net_idx:[%x]\n", net_idx);
	vOutputString("uuid:[%s]\n", bt_hex(uuid, 16));
	vOutputString("addr:[%x]\n", addr);
	vOutputString("num_elem:[%x]\n", num_elem);
	
	/** Added by bouffalolab, 
	 * clear message cache for node that has proved again.
	 **/
	bt_mesh_rx_reset_node(addr);
	
}

static void blemeshcli_beacon_listen(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t val;
	if(argc != 2){
		vOutputString("Number of Parameters is not correct\r\n");
		return;
	}

	get_uint8_from_string(&argv[1], &val);
	vOutputString("Beacon listen:[%x]\n", val);
	if (val) {
		prov.unprovisioned_beacon = print_unprovisioned_beacon;
	} else {
		prov.unprovisioned_beacon = NULL;
	}
	return;
}

/* Default net, app & dev key values, unless otherwise specified */
static const u8_t default_key[16] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};
static void blemeshcli_provision(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	const u8_t *net_key = default_key;
	u16_t net_idx, addr;
	u32_t iv_index;
	int err;

	if (argc < 3) {
		return;
	}

	net_idx = strtoul(argv[1], NULL, 0);
	addr = strtoul(argv[2], NULL, 0);

	if (argc > 3) {
		iv_index = strtoul(argv[3], NULL, 0);
	} else {
		iv_index = 0U;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		const struct bt_mesh_cdb_subnet *sub;

		sub = bt_mesh_cdb_subnet_get(net_idx);
		if (!sub) {
			vOutputString("No cdb entry for subnet 0x%03x\n",
				    net_idx);
			return;
		}

		net_key = sub->keys[sub->kr_flag].net_key;
	}

	err = bt_mesh_provision(net_key, net_idx, 0, iv_index, addr,
				default_key);
	if (err) {
		vOutputString("Provisioning failed (err %d)\n", err);
	}

	return;
}

static void configure_self(struct bt_mesh_cdb_node *self)
{
	struct bt_mesh_cdb_app_key *key;
	int err;

	vOutputString("Configuring self...\n");

	key = bt_mesh_cdb_app_key_get(app_idx);
	if (key == NULL) {
		vOutputString("No app-key 0x%04x\n", app_idx);
		return;
	}

	/* Add Application Key */
	err = bt_mesh_cfg_app_key_add(self->net_idx, self->addr, self->net_idx,
				      app_idx, key->keys[0].app_key, NULL);
	if (err < 0) {
		vOutputString("Failed to add app-key (err %d)\n", err);
		return;
	}

	#if 0
	/* Bind to Health model */
	err = bt_mesh_cfg_mod_app_bind(self->net_idx, self->addr, self->addr,
				       app_idx, BT_MESH_MODEL_ID_HEALTH_CLI,
				       NULL);
	if (err < 0) {
		vOutputString("Failed to bind app-key (err %d)\n", err);
		return;
	}
	#endif
	/* Bind to Generic onoff model */
	err = bt_mesh_cfg_mod_app_bind(self->net_idx, self->addr, self->addr,
				       app_idx, BT_MESH_MODEL_ID_GEN_ONOFF_CLI,
				       NULL);
	if (err < 0) {
		vOutputString("Failed to bind app-key (err %d)\n", err);
		return;
	}

	atomic_set_bit(self->flags, BT_MESH_CDB_NODE_CONFIGURED);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_node_store(self);
	}

	vOutputString("Configuration complete\n");
}

static void configure_node(struct bt_mesh_cdb_node *node)
{
	struct bt_mesh_cdb_app_key *key;
	struct bt_mesh_cfg_mod_pub pub;
	u8_t status;
	int err;

	vOutputString("Configuring node 0x%04x...\n", node->addr);

	key = bt_mesh_cdb_app_key_get(app_idx);
	if (key == NULL) {
		vOutputString("No app-key 0x%04x\n", app_idx);
		return;
	}

	/* Add Application Key */
	err = bt_mesh_cfg_app_key_add(net_idx, node->addr, net_idx, app_idx,
				      key->keys[0].app_key, NULL);
	if (err < 0) {
		vOutputString("Failed to add app-key (err %d)\n", err);
		return;
	}
	#if 0
	/* Bind to Health model */
	err = bt_mesh_cfg_mod_app_bind(net_idx, node->addr, node->addr, app_idx,
				       BT_MESH_MODEL_ID_HEALTH_SRV, NULL);
	if (err < 0) {
		vOutputString("Failed to bind app-key (err %d)\n", err);
		return;
	}
	#endif

	/* Bind to Health model */
	err = bt_mesh_cfg_mod_app_bind(net_idx, node->addr, node->addr, app_idx,
				       BT_MESH_MODEL_ID_GEN_ONOFF_SRV, NULL);
	if (err < 0) {
		vOutputString("Failed to bind app-key (err %d)\n", err);
		return;
	}

	pub.addr = 1;
	pub.app_idx = key->app_idx;
	pub.cred_flag = false;
	pub.ttl = 7;
	pub.period = BT_MESH_PUB_PERIOD_10SEC(1);
	pub.transmit = 0;

	err = bt_mesh_cfg_mod_pub_set(net_idx, node->addr, node->addr,
				      BT_MESH_MODEL_ID_HEALTH_SRV, &pub,
				      &status);
	if (err < 0) {
		vOutputString("mod_pub_set %d, %d\n", err, status);
		return;
	}

	atomic_set_bit(node->flags, BT_MESH_CDB_NODE_CONFIGURED);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_node_store(node);
	}

	vOutputString("Configuration complete\n");
}


static u8_t check_unconfigured(struct bt_mesh_cdb_node *node, void *data)
{
	if (!atomic_test_bit(node->flags, BT_MESH_CDB_NODE_CONFIGURED)) {
		if (node->addr == self_addr) {
			configure_self(node);
		} else {
			configure_node(node);
		}
	}

	return BT_MESH_CDB_ITER_CONTINUE;
}

/*CONFIG node*/
static void blemeshcli_node_cfg(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	if (argc < 1) {
		return;
	}
	
	bt_mesh_cdb_node_foreach(check_unconfigured, NULL);
	vOutputString("Node config ok\n");
}


#endif /* CONFIG_BT_MESH_PROVISIONER */

#if defined(CONFIG_BT_MESH_PROVISIONER)
static void blemeshcli_get_comp(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	if (argc != 4) {
		vOutputString("Number of Parameters is not correct\r\n");
		return;
	}
	
	NET_BUF_SIMPLE_DEFINE(comp_buf, BT_MESH_RX_SDU_MAX);
	u8_t status, page;
	u16_t net_idx, dst;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint8_from_string(&argv[3], &page);

	int err = bt_mesh_cfg_comp_data_get(net_idx, dst, page,
					&status, &comp_buf);
	if (err) {
		vOutputString("Getting composition failed (err %d)\n", err);
		return;
	}

	if (status != 0x00) {
		vOutputString("Got non-success status 0x%02x\n", status);
		return;
	}

	vOutputString("Got Composition Data for 0x%04x:\n", dst);
	vOutputString("\tCID      0x%04x\n",
		    net_buf_simple_pull_le16(&comp_buf));
	vOutputString("\tPID      0x%04x\n",
		    net_buf_simple_pull_le16(&comp_buf));
	vOutputString("\tVID      0x%04x\n",
		    net_buf_simple_pull_le16(&comp_buf));
	vOutputString("\tCRPL     0x%04x\n",
		    net_buf_simple_pull_le16(&comp_buf));
	vOutputString("\tFeatures 0x%04x\n",
		    net_buf_simple_pull_le16(&comp_buf));

	while (comp_buf.len > 4) {
		u8_t sig, vnd;
		u16_t loc;
		int i;

		loc = net_buf_simple_pull_le16(&comp_buf);
		sig = net_buf_simple_pull_u8(&comp_buf);
		vnd = net_buf_simple_pull_u8(&comp_buf);

		vOutputString("\tElement @ 0x%04x:\n", loc);

		if (comp_buf.len < ((sig * 2U) + (vnd * 4U))) {
			vOutputString("\t\t...truncated data!\n");
			break;
		}

		if (sig) {
			vOutputString("\t\tSIG Models:\n");
		} else {
			vOutputString("\t\tNo SIG Models\n");
		}

		for (i = 0; i < sig; i++) {
			u16_t mod_id = net_buf_simple_pull_le16(&comp_buf);

			vOutputString("\t\t\t0x%04x\n", mod_id);
		}

		if (vnd) {
			vOutputString("\t\tVendor Models:\n");
		} else {
			vOutputString("\t\tNo Vendor Models\n");
		}

		for (i = 0; i < vnd; i++) {
			u16_t cid = net_buf_simple_pull_le16(&comp_buf);
			u16_t mod_id = net_buf_simple_pull_le16(&comp_buf);

			vOutputString("\t\t\tCompany 0x%04x: 0x%04x\n", cid,
				    mod_id);
		}
	}

	return;
}

/* Add by bouffalo */
static void blemeshcli_set_dst(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	if (argc != 2) {
		return;
	}
	net.dst = strtoul(argv[1], NULL, 0);
	vOutputString("Net dst[0x%x]\n", net.dst);
}

#define CID_NVAL   0xffff
static void blemeshcli_net_key_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	bool has_key_val = (argc > 2);
	u8_t key_val[16];
	u16_t key_net_idx, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 4) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[5], strlen(argv[5]),
			      key_val, sizeof(key_val));
		(void)memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		struct bt_mesh_cdb_subnet *subnet;

		subnet = bt_mesh_cdb_subnet_get(key_net_idx);
		if (subnet) {
			if (has_key_val) {
				vOutputString("Subnet 0x%03x already has a value",
					    key_net_idx);
				return;
			}

			memcpy(key_val, subnet->keys[0].net_key, 16);
		} else {
			subnet = bt_mesh_cdb_subnet_alloc(key_net_idx);
			if (!subnet) {
				vOutputString("No space for subnet in cdb");
				return;
			}

			memcpy(subnet->keys[0].net_key, key_val, 16);
			bt_mesh_cdb_subnet_store(subnet);
		}
	}

	err = bt_mesh_cfg_net_key_add(net_idx, dst, key_net_idx,
				      key_val, &status);
	if (err) {
		vOutputString("Unable to send NetKey Add (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("NetKeyAdd failed with status 0x%02x",
			    status);
	} else {
		vOutputString("NetKey added with NetKey Index 0x%03x",
			    key_net_idx);
	}

	return;
}

/** Added by bouffalo
 * Default net, app & dev key values, unless otherwise specified 
 **/
static const u8_t default_new_key[16] = {
	0x02, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x02, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

/* Added by bouffalo */
static void blemeshcli_net_key_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	bool has_key_val = (argc > 2);
	u8_t key_val[16];
	u16_t key_net_idx, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 4) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[5], strlen(argv[5]),
			      key_val, sizeof(key_val));
		(void)memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_new_key, sizeof(key_val));
	}

	err = bt_mesh_cfg_net_key_update(net_idx, dst, key_net_idx,
				      key_val, &status);
	if (err) {
		vOutputString("Unable to send NetKey Update (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("NetKeyUpdate failed with status 0x%02x",
			    status);
	} else {
		vOutputString("NetKey updated with NetKey Index 0x%03x",
			    key_net_idx);
	}
	return;
}

static void blemeshcli_net_key_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t keys[16], net_idx, dst;
	size_t cnt;
	int err, i;

	cnt = ARRAY_SIZE(keys);
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	err = bt_mesh_cfg_net_key_get(net_idx, dst, keys, &cnt);
	if (err) {
		vOutputString("Unable to send NetKeyGet (err %d)", err);
		return;
	}

	vOutputString("NetKeys known by 0x%04x:", dst);
	for (i = 0; i < cnt; i++) {
		vOutputString("\t0x%03x", keys[i]);
	}

	return;
}

static void blemeshcli_net_key_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t key_net_idx, net_idx, dst;
	u8_t status;
	int err;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);

	err = bt_mesh_cfg_net_key_del(net_idx, dst, key_net_idx,
				      &status);
	if (err) {
		vOutputString("Unable to send NetKeyDel (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("NetKeyDel failed with status 0x%02x",
			    status);
	} else {
		vOutputString("NetKey 0x%03x deleted", key_net_idx);
	}

	return;
}

static void blemeshcli_app_key_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t key_val[16];
	u16_t net_idx, dst, key_net_idx, key_app_idx;
	bool has_key_val = (argc > 5);
	u8_t status;
	int err;

	if (argc < 5) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);

	get_uint16_from_string(&argv[3], &key_net_idx);
	get_uint16_from_string(&argv[4], &key_app_idx);

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[5], strlen(argv[5]),
			      key_val, sizeof(key_val));
		(void)memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		struct bt_mesh_cdb_app_key *app_key;

		app_key = bt_mesh_cdb_app_key_get(key_app_idx);
		if (app_key) {
			if (has_key_val) {
				vOutputString("App key 0x%03x already has a value",
					key_app_idx);
				return;
			}

			memcpy(key_val, app_key->keys[0].app_key, 16);
		} else {
			app_key = bt_mesh_cdb_app_key_alloc(key_net_idx,
							    key_app_idx);
			if (!app_key) {
				vOutputString("No space for app key in cdb");
				return;
			}

			memcpy(app_key->keys[0].app_key, key_val, 16);
			bt_mesh_cdb_app_key_store(app_key);
		}
	}

	err = bt_mesh_cfg_app_key_add(net_idx, dst, key_net_idx,
				      key_app_idx, key_val, &status);
	if (err) {
		vOutputString("Unable to send App Key Add (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("AppKeyAdd failed with status 0x%02x",
			    status);
	} else {
		vOutputString("AppKey added, NetKeyIndex 0x%04x "
			    "AppKeyIndex 0x%04x", key_net_idx, key_app_idx);
	}

	return;
}

/* Added by bouffalo */
static void blemeshcli_app_key_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t key_val[16];
	u16_t key_net_idx, key_app_idx, net_idx, dst;
	bool has_key_val = (argc > 3);
	u8_t status;
	int err;

	if (argc < 5) {
		return;
	}

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);
	get_uint16_from_string(&argv[4], &key_app_idx);

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[5], strlen(argv[5]),
			      key_val, sizeof(key_val));
		(void)memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_new_key, sizeof(key_val));
	}

	err = bt_mesh_cfg_app_key_update(net_idx, dst, key_net_idx,
				      key_app_idx, key_val, &status);
	if (err) {
		vOutputString("Unable to send App Key Add (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("AppKeyAdd failed with status 0x%02x",
			    status);
	} else {
		vOutputString("AppKey added, NetKeyIndex 0x%04x "
			    "AppKeyIndex 0x%04x", key_net_idx, key_app_idx);
	}

	return;
}

static void blemeshcli_app_key_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t key_net_idx, net_idx, dst;
	u16_t keys[16];
	size_t cnt;
	u8_t status;
	int err, i;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);
	cnt = ARRAY_SIZE(keys);

	err = bt_mesh_cfg_app_key_get(net_idx, dst, key_net_idx, &status,
				      keys, &cnt);
	if (err) {
		vOutputString("Unable to send AppKeyGet (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("AppKeyGet failed with status 0x%02x",
			    status);
		return;
	}

	vOutputString("AppKeys for NetKey 0x%03x known by 0x%04x:", net_idx,
		    dst);
	for (i = 0; i < cnt; i++) {
		vOutputString("\t0x%03x", keys[i]);
	}

	return;
}

static void blemeshcli_app_key_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t key_net_idx, key_app_idx, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 5) {
		return;
	}

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);
	get_uint16_from_string(&argv[4], &key_app_idx);

	err = bt_mesh_cfg_app_key_del(net_idx, dst, key_net_idx,
				      key_app_idx, &status);
	if (err) {
		vOutputString("Unable to send App Key del(err %d)", err);
		return;
	}

	if (status) {
		vOutputString("AppKeyDel failed with status 0x%02x",
			    status);
	} else {
		vOutputString("AppKey deleted, NetKeyIndex 0x%04x "
			    "AppKeyIndex 0x%04x", key_net_idx, key_app_idx);
	}

	return;
}

/* Added bouffalo */
static void blemeshcli_kr_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t idx;
	u8_t kr;
	if (argc != 3) {
		return;
	}

	get_uint16_from_string(&argv[1], &idx);
	get_uint8_from_string(&argv[2], &kr);
	struct bt_mesh_subnet *sub = bt_mesh_subnet_get(idx);
	bool kr_change = bt_mesh_kr_update(sub, kr, true);
	if (kr_change) {
		bt_mesh_net_beacon_update(sub);
	}
}

static void blemeshcli_mod_app_bind(char *pcWriteBuffer, int xWriteBufferLen, int argc,
			    char *argv[])
{
	u16_t elem_addr, mod_app_idx, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 6) {
		return;
	}
	u16_t net_idx, dst;
		
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);

	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &mod_app_idx);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_app_bind_vnd(net_idx, dst,
						   elem_addr, mod_app_idx,
						   mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_mod_app_bind(net_idx, dst, elem_addr,
					       mod_app_idx, mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model App Bind (err %d)",
			    err);
		return;
	}

	if (status) {
		vOutputString("Model App Bind failed with status 0x%02x",
			    status);
	} else {
		vOutputString("AppKey successfully bound");
	}

	return;
}


static void blemeshcli_mod_app_unbind(char *pcWriteBuffer, int xWriteBufferLen, int argc,
			    char *argv[])
{
	u16_t elem_addr, mod_app_idx, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 6) {
		return;
	}
	u16_t net_idx, dst;
		
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &mod_app_idx);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_app_unbind_vnd(net_idx, dst,
						   elem_addr, mod_app_idx,
						   mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_mod_app_unbind(net_idx, dst,
				elem_addr, mod_app_idx, mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model App Unbind (err %d)",
			    err);
		return;
	}

	if (status) {
		vOutputString("Model App Unbind failed with status 0x%02x",
			    status);
	} else {
		vOutputString("AppKey successfully unbound");
	}

	return;
}

static void blemeshcli_mod_app_get(char *pcWriteBuffer, int xWriteBufferLen, int argc,
			      char *argv[])
{
	u16_t elem_addr, mod_id, cid, net_idx, dst;
	u16_t apps[16];
	u8_t status;
	size_t cnt;
	int err, i;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &mod_id);
	cnt = ARRAY_SIZE(apps);

	if (argc > 5) {
		get_uint16_from_string(&argv[5], &cid);
		err = bt_mesh_cfg_mod_app_get_vnd(net_idx, dst,
						  elem_addr, mod_id, cid,
						  &status, apps, &cnt);
	} else {
		err = bt_mesh_cfg_mod_app_get(net_idx, dst, elem_addr,
					      mod_id, &status, apps, &cnt);
	}

	if (err) {
		vOutputString("Unable to send Model App Get (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Model App Get failed with status 0x%02x\n",
			    status);
	} else {
		vOutputString("Apps bound to Element 0x%04x, Model 0x%04x %s:\n",
			elem_addr, mod_id, argc > 5 ? argv[5] : "(SIG)");

		if (!cnt) {
			vOutputString("\tNone.\n");
		}

		for (i = 0; i < cnt; i++) {
			vOutputString("\t0x%04x\n", apps[i]);
		}
	}

	return;
}

static void blemeshcli_mod_sub_add(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 6) {
		return;
	}

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &sub_addr);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_add_vnd(net_idx, dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_add(net_idx, dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription Add "
			    "(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription Add failed with status "
			    "0x%02x\n", status);
	} else {
		vOutputString("Model subscription was successful\n");
	}

	return;
}

/** Added by bouffalo 
 * Config Model Subscription Overwrite.
 **/
static void blemeshcli_mod_sub_ow(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 6) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &sub_addr);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_overwrite_vnd(net_idx, dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_overwrite(net_idx, dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription overwrite "
			    "(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription overwrite failed with status "
			    "0x%02x\n", status);
	} else {
		vOutputString("Model subscription was successful\n");
	}

	return;
}


static void blemeshcli_mod_sub_del(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 6) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &sub_addr);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_del_vnd(net_idx, dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_del(net_idx, dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription Delete "
			    "(err %d)\n");
		return;
	}

	if (status) {
		vOutputString("Model Subscription Delete failed with "
			    "status 0x%02x\n", status);
	} else {
		vOutputString("Model subscription deltion was successful\n");
	}

	return;
}

static void blemeshcli_mod_sub_del_all(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t elem_addr, mod_id, cid, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 5) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_del_all_vnd(net_idx, dst,
						  elem_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_del_all(net_idx, dst, elem_addr,
					      mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription Delete "
			    "(err %d)\n");
		return;
	}

	if (status) {
		vOutputString("Model Subscription Delete failed with "
			    "status 0x%02x\n", status);
	} else {
		vOutputString("Model subscription deltion was successful\n");
	}

	return;
}

static void blemeshcli_mod_sub_add_va(char *pcWriteBuffer, int xWriteBufferLen, int argc,
			      char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid, net_idx, dst;
	u8_t label[16];
	u8_t status;
	size_t len;
	int err;

	if (argc < 6) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);

	len = hex2bin(argv[4], strlen(argv[4]), label, sizeof(label));
	(void)memset(label + len, 0, sizeof(label) - len);

	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_va_add_vnd(net_idx, dst,
						     elem_addr, label, mod_id,
						     cid, &sub_addr, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_va_add(net_idx, dst,
						 elem_addr, label, mod_id,
						 &sub_addr, &status);
	}

	if (err) {
		vOutputString("Unable to send Mod Sub VA Add (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Mod Sub VA Add failed with status 0x%02x\n",
			    status);
	} else {
		vOutputString("0x%04x subscribed to Label UUID %s "
			    "(va 0x%04x)\n", elem_addr, argv[2], sub_addr);
	}

	return;
}

				

/** Added by bouffalo 
 * Config Model Subscription Overwrite.
 **/
static void blemeshcli_mod_sub_ow_va(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t elem_addr, mod_id, cid, net_idx, dst;
	u8_t status;
	int err;
	u16_t virt_addr;
	size_t len;

	if (argc < 4) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	u8_t label[16];
	len = hex2bin(argv[4], strlen(argv[4]), label, sizeof(label));
	(void)memset(label + len, 0, sizeof(label) - len);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_va_overwrite_vnd(net_idx, dst,
					 elem_addr, label,
					 mod_id, cid,
					 &virt_addr, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_va_overwrite(net_idx, dst, elem_addr,
				     label,
				     mod_id, &virt_addr,
				     &status);
	}
	
	if (err) {
		vOutputString("Unable to send Model Subscription overwrite "
				"(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription overwrite failed with status "
				"0x%02x\n", status);
	} else {
		vOutputString("Model subscription was successful\n");
	}

	return;
}


static void blemeshcli_mod_sub_del_va(char *pcWriteBuffer, int xWriteBufferLen, int argc,
			      char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid, net_idx, dst;
	u8_t label[16];
	u8_t status;
	size_t len;
	int err;

	if (argc < 6) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);

	len = hex2bin(argv[4], strlen(argv[4]), label, sizeof(label));
	(void)memset(label + len, 0, sizeof(label) - len);

	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_va_del_vnd(net_idx, dst,
						     elem_addr, label, mod_id,
						     cid, &sub_addr, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_va_del(net_idx, dst,
						 elem_addr, label, mod_id,
						 &sub_addr, &status);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription Delete "
			    "(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription Delete failed with "
			    "status 0x%02x\n", status);
	} else {
		vOutputString("0x%04x unsubscribed from Label UUID %s "
			    "(va 0x%04x)\n", elem_addr, argv[4], sub_addr);
	}

	return;
}

static void blemeshcli_mod_sub_get(char *pcWriteBuffer, int xWriteBufferLen, int argc,
			      char *argv[])
{
	u16_t elem_addr, mod_id, cid, net_idx, dst;
	u16_t subs[16];
	u8_t status;
	size_t cnt;
	int err, i;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &mod_id);
	cnt = ARRAY_SIZE(subs);

	if (argc > 5) {
		get_uint16_from_string(&argv[5], &cid);
		err = bt_mesh_cfg_mod_sub_get_vnd(net_idx, dst,
						  elem_addr, mod_id, cid,
						  &status, subs, &cnt);
	} else {
		err = bt_mesh_cfg_mod_sub_get(net_idx, dst, elem_addr,
					      mod_id, &status, subs, &cnt);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription Get "
			    "(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription Get failed with "
			    "status 0x%02x\n", status);
	} else {
		vOutputString("Model Subscriptions for Element 0x%04x, "
			"Model 0x%04x %s:\n",
			elem_addr, mod_id, argc > 5 ? argv[5] : "(SIG)");

		if (!cnt) {
			vOutputString("\tNone.\n");
		}

		for (i = 0; i < cnt; i++) {
			vOutputString("\t0x%04x\n", subs[i]);
		}
	}

	return;
}

static void mod_pub_get(u16_t addr, u16_t mod_id,
		       u16_t cid)
{
	struct bt_mesh_cfg_mod_pub pub;
	u8_t status;
	int err;

	if (cid == CID_NVAL) {
		err = bt_mesh_cfg_mod_pub_get(net.net_idx, net.dst, addr,
					      mod_id, &pub, &status);
	} else {
		err = bt_mesh_cfg_mod_pub_get_vnd(net.net_idx, net.dst, addr,
						  mod_id, cid, &pub, &status);
	}

	if (err) {
		vOutputString("Model Publication Get failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Model Publication Get failed "
			    "(status 0x%02x)\n", status);
		return;
	}

	vOutputString("Model Publication for Element 0x%04x, Model 0x%04x:\n"
		    "\tPublish Address:                0x%04x\n"
		    "\tAppKeyIndex:                    0x%04x\n"
		    "\tCredential Flag:                %u\n"
		    "\tPublishTTL:                     %u\n"
		    "\tPublishPeriod:                  0x%02x\n"
		    "\tPublishRetransmitCount:         %u\n"
		    "\tPublishRetransmitInterval:      %ums\n",
		    addr, mod_id, pub.addr, pub.app_idx, pub.cred_flag, pub.ttl,
		    pub.period, BT_MESH_PUB_TRANSMIT_COUNT(pub.transmit),
		    BT_MESH_PUB_TRANSMIT_INT(pub.transmit));

	return;
}

static void mod_pub_set(u16_t addr, u16_t mod_id,
		       u16_t cid, int argc, char *argv[])
{
	struct bt_mesh_cfg_mod_pub pub;
	u8_t status, count;
	u16_t interval, net_idx, dst;
	int err;
	get_uint16_from_string(&argv[0], &net_idx);
	get_uint16_from_string(&argv[1], &dst);
	get_uint16_from_string(&argv[2], &pub.addr);
	get_uint16_from_string(&argv[3], &pub.app_idx);
	get_uint8_from_string(&argv[4], (uint8_t*)&pub.cred_flag);
	get_uint8_from_string(&argv[5], &pub.ttl);
	get_uint8_from_string(&argv[6], &pub.period);

	get_uint8_from_string(&argv[7], &count);
	if (count > 9) {
		vOutputString("Invalid retransmit count\n");
		return;
	}

	get_uint16_from_string(&argv[8], &interval);
	if (interval > (31 * 50) || (interval % 50)) {
		vOutputString("Invalid retransmit interval %u\n", interval);
		return;
	}

	pub.transmit = BT_MESH_PUB_TRANSMIT(count, interval);

	vOutputString("count[%x] interval[%x]\n", count, interval);

	if (cid == CID_NVAL) {
		err = bt_mesh_cfg_mod_pub_set(net_idx, dst, addr,
					      mod_id, &pub, &status);
	} else {
		err = bt_mesh_cfg_mod_pub_set_vnd(net_idx, dst, addr,
						  mod_id, cid, &pub, &status);
	}

	if (err) {
		vOutputString("Model Publication Set failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Model Publication Set failed \n"
			    "(status 0x%02x)", status);
	} else {
		vOutputString("Model Publication successfully set\n");
	}

	return;
}

static void mod_pub_va_set(u16_t addr, u16_t mod_id,
			  u16_t cid, int argc, char *argv[])
{
	struct bt_mesh_cfg_mod_pub pub;
	u8_t status, count;
	u16_t interval, net_idx, dst;
	int err;
	u8_t label[16];

	get_uint16_from_string(&argv[0], &net_idx);
	get_uint16_from_string(&argv[1], &dst);
	get_bytearray_from_string(&argv[2], label, sizeof(label));
	get_uint16_from_string(&argv[3], &pub.app_idx);
	get_uint8_from_string(&argv[4], (uint8_t*)&pub.cred_flag);
	get_uint8_from_string(&argv[5], &pub.ttl);
	get_uint8_from_string(&argv[6], &pub.period);

	get_uint8_from_string(&argv[7], &count);
	if (count > 9) {
		vOutputString("Invalid retransmit count\n");
		return;
	}

	get_uint16_from_string(&argv[8], &interval);
	if (interval > (31 * 50) || (interval % 50)) {
		vOutputString("Invalid retransmit interval %u\n", interval);
		return;
	}

	pub.transmit = BT_MESH_PUB_TRANSMIT(count, interval);

	vOutputString("count[%x] interval[%x]\n", count, interval);

	if (cid == CID_NVAL) {
		err = bt_mesh_cfg_mod_pub_va_set(net_idx, dst, addr,
				label, mod_id, &pub, &status);
	} else {
		err = bt_mesh_cfg_mod_pub_va_set(net_idx, dst, addr,
			label, mod_id, &pub, &status);
	}

	if (err) {
		vOutputString("Model Publication Set failed (err %d)\n",
				err);
		return;
	}

	if (status) {
		vOutputString("Model Publication Set failed \n"
				"(status 0x%02x)", status);
	} else {
		vOutputString("Model Publication successfully set\n");
	}

	return;
}

static void blemeshcli_mod_pub(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t addr, mod_id, cid;

	if (argc < 3) {
		return;
	}

	get_uint16_from_string(&argv[1], &addr);
	get_uint16_from_string(&argv[2], &mod_id);

	argc -= 3;
	argv += 3;

	if (argc == 1 || argc == 8) {
		get_uint16_from_string(&argv[0], &cid);
		argc--;
		argv++;
	} else {
		cid = CID_NVAL;
	}

	if (argc > 0) {
		if (argc < 7) {
			return;
		}

		mod_pub_set(addr, mod_id, cid, argc, argv);
	} else {
		mod_pub_get(addr, mod_id, cid);
	}
}

static void blemeshcli_mod_pub_va(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t addr, mod_id, cid;

	if (argc < 3) {
		return;
	}

	get_uint16_from_string(&argv[1], &addr);
	get_uint16_from_string(&argv[2], &mod_id);

	argc -= 3;
	argv += 3;

	if (argc == 1 || argc == 8) {
		get_uint16_from_string(&argv[0], &cid);
		argc--;
		argv++;
	} else {
		cid = CID_NVAL;
	}

	if (argc > 0) {
		if (argc < 7) {
			return;
		}

		mod_pub_va_set(addr, mod_id, cid, argc, argv);
	} else {
		mod_pub_get(addr, mod_id, cid);
	}
}

static void hb_sub_print(struct bt_mesh_cfg_hb_sub *sub)
{
	vOutputString("Heartbeat Subscription:\n"
		    "\tSource:      0x%04x\n"
		    "\tDestination: 0x%04x\n"
		    "\tPeriodLog:   0x%02x\n"
		    "\tCountLog:    0x%02x\n"
		    "\tMinHops:     %u\n"
		    "\tMaxHops:     %u\n",
		    sub->src, sub->dst, sub->period, sub->count,
		    sub->min, sub->max);
}

static void hb_sub_get(size_t argc, char *argv[])
{
	struct bt_mesh_cfg_hb_sub sub;
	u8_t status;
	u16_t net_idx, dst;
	int err;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	err = bt_mesh_cfg_hb_sub_get(net_idx, dst, &sub, &status);
	if (err) {
		vOutputString("Heartbeat Subscription Get failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Heartbeat Subscription Get failed "
			    "(status 0x%02x)\n", status);
	} else {
		hb_sub_print(&sub);
	}

	return;
}

static void hb_sub_set(size_t argc, char *argv[])
{
	struct bt_mesh_cfg_hb_sub sub;
	u8_t status;
	u16_t net_idx, dst;
	int err;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &sub.src);
	get_uint16_from_string(&argv[4], &sub.dst);
	get_uint8_from_string(&argv[5], &sub.period);

	err = bt_mesh_cfg_hb_sub_set(net_idx, dst, &sub, &status);
	if (err) {
		vOutputString("Heartbeat Subscription Set failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Heartbeat Subscription Set failed "
			    "(status 0x%02x)\n", status);
	} else {
		hb_sub_print(&sub);
	}

	return;
}

static void blemeshcli_hb_sub(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	if (argc > 1) {
		if (argc < 4) {
			return;
		}

		hb_sub_set(argc, argv);
	} else {
		hb_sub_get(argc, argv);
	}
}

static void hb_pub_get(size_t argc, char *argv[])
{
	struct bt_mesh_cfg_hb_pub pub;
	u8_t status;
	u16_t net_idx, dst;
	int err;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	err = bt_mesh_cfg_hb_pub_get(net_idx, dst, &pub, &status);
	if (err) {
		vOutputString("Heartbeat Publication Get failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Heartbeat Publication Get failed "
			    "(status 0x%02x)\n", status);
		return;
	}

	vOutputString("Heartbeat publication:\n");
	vOutputString("\tdst 0x%04x count 0x%02x period 0x%02x\n",
		    pub.dst, pub.count, pub.period);
	vOutputString("\tttl 0x%02x feat 0x%04x net_idx 0x%04x\n",
		    pub.ttl, pub.feat, pub.net_idx);

	return;
}

static void hb_pub_set(size_t argc, char *argv[])
{
	struct bt_mesh_cfg_hb_pub pub;
	u8_t status;
	u16_t net_idx, dst;
	int err;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &pub.dst);
	get_uint8_from_string(&argv[4], &pub.count);
	get_uint8_from_string(&argv[5], &pub.period);
	get_uint8_from_string(&argv[6], &pub.ttl);
	get_uint16_from_string(&argv[7], &pub.feat);
	get_uint16_from_string(&argv[8], &pub.net_idx);

	err = bt_mesh_cfg_hb_pub_set(net_idx, dst, &pub, &status);
	if (err) {
		vOutputString("Heartbeat Publication Set failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Heartbeat Publication Set failed "
			    "(status 0x%02x)\n", status);
	} else {
		vOutputString("Heartbeat publication successfully set\n");
	}

	return;
}

static void blemeshcli_hb_pub(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	if (argc > 1) {
		if (argc < 7) {
			return;
		}

		hb_pub_set(argc, argv);
	} else {
		hb_pub_get(argc, argv);
	}
}

/* Add by bouffalo */
static void blemeshcli_krp_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	struct bt_mesh_cfg_krp krp_buf;
	u8_t status;
	u16_t net_idx, dst;
	int err;
	if (argc != 4) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &krp_buf.NetKeyIndex);

	err = bt_mesh_cfg_krp_get(net_idx, dst,
					&status, &krp_buf);
	if (err) {
		vOutputString("Getting Key Refresh Phase failed (err %d)\n", err);
		return;
	}

	if (status != 0x00) {
		vOutputString("Got non-success status 0x%02x\n", status);
		return;
	}

	vOutputString("Got Key Refresh Phase for 0x%04x:\n", dst);
	vOutputString("\tNetKeyIndex      0x%04x\n",
		    krp_buf.NetKeyIndex);
	vOutputString("\tPhase      0x%04x\n",
		    krp_buf.Phase);

	return;
}

/* Add by bouffalo */
static void blemeshcli_krp_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	struct bt_mesh_cfg_krp krp_buf;
	u8_t status;
	u16_t net_idx, dst;
	int err;
	if (argc != 3) {
		return;
	}
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &krp_buf.NetKeyIndex);
	get_uint8_from_string(&argv[4], &krp_buf.Phase);
	err = bt_mesh_cfg_krp_set(net_idx, dst,
					&status, &krp_buf);
	if (err) {
		vOutputString("Getting Key Refresh Phase failed (err %d)\n", err);
		return;
	}

	if (status != 0x00) {
		vOutputString("Got non-success status 0x%02x\n", status);
		return;
	}

	vOutputString("Got Key Refresh Phase for 0x%04x:\n", dst);
	vOutputString("\tNetKeyIndex      0x%04x\n",
		    krp_buf.NetKeyIndex);
	vOutputString("\tPhase      0x%04x\n",
		    krp_buf.Phase);

	return;
}

/* Add by bouffalo */
static void blemeshcli_cfg_bcn_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t status = 0;
	int err;
	if (argc != 3) {
		return;
	}
	u16_t net_idx;
	u16_t dst;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	err = bt_mesh_cfg_beacon_get(net_idx, dst, &status);
	if (err) {
		vOutputString("Getting Config Beacon Status failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Config Beacon Status for 0x%04x:\n", dst);
	vOutputString("\tSecure Network Beacon state      0x%02x\n",
		    status);
}

/* Add by bouffalo */
static void blemeshcli_cfg_bcn_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t status = 0;
	int err;
	if (argc != 4) {
		return;
	}
	u16_t net_idx = 0;
	u16_t dst = 0;
	u8_t val = 0;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint8_from_string(&argv[3], &val);
	
	err = bt_mesh_cfg_beacon_set(net_idx, dst, val, &status);
	if (err) {
		vOutputString("Getting Config Beacon Status failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Config Beacon Status for 0x%04x:\n", dst);
	vOutputString("\tSecure Network Beacon state      0x%02x\n",
		    status);
}

/* Add by bouffalo */
static void blemeshcli_cfg_dttl_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	int err;
	if (argc != 3) {
		return;
	}
	u16_t net_idx;
	u16_t dst;
	u8_t ttl = 0;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	err = bt_mesh_cfg_ttl_get(net_idx, dst, &ttl);
	if (err) {
		vOutputString("Getting Default TTL failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Default TTL for 0x%04x:\n", dst);
	vOutputString("\tDefault TTL      0x%02x\n",
		    ttl);
}

/* Add by bouffalo */
static void blemeshcli_cfg_dttl_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	int err;
	if (argc != 4) {
		return;
	}
	u16_t net_idx = 0;
	u16_t dst = 0;
	u8_t val = 0;
	u8_t ttl = 0;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint8_from_string(&argv[3], &val);
	
	err = bt_mesh_cfg_ttl_set(net_idx, dst, val, &ttl);
	if (err) {
		vOutputString("Getting Default TTL failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Default TTL for 0x%04x:\n", dst);
	vOutputString("\tDefault TTL      0x%02x\n",
		    ttl);
}

/* Add by bouffalo */
static void blemeshcli_cfg_gpxy_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t status = 0;
	int err;
	if (argc != 3) {
		return;
	}
	u16_t net_idx;
	u16_t dst;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	err = bt_mesh_cfg_gatt_proxy_get(net_idx, dst, &status);
	if (err) {
		vOutputString("Getting Default TTL failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Default TTL for 0x%04x:\n", dst);
	vOutputString("\tDefault TTL      0x%02x\n",
		    status);
}

/* Add by bouffalo */
static void blemeshcli_cfg_gpxy_set(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t status = 0;
	int err;
	if (argc != 4) {
		return;
	}
	u16_t net_idx = 0;
	u16_t dst = 0;
	u8_t val = 0;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint8_from_string(&argv[3], &val);
	
	err = bt_mesh_cfg_gatt_proxy_set(net_idx, dst, val, &status);
	if (err) {
		vOutputString("Getting Default TTL failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Default TTL for 0x%04x:\n", dst);
	vOutputString("\tDefault TTL      0x%02x\n",
		    status);
}

static void blemeshcli_friend(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t frnd;
	int err = -1;

	if (argc == 3) {
		u16_t net_idx = 0;
		u16_t dst = 0;
		
		get_uint16_from_string(&argv[1], &net_idx);
		get_uint16_from_string(&argv[2], &dst);
		err = bt_mesh_cfg_friend_get(net_idx, dst, &frnd);
	} else if(argc == 4){
		u16_t net_idx = 0;
		u16_t dst = 0;
		u8_t val = 0;
		
		get_uint16_from_string(&argv[1], &net_idx);
		get_uint16_from_string(&argv[2], &dst);
		get_uint8_from_string(&argv[3], &val);

		err = bt_mesh_cfg_friend_set(net_idx, dst, val, &frnd);
	}

	if (err) {
		vOutputString("Unable to send Friend Get/Set (err %d)",err);
		return;
	}

	vOutputString("Friend is set to 0x%02x", frnd);

	return;
}

static void blemeshcli_relay(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t relay, transmit;
	int err;

	if (argc < 4) {
		u16_t net_idx = 0;
		u16_t dst = 0;
		
		get_uint16_from_string(&argv[1], &net_idx);
		get_uint16_from_string(&argv[2], &dst);
		err = bt_mesh_cfg_relay_get(net_idx, dst, &relay,
					    &transmit);
	} else {
		u8_t count, interval, new_transmit, val;
		u16_t net_idx = 0;
		u16_t dst = 0;
		
		get_uint8_from_string(&argv[3], &val);
		get_uint16_from_string(&argv[1], &net_idx);
		get_uint16_from_string(&argv[2], &dst);

		if (val) {
			if (argc > 4) {
				get_uint8_from_string(&argv[4], &count);
			} else {
				count = 2U;
			}

			if (argc > 5) {
				get_uint8_from_string(&argv[5], &interval);
			} else {
				interval = 20U;
			}

			new_transmit = BT_MESH_TRANSMIT(count, interval);
		} else {
			new_transmit = 0U;
		}

		err = bt_mesh_cfg_relay_set(net_idx, dst, val,
					    new_transmit, &relay, &transmit);
	}

	if (err) {
		vOutputString("Unable to send Relay Get/Set (err %d)",
			    err);
		return;
	}

	vOutputString("Relay is 0x%02x, Transmit 0x%02x (count %u interval"
		    " %ums)", relay, transmit, BT_MESH_TRANSMIT_COUNT(transmit),
		    BT_MESH_TRANSMIT_INT(transmit));

	return;
}

/* Added by bouffalo */
static void blemeshcli_node_identify(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t status, identity;
	u16_t netkeyindex, net_idx, dst;
	int err = -1;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);

	if (argc == 5) {
		get_uint16_from_string(&argv[3], &netkeyindex);
		get_uint8_from_string(&argv[4], &identity);
		err = bt_mesh_cfg_node_identity_set(net_idx, dst,
						&status, &netkeyindex, &identity);
	} else if (argc == 4) {
		get_uint16_from_string(&argv[3], &netkeyindex);
		err = bt_mesh_cfg_node_identity_get(net_idx, dst,
						&status, &netkeyindex, &identity);
	}

	if (err) {
		vOutputString("Unable to node identity Get/Set (err %d)",
				err);
		return;
	}

	vOutputString("netkeyindex is %x, identity %x",
					netkeyindex, identity);

}

/* Added by bouffalo */
static void blemeshcli_node_reset(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t net_idx, dst;
	if (argc != 3) {
		vOutputString("Number of Parameters is not correct\r\n");
		return;
	}

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	int err = bt_mesh_cfg_node_reset(net_idx, dst);
	if (err) {
		vOutputString("Unable to node reset  (err %d)\n",
				err);
	}

#if defined(CONFIG_BT_MESH_CDB)
	/* Do local cdb clear */
	struct bt_mesh_cdb_node *node;
	
	node = bt_mesh_cdb_node_get(dst);
	if (node == NULL) {
		vOutputString("No node with address 0x%04x\n", dst);
		return;
	}

	bt_mesh_cdb_node_del(node, true);

	vOutputString("Deleted node 0x%04x\n", dst);
#endif
}

/* Added by bouffalo */
static void blemeshcli_network_trans(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t count, interval;
	u16_t net_idx, dst;
	int err = -1;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	if (argc == 5) {
		get_uint8_from_string(&argv[3], &count);
		get_uint8_from_string(&argv[4], &interval);
		err = bt_mesh_cfg_network_transmit_set(net_idx, dst,
						&count, &interval);
	} else if (argc == 1) {
		err = bt_mesh_cfg_network_transmit_get(net_idx, dst,
						&count, &interval);
	}

	if (err) {
		vOutputString("Unable to Network Transmit Get/Set (err %d)",
				err);
		return;
	}

	vOutputString("count is %x, interval %x",
					count, interval);
}

/* Added by bouffalo */
static void blemeshcli_lpn_timeout_get(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u16_t lpn_addr, net_idx, dst;
	u32_t poll_timeout;
	int err;

	if (argc != 4) {
		return ;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &lpn_addr);
	err = bt_mesh_cfg_lpn_poll_timeout_get(net_idx, dst,
					&lpn_addr, &poll_timeout);

	if (err) {
		vOutputString("Unable to LPN Poll time out Get/Set (err %d)",
				err);
		return;
	}

	vOutputString("lpn_addr is %x, poll_timeout %x",
					lpn_addr, poll_timeout);
}

/* Added by bouffalo for health client fault state*/
static void blemeshcli_clhm_fault(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	int err = 0;
	u16_t app_idx, cid, dst;
	u8_t test_id, faults[16];
	u32_t fault_count;

	if(0 == strcmp(argv[1], "test")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		get_uint16_from_string(&argv[4], &cid);
		get_uint8_from_string(&argv[5], &test_id);
		if(argc == 6)
			/* Health Fault Test */
			err = bt_mesh_health_fault_test(dst, app_idx, cid,
						 test_id, faults, &fault_count);
		else
			/* Health Fault Test Unacknowledged */
			err = bt_mesh_health_fault_test(dst, app_idx, cid,
						 test_id, NULL, &fault_count);
	}
	else if(0 == strcmp(argv[1], "get")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		get_uint16_from_string(&argv[4], &cid);
		err = bt_mesh_health_fault_get(dst, app_idx, cid,
						 &test_id, faults, &fault_count);
	}
	else if(0 == strcmp(argv[1], "clear")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		get_uint16_from_string(&argv[4], &cid);
		if(argc == 5)
			/* Health Fault Clear */
			err = bt_mesh_health_fault_clear(dst, app_idx, cid,
							 &test_id, faults, &fault_count);
		else
			/* Health Fault Clear Unacknowledged */
			err = bt_mesh_health_fault_clear(dst, app_idx, cid,
							 NULL, faults, &fault_count);
	}

	if (err) {
		vOutputString("Unable to health fault Get/Set (err %d)",
				err);
		return;
	}

}

/* Added by bouffalo for health client period state*/
static void blemeshcli_clhm_period(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t divisor, updated_divisor;
	u16_t app_idx, dst;
	int err = -1;

	if(0 == strcmp(argv[1], "get")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		err = bt_mesh_health_period_get(dst, app_idx, &divisor);
	}
	else if(0 == strcmp(argv[1], "set")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		get_uint8_from_string(&argv[4], &divisor);
		if(argc == 5)
			/* Acknowledged*/
			err = bt_mesh_health_period_set(dst, app_idx, divisor,
				 &updated_divisor);
		else
			/* Unacknowledged */
			err = bt_mesh_health_period_set(dst, app_idx, divisor,
				 NULL);
	}

	if (err) {
		vOutputString("Unable to health period Get/Set (err %d)",
				err);
		return;
	}
}
/* Added by bouffalo for health client attention timer state*/
static void blemeshcli_clhm_ats(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t attention, updated_attention;
	int err = -1;
	u16_t app_idx, dst;

	if(0 == strcmp(argv[1], "get")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		err = bt_mesh_health_attention_get(dst, app_idx, &attention);
	}
	else if(0 == strcmp(argv[1], "set")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		get_uint8_from_string(&argv[4], &attention);
		if(argc == 5)
			/* Acknowledged*/
			err = bt_mesh_health_attention_set(dst, app_idx, attention,
					 &updated_attention);
		else
			/* Unacknowledged */
			err = bt_mesh_health_attention_set(dst, app_idx, attention,
					 NULL);
	}

	if (err) {
		vOutputString("Unable to health attention Get/Set (err %d)",
				err);
		return;
	}
}
#endif

#if defined(CFG_NODE_SEND_CFGCLI_MSG) && defined(CONFIG_BT_MESH_CDB)
static void blemeshcli_pvnr_devkey(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	u8_t key[16], num_elem;
	u16_t addr;
	
	get_uint16_from_string(&argv[1], &addr);
	get_uint8_from_string(&argv[2], &num_elem);
	get_bytearray_from_string(&argv[3], key, sizeof(key));

	if(NULL == bt_mesh_cdb_node_set_fornode(addr, num_elem, key)){
		vOutputString("Set node devkey error\n");
	}
	else{
		vOutputString("Set node Addr[%x] element[%x], devkey[%s]\n",
						addr, num_elem, bt_hex(key, sizeof(key)));
	}
}
#endif /* CFG_NODE_SEND_CFGCLI_MSG */

#if defined(CONFIG_BT_MESH_SYNC)
static void blemeshcli_sync_start(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	return blsync_blemesh_start();
}

static void blemeshcli_sync_stop(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
	return blsync_blemesh_stop();
}
#endif


#if defined(CONFIG_BT_MESH_TEST)
#define CONFIG_NODE_SIZE_BUFF (CONFIG_BT_MESH_NODE_COUNT<<1)
uint16_t node_list[CONFIG_NODE_SIZE_BUFF];

#define NODE_ADDR_FUN_MSK (0x8000)

/* check all nodelist is check*/
static int nodelist_check(uint16_t addr)
{
    int i = 0;

    if((node_list[addr] & (NODE_ADDR_FUN_MSK-1)) == addr){
        node_list[addr] |=  NODE_ADDR_FUN_MSK;
        for(i = 0; i < CONFIG_NODE_SIZE_BUFF; ++i){
            if(node_list[i] == 0)
                continue;
            if((node_list[i] & NODE_ADDR_FUN_MSK) == 0)
                return 0;
        }
    }

    return 1;
}


static int nodelist_check_clear()
{
    int i = 0;
    for(i = 0; i < CONFIG_NODE_SIZE_BUFF; ++i){
        if(node_list[i] == 0)
            continue;
        node_list[i] &= NODE_ADDR_FUN_MSK-1;
    }
    return 0;
}


static void blemeshcli_nodelist_op(char *pcWriteBuffer, int xWriteBufferLen, int argc, char *argv[])
{
    /** arg1: 1,add address from start to end; 2,add single address, 3,delete single address
     *  arg2: start address or single address.
     *  arg3: end address.
     **/
    u8_t type = 0;
    u16_t i, saddr = 0, eaddr = 0;

    if (argc >= 3) {
        get_uint8_from_string(&argv[1], &type);
        get_uint16_from_string(&argv[2], &saddr);
        if(saddr >= CONFIG_NODE_SIZE_BUFF){
            vOutputString("node_list cache is small\r\n");
            return;
        }
        if(type == 1 && argc == 4){
            get_uint16_from_string(&argv[3], &eaddr);
            if(eaddr >= CONFIG_NODE_SIZE_BUFF){
                vOutputString("node_list cache is small\r\n");
                return;
            }
            if(eaddr <= saddr){
                vOutputString("Start address isn't bigger than end address\r\n");
                return;
            }
            for(i = saddr; i <= eaddr; ++i){
                node_list[i] = i;
            }
        }
        else if(type == 2){
            node_list[saddr] = saddr;
        }
        else{
            node_list[saddr] = 0;
        }
	}
    else{
        vOutputString("invalid parameter\r\n");
        return;
    }
}
#endif /* CONFIG_BT_MESH_TEST */

int blemesh_cli_register(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(btStackCmdSet, sizeof(btMeshCmdSet)/sizeof(btMeshCmdSet[0]));
    return 0;
}