/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stdlib.h>

#include <utils_debug.h>
#include <utils_log.h>
#include <blog.h>

#include <looprt.h>
#include <loopset.h>
#include <bl_gpio.h>


#define EVT_MAP_LED_TRIGGER     (1U << 0)
#define EVT_MAP_LED_WAITING     (1U << 1)

struct led_trigger_ctx {
    utils_dlist_t trigger_queue;
    utils_dlist_t waiting_queue;
};
struct led_trigger_item {
    utils_dlist_t dlist_item;
    struct loop_timer timer;
    int type;//0: blink; 1: hearbeat
    int pin, active;
    int current_val;
};

struct loop_evt_handler_holder {
    const struct loop_evt_handler handler;
    struct led_trigger_ctx *ctx;
};
static struct led_trigger_ctx led_ctx;

static inline void _waiting_queue_handle(struct led_trigger_ctx *ctx)
{
    utils_dlist_t *tmp;
    struct led_trigger_item *node;

    utils_dlist_for_each_entry_safe(&ctx->waiting_queue, tmp, node, struct led_trigger_item, dlist_item) {
        blog_info("[LED] New Trigger: PIN %d, active level %s, type %s\r\n",
            node->pin,
            node->active ? "Hi" : "Lo",
            node->type  ? "Hearbeat" : "Blink"
        );
        looprt_timer_register(&node->timer);
        utils_dlist_del(&node->dlist_item);
        utils_dlist_add(&node->dlist_item, &ctx->trigger_queue);
    }
}

static int _led_bloop_evt(struct loop_ctx *loop, const struct loop_evt_handler *handler, uint32_t *bitmap_evt, uint32_t *evt_type_map)
{
    uint32_t map = *evt_type_map;
    struct loop_evt_handler_holder *handler_holder = (struct loop_evt_handler_holder*)handler;

redo:
    if (map & EVT_MAP_LED_TRIGGER) {
        map &= (~EVT_MAP_LED_TRIGGER);
    } else if (map & EVT_MAP_LED_WAITING) {
        _waiting_queue_handle(handler_holder->ctx);
        map &= (~EVT_MAP_LED_WAITING);
    } else {
        BL_ASSERT_ERROR(0 == map);
    }

    if (map) {
        goto redo;
    }

    *evt_type_map = 0;
    return 0;
}

static int _led_bloop_msg(struct loop_ctx *loop, const struct loop_evt_handler *handler, struct loop_msg *msg)
{
    blog_debug("[LED] [MSG] called with msg info\r\n"
           "    priority %u\r\n"
           "    dst %u\r\n"
           "    msgid %u\r\n"
           "    src %u\r\n"
           "    arg1 %p\r\n"
           "    arg2 %p\r\n",
           msg->u.header.priority,
           msg->u.header.id_dst,
           msg->u.header.id_msg,
           msg->u.header.id_src,
           msg->arg1,
           msg->arg2
    );
    return 0;
}

int loopset_led_hook_on_looprt(void)
{
    static const struct loop_evt_handler_holder _led_bloop_handler_holder = {
        .handler = {
            .name = "Led Trigger",
            .evt = _led_bloop_evt,
            .handle = _led_bloop_msg,
        },
        .ctx = &led_ctx,
    };

    INIT_UTILS_DLIST_HEAD(&(led_ctx.trigger_queue));
    INIT_UTILS_DLIST_HEAD(&(led_ctx.waiting_queue));

    return looprt_handler_register(&_led_bloop_handler_holder.handler, 1);
}

static void _cb_led_trigger(struct loop_ctx *loop, struct loop_timer *timer, void *arg)
{
    struct led_trigger_item *trigger = (struct led_trigger_item*)arg;

    bl_gpio_output_set(trigger->pin, trigger->current_val ? 1 : 0);
    blog_debug("[LED] [CB] Set pin %d to %s\r\n",
        trigger->pin,
        trigger->current_val ? "Hi" : "Lo"
    );
    trigger->current_val = !(trigger->current_val);
}

void loopset_led_trigger(int pin, unsigned int timeon_ms)
{
    struct led_trigger_item *trigger;

    trigger = pvPortMalloc(sizeof(struct led_trigger_item));
    BL_ASSERT(trigger);
    memset(trigger, 0, sizeof(struct led_trigger_item));
    trigger->pin = pin;

    bloop_timer_init(&trigger->timer, 0);
    bloop_timer_repeat_enable(&trigger->timer);
    bloop_timer_configure(&trigger->timer, timeon_ms,
        _cb_led_trigger,
        trigger,
        1,
        EVT_MAP_LED_TRIGGER
    );
    bl_gpio_enable_output(pin, 0, 0);

    taskENTER_CRITICAL();
    utils_dlist_add(&trigger->dlist_item, &led_ctx.waiting_queue);
    taskEXIT_CRITICAL();

    looprt_evt_notify_async(1, EVT_MAP_LED_WAITING);
}
