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
#include <bl_ir.h>
#include <aos/yloop.h>

#include <loopset_ir.h>

struct loop_evt_handler_holder {
    const struct loop_evt_handler handler;
};

static int _ir_bloop_evt(struct loop_ctx *loop, const struct loop_evt_handler *handler, uint32_t *bitmap_evt, uint32_t *evt_type_map)
{
    uint32_t data;

    data = bl_receivedata();
    if (0 == data) {
        aos_post_event(EV_IR, CODE_IR_RECEIVE_NEC_REPEAT, 0);        
    } else {
        aos_post_event(EV_IR, CODE_IR_RECEIVE_NEC, data);        
    }
    bl_enable_rx_int();

    return 0;
}

static int _ir_bloop_msg(struct loop_ctx *loop, const struct loop_evt_handler *handler, struct loop_msg *msg)
{
    blog_debug("[IR] [MSG] called with msg info\r\n");

    return 0;
}

int loopset_ir_hook_on_looprt(void)
{
    static const struct loop_evt_handler_holder _ir_bloop_handler_holder = {
        .handler = {
            .name = "IR Trigger",
            .evt = _ir_bloop_evt,
            .handle = _ir_bloop_msg,
        }, 
    };

    //TODO use run-time priority based register
    return looprt_handler_register(&_ir_bloop_handler_holder.handler, IR_PRIORITY);
}

void ir_async_post(void)
{
    looprt_evt_notify_async_fromISR(IR_PRIORITY, EVT_MAP_IR_TRIGGER);

    return;
}

