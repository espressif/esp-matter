/*
 * Copyright (c) 2012-2020, Texas Instruments Incorporated
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
 *  ======== Idle.c ========
 *  Implementation of Idle_loop.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Idle.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/runtime/Startup.h>

#if Task_enableIdleTask_D && BIOS_taskEnabled_D
Task_Struct Idle_taskStruct;
char Idle_taskStack[Task_idleTaskStackSize_D] __attribute__ ((aligned(8)));
#endif

bool Idle_initDone = false;

/*
 *  ======== Idle_init ========
 */
void Idle_init()
{
    /* Interrupts are disabled at this point */
    if (Idle_initDone) {
        return;
    }
    Idle_initDone = true;

#if Task_enableIdleTask_D && BIOS_taskEnabled_D
    Task_Params tskParams;

    Task_Params_init(&tskParams);
    tskParams.priority = 0;
    tskParams.stack = Idle_taskStack;
    tskParams.stackSize = sizeof(Idle_taskStack);
    tskParams.vitalTaskFlag = Task_idleTaskVitalTaskFlag_D;
    tskParams.name = "ti.sysbios.knl.Task.IdleTask";
    Task_construct(&Idle_taskStruct, Idle_loop, &tskParams, NULL);
    Task_module->idleTask = &Idle_taskStruct;
#endif
}

/* REQ_TAG(SYSBIOS-514) */

/*
 *  ======== Idle_loop ========
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
void Idle_loop(uintptr_t arg1, uintptr_t arg2)
{
    /* INFINITE_LOOP.LOCAL */
    for (;;) {
        Idle_run();
    }
}

/*
 *  ======== Idle_run ========
 */
/* REQ_TAG(SYSBIOS-515), REQ_TAG(SYSBIOS-516), REQ_TAG(SYSBIOS-517) */
void Idle_run(void)
{
#if Idle_numFuncs_D
    int i;

    /* CWARN.CONSTCOND.IF */
    for (i = 0; i < Idle_numFuncs_D; i++) {
        Idle_funcList[i]();
    }
#endif
}
