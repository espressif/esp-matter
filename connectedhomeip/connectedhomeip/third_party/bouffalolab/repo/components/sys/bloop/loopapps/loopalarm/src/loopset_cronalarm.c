#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils_debug.h>
#include <utils_log.h>
#include <blog.h>
#include <looprt.h>
#include <loopset.h>
#include <loopset_cronalarm.h>
#include <cronalarms.h>

#define CRON_ALARM_PRIORITY   4

struct loop_evt_handler_holder {
    const struct loop_evt_handler handler;
};

struct cronalarm_trigger_item {
    struct loop_timer timer;
};

struct cronalarm_trigger_item cronalarm_item;

static int _cronalarm_bloop_evt(struct loop_ctx *loop, const struct loop_evt_handler *handler, uint32_t *bitmap_evt, uint32_t *evt_type_map)
{
    uint32_t map = *evt_type_map;
redo:
    if (map & EVT_MAP_CRON_ALARM_TRIGGER) {      
        looprt_timer_register(&cronalarm_item.timer);
        map &= (~EVT_MAP_CRON_ALARM_TRIGGER);
        blog_debug("end once timer trigger \r\n");
    }else {
        BL_ASSERT_ERROR(0 == map);
    }

    if (map) {
        goto redo;
    }

    *evt_type_map = 0;
    return 0;
}

static int _cronalarm_bloop_msg(struct loop_ctx *loop, const struct loop_evt_handler *handler, struct loop_msg *msg)
{
    blog_debug("[CRON ALARM] [MSG] called with msg info\r\n");
    return 0;
}

int loopapp_cronalarm_hook_on_looprt(void)
{
    static const struct loop_evt_handler_holder _cronalarm_bloop_handler_holder = {
        .handler = {
            .name = "Cron Alarm Trigger",
            .evt = _cronalarm_bloop_evt,
            .handle = _cronalarm_bloop_msg,
        },
    };

    return looprt_handler_register(&_cronalarm_bloop_handler_holder.handler, CRON_ALARM_PRIORITY);
}

static void _cb_cronalarm_trigger(struct loop_ctx *loop, struct loop_timer *timer, void *arg)
{
    cron_alarm_service();
    return;
}

void loopapp_cronalarm_trigger(void)
{
    memset(&cronalarm_item, 0, sizeof(struct cronalarm_trigger_item));

    bloop_timer_init(&cronalarm_item.timer, 0);
    bloop_timer_repeat_enable(&cronalarm_item.timer);
    bloop_timer_configure(&cronalarm_item.timer, 1000,
        _cb_cronalarm_trigger,
        &cronalarm_item,
        1,
        EVT_MAP_CRON_ALARM_TRIGGER
    );

    looprt_evt_notify_async(CRON_ALARM_PRIORITY, EVT_MAP_CRON_ALARM_TRIGGER);
}
