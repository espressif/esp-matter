#include <wifi_bt_coex.h>
#include "wifi_bt_coex_impl.h"

int wifi_bt_coex_dump_all(void)
{
    wifi_bt_coex_dump_all_impl();

    return 0;
}

int wifi_bt_coex_force_wlan(void)
{
    return wifi_bt_coex_force_wlan_impl();
}

int wifi_bt_coex_force_bt(void)
{
    return wifi_bt_coex_force_bt_impl();
}

int wifi_bt_coex_force_mode(enum WIFI_BT_COEX_FORCE_MODE mode)
{
    switch (mode) {
        case WIFI_BT_COEX_FORCE_MODE_PTA_FORCE:
        case WIFI_BT_COEX_FORCE_MODE_PTI_DEFAULT_FORCE:
        case WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE:
        case WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2:
        {
            return wifi_bt_coex_force_mode_impl(mode);
        }
        break;
        default:
        {
            /*Empty Here*/
            return -1;
        }
    }

    return 0;
}

int wifi_bt_coex_event_notify(enum WIFI_BT_COEX_EVENT event, void *event_arg)
{
    return wifi_bt_coex_event_handler_impl(event, event_arg);
}
