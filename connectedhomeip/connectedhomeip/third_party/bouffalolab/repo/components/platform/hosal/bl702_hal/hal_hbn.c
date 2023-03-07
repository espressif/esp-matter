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
#include "bl_sys.h"
#include "bl_hbn.h"
#include "hal_sys.h"
#include <utils_log.h> 
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <utils_notifier.h>

static ntf_list_t *hbn_list = NULL;

static int ntf_init(void)
{
    hbn_list = pvPortMalloc(sizeof(ntf_list_t));
    if (!hbn_list) {
        log_error("malloc error.\r\n");
        return -1;
    }
    utils_notifier_chain_init(hbn_list);//utils_list_init
    return 0;
}

static int ntf_reg(utils_notifier_fn_t cb, void *arg)
{
    utils_notifier_t *hbn_node = NULL;

    if (!hbn_list) {
        log_error("ntfgpio not init.\r\n");
        return -1;
    }
    hbn_node = pvPortMalloc(sizeof(utils_notifier_t));
    if (!hbn_node) {
        log_error("malloc error.\r\n");
        return -1;
    }
    hbn_node->cb = cb;
    hbn_node->cb_arg = arg;

    utils_notifier_chain_register(hbn_list, hbn_node);

    return 0;
}

int hal_hbn_init(uint8_t *pinbuf, uint8_t pinbuf_size)
{
    hbn_type_t *hbn = NULL;

    if ((pinbuf_size == 0) || (pinbuf_size > 128)) {
        log_error("arg error.\r\n");
        return -1;
    }
    hbn = pvPortMalloc(pinbuf_size + sizeof(hbn_type_t));
    if (!hbn) {
        log_error("mem error");
        return -1;
    }

    hbn->active = 1;
    hbn->buflen = pinbuf_size;
    memcpy(hbn->buf, pinbuf, pinbuf_size);
    
    if (!hbn_list) {
        ntf_init();
    }

    ntf_reg((utils_notifier_fn_t)bl_hbn_enter, hbn);

    return 0;
}

int hal_hbn_enter(uint32_t time)
{
    utils_notifier_chain_call(hbn_list, &time);

    return -1;
}

