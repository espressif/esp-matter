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

#include "includes.h"

#include "common.h"
#include "eloop.h"
#include "l2_packet.h"
#include "lwip/sockets.h"
#ifdef CONFIG_DRIVER_GEN4M
#include "wifi_netif.h"
#endif
#include "../wpa_supplicant/wpa_supplicant_i.h"

extern struct wpa_global *global_entry;
extern struct netif sta_netif;

#define L2_PORT (66)
#define L2_PORT_SECOND (76)

struct l2_packet_data {
	char ifname[17];
	u8 own_addr[ETH_ALEN];
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len);
	void *rx_callback_ctx;
	int l2_hdr; /* whether to include layer 2 (Ethernet) header data
             * buffers */
	int fd;
};


int l2_packet_get_own_addr(struct l2_packet_data *l2, u8 *addr)
{
	os_memcpy(addr, l2->own_addr, ETH_ALEN);
	return 0;
}


int l2_packet_send(struct l2_packet_data *l2, const u8 *dst_addr, u16 proto,
		   const u8 *buf, size_t len)
{
	int ret;
	u8 *eth_buff = os_malloc(len + 14);
	if (eth_buff == NULL)
		return -1;

	if (l2 == NULL) {
		wpa_printf(MSG_ERROR, "l2 == NULL\n");
		os_free(eth_buff);
		return -1;
	}

#if 1
	if (l2->l2_hdr) {
		// wpa_hexdump(MSG_DEBUG, "=== L2 has pkt hdr =====", (unsigned char *)l2->l2_hdr, 14);
	}
#endif

	/*
	 * TODO: Send frame (may need different implementation depending on
	 * whether l2->l2_hdr is set).
	 */
	if (l2->l2_hdr) {
#if 0
		eth_buff[0] = 0x00;
		eth_buff[1] = 0x00;
#endif
		os_memcpy((eth_buff), dst_addr, ETH_ALEN);
		os_memcpy((eth_buff + ETH_ALEN), l2->own_addr, ETH_ALEN);
		eth_buff[ETH_ALEN + ETH_ALEN] = ((proto & 0xFF00) >> 8); //0x88;
		eth_buff[ETH_ALEN + ETH_ALEN + 1] = (proto & 0x00FF); //0x8E;
		os_memcpy(eth_buff + 14, buf, len);

		wpa_printf(MSG_DEBUG, "[%s] l2->ifname = %s\n", __FUNCTION__, l2->ifname);
		wpa_hexdump(MSG_DEBUG, "l2_packet_send", (unsigned char *)eth_buff, 14);
		wpa_printf(MSG_DEBUG, "[%s] L2 packet send to CONNSYS\n", __FUNCTION__);
#ifndef CONFIG_DRIVER_NONE
		ret = mtk_l2_packet_send((u8 *)eth_buff, len + 14, &sta_netif);
		if (ret < 0) {
			wpa_printf(MSG_ERROR, "l2_packet_send - connsys send fail: %d", ret);
		}
#else
		wpa_printf(MSG_ERROR, "l2_packet_send - skip CONFIG_DRIVER_NONE");
#endif
		os_free(eth_buff);
	} else {
		wpa_printf(MSG_ERROR, "l2_packet_send NOT SUPPORT - no L2 header.\n");
#if 0
		struct sockaddr_ll ll;
		os_memset(&ll, 0, sizeof(ll));
		ll.sll_family = AF_PACKET;
		ll.sll_ifindex = l2->ifindex;
		ll.sll_protocol = htons(proto);
		ll.sll_halen = ETH_ALEN;
		os_memcpy(ll.sll_addr, dst_addr, ETH_ALEN);
		ret = sendto(l2->fd, buf, len, 0, (struct sockaddr *) &ll,
			     sizeof(ll));
		if (ret < 0) {
			wpa_printf(MSG_ERROR, "l2_packet_send - sendto: %s",
				   strerror(errno));
		}
#endif
		os_free(eth_buff);
	}


	return 0;
}

/* rtos_porting */
#ifdef CONFIG_DRIVER_NONE
void l2_packet_receive(int sock, void *eloop_ctx, void *sock_ctx)
{
	struct l2_packet_data *l2 = eloop_ctx;
	struct sockaddr_in addr;
	unsigned char buf[1024];
	int len;
	socklen_t fromlen = sizeof(addr);

	len = recvfrom(sock, buf, sizeof(buf), 0,
		       (struct sockaddr *)&addr, &fromlen);
	if (len < 0 || len <= 16) {
		wpa_printf(MSG_DEBUG, "recv (len=%d)", len);
		wpa_hexdump(MSG_ERROR, "packet:", (buf), len);
		return;
	}
	wpa_printf(MSG_DEBUG, "EAPOL len(%d)", len);
	wpa_hexdump(MSG_DEBUG, "l2_packet_receive [EAPOL-header]", (buf), 32);

	l2->rx_callback(l2->rx_callback_ctx, &(buf[6]), &(buf[14]), (len - 14));
}
#else
extern void l2_packet_receive(int sock, void *eloop_ctx, void *sock_ctx);
#endif


struct l2_packet_data *l2_packet_init(
	const char *ifname, const u8 *own_addr, unsigned short protocol,
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len),
	void *rx_callback_ctx, int l2_hdr)
{
	struct l2_packet_data *l2;
	struct sockaddr_in ll;
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;

	l2 = os_zalloc(sizeof(struct l2_packet_data));
	if (l2 == NULL) {
		wpa_printf(MSG_ERROR, "ERROR! null L2!");
		return NULL;
	}
	os_strlcpy(l2->ifname, ifname, sizeof(l2->ifname));
	l2->rx_callback = rx_callback;
	l2->rx_callback_ctx = rx_callback_ctx;
	l2->l2_hdr = l2_hdr;

#if 0
	l2->fd = socket(PF_INET, SOCK_DGRAM, htons(protocol));
#else

	wpa_printf(MSG_DEBUG, "[%s] ifname: %s\n", __FUNCTION__, ifname);

#if defined(CONFIG_REPEATER)
	if (global_entry->op_mode == 3) {
#ifdef CONFIG_AP
		if (os_strstr(ifname, "ra0")) {
#else
		if (os_strstr(ifname, "p2p0")) {
#endif
			l2->fd = global_entry->l2_fd;
			wpa_printf(MSG_DEBUG, "[%s] AP socket ID: %d", __FUNCTION__, l2->fd);
		} else {
			l2->fd = global_entry->l2_fd_second;
			wpa_printf(MSG_DEBUG, "[%s] STA socket ID: %d", __FUNCTION__, l2->fd);
		}
	} else

#endif /* !CONFIG_REPEATER */
	{
		l2->fd = global_entry->l2_fd;
	}
#endif

	if (l2->fd < 0) {
		wpa_printf(MSG_ERROR, "error socket");
		os_free(l2);
		return NULL;
	}

	if (setsockopt(l2->fd, SOL_SOCKET, SO_RCVTIMEO,
	    (char *)&timeout, sizeof(timeout)) < 0) {
		wpa_printf(MSG_ERROR, "error set socket timeout");
		os_free(l2);
		return NULL;
	}

	os_memset(&ll, 0, sizeof(ll));
	ll.sin_family = PF_INET;
	ll.sin_addr.s_addr = htonl((127 << 24) | 1);

#if defined(CONFIG_REPEATER)
	if (global_entry->op_mode == 3) {
#ifdef CONFIG_AP
		if (os_strstr(ifname, "ra0")) {
#else
		if (os_strstr(ifname, "p2p0")) {
#endif
			ll.sin_port = htons(L2_PORT);
			wpa_printf(MSG_DEBUG, "[%s] AP socket PORT: %d", __FUNCTION__, L2_PORT);
		} else {
			ll.sin_port = htons(L2_PORT_SECOND);
			wpa_printf(MSG_DEBUG, "[%s] STA socket PORT: %d", __FUNCTION__, L2_PORT_SECOND);
		}
	} else
#endif /* !CONFIG_REPEATER */
	{
		ll.sin_port = htons(L2_PORT);
	}

	if (bind(l2->fd, (struct sockaddr *) &ll, sizeof(ll)) < 0) {
		wpa_printf(MSG_DEBUG, "[%s] bind[PF_PACKET]: ERROR", __FUNCTION__);
		close(l2->fd);
		os_free(l2);
		return NULL;
	}
	os_memcpy(l2->own_addr, own_addr, ETH_ALEN);
	eloop_register_read_sock(l2->fd, l2_packet_receive, l2, NULL); //Remove temp, it should be recovered later

#if 0
	memcpy(&g_ll, &ll, sizeof(struct sockaddr_in));
	gSocket = l2->fd;

	eloop_register_timeout(20, 0, l2_simulation, NULL, NULL); //Just used for trigger, not necessary
#endif

	return l2;
}

struct l2_packet_data *l2_packet_init_bridge(
	const char *br_ifname, const char *ifname, const u8 *own_addr,
	unsigned short protocol,
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len),
	void *rx_callback_ctx, int l2_hdr)
{
	return l2_packet_init(br_ifname, own_addr, protocol, rx_callback,
			      rx_callback_ctx, l2_hdr);
}

void l2_packet_deinit(struct l2_packet_data *l2)
{
	wpa_printf(MSG_DEBUG, "%s", __FUNCTION__);
	if (l2 == NULL) {
		return;
	}

	if (l2->fd >= 0) {
		eloop_unregister_read_sock(l2->fd);
	}

	os_free(l2);
}


int l2_packet_get_ip_addr(struct l2_packet_data *l2, char *buf, size_t len)
{
	/* TODO: get interface IP address */
	return -1;
}


void l2_packet_notify_auth_start(struct l2_packet_data *l2)
{
	/* This function can be left empty */
}
