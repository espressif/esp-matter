/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief
 *   This file includes the platform-specific initializers.
 */

#include <stdio.h>

#include <openthread-system.h>

#include <wiced_memory.h>
#include <wiced_platform.h>

#if !OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
#include "ifx_ot_cli_cmd.h"
#endif

extern void otPlatAlramInit(void);
extern void otPlatRadioInit(void);
extern void otPlatUartInit(void);

typedef struct
{
    struct
    {
        uint32_t post_init;
    } event_code;
} system_cb_t;

static system_cb_t system_cb = {0};

#ifndef CHIP_HAVE_CONFIG_H
static void system_post_init(void)
{
    printf("system_post_init\n");

#if !OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    ifx_ot_cli_cmd_install(otInstanceInitSingle());
#endif // !OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

    printf("Free RAM sizes: %lu\n", wiced_memory_get_free_bytes());
}
#endif // !CHIP_HAVE_CONFIG_H

void otSysInit(int argc, char *argv[])
{
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);

    /* Initialize the Alarm Abstraction Layer. */
    otPlatAlramInit();

    /* Initialize the Radio Abstraction Layer. */
    otPlatRadioInit();

#ifndef CHIP_HAVE_CONFIG_H
    /* Initialize the UART Abstraction Layer. */
    otPlatUartInit();

    /* Get event code for system post-initialization. */
    if (!wiced_platform_application_thread_event_register(&system_cb.event_code.post_init, system_post_init))
    {
        printf("%s: Fail to get event code.\n", __FUNCTION__);
        return;
    }

    /* Issue an event for system post-initialization. */
    wiced_platform_application_thread_event_set(system_cb.event_code.post_init);
#endif /* CHIP_HAVE_CONFIG_H */
}

void otSysDeinit(void)
{
}

bool otSysPseudoResetWasRequested(void)
{
    return false;
}

void otSysProcessDrivers(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    wiced_platform_application_thread_event_dispatch();
}
