/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#if defined(MTK_MINICLI_ENABLE)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#include <cli.h>
#include "minisupp_cli.h"

#ifdef MTK_WIFI_DIRECT_ENABLE
#include "p2p_supplicant.h"
#include "p2p.h"
#endif

#include "wpa_supplicant_task.h"
#include "wifi_api.h"
#include "string.h"

/* porting UT with LWIP */
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"

#if (defined(__CC_ARM)) || (defined(__ICCARM__))
#include "includes.h"
#endif
#include "driver.h"
#include "list.h"
#include "../wpa_supplicant/wpa_supplicant_i.h"
#include "../wpa_supplicant/config_wpa_supplicant.h"
#include "eapol_supp/eapol_supp_sm.h"
#include "common/ieee802_11_defs.h"
#include "common/wpa_common.h"
#include "../wpa_supplicant/bss_wpa_supplicant.h"
#include "../wpa_supplicant/scan_wpa_supplicant.h"
#include "../wpa_supplicant/notify.h"
#include "../wpa_supplicant/ap.h"
#include "../wpa_supplicant/driver_i.h"
#include "utils/eloop.h"
#include "rsn_supp/wpa.h"

#include "nvdm.h"
#include "gl_upperlayer.h"

extern int32_t supplicant_ready_query(uint8_t init_ready, uint8_t opmode_ready, uint32_t timeout);
extern void wpa_supplicant_entry_debug_level(int debug_level);
extern int wpa_debug_level;
extern char *sta_ifname;
extern char *ap_ifname;
extern struct wpa_global *global_entry;
extern const char *ctrl_interface;
extern struct wpa_supplicant *g_wpa_s;
extern uint8_t g_ap_created;
#ifdef CONFIG_AP
extern struct netif ap_netif;
#endif
#define SUPPLICANT_MAJOR_VERSION    (2)
#define SUPPLICANT_MINOR_VERSION    (10)
#define SUPP_FEATURE_MAJOR_VERSION  (2)
#define SUPP_FEATURE_MINOR_VERSION  (0)
#define SUPP_FEATURE_SERIAL_VERSION (0)
#ifdef SuppBuildTime
char minisupp_build_date_time[] = SuppBuildTime;
#else
char minisupp_build_date_time[] = "----\0";
#endif
#ifdef SuppTOTShaXDig
char minisupp_tot_sha[] = SuppTOTShaXDig;
#else
char minisupp_tot_sha[] = "deadbeef\0";
#endif


/* FreeRTOS porting minisupp alone */
#if CFG_DEBUG_SETTING /* driver_none_level_output */
static err_t driver_none_level_output(struct netif *netif, struct pbuf *p)
{
    wpa_printf(MSG_ERROR, "%s>", __func__);
    return 0;
}
#endif /* driver_none_level_output */

#if CFG_DEBUG_SETTING /* driver_none_ip4_output */
static err_t driver_none_ip4_output(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
    wpa_printf(MSG_ERROR, "%s>", __func__);
    return 0;
}
#endif /* driver_none_ip4_output */

#if LWIP_IPV6
static err_t driver_none_ip6_output(struct netif *netif, struct pbuf *p, const ip6_addr_t *ipaddr)
{
    wpa_printf(MSG_ERROR, "%s>", __func__);
    return 0;
}
#endif

#if CFG_DEBUG_SETTING /* driver_none_netif_init */
static err_t driver_none_netif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->state = NULL;
    netif->name[0] = 'u';
    netif->name[1] = 't';
    /* We directly use etharp_output() here to save a function call.
        * You can instead declare your own function an call etharp_output()
        * from it if you have to do some checks before sending (e.g. if link
        * is available...) */
    netif->output = driver_none_ip4_output;
#if LWIP_IPV6
    netif->output_ip6 = driver_none_ip6_output;
#endif /* LWIP_IPV6 */

    netif->linkoutput = (netif_linkoutput_fn)driver_none_level_output;
    return 0;
}
#endif /* driver_none_netif_init */

#if CFG_DEBUG_SETTING /* _minisupp_ut_lwip_init */
static uint8_t _minisupp_ut_lwip_init(uint8_t len, char *param[])
{
    /* KOKO::TODO freertos UT w/o driver
     * init sta_if & ap_if
     */
    lwip_socket_init();
    tcpip_init(NULL, NULL);

#if CFG_DEBUG_SETTING_ADDR
    /* set MAC hardware address length */
    sta_if.hwaddr_len = ETHARP_HWADDR_LEN;
    ap_if.hwaddr_len = ETHARP_HWADDR_LEN;

    netif_add(&sta_if, NULL, IP4_ADDR_ANY, IP4_ADDR_ANY, NULL, driver_none_netif_init, tcpip_input);
    netif_add(&ap_if, NULL, IP4_ADDR_ANY, IP4_ADDR_ANY, NULL, driver_none_netif_init, tcpip_input);
#endif

    return 0;
}
#endif /* _minisupp_ut_lwip_init */

#if CFG_DEBUG_SETTING /* _minisupp_cli_show_interfaces */
static uint8_t _minisupp_cli_show_interfaces(uint8_t len, char *param[])
{
    /* mtk_supplicant_show_interfaces(); */
    return 0;
}
#endif /* _minisupp_cli_show_interfaces */

static uint8_t _minisupp_cli_set_debug_level(uint8_t len, char *param[])
{
    wpa_supplicant_entry_debug_level(atoi(param[0]));
    return 0;
}
static uint8_t _minisupp_cli_get_debug_level(uint8_t len, char *param[])
{
    wpa_printf(MSG_DEBUG, "supplicant log level %d", wpa_debug_level);
    return 0;
}

uint8_t _minisupp_cli_get_ver(uint8_t len, char *param[])
{
    wpa_printf(MSG_INFO, "minisupp ver: %u.%u\n",
               SUPPLICANT_MAJOR_VERSION,
               SUPPLICANT_MINOR_VERSION);
    wpa_printf(MSG_INFO, "build time: %s\n",
               minisupp_build_date_time);
    wpa_printf(MSG_INFO, "SDK-%u.%u.%u (%s)\n",
               SUPP_FEATURE_MAJOR_VERSION,
               SUPP_FEATURE_MINOR_VERSION,
               SUPP_FEATURE_SERIAL_VERSION,
               minisupp_tot_sha);

    return 0;
}

uint8_t _minisupp_cli_init(uint8_t len, char *param[])
{
    wifi_config_t config;

    memset(&config, 0, sizeof(config));
    wpa_printf(MSG_ERROR, "%s", __func__);
    wpa_supplicant_task_init(&config, NULL);
    return 0;
}

static uint8_t _minisupp_cli_deinit(uint8_t len, char *param[])
{
    eloop_terminate();
    wpa_printf(MSG_ERROR, "%s: start to deinit", __func__);
    return 0;
}

static uint8_t _minisupp_cli_req_scan(uint8_t len, char *param[])
{
    int sec = 0;
    int usec = 0;
    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);

    if (len > 2) {
        wpa_printf(MSG_ERROR, "LEN(CMD): %d", len);
        return -1;
    }

    if (len >= 1) {
        wpa_printf(MSG_DEBUG, "PARAM[0]: %s", param[0]);
        sec = atoi(param[0]);
    }
    if (len == 2) {
        wpa_printf(MSG_DEBUG, "PARAM[1]: %s", param[1]);
        usec = atoi(param[1]);
    }

    wpa_printf(MSG_ERROR, "%s> wpa_s %p sec %d usec %d", __func__, wpa_s, sec, usec);
    if (wpa_s)
        wpa_supplicant_req_scan(wpa_s, sec, usec);
    return 0;
}

static uint8_t _minisupp_cli_stop_scan(uint8_t len, char *param[])
{
    supplicant_ready_query(TRUE, TRUE, 3000);

    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
    if (wpa_s) {
        wpa_supplicant_cancel_sched_scan(wpa_s);
        wpa_supplicant_cancel_scan(wpa_s);
    }
    return 0;
}

uint8_t _minisupp_cli_add_network(uint8_t len, char *param[])
{
    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
    struct wpa_ssid *ssid;

    wpa_printf(MSG_DEBUG, "CTRL_IFACE: ADD_NETWORK");

    if (wpa_s == NULL)
        return -1;

    ssid = wpa_config_add_network(wpa_s->conf);
    if (ssid == NULL)
        return -1;
    wpa_printf(MSG_DEBUG, "CTRL_IFACE: ID = %d\n", ssid->id);
#ifdef CONFIG_CTRL_IFACE_DBUS_NEW
    wpas_notify_network_added(wpa_s, ssid);
#endif /* CONFIG_CTRL_IFACE_DBUS_NEW */
    /* Scan hidden AP */
    ssid->scan_ssid = 1;
    ssid->disabled = 1;
    wpa_config_set_network_defaults(ssid);

    return 0;
}

uint8_t _minisupp_cli_enable_network(uint8_t len, char *param[])
{
    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
    int id;
    struct wpa_ssid *ssid;

    if (wpa_s == NULL)
        return -1;

    if (len > 1) {
        wpa_printf(MSG_ERROR, "LEN(CMD): %d", len);
        return -1;
    }
    wpa_printf(MSG_DEBUG, "CMD: %s", param[0]);

    /* cmd: "<network id>" or "all" */
    if (os_strcmp(param[0], "all") == 0) {
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: ENABLE_NETWORK all");
        ssid = NULL;
    } else {
        id = atoi(param[0]);
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: ENABLE_NETWORK id=%d", id);

        ssid = wpa_config_get_network(wpa_s->conf, id);
        if (ssid == NULL) {
            wpa_printf(MSG_DEBUG, "CTRL_IFACE: Could not find "
                       "network id=%d", id);
            return -1;
        }
        if (ssid->disabled == 2) {
            wpa_printf(MSG_DEBUG, "CTRL_IFACE: Cannot use "
                       "ENABLE_NETWORK with persistent P2P group");
            return -1;
        }

        if (os_strstr(param[0], " no-connect")) {
            ssid->disabled = 0;
            return 0;
        }
    }
    if (ssid)
        mtk_freertos_wpa_set_favor_ssid(wpa_s, ssid->ssid, ssid->ssid_len);
    wpa_s->scan_min_time.sec = 0;
    wpa_s->scan_min_time.usec = 0;
    wpa_supplicant_enable_network(wpa_s, ssid);

    return 0;
}

uint8_t _minisupp_cli_disable_network(uint8_t len, char *param[])
{
    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
    int id;
    struct wpa_ssid *ssid;

    if (wpa_s == NULL)
        return -1;

    if (len > 1) {
        wpa_printf(MSG_ERROR, "LEN(CMD): %d", len);
        return -1;
    }
    wpa_printf(MSG_DEBUG, "CMD: %s", param[0]);

    /* cmd: "<network id>" or "all" */
    if (os_strcmp(param[0], "all") == 0) {
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: DISABLE_NETWORK all");
        ssid = NULL;
    } else {
        id = atoi(param[0]);
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: DISABLE_NETWORK id=%d", id);

        ssid = wpa_config_get_network(wpa_s->conf, id);
        if (ssid == NULL) {
            wpa_printf(MSG_DEBUG, "CTRL_IFACE: Could not find "
                       "network id=%d", id);
            return -1;
        }
        if (ssid->disabled == 2) {
            wpa_printf(MSG_DEBUG, "CTRL_IFACE: Cannot use "
                       "DISABLE_NETWORK with persistent P2P "
                       "group");
            return -1;
        }
    }
    wpa_supplicant_disable_network(wpa_s, ssid);

    return 0;
}

uint8_t _minisupp_cli_remove_network(uint8_t len, char *param[])
{
    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
    int id;
    struct wpa_ssid *ssid;
    int was_disabled;

    if (wpa_s == NULL)
        return -1;

    if (len > 1) {
        wpa_printf(MSG_ERROR, "LEN(CMD): %d", len);
        return -1;
    }
    wpa_printf(MSG_DEBUG, "CMD: %s", param[0]);
    /* cmd: "<network id>" or "all" */
    if (os_strcmp(param[0], "all") == 0) {
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: REMOVE_NETWORK all");
        if (wpa_s->sched_scanning)
            wpa_supplicant_cancel_sched_scan(wpa_s);

        eapol_sm_invalidate_cached_session(wpa_s->eapol);
        if (wpa_s->current_ssid) {
#ifdef CONFIG_SME
            wpa_s->sme.prev_bssid_set = 0;
#endif /* CONFIG_SME */
            wpa_sm_set_config(wpa_s->wpa, NULL);
            eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);
            if (wpa_s->wpa_state >= WPA_AUTHENTICATING)
                wpa_s->own_disconnect_req = 1;
            wpa_supplicant_deauthenticate(
                wpa_s, WLAN_REASON_DEAUTH_LEAVING);
        }
        ssid = wpa_s->conf->ssid;
        while (ssid) {
            struct wpa_ssid *remove_ssid = ssid;
            id = ssid->id;
            ssid = ssid->next;
            if (wpa_s->last_ssid == remove_ssid)
                wpa_s->last_ssid = NULL;
            wpas_notify_network_removed(wpa_s, remove_ssid);
            wpa_config_remove_network(wpa_s->conf, id);
        }
        return 0;
    }

    id = atoi(param[0]);
    wpa_printf(MSG_DEBUG, "CTRL_IFACE: REMOVE_NETWORK id=%d", id);

    ssid = wpa_config_get_network(wpa_s->conf, id);
    if (ssid)
        wpas_notify_network_removed(wpa_s, ssid);
    if (ssid == NULL) {
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: Could not find network "
                   "id=%d", id);
        return -1;
    }

    if (wpa_s->last_ssid == ssid)
        wpa_s->last_ssid = NULL;

    if (ssid == wpa_s->current_ssid || wpa_s->current_ssid == NULL) {
#ifdef CONFIG_SME
        wpa_s->sme.prev_bssid_set = 0;
#endif /* CONFIG_SME */
        /*
         * Invalidate the EAP session cache if the current or
         * previously used network is removed.
         */
        eapol_sm_invalidate_cached_session(wpa_s->eapol);
    }

    if (ssid == wpa_s->current_ssid) {
        wpa_sm_set_config(wpa_s->wpa, NULL);
        eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);

        if (wpa_s->wpa_state >= WPA_AUTHENTICATING)
            wpa_s->own_disconnect_req = 1;
        wpa_supplicant_deauthenticate(wpa_s,
                                      WLAN_REASON_DEAUTH_LEAVING);
    }

    was_disabled = ssid->disabled;

    if (wpa_config_remove_network(wpa_s->conf, id) < 0) {
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: Not able to remove the "
                   "network id=%d", id);
        return -1;
    }

    if (!was_disabled && wpa_s->sched_scanning) {
        wpa_printf(MSG_DEBUG, "Stop ongoing sched_scan to remove "
                   "network from filters");
        wpa_supplicant_cancel_sched_scan(wpa_s);
        wpa_supplicant_req_scan(wpa_s, 0, 0);
    }

    return 0;
}

uint8_t _minisupp_cli_set_sae_groups(uint8_t len, char *param[])
{
    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
    int *groups;
    int i;

    if (len > 10) {
        wpa_printf(MSG_DEBUG, "len %d > 10, is too long, fail", len);
        return -1;
    }

    wpa_printf(MSG_DEBUG, "len %d, SAE GROUPS", len);

    groups = os_calloc(10, sizeof(int));
    if (groups == NULL) {
        wpa_printf(MSG_ERROR, "Ctrl Iface: Invalid sae_groups");
        return -1;
    }

    for (i = 0; i < len; i++) {
        wpa_printf(MSG_DEBUG, "SAE GROUPS %s", param[i]);
        groups[i] = atoi(param[i]);
    }

    os_free(wpa_s->conf->sae_groups);
    wpa_s->conf->sae_groups = groups;

    wpa_printf(MSG_DEBUG, "SAE GROUPS %d %d %d", wpa_s->conf->sae_groups[0],
               wpa_s->conf->sae_groups[1], wpa_s->conf->sae_groups[2]);

    return 0;
}

uint8_t _minisupp_cli_get_sae_groups(uint8_t len, char *param[])
{
    wpa_printf(MSG_INFO, "SAE GROUPS 19 20 21");
    return 0;
}

uint8_t _minisupp_cli_select_network(uint8_t len, char *param[])
{
    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
    int id;
    struct wpa_ssid *ssid;
#if CFG_DEBUG_SETTING_FREQ
    char *pos;
#endif

    if (wpa_s == NULL)
        return -1;

    if (len > 1) {
        wpa_printf(MSG_ERROR, "LEN(CMD): %d", len);
        return -1;
    }
    wpa_printf(MSG_DEBUG, "CMD: %s", param[0]);
    /* cmd: "<network id>" or "any" */
    if (os_strncmp(param[0], "any", 3) == 0) {
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: SELECT_NETWORK any");
        ssid = NULL;
    } else {
        id = atoi(param[0]);
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: SELECT_NETWORK id=%d", id);

        ssid = wpa_config_get_network(wpa_s->conf, id);
        if (ssid == NULL) {
            wpa_printf(MSG_DEBUG, "CTRL_IFACE: Could not find "
                       "network id=%d", id);
            return -1;
        }
        if (ssid->disabled == 2) {
            wpa_printf(MSG_DEBUG, "CTRL_IFACE: Cannot use "
                       "SELECT_NETWORK with persistent P2P group");
            return -1;
        }
    }

#if CFG_DEBUG_SETTING_FREQ
    pos = os_strstr(param[0], " freq=");
    if (pos) {
        int *freqs = freq_range_to_channel_list(wpa_s, pos + 6);
        if (freqs) {
            wpa_s->scan_req = MANUAL_SCAN_REQ;
            os_free(wpa_s->manual_scan_freqs);
            wpa_s->manual_scan_freqs = freqs;
        }
    }
#endif

    wpa_s->scan_min_time.sec = 0;
    wpa_s->scan_min_time.usec = 0;
    wpa_s->conf->ap_scan = 1; // Kevin, dbg
    wpa_supplicant_select_network(wpa_s, ssid);

    return 0;
}

static int _minisupp_cli_update_network(
    struct wpa_supplicant *wpa_s, struct wpa_ssid *ssid,
    char *name, char *value)
{
    int ret;

    ret = wpa_config_set(ssid, name, value, 0);
    if (ret < 0) {
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: Failed to set network "
                   "variable '%s'", name);
        return -1;
    }
    if (ret == 1)
        return 0; /* No change to the previously configured value */

    if (os_strcmp(name, "bssid") != 0 &&
        os_strcmp(name, "priority") != 0) {
        wpa_sm_pmksa_cache_flush(wpa_s->wpa, ssid);

        if (wpa_s->current_ssid == ssid ||
            wpa_s->current_ssid == NULL) {
            /*
             * Invalidate the EAP session cache if anything in the
             * current or previously used configuration changes.
             */
            eapol_sm_invalidate_cached_session(wpa_s->eapol);
        }
    }

    if ((os_strcmp(name, "psk") == 0 &&
         value[0] == '"' && ssid->ssid_len) ||
        (os_strcmp(name, "ssid") == 0 && ssid->passphrase))
        wpa_config_update_psk(ssid);
    else if (os_strcmp(name, "priority") == 0)
        wpa_config_update_prio_list(wpa_s->conf);

    return 0;
}

uint8_t _minisupp_cli_set_network(uint8_t len, char *param[])
{
    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
    int id, ret, prev_bssid_set, prev_disabled;
    struct wpa_ssid *ssid;
    char *name, *value;
    u8 prev_bssid[ETH_ALEN];
    char buf[64];

    if (wpa_s == NULL)
        return -1;

    if (len < 3) {
        wpa_printf(MSG_ERROR, "LEN(CMD): %d", len);
        return -1;
    }
    /* cmd: "<network id> <variable name> <value>" */
    name = param[1];

    if (len > 3) {
        if (snprintf(buf, sizeof(buf), "%s", param[2]) < 0) {
            wpa_printf(MSG_ERROR, "LEN(CMD): %d", len);
            return -1;
        }
        for (int i = 3; i < len; ++i) {
            if (snprintf(buf + strlen(buf), sizeof(buf) -
                         strlen(buf), " %s", param[i]) < 0) {
                wpa_printf(MSG_ERROR, "LEN(CMD): %d", len);
                return -1;
            }
        }
        value = buf;
    } else {
        value = param[2];
    }

    id = atoi(param[0]);
    wpa_printf(MSG_DEBUG, "CTRL_IFACE: SET_NETWORK id=%d name='%s'",
               id, name);
    wpa_hexdump_ascii_key(MSG_DEBUG, "CTRL_IFACE: value",
                          (u8 *) value, os_strlen(value));

    ssid = wpa_config_get_network(wpa_s->conf, id);
    if (ssid == NULL) {
        wpa_printf(MSG_DEBUG, "CTRL_IFACE: Could not find network "
                   "id=%d", id);
        return -1;
    }

    prev_bssid_set = ssid->bssid_set;
    prev_disabled = ssid->disabled;
    os_memcpy(prev_bssid, ssid->bssid, ETH_ALEN);
    ret = _minisupp_cli_update_network(wpa_s, ssid, name,
                                       value);
    if (ret == 0 &&
        (ssid->bssid_set != prev_bssid_set ||
         os_memcmp(ssid->bssid, prev_bssid, ETH_ALEN) != 0))
        wpas_notify_network_bssid_set_changed(wpa_s, ssid);

    if (prev_disabled != ssid->disabled &&
        (prev_disabled == 2 || ssid->disabled == 2))
        wpas_notify_network_type_changed(wpa_s, ssid);

    return ret;
}


uint8_t _minisupp_cli_iface_list_networks(uint8_t len, char *param[])
{
    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
    char *pos, *end;
    struct wpa_ssid *ssid;
    char buf[128];
    size_t buflen = 128;
    int ret;

    pos = buf;
    end = buf + buflen;
    wpa_printf(MSG_ERROR, "network id / ssid / bssid / flags");

    if (wpa_s == NULL)
        return -1;

    ssid = wpa_s->conf->ssid;

    while (ssid) {
        pos = buf;
        end = buf + buflen;
        ret = os_snprintf(pos, end - pos, "%d\t%s",
                          ssid->id,
                          wpa_ssid_txt(ssid->ssid, ssid->ssid_len));
        if (os_snprintf_error(end - pos, ret))
            break;
        pos += ret;
        if (ssid->bssid_set) {
            ret = os_snprintf(pos, end - pos, "\t" MACSTR,
                              MAC2STR(ssid->bssid));
        } else {
            ret = os_snprintf(pos, end - pos, "\tany");
        }
        if (os_snprintf_error(end - pos, ret))
            break;
        pos += ret;
        ret = os_snprintf(pos, end - pos, "\t%s%s%s%s",
                          ssid == wpa_s->current_ssid ?
                          "[CURRENT]" : "",
                          ssid->disabled ? "[DISABLED]" : "",
                          ssid->disabled_until.sec ?
                          "[TEMP-DISABLED]" : "",
                          ssid->disabled == 2 ? "[P2P-PERSISTENT]" :
                          "");
        if (os_snprintf_error(end - pos, ret))
            break;
        pos += ret;
        ret = os_snprintf(pos, end - pos, "\n");
        if (os_snprintf_error(end - pos, ret))
            break;
        pos += ret;
        wpa_printf(MSG_ERROR, "%s", buf);
        ssid = ssid->next;
    }

    return 0;
}

static char *wpa_supplicant_cipher_txt(char *pos, char *end, int cipher)
{
    int ret;
    ret = os_snprintf(pos, end - pos, "-");
    if (os_snprintf_error(end - pos, ret))
        return pos;
    pos += ret;
    ret = wpa_write_ciphers(pos, end, cipher, "+");
    if (ret < 0)
        return pos;
    pos += ret;
    return pos;
}

static char *wpa_supplicant_ie_txt(char *pos, char *end, const char *proto,
                                   const u8 *ie, size_t ie_len)
{
    struct wpa_ie_data data;
    char *start;
    int ret;

    ret = os_snprintf(pos, end - pos, "[%s-", proto);
    if (os_snprintf_error(end - pos, ret))
        return pos;
    pos += ret;

    if (wpa_parse_wpa_ie(ie, ie_len, &data) < 0) {
        ret = os_snprintf(pos, end - pos, "?]");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
        return pos;
    }

    start = pos;
    if (data.key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
        ret = os_snprintf(pos, end - pos, "%sEAP",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
    if (data.key_mgmt & WPA_KEY_MGMT_PSK) {
        ret = os_snprintf(pos, end - pos, "%sPSK",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
    if (data.key_mgmt & WPA_KEY_MGMT_WPA_NONE) {
        ret = os_snprintf(pos, end - pos, "%sNone",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
    if (data.key_mgmt & WPA_KEY_MGMT_SAE) {
        ret = os_snprintf(pos, end - pos, "%sSAE",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
#ifdef CONFIG_IEEE80211R
    if (data.key_mgmt & WPA_KEY_MGMT_FT_IEEE8021X) {
        ret = os_snprintf(pos, end - pos, "%sFT/EAP",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
    if (data.key_mgmt & WPA_KEY_MGMT_FT_PSK) {
        ret = os_snprintf(pos, end - pos, "%sFT/PSK",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
    if (data.key_mgmt & WPA_KEY_MGMT_FT_SAE) {
        ret = os_snprintf(pos, end - pos, "%sFT/SAE",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
    if (data.key_mgmt & WPA_KEY_MGMT_IEEE8021X_SHA256) {
        ret = os_snprintf(pos, end - pos, "%sEAP-SHA256",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
    if (data.key_mgmt & WPA_KEY_MGMT_PSK_SHA256) {
        ret = os_snprintf(pos, end - pos, "%sPSK-SHA256",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
#endif /* CONFIG_IEEE80211W */

#ifdef CONFIG_SUITEB
    if (data.key_mgmt & WPA_KEY_MGMT_IEEE8021X_SUITE_B) {
        ret = os_snprintf(pos, end - pos, "%sEAP-SUITE-B",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
#endif /* CONFIG_SUITEB */

#ifdef CONFIG_SUITEB192
    if (data.key_mgmt & WPA_KEY_MGMT_IEEE8021X_SUITE_B_192) {
        ret = os_snprintf(pos, end - pos, "%sEAP-SUITE-B-192",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }
#endif /* CONFIG_SUITEB192 */

    if (data.key_mgmt & WPA_KEY_MGMT_OSEN) {
        ret = os_snprintf(pos, end - pos, "%sOSEN",
                          pos == start ? "" : "+");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }

    pos = wpa_supplicant_cipher_txt(pos, end, data.pairwise_cipher);

    if (data.capabilities & WPA_CAPABILITY_PREAUTH) {
        ret = os_snprintf(pos, end - pos, "-preauth");
        if (os_snprintf_error(end - pos, ret))
            return pos;
        pos += ret;
    }

    ret = os_snprintf(pos, end - pos, "]");
    if (os_snprintf_error(end - pos, ret))
        return pos;
    pos += ret;

    return pos;
}

/* Format one result on one text line into a buffer. */
static int wpa_supplicant_ctrl_iface_scan_result(
    struct wpa_supplicant *wpa_s,
    const struct wpa_bss *bss, char *buf, size_t buflen)
{
    char *pos, *end;
    int ret;
    const u8 *ie, *ie2, *osen_ie = NULL;
#if CFG_DEBUG_SETTING_MESH
    const u8 *p2p, *mesh;

    mesh = wpa_bss_get_ie(bss, WLAN_EID_MESH_ID);
    p2p = wpa_bss_get_vendor_ie(bss, P2P_IE_VENDOR_TYPE);
    if (!p2p)
        p2p = wpa_bss_get_vendor_ie_beacon(bss, P2P_IE_VENDOR_TYPE);
    if (p2p && bss->ssid_len == P2P_WILDCARD_SSID_LEN &&
        os_memcmp(bss->ssid, P2P_WILDCARD_SSID, P2P_WILDCARD_SSID_LEN) ==
        0)
        return 0; /* Do not show P2P listen discovery results here */
#endif

    pos = buf;
    end = buf + buflen;

    ret = os_snprintf(pos, end - pos, MACSTR "\t%d\t%d\t",
                      MAC2STR(bss->bssid), bss->freq, bss->level);
    if (os_snprintf_error(end - pos, ret))
        return -1;
    pos += ret;
    ie = wpa_bss_get_vendor_ie(bss, WPA_IE_VENDOR_TYPE);

    /* KOKO TODO: save some space */
    if (ie)
        pos = wpa_supplicant_ie_txt(pos, end, "WPA", ie, 2 + ie[1]);

    ie2 = wpa_bss_get_ie(bss, WLAN_EID_RSN);
    if (ie2)
        pos = wpa_supplicant_ie_txt(pos, end, "WPA2",
                                    ie2, 2 + ie2[1]);
#if CFG_DEBUG_SETTING_OSEN
    osen_ie = wpa_bss_get_vendor_ie(bss, OSEN_IE_VENDOR_TYPE);

    if (osen_ie)
        pos = wpa_supplicant_ie_txt(pos, end, "OSEN",
                                    osen_ie, 2 + osen_ie[1]);
    /* KOKO TODO: save some space */
    pos = wpa_supplicant_wps_ie_txt(wpa_s, pos, end, bss);
#endif
    if (!ie && !ie2 && !osen_ie && (bss->caps & IEEE80211_CAP_PRIVACY)) {
        ret = os_snprintf(pos, end - pos, "[WEP]");
        if (os_snprintf_error(end - pos, ret))
            return -1;
        pos += ret;
    }

#if CFG_DEBUG_SETTING_MESH
    if (mesh) {
        ret = os_snprintf(pos, end - pos, "[MESH]");
        if (os_snprintf_error(end - pos, ret))
            return -1;
        pos += ret;
    }
#endif
#ifdef CONFIG_IEEE80211AD
    if (bss_is_dmg(bss)) {
        const char *s;
        ret = os_snprintf(pos, end - pos, "[DMG]");
        if (os_snprintf_error(end - pos, ret))
            return -1;
        pos += ret;
        switch (bss->caps & IEEE80211_CAP_DMG_MASK) {
            case IEEE80211_CAP_DMG_IBSS:
                s = "[IBSS]";
                break;
            case IEEE80211_CAP_DMG_AP:
                s = "[ESS]";
                break;
            case IEEE80211_CAP_DMG_PBSS:
                s = "[PBSS]";
                break;
            default:
                s = "";
                break;
        }
        ret = os_snprintf(pos, end - pos, "%s", s);
        if (os_snprintf_error(end - pos, ret))
            return -1;
        pos += ret;
    } else
#endif /* CONFIG_IEEE80211AD */
    {
        if (bss->caps & IEEE80211_CAP_IBSS) {
            ret = os_snprintf(pos, end - pos, "[IBSS]");
            if (os_snprintf_error(end - pos, ret))
                return -1;
            pos += ret;
        }
        if (bss->caps & IEEE80211_CAP_ESS) {
            ret = os_snprintf(pos, end - pos, "[ESS]");
            if (os_snprintf_error(end - pos, ret))
                return -1;
            pos += ret;
        }
    }

#if CFG_DEBUG_SETTING_P2P
    if (p2p) {
        ret = os_snprintf(pos, end - pos, "[P2P]");
        if (os_snprintf_error(end - pos, ret))
            return -1;
        pos += ret;
    }
#endif

#ifdef CONFIG_HS20
    if (wpa_bss_get_vendor_ie(bss, HS20_IE_VENDOR_TYPE) && ie2) {
        ret = os_snprintf(pos, end - pos, "[HS20]");
        if (os_snprintf_error(end - pos, ret))
            return -1;
        pos += ret;
    }
#endif /* CONFIG_HS20 */
#ifdef CONFIG_FST
    if (wpa_bss_get_ie(bss, WLAN_EID_MULTI_BAND)) {
        ret = os_snprintf(pos, end - pos, "[FST]");
        if (os_snprintf_error(end - pos, ret))
            return -1;
        pos += ret;
    }
#endif /* CONFIG_FST */

    ret = os_snprintf(pos, end - pos, "\t%s",
                      wpa_ssid_txt(bss->ssid, bss->ssid_len));
    if (os_snprintf_error(end - pos, ret))
        return -1;
    pos += ret;

    ret = os_snprintf(pos, end - pos, "\n");
    if (os_snprintf_error(end - pos, ret))
        return -1;
    pos += ret;

    return pos - buf;
}

static uint8_t _minisupp_cli_scan_results(uint8_t len, char *param[])
{
    struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
    char *pos, *end;
    struct wpa_bss *bss;
    char buf[128];
    size_t buflen = 128;
    int ret;

    wpa_printf(MSG_ERROR, "bssid / frequency / signal level / "
               "flags / ssid");

    if (wpa_s == NULL)
        return -1;

    dl_list_for_each(bss, &wpa_s->bss_id, struct wpa_bss, list_id) {
        pos = buf;
        end = buf + buflen;
        ret = wpa_supplicant_ctrl_iface_scan_result(wpa_s, bss, pos,
                                                    end - pos);
        if (ret < 0 || ret >= end - pos)
            break;
        wpa_printf(MSG_ERROR, "%s", pos);
    }

    return 0;
}

#if MTK_WIFI_AP_ENABLE_FLAG
extern nvdm_status_t hostapd_config_read(const char *data_item_name,
                                         char *buf, int *errors, uint32_t size);
static uint8_t _minisupp_cli_start_ap(uint8_t len, char *param[])
{
    struct wpa_supplicant *wpa_s;
    struct wpa_interface *ifaces;
    char buffer[SSID_MAX_LEN];
    int errors = 0;
    uint32_t size = 0;

    if (g_ap_created == 1) {
        wpa_printf(MSG_ERROR, "[ERROR]%s AP has been created.", __func__);
        return 0;
    }

    ifaces = os_zalloc(sizeof(struct wpa_interface));

    if (ifaces == NULL) {
        wpa_printf(MSG_ERROR, "[ERROR]%s ifaces NULL", __func__);
        return -1;
    }
    wpa_printf(MSG_DEBUG, "add iface ra0");
    ifaces->bridge_ifname = NULL;
    ifaces->ctrl_interface = ctrl_interface;
    ifaces->driver = ctrl_interface;
    ifaces->ifname = "ra0";
    ifaces->confname = "ap_config";

    //wpa_printf(MSG_ERROR, "%s", __func__);
    wpa_s = wpa_supplicant_add_iface(global_entry, ifaces, g_wpa_s);
    if (wpa_s == NULL)
        return -1;
    wpa_s->l2 = wpa_drv_get_socket(wpa_s);
    wpa_s->conf->ssid->mode = WPAS_MODE_AP;

    size = sizeof(buffer);
    if (hostapd_config_read("ssid", buffer, &errors, size)
        == NVDM_STATUS_OK) {
        wpa_s->conf->ssid->ssid_len = os_strlen(buffer);
        if (wpa_s->conf->ssid->ssid_len > SSID_MAX_LEN ||
            wpa_s->conf->ssid->ssid_len < 1) {
            wpa_printf(MSG_ERROR, "invalid SSID '%s'",
                       buffer);
            errors++;
        } else {
            if (wpa_s->conf->ssid->ssid == NULL)
                wpa_s->conf->ssid->ssid = os_zalloc(WIFI_MAX_LENGTH_OF_SSID + 1);
            os_memcpy(wpa_s->conf->ssid->ssid, buffer, os_strlen(buffer));
            wpa_printf(MSG_ERROR, "SSID = '%s'",
                       wpa_s->conf->ssid->ssid);
        }
    }
    wpa_s->conf->ssid->pairwise_cipher = WPA_CIPHER_NONE;
    netif_set_default(&ap_netif);
    netif_set_link_up(&ap_netif);
    wpa_supplicant_create_ap(wpa_s, wpa_s->conf->ssid);
    g_ap_created = 1;

    os_free(ifaces);
    return 0;
}

static uint8_t _minisupp_cli_stop_ap(uint8_t len, char *param[])
{
    if (g_ap_created == 0) {
        wpa_printf(MSG_ERROR, "[ERROR]%s AP has been stopped.", __func__);
        return 0;
    }

    netif_set_link_down(&ap_netif);
    wpa_supplicant_entry_iface_remove(ap_ifname);
    g_ap_created = 0;

    return 0;
}
#endif /* CFG_SUPP_WPACLI */

const cmd_t minisupp_cli[] = {
#if !defined(MTK_RELEASE_MODE) || (MTK_RELEASE_MODE != MTK_M_RELEASE)
    { "init", "enable wpa_supplicant", _minisupp_cli_init, NULL},
    { "quit", "quit wpa_supplicant", _minisupp_cli_deinit, NULL},
    { "scan", "scan $SEC $USEC", _minisupp_cli_req_scan, NULL},
    { "scan stop", "cancel scan", _minisupp_cli_stop_scan, NULL},
    { "add_network", "add_network", _minisupp_cli_add_network, NULL},
    { "remove_network", "remove_network $ID", _minisupp_cli_remove_network, NULL},
    { "disable_network", "disable_network $ID", _minisupp_cli_disable_network, NULL},
    { "enable_network", "enable_network $ID", _minisupp_cli_enable_network, NULL},
    { "select_network", "select_network $ID", _minisupp_cli_select_network, NULL},
    { "set_network", "set_network <network id> <variable name> <value>", _minisupp_cli_set_network, NULL},
    { "list_network", "list_network", _minisupp_cli_iface_list_networks, NULL},
    { "scan_results", "scan_results", _minisupp_cli_scan_results, NULL},
    { "sae_groups", "set_sae_groups", _minisupp_cli_set_sae_groups, NULL},
    { "get_sae_groups", "get_sae_groups", _minisupp_cli_get_sae_groups, NULL},
    
#if MTK_WIFI_AP_ENABLE_FLAG
    { "start_ap", "start_ap", _minisupp_cli_start_ap, NULL},
    { "stop_ap", "stop_ap", _minisupp_cli_stop_ap, NULL},
#endif
#endif
    { "ver", "version", _minisupp_cli_get_ver, NULL},
    { "log_level_set", "set", _minisupp_cli_set_debug_level, NULL},
    { "log_level_get", "show", _minisupp_cli_get_debug_level, NULL},
    { NULL, NULL, NULL, NULL }
};
#endif /* MTK_MINICLI_ENABLE */

