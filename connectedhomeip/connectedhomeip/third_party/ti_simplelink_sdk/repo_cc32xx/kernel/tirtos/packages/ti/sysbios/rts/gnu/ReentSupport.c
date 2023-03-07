/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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
 *  ======== ReentSupport.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>

#include "package/internal/ReentSupport.xdc.h"

#include <stdio.h>
#include <reent.h>
#include <string.h>

extern void ti_sysbios_rts_gnu_ReentSupport_checkIfCorrectLibrary();

/*
 *  ======== ReentSupport_Module_startup ========
 */
Int ReentSupport_Module_startup (Int phase)
{
    /*
     * Create a dummy lock object (defined in sys/lock.h) and try to
     * initialize its fields to ensure we are using the correct header
     * file include path. If the wrong header files are used (that do
     * not have the correct lock structure definitions), a compiler
     * error will be generated.
     */
    volatile _LOCK_T lock;

    lock.init_done = 1;
    lock = lock;        /* Suppress unused variable compiler warning */

    /*
     * Make a call to a special stub function in the Newlib BSP library.
     * If the application is not linking with the correct BSP library
     * (i.e. it is not linking with the Newlib BSP library shipped
     * with SYS/BIOS), the function symbol will not be found and the
     * link will fail, thereby indicating to the user that the wrong
     * library was being linked with the application. (see FAQ section of
     * http://processors.wiki.ti.com/index.php/SYS/BIOS_with_GCC_(CortexA)
     * wiki page for more info).
     */
    if (ReentSupport_enableReentSupport) {
        ti_sysbios_rts_gnu_ReentSupport_checkIfCorrectLibrary();
    }

    return (Startup_DONE);
}

/*
 *************************************************************************
 *                       Internal functions
 *************************************************************************
 */

/*
 *  ======== ReentSupport_initGlobalReent ========
 */
Void ReentSupport_initGlobalReent()
{
    _REENT_SMALL_CHECK_INIT(_GLOBAL_REENT);
}

/*
 *  ======== ReentSupport_taskRegHook ========
 *  The Task register hook is called once per hookset, before main and before
 *  any Task initialization has been done. 
 *  
 *  This function allows the ReentSupport module to store its hookset id,
 *  which is passed to Task_get/setHookContext. The HookContext can be an
 *  arbitrary structure. The ReentSupport module has defined a HookContext
 *  to be of type struct reent*.
 */
Void ReentSupport_taskRegHook(Int id)
{
    ReentSupport_module->taskHId = id;
}

/*
 *  ======== ReentSupport_taskDeleteHook ========
 */
Void ReentSupport_taskDeleteHook(Task_Handle task)
{
    struct _reent *pStoredContext;

    pStoredContext = 
        (struct _reent *)Task_getHookContext(task, 
                                             ReentSupport_module->taskHId);

    if (pStoredContext != NULL) {
        _reclaim_reent(pStoredContext);

        /*
         * Newlib nano does not close the file descriptors allocated for
         * stdin/stdout/stderr in _cleanup_r() called from _reclaim_reent().
         * This can cause a memory leak as the stdin/stdout/stderr file
         * objects are not marked as free when the Task is deleted. Therefore,
         * the now unused file objects cannot be reused, causing Newlib to
         * allocate more memory for new file objects.
         */
        _fclose_r(pStoredContext, pStoredContext->_stdin);
        _fclose_r(pStoredContext, pStoredContext->_stdout);
        _fclose_r(pStoredContext, pStoredContext->_stderr);

        Memory_free(Task_Object_heap(), pStoredContext, sizeof(struct _reent));
    }
}

/*
 *  ======== ReentSupport_getReent ========
 */
Ptr ReentSupport_getReent()
{
    Task_Handle task;
    struct _reent *pCurTaskEnv;

    Assert_isTrue(((BIOS_getThreadType() != BIOS_ThreadType_Hwi) &&
                   (BIOS_getThreadType() != BIOS_ThreadType_Swi)),
                    ReentSupport_A_badThreadType);

    task = Task_self();

    pCurTaskEnv = (struct _reent *)Task_getHookContext(task,
            ReentSupport_module->taskHId);

    if (pCurTaskEnv == NULL) {
        /*
         * In case Memory_alloc fails, the heap proxy's alloc function
         * will internally call Error_raise(). Error_raise() logs the
         * error message and if the System proxy is SysStd, it will call
         *  __getreent() which ends up calling this function.
         *
         * In order to prevent an infinite call loop, assign the global
         * reentrancy structure to the current task. If allocation fails
         * and the System proxy is SysStd, it will let Error_raise()
         * print the error message.
         *
         * Note: Global reentrancy structure is initialized in
         *       newlib/libc/reent/impure.c
         */
        pCurTaskEnv = _GLOBAL_REENT;
        Task_setHookContext(task, ReentSupport_module->taskHId, pCurTaskEnv);

        pCurTaskEnv = Memory_alloc(Task_Object_heap(), sizeof(struct _reent),
                            0, NULL);

        _REENT_INIT_PTR(pCurTaskEnv);
        _REENT_SMALL_CHECK_INIT(pCurTaskEnv);

        Task_setHookContext(task, ReentSupport_module->taskHId, pCurTaskEnv);
    }

    return (Ptr)(pCurTaskEnv);
}
