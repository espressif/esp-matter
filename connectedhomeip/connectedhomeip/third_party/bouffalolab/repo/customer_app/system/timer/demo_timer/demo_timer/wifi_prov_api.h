#ifndef __WIFI_PROV_API_H__
#define __WIFI_PROV_API_H__
int wifi_prov_api_event_trigger_connect(void);
int wifi_prov_api_event_trigger_disconnect(void);

/*set len to the ssid len, if ssid is NOT '\0' tailed*/
int wifi_prov_api_event_trigger_ssid(const char *ssid, int len);

/*set len to the ssid len, if ssid is NOT '\0' tailed*/
int wifi_prov_api_event_trigger_password(const char *password, int len);
#endif
