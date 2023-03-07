/*
 * Copyright (c) 2013, Texas Instruments Incorporated
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
 *  ======== Idle.xdc ========
 *
 */
package ti.sysbios.knl;

import xdc.rov.ViewInfo;

/*!
 *  ======== Idle ========
 *  Idle Thread Manager.
 *
 *  The Idle module is used to specify a list of functions to be called
 *  when no other tasks are running in the system. 
 *  
 *  If tasking is enabled (ie {@link ti.sysbios.BIOS#taskEnabled 
 *  BIOS.taskEnabled} = true), then the Task module will create an "Idle task"
 *  with the lowest possible priority. When no other tasks are running, this
 *  idle task runs in an infinite loop, calling the list of functions 
 *  specified by the Idle module. 
 *
 *  If tasking is disabled (ie {@link ti.sysbios.BIOS#taskEnabled 
 *  BIOS.taskEnabled} = false), then the idle functions are called in an 
 *  infinite loop within the {@link ti.sysbios.BIOS#start BIOS_start} 
 *  function called within main().
 *
 *  The list of idle functions is only statically configurable; it cannot be
 *  modified at runtime.
 *
 */

@DirectCall
module Idle
{
    /*! Idle function type definition. */
    typedef Void (*FuncPtr)();

    metaonly struct ModuleView {
        UInt    index;
        UInt    coreId;
        String  fxn;
    }
    
    /*! @_nodoc */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo = 
        xdc.rov.ViewInfo.create({
            viewMap: [
                ['Idle.funcList',
                    {
                        type: ViewInfo.MODULE_DATA,   
                        viewInitFxn: 'viewInitModule',   
                        structName: 'ModuleView'
                    }
                ]
            ]
        });
    
    /*!
     *  ======== funcList ========
     *  @_nodoc
     *  The array of functions to be called when no other Tasks are running.
     */
    config FuncPtr funcList[length] = [];
    
    /*!
     *  ======== coreList ========
     *  @_nodoc
     *  The array of coreIds associated with Idle funcList[]
     */
    config UInt coreList[length] = [];
    
    /*!
     *  ======== idleFxns ========
     *  Functions to be called when no other Tasks are running
     *
     *  Functions added to the Idle.idleFxns[] array, as well as those
     *  added via the Idle.addFunc() API will be run by the Idle loop.
     *
     *  @a(NOTE)
     *  This array is intended for use by the GUI config tool.
     *
     *  Config script authors are encourged to use the 
     *  {@link #addFunc Idle.addFunc()} API to add idle functions
     *  to their applications.
     */
    metaonly config FuncPtr idleFxns[length] = [
        null, null, null, null,  /* slots for GUI */
        null, null, null, null
    ];
    
    /*!
     *  ======== addFunc ========
     *  Statically add a function to the Idle function list.
     *
     *  Functions added to the Idle function list are 
     *  called repeatedly by the Idle task function.
     *
     *  @see Idle#run
     *
     *  Usage example:
     *
     *  @p(code)
     *  var Idle = xdc.useModule('ti.sysbios.knl.Idle');
     *  Idle.addFunc('&myIdleFunc'); // add myIdleFunc() 
     *  @p
     *
     *  @a(NOTE)
     *  Idle functions have the following signature:
     *  @p(code)
     *  Void func(Void);
     *  @p
     */
    metaonly Void addFunc(FuncPtr func);
    
    /*!
     *  ======== addCoreFunc ========
     *  Statically add a core-unique function to the Idle function list.
     */
    metaonly Void addCoreFunc(FuncPtr func, UInt coreId);
    
    /*!
     *  ======== loop ========
     *  @_nodoc
     *  Idle loop which calls the idle functions in an infinite loop.
     *
     *  This function is called internally and is not normally intended
     *  to be called by the client.
     *
     *  When tasking is enabled, the Task module creates an Idle task which
     *  simply calls this function. If tasking is disabled, then this function
     *  is called after main and any module startup functions.
     *
     *  The body of this function is an infinite loop that calls the "run" 
     *  function.
     */
    Void loop(UArg arg1, UArg arg2);

    /*!
     *  ======== run ========
     *  Make one pass through idle functions
     *
     *  This function is called repeatedly by the Idle task when
     *  the Idle task has been enabled in the Task module 
     *  (see {@link Task#enableIdleTask}). 
     *
     *  This function makes one pass through an internal static array 
     *  of functions made up of functions added using the 
     *  {@link #addFunc Idle.addFunc()} API as well as any functions
     *  defined in the GUI tool's {@link #idleFxns Idle.idleFxns[]} array.
     *
     *  This function returns after all functions have been executed one 
     *  time.
     *
     *  @see Idle#addFunc
     *  @see Task#enableIdleTask
     *  @see Task#allBlockedFunc
     */
    Void run();

}
