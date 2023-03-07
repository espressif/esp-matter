/** @file mlan_api.c
 *
 *  @brief This file provides more APIs for mlan.
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

#include <stdio.h>
#include <mlan_api.h>

/* Additional WMSDK header files */
#include <wmerrno.h>
#include <wm_os.h>

#include <wifi.h>

#include "wifi-sdio.h"
#include "wifi-internal.h"

/*
 * Bit 0 : Assoc Req
 * Bit 1 : Assoc Resp
 * Bit 2 : ReAssoc Req
 * Bit 3 : ReAssoc Resp
 * Bit 4 : Probe Req
 * Bit 5 : Probe Resp
 * Bit 8 : Beacon
 */
/** Mask for Assoc request frame */
#define MGMT_MASK_ASSOC_REQ 0x01
/** Mask for ReAssoc request frame */
#define MGMT_MASK_REASSOC_REQ 0x04
/** Mask for Assoc response frame */
#define MGMT_MASK_ASSOC_RESP 0x02
/** Mask for ReAssoc response frame */
#define MGMT_MASK_REASSOC_RESP 0x08
/** Mask for probe request frame */
#define MGMT_MASK_PROBE_REQ 0x10
/** Mask for probe response frame */
#define MGMT_MASK_PROBE_RESP 0x20
/** Mask for beacon frame */
#define MGMT_MASK_BEACON 0x100
/** Mask to clear previous settings */
#define MGMT_MASK_CLEAR 0x000

static const char driver_version_format[] = "SD878x-%s-%s-WM";
static const char driver_version[]        = "702.1.0";

static unsigned int mgmt_ie_index_bitmap = 0x00;
static int wifi_11d_country              = 0x00;

/* This were static functions in mlan file */
mlan_status wlan_cmd_802_11_deauthenticate(IN pmlan_private pmpriv, IN HostCmd_DS_COMMAND *cmd, IN t_void *pdata_buf);
mlan_status wlan_cmd_reg_access(IN HostCmd_DS_COMMAND *cmd, IN t_u16 cmd_action, IN t_void *pdata_buf);
mlan_status wlan_cmd_mem_access(IN HostCmd_DS_COMMAND *cmd, IN t_u16 cmd_action, IN t_void *pdata_buf);
mlan_status wlan_cmd_recfg_tx_buf(mlan_private *priv, HostCmd_DS_COMMAND *cmd, int cmd_action, void *pdata_buf);
mlan_status wlan_cmd_auto_reconnect(IN HostCmd_DS_COMMAND *cmd, IN t_u16 cmd_action, IN t_void *pdata_buf);
mlan_status wlan_cmd_rx_mgmt_indication(IN pmlan_private pmpriv,
                                        IN HostCmd_DS_COMMAND *cmd,
                                        IN t_u16 cmd_action,
                                        IN t_void *pdata_buf);
mlan_status wlan_misc_ioctl_region(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req);

int wifi_deauthenticate(uint8_t *bssid)
{
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    wifi_get_command_lock();

    /* fixme: check if this static selection is ok */
    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    wlan_cmd_802_11_deauthenticate((mlan_private *)mlan_adap->priv[0], cmd, bssid);
    wifi_wait_for_cmdresp(NULL);
    return WM_SUCCESS;
}

int wifi_get_eeprom_data(uint32_t offset, uint32_t byte_count, uint8_t *buf)
{
    mlan_ds_read_eeprom eeprom_rd;
    eeprom_rd.offset     = offset;
    eeprom_rd.byte_count = byte_count;

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->command = HostCmd_CMD_802_11_EEPROM_ACCESS;
    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    wlan_cmd_reg_access(cmd, HostCmd_ACT_GEN_GET, &eeprom_rd);
    wifi_wait_for_cmdresp(buf);
    return wm_wifi.cmd_resp_status;
}

int wifi_reg_access(wifi_reg_t reg_type, uint16_t action, uint32_t offset, uint32_t *value)
{
    mlan_ds_reg_rw reg_rw;
    reg_rw.offset = offset;
    reg_rw.value  = *value;
    uint16_t hostcmd;
    switch (reg_type)
    {
        case REG_MAC:
            hostcmd = HostCmd_CMD_MAC_REG_ACCESS;
            break;
        case REG_BBP:
            hostcmd = HostCmd_CMD_BBP_REG_ACCESS;
            break;
        case REG_RF:
            hostcmd = HostCmd_CMD_RF_REG_ACCESS;
            break;
        default:
            wifi_e("Incorrect register type");
            return -WM_FAIL;
            break;
    }

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->command = hostcmd;
    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    wlan_cmd_reg_access(cmd, action, &reg_rw);
    wifi_wait_for_cmdresp(action == HostCmd_ACT_GEN_GET ? value : NULL);
    return wm_wifi.cmd_resp_status;
}

int wifi_mem_access(uint16_t action, uint32_t addr, uint32_t *value)
{
    mlan_ds_mem_rw mem_rw;
    mem_rw.addr  = addr;
    mem_rw.value = *value;

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->command = HostCmd_CMD_MEM_ACCESS;
    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    wlan_cmd_mem_access(cmd, action, &mem_rw);

    wifi_wait_for_cmdresp(action == HostCmd_ACT_GEN_GET ? value : NULL);
    return wm_wifi.cmd_resp_status;
}

static int wifi_auto_reconnect(uint16_t action, wifi_auto_reconnect_config_t *auto_reconnect_config)
{
    mlan_ds_auto_reconnect auto_reconnect;

    memset(&auto_reconnect, 0x00, sizeof(mlan_ds_auto_reconnect));

    if (auto_reconnect_config)
    {
        auto_reconnect.reconnect_counter  = auto_reconnect_config->reconnect_counter;
        auto_reconnect.reconnect_interval = auto_reconnect_config->reconnect_interval;
        auto_reconnect.flags              = auto_reconnect_config->flags;
    }

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->command = HostCmd_CMD_AUTO_RECONNECT;
    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    wlan_cmd_auto_reconnect(cmd, action, &auto_reconnect);

    wifi_wait_for_cmdresp(action == HostCmd_ACT_GEN_GET ? auto_reconnect_config : NULL);

    return wm_wifi.cmd_resp_status;
}

int wifi_auto_reconnect_enable(wifi_auto_reconnect_config_t auto_reconnect_config)
{
    return wifi_auto_reconnect(HostCmd_ACT_GEN_SET, &auto_reconnect_config);
}

int wifi_auto_reconnect_disable()
{
    return wifi_auto_reconnect(HostCmd_ACT_GEN_SET, NULL);
}

int wifi_get_auto_reconnect_config(wifi_auto_reconnect_config_t *auto_reconnect_config)
{
    return wifi_auto_reconnect(HostCmd_ACT_GEN_GET, auto_reconnect_config);
}

int wifi_get_tsf(uint32_t *tsf_high, uint32_t *tsf_low)
{
    t_u64 tsf;

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    memset(cmd, 0, sizeof(HostCmd_DS_COMMAND));

    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_GET_TSF,
                                              HostCmd_ACT_GEN_GET, 0, NULL, NULL, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(&tsf);

    *tsf_high = tsf >> 32;
    *tsf_low  = (t_u32)tsf;

    return wm_wifi.cmd_resp_status;
}

int wifi_send_rssi_info_cmd(wifi_rssi_info_t *rssi_info)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_RSSI_INFO,
                                              HostCmd_ACT_GEN_GET, 0, NULL, NULL, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(rssi_info);
    return wm_wifi.cmd_resp_status;
}

int wifi_send_rf_channel_cmd(wifi_rf_channel_t *rf_channel)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    /*
      SET operation is not supported according to spec. So we are
      sending NULL as one param below.
    */
    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_802_11_RF_CHANNEL,
                                              HostCmd_ACT_GEN_GET, 0, NULL, NULL, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(rf_channel);
    return wm_wifi.cmd_resp_status;
}

int wifi_send_remain_on_channel_cmd(unsigned int bss_type, wifi_remain_on_channel_t *remain_on_channel)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(0 /* seq_num */, 0 /* bss_num */, bss_type);
    cmd->result  = 0x0;

    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_802_11_REMAIN_ON_CHANNEL,
                                              HostCmd_ACT_GEN_SET, 0, NULL, remain_on_channel, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(NULL);
    return wm_wifi.cmd_resp_status;
}

/* power_level is not used when cmd_action is GET */
int wifi_get_set_rf_tx_power(t_u16 cmd_action, wifi_tx_power_t *tx_power)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->seq_num   = 0x0;
    cmd->result    = 0x0;
    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_802_11_RF_TX_POWER,
                                              cmd_action, 0, NULL, &tx_power->current_level, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(cmd_action == HostCmd_ACT_GEN_GET ? tx_power : NULL);
    return wm_wifi.cmd_resp_status;
}

int wifi_get_data_rate(wifi_ds_rate *ds_rate)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_802_11_TX_RATE_QUERY, 0,
                                              0, NULL, NULL, cmd);

    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(ds_rate);
    return wm_wifi.cmd_resp_status;
}

int wifi_set_pmfcfg(t_u8 mfpc, t_u8 mfpr)
{
    mlan_private *pmpriv = (mlan_private *)mlan_adap->priv[0];

    pmpriv->pmfcfg.mfpc = mfpc;
    pmpriv->pmfcfg.mfpr = mfpr;

    return WM_SUCCESS;
}

int wifi_get_pmfcfg(t_u8 *mfpc, t_u8 *mfpr)
{
    mlan_private *pmpriv = (mlan_private *)mlan_adap->priv[0];

    *mfpc = pmpriv->pmfcfg.mfpc;
    *mfpr = pmpriv->pmfcfg.mfpr;

    return WM_SUCCESS;
}

int wifi_get_tbtt_offset(wifi_tbtt_offset_t *tbtt_offset)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    mlan_status rv =
        wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_TBTT_OFFSET, 0, 0, NULL, NULL, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(tbtt_offset);

    return wm_wifi.cmd_resp_status;
}

int wifi_set_packet_filters(wifi_flt_cfg_t *flt_cfg)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
    HostCmd_DS_MEF_CFG *mef_hdr;
    mef_entry_header *entry_hdr;
    t_u8 *buf = (t_u8 *)cmd, *filter_buf = NULL;
    t_u32 buf_len;
    int i;
    mef_op op;
    t_u32 dnum;

    memset(cmd, 0, WIFI_FW_CMDBUF_SIZE);

    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_MEF_CFG);
    buf_len      = S_DS_GEN;

    /** Fill HostCmd_DS_MEF_CFG*/
    mef_hdr           = (HostCmd_DS_MEF_CFG *)(buf + buf_len);
    mef_hdr->criteria = wlan_cpu_to_le32(flt_cfg->criteria);
    mef_hdr->nentries = wlan_cpu_to_le16(flt_cfg->nentries);
    buf_len += sizeof(HostCmd_DS_MEF_CFG);

    /** Fill entry header data*/
    entry_hdr         = (mef_entry_header *)(buf + buf_len);
    entry_hdr->mode   = flt_cfg->mef_entry.mode;
    entry_hdr->action = flt_cfg->mef_entry.action;
    buf_len += sizeof(mef_entry_header);

    for (i = 0; i < flt_cfg->mef_entry.filter_num; i++)
    {
        if (flt_cfg->mef_entry.filter_item[i].type == TYPE_DNUM_EQ)
        {
            /* Format of decimal num:
             * |   5 bytes  |    5 bytes    |    5 bytes    |        1 byte         |
             * |   pattern  |     offset    |  num of bytes |  type (TYPE_DNUM_EQ)  |
             */

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push pattern */
            op.operand_type = OPERAND_DNUM;
            dnum            = flt_cfg->mef_entry.filter_item[i].pattern;
            memcpy(filter_buf, &dnum, sizeof(dnum));
            memcpy(filter_buf + sizeof(dnum), &(op.operand_type), 1);
            buf_len += sizeof(dnum) + 1;

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push offset */
            op.operand_type = OPERAND_DNUM;
            dnum            = flt_cfg->mef_entry.filter_item[i].offset;
            memcpy(filter_buf, &dnum, sizeof(dnum));
            memcpy(filter_buf + sizeof(dnum), &(op.operand_type), 1);
            buf_len += sizeof(dnum) + 1;

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push num of bytes */
            op.operand_type = OPERAND_DNUM;
            dnum            = flt_cfg->mef_entry.filter_item[i].num_bytes;
            memcpy(filter_buf, &dnum, sizeof(dnum));
            memcpy(filter_buf + sizeof(dnum), &(op.operand_type), 1);
            buf_len += sizeof(dnum) + 1;

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push type */
            op.operand_type = TYPE_DNUM_EQ;
            memcpy(filter_buf, &(op.operand_type), 1);
            buf_len += 1;

            filter_buf = (t_u8 *)(buf + buf_len);
        }
        else if (flt_cfg->mef_entry.filter_item[i].type == TYPE_BYTE_EQ)
        {
            /* Format of byte seq:
             * |   5 bytes  |      val      |    5 bytes    |        1 byte         |
             * |   repeat   |   bytes seq   |    offset     |  type (TYPE_BYTE_EQ)  |
             */

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push repeat */
            op.operand_type = OPERAND_DNUM;
            dnum            = flt_cfg->mef_entry.filter_item[i].repeat;
            memcpy(filter_buf, &dnum, sizeof(dnum));
            memcpy(filter_buf + sizeof(dnum), &(op.operand_type), 1);
            buf_len += sizeof(dnum) + 1;

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push bytes seq */
            op.operand_type = OPERAND_BYTE_SEQ;
            memcpy(filter_buf, flt_cfg->mef_entry.filter_item[i].byte_seq,
                   flt_cfg->mef_entry.filter_item[i].num_byte_seq);
            memcpy(filter_buf + flt_cfg->mef_entry.filter_item[i].num_byte_seq,
                   &(flt_cfg->mef_entry.filter_item[i].num_byte_seq), 1);
            memcpy(filter_buf + flt_cfg->mef_entry.filter_item[i].num_byte_seq + 1, &(op.operand_type), 1);
            buf_len += flt_cfg->mef_entry.filter_item[i].num_byte_seq + 2;

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push offset */
            op.operand_type = OPERAND_DNUM;
            dnum            = flt_cfg->mef_entry.filter_item[i].offset;
            memcpy(filter_buf, &dnum, sizeof(dnum));
            memcpy(filter_buf + sizeof(dnum), &(op.operand_type), 1);
            buf_len += sizeof(dnum) + 1;

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push type */
            op.operand_type = TYPE_BYTE_EQ;
            memcpy(filter_buf, &(op.operand_type), 1);
            buf_len += 1;

            filter_buf = (t_u8 *)(buf + buf_len);
        }
        else if (flt_cfg->mef_entry.filter_item[i].type == TYPE_BIT_EQ)
        {
            /* Format of bit seq:
             * |   val      |    5 bytes    |      val      |        1 byte         |
             * | bytes seq  |    offset     |    mask seq   |  type (TYPE_BIT_EQ)   |
             */

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push bytes seq */
            op.operand_type = OPERAND_BYTE_SEQ;
            memcpy(filter_buf, flt_cfg->mef_entry.filter_item[i].byte_seq,
                   flt_cfg->mef_entry.filter_item[i].num_byte_seq);
            memcpy(filter_buf + flt_cfg->mef_entry.filter_item[i].num_byte_seq,
                   &(flt_cfg->mef_entry.filter_item[i].num_byte_seq), 1);
            memcpy(filter_buf + flt_cfg->mef_entry.filter_item[i].num_byte_seq + 1, &(op.operand_type), 1);
            buf_len += flt_cfg->mef_entry.filter_item[i].num_byte_seq + 2;

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push offset */
            op.operand_type = OPERAND_DNUM;
            dnum            = flt_cfg->mef_entry.filter_item[i].offset;
            memcpy(filter_buf, &dnum, sizeof(dnum));
            memcpy(filter_buf + sizeof(dnum), &(op.operand_type), 1);
            buf_len += sizeof(dnum) + 1;

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push mask seq */
            op.operand_type = OPERAND_BYTE_SEQ;
            memcpy(filter_buf, flt_cfg->mef_entry.filter_item[i].mask_seq,
                   flt_cfg->mef_entry.filter_item[i].num_mask_seq);
            memcpy(filter_buf + flt_cfg->mef_entry.filter_item[i].num_mask_seq,
                   &(flt_cfg->mef_entry.filter_item[i].num_mask_seq), 1);
            memcpy(filter_buf + flt_cfg->mef_entry.filter_item[i].num_mask_seq + 1, &(op.operand_type), 1);
            buf_len += flt_cfg->mef_entry.filter_item[i].num_mask_seq + 2;

            filter_buf = (t_u8 *)(buf + buf_len);

            /* push type */
            op.operand_type = TYPE_BIT_EQ;
            memcpy(filter_buf, &(op.operand_type), 1);
            buf_len += 1;

            filter_buf = (t_u8 *)(buf + buf_len);
        }
        else
            goto done;

        if (i != 0)
        {
            filter_buf = (t_u8 *)(buf + buf_len);

            op.operand_type = flt_cfg->mef_entry.rpn[i];
            memcpy(filter_buf, &(op.operand_type), 1);
            buf_len += 1;

            filter_buf = (t_u8 *)(buf + buf_len);
        }
    }

    entry_hdr->len = buf_len - sizeof(HostCmd_DS_MEF_CFG) - S_DS_GEN - sizeof(mef_entry_header);
    cmd->size      = wlan_cpu_to_le16(buf_len);
done:
    wifi_wait_for_cmdresp(NULL);

    return wm_wifi.cmd_resp_status;
}

#define FLTR_BUF_IP_OFFSET 24

int wifi_set_auto_arp(t_u32 *ipv4_addr)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
    HostCmd_DS_MEF_CFG *mef_hdr;
    t_u8 *buf = (t_u8 *)cmd, *filter = NULL;
    t_u32 buf_len;
    t_u8 fltr_buf[] = {0x01, 0x10, 0x21, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x06, 0x02, 0x02,
                       0x14, 0x00, 0x00, 0x00, 0x01, 0x41, 0x01, 0x00, 0x00, 0x00, 0x01, 0xc0, 0xa8,
                       0x01, 0x6d, 0x04, 0x02, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x41, 0x44};

    memset(cmd, 0, WIFI_FW_CMDBUF_SIZE);

    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_MEF_CFG);
    buf_len      = S_DS_GEN;

    /** Fill HostCmd_DS_MEF_CFG*/
    mef_hdr           = (HostCmd_DS_MEF_CFG *)(buf + buf_len);
    mef_hdr->criteria = wlan_cpu_to_le32(MBIT(0) | MBIT(1) | MBIT(3));
    mef_hdr->nentries = wlan_cpu_to_le16(1);
    buf_len += sizeof(HostCmd_DS_MEF_CFG);

    filter = buf + buf_len;
    memcpy(filter, fltr_buf, sizeof(fltr_buf));
    memcpy(&filter[FLTR_BUF_IP_OFFSET], ipv4_addr, sizeof(t_u32));
    buf_len += sizeof(fltr_buf);

    cmd->size = wlan_cpu_to_le16(buf_len);
    wifi_wait_for_cmdresp(NULL);

    return wm_wifi.cmd_resp_status;
}

int wifi_tcp_keep_alive(wifi_tcp_keep_alive_t *keep_alive, t_u8 *src_mac, t_u32 src_ip)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
    t_u16 cmd_action        = HostCmd_ACT_GEN_SET;

    HostCmd_DS_AUTO_TX *auto_tx_cmd = (HostCmd_DS_AUTO_TX *)((t_u8 *)cmd + S_DS_GEN);
    t_u8 *pos                       = (t_u8 *)auto_tx_cmd + sizeof(auto_tx_cmd->action);
    t_u16 len                       = 0;

    MrvlIEtypes_Cloud_Keep_Alive_t *keep_alive_tlv = MNULL;
    MrvlIEtypes_Keep_Alive_Ctrl_t *ctrl_tlv        = MNULL;
    MrvlIEtypes_Keep_Alive_Pkt_t *pkt_tlv          = MNULL;
    t_u8 eth_ip[]                                  = {0x08, 0x00};
    t_u8 ip_packet[67] = {0x45, 0x00, 0x00, 0x43, 0x8c, 0x9e, 0x00, 0x00, 0xff, 0x06, 0xac, 0xbf, 0xc0, 0xa8,
                          0x00, 0x7c, 0xc0, 0xa8, 0x00, 0x8a, 0xc0, 0x03, 0x22, 0xb7, 0xb0, 0xb6, 0x60, 0x9f,
                          0x42, 0xdd, 0x9e, 0x1e, 0x50, 0x18, 0x80, 0x00, 0xd0, 0x88, 0x00, 0x00, 0x74, 0x68,
                          0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x6b, 0x65, 0x65, 0x70, 0x20, 0x61,
                          0x6c, 0x69, 0x76, 0x65, 0x20, 0x70, 0x61, 0x63, 0x6b, 0x65, 0x74};
#if 0
	t_u8 ip_packet2[41] = {0x45, 0x00, 0x00, 0x29, 0x76, 0x51, 0x40, 0x00, 0x80, 0x06, 0xf2, 0x4c, 0xc0, 0xa8, 0x01, 0x0a, 0xc0, 0xa8, 0x01, 0x0c, 0xfb, 0xd8, 0x01, 0xbd, 0x76, 0xe3, 0x34, 0x62, 0x06, 0x80, 0x8b, 0x62, 0x50, 0x10, 0x01, 0x00, 0xe1, 0xe4, 0x00, 0x00, 0x00};
#endif
    t_u16 pkt_len = 67;

    if (keep_alive->reset)
        cmd_action = HostCmd_ACT_GEN_RESET;

    cmd->command        = wlan_cpu_to_le16(HostCmd_CMD_AUTO_TX);
    cmd->size           = S_DS_GEN + sizeof(HostCmd_DS_AUTO_TX);
    auto_tx_cmd->action = wlan_cpu_to_le16(cmd_action);

    keep_alive_tlv = (MrvlIEtypes_Cloud_Keep_Alive_t *)pos;

    keep_alive_tlv->header.type   = wlan_cpu_to_le16(TLV_TYPE_CLOUD_KEEP_ALIVE);
    keep_alive_tlv->keep_alive_id = 1; // keep_alive->mkeep_alive_id;
    keep_alive_tlv->enable        = keep_alive->enable;
    len                           = len + sizeof(keep_alive_tlv->keep_alive_id) + sizeof(keep_alive_tlv->enable);
    pos                           = pos + len + sizeof(MrvlIEtypesHeader_t);
    if (cmd_action == HostCmd_ACT_GEN_SET)
    {
        if (keep_alive->enable)
        {
            ctrl_tlv              = (MrvlIEtypes_Keep_Alive_Ctrl_t *)pos;
            ctrl_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_KEEP_ALIVE_CTRL);
            ctrl_tlv->header.len =
                wlan_cpu_to_le16(sizeof(MrvlIEtypes_Keep_Alive_Ctrl_t) - sizeof(MrvlIEtypesHeader_t));
            ctrl_tlv->snd_interval   = wlan_cpu_to_le32(keep_alive->timeout);
            ctrl_tlv->retry_interval = wlan_cpu_to_le16(keep_alive->interval);
            ctrl_tlv->retry_count    = wlan_cpu_to_le16(keep_alive->max_keep_alives);
            len                      = len + sizeof(MrvlIEtypes_Keep_Alive_Ctrl_t);

            pos                  = pos + sizeof(MrvlIEtypes_Keep_Alive_Ctrl_t);
            pkt_tlv              = (MrvlIEtypes_Keep_Alive_Pkt_t *)pos;
            pkt_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_KEEP_ALIVE_PKT);
            memcpy(pkt_tlv->eth_header.dest_addr, keep_alive->dst_mac, MLAN_MAC_ADDR_LENGTH);
            memcpy(pkt_tlv->eth_header.src_addr, src_mac, MLAN_MAC_ADDR_LENGTH);
            memcpy((t_u8 *)&pkt_tlv->eth_header.h803_len, eth_ip, sizeof(t_u16));
            memcpy(ip_packet + 12, &src_ip, sizeof(t_u32));
            memcpy(ip_packet + 16, &keep_alive->dst_ip, sizeof(t_u32));
            memcpy(pkt_tlv->ip_packet, ip_packet, pkt_len);
            pkt_tlv->header.len = wlan_cpu_to_le16(sizeof(Eth803Hdr_t) + pkt_len);
            len                 = len + sizeof(MrvlIEtypesHeader_t) + sizeof(Eth803Hdr_t) + pkt_len;
        }
        else
        {
            pkt_tlv              = (MrvlIEtypes_Keep_Alive_Pkt_t *)pos;
            pkt_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_KEEP_ALIVE_PKT);
            pkt_tlv->header.len  = 0;
            len                  = len + sizeof(MrvlIEtypesHeader_t);
        }
    }
    if (cmd_action == HostCmd_ACT_GEN_RESET)
    {
        pkt_tlv              = (MrvlIEtypes_Keep_Alive_Pkt_t *)pos;
        pkt_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_KEEP_ALIVE_PKT);
        pkt_tlv->header.len  = 0;
        len                  = len + sizeof(MrvlIEtypesHeader_t);
    }
    keep_alive_tlv->header.len = wlan_cpu_to_le16(len);

    cmd->size = cmd->size + len + sizeof(MrvlIEtypesHeader_t);
    cmd->size = wlan_cpu_to_le16(cmd->size);

    cmd->seq_num = 0x00;
    cmd->result  = 0x00;

    wifi_wait_for_cmdresp(NULL);

    return wm_wifi.cmd_resp_status;
}

int wifi_nat_keep_alive(wifi_nat_keep_alive_t *keep_alive, t_u8 *src_mac, t_u32 src_ip, t_u16 src_port)
{
    t_u16 d_port, s_port;

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
    t_u8 payload[40]        = {0x00, 0x26, 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x45, 0x00, 0x00, 0x1d,
                        0xbe, 0xef, 0x00, 0x00, 0x80, 0x11, 0xf9, 0xc5, 0xc0, 0xa8, 0x01, 0x03, 0xff, 0xff,
                        0xff, 0xff, 0x11, 0x94, 0x11, 0x94, 0x00, 0x09, 0x5b, 0x98, 0xff, 0x00};
    t_u8 payload_len        = 40;

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_AUTO_TX);

    HostCmd_DS_802_11_AUTO_TX *auto_tx_cmd = (HostCmd_DS_802_11_AUTO_TX *)((t_u8 *)cmd + S_DS_GEN);

    auto_tx_cmd->action = HostCmd_ACT_GEN_SET;

    MrvlIEtypesHeader_t *header = &auto_tx_cmd->auto_tx.header;

    header->type = wlan_cpu_to_le16(TLV_TYPE_AUTO_TX);
    header->len  = wlan_cpu_to_le16(0x3a);

    AutoTx_MacFrame_t *atmf = &auto_tx_cmd->auto_tx.auto_tx_mac_frame;

    memset(atmf, 0, sizeof(AutoTx_MacFrame_t));

    atmf->interval  = wlan_cpu_to_le16(keep_alive->interval);
    atmf->priority  = 0x07;
    atmf->frame_len = wlan_cpu_to_le16(0x34);

    memcpy(atmf->dest_mac_addr, keep_alive->dst_mac, MLAN_MAC_ADDR_LENGTH);
    memcpy(atmf->src_mac_addr, src_mac, MLAN_MAC_ADDR_LENGTH);

    memcpy(atmf->payload, payload, payload_len);

    memcpy(atmf->payload + 22, &src_ip, sizeof(t_u32));
    memcpy(atmf->payload + 26, &keep_alive->dst_ip, sizeof(t_u32));

    s_port = mlan_htons(src_port);
    memcpy(atmf->payload + 30, &s_port, sizeof(t_u16));

    d_port = mlan_htons(keep_alive->dst_port);
    memcpy(atmf->payload + 32, &d_port, sizeof(t_u16));

    cmd->size = 0x48;
    cmd->size = wlan_cpu_to_le16(cmd->size);

    cmd->seq_num = 0x00;
    cmd->result  = 0x00;

    wifi_wait_for_cmdresp(NULL);

    return wm_wifi.cmd_resp_status;
}

#ifdef CONFIG_RF_TEST_MODE
static uint8_t channel_set    = 0;
static uint8_t band_set       = 0;
static uint8_t bandwidth_set  = 0;
static uint8_t tx_antenna_set = 0;
static uint8_t rx_antenna_set = 0;

int wifi_get_set_rf_test_generic(t_u16 cmd_action, wifi_mfg_cmd_generic_cfg_t *wifi_mfg_cmd_generic_cfg)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->seq_num   = 0x0;
    cmd->result    = 0x0;
    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_MFG_COMMAND, cmd_action,
                                              0, NULL, wifi_mfg_cmd_generic_cfg, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(cmd_action == HostCmd_ACT_GEN_GET ? wifi_mfg_cmd_generic_cfg : NULL);
    return wm_wifi.cmd_resp_status;
}

int wifi_get_set_rf_test_tx_frame(t_u16 cmd_action, wifi_mfg_cmd_tx_frame_t *wifi_mfg_cmd_tx_frame)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->seq_num   = 0x0;
    cmd->result    = 0x0;
    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_MFG_COMMAND, cmd_action,
                                              0, NULL, wifi_mfg_cmd_tx_frame, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(cmd_action == HostCmd_ACT_GEN_GET ? wifi_mfg_cmd_tx_frame : NULL);
    return wm_wifi.cmd_resp_status;
}

int wifi_get_set_rf_test_tx_cont(t_u16 cmd_action, wifi_mfg_cmd_tx_cont_t *wifi_mfg_cmd_tx_cont)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->seq_num   = 0x0;
    cmd->result    = 0x0;
    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_MFG_COMMAND, cmd_action,
                                              0, NULL, wifi_mfg_cmd_tx_cont, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(cmd_action == HostCmd_ACT_GEN_GET ? wifi_mfg_cmd_tx_cont : NULL);
    return wm_wifi.cmd_resp_status;
}

int wifi_set_rf_test_mode()
{
    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_SET_TEST_MODE;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_SET;

    return wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_SET, &wifi_mfg_cmd_generic_cfg);
}

int wifi_set_rf_channel(const uint8_t channel)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    /* Check if Channel is allowed as per WWSM */
    if (!wlan_is_channel_valid(channel))
        return -WM_FAIL;

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_RF_CHAN;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_SET;

    wifi_mfg_cmd_generic_cfg.data1 = channel;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_SET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
        channel_set = 1;

    return ret;
}

int wifi_get_rf_channel(uint8_t *channel)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    if (!channel_set)
    {
        wifi_e("RF Channel not set");
        return -WM_FAIL;
    }

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_RF_CHAN;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_GET;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_GET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
        *channel = wifi_mfg_cmd_generic_cfg.data1;

    return ret;
}

int wifi_set_rf_band(const uint8_t band)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    if (band != 0
#ifdef CONFIG_5GHz_SUPPORT
        && band != 1
#endif
    )
        return -WM_FAIL;

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_RF_BAND_AG;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_SET;

    wifi_mfg_cmd_generic_cfg.data1 = band;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_SET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
        band_set = 1;

    return ret;
}

int wifi_get_rf_band(uint8_t *band)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    if (!band_set)
    {
        wifi_e("RF Band not set");
        return -WM_FAIL;
    }

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_RF_BAND_AG;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_GET;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_GET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
        *band = wifi_mfg_cmd_generic_cfg.data1;

    return ret;
}

int wifi_set_rf_bandwidth(const uint8_t bandwidth)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    if ((bandwidth != 0)
#ifdef CONFIG_5GHz_SUPPORT
        && (bandwidth != 1)
#endif
    )
        return -WM_FAIL;

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_RF_CHANNELBW;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_SET;

    wifi_mfg_cmd_generic_cfg.data1 = bandwidth;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_SET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
        bandwidth_set = 1;

    return ret;
}

int wifi_get_rf_bandwidth(uint8_t *bandwidth)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    if (!bandwidth_set)
    {
        wifi_e("Bandwidth not set");
        return -WM_FAIL;
    }

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_RF_CHANNELBW;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_GET;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_GET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
        *bandwidth = wifi_mfg_cmd_generic_cfg.data1;

    return ret;
}

int wifi_get_rf_per(uint32_t *rx_tot_pkt_count, uint32_t *rx_mcast_bcast_count, uint32_t *rx_pkt_fcs_error)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_CLR_RX_ERR;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_GET;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_GET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
    {
        *rx_tot_pkt_count     = wifi_mfg_cmd_generic_cfg.data1;
        *rx_mcast_bcast_count = wifi_mfg_cmd_generic_cfg.data2;
        *rx_pkt_fcs_error     = wifi_mfg_cmd_generic_cfg.data3;
    }

    return ret;
}

int wifi_set_rf_tx_cont_mode(const uint32_t enable_tx,
                             const uint32_t cw_mode,
                             const uint32_t payload_pattern,
                             const uint32_t cs_mode,
                             const uint32_t act_sub_ch,
                             const uint32_t tx_rate)
{
    wifi_mfg_cmd_tx_cont_t wifi_mfg_cmd_tx_cont;

    if ((enable_tx > 1) || (cw_mode > 1) || (cs_mode > 1) || (act_sub_ch == 2 || act_sub_ch > 3))
        return -WM_FAIL;

    memset(&wifi_mfg_cmd_tx_cont, 0x00, sizeof(wifi_mfg_cmd_tx_cont_t));

    wifi_mfg_cmd_tx_cont.mfg_cmd = MFG_CMD_TX_CONT;
    wifi_mfg_cmd_tx_cont.action  = HostCmd_ACT_GEN_SET;

    wifi_mfg_cmd_tx_cont.enable_tx       = enable_tx;
    wifi_mfg_cmd_tx_cont.cw_mode         = cw_mode;
    wifi_mfg_cmd_tx_cont.payload_pattern = payload_pattern;
    wifi_mfg_cmd_tx_cont.cs_mode         = cs_mode;
    wifi_mfg_cmd_tx_cont.act_sub_ch      = act_sub_ch;
    wifi_mfg_cmd_tx_cont.tx_rate         = tx_rate;

    return wifi_get_set_rf_test_tx_cont(HostCmd_ACT_GEN_SET, &wifi_mfg_cmd_tx_cont);
}

int wifi_set_rf_tx_antenna(const uint8_t antenna)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    if (antenna != 1 && antenna != 2)
        return -WM_FAIL;

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_TX_ANT;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_SET;

    wifi_mfg_cmd_generic_cfg.data1 = antenna;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_SET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
        tx_antenna_set = 1;

    return ret;
}

int wifi_get_rf_tx_antenna(uint8_t *antenna)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    if (!tx_antenna_set)
    {
        wifi_e("Tx Antenna not set");
        return -WM_FAIL;
    }

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_TX_ANT;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_GET;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_GET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
        *antenna = wifi_mfg_cmd_generic_cfg.data1;

    return ret;
}

int wifi_set_rf_rx_antenna(const uint8_t antenna)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    if (antenna != 1 && antenna != 2)
        return -WM_FAIL;

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_RX_ANT;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_SET;

    wifi_mfg_cmd_generic_cfg.data1 = antenna;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_SET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
        rx_antenna_set = 1;

    return ret;
}

int wifi_get_rf_rx_antenna(uint8_t *antenna)
{
    int ret;

    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    if (!rx_antenna_set)
    {
        wifi_e("Rx antenna not set");
        return -WM_FAIL;
    }

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_RX_ANT;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_GET;

    ret = wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_GET, &wifi_mfg_cmd_generic_cfg);

    if (ret == WM_SUCCESS)
        *antenna = wifi_mfg_cmd_generic_cfg.data1;

    return ret;
}

int wifi_set_rf_tx_power(const uint8_t power, const uint8_t mod, const uint8_t path_id)
{
    wifi_mfg_cmd_generic_cfg_t wifi_mfg_cmd_generic_cfg;

    if (power > 24)
        return -WM_FAIL;

    if (mod != 0 && mod != 1 && mod != 2)
        return -WM_FAIL;

    if (path_id != 0 && path_id != 1 && path_id != 2)
        return -WM_FAIL;

    memset(&wifi_mfg_cmd_generic_cfg, 0x00, sizeof(wifi_mfg_cmd_generic_cfg_t));

    wifi_mfg_cmd_generic_cfg.mfg_cmd = MFG_CMD_RFPWR;
    wifi_mfg_cmd_generic_cfg.action  = HostCmd_ACT_GEN_SET;

    wifi_mfg_cmd_generic_cfg.data1 = power;
    wifi_mfg_cmd_generic_cfg.data2 = mod;
    wifi_mfg_cmd_generic_cfg.data3 = path_id;

    return wifi_get_set_rf_test_generic(HostCmd_ACT_GEN_SET, &wifi_mfg_cmd_generic_cfg);
}

int wifi_set_rf_tx_frame(const uint32_t enable,
                         const uint32_t data_rate,
                         const uint32_t frame_pattern,
                         const uint32_t frame_length,
                         const uint32_t adjust_burst_sifs,
                         const uint32_t burst_sifs_in_us,
                         const uint32_t short_preamble,
                         const uint32_t act_sub_ch,
                         const uint32_t short_gi,
                         const uint32_t adv_coding,
                         const uint32_t tx_bf,
                         const uint32_t gf_mode,
                         const uint32_t stbc,
                         const uint32_t *bssid)
{
    wifi_mfg_cmd_tx_frame_t wifi_mfg_cmd_tx_frame;

    if (enable > 1 || frame_length < 1 || frame_length > 0x400 || burst_sifs_in_us > 255 || short_preamble > 1 ||
        act_sub_ch == 2 || act_sub_ch > 3 || short_gi > 1 || adv_coding > 1 || tx_bf > 1 || gf_mode > 1 || stbc > 1)
        return -WM_FAIL;

    memset(&wifi_mfg_cmd_tx_frame, 0x00, sizeof(wifi_mfg_cmd_tx_frame_t));

    wifi_mfg_cmd_tx_frame.mfg_cmd = MFG_CMD_TX_FRAME;
    wifi_mfg_cmd_tx_frame.action  = HostCmd_ACT_GEN_SET;

    wifi_mfg_cmd_tx_frame.enable        = enable;
    wifi_mfg_cmd_tx_frame.data_rate     = data_rate;
    wifi_mfg_cmd_tx_frame.frame_pattern = frame_pattern;
    wifi_mfg_cmd_tx_frame.frame_length  = frame_length;
    memcpy(wifi_mfg_cmd_tx_frame.bssid, bssid, MLAN_MAC_ADDR_LENGTH);
    wifi_mfg_cmd_tx_frame.adjust_burst_sifs = adjust_burst_sifs;
    wifi_mfg_cmd_tx_frame.burst_sifs_in_us  = burst_sifs_in_us;
    wifi_mfg_cmd_tx_frame.short_preamble    = short_preamble;
    wifi_mfg_cmd_tx_frame.act_sub_ch        = act_sub_ch;
    wifi_mfg_cmd_tx_frame.short_gi          = short_gi;
    wifi_mfg_cmd_tx_frame.adv_coding        = adv_coding;
    wifi_mfg_cmd_tx_frame.tx_bf             = tx_bf;
    wifi_mfg_cmd_tx_frame.gf_mode           = gf_mode;
    wifi_mfg_cmd_tx_frame.stbc              = stbc;

    return wifi_get_set_rf_test_tx_frame(HostCmd_ACT_GEN_SET, &wifi_mfg_cmd_tx_frame);
}
#endif

/*
 * fixme: Currently, we support only single SSID based scan. We can extend
 * this to a list of multiple SSIDs. The mlan API supports this.
 */
int wifi_send_scan_cmd(t_u8 bss_mode,
                       const t_u8 *specific_bssid,
                       const char *ssid,
                       const char *ssid2,
                       const t_u8 num_channels,
                       const wifi_scan_channel_list_t *chan_list,
                       const t_u8 num_probes,
                       const bool keep_previous_scan,
                       const bool active_scan_triggered)
{
    int ssid_len  = 0, i;
    int ssid2_len = 0;

    mlan_adap->active_scan_triggered = MFALSE;

    if (ssid)
    {
        ssid_len = strlen(ssid);
        if (ssid_len > MLAN_MAX_SSID_LENGTH)
            return -WM_E_INVAL;
    }

    if (ssid2)
    {
        ssid2_len = strlen(ssid2);
        if (ssid2_len > MLAN_MAX_SSID_LENGTH)
            return -WM_E_INVAL;
    }

    wlan_user_scan_cfg *user_scan_cfg = os_mem_alloc(sizeof(wlan_user_scan_cfg));
    if (!user_scan_cfg)
        return -WM_E_NOMEM;

    memset(user_scan_cfg, 0x00, sizeof(wlan_user_scan_cfg));

    user_scan_cfg->bss_mode           = bss_mode;
    user_scan_cfg->keep_previous_scan = keep_previous_scan;

    if (num_probes > 0 && num_probes <= MAX_PROBES)
        user_scan_cfg->num_probes = num_probes;

    if (specific_bssid)
    {
        memcpy(user_scan_cfg->specific_bssid, specific_bssid, MLAN_MAC_ADDR_LENGTH);
    }

    if (ssid)
    {
        memcpy(user_scan_cfg->ssid_list[0].ssid, ssid, ssid_len);
        /* For Wildcard SSID do not set max len */
        /* user_scan_cfg->ssid_list[0].max_len = ssid_len; */
    }

    if (ssid2)
    {
        memcpy(user_scan_cfg->ssid_list[1].ssid, ssid2, ssid2_len);
    }

    if (num_channels > 0 && num_channels <= WLAN_USER_SCAN_CHAN_MAX && chan_list)
    {
        for (i = 0; i < num_channels; i++)
        {
            /** Channel Number to scan */
            user_scan_cfg->chan_list[i].chan_number = chan_list[i].chan_number;
            /** Radio type: 'B/G' Band = 0, 'A' Band = 1 */
            /* fixme: B/G is hardcoded here. Ask the caller first to
               send the radio type and then change here */
            if (chan_list[i].chan_number > 14)
                user_scan_cfg->chan_list[i].radio_type = 1;
            /** Scan type: Active = 1, Passive = 2 */
            /* fixme: Active is hardcoded here. Ask the caller first to
               send the  type and then change here */
            user_scan_cfg->chan_list[i].scan_type = chan_list[i].scan_type;
            /** Scan duration in milliseconds; if 0 default used */
            user_scan_cfg->chan_list[i].scan_time = chan_list[i].scan_time;
        }
    }

    if (active_scan_triggered)
        mlan_adap->active_scan_triggered = MTRUE;

    mlan_status rv = wlan_scan_networks((mlan_private *)mlan_adap->priv[0], NULL, user_scan_cfg);
    if (rv != MLAN_STATUS_SUCCESS)
    {
        wifi_e("Scan command failed");
        os_mem_free(user_scan_cfg);
        return -WM_FAIL;
    }

    /* fixme: Can we free this immediately after wlan_scan_networks
       call returns */
    os_mem_free(user_scan_cfg);
    return WM_SUCCESS;
}

static int wifi_send_key_material_cmd(int bss_index, mlan_ds_sec_cfg *sec)
{
    /* fixme: check if this needs to go on heap */
    mlan_ioctl_req req;
    mlan_status rv = MLAN_STATUS_SUCCESS;

    memset(&req, 0x00, sizeof(mlan_ioctl_req));
    req.pbuf      = (t_u8 *)sec;
    req.buf_len   = sizeof(mlan_ds_sec_cfg);
    req.bss_index = bss_index;
    req.req_id    = MLAN_IOCTL_SEC_CFG;
    req.action    = MLAN_ACT_SET;

    if (bss_index)
        rv = wlan_ops_uap_ioctl(mlan_adap, &req);
    else
        rv = wlan_ops_sta_ioctl(mlan_adap, &req);

    if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING)
    {
        return -WM_FAIL;
    }

    return WM_SUCCESS;
}

uint8_t broadcast_mac_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

int wifi_set_key(int bss_index,
                 bool is_pairwise,
                 const uint8_t key_index,
                 const uint8_t *key,
                 unsigned key_len,
                 const uint8_t *mac_addr)
{
    /* fixme: check if this needs to go on heap */
    mlan_ds_sec_cfg sec;

    memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
    sec.sub_command = MLAN_OID_SEC_CFG_ENCRYPT_KEY;

    if (key_len > MAX_WEP_KEY_SIZE)
    {
        sec.param.encrypt_key.key_flags = KEY_FLAG_TX_SEQ_VALID | KEY_FLAG_RX_SEQ_VALID;
        if (is_pairwise)
        {
            sec.param.encrypt_key.key_flags |= KEY_FLAG_SET_TX_KEY;
        }
        else
        {
            sec.param.encrypt_key.key_flags |= KEY_FLAG_GROUP_KEY;
        }
        sec.param.encrypt_key.key_index = key_index;
        memcpy(sec.param.encrypt_key.mac_addr, mac_addr, MLAN_MAC_ADDR_LENGTH);
    }
    else
    {
        sec.param.encrypt_key.key_index          = MLAN_KEY_INDEX_DEFAULT;
        sec.param.encrypt_key.is_current_wep_key = MTRUE;
    }

    sec.param.encrypt_key.key_len = key_len;
    memcpy(sec.param.encrypt_key.key_material, key, key_len);

    return wifi_send_key_material_cmd(bss_index, &sec);
}

int wifi_set_igtk_key(int bss_index, const uint8_t *pn, const uint16_t key_index, const uint8_t *key, unsigned key_len)
{
    /* fixme: check if this needs to go on heap */
    mlan_ds_sec_cfg sec;

    memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
    sec.sub_command = MLAN_OID_SEC_CFG_ENCRYPT_KEY;

    sec.param.encrypt_key.key_flags = KEY_FLAG_TX_SEQ_VALID | KEY_FLAG_RX_SEQ_VALID;

    sec.param.encrypt_key.key_flags = KEY_FLAG_AES_MCAST_IGTK;
    sec.param.encrypt_key.key_index = key_index;

    memcpy(sec.param.encrypt_key.pn, pn, SEQ_MAX_SIZE);
    sec.param.encrypt_key.key_len = key_len;
    memcpy(sec.param.encrypt_key.key_material, key, key_len);

    return wifi_send_key_material_cmd(bss_index, &sec);
}

int wifi_remove_key(int bss_index, bool is_pairwise, const uint8_t key_index, const uint8_t *mac_addr)
{
    /* fixme: check if this needs to go on heap */
    mlan_ds_sec_cfg sec;

    memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
    sec.sub_command = MLAN_OID_SEC_CFG_ENCRYPT_KEY;

    sec.param.encrypt_key.key_flags  = KEY_FLAG_REMOVE_KEY;
    sec.param.encrypt_key.key_remove = MTRUE;

    sec.param.encrypt_key.key_index = key_index;
    memcpy(sec.param.encrypt_key.mac_addr, mac_addr, MLAN_MAC_ADDR_LENGTH);

    sec.param.encrypt_key.key_len = WPA_AES_KEY_LEN;

    return wifi_send_key_material_cmd(bss_index, &sec);
}

static wifi_antcfg_t wifi_antcfg;

static int wifi_send_rf_antenna_cmd(t_u16 action, t_u32 *ant_mode, t_u16 *evaluate_time)
{
    mlan_private *pmpriv = (mlan_private *)mlan_adap->priv[0];
    mlan_ds_ant_cfg_1x1 ant_cfg_1x1;

    memset(&ant_cfg_1x1, 0x00, sizeof(mlan_ds_ant_cfg_1x1));

    ant_cfg_1x1.antenna       = *ant_mode;
    ant_cfg_1x1.evaluate_time = *evaluate_time;

    if (action != HostCmd_ACT_GEN_GET && action != HostCmd_ACT_GEN_SET)
        return -WM_FAIL;

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    memset(cmd, 0x00, S_DS_GEN + sizeof(HostCmd_DS_802_11_RF_ANTENNA));

    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    mlan_status rv =
        wlan_ops_sta_prepare_cmd(pmpriv, HostCmd_CMD_802_11_RF_ANTENNA, action, 0, NULL, &ant_cfg_1x1, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
    {
        wifi_put_command_lock();
        return -WM_FAIL;
    }

    wifi_wait_for_cmdresp(action == HostCmd_ACT_GEN_GET ? &wifi_antcfg : NULL);

    *ant_mode      = wifi_antcfg.ant_mode;
    *evaluate_time = wifi_antcfg.evaluate_time;

    return wm_wifi.cmd_resp_status;
}

int wifi_get_antenna(t_u32 *ant_mode, t_u16 *evaluate_time)
{
    if (!ant_mode)
        return -WM_E_INVAL;

    int rv = wifi_send_rf_antenna_cmd(HostCmd_ACT_GEN_GET, ant_mode, evaluate_time);
    if (rv != WM_SUCCESS || wm_wifi.cmd_resp_status != WM_SUCCESS)
        return -WM_FAIL;

    return WM_SUCCESS;
}

int wifi_set_antenna(t_u32 ant_mode, t_u16 evaluate_time)
{
    return wifi_send_rf_antenna_cmd(HostCmd_ACT_GEN_SET, &ant_mode, &evaluate_time);
}

static int wifi_send_get_log_cmd(wlan_pkt_stats_t *stats)
{
    mlan_private *pmpriv = (mlan_private *)mlan_adap->priv[0];

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->seq_num = 0x0;
    cmd->result  = 0x0;

    mlan_status rv =
        wlan_ops_sta_prepare_cmd(pmpriv, HostCmd_CMD_802_11_GET_LOG, HostCmd_ACT_GEN_GET, 0, NULL, NULL, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    return wifi_wait_for_cmdresp(stats);
}

int wifi_get_log(wlan_pkt_stats_t *stats)
{
    int rv = wifi_send_get_log_cmd(stats);
    if (rv != WM_SUCCESS || wm_wifi.cmd_resp_status != WM_SUCCESS)
        return -WM_FAIL;

    return WM_SUCCESS;
}

static int wifi_send_cmd_802_11_supplicant_pmk(int mode, mlan_ds_sec_cfg *sec, t_u32 action)
{
    /* fixme: check if this needs to go on heap */
    mlan_ioctl_req req;

    memset(&req, 0x00, sizeof(mlan_ioctl_req));
    req.pbuf      = (t_u8 *)sec;
    req.buf_len   = sizeof(mlan_ds_sec_cfg);
    req.bss_index = 0;
    req.req_id    = MLAN_IOCTL_SEC_CFG;
    req.action    = action;

    mlan_status rv = wlan_ops_sta_ioctl(mlan_adap, &req);
    if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING)
    {
        return -WM_FAIL;
    }

    return WM_SUCCESS;
}

int wifi_send_add_wpa_pmk(int mode, char *ssid, char *bssid, char *pmk, unsigned int len)
{
    if (!ssid || (len != MLAN_MAX_KEY_LENGTH))
        return -WM_E_INVAL;

    mlan_ds_sec_cfg sec;

    memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
    sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

    /* SSID */
    int ssid_len = strlen(ssid);
    if (ssid_len > MLAN_MAX_SSID_LENGTH)
        return -WM_E_INVAL;

    mlan_ds_passphrase *pp = &sec.param.passphrase;
    pp->ssid.ssid_len      = ssid_len;
    memcpy(pp->ssid.ssid, ssid, ssid_len);

    /* MAC */
    if (bssid)
        memcpy(pp->bssid, bssid, MLAN_MAC_ADDR_LENGTH);

    /* PMK */
    pp->psk_type = MLAN_PSK_PMK;
    memcpy(pp->psk.pmk.pmk, pmk, len);

    return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

/* fixme: This function has not been tested because of known issue in
   calling function. The calling function has been disabled for that */
int wifi_send_get_wpa_pmk(int mode, char *ssid)
{
    if (!ssid)
        return -WM_E_INVAL;

    mlan_ds_sec_cfg sec;

    memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
    sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

    /* SSID */
    int ssid_len = strlen(ssid);
    if (ssid_len > MLAN_MAX_SSID_LENGTH)
        return -WM_E_INVAL;

    mlan_ds_passphrase *pp = &sec.param.passphrase;
    pp->ssid.ssid_len      = ssid_len;
    memcpy(pp->ssid.ssid, ssid, ssid_len);

    /* Zero MAC */

    pp->psk_type = MLAN_PSK_QUERY;

    return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_GET);
}

/*
Note:
Passphrase can be between 8 to 63 if it is ASCII and 64 if its PSK
hexstring
*/
int wifi_send_add_wpa_psk(int mode, char *ssid, char *passphrase, unsigned int len)
{
    if (!ssid || ((len < MLAN_MIN_PASSPHRASE_LENGTH) || (len > MLAN_MAX_PASSPHRASE_LENGTH)))
        return -WM_E_INVAL;

    mlan_ds_sec_cfg sec;

    memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
    sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

    /* SSID */
    int ssid_len = strlen(ssid);
    if (ssid_len > MLAN_MAX_SSID_LENGTH)
        return -WM_E_INVAL;

    mlan_ds_passphrase *pp = &sec.param.passphrase;
    pp->ssid.ssid_len      = ssid_len;
    memcpy(pp->ssid.ssid, ssid, ssid_len);

    /* Zero MAC */

    /* Passphrase */
    pp->psk_type                      = MLAN_PSK_PASSPHRASE;
    pp->psk.passphrase.passphrase_len = len;
    memcpy(pp->psk.passphrase.passphrase, passphrase, len);

    return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

/*
Note:
Password can be between 1 to 255 if it is ASCII
*/
int wifi_send_add_wpa3_password(int mode, char *ssid, char *password, unsigned int len)
{
    if (!ssid || ((len < MLAN_MIN_PASSWORD_LENGTH) || (len > MLAN_MAX_PASSWORD_LENGTH)))
        return -WM_E_INVAL;

    mlan_ds_sec_cfg sec;

    memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
    sec.sub_command = MLAN_OID_SEC_CFG_PASSWORD;

    /* SSID */
    int ssid_len = strlen(ssid);
    if (ssid_len > MLAN_MAX_SSID_LENGTH)
        return -WM_E_INVAL;

    mlan_ds_passphrase *pp = &sec.param.passphrase;
    pp->ssid.ssid_len      = ssid_len;
    memcpy(pp->ssid.ssid, ssid, ssid_len);

    /* Zero MAC */

    /* Passphrase */
    pp->psk_type              = MLAN_PSK_PASSWORD;
    pp->password.password_len = len;
    memcpy(pp->password.password, password, len);

    return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

int wifi_send_clear_wpa_psk(int mode, const char *ssid)
{
    if (!ssid)
        return -WM_E_INVAL;

    mlan_ds_sec_cfg sec;

    memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
    sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

    /* SSID */
    int ssid_len = strlen(ssid);
    if (ssid_len > MLAN_MAX_SSID_LENGTH)
        return -WM_E_INVAL;

    sec.param.passphrase.ssid.ssid_len = ssid_len;
    strcpy((char *)sec.param.passphrase.ssid.ssid, ssid);

    /* Zero MAC */

    sec.param.passphrase.psk_type = MLAN_PSK_CLEAR;
    return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

int wifi_send_enable_supplicant(int mode, const char *ssid)
{
    if (!ssid)
        return -WM_E_INVAL;

    mlan_ds_sec_cfg sec;

    memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
    sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

    /* SSID */
    int ssid_len = strlen(ssid);
    if (ssid_len > MLAN_MAX_SSID_LENGTH)
        return -WM_E_INVAL;

    sec.param.passphrase.ssid.ssid_len = ssid_len;
    strcpy((char *)sec.param.passphrase.ssid.ssid, ssid);

    /* Zero MAC */

    sec.param.passphrase.psk_type = MLAN_PSK_PASSPHRASE;
    return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

int wifi_send_disable_supplicant(int mode)
{
    mlan_ds_sec_cfg sec;

    memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
    sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

    sec.param.passphrase.psk_type = MLAN_PSK_CLEAR;

    return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

int wifi_set_mac_multicast_addr(const char *mlist, t_u32 num_of_addr)
{
    if (!mlist)
        return -WM_E_INVAL;
    if (num_of_addr > MLAN_MAX_MULTICAST_LIST_SIZE)
        return -WM_E_INVAL;

    mlan_multicast_list *pmcast_list;
    pmcast_list = os_mem_alloc(sizeof(mlan_multicast_list));
    if (!pmcast_list)
        return -WM_E_NOMEM;

    memcpy(pmcast_list->mac_list, mlist, num_of_addr * MLAN_MAC_ADDR_LENGTH);
    pmcast_list->num_multicast_addr = num_of_addr;
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_MAC_MULTICAST_ADR,
                                              HostCmd_ACT_GEN_SET, 0, NULL, pmcast_list, cmd);

    if (rv != MLAN_STATUS_SUCCESS)
    {
        os_mem_free(pmcast_list);
        return -WM_FAIL;
    }
    wifi_wait_for_cmdresp(NULL);
    os_mem_free(pmcast_list);
    return WM_SUCCESS;
}

int wifi_get_otp_user_data(uint8_t *buf, uint16_t len)
{
    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
    mlan_ds_misc_otp_user_data pdata;

    if (!buf)
        return -WM_E_INVAL;

    cmd->seq_num           = 0x0;
    cmd->result            = 0x0;
    pdata.user_data_length = len > MAX_OTP_USER_DATA_LEN ? MAX_OTP_USER_DATA_LEN : len;

    mlan_status rv = wlan_ops_sta_prepare_cmd((mlan_private *)mlan_adap->priv[0], HostCmd_CMD_OTP_READ_USER_DATA,
                                              HostCmd_ACT_GEN_GET, 0, NULL, &pdata, cmd);
    if (rv != MLAN_STATUS_SUCCESS)
        return -WM_FAIL;

    wifi_wait_for_cmdresp(buf);
    return wm_wifi.cmd_resp_status;
}

int wifi_get_cal_data(wifi_cal_data_t *cal_data)
{
    uint32_t size = S_DS_GEN + sizeof(HostCmd_DS_802_11_CFG_DATA) - 1;

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_CFG_DATA);

    HostCmd_DS_802_11_CFG_DATA *cfg_data_cmd = (HostCmd_DS_802_11_CFG_DATA *)((uint32_t)cmd + S_DS_GEN);

    cfg_data_cmd->action   = HostCmd_ACT_GEN_GET;
    cfg_data_cmd->type     = 0x02;
    cfg_data_cmd->data_len = 0x00;

    cmd->size    = size;
    cmd->seq_num = 0x00;
    cmd->result  = 0x00;

    wifi_wait_for_cmdresp(cal_data);

    return wm_wifi.cmd_resp_status;
}

int wifi_get_firmware_version_ext(wifi_fw_version_ext_t *version_ext)
{
    if (!version_ext)
        return -WM_E_INVAL;

    mlan_private *pmpriv = (mlan_private *)mlan_adap->priv[0];
    mlan_status rv       = wifi_prepare_and_send_cmd(pmpriv, HostCmd_CMD_VERSION_EXT, HostCmd_ACT_GEN_GET, 0, NULL,
                                               &version_ext->version_str_sel, MLAN_BSS_TYPE_STA, version_ext);
    return (rv == MLAN_STATUS_SUCCESS ? WM_SUCCESS : -WM_FAIL);
}

int wifi_get_firmware_version(wifi_fw_version_t *ver)
{
    if (!ver)
        return -WM_E_INVAL;

    union
    {
        uint32_t l;
        uint8_t c[4];
    } u_ver;
    char fw_ver[32];

    u_ver.l = mlan_adap->fw_release_number;
    sprintf(fw_ver, "%u.%u.%u.p%u", u_ver.c[2], u_ver.c[1], u_ver.c[0], u_ver.c[3]);

    snprintf(ver->version_str, MLAN_MAX_VER_STR_LEN, driver_version_format, fw_ver, driver_version);

    return WM_SUCCESS;
}

/* Region: US(US) or Canada(CA) or Singapore(SG) 2.4 GHz */
wifi_sub_band_set_t subband_US_CA_SG_2_4_GHz[] = {{1, 11, 20}};

/* Region: Europe(EU), Australia(AU), Republic of Korea(KR),
China(CN) 2.4 GHz */
wifi_sub_band_set_t subband_EU_AU_KR_CN_2_4GHz[] = {{1, 13, 20}};

/* Region: France(FR) 2.4 GHz */
wifi_sub_band_set_t subband_FR_2_4GHz[] = {{1, 9, 20}, {10, 4, 10}};

/* Region: Japan(JP) 2.4 GHz */
wifi_sub_band_set_t subband_JP_2_4GHz[] = {
    {1, 14, 20},
};

/* Region: Constrained 2.4 Ghz */
wifi_sub_band_set_t subband_CS_2_4GHz[] = {{1, 9, 20}, {10, 2, 10}};

#ifdef CONFIG_5GHz_SUPPORT

/* Region: US(US) or France(FR) or Singapore(SG) 5 GHz */
wifi_sub_band_set_t subband_US_SG_FR_5_GHz[] = {{36, 8, 20}, {100, 11, 20}, {149, 5, 20}};

/* Region: Canada(CA) 5 GHz */
wifi_sub_band_set_t subband_CA_5_GHz[] = {{36, 8, 20}, {100, 5, 20}, {132, 3, 20}, {149, 5, 20}};

/* Region: Region: Europe(EU), Australia(AU), Republic of Korea(KR)
 * 5 GHz */
wifi_sub_band_set_t subband_EU_AU_KR_5_GHz[] = {
    {36, 8, 20},
    {100, 11, 20},
};

/* Region: Japan(JP) 5 GHz */
wifi_sub_band_set_t subband_JP_5_GHz[] = {
    {8, 3, 23},
    {36, 8, 23},
    {100, 11, 23},
};

/* Region: China(CN) 5 Ghz */
wifi_sub_band_set_t subband_CN_5_GHz[] = {
    {149, 5, 33},
};

#endif /* CONFIG_5GHz_SUPPORT */

int wifi_get_region_code(t_u32 *region_code)
{
    mlan_ds_misc_cfg misc;

    mlan_ioctl_req req = {
        .bss_index = 0,
        .pbuf      = (t_u8 *)&misc,
        .action    = MLAN_ACT_GET,
    };

    mlan_status mrv = wlan_misc_ioctl_region(mlan_adap, &req);
    if (mrv != MLAN_STATUS_SUCCESS)
    {
        wifi_w("Unable to get region code");
        return -WM_FAIL;
    }

    *region_code = misc.param.region_code;
    return WM_SUCCESS;
}

int wifi_set_region_code(t_u32 region_code)
{
    mlan_ds_misc_cfg misc = {
        .param.region_code = region_code,
    };

    mlan_ioctl_req req = {
        .bss_index = 0,
        .pbuf      = (t_u8 *)&misc,
        .action    = MLAN_ACT_SET,
    };

    mlan_status mrv = wlan_misc_ioctl_region(mlan_adap, &req);
    if (mrv != MLAN_STATUS_SUCCESS)
    {
        wifi_w("Unable to set region code");
        return -WM_FAIL;
    }

    return WM_SUCCESS;
}

static int wifi_send_11d_cfg_ioctl(mlan_ds_11d_cfg *d_cfg)
{
    /* fixme: check if this needs to go on heap */
    mlan_ioctl_req req;

    memset(&req, 0x00, sizeof(mlan_ioctl_req));
    req.pbuf      = (t_u8 *)d_cfg;
    req.buf_len   = sizeof(mlan_ds_11d_cfg);
    req.bss_index = 0;
    req.req_id    = MLAN_IOCTL_11D_CFG;
    req.action    = HostCmd_ACT_GEN_SET;

    mlan_status rv = wlan_ops_sta_ioctl(mlan_adap, &req);
    if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING)
    {
        return -WM_FAIL;
    }

    return WM_SUCCESS;
}
int wifi_set_domain_params(wifi_domain_param_t *dp)
{
    if (!dp)
        return -WM_E_INVAL;

    mlan_ds_11d_cfg d_cfg;

    memset(&d_cfg, 0x00, sizeof(mlan_ds_11d_cfg));

    d_cfg.sub_command = MLAN_OID_11D_DOMAIN_INFO;

    memcpy(&d_cfg.param.domain_info.country_code, dp->country_code, COUNTRY_CODE_LEN);

    d_cfg.param.domain_info.band = BAND_B | BAND_G;

    d_cfg.param.domain_info.band |= BAND_GN;
#ifdef CONFIG_5GHz_SUPPORT
    d_cfg.param.domain_info.band |= BAND_AN;
#endif
    d_cfg.param.domain_info.no_of_sub_band = dp->no_of_sub_band;

    wifi_sub_band_set_t *is  = dp->sub_band;
    mlan_ds_subband_set_t *s = d_cfg.param.domain_info.sub_band;
    int i;
    for (i = 0; i < dp->no_of_sub_band; i++)
    {
        s[i].first_chan = is[i].first_chan;
        s[i].no_of_chan = is[i].no_of_chan;
        s[i].max_tx_pwr = is[i].max_tx_pwr;
    }

    wifi_enable_11d_support_APIs();

    return wifi_send_11d_cfg_ioctl(&d_cfg);
}

int wifi_enable_11d_support()
{
    mlan_private *pmpriv = (mlan_private *)mlan_adap->priv[0];

    wrapper_wlan_11d_enable();

    return wlan_enable_11d_support(pmpriv);
}

int wifi_enable_11d_support_APIs()
{
    mlan_private *pmpriv = (mlan_private *)mlan_adap->priv[0];

    return wlan_11d_support_APIs(pmpriv);
}

wifi_sub_band_set_t *get_sub_band_from_country(int country, int *nr_sb)
{
    *nr_sb = 1;

    switch (country)
    {
        case 1:
        case 2:
        case 3:
            return subband_US_CA_SG_2_4_GHz;
        case 4:
        case 5:
        case 6:
        case 9:
            return subband_EU_AU_KR_CN_2_4GHz;
        case 7:
            *nr_sb = 2;
            return subband_FR_2_4GHz;
        case 8:
            return subband_JP_2_4GHz;
        default:
            *nr_sb = 2;
            return subband_CS_2_4GHz;
    }
}

static wifi_sub_band_set_t *get_sub_band_from_region_code(int region_code, int *nr_sb)
{
    *nr_sb = 1;

    switch (region_code)
    {
        case 0x10:
            return subband_US_CA_SG_2_4_GHz;
        case 0x30:
            return subband_EU_AU_KR_CN_2_4GHz;
        case 0x32:
            *nr_sb = 2;
            return subband_FR_2_4GHz;
        case 0xFF:
            return subband_JP_2_4GHz;
        default:
            *nr_sb = 2;
            return subband_CS_2_4GHz;
    }
}

#ifdef CONFIG_5GHz_SUPPORT
static wifi_sub_band_set_t *get_sub_band_from_country_5ghz(int country, int *nr_sb)
{
    *nr_sb = 1;

    switch (country)
    {
        case 1:
        case 3:
        case 7:
            *nr_sb = 3;
            return subband_US_SG_FR_5_GHz;
        case 2:
            *nr_sb = 4;
            return subband_CA_5_GHz;
        case 4:
        case 5:
        case 6:
            *nr_sb = 2;
            return subband_EU_AU_KR_5_GHz;
        case 8:
            *nr_sb = 3;
            return subband_JP_5_GHz;
        case 9:
            return subband_CN_5_GHz;
        default:
            *nr_sb = 3;
            return subband_US_SG_FR_5_GHz;
    }
}

static wifi_sub_band_set_t *get_sub_band_from_region_code_5ghz(int region_code, int *nr_sb)
{
    *nr_sb = 1;

    switch (region_code)
    {
        case 0x10:
        case 0x32:
            *nr_sb = 3;
            return subband_US_SG_FR_5_GHz;
        case 0x20:
            *nr_sb = 4;
            return subband_CA_5_GHz;
        case 0x30:
            *nr_sb = 2;
            return subband_EU_AU_KR_5_GHz;
        case 0x40:
            *nr_sb = 3;
            return subband_JP_5_GHz;
        case 0x50:
            return subband_CN_5_GHz;
        default:
            *nr_sb = 3;
            return subband_US_SG_FR_5_GHz;
    }
}
#endif /* CONFIG_5GHz_SUPPORT */

bool wifi_11d_is_channel_allowed(int channel)
{
    int i, j, k, nr_sb = 0;

    mlan_private *pmpriv = (mlan_private *)mlan_adap->priv[0];

    wifi_sub_band_set_t *sub_band = NULL;

    if (channel > 14)
    {
#ifdef CONFIG_5GHz_SUPPORT
        if (wifi_11d_country == 0x00)
            sub_band = get_sub_band_from_region_code_5ghz(pmpriv->adapter->region_code, &nr_sb);
        else
            sub_band = get_sub_band_from_country_5ghz(wifi_11d_country, &nr_sb);
#else
        wifi_w("5 GHz support is not enabled");

#endif /* CONFIG_5GHz_SUPPORT */
    }
    else
    {
        if (wifi_11d_country == 0x00)
            sub_band = get_sub_band_from_region_code(pmpriv->adapter->region_code, &nr_sb);
        else
            sub_band = get_sub_band_from_country(wifi_11d_country, &nr_sb);
    }

    for (i = 0; i < nr_sb; i++)
    {
        j = sub_band[i].first_chan;

        for (k = 0; k < sub_band[i].no_of_chan; k++)
        {
            if (j == channel)
                return true;

            if (channel > 14)
                j += 4;
            else
                j++;
        }
    }

    return false;
}

char *wifi_get_country_str(int country)
{
    switch (country)
    {
        case COUNTRY_WW:
            return "WW ";
        case COUNTRY_US:
            return "US ";
        case COUNTRY_CA:
            return "CA ";
        case COUNTRY_SG:
            return "SG ";
        case COUNTRY_EU:
            return "EU ";
        case COUNTRY_AU:
            return "AU ";
        case COUNTRY_KR:
            return "KR ";
        case COUNTRY_FR:
            return "FR ";
        case COUNTRY_JP:
            return "JP ";
        case COUNTRY_CN:
            return "CN ";
        default:
            return "WW ";
    }
}

static wifi_domain_param_t *get_11d_domain_params(int country, wifi_sub_band_set_t *sub_band, int nr_sb)
{
    wifi_domain_param_t *dp = os_mem_alloc(sizeof(wifi_domain_param_t) + (sizeof(wifi_sub_band_set_t) * (nr_sb - 1)));

    memcpy(dp->country_code, wifi_get_country_str(country), COUNTRY_CODE_LEN);

    dp->no_of_sub_band = nr_sb;
    memcpy(&dp->sub_band[0], sub_band, nr_sb * sizeof(wifi_sub_band_set_t));

    return dp;
}

int wifi_get_country()
{
    return wifi_11d_country;
}

int wifi_set_country(int country)
{
    int ret, nr_sb;

    wifi_11d_country = country;

    wifi_sub_band_set_t *sub_band = get_sub_band_from_country(country, &nr_sb);

    wifi_domain_param_t *dp = get_11d_domain_params(country, sub_band, nr_sb);

#if 0
	ret = wifi_uap_set_domain_params(dp);

	if (ret != WM_SUCCESS) {
		wifi_11d_country = 0x00;
		os_mem_free(dp);
		return ret;
	}
#endif
    ret = wifi_set_domain_params(dp);

    if (ret != WM_SUCCESS)
    {
        wifi_11d_country = 0x00;
        os_mem_free(dp);
        return ret;
    }

    os_mem_free(dp);
    return WM_SUCCESS;
}

int wifi_enable_ecsa_support()
{
    return wrapper_wlan_ecsa_enable();
}

bool wifi_is_ecsa_enabled()
{
    return mlan_adap->ecsa_enable;
}

static int get_free_mgmt_ie_index()
{
    if (!(mgmt_ie_index_bitmap & MBIT(0)))
        return 0;
    else if (!(mgmt_ie_index_bitmap & MBIT(1)))
        return 1;
    else if (!(mgmt_ie_index_bitmap & MBIT(2)))
        return 2;
    else if (!(mgmt_ie_index_bitmap & MBIT(3)))
        return 3;
    return -1;
}

static void set_ie_index(int index)
{
    mgmt_ie_index_bitmap |= (MBIT(index));
}

static void clear_ie_index(int index)
{
    mgmt_ie_index_bitmap &= ~(MBIT(index));
}

int wifi_config_mgmt_ie(
    unsigned int bss_type, int action, IEEEtypes_ElementId_t index, void *buffer, unsigned int *ie_len)
{
    uint8_t *buf, *pos;
    IEEEtypes_Header_t *ptlv_header = NULL;
    uint16_t buf_len                = 0;
    tlvbuf_custom_ie *tlv           = NULL;
    custom_ie *ie_ptr               = NULL;
    int mgmt_ie_index               = -1;
    int total_len =
        sizeof(tlvbuf_custom_ie) + 2 * (sizeof(custom_ie) - MAX_IE_SIZE) + sizeof(IEEEtypes_Header_t) + *ie_len;

    buf = (uint8_t *)os_mem_alloc(total_len);
    if (!buf)
    {
        wifi_e("Cannot allocate memory");
        return -WM_FAIL;
    }

    memset(buf, 0, total_len);

    tlv       = (tlvbuf_custom_ie *)buf;
    tlv->type = MRVL_MGMT_IE_LIST_TLV_ID;

    /* Locate headers */
    ie_ptr = (custom_ie *)(tlv->ie_data);
    /* Set TLV fields */
    buf_len = sizeof(tlvbuf_custom_ie);

    if (action == HostCmd_ACT_GEN_SET)
    {
        if (*ie_len == 0)
        {
            if (index != MGMT_RSN_IE || index != MGMT_VENDOR_SPECIFIC_221 || index != MGMT_WPA_IE ||
                index != MGMT_WPS_IE)
            {
                if (buf)
                    os_mem_free(buf);
                return -WM_FAIL;
            }

            /* Clear WPS IE */
            ie_ptr->mgmt_subtype_mask = MGMT_MASK_CLEAR;
            ie_ptr->ie_length         = 0;
            ie_ptr->ie_index          = index;

            ie_ptr                    = (custom_ie *)(((uint8_t *)ie_ptr) + sizeof(custom_ie) - MAX_IE_SIZE);
            ie_ptr->mgmt_subtype_mask = MGMT_MASK_CLEAR;
            ie_ptr->ie_length         = 0;
            ie_ptr->ie_index          = index + 1;
            tlv->length               = 2 * (sizeof(custom_ie) - MAX_IE_SIZE);
            buf_len += tlv->length;
            clear_ie_index(index);
        }
        else
        {
            /* Set WPS IE */
            mgmt_ie_index = get_free_mgmt_ie_index();

            if (mgmt_ie_index < 0)
            {
                if (buf)
                    os_mem_free(buf);
                return -WM_FAIL;
            }

            pos         = ie_ptr->ie_buffer;
            ptlv_header = (IEEEtypes_Header_t *)pos;
            pos += sizeof(IEEEtypes_Header_t);

            ptlv_header->element_id = index;
            ptlv_header->len        = *ie_len;
            if (bss_type == MLAN_BSS_TYPE_UAP)
                ie_ptr->mgmt_subtype_mask =
                    MGMT_MASK_BEACON | MGMT_MASK_PROBE_RESP | MGMT_MASK_ASSOC_RESP | MGMT_MASK_REASSOC_RESP;
            else if (bss_type == MLAN_BSS_TYPE_STA)
                ie_ptr->mgmt_subtype_mask = MGMT_MASK_PROBE_REQ | MGMT_MASK_ASSOC_REQ | MGMT_MASK_REASSOC_REQ;

            tlv->length       = sizeof(custom_ie) + sizeof(IEEEtypes_Header_t) + *ie_len - MAX_IE_SIZE;
            ie_ptr->ie_length = sizeof(IEEEtypes_Header_t) + *ie_len;
            ie_ptr->ie_index  = mgmt_ie_index;

            buf_len += tlv->length;

            memcpy(pos, buffer, *ie_len);
        }
    }
    else if (action == HostCmd_ACT_GEN_GET)
    {
        /* Get WPS IE */
        tlv->length = 0;
    }

    mlan_status rv = wrapper_wlan_cmd_mgmt_ie(bss_type, buf, buf_len, action);

    if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING)
    {
        os_mem_free(buf);
        return -WM_FAIL;
    }

    if (action == HostCmd_ACT_GEN_GET)
    {
        if (wm_wifi.cmd_resp_status)
        {
            wifi_w("Unable to get mgmt ie buffer");
            os_mem_free(buf);
            return wm_wifi.cmd_resp_status;
        }
        ie_ptr = (custom_ie *)(buf);
        memcpy(buffer, ie_ptr->ie_buffer, ie_ptr->ie_length);
        *ie_len = ie_ptr->ie_length;
    }

    if (buf)
        os_mem_free(buf);

    if ((action == HostCmd_ACT_GEN_SET) && *ie_len)
    {
        set_ie_index(mgmt_ie_index);
        return mgmt_ie_index;
    }
    else
        return WM_SUCCESS;
}

int wifi_get_mgmt_ie(unsigned int bss_type, IEEEtypes_ElementId_t index, void *buf, unsigned int *buf_len)
{
    return wifi_config_mgmt_ie(bss_type, HostCmd_ACT_GEN_GET, index, buf, buf_len);
}

int wifi_set_mgmt_ie(unsigned int bss_type, IEEEtypes_ElementId_t id, void *buf, unsigned int buf_len)
{
    unsigned int data_len = buf_len;

    return wifi_config_mgmt_ie(bss_type, HostCmd_ACT_GEN_SET, id, buf, &data_len);
}

int wifi_clear_mgmt_ie(unsigned int bss_type, IEEEtypes_ElementId_t index)
{
    unsigned int data_len = 0;
    return wifi_config_mgmt_ie(bss_type, HostCmd_ACT_GEN_SET, index, NULL, &data_len);
}

int wifi_set_chanlist(wifi_chanlist_t *chanlist)
{
    mlan_status ret;
    int i          = 0;
    t_u8 cfp_no_bg = 0;
#ifdef CONFIG_5GHz_SUPPORT
    t_u8 cfp_no_a = 0;
#endif

#ifdef OTP_CHANINFO
    mlan_adapter *pmadapter = mlan_adap->priv[0]->adapter;
    if (!(pmadapter->otp_region && pmadapter->otp_region->force_reg))
    {
#endif
        /*
         * Validate if the channels provided in the channel list
         * are valid channels according to World Wide Safe Mode.
         */
        for (i = 0; i < chanlist->num_chans; i++)
        {
            if (!wlan_is_channel_and_freq_valid(chanlist->chan_info[i].chan_num, chanlist->chan_info[i].chan_freq))
            {
                wifi_e("Invalid channel %d\r\n", chanlist->chan_info[i].chan_num);
                return -WM_FAIL;
            }
        }

        /* Configure Custom CFP Tables */
#ifdef CONFIG_5GHz_SUPPORT
        ret = wlan_set_custom_cfp_table(chanlist, &cfp_no_bg, &cfp_no_a);
#else
    ret = wlan_set_custom_cfp_table(chanlist, &cfp_no_bg);
#endif
        if (ret != MLAN_STATUS_SUCCESS)
        {
            wifi_e("Failed to set Custom CFP Table");
            return -WM_FAIL;
        }

        /* Set Region Table */
#ifdef CONFIG_5GHz_SUPPORT
        wlan_set_custom_regiontable((mlan_private *)mlan_adap->priv[0], cfp_no_bg, cfp_no_a);
#else
    wlan_set_custom_regiontable((mlan_private *)mlan_adap->priv[0], cfp_no_bg);
#endif
#ifdef OTP_CHANINFO
    }
#endif

    return WM_SUCCESS;
}

int wifi_get_chanlist(wifi_chanlist_t *chanlist)
{
    mlan_adapter *pmadapter     = mlan_adap->priv[0]->adapter;
    region_chan_t *pchan_region = MNULL;
    chan_freq_power_t *cfp      = MNULL;
    t_u32 region_idx            = 0;
    t_u32 next_chan             = 0;
    chanlist->num_chans         = 0;

    for (region_idx = 0; region_idx < NELEMENTS(pmadapter->region_channel); region_idx++)
    {
        if (!pmadapter->region_channel[region_idx].valid)
            continue;

        pchan_region = &pmadapter->region_channel[region_idx];

        for (next_chan = 0; next_chan < pchan_region->num_cfp; next_chan++)
        {
            cfp = pchan_region->pcfp + next_chan;
            if (!cfp)
            {
                wifi_e("No cfp configured");
                return -WM_FAIL;
            }

            if (cfp->dynamic.flags & NXP_CHANNEL_DISABLED)
                continue;

            chanlist->chan_info[chanlist->num_chans].chan_num                     = cfp->channel;
            chanlist->chan_info[chanlist->num_chans].chan_freq                    = cfp->freq;
            chanlist->chan_info[chanlist->num_chans].passive_scan_or_radar_detect = cfp->passive_scan_or_radar_detect;
            chanlist->num_chans++;

            if (chanlist->num_chans >= NELEMENTS(chanlist->chan_info))
                break;
        }
    }

    return WM_SUCCESS;
}

void wifi_get_active_channel_list(t_u8 *chan_list, t_u8 *num_chans)
{
    if (chan_list && num_chans)
    {
        wlan_get_active_channel_list((mlan_private *)mlan_adap->priv[0], chan_list, num_chans);
    }
}

int wifi_set_txpwrlimit(wifi_txpwrlimit_t *txpwrlimit)
{
    int i, ret;
    HostCmd_DS_COMMAND *cmd                = wifi_get_command_buffer();
    t_u8 *pByte                            = NULL;
    MrvlIETypes_ChanTRPCConfig_t *trpc_tlv = NULL;

    wifi_get_command_lock();

    cmd->command = HostCmd_CMD_CHANNEL_TRPC_CONFIG;
    cmd->seq_num = 0x0;
    cmd->result  = 0x0;
    cmd->size    = S_DS_GEN + 2 * sizeof(t_u16) +
                txpwrlimit->num_chans * (sizeof(MrvlIEtypesHeader_t) + sizeof(MrvlChannelDesc_t)) +
                (txpwrlimit->num_chans * txpwrlimit->txpwrlimit_config->num_mod_grps * sizeof(MrvlChanTrpcEntry_t));

    HostCmd_DS_CHAN_TRPC_CONFIG *txpwrlimit_config = (HostCmd_DS_CHAN_TRPC_CONFIG *)((uint8_t *)cmd + S_DS_GEN);

    txpwrlimit_config->action   = HostCmd_ACT_GEN_SET;
    txpwrlimit_config->reserved = txpwrlimit->subband;

    pByte = (t_u8 *)txpwrlimit_config->tlv_buffer;

    for (i = 0; i < txpwrlimit->num_chans; i++)
    {
        trpc_tlv              = (MrvlIETypes_ChanTRPCConfig_t *)pByte;
        trpc_tlv->header.type = TLV_TYPE_CHANNEL_TRPC_CONFIG;
        trpc_tlv->header.len =
            sizeof(MrvlChannelDesc_t) + txpwrlimit->txpwrlimit_config->num_mod_grps * sizeof(MrvlChanTrpcEntry_t);
        trpc_tlv->start_freq = txpwrlimit->txpwrlimit_config[i].chan_desc.start_freq;
        trpc_tlv->width      = txpwrlimit->txpwrlimit_config[i].chan_desc.chan_width;
        trpc_tlv->chan_num   = txpwrlimit->txpwrlimit_config[i].chan_desc.chan_num;
        memcpy(trpc_tlv->mod_group, txpwrlimit->txpwrlimit_config[i].txpwrlimit_entry,
               txpwrlimit->txpwrlimit_config->num_mod_grps * sizeof(MrvlChanTrpcEntry_t));
        pByte += trpc_tlv->header.len + sizeof(trpc_tlv->header);
    }
    ret = wifi_wait_for_cmdresp(NULL);
    return ret;
}

int wifi_get_txpwrlimit(wifi_SubBand_t subband, wifi_txpwrlimit_t *txpwrlimit)
{
    int ret;

    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    wifi_get_command_lock();

    cmd->command = HostCmd_CMD_CHANNEL_TRPC_CONFIG;
    cmd->seq_num = 0x0;
    cmd->result  = 0x0;
    cmd->size    = S_DS_GEN + 2 * sizeof(t_u16);

    HostCmd_DS_CHAN_TRPC_CONFIG *txpwrlimit_config = (HostCmd_DS_CHAN_TRPC_CONFIG *)((uint8_t *)cmd + S_DS_GEN);

    txpwrlimit_config->action   = HostCmd_ACT_GEN_GET;
    txpwrlimit_config->reserved = subband;

    ret = wifi_wait_for_cmdresp(txpwrlimit);
    return ret;
}

void wifi_set_curr_bss_channel(uint8_t channel)
{
    mlan_private *pmpriv = (mlan_private *)mlan_adap->priv[0];

    pmpriv->curr_bss_params.bss_descriptor.channel = channel;
}

#ifdef OTP_CHANINFO
int wifi_get_fw_region_and_cfp_tables()
{
    int ret;

    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    wifi_get_command_lock();

    cmd->command = HostCmd_CMD_CHAN_REGION_CFG;
    cmd->seq_num = 0x0;
    cmd->result  = 0x0;
    cmd->size    = S_DS_GEN + sizeof(HostCmd_DS_CHAN_REGION_CFG);

    HostCmd_DS_CHAN_REGION_CFG *chan_region_cfg = (HostCmd_DS_CHAN_REGION_CFG *)((uint8_t *)cmd + S_DS_GEN);

    chan_region_cfg->action = HostCmd_ACT_GEN_GET;

    ret = wifi_wait_for_cmdresp(NULL);
    return ret;
}
#endif

int wifi_set_ed_mac_mode(wifi_ed_mac_ctrl_t *wifi_ed_mac_ctrl)
{
    int ret;

    if (!wifi_ed_mac_ctrl)
        return -WM_FAIL;

    mlan_private *pmpriv    = (mlan_private *)mlan_adap->priv[0];
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    wifi_get_command_lock();

    cmd->command = HostCmd_CMD_ED_MAC_MODE;
    cmd->seq_num = 0x0;
    cmd->result  = 0x0;
    cmd->size    = S_DS_GEN + sizeof(HostCmd_CONFIG_ED_MAC_MODE);

    HostCmd_CONFIG_ED_MAC_MODE *ed_mac_mode = (HostCmd_CONFIG_ED_MAC_MODE *)((uint8_t *)cmd + S_DS_GEN);

    ed_mac_mode->ed_ctrl_2g   = wlan_cpu_to_le16(wifi_ed_mac_ctrl->ed_ctrl_2g);
    ed_mac_mode->ed_offset_2g = wlan_cpu_to_le16(wifi_ed_mac_ctrl->ed_offset_2g);
#ifdef CONFIG_5GHz_SUPPORT
    ed_mac_mode->ed_ctrl_5g         = wlan_cpu_to_le16(wifi_ed_mac_ctrl->ed_ctrl_5g);
    ed_mac_mode->ed_offset_5g       = wlan_cpu_to_le16(wifi_ed_mac_ctrl->ed_offset_5g);
    ed_mac_mode->ed_bitmap_txq_lock = 0xff;
#endif

    pmpriv->ed_mac_mode.ed_ctrl_2g   = wlan_cpu_to_le16(wifi_ed_mac_ctrl->ed_ctrl_2g);
    pmpriv->ed_mac_mode.ed_offset_2g = wlan_cpu_to_le16(wifi_ed_mac_ctrl->ed_offset_2g);
#ifdef CONFIG_5GHz_SUPPORT
    pmpriv->ed_mac_mode.ed_ctrl_5g   = wlan_cpu_to_le16(wifi_ed_mac_ctrl->ed_ctrl_5g);
    pmpriv->ed_mac_mode.ed_offset_5g = wlan_cpu_to_le16(wifi_ed_mac_ctrl->ed_offset_5g);
#endif

    ret = wifi_wait_for_cmdresp(NULL);
    return ret;
}

int wifi_get_ed_mac_mode(wifi_ed_mac_ctrl_t *wifi_ed_mac_ctrl)
{
    mlan_private *pmpriv = (mlan_private *)mlan_adap->priv[0];

    if (!wifi_ed_mac_ctrl)
        return -WM_FAIL;

    memset(wifi_ed_mac_ctrl, 0x00, sizeof(wifi_ed_mac_ctrl_t));

    wifi_ed_mac_ctrl->ed_ctrl_2g   = wlan_cpu_to_le16(pmpriv->ed_mac_mode.ed_ctrl_2g);
    wifi_ed_mac_ctrl->ed_offset_2g = wlan_cpu_to_le16(pmpriv->ed_mac_mode.ed_offset_2g);
#ifdef CONFIG_5GHz_SUPPORT
    wifi_ed_mac_ctrl->ed_ctrl_5g   = wlan_cpu_to_le16(pmpriv->ed_mac_mode.ed_ctrl_5g);
    wifi_ed_mac_ctrl->ed_offset_5g = wlan_cpu_to_le16(pmpriv->ed_mac_mode.ed_offset_5g);
#endif

    return WM_SUCCESS;
}

#ifndef IEEEtypes_SSID_SIZE
#define IEEEtypes_SSID_SIZE 32
#endif /* IEEEtypes_SSID_SIZE */
#define MRVL_SSID_TLV_ID          0x0000
#define PROPRIETARY_TLV_BASE_ID   0x0100
#define MRVL_BEACON_PERIOD_TLV_ID (PROPRIETARY_TLV_BASE_ID + 0x2c)
#define TLV_TYPE_CHANLIST         (PROPRIETARY_TLV_BASE_ID + 0x01)
#define TLV_TYPE_PASSTHROUGH      (PROPRIETARY_TLV_BASE_ID + 0x0a)
#define TLV_TYPE_SMCADDRRANGE     (PROPRIETARY_TLV_BASE_ID + 0xcc)
#define TLV_TYPE_SMCFRAMEFILTER   (PROPRIETARY_TLV_BASE_ID + 0xd1)

int wifi_set_smart_mode_cfg(char *ssid,
                            int beacon_period,
                            wifi_chan_list_param_set_t *chan_list,
                            uint8_t *smc_start_addr,
                            uint8_t *smc_end_addr,
                            uint16_t filter_type,
                            int smc_frame_filter_len,
                            uint8_t *smc_frame_filter,
                            int custom_ie_len,
                            uint8_t *custom_ie)
{
    unsigned int ssid_len                              = 0, i;
    uint32_t size                                      = S_DS_GEN + sizeof(HostCmd_DS_SYS_CONFIG) - 1;
    MrvlIEtypes_SsIdParamSet_t *tlv_ssid               = NULL;
    MrvlIEtypes_beacon_period_t *tlv_beacon_period     = NULL;
    MrvlIEtypes_ChanListParamSet_t *tlv_chan_list      = NULL;
    MrvlIEtypes_Data_t *tlv_custom_ie                  = NULL;
    MrvlIETypes_SmcAddrRange_t *tlv_smc_addr_range     = NULL;
    MrvlIETypes_SmcFrameFilter_t *tlv_smc_frame_filter = NULL;

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->command                          = wlan_cpu_to_le16(HOST_CMD_SMART_MODE_CFG);
    HostCmd_DS_SYS_CONFIG *sys_config_cmd = (HostCmd_DS_SYS_CONFIG *)((uint32_t)cmd + S_DS_GEN);
    sys_config_cmd->action                = HostCmd_ACT_GEN_SET;
    uint8_t *tlv                          = (uint8_t *)sys_config_cmd->tlv_buffer;

    ssid_len = strlen(ssid);
    if (ssid_len > IEEEtypes_SSID_SIZE || custom_ie_len > 255)
        return -WM_E_INVAL;
    ;

    tlv_ssid              = (MrvlIEtypes_SsIdParamSet_t *)sys_config_cmd->tlv_buffer;
    tlv_ssid->header.type = MRVL_SSID_TLV_ID;
    tlv_ssid->header.len  = strlen(ssid);
    memcpy(tlv_ssid->ssid, ssid, strlen(ssid));
    size += sizeof(tlv_ssid->header) + tlv_ssid->header.len;
    tlv += sizeof(tlv_ssid->header) + tlv_ssid->header.len;
    tlv_beacon_period                = (MrvlIEtypes_beacon_period_t *)tlv;
    tlv_beacon_period->header.type   = MRVL_BEACON_PERIOD_TLV_ID;
    tlv_beacon_period->header.len    = sizeof(uint16_t);
    tlv_beacon_period->beacon_period = beacon_period;

    size += sizeof(tlv_beacon_period->header) + tlv_beacon_period->header.len;
    tlv += sizeof(tlv_beacon_period->header) + tlv_beacon_period->header.len;

    tlv_chan_list              = (MrvlIEtypes_ChanListParamSet_t *)tlv;
    tlv_chan_list->header.type = TLV_TYPE_CHANLIST;
    tlv_chan_list->header.len  = chan_list->no_of_channels * sizeof(ChanScanParamSet_t);

    for (i = 0; i < (unsigned int)chan_list->no_of_channels; i++)
    {
        tlv_chan_list->chan_scan_param[i].chan_number   = chan_list->chan_scan_param[i].chan_number;
        tlv_chan_list->chan_scan_param[i].min_scan_time = chan_list->chan_scan_param[i].min_scan_time;
        tlv_chan_list->chan_scan_param[i].max_scan_time = chan_list->chan_scan_param[i].max_scan_time;
    }

    size += sizeof(tlv_chan_list->header) + tlv_chan_list->header.len;
    tlv += sizeof(tlv_chan_list->header) + tlv_chan_list->header.len;

    if (custom_ie && custom_ie_len > 0)
    {
        tlv_custom_ie              = (MrvlIEtypes_Data_t *)tlv;
        tlv_custom_ie->header.type = TLV_TYPE_PASSTHROUGH;
        tlv_custom_ie->header.len  = custom_ie_len;
        memcpy(tlv_custom_ie->data, custom_ie, custom_ie_len);

        size += sizeof(tlv_custom_ie->header) + tlv_custom_ie->header.len;
        tlv += sizeof(tlv_custom_ie->header) + tlv_custom_ie->header.len;
    }

    if (smc_start_addr && smc_end_addr)
    {
        tlv_smc_addr_range              = (MrvlIETypes_SmcAddrRange_t *)tlv;
        tlv_smc_addr_range->header.type = TLV_TYPE_SMCADDRRANGE;
        tlv_smc_addr_range->header.len  = 2 * MLAN_MAC_ADDR_LENGTH + sizeof(uint16_t);

        memcpy(tlv_smc_addr_range->smcstartAddr, smc_start_addr, MLAN_MAC_ADDR_LENGTH);
        memcpy(tlv_smc_addr_range->smcendAddr, smc_end_addr, MLAN_MAC_ADDR_LENGTH);

        tlv_smc_addr_range->filter_type = filter_type;

        size += sizeof(tlv_smc_addr_range->header) + tlv_smc_addr_range->header.len;
        tlv += sizeof(tlv_smc_addr_range->header) + tlv_smc_addr_range->header.len;
    }

    tlv_smc_frame_filter              = (MrvlIETypes_SmcFrameFilter_t *)tlv;
    tlv_smc_frame_filter->header.type = TLV_TYPE_SMCFRAMEFILTER;
    tlv_smc_frame_filter->header.len  = smc_frame_filter_len;
    memcpy(tlv_smc_frame_filter->frame_filter, smc_frame_filter, smc_frame_filter_len);

    size += sizeof(tlv_smc_frame_filter->header) + tlv_smc_frame_filter->header.len;
    tlv += sizeof(tlv_smc_frame_filter->header) + tlv_smc_frame_filter->header.len;

    cmd->size    = size;
    cmd->seq_num = 0x00;
    cmd->result  = 0x00;

    wifi_wait_for_cmdresp(NULL);

    return WM_SUCCESS;
}

int wifi_get_smart_mode_cfg()
{
    uint32_t size = S_DS_GEN + sizeof(HostCmd_DS_SYS_CONFIG) - 1;

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->command                          = wlan_cpu_to_le16(HOST_CMD_SMART_MODE_CFG);
    HostCmd_DS_SYS_CONFIG *sys_config_cmd = (HostCmd_DS_SYS_CONFIG *)((uint32_t)cmd + S_DS_GEN);
    sys_config_cmd->action                = HostCmd_ACT_GEN_GET;

    cmd->size    = size;
    cmd->seq_num = 0x00;
    cmd->result  = 0x00;

    wifi_wait_for_cmdresp(NULL);
    return WM_SUCCESS;
}

int wifi_start_smart_mode()
{
    uint32_t size = S_DS_GEN + sizeof(HostCmd_DS_SYS_CONFIG) - 1;

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->command                          = wlan_cpu_to_le16(HOST_CMD_SMART_MODE_CFG);
    HostCmd_DS_SYS_CONFIG *sys_config_cmd = (HostCmd_DS_SYS_CONFIG *)((uint32_t)cmd + S_DS_GEN);
    sys_config_cmd->action                = HostCmd_ACT_GEN_START;

    cmd->size    = size;
    cmd->seq_num = 0x00;
    cmd->result  = 0x00;

    wifi_wait_for_cmdresp(NULL);
    return WM_SUCCESS;
}

int wifi_stop_smart_mode()
{
    uint32_t size = S_DS_GEN + sizeof(HostCmd_DS_SYS_CONFIG) - 1;

    wifi_get_command_lock();
    HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

    cmd->command                          = wlan_cpu_to_le16(HOST_CMD_SMART_MODE_CFG);
    HostCmd_DS_SYS_CONFIG *sys_config_cmd = (HostCmd_DS_SYS_CONFIG *)((uint32_t)cmd + S_DS_GEN);
    sys_config_cmd->action                = HostCmd_ACT_GEN_STOP;

    cmd->size    = size;
    cmd->seq_num = 0x00;
    cmd->result  = 0x00;

    wifi_wait_for_cmdresp(NULL);

    return WM_SUCCESS;
}
