/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== nspmApis.c ========
 *  TF-M PSA Non-Secure Partition Management support code
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Error.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/devices/cc13x4_cc26x4/cmsis/core/tz_context.h>
#include "svcHandler.h"
#include "tfm_api.h"

static int nspmHookSetId;
static TZ_ModuleId_t dummyModule;

/*
 *  ======== nspmGetCurrentClientId ========
 *  Return the clientId associated with the currently running task
 */
int32_t nspmGetCurrentClientId(void)
{
    return ((int32_t)Task_self() | 0x80000000);
}

/*
 *  ======== nspmRegisterHook ========
 *  Invoked during Task module startup before main for each HookSet
 */
void nspmRegisterHook(int hookSetId)
{
    nspmHookSetId = hookSetId;
    initContextSystem();
}

/*
 *  ======== nspmCreateHook ========
 *  Invoked during Task_create for dynamically created Tasks
 */
void nspmCreateHook(Task_Handle task, Error_Block *eb)
{
    TZ_MemoryId_t memoryId;

    /* get an SPM memory ID associated with this task */
    memoryId = allocModuleContext(dummyModule);

    /* save it as this task's hook context */
    Task_setHookContext(task, nspmHookSetId, (void *)memoryId);
}

/*
 *  ======== nspmSwitchHook ========
 *  Invoked whenever a Task switch is made ready to run
 */
void nspmSwitchHook(Task_Handle prev, Task_Handle next)
{
    void * prevMemoryId;
    void * nextMemoryId;

    if (prev != NULL) {
        prevMemoryId = Task_getHookContext(prev, nspmHookSetId);
        storeContext((TZ_MemoryId_t)prevMemoryId);
    }

    nextMemoryId = Task_getHookContext(next, nspmHookSetId);
    loadContext((TZ_MemoryId_t)nextMemoryId);
/*
 * !!! tfm_register_client_id() is currently (10/15/2019) only supported in the library model for PSA
    tfm_register_client_id((int32_t)next | 0x80000000);
 */
}

/*
 *  ======== nspmDeleteHook ========
 *  Invoked upon Task deletion
 */
void nspmDeleteHook(Task_Handle task)
{
    void * memoryId;

    memoryId = Task_getHookContext(task, nspmHookSetId);
    freeModuleContext((TZ_MemoryId_t)memoryId);
}
