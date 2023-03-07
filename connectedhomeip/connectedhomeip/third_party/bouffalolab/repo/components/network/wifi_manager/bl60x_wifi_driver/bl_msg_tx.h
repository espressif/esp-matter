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
#ifndef __RWNX_MSG_TX_H__
#define __RWNX_MSG_TX_H__
#include "bl_defs.h"

struct bl_send_scanu_para {
    uint16_t *channels;
    uint16_t channel_num;
    struct mac_addr *bssid;
    struct mac_ssid *ssid;
    uint8_t *mac;
    uint8_t scan_mode;
    uint32_t duration_scan;
};

int bl_send_reset(struct bl_hw *bl_hw);
int bl_send_monitor_enable(struct bl_hw *bl_hw, struct mm_monitor_cfm *cfm);
int bl_send_monitor_disable(struct bl_hw *bl_hw, struct mm_monitor_cfm *cfm);
/*
 *  use_40MHZ:
 *      0: Don't use 40MHZ
 *      1: Use lower band as second band
 *      2: Use higher band as second band
 * */
int bl_send_monitor_channel_set(struct bl_hw *bl_hw, struct mm_monitor_channel_cfm *cfm, int channel, int use_40Mhz);
int bl_send_version_req(struct bl_hw *bl_hw, struct mm_version_cfm *cfm);
int bl_send_me_config_req(struct bl_hw *bl_hw);
int bl_send_me_chan_config_req(struct bl_hw *bl_hw);
int bl_send_me_rate_config_req(struct bl_hw *bl_hw, uint8_t sta_idx, uint16_t fixed_rate_cfg);
int bl_send_start(struct bl_hw *bl_hw);
int bl_send_add_if(struct bl_hw *bl_hw, const unsigned char *mac,
                     enum nl80211_iftype iftype, bool p2p, struct mm_add_if_cfm *cfm);
int bl_send_remove_if(struct bl_hw *bl_hw, uint8_t inst_nbr);
int bl_send_scanu_req(struct bl_hw *bl_hw, struct bl_send_scanu_para *scanu_para);
int bl_send_scanu_raw_send(struct bl_hw *bl_hw, uint8_t *pkt, int len);
int bl_send_sm_connect_req(struct bl_hw *bl_hw, struct cfg80211_connect_params *sme, struct sm_connect_cfm *cfm);
int bl_send_sm_connect_abort_req(struct bl_hw *bl_hw, struct sm_connect_abort_cfm *cfm);
int bl_send_sm_disconnect_req(struct bl_hw *bl_hw);
int bl_send_mm_powersaving_req(struct bl_hw *bl_hw, int mode);
int bl_send_mm_denoise_req(struct bl_hw *bl_hw, int mode);
int bl_send_apm_start_req(struct bl_hw *bl_hw, struct apm_start_cfm *cfm, char *ssid, char *password, int channel, uint8_t vif_index, uint8_t hidden_ssid, uint16_t bcn_int);
int bl_send_apm_stop_req(struct bl_hw *bl_hw, uint8_t vif_idx);
int bl_send_apm_sta_del_req(struct bl_hw *bl_hw, struct apm_sta_del_cfm *cfm, uint8_t sta_idx, uint8_t vif_idx);
int bl_send_apm_conf_max_sta_req(struct bl_hw *bl_hw, uint8_t max_sta_supported);
int bl_send_cfg_task_req(struct bl_hw *bl_hw, uint32_t ops, uint32_t task, uint32_t element, uint32_t type, void *arg1, void *arg2);
int bl_send_channel_set_req(struct bl_hw *bl_hw, int channel);
void bl_msg_update_channel_cfg(const char *code);
int bl_msg_get_channel_nums();
int bl_get_fixed_channels_is_valid(uint16_t *channels, uint16_t channel_num);
int bl_send_beacon_interval_set(struct bl_hw *bl_hw, struct mm_set_beacon_int_cfm *cfm, uint16_t beacon_int);
uint16_t phy_channel_to_freq(uint8_t band, int channel);
uint8_t phy_freq_to_channel(uint8_t band, uint16_t freq);
#endif
