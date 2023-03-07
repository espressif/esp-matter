/*
 * Driver interface for RADIUS server or WPS ER only (no driver)
 * Copyright (c) 2008, Atheros Communications
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "driver.h"

/* porting UT with LWIP */
struct netif ap_if;
struct netif sta_if;

struct none_driver_data {
	struct hostapd_data *hapd;
	void *ctx;
};


static void * none_driver_hapd_init(struct hostapd_data *hapd,
				    struct wpa_init_params *params)
{
	struct none_driver_data *drv;

	drv = os_zalloc(sizeof(struct none_driver_data));
	if (drv == NULL) {
		wpa_printf(MSG_ERROR, "Could not allocate memory for none "
			   "driver data");
		return NULL;
	}
	drv->hapd = hapd;

	return drv;
}


static void none_driver_hapd_deinit(void *priv)
{
	struct none_driver_data *drv = priv;

	os_free(drv);
}


static void * none_driver_init(void *ctx, const char *ifname)
{
	struct none_driver_data *drv;

	drv = os_zalloc(sizeof(struct none_driver_data));
	if (drv == NULL) {
		wpa_printf(MSG_ERROR, "Could not allocate memory for none "
			   "driver data");
		return NULL;
	}
	drv->ctx = ctx;

	return drv;
}


static void none_driver_deinit(void *priv)
{
	struct none_driver_data *drv = priv;

	os_free(drv);
}

struct l2_packet_data *none_get_sock_recv(void *priv)
{
	/* struct wpa_driver_gen4m_data *inband_drv = priv; */
	wpa_printf(MSG_ERROR, "%s not supported", __func__);
	return NULL;
}

static int none_driver_scan(void *priv,
			    struct wpa_driver_scan_params *params)
{
	printf("%s not supported\r\n", __func__);
	return 0;
}

static int none_driver_associate(
	void *priv, struct wpa_driver_associate_params *params)
{
	printf("%s not supported\r\n", __func__);
	return 0;
}

static int none_driver_deauthenticate(void *priv, const u8 *addr,
				      int reason_code)
{
	printf("%s not supported\r\n", __func__);
	return 0;
}

static int none_driver_disassociate(void *priv, const u8 *addr,
				    int reason_code)
{
	printf("%s not supported\r\n", __func__);
	return 0;
}

const struct wpa_driver_ops wpa_driver_none_ops = {
	.name = "none",
	.desc = "no driver (RADIUS server/WPS ER)",
	.hapd_init = none_driver_hapd_init,
	.hapd_deinit = none_driver_hapd_deinit,
	.init = none_driver_init,
	.deinit = none_driver_deinit,
	.get_sock = none_get_sock_recv,
	.scan2 = none_driver_scan,
	.associate = none_driver_associate,
	.deauthenticate = none_driver_deauthenticate,
	.disassociate = none_driver_disassociate,
};
