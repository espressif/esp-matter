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

#include <stdint.h>

#include "wifi_api.h"

#ifdef CONFIG_DRIVER_INBAND
#include "driver_inband.h"
#include "wifi_scan.h"
#include "wifi_inband.h"
#include "wifi_init.h"
#include "wifi_event.h"
#include "ethernetif.h"
#include "hal_lp.h"
#include "hal_sleep_manager.h"
#endif
#ifdef CONFIG_DRIVER_GEN4M
#include "lwip/netif.h"
#include "nvdm.h"
#include "wifi_event_gen4m.h"
#include "wifi_netif.h"
#endif
#include "wpa_supplicant_task.h"

#include "includes.h"
#include "common.h"
#include "wpa_supplicant_i.h"
#include "driver_i.h"
#include "driver.h"
#include "wifi_os_api.h"
#include "ap.h"
#include "config_wpa_supplicant.h"
#include "config_ssid.h"

#include "eloop.h"
#include "lwip/sockets.h"
#include "scan_wpa_supplicant.h"
#include "bss_wpa_supplicant.h"

int    wake_up_eloop = -1;
int    wake_up_port = 6668;
struct sockaddr_in wake_up_addr;

extern int wpa_debug_level;
extern unsigned char g_mode_change;

int g_supplicant_ready = 0;
int g_iface_change_locker = 0;

#if defined(CONFIG_REPEATER)
static const char *ap_config_name = "ap_config";
static const char *sta_config_name = "sta_config";
#endif
#ifdef CONFIG_DRIVER_NONE
static const char *ctrl_interface = "none";
#endif /* CONFIG_DRIVER_NONE */
#ifdef CONFIG_DRIVER_INBAND
static const char *ctrl_interface = "inband";
#endif
#ifdef CONFIG_DRIVER_GEN4M
const char *ctrl_interface = "gen4m";
struct wpa_supplicant *g_wpa_s;
uint8_t g_ap_created;
#endif
const char *sta_ifname = "wlan0";
#ifdef CONFIG_AP
const char *ap_ifname  = "ra0";
#else
const char *ap_ifname  = "p2p0";
#endif
#ifdef MTK_WIFI_DIRECT_ENABLE
const char *p2p_ifname  = "p2p0";
const char *g_p2p_device_name = "MT7687 P2P_Device";
const char *g_p2p_ssid_postfix = "MT7687";
#endif/*MTK_WIFI_DIRECT_ENABLE*/

struct wpa_global *global_entry;
#ifdef CONFIG_AP
extern struct netif ap_netif;
#endif

os_semaphore_t opmode_switch_mutex = NULL;
os_semaphore_t g_wait_wpa_ready = NULL;

typedef struct {
	uint8_t port;
	char *ifname;
} port_match_iface_t;

static const port_match_iface_t port_match_iface_table[] = {
	{WIFI_PORT_STA,  "wlan0"},
#ifdef CONFIG_AP
	{WIFI_PORT_AP,  "ra0"}
#else
	{WIFI_PORT_AP,  "p2p0"}
#endif
};
extern unsigned char g_wpa_supplicant_own_addr[ETH_ALEN];

void wake_up_eloop_recv(int sock, void *eloop_ctx, void *sock_ctx)
{
	struct sockaddr_in addr;
	uint8_t buf[6] = {0};

	socklen_t fromlen = sizeof(addr);

	os_memset(&addr, 0, sizeof(addr));

	recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &fromlen);

	wpa_printf(MSG_DEBUG, "wakeup data:%s.\n", buf);
}

#if defined(CONFIG_REPEATER)
#define IDX_STA_FIRST_ENABLE 0
int wpa_supplicant_entry_dual_iface(unsigned char op_mode)
{
	struct wpa_interface *iface = NULL, *ifaces = NULL;
	struct wpa_supplicant *wpa_s = NULL;
	int ret = 0, idx;
	unsigned short protocol = ETH_P_EAPOL;
#ifdef CONFIG_AP
	char buffer[SSID_MAX_LEN];
	int errors = 0;
	uint32_t size = 0;
#endif

	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	iface = ifaces = os_zalloc(sizeof(struct wpa_interface) * 2);
	if (ifaces == NULL) {
		return -1;
	}

	global_entry->op_mode = op_mode;

	if (global_entry->l2_fd == 0) {
		global_entry->l2_fd = socket(PF_INET, SOCK_DGRAM, htons(protocol));
		wpa_printf(MSG_DEBUG, "open socket AP  ====> socket ID=%d", global_entry->l2_fd);
	}
	if (global_entry->l2_fd_second == 0) {
		global_entry->l2_fd_second = socket(PF_INET, SOCK_DGRAM, htons(protocol));
		wpa_printf(MSG_DEBUG, "open socket STA ====> socket ID=%d", global_entry->l2_fd_second);
	}
	/* KOKO::TODO freertos porting exported by LWIP */
#ifdef CONFIG_DRIVER_GEN4M
	register_eapol_rx_socket_dual_intf(global_entry->l2_fd, global_entry->l2_fd_second);
#endif

	for (idx = 0; idx < 2; idx++) {
		iface[idx].bridge_ifname = NULL;
		iface[idx].ctrl_interface = ctrl_interface;
		iface[idx].driver = ctrl_interface;
		if (idx == IDX_STA_FIRST_ENABLE) {
			iface[idx].ifname = ap_ifname;
			iface[idx].confname = ap_config_name;
		} else {
			iface[idx].ifname = sta_ifname;
			iface[idx].confname = sta_config_name;
		}

		netif_set_link_up(&ap_netif);
		netif_set_default(&sta_netif);

		wpa_printf(MSG_DEBUG, "%s: [%d]iface->ifname=%s",
			   __FUNCTION__, idx, iface[idx].ifname);

		wpa_s = wpa_supplicant_add_iface(global_entry, &iface[idx], NULL);
		if (wpa_s == NULL) {
			ret = -1;
			goto fn_out;
		} else {
			wpa_s->l2 = wpa_drv_get_socket(wpa_s);
#ifdef CONFIG_AP
			if (wpa_s->conf->ssid->mode == WPAS_MODE_AP) {
				wpa_printf(MSG_DEBUG, "%s: Create AP\n", __FUNCTION__);
				wpa_s->op_mode = WIFI_MODE_AP_ONLY;
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
				wpa_supplicant_create_ap(wpa_s, wpa_s->conf->ssid);
			} else {
#endif
				wpa_printf(MSG_DEBUG, "%s: Create STA\n", __FUNCTION__);
				wpa_s->op_mode = WIFI_MODE_STA_ONLY;
#ifdef CONFIG_AP
			}
#endif
		}
	}
#ifdef MTK_MINISUPP_ENABLE
	__g_wpa_supplicant_api.get_sta_qos_bit = mtk_ctrl_get_sta_qos_bit;
#endif

fn_out:
	if (iface) {
		os_free(iface);
	}
	return ret;
}
#endif /* CONFIG_REPEATER */

struct wpa_supplicant *mtk_supplicant_get_interface_by_port(uint8_t port)
{
	struct wpa_supplicant *iface;
	if (global_entry->ifaces) {
		iface = global_entry->ifaces;
		uint8_t count = sizeof(port_match_iface_table) / sizeof(port_match_iface_table[0]);
		for (uint8_t index = 0; index < count; index++) {
			if (port == port_match_iface_table[index].port) {
				while (iface != NULL) {
					if (0 == os_strcmp(iface->ifname, port_match_iface_table[index].ifname)) {
						return iface;
					}
					iface = iface->next;
				}
			}
		}
	}
	wpa_printf(MSG_ERROR, "%s: no matched port.", __FUNCTION__);
	return NULL;
}

void wpa_supplicant_entry_debug_level(int debug_level)
{
	global_entry->params.wpa_debug_level = debug_level;
	wpa_debug_level = debug_level;
#if 0 /* TODO:: add hadron debug system */
	if (debug_level <= MSG_INFO) {
		log_config_print_level(minisupp, PRINT_LEVEL_INFO);
	} else if (debug_level == MSG_WARNING) {
		log_config_print_level(minisupp, PRINT_LEVEL_WARNING);
	} else {
		log_config_print_level(minisupp, PRINT_LEVEL_ERROR);
	}
#endif

	wpa_printf(MSG_DEBUG, "wpa_debug_level = %d", wpa_debug_level);
}

int32_t supplicant_ready_query(uint8_t init_ready, uint8_t opmode_ready, uint32_t timeout) //for portect set parameters before supplicant interface ready
{
#if 0
	uint32_t time_step = 100; /*unit: ms*/
	uint32_t time_count = 0;
	while (((init_ready == TRUE) && (g_supplicant_ready == 0)) ||
			((opmode_ready == TRUE) && (global_entry->op_mode_change_flag == 1))) {
		os_sleep(0, time_step);

		if ((++time_count)*time_step > timeout) {
			wpa_printf(MSG_ERROR, "supplicant interface not ready!");
			return -1;
		}
	}
#endif
	return 0;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
extern uint8_t wifi_init_handle;
#endif

int wpa_supplicant_entry(unsigned char op_mode)
{
	int exitcode = -1;
	struct wpa_params params;
	unsigned short protocol = ETH_P_EAPOL;

	struct wpa_supplicant *wpa_s = NULL;
	struct wpa_interface *ifaces = NULL;

	os_memset(&params, 0, sizeof(struct wpa_params));
	if (!g_wait_wpa_ready)
		g_wait_wpa_ready = wifi_os_semphr_create_binary();

	/* struct wpa_global * */
	global_entry = wpa_supplicant_init(&params);
	if (global_entry == NULL) {
		wpa_printf(MSG_ERROR, "Failed to initialize wpa_supplicant");
		exitcode = -1;
		goto out;
	}

	global_entry->op_mode = op_mode;
	/* has debug log after this line */
	/* wpa_supplicant_entry_debug_level(MSG_ERROR); */
	wpa_supplicant_entry_debug_level(MSG_DEBUG);
	wpa_printf(MSG_ERROR, "wpa_supplicant_entry op_mode %d", op_mode);

	if (op_mode == 3)
		ifaces = os_zalloc(sizeof(struct wpa_interface) * 2);
	else
		ifaces = os_zalloc(sizeof(struct wpa_interface));

	if (ifaces == NULL) {
		wpa_printf(MSG_ERROR, "[ERROR]wpa_supplicant_entry ifaces NULL");
		return -1;
	}


	wake_up_eloop = socket(PF_INET, SOCK_DGRAM, 0);
	wpa_printf(MSG_ERROR, "Create wake_up socket: %d, %s-%d", wake_up_eloop, __FUNCTION__, __LINE__);

	os_memset(&wake_up_addr, 0, sizeof(wake_up_addr));
	wake_up_addr.sin_family = AF_INET;
	wake_up_addr.sin_addr.s_addr = htonl((127 << 24) | 1);
	wake_up_addr.sin_port = htons(wake_up_port);
	if (bind(wake_up_eloop, (struct sockaddr *)&wake_up_addr, sizeof(wake_up_addr)) < 0) {
		wpa_printf(MSG_ERROR, "bind socket addr fail %s-%d", __FUNCTION__, __LINE__);
		goto out;
	}

	/*
	    To create L2 RAW socket one time.
	*/

	if (op_mode == 3) {
		global_entry->l2_fd = socket(PF_INET, SOCK_DGRAM, htons(protocol));
		wpa_printf(MSG_DEBUG, "open socket AP  ====> socket ID=%d", global_entry->l2_fd);
		global_entry->l2_fd_second = socket(PF_INET, SOCK_DGRAM, htons(protocol));
		wpa_printf(MSG_DEBUG, "open socket STA ====> socket ID=%d", global_entry->l2_fd_second);

		/* KOKO:TODO freertos porting exported by LWIP */
#ifdef CONFIG_DRIVER_GEN4M
		register_eapol_rx_socket_dual_intf(global_entry->l2_fd, global_entry->l2_fd_second);
#endif
	} else {
		global_entry->l2_fd = socket(PF_INET, SOCK_DGRAM, htons(protocol));
		wpa_printf(MSG_ERROR, "====> init L2 packet, socket ID=%d", global_entry->l2_fd);
		/* KOKO::TODO freertos porting exported by LWIP */
#ifdef CONFIG_DRIVER_GEN4M
		register_eapol_rx_socket(global_entry->l2_fd);
#endif
	}

	if (op_mode == 3) {
		wpa_printf(MSG_DEBUG, "add iface wlan0 and ra0,op_mode=%d", op_mode);
		ifaces[0].bridge_ifname = NULL;
		ifaces[0].ctrl_interface = ctrl_interface;
		ifaces[0].driver = ctrl_interface;
		ifaces[0].ifname = "ra0";
		ifaces[0].confname = "ap_config";
		wpa_s = wpa_supplicant_add_iface(global_entry, &ifaces[0], NULL);
		wpa_s->l2 = wpa_drv_get_socket(wpa_s);
		wpa_printf(MSG_DEBUG, "%s: Create AP iface ra0\n", __FUNCTION__);
		wpa_s->op_mode = WIFI_MODE_AP_ONLY;
#ifdef CONFIG_DRIVER_NONE
		__process_global_event[WIFI_PORT_AP].ready = 1;
#endif
#ifdef CONFIG_AP
		wpa_supplicant_create_ap(wpa_s, wpa_s->conf->ssid);
#endif
		ifaces[1].bridge_ifname = NULL;
		ifaces[1].ctrl_interface = ctrl_interface;
		ifaces[1].driver = ctrl_interface;
		ifaces[1].ifname = "wlan0";
		ifaces[1].confname = "sta_config";
		wpa_s = wpa_supplicant_add_iface(global_entry, &ifaces[1], NULL);
		wpa_s->op_mode = WIFI_MODE_STA_ONLY;
		wpa_s->l2 = wpa_drv_get_socket(wpa_s);
		wpa_printf(MSG_DEBUG, "%s: Create STA iface wlan0\n", __FUNCTION__);
		g_wpa_s = wpa_s;
#ifdef CONFIG_DRIVER_NONE
		__process_global_event[WIFI_PORT_APCLI].ready = 1;
#endif

#ifdef CONFIG_AP
	} else if (op_mode == 2) {
		ifaces[0].bridge_ifname = NULL;
		ifaces[0].ctrl_interface = ctrl_interface;
		ifaces[0].driver = ctrl_interface;
		ifaces[0].ifname = "ra0";
		ifaces[0].confname = "ap_config";

		//wpa_printf(MSG_ERROR, "%s", __func__);
		wpa_s = wpa_supplicant_add_iface(global_entry, &ifaces[0], g_wpa_s);
		if (wpa_s == NULL)
			return -1;
		wpa_s->l2 = wpa_drv_get_socket(wpa_s);
		wpa_s->conf->ssid->mode = WPAS_MODE_AP;

		wpa_supplicant_create_ap(wpa_s, wpa_s->conf->ssid);
		g_ap_created = 1;
#endif
	} else {
		wpa_printf(MSG_DEBUG, "add iface wlan0, op_mode=%d", op_mode);
		ifaces[0].bridge_ifname = NULL;
		ifaces[0].ctrl_interface = ctrl_interface;
		ifaces[0].driver = ctrl_interface;
		ifaces[0].ifname = "wlan0";
		ifaces[0].confname = "sta_config";

		wpa_s = wpa_supplicant_add_iface(global_entry, &ifaces[0], NULL);
		wpa_s->op_mode = op_mode;
		wpa_s->l2 = wpa_drv_get_socket(wpa_s);
#ifdef CONFIG_DRIVER_NONE
		__process_global_event[WIFI_PORT_STA].ready = 1;
#endif
	}

	g_supplicant_ready = 1;
	opmode_switch_mutex = wifi_os_semphr_create_mutex();
	/* inform the upper layers */
	wifi_os_semphr_give(g_wait_wpa_ready);
	/* Sample Code+ */
	eloop_register_read_sock(wake_up_eloop, wake_up_eloop_recv, NULL, NULL);
	/* Sample Code- */
	exitcode = wpa_supplicant_run(global_entry);

	wpa_printf(MSG_DEBUG, "========= Supplicant Exit =======");
#ifdef CONFIG_DRIVER_GEN4M
	if (op_mode == 3)
		unregister_eapol_rx_socket_dual_intf(global_entry->l2_fd, global_entry->l2_fd_second);
	else
		unregister_eapol_rx_socket();
#endif
	wpa_supplicant_deinit(global_entry);

out:
	os_free(params.pid_file);
	if (ifaces)
		os_free(ifaces);
	if (wpa_s)
		os_free(wpa_s);

	os_program_deinit();

	close(wake_up_eloop);

	return exitcode;

}

void mtk_freertos_api2supp_semphr_take(void)
{
	if (!g_wait_wpa_ready)
		g_wait_wpa_ready = wifi_os_semphr_create_binary();

	if (g_wait_wpa_ready)
		wifi_os_semphr_take(g_wait_wpa_ready, NULL);
}

int wpa_supplicant_entry_iface_remove(const char *ifname)
{
	struct wpa_supplicant *wpa_s = NULL;
	int ret = 0;

	wpa_s = wpa_supplicant_get_iface(global_entry, (const char *)ifname);
	if (wpa_s == NULL) {
		return -1;
	}
	wpa_printf(MSG_DEBUG, "CTRL_IFACE GLOBAL INTERFACE_REMOVE '%s'\n", ifname);
	wpa_bss_flush(wpa_s);

	ret = wpa_supplicant_remove_iface(global_entry, wpa_s, 0);

	if (wpa_s)
		os_free(wpa_s);

	return ret;
}

extern unsigned char g_scan_done_flag; //pengfei+, for Sta_Fast_Link feature
int wpa_supplicant_entry_iface_change(unsigned char op_mode)
{
	struct wpa_supplicant *wpa_s = NULL;
	struct wpa_interface *iface = NULL, *ifaces = NULL;
	int ret = 0;
#ifdef CONFIG_AP
	char buffer[SSID_MAX_LEN];
	int errors = 0;
	uint32_t size = 0;
#endif

	/*
	    To prevent function re-entry.
	*/
	while (g_iface_change_locker) {
		wpa_printf(MSG_ERROR, "%s: g_iface_change_locker = %d\n",
			   __FUNCTION__, g_iface_change_locker);
	}
	g_iface_change_locker = 1;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	iface = ifaces = os_zalloc(sizeof(struct wpa_interface));
	if (ifaces == NULL) {
		g_iface_change_locker = 0;
		return -1;
	}

	iface->bridge_ifname = NULL;
	iface->ctrl_interface = ctrl_interface;
	iface->driver = ctrl_interface;
	if (op_mode == WIFI_MODE_STA_ONLY) {
		wpa_printf(MSG_DEBUG, "%s: change mode to %d\n.", __FUNCTION__, op_mode);
		iface->confname = "sta_config";
		netif_set_link_down(&ap_netif);
		wpa_supplicant_entry_iface_remove(ap_ifname);
		g_ap_created = 0;
		iface->ifname = sta_ifname;
		netif_set_default(&sta_netif);
	} else { /*WIFI_MODE_AP_ONLY*/
		wpa_printf(MSG_DEBUG, "%s: change mode to %d\n.", __FUNCTION__, op_mode);
		iface->confname = "ap_config";
		wpa_supplicant_entry_iface_remove(sta_ifname);
		iface->ifname = ap_ifname;
		netif_set_default(&ap_netif);
		netif_set_link_up(&ap_netif);
		g_mode_change = 1;
	}

	wpa_printf(MSG_DEBUG, "%s: iface->ifname=%s, g_mode_change = %d\n", __FUNCTION__, iface->ifname, g_mode_change);

	if (wpa_supplicant_get_iface(global_entry, iface->ifname)) {
		ret = -1;
		goto fn_out;
	}

	global_entry->op_mode = op_mode;
	register_eapol_rx_socket(global_entry->l2_fd);
	wpa_s = wpa_supplicant_add_iface(global_entry, iface, NULL);
	if (wpa_s == NULL) {
		ret = -1;
		goto fn_out;
	}

	wpa_s->op_mode = op_mode;
	(global_entry->ifaces->l2) = wpa_drv_get_socket(wpa_s);
#ifdef CONFIG_AP
	if (wpa_s->conf->ssid->mode == WPAS_MODE_AP) {
		wpa_printf(MSG_DEBUG, "%s: Create AP\n", __FUNCTION__);
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
		__process_global_event[WIFI_PORT_AP].ready = 1;
		wpa_supplicant_create_ap(wpa_s, wpa_s->conf->ssid);
		g_ap_created = 1;
	} else
#endif /* CONFIG_AP */
	{
		__process_global_event[WIFI_PORT_STA].ready = 1;
		if (g_scan_done_flag == 1) { //pengfei+, for Sta_Fast_Link feature
			g_scanning = 0;
			g_scan_by_app = 0;
			g_scan_done_flag = 0;
#ifdef MTK_MINISUPP_ENABLE
			if (__process_global_event[WIFI_PORT_STA].func != NULL)
				__process_global_event[WIFI_PORT_STA].func(__process_global_event[WIFI_PORT_STA].priv, WIFI_EVENT_ID_IOT_SCAN_LIST, NULL, 0);
#endif /* MTK_MINISUPP_ENABLE */
		}

	}

fn_out:
	if (ret == 0) {
#ifdef MTK_MINISUPP_ENABLE
		__g_wpa_supplicant_api.get_sta_qos_bit = mtk_ctrl_get_sta_qos_bit;
#endif
	}
	os_free(iface);
	g_iface_change_locker = 0;
	return ret;
}


void wpa_supplicant_entry_op_mode_get(unsigned char *op_mode)
{
	if (global_entry == NULL) {
		wpa_printf(MSG_ERROR, "global_entry is NULL");
		*op_mode = 0;
		return;
	}
	*op_mode = global_entry->op_mode;
}

int wpa_supplicant_entry_op_mode_set(unsigned char op_mode)
{
	uint8_t op_mode_change = 0;
	int ret = 0;

	if (global_entry == NULL) {
		wpa_printf(MSG_ERROR, "%s: global_entry is null.\n", __FUNCTION__);
		return -1;
	}

	if (op_mode != global_entry->op_mode) {
		op_mode_change = 1;
		supplicant_ready_query(TRUE, FALSE, 1000);
	}

	if (op_mode_change == 0) {
		wpa_printf(MSG_ERROR, "%s: same op_mode(=%d), recovery event register & get_sta_qos_bit hook.\n",
			   __FUNCTION__, op_mode);
		/*
		#ifdef MTK_MINISUPP_ENABLE
		        __g_wpa_supplicant_api.get_sta_qos_bit = mtk_ctrl_get_sta_qos_bit;
		#endif
		*/
		ret = 0;
	} else {
		if ((op_mode != WIFI_MODE_REPEATER)
				&& (global_entry->op_mode != WIFI_MODE_REPEATER)) {
			unregister_process_global_event();
			ret = wpa_supplicant_entry_iface_change(op_mode);
		} else {
#if defined(CONFIG_REPEATER)
			wpa_printf(MSG_ERROR, "%s: op_mode = %d.\n", __FUNCTION__, op_mode);
			unregister_process_global_event();
			if (global_entry->op_mode != WIFI_MODE_STA_ONLY) {
				netif_set_link_down(&ap_netif);
				wpa_supplicant_entry_iface_remove(ap_ifname);
				g_ap_created = 0;
			}
			if (global_entry->op_mode != WIFI_MODE_AP_ONLY) {
				wpa_supplicant_entry_iface_remove(sta_ifname);
			}

			if (op_mode == WIFI_MODE_REPEATER) {
				ret = wpa_supplicant_entry_dual_iface(op_mode);
			} else {
				ret = wpa_supplicant_entry_iface_change(op_mode);
			}
#else
			wpa_printf(MSG_ERROR, "%s: op_mode %d is not supported.\n", __FUNCTION__, op_mode);
#endif /* !CONFIG_REPEATER */
		}
	}
	return ret;
}

