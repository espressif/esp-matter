/*
 * Internal WPA/RSN supplicant state machine definitions
 * Copyright (c) 2004-2010, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef WPA_I_H
#define WPA_I_H

#include <bl_supplicant/bl_wifi_driver.h>

typedef void (*WPA_TLV_PACK_CB)(eapol_frame_id_t id, void *data, uint16_t len);

struct install_key {
#ifdef CONFIG_MIC_FAILURE_COUNTERMEASURE
    int mic_errors_seen; /* Michael MIC errors with the current PTK */
#endif
#if 0 // useless
    enum wpa_alg alg;
    u8 addr[ETH_ALEN];
    int key_idx;
    int set_tx;
    u8 key[32];
#endif
    u8 seq[10];
};

#ifdef PMKSA_CACHE
#define ASSOC_IE_LEN 24 + 2 + PMKID_LEN + RSN_SELECTOR_LEN
#else
#define ASSOC_IE_LEN 24 + 2 + RSN_SELECTOR_LEN
#endif
/**
 * struct wpa_sm - Internal WPA state machine data
 */
struct wpa_sm {
    u8 pmk[PMK_LEN];
    u8 pmk_len;

    struct wpa_ptk ptk, tptk;
    bool ptk_set, tptk_set;
    bool renew_snonce;
    u8 snonce[WPA_NONCE_LEN];
    u8 anonce[WPA_NONCE_LEN]; /* ANonce from the last 1/4 msg */
    u8 rx_replay_counter[WPA_REPLAY_COUNTER_LEN];
    bool rx_replay_counter_set;
    u8 request_counter[WPA_REPLAY_COUNTER_LEN];
#ifdef CONFIG_PMKSA_CACHE
    struct rsn_pmksa_cache *pmksa; /* PMKSA cache */
    struct rsn_pmksa_cache_entry *cur_pmksa; /* current PMKSA entry */
    void *network_ctx;
#endif

    u8 proto;
    u16 pairwise_cipher;
    u16 group_cipher;
    u16 key_mgmt;
    u16 mgmt_group_cipher;

#ifdef CONFIG_VALIDATE_IE_IN_PROCESS_3_OF_4
    int rsn_enabled; /* Whether RSN is enabled in configuration */
#endif

#ifdef CONFIG_MIC_FAILURE_COUNTERMEASURE
    int countermeasures; /*TKIP countermeasures state flag, 1:in countermeasures state*/
    bl_wifi_timer_t cm_timer;
#endif

    u8 assoc_wpa_ie_len;
    u8 assoc_wpa_ie[ASSOC_IE_LEN]; /* Own WPA/RSN IE from (Re)AssocReq */

#ifdef CONFIG_SUPPLICANT_REKEY_WHEN_TIMEDOUT
    int wpa_ptk_rekey;
#endif
    u8 own_addr[ETH_ALEN];

    u8 bssid[ETH_ALEN];

    enum wpa_states wpa_state;

#ifdef CONFIG_VALIDATE_IE_IN_PROCESS_3_OF_4
    u8 *ap_wpa_ie, *ap_rsn_ie;
    size_t ap_wpa_ie_len, ap_rsn_ie_len;
#endif

    bool key_install;

    struct install_key install_ptk;
    struct install_key install_gtk;

    struct wpa_gtk_data gd; //used for calllback save param
    u16 key_info;       //used for txcallback param
    u16 txcb_flags;
    bool ap_notify_completed_rsne;
    wifi_pmf_config_t pmf_cfg;
    struct rsn_sppamsdu_sup spp_sup;

    uint8_t vif_idx;
    uint8_t sta_idx;
    bl_wifi_timer_t fourway_hs_timer;
    WPA_TLV_PACK_CB tlv_pack_cb;
};

/**
 * set_key - Configure encryption key
 * @ifname: Interface name (for multi-SSID/VLAN support)
 * @priv: private driver interface data
 * @alg: encryption algorithm (%WPA_ALG_NONE, %WPA_ALG_WEP,
 *	%WPA_ALG_TKIP, %WPA_ALG_CCMP, %WPA_ALG_IGTK, %WPA_ALG_PMK);
 *	%WPA_ALG_NONE clears the key.
 * @addr: address of the peer STA or ff:ff:ff:ff:ff:ff for
 *	broadcast/default keys
 * @key_idx: key index (0..3), usually 0 for unicast keys; 0..4095 for
 *	IGTK
 * @set_tx: configure this key as the default Tx key (only used when
 *	driver does not support separate unicast/individual key
 * @seq: sequence number/packet number, seq_len octets, the next
 *	packet number to be used for in replay protection; configured
 *	for Rx keys (in most cases, this is only used with broadcast
 *	keys and set to zero for unicast keys)
 * @seq_len: length of the seq, depends on the algorithm:
 *	TKIP: 6 octets, CCMP: 6 octets, IGTK: 6 octets
 * @key: key buffer; TKIP: 16-byte temporal key, 8-byte Tx Mic key,
 *	8-byte Rx Mic Key
 * @key_len: length of the key buffer in octets (WEP: 5 or 13,
 *	TKIP: 32, CCMP: 16, IGTK: 16)
 *
 * Returns: 0 on success, -1 on failure
 *
 * Configure the given key for the kernel driver. If the driver
 * supports separate individual keys (4 default keys + 1 individual),
 * addr can be used to determine whether the key is default or
 * individual. If only 4 keys are supported, the default key with key
 * index 0 is used as the individual key. STA must be configured to use
 * it as the default Tx key (set_tx is set) and accept Rx for all the
 * key indexes. In most cases, WPA uses only key indexes 1 and 2 for
 * broadcast keys, so key index 0 is available for this kind of
 * configuration.
 *
 * Please note that TKIP keys include separate TX and RX MIC keys and
 * some drivers may expect them in different order than wpa_supplicant
 * is using. If the TX/RX keys are swapped, all TKIP encrypted packets
 * will tricker Michael MIC errors. This can be fixed by changing the
 * order of MIC keys by swapping te bytes 16..23 and 24..31 of the key
 * in driver_*.c set_key() implementation, see driver_ndis.c for an
 * example on how this can be done.
 */


typedef void (*WPA_SEND_FUNC)(uint8_t sta_idx, void *buffer, u16 len);

typedef void (*WPA_SET_ASSOC_IE)(uint8_t vif_idx, u8 proto, u8 *assoc_buf, u32 assoc_wpa_ie_len);

typedef void (*WPA_INSTALL_KEY)(uint8_t vif_idx, uint8_t sta_idx, enum wpa_alg alg, int key_idx, int set_tx,
              u8 *seq, size_t seq_len, u8 *key, size_t key_len, bool pairwise);

typedef int (*WPA_GET_KEY)(u8 *ifx, int *alg, u8 *addt, int *keyidx, u8 *key, size_t key_len, int key_entry_valid);

typedef void (*WPA_DEAUTH_FUNC)(uint8_t sta_idx, u8 reason_code);

typedef void (*WPA_NEG_COMPLETE)(uint8_t sta_idx);

bool wpa_sm_init(void);

void wpa_sm_deinit(void);

void eapol_txcb(void *eb);

void wpa_set_profile(u32 wpa_proto, u16 key_mgmt);

int wpa_set_bss(u8 vif_idx, u8 sta_idx, char *macddr, char *bssid, u8 pairwise_cipher, u8 group_cipher, bool pmf_required, u8 mgmt_group_cipher);

int wpa_set_parm_at_connect(wifi_connect_parm_t *parm);

int wpa_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len);
void wpa_sendto_wrapper(bool is_sta, void *buffer, u16 len, struct bl_custom_tx_cfm *tx_cfm);
void wpa_config_assoc_ie(uint8_t vif_idx, u8 proto, u8 *assoc_buf, u32 assoc_wpa_ie_len);
void wpa_install_key(uint8_t vif_idx, uint8_t sta_idx, enum wpa_alg alg, int key_idx, int set_tx,
                     u8 *seq, size_t seq_len, u8 *key, size_t key_len, bool pairwise);
int wpa_get_key(uint8_t *ifx, int *alg, u8 *addr, int *key_idx,
                u8 *key, size_t key_len, int key_entry_valid);
void wpa_deauthenticate(uint8_t sta_idx, u8 reason_code);
void wpa_neg_complete(uint8_t sta_idx);
void wpa_set_4way_handshake_timer(void);
void wpa_clear_4way_handshake_timer(void);
void wpa_reg_diag_tlv_cb(void* tlv_pack_cb);

#endif /* WPA_I_H */
