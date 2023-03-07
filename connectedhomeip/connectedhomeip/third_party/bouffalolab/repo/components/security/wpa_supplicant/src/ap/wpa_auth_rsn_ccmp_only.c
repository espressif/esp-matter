/*
 * IEEE 802.11 RSN / WPA Authenticator
 * Copyright (c) 2004-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include <bl_supplicant/bl_wifi_driver.h>
#include "utils/includes.h"
#include "utils/common.h"
#include "utils/state_machine.h"
#include "common/ieee802_11_defs.h"
#include "ap/wpa_auth.h"
#include "ap/wpa_auth_i.h"
#include "ap/wpa_auth_ie.h"
#include "utils/wpa_debug.h"
#include "hostapd.h"
#include "rsn_supp/wpa.h"
#include "ap/ap_config.h"
#include "common/wpa_common.h"

#include "crypto/aes_wrap.h"
#include "crypto/crypto.h"
#include "crypto/sha1.h"
#include "crypto/sha256.h"
#include "crypto/random.h"

#include "bl_supplicant/bl_wifi_driver.h"

#define STATE_MACHINE_DATA struct wpa_state_machine
#define STATE_MACHINE_DEBUG_PREFIX "WPA"
#define STATE_MACHINE_ADDR sm->addr


static int wpa_sm_step(struct wpa_state_machine *sm);
static int wpa_verify_key_mic(struct wpa_ptk *PTK, u8 *data, size_t data_len);
static void wpa_group_sm_step(struct wpa_authenticator *wpa_auth,
                  struct wpa_group *group);
static void wpa_request_new_ptk(struct wpa_state_machine *sm);
static int wpa_gtk_update(struct wpa_authenticator *wpa_auth,
              struct wpa_group *group);
static int wpa_group_config_group_keys(struct wpa_authenticator *wpa_auth,
                       struct wpa_group *group);

static const u32 dot11RSNAConfigGroupUpdateCount = 4;
static const u32 dot11RSNAConfigPairwiseUpdateCount = 4;

static struct wpa_state_machine *wpa_auth_get_sm(u32 index)
{
    struct hostapd_data *hapd = (struct hostapd_data *)bl_wifi_get_hostap_private_internal();
    if (!hapd) {
        return NULL;
    }

    if ((index < WPA_SM_MAX_INDEX) && (BIT(index) & hapd->sm_valid_bitmap)) {
        return hapd->sm_table[index];
    }

    return NULL;
}

static void wpa_auth_add_sm(struct wpa_state_machine *sm)
{
    struct hostapd_data *hapd = (struct hostapd_data *)bl_wifi_get_hostap_private_internal();
    u8 i;
    for (i = 0; i < WPA_SM_MAX_INDEX; i++) {
        if (BIT(i) & hapd->sm_valid_bitmap) {
            if (hapd->sm_table[i] == sm) {
                wpa_printf( MSG_INFO, "add sm already exist i=%d", i);
            }
            continue;
        }
        hapd->sm_table[i] = sm;
        hapd->sm_valid_bitmap |= BIT(i);
        sm->index = i;
        wpa_printf( MSG_DEBUG, "add sm, index=%d bitmap=%x\n", i, hapd->sm_valid_bitmap);
        return;
    }
}

static void wpa_auth_del_sm(struct wpa_state_machine *sm)
{
    struct hostapd_data *hapd = (struct hostapd_data *)bl_wifi_get_hostap_private_internal();
    if (!hapd) {
        return;
    }
    if (sm && (sm->index < WPA_SM_MAX_INDEX)) {
        if (sm != hapd->sm_table[sm->index]) {
            wpa_printf( MSG_INFO, "del sm error %lu", sm->index);
        }
        hapd->sm_table[sm->index] = NULL;
        hapd->sm_valid_bitmap &= ~BIT(sm->index);
        wpa_printf( MSG_DEBUG, "del sm, index=%lu bitmap=%x\n", sm->index, hapd->sm_valid_bitmap);
    }
}

static inline int wpa_auth_mic_failure_report(
    struct wpa_authenticator *wpa_auth, const u8 *addr)
{
    return 0;
}

static inline const u8 *wpa_auth_get_psk(struct wpa_authenticator *wpa_auth,
                      const u8 *addr, const u8 *prev_psk)
{
    struct hostapd_data *hapd = (struct hostapd_data *)bl_wifi_get_hostap_private_internal();

    if (!hapd) {
        return NULL;
    }

    return (u8*)hostapd_get_psk(hapd->conf, addr, prev_psk);
}

static inline int wpa_auth_get_msk(struct wpa_authenticator *wpa_auth,
                   const u8 *addr, u8 *msk, size_t *len)
{
    return -1;
}

static inline int wpa_auth_set_key(struct wpa_authenticator *wpa_auth,
                   int vlan_id,
                   uint8_t vif_idx, uint8_t sta_idx,
                   enum wpa_alg alg, const u8 *addr, int idx,
                   u8 *key, size_t key_len, bool pairwise)
{
    return bl_wifi_set_ap_key_internal(vif_idx, sta_idx, alg, idx, key, key_len, pairwise);
}


static inline int wpa_auth_get_seqnum(struct wpa_authenticator *wpa_auth,
                      const u8 *addr, int idx, u8 *seq)
{
    return -1;
}

static inline int
wpa_auth_send_eapol(struct wpa_authenticator *wpa_auth, const u8 *addr,
            const u8 *data, size_t data_len, int encrypt)
{
    void *buffer = os_malloc(256);
    struct l2_ethhdr *eth = buffer;

    if (!buffer){
        wpa_printf( MSG_DEBUG, "send_eapol, buffer=%p\n", buffer);
        return -1;
    }

    memcpy(eth->h_dest, addr, ETH_ALEN);
    memcpy(eth->h_source, wpa_auth->addr, ETH_ALEN);
    eth->h_proto = host_to_be16(ETH_P_EAPOL);

    memcpy((char *)buffer + sizeof(struct l2_ethhdr), data, data_len);
    wpa_sendto_wrapper(false, buffer, sizeof(struct l2_ethhdr) + data_len, NULL);
    os_free(buffer);
    return 0;
}

static void wpa_sta_disconnect(struct wpa_authenticator *wpa_auth,
                   const u8 *addr, uint8_t vif_idx, uint8_t sta_idx)
{
    wpa_printf(MSG_DEBUG, "wpa_sta_disconnect STA " MACSTR, MAC2STR(addr));
    bl_wifi_ap_deauth_internal(vif_idx, sta_idx, WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT);
    return;
}

static void wpa_rekey_gtk(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_authenticator *wpa_auth = eloop_ctx;
    struct wpa_group *group;

    for (group = wpa_auth->group; group; group = group->next) {
        group->GTKReKey = TRUE;
        do {
            group->changed = FALSE;
            wpa_group_sm_step(wpa_auth, group);
        } while (group->changed);
    }
}

static struct wpa_group *wpa_group_init(struct wpa_authenticator *wpa_auth,
                     int vlan_id)
{
    struct wpa_group *group;
    group = (struct wpa_group *)os_zalloc(sizeof(struct wpa_group));
    if (group == NULL)
        return NULL;

    group->GTKAuthenticator = TRUE;
    group->GTK_len = wpa_cipher_key_len(wpa_auth->conf.wpa_group);

    group->GInit = TRUE;
    wpa_group_sm_step(wpa_auth, group);
    group->GInit = FALSE;
    wpa_group_sm_step(wpa_auth, group);

    return group;
}


/**
 * wpa_init - Initialize WPA authenticator
 * @addr: Authenticator address
 * @conf: Configuration for WPA authenticator
 * @cb: Callback functions for WPA authenticator
 * Returns: Pointer to WPA authenticator data or %NULL on failure
 */
struct wpa_authenticator *wpa_init(uint8_t vif_idx, const u8 *addr,
                    struct wpa_auth_config *conf,
                    struct wpa_auth_callbacks *cb)
{
    struct wpa_authenticator *wpa_auth;
    wpa_auth = (struct wpa_authenticator *)os_zalloc(sizeof(struct wpa_authenticator));
    if (wpa_auth == NULL)
        return NULL;
    wpa_auth->vif_idx = vif_idx;
    memcpy(wpa_auth->addr, addr, ETH_ALEN);
    memcpy(&wpa_auth->conf, conf, sizeof(*conf));

    wpa_auth_gen_wpa_ie_rsn_ccmp_only(wpa_auth);

    wpa_auth->group = wpa_group_init(wpa_auth, 0);
    if (wpa_auth->group == NULL) {
        os_free(wpa_auth);
        return NULL;
    }

    return wpa_auth;
}

struct wpa_state_machine *
wpa_auth_sta_init(struct wpa_authenticator *wpa_auth, const u8 *addr)
{
    struct wpa_state_machine *sm;

    sm = (struct wpa_state_machine *)os_zalloc(sizeof(struct wpa_state_machine));
    if (sm == NULL)
        return NULL;
    memcpy(sm->addr, addr, ETH_ALEN);

    sm->wpa_auth = wpa_auth;
    sm->group = wpa_auth->group;
    wpa_auth_add_sm(sm);

    return sm;
}

int wpa_auth_sta_associated(struct wpa_authenticator *wpa_auth,
                struct wpa_state_machine *sm)
{
    if (wpa_auth == NULL || !wpa_auth->conf.wpa || sm == NULL)
        return -1;

    if (sm->started) {
        memset(&sm->key_replay, 0, sizeof(sm->key_replay));
        sm->ReAuthenticationRequest = TRUE;
        return wpa_sm_step(sm);
    }

    sm->started = 1;

    sm->Init = TRUE;
    if (wpa_sm_step(sm) == 1)
        return 1; /* should not really happen */
    sm->Init = FALSE;
    sm->AuthenticationRequest = TRUE;
    return wpa_sm_step(sm);
}


void wpa_auth_sta_no_wpa(struct wpa_state_machine *sm)
{
    /* WPA/RSN was not used - clear WPA state. This is needed if the STA
     * reassociates back to the same AP while the previous entry for the
     * STA has not yet been removed. */
    if (sm == NULL)
        return;

    sm->wpa_key_mgmt = 0;
}


static void wpa_free_sta_sm(struct wpa_state_machine *sm)
{
    wpa_auth_del_sm(sm);
    if (sm->GUpdateStationKeys) {
        sm->group->GKeyDoneStations--;
        sm->GUpdateStationKeys = FALSE;
    }
    wpa_printf( MSG_DEBUG, "wpa_free_sta_sm: free eapol=%p\n", sm->last_rx_eapol_key);
    os_free(sm->last_rx_eapol_key);
    os_free(sm->wpa_ie);
    os_free(sm);
}


void wpa_auth_sta_deinit(struct wpa_state_machine *sm)
{
    wpa_printf( MSG_DEBUG, "deinit sm=%p\n", sm);
    if (sm == NULL)
        return;

    bl_wifi_timer_disarm(&sm->resend_eapol);
    bl_wifi_timer_done(&sm->resend_eapol);

    if (sm->in_step_loop) {
        /* Must not free state machine while wpa_sm_step() is running.
         * Freeing will be completed in the end of wpa_sm_step(). */
        wpa_printf( MSG_DEBUG, "WPA: Registering pending STA state "
               "machine deinit for " MACSTR, MAC2STR(sm->addr));
        sm->pending_deinit = 1;
    } else
        wpa_free_sta_sm(sm);
}


static void wpa_request_new_ptk(struct wpa_state_machine *sm)
{
    if (sm == NULL)
        return;

    sm->PTKRequest = TRUE;
    sm->PTK_valid = 0;
}

static int wpa_replay_counter_valid(struct wpa_key_replay_counter *ctr,
                    const u8 *replay_counter)
{
    int i;
    for (i = 0; i < RSNA_MAX_EAPOL_RETRIES; i++) {
        if (!ctr[i].valid)
            break;
        if (memcmp(replay_counter, ctr[i].counter,
                  WPA_REPLAY_COUNTER_LEN) == 0)
            return 1;
    }
    return 0;
}

static void wpa_replay_counter_mark_invalid(struct wpa_key_replay_counter *ctr,
                        const u8 *replay_counter)
{
    int i;
    for (i = 0; i < RSNA_MAX_EAPOL_RETRIES; i++) {
        if (ctr[i].valid &&
            (replay_counter == NULL ||
             memcmp(replay_counter, ctr[i].counter,
                   WPA_REPLAY_COUNTER_LEN) == 0))
            ctr[i].valid = FALSE;
    }
}

static int wpa_receive_error_report(struct wpa_authenticator *wpa_auth,
                    struct wpa_state_machine *sm, int group)
{
    if (group && wpa_auth->conf.wpa_group != WPA_CIPHER_TKIP) {
    } else if (!group && sm->pairwise != WPA_CIPHER_TKIP) {
    } else {
        if (wpa_auth_mic_failure_report(wpa_auth, sm->addr) > 0)
            return 1; /* STA entry was removed */
    }

    /*
     * Error report is not a request for a new key handshake, but since
     * Authenticator may do it, let's change the keys now anyway.
     */
    wpa_request_new_ptk(sm);
    return 0;
}

void wpa_receive(struct wpa_authenticator *wpa_auth, struct wpa_state_machine *sm, u8 *data, size_t data_len)
{
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    u16 key_info, key_data_length;
    enum { PAIRWISE_2, PAIRWISE_4, GROUP_2, REQUEST,
           SMK_M1, SMK_M3, SMK_ERROR } msg;
    struct wpa_eapol_ie_parse kde;
    int ft;
    const u8 *eapol_key_ie;
    size_t eapol_key_ie_len;

    if (wpa_auth == NULL || !wpa_auth->conf.wpa || sm == NULL)
        return;

    if (data_len < sizeof(*hdr) + sizeof(*key))
        return;

    hdr = (struct ieee802_1x_hdr *) data;
    key = (struct wpa_eapol_key *) (hdr + 1);
    key_info = WPA_GET_BE16(key->key_info);
    key_data_length = WPA_GET_BE16(key->key_data_length);
    wpa_printf( MSG_DEBUG, "WPA: Received EAPOL-Key from " MACSTR
           " key_info=0x%x type=%u key_data_length=%u\n",
           MAC2STR(sm->addr), key_info, key->type, key_data_length);
    if (key_data_length > data_len - sizeof(*hdr) - sizeof(*key)) {
        wpa_printf( MSG_INFO, "WPA: Invalid EAPOL-Key frame - "
               "key_data overflow (%d > %lu)\n",
               key_data_length,
               (unsigned long) (data_len - sizeof(*hdr) -
                        sizeof(*key)));
        return;
    }

    if (key->type == EAPOL_KEY_TYPE_WPA) {
        /*
         * Some deployed station implementations seem to send
         * msg 4/4 with incorrect type value in WPA2 mode.
         */
        wpa_printf( MSG_DEBUG, "Workaround: Allow EAPOL-Key "
               "with unexpected WPA type in RSN mode\n");
    } else if (key->type != EAPOL_KEY_TYPE_RSN) {
        wpa_printf( MSG_DEBUG, "Ignore EAPOL-Key with "
               "unexpected type %d in RSN mode\n",
               key->type);
        return;
    }

    wpa_hexdump(MSG_DEBUG, "WPA: Received Key Nonce", key->key_nonce,
            WPA_NONCE_LEN);
    wpa_hexdump(MSG_DEBUG, "WPA: Received Replay Counter",
            key->replay_counter, WPA_REPLAY_COUNTER_LEN);

    /* FIX: verify that the EAPOL-Key frame was encrypted if pairwise keys
     * are set */

    if ((key_info & (WPA_KEY_INFO_SMK_MESSAGE | WPA_KEY_INFO_REQUEST)) ==
        (WPA_KEY_INFO_SMK_MESSAGE | WPA_KEY_INFO_REQUEST)) {
        if (key_info & WPA_KEY_INFO_ERROR) {
            msg = SMK_ERROR;
        } else {
            msg = SMK_M1;
        }
    } else if (key_info & WPA_KEY_INFO_SMK_MESSAGE) {
        msg = SMK_M3;
    } else if (key_info & WPA_KEY_INFO_REQUEST) {
        msg = REQUEST;
    } else if (!(key_info & WPA_KEY_INFO_KEY_TYPE)) {
        msg = GROUP_2;
    } else if (key_data_length == 0) {
        msg = PAIRWISE_4;
    } else {
        msg = PAIRWISE_2;
    }

    /* TODO: key_info type validation for PeerKey */
    if (msg == REQUEST || msg == PAIRWISE_2 || msg == PAIRWISE_4 ||
        msg == GROUP_2) {
        u16 ver = key_info & WPA_KEY_INFO_TYPE_MASK;
        if (sm->pairwise == WPA_CIPHER_CCMP ||
            sm->pairwise == WPA_CIPHER_GCMP) {

            if (ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
                return;
            }
        }
    }

    if (key_info & WPA_KEY_INFO_REQUEST) {
        if (sm->req_replay_counter_used &&
            memcmp(key->replay_counter, sm->req_replay_counter,
                  WPA_REPLAY_COUNTER_LEN) <= 0) {
            return;
        }
    }

    if (!(key_info & WPA_KEY_INFO_REQUEST) &&
        !wpa_replay_counter_valid(sm->key_replay, key->replay_counter)) {
        int i;

        if (msg == PAIRWISE_2 &&
            wpa_replay_counter_valid(sm->prev_key_replay,
                         key->replay_counter) &&
            sm->wpa_ptk_state == WPA_PTK_PTKINITNEGOTIATING &&
            memcmp(sm->SNonce, key->key_nonce, WPA_NONCE_LEN) != 0)
        {
            /*
             * Some supplicant implementations (e.g., Windows XP
             * WZC) update SNonce for each EAPOL-Key 2/4. This
             * breaks the workaround on accepting any of the
             * pending requests, so allow the SNonce to be updated
             * even if we have already sent out EAPOL-Key 3/4.
             */
            sm->update_snonce = 1;
            wpa_replay_counter_mark_invalid(sm->prev_key_replay,
                            key->replay_counter);
            goto continue_processing;
        }

        if (msg == PAIRWISE_2 &&
            wpa_replay_counter_valid(sm->prev_key_replay,
                         key->replay_counter) &&
            sm->wpa_ptk_state == WPA_PTK_PTKINITNEGOTIATING) {
        } else {
        }
        for (i = 0; i < RSNA_MAX_EAPOL_RETRIES; i++) {
            if (!sm->key_replay[i].valid)
                break;
            wpa_hexdump(MSG_DEBUG, "pending replay counter",
                    sm->key_replay[i].counter,
                    WPA_REPLAY_COUNTER_LEN);
        }
        wpa_hexdump(MSG_DEBUG, "received replay counter",
                key->replay_counter, WPA_REPLAY_COUNTER_LEN);
        return;
    }

continue_processing:
    switch (msg) {
    case PAIRWISE_2:
        if (sm->wpa_ptk_state != WPA_PTK_PTKSTART &&
            sm->wpa_ptk_state != WPA_PTK_PTKCALCNEGOTIATING &&
            (!sm->update_snonce ||
             sm->wpa_ptk_state != WPA_PTK_PTKINITNEGOTIATING)) {
            return;
        }
        random_add_randomness(key->key_nonce, WPA_NONCE_LEN);
        if (wpa_parse_kde_ies((u8 *) (key + 1), key_data_length,
                      &kde) < 0) {
            return;
        }
        if (kde.rsn_ie) {
            eapol_key_ie = kde.rsn_ie;
            eapol_key_ie_len = kde.rsn_ie_len;
        } else {
            eapol_key_ie = kde.wpa_ie;
            eapol_key_ie_len = kde.wpa_ie_len;
        }
        ft = sm->wpa == WPA_VERSION_WPA2 &&
            wpa_key_mgmt_ft(sm->wpa_key_mgmt);
        if (sm->wpa_ie == NULL ||
            wpa_compare_rsn_ie(ft,
                       sm->wpa_ie, sm->wpa_ie_len,
                       eapol_key_ie, eapol_key_ie_len)) {
            if (sm->wpa_ie) {
                wpa_hexdump(MSG_DEBUG, "WPA IE in AssocReq",
                        sm->wpa_ie, sm->wpa_ie_len);
            }
            wpa_hexdump(MSG_DEBUG, "WPA IE in msg 2/4",
                    eapol_key_ie, eapol_key_ie_len);
            /* MLME-DEAUTHENTICATE.request */
            wpa_sta_disconnect(wpa_auth, sm->addr, sm->wpa_auth->vif_idx, sm->sta_idx);
            return;
        }
        break;
    case PAIRWISE_4:
        if (sm->wpa_ptk_state != WPA_PTK_PTKINITNEGOTIATING ||
            !sm->PTK_valid) {
            return;
        }
        break;
    case GROUP_2:
        if (sm->wpa_ptk_group_state != WPA_PTK_GROUP_REKEYNEGOTIATING
            || !sm->PTK_valid) {
            return;
        }
        break;
    case SMK_M1:
    case SMK_M3:
    case SMK_ERROR:
        return; /* STSL disabled - ignore SMK messages */
    case REQUEST:
        break;
    }


    if (key_info & WPA_KEY_INFO_ACK) {
        return;
    }

    if (!(key_info & WPA_KEY_INFO_MIC)) {
        return;
    }

    sm->MICVerified = FALSE;
    if (sm->PTK_valid && !sm->update_snonce) {
        if (wpa_verify_key_mic(&sm->PTK, data, data_len)) {
            return;
        }
        sm->MICVerified = TRUE;
        bl_wifi_timer_disarm(&sm->resend_eapol);
        bl_wifi_timer_done(&sm->resend_eapol);
        sm->pending_1_of_4_timeout = 0;
    }

    if (key_info & WPA_KEY_INFO_REQUEST) {
        if (sm->MICVerified) {
            sm->req_replay_counter_used = 1;
            memcpy(sm->req_replay_counter, key->replay_counter,
                  WPA_REPLAY_COUNTER_LEN);
        } else {
            return;
        }

        /*
         * TODO: should decrypt key data field if encryption was used;
         * even though MAC address KDE is not normally encrypted,
         * supplicant is allowed to encrypt it.
         */
        if (msg == SMK_ERROR) {
            return;
        } else if (key_info & WPA_KEY_INFO_ERROR) {
            if (wpa_receive_error_report(
                    wpa_auth, sm,
                    !(key_info & WPA_KEY_INFO_KEY_TYPE)) > 0)
                return; /* STA entry was removed */
        } else if (key_info & WPA_KEY_INFO_KEY_TYPE) {
            wpa_request_new_ptk(sm);
        } else if (key_data_length > 0 &&
               wpa_parse_kde_ies((const u8 *) (key + 1),
                         key_data_length, &kde) == 0 &&
               kde.mac_addr) {
        } else {
            wpa_rekey_gtk(wpa_auth, NULL);
        }
    } else {
        /* Do not allow the same key replay counter to be reused. */
        wpa_replay_counter_mark_invalid(sm->key_replay,
                        key->replay_counter);

        if (msg == PAIRWISE_2) {
            /*
             * Maintain a copy of the pending EAPOL-Key frames in
             * case the EAPOL-Key frame was retransmitted. This is
             * needed to allow EAPOL-Key msg 2/4 reply to another
             * pending msg 1/4 to update the SNonce to work around
             * unexpected supplicant behavior.
             */
            memcpy(sm->prev_key_replay, sm->key_replay,
                  sizeof(sm->key_replay));
        } else {
            memset(sm->prev_key_replay, 0,
                  sizeof(sm->prev_key_replay));
        }

        /*
         * Make sure old valid counters are not accepted anymore and
         * do not get copied again.
         */
        wpa_replay_counter_mark_invalid(sm->key_replay, NULL);
    }

    wpa_printf( MSG_DEBUG, "wpa_rx: free eapol=%p\n", sm->last_rx_eapol_key);
    os_free(sm->last_rx_eapol_key);
    sm->last_rx_eapol_key = (u8 *)os_malloc(data_len);
    if (sm->last_rx_eapol_key == NULL)
        return;
    wpa_printf( MSG_DEBUG, "wpa_rx: new eapol=%p\n", sm->last_rx_eapol_key);
    memcpy(sm->last_rx_eapol_key, data, data_len);
    sm->last_rx_eapol_key_len = data_len;

    sm->rx_eapol_key_secure = !!(key_info & WPA_KEY_INFO_SECURE);
    sm->EAPOLKeyReceived = TRUE;
    sm->EAPOLKeyPairwise = !!(key_info & WPA_KEY_INFO_KEY_TYPE);
    sm->EAPOLKeyRequest = !!(key_info & WPA_KEY_INFO_REQUEST);
    memcpy(sm->SNonce, key->key_nonce, WPA_NONCE_LEN);
    wpa_sm_step(sm);
}


void __wpa_send_eapol(struct wpa_authenticator *wpa_auth,
              struct wpa_state_machine *sm, int key_info,
              const u8 *key_rsc, const u8 *nonce,
              const u8 *kde, size_t kde_len,
              int keyidx, int encr)
{
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    size_t len;
    int alg;
    int key_data_len, pad_len = 0;
    u8 *buf, *pos;
    int version, pairwise;
    int i;

    wpa_printf( MSG_DEBUG, "wpa_auth=%p sm=%p  kdersc=%p kde=%p nounce=%p kde_len=%u keyidx=%d encr=%d\n",
                   wpa_auth,sm, key_rsc, kde, nonce, kde_len, keyidx, encr);
    len = sizeof(struct ieee802_1x_hdr) + sizeof(struct wpa_eapol_key);

    version = WPA_KEY_INFO_TYPE_HMAC_SHA1_AES;

    pairwise = key_info & WPA_KEY_INFO_KEY_TYPE;

    wpa_printf( MSG_DEBUG, "WPA: Send EAPOL(version=%d secure=%d mic=%d "
           "ack=%d install=%d pairwise=%d kde_len=%lu keyidx=%d "
           "encr=%d)\n",
           version,
           (key_info & WPA_KEY_INFO_SECURE) ? 1 : 0,
           (key_info & WPA_KEY_INFO_MIC) ? 1 : 0,
           (key_info & WPA_KEY_INFO_ACK) ? 1 : 0,
           (key_info & WPA_KEY_INFO_INSTALL) ? 1 : 0,
           pairwise, (unsigned long) kde_len, keyidx, encr);

    key_data_len = kde_len;

    if (encr) {
        pad_len = key_data_len % 8;
        if (pad_len)
            pad_len = 8 - pad_len;
        key_data_len += pad_len + 8;
    }

    len += key_data_len;

    hdr = (struct ieee802_1x_hdr *)os_zalloc(len);
    if (hdr == NULL)
        return;
    hdr->version = wpa_auth->conf.eapol_version;
    hdr->type = IEEE802_1X_TYPE_EAPOL_KEY;
    hdr->length = host_to_be16(len  - sizeof(*hdr));
    key = (struct wpa_eapol_key *) (hdr + 1);

    key->type = EAPOL_KEY_TYPE_RSN;
    key_info |= version;
    if (encr)
        key_info |= WPA_KEY_INFO_ENCR_KEY_DATA;
    WPA_PUT_BE16(key->key_info, key_info);

    alg = pairwise ? sm->pairwise : wpa_auth->conf.wpa_group;
    WPA_PUT_BE16(key->key_length, wpa_cipher_key_len(alg));
    if (key_info & WPA_KEY_INFO_SMK_MESSAGE)
        WPA_PUT_BE16(key->key_length, 0);

    /* FIX: STSL: what to use as key_replay_counter? */
    for (i = RSNA_MAX_EAPOL_RETRIES - 1; i > 0; i--) {
        sm->key_replay[i].valid = sm->key_replay[i - 1].valid;
        memcpy(sm->key_replay[i].counter,
              sm->key_replay[i - 1].counter,
              WPA_REPLAY_COUNTER_LEN);
    }
    inc_byte_array(sm->key_replay[0].counter, WPA_REPLAY_COUNTER_LEN);
    memcpy(key->replay_counter, sm->key_replay[0].counter,
          WPA_REPLAY_COUNTER_LEN);
    sm->key_replay[0].valid = TRUE;

    if (nonce)
        memcpy(key->key_nonce, nonce, WPA_NONCE_LEN);

    if (key_rsc)
        memcpy(key->key_rsc, key_rsc, WPA_KEY_RSC_LEN);

    if (kde && !encr) {
        memcpy(key + 1, kde, kde_len);
        WPA_PUT_BE16(key->key_data_length, kde_len);
    } else if (encr && kde) {
        buf = (u8 *)os_zalloc(key_data_len);
        if (buf == NULL) {
            os_free(hdr);
            return;
        }
        pos = buf;
        memcpy(pos, kde, kde_len);
        pos += kde_len;

        if (pad_len)
            *pos++ = 0xdd;

        wpa_hexdump_key(MSG_DEBUG, "Plaintext EAPOL-Key Key Data",
                buf, key_data_len);
        if (aes_wrap(sm->PTK.kek, (key_data_len - 8) / 8, buf,
                (u8 *) (key + 1))) {
            os_free(hdr);
            os_free(buf);
            return;
        }
        WPA_PUT_BE16(key->key_data_length, key_data_len);
        os_free(buf);
    }

    if (key_info & WPA_KEY_INFO_MIC) {
        if (!sm->PTK_valid) {
            os_free(hdr);
            return;
        }
        wpa_eapol_key_mic(sm->PTK.kck, version, (u8 *) hdr, len,
                  key->key_mic);
    }

    wpa_auth_send_eapol(wpa_auth, sm->addr, (u8 *) hdr, len, sm->pairwise_set);
    os_free(hdr);
}

int hostap_eapol_resend_process(void *timeout_ctx)
{
    u32 index = (u32)timeout_ctx;
    struct wpa_state_machine *sm = wpa_auth_get_sm(index);

    wpa_printf( MSG_DEBUG, "resend eapol1");

    if (sm) {
        sm->pending_1_of_4_timeout = 0;
        sm->TimeoutEvt = TRUE;
        sm->in_step_loop = 0;
        wpa_sm_step(sm);
    } else {
        wpa_printf( MSG_INFO, "Station left, stop send EAPOL frame");
    }

    return 0;
}

void resend_eapol_handle(void *timeout_ctx)
{
    hostap_eapol_resend_process(timeout_ctx);
}

static void wpa_send_eapol(struct wpa_authenticator *wpa_auth,
               struct wpa_state_machine *sm, int key_info,
               const u8 *key_rsc, const u8 *nonce,
               const u8 *kde, size_t kde_len,
               int keyidx, int encr)
{
    int pairwise = key_info & WPA_KEY_INFO_KEY_TYPE;
    int ctr;

    if (sm == NULL)
        return;

    __wpa_send_eapol(wpa_auth, sm, key_info, key_rsc, nonce, kde, kde_len,
             keyidx, encr);

    ctr = pairwise ? sm->TimeoutCtr : sm->GTimeoutCtr;
    if (pairwise && ctr == 1 && !(key_info & WPA_KEY_INFO_MIC))
        sm->pending_1_of_4_timeout = 1;
    bl_wifi_timer_disarm(&sm->resend_eapol);
    bl_wifi_timer_setfn(&sm->resend_eapol, (bl_wifi_timer_func_t *)resend_eapol_handle, (void*)(sm->index));
    bl_wifi_timer_arm(&sm->resend_eapol, 1000, 0);
}


static int wpa_verify_key_mic(struct wpa_ptk *PTK, u8 *data, size_t data_len)
{
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    u16 key_info;
    int ret = 0;
    int mic_ret = 0;
    u8 mic[16];
#ifdef DEBUG_PRINT
    char debug_log[8];
#endif

    if (data_len < sizeof(*hdr) + sizeof(*key)){
        wpa_printf( MSG_DEBUG, "invalid data length, len=%u\n", data_len);
        return -1;
    }

    hdr = (struct ieee802_1x_hdr *) data;
    key = (struct wpa_eapol_key *) (hdr + 1);
    key_info = WPA_GET_BE16(key->key_info);
    memcpy(mic, key->key_mic, 16);
    memset(key->key_mic, 0, 16);

    mic_ret = wpa_eapol_key_mic(PTK->kck, key_info & WPA_KEY_INFO_TYPE_MASK, data, data_len, key->key_mic);
    if ( mic_ret || memcmp(mic, key->key_mic, 16) != 0) {
        ret = -1;
    }
#ifdef DEBUG_PRINT
    os_memset(debug_log, 0, 8);
    os_memcpy(debug_log, "you mic", sizeof("you mic"));
    wpa_dump_mem(debug_log, mic, 16);
    os_memset(debug_log, 0, 8);
    os_memcpy(debug_log, "my mic", sizeof("my mic"));
    wpa_dump_mem(debug_log, key->key_mic, 16);
#endif

    memcpy(key->key_mic, mic, 16);
    return ret;
}


void wpa_remove_ptk(struct wpa_state_machine *sm)
{
    sm->PTK_valid = FALSE;
    memset(&sm->PTK, 0, sizeof(sm->PTK));
    wpa_auth_set_key(sm->wpa_auth, 0, sm->wpa_auth->vif_idx, sm->sta_idx, WIFI_WPA_ALG_NONE, sm->addr, 0, NULL, 0, true);
    sm->pairwise_set = FALSE;
}


int wpa_auth_sm_event(struct wpa_state_machine *sm, wpa_event event)
{
    int remove_ptk = 1;

    if (sm == NULL)
        return -1;

    switch (event) {
    case WPA_AUTH:
    case WPA_ASSOC:
        break;
    case WPA_DEAUTH:
    case WPA_DISASSOC:
        sm->DeauthenticationRequest = TRUE;
        break;
    case WPA_REAUTH:
    case WPA_REAUTH_EAPOL:
        if (!sm->started) {
            /*
             * When using WPS, we may end up here if the STA
             * manages to re-associate without the previous STA
             * entry getting removed. Consequently, we need to make
             * sure that the WPA state machines gets initialized
             * properly at this point.
             */
            wpa_printf( MSG_DEBUG, "WPA state machine had not been "
                   "started - initialize now");
            sm->started = 1;
            sm->Init = TRUE;
            if (wpa_sm_step(sm) == 1)
                return 1; /* should not really happen */
            sm->Init = FALSE;
            sm->AuthenticationRequest = TRUE;
            break;
        }
        if (sm->GUpdateStationKeys) {
            /*
             * Reauthentication cancels the pending group key
             * update for this STA.
             */
            sm->group->GKeyDoneStations--;
            sm->GUpdateStationKeys = FALSE;
            sm->PtkGroupInit = TRUE;
        }
        sm->ReAuthenticationRequest = TRUE;
        break;
    case WPA_ASSOC_FT:
        break;
    }

    if (remove_ptk) {
        sm->PTK_valid = FALSE;
        memset(&sm->PTK, 0, sizeof(sm->PTK));

        if (event != WPA_REAUTH_EAPOL)
            wpa_remove_ptk(sm);
    }

    return wpa_sm_step(sm);
}


SM_STATE(WPA_PTK, INITIALIZE)
{
    SM_ENTRY_MA(WPA_PTK, INITIALIZE, wpa_ptk);
    if (sm->Init) {
        /* Init flag is not cleared here, so avoid busy
         * loop by claiming nothing changed. */
        sm->changed = FALSE;
    }

    sm->keycount = 0;
    if (sm->GUpdateStationKeys)
        sm->group->GKeyDoneStations--;
    sm->GUpdateStationKeys = FALSE;
    if (1 /* Unicast cipher supported AND (ESS OR ((IBSS or WDS) and
           * Local AA > Remote AA)) */) {
        sm->Pair = TRUE;
    }
    wpa_remove_ptk(sm);
    sm->TimeoutCtr = 0;
}


SM_STATE(WPA_PTK, DISCONNECT)
{
    SM_ENTRY_MA(WPA_PTK, DISCONNECT, wpa_ptk);
    sm->Disconnect = FALSE;
    wpa_sta_disconnect(sm->wpa_auth, sm->addr, sm->wpa_auth->vif_idx, sm->sta_idx);
}


SM_STATE(WPA_PTK, DISCONNECTED)
{
    SM_ENTRY_MA(WPA_PTK, DISCONNECTED, wpa_ptk);
    sm->DeauthenticationRequest = FALSE;
}


SM_STATE(WPA_PTK, AUTHENTICATION)
{
    SM_ENTRY_MA(WPA_PTK, AUTHENTICATION, wpa_ptk);
    memset(&sm->PTK, 0, sizeof(sm->PTK));
    sm->PTK_valid = FALSE;
    sm->AuthenticationRequest = FALSE;
}


static void wpa_group_ensure_init(struct wpa_state_machine *sm, struct wpa_authenticator *wpa_auth,
                  struct wpa_group *group)
{
    if (group->first_sta_seen)
        return;
    /*
     * System has run bit further than at the time hostapd was started
     * potentially very early during boot up. This provides better chances
     * of collecting more randomness on embedded systems. Re-initialize the
     * GMK and Counter here to improve their strength if there was not
     * enough entropy available immediately after system startup.
     */
    wpa_printf( MSG_DEBUG, "WPA: Re-initialize GMK/Counter on first "
           "station\n");
    group->first_sta_seen = TRUE;

    wpa_gtk_update(wpa_auth, group);
    wpa_group_config_group_keys(wpa_auth, group);
}


SM_STATE(WPA_PTK, AUTHENTICATION2)
{
    SM_ENTRY_MA(WPA_PTK, AUTHENTICATION2, wpa_ptk);

    wpa_group_ensure_init(sm, sm->wpa_auth, sm->group);

    /*
     * Definition of ANonce selection in IEEE Std 802.11i-2004 is somewhat
     * ambiguous. The Authenticator state machine uses a counter that is
     * incremented by one for each 4-way handshake. However, the security
     * analysis of 4-way handshake points out that unpredictable nonces
     * help in preventing precomputation attacks. Instead of the state
     * machine definition, use an unpredictable nonce value here to provide
     * stronger protection against potential precomputation attacks.
     */
    if (os_get_random(sm->ANonce, WPA_NONCE_LEN)) {
        wpa_printf( MSG_ERROR, "WPA: Failed to get random data for "
               "ANonce.");
        wpa_sta_disconnect(sm->wpa_auth, sm->addr, sm->wpa_auth->vif_idx, sm->sta_idx);
        return;
    }
    wpa_hexdump(MSG_DEBUG, "WPA: Assign ANonce", sm->ANonce,
            WPA_NONCE_LEN);
    sm->ReAuthenticationRequest = FALSE;
    /* IEEE 802.11i does not clear TimeoutCtr here, but this is more
     * logical place than INITIALIZE since AUTHENTICATION2 can be
     * re-entered on ReAuthenticationRequest without going through
     * INITIALIZE. */
    sm->TimeoutCtr = 0;
}


SM_STATE(WPA_PTK, INITPSK)
{
    const u8 *psk;
    SM_ENTRY_MA(WPA_PTK, INITPSK, wpa_ptk);
    psk = wpa_auth_get_psk(sm->wpa_auth, sm->addr, NULL);
    if (psk) {
        memcpy(sm->PMK, psk, PMK_LEN);
    }
    sm->req_replay_counter_used = 0;
}


SM_STATE(WPA_PTK, PTKSTART)
{
    u8 *pmkid = NULL;
    size_t pmkid_len = 0;

    SM_ENTRY_MA(WPA_PTK, PTKSTART, wpa_ptk);
    sm->PTKRequest = FALSE;
    sm->TimeoutEvt = FALSE;

    sm->TimeoutCtr++;
    if (sm->TimeoutCtr > (int) dot11RSNAConfigPairwiseUpdateCount) {
        /* No point in sending the EAPOL-Key - we will disconnect
         * immediately following this. */
        return;
    }

    wpa_send_eapol(sm->wpa_auth, sm,
               WPA_KEY_INFO_ACK | WPA_KEY_INFO_KEY_TYPE, NULL,
               sm->ANonce, pmkid, pmkid_len, 0, 0);
}


static int wpa_derive_ptk(struct wpa_state_machine *sm, const u8 *pmk,
              struct wpa_ptk *ptk)
{
    size_t ptk_len = 48;

    wpa_pmk_to_ptk(pmk, PMK_LEN, "Pairwise key expansion",
               sm->wpa_auth->addr, sm->addr, sm->ANonce, sm->SNonce,
               (u8 *) ptk, ptk_len,
               wpa_key_mgmt_sha256(sm->wpa_key_mgmt));

    return 0;
}


SM_STATE(WPA_PTK, PTKCALCNEGOTIATING)
{
    struct wpa_ptk PTK;
    int ok = 0;
    const u8 *pmk = NULL;

    SM_ENTRY_MA(WPA_PTK, PTKCALCNEGOTIATING, wpa_ptk);
    sm->EAPOLKeyReceived = FALSE;
    sm->update_snonce = FALSE;

    /* WPA with IEEE 802.1X: use the derived PMK from EAP
     * WPA-PSK: iterate through possible PSKs and select the one matching
     * the packet */
    for (;;) {
        if (wpa_key_mgmt_wpa_psk(sm->wpa_key_mgmt)) {
            wpa_printf( MSG_DEBUG, "wpa psk\n");
            pmk = wpa_auth_get_psk(sm->wpa_auth, sm->addr, pmk);
            if (pmk == NULL){
                wpa_printf( MSG_DEBUG, "pmk is null\n");
                break;
            }
        } else {
            pmk = sm->PMK;
        }

        wpa_derive_ptk(sm, pmk, &PTK);

        if (wpa_verify_key_mic(&PTK, sm->last_rx_eapol_key,
                       sm->last_rx_eapol_key_len) == 0) {
            wpa_printf( MSG_DEBUG, "mic verify ok, pmk=%p\n", pmk);
            ok = 1;
            break;
        } else {
            wpa_printf( MSG_DEBUG, "mic verify fail, pmk=%p\n", pmk);
        }

        if (!wpa_key_mgmt_wpa_psk(sm->wpa_key_mgmt)){
            wpa_printf( MSG_DEBUG, "wpa_key_mgmt=%x\n", sm->wpa_key_mgmt);
            break;
        }
    }

    if (!ok) {
        return;
    }

    sm->pending_1_of_4_timeout = 0;

    if (wpa_key_mgmt_wpa_psk(sm->wpa_key_mgmt)) {
        /* PSK may have changed from the previous choice, so update
         * state machine data based on whatever PSK was selected here.
         */
        memcpy(sm->PMK, pmk, PMK_LEN);
    }

    sm->MICVerified = TRUE;

    memcpy(&sm->PTK, &PTK, sizeof(PTK));
    sm->PTK_valid = TRUE;
}


SM_STATE(WPA_PTK, PTKCALCNEGOTIATING2)
{
    SM_ENTRY_MA(WPA_PTK, PTKCALCNEGOTIATING2, wpa_ptk);
    sm->TimeoutCtr = 0;
}


SM_STATE(WPA_PTK, PTKINITNEGOTIATING)
{
    u8 rsc[WPA_KEY_RSC_LEN], *_rsc, *gtk, *kde, *pos;
    size_t gtk_len, kde_len;
    struct wpa_group *gsm = sm->group;
    u8 *wpa_ie;
    int wpa_ie_len, secure, keyidx, encr = 0;

    SM_ENTRY_MA(WPA_PTK, PTKINITNEGOTIATING, wpa_ptk);
    sm->TimeoutEvt = FALSE;

    sm->TimeoutCtr++;
    if (sm->TimeoutCtr > (int) dot11RSNAConfigPairwiseUpdateCount) {
        /* No point in sending the EAPOL-Key - we will disconnect
         * immediately following this. */
        return;
    }

    /* Send EAPOL(1, 1, 1, Pair, P, RSC, ANonce, MIC(PTK), RSNIE, [MDIE],
       GTK[GN], IGTK, [FTIE], [TIE * 2])
     */
    memset(rsc, 0, WPA_KEY_RSC_LEN);
    wpa_auth_get_seqnum(sm->wpa_auth, NULL, gsm->GN, rsc);
    /* If FT is used, wpa_auth->wpa_ie includes both RSNIE and MDIE */
    wpa_ie = sm->wpa_auth->wpa_ie;
    wpa_ie_len = sm->wpa_auth->wpa_ie_len;

    /* WPA2 send GTK in the 4-way handshake */
    secure = 1;
    gtk = gsm->GTK[gsm->GN - 1];
    gtk_len = gsm->GTK_len;
    keyidx = gsm->GN;
    _rsc = rsc;
    encr = 1;

    kde_len = wpa_ie_len;
    if (gtk)
        kde_len += 2 + RSN_SELECTOR_LEN + 2 + gtk_len;
    kde = (u8 *)os_malloc(kde_len);
    if (kde == NULL)
        return;

    pos = kde;
    memcpy(pos, wpa_ie, wpa_ie_len);
    pos += wpa_ie_len;
    if (gtk) {
        u8 hdr[2];
        hdr[0] = keyidx & 0x03;
        hdr[1] = 0;
        pos = wpa_add_kde(pos, RSN_KEY_DATA_GROUPKEY, hdr, 2,
                  gtk, gtk_len);
    }

    wpa_send_eapol(sm->wpa_auth, sm,
               (secure ? WPA_KEY_INFO_SECURE : 0) | WPA_KEY_INFO_MIC |
               WPA_KEY_INFO_ACK | WPA_KEY_INFO_INSTALL |
               WPA_KEY_INFO_KEY_TYPE,
               _rsc, sm->ANonce, kde, pos - kde, keyidx, encr);
    os_free(kde);
}


SM_STATE(WPA_PTK, PTKINITDONE)
{
    SM_ENTRY_MA(WPA_PTK, PTKINITDONE, wpa_ptk);
    sm->EAPOLKeyReceived = FALSE;
    if (sm->Pair) {
        enum wpa_alg alg = wpa_cipher_to_alg(sm->pairwise);
        int klen = wpa_cipher_key_len(sm->pairwise);
        if (wpa_auth_set_key(sm->wpa_auth, 0, sm->wpa_auth->vif_idx, sm->sta_idx, alg, sm->addr, 0,
                     sm->PTK.tk1, klen, true)) {
            wpa_sta_disconnect(sm->wpa_auth, sm->addr, sm->wpa_auth->vif_idx, sm->sta_idx);
            return;
        }
        /* FIX: MLME-SetProtection.Request(TA, Tx_Rx) */
        sm->pairwise_set = TRUE;

#ifdef CONFIG_AUTHENTICATOR_REKEY_WHEN_TIMEDOUT
        if (sm->wpa_auth->conf.wpa_ptk_rekey) {
            eloop_cancel_timeout(wpa_rekey_ptk, sm->wpa_auth, sm);
            eloop_register_timeout(sm->wpa_auth->conf.
                           wpa_ptk_rekey, 0, wpa_rekey_ptk,
                           sm->wpa_auth, sm);
        }
#endif
    }

    sm->has_GTK = TRUE;

    bl_wifi_wpa_ptk_init_done_internal(sm->sta_idx);
}


SM_STEP(WPA_PTK)
{

    if (sm->Init)
        SM_ENTER(WPA_PTK, INITIALIZE);
    else if (sm->Disconnect
         /* || FIX: dot11RSNAConfigSALifetime timeout */) {
        SM_ENTER(WPA_PTK, DISCONNECT);
    }
    else if (sm->DeauthenticationRequest)
        SM_ENTER(WPA_PTK, DISCONNECTED);
    else if (sm->AuthenticationRequest)
        SM_ENTER(WPA_PTK, AUTHENTICATION);
    else if (sm->ReAuthenticationRequest)
        SM_ENTER(WPA_PTK, AUTHENTICATION2);
    else if (sm->PTKRequest)
        SM_ENTER(WPA_PTK, PTKSTART);
    else switch (sm->wpa_ptk_state) {
    case WPA_PTK_INITIALIZE:
        break;
    case WPA_PTK_DISCONNECT:
        SM_ENTER(WPA_PTK, DISCONNECTED);
        break;
    case WPA_PTK_DISCONNECTED:
        SM_ENTER(WPA_PTK, INITIALIZE);
        break;
    case WPA_PTK_AUTHENTICATION:
        SM_ENTER(WPA_PTK, AUTHENTICATION2);
        break;
    case WPA_PTK_AUTHENTICATION2:
        if (wpa_key_mgmt_wpa_psk(sm->wpa_key_mgmt)
             /* FIX: && 802.1X::keyRun */)
            SM_ENTER(WPA_PTK, INITPSK);
        break;
    case WPA_PTK_INITPMK:
        SM_ENTER(WPA_PTK, DISCONNECT);
        break;
    case WPA_PTK_INITPSK:
        if (wpa_auth_get_psk(sm->wpa_auth, sm->addr, NULL))
            SM_ENTER(WPA_PTK, PTKSTART);
        else {
            SM_ENTER(WPA_PTK, DISCONNECT);
        }
        break;
    case WPA_PTK_PTKSTART:
        if (sm->EAPOLKeyReceived && !sm->EAPOLKeyRequest &&
            sm->EAPOLKeyPairwise)
            SM_ENTER(WPA_PTK, PTKCALCNEGOTIATING);
        else if (sm->TimeoutCtr >
             (int) dot11RSNAConfigPairwiseUpdateCount) {
            SM_ENTER(WPA_PTK, DISCONNECT);
        } else if (sm->TimeoutEvt)
            SM_ENTER(WPA_PTK, PTKSTART);
        break;
    case WPA_PTK_PTKCALCNEGOTIATING:
        if (sm->MICVerified)
            SM_ENTER(WPA_PTK, PTKCALCNEGOTIATING2);
        else if (sm->EAPOLKeyReceived && !sm->EAPOLKeyRequest &&
             sm->EAPOLKeyPairwise)
            SM_ENTER(WPA_PTK, PTKCALCNEGOTIATING);
        else if (sm->TimeoutEvt)
            SM_ENTER(WPA_PTK, PTKSTART);
        break;
    case WPA_PTK_PTKCALCNEGOTIATING2:
        SM_ENTER(WPA_PTK, PTKINITNEGOTIATING);
        break;
    case WPA_PTK_PTKINITNEGOTIATING:
        if (sm->update_snonce)
            SM_ENTER(WPA_PTK, PTKCALCNEGOTIATING);
        else if (sm->EAPOLKeyReceived && !sm->EAPOLKeyRequest &&
             sm->EAPOLKeyPairwise && sm->MICVerified)
            SM_ENTER(WPA_PTK, PTKINITDONE);
        else if (sm->TimeoutCtr >
             (int) dot11RSNAConfigPairwiseUpdateCount) {
            SM_ENTER(WPA_PTK, DISCONNECT);
        } else if (sm->TimeoutEvt)
            SM_ENTER(WPA_PTK, PTKINITNEGOTIATING);
        break;
    case WPA_PTK_PTKINITDONE:
        break;
    }
}


SM_STATE(WPA_PTK_GROUP, IDLE)
{
    SM_ENTRY_MA(WPA_PTK_GROUP, IDLE, wpa_ptk_group);
    if (sm->Init) {
        /* Init flag is not cleared here, so avoid busy
         * loop by claiming nothing changed. */
        sm->changed = FALSE;
    }
    sm->GTimeoutCtr = 0;
}


SM_STATE(WPA_PTK_GROUP, REKEYNEGOTIATING)
{
    u8 rsc[WPA_KEY_RSC_LEN];
    struct wpa_group *gsm = sm->group;
    u8 *kde, *pos, hdr[2];
    size_t kde_len;
    u8 *gtk;

    SM_ENTRY_MA(WPA_PTK_GROUP, REKEYNEGOTIATING, wpa_ptk_group);

    sm->GTimeoutCtr++;
    if (sm->GTimeoutCtr > (int) dot11RSNAConfigGroupUpdateCount) {
        /* No point in sending the EAPOL-Key - we will disconnect
         * immediately following this. */
        return;
    }

    sm->TimeoutEvt = FALSE;
    /* Send EAPOL(1, 1, 1, !Pair, G, RSC, GNonce, MIC(PTK), GTK[GN]) */
    memset(rsc, 0, WPA_KEY_RSC_LEN);
    if (gsm->wpa_group_state == WPA_GROUP_SETKEYSDONE)
        wpa_auth_get_seqnum(sm->wpa_auth, NULL, gsm->GN, rsc);

    gtk = gsm->GTK[gsm->GN - 1];
    kde_len = 2 + RSN_SELECTOR_LEN + 2 + gsm->GTK_len;
    kde = (u8 *)os_malloc(kde_len);
    if (kde == NULL)
        return;

    pos = kde;
    hdr[0] = gsm->GN & 0x03;
    hdr[1] = 0;
    pos = wpa_add_kde(pos, RSN_KEY_DATA_GROUPKEY, hdr, 2,
              gtk, gsm->GTK_len);

    wpa_send_eapol(sm->wpa_auth, sm,
               WPA_KEY_INFO_SECURE | WPA_KEY_INFO_MIC |
               WPA_KEY_INFO_ACK |
               (!sm->Pair ? WPA_KEY_INFO_INSTALL : 0),
               rsc, gsm->GNonce, kde, pos - kde, gsm->GN, 1);
    os_free(kde);  // NOLINT(clang-analyzer-unix.Malloc)
}


SM_STATE(WPA_PTK_GROUP, REKEYESTABLISHED)
{
    SM_ENTRY_MA(WPA_PTK_GROUP, REKEYESTABLISHED, wpa_ptk_group);
    sm->EAPOLKeyReceived = FALSE;
    if (sm->GUpdateStationKeys)
        sm->group->GKeyDoneStations--;
    sm->GUpdateStationKeys = FALSE;
    sm->GTimeoutCtr = 0;
    /* FIX: MLME.SetProtection.Request(TA, Tx_Rx) */
    sm->has_GTK = TRUE;
}


SM_STATE(WPA_PTK_GROUP, KEYERROR)
{
    SM_ENTRY_MA(WPA_PTK_GROUP, KEYERROR, wpa_ptk_group);
    if (sm->GUpdateStationKeys)
        sm->group->GKeyDoneStations--;
    sm->GUpdateStationKeys = FALSE;
    sm->Disconnect = TRUE;
}


SM_STEP(WPA_PTK_GROUP)
{
    if (sm->Init || sm->PtkGroupInit) {
        SM_ENTER(WPA_PTK_GROUP, IDLE);
        sm->PtkGroupInit = FALSE;
    } else switch (sm->wpa_ptk_group_state) {
    case WPA_PTK_GROUP_IDLE:
        if (sm->GUpdateStationKeys ||
            (sm->wpa == WPA_VERSION_WPA && sm->PInitAKeys))
            SM_ENTER(WPA_PTK_GROUP, REKEYNEGOTIATING);
        break;
    case WPA_PTK_GROUP_REKEYNEGOTIATING:
        if (sm->EAPOLKeyReceived && !sm->EAPOLKeyRequest &&
            !sm->EAPOLKeyPairwise && sm->MICVerified)
            SM_ENTER(WPA_PTK_GROUP, REKEYESTABLISHED);
        else if (sm->GTimeoutCtr >
             (int) dot11RSNAConfigGroupUpdateCount)
            SM_ENTER(WPA_PTK_GROUP, KEYERROR);
        else if (sm->TimeoutEvt)
            SM_ENTER(WPA_PTK_GROUP, REKEYNEGOTIATING);
        break;
    case WPA_PTK_GROUP_KEYERROR:
        SM_ENTER(WPA_PTK_GROUP, IDLE);
        break;
    case WPA_PTK_GROUP_REKEYESTABLISHED:
        SM_ENTER(WPA_PTK_GROUP, IDLE);
        break;
    }
}


static int wpa_gtk_update(struct wpa_authenticator *wpa_auth,
              struct wpa_group *group)
{
    int ret = 0;

    if (os_get_random(group->GTK[group->GN - 1], group->GTK_len)) {
        ret = -1;
    }
    wpa_hexdump_key(MSG_DEBUG, "GTK",
            group->GTK[group->GN - 1], group->GTK_len);

    return ret;
}


static void wpa_group_gtk_init(struct wpa_authenticator *wpa_auth,
                   struct wpa_group *group)
{
    wpa_printf( MSG_DEBUG, "WPA: group state machine entering state "
           "GTK_INIT (VLAN-ID %d)\n", group->vlan_id);
    group->changed = FALSE; /* GInit is not cleared here; avoid loop */
    group->wpa_group_state = WPA_GROUP_GTK_INIT;

    /* GTK[0..N] = 0 */
    memset(group->GTK, 0, sizeof(group->GTK));
    group->GN = 1;
    group->GM = 2;
    /* GTK[GN] = CalcGTK() */
    wpa_gtk_update(wpa_auth, group);
}


static void wpa_group_setkeys(struct wpa_authenticator *wpa_auth,
                  struct wpa_group *group)
{
    int tmp;

    wpa_printf( MSG_DEBUG, "WPA: group state machine entering state "
           "SETKEYS (VLAN-ID %d)\n", group->vlan_id);
    group->changed = TRUE;
    group->wpa_group_state = WPA_GROUP_SETKEYS;
    group->GTKReKey = FALSE;
    tmp = group->GM;
    group->GM = group->GN;
    group->GN = tmp;
    /* "GKeyDoneStations = GNoStations" is done in more robust way by
     * counting the STAs that are marked with GUpdateStationKeys instead of
     * including all STAs that could be in not-yet-completed state. */
    wpa_gtk_update(wpa_auth, group);

    if (group->GKeyDoneStations) {
        wpa_printf( MSG_DEBUG, "wpa_group_setkeys: Unexpected "
               "GKeyDoneStations=%d when starting new GTK rekey",
               group->GKeyDoneStations);
        group->GKeyDoneStations = 0;
    }
    wpa_printf( MSG_DEBUG, "wpa_group_setkeys: GKeyDoneStations=%d",
           group->GKeyDoneStations);
}


static int wpa_group_config_group_keys(struct wpa_authenticator *wpa_auth,
                       struct wpa_group *group)
{
    int ret = 0;
    if (wpa_auth_set_key(wpa_auth, group->vlan_id,
                 wpa_auth->vif_idx, 0,
                 wpa_cipher_to_alg(wpa_auth->conf.wpa_group),
                 (uint8_t *)broadcast_ether_addr, group->GN,
                 group->GTK[group->GN - 1], group->GTK_len, false) < 0)
        ret = -1;

    return ret;
}


static int wpa_group_setkeysdone(struct wpa_authenticator *wpa_auth,
                 struct wpa_group *group)
{
    wpa_printf( MSG_DEBUG, "WPA: group state machine entering state "
           "SETKEYSDONE (VLAN-ID %d)\n", group->vlan_id);
    group->changed = TRUE;
    group->wpa_group_state = WPA_GROUP_SETKEYSDONE;

    if (wpa_group_config_group_keys(wpa_auth, group) < 0)
        return -1;

    return 0;
}


static void wpa_group_sm_step(struct wpa_authenticator *wpa_auth,
                  struct wpa_group *group)
{
    if (group->GInit) {
        wpa_group_gtk_init(wpa_auth, group);
    } else if (group->wpa_group_state == WPA_GROUP_GTK_INIT &&
           group->GTKAuthenticator) {
        wpa_group_setkeysdone(wpa_auth, group);
    } else if (group->wpa_group_state == WPA_GROUP_SETKEYSDONE &&
           group->GTKReKey) {
        wpa_group_setkeys(wpa_auth, group);
    } else if (group->wpa_group_state == WPA_GROUP_SETKEYS) {
        if (group->GKeyDoneStations == 0)
            wpa_group_setkeysdone(wpa_auth, group);
        else if (group->GTKReKey)
            wpa_group_setkeys(wpa_auth, group);
    }
}


static int wpa_sm_step(struct wpa_state_machine *sm)
{
    if (sm->in_step_loop) {
        /* This should not happen, but if it does, make sure we do not
         * end up freeing the state machine too early by exiting the
         * recursive call. */
        wpa_printf( MSG_ERROR, "WPA: wpa_sm_step() called recursively");
        return 0;
    }

    sm->in_step_loop = 1;
    do {
        if (sm->pending_deinit)
            break;

        sm->changed = FALSE;
        sm->wpa_auth->group->changed = FALSE;

        SM_STEP_RUN(WPA_PTK);
        if (sm->pending_deinit)
            break;
        SM_STEP_RUN(WPA_PTK_GROUP);
        if (sm->pending_deinit)
            break;
        wpa_group_sm_step(sm->wpa_auth, sm->group);
    } while (sm->changed || sm->wpa_auth->group->changed);
    sm->in_step_loop = 0;

    if (sm->pending_deinit) {
        wpa_printf( MSG_DEBUG, "WPA: Completing pending STA state "
               "machine deinit for " MACSTR, MAC2STR(sm->addr));
        wpa_free_sta_sm(sm);
        return 1;
    }
    return 0;
}

bool wpa_ap_join(void **sm, uint8_t *mac, uint8_t *wpa_ie, uint8_t wpa_ie_len)
{
    struct hostapd_data *hapd = (struct hostapd_data*)bl_wifi_get_hostap_private_internal();
    struct wpa_state_machine **wpa_sm;
    struct wpa_state_machine *wpa_sm_new;

    if (!sm || !mac || !wpa_ie){
        return false;
    }

    wpa_sm = (struct wpa_state_machine **)sm;

    if (hapd) {
        if (hapd->wpa_auth->conf.wpa) {
            if (*wpa_sm){
                wpa_auth_sta_deinit(*wpa_sm);
                *wpa_sm = NULL;
            }

            wpa_sm_new = wpa_auth_sta_init(hapd->wpa_auth, mac);
            wpa_printf(MSG_DEBUG, "init wpa sm=%p\n", wpa_sm_new);

            if (wpa_sm_new == NULL) {
                return false;
            }

            if (wpa_validate_wpa_ie_rsn_ccmp_only(hapd->wpa_auth, wpa_sm_new, wpa_ie, wpa_ie_len)) {
                wpa_auth_sta_deinit(wpa_sm_new);
                return false;
            }
            *wpa_sm = wpa_sm_new;
        }
    }

    return true;
}

void wpa_ap_sta_associated(void *wpa_sm, uint8_t sta_idx)
{
    struct hostapd_data *hapd = (struct hostapd_data*)bl_wifi_get_hostap_private_internal();
    struct wpa_state_machine *sm = (struct wpa_state_machine *)wpa_sm;

    if (!hapd || !wpa_sm) {
        return;
    }

    sm->sta_idx = sta_idx;

    wpa_auth_sta_associated(hapd->wpa_auth, wpa_sm);
}

bool wpa_ap_remove(void *sm)
{
    struct wpa_state_machine *wpa_sm;
    if (!sm) return false;

    wpa_sm = (struct wpa_state_machine*)sm;
    wpa_auth_sta_deinit(wpa_sm);

    return true;
}
