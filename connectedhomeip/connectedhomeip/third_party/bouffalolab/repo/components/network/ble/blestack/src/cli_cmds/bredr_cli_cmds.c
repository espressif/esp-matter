/** @file
 * @brief Bluetooth BR/EDR shell module
 *
 * Provide some BR/EDR commands that can be useful to applications.
 */

#include <stdlib.h>
#include <string.h>
#include <byteorder.h>
#include <bluetooth.h>
#include <hci_host.h>
#include <conn.h>
#include <conn_internal.h>
#include <hci_core.h>
#include <l2cap.h>
#include <l2cap_internal.h>
#if CONFIG_BT_A2DP
#include <a2dp.h>
#endif
#if CONFIG_BT_AVRCP
#include <avrcp.h>
#endif
#if CONFIG_BT_AVRCP
#include <rfcomm.h>
#endif
#if CONFIG_BT_HFP
#include <hfp_hf.h>
#endif

#include "cli.h"

#if PCM_PRINTF
#include "oi_codec_sbc.h"
#endif

static void bredr_connected(struct bt_conn *conn, u8_t err);
static void bredr_disconnected(struct bt_conn *conn, u8_t reason);

static bool init = false;
static struct bt_conn_info conn_info;
static struct bt_conn *default_conn = NULL;

static struct bt_conn_cb conn_callbacks = {
    .connected = bredr_connected,
    .disconnected = bredr_disconnected,
};

#if CONFIG_BT_A2DP
static void a2dp_chain(struct bt_conn *conn, uint8_t state);
static void a2dp_stream(uint8_t state);

static struct a2dp_callback a2dp_callbacks =
{
    .chain = a2dp_chain,
    .stream = a2dp_stream,
};
#endif

#if CONFIG_BT_AVRCP
static void avrcp_chain(struct bt_conn *conn, uint8_t state);
static void avrcp_absvol(uint8_t vol);
static void avrcp_play_status(uint32_t song_len, uint32_t song_pos, uint8_t status);

static struct avrcp_callback avrcp_callbacks =
{
    .chain = avrcp_chain,
    .abs_vol = avrcp_absvol,
    .play_status = avrcp_play_status,
};
#endif

#if PCM_PRINTF
static void pcm(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#endif
static void bredr_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void bredr_write_local_name(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_write_eir(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_discoverable(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_connectable(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_disconnect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_remote_name(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_l2cap_send_test_data(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_l2cap_disconnect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_l2cap_echo_req(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_security(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);

#if BR_EDR_PTS_TEST
static void bredr_sdp_client_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#endif

#if CONFIG_BT_A2DP
static void a2dp_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#if BR_EDR_PTS_TEST
static void avdtp_set_conf_reject(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#endif
#endif

#if CONFIG_BT_AVRCP
static void avrcp_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void avrcp_pth_key(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void avrcp_pth_key_act(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void avrcp_change_vol(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void avrcp_get_play_status(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#endif

#if CONFIG_BT_HFP
static void hfp_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void sco_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void hfp_answer(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void hfp_terminate_call(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void hfp_outgoint_call(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void hfp_outgoint_call_with_mem_loc(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void hfp_outgoint_call_last_number_dialed(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void hfp_disable_nrec(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void hfp_voice_recognition(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void hfp_voice_req_phone_num(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#endif

const struct cli_command bredr_cmd_set[] STATIC_CLI_CMD_ATTRIBUTE = {
    #if PCM_PRINTF
    {"pcm", "", pcm},
    #endif
    {"bredr_init", "", bredr_init},
    {"bredr_name", "", bredr_write_local_name},
    {"bredr_eir", "", bredr_write_eir},
    {"bredr_connectable", "", bredr_connectable},
    {"bredr_discoverable", "", bredr_discoverable},
    {"bredr_connect", "", bredr_connect},
    {"bredr_disconnect", "", bredr_disconnect},
    {"bredr_remote_name", "", bredr_remote_name},
    {"bredr_l2cap_send_test_data", "", bredr_l2cap_send_test_data},
    {"bredr_l2cap_disconnect_req", "", bredr_l2cap_disconnect},
    {"bredr_l2cap_echo_req", "", bredr_l2cap_echo_req},
    {"bredr_security", "", bredr_security},
    #if BR_EDR_PTS_TEST
    {"bredr_sdp_client_connect", "", bredr_sdp_client_connect},
    #endif
        
    #if CONFIG_BT_A2DP
    {"a2dp_connect", "", a2dp_connect},
    #if BR_EDR_PTS_TEST
    {"avdtp_set_conf_reject", "", avdtp_set_conf_reject},
    #endif
    #endif

    #if CONFIG_BT_AVRCP
    {"avrcp_connect", "", avrcp_connect},
    {"avrcp_pth_key", "", avrcp_pth_key},
    {"avrcp_pth_key_act", "", avrcp_pth_key_act},
    {"avrcp_change_vol", "", avrcp_change_vol},
    {"avrcp_play_status", "", avrcp_get_play_status},
    #endif

    #if CONFIG_BT_HFP
    {"hfp_connect", "", hfp_connect},
    {"sco_connect", "", sco_connect},
    {"hfp_answer", "", hfp_answer},
    {"hfp_terminate_call", "", hfp_terminate_call},
    {"hfp_outgoing_call", "", hfp_outgoint_call},
    {"hfp_outgoint_call_with_mem_loc", "", hfp_outgoint_call_with_mem_loc},
    {"hfp_outgoint_call_last_number_dialed", "", hfp_outgoint_call_last_number_dialed},
    {"hfp_disable_nrec", "", hfp_disable_nrec},
    {"hfp_voice_recognition", "", hfp_voice_recognition},
    {"hfp_voice_req_phone_num", "", hfp_voice_req_phone_num},    
    #endif
};


#if PCM_PRINTF
extern OI_BYTE sbc_frame[];
extern OI_UINT32 sbc_frame_len;
OI_INT16 pcm_data[1024];
OI_UINT32 pcm_bytes = sizeof(pcm_data);
OI_INT16 cool_edit[600000];
OI_UINT32 byte_index = 0;
static void pcm(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    //a2dp_sbc_decode_init();
    //a2dp_sbc_decode_process(sbc_frame, sbc_frame_len);

    printf("pcm data count: %d \n", byte_index);

    OI_UINT32 samps = byte_index / sizeof(OI_INT16);

    printf("SAMPLES:    %d\n", samps);
    printf("BITSPERSAMPLE:  16\n");
    printf("CHANNELS:   2\n");
    printf("SAMPLERATE: 44100\n");
    printf("NORMALIZED: FALSE\n");

    for(int i = 0; i < samps; i ++)
    {
        printf("%d\n", cool_edit[i]);
    }

}
#endif

static void bredr_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if(init){
        printf("bredr has initialized\n");
        return;
    }

    default_conn = NULL;
    bt_conn_cb_register(&conn_callbacks);
#if CONFIG_BT_A2DP
    a2dp_cb_register(&a2dp_callbacks);
#endif
#if CONFIG_BT_AVRCP
    avrcp_cb_register(&avrcp_callbacks);
#endif

    init = true;
    printf("bredr init successfully\n");
}


static void bredr_connected(struct bt_conn *conn, u8_t err)
{
    if(err || conn->type != BT_CONN_TYPE_BR)
    {
        return;
    }

    char addr[BT_ADDR_STR_LEN];

    bt_conn_get_info(conn, &conn_info);
    bt_addr_to_str(conn_info.br.dst, addr, sizeof(addr));

    if (err) {
        printf("bredr failed to connect to %s (%u) \r\n", addr, err);
        return;
    }

    printf("bredr connected: %s \r\n", addr);

    if (!default_conn)
    {
        default_conn = conn;
    }

    bt_br_set_connectable(false);

}

static void bredr_disconnected(struct bt_conn *conn, u8_t reason)
{
    if(conn->type != BT_CONN_TYPE_BR)
    {
        return;
    }

    char addr[BT_ADDR_STR_LEN];

    bt_conn_get_info(conn, &conn_info);
    bt_addr_to_str(conn_info.br.dst, addr, sizeof(addr));

    printf("bredr disconnected: %s (reason %u) \r\n", addr, reason);

    if (default_conn == conn)
    {
        default_conn = NULL;
    }
}

static void bredr_write_local_name(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    char *name = "BL-BT";

    err = bt_br_write_local_name(name);
    if (err) {
        printf("BR/EDR write local name failed, (err %d)\n", err);
    } else {
        printf("BR/EDR write local name done.\n");
    }
}

static void bredr_write_eir(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    char *name = "Bouffalolab-classic-bluetooth";
    uint8_t fec = 1;
    uint8_t data[32] = {0};

    data[0] = 30;
    data[1] = 0x09;
    memcpy(data+2, name, strlen(name));

    for(int i = 0; i < strlen(name); i++)
    {
        printf("0x%02x ", data[2+i]);
    }
    printf("\n");

    err = bt_br_write_eir(fec, data);
    if (err) {
        printf("BR/EDR write EIR failed, (err %d)\n", err);
    } else {
        printf("BR/EDR write EIR done.\n");
    }
}

static void bredr_discoverable(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t action;
    
    if(argc != 2){
        printf("Number of parameters is not correct\n");
        return;
    }

    get_uint8_from_string(&argv[1], &action);
    
    if (action == 1) {
        err = bt_br_set_discoverable(true);
    } else if (action == 0) {
        err = bt_br_set_discoverable(false);
    } else {
        printf("Arg1 is invalid\n");
        return;
    }

    if (err) {
        printf("BR/EDR set discoverable failed, (err %d)\n", err);
    } else {
    	printf("BR/EDR set discoverable done.\n");
    }
}

static void bredr_connectable(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t action;
    
    if(argc != 2){
        printf("Number of parameters is not correct\n");
        return;
    }

    get_uint8_from_string(&argv[1], &action);
    
    if (action == 1) {
        err = bt_br_set_connectable(true);
    } else if (action == 0) {
        err = bt_br_set_connectable(false);
    } else {
        printf("Arg1 is invalid\n");
        return;
    }

    if (err) {
        printf("BR/EDR set connectable failed, (err %d)\n", err);
    } else {
    	printf("BR/EDR set connectable done.\n");
    }
}

static void bredr_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    struct bt_conn *conn;
    u8_t  addr_val[6];
    bt_addr_t peer_addr;
    struct bt_br_conn_param param;
    char addr_str[18];

    get_bytearray_from_string(&argv[1], addr_val, 6);
    reverse_bytearray(addr_val, peer_addr.val, 6);

    bt_addr_to_str(&peer_addr, addr_str, sizeof(addr_str));
    printf("%s, create bredr connection with : %s \n", __func__, addr_str);

    param.allow_role_switch = true;

    conn = bt_conn_create_br(&peer_addr, &param);
    if (conn) {
        printf("Connect bredr ACL success.\n");
    } else {
        printf("Connect bredr ACL fail.\n");
    }
}

static void bredr_disconnect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    int err = bt_conn_disconnect(default_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    if (err) {
        printf("Disconnection failed.\n");
    } else {
        printf("Disconnect successfully.\n");
    }

}

void remote_name(const char *name)
{
    if (name) {
        printf("%s, remote name len: %d,  : %s\n", __func__, strlen(name), name);
    } else {
        printf("%s, remote name request fail\n", __func__);
    }
}

static void bredr_remote_name(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    u8_t  addr_val[6];
    bt_addr_t peer_addr;
    char addr_str[18];

    get_bytearray_from_string(&argv[1], addr_val, 6);
    reverse_bytearray(addr_val, peer_addr.val, 6);

    bt_addr_to_str(&peer_addr, addr_str, sizeof(addr_str));
    printf("%s, create bredr connection with : %s \n", __func__, addr_str);

    int err = remote_name_req(&peer_addr, remote_name);
    if (!err) {
        printf("remote name request pending.\n");
    } else {
        printf("remote name request fail.\n");
    }
}

static void bredr_l2cap_send_test_data(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    uint8_t test_data[10]={0x01, 0x02, 0x3,0x04,0x05,0x06,0x07,0x08,0x09,0xa0};
    uint16_t cid;

    get_uint16_from_string(&argv[1], &cid);

    extern int bt_l2cap_send_data(struct bt_conn *conn, uint16_t tx_cid, uint8_t *data, uint8_t len);
    err = bt_l2cap_send_data(default_conn, cid, test_data, 10);

    if(err)
        printf("Fail to send l2cap test data with error (%d)\r\n", err);
    else
        printf("Send l2cap test data successfully\r\n");
}

static void bredr_l2cap_disconnect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    uint16_t tx_cid;
    
    get_uint16_from_string(&argv[1], &tx_cid);

    err = bt_l2cap_disconnect(default_conn, tx_cid);

    if(err)
        printf("Fail to send l2cap disconnect request with error (%d)\r\n", err);
    else
        printf("Send l2cap disconnect request successfully\r\n");
}

static void bredr_l2cap_echo_req(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;

    err = bt_l2cap_br_echo_req(default_conn);

    if(err)
        printf("Fail to send l2cap echo request with error (%d)\r\n", err);
    else
        printf("Send l2cap echo request successfully\r\n");
}

static void bredr_security(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int err;
    u8_t sec_level = BT_SECURITY_L2;

    if(!default_conn){
        printf("Not connected\r\n");
        return;
    }

    if(argc == 2)
        get_uint8_from_string(&argv[1], &sec_level);
    
    err = bt_conn_set_security(default_conn, sec_level);

    if(err){
        printf("Failed to start security, (err %d) \r\n", err);
    }else{
        printf("Start security successfully\r\n");
    }
}


#if BR_EDR_PTS_TEST
extern int bt_sdp_client_connect(struct bt_conn *conn);
static void bredr_sdp_client_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    bt_sdp_client_connect(default_conn);    
}
#endif

#if CONFIG_BT_A2DP
static void a2dp_chain(struct bt_conn *conn, uint8_t state)
{
    printf("%s, conn: %p \n", __func__, conn);

    if (state == BT_A2DP_CHAIN_CONNECTED) {
        printf("a2dp connected. \n");
    } else if (state == BT_A2DP_CHAIN_DISCONNECTED) {
        printf("a2dp disconnected. \n");
    }
}

static void a2dp_stream(uint8_t state)
{
    printf("%s, state: %d \n", __func__, state);

    if (state == BT_A2DP_STREAM_START) {
        printf("a2dp play. \n");
    } else if (state == BT_A2DP_STREAM_SUSPEND) {
        printf("a2dp stop. \n");
    }
}

static void a2dp_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_connect(default_conn);
    if(ret) {
        printf("a2dp connect successfully.\n");
    } else {
        printf("a2dp connect fail. \n");
    }
}

#if BR_EDR_PTS_TEST
extern uint8_t reject_set_conf_pts;
extern uint8_t reject_error_code;
static void avdtp_set_conf_reject(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    get_uint8_from_string(&argv[1], &reject_set_conf_pts);
    get_uint8_from_string(&argv[2], &reject_error_code);
}

#endif

#endif

#if CONFIG_BT_AVRCP
static void avrcp_chain(struct bt_conn *conn, uint8_t state)
{
    printf("%s, conn: %p \n", __func__, conn);

    if (state == BT_AVRCP_CHAIN_CONNECTED) {
        printf("avrcp connected. \n");
    } else if (state == BT_AVRCP_CHAIN_DISCONNECTED) {
        printf("avrcp disconnected. \n");
    }
}

static void avrcp_absvol(uint8_t vol)
{
    printf("%s, vol: %d \n", __func__, vol);
}

static void avrcp_play_status(uint32_t song_len, uint32_t song_pos, uint8_t status)
{
    printf("%s, song length: %d, song position: %d, play status: %d \n", __func__, song_len, song_pos, status);
}

static void avrcp_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = bt_avrcp_connect(default_conn);
    if(err) {
        printf("avrcp connect failed, err: %d\n", err);
    } else {
        printf("avrcp connect successfully.\n");
    }
}

static void avrcp_pth_key(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t key;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    get_uint8_from_string(&argv[1], &key);

    err = avrcp_pasthr_cmd(NULL, PASTHR_STATE_PRESSED, key);
    if(err) {
        printf("avrcp key pressed failed, err: %d\n", err);
    } else {
        printf("avrcp key pressed successfully.\n");
    }

    err = avrcp_pasthr_cmd(NULL, PASTHR_STATE_RELEASED, key);
    if(err) {
        printf("avrcp key released failed, err: %d\n", err);
    } else {
        printf("avrcp key play released successfully.\n");
    }
}

static void avrcp_pth_key_act(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t key;
    uint8_t action;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    get_uint8_from_string(&argv[1], &key);
    get_uint8_from_string(&argv[2], &action);

    if (action != PASTHR_STATE_PRESSED && action != PASTHR_STATE_RELEASED)
    {
        printf("invalid key action.\n");
        return;
    }

    err = avrcp_pasthr_cmd(NULL, action, key);
    if(err) {
        printf("avrcp key action failed, err: %d\n", err);
    } else {
        printf("avrcp %s key %d successfully.\n", action ? "released":"pressed", key);
    }
}

static void avrcp_change_vol(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t vol;
    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    get_uint8_from_string(&argv[1], &vol);
    err = avrcp_change_volume(vol);
    if (err) {
        printf("avrcp change volume fail\n");
    } else {
        printf("avrcp change volume success\n");
    }
}

static void avrcp_get_play_status(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = avrcp_get_play_status_cmd(NULL);
    if(err) {
        printf("avrcp get play status fail\n");
    } else {
        printf("avrcp get play status success\n");
    }
}

#endif

#if CONFIG_BT_HFP
#if 0
static void rfcomm_recv(struct bt_rfcomm_dlc *dlci, struct net_buf *buf)
{
	printf("hfp incoming data dlc %p len %u \n", dlci, buf->len);
}

static void rfcomm_connected(struct bt_rfcomm_dlc *dlci)
{
	printf("hfp dlc %p connected \n", dlci);
}

static void rfcomm_disconnected(struct bt_rfcomm_dlc *dlci)
{
	printf("hfp dlc %p disconnected \n", dlci);
}

static struct bt_rfcomm_dlc_ops rfcomm_ops = {
	.recv		= rfcomm_recv,
	.connected	= rfcomm_connected,
	.disconnected	= rfcomm_disconnected,
};

static struct bt_rfcomm_dlc rfcomm_dlc = {
	.ops = &rfcomm_ops,
	.mtu = 30,
};
#endif
static void hfp_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = bt_hfp_hf_initiate_connect(default_conn);
    if (err) {
        printf("hfp connect fail.\n");
    } else {
        printf("hfp connect pending.\n");
    }
}

static void sco_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    struct bt_conn *conn;
    u8_t  addr_val[6];
    bt_addr_t peer_addr;
    char addr_str[18];

    get_bytearray_from_string(&argv[1], addr_val, 6);
    reverse_bytearray(addr_val, peer_addr.val, 6);

    bt_addr_to_str(&peer_addr, addr_str, sizeof(addr_str));
    printf("%s, create sco connection with : %s \n", __func__, addr_str);

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    conn = bt_conn_create_sco(&peer_addr);
    if (!conn) {
        printf("sco connect fail.\n");
    } else {
        printf("sco connect success.\n");
    }
}

static void hfp_answer(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_ATA, NULL);
    if(err)
        printf("Fail to send answer AT command with err:%d\r\n", err);
    else
        printf("send answer AT command successfully\r\n");
        
}

static void hfp_terminate_call(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_CHUP, NULL);
    if(err)
        printf("Fail to send terminate call AT command with err:%d\r\n", err);
    else
        printf("send terminate call AT command successfully\r\n");
        
}

static void hfp_outgoint_call(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_DDD, "D1234567;");
    if(err)
        printf("Fail to send outgoing call AT command with err:%d\r\n", err);
    else
        printf("send outgoing call AT command successfully\r\n");
        
}

static void hfp_outgoint_call_with_mem_loc(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    uint8_t phone_mem_loc = 0;
    char str[5] = "D>";
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    get_uint8_from_string(&argv[1], &phone_mem_loc);
    sprintf(str, ">%d;", phone_mem_loc);
    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_DDD, str);
    
    if(err)
        printf("Fail to send outgoing call with memory location AT command with err:%d\r\n", err);
    else
        printf("send outgoing call with memory location AT command successfully\r\n");
        
}

static void hfp_outgoint_call_last_number_dialed(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    uint8_t phone_mem_loc = 0;
    char *str = "+BLDN";
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_DDD, str);
    
    if(err)
        printf("Fail to send outgoing call to the last number dialed AT command with err:%d\r\n", err);
    else
        printf("send outgoing call to the last number dialed  AT command successfully\r\n");
        
}

static void hfp_disable_nrec(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_NREC, NULL);
    if(err)
        printf("Fail to send disable nrec AT command with err:%d\r\n", err);
    else
        printf("send disable nrec AT command successfully\r\n");
        
}

static void hfp_voice_recognition(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    uint8_t enable = 0;

    get_uint8_from_string(&argv[1], &enable);
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd_arg(default_conn, BT_HFP_HF_AT_BVRA, enable);
    if(err)
        printf("Fail to send voice recognition AT command with err:%d\r\n", err);
    else
        printf("send voice recognition AT command successfully\r\n");
}

static void hfp_voice_req_phone_num(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    uint8_t enable = 0;

    get_uint8_from_string(&argv[1], &enable);
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd_arg(default_conn, BT_HFP_HF_AT_BINP, enable);
    if(err)
        printf("Fail to send reqeust phone number to the AG AT command with err:%d\r\n", err);
    else
        printf("send reqeust phone number to the AG AT command successfully\r\n");
}

#endif

int bredr_cli_register(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(bredr_cmd_set, sizeof(bredr_cmd_set)/sizeof(bredr_cmd_set[0]));
    return 0;
}	       
