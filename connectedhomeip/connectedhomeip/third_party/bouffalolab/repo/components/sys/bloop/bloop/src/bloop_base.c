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
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <bl_timer.h>
#include <blog.h>
#include <bloop.h>
#include <utils_debug.h>

int bloop_init(struct loop_ctx *loop)
{
    int i;

    memset(loop, 0, sizeof(struct loop_ctx));
    for (i = 0; i < sizeof(loop->list)/sizeof(loop->list[0]); i++) {
        utils_list_init(&(loop->list[i]));
    }
    INIT_UTILS_DLIST_HEAD(&(loop->timer_dlist));
    INIT_UTILS_DLIST_HEAD(&(loop->timer_dued));
    printf("=== %d task inited\r\n", i);

    return 0;
}

int bloop_handler_register(struct loop_ctx *loop, const struct loop_evt_handler *handler, int priority)
{
    int i;

    if (priority < 0 || priority >= LOOP_TASK_MAX) {
        /* out of range */
        return -1;
    }

    if (NULL == loop->handlers[priority]) {
        i = priority;
    } else {
        for (i = priority; i < LOOP_TASK_MAX; i++) {
            if (NULL == loop->handlers[priority]) {
                break;
            }
        }
        if (LOOP_TASK_MAX == i) {
            /*No valid task SLOT*/
            return -1;
        }
    }

    loop->handlers[i] = handler;
    return 0;
}

int bloop_handler_unregister(struct loop_ctx *loop, const struct loop_evt_handler *handler, int priority)
{
    if (priority < 0 || priority >= LOOP_TASK_MAX) {
        /* out of range */
        return -1;
    }
    if (NULL == loop->handlers[priority]) {
        /* empty */
        return -2;
    }
    if (handler != loop->handlers[priority]) {
        /* something mustbe wrong */
        return -3;
    }

    loop->handlers[priority] = NULL;
    return 0;
}

void bloop_timer_init(struct loop_timer *timer, int use_auto_free)
{
    memset(timer, 0, sizeof(struct loop_timer));
    utils_dlist_init(&timer->dlist_item);
    timer->flags = use_auto_free ? LOOP_TIMER_FLAG_AUTO_FREE : LOOP_TIMER_FLAG_NONE;
}

void bloop_timer_configure(struct loop_timer *timer, unsigned int delay_ms,
        void(*cb)(struct loop_ctx *loop, struct loop_timer *timer, void *arg), void *arg,
        int idx_task, uint32_t evt_type_map)
{
    timer->time_added = xTaskGetTickCount();
    timer->time_target = timer->time_added + delay_ms;
    timer->cb = cb;
    timer->arg = arg;
    timer->idx_task = idx_task;
    timer->evt_type_map = evt_type_map;
}

void bloop_timer_repeat_enable(struct loop_timer *timer)
{
    timer->flags |=  LOOP_TIMER_FLAG_AUTO_REPEAT;
}

void bloop_timer_repeat_reconfigure(struct loop_timer *timer)
{
    int delay_ms;

    delay_ms = (int)timer->time_target - (int)timer->time_added;
    timer->time_added = xTaskGetTickCount();
    timer->time_target = timer->time_added + delay_ms;
}

void bloop_timer_register(struct loop_ctx *loop, struct loop_timer *timer)
{
    int found = 0;
    struct loop_timer *node = NULL, *node_pre = NULL;

    if (utils_dlist_empty(&loop->timer_dlist)) {
        utils_dlist_add(&timer->dlist_item, &loop->timer_dlist);
    } else {
        utils_dlist_for_each_entry(&loop->timer_dlist, node, struct loop_timer, dlist_item) {
            if ((int)timer->time_target - (int)node->time_target <= 0) {
                /* ascend list on time_target*/
                if (NULL == node_pre) {
                    /* Still add to the head*/
                    utils_dlist_add(&timer->dlist_item, &loop->timer_dlist);
                } else {
                    utils_dlist_add(&timer->dlist_item, &node_pre->dlist_item);
                }
                found = 1;
                break;
            } else {
                node_pre = node;
            }
        }
        if (0 == found) {
            /*add to the tail*/
            utils_dlist_add(&timer->dlist_item, &node_pre->dlist_item);
        }
    }
}

/* timer maybe free after this function*/
static inline void _timer_is_up_handle(struct loop_ctx *loop, struct loop_timer *timer)
{
    bloop_evt_set_sync(loop, timer->idx_task, timer->evt_type_map);

    if (timer->cb) {
        timer->cb(loop, timer, timer->arg);
    }

    utils_dlist_del(&timer->dlist_item);
    utils_dlist_add(&timer->dlist_item, &loop->timer_dued);
}

static inline void _timer_dued_clean(struct loop_ctx *loop)
{
    utils_dlist_t *tmp;
    struct loop_timer *timer;

    utils_dlist_for_each_entry_safe(&loop->timer_dued, tmp, timer, struct loop_timer, dlist_item) {
        utils_dlist_del(&timer->dlist_item);
        if (LOOP_TIMER_IS_AUTO_FREE(timer)) {
            blog_debug("Free now\r\n");
            vPortFree(timer);
        } else if (LOOP_TIMER_IS_AUTO_REPEAT(timer)) {
            blog_debug("Repeat timer\r\n");
            bloop_timer_repeat_reconfigure(timer);
            bloop_timer_register(loop, timer);
        }
    }
}

static int _bloop_wait(struct loop_ctx *loop)
{
    struct loop_timer *timer = NULL;
    utils_dlist_t *tmp;
    int time2wait;
    unsigned int time_now;

copy_evt:
    /* Copy latest evt from ASYNC evt */
    taskENTER_CRITICAL();
    loop->bitmap_evt_sync |= loop->bitmap_evt_async;
    loop->bitmap_evt_async = 0;
    taskEXIT_CRITICAL();

    if (0 == loop->bitmap_evt_sync) {
        if (utils_dlist_empty(&loop->timer_dlist)) {
            /* timer list is empty*/
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        } else {
            /* timer list is NOT empty, so wait with timeout*/
            timer = utils_dlist_first_entry(&loop->timer_dlist, struct loop_timer, dlist_item);
            time2wait = (int)timer->time_target - (int)xTaskGetTickCount();
            if (time2wait > 0) {
                ulTaskNotifyTake(pdTRUE, time2wait);
            } else {
                /*time is up, so quit now*/
                goto handle_timer;
            }
        }
        goto copy_evt;
    }

handle_timer:
    if (!utils_dlist_empty(&loop->timer_dlist)) {
        time_now = xTaskGetTickCount(); //must use time_now to skeep the snapshot of tiemstamp
        utils_dlist_for_each_entry_safe(&loop->timer_dlist, tmp, timer, struct loop_timer, dlist_item) {
            if ((int)time_now - (int)timer->time_target >= 0) {
                /*this timer is up*/
                _timer_is_up_handle(loop, timer);
            } else {
                /* Break now, since timer list is ascend*/
                break;
            }
        }
        _timer_dued_clean(loop);
    }

    return 0;
}

static inline int _evt_highest(struct loop_ctx *loop)
{
    return 31 - __builtin_clz(loop->bitmap_evt_sync);
}

static inline int _msg_highest(struct loop_ctx *loop)
{
    return 31 - __builtin_clz(loop->bitmap_msg);
}

static inline void _evt_handle(struct loop_ctx *loop, int highest_evt)
{
    const struct loop_evt_handler *handler;
    struct loop_evt_handler_statistic *statistic;
    int time_start;

    handler = loop->handlers[highest_evt];
    BL_ASSERT_ERROR(handler);

    /* Update all the possible EVT Map for this handler*/
    taskENTER_CRITICAL();
    loop->evt_type_map_sync[highest_evt] |= loop->evt_type_map_async[highest_evt];
    loop->evt_type_map_async[highest_evt] = 0;
    taskEXIT_CRITICAL();

    time_start = bl_timer_now_us();
    handler->evt(loop, handler, &(loop->bitmap_evt_sync), &(loop->evt_type_map_sync[highest_evt]));
    //FIXME not Count in to the task switch during evt handler
    statistic = &loop->statistic[highest_evt];
    statistic->time_consumed = (int)bl_timer_now_us() - (int)time_start;
    statistic->time_accumulated += statistic->time_consumed;
    if (statistic->time_max < statistic->time_consumed) {
        /* TODO: 使用中值统计更合理？*/
        statistic->time_max = statistic->time_consumed;
    }
    statistic->count_triggered++;
    bloop_evt_unset_sync(loop, highest_evt);
}

static inline void _msg_handle(struct loop_ctx *loop, int highest_msg)
{
#if 0
    const struct loop_evt_handler *handler;
#endif
    struct loop_msg *msg;

    //TODO use containerof
    msg = (struct loop_msg*)utils_list_pop_front(&loop->list[highest_msg]);
    BL_ASSERT_ERROR(msg);

#if 0
    handler = loop->handlers[msg->u.header.priority];
#endif

#if 0
            uint8_t priority;
            uint8_t id_dst;
            uint8_t id_msg;
            uint8_t id_src;
        } header;
    } u;
    BL_ASSERT_ERROR(handler);
#endif
}

static void _bloop_handle_set(struct loop_ctx *loop)
{
    loop->looper = xTaskGetCurrentTaskHandle();
}

void bloop_wait_startup(struct loop_ctx *loop)
{
    while (NULL == loop->looper) {
        vTaskDelay(1);
    }
}

int bloop_run(struct loop_ctx *loop)
{
    int highest_evt, highest_msg;

    /*setup current task handle of looper */
    _bloop_handle_set(loop);

    while (0 == _bloop_wait(loop)) {
        /*check highest EVT or MSG*/
        highest_evt = _evt_highest(loop);
        highest_msg = _msg_highest(loop);
        if (highest_evt >= highest_msg && highest_evt >= 0) {
            /*highest_evt may have evt to handle*/
            _evt_handle(loop, highest_evt);
        } else if (highest_msg >= 0) {
            /*highest_msg may have msg to handle*/
            _msg_handle(loop, highest_msg);
        }
    }

    return 0;
}

void bloop_evt_set_async(struct loop_ctx *loop, unsigned int evt, uint32_t evt_map)
{
    BL_ASSERT_ERROR(evt < LOOP_TASK_MAX);

	taskENTER_CRITICAL();
    loop->bitmap_evt_async |= (1 << evt);
    loop->evt_type_map_async[evt] |= evt_map;
	taskEXIT_CRITICAL();

    /*wait up looper in case*/
    xTaskNotifyGive(loop->looper);
}

void bloop_evt_set_async_fromISR(struct loop_ctx *loop, unsigned int evt, uint32_t evt_map)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    BL_ASSERT_ERROR(evt < LOOP_TASK_MAX);

    loop->bitmap_evt_async |= (1 << evt);
    loop->evt_type_map_async[evt] |= evt_map;

    /*wait up looper in case*/
    vTaskNotifyGiveFromISR(loop->looper, &xHigherPriorityTaskWoken);
    if (pdTRUE == xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void bloop_evt_set_sync(struct loop_ctx *loop, unsigned int evt, uint32_t evt_map)
{
    BL_ASSERT_ERROR(evt < LOOP_TASK_MAX);

    loop->bitmap_evt_sync |= (1 << evt);
    loop->evt_type_map_async[evt] |= evt_map;
}

void bloop_evt_unset_sync(struct loop_ctx *loop, unsigned int evt)
{
    BL_ASSERT_ERROR(evt < LOOP_TASK_MAX);

    loop->bitmap_evt_sync &= (~(1 << evt));
}

static void _dump_task_handler(const struct loop_evt_handler *handler,
        struct loop_evt_handler_statistic *statistic,
        uint32_t bitmap_async,
        uint32_t bitmap_sync)
{
    printf("      evt handler %p,", handler->evt);
    printf(" msg handler %p,", handler->handle);
    printf(" trigged cnt %u,", statistic->count_triggered);
    printf(" bitmap async %lx sync %lx,", bitmap_async, bitmap_sync);
    printf(" time consumed %dus acc %dms, max %uus\r\n",
        statistic->time_consumed,
        statistic->time_accumulated/1000,
        statistic->time_max
    );
}

static void _dump_timer_dlist(utils_dlist_t *dlist)
{
    struct loop_timer *node;
    unsigned int time_now;
    int count = 0;

    time_now = xTaskGetTickCount();
    puts("--->>> timer list:\r\n");
    utils_dlist_for_each_entry(dlist, node, struct loop_timer, dlist_item) {
        printf("    timer[%02d]: %u(diff %d)ms, \t\t task idx %02d, evt map %08lx, ptr %p\r\n",
                count,
                node->time_target,
                (int)time_now - (int)node->time_target,
                node->idx_task,
                node->evt_type_map,
                node->cb
        );
        count++;
    }
}

int bloop_status_dump(struct loop_ctx *loop)
{
    int i;
    
    puts("====== bloop dump ======\r\n");
    printf("  bitmap_evt %lx\r\n", loop->bitmap_evt_sync);
    printf("  bitmap_msg %lx\r\n", loop->bitmap_msg);
    _dump_timer_dlist(&loop->timer_dlist);
    printf("  %d task:\r\n", sizeof(loop->list)/sizeof(loop->list[0]));
    for (i = sizeof(loop->list)/sizeof(loop->list[0]) - 1; i >= 0; i--) {
        printf("    task[%02d] : %s\r\n",
                i,
                loop->handlers[i] ? loop->handlers[i]->name : "empty"
        );
        if (loop->handlers[i]) {
            _dump_task_handler(
                    loop->handlers[i],
                    &(loop->statistic[i]),
                    loop->evt_type_map_async[i],
                    loop->evt_type_map_sync[i]
            );
        }
    }
    return 0;
}
