/*
 * Copyright (c) 2018-2020, Texas Instruments Incorporated
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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/TaskSupport.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Types.h>

extern void TaskSupport_glue();
extern void TaskSupport_glueUnpriv();

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

const unsigned int TaskSupport_stackAlignment = 8;

/*
 *  ======== TaskSupport_init ========
 */
void TaskSupport_init(void)
{
}

/*
 *  ======== TaskSupport_buildTaskStack ========
 *
 *  initial stack image:
 *
 *      dummy
 *      stack limit
 *      d8-d15 = 0      if vfp supported
 *      r4 = -1
 *      r5 = -1
 *      r6 = -1
 *      r7 = -1
 *      r8 = -1
 *      r9 = -1
 *      r10 = -1
 *      r11 = -1
 *      glue
 *      arg0
 *      arg1
 *      fxn
 *      exit
 *      enter
 */
void * TaskSupport_buildTaskStack(void * stackBase, size_t stackSize, Task_FuncPtr fxn,
    TaskSupport_FuncPtr exit, TaskSupport_FuncPtr enter, uintptr_t arg0, uintptr_t arg1)
{
    uint32_t *stack = (uint32_t*)stackBase;
    size_t idx = stackSize >> 2;

    stack[--idx] = (uint32_t)enter;
    stack[--idx] = (uint32_t)exit;
    stack[--idx] = (uint32_t)fxn;
    stack[--idx] = (uint32_t)arg1;
    stack[--idx] = (uint32_t)arg0;

    stack[--idx] = (uint32_t)&TaskSupport_glue;

    /* Init r4-r11 with -1 */
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);

#if (defined(__ti__) && defined(__ARM_FP)) || \
    (defined(__IAR_SYSTEMS_ICC__) && defined(__ARMVFP__)) || \
    (defined(__GNUC__) && !defined(__ti__) && \
     defined(__VFP_FP__) && !defined(__SOFTFP__)) || \
    (defined(__clang__) && \
     defined(__VFP_FP__) && !defined(__SOFTFP__))
    /* Make room for 8 VFP saved by parent regs D8-D15 */
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
    stack[--idx] = (~0);
#endif

#if (defined(__IAR_SYSTEMS_ICC__) && (__CORE__ == __ARM8M_MAINLINE__)) || \
    (defined(__GNUC__) && !defined(__ti__) && \
     defined(__ARM_ARCH_8M_MAIN__)) || \
    (defined(__clang__) && defined(__ARM_ARCH_8M_MAIN__))
    /*
     * Store task stack base address in initial stack frame. Task swap function
     * will read this and program the hardware stack limit register.
     */
    stack[--idx] = (~0);
    stack[--idx] = (uint32_t)stackBase;
#endif

    return (&stack[idx]);
}

/*
 *  ======== TaskSupport_start ========
 *  Create a task's initial stack image
 */
void * TaskSupport_start(void * currTsk, TaskSupport_FuncPtr enter, TaskSupport_FuncPtr exit, Error_Block *eb)
{
    void * sp;
    unsigned int size;
    char *sptr;
    Task_Object *tsk = (Task_Object *)(currTsk);

    if (Task_initStackFlag) {
        sptr = (char *)tsk->stack;
        size = tsk->stackSize;
        while (size--) {
            *sptr++ = 0xbe;     /* fill stack with known cookie */
        }
    }
    /* Still allow for stack overflow checking */
    else if (Task_checkStackFlag) {
        sptr = (char *)tsk->stack;
        *sptr = 0xbe;
    }

    /*
     * The task stack buffer is aligned to 8 bytes by the
     * Task create code and buildTaskStack preserves the
     * 8-byte stack alignment. However, the Task swap code
     * pops an odd number of registers leaving the stack
     * 4-byte aligned on Task func entry. Therefore, the
     * stack size passed to buildTaskStack must be adjusted
     * such that the resulting stack is 4-byte aligned.
     * With this change, the stack will be 8-byte aligned
     * on Task func entry.
     */
    sp = TaskSupport_buildTaskStack((void *)tsk->stack, tsk->stackSize - 4, tsk->fxn, exit, enter, tsk->arg0, tsk->arg1);

    return (sp);
}

/*
 *  ======== TaskSupport_checkStack ========
 *  Check for stack overflow.
 */
bool TaskSupport_checkStack(char *stack, size_t size)
{
    if (*stack != (char)0xbe) {
        return (false);         /* stack overflow (or corrupted) */
    }
    else {
        return (true);          /* stack OK */
    }
}

/*
 *  ======== TaskSupport_stackUsed ========
 *  Returns the task stack usage
 *
 *  @param(task)     Task_Object void *
 */
size_t TaskSupport_stackUsed(char *stack, size_t size)
{
    char *sp;

    sp = stack;

    do {
    } while(*sp++ == (char)0xbe);

    return (size - (--sp - stack));
}

/*
 *  ======== TaskSupport_getStackAlignment ========
 */
unsigned int TaskSupport_getStackAlignment()
{
    return (TaskSupport_stackAlignment);
}
