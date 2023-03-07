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
#ifndef __BLOOP_H__
#define __BLOOP_H__

#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>

#include <utils_list.h>
/* Section for define loop structure*/
struct loop_ctx;

struct loop_msg {
    struct utils_list_hdr item;
    union {
        void *container;
        struct {
            uint8_t priority;
            uint8_t id_dst;
            uint8_t id_msg;
            uint8_t id_src;
        } header;
    } u;
    void *arg1;
    void *arg2;
    unsigned int time_added;
    unsigned int time_consumed;
};

struct loop_evt_handler_statistic {
    unsigned int time_max;
    unsigned int time_consumed;
    unsigned int time_accumulated;
    unsigned int count_triggered;
};

struct loop_evt_handler {
    const char *name;
    int (*evt)(struct loop_ctx *loop, const struct loop_evt_handler *handler, uint32_t *bitmap_evt, uint32_t *evt_type_map);
    int (*handle)(struct loop_ctx *loop, const struct loop_evt_handler *handler, struct loop_msg *msg);
};

#define LOOP_TASK_MAX  (32)
#define LOOP_TASK_PRIORITY_HIGHEST  (31)

struct loop_ctx {
    TaskHandle_t looper;
    uint32_t bitmap_evt_async;
    uint32_t bitmap_evt_sync;
    uint32_t bitmap_msg;
    uint32_t evt_type_map_async[LOOP_TASK_MAX];
    uint32_t evt_type_map_sync[LOOP_TASK_MAX];
    struct utils_list list[LOOP_TASK_MAX];
    struct loop_evt_handler_statistic statistic[LOOP_TASK_MAX];
    const struct loop_evt_handler *(handlers[LOOP_TASK_MAX]);
    /* List for timer */
    utils_dlist_t timer_dlist;
    utils_dlist_t timer_dued;
};

struct loop_timer {
    utils_dlist_t dlist_item;

#define LOOP_TIMER_FLAG_NONE            (0 << 0)
#define LOOP_TIMER_FLAG_AUTO_REPEAT     (1 << 0)
#define LOOP_TIMER_FLAG_AUTO_FREE       (1 << 1)
#define LOOP_TIMER_IS_AUTO_FREE(timer)  (timer->flags & LOOP_TIMER_FLAG_AUTO_FREE)
#define LOOP_TIMER_IS_AUTO_REPEAT(timer)  (timer->flags & LOOP_TIMER_FLAG_AUTO_REPEAT)
    uint8_t flags;

    unsigned int time_added;
    unsigned int time_target;
    int idx_task;
    uint32_t evt_type_map;
    void (*cb)(struct loop_ctx *loop, struct loop_timer *timer, void *arg);
    void *arg;
};

/* built-in handler*/
extern struct loop_evt_handler bloop_handler_sys;
#define LOOP_TASK_SYS_EVT_DUMP      (1U << 0)

int bloop_init(struct loop_ctx *loop);
int bloop_run(struct loop_ctx *loop);
int bloop_status_dump(struct loop_ctx *loop);
int bloop_handler_register(struct loop_ctx *loop, const struct loop_evt_handler *handler, int priority);
int bloop_handler_unregister(struct loop_ctx *loop, const struct loop_evt_handler *handler, int priority);
void bloop_timer_init(struct loop_timer *timer, int use_auto_free);
void bloop_timer_configure(struct loop_timer *timer, unsigned int delay_ms,
        void(*cb)(struct loop_ctx *loop, struct loop_timer *timer, void *arg), void *arg,
        int idx_task, uint32_t evt_type_map);
void bloop_timer_repeat_enable(struct loop_timer *timer);
void bloop_timer_register(struct loop_ctx *loop, struct loop_timer *timer);
void bloop_wait_startup(struct loop_ctx *loop);

/* ASYNC API is used in another code routine, such as Thread or IRQ Handler*/
void bloop_evt_set_async(struct loop_ctx *loop, unsigned int evt, uint32_t evt_map);
void bloop_evt_set_async_fromISR(struct loop_ctx *loop, unsigned int evt, uint32_t evt_map);
/* SYNC API is used in loop contex*/
void bloop_evt_set_sync(struct loop_ctx *loop, unsigned int evt, uint32_t evt_map);
void bloop_evt_unset_sync(struct loop_ctx *loop, unsigned int evt);
#endif
