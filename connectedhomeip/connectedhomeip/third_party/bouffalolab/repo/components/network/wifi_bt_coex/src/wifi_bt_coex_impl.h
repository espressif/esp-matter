#ifndef __WIFI_BT_COEX_BT_IMPL_H__
#define __WIFI_BT_COEX_BT_IMPL_H__
#include <wifi_bt_coex.h>

int wifi_bt_coex_dump_all_impl(void);
int wifi_bt_coex_force_wlan_impl(void);
int wifi_bt_coex_force_bt_impl(void);
int wifi_bt_coex_force_mode_impl(enum WIFI_BT_COEX_FORCE_MODE mode);

int wifi_bt_coex_event_handler_impl(enum WIFI_BT_COEX_EVENT event, void *event_arg);

#endif
