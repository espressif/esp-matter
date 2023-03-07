/*
 * Copyright (c) 2013-2016, Texas Instruments Incorporated
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
 *  ======== ITaskSupport.xdc ========
 *
 *
 */
import xdc.runtime.Error;

/*!
 *  ======== ITaskSupport ========
 *  @_nodoc
 *  Interface for Task Support Module.
 */

@DirectCall
interface ITaskSupport
{
    /*! @_nodoc Prototype of task enter and exit functions. */
    typedef Void (*FuncPtr)();

    /*! default Task stack size. Value is unique to each target */
    readonly config SizeT defaultStackSize;

    /*! required stack alignment (in MAUs). Unique to each target */
    readonly config UInt stackAlignment;

    /*!
     *  ======== start ========
     *  @_nodoc
     *  Deals with task startup.
     *
     *  This function is called as part of the new Task instance creation.
     *  It is called after the Task instance is almost completely initialized
     *  but before its mask and readyQ have been initialized.
     *
     *  Prior to calling this method, the instance's mode is set to
     *  Task_RUNNING if initStackFlag is TRUE; otherwise mode is
     *  set to Task_BLOCKED.
     *
     *  This method typically initializes the Task instance's stack to
     *  enable it to be switched to via switch(); e.g., by placing a return
     *  to the Task switch and Task exit functions on the stack.
     *
     *  If an error is raised by this method, task creation fails.
     *
     *  @param(curTask) handle to the Task instance
     *  @param(enter)   pointer to a function to call just before entering
     *                  task's main
     *  @param(exit)    pointer to the function to call to exit a task thread
     *
     *  @b(returns)     tskContext (i.e., thread's current stack pointer)
     */
    Ptr start(Ptr curTask, FuncPtr enter, FuncPtr exit, Error.Block *eb);

    /*!
     *  ======== switch ========
     *  @_nodoc
     *  Switch from old task to new task.
     *
     *  @param(oldtskContest)   old task context
     *  @param(newtskContest)   new task context
     */
    Void swap(Ptr *oldtskContext, Ptr *newtskContext);

    /*!
     *  ======== checkStack ========
     *  @_nodoc
     *  Check for stack overflow.
     *
     *  @param(stack)     stack base address
     *  @param(size)      size of stack
     *
     *  &b(returns) TRUE if stack is OK, FALSE if overflow
     */
    Bool checkStack(Char *stack, SizeT size);

    /*!
     *  ======== stackUsed ========
     *  @_nodoc
     *  Returns the task stack usage
     *
     *  @param(stack)     stack base address
     *  @param(size)      size of stack
     */
    SizeT stackUsed(Char *stack, SizeT size);

    /*!
     *  ======== getStackAlignment ========
     *  @_nodoc
     *  Returns the target-specific stackAlignment
     *
     *  b(returns) stackAlignment for this target
     */
    UInt getStackAlignment();

    /*!
     *  ======== getDefaultStackSize ========
     *  @_nodoc
     *  Returns the target-specific defaultStackSize
     *
     *  b(returns) defaultStackSize for this target
     */
    SizeT getDefaultStackSize();

    /*!
     *  ======== getCheckValueAddr ========
     *  @_nodoc
     *  Returns the address of the check value field that is used to
     *  store the computed check value to be used for data integrity
     *  checking of the Task object.
     *
     *  The {@link #start} function reserves 2 to 8 bytes (depending on
     *  the architecture) at the top of the stack for maintaining stack
     *  alignment. This function reclaims 32-bits at the top of the stack
     *  for the purpose of storing the Task object's check value.
     */
    Ptr getCheckValueAddr(Ptr curTask);

    /*!
     *  ======== stackUsed$view ========
     *  @_nodoc
     *  Returns the task stack usage.
     *
     *  Used at ROV time.
     *
     *  @param(stack)     stack (i.e., "maximum" address of Task's stack)
     *  @param(size)      size of stack
     */
    metaonly SizeT stackUsed$view(UInt8 stack[]);

    /*!
     *  ======== getCallStack$view ========
     *  @_nodoc
     *  Returns the task's call stack.
     *
     *  Used by the Task ROV view.
     *
     *  @param(taskRawView)     raw ROV task view
     *  @param(taskState)       interpreted task state
     *  @param(threadType)      current BIOS threadType
     */
    metaonly function getCallStack$view(taskRawView, taskState, threadType);
}

