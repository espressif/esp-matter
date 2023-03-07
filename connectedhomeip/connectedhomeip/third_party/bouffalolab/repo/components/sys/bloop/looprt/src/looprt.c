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

#include <looprt.h>
#include <FreeRTOS.h>

/* Build the rocket with wheels*/
static struct loop_ctx looprt;

static void proc_entry_looprt(void *pvParameters)
{
    bloop_run(&looprt);

    /* Never Reach Here*/
    while (1) {
        puts("--->>> Error terminated looprt\r\n");
        vTaskDelay(1000);
    }
}

void looprt_evt_notify_async(unsigned int task, uint32_t evt_map)
{
    bloop_evt_set_async(&looprt, task, evt_map);
}

void looprt_evt_notify_async_fromISR(unsigned int task, uint32_t evt_map)
{
    bloop_evt_set_async_fromISR(&looprt, task, evt_map);
}

void looprt_evt_status_dump(void)
{
    bloop_status_dump(&looprt);
}

void looprt_evt_schedule(int task, uint32_t evt_map, int delay_ms)
{
    struct loop_timer *timer;

    timer = pvPortMalloc(sizeof(struct loop_timer));
    if (NULL == timer) {
        return;
    }
    bloop_timer_init(timer, 1);
    bloop_timer_configure(timer, delay_ms, NULL, NULL, task, evt_map);
    bloop_timer_register(&looprt, timer);
}

int looprt_start(StackType_t *proc_stack_looprt, int stack_count, StaticTask_t *proc_task_looprt)
{
    bloop_init(&looprt);

    /* Register necessary looper handler before looper starts*/
    bloop_handler_register(&looprt, &bloop_handler_sys, LOOP_TASK_PRIORITY_HIGHEST);

    looprt_evt_status_dump();

    xTaskCreateStatic(proc_entry_looprt, (char*)"bloop_rt", stack_count, NULL, 26, proc_stack_looprt, proc_task_looprt);

    bloop_wait_startup(&looprt);

    return 0;
}

int looprt_start_auto(void)
{
    bloop_init(&looprt);

    /* Register necessary looper handler before looper starts*/
    bloop_handler_register(&looprt, &bloop_handler_sys, LOOP_TASK_PRIORITY_HIGHEST);

    looprt_evt_status_dump();

    xTaskCreate(proc_entry_looprt, (char*)"bloop_rt", 4096, NULL, 26, NULL);

    bloop_wait_startup(&looprt);

    return 0;
}

int looprt_handler_register(const struct loop_evt_handler *handler, int priority)
{
    if (NULL == looprt.looper) {
        return -1;
    }
    return bloop_handler_register(&looprt, handler, priority);
}

int looprt_timer_register(struct loop_timer *timer)
{
    bloop_timer_register(&looprt, timer);

    return 0;
}
