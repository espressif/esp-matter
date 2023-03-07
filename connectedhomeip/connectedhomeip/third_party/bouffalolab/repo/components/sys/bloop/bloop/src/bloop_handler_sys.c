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

#include <bloop.h>
#include <utils_debug.h>

static int loop_evt_entity_sys_evt(struct loop_ctx *loop, const struct loop_evt_handler *handler, uint32_t *bitmap_evt, uint32_t *evt_type_map)
{
    uint32_t map = *evt_type_map;

    printf("[SYS] [EVT] called with bitmap_evt %08lx, bitmap_task %08lx\r\n", *bitmap_evt,  *evt_type_map);

redo:
    if (map & LOOP_TASK_SYS_EVT_DUMP) {
        bloop_status_dump(loop);
        map &= (~LOOP_TASK_SYS_EVT_DUMP);
    } else {
        BL_ASSERT_ERROR(0 == map);
    }
    if (map) {
        goto redo;
    }

    *evt_type_map = 0;

    return 0;
}

static int loop_evt_entity_sys_handler(struct loop_ctx *loop, const struct loop_evt_handler *handler, struct loop_msg *msg)
{
    printf("[SYS] [MSG] called with msg info\r\n"
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

struct loop_evt_handler bloop_handler_sys = {
    .name = "SYS [built-in]",
    .evt = loop_evt_entity_sys_evt,
    .handle = loop_evt_entity_sys_handler,
};

