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
#ifndef __APP_COMMON_H__
#define __APP_COMMON_H__
#include <aos/kernel.h>
#include <aos/yloop.h>

struct AppEvent{
    input_event_t event;
    void *pdata;
};

static struct AppEvent gAppEvt;

__inline static void AppDelayHandler(void *private_data)
{
    struct AppEvent *evt = (struct AppEvent *)private_data;
    input_event_t *event = &evt->event;
    aos_post_event(event->type,event->code,event->value);
}

/*!
    Task register.
*/
#define TASK_REGISTER(TASK,TASK_ID,DATA) {\
        aos_register_event_filter(TASK_ID,TASK,DATA);\
}

/*!
    Immediately trigger asynchronous events.
*/
#define SEND_EVENT_NOW(TASK_ID,EVT_ID,VAL){\
        aos_post_event(TASK_ID,EVT_ID,VAL);\
}

/*!
    Condition: Valid only in registered TASK
    Cancel current delay messages.
*/
#define CANCEL_MESSAGE(TASK_ID,EVT_ID,MSG){\
        gAppEvt.event.type = TASK_ID;\
        gAppEvt.event.code = EVT_ID;\
        gAppEvt.pdata = MSG;\
        aos_cancel_delayed_action(-1,AppDelayHandler,&gAppEvt);\
}

/*!
    Condition: Valid only in registered TASK.
    Delay time to send messages.
*/
#define SEND_MESSAGE_LATER(TASK_ID,EVT_ID,MSG,TIME){\
        gAppEvt.event.type = TASK_ID;\
        gAppEvt.event.code = EVT_ID;\
        gAppEvt.pdata = MSG;\
        aos_cancel_delayed_action(-1,AppDelayHandler,&gAppEvt);\
        aos_post_delayed_action(TIME,AppDelayHandler,&gAppEvt);\
}

#endif
