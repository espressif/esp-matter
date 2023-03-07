/*
 * wpa_supplicant - WPA/RSN IE and KDE processing
 * Copyright (c) 2003-2008, Jouni Malinen <j@w1.fi>
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

#include "utils/includes.h"

#include "utils/common.h"
#include "rsn_supp/wpa.h"
#include "common/ieee802_11_defs.h"
#include "rsn_supp/wpa_ie.h"
#include "rsn_supp/pmksa_cache.h"

/**
 * wpa_parse_wpa_ie - Parse WPA/RSN IE
 * @wpa_ie: Pointer to WPA or RSN IE
 * @wpa_ie_len: Length of the WPA/RSN IE
 * @data: Pointer to data area for parsing results
 * Returns: 0 on success, -1 on failure
 *
 * Parse the contents of WPA or RSN IE and write the parsed data into data.
 */
int  wpa_parse_wpa_ie(const u8 *wpa_ie, size_t wpa_ie_len,
		     struct wpa_ie_data *data)
{
    if (wpa_ie_len >= 1 && wpa_ie[0] == WLAN_EID_RSN) {
        return wpa_parse_wpa_ie_rsn(wpa_ie, wpa_ie_len, data);
    } else if (wpa_ie[0] == WLAN_EID_WAPI) {
        return 0;
    }

    return wpa_parse_wpa_ie_wpa(wpa_ie, wpa_ie_len, data);
}


static int  wpa_gen_wpa_ie_wpa(u8 *wpa_ie, size_t wpa_ie_len,
                  int pairwise_cipher, int group_cipher,
                  int key_mgmt)
{
    u8 *pos;
    struct wpa_ie_hdr *hdr;

    if (wpa_ie_len < sizeof(*hdr) + WPA_SELECTOR_LEN +
        2 + WPA_SELECTOR_LEN + 2 + WPA_SELECTOR_LEN)
        return -1;

    hdr = (struct wpa_ie_hdr *) wpa_ie;
    hdr->elem_id = WLAN_EID_VENDOR_SPECIFIC;
    RSN_SELECTOR_PUT(hdr->oui, WPA_OUI_TYPE);
    WPA_PUT_LE16(hdr->version, WPA_VERSION);
    pos = (u8 *) (hdr + 1);

    if (group_cipher == WPA_CIPHER_CCMP) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_CCMP);
    } else if (group_cipher == WPA_CIPHER_TKIP) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_TKIP);
    } else if (group_cipher == WPA_CIPHER_WEP104) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_WEP104);
    } else if (group_cipher == WPA_CIPHER_WEP40) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_WEP40);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid group cipher (%d).",
               group_cipher);
        return -1;
    }
    pos += WPA_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;
    if (pairwise_cipher == WPA_CIPHER_CCMP) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_CCMP);
    } else if (pairwise_cipher == WPA_CIPHER_TKIP) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_TKIP);
    } else if (pairwise_cipher == WPA_CIPHER_NONE) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_NONE);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid pairwise cipher (%d).",
               pairwise_cipher);
        return -1;
    }
    pos += WPA_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;
    if (key_mgmt == WPA_KEY_MGMT_IEEE8021X) {
        RSN_SELECTOR_PUT(pos, WPA_AUTH_KEY_MGMT_UNSPEC_802_1X);
    } else if (key_mgmt == WPA_KEY_MGMT_PSK) {
        RSN_SELECTOR_PUT(pos, WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X);
    } else if (key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
        RSN_SELECTOR_PUT(pos, WPA_AUTH_KEY_MGMT_NONE);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid key management type (%d).",
               key_mgmt);
        return -1;
    }
    pos += WPA_SELECTOR_LEN;

    /* WPA Capabilities; use defaults, so no need to include it */

    hdr->len = (pos - wpa_ie) - 2;

    WPA_ASSERT((size_t) (pos - wpa_ie) <= wpa_ie_len);

    return pos - wpa_ie;
}


static int  wpa_gen_wpa_ie_rsn(u8 *rsn_ie, size_t rsn_ie_len,
                  int pairwise_cipher, int group_cipher,
                  int key_mgmt, int mgmt_group_cipher,
                  struct wpa_sm *sm)
{
#ifndef CONFIG_NO_WPA2
    u8 *pos;
    struct rsn_ie_hdr *hdr;
    u16 capab;
    u8 min_len = 0;

#ifdef CONFIG_PMKSA_CACHE
    if (rsn_ie_len < sizeof(*hdr) + RSN_SELECTOR_LEN +
            2 + RSN_SELECTOR_LEN + 2 + RSN_SELECTOR_LEN + 2 +
            (sm->cur_pmksa ? 2 + PMKID_LEN : 0)) {
        wpa_printf(MSG_DEBUG, "RSN: Too short IE buffer (%lu bytes)",
                (unsigned long) rsn_ie_len);
        return -1;
    }
#else
    if (rsn_ie_len < sizeof(*hdr) + RSN_SELECTOR_LEN +
            2 + RSN_SELECTOR_LEN + 2 + RSN_SELECTOR_LEN + 2) {
        wpa_printf(MSG_DEBUG, "RSN: Too short IE buffer (%lu bytes)",
                (unsigned long) rsn_ie_len);
        return -1;
    }
#endif

    /* For WPA2-PSK, if the RSNE in AP beacon/probe response doesn't specify the
     * pairwise cipher or AKM suite, the RSNE IE in association request
     * should only contain group cihpher suite, otherwise the WPA2 improvements
     * certification will fail.
     */
    if ( (sm->ap_notify_completed_rsne == true) || (key_mgmt == WPA_KEY_MGMT_IEEE8021X) ) {
        min_len = sizeof(*hdr) + RSN_SELECTOR_LEN + 2 + RSN_SELECTOR_LEN + 2 + RSN_SELECTOR_LEN + 2;
    } else {
        min_len = sizeof(*hdr) + RSN_SELECTOR_LEN;
    }

    if (rsn_ie_len < min_len) {
        wpa_printf(MSG_DEBUG, "RSN: Too short IE buffer (%lu bytes)", (unsigned long) rsn_ie_len);
    }

    hdr = (struct rsn_ie_hdr *) rsn_ie;
    hdr->elem_id = WLAN_EID_RSN;
    WPA_PUT_LE16(hdr->version, RSN_VERSION);
    pos = (u8 *) (hdr + 1);

    if (group_cipher == WPA_CIPHER_CCMP) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_CCMP);
    } else if (group_cipher == WPA_CIPHER_TKIP) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_TKIP);
    } else if (group_cipher == WPA_CIPHER_WEP104) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_WEP104);
    } else if (group_cipher == WPA_CIPHER_WEP40) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_WEP40);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid group cipher (%d).",
               group_cipher);
        return -1;
    }
    pos += RSN_SELECTOR_LEN;

    if ( (sm->ap_notify_completed_rsne == false) && (key_mgmt != WPA_KEY_MGMT_IEEE8021X) ) {
        hdr->len = (pos - rsn_ie) - 2;
        return (pos - rsn_ie);
    }

    *pos++ = 1;
    *pos++ = 0;
    if (pairwise_cipher == WPA_CIPHER_CCMP) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_CCMP);
    } else if (pairwise_cipher == WPA_CIPHER_TKIP) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_TKIP);
    } else if (pairwise_cipher == WPA_CIPHER_NONE) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_NONE);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid pairwise cipher (%d).",
               pairwise_cipher);
        return -1;
    }
    pos += RSN_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;
    if (key_mgmt == WPA_KEY_MGMT_IEEE8021X) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_UNSPEC_802_1X);
    } else if (key_mgmt == WPA_KEY_MGMT_PSK) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X);
#ifdef CONFIG_IEEE80211R
    } else if (key_mgmt == WPA_KEY_MGMT_FT_IEEE8021X) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_802_1X);
    } else if (key_mgmt == WPA_KEY_MGMT_FT_PSK) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_PSK);
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
    } else if (key_mgmt == WPA_KEY_MGMT_IEEE8021X_SHA256) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_802_1X_SHA256);
    } else if (key_mgmt == WPA_KEY_MGMT_PSK_SHA256) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_PSK_SHA256);
#ifdef CONFIG_WPA3_SAE
    } else if (key_mgmt == WPA_KEY_MGMT_SAE) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_SAE);
#endif /* CONFIG_WPA3_SAE */
#endif /* CONFIG_IEEE80211W */
    } else {
        wpa_printf(MSG_DEBUG, "Invalid key management type (%d).",
               key_mgmt);
        return -1;
    }
    pos += RSN_SELECTOR_LEN;

    /* RSN Capabilities */
    capab = 0;
#ifdef CONFIG_IEEE80211W
    if (sm->pmf_cfg.capable) {
        capab |= WPA_CAPABILITY_MFPC;
        if (sm->pmf_cfg.required || key_mgmt == WPA_KEY_MGMT_SAE) {
            capab |= WPA_CAPABILITY_MFPR;
        }
    }
#endif /* CONFIG_IEEE80211W */

    if (sm->spp_sup.capable) {
        capab |= WPA_CAPABILITY_SPP_CAPABLE;
    }

    if (sm->spp_sup.require) {
        capab |= WPA_CAPABILITY_SPP_REQUIRED;
    }

    WPA_PUT_LE16(pos, capab);
    pos += 2;

#ifdef CONFIG_PMKSA_CACHE
    if (sm->cur_pmksa) {
        /* PMKID Count (2 octets, little endian) */
        *pos++ = 1;
        *pos++ = 0;
        /* PMKID */
        os_memcpy(pos, sm->cur_pmksa->pmkid, PMKID_LEN);
        pos += PMKID_LEN;
    }
#endif

#ifdef CONFIG_IEEE80211W
    if (mgmt_group_cipher == WPA_CIPHER_AES_128_CMAC) {
#ifdef CONFIG_PMKSA_CACHE
        if (!sm->cur_pmksa) {
            /* 0 PMKID Count */
            WPA_PUT_LE16(pos, 0);
            pos += 2;
        }
#else
        WPA_PUT_LE16(pos, 0);
        pos += 2;
#endif

        /* Management Group Cipher Suite */
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_AES_128_CMAC);
        pos += RSN_SELECTOR_LEN;
    }
#endif /* CONFIG_IEEE80211W */

    hdr->len = (pos - rsn_ie) - 2;

    WPA_ASSERT((size_t) (pos - rsn_ie) <= rsn_ie_len);

    return pos - rsn_ie;
#else /* CONFIG_NO_WPA2 */
    return -1;
#endif /* CONFIG_NO_WPA2 */
}


/**
 * wpa_gen_wpa_ie - Generate WPA/RSN IE based on current security policy
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @wpa_ie: Pointer to memory area for the generated WPA/RSN IE
 * @wpa_ie_len: Maximum length of the generated WPA/RSN IE
 * Returns: Length of the generated WPA/RSN IE or -1 on failure
 */
int  wpa_gen_wpa_ie(struct wpa_sm *sm, u8 *wpa_ie, size_t wpa_ie_len)
{
    if (sm->proto == WPA_PROTO_RSN) {
        return wpa_gen_wpa_ie_rsn(wpa_ie, wpa_ie_len,
                      sm->pairwise_cipher,
                      sm->group_cipher,
                      sm->key_mgmt, sm->mgmt_group_cipher,
                      sm);
    } else if (sm->proto == WPA_PROTO_WAPI) {
        return 0;
    }

    return wpa_gen_wpa_ie_wpa(wpa_ie, wpa_ie_len,
                    sm->pairwise_cipher,
                    sm->group_cipher,
                    sm->key_mgmt);
}
