/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 *  ======== error.c ========
 */

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/drivers/Board.h>

Void task1(UArg arg0, UArg arg1);
Void task2(UArg arg0, UArg arg1);
Void timerFxn(UArg arg);

/* Pre-configured hook function called when errors are raised */
Void errHook(Error_Block *eb);
Int canFail1(Error_Block *eb, UInt val);
Void canFail2(Error_Block *eb, UInt val);

Task_Handle task1Handle, task2Handle;
Int numTasksCreated = 0;

/*
 *  ======== main ========
 */
int main()
{
    Task_Params taskParams;
    Error_Block eb;
    Memory_Stats stats;

    /* Call driver init functions */
    Board_init();

    Error_init(&eb);

    /* Picking a stackSize such that the second Task_create() will fail */
    Memory_getStats(Memory_defaultHeapInstance, &stats);

    Task_Params_init(&taskParams);
    taskParams.priority = 1;
    taskParams.stackSize = (stats.totalFreeSize/2) + 64;

    /*
     * Create two tasks, The first one succeeds and the second one fails
     * We catch the second failure in the Error_Block
     */
    task1Handle = Task_create(task1, &taskParams, &eb);

    if (Error_check(&eb)) {
        /* Should not get here */
        System_printf("First Task_create() failed\n");
        BIOS_exit(0);
    }

    Error_init(&eb);

    task2Handle = Task_create(task2, &taskParams, &eb);

    if (Error_check(&eb)) {
        /* Should get here */
        System_printf("Second Task_create() failed\n");
    }

    BIOS_start();    /* Does not return */
    return(0);
}

/*
 *  ======== task1 ========
 */
Void task1(UArg arg0, UArg arg1)
{
    Error_Block eb;
    Int value;

    Error_init(&eb);
    System_printf("Running task1 function\n");

    /*
     * Showing a case where we call a function that takes an Error_Block.
     * This function calls another function which can fail.
     * The Error_Block to passed up to the caller in case an error occurs.
     */
    value = canFail1(&eb, 11);
    System_printf("Value returned from canFail1 = %d\n", value);

    if (Error_check(&eb)) {
        /* Should get here */
        System_printf("Incorrect value used. Must be a multiple of 2\n");
    }

    /*
     * Showing a case where a Memory_alloc() is called but no Error_Block
     * is passed.  This Memory_alloc() will fail because the size is larger
     * than the available memory in the default heap.
     *
     * This call will cause the program to abort.  The error message will
     * be output to the SysMin buffer and then flushed to the console.
     */
    Memory_alloc(NULL, 0xffffff, 0, NULL);
}

/*
 *  ======== timerFxn ========
 */
Void timerFxn(UArg arg)
{
    System_printf("Running timerFxn\n");
}

/*
 *  ======== task2 ========
 */
Void task2(UArg arg0, UArg arg1)
{
    System_printf("Running task2 function\n");
}

/*
 *  ======== errorHook ========
 */
Void errorHook(Error_Block *eb)
{
    System_printf(Error_getMsg(eb), Error_getData(eb)->arg[0],
        Error_getData(eb)->arg[1]);
    System_printf("\n");
}

/*
 *  ======== canFail1 ========
 */
Int canFail1(Error_Block *eb, UInt val)
{
    canFail2(eb, val);

    if (Error_check(eb)) {
        return (-1);
    }
    else {
        return (val * 10);
    }
}

/*
 *  ======== canFail2 ========
 *  fails if val is not a mulitple of 2.
 */
Void canFail2(Error_Block *eb, UInt val)
{
    if (val % 2) {
        Error_raise(eb, Error_E_generic, "Value is not a multiple of 2", 0);
    }
}
