/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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
 *  ======== MultithreadSupport.xdc ========
 */

package ti.sysbios.rts.iar;

import xdc.runtime.Error;
import xdc.runtime.Assert;

import ti.sysbios.knl.Task;
import ti.sysbios.knl.Semaphore;

/*!
 *  ======== MultithreadSupport ========
 *  Provide the kernel support needed for IAR re-entrant C run-time
 *
 *  This multi-thread support module uses hook functions, hook context,
 *  and an overloaded implementation of the C library's lock and thread
 *  local storage access functions to make C run-time library calls
 *  re-entrant.
 *
 *  This module is used only with the IAR compiler.
 *
 *  To enable multi-thread support, load this module in your application
 *  configuration script.
 *
 *  @p(code)
 *  xdc.useModule('ti.sysbios.rts.iar.MultithreadSupport');
 *  @p
 *
 *  If your application is using a module which requires multi-thread
 *  support, then that module is responsible for loading this module.
 *  For example, the `ti.posix.tirtos.Settings` module will load this
 *  module. In this case, it is not necessary to load it explicitly.
 *
 *  When this module is used, it will contribute the IAR linker option
 *  `--threaded_lib` to the linker command.
 *
 *  When using the IAR Embedded Workbench IDE, if you enable thread support
 *  in your project settings, you must also include this module in your
 *  configuration (unless it is already used as described above). There is
 *  no mechanism for the IAR IDE to communicate the project selections to
 *  this module.
 *
 *  On memory limited devices, it is possible to disable the re-entrant
 *  support in order to minimize the memory footprint. See
 *  {@link #metaenableMultithreadSupport enableMultithreadSupport} for
 *  details.
 *
 *  @a(note)
 *  Calling C run-time functions from SWI or HWI threads is not supported
 *  and will generate an exception if  multi-thread support is enabled.
 */

@Template ("./MultithreadSupport.xdt")

module MultithreadSupport
{
    /*!
     *  ======== enableMultithreadSupport ========
     *  Disable the multi-thread support feature
     *
     *  When necessary, it is possible to disable the re-entrant support in
     *  order to minimize the memory footprint. For example, if using POSIX
     *  support on a memory limited device, you may disable re-entrant support
     *  by adding the following to your application configuration script.
     *
     *  @p(code)
     *  var MultithreadSupport = xdc.useModule('ti.sysbios.rts.iar.MultithreadSupport');
     *  MultithreadSupport.enableMultithreadSupport = false;
     *  @p
     *
     *  @a(note)
     *  When multi-thread support is disabled, errno will be a global
     *  symbol. If multiple threads are referencing errno, it will not
     *  be thread-safe.
     */
    config Bool enableMultithreadSupport = true;

    /*!
     *  ======== A_badThreadType ========
     *  Asserted in MultithreadSupport_perThreadAccess()
     *
     *  @_nodoc
     */
    config Assert.Id A_badThreadType = {
        msg: "A_badThreadType: Cannot call a C runtime library API from a Hwi or Swi thread."
    };

    /*!
     *  ======== A_badLockRelease ========
     *  Asserted in MultithreadSupport_releaseLock()
     *
     *  @_nodoc
     */
    config Assert.Id A_badLockRelease = {
        msg: "A_badLockRelease: Trying to release a lock not owned by this thread."
    };

internal:   /* not for client use */

    /*
     *  ======== perThreadAccess ========
     *  Returns a pointer the symbol in the current task's TLS memory
     *
     *  Calculates the symbol address based on the input symbol pointer
     *  in main task's TLS memory and returns the address to the symbol
     *  in the current task's TLS memory.
     *
     *  @param(symbp) Pointer to symbol in the main task's TLS memory.
     *
     */
    Void *perThreadAccess(Void *symbp);

    /*
     *  ======== getTlsPtr ========
     *  Returns a pointer to the current task's TLS memory
     */
    void *getTlsPtr();

    /*
     *  ======== getTlsAddr ========
     *  Return address of thread-local storage buffer
     *
     *  This function is generated. It's implementation differs depending
     *  if SYS/BIOS is in ROM or not.
     */
    void *getTlsAddr();

    /*
     *  ======== initLock ========
     *  Initializes a system lock
     *
     *  Creates a system lock and assigns it to the pointer passed as input.
     *
     *  @param(ptr) Pointer to a lock struct pointer.
     *
     */
    Void initLock(Void **ptr);

    /*
     *  ======== destroyLock ========
     *  Destroy a system lock
     *
     *  Deletes the semaphore in the lock and frees the lock.
     *
     *  @param(ptr) Pointer to a lock struct pointer.
     *
     */
    Void destroyLock(Void **ptr);

    /*
     *  ======== acquireLock ========
     *  Acquire a system lock
     *
     *  Blocks the task if lock is not available. Supports nested calls.
     *
     *  @param(ptr) Pointer to a lock struct pointer.
     *
     */
    Void acquireLock(Void **ptr);

    /*
     *  ======== releaseLock ========
     *  Release a system lock
     *
     *  Releases the lock to other waiting task if any. Supports nested calls.
     *
     *  @param(ptr) Pointer to a lock struct pointer.
     *
     */
    Void releaseLock(Void **ptr);

    /*
     *  ======== taskCreateHook ========
     *  Create task hook function
     *
     *  It is used to create and initialize all task's hook context.
     *
     *  @param(task) Handle of the Task to initialize.
     *  @param(eb) Error block.
     *
     */
    Void taskCreateHook(Task.Handle task, Error.Block *eb);

    /*
     *  ======== taskDeleteHook ========
     *  Delete hook function used to remove the task's hook context
     *
     *  @param(task) Handle of the Task to delete.
     *
     */
    Void taskDeleteHook(Task.Handle task);

    /*
     *  ======== taskRegHook ========
     *  Registration function for the module's hook
     *
     *  @param(id) The id of the hook for use in load.
     *
     */
    Void taskRegHook(Int id);

    /* -------- Internal Module Types -------- */

    struct Module_State {
        Int taskHId;    /* Task Hook Context Id for this module */
        Ptr deletedTaskTLSPtr;
        Task.Handle curTaskHandle;
        Semaphore.Handle lock;
    };
}
