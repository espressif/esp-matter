/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== ReentSupport.xdc ========
 */

package ti.sysbios.rts.gnu;

import xdc.rov.ViewInfo;

import xdc.runtime.Error;
import xdc.runtime.Assert;

import ti.sysbios.knl.Task;
import ti.sysbios.knl.Semaphore;

/*!
 *  ======== ReentSupport ========
 *  Newlib RTS library re-entrancy support module
 *
 *  The Reentrancy Support module implements locking APIs for the
 *  Newlib libraries and provides an overloaded implementation of
 *  the library's __getreent() function to make the C runtime library
 *  calls re-entrant and thread safe.
 *
 *  The C runtime library (newlib libc/libm) functions internally
 *  call __getreent() to get the address of the currently executing
 *  thread's reentrancy structure.
 *
 *  The __getreent() function allocates storage for the reentrancy
 *  structure when it is called for the very first time within a
 *  thread context. Any subsequent calls to __getreent() within the
 *  same thread context read the current thread's stored context to
 *  determine the previously allocated reentrancy structure's address
 *  and return it.
 *
 *  When a thread is deleted, the DeleteHook is called and will free
 *  any memory that was allocated to store the reentrancy structure
 *  associated with the thread.
 *
 *  The C runtime library calls locking APIs to ensure thread
 *  safety. The locking APIs are defined in the sys/lock.h header
 *  that is distributed with XDC tools. This module provides an
 *  implementation for these locking APIs.
 *
 *  Reentrancy support is enabled by default if tasking is enabled
 *  and can be disabled by adding the following code to the application's
 *  config script.
 *
 *  @p(code)
 *  var ReentSupport = xdc.useModule('ti.sysbios.rts.gnu.ReentSupport');
 *
 *  // 'true' to enable Task level reentrancy support (default)
 *  // 'false' to disable Task level reentrancy support
 *  ReentSupport.enableReentSupport = false;
 *  @p
 *
 *  Note: Calling C runtime functions from SWI and HWI threads
 *        is not supported and will generate an exception if
 *        reentrancy support is enabled.
 *
 */

@Template ("./ReentSupport.xdt") /* generate __getreent() function */
@ModuleStartup

module ReentSupport 
{
    /*!
     *  ======== ModuleView ========
     *  @_nodoc
     */
    metaonly struct ModuleView {
        Bool enableReentSupport;
    }

    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo = 
        ViewInfo.create({
            viewMap: [
            [
                'Module',
                {
                    type: ViewInfo.MODULE,
                    viewInitFxn: 'viewInitModule',
                    structName: 'ModuleView'
                }
            ],
            ]
        });

    /*! 
     *  ======== enableReentSupport ========
     *  Enable re-entrancy support
     */
    config Bool enableReentSupport = true;

    // Asserts

    /*! Asserted in ReentSupport_getReent() */
    config Assert.Id A_badThreadType = {
        msg: "A_badThreadType: Cannot call a C runtime library API from a Hwi or Swi thread."
    };

internal:   /* not for client use */ 

    /* -------- Hook Functions -------- */

    /*
     *  ======== getReent ========
     *  Return a pointer to the current thread's Reentrancy structure
     *
     *  @b(returns) Pointer to current thread's Reentrancy structure
     *
     */
    Ptr getReent();

    /*
     *  ======== initGlobalReent ========
     *  Initialize global re-entrancy structure
     */
    Void initGlobalReent();

    /*
     *  ======== taskCreateHook ========
     *  Create hook function used to create and initialize all task's
     *  hook context.
     *                
     *  @param(task) Handle of the Task to initialize.
     *  @param(eb) Error block.
     *
     */
    Void taskCreateHook(Task.Handle task, Error.Block *eb);

    /*
     *  ======== taskDeleteHook ========
     *  Delete hook function used to remove the task's hook context.
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
        Int               taskHId;   /* Task Hook Context Id for this module */
        Semaphore.Handle  lock;      /* Static binary semaphore handle */
    };
}
