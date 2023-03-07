/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NETWORK_CFG_H_
#define _NETWORK_CFG_H_

/*${header:start}*/
/*${header:end}*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/

/* Common WiFi parameters */
#ifndef WIFI_SSID
#define WIFI_SSID "nxp-ota"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "NXP0123456789"
#endif

#ifndef WIFI_SECURITY
#define WIFI_SECURITY WICED_SECURITY_WPA2_MIXED_PSK
#endif

/* Parameters that apply to AP mode only */
#ifndef WIFI_AP_MODE
#define WIFI_AP_MODE 1
#endif

#ifndef WIFI_AP_CHANNEL
#define WIFI_AP_CHANNEL 1
#endif

#define WIFI_AP_IP_ADDR  "192.168.1.1"
#define WIFI_AP_NET_MASK "255.255.0.0" /* IP address configuration. */
/*${macro:end}*/

#endif /* _NETWORK_CFG_H_ */
