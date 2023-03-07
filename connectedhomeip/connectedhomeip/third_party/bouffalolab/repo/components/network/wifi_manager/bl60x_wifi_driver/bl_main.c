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
#include <stdio.h>
#include <string.h>

#include <lwip/opt.h>
#include <lwip/def.h>
#include <lwip/mem.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>
#include <ethernetif.h>
#include <bl_wifi.h>

#include "bl_main.h"
#include "bl_defs.h"
#include "bl_utils.h"
#include "bl_platform.h"
#include "bl_msg_tx.h"
#include "bl_irqs.h"
#include "bl_tx.h"

#include <bl_os_private.h>
#define USER_UNUSED(a) ((void)(a))
#define RWNX_PRINT_CFM_ERR(req) \
        bl_os_printf("%s: Status Error(%d)\n", #req, (&req##_cfm)->status)

struct bl_hw wifi_hw;

int bl_cfg80211_connect(struct bl_hw *bl_hw, struct cfg80211_connect_params *sme);

static void bl_set_vers(struct mm_version_cfm *version_cfm_ptr)
{
    u32 vers = version_cfm_ptr->version_lmac;

    USER_UNUSED(vers);
    RWNX_DBG(RWNX_FN_ENTRY_STR);

    bl_os_printf("[version] lmac %u.%u.%u.%u\r\n",
        (unsigned int)((vers >> 24) & 0xFF),
        (unsigned int)((vers >> 16) & 0xFF),
        (unsigned int)((vers >>  8) & 0xFF),
        (unsigned int)((vers >>  0) & 0xFF)
    );
    bl_os_printf("[version] version_machw_1 %08X\r\n", (unsigned int)version_cfm_ptr->version_machw_1);
    bl_os_printf("[version] version_machw_2 %08X\r\n", (unsigned int)version_cfm_ptr->version_machw_2);
    bl_os_printf("[version] version_phy_1 %08X\r\n", (unsigned int)version_cfm_ptr->version_phy_1);
    bl_os_printf("[version] version_phy_2 %08X\r\n", (unsigned int)version_cfm_ptr->version_phy_2);
    bl_os_printf("[version] features %08X\r\n", (unsigned int)version_cfm_ptr->features);

    RWNX_DBG(RWNX_FN_LEAVE_STR);
}


/**
 ****************************************************************************************
 *
 * @file bl_main.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

/**
 * int (*ndo_open)(struct net_device *dev);
 *     This function is called when network device transistions to the up
 *     state.
 *
 * - Start FW if this is the first interface opened
 * - Add interface at fw level
 */
int bl_open(struct bl_hw *bl_hw)
{
#if 0
    struct mm_add_if_cfm add_if_cfm;
#if 0
    struct apm_start_cfm start_ap_cfm;
#endif
    int error = 0;
    const unsigned char mac[] = {0x00, 0x50, 0x43, 0xC9, 0x00, 0x01};
    struct cfg80211_connect_params sme;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    // Check if it is the first opened VIF
    if (bl_hw->vif_started == 0)
    {
        // Start the FW
       os_printf("[WF] MM_START_REQ Sending\r\n");
       error = bl_send_start(bl_hw);
       os_printf("[WF] MM_START_REQ Done\r\n");
       if (error) {
           return error;
       }

       /* Device is now started */
       bl_hw->drv_flags |= (1 << RWNX_DEV_STARTED);
    }

#if 0
    /* Forward the information to the LMAC,
     *     p2p value not used in FMAC configuration, iftype is sufficient */
    os_printf("[WF] MM_ADD_IF_REQ Sending\r\n");
    error = bl_send_add_if(bl_hw, mac, NL80211_IFTYPE_AP, false, &add_if_cfm);
    os_printf("[WF] MM_ADD_IF_REQ Done\r\n");
    if (error) {
        return error;
    }

    if (add_if_cfm.status != 0) {
        RWNX_PRINT_CFM_ERR(add_if);
        return -EIO;
    }

    /* Save the index retrieved from LMAC */
    bl_hw->vif_index = add_if_cfm.inst_nbr;

    os_printf("[WF] vif_index from LAMC is %d\r\n", bl_hw->vif_index);

    os_printf("[WF] APM_START_REQ Sending\r\n");
    bl_send_apm_start_req(bl_hw, &start_ap_cfm);
    os_printf("[WF] APM_START_REQ Done\r\n");
#else
    /* Forward the information to the LMAC,
     *     p2p value not used in FMAC configuration, iftype is sufficient */
    os_printf("[WF] MM_ADD_IF_REQ Sending\r\n");
    error = bl_send_add_if(bl_hw, mac, NL80211_IFTYPE_STATION, false, &add_if_cfm);
    os_printf("[WF] MM_ADD_IF_REQ Done\r\n");
    if (error) {
        return error;
    }

    if (add_if_cfm.status != 0) {
        RWNX_PRINT_CFM_ERR(add_if);
        return -EIO;
    }

    /* Save the index retrieved from LMAC */
    bl_hw->vif_index = add_if_cfm.inst_nbr;

#undef os_printf
    os_printf("[WF] vif_index from LAMC is %d\r\n", bl_hw->vif_index);
#define os_printf(...) do {} while(0)

#if 0
    os_printf("[WF] SM_CONNECT_REQ Sending\r\n");
    memset(&sme, 0, sizeof(struct cfg80211_connect_params));
    sme.crypto.n_ciphers_pairwise = 0;
    sme.ssid_len = 7;
    sme.ssid = "LEDE_4G";
    sme.auth_type = NL80211_AUTHTYPE_AUTOMATIC;
    bl_cfg80211_connect(bl_hw, &sme);
    os_printf("[WF] SM_CONNECT_REQ Done\r\n");
#endif
#endif

    return error;
#else
    return 0;
#endif
}

int bl_main_connect(const uint8_t* ssid, int ssid_len, const uint8_t *psk, int psk_len, const uint8_t *pmk, int pmk_len, const uint8_t *mac, const uint8_t band, const uint16_t freq, const uint32_t flags)
{
    struct cfg80211_connect_params sme;

    memset(&sme, 0, sizeof(struct cfg80211_connect_params));
    sme.crypto.n_ciphers_pairwise = 0;
    sme.ssid_len = ssid_len;
    sme.ssid = ssid;
    sme.auth_type = NL80211_AUTHTYPE_AUTOMATIC;
    sme.key = psk;
    sme.key_len = psk_len;
    sme.pmk = pmk;
    sme.pmk_len = pmk_len;
    sme.flags = flags;

    if (mac){
        sme.bssid = mac;
    }

    if (freq > 0) {
        sme.channel.center_freq = freq;
        sme.channel.band = band;
        sme.channel.flags = 0;
    }

    bl_cfg80211_connect(&wifi_hw, &sme);

    return 0;
}

int bl_main_disconnect()
{
    bl_send_sm_disconnect_req(&wifi_hw);
    return 0;
}

int bl_main_powersaving(int mode)
{
    return bl_send_mm_powersaving_req(&wifi_hw, mode);
}

int bl_main_denoise(int mode)
{
    return bl_send_mm_denoise_req(&wifi_hw, mode);
}

int bl_main_monitor()
{
    struct mm_monitor_cfm cfm;

    memset(&cfm, 0, sizeof(cfm));
    bl_send_monitor_enable(&wifi_hw, &cfm);
    return 0;
}

int bl_main_monitor_disable()
{
    struct mm_monitor_cfm cfm;

    memset(&cfm, 0, sizeof(cfm));
    bl_send_monitor_disable(&wifi_hw, &cfm);
    return 0;
}

int bl_main_phy_up()
{
    int error = 0;

    error = bl_send_start(&wifi_hw);
    if (error) {
        return -1;
    }

    return 0;
}

int bl_main_channel_set(int channel)
{
    bl_send_channel_set_req(&wifi_hw, channel);

    return 0;
}

int bl_main_monitor_channel_set(int channel, int use_40MHZ)
{
    struct mm_monitor_channel_cfm cfm;

    bl_send_monitor_channel_set(&wifi_hw, &cfm, channel, use_40MHZ);

    return 0;
}

int bl_main_beacon_interval_set(uint16_t beacon_int)
{
    struct mm_set_beacon_int_cfm cfm;

    bl_send_beacon_interval_set(&wifi_hw, &cfm, beacon_int);

    return 0;
}

int bl_main_if_remove(uint8_t vif_index)
{
    bl_os_printf("[WF] MM_REMOVE_IF_REQ Sending with vif_index %u...\r\n", vif_index);
    bl_send_remove_if(&wifi_hw, vif_index);
    bl_os_printf("[WF] MM_REMOVE_IF_REQ Done\r\n");
    return 0;
}

int bl_main_raw_send(uint8_t *pkt , int len)
{
    return bl_send_scanu_raw_send(&wifi_hw, pkt, len);
}

int bl_main_rate_config(uint8_t sta_idx, uint16_t fixed_rate_cfg)
{
    return bl_send_me_rate_config_req(&wifi_hw, sta_idx, fixed_rate_cfg);
}

int bl_main_set_country_code(char *country_code)
{
    bl_os_log_info("%s: country code: %s\r\n", __func__, country_code);
    bl_msg_update_channel_cfg((const char *)country_code);
    bl_send_me_chan_config_req(&wifi_hw);

    return 0;
}

int bl_main_get_channel_nums()
{
    return bl_msg_get_channel_nums();
}

int bl_main_if_add(int is_sta, struct netif *netif, uint8_t *vif_index)
{
    struct mm_add_if_cfm add_if_cfm;
    int error = 0;

    bl_os_printf("[WF] MM_ADD_IF_REQ Sending: %s\r\n", is_sta ? "STA" : "AP");
    error = bl_send_add_if(
            &wifi_hw,
            netif->hwaddr,
            is_sta ? NL80211_IFTYPE_STATION : NL80211_IFTYPE_AP,
            false,
            &add_if_cfm
    );
    bl_os_printf("[WF] MM_ADD_IF_REQ Done\r\n");
    if (error) {
        return error;
    }

    if (add_if_cfm.status != 0) {
        RWNX_PRINT_CFM_ERR(add_if);
        return -EIO;
    }
    /* Save the index retrieved from LMAC */
    if (is_sta) {
        wifi_hw.vif_index_sta = add_if_cfm.inst_nbr;
    } else {
        wifi_hw.vif_index_ap = add_if_cfm.inst_nbr;
    }
    *vif_index = add_if_cfm.inst_nbr;

    bl_os_printf("[WF] vif_index from LAMC is %d\r\n", *vif_index);
    wifi_hw.vif_table[add_if_cfm.inst_nbr].dev = netif;
    wifi_hw.vif_table[add_if_cfm.inst_nbr].up = 1;

    return error;
}

int bl_main_apm_start(char *ssid, char *password, int channel, uint8_t vif_index, uint8_t hidden_ssid, uint16_t bcn_int)
{
    int error = 0;
    struct apm_start_cfm start_ap_cfm;

    memset(&start_ap_cfm, 0, sizeof(start_ap_cfm));
    bl_os_printf("[WF] APM_START_REQ Sending with vif_index %u\r\n", vif_index);
    error = bl_send_apm_start_req(&wifi_hw, &start_ap_cfm, ssid, password, channel, vif_index, hidden_ssid, bcn_int);
    bl_os_printf("[WF] APM_START_REQ Done\r\n");
    bl_os_printf("[WF] status is %02X\r\n", start_ap_cfm.status);
    bl_os_printf("[WF] vif_idx is %02X\r\n", start_ap_cfm.vif_idx);
    bl_os_printf("[WF] ch_idx is %02X\r\n", start_ap_cfm.ch_idx);
    bl_os_printf("[WF] bcmc_idx is %02X\r\n", start_ap_cfm.bcmc_idx);
    wifi_hw.ap_bcmc_idx = start_ap_cfm.bcmc_idx;

    return error;
}

int bl_main_apm_stop(uint8_t vif_index)
{
    int error = 0;

    bl_os_printf("[WF] APM_STOP_REQ Sending with vif_index %u\r\n", vif_index);
    error = bl_send_apm_stop_req(&wifi_hw, vif_index);
    bl_os_printf("[WF] APM_STOP_REQ Done\r\n");

    return error;
}

int bl_main_apm_sta_cnt_get(uint8_t *sta_cnt)
{
    struct bl_hw *bl_hw = &wifi_hw;
    uint8_t cnt = 0, i;
    struct bl_sta *sta;
    uint8_t total_sta_cnt = sizeof(bl_hw->sta_table)/sizeof(bl_hw->sta_table[0]);

    for (i = 0; i < total_sta_cnt; i++) {
        sta = &(bl_hw->sta_table[i]);
        if (0 == sta->is_used) {
            /*empty entry*/
            continue;
        }
        cnt++;
    }
    (*sta_cnt) = total_sta_cnt;
    bl_os_log_info("Max limit sta cnt = %u, valid sta cnt = %u\r\n", total_sta_cnt, cnt);
    return 0;
}

int bl_main_apm_sta_info_get(struct wifi_apm_sta_info *apm_sta_info, uint8_t idx)
{
    struct bl_hw *bl_hw = &wifi_hw;
    struct bl_sta *sta;

    sta = &(bl_hw->sta_table[idx]);
    if (0 == sta->is_used) {
        /*empty entry*/
        return 0;
    }
    apm_sta_info->sta_idx = sta->sta_idx;
    apm_sta_info->is_used = sta->is_used;
    apm_sta_info->rssi = sta->rssi;
    apm_sta_info->tsflo = sta->tsflo;
    apm_sta_info->tsfhi = sta->tsfhi;
    apm_sta_info->data_rate = sta->data_rate;
    memcpy(apm_sta_info->sta_mac, sta->sta_addr.array, 6);

    return 0;
}

int bl_main_apm_sta_delete(uint8_t sta_idx)
{
    struct bl_hw *bl_hw = &wifi_hw;
    struct bl_sta *sta;
    struct apm_sta_del_cfm sta_del_cfm;
    uint8_t vif_idx = 0;

    sta = &(bl_hw->sta_table[sta_idx]);
    if (sta == NULL)
        return -1;

    memset(&sta_del_cfm, 0, sizeof(struct apm_sta_del_cfm));
    vif_idx = sta->vif_idx;
    bl_os_printf("[WF] APM_STA_DEL_REQ: sta_idx = %u, vif_idx = %u\r\n", sta_idx, vif_idx);

    bl_send_apm_sta_del_req(bl_hw, &sta_del_cfm, sta_idx, vif_idx);
    if (sta_del_cfm.status != 0) {
        bl_os_log_info("del sta failure, cfm status = 0x%x\r\n", sta_del_cfm.status);
        return -1;
    }

    memset(sta, 0, sizeof(struct bl_sta));
    return 0;
}

int bl_main_apm_remove_all_sta()
{
    struct bl_hw *bl_hw = &wifi_hw;
    struct bl_sta *sta;
    uint8_t total_sta_cnt = sizeof(bl_hw->sta_table)/sizeof(bl_hw->sta_table[0]);
    uint8_t i;

    for (i = 0; i < total_sta_cnt; i++) {
        sta = &(bl_hw->sta_table[i]);
        if (1 == sta->is_used) {
            bl_os_log_info("del sta[%u]\r\n", i);
            bl_main_apm_sta_delete(i);
        }
    }
    return 0;
}

int bl_main_conf_max_sta(uint8_t max_sta_supported)
{
    return bl_send_apm_conf_max_sta_req(&wifi_hw, max_sta_supported);
}

int bl_main_cfg_task_req(uint32_t ops, uint32_t task, uint32_t element, uint32_t type, void *arg1, void *arg2)
{
    return bl_send_cfg_task_req(&wifi_hw, ops, task, element, type, arg1, arg2);
}

int bl_main_scan(struct netif *netif, uint16_t *fixed_channels, uint16_t channel_num, struct mac_addr *bssid, struct mac_ssid *ssid, uint8_t scan_mode, uint32_t duration_scan)
{
    struct bl_send_scanu_para scanu_para;

    scanu_para.channels = fixed_channels;
    scanu_para.channel_num = channel_num;
    scanu_para.bssid = bssid;
    scanu_para.ssid = ssid;
    scanu_para.mac = netif->hwaddr;
    scanu_para.scan_mode = scan_mode;
    scanu_para.duration_scan = duration_scan;

    if (0 == channel_num) {
        scanu_para.channels = NULL;
        scanu_para.channel_num = 0;
        bl_send_scanu_req(&wifi_hw, &scanu_para);
    } else {
        if (bl_get_fixed_channels_is_valid(fixed_channels, channel_num)) {
            bl_send_scanu_req(&wifi_hw, &scanu_para);
        } else {
            bl_os_printf("---->unvalid channel");
        }
    }
    return 0;
}

int bl_main_connect_abort(uint8_t *status)
{
    struct sm_connect_abort_cfm connect_abort_cfm = {};
    bl_send_sm_connect_abort_req(&wifi_hw, &connect_abort_cfm);
    *status = connect_abort_cfm.status;
    return 0;
}

static int cfg80211_init(struct bl_hw *bl_hw)
{
    int ret = 0;
    struct mm_version_cfm version_cfm = {};

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    INIT_LIST_HEAD(&bl_hw->vifs);

    bl_hw->mod_params = &bl_mod_params;

    ret = bl_platform_on(bl_hw);
    if (ret) {
        bl_os_printf("bl_platform_on Error\r\n");
        goto err_out;
    }

    ipc_host_enable_irq(bl_hw->ipc_env, IPC_IRQ_E2A_ALL);
    bl_wifi_enable_irq();

    /* Reset FW */
    ret = bl_send_reset(bl_hw);
    if (ret) {
        bl_os_printf("bl_send_reset Error\r\n");
        goto err_out;
    }
    bl_os_msleep(5);
    ret = bl_send_version_req(bl_hw, &version_cfm);
    if (ret) {
        goto err_out;
    }
    bl_set_vers(&version_cfm);
    ret = bl_handle_dynparams(bl_hw);
    if (ret) {
        bl_os_printf("bl_handle_dynparams Error\r\n");
        goto err_out;
    }

    /* Set parameters to firmware */
    bl_send_me_config_req(bl_hw);

    /* Set channel parameters to firmware (must be done after WiPHY registration) */
    bl_send_me_chan_config_req(bl_hw);


err_out:
    RWNX_DBG(RWNX_FN_LEAVE_STR);
    return ret;
}

int bl_cfg80211_connect(struct bl_hw *bl_hw, struct cfg80211_connect_params *sme)
{
    struct sm_connect_cfm sm_connect_cfm;
    int error = 0;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    /* Forward the information to the LMAC */
    error = bl_send_sm_connect_req(bl_hw, sme, &sm_connect_cfm);
    if (error) {
        return error;
    }

    // Check the status
    switch (sm_connect_cfm.status)
    {
        case CO_OK:
            error = 0;
            break;
        case CO_BUSY:
            error = -EINPROGRESS;
            break;
        case CO_OP_IN_PROGRESS:
            error = -EALREADY;
            break;
        default:
            error = -EIO;
            break;
    }
    RWNX_DBG(RWNX_FN_LEAVE_STR);

    return error;
}

int bl_cfg80211_disconnect(struct bl_hw *bl_hw)
{
    RWNX_DBG(RWNX_FN_ENTRY_STR);

    return bl_send_sm_disconnect_req(bl_hw);
}

void bl_main_event_handle()
{
    bl_irq_bottomhalf(&wifi_hw);
    bl_tx_try_flush();
}

void bl_main_lowlevel_init()
{
    /*Call IRQ init here, since GroupEvent is used here*/
    bl_irqs_init(&wifi_hw);
}


int bl_main_tx_still_free(void)
{
    return ipc_host_txdesc_left(wifi_hw.ipc_env, 0, 0);
}

int bl_main_rtthread_start(struct bl_hw **bl_hw)
{
    int ret;

    /*start the bl606 wifi service*/
    bl_main_lowlevel_init();

    /*startup wifi firmware*/
    *bl_hw = &wifi_hw;
    ret = cfg80211_init(&wifi_hw);
    //TODO check ret from cfg80211_init
    ret = bl_open(*bl_hw);
    return ret;
}