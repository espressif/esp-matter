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
#include <string.h>
#include <bl_os_private.h>
#include <utils_tlv_bl.h>
#include <bl60x_fw_api.h>

#include "errno.h"
#include "bl_msg_tx.h"
#include "bl_utils.h"

// field definitions
#define NXMAC_EN_DUPLICATE_DETECTION_BIT         ((uint32_t)0x80000000)
#define NXMAC_EN_DUPLICATE_DETECTION_POS         31
#define NXMAC_ACCEPT_UNKNOWN_BIT                 ((uint32_t)0x40000000)
#define NXMAC_ACCEPT_UNKNOWN_POS                 30
#define NXMAC_ACCEPT_OTHER_DATA_FRAMES_BIT       ((uint32_t)0x20000000)
#define NXMAC_ACCEPT_OTHER_DATA_FRAMES_POS       29
#define NXMAC_ACCEPT_QO_S_NULL_BIT               ((uint32_t)0x10000000)
#define NXMAC_ACCEPT_QO_S_NULL_POS               28
#define NXMAC_ACCEPT_QCFWO_DATA_BIT              ((uint32_t)0x08000000)
#define NXMAC_ACCEPT_QCFWO_DATA_POS              27
#define NXMAC_ACCEPT_Q_DATA_BIT                  ((uint32_t)0x04000000)
#define NXMAC_ACCEPT_Q_DATA_POS                  26
#define NXMAC_ACCEPT_CFWO_DATA_BIT               ((uint32_t)0x02000000)
#define NXMAC_ACCEPT_CFWO_DATA_POS               25
#define NXMAC_ACCEPT_DATA_BIT                    ((uint32_t)0x01000000)
#define NXMAC_ACCEPT_DATA_POS                    24
#define NXMAC_ACCEPT_OTHER_CNTRL_FRAMES_BIT      ((uint32_t)0x00800000)
#define NXMAC_ACCEPT_OTHER_CNTRL_FRAMES_POS      23
#define NXMAC_ACCEPT_CF_END_BIT                  ((uint32_t)0x00400000)
#define NXMAC_ACCEPT_CF_END_POS                  22
#define NXMAC_ACCEPT_ACK_BIT                     ((uint32_t)0x00200000)
#define NXMAC_ACCEPT_ACK_POS                     21
#define NXMAC_ACCEPT_CTS_BIT                     ((uint32_t)0x00100000)
#define NXMAC_ACCEPT_CTS_POS                     20
#define NXMAC_ACCEPT_RTS_BIT                     ((uint32_t)0x00080000)
#define NXMAC_ACCEPT_RTS_POS                     19
#define NXMAC_ACCEPT_PS_POLL_BIT                 ((uint32_t)0x00040000)
#define NXMAC_ACCEPT_PS_POLL_POS                 18
#define NXMAC_ACCEPT_BA_BIT                      ((uint32_t)0x00020000)
#define NXMAC_ACCEPT_BA_POS                      17
#define NXMAC_ACCEPT_BAR_BIT                     ((uint32_t)0x00010000)
#define NXMAC_ACCEPT_BAR_POS                     16
#define NXMAC_ACCEPT_OTHER_MGMT_FRAMES_BIT       ((uint32_t)0x00008000)
#define NXMAC_ACCEPT_OTHER_MGMT_FRAMES_POS       15
#define NXMAC_ACCEPT_ALL_BEACON_BIT              ((uint32_t)0x00002000)
#define NXMAC_ACCEPT_ALL_BEACON_POS              13
#define NXMAC_ACCEPT_NOT_EXPECTED_BA_BIT         ((uint32_t)0x00001000)
#define NXMAC_ACCEPT_NOT_EXPECTED_BA_POS         12
#define NXMAC_ACCEPT_DECRYPT_ERROR_FRAMES_BIT    ((uint32_t)0x00000800)
#define NXMAC_ACCEPT_DECRYPT_ERROR_FRAMES_POS    11
#define NXMAC_ACCEPT_BEACON_BIT                  ((uint32_t)0x00000400)
#define NXMAC_ACCEPT_BEACON_POS                  10
#define NXMAC_ACCEPT_PROBE_RESP_BIT              ((uint32_t)0x00000200)
#define NXMAC_ACCEPT_PROBE_RESP_POS              9
#define NXMAC_ACCEPT_PROBE_REQ_BIT               ((uint32_t)0x00000100)
#define NXMAC_ACCEPT_PROBE_REQ_POS               8
#define NXMAC_ACCEPT_MY_UNICAST_BIT              ((uint32_t)0x00000080)
#define NXMAC_ACCEPT_MY_UNICAST_POS              7
#define NXMAC_ACCEPT_UNICAST_BIT                 ((uint32_t)0x00000040)
#define NXMAC_ACCEPT_UNICAST_POS                 6
#define NXMAC_ACCEPT_ERROR_FRAMES_BIT            ((uint32_t)0x00000020)
#define NXMAC_ACCEPT_ERROR_FRAMES_POS            5
#define NXMAC_ACCEPT_OTHER_BSSID_BIT             ((uint32_t)0x00000010)
#define NXMAC_ACCEPT_OTHER_BSSID_POS             4
#define NXMAC_ACCEPT_BROADCAST_BIT               ((uint32_t)0x00000008)
#define NXMAC_ACCEPT_BROADCAST_POS               3
#define NXMAC_ACCEPT_MULTICAST_BIT               ((uint32_t)0x00000004)
#define NXMAC_ACCEPT_MULTICAST_POS               2
#define NXMAC_DONT_DECRYPT_BIT                   ((uint32_t)0x00000002)
#define NXMAC_DONT_DECRYPT_POS                   1
#define NXMAC_EXC_UNENCRYPTED_BIT                ((uint32_t)0x00000001)
#define NXMAC_EXC_UNENCRYPTED_POS                0

static const struct mac_addr mac_addr_bcst = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
static const struct mac_addr mac_addr_zero = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

static const struct ieee80211_channel bl_channels_24_General[] = {
    { .band = NL80211_BAND_2GHZ, .center_freq = 2412, .hw_value = 1, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2417, .hw_value = 2, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2422, .hw_value = 3, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2427, .hw_value = 4, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2432, .hw_value = 5, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2437, .hw_value = 6, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2442, .hw_value = 7, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2447, .hw_value = 8, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2452, .hw_value = 9, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2457, .hw_value = 10, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2462, .hw_value = 11, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2467, .hw_value = 12, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2472, .hw_value = 13, .max_power=20},
    { .band = NL80211_BAND_2GHZ, .center_freq = 2484, .hw_value = 14, .max_power=20},
};

static const struct ieee80211_dot_d country_list[] = 
{
    /*First Country is the default country*/
    {
        .code   = "CN",
        .channel_num = 13,
        .channels = bl_channels_24_General,
    },
    {
        .code   = "JP",
        .channel_num = 14,
        .channels = bl_channels_24_General,
    },
    {
        .code   = "US",
        .channel_num = 11,
        .channels = bl_channels_24_General,
    },
    {
        .code   = "EU",
        .channel_num = 13,
        .channels = bl_channels_24_General,
    },
};

static int channel_num_default;
static const struct ieee80211_channel *channels_default;
static const struct ieee80211_dot_d *country_default;

static int cfg80211_get_channel_list(const char *code, int *channel_num, const struct ieee80211_channel **channels, const struct ieee80211_dot_d **country_default)
{
    int i;

    for (i = 0; i < sizeof(country_list)/sizeof(country_list[0]); i++) {
        if (0 == strcmp(country_list[i].code, code)) {
            if(channel_num){
                *channel_num = country_list[i].channel_num;
            }
            if (channels) {
                *channels = country_list[i].channels;
            }
            if (country_default) {
                *country_default = &country_list[i];
            }
            return 0;
        }
    }
    /*NOT found code*/
    return -1;
}

void bl_msg_update_channel_cfg(const char *code)
{
    if (cfg80211_get_channel_list(code, &channel_num_default, &channels_default, &country_default)) {
        /*get channel list failed, so we set the default one*/
        channel_num_default = sizeof(bl_channels_24_General)/sizeof(bl_channels_24_General[0]);
        channels_default = bl_channels_24_General;
        country_default = &country_list[0];
        bl_os_printf("[WF] %s NOT found, using General instead, num of channel %d\r\n", code, channel_num_default);
    } else {
        bl_os_printf("[WF] country code %s used, num of channel %d\r\n", code, channel_num_default);
    }

}

int bl_msg_get_channel_nums()
{
    return channel_num_default;
}

int bl_get_fixed_channels_is_valid(uint16_t *channels, uint16_t channel_num)
{
    int i;
    int channel;

    if (0 == channel_num) {
        return 0;
    }

    for (i = 0; i < channel_num; i++) {
        channel = channels[i];
        if (0 == channel || (channel > bl_msg_get_channel_nums())) {
            return 0;
        }
    }

    return 1;
}

inline uint16_t phy_channel_to_freq(uint8_t band, int channel)
{
    uint16_t freq = 0xFFFF;

    do
    {
        //2.4.GHz
        if (band == PHY_BAND_2G4)
        {
            // Check if the channel number is in the expected range
            if ((channel < 1) || (channel > 14))
                break;

            // Compute the channel number
            if (channel == 14)
                freq = 2484;
            else
                freq = 2407 + channel * 5;
        }
        //5 GHz
        else if (band == PHY_BAND_5G)
        {
            // Check if frequency is in the expected range
            if ((channel < 1) || (channel > 165))
                break;

            // Compute the channel number
            freq = 5000 + channel * 5;
        }
    } while(0);

    return (freq);
}

inline uint8_t phy_freq_to_channel(uint8_t band, uint16_t freq)
{
    uint8_t channel = 0;

    do
    {
        //2.4.GHz
        if (band == PHY_BAND_2G4)
        {
            // Check if the frequency is in the expected range
            if ((freq < 2412) || (freq > 2484))
                break;

            if (freq == 2484)
                channel = 14;
            else
                channel = (freq - 2407) / 5;
        }
#if 0
        //5 GHz
        else if (band == PHY_BAND_5G)
        {
            // Check if frequency is in the expected range (34-165)
            if ((freq < 5170) || (freq > 5825))
                break;

            channel = (freq - 5000) / 5;
        }
#endif
    } while (0);

    return (channel);
}

/**
 ****************************************************************************************
 *
 * @file bl_msg_tx.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

static inline void *bl_msg_zalloc(ke_msg_id_t const id,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id,
                                    uint16_t const param_len)
{
    struct lmac_msg *msg;

    msg = (struct lmac_msg *)bl_os_malloc(sizeof(struct lmac_msg) + param_len);
    if (msg == NULL) {
        bl_os_printf("%s: msg allocation failed\n", __func__);
        return NULL;
    }
    memset(msg, 0, sizeof(struct lmac_msg) + param_len);

    msg->id = id;
    msg->dest_id = dest_id;
    msg->src_id = src_id;
    msg->param_len = param_len;

    return msg->param;
}

static inline bool is_non_blocking_msg(int id) {
    return ((id == MM_TIM_UPDATE_REQ) || (id == ME_RC_SET_RATE_REQ) ||
            (id == MM_BFMER_ENABLE_REQ) || (id == ME_TRAFFIC_IND_REQ));
}

#define BITS_PER_LONG 32
#define BIT_WORD(nr)        ((nr) / BITS_PER_LONG)
/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static inline int test_bit(int nr, const volatile unsigned long *addr)
{
    return 1UL & (addr[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG-1)));
}

static int bl_send_msg(struct bl_hw *bl_hw, const void *msg_params,
                         int reqcfm, ke_msg_id_t reqid, void *cfm)
{
    struct lmac_msg *msg;
    struct bl_cmd *cmd;
    bool nonblock;
    int ret;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    msg = container_of((void *)msg_params, struct lmac_msg, param);

    if (!bl_hw->ipc_env) {
        bl_os_printf("%s: bypassing (restart must have failed)\r\n", __func__);
        bl_os_free(msg);
        RWNX_DBG(RWNX_FN_LEAVE_STR);
        return -EBUSY;
    }

    nonblock = is_non_blocking_msg(msg->id);

    cmd = bl_os_malloc(sizeof(struct bl_cmd));
    if (NULL == cmd) {
        bl_os_free(msg);
        bl_os_printf("%s: failed to allocate mem for cmd, size is %d\r\n", __func__, sizeof(struct bl_cmd));
        return -ENOMEM;
    }
    memset(cmd, 0, sizeof(struct bl_cmd));
    cmd->result  = EINTR;
    cmd->id      = msg->id;
    cmd->reqid   = reqid;
    cmd->a2e_msg = msg;
    cmd->e2a_msg = cfm;
    if (nonblock)
        cmd->flags = RWNX_CMD_FLAG_NONBLOCK;
    if (reqcfm)
        cmd->flags |= RWNX_CMD_FLAG_REQ_CFM;
    ret = bl_hw->cmd_mgr.queue(&bl_hw->cmd_mgr, cmd);

    if (!nonblock) {
        bl_os_free(cmd);
    } else {
        ret = cmd->result;
    }

    RWNX_DBG(RWNX_FN_LEAVE_STR);
    return ret;
}

int bl_send_reset(struct bl_hw *bl_hw)
{
    void *void_param;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* RESET REQ has no parameter */
    void_param = bl_msg_zalloc(MM_RESET_REQ, TASK_MM, DRV_TASK_ID, 0);
    if (!void_param)
        return -ENOMEM;

    return bl_send_msg(bl_hw, void_param, 1, MM_RESET_CFM, NULL);
}

int bl_send_monitor_enable(struct bl_hw *bl_hw, struct mm_monitor_cfm *cfm)
{
    struct mm_monitor_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    req = bl_msg_zalloc(MM_MONITOR_REQ, TASK_MM, DRV_TASK_ID, sizeof(struct mm_monitor_req));
    if (!req)
        return -ENOMEM;

    req->enable = 1;

    return bl_send_msg(bl_hw, req, 1, MM_MONITOR_CFM, cfm);
}

int bl_send_monitor_disable(struct bl_hw *bl_hw, struct mm_monitor_cfm *cfm)
{
    struct mm_monitor_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    req = bl_msg_zalloc(MM_MONITOR_REQ, TASK_MM, DRV_TASK_ID, sizeof(struct mm_monitor_req));
    if (!req)
        return -ENOMEM;

    req->enable = 0;

    return bl_send_msg(bl_hw, req, 1, MM_MONITOR_CFM, cfm);
}

int bl_send_beacon_interval_set(struct bl_hw *bl_hw, struct mm_set_beacon_int_cfm *cfm, uint16_t beacon_int)
{
    struct mm_set_beacon_int_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    req = bl_msg_zalloc(MM_SET_BEACON_INT_REQ, TASK_MM, DRV_TASK_ID, sizeof(struct mm_set_beacon_int_req));
    if (!req)
        return -ENOMEM;

    req->beacon_int = beacon_int;

    return bl_send_msg(bl_hw, req, 1, MM_SET_BEACON_INT_CFM, cfm);
}

//TODO we only support 2.4GHz
int bl_send_monitor_channel_set(struct bl_hw *bl_hw, struct mm_monitor_channel_cfm *cfm, int channel, int use_40Mhz)
{
    struct mm_monitor_channel_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    req = bl_msg_zalloc(MM_MONITOR_CHANNEL_REQ, TASK_MM, DRV_TASK_ID, sizeof(struct mm_monitor_channel_req));
    if (!req) {
        return -ENOMEM;
    }

    req->freq = phy_channel_to_freq(PHY_BAND_2G4, channel);

    return bl_send_msg(bl_hw, req, 1, MM_MONITOR_CHANNEL_CFM, cfm);
}

int bl_send_version_req(struct bl_hw *bl_hw, struct mm_version_cfm *cfm)
{
    int ret;
    void *void_param;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* VERSION REQ has no parameter */
    void_param = bl_msg_zalloc(MM_VERSION_REQ, TASK_MM, DRV_TASK_ID, 0);
    if (!void_param) {
        RWNX_DBG(RWNX_FN_LEAVE_STR);
        return -ENOMEM;
    }
    ret = bl_send_msg(bl_hw, void_param, 1, MM_VERSION_CFM, cfm);
    RWNX_DBG(RWNX_FN_LEAVE_STR);
    return ret;
}

int bl_send_me_config_req(struct bl_hw *bl_hw)
{
    struct me_config_req *req;
    uint8_t *ht_mcs = (uint8_t *)&(bl_hw->ht_cap.mcs);
    int i, ret;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the ME_CONFIG_REQ message */
    req = bl_msg_zalloc(ME_CONFIG_REQ, TASK_ME, DRV_TASK_ID,
                                   sizeof(struct me_config_req));
    if (!req) {
        RWNX_DBG(RWNX_FN_LEAVE_STR);
        return -ENOMEM;
    }

    /* Set parameters for the ME_CONFIG_REQ message */
    bl_os_printf("[ME] HT supp %d, VHT supp %d\r\n", 1, 0);

    req->ht_supp = 1;
    req->vht_supp = 0;
    req->ht_cap.ht_capa_info = cpu_to_le16(bl_hw->ht_cap.cap);

    /*AMPDU MAX Length:  
     * 0x0:8K
     * 0x1:16K
     * 0x2:32K
     * 0x3:64K
     */
    req->ht_cap.a_mpdu_param = 0x3;

    for (i = 0; i < sizeof(bl_hw->ht_cap.mcs); i++) {
        req->ht_cap.mcs_rate[i] = ht_mcs[i];
    }
    req->ht_cap.ht_extended_capa = 0;
    req->ht_cap.tx_beamforming_capa = 0;
    req->ht_cap.asel_capa = 0;

    //TODO talk with firmware guys
#if 0
    req->vht_cap.vht_capa_info = cpu_to_le32(vht_cap->cap);
    req->vht_cap.rx_highest = cpu_to_le16(vht_cap->vht_mcs.rx_highest);
    req->vht_cap.rx_mcs_map = cpu_to_le16(vht_cap->vht_mcs.rx_mcs_map);
    req->vht_cap.tx_highest = cpu_to_le16(vht_cap->vht_mcs.tx_highest);
    req->vht_cap.tx_mcs_map = cpu_to_le16(vht_cap->vht_mcs.tx_mcs_map);
#endif

    req->ps_on = bl_hw->mod_params->ps_on;
    req->tx_lft = bl_hw->mod_params->tx_lft;

    /* Send the ME_CONFIG_REQ message to LMAC FW */
    ret = bl_send_msg(bl_hw, req, 1, ME_CONFIG_CFM, NULL);
    RWNX_DBG(RWNX_FN_LEAVE_STR);
    return ret;
}

static uint8_t passive_scan_flag(uint32_t flags) {
    if (flags & (IEEE80211_CHAN_NO_IR | IEEE80211_CHAN_RADAR))
        return SCAN_PASSIVE_BIT;
    return 0;
}

int bl_send_me_chan_config_req(struct bl_hw *bl_hw)
{
    struct me_chan_config_req *req;
    int i;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the ME_CHAN_CONFIG_REQ message */
    req = bl_msg_zalloc(ME_CHAN_CONFIG_REQ, TASK_ME, DRV_TASK_ID,
                                            sizeof(struct me_chan_config_req));
    if (!req)
        return -ENOMEM;

    req->chan2G4_cnt = 0;
    for (i = 0; i < channel_num_default; i++) {
        req->chan2G4[req->chan2G4_cnt].flags = 0;
        if (channels_default[i].flags & IEEE80211_CHAN_DISABLED)
            req->chan2G4[req->chan2G4_cnt].flags |= SCAN_DISABLED_BIT;
        req->chan2G4[req->chan2G4_cnt].flags |= passive_scan_flag(channels_default[i].flags);
        req->chan2G4[req->chan2G4_cnt].band = NL80211_BAND_2GHZ;
        req->chan2G4[req->chan2G4_cnt].freq = channels_default[i].center_freq;
        req->chan2G4[req->chan2G4_cnt].tx_power = channels_default[i].max_power;
        req->chan2G4_cnt++;
        if (req->chan2G4_cnt == SCAN_CHANNEL_2G4)
            break;
    }

    /* Send the ME_CHAN_CONFIG_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 1, ME_CHAN_CONFIG_CFM, NULL);
}

int bl_send_me_rate_config_req(struct bl_hw *bl_hw, uint8_t sta_idx, uint16_t fixed_rate_cfg)
{
    struct me_rc_set_rate_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the ME_RC_SET_RATE_REQ message */
    req = bl_msg_zalloc(ME_RC_SET_RATE_REQ, TASK_ME, DRV_TASK_ID, sizeof(struct me_rc_set_rate_req));
    if (!req) {
        return -ENOMEM;
    }
    req->sta_idx = sta_idx;
    req->fixed_rate_cfg = fixed_rate_cfg;
    req->power_table_req = 1;

    return bl_send_msg(bl_hw, req, 0, 0, NULL);
}

int bl_send_start(struct bl_hw *bl_hw)
{
    struct mm_start_req *start_req_param;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the START REQ message */
    start_req_param = bl_msg_zalloc(MM_START_REQ, TASK_MM, DRV_TASK_ID,
                                      sizeof(struct mm_start_req));
    if (!start_req_param)
        return -ENOMEM;

    memset(&start_req_param->phy_cfg, 0, sizeof(start_req_param->phy_cfg));
    //XXX magic number
    start_req_param->phy_cfg.parameters[0] = 0x1;
    start_req_param->uapsd_timeout = (u32_l)bl_hw->mod_params->uapsd_timeout;
    start_req_param->lp_clk_accuracy = (u16_l)bl_hw->mod_params->lp_clk_ppm;

    /* Send the START REQ message to LMAC FW */
    return bl_send_msg(bl_hw, start_req_param, 1, MM_START_CFM, NULL);
}

int bl_send_add_if(struct bl_hw *bl_hw, const unsigned char *mac,
                     enum nl80211_iftype iftype, bool p2p, struct mm_add_if_cfm *cfm)
{
    struct mm_add_if_req *add_if_req_param;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the ADD_IF_REQ message */
    add_if_req_param = bl_msg_zalloc(MM_ADD_IF_REQ, TASK_MM, DRV_TASK_ID,
                                       sizeof(struct mm_add_if_req));
    if (!add_if_req_param)
        return -ENOMEM;

    /* Set parameters for the ADD_IF_REQ message */
    memcpy(&(add_if_req_param->addr.array[0]), mac, ETH_ALEN);
    switch (iftype) {
    case NL80211_IFTYPE_P2P_CLIENT:
        add_if_req_param->p2p = true;
        __attribute__((fallthrough));
        // no break
    case NL80211_IFTYPE_STATION:
        add_if_req_param->type = MM_STA;
        break;

    case NL80211_IFTYPE_ADHOC:
        add_if_req_param->type = MM_IBSS;
        break;

    case NL80211_IFTYPE_P2P_GO:
        add_if_req_param->p2p = true;
        __attribute__((fallthrough));
        // no break
    case NL80211_IFTYPE_AP:
        add_if_req_param->type = MM_AP;
        break;
    case NL80211_IFTYPE_MESH_POINT:
        add_if_req_param->type = MM_MESH_POINT;
        break;
    case NL80211_IFTYPE_AP_VLAN:
        return -1;
    default:
        add_if_req_param->type = MM_STA;
        break;
    }

    /* Send the ADD_IF_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, add_if_req_param, 1, MM_ADD_IF_CFM, cfm);
}

int bl_send_remove_if(struct bl_hw *bl_hw, uint8_t inst_nbr)
{
    struct mm_remove_if_req *remove_if_req_param;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    remove_if_req_param = bl_msg_zalloc(MM_REMOVE_IF_REQ, TASK_MM, DRV_TASK_ID, sizeof(struct mm_remove_if_req));
    if (!remove_if_req_param) {
        return -ENOMEM;
    }
    remove_if_req_param->inst_nbr = inst_nbr;

    return bl_send_msg(bl_hw, remove_if_req_param, 1, MM_REMOVE_IF_CFM, NULL);
}

int bl_send_scanu_req(struct bl_hw *bl_hw, struct bl_send_scanu_para *scanu_para)
{
    struct scanu_start_req *req;
    int i, index;
    uint8_t chan_flags = 0;
    const struct ieee80211_channel *chan;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the SCANU_START_REQ message */
    req = bl_msg_zalloc(SCANU_START_REQ, TASK_SCANU, DRV_TASK_ID,
                          sizeof(struct scanu_start_req));
    if (!req) {
        return -ENOMEM;
    }

    /* Set parameters */
    // Always use idx 0, because vif_idx in vif_entry could be 0, leading to probe_rep tx fail
    req->vif_idx = 0;
    if (0 == scanu_para->channel_num) {
        req->chan_cnt = channel_num_default;
    } else {
        req->chan_cnt = scanu_para->channel_num;
    }

    req->ssid_cnt = 1;
    if (scanu_para->ssid != NULL && scanu_para->ssid->length) {
        req->ssid[0].length = scanu_para->ssid->length;
        memcpy(req->ssid[0].array, scanu_para->ssid->array, req->ssid[0].length);
    } else {
        req->ssid[0].length = 0;
        //if specfied ssid, ignore user setting passive mode
        if (req->ssid_cnt == 0 || scanu_para->scan_mode == SCAN_PASSIVE) 
        {
            chan_flags |= SCAN_PASSIVE_BIT;
        }
    }
    memcpy((uint8_t *)&(req->bssid), (uint8_t *)scanu_para->bssid, ETH_ALEN);
    memcpy(&(req->mac), scanu_para->mac, ETH_ALEN);
    req->no_cck = true;//FIXME params? talk with firmware guys

#if 0
    for (i = 0; i < req->ssid_cnt; i++) {
        int j;
        for (j = 0; j < param->ssids[i].ssid_len; j++)
            req->ssid[i].array[j] = param->ssids[i].ssid[j];
        req->ssid[i].length = param->ssids[i].ssid_len;
    }
#endif

    //XXX custom ie can be added
    req->add_ie_len = 0;
    req->add_ies = 0;

    for (i = 0; i < req->chan_cnt; i++) {
        index = (channel_num_default == req->chan_cnt) ? i : (scanu_para->channels[i] - 1);
        chan = &(channels_default[index]);

        req->chan[i].band = chan->band;
        req->chan[i].freq = chan->center_freq;
        req->chan[i].flags = chan_flags | passive_scan_flag(chan->flags);
        req->chan[i].tx_power = chan->max_reg_power;
    }

    req->duration_scan = scanu_para->duration_scan;

    /* Send the SCANU_START_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 0, 0, NULL);
}

int bl_send_scanu_raw_send(struct bl_hw *bl_hw, uint8_t *pkt, int len)
{
    struct scanu_raw_send_req *req;
    struct scanu_raw_send_cfm cfm;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the SCANU_RAW_SEND_REQ message */
    req = bl_msg_zalloc(SCANU_RAW_SEND_REQ, TASK_SCANU, DRV_TASK_ID, sizeof(struct scanu_raw_send_req));
    if (!req) {
        return -ENOMEM;
    }

    /* Set parameters */
    req->pkt = pkt;
    req->len = len;

    /* Send the SCANU_RAW_SEND_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 1, SCANU_RAW_SEND_CFM, &cfm);
}

static inline bool use_pairwise_key(struct cfg80211_crypto_settings *crypto)
{
    if ((crypto->cipher_group ==  WLAN_CIPHER_SUITE_WEP40) ||
        (crypto->cipher_group ==  WLAN_CIPHER_SUITE_WEP104))
        return false;

    return true;
}

int bl_send_sm_connect_req(struct bl_hw *bl_hw, struct cfg80211_connect_params *sme, struct sm_connect_cfm *cfm)
{
    struct sm_connect_req *req;
    int i;
    u32_l flags = sme->flags;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the SM_CONNECT_REQ message */
    req = bl_msg_zalloc(SM_CONNECT_REQ, TASK_SM, DRV_TASK_ID,
                                   sizeof(struct sm_connect_req));
    if (!req)
        return -ENOMEM;

#if 0 // useless
    /* Set parameters for the SM_CONNECT_REQ message */
    if (sme->crypto.n_ciphers_pairwise &&
        ((sme->crypto.ciphers_pairwise[0] == WLAN_CIPHER_SUITE_WEP40) ||
         (sme->crypto.ciphers_pairwise[0] == WLAN_CIPHER_SUITE_TKIP) ||
         (sme->crypto.ciphers_pairwise[0] == WLAN_CIPHER_SUITE_WEP104)))
        flags |= DISABLE_HT;

    if (sme->crypto.control_port)
        flags |= CONTROL_PORT_HOST;

    if (sme->crypto.control_port_no_encrypt)
        flags |= CONTROL_PORT_NO_ENC;

    if (use_pairwise_key(&sme->crypto))
        flags |= WPA_WPA2_IN_USE;

    if (sme->mfp == NL80211_MFP_REQUIRED)
        flags |= MFP_IN_USE;

    if (sme->crypto.control_port_ethertype)
        req->ctrl_port_ethertype = sme->crypto.control_port_ethertype;
    else
        req->ctrl_port_ethertype = ETH_P_PAE;
#endif
    req->ctrl_port_ethertype = ETH_P_PAE;

    if (sme->bssid && !MAC_ADDR_CMP(sme->bssid, mac_addr_bcst.array) && !MAC_ADDR_CMP(sme->bssid, mac_addr_zero.array)) {
        for (i=0;i<ETH_ALEN;i++)
            req->bssid.array[i] = sme->bssid[i];
    }
    else
        req->bssid = mac_addr_bcst;
    req->vif_idx = bl_hw->vif_index_sta;
    if (sme->channel.center_freq) {
        req->chan.band = sme->channel.band;
        req->chan.freq = sme->channel.center_freq;
        req->chan.flags = passive_scan_flag(sme->channel.flags);
    } else {
        req->chan.freq = (u16_l)-1;
    }
    for (i = 0; i < sme->ssid_len; i++)
        req->ssid.array[i] = sme->ssid[i];
    req->ssid.length = sme->ssid_len;
    req->flags = flags;
#if 0 // useless
    if (WARN_ON(sme->ie_len > sizeof(req->ie_buf)))
        return -EINVAL;
    if (sme->ie_len)
        memcpy(req->ie_buf, sme->ie, sme->ie_len);
    req->ie_len = sme->ie_len;
#endif
    req->listen_interval = bl_mod_params.listen_itv;
    req->dont_wait_bcmc = !bl_mod_params.listen_bcmc;

    /* Set auth_type */
    if (sme->auth_type == NL80211_AUTHTYPE_AUTOMATIC)
        req->auth_type = NL80211_AUTHTYPE_OPEN_SYSTEM;
    else
        req->auth_type = sme->auth_type;

    /* Set UAPSD queues */
    req->uapsd_queues = bl_mod_params.uapsd_queues;
    req->is_supplicant_enabled = 1;
    if (sme->key_len) {
        memcpy(req->phrase, sme->key, sme->key_len);
    }
    if (sme->pmk_len) {
        memcpy(req->phrase_pmk, sme->pmk, sme->pmk_len);
    }

    /* Send the SM_CONNECT_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 1, SM_CONNECT_CFM, cfm);
}

int bl_send_sm_disconnect_req(struct bl_hw *bl_hw)
{
    struct sm_disconnect_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the SM_DISCONNECT_REQ message */
    req = bl_msg_zalloc(SM_DISCONNECT_REQ, TASK_SM, DRV_TASK_ID, sizeof(struct sm_disconnect_req));
    if (!req) {
        return -ENOMEM;
    }

    /* Set parameters for the SM_DISCONNECT_REQ message */
    req->vif_idx = bl_hw->vif_index_sta;

    /* Send the SM_DISCONNECT_REQ message to LMAC FW */
    //return bl_send_msg(bl_hw, req, 1, SM_DISCONNECT_IND, NULL);
    return bl_send_msg(bl_hw, req, 1, SM_DISCONNECT_CFM, NULL);
}

int bl_send_sm_connect_abort_req(struct bl_hw *bl_hw, struct sm_connect_abort_cfm *cfm)
{
    struct sm_connect_abort_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    req = bl_msg_zalloc(SM_CONNECT_ABORT_REQ, TASK_SM, DRV_TASK_ID, sizeof(struct sm_connect_abort_req));
    if (!req) {
        return -ENOMEM;
    }
    /* Set parameters for the SM_CONNECT_ABORT_REQ message */
    req->vif_idx = bl_hw->vif_index_sta;

    return bl_send_msg(bl_hw, req, 1, SM_CONNECT_ABORT_CFM, cfm);
}

int bl_send_mm_powersaving_req(struct bl_hw *bl_hw, int mode)
{
    struct mm_set_ps_mode_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the MM_SET_PS_MODE_REQ message */
    req = bl_msg_zalloc(MM_SET_PS_MODE_REQ, TASK_MM, DRV_TASK_ID, sizeof(struct mm_set_ps_mode_req));
    if (!req) {
        return -ENOMEM;
    }

    /* Set parameters for the MM_SET_PS_MODE_REQ message */
    req->new_state = mode;

    /* Send the MM_SET_PS_MODE_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 1, MM_SET_PS_MODE_CFM, NULL);
}

int bl_send_mm_denoise_req(struct bl_hw *bl_hw, int mode)
{
    struct mm_set_denoise_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the MM_DENOISE_REQ message */
    req = bl_msg_zalloc(MM_DENOISE_REQ, TASK_MM, DRV_TASK_ID, sizeof(struct mm_set_denoise_req));
    if (!req) {
        return -ENOMEM;
    }

    /* Set parameters for the MM_SET_PS_MODE_REQ message */
    req->denoise_mode = mode;

    /* Send the MM_SET_PS_MODE_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 1, MM_SET_PS_MODE_CFM, NULL);
}

/* Country IE definition
MAC_COUNTRY_2G4_USA       {7,  6, 'U', 'S', 32,  1, 11, 20} // X'10' FCC
MAC_COUNTRY_2G4_CANADA    {7,  6, 'C', 'A', 32,  1, 11, 20} // X'20' DOC/IC
MAC_COUNTRY_2G4_EUROPE    {7,  6, 'E', 'U', 32,  1, 13, 20} // X'30' ETSI
MAC_COUNTRY_2G4_SPAIN     {7,  6, 'S', 'P', 32, 10,  2, 20} // X'31'
MAC_COUNTRY_2G4_FRANCE    {7,  6, 'F', 'R', 32, 10,  4, 20} // X'32'
MAC_COUNTRY_2G4_JAPAN     {7,  6, 'J', 'P', 32, 14,  1, 20} // X'40'
MAC_COUNTRY_2G4_CHINA     {7,  6, 'C', 'N', 32,  1, 13, 20} // X'50'
*/

static int _fill_country_code_ie(uint8_t *buf, uint8_t buf_len_max)
{
    if (NULL == country_default || NULL == channels_default) {
        return 0;
    }

    // Tag: Country Informance
    buf[0] = 7;
    // Tag lenth
    buf[1] = 6;
    //Country Code
    buf[2] = country_default->code[0];
    buf[3] = country_default->code[1];
    //Environment
    buf[4] = 32;//Any
    //First Channel
    buf[5] = 1;
    //Channel Num
    buf[6] = buf[5] - 1 + country_default ->channel_num;
    //Max power
    buf[7] = channels_default->max_power;

    return 8;
}

int bl_send_apm_start_req(struct bl_hw *bl_hw, struct apm_start_cfm *cfm, char *ssid, char *password, int channel, uint8_t vif_index, uint8_t hidden_ssid, uint16_t bcn_int)
{
    struct apm_start_req *req;
    uint8_t rate[] = {0x82,0x84,0x8b,0x96,0x12,0x24,0x48,0x6c,0x0c,0x18,0x30,0x60};


    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the APM_START_REQ message */
    req = bl_msg_zalloc(APM_START_REQ, TASK_APM, DRV_TASK_ID, sizeof(struct apm_start_req));
    if (!req) {
        return -ENOMEM;
    }

    req->chan.band = NL80211_BAND_2GHZ;
    req->chan.freq = phy_channel_to_freq(req->chan.band, channel);
    req->chan.flags = 0;
    req->chan.tx_power = 0;

    /* Set parameters for the APM_START_REQ message */
    req->center_freq1 = req->chan.freq;
    req->center_freq2 = 0;
    req->ch_width = PHY_CHNL_BW_20;
    req->hidden_ssid = hidden_ssid;
    req->bcn_addr = 0;
    req->bcn_len = 0;
    req->tim_oft = 0;
    req->bcn_int = bcn_int;
    req->flags = 0x08;
    //req->ctrl_port_ethertype = ETH_P_PAE;
    req->ctrl_port_ethertype = 0x8e88;
    req->tim_len = 0x6;
    req->vif_idx = vif_index;

    /*added for EMBEDED*/
#if 0
    /// Enable APM Embedded
    bool apm_emb_enabled;
    /// rate set
    struct mac_rateset rate_set;
    /// Beacon dtim period
    uint8_t beacon_period;
    /// Qos is supported
    uint8_t qos_supported;
    /// SSID of the AP
    struct mac_ssid ssid;
    /// AP Security type
    uint8_t ap_sec_type;
    /// AP Passphrase
    uint8_t phrase[MAX_PSK_PASS_PHRASE_LEN];
#else
    if (strlen(password)) {
        req->ap_sec_type = 1;
    } else {
        req->ap_sec_type = 0;
    }
    req->apm_emb_enabled = 1;
    memcpy(req->ssid.array, ssid, strlen(ssid));//FIXME potential buffer overflow
    memcpy(req->phrase, password, strlen(password));//FIXME potential buffer overflow
    req->ssid.length = strlen(ssid);
    req->rate_set.length = 12;
    memcpy(req->rate_set.array, rate, req->rate_set.length);
    req->beacon_period = 0x1; //force AP DTIM period
    req->qos_supported = 1;
#endif
    req->bcn_buf_len = _fill_country_code_ie(req->bcn_buf, sizeof(req->bcn_buf));

    /* Send the APM_START_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 1, APM_START_CFM, cfm);
}

int bl_send_apm_stop_req(struct bl_hw *bl_hw, uint8_t vif_idx)
{
    struct apm_stop_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the APM_STOP_REQ message */
    req = bl_msg_zalloc(APM_STOP_REQ, TASK_APM, DRV_TASK_ID, sizeof(struct apm_stop_req));
    if (!req) {
        return -ENOMEM;
    }

    /* Set parameters for the APM_STOP_REQ message */
    req->vif_idx = vif_idx;

    /* Send the APM_STOP_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 1, APM_STOP_CFM, NULL);
}

int bl_send_apm_sta_del_req(struct bl_hw *bl_hw, struct apm_sta_del_cfm *cfm, uint8_t sta_idx, uint8_t vif_idx)
{
    struct apm_sta_del_req *req;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Build the APM_STOP_REQ message */
    req = bl_msg_zalloc(APM_STA_DEL_REQ, TASK_APM, DRV_TASK_ID, sizeof(struct apm_sta_del_req));
    if (!req) {
        return -ENOMEM;
    }

    /* Set parameters for the APM_STA_DEL_REQ message */
    req->vif_idx = vif_idx;
    req->sta_idx = sta_idx;

    /* Send the APM_STA_DEL_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 1, APM_STA_DEL_CFM, cfm);
}

int bl_send_apm_conf_max_sta_req(struct bl_hw *bl_hw, uint8_t max_sta_supported)
{
    struct apm_conf_max_sta_req *req;

    /* Build the APM_STOP_REQ message */
    req = bl_msg_zalloc(APM_CONF_MAX_STA_REQ, TASK_APM, DRV_TASK_ID, sizeof(struct apm_conf_max_sta_req));
    if (!req) {
        return -ENOMEM;
    }

    /* Set parameters for the APM_STOP_REQ message */
    req->max_sta_supported = max_sta_supported;

    /* Send the APM_STOP_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 1, APM_CONF_MAX_STA_CFM, NULL);
}

int bl_send_cfg_task_req(struct bl_hw *bl_hw, uint32_t ops, uint32_t task, uint32_t element, uint32_t type, void *arg1, void *arg2)
{
    struct cfg_start_req *req;
#define ENTRY_BUF_SIZE      (8)

    /* Build the APM_STOP_REQ message */
    //FIXME static allocated size
    req = bl_msg_zalloc(CFG_START_REQ, TASK_CFG, DRV_TASK_ID, sizeof(struct cfg_start_req) + 32);
    if (!req) {
        return -ENOMEM;
    }

    /* Set parameters for the APM_STOP_REQ message */
    req->ops = ops;
    switch (req->ops) {
        case CFG_ELEMENT_TYPE_OPS_SET:
        {
            req->u.set[0].task = task;
            req->u.set[0].element = element;
            req->u.set[0].type = type;
            req->u.set[0].length = utils_tlv_bl_pack_auto(
                req->u.set[0].buf,
                ENTRY_BUF_SIZE,
                type, 
                arg1
            );
        }
        break;
        case CFG_ELEMENT_TYPE_OPS_GET:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_OPS_RESET:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_OPS_DUMP_DEBUG:
        {
            req->u.set[0].task = task;
            req->u.set[0].element = element;
            req->u.set[0].length = 0;
        }
        break;
        default:
        {
            /*empty here*/
            assert(0);
        }
    }

    /* Send the APM_STOP_REQ message to LMAC FW */
    return bl_send_msg(bl_hw, req, 1, CFG_START_CFM, NULL);
}

int bl_send_channel_set_req(struct bl_hw *bl_hw, int channel)
{
    struct mm_set_channel_req *param;
    struct mm_set_channel_cfm cfm;


    RWNX_DBG(RWNX_FN_ENTRY_STR);

    param = bl_msg_zalloc(MM_SET_CHANNEL_REQ, TASK_MM, DRV_TASK_ID, sizeof(struct mm_set_channel_req));
    if (!param) {
        return -ENOMEM;
    }
    memset(&cfm, 0, sizeof(struct mm_set_channel_cfm));

    param->band = PHY_BAND_2G4;
    param->type = PHY_CHNL_BW_20;
    param->prim20_freq = phy_channel_to_freq(param->band, channel);
    param->center1_freq = phy_channel_to_freq(param->band, channel);//useless when bandwidth bigger than 20MHZ?
    param->center2_freq = phy_channel_to_freq(param->band, channel);//useless when bandwidth bigger than 20MHZ? 
    param->index = 0;
    param->tx_power = 15;//FIXME which value should be tx_power set?

    return bl_send_msg(bl_hw, param, 1, MM_SET_CHANNEL_CFM, &cfm);
}

