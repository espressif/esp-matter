/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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
 *  ======== TaskSupport.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>

#define ti_sysbios_knl_Task__internalaccess
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/interfaces/ITaskSupport.h>
#include "package/internal/TaskSupport.xdc.h"

#define TaskSupport_buildTaskStack ti_sysbios_family_arm_m3_TaskSupport_buildTaskStack

extern Ptr TaskSupport_buildTaskStack(Ptr stack, Task_FuncPtr fxn, TaskSupport_FuncPtr exit, TaskSupport_FuncPtr enter, UArg arg0, UArg arg1);

/*
 *
 * Here is a pseudo image of the initial C6x task stack:
 *
 *  saved-by-callee registers
 *  Task enter()
 *  Task glue()
 *  arg0
 *  arg1
 *  User's task func()
 *  Task_exit()
 *
 * The initial stack consists of the registers that are preserved by a
 * called C function as defined by the C compiler. These are the registers
 * pushed and popped by TaskSupport_swap.
 *
 * Below (or above, depending on your view of the stack) those registers
 * are the Task enter() function which sets up the first invocation of the
 * task in the same manner as if it had been "returned" to from Task_restore()
 * which is the normal path back to an unblocked task.
 *
 * Below the Task enter() function address on the initial task stack is the
 * address of the Glue function. This address is popped by the return from
 * Task enter(). Glue pops the 2 UArgs below it on the stack into the
 * appropriate registers for a fxn(arg0, arg1) call, and then "returns"
 * into the user's task function, which is the next to last address on
 * the stack.
 *
 * Below the Task's function address on the stack is the Task_exit()
 * function address which is returned into if the task function falls
 * out the bottom.
 *
 * The TaskSupport_swap() function pushes all the saved-by-callee
 * registers onto the task stack, then saves the updated SP into
 * the "old" task object's context (SP) address passed to it. Then it
 * loads the "new" task object's context (SP) (the second arg passed
 * to swap) into the SP, unrolls the saved registers and returns into
 * the new task.
 *
 * The pointer returned by the buildTaskStack function is the task's initial
 * stack pointer. This address is stored into the task object's context
 * field and is passed to the TaskSupport_swap() function later.
 *
 */

/*
 *  ======== TaskSupport_start ========
 *  Create a task's initial stack image
 */
Ptr TaskSupport_start(Ptr currTsk, ITaskSupport_FuncPtr enter, ITaskSupport_FuncPtr exit, Error_Block *eb)
{
    Ptr sp;
    UInt size;
    Char *sptr;
    Task_Object *tsk = (Task_Object *)(currTsk);

    if (Task_initStackFlag) {
        sptr = (Char *)tsk->stack;
        size = tsk->stackSize;
        while (size--) {
            *sptr++ = 0xbe;     /* fill stack with known cookie */
        }
    }
    /* Still allow for stack overflow checking */
    else if (Task_checkStackFlag) {
        sptr = (Char *)tsk->stack;
        *sptr = 0xbe;
    }

    /*
     * The stack buffer is already aligned on 8 bytes.
     * buildTaskStack creates a stack image that results in 8 byte alignment
     * on Task func entry only if passed a 4 byte aligned stack ptr
     *
     * The top 32-bits of the stack are reclaimed by
     * TaskSupport_getCheckValueAddr() as storage for the Task object's
     * check value when Task object data integrity checking is enabled.
     */
    sp = TaskSupport_buildTaskStack((Ptr)((SizeT)tsk->stack + tsk->stackSize-4), tsk->fxn, exit, enter, tsk->arg0, tsk->arg1);

    return (sp);
}

/*
 *  ======== TaskSupport_checkStack ========
 *  Check for stack overflow.
 */
Bool TaskSupport_checkStack(Char *stack, SizeT size)
{
    if (*stack != (Char)0xbe) {
        return (FALSE);         /* stack overflow (or corrupted) */
    }
    else {
        return (TRUE);          /* stack OK */
    }
}

/*
 *  ======== TaskSupport_stackUsed ========
 *  Returns the task stack usage
 *
 *  @param(task)     Task_Object Ptr
 */
SizeT TaskSupport_stackUsed(Char *stack, SizeT size)
{
    Char *sp;

    sp = stack;

    do {
    } while(*sp++ == (Char)0xbe);

    return (size - (--sp - stack));
}

/*
 *  ======== getDefaultStackSize ========
 */
SizeT TaskSupport_getDefaultStackSize()
{
    return (TaskSupport_defaultStackSize);
}

/*
 *  ======== getDefaultStackAlignment ========
 */
UInt TaskSupport_getStackAlignment()
{
    return (TaskSupport_stackAlignment);
}

/*
 *  ======== getCheckValueAddr ========
 */
Ptr TaskSupport_getCheckValueAddr(Ptr curTask)
{
    Task_Object *tsk = (Task_Object *)(curTask);

    return ((Ptr)((SizeT)tsk->stack + tsk->stackSize - 4));
}
