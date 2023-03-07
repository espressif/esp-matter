#ifndef _AP_HTTPSRV_H_
#define _AP_HTTPSRV_H_
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Common WiFi parameters */
#ifndef WIFI_SSID
#define WIFI_SSID "nxp_configuration_access_point"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "NXP0123456789"
#endif

/* Parameters that apply to AP mode only */
#ifndef WIFI_AP_CHANNEL
#define WIFI_AP_CHANNEL 1
#endif

#define WIFI_AP_IP_ADDR  "192.168.1.1"
#define WIFI_AP_NET_MASK "255.255.0.0"

#define MAX_RETRY_TICKS 50

#ifndef EXAMPLE_NETIF_INIT_FN
/*! @brief Network interface initialization function. */
#define EXAMPLE_NETIF_INIT_FN ethernetif0_init
#endif /* EXAMPLE_NETIF_INIT_FN */

#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG LWIP_DBG_ON
#endif
#ifndef HTTPD_STACKSIZE
#define HTTPD_STACKSIZE 2048
#endif
#ifndef HTTPD_PRIORITY
#define HTTPD_PRIORITY DEFAULT_THREAD_PRIO
#endif
#ifndef DEBUG_WS
#define DEBUG_WS 0
#endif

#define CGI_BUFFER_LENGTH        (2048)
#define CONNECTION_INFO_FILENAME ("connection_info.dat")

#define WEBCONFIG_DEBUG

#ifdef WEBCONFIG_DEBUG
#define WC_DEBUG(__fmt__, ...) PRINTF(__fmt__, ##__VA_ARGS__)
#else
#define WC_DEBUG(...)
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#endif
