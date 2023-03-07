#ifndef __WIFI_PROV_API_H__
#define __WIFI_PROV_API_H__
#include "blsync_ble.h"
int wifi_prov_api_event_trigger_connect(struct wifi_conn *info);
int wifi_prov_api_event_trigger_disconnect(void);
int wifi_prov_api_event_trigger_scan(void(*complete)(void *));
int wifi_prov_api_event_state_get(void(*state_get)(void *));
#endif
