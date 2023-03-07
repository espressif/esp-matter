/*
 * ACS - Automatic Channel Selection module
 * Copyright (c) 2011, Atheros Communications
 * Copyright (c) 2013, Qualcomm Atheros, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef ACS_H
#define ACS_H

#ifdef CONFIG_ACS

enum hostapd_chan_status acs_init(struct hostapd_iface *iface);
void acs_cleanup(struct hostapd_iface *iface);

#else /* CONFIG_ACS */

static inline enum hostapd_chan_status acs_init(struct hostapd_iface *iface)
{
#ifdef CONFIG_ACS_OFFLOAD
	if (hostapd_drv_do_acs(iface->bss[0]))
		return HOSTAPD_CHAN_INVALID;
	return HOSTAPD_CHAN_ACS;
#else
	wpa_printf(MSG_ERROR, "ACS was disabled on your build, rebuild hostapd with CONFIG_ACS=y or set channel");
	return HOSTAPD_CHAN_INVALID;
#endif
}

static inline void acs_cleanup(struct hostapd_iface *iface)
{
}

#endif /* CONFIG_ACS */

#endif /* ACS_H */
