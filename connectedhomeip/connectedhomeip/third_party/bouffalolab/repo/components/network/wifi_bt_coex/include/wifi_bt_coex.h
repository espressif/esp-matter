#ifndef __WIFI_BT_COEX_BT_H__
#define __WIFI_BT_COEX_BT_H__
#include <FreeRTOS.h>
#include <timers.h>

enum WIFI_BT_COEX_FORCE_MODE {
    WIFI_BT_COEX_FORCE_MODE_PTA_FORCE,
    WIFI_BT_COEX_FORCE_MODE_PTI_DEFAULT_FORCE,
    WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE,
    WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2,
};

enum WIFI_BT_COEX_EVENT {
    WIFI_BT_COEX_EVENT_BT_A2DP_UNDERRUN,
};

struct wifi_bt_coex_ctx {
    TimerHandle_t coexTimer;
    uint32_t timer_now;
    uint32_t timer_max;
    uint32_t timer_toggle_start;
    uint32_t timer_toggle_end;
    uint32_t timeus_last_called;
    uint32_t time_step_inc;
    uint32_t time_step_dec;
};

int wifi_bt_coex_cli_init(void);
int wifi_bt_coex_dump_all(void);
int wifi_bt_coex_force_wlan(void);
int wifi_bt_coex_force_bt(void);
int wifi_bt_coex_force_mode(enum WIFI_BT_COEX_FORCE_MODE mode);

/*Following is the event section*/
int wifi_bt_coex_event_notify(enum WIFI_BT_COEX_EVENT event, void *event_arg);
#endif
