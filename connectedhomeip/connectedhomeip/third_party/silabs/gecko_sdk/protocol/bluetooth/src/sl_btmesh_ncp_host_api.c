/***************************************************************************//**
 * @brief SL_BT_API commands for NCP host
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "sl_btmesh_api.h"
#include "sli_btmesh_api.h"

void sl_btmesh_host_handle_command();
void sl_btmesh_host_handle_command_noresponse();
extern sl_btmesh_msg_t *sl_btmesh_cmd_msg;
extern sl_btmesh_msg_t *sl_btmesh_rsp_msg;

sl_status_t sl_btmesh_node_init() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_init_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_init.result;

}

sl_status_t sl_btmesh_node_set_exportable_keys() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_set_exportable_keys_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_exportable_keys.result;

}

sl_status_t sl_btmesh_node_start_unprov_beaconing(uint8_t bearer) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_start_unprov_beaconing.bearer=bearer;

    cmd->header=sl_btmesh_cmd_node_start_unprov_beaconing_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_start_unprov_beaconing.result;

}

sl_status_t sl_btmesh_node_stop_unprov_beaconing() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_stop_unprov_beaconing_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_stop_unprov_beaconing.result;

}

sl_status_t sl_btmesh_node_get_rssi(int8_t *rssi) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_get_rssi_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *rssi = rsp->data.rsp_node_get_rssi.rssi;
    return rsp->data.rsp_node_get_rssi.result;

}

sl_status_t sl_btmesh_node_send_input_oob_request_response(size_t data_len,
                                                           const uint8_t* data) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    if ((1+data_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_node_send_input_oob_request_response.data.len=data_len;
    memcpy(cmd->data.cmd_node_send_input_oob_request_response.data.data,data,data_len);

    cmd->header=sl_btmesh_cmd_node_send_input_oob_request_response_id+(((1+data_len)&0xff)<<8)+(((1+data_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_send_input_oob_request_response.result;

}

sl_status_t sl_btmesh_node_get_uuid(uuid_128 *uuid) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_get_uuid_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *uuid = rsp->data.rsp_node_get_uuid.uuid;
    return rsp->data.rsp_node_get_uuid.result;

}

sl_status_t sl_btmesh_node_set_provisioning_data(aes_key_128 device_key,
                                                 aes_key_128 network_key,
                                                 uint16_t netkey_index,
                                                 uint32_t iv_index,
                                                 uint16_t address,
                                                 uint8_t kr_in_progress) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    memcpy(&cmd->data.cmd_node_set_provisioning_data.device_key,&device_key,sizeof(aes_key_128));
    memcpy(&cmd->data.cmd_node_set_provisioning_data.network_key,&network_key,sizeof(aes_key_128));
    cmd->data.cmd_node_set_provisioning_data.netkey_index=netkey_index;
    cmd->data.cmd_node_set_provisioning_data.iv_index=iv_index;
    cmd->data.cmd_node_set_provisioning_data.address=address;
    cmd->data.cmd_node_set_provisioning_data.kr_in_progress=kr_in_progress;

    cmd->header=sl_btmesh_cmd_node_set_provisioning_data_id+(((41)&0xff)<<8)+(((41)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_provisioning_data.result;

}

sl_status_t sl_btmesh_node_init_oob(uint8_t public_key,
                                    uint8_t auth_methods,
                                    uint16_t output_actions,
                                    uint8_t output_size,
                                    uint16_t input_actions,
                                    uint8_t input_size,
                                    uint16_t oob_location) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_init_oob.public_key=public_key;
    cmd->data.cmd_node_init_oob.auth_methods=auth_methods;
    cmd->data.cmd_node_init_oob.output_actions=output_actions;
    cmd->data.cmd_node_init_oob.output_size=output_size;
    cmd->data.cmd_node_init_oob.input_actions=input_actions;
    cmd->data.cmd_node_init_oob.input_size=input_size;
    cmd->data.cmd_node_init_oob.oob_location=oob_location;

    cmd->header=sl_btmesh_cmd_node_init_oob_id+(((10)&0xff)<<8)+(((10)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_init_oob.result;

}

sl_status_t sl_btmesh_node_set_ivrecovery_mode(uint8_t mode) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_set_ivrecovery_mode.mode=mode;

    cmd->header=sl_btmesh_cmd_node_set_ivrecovery_mode_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_ivrecovery_mode.result;

}

sl_status_t sl_btmesh_node_get_ivrecovery_mode(uint8_t *mode) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_get_ivrecovery_mode_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *mode = rsp->data.rsp_node_get_ivrecovery_mode.mode;
    return rsp->data.rsp_node_get_ivrecovery_mode.result;

}

sl_status_t sl_btmesh_node_get_statistics(size_t max_statistics_size,
                                          size_t *statistics_len,
                                          uint8_t *statistics) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_get_statistics_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *statistics_len = rsp->data.rsp_node_get_statistics.statistics.len;
    if (rsp->data.rsp_node_get_statistics.statistics.len <= max_statistics_size) {
        memcpy(statistics,rsp->data.rsp_node_get_statistics.statistics.data,rsp->data.rsp_node_get_statistics.statistics.len);
    }
    return rsp->data.rsp_node_get_statistics.result;

}

sl_status_t sl_btmesh_node_clear_statistics() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_clear_statistics_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_clear_statistics.result;

}

sl_status_t sl_btmesh_node_set_net_relay_delay(uint8_t min_ms, uint8_t max_ms) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_set_net_relay_delay.min_ms=min_ms;
    cmd->data.cmd_node_set_net_relay_delay.max_ms=max_ms;

    cmd->header=sl_btmesh_cmd_node_set_net_relay_delay_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_net_relay_delay.result;

}

sl_status_t sl_btmesh_node_get_net_relay_delay(uint8_t *min_ms,
                                               uint8_t *max_ms) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_get_net_relay_delay_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *min_ms = rsp->data.rsp_node_get_net_relay_delay.min_ms;
    *max_ms = rsp->data.rsp_node_get_net_relay_delay.max_ms;
    return rsp->data.rsp_node_get_net_relay_delay.result;

}

sl_status_t sl_btmesh_node_get_ivupdate_state(uint32_t *iv_index,
                                              uint8_t *state) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_get_ivupdate_state_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *iv_index = rsp->data.rsp_node_get_ivupdate_state.iv_index;
    *state = rsp->data.rsp_node_get_ivupdate_state.state;
    return rsp->data.rsp_node_get_ivupdate_state.result;

}

sl_status_t sl_btmesh_node_request_ivupdate() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_request_ivupdate_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_request_ivupdate.result;

}

sl_status_t sl_btmesh_node_get_seq_remaining(uint16_t elem_index,
                                             uint32_t *count) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_get_seq_remaining.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_node_get_seq_remaining_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *count = rsp->data.rsp_node_get_seq_remaining.count;
    return rsp->data.rsp_node_get_seq_remaining.result;

}

sl_status_t sl_btmesh_node_save_replay_protection_list() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_save_replay_protection_list_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_save_replay_protection_list.result;

}

sl_status_t sl_btmesh_node_set_uuid(uuid_128 uuid) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_set_uuid.uuid=uuid;

    cmd->header=sl_btmesh_cmd_node_set_uuid_id+(((16)&0xff)<<8)+(((16)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_uuid.result;

}

sl_status_t sl_btmesh_node_get_replay_protection_list_status(uint16_t *total_entries,
                                                             uint16_t *unsaved_entries) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_get_replay_protection_list_status_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *total_entries = rsp->data.rsp_node_get_replay_protection_list_status.total_entries;
    *unsaved_entries = rsp->data.rsp_node_get_replay_protection_list_status.unsaved_entries;
    return rsp->data.rsp_node_get_replay_protection_list_status.result;

}

sl_status_t sl_btmesh_node_get_element_address(uint16_t elem_index,
                                               uint16_t *address) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_get_element_address.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_node_get_element_address_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *address = rsp->data.rsp_node_get_element_address.address;
    return rsp->data.rsp_node_get_element_address.result;

}

sl_status_t sl_btmesh_node_send_static_oob_request_response(size_t data_len,
                                                            const uint8_t* data) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    if ((1+data_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_node_send_static_oob_request_response.data.len=data_len;
    memcpy(cmd->data.cmd_node_send_static_oob_request_response.data.data,data,data_len);

    cmd->header=sl_btmesh_cmd_node_send_static_oob_request_response_id+(((1+data_len)&0xff)<<8)+(((1+data_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_send_static_oob_request_response.result;

}

sl_status_t sl_btmesh_node_reset() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_reset_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_reset.result;

}

sl_status_t sl_btmesh_node_set_beacon_reporting(uint8_t report) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_set_beacon_reporting.report=report;

    cmd->header=sl_btmesh_cmd_node_set_beacon_reporting_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_beacon_reporting.result;

}

sl_status_t sl_btmesh_node_set_iv_update_age(uint32_t age_sec) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_set_iv_update_age.age_sec=age_sec;

    cmd->header=sl_btmesh_cmd_node_set_iv_update_age_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_iv_update_age.result;

}

sl_status_t sl_btmesh_node_get_key_count(uint8_t type, uint32_t *count) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_get_key_count.type=type;

    cmd->header=sl_btmesh_cmd_node_get_key_count_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *count = rsp->data.rsp_node_get_key_count.count;
    return rsp->data.rsp_node_get_key_count.result;

}

sl_status_t sl_btmesh_node_get_key(uint8_t type,
                                   uint32_t index,
                                   uint8_t current,
                                   uint16_t *id,
                                   uint16_t *netkey_index,
                                   aes_key_128 *key) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_get_key.type=type;
    cmd->data.cmd_node_get_key.index=index;
    cmd->data.cmd_node_get_key.current=current;

    cmd->header=sl_btmesh_cmd_node_get_key_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *id = rsp->data.rsp_node_get_key.id;
    *netkey_index = rsp->data.rsp_node_get_key.netkey_index;
    memcpy(key,&rsp->data.rsp_node_get_key.key,sizeof(aes_key_128));
    return rsp->data.rsp_node_get_key.result;

}

sl_status_t sl_btmesh_node_get_networks(size_t max_networks_size,
                                        size_t *networks_len,
                                        uint8_t *networks) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_get_networks_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *networks_len = rsp->data.rsp_node_get_networks.networks.len;
    if (rsp->data.rsp_node_get_networks.networks.len <= max_networks_size) {
        memcpy(networks,rsp->data.rsp_node_get_networks.networks.data,rsp->data.rsp_node_get_networks.networks.len);
    }
    return rsp->data.rsp_node_get_networks.result;

}

sl_status_t sl_btmesh_node_get_element_seqnum(uint16_t elem_index,
                                              uint32_t *seqnum) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_get_element_seqnum.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_node_get_element_seqnum_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *seqnum = rsp->data.rsp_node_get_element_seqnum.seqnum;
    return rsp->data.rsp_node_get_element_seqnum.result;

}

sl_status_t sl_btmesh_node_set_model_option(uint16_t elem_index,
                                            uint16_t vendor_id,
                                            uint16_t model_id,
                                            uint8_t option,
                                            uint32_t value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_set_model_option.elem_index=elem_index;
    cmd->data.cmd_node_set_model_option.vendor_id=vendor_id;
    cmd->data.cmd_node_set_model_option.model_id=model_id;
    cmd->data.cmd_node_set_model_option.option=option;
    cmd->data.cmd_node_set_model_option.value=value;

    cmd->header=sl_btmesh_cmd_node_set_model_option_id+(((11)&0xff)<<8)+(((11)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_model_option.result;

}

sl_status_t sl_btmesh_node_get_local_dcd(uint8_t page) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_get_local_dcd.page=page;

    cmd->header=sl_btmesh_cmd_node_get_local_dcd_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_get_local_dcd.result;

}

sl_status_t sl_btmesh_node_power_off() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_power_off_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_power_off.result;

}

sl_status_t sl_btmesh_node_set_adv_phy(uint8_t phy) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_set_adv_phy.phy=phy;

    cmd->header=sl_btmesh_cmd_node_set_adv_phy_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_adv_phy.result;

}

sl_status_t sl_btmesh_node_get_adv_phy(uint8_t *phy) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_node_get_adv_phy_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *phy = rsp->data.rsp_node_get_adv_phy.phy;
    return rsp->data.rsp_node_get_adv_phy.result;

}

sl_status_t sl_btmesh_node_set_unprov_beaconing_adv_interval(uint16_t adv_interval_min,
                                                             uint16_t adv_interval_max) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_set_unprov_beaconing_adv_interval.adv_interval_min=adv_interval_min;
    cmd->data.cmd_node_set_unprov_beaconing_adv_interval.adv_interval_max=adv_interval_max;

    cmd->header=sl_btmesh_cmd_node_set_unprov_beaconing_adv_interval_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_unprov_beaconing_adv_interval.result;

}

sl_status_t sl_btmesh_node_set_proxy_service_adv_interval(uint16_t adv_interval_min,
                                                          uint16_t adv_interval_max) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_set_proxy_service_adv_interval.adv_interval_min=adv_interval_min;
    cmd->data.cmd_node_set_proxy_service_adv_interval.adv_interval_max=adv_interval_max;

    cmd->header=sl_btmesh_cmd_node_set_proxy_service_adv_interval_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_proxy_service_adv_interval.result;

}

sl_status_t sl_btmesh_node_set_provisioning_service_adv_interval(uint16_t adv_interval_min,
                                                                 uint16_t adv_interval_max) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_node_set_provisioning_service_adv_interval.adv_interval_min=adv_interval_min;
    cmd->data.cmd_node_set_provisioning_service_adv_interval.adv_interval_max=adv_interval_max;

    cmd->header=sl_btmesh_cmd_node_set_provisioning_service_adv_interval_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_node_set_provisioning_service_adv_interval.result;

}

sl_status_t sl_btmesh_prov_init() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_prov_init_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_init.result;

}

sl_status_t sl_btmesh_prov_scan_unprov_beacons() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_prov_scan_unprov_beacons_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_scan_unprov_beacons.result;

}

sl_status_t sl_btmesh_prov_create_provisioning_session(uint16_t netkey_index,
                                                       uuid_128 uuid,
                                                       uint8_t attention_timer_sec) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_create_provisioning_session.netkey_index=netkey_index;
    cmd->data.cmd_prov_create_provisioning_session.uuid=uuid;
    cmd->data.cmd_prov_create_provisioning_session.attention_timer_sec=attention_timer_sec;

    cmd->header=sl_btmesh_cmd_prov_create_provisioning_session_id+(((19)&0xff)<<8)+(((19)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_create_provisioning_session.result;

}

sl_status_t sl_btmesh_prov_set_provisioning_suspend_event(uint8_t status) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_set_provisioning_suspend_event.status=status;

    cmd->header=sl_btmesh_cmd_prov_set_provisioning_suspend_event_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_set_provisioning_suspend_event.result;

}

sl_status_t sl_btmesh_prov_provision_adv_device(uuid_128 uuid) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_provision_adv_device.uuid=uuid;

    cmd->header=sl_btmesh_cmd_prov_provision_adv_device_id+(((16)&0xff)<<8)+(((16)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_provision_adv_device.result;

}

sl_status_t sl_btmesh_prov_provision_gatt_device(uuid_128 uuid,
                                                 uint8_t connection) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_provision_gatt_device.uuid=uuid;
    cmd->data.cmd_prov_provision_gatt_device.connection=connection;

    cmd->header=sl_btmesh_cmd_prov_provision_gatt_device_id+(((17)&0xff)<<8)+(((17)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_provision_gatt_device.result;

}

sl_status_t sl_btmesh_prov_continue_provisioning(uuid_128 uuid) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_continue_provisioning.uuid=uuid;

    cmd->header=sl_btmesh_cmd_prov_continue_provisioning_id+(((16)&0xff)<<8)+(((16)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_continue_provisioning.result;

}

sl_status_t sl_btmesh_prov_abort_provisioning(uuid_128 uuid, uint8_t reason) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_abort_provisioning.uuid=uuid;
    cmd->data.cmd_prov_abort_provisioning.reason=reason;

    cmd->header=sl_btmesh_cmd_prov_abort_provisioning_id+(((17)&0xff)<<8)+(((17)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_abort_provisioning.result;

}

sl_status_t sl_btmesh_prov_set_device_address(uuid_128 uuid, uint16_t address) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_set_device_address.uuid=uuid;
    cmd->data.cmd_prov_set_device_address.address=address;

    cmd->header=sl_btmesh_cmd_prov_set_device_address_id+(((18)&0xff)<<8)+(((18)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_set_device_address.result;

}

sl_status_t sl_btmesh_prov_create_network(uint16_t netkey_index,
                                          size_t key_len,
                                          const uint8_t* key) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_create_network.netkey_index=netkey_index;
    if ((3+key_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_prov_create_network.key.len=key_len;
    memcpy(cmd->data.cmd_prov_create_network.key.data,key,key_len);

    cmd->header=sl_btmesh_cmd_prov_create_network_id+(((3+key_len)&0xff)<<8)+(((3+key_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_create_network.result;

}

sl_status_t sl_btmesh_prov_create_appkey(uint16_t netkey_index,
                                         uint16_t appkey_index,
                                         size_t key_len,
                                         const uint8_t* key,
                                         size_t max_application_key_size,
                                         size_t *application_key_len,
                                         uint8_t *application_key) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_create_appkey.netkey_index=netkey_index;
    cmd->data.cmd_prov_create_appkey.appkey_index=appkey_index;
    if ((5+key_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_prov_create_appkey.key.len=key_len;
    memcpy(cmd->data.cmd_prov_create_appkey.key.data,key,key_len);

    cmd->header=sl_btmesh_cmd_prov_create_appkey_id+(((5+key_len)&0xff)<<8)+(((5+key_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *application_key_len = rsp->data.rsp_prov_create_appkey.application_key.len;
    if (rsp->data.rsp_prov_create_appkey.application_key.len <= max_application_key_size) {
        memcpy(application_key,rsp->data.rsp_prov_create_appkey.application_key.data,rsp->data.rsp_prov_create_appkey.application_key.len);
    }
    return rsp->data.rsp_prov_create_appkey.result;

}

sl_status_t sl_btmesh_prov_send_oob_pkey_response(uuid_128 uuid,
                                                  size_t pkey_len,
                                                  const uint8_t* pkey) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_send_oob_pkey_response.uuid=uuid;
    if ((17+pkey_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_prov_send_oob_pkey_response.pkey.len=pkey_len;
    memcpy(cmd->data.cmd_prov_send_oob_pkey_response.pkey.data,pkey,pkey_len);

    cmd->header=sl_btmesh_cmd_prov_send_oob_pkey_response_id+(((17+pkey_len)&0xff)<<8)+(((17+pkey_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_send_oob_pkey_response.result;

}

sl_status_t sl_btmesh_prov_send_oob_auth_response(uuid_128 uuid,
                                                  size_t data_len,
                                                  const uint8_t* data) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_send_oob_auth_response.uuid=uuid;
    if ((17+data_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_prov_send_oob_auth_response.data.len=data_len;
    memcpy(cmd->data.cmd_prov_send_oob_auth_response.data.data,data,data_len);

    cmd->header=sl_btmesh_cmd_prov_send_oob_auth_response_id+(((17+data_len)&0xff)<<8)+(((17+data_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_send_oob_auth_response.result;

}

sl_status_t sl_btmesh_prov_set_oob_requirements(uuid_128 uuid,
                                                uint8_t public_key,
                                                uint8_t auth_methods,
                                                uint16_t output_actions,
                                                uint16_t input_actions,
                                                uint8_t min_size,
                                                uint8_t max_size) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_set_oob_requirements.uuid=uuid;
    cmd->data.cmd_prov_set_oob_requirements.public_key=public_key;
    cmd->data.cmd_prov_set_oob_requirements.auth_methods=auth_methods;
    cmd->data.cmd_prov_set_oob_requirements.output_actions=output_actions;
    cmd->data.cmd_prov_set_oob_requirements.input_actions=input_actions;
    cmd->data.cmd_prov_set_oob_requirements.min_size=min_size;
    cmd->data.cmd_prov_set_oob_requirements.max_size=max_size;

    cmd->header=sl_btmesh_cmd_prov_set_oob_requirements_id+(((24)&0xff)<<8)+(((24)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_set_oob_requirements.result;

}

sl_status_t sl_btmesh_prov_start_key_refresh(uint16_t netkey_index,
                                             uint8_t num_appkeys,
                                             size_t appkey_indices_len,
                                             const uint8_t* appkey_indices) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_start_key_refresh.netkey_index=netkey_index;
    cmd->data.cmd_prov_start_key_refresh.num_appkeys=num_appkeys;
    if ((4+appkey_indices_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_prov_start_key_refresh.appkey_indices.len=appkey_indices_len;
    memcpy(cmd->data.cmd_prov_start_key_refresh.appkey_indices.data,appkey_indices,appkey_indices_len);

    cmd->header=sl_btmesh_cmd_prov_start_key_refresh_id+(((4+appkey_indices_len)&0xff)<<8)+(((4+appkey_indices_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_start_key_refresh.result;

}

sl_status_t sl_btmesh_prov_get_key_refresh_exclusion(uint16_t key,
                                                     uuid_128 uuid,
                                                     uint8_t *status) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_get_key_refresh_exclusion.key=key;
    cmd->data.cmd_prov_get_key_refresh_exclusion.uuid=uuid;

    cmd->header=sl_btmesh_cmd_prov_get_key_refresh_exclusion_id+(((18)&0xff)<<8)+(((18)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *status = rsp->data.rsp_prov_get_key_refresh_exclusion.status;
    return rsp->data.rsp_prov_get_key_refresh_exclusion.result;

}

sl_status_t sl_btmesh_prov_set_key_refresh_exclusion(uint16_t key,
                                                     uint8_t status,
                                                     uuid_128 uuid) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_set_key_refresh_exclusion.key=key;
    cmd->data.cmd_prov_set_key_refresh_exclusion.status=status;
    cmd->data.cmd_prov_set_key_refresh_exclusion.uuid=uuid;

    cmd->header=sl_btmesh_cmd_prov_set_key_refresh_exclusion_id+(((19)&0xff)<<8)+(((19)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_set_key_refresh_exclusion.result;

}

sl_status_t sl_btmesh_prov_get_ddb_entry(uuid_128 uuid,
                                         aes_key_128 *device_key,
                                         uint16_t *netkey_index,
                                         uint16_t *address,
                                         uint8_t *elements) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_get_ddb_entry.uuid=uuid;

    cmd->header=sl_btmesh_cmd_prov_get_ddb_entry_id+(((16)&0xff)<<8)+(((16)&0x700)>>8);


    sl_btmesh_host_handle_command();
    memcpy(device_key,&rsp->data.rsp_prov_get_ddb_entry.device_key,sizeof(aes_key_128));
    *netkey_index = rsp->data.rsp_prov_get_ddb_entry.netkey_index;
    *address = rsp->data.rsp_prov_get_ddb_entry.address;
    *elements = rsp->data.rsp_prov_get_ddb_entry.elements;
    return rsp->data.rsp_prov_get_ddb_entry.result;

}

sl_status_t sl_btmesh_prov_delete_ddb_entry(uuid_128 uuid) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_delete_ddb_entry.uuid=uuid;

    cmd->header=sl_btmesh_cmd_prov_delete_ddb_entry_id+(((16)&0xff)<<8)+(((16)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_delete_ddb_entry.result;

}

sl_status_t sl_btmesh_prov_add_ddb_entry(uuid_128 uuid,
                                         aes_key_128 device_key,
                                         uint16_t netkey_index,
                                         uint16_t address,
                                         uint8_t elements) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_add_ddb_entry.uuid=uuid;
    memcpy(&cmd->data.cmd_prov_add_ddb_entry.device_key,&device_key,sizeof(aes_key_128));
    cmd->data.cmd_prov_add_ddb_entry.netkey_index=netkey_index;
    cmd->data.cmd_prov_add_ddb_entry.address=address;
    cmd->data.cmd_prov_add_ddb_entry.elements=elements;

    cmd->header=sl_btmesh_cmd_prov_add_ddb_entry_id+(((37)&0xff)<<8)+(((37)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_add_ddb_entry.result;

}

sl_status_t sl_btmesh_prov_list_ddb_entries(uint16_t *count) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_prov_list_ddb_entries_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *count = rsp->data.rsp_prov_list_ddb_entries.count;
    return rsp->data.rsp_prov_list_ddb_entries.result;

}

sl_status_t sl_btmesh_prov_initialize_network(uint16_t address, uint32_t ivi) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_initialize_network.address=address;
    cmd->data.cmd_prov_initialize_network.ivi=ivi;

    cmd->header=sl_btmesh_cmd_prov_initialize_network_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_initialize_network.result;

}

sl_status_t sl_btmesh_prov_get_key_refresh_appkey_exclusion(uint16_t netkey_index,
                                                            uint16_t appkey_index,
                                                            uuid_128 uuid,
                                                            uint8_t *status) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_get_key_refresh_appkey_exclusion.netkey_index=netkey_index;
    cmd->data.cmd_prov_get_key_refresh_appkey_exclusion.appkey_index=appkey_index;
    cmd->data.cmd_prov_get_key_refresh_appkey_exclusion.uuid=uuid;

    cmd->header=sl_btmesh_cmd_prov_get_key_refresh_appkey_exclusion_id+(((20)&0xff)<<8)+(((20)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *status = rsp->data.rsp_prov_get_key_refresh_appkey_exclusion.status;
    return rsp->data.rsp_prov_get_key_refresh_appkey_exclusion.result;

}

sl_status_t sl_btmesh_prov_set_key_refresh_appkey_exclusion(uint16_t netkey_index,
                                                            uint16_t appkey_index,
                                                            uint8_t status,
                                                            uuid_128 uuid) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_set_key_refresh_appkey_exclusion.netkey_index=netkey_index;
    cmd->data.cmd_prov_set_key_refresh_appkey_exclusion.appkey_index=appkey_index;
    cmd->data.cmd_prov_set_key_refresh_appkey_exclusion.status=status;
    cmd->data.cmd_prov_set_key_refresh_appkey_exclusion.uuid=uuid;

    cmd->header=sl_btmesh_cmd_prov_set_key_refresh_appkey_exclusion_id+(((21)&0xff)<<8)+(((21)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_set_key_refresh_appkey_exclusion.result;

}

sl_status_t sl_btmesh_prov_stop_scan_unprov_beacons() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_prov_stop_scan_unprov_beacons_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_stop_scan_unprov_beacons.result;

}

sl_status_t sl_btmesh_prov_update_device_netkey_index(uuid_128 uuid,
                                                      uint16_t netkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_update_device_netkey_index.uuid=uuid;
    cmd->data.cmd_prov_update_device_netkey_index.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_prov_update_device_netkey_index_id+(((18)&0xff)<<8)+(((18)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_update_device_netkey_index.result;

}

sl_status_t sl_btmesh_prov_suspend_key_refresh(uint16_t netkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_suspend_key_refresh.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_prov_suspend_key_refresh_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_suspend_key_refresh.result;

}

sl_status_t sl_btmesh_prov_resume_key_refresh(uint16_t netkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_resume_key_refresh.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_prov_resume_key_refresh_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_resume_key_refresh.result;

}

sl_status_t sl_btmesh_prov_get_key_refresh_phase(uint16_t netkey_index,
                                                 uint8_t *phase) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_get_key_refresh_phase.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_prov_get_key_refresh_phase_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *phase = rsp->data.rsp_prov_get_key_refresh_phase.phase;
    return rsp->data.rsp_prov_get_key_refresh_phase.result;

}

sl_status_t sl_btmesh_prov_start_key_refresh_from_phase(uint8_t phase,
                                                        uint16_t netkey_index,
                                                        uint8_t num_appkeys,
                                                        size_t appkey_indices_len,
                                                        const uint8_t* appkey_indices) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_start_key_refresh_from_phase.phase=phase;
    cmd->data.cmd_prov_start_key_refresh_from_phase.netkey_index=netkey_index;
    cmd->data.cmd_prov_start_key_refresh_from_phase.num_appkeys=num_appkeys;
    if ((5+appkey_indices_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_prov_start_key_refresh_from_phase.appkey_indices.len=appkey_indices_len;
    memcpy(cmd->data.cmd_prov_start_key_refresh_from_phase.appkey_indices.data,appkey_indices,appkey_indices_len);

    cmd->header=sl_btmesh_cmd_prov_start_key_refresh_from_phase_id+(((5+appkey_indices_len)&0xff)<<8)+(((5+appkey_indices_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_start_key_refresh_from_phase.result;

}

sl_status_t sl_btmesh_prov_flush_key_refresh_state(uint16_t netkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_flush_key_refresh_state.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_prov_flush_key_refresh_state_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_prov_flush_key_refresh_state.result;

}

sl_status_t sl_btmesh_prov_test_identity(uint16_t address,
                                         uint16_t netkey_index,
                                         size_t data_len,
                                         const uint8_t* data,
                                         uint8_t *match) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_prov_test_identity.address=address;
    cmd->data.cmd_prov_test_identity.netkey_index=netkey_index;
    if ((5+data_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_prov_test_identity.data.len=data_len;
    memcpy(cmd->data.cmd_prov_test_identity.data.data,data,data_len);

    cmd->header=sl_btmesh_cmd_prov_test_identity_id+(((5+data_len)&0xff)<<8)+(((5+data_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *match = rsp->data.rsp_prov_test_identity.match;
    return rsp->data.rsp_prov_test_identity.result;

}

sl_status_t sl_btmesh_proxy_connect(uint8_t connection, uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_proxy_connect.connection=connection;

    cmd->header=sl_btmesh_cmd_proxy_connect_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_proxy_connect.handle;
    return rsp->data.rsp_proxy_connect.result;

}

sl_status_t sl_btmesh_proxy_disconnect(uint32_t handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_proxy_disconnect.handle=handle;

    cmd->header=sl_btmesh_cmd_proxy_disconnect_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_proxy_disconnect.result;

}

sl_status_t sl_btmesh_proxy_set_filter_type(uint32_t handle,
                                            uint16_t netkey_index,
                                            uint8_t type) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_proxy_set_filter_type.handle=handle;
    cmd->data.cmd_proxy_set_filter_type.netkey_index=netkey_index;
    cmd->data.cmd_proxy_set_filter_type.type=type;

    cmd->header=sl_btmesh_cmd_proxy_set_filter_type_id+(((7)&0xff)<<8)+(((7)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_proxy_set_filter_type.result;

}

sl_status_t sl_btmesh_proxy_allow(uint32_t handle,
                                  uint16_t netkey_index,
                                  uint16_t address) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_proxy_allow.handle=handle;
    cmd->data.cmd_proxy_allow.netkey_index=netkey_index;
    cmd->data.cmd_proxy_allow.address=address;

    cmd->header=sl_btmesh_cmd_proxy_allow_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_proxy_allow.result;

}

sl_status_t sl_btmesh_proxy_deny(uint32_t handle,
                                 uint16_t netkey_index,
                                 uint16_t address) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_proxy_deny.handle=handle;
    cmd->data.cmd_proxy_deny.netkey_index=netkey_index;
    cmd->data.cmd_proxy_deny.address=address;

    cmd->header=sl_btmesh_cmd_proxy_deny_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_proxy_deny.result;

}

sl_status_t sl_btmesh_proxy_optimisation_toggle(uint8_t enable) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_proxy_optimisation_toggle.enable=enable;

    cmd->header=sl_btmesh_cmd_proxy_optimisation_toggle_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_proxy_optimisation_toggle.result;

}

sl_status_t sl_btmesh_vendor_model_send(uint16_t destination_address,
                                        int8_t va_index,
                                        uint16_t appkey_index,
                                        uint16_t elem_index,
                                        uint16_t vendor_id,
                                        uint16_t model_id,
                                        uint8_t nonrelayed,
                                        uint8_t opcode,
                                        uint8_t final,
                                        size_t payload_len,
                                        const uint8_t* payload) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_vendor_model_send.destination_address=destination_address;
    cmd->data.cmd_vendor_model_send.va_index=va_index;
    cmd->data.cmd_vendor_model_send.appkey_index=appkey_index;
    cmd->data.cmd_vendor_model_send.elem_index=elem_index;
    cmd->data.cmd_vendor_model_send.vendor_id=vendor_id;
    cmd->data.cmd_vendor_model_send.model_id=model_id;
    cmd->data.cmd_vendor_model_send.nonrelayed=nonrelayed;
    cmd->data.cmd_vendor_model_send.opcode=opcode;
    cmd->data.cmd_vendor_model_send.final=final;
    if ((15+payload_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_vendor_model_send.payload.len=payload_len;
    memcpy(cmd->data.cmd_vendor_model_send.payload.data,payload,payload_len);

    cmd->header=sl_btmesh_cmd_vendor_model_send_id+(((15+payload_len)&0xff)<<8)+(((15+payload_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_vendor_model_send.result;

}

sl_status_t sl_btmesh_vendor_model_set_publication(uint16_t elem_index,
                                                   uint16_t vendor_id,
                                                   uint16_t model_id,
                                                   uint8_t opcode,
                                                   uint8_t final,
                                                   size_t payload_len,
                                                   const uint8_t* payload) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_vendor_model_set_publication.elem_index=elem_index;
    cmd->data.cmd_vendor_model_set_publication.vendor_id=vendor_id;
    cmd->data.cmd_vendor_model_set_publication.model_id=model_id;
    cmd->data.cmd_vendor_model_set_publication.opcode=opcode;
    cmd->data.cmd_vendor_model_set_publication.final=final;
    if ((9+payload_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_vendor_model_set_publication.payload.len=payload_len;
    memcpy(cmd->data.cmd_vendor_model_set_publication.payload.data,payload,payload_len);

    cmd->header=sl_btmesh_cmd_vendor_model_set_publication_id+(((9+payload_len)&0xff)<<8)+(((9+payload_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_vendor_model_set_publication.result;

}

sl_status_t sl_btmesh_vendor_model_clear_publication(uint16_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_vendor_model_clear_publication.elem_index=elem_index;
    cmd->data.cmd_vendor_model_clear_publication.vendor_id=vendor_id;
    cmd->data.cmd_vendor_model_clear_publication.model_id=model_id;

    cmd->header=sl_btmesh_cmd_vendor_model_clear_publication_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_vendor_model_clear_publication.result;

}

sl_status_t sl_btmesh_vendor_model_publish(uint16_t elem_index,
                                           uint16_t vendor_id,
                                           uint16_t model_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_vendor_model_publish.elem_index=elem_index;
    cmd->data.cmd_vendor_model_publish.vendor_id=vendor_id;
    cmd->data.cmd_vendor_model_publish.model_id=model_id;

    cmd->header=sl_btmesh_cmd_vendor_model_publish_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_vendor_model_publish.result;

}

sl_status_t sl_btmesh_vendor_model_init(uint16_t elem_index,
                                        uint16_t vendor_id,
                                        uint16_t model_id,
                                        uint8_t publish,
                                        size_t opcodes_len,
                                        const uint8_t* opcodes) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_vendor_model_init.elem_index=elem_index;
    cmd->data.cmd_vendor_model_init.vendor_id=vendor_id;
    cmd->data.cmd_vendor_model_init.model_id=model_id;
    cmd->data.cmd_vendor_model_init.publish=publish;
    if ((8+opcodes_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_vendor_model_init.opcodes.len=opcodes_len;
    memcpy(cmd->data.cmd_vendor_model_init.opcodes.data,opcodes,opcodes_len);

    cmd->header=sl_btmesh_cmd_vendor_model_init_id+(((8+opcodes_len)&0xff)<<8)+(((8+opcodes_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_vendor_model_init.result;

}

sl_status_t sl_btmesh_vendor_model_deinit(uint16_t elem_index,
                                          uint16_t vendor_id,
                                          uint16_t model_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_vendor_model_deinit.elem_index=elem_index;
    cmd->data.cmd_vendor_model_deinit.vendor_id=vendor_id;
    cmd->data.cmd_vendor_model_deinit.model_id=model_id;

    cmd->header=sl_btmesh_cmd_vendor_model_deinit_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_vendor_model_deinit.result;

}

sl_status_t sl_btmesh_vendor_model_send_tracked(uint16_t destination_address,
                                                int8_t va_index,
                                                uint16_t appkey_index,
                                                uint16_t elem_index,
                                                uint16_t vendor_id,
                                                uint16_t model_id,
                                                uint8_t nonrelayed,
                                                uint8_t segment,
                                                uint8_t opcode,
                                                uint8_t final,
                                                size_t payload_len,
                                                const uint8_t* payload,
                                                uint16_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_vendor_model_send_tracked.destination_address=destination_address;
    cmd->data.cmd_vendor_model_send_tracked.va_index=va_index;
    cmd->data.cmd_vendor_model_send_tracked.appkey_index=appkey_index;
    cmd->data.cmd_vendor_model_send_tracked.elem_index=elem_index;
    cmd->data.cmd_vendor_model_send_tracked.vendor_id=vendor_id;
    cmd->data.cmd_vendor_model_send_tracked.model_id=model_id;
    cmd->data.cmd_vendor_model_send_tracked.nonrelayed=nonrelayed;
    cmd->data.cmd_vendor_model_send_tracked.segment=segment;
    cmd->data.cmd_vendor_model_send_tracked.opcode=opcode;
    cmd->data.cmd_vendor_model_send_tracked.final=final;
    if ((16+payload_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_vendor_model_send_tracked.payload.len=payload_len;
    memcpy(cmd->data.cmd_vendor_model_send_tracked.payload.data,payload,payload_len);

    cmd->header=sl_btmesh_cmd_vendor_model_send_tracked_id+(((16+payload_len)&0xff)<<8)+(((16+payload_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_vendor_model_send_tracked.handle;
    return rsp->data.rsp_vendor_model_send_tracked.result;

}

sl_status_t sl_btmesh_vendor_model_set_publication_tracked(uint16_t elem_index,
                                                           uint16_t vendor_id,
                                                           uint16_t model_id,
                                                           uint8_t segment,
                                                           uint8_t opcode,
                                                           uint8_t final,
                                                           size_t payload_len,
                                                           const uint8_t* payload,
                                                           uint16_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_vendor_model_set_publication_tracked.elem_index=elem_index;
    cmd->data.cmd_vendor_model_set_publication_tracked.vendor_id=vendor_id;
    cmd->data.cmd_vendor_model_set_publication_tracked.model_id=model_id;
    cmd->data.cmd_vendor_model_set_publication_tracked.segment=segment;
    cmd->data.cmd_vendor_model_set_publication_tracked.opcode=opcode;
    cmd->data.cmd_vendor_model_set_publication_tracked.final=final;
    if ((10+payload_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_vendor_model_set_publication_tracked.payload.len=payload_len;
    memcpy(cmd->data.cmd_vendor_model_set_publication_tracked.payload.data,payload,payload_len);

    cmd->header=sl_btmesh_cmd_vendor_model_set_publication_tracked_id+(((10+payload_len)&0xff)<<8)+(((10+payload_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_vendor_model_set_publication_tracked.handle;
    return rsp->data.rsp_vendor_model_set_publication_tracked.result;

}

sl_status_t sl_btmesh_health_client_get(uint16_t server_address,
                                        uint16_t elem_index,
                                        uint16_t appkey_index,
                                        uint16_t vendor_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_health_client_get.server_address=server_address;
    cmd->data.cmd_health_client_get.elem_index=elem_index;
    cmd->data.cmd_health_client_get.appkey_index=appkey_index;
    cmd->data.cmd_health_client_get.vendor_id=vendor_id;

    cmd->header=sl_btmesh_cmd_health_client_get_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_health_client_get.result;

}

sl_status_t sl_btmesh_health_client_clear(uint16_t server_address,
                                          uint16_t elem_index,
                                          uint16_t appkey_index,
                                          uint16_t vendor_id,
                                          uint8_t reliable) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_health_client_clear.server_address=server_address;
    cmd->data.cmd_health_client_clear.elem_index=elem_index;
    cmd->data.cmd_health_client_clear.appkey_index=appkey_index;
    cmd->data.cmd_health_client_clear.vendor_id=vendor_id;
    cmd->data.cmd_health_client_clear.reliable=reliable;

    cmd->header=sl_btmesh_cmd_health_client_clear_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_health_client_clear.result;

}

sl_status_t sl_btmesh_health_client_test(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t appkey_index,
                                         uint8_t test_id,
                                         uint16_t vendor_id,
                                         uint8_t reliable) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_health_client_test.server_address=server_address;
    cmd->data.cmd_health_client_test.elem_index=elem_index;
    cmd->data.cmd_health_client_test.appkey_index=appkey_index;
    cmd->data.cmd_health_client_test.test_id=test_id;
    cmd->data.cmd_health_client_test.vendor_id=vendor_id;
    cmd->data.cmd_health_client_test.reliable=reliable;

    cmd->header=sl_btmesh_cmd_health_client_test_id+(((10)&0xff)<<8)+(((10)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_health_client_test.result;

}

sl_status_t sl_btmesh_health_client_get_period(uint16_t server_address,
                                               uint16_t elem_index,
                                               uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_health_client_get_period.server_address=server_address;
    cmd->data.cmd_health_client_get_period.elem_index=elem_index;
    cmd->data.cmd_health_client_get_period.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_health_client_get_period_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_health_client_get_period.result;

}

sl_status_t sl_btmesh_health_client_set_period(uint16_t server_address,
                                               uint16_t elem_index,
                                               uint16_t appkey_index,
                                               uint8_t period,
                                               uint8_t reliable) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_health_client_set_period.server_address=server_address;
    cmd->data.cmd_health_client_set_period.elem_index=elem_index;
    cmd->data.cmd_health_client_set_period.appkey_index=appkey_index;
    cmd->data.cmd_health_client_set_period.period=period;
    cmd->data.cmd_health_client_set_period.reliable=reliable;

    cmd->header=sl_btmesh_cmd_health_client_set_period_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_health_client_set_period.result;

}

sl_status_t sl_btmesh_health_client_get_attention(uint16_t server_address,
                                                  uint16_t elem_index,
                                                  uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_health_client_get_attention.server_address=server_address;
    cmd->data.cmd_health_client_get_attention.elem_index=elem_index;
    cmd->data.cmd_health_client_get_attention.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_health_client_get_attention_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_health_client_get_attention.result;

}

sl_status_t sl_btmesh_health_client_set_attention(uint16_t server_address,
                                                  uint16_t elem_index,
                                                  uint16_t appkey_index,
                                                  uint8_t attention_sec,
                                                  uint8_t reliable) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_health_client_set_attention.server_address=server_address;
    cmd->data.cmd_health_client_set_attention.elem_index=elem_index;
    cmd->data.cmd_health_client_set_attention.appkey_index=appkey_index;
    cmd->data.cmd_health_client_set_attention.attention_sec=attention_sec;
    cmd->data.cmd_health_client_set_attention.reliable=reliable;

    cmd->header=sl_btmesh_cmd_health_client_set_attention_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_health_client_set_attention.result;

}

sl_status_t sl_btmesh_health_server_set_fault(uint16_t elem_index, uint8_t id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_health_server_set_fault.elem_index=elem_index;
    cmd->data.cmd_health_server_set_fault.id=id;

    cmd->header=sl_btmesh_cmd_health_server_set_fault_id+(((3)&0xff)<<8)+(((3)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_health_server_set_fault.result;

}

sl_status_t sl_btmesh_health_server_clear_fault(uint16_t elem_index,
                                                uint8_t id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_health_server_clear_fault.elem_index=elem_index;
    cmd->data.cmd_health_server_clear_fault.id=id;

    cmd->header=sl_btmesh_cmd_health_server_clear_fault_id+(((3)&0xff)<<8)+(((3)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_health_server_clear_fault.result;

}

sl_status_t sl_btmesh_health_server_send_test_response(uint16_t client_address,
                                                       uint16_t elem_index,
                                                       uint16_t appkey_index,
                                                       uint16_t vendor_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_health_server_send_test_response.client_address=client_address;
    cmd->data.cmd_health_server_send_test_response.elem_index=elem_index;
    cmd->data.cmd_health_server_send_test_response.appkey_index=appkey_index;
    cmd->data.cmd_health_server_send_test_response.vendor_id=vendor_id;

    cmd->header=sl_btmesh_cmd_health_server_send_test_response_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_health_server_send_test_response.result;

}

sl_status_t sl_btmesh_generic_client_get(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t model_id,
                                         uint16_t appkey_index,
                                         uint8_t type) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_generic_client_get.server_address=server_address;
    cmd->data.cmd_generic_client_get.elem_index=elem_index;
    cmd->data.cmd_generic_client_get.model_id=model_id;
    cmd->data.cmd_generic_client_get.appkey_index=appkey_index;
    cmd->data.cmd_generic_client_get.type=type;

    cmd->header=sl_btmesh_cmd_generic_client_get_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_get.result;

}

sl_status_t sl_btmesh_generic_client_set(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t model_id,
                                         uint16_t appkey_index,
                                         uint8_t tid,
                                         uint32_t transition_ms,
                                         uint16_t delay_ms,
                                         uint16_t flags,
                                         uint8_t type,
                                         size_t parameters_len,
                                         const uint8_t* parameters) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_generic_client_set.server_address=server_address;
    cmd->data.cmd_generic_client_set.elem_index=elem_index;
    cmd->data.cmd_generic_client_set.model_id=model_id;
    cmd->data.cmd_generic_client_set.appkey_index=appkey_index;
    cmd->data.cmd_generic_client_set.tid=tid;
    cmd->data.cmd_generic_client_set.transition_ms=transition_ms;
    cmd->data.cmd_generic_client_set.delay_ms=delay_ms;
    cmd->data.cmd_generic_client_set.flags=flags;
    cmd->data.cmd_generic_client_set.type=type;
    if ((19+parameters_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_generic_client_set.parameters.len=parameters_len;
    memcpy(cmd->data.cmd_generic_client_set.parameters.data,parameters,parameters_len);

    cmd->header=sl_btmesh_cmd_generic_client_set_id+(((19+parameters_len)&0xff)<<8)+(((19+parameters_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_set.result;

}

sl_status_t sl_btmesh_generic_client_publish(uint16_t elem_index,
                                             uint16_t model_id,
                                             uint8_t tid,
                                             uint32_t transition_ms,
                                             uint16_t delay_ms,
                                             uint16_t flags,
                                             uint8_t type,
                                             size_t parameters_len,
                                             const uint8_t* parameters) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_generic_client_publish.elem_index=elem_index;
    cmd->data.cmd_generic_client_publish.model_id=model_id;
    cmd->data.cmd_generic_client_publish.tid=tid;
    cmd->data.cmd_generic_client_publish.transition_ms=transition_ms;
    cmd->data.cmd_generic_client_publish.delay_ms=delay_ms;
    cmd->data.cmd_generic_client_publish.flags=flags;
    cmd->data.cmd_generic_client_publish.type=type;
    if ((15+parameters_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_generic_client_publish.parameters.len=parameters_len;
    memcpy(cmd->data.cmd_generic_client_publish.parameters.data,parameters,parameters_len);

    cmd->header=sl_btmesh_cmd_generic_client_publish_id+(((15+parameters_len)&0xff)<<8)+(((15+parameters_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_publish.result;

}

sl_status_t sl_btmesh_generic_client_get_params(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t model_id,
                                                uint16_t appkey_index,
                                                uint8_t type,
                                                size_t parameters_len,
                                                const uint8_t* parameters) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_generic_client_get_params.server_address=server_address;
    cmd->data.cmd_generic_client_get_params.elem_index=elem_index;
    cmd->data.cmd_generic_client_get_params.model_id=model_id;
    cmd->data.cmd_generic_client_get_params.appkey_index=appkey_index;
    cmd->data.cmd_generic_client_get_params.type=type;
    if ((10+parameters_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_generic_client_get_params.parameters.len=parameters_len;
    memcpy(cmd->data.cmd_generic_client_get_params.parameters.data,parameters,parameters_len);

    cmd->header=sl_btmesh_cmd_generic_client_get_params_id+(((10+parameters_len)&0xff)<<8)+(((10+parameters_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_get_params.result;

}

sl_status_t sl_btmesh_generic_client_init() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init.result;

}

sl_status_t sl_btmesh_generic_client_init_common() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_common_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_common.result;

}

sl_status_t sl_btmesh_generic_client_init_on_off() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_on_off_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_on_off.result;

}

sl_status_t sl_btmesh_generic_client_init_level() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_level_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_level.result;

}

sl_status_t sl_btmesh_generic_client_init_default_transition_time() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_default_transition_time_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_default_transition_time.result;

}

sl_status_t sl_btmesh_generic_client_init_power_on_off() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_power_on_off_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_power_on_off.result;

}

sl_status_t sl_btmesh_generic_client_init_power_level() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_power_level_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_power_level.result;

}

sl_status_t sl_btmesh_generic_client_init_battery() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_battery_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_battery.result;

}

sl_status_t sl_btmesh_generic_client_init_location() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_location_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_location.result;

}

sl_status_t sl_btmesh_generic_client_init_property() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_property_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_property.result;

}

sl_status_t sl_btmesh_generic_client_init_lightness() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_lightness_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_lightness.result;

}

sl_status_t sl_btmesh_generic_client_init_ctl() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_ctl_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_ctl.result;

}

sl_status_t sl_btmesh_generic_client_init_hsl() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_client_init_hsl_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_client_init_hsl.result;

}

sl_status_t sl_btmesh_generic_server_respond(uint16_t client_address,
                                             uint16_t elem_index,
                                             uint16_t model_id,
                                             uint16_t appkey_index,
                                             uint32_t remaining_ms,
                                             uint16_t flags,
                                             uint8_t type,
                                             size_t parameters_len,
                                             const uint8_t* parameters) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_generic_server_respond.client_address=client_address;
    cmd->data.cmd_generic_server_respond.elem_index=elem_index;
    cmd->data.cmd_generic_server_respond.model_id=model_id;
    cmd->data.cmd_generic_server_respond.appkey_index=appkey_index;
    cmd->data.cmd_generic_server_respond.remaining_ms=remaining_ms;
    cmd->data.cmd_generic_server_respond.flags=flags;
    cmd->data.cmd_generic_server_respond.type=type;
    if ((16+parameters_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_generic_server_respond.parameters.len=parameters_len;
    memcpy(cmd->data.cmd_generic_server_respond.parameters.data,parameters,parameters_len);

    cmd->header=sl_btmesh_cmd_generic_server_respond_id+(((16+parameters_len)&0xff)<<8)+(((16+parameters_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_respond.result;

}

sl_status_t sl_btmesh_generic_server_update(uint16_t elem_index,
                                            uint16_t model_id,
                                            uint32_t remaining_ms,
                                            uint8_t type,
                                            size_t parameters_len,
                                            const uint8_t* parameters) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_generic_server_update.elem_index=elem_index;
    cmd->data.cmd_generic_server_update.model_id=model_id;
    cmd->data.cmd_generic_server_update.remaining_ms=remaining_ms;
    cmd->data.cmd_generic_server_update.type=type;
    if ((10+parameters_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_generic_server_update.parameters.len=parameters_len;
    memcpy(cmd->data.cmd_generic_server_update.parameters.data,parameters,parameters_len);

    cmd->header=sl_btmesh_cmd_generic_server_update_id+(((10+parameters_len)&0xff)<<8)+(((10+parameters_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_update.result;

}

sl_status_t sl_btmesh_generic_server_publish(uint16_t elem_index,
                                             uint16_t model_id,
                                             uint8_t type) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_generic_server_publish.elem_index=elem_index;
    cmd->data.cmd_generic_server_publish.model_id=model_id;
    cmd->data.cmd_generic_server_publish.type=type;

    cmd->header=sl_btmesh_cmd_generic_server_publish_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_publish.result;

}

sl_status_t sl_btmesh_generic_server_init() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init.result;

}

sl_status_t sl_btmesh_generic_server_init_common() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_common_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_common.result;

}

sl_status_t sl_btmesh_generic_server_init_on_off() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_on_off_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_on_off.result;

}

sl_status_t sl_btmesh_generic_server_init_level() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_level_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_level.result;

}

sl_status_t sl_btmesh_generic_server_init_default_transition_time() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_default_transition_time_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_default_transition_time.result;

}

sl_status_t sl_btmesh_generic_server_init_power_on_off() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_power_on_off_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_power_on_off.result;

}

sl_status_t sl_btmesh_generic_server_init_power_level() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_power_level_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_power_level.result;

}

sl_status_t sl_btmesh_generic_server_init_battery() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_battery_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_battery.result;

}

sl_status_t sl_btmesh_generic_server_init_location() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_location_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_location.result;

}

sl_status_t sl_btmesh_generic_server_init_property() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_property_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_property.result;

}

sl_status_t sl_btmesh_generic_server_init_lightness() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_lightness_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_lightness.result;

}

sl_status_t sl_btmesh_generic_server_init_ctl() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_ctl_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_ctl.result;

}

sl_status_t sl_btmesh_generic_server_init_hsl() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_generic_server_init_hsl_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_generic_server_init_hsl.result;

}

sl_status_t sl_btmesh_generic_server_get_cached_state(uint16_t elem_index,
                                                      uint16_t model_id,
                                                      uint8_t type,
                                                      uint32_t *remaining_ms,
                                                      size_t max_parameters_size,
                                                      size_t *parameters_len,
                                                      uint8_t *parameters) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_generic_server_get_cached_state.elem_index=elem_index;
    cmd->data.cmd_generic_server_get_cached_state.model_id=model_id;
    cmd->data.cmd_generic_server_get_cached_state.type=type;

    cmd->header=sl_btmesh_cmd_generic_server_get_cached_state_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *remaining_ms = rsp->data.rsp_generic_server_get_cached_state.remaining_ms;
    *parameters_len = rsp->data.rsp_generic_server_get_cached_state.parameters.len;
    if (rsp->data.rsp_generic_server_get_cached_state.parameters.len <= max_parameters_size) {
        memcpy(parameters,rsp->data.rsp_generic_server_get_cached_state.parameters.data,rsp->data.rsp_generic_server_get_cached_state.parameters.len);
    }
    return rsp->data.rsp_generic_server_get_cached_state.result;

}

sl_status_t sl_btmesh_test_get_nettx(uint8_t *count, uint8_t *interval) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_test_get_nettx_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *count = rsp->data.rsp_test_get_nettx.count;
    *interval = rsp->data.rsp_test_get_nettx.interval;
    return rsp->data.rsp_test_get_nettx.result;

}

sl_status_t sl_btmesh_test_set_nettx(uint8_t count, uint8_t interval) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_nettx.count=count;
    cmd->data.cmd_test_set_nettx.interval=interval;

    cmd->header=sl_btmesh_cmd_test_set_nettx_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_nettx.result;

}

sl_status_t sl_btmesh_test_get_relay(uint8_t *enabled,
                                     uint8_t *count,
                                     uint8_t *interval) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_test_get_relay_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *enabled = rsp->data.rsp_test_get_relay.enabled;
    *count = rsp->data.rsp_test_get_relay.count;
    *interval = rsp->data.rsp_test_get_relay.interval;
    return rsp->data.rsp_test_get_relay.result;

}

sl_status_t sl_btmesh_test_set_relay(uint8_t enabled,
                                     uint8_t count,
                                     uint8_t interval) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_relay.enabled=enabled;
    cmd->data.cmd_test_set_relay.count=count;
    cmd->data.cmd_test_set_relay.interval=interval;

    cmd->header=sl_btmesh_cmd_test_set_relay_id+(((3)&0xff)<<8)+(((3)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_relay.result;

}

sl_status_t sl_btmesh_test_set_adv_scan_params(uint16_t adv_interval_min,
                                               uint16_t adv_interval_max,
                                               uint8_t adv_repeat_packets,
                                               uint8_t adv_use_random_address,
                                               uint8_t adv_channel_map,
                                               uint16_t scan_interval,
                                               uint16_t scan_window) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_adv_scan_params.adv_interval_min=adv_interval_min;
    cmd->data.cmd_test_set_adv_scan_params.adv_interval_max=adv_interval_max;
    cmd->data.cmd_test_set_adv_scan_params.adv_repeat_packets=adv_repeat_packets;
    cmd->data.cmd_test_set_adv_scan_params.adv_use_random_address=adv_use_random_address;
    cmd->data.cmd_test_set_adv_scan_params.adv_channel_map=adv_channel_map;
    cmd->data.cmd_test_set_adv_scan_params.scan_interval=scan_interval;
    cmd->data.cmd_test_set_adv_scan_params.scan_window=scan_window;

    cmd->header=sl_btmesh_cmd_test_set_adv_scan_params_id+(((11)&0xff)<<8)+(((11)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_adv_scan_params.result;

}

sl_status_t sl_btmesh_test_set_ivupdate_test_mode(uint8_t mode) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_ivupdate_test_mode.mode=mode;

    cmd->header=sl_btmesh_cmd_test_set_ivupdate_test_mode_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_ivupdate_test_mode.result;

}

sl_status_t sl_btmesh_test_get_ivupdate_test_mode(uint8_t *mode) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_test_get_ivupdate_test_mode_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *mode = rsp->data.rsp_test_get_ivupdate_test_mode.mode;
    return rsp->data.rsp_test_get_ivupdate_test_mode.result;

}

sl_status_t sl_btmesh_test_set_segment_send_delay(uint8_t delay) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_segment_send_delay.delay=delay;

    cmd->header=sl_btmesh_cmd_test_set_segment_send_delay_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_segment_send_delay.result;

}

sl_status_t sl_btmesh_test_set_ivupdate_state(uint8_t state) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_ivupdate_state.state=state;

    cmd->header=sl_btmesh_cmd_test_set_ivupdate_state_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_ivupdate_state.result;

}

sl_status_t sl_btmesh_test_send_beacons() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_test_send_beacons_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_send_beacons.result;

}

sl_status_t sl_btmesh_test_bind_local_model_app(uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint16_t vendor_id,
                                                uint16_t model_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_bind_local_model_app.elem_index=elem_index;
    cmd->data.cmd_test_bind_local_model_app.appkey_index=appkey_index;
    cmd->data.cmd_test_bind_local_model_app.vendor_id=vendor_id;
    cmd->data.cmd_test_bind_local_model_app.model_id=model_id;

    cmd->header=sl_btmesh_cmd_test_bind_local_model_app_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_bind_local_model_app.result;

}

sl_status_t sl_btmesh_test_unbind_local_model_app(uint16_t elem_index,
                                                  uint16_t appkey_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_unbind_local_model_app.elem_index=elem_index;
    cmd->data.cmd_test_unbind_local_model_app.appkey_index=appkey_index;
    cmd->data.cmd_test_unbind_local_model_app.vendor_id=vendor_id;
    cmd->data.cmd_test_unbind_local_model_app.model_id=model_id;

    cmd->header=sl_btmesh_cmd_test_unbind_local_model_app_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_unbind_local_model_app.result;

}

sl_status_t sl_btmesh_test_add_local_model_sub(uint16_t elem_index,
                                               uint16_t vendor_id,
                                               uint16_t model_id,
                                               uint16_t sub_address) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_add_local_model_sub.elem_index=elem_index;
    cmd->data.cmd_test_add_local_model_sub.vendor_id=vendor_id;
    cmd->data.cmd_test_add_local_model_sub.model_id=model_id;
    cmd->data.cmd_test_add_local_model_sub.sub_address=sub_address;

    cmd->header=sl_btmesh_cmd_test_add_local_model_sub_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_add_local_model_sub.result;

}

sl_status_t sl_btmesh_test_remove_local_model_sub(uint16_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint16_t sub_address) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_remove_local_model_sub.elem_index=elem_index;
    cmd->data.cmd_test_remove_local_model_sub.vendor_id=vendor_id;
    cmd->data.cmd_test_remove_local_model_sub.model_id=model_id;
    cmd->data.cmd_test_remove_local_model_sub.sub_address=sub_address;

    cmd->header=sl_btmesh_cmd_test_remove_local_model_sub_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_remove_local_model_sub.result;

}

sl_status_t sl_btmesh_test_add_local_model_sub_va(uint16_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  size_t sub_address_len,
                                                  const uint8_t* sub_address) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_add_local_model_sub_va.elem_index=elem_index;
    cmd->data.cmd_test_add_local_model_sub_va.vendor_id=vendor_id;
    cmd->data.cmd_test_add_local_model_sub_va.model_id=model_id;
    if ((7+sub_address_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_test_add_local_model_sub_va.sub_address.len=sub_address_len;
    memcpy(cmd->data.cmd_test_add_local_model_sub_va.sub_address.data,sub_address,sub_address_len);

    cmd->header=sl_btmesh_cmd_test_add_local_model_sub_va_id+(((7+sub_address_len)&0xff)<<8)+(((7+sub_address_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_add_local_model_sub_va.result;

}

sl_status_t sl_btmesh_test_remove_local_model_sub_va(uint16_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id,
                                                     size_t sub_address_len,
                                                     const uint8_t* sub_address) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_remove_local_model_sub_va.elem_index=elem_index;
    cmd->data.cmd_test_remove_local_model_sub_va.vendor_id=vendor_id;
    cmd->data.cmd_test_remove_local_model_sub_va.model_id=model_id;
    if ((7+sub_address_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_test_remove_local_model_sub_va.sub_address.len=sub_address_len;
    memcpy(cmd->data.cmd_test_remove_local_model_sub_va.sub_address.data,sub_address,sub_address_len);

    cmd->header=sl_btmesh_cmd_test_remove_local_model_sub_va_id+(((7+sub_address_len)&0xff)<<8)+(((7+sub_address_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_remove_local_model_sub_va.result;

}

sl_status_t sl_btmesh_test_get_local_model_sub(uint16_t elem_index,
                                               uint16_t vendor_id,
                                               uint16_t model_id,
                                               size_t max_addresses_size,
                                               size_t *addresses_len,
                                               uint8_t *addresses) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_get_local_model_sub.elem_index=elem_index;
    cmd->data.cmd_test_get_local_model_sub.vendor_id=vendor_id;
    cmd->data.cmd_test_get_local_model_sub.model_id=model_id;

    cmd->header=sl_btmesh_cmd_test_get_local_model_sub_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *addresses_len = rsp->data.rsp_test_get_local_model_sub.addresses.len;
    if (rsp->data.rsp_test_get_local_model_sub.addresses.len <= max_addresses_size) {
        memcpy(addresses,rsp->data.rsp_test_get_local_model_sub.addresses.data,rsp->data.rsp_test_get_local_model_sub.addresses.len);
    }
    return rsp->data.rsp_test_get_local_model_sub.result;

}

sl_status_t sl_btmesh_test_set_local_model_pub(uint16_t elem_index,
                                               uint16_t appkey_index,
                                               uint16_t vendor_id,
                                               uint16_t model_id,
                                               uint16_t pub_address,
                                               uint8_t ttl,
                                               uint8_t period,
                                               uint8_t retrans,
                                               uint8_t credentials) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_local_model_pub.elem_index=elem_index;
    cmd->data.cmd_test_set_local_model_pub.appkey_index=appkey_index;
    cmd->data.cmd_test_set_local_model_pub.vendor_id=vendor_id;
    cmd->data.cmd_test_set_local_model_pub.model_id=model_id;
    cmd->data.cmd_test_set_local_model_pub.pub_address=pub_address;
    cmd->data.cmd_test_set_local_model_pub.ttl=ttl;
    cmd->data.cmd_test_set_local_model_pub.period=period;
    cmd->data.cmd_test_set_local_model_pub.retrans=retrans;
    cmd->data.cmd_test_set_local_model_pub.credentials=credentials;

    cmd->header=sl_btmesh_cmd_test_set_local_model_pub_id+(((14)&0xff)<<8)+(((14)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_local_model_pub.result;

}

sl_status_t sl_btmesh_test_set_local_model_pub_va(uint16_t elem_index,
                                                  uint16_t appkey_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint8_t ttl,
                                                  uint8_t period,
                                                  uint8_t retrans,
                                                  uint8_t credentials,
                                                  size_t pub_address_len,
                                                  const uint8_t* pub_address) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_local_model_pub_va.elem_index=elem_index;
    cmd->data.cmd_test_set_local_model_pub_va.appkey_index=appkey_index;
    cmd->data.cmd_test_set_local_model_pub_va.vendor_id=vendor_id;
    cmd->data.cmd_test_set_local_model_pub_va.model_id=model_id;
    cmd->data.cmd_test_set_local_model_pub_va.ttl=ttl;
    cmd->data.cmd_test_set_local_model_pub_va.period=period;
    cmd->data.cmd_test_set_local_model_pub_va.retrans=retrans;
    cmd->data.cmd_test_set_local_model_pub_va.credentials=credentials;
    if ((13+pub_address_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_test_set_local_model_pub_va.pub_address.len=pub_address_len;
    memcpy(cmd->data.cmd_test_set_local_model_pub_va.pub_address.data,pub_address,pub_address_len);

    cmd->header=sl_btmesh_cmd_test_set_local_model_pub_va_id+(((13+pub_address_len)&0xff)<<8)+(((13+pub_address_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_local_model_pub_va.result;

}

sl_status_t sl_btmesh_test_get_local_model_pub(uint16_t elem_index,
                                               uint16_t vendor_id,
                                               uint16_t model_id,
                                               uint16_t *appkey_index,
                                               uint16_t *pub_address,
                                               uint8_t *ttl,
                                               uint8_t *period,
                                               uint8_t *retrans,
                                               uint8_t *credentials) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_get_local_model_pub.elem_index=elem_index;
    cmd->data.cmd_test_get_local_model_pub.vendor_id=vendor_id;
    cmd->data.cmd_test_get_local_model_pub.model_id=model_id;

    cmd->header=sl_btmesh_cmd_test_get_local_model_pub_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *appkey_index = rsp->data.rsp_test_get_local_model_pub.appkey_index;
    *pub_address = rsp->data.rsp_test_get_local_model_pub.pub_address;
    *ttl = rsp->data.rsp_test_get_local_model_pub.ttl;
    *period = rsp->data.rsp_test_get_local_model_pub.period;
    *retrans = rsp->data.rsp_test_get_local_model_pub.retrans;
    *credentials = rsp->data.rsp_test_get_local_model_pub.credentials;
    return rsp->data.rsp_test_get_local_model_pub.result;

}

sl_status_t sl_btmesh_test_set_local_heartbeat_subscription(uint16_t subscription_source,
                                                            uint16_t subscription_destination,
                                                            uint8_t period_log) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_local_heartbeat_subscription.subscription_source=subscription_source;
    cmd->data.cmd_test_set_local_heartbeat_subscription.subscription_destination=subscription_destination;
    cmd->data.cmd_test_set_local_heartbeat_subscription.period_log=period_log;

    cmd->header=sl_btmesh_cmd_test_set_local_heartbeat_subscription_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_local_heartbeat_subscription.result;

}

sl_status_t sl_btmesh_test_get_local_heartbeat_subscription(uint16_t *count,
                                                            uint8_t *hop_min,
                                                            uint8_t *hop_max) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_test_get_local_heartbeat_subscription_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *count = rsp->data.rsp_test_get_local_heartbeat_subscription.count;
    *hop_min = rsp->data.rsp_test_get_local_heartbeat_subscription.hop_min;
    *hop_max = rsp->data.rsp_test_get_local_heartbeat_subscription.hop_max;
    return rsp->data.rsp_test_get_local_heartbeat_subscription.result;

}

sl_status_t sl_btmesh_test_get_local_heartbeat_publication(uint16_t *publication_address,
                                                           uint8_t *count,
                                                           uint8_t *period_log,
                                                           uint8_t *ttl,
                                                           uint16_t *features,
                                                           uint16_t *publication_netkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_test_get_local_heartbeat_publication_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *publication_address = rsp->data.rsp_test_get_local_heartbeat_publication.publication_address;
    *count = rsp->data.rsp_test_get_local_heartbeat_publication.count;
    *period_log = rsp->data.rsp_test_get_local_heartbeat_publication.period_log;
    *ttl = rsp->data.rsp_test_get_local_heartbeat_publication.ttl;
    *features = rsp->data.rsp_test_get_local_heartbeat_publication.features;
    *publication_netkey_index = rsp->data.rsp_test_get_local_heartbeat_publication.publication_netkey_index;
    return rsp->data.rsp_test_get_local_heartbeat_publication.result;

}

sl_status_t sl_btmesh_test_set_local_heartbeat_publication(uint16_t publication_address,
                                                           uint8_t count_log,
                                                           uint8_t period_log,
                                                           uint8_t ttl,
                                                           uint16_t features,
                                                           uint16_t publication_netkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_local_heartbeat_publication.publication_address=publication_address;
    cmd->data.cmd_test_set_local_heartbeat_publication.count_log=count_log;
    cmd->data.cmd_test_set_local_heartbeat_publication.period_log=period_log;
    cmd->data.cmd_test_set_local_heartbeat_publication.ttl=ttl;
    cmd->data.cmd_test_set_local_heartbeat_publication.features=features;
    cmd->data.cmd_test_set_local_heartbeat_publication.publication_netkey_index=publication_netkey_index;

    cmd->header=sl_btmesh_cmd_test_set_local_heartbeat_publication_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_local_heartbeat_publication.result;

}

SL_BGAPI_DEPRECATED sl_status_t sl_btmesh_test_set_local_config(uint16_t id,
                                            uint16_t netkey_index,
                                            size_t value_len,
                                            const uint8_t* value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_local_config.id=id;
    cmd->data.cmd_test_set_local_config.netkey_index=netkey_index;
    if ((5+value_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_test_set_local_config.value.len=value_len;
    memcpy(cmd->data.cmd_test_set_local_config.value.data,value,value_len);

    cmd->header=sl_btmesh_cmd_test_set_local_config_id+(((5+value_len)&0xff)<<8)+(((5+value_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_local_config.result;

}

SL_BGAPI_DEPRECATED sl_status_t sl_btmesh_test_get_local_config(uint16_t id,
                                            uint16_t netkey_index,
                                            size_t max_data_size,
                                            size_t *data_len,
                                            uint8_t *data) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_get_local_config.id=id;
    cmd->data.cmd_test_get_local_config.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_test_get_local_config_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *data_len = rsp->data.rsp_test_get_local_config.data.len;
    if (rsp->data.rsp_test_get_local_config.data.len <= max_data_size) {
        memcpy(data,rsp->data.rsp_test_get_local_config.data.data,rsp->data.rsp_test_get_local_config.data.len);
    }
    return rsp->data.rsp_test_get_local_config.result;

}

sl_status_t sl_btmesh_test_add_local_key(uint8_t key_type,
                                         aes_key_128 key,
                                         uint16_t key_index,
                                         uint16_t netkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_add_local_key.key_type=key_type;
    memcpy(&cmd->data.cmd_test_add_local_key.key,&key,sizeof(aes_key_128));
    cmd->data.cmd_test_add_local_key.key_index=key_index;
    cmd->data.cmd_test_add_local_key.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_test_add_local_key_id+(((21)&0xff)<<8)+(((21)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_add_local_key.result;

}

sl_status_t sl_btmesh_test_remove_local_key(uint8_t key_type,
                                            uint16_t key_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_remove_local_key.key_type=key_type;
    cmd->data.cmd_test_remove_local_key.key_index=key_index;

    cmd->header=sl_btmesh_cmd_test_remove_local_key_id+(((3)&0xff)<<8)+(((3)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_remove_local_key.result;

}

sl_status_t sl_btmesh_test_update_local_key(uint8_t key_type,
                                            aes_key_128 key,
                                            uint16_t key_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_update_local_key.key_type=key_type;
    memcpy(&cmd->data.cmd_test_update_local_key.key,&key,sizeof(aes_key_128));
    cmd->data.cmd_test_update_local_key.key_index=key_index;

    cmd->header=sl_btmesh_cmd_test_update_local_key_id+(((19)&0xff)<<8)+(((19)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_update_local_key.result;

}

sl_status_t sl_btmesh_test_set_sar_config(uint32_t incomplete_timer_ms,
                                          uint32_t pending_ack_base_ms,
                                          uint32_t pending_ack_mul_ms,
                                          uint32_t wait_for_ack_base_ms,
                                          uint32_t wait_for_ack_mul_ms,
                                          uint8_t max_send_rounds) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_sar_config.incomplete_timer_ms=incomplete_timer_ms;
    cmd->data.cmd_test_set_sar_config.pending_ack_base_ms=pending_ack_base_ms;
    cmd->data.cmd_test_set_sar_config.pending_ack_mul_ms=pending_ack_mul_ms;
    cmd->data.cmd_test_set_sar_config.wait_for_ack_base_ms=wait_for_ack_base_ms;
    cmd->data.cmd_test_set_sar_config.wait_for_ack_mul_ms=wait_for_ack_mul_ms;
    cmd->data.cmd_test_set_sar_config.max_send_rounds=max_send_rounds;

    cmd->header=sl_btmesh_cmd_test_set_sar_config_id+(((21)&0xff)<<8)+(((21)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_sar_config.result;

}

sl_status_t sl_btmesh_test_set_adv_bearer_state(uint8_t state) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_adv_bearer_state.state=state;

    cmd->header=sl_btmesh_cmd_test_set_adv_bearer_state_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_adv_bearer_state.result;

}

sl_status_t sl_btmesh_test_prov_get_device_key(uint16_t address,
                                               aes_key_128 *device_key) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_prov_get_device_key.address=address;

    cmd->header=sl_btmesh_cmd_test_prov_get_device_key_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    memcpy(device_key,&rsp->data.rsp_test_prov_get_device_key.device_key,sizeof(aes_key_128));
    return rsp->data.rsp_test_prov_get_device_key.result;

}

sl_status_t sl_btmesh_test_prov_prepare_key_refresh(aes_key_128 net_key,
                                                    size_t app_keys_len,
                                                    const uint8_t* app_keys) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    memcpy(&cmd->data.cmd_test_prov_prepare_key_refresh.net_key,&net_key,sizeof(aes_key_128));
    if ((17+app_keys_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_test_prov_prepare_key_refresh.app_keys.len=app_keys_len;
    memcpy(cmd->data.cmd_test_prov_prepare_key_refresh.app_keys.data,app_keys,app_keys_len);

    cmd->header=sl_btmesh_cmd_test_prov_prepare_key_refresh_id+(((17+app_keys_len)&0xff)<<8)+(((17+app_keys_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_prov_prepare_key_refresh.result;

}

sl_status_t sl_btmesh_test_cancel_segmented_tx(uint16_t src_addr,
                                               uint16_t dst_addr) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_cancel_segmented_tx.src_addr=src_addr;
    cmd->data.cmd_test_cancel_segmented_tx.dst_addr=dst_addr;

    cmd->header=sl_btmesh_cmd_test_cancel_segmented_tx_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_cancel_segmented_tx.result;

}

sl_status_t sl_btmesh_test_set_iv_index(uint32_t iv_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_iv_index.iv_index=iv_index;

    cmd->header=sl_btmesh_cmd_test_set_iv_index_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_iv_index.result;

}

sl_status_t sl_btmesh_test_set_element_seqnum(uint16_t elem_index,
                                              uint32_t seqnum) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_element_seqnum.elem_index=elem_index;
    cmd->data.cmd_test_set_element_seqnum.seqnum=seqnum;

    cmd->header=sl_btmesh_cmd_test_set_element_seqnum_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_element_seqnum.result;

}

sl_status_t sl_btmesh_test_set_model_option(uint16_t elem_index,
                                            uint16_t vendor_id,
                                            uint16_t model_id,
                                            uint8_t option,
                                            uint32_t value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_model_option.elem_index=elem_index;
    cmd->data.cmd_test_set_model_option.vendor_id=vendor_id;
    cmd->data.cmd_test_set_model_option.model_id=model_id;
    cmd->data.cmd_test_set_model_option.option=option;
    cmd->data.cmd_test_set_model_option.value=value;

    cmd->header=sl_btmesh_cmd_test_set_model_option_id+(((11)&0xff)<<8)+(((11)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_model_option.result;

}

sl_status_t sl_btmesh_test_get_local_model_app_bindings(uint16_t elem_index,
                                                        uint16_t vendor_id,
                                                        uint16_t model_id,
                                                        size_t max_appkeys_size,
                                                        size_t *appkeys_len,
                                                        uint8_t *appkeys) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_get_local_model_app_bindings.elem_index=elem_index;
    cmd->data.cmd_test_get_local_model_app_bindings.vendor_id=vendor_id;
    cmd->data.cmd_test_get_local_model_app_bindings.model_id=model_id;

    cmd->header=sl_btmesh_cmd_test_get_local_model_app_bindings_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *appkeys_len = rsp->data.rsp_test_get_local_model_app_bindings.appkeys.len;
    if (rsp->data.rsp_test_get_local_model_app_bindings.appkeys.len <= max_appkeys_size) {
        memcpy(appkeys,rsp->data.rsp_test_get_local_model_app_bindings.appkeys.data,rsp->data.rsp_test_get_local_model_app_bindings.appkeys.len);
    }
    return rsp->data.rsp_test_get_local_model_app_bindings.result;

}

sl_status_t sl_btmesh_test_get_replay_protection_list_entry(uint16_t address,
                                                            uint32_t *seq,
                                                            uint32_t *seq_ivindex) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_get_replay_protection_list_entry.address=address;

    cmd->header=sl_btmesh_cmd_test_get_replay_protection_list_entry_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *seq = rsp->data.rsp_test_get_replay_protection_list_entry.seq;
    *seq_ivindex = rsp->data.rsp_test_get_replay_protection_list_entry.seq_ivindex;
    return rsp->data.rsp_test_get_replay_protection_list_entry.result;

}

sl_status_t sl_btmesh_test_clear_replay_protection_list_entry(uint16_t address) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_clear_replay_protection_list_entry.address=address;

    cmd->header=sl_btmesh_cmd_test_clear_replay_protection_list_entry_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_clear_replay_protection_list_entry.result;

}

sl_status_t sl_btmesh_test_set_replay_protection_list_diagnostics(uint8_t enable) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_replay_protection_list_diagnostics.enable=enable;

    cmd->header=sl_btmesh_cmd_test_set_replay_protection_list_diagnostics_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_test_set_replay_protection_list_diagnostics.result;

}

sl_status_t sl_btmesh_test_get_model_option(uint16_t elem_index,
                                            uint16_t vendor_id,
                                            uint16_t model_id,
                                            uint8_t option,
                                            uint32_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_get_model_option.elem_index=elem_index;
    cmd->data.cmd_test_get_model_option.vendor_id=vendor_id;
    cmd->data.cmd_test_get_model_option.model_id=model_id;
    cmd->data.cmd_test_get_model_option.option=option;

    cmd->header=sl_btmesh_cmd_test_get_model_option_id+(((7)&0xff)<<8)+(((7)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *value = rsp->data.rsp_test_get_model_option.value;
    return rsp->data.rsp_test_get_model_option.result;

}

sl_status_t sl_btmesh_test_get_default_ttl(uint8_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_test_get_default_ttl_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *value = rsp->data.rsp_test_get_default_ttl.value;
    return rsp->data.rsp_test_get_default_ttl.result;

}

sl_status_t sl_btmesh_test_set_default_ttl(uint8_t set_value, uint8_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_default_ttl.set_value=set_value;

    cmd->header=sl_btmesh_cmd_test_set_default_ttl_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *value = rsp->data.rsp_test_set_default_ttl.value;
    return rsp->data.rsp_test_set_default_ttl.result;

}

sl_status_t sl_btmesh_test_get_gatt_proxy(uint8_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_test_get_gatt_proxy_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *value = rsp->data.rsp_test_get_gatt_proxy.value;
    return rsp->data.rsp_test_get_gatt_proxy.result;

}

sl_status_t sl_btmesh_test_set_gatt_proxy(uint8_t set_value, uint8_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_gatt_proxy.set_value=set_value;

    cmd->header=sl_btmesh_cmd_test_set_gatt_proxy_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *value = rsp->data.rsp_test_set_gatt_proxy.value;
    return rsp->data.rsp_test_set_gatt_proxy.result;

}

sl_status_t sl_btmesh_test_get_identity(uint16_t get_netkey_index,
                                        uint16_t *netkey_index,
                                        uint8_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_get_identity.get_netkey_index=get_netkey_index;

    cmd->header=sl_btmesh_cmd_test_get_identity_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *netkey_index = rsp->data.rsp_test_get_identity.netkey_index;
    *value = rsp->data.rsp_test_get_identity.value;
    return rsp->data.rsp_test_get_identity.result;

}

sl_status_t sl_btmesh_test_set_identity(uint16_t set_netkey_index,
                                        uint8_t set_value,
                                        uint16_t *netkey_index,
                                        uint8_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_identity.set_netkey_index=set_netkey_index;
    cmd->data.cmd_test_set_identity.set_value=set_value;

    cmd->header=sl_btmesh_cmd_test_set_identity_id+(((3)&0xff)<<8)+(((3)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *netkey_index = rsp->data.rsp_test_set_identity.netkey_index;
    *value = rsp->data.rsp_test_set_identity.value;
    return rsp->data.rsp_test_set_identity.result;

}

sl_status_t sl_btmesh_test_get_friend(uint8_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_test_get_friend_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *value = rsp->data.rsp_test_get_friend.value;
    return rsp->data.rsp_test_get_friend.result;

}

sl_status_t sl_btmesh_test_set_friend(uint8_t set_value, uint8_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_friend.set_value=set_value;

    cmd->header=sl_btmesh_cmd_test_set_friend_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *value = rsp->data.rsp_test_set_friend.value;
    return rsp->data.rsp_test_set_friend.result;

}

sl_status_t sl_btmesh_test_get_beacon(uint8_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_test_get_beacon_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *value = rsp->data.rsp_test_get_beacon.value;
    return rsp->data.rsp_test_get_beacon.result;

}

sl_status_t sl_btmesh_test_set_beacon(uint8_t set_value, uint8_t *value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_test_set_beacon.set_value=set_value;

    cmd->header=sl_btmesh_cmd_test_set_beacon_id+(((1)&0xff)<<8)+(((1)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *value = rsp->data.rsp_test_set_beacon.value;
    return rsp->data.rsp_test_set_beacon.result;

}

sl_status_t sl_btmesh_lpn_init() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_lpn_init_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lpn_init.result;

}

sl_status_t sl_btmesh_lpn_deinit() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_lpn_deinit_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lpn_deinit.result;

}

sl_status_t sl_btmesh_lpn_establish_friendship(uint16_t netkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lpn_establish_friendship.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_lpn_establish_friendship_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lpn_establish_friendship.result;

}

sl_status_t sl_btmesh_lpn_poll(uint16_t netkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lpn_poll.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_lpn_poll_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lpn_poll.result;

}

sl_status_t sl_btmesh_lpn_terminate_friendship(uint16_t netkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lpn_terminate_friendship.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_lpn_terminate_friendship_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lpn_terminate_friendship.result;

}

sl_status_t sl_btmesh_lpn_config(uint8_t setting_id, uint32_t value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lpn_config.setting_id=setting_id;
    cmd->data.cmd_lpn_config.value=value;

    cmd->header=sl_btmesh_cmd_lpn_config_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lpn_config.result;

}

sl_status_t sl_btmesh_friend_init() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_friend_init_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_friend_init.result;

}

sl_status_t sl_btmesh_friend_deinit() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_friend_deinit_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_friend_deinit.result;

}

sl_status_t sl_btmesh_config_client_cancel_request(uint32_t handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_cancel_request.handle=handle;

    cmd->header=sl_btmesh_cmd_config_client_cancel_request_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_config_client_cancel_request.result;

}

sl_status_t sl_btmesh_config_client_get_request_status(uint32_t handle,
                                                       uint16_t *server_address,
                                                       uint16_t *opcode,
                                                       uint32_t *age_ms,
                                                       uint32_t *remaining_ms,
                                                       uint8_t *friend_acked) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_request_status.handle=handle;

    cmd->header=sl_btmesh_cmd_config_client_get_request_status_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *server_address = rsp->data.rsp_config_client_get_request_status.server_address;
    *opcode = rsp->data.rsp_config_client_get_request_status.opcode;
    *age_ms = rsp->data.rsp_config_client_get_request_status.age_ms;
    *remaining_ms = rsp->data.rsp_config_client_get_request_status.remaining_ms;
    *friend_acked = rsp->data.rsp_config_client_get_request_status.friend_acked;
    return rsp->data.rsp_config_client_get_request_status.result;

}

sl_status_t sl_btmesh_config_client_get_default_timeout(uint32_t *timeout_ms,
                                                        uint32_t *lpn_timeout_ms) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_config_client_get_default_timeout_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *timeout_ms = rsp->data.rsp_config_client_get_default_timeout.timeout_ms;
    *lpn_timeout_ms = rsp->data.rsp_config_client_get_default_timeout.lpn_timeout_ms;
    return rsp->data.rsp_config_client_get_default_timeout.result;

}

sl_status_t sl_btmesh_config_client_set_default_timeout(uint32_t timeout_ms,
                                                        uint32_t lpn_timeout_ms) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_default_timeout.timeout_ms=timeout_ms;
    cmd->data.cmd_config_client_set_default_timeout.lpn_timeout_ms=lpn_timeout_ms;

    cmd->header=sl_btmesh_cmd_config_client_set_default_timeout_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_config_client_set_default_timeout.result;

}

sl_status_t sl_btmesh_config_client_add_netkey(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint16_t netkey_index,
                                               uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_add_netkey.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_add_netkey.server_address=server_address;
    cmd->data.cmd_config_client_add_netkey.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_config_client_add_netkey_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_add_netkey.handle;
    return rsp->data.rsp_config_client_add_netkey.result;

}

sl_status_t sl_btmesh_config_client_remove_netkey(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint16_t netkey_index,
                                                  uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_remove_netkey.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_remove_netkey.server_address=server_address;
    cmd->data.cmd_config_client_remove_netkey.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_config_client_remove_netkey_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_remove_netkey.handle;
    return rsp->data.rsp_config_client_remove_netkey.result;

}

sl_status_t sl_btmesh_config_client_list_netkeys(uint16_t enc_netkey_index,
                                                 uint16_t server_address,
                                                 uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_list_netkeys.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_list_netkeys.server_address=server_address;

    cmd->header=sl_btmesh_cmd_config_client_list_netkeys_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_list_netkeys.handle;
    return rsp->data.rsp_config_client_list_netkeys.result;

}

sl_status_t sl_btmesh_config_client_add_appkey(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint16_t appkey_index,
                                               uint16_t netkey_index,
                                               uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_add_appkey.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_add_appkey.server_address=server_address;
    cmd->data.cmd_config_client_add_appkey.appkey_index=appkey_index;
    cmd->data.cmd_config_client_add_appkey.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_config_client_add_appkey_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_add_appkey.handle;
    return rsp->data.rsp_config_client_add_appkey.result;

}

sl_status_t sl_btmesh_config_client_remove_appkey(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint16_t appkey_index,
                                                  uint16_t netkey_index,
                                                  uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_remove_appkey.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_remove_appkey.server_address=server_address;
    cmd->data.cmd_config_client_remove_appkey.appkey_index=appkey_index;
    cmd->data.cmd_config_client_remove_appkey.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_config_client_remove_appkey_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_remove_appkey.handle;
    return rsp->data.rsp_config_client_remove_appkey.result;

}

sl_status_t sl_btmesh_config_client_list_appkeys(uint16_t enc_netkey_index,
                                                 uint16_t server_address,
                                                 uint16_t netkey_index,
                                                 uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_list_appkeys.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_list_appkeys.server_address=server_address;
    cmd->data.cmd_config_client_list_appkeys.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_config_client_list_appkeys_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_list_appkeys.handle;
    return rsp->data.rsp_config_client_list_appkeys.result;

}

sl_status_t sl_btmesh_config_client_bind_model(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint8_t elem_index,
                                               uint16_t vendor_id,
                                               uint16_t model_id,
                                               uint16_t appkey_index,
                                               uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_bind_model.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_bind_model.server_address=server_address;
    cmd->data.cmd_config_client_bind_model.elem_index=elem_index;
    cmd->data.cmd_config_client_bind_model.vendor_id=vendor_id;
    cmd->data.cmd_config_client_bind_model.model_id=model_id;
    cmd->data.cmd_config_client_bind_model.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_config_client_bind_model_id+(((11)&0xff)<<8)+(((11)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_bind_model.handle;
    return rsp->data.rsp_config_client_bind_model.result;

}

sl_status_t sl_btmesh_config_client_unbind_model(uint16_t enc_netkey_index,
                                                 uint16_t server_address,
                                                 uint8_t elem_index,
                                                 uint16_t vendor_id,
                                                 uint16_t model_id,
                                                 uint16_t appkey_index,
                                                 uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_unbind_model.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_unbind_model.server_address=server_address;
    cmd->data.cmd_config_client_unbind_model.elem_index=elem_index;
    cmd->data.cmd_config_client_unbind_model.vendor_id=vendor_id;
    cmd->data.cmd_config_client_unbind_model.model_id=model_id;
    cmd->data.cmd_config_client_unbind_model.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_config_client_unbind_model_id+(((11)&0xff)<<8)+(((11)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_unbind_model.handle;
    return rsp->data.rsp_config_client_unbind_model.result;

}

sl_status_t sl_btmesh_config_client_list_bindings(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint8_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_list_bindings.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_list_bindings.server_address=server_address;
    cmd->data.cmd_config_client_list_bindings.elem_index=elem_index;
    cmd->data.cmd_config_client_list_bindings.vendor_id=vendor_id;
    cmd->data.cmd_config_client_list_bindings.model_id=model_id;

    cmd->header=sl_btmesh_cmd_config_client_list_bindings_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_list_bindings.handle;
    return rsp->data.rsp_config_client_list_bindings.result;

}

sl_status_t sl_btmesh_config_client_get_model_pub(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint8_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_model_pub.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_model_pub.server_address=server_address;
    cmd->data.cmd_config_client_get_model_pub.elem_index=elem_index;
    cmd->data.cmd_config_client_get_model_pub.vendor_id=vendor_id;
    cmd->data.cmd_config_client_get_model_pub.model_id=model_id;

    cmd->header=sl_btmesh_cmd_config_client_get_model_pub_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_model_pub.handle;
    return rsp->data.rsp_config_client_get_model_pub.result;

}

sl_status_t sl_btmesh_config_client_set_model_pub(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint8_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint16_t address,
                                                  uint16_t appkey_index,
                                                  uint8_t credentials,
                                                  uint8_t ttl,
                                                  uint32_t period_ms,
                                                  uint8_t retransmit_count,
                                                  uint16_t retransmit_interval_ms,
                                                  uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_model_pub.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_model_pub.server_address=server_address;
    cmd->data.cmd_config_client_set_model_pub.elem_index=elem_index;
    cmd->data.cmd_config_client_set_model_pub.vendor_id=vendor_id;
    cmd->data.cmd_config_client_set_model_pub.model_id=model_id;
    cmd->data.cmd_config_client_set_model_pub.address=address;
    cmd->data.cmd_config_client_set_model_pub.appkey_index=appkey_index;
    cmd->data.cmd_config_client_set_model_pub.credentials=credentials;
    cmd->data.cmd_config_client_set_model_pub.ttl=ttl;
    cmd->data.cmd_config_client_set_model_pub.period_ms=period_ms;
    cmd->data.cmd_config_client_set_model_pub.retransmit_count=retransmit_count;
    cmd->data.cmd_config_client_set_model_pub.retransmit_interval_ms=retransmit_interval_ms;

    cmd->header=sl_btmesh_cmd_config_client_set_model_pub_id+(((22)&0xff)<<8)+(((22)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_model_pub.handle;
    return rsp->data.rsp_config_client_set_model_pub.result;

}

sl_status_t sl_btmesh_config_client_set_model_pub_va(uint16_t enc_netkey_index,
                                                     uint16_t server_address,
                                                     uint8_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id,
                                                     uuid_128 address,
                                                     uint16_t appkey_index,
                                                     uint8_t credentials,
                                                     uint8_t ttl,
                                                     uint32_t period_ms,
                                                     uint8_t retransmit_count,
                                                     uint16_t retransmit_interval_ms,
                                                     uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_model_pub_va.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_model_pub_va.server_address=server_address;
    cmd->data.cmd_config_client_set_model_pub_va.elem_index=elem_index;
    cmd->data.cmd_config_client_set_model_pub_va.vendor_id=vendor_id;
    cmd->data.cmd_config_client_set_model_pub_va.model_id=model_id;
    cmd->data.cmd_config_client_set_model_pub_va.address=address;
    cmd->data.cmd_config_client_set_model_pub_va.appkey_index=appkey_index;
    cmd->data.cmd_config_client_set_model_pub_va.credentials=credentials;
    cmd->data.cmd_config_client_set_model_pub_va.ttl=ttl;
    cmd->data.cmd_config_client_set_model_pub_va.period_ms=period_ms;
    cmd->data.cmd_config_client_set_model_pub_va.retransmit_count=retransmit_count;
    cmd->data.cmd_config_client_set_model_pub_va.retransmit_interval_ms=retransmit_interval_ms;

    cmd->header=sl_btmesh_cmd_config_client_set_model_pub_va_id+(((36)&0xff)<<8)+(((36)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_model_pub_va.handle;
    return rsp->data.rsp_config_client_set_model_pub_va.result;

}

sl_status_t sl_btmesh_config_client_add_model_sub(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint8_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint16_t sub_address,
                                                  uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_add_model_sub.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_add_model_sub.server_address=server_address;
    cmd->data.cmd_config_client_add_model_sub.elem_index=elem_index;
    cmd->data.cmd_config_client_add_model_sub.vendor_id=vendor_id;
    cmd->data.cmd_config_client_add_model_sub.model_id=model_id;
    cmd->data.cmd_config_client_add_model_sub.sub_address=sub_address;

    cmd->header=sl_btmesh_cmd_config_client_add_model_sub_id+(((11)&0xff)<<8)+(((11)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_add_model_sub.handle;
    return rsp->data.rsp_config_client_add_model_sub.result;

}

sl_status_t sl_btmesh_config_client_add_model_sub_va(uint16_t enc_netkey_index,
                                                     uint16_t server_address,
                                                     uint8_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id,
                                                     uuid_128 sub_address,
                                                     uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_add_model_sub_va.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_add_model_sub_va.server_address=server_address;
    cmd->data.cmd_config_client_add_model_sub_va.elem_index=elem_index;
    cmd->data.cmd_config_client_add_model_sub_va.vendor_id=vendor_id;
    cmd->data.cmd_config_client_add_model_sub_va.model_id=model_id;
    cmd->data.cmd_config_client_add_model_sub_va.sub_address=sub_address;

    cmd->header=sl_btmesh_cmd_config_client_add_model_sub_va_id+(((25)&0xff)<<8)+(((25)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_add_model_sub_va.handle;
    return rsp->data.rsp_config_client_add_model_sub_va.result;

}

sl_status_t sl_btmesh_config_client_remove_model_sub(uint16_t enc_netkey_index,
                                                     uint16_t server_address,
                                                     uint8_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id,
                                                     uint16_t sub_address,
                                                     uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_remove_model_sub.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_remove_model_sub.server_address=server_address;
    cmd->data.cmd_config_client_remove_model_sub.elem_index=elem_index;
    cmd->data.cmd_config_client_remove_model_sub.vendor_id=vendor_id;
    cmd->data.cmd_config_client_remove_model_sub.model_id=model_id;
    cmd->data.cmd_config_client_remove_model_sub.sub_address=sub_address;

    cmd->header=sl_btmesh_cmd_config_client_remove_model_sub_id+(((11)&0xff)<<8)+(((11)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_remove_model_sub.handle;
    return rsp->data.rsp_config_client_remove_model_sub.result;

}

sl_status_t sl_btmesh_config_client_remove_model_sub_va(uint16_t enc_netkey_index,
                                                        uint16_t server_address,
                                                        uint8_t elem_index,
                                                        uint16_t vendor_id,
                                                        uint16_t model_id,
                                                        uuid_128 sub_address,
                                                        uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_remove_model_sub_va.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_remove_model_sub_va.server_address=server_address;
    cmd->data.cmd_config_client_remove_model_sub_va.elem_index=elem_index;
    cmd->data.cmd_config_client_remove_model_sub_va.vendor_id=vendor_id;
    cmd->data.cmd_config_client_remove_model_sub_va.model_id=model_id;
    cmd->data.cmd_config_client_remove_model_sub_va.sub_address=sub_address;

    cmd->header=sl_btmesh_cmd_config_client_remove_model_sub_va_id+(((25)&0xff)<<8)+(((25)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_remove_model_sub_va.handle;
    return rsp->data.rsp_config_client_remove_model_sub_va.result;

}

sl_status_t sl_btmesh_config_client_set_model_sub(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint8_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint16_t sub_address,
                                                  uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_model_sub.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_model_sub.server_address=server_address;
    cmd->data.cmd_config_client_set_model_sub.elem_index=elem_index;
    cmd->data.cmd_config_client_set_model_sub.vendor_id=vendor_id;
    cmd->data.cmd_config_client_set_model_sub.model_id=model_id;
    cmd->data.cmd_config_client_set_model_sub.sub_address=sub_address;

    cmd->header=sl_btmesh_cmd_config_client_set_model_sub_id+(((11)&0xff)<<8)+(((11)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_model_sub.handle;
    return rsp->data.rsp_config_client_set_model_sub.result;

}

sl_status_t sl_btmesh_config_client_set_model_sub_va(uint16_t enc_netkey_index,
                                                     uint16_t server_address,
                                                     uint8_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id,
                                                     uuid_128 sub_address,
                                                     uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_model_sub_va.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_model_sub_va.server_address=server_address;
    cmd->data.cmd_config_client_set_model_sub_va.elem_index=elem_index;
    cmd->data.cmd_config_client_set_model_sub_va.vendor_id=vendor_id;
    cmd->data.cmd_config_client_set_model_sub_va.model_id=model_id;
    cmd->data.cmd_config_client_set_model_sub_va.sub_address=sub_address;

    cmd->header=sl_btmesh_cmd_config_client_set_model_sub_va_id+(((25)&0xff)<<8)+(((25)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_model_sub_va.handle;
    return rsp->data.rsp_config_client_set_model_sub_va.result;

}

sl_status_t sl_btmesh_config_client_clear_model_sub(uint16_t enc_netkey_index,
                                                    uint16_t server_address,
                                                    uint8_t elem_index,
                                                    uint16_t vendor_id,
                                                    uint16_t model_id,
                                                    uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_clear_model_sub.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_clear_model_sub.server_address=server_address;
    cmd->data.cmd_config_client_clear_model_sub.elem_index=elem_index;
    cmd->data.cmd_config_client_clear_model_sub.vendor_id=vendor_id;
    cmd->data.cmd_config_client_clear_model_sub.model_id=model_id;

    cmd->header=sl_btmesh_cmd_config_client_clear_model_sub_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_clear_model_sub.handle;
    return rsp->data.rsp_config_client_clear_model_sub.result;

}

sl_status_t sl_btmesh_config_client_list_subs(uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              uint8_t elem_index,
                                              uint16_t vendor_id,
                                              uint16_t model_id,
                                              uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_list_subs.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_list_subs.server_address=server_address;
    cmd->data.cmd_config_client_list_subs.elem_index=elem_index;
    cmd->data.cmd_config_client_list_subs.vendor_id=vendor_id;
    cmd->data.cmd_config_client_list_subs.model_id=model_id;

    cmd->header=sl_btmesh_cmd_config_client_list_subs_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_list_subs.handle;
    return rsp->data.rsp_config_client_list_subs.result;

}

sl_status_t sl_btmesh_config_client_get_heartbeat_pub(uint16_t enc_netkey_index,
                                                      uint16_t server_address,
                                                      uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_heartbeat_pub.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_heartbeat_pub.server_address=server_address;

    cmd->header=sl_btmesh_cmd_config_client_get_heartbeat_pub_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_heartbeat_pub.handle;
    return rsp->data.rsp_config_client_get_heartbeat_pub.result;

}

sl_status_t sl_btmesh_config_client_set_heartbeat_pub(uint16_t enc_netkey_index,
                                                      uint16_t server_address,
                                                      uint16_t destination_address,
                                                      uint16_t netkey_index,
                                                      uint8_t count_log,
                                                      uint8_t period_log,
                                                      uint8_t ttl,
                                                      uint16_t features,
                                                      uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_heartbeat_pub.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_heartbeat_pub.server_address=server_address;
    cmd->data.cmd_config_client_set_heartbeat_pub.destination_address=destination_address;
    cmd->data.cmd_config_client_set_heartbeat_pub.netkey_index=netkey_index;
    cmd->data.cmd_config_client_set_heartbeat_pub.count_log=count_log;
    cmd->data.cmd_config_client_set_heartbeat_pub.period_log=period_log;
    cmd->data.cmd_config_client_set_heartbeat_pub.ttl=ttl;
    cmd->data.cmd_config_client_set_heartbeat_pub.features=features;

    cmd->header=sl_btmesh_cmd_config_client_set_heartbeat_pub_id+(((13)&0xff)<<8)+(((13)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_heartbeat_pub.handle;
    return rsp->data.rsp_config_client_set_heartbeat_pub.result;

}

sl_status_t sl_btmesh_config_client_get_heartbeat_sub(uint16_t enc_netkey_index,
                                                      uint16_t server_address,
                                                      uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_heartbeat_sub.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_heartbeat_sub.server_address=server_address;

    cmd->header=sl_btmesh_cmd_config_client_get_heartbeat_sub_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_heartbeat_sub.handle;
    return rsp->data.rsp_config_client_get_heartbeat_sub.result;

}

sl_status_t sl_btmesh_config_client_set_heartbeat_sub(uint16_t enc_netkey_index,
                                                      uint16_t server_address,
                                                      uint16_t source_address,
                                                      uint16_t destination_address,
                                                      uint8_t period_log,
                                                      uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_heartbeat_sub.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_heartbeat_sub.server_address=server_address;
    cmd->data.cmd_config_client_set_heartbeat_sub.source_address=source_address;
    cmd->data.cmd_config_client_set_heartbeat_sub.destination_address=destination_address;
    cmd->data.cmd_config_client_set_heartbeat_sub.period_log=period_log;

    cmd->header=sl_btmesh_cmd_config_client_set_heartbeat_sub_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_heartbeat_sub.handle;
    return rsp->data.rsp_config_client_set_heartbeat_sub.result;

}

sl_status_t sl_btmesh_config_client_get_beacon(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_beacon.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_beacon.server_address=server_address;

    cmd->header=sl_btmesh_cmd_config_client_get_beacon_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_beacon.handle;
    return rsp->data.rsp_config_client_get_beacon.result;

}

sl_status_t sl_btmesh_config_client_set_beacon(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint8_t value,
                                               uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_beacon.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_beacon.server_address=server_address;
    cmd->data.cmd_config_client_set_beacon.value=value;

    cmd->header=sl_btmesh_cmd_config_client_set_beacon_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_beacon.handle;
    return rsp->data.rsp_config_client_set_beacon.result;

}

sl_status_t sl_btmesh_config_client_get_default_ttl(uint16_t enc_netkey_index,
                                                    uint16_t server_address,
                                                    uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_default_ttl.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_default_ttl.server_address=server_address;

    cmd->header=sl_btmesh_cmd_config_client_get_default_ttl_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_default_ttl.handle;
    return rsp->data.rsp_config_client_get_default_ttl.result;

}

sl_status_t sl_btmesh_config_client_set_default_ttl(uint16_t enc_netkey_index,
                                                    uint16_t server_address,
                                                    uint8_t value,
                                                    uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_default_ttl.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_default_ttl.server_address=server_address;
    cmd->data.cmd_config_client_set_default_ttl.value=value;

    cmd->header=sl_btmesh_cmd_config_client_set_default_ttl_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_default_ttl.handle;
    return rsp->data.rsp_config_client_set_default_ttl.result;

}

sl_status_t sl_btmesh_config_client_get_gatt_proxy(uint16_t enc_netkey_index,
                                                   uint16_t server_address,
                                                   uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_gatt_proxy.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_gatt_proxy.server_address=server_address;

    cmd->header=sl_btmesh_cmd_config_client_get_gatt_proxy_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_gatt_proxy.handle;
    return rsp->data.rsp_config_client_get_gatt_proxy.result;

}

sl_status_t sl_btmesh_config_client_set_gatt_proxy(uint16_t enc_netkey_index,
                                                   uint16_t server_address,
                                                   uint8_t value,
                                                   uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_gatt_proxy.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_gatt_proxy.server_address=server_address;
    cmd->data.cmd_config_client_set_gatt_proxy.value=value;

    cmd->header=sl_btmesh_cmd_config_client_set_gatt_proxy_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_gatt_proxy.handle;
    return rsp->data.rsp_config_client_set_gatt_proxy.result;

}

sl_status_t sl_btmesh_config_client_get_relay(uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_relay.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_relay.server_address=server_address;

    cmd->header=sl_btmesh_cmd_config_client_get_relay_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_relay.handle;
    return rsp->data.rsp_config_client_get_relay.result;

}

sl_status_t sl_btmesh_config_client_set_relay(uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              uint8_t value,
                                              uint8_t retransmit_count,
                                              uint16_t retransmit_interval_ms,
                                              uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_relay.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_relay.server_address=server_address;
    cmd->data.cmd_config_client_set_relay.value=value;
    cmd->data.cmd_config_client_set_relay.retransmit_count=retransmit_count;
    cmd->data.cmd_config_client_set_relay.retransmit_interval_ms=retransmit_interval_ms;

    cmd->header=sl_btmesh_cmd_config_client_set_relay_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_relay.handle;
    return rsp->data.rsp_config_client_set_relay.result;

}

sl_status_t sl_btmesh_config_client_get_network_transmit(uint16_t enc_netkey_index,
                                                         uint16_t server_address,
                                                         uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_network_transmit.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_network_transmit.server_address=server_address;

    cmd->header=sl_btmesh_cmd_config_client_get_network_transmit_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_network_transmit.handle;
    return rsp->data.rsp_config_client_get_network_transmit.result;

}

sl_status_t sl_btmesh_config_client_set_network_transmit(uint16_t enc_netkey_index,
                                                         uint16_t server_address,
                                                         uint8_t transmit_count,
                                                         uint16_t transmit_interval_ms,
                                                         uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_network_transmit.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_network_transmit.server_address=server_address;
    cmd->data.cmd_config_client_set_network_transmit.transmit_count=transmit_count;
    cmd->data.cmd_config_client_set_network_transmit.transmit_interval_ms=transmit_interval_ms;

    cmd->header=sl_btmesh_cmd_config_client_set_network_transmit_id+(((7)&0xff)<<8)+(((7)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_network_transmit.handle;
    return rsp->data.rsp_config_client_set_network_transmit.result;

}

sl_status_t sl_btmesh_config_client_get_identity(uint16_t enc_netkey_index,
                                                 uint16_t server_address,
                                                 uint16_t netkey_index,
                                                 uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_identity.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_identity.server_address=server_address;
    cmd->data.cmd_config_client_get_identity.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_config_client_get_identity_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_identity.handle;
    return rsp->data.rsp_config_client_get_identity.result;

}

sl_status_t sl_btmesh_config_client_set_identity(uint16_t enc_netkey_index,
                                                 uint16_t server_address,
                                                 uint16_t netkey_index,
                                                 uint8_t value,
                                                 uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_identity.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_identity.server_address=server_address;
    cmd->data.cmd_config_client_set_identity.netkey_index=netkey_index;
    cmd->data.cmd_config_client_set_identity.value=value;

    cmd->header=sl_btmesh_cmd_config_client_set_identity_id+(((7)&0xff)<<8)+(((7)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_identity.handle;
    return rsp->data.rsp_config_client_set_identity.result;

}

sl_status_t sl_btmesh_config_client_get_friend(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_friend.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_friend.server_address=server_address;

    cmd->header=sl_btmesh_cmd_config_client_get_friend_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_friend.handle;
    return rsp->data.rsp_config_client_get_friend.result;

}

sl_status_t sl_btmesh_config_client_set_friend(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint8_t value,
                                               uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_set_friend.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_set_friend.server_address=server_address;
    cmd->data.cmd_config_client_set_friend.value=value;

    cmd->header=sl_btmesh_cmd_config_client_set_friend_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_set_friend.handle;
    return rsp->data.rsp_config_client_set_friend.result;

}

sl_status_t sl_btmesh_config_client_get_key_refresh_phase(uint16_t enc_netkey_index,
                                                          uint16_t server_address,
                                                          uint16_t netkey_index,
                                                          uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_key_refresh_phase.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_key_refresh_phase.server_address=server_address;
    cmd->data.cmd_config_client_get_key_refresh_phase.netkey_index=netkey_index;

    cmd->header=sl_btmesh_cmd_config_client_get_key_refresh_phase_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_key_refresh_phase.handle;
    return rsp->data.rsp_config_client_get_key_refresh_phase.result;

}

sl_status_t sl_btmesh_config_client_get_lpn_polltimeout(uint16_t enc_netkey_index,
                                                        uint16_t server_address,
                                                        uint16_t lpn_address,
                                                        uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_lpn_polltimeout.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_lpn_polltimeout.server_address=server_address;
    cmd->data.cmd_config_client_get_lpn_polltimeout.lpn_address=lpn_address;

    cmd->header=sl_btmesh_cmd_config_client_get_lpn_polltimeout_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_lpn_polltimeout.handle;
    return rsp->data.rsp_config_client_get_lpn_polltimeout.result;

}

sl_status_t sl_btmesh_config_client_get_dcd(uint16_t enc_netkey_index,
                                            uint16_t server_address,
                                            uint8_t page,
                                            uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_get_dcd.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_get_dcd.server_address=server_address;
    cmd->data.cmd_config_client_get_dcd.page=page;

    cmd->header=sl_btmesh_cmd_config_client_get_dcd_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_get_dcd.handle;
    return rsp->data.rsp_config_client_get_dcd.result;

}

sl_status_t sl_btmesh_config_client_reset_node(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint32_t *handle) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_config_client_reset_node.enc_netkey_index=enc_netkey_index;
    cmd->data.cmd_config_client_reset_node.server_address=server_address;

    cmd->header=sl_btmesh_cmd_config_client_reset_node_id+(((4)&0xff)<<8)+(((4)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *handle = rsp->data.rsp_config_client_reset_node.handle;
    return rsp->data.rsp_config_client_reset_node.result;

}

sl_status_t sl_btmesh_sensor_server_init(uint16_t elem_index,
                                         size_t descriptors_len,
                                         const uint8_t* descriptors) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_server_init.elem_index=elem_index;
    if ((3+descriptors_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_server_init.descriptors.len=descriptors_len;
    memcpy(cmd->data.cmd_sensor_server_init.descriptors.data,descriptors,descriptors_len);

    cmd->header=sl_btmesh_cmd_sensor_server_init_id+(((3+descriptors_len)&0xff)<<8)+(((3+descriptors_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_server_init.result;

}

sl_status_t sl_btmesh_sensor_server_deinit(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_server_deinit.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_sensor_server_deinit_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_server_deinit.result;

}

sl_status_t sl_btmesh_sensor_server_send_descriptor_status(uint16_t client_address,
                                                           uint16_t elem_index,
                                                           uint16_t appkey_index,
                                                           uint8_t flags,
                                                           size_t descriptors_len,
                                                           const uint8_t* descriptors) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_server_send_descriptor_status.client_address=client_address;
    cmd->data.cmd_sensor_server_send_descriptor_status.elem_index=elem_index;
    cmd->data.cmd_sensor_server_send_descriptor_status.appkey_index=appkey_index;
    cmd->data.cmd_sensor_server_send_descriptor_status.flags=flags;
    if ((8+descriptors_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_server_send_descriptor_status.descriptors.len=descriptors_len;
    memcpy(cmd->data.cmd_sensor_server_send_descriptor_status.descriptors.data,descriptors,descriptors_len);

    cmd->header=sl_btmesh_cmd_sensor_server_send_descriptor_status_id+(((8+descriptors_len)&0xff)<<8)+(((8+descriptors_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_server_send_descriptor_status.result;

}

sl_status_t sl_btmesh_sensor_server_send_status(uint16_t client_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                size_t sensor_data_len,
                                                const uint8_t* sensor_data) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_server_send_status.client_address=client_address;
    cmd->data.cmd_sensor_server_send_status.elem_index=elem_index;
    cmd->data.cmd_sensor_server_send_status.appkey_index=appkey_index;
    cmd->data.cmd_sensor_server_send_status.flags=flags;
    if ((8+sensor_data_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_server_send_status.sensor_data.len=sensor_data_len;
    memcpy(cmd->data.cmd_sensor_server_send_status.sensor_data.data,sensor_data,sensor_data_len);

    cmd->header=sl_btmesh_cmd_sensor_server_send_status_id+(((8+sensor_data_len)&0xff)<<8)+(((8+sensor_data_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_server_send_status.result;

}

sl_status_t sl_btmesh_sensor_server_send_column_status(uint16_t client_address,
                                                       uint16_t elem_index,
                                                       uint16_t appkey_index,
                                                       uint8_t flags,
                                                       uint16_t property_id,
                                                       size_t sensor_data_len,
                                                       const uint8_t* sensor_data) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_server_send_column_status.client_address=client_address;
    cmd->data.cmd_sensor_server_send_column_status.elem_index=elem_index;
    cmd->data.cmd_sensor_server_send_column_status.appkey_index=appkey_index;
    cmd->data.cmd_sensor_server_send_column_status.flags=flags;
    cmd->data.cmd_sensor_server_send_column_status.property_id=property_id;
    if ((10+sensor_data_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_server_send_column_status.sensor_data.len=sensor_data_len;
    memcpy(cmd->data.cmd_sensor_server_send_column_status.sensor_data.data,sensor_data,sensor_data_len);

    cmd->header=sl_btmesh_cmd_sensor_server_send_column_status_id+(((10+sensor_data_len)&0xff)<<8)+(((10+sensor_data_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_server_send_column_status.result;

}

sl_status_t sl_btmesh_sensor_server_send_series_status(uint16_t client_address,
                                                       uint16_t elem_index,
                                                       uint16_t appkey_index,
                                                       uint8_t flags,
                                                       uint16_t property_id,
                                                       size_t sensor_data_len,
                                                       const uint8_t* sensor_data) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_server_send_series_status.client_address=client_address;
    cmd->data.cmd_sensor_server_send_series_status.elem_index=elem_index;
    cmd->data.cmd_sensor_server_send_series_status.appkey_index=appkey_index;
    cmd->data.cmd_sensor_server_send_series_status.flags=flags;
    cmd->data.cmd_sensor_server_send_series_status.property_id=property_id;
    if ((10+sensor_data_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_server_send_series_status.sensor_data.len=sensor_data_len;
    memcpy(cmd->data.cmd_sensor_server_send_series_status.sensor_data.data,sensor_data,sensor_data_len);

    cmd->header=sl_btmesh_cmd_sensor_server_send_series_status_id+(((10+sensor_data_len)&0xff)<<8)+(((10+sensor_data_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_server_send_series_status.result;

}

sl_status_t sl_btmesh_sensor_setup_server_send_cadence_status(uint16_t client_address,
                                                              uint16_t elem_index,
                                                              uint16_t appkey_index,
                                                              uint8_t flags,
                                                              uint16_t property_id,
                                                              size_t params_len,
                                                              const uint8_t* params) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_setup_server_send_cadence_status.client_address=client_address;
    cmd->data.cmd_sensor_setup_server_send_cadence_status.elem_index=elem_index;
    cmd->data.cmd_sensor_setup_server_send_cadence_status.appkey_index=appkey_index;
    cmd->data.cmd_sensor_setup_server_send_cadence_status.flags=flags;
    cmd->data.cmd_sensor_setup_server_send_cadence_status.property_id=property_id;
    if ((10+params_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_setup_server_send_cadence_status.params.len=params_len;
    memcpy(cmd->data.cmd_sensor_setup_server_send_cadence_status.params.data,params,params_len);

    cmd->header=sl_btmesh_cmd_sensor_setup_server_send_cadence_status_id+(((10+params_len)&0xff)<<8)+(((10+params_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_setup_server_send_cadence_status.result;

}

sl_status_t sl_btmesh_sensor_setup_server_send_settings_status(uint16_t client_address,
                                                               uint16_t elem_index,
                                                               uint16_t appkey_index,
                                                               uint8_t flags,
                                                               uint16_t property_id,
                                                               size_t setting_ids_len,
                                                               const uint8_t* setting_ids) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_setup_server_send_settings_status.client_address=client_address;
    cmd->data.cmd_sensor_setup_server_send_settings_status.elem_index=elem_index;
    cmd->data.cmd_sensor_setup_server_send_settings_status.appkey_index=appkey_index;
    cmd->data.cmd_sensor_setup_server_send_settings_status.flags=flags;
    cmd->data.cmd_sensor_setup_server_send_settings_status.property_id=property_id;
    if ((10+setting_ids_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_setup_server_send_settings_status.setting_ids.len=setting_ids_len;
    memcpy(cmd->data.cmd_sensor_setup_server_send_settings_status.setting_ids.data,setting_ids,setting_ids_len);

    cmd->header=sl_btmesh_cmd_sensor_setup_server_send_settings_status_id+(((10+setting_ids_len)&0xff)<<8)+(((10+setting_ids_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_setup_server_send_settings_status.result;

}

sl_status_t sl_btmesh_sensor_setup_server_send_setting_status(uint16_t client_address,
                                                              uint16_t elem_index,
                                                              uint16_t appkey_index,
                                                              uint8_t flags,
                                                              uint16_t property_id,
                                                              uint16_t setting_id,
                                                              size_t raw_value_len,
                                                              const uint8_t* raw_value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_setup_server_send_setting_status.client_address=client_address;
    cmd->data.cmd_sensor_setup_server_send_setting_status.elem_index=elem_index;
    cmd->data.cmd_sensor_setup_server_send_setting_status.appkey_index=appkey_index;
    cmd->data.cmd_sensor_setup_server_send_setting_status.flags=flags;
    cmd->data.cmd_sensor_setup_server_send_setting_status.property_id=property_id;
    cmd->data.cmd_sensor_setup_server_send_setting_status.setting_id=setting_id;
    if ((12+raw_value_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_setup_server_send_setting_status.raw_value.len=raw_value_len;
    memcpy(cmd->data.cmd_sensor_setup_server_send_setting_status.raw_value.data,raw_value,raw_value_len);

    cmd->header=sl_btmesh_cmd_sensor_setup_server_send_setting_status_id+(((12+raw_value_len)&0xff)<<8)+(((12+raw_value_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_setup_server_send_setting_status.result;

}

sl_status_t sl_btmesh_sensor_client_init() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_sensor_client_init_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_init.result;

}

sl_status_t sl_btmesh_sensor_client_deinit() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_sensor_client_deinit_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_deinit.result;

}

sl_status_t sl_btmesh_sensor_client_get_descriptor(uint16_t server_address,
                                                   uint16_t elem_index,
                                                   uint16_t appkey_index,
                                                   uint8_t flags,
                                                   uint16_t property_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_client_get_descriptor.server_address=server_address;
    cmd->data.cmd_sensor_client_get_descriptor.elem_index=elem_index;
    cmd->data.cmd_sensor_client_get_descriptor.appkey_index=appkey_index;
    cmd->data.cmd_sensor_client_get_descriptor.flags=flags;
    cmd->data.cmd_sensor_client_get_descriptor.property_id=property_id;

    cmd->header=sl_btmesh_cmd_sensor_client_get_descriptor_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_get_descriptor.result;

}

sl_status_t sl_btmesh_sensor_client_get(uint16_t server_address,
                                        uint16_t elem_index,
                                        uint16_t appkey_index,
                                        uint8_t flags,
                                        uint16_t property_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_client_get.server_address=server_address;
    cmd->data.cmd_sensor_client_get.elem_index=elem_index;
    cmd->data.cmd_sensor_client_get.appkey_index=appkey_index;
    cmd->data.cmd_sensor_client_get.flags=flags;
    cmd->data.cmd_sensor_client_get.property_id=property_id;

    cmd->header=sl_btmesh_cmd_sensor_client_get_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_get.result;

}

sl_status_t sl_btmesh_sensor_client_get_column(uint16_t server_address,
                                               uint16_t elem_index,
                                               uint16_t appkey_index,
                                               uint8_t flags,
                                               uint16_t property_id,
                                               size_t column_id_len,
                                               const uint8_t* column_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_client_get_column.server_address=server_address;
    cmd->data.cmd_sensor_client_get_column.elem_index=elem_index;
    cmd->data.cmd_sensor_client_get_column.appkey_index=appkey_index;
    cmd->data.cmd_sensor_client_get_column.flags=flags;
    cmd->data.cmd_sensor_client_get_column.property_id=property_id;
    if ((10+column_id_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_client_get_column.column_id.len=column_id_len;
    memcpy(cmd->data.cmd_sensor_client_get_column.column_id.data,column_id,column_id_len);

    cmd->header=sl_btmesh_cmd_sensor_client_get_column_id+(((10+column_id_len)&0xff)<<8)+(((10+column_id_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_get_column.result;

}

sl_status_t sl_btmesh_sensor_client_get_series(uint16_t server_address,
                                               uint16_t elem_index,
                                               uint16_t appkey_index,
                                               uint8_t flags,
                                               uint16_t property_id,
                                               size_t column_ids_len,
                                               const uint8_t* column_ids) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_client_get_series.server_address=server_address;
    cmd->data.cmd_sensor_client_get_series.elem_index=elem_index;
    cmd->data.cmd_sensor_client_get_series.appkey_index=appkey_index;
    cmd->data.cmd_sensor_client_get_series.flags=flags;
    cmd->data.cmd_sensor_client_get_series.property_id=property_id;
    if ((10+column_ids_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_client_get_series.column_ids.len=column_ids_len;
    memcpy(cmd->data.cmd_sensor_client_get_series.column_ids.data,column_ids,column_ids_len);

    cmd->header=sl_btmesh_cmd_sensor_client_get_series_id+(((10+column_ids_len)&0xff)<<8)+(((10+column_ids_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_get_series.result;

}

sl_status_t sl_btmesh_sensor_client_get_cadence(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                uint16_t property_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_client_get_cadence.server_address=server_address;
    cmd->data.cmd_sensor_client_get_cadence.elem_index=elem_index;
    cmd->data.cmd_sensor_client_get_cadence.appkey_index=appkey_index;
    cmd->data.cmd_sensor_client_get_cadence.flags=flags;
    cmd->data.cmd_sensor_client_get_cadence.property_id=property_id;

    cmd->header=sl_btmesh_cmd_sensor_client_get_cadence_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_get_cadence.result;

}

sl_status_t sl_btmesh_sensor_client_set_cadence(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                uint16_t property_id,
                                                size_t params_len,
                                                const uint8_t* params) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_client_set_cadence.server_address=server_address;
    cmd->data.cmd_sensor_client_set_cadence.elem_index=elem_index;
    cmd->data.cmd_sensor_client_set_cadence.appkey_index=appkey_index;
    cmd->data.cmd_sensor_client_set_cadence.flags=flags;
    cmd->data.cmd_sensor_client_set_cadence.property_id=property_id;
    if ((10+params_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_client_set_cadence.params.len=params_len;
    memcpy(cmd->data.cmd_sensor_client_set_cadence.params.data,params,params_len);

    cmd->header=sl_btmesh_cmd_sensor_client_set_cadence_id+(((10+params_len)&0xff)<<8)+(((10+params_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_set_cadence.result;

}

sl_status_t sl_btmesh_sensor_client_get_settings(uint16_t server_address,
                                                 uint16_t elem_index,
                                                 uint16_t appkey_index,
                                                 uint8_t flags,
                                                 uint16_t property_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_client_get_settings.server_address=server_address;
    cmd->data.cmd_sensor_client_get_settings.elem_index=elem_index;
    cmd->data.cmd_sensor_client_get_settings.appkey_index=appkey_index;
    cmd->data.cmd_sensor_client_get_settings.flags=flags;
    cmd->data.cmd_sensor_client_get_settings.property_id=property_id;

    cmd->header=sl_btmesh_cmd_sensor_client_get_settings_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_get_settings.result;

}

sl_status_t sl_btmesh_sensor_client_get_setting(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                uint16_t property_id,
                                                uint16_t setting_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_client_get_setting.server_address=server_address;
    cmd->data.cmd_sensor_client_get_setting.elem_index=elem_index;
    cmd->data.cmd_sensor_client_get_setting.appkey_index=appkey_index;
    cmd->data.cmd_sensor_client_get_setting.flags=flags;
    cmd->data.cmd_sensor_client_get_setting.property_id=property_id;
    cmd->data.cmd_sensor_client_get_setting.setting_id=setting_id;

    cmd->header=sl_btmesh_cmd_sensor_client_get_setting_id+(((11)&0xff)<<8)+(((11)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_get_setting.result;

}

sl_status_t sl_btmesh_sensor_client_set_setting(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                uint16_t property_id,
                                                uint16_t setting_id,
                                                size_t raw_value_len,
                                                const uint8_t* raw_value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_sensor_client_set_setting.server_address=server_address;
    cmd->data.cmd_sensor_client_set_setting.elem_index=elem_index;
    cmd->data.cmd_sensor_client_set_setting.appkey_index=appkey_index;
    cmd->data.cmd_sensor_client_set_setting.flags=flags;
    cmd->data.cmd_sensor_client_set_setting.property_id=property_id;
    cmd->data.cmd_sensor_client_set_setting.setting_id=setting_id;
    if ((12+raw_value_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_sensor_client_set_setting.raw_value.len=raw_value_len;
    memcpy(cmd->data.cmd_sensor_client_set_setting.raw_value.data,raw_value,raw_value_len);

    cmd->header=sl_btmesh_cmd_sensor_client_set_setting_id+(((12+raw_value_len)&0xff)<<8)+(((12+raw_value_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_sensor_client_set_setting.result;

}

sl_status_t sl_btmesh_lc_client_init(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_client_init.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_lc_client_init_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_client_init.result;

}

sl_status_t sl_btmesh_lc_client_get_mode(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_client_get_mode.server_address=server_address;
    cmd->data.cmd_lc_client_get_mode.elem_index=elem_index;
    cmd->data.cmd_lc_client_get_mode.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_lc_client_get_mode_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_client_get_mode.result;

}

sl_status_t sl_btmesh_lc_client_set_mode(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t appkey_index,
                                         uint8_t flags,
                                         uint8_t mode) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_client_set_mode.server_address=server_address;
    cmd->data.cmd_lc_client_set_mode.elem_index=elem_index;
    cmd->data.cmd_lc_client_set_mode.appkey_index=appkey_index;
    cmd->data.cmd_lc_client_set_mode.flags=flags;
    cmd->data.cmd_lc_client_set_mode.mode=mode;

    cmd->header=sl_btmesh_cmd_lc_client_set_mode_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_client_set_mode.result;

}

sl_status_t sl_btmesh_lc_client_get_om(uint16_t server_address,
                                       uint16_t elem_index,
                                       uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_client_get_om.server_address=server_address;
    cmd->data.cmd_lc_client_get_om.elem_index=elem_index;
    cmd->data.cmd_lc_client_get_om.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_lc_client_get_om_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_client_get_om.result;

}

sl_status_t sl_btmesh_lc_client_set_om(uint16_t server_address,
                                       uint16_t elem_index,
                                       uint16_t appkey_index,
                                       uint8_t flags,
                                       uint8_t mode) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_client_set_om.server_address=server_address;
    cmd->data.cmd_lc_client_set_om.elem_index=elem_index;
    cmd->data.cmd_lc_client_set_om.appkey_index=appkey_index;
    cmd->data.cmd_lc_client_set_om.flags=flags;
    cmd->data.cmd_lc_client_set_om.mode=mode;

    cmd->header=sl_btmesh_cmd_lc_client_set_om_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_client_set_om.result;

}

sl_status_t sl_btmesh_lc_client_get_light_onoff(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_client_get_light_onoff.server_address=server_address;
    cmd->data.cmd_lc_client_get_light_onoff.elem_index=elem_index;
    cmd->data.cmd_lc_client_get_light_onoff.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_lc_client_get_light_onoff_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_client_get_light_onoff.result;

}

sl_status_t sl_btmesh_lc_client_set_light_onoff(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                uint8_t target_state,
                                                uint8_t tid,
                                                uint32_t transition_time_ms,
                                                uint16_t message_delay_ms) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_client_set_light_onoff.server_address=server_address;
    cmd->data.cmd_lc_client_set_light_onoff.elem_index=elem_index;
    cmd->data.cmd_lc_client_set_light_onoff.appkey_index=appkey_index;
    cmd->data.cmd_lc_client_set_light_onoff.flags=flags;
    cmd->data.cmd_lc_client_set_light_onoff.target_state=target_state;
    cmd->data.cmd_lc_client_set_light_onoff.tid=tid;
    cmd->data.cmd_lc_client_set_light_onoff.transition_time_ms=transition_time_ms;
    cmd->data.cmd_lc_client_set_light_onoff.message_delay_ms=message_delay_ms;

    cmd->header=sl_btmesh_cmd_lc_client_set_light_onoff_id+(((15)&0xff)<<8)+(((15)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_client_set_light_onoff.result;

}

sl_status_t sl_btmesh_lc_client_get_property(uint16_t server_address,
                                             uint16_t elem_index,
                                             uint16_t appkey_index,
                                             uint16_t property_id) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_client_get_property.server_address=server_address;
    cmd->data.cmd_lc_client_get_property.elem_index=elem_index;
    cmd->data.cmd_lc_client_get_property.appkey_index=appkey_index;
    cmd->data.cmd_lc_client_get_property.property_id=property_id;

    cmd->header=sl_btmesh_cmd_lc_client_get_property_id+(((8)&0xff)<<8)+(((8)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_client_get_property.result;

}

sl_status_t sl_btmesh_lc_client_set_property(uint16_t server_address,
                                             uint16_t elem_index,
                                             uint16_t appkey_index,
                                             uint8_t flags,
                                             uint16_t property_id,
                                             size_t params_len,
                                             const uint8_t* params) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_client_set_property.server_address=server_address;
    cmd->data.cmd_lc_client_set_property.elem_index=elem_index;
    cmd->data.cmd_lc_client_set_property.appkey_index=appkey_index;
    cmd->data.cmd_lc_client_set_property.flags=flags;
    cmd->data.cmd_lc_client_set_property.property_id=property_id;
    if ((10+params_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_lc_client_set_property.params.len=params_len;
    memcpy(cmd->data.cmd_lc_client_set_property.params.data,params,params_len);

    cmd->header=sl_btmesh_cmd_lc_client_set_property_id+(((10+params_len)&0xff)<<8)+(((10+params_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_client_set_property.result;

}

sl_status_t sl_btmesh_lc_server_init(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_server_init.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_lc_server_init_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_server_init.result;

}

sl_status_t sl_btmesh_lc_server_deinit(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_server_deinit.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_lc_server_deinit_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_server_deinit.result;

}

sl_status_t sl_btmesh_lc_server_update_mode(uint16_t elem_index, uint8_t mode) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_server_update_mode.elem_index=elem_index;
    cmd->data.cmd_lc_server_update_mode.mode=mode;

    cmd->header=sl_btmesh_cmd_lc_server_update_mode_id+(((3)&0xff)<<8)+(((3)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_server_update_mode.result;

}

sl_status_t sl_btmesh_lc_server_update_om(uint16_t elem_index, uint8_t om) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_server_update_om.elem_index=elem_index;
    cmd->data.cmd_lc_server_update_om.om=om;

    cmd->header=sl_btmesh_cmd_lc_server_update_om_id+(((3)&0xff)<<8)+(((3)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_server_update_om.result;

}

sl_status_t sl_btmesh_lc_server_update_light_onoff(uint16_t elem_index,
                                                   uint8_t light_onoff,
                                                   uint32_t transition_time_ms) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_server_update_light_onoff.elem_index=elem_index;
    cmd->data.cmd_lc_server_update_light_onoff.light_onoff=light_onoff;
    cmd->data.cmd_lc_server_update_light_onoff.transition_time_ms=transition_time_ms;

    cmd->header=sl_btmesh_cmd_lc_server_update_light_onoff_id+(((7)&0xff)<<8)+(((7)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_server_update_light_onoff.result;

}

sl_status_t sl_btmesh_lc_server_init_all_properties(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_server_init_all_properties.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_lc_server_init_all_properties_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_server_init_all_properties.result;

}

sl_status_t sl_btmesh_lc_server_set_publish_mask(uint16_t elem_index,
                                                 uint16_t status_type,
                                                 uint8_t value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_server_set_publish_mask.elem_index=elem_index;
    cmd->data.cmd_lc_server_set_publish_mask.status_type=status_type;
    cmd->data.cmd_lc_server_set_publish_mask.value=value;

    cmd->header=sl_btmesh_cmd_lc_server_set_publish_mask_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_server_set_publish_mask.result;

}

sl_status_t sl_btmesh_lc_server_set_regulator_interval(uint16_t elem_index,
                                                       uint8_t value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_server_set_regulator_interval.elem_index=elem_index;
    cmd->data.cmd_lc_server_set_regulator_interval.value=value;

    cmd->header=sl_btmesh_cmd_lc_server_set_regulator_interval_id+(((3)&0xff)<<8)+(((3)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_server_set_regulator_interval.result;

}

sl_status_t sl_btmesh_lc_server_set_event_mask(uint16_t elem_index,
                                               uint16_t event_type,
                                               uint8_t value) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_server_set_event_mask.elem_index=elem_index;
    cmd->data.cmd_lc_server_set_event_mask.event_type=event_type;
    cmd->data.cmd_lc_server_set_event_mask.value=value;

    cmd->header=sl_btmesh_cmd_lc_server_set_event_mask_id+(((5)&0xff)<<8)+(((5)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_server_set_event_mask.result;

}

sl_status_t sl_btmesh_lc_server_get_lc_state(uint16_t elem_index,
                                             uint8_t *state,
                                             uint32_t *transition_time) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_server_get_lc_state.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_lc_server_get_lc_state_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *state = rsp->data.rsp_lc_server_get_lc_state.state;
    *transition_time = rsp->data.rsp_lc_server_get_lc_state.transition_time;
    return rsp->data.rsp_lc_server_get_lc_state.result;

}

sl_status_t sl_btmesh_lc_setup_server_update_property(uint16_t elem_index,
                                                      uint16_t property_id,
                                                      size_t params_len,
                                                      const uint8_t* params) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_lc_setup_server_update_property.elem_index=elem_index;
    cmd->data.cmd_lc_setup_server_update_property.property_id=property_id;
    if ((5+params_len) > SL_BGAPI_MAX_PAYLOAD_SIZE )
    {
        return SL_STATUS_COMMAND_TOO_LONG;
    }
    cmd->data.cmd_lc_setup_server_update_property.params.len=params_len;
    memcpy(cmd->data.cmd_lc_setup_server_update_property.params.data,params,params_len);

    cmd->header=sl_btmesh_cmd_lc_setup_server_update_property_id+(((5+params_len)&0xff)<<8)+(((5+params_len)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_lc_setup_server_update_property.result;

}

sl_status_t sl_btmesh_scene_client_init(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scene_client_init.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_scene_client_init_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_client_init.result;

}

sl_status_t sl_btmesh_scene_client_get(uint16_t server_address,
                                       uint16_t elem_index,
                                       uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scene_client_get.server_address=server_address;
    cmd->data.cmd_scene_client_get.elem_index=elem_index;
    cmd->data.cmd_scene_client_get.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_scene_client_get_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_client_get.result;

}

sl_status_t sl_btmesh_scene_client_get_register(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scene_client_get_register.server_address=server_address;
    cmd->data.cmd_scene_client_get_register.elem_index=elem_index;
    cmd->data.cmd_scene_client_get_register.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_scene_client_get_register_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_client_get_register.result;

}

sl_status_t sl_btmesh_scene_client_recall(uint16_t server_address,
                                          uint16_t elem_index,
                                          uint16_t selected_scene,
                                          uint16_t appkey_index,
                                          uint8_t flags,
                                          uint8_t tid,
                                          uint32_t transition_time_ms,
                                          uint16_t delay_ms) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scene_client_recall.server_address=server_address;
    cmd->data.cmd_scene_client_recall.elem_index=elem_index;
    cmd->data.cmd_scene_client_recall.selected_scene=selected_scene;
    cmd->data.cmd_scene_client_recall.appkey_index=appkey_index;
    cmd->data.cmd_scene_client_recall.flags=flags;
    cmd->data.cmd_scene_client_recall.tid=tid;
    cmd->data.cmd_scene_client_recall.transition_time_ms=transition_time_ms;
    cmd->data.cmd_scene_client_recall.delay_ms=delay_ms;

    cmd->header=sl_btmesh_cmd_scene_client_recall_id+(((16)&0xff)<<8)+(((16)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_client_recall.result;

}

sl_status_t sl_btmesh_scene_client_store(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t selected_scene,
                                         uint16_t appkey_index,
                                         uint8_t flags) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scene_client_store.server_address=server_address;
    cmd->data.cmd_scene_client_store.elem_index=elem_index;
    cmd->data.cmd_scene_client_store.selected_scene=selected_scene;
    cmd->data.cmd_scene_client_store.appkey_index=appkey_index;
    cmd->data.cmd_scene_client_store.flags=flags;

    cmd->header=sl_btmesh_cmd_scene_client_store_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_client_store.result;

}

sl_status_t sl_btmesh_scene_client_delete(uint16_t server_address,
                                          uint16_t elem_index,
                                          uint16_t selected_scene,
                                          uint16_t appkey_index,
                                          uint8_t flags) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scene_client_delete.server_address=server_address;
    cmd->data.cmd_scene_client_delete.elem_index=elem_index;
    cmd->data.cmd_scene_client_delete.selected_scene=selected_scene;
    cmd->data.cmd_scene_client_delete.appkey_index=appkey_index;
    cmd->data.cmd_scene_client_delete.flags=flags;

    cmd->header=sl_btmesh_cmd_scene_client_delete_id+(((9)&0xff)<<8)+(((9)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_client_delete.result;

}

sl_status_t sl_btmesh_scene_server_init(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scene_server_init.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_scene_server_init_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_server_init.result;

}

sl_status_t sl_btmesh_scene_server_deinit(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scene_server_deinit.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_scene_server_deinit_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_server_deinit.result;

}

sl_status_t sl_btmesh_scene_server_reset_register(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scene_server_reset_register.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_scene_server_reset_register_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_server_reset_register.result;

}

sl_status_t sl_btmesh_scene_server_enable_compact_recall_events() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_scene_server_enable_compact_recall_events_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_server_enable_compact_recall_events.result;

}

sl_status_t sl_btmesh_scene_setup_server_init(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scene_setup_server_init.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_scene_setup_server_init_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scene_setup_server_init.result;

}

sl_status_t sl_btmesh_scheduler_client_init(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scheduler_client_init.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_scheduler_client_init_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scheduler_client_init.result;

}

sl_status_t sl_btmesh_scheduler_client_deinit(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scheduler_client_deinit.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_scheduler_client_deinit_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scheduler_client_deinit.result;

}

sl_status_t sl_btmesh_scheduler_client_get(uint16_t server_address,
                                           uint16_t elem_index,
                                           uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scheduler_client_get.server_address=server_address;
    cmd->data.cmd_scheduler_client_get.elem_index=elem_index;
    cmd->data.cmd_scheduler_client_get.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_scheduler_client_get_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scheduler_client_get.result;

}

sl_status_t sl_btmesh_scheduler_client_get_action(uint16_t server_address,
                                                  uint16_t elem_index,
                                                  uint16_t appkey_index,
                                                  uint8_t index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scheduler_client_get_action.server_address=server_address;
    cmd->data.cmd_scheduler_client_get_action.elem_index=elem_index;
    cmd->data.cmd_scheduler_client_get_action.appkey_index=appkey_index;
    cmd->data.cmd_scheduler_client_get_action.index=index;

    cmd->header=sl_btmesh_cmd_scheduler_client_get_action_id+(((7)&0xff)<<8)+(((7)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scheduler_client_get_action.result;

}

sl_status_t sl_btmesh_scheduler_client_set_action(uint16_t server_address,
                                                  uint16_t elem_index,
                                                  uint16_t appkey_index,
                                                  uint8_t flags,
                                                  uint8_t index,
                                                  uint8_t year,
                                                  uint16_t month,
                                                  uint8_t day,
                                                  uint8_t hour,
                                                  uint8_t minute,
                                                  uint8_t second,
                                                  uint8_t day_of_week,
                                                  uint8_t action,
                                                  uint32_t transition_time_ms,
                                                  uint16_t scene_number) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scheduler_client_set_action.server_address=server_address;
    cmd->data.cmd_scheduler_client_set_action.elem_index=elem_index;
    cmd->data.cmd_scheduler_client_set_action.appkey_index=appkey_index;
    cmd->data.cmd_scheduler_client_set_action.flags=flags;
    cmd->data.cmd_scheduler_client_set_action.index=index;
    cmd->data.cmd_scheduler_client_set_action.year=year;
    cmd->data.cmd_scheduler_client_set_action.month=month;
    cmd->data.cmd_scheduler_client_set_action.day=day;
    cmd->data.cmd_scheduler_client_set_action.hour=hour;
    cmd->data.cmd_scheduler_client_set_action.minute=minute;
    cmd->data.cmd_scheduler_client_set_action.second=second;
    cmd->data.cmd_scheduler_client_set_action.day_of_week=day_of_week;
    cmd->data.cmd_scheduler_client_set_action.action=action;
    cmd->data.cmd_scheduler_client_set_action.transition_time_ms=transition_time_ms;
    cmd->data.cmd_scheduler_client_set_action.scene_number=scene_number;

    cmd->header=sl_btmesh_cmd_scheduler_client_set_action_id+(((23)&0xff)<<8)+(((23)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scheduler_client_set_action.result;

}

sl_status_t sl_btmesh_scheduler_server_init(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scheduler_server_init.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_scheduler_server_init_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scheduler_server_init.result;

}

sl_status_t sl_btmesh_scheduler_server_deinit(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scheduler_server_deinit.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_scheduler_server_deinit_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scheduler_server_deinit.result;

}

sl_status_t sl_btmesh_scheduler_server_get(uint16_t elem_index,
                                           uint16_t *status) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scheduler_server_get.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_scheduler_server_get_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *status = rsp->data.rsp_scheduler_server_get.status;
    return rsp->data.rsp_scheduler_server_get.result;

}

sl_status_t sl_btmesh_scheduler_server_get_action(uint16_t elem_index,
                                                  uint8_t index,
                                                  uint8_t *index_,
                                                  uint8_t *year,
                                                  uint16_t *month,
                                                  uint8_t *day,
                                                  uint8_t *hour,
                                                  uint8_t *minute,
                                                  uint8_t *second,
                                                  uint8_t *day_of_week,
                                                  uint8_t *action,
                                                  uint32_t *transition_time_ms,
                                                  uint16_t *scene_number) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scheduler_server_get_action.elem_index=elem_index;
    cmd->data.cmd_scheduler_server_get_action.index=index;

    cmd->header=sl_btmesh_cmd_scheduler_server_get_action_id+(((3)&0xff)<<8)+(((3)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *index_ = rsp->data.rsp_scheduler_server_get_action.index_;
    *year = rsp->data.rsp_scheduler_server_get_action.year;
    *month = rsp->data.rsp_scheduler_server_get_action.month;
    *day = rsp->data.rsp_scheduler_server_get_action.day;
    *hour = rsp->data.rsp_scheduler_server_get_action.hour;
    *minute = rsp->data.rsp_scheduler_server_get_action.minute;
    *second = rsp->data.rsp_scheduler_server_get_action.second;
    *day_of_week = rsp->data.rsp_scheduler_server_get_action.day_of_week;
    *action = rsp->data.rsp_scheduler_server_get_action.action;
    *transition_time_ms = rsp->data.rsp_scheduler_server_get_action.transition_time_ms;
    *scene_number = rsp->data.rsp_scheduler_server_get_action.scene_number;
    return rsp->data.rsp_scheduler_server_get_action.result;

}

sl_status_t sl_btmesh_scheduler_server_set_action(uint16_t elem_index,
                                                  uint8_t index,
                                                  uint8_t year,
                                                  uint16_t month,
                                                  uint8_t day,
                                                  uint8_t hour,
                                                  uint8_t minute,
                                                  uint8_t second,
                                                  uint8_t day_of_week,
                                                  uint8_t action,
                                                  uint32_t transition_time_ms,
                                                  uint16_t scene_number) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_scheduler_server_set_action.elem_index=elem_index;
    cmd->data.cmd_scheduler_server_set_action.index=index;
    cmd->data.cmd_scheduler_server_set_action.year=year;
    cmd->data.cmd_scheduler_server_set_action.month=month;
    cmd->data.cmd_scheduler_server_set_action.day=day;
    cmd->data.cmd_scheduler_server_set_action.hour=hour;
    cmd->data.cmd_scheduler_server_set_action.minute=minute;
    cmd->data.cmd_scheduler_server_set_action.second=second;
    cmd->data.cmd_scheduler_server_set_action.day_of_week=day_of_week;
    cmd->data.cmd_scheduler_server_set_action.action=action;
    cmd->data.cmd_scheduler_server_set_action.transition_time_ms=transition_time_ms;
    cmd->data.cmd_scheduler_server_set_action.scene_number=scene_number;

    cmd->header=sl_btmesh_cmd_scheduler_server_set_action_id+(((18)&0xff)<<8)+(((18)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_scheduler_server_set_action.result;

}

sl_status_t sl_btmesh_time_server_init(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_init.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_time_server_init_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_server_init.result;

}

sl_status_t sl_btmesh_time_server_deinit(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_deinit.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_time_server_deinit_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_server_deinit.result;

}

sl_status_t sl_btmesh_time_server_get_time(uint16_t elem_index,
                                           uint64_t *tai_seconds,
                                           uint8_t *subsecond,
                                           uint8_t *uncertainty,
                                           uint8_t *time_authority,
                                           int16_t *time_zone_offset,
                                           int32_t *tai_utc_delta) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_get_time.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_time_server_get_time_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *tai_seconds = rsp->data.rsp_time_server_get_time.tai_seconds;
    *subsecond = rsp->data.rsp_time_server_get_time.subsecond;
    *uncertainty = rsp->data.rsp_time_server_get_time.uncertainty;
    *time_authority = rsp->data.rsp_time_server_get_time.time_authority;
    *time_zone_offset = rsp->data.rsp_time_server_get_time.time_zone_offset;
    *tai_utc_delta = rsp->data.rsp_time_server_get_time.tai_utc_delta;
    return rsp->data.rsp_time_server_get_time.result;

}

sl_status_t sl_btmesh_time_server_set_time(uint16_t elem_index,
                                           uint64_t tai_seconds,
                                           uint8_t subsecond,
                                           uint8_t uncertainty,
                                           uint8_t time_authority,
                                           int16_t time_zone_offset,
                                           int32_t tai_utc_delta) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_set_time.elem_index=elem_index;
    cmd->data.cmd_time_server_set_time.tai_seconds=tai_seconds;
    cmd->data.cmd_time_server_set_time.subsecond=subsecond;
    cmd->data.cmd_time_server_set_time.uncertainty=uncertainty;
    cmd->data.cmd_time_server_set_time.time_authority=time_authority;
    cmd->data.cmd_time_server_set_time.time_zone_offset=time_zone_offset;
    cmd->data.cmd_time_server_set_time.tai_utc_delta=tai_utc_delta;

    cmd->header=sl_btmesh_cmd_time_server_set_time_id+(((19)&0xff)<<8)+(((19)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_server_set_time.result;

}

sl_status_t sl_btmesh_time_server_get_time_zone_offset_new(uint16_t elem_index,
                                                           int16_t *new_offset,
                                                           uint64_t *tai_of_zone_change) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_get_time_zone_offset_new.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_time_server_get_time_zone_offset_new_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *new_offset = rsp->data.rsp_time_server_get_time_zone_offset_new.new_offset;
    *tai_of_zone_change = rsp->data.rsp_time_server_get_time_zone_offset_new.tai_of_zone_change;
    return rsp->data.rsp_time_server_get_time_zone_offset_new.result;

}

sl_status_t sl_btmesh_time_server_set_time_zone_offset_new(uint16_t elem_index,
                                                           int16_t new_offset,
                                                           uint64_t tai_of_zone_change) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_set_time_zone_offset_new.elem_index=elem_index;
    cmd->data.cmd_time_server_set_time_zone_offset_new.new_offset=new_offset;
    cmd->data.cmd_time_server_set_time_zone_offset_new.tai_of_zone_change=tai_of_zone_change;

    cmd->header=sl_btmesh_cmd_time_server_set_time_zone_offset_new_id+(((12)&0xff)<<8)+(((12)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_server_set_time_zone_offset_new.result;

}

sl_status_t sl_btmesh_time_server_get_tai_utc_delta_new(uint16_t elem_index,
                                                        int32_t *new_delta,
                                                        uint64_t *tai_of_delta_change) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_get_tai_utc_delta_new.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_time_server_get_tai_utc_delta_new_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *new_delta = rsp->data.rsp_time_server_get_tai_utc_delta_new.new_delta;
    *tai_of_delta_change = rsp->data.rsp_time_server_get_tai_utc_delta_new.tai_of_delta_change;
    return rsp->data.rsp_time_server_get_tai_utc_delta_new.result;

}

sl_status_t sl_btmesh_time_server_set_tai_utc_delta_new(uint16_t elem_index,
                                                        int32_t new_delta,
                                                        uint64_t tai_of_delta_change) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_set_tai_utc_delta_new.elem_index=elem_index;
    cmd->data.cmd_time_server_set_tai_utc_delta_new.new_delta=new_delta;
    cmd->data.cmd_time_server_set_tai_utc_delta_new.tai_of_delta_change=tai_of_delta_change;

    cmd->header=sl_btmesh_cmd_time_server_set_tai_utc_delta_new_id+(((14)&0xff)<<8)+(((14)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_server_set_tai_utc_delta_new.result;

}

sl_status_t sl_btmesh_time_server_get_time_role(uint16_t elem_index,
                                                uint8_t *time_role) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_get_time_role.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_time_server_get_time_role_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *time_role = rsp->data.rsp_time_server_get_time_role.time_role;
    return rsp->data.rsp_time_server_get_time_role.result;

}

sl_status_t sl_btmesh_time_server_set_time_role(uint16_t elem_index,
                                                uint8_t time_role) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_set_time_role.elem_index=elem_index;
    cmd->data.cmd_time_server_set_time_role.time_role=time_role;

    cmd->header=sl_btmesh_cmd_time_server_set_time_role_id+(((3)&0xff)<<8)+(((3)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_server_set_time_role.result;

}

sl_status_t sl_btmesh_time_server_get_datetime(uint16_t elem_index,
                                               uint16_t *year,
                                               uint8_t *month,
                                               uint8_t *day,
                                               uint8_t *hour,
                                               uint8_t *min,
                                               uint8_t *sec,
                                               uint16_t *ms,
                                               int16_t *timezone,
                                               uint8_t *day_of_week) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_get_datetime.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_time_server_get_datetime_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    *year = rsp->data.rsp_time_server_get_datetime.year;
    *month = rsp->data.rsp_time_server_get_datetime.month;
    *day = rsp->data.rsp_time_server_get_datetime.day;
    *hour = rsp->data.rsp_time_server_get_datetime.hour;
    *min = rsp->data.rsp_time_server_get_datetime.min;
    *sec = rsp->data.rsp_time_server_get_datetime.sec;
    *ms = rsp->data.rsp_time_server_get_datetime.ms;
    *timezone = rsp->data.rsp_time_server_get_datetime.timezone;
    *day_of_week = rsp->data.rsp_time_server_get_datetime.day_of_week;
    return rsp->data.rsp_time_server_get_datetime.result;

}

sl_status_t sl_btmesh_time_server_publish(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_publish.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_time_server_publish_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_server_publish.result;

}

sl_status_t sl_btmesh_time_server_status(uint16_t destination_address,
                                         uint16_t elem_index,
                                         uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_server_status.destination_address=destination_address;
    cmd->data.cmd_time_server_status.elem_index=elem_index;
    cmd->data.cmd_time_server_status.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_time_server_status_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_server_status.result;

}

sl_status_t sl_btmesh_time_client_init(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_client_init.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_time_client_init_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_client_init.result;

}

sl_status_t sl_btmesh_time_client_deinit(uint16_t elem_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_client_deinit.elem_index=elem_index;

    cmd->header=sl_btmesh_cmd_time_client_deinit_id+(((2)&0xff)<<8)+(((2)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_client_deinit.result;

}

sl_status_t sl_btmesh_time_client_get_time(uint16_t server_address,
                                           uint16_t elem_index,
                                           uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_client_get_time.server_address=server_address;
    cmd->data.cmd_time_client_get_time.elem_index=elem_index;
    cmd->data.cmd_time_client_get_time.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_time_client_get_time_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_client_get_time.result;

}

sl_status_t sl_btmesh_time_client_set_time(uint16_t server_address,
                                           uint16_t elem_index,
                                           uint16_t appkey_index,
                                           uint64_t tai_seconds,
                                           uint8_t subsecond,
                                           uint8_t uncertainty,
                                           uint8_t time_authority,
                                           int32_t tai_utc_delta,
                                           int16_t time_zone_offset) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_client_set_time.server_address=server_address;
    cmd->data.cmd_time_client_set_time.elem_index=elem_index;
    cmd->data.cmd_time_client_set_time.appkey_index=appkey_index;
    cmd->data.cmd_time_client_set_time.tai_seconds=tai_seconds;
    cmd->data.cmd_time_client_set_time.subsecond=subsecond;
    cmd->data.cmd_time_client_set_time.uncertainty=uncertainty;
    cmd->data.cmd_time_client_set_time.time_authority=time_authority;
    cmd->data.cmd_time_client_set_time.tai_utc_delta=tai_utc_delta;
    cmd->data.cmd_time_client_set_time.time_zone_offset=time_zone_offset;

    cmd->header=sl_btmesh_cmd_time_client_set_time_id+(((23)&0xff)<<8)+(((23)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_client_set_time.result;

}

sl_status_t sl_btmesh_time_client_get_time_zone(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_client_get_time_zone.server_address=server_address;
    cmd->data.cmd_time_client_get_time_zone.elem_index=elem_index;
    cmd->data.cmd_time_client_get_time_zone.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_time_client_get_time_zone_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_client_get_time_zone.result;

}

sl_status_t sl_btmesh_time_client_set_time_zone(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                int16_t time_zone_offset_new,
                                                uint64_t tai_of_zone_change) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_client_set_time_zone.server_address=server_address;
    cmd->data.cmd_time_client_set_time_zone.elem_index=elem_index;
    cmd->data.cmd_time_client_set_time_zone.appkey_index=appkey_index;
    cmd->data.cmd_time_client_set_time_zone.time_zone_offset_new=time_zone_offset_new;
    cmd->data.cmd_time_client_set_time_zone.tai_of_zone_change=tai_of_zone_change;

    cmd->header=sl_btmesh_cmd_time_client_set_time_zone_id+(((16)&0xff)<<8)+(((16)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_client_set_time_zone.result;

}

sl_status_t sl_btmesh_time_client_get_tai_utc_delta(uint16_t server_address,
                                                    uint16_t elem_index,
                                                    uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_client_get_tai_utc_delta.server_address=server_address;
    cmd->data.cmd_time_client_get_tai_utc_delta.elem_index=elem_index;
    cmd->data.cmd_time_client_get_tai_utc_delta.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_time_client_get_tai_utc_delta_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_client_get_tai_utc_delta.result;

}

sl_status_t sl_btmesh_time_client_set_tai_utc_delta(uint16_t server_address,
                                                    uint16_t elem_index,
                                                    uint16_t appkey_index,
                                                    int32_t tai_utc_delta_new,
                                                    uint64_t tai_of_delta_change) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_client_set_tai_utc_delta.server_address=server_address;
    cmd->data.cmd_time_client_set_tai_utc_delta.elem_index=elem_index;
    cmd->data.cmd_time_client_set_tai_utc_delta.appkey_index=appkey_index;
    cmd->data.cmd_time_client_set_tai_utc_delta.tai_utc_delta_new=tai_utc_delta_new;
    cmd->data.cmd_time_client_set_tai_utc_delta.tai_of_delta_change=tai_of_delta_change;

    cmd->header=sl_btmesh_cmd_time_client_set_tai_utc_delta_id+(((18)&0xff)<<8)+(((18)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_client_set_tai_utc_delta.result;

}

sl_status_t sl_btmesh_time_client_get_time_role(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_client_get_time_role.server_address=server_address;
    cmd->data.cmd_time_client_get_time_role.elem_index=elem_index;
    cmd->data.cmd_time_client_get_time_role.appkey_index=appkey_index;

    cmd->header=sl_btmesh_cmd_time_client_get_time_role_id+(((6)&0xff)<<8)+(((6)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_client_get_time_role.result;

}

sl_status_t sl_btmesh_time_client_set_time_role(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t time_role) {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;

    cmd->data.cmd_time_client_set_time_role.server_address=server_address;
    cmd->data.cmd_time_client_set_time_role.elem_index=elem_index;
    cmd->data.cmd_time_client_set_time_role.appkey_index=appkey_index;
    cmd->data.cmd_time_client_set_time_role.time_role=time_role;

    cmd->header=sl_btmesh_cmd_time_client_set_time_role_id+(((7)&0xff)<<8)+(((7)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_time_client_set_time_role.result;

}

sl_status_t sl_btmesh_migration_migrate_keys() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_migration_migrate_keys_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_migration_migrate_keys.result;

}

sl_status_t sl_btmesh_migration_migrate_ddb() {
    struct sl_btmesh_packet *cmd = (struct sl_btmesh_packet *)sl_btmesh_cmd_msg;

    struct sl_btmesh_packet *rsp = (struct sl_btmesh_packet *)sl_btmesh_rsp_msg;


    cmd->header=sl_btmesh_cmd_migration_migrate_ddb_id+(((0)&0xff)<<8)+(((0)&0x700)>>8);


    sl_btmesh_host_handle_command();
    return rsp->data.rsp_migration_migrate_ddb.result;

}
