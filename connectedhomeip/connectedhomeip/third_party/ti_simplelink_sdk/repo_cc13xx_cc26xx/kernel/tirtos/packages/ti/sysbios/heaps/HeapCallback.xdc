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
 *  ======== HeapCallback.xdc ========
 */

package ti.sysbios.heaps;

import xdc.rov.ViewInfo;
import xdc.runtime.Memory;

/*!
 *  ======== HeapCallback ========
 *  A heap that calls user supplied callback functions
 *
 *  The HeapCallback module enables users to provide a custom heap
 *  implementation by providing callback functions that will be invoked
 *  by HeapCallback for the various heap management functions.
 *
 *  The user-supplied HeapCallback.initInstFxn is called during boot time to
 *  initialize any HeapCallback objects that were created in the .cfg file.
 *  The user-supplied HeapCallback.createInstFxn is called during runtime
 *  for any calls to HeapCallback_create().  Both of these functions return
 *  a context value (typically a pointer to an object managed by the
 *  user-supplied heap code).  This context value is passed to subsequent
 *  user allocInstFxn, freeInstFxn, etc. functions.
 *
 *  HeapCallback_alloc(), HeapCallback_free() and HeapCallback_getStats()
 *  call the user-supplied allocInstFxn, freeInstFxn and getStatsInstFxn
 *  functions with the context value returned by initInstFxn or createInstFxn.
 *
 *  HeapCallback_delete() calls the user-supplied instDeleteFxn with the
 *  context returned by the createInstFxn.
 *
 *  @a(Examples)
 *  Configuration example: The following XDC configuration statements
 *  creates a HeapCallback instance and plugs in the user defined functions.
 *
 *  @p(code)
 *  var HeapCallback = xdc.useModule('ti.sysbios.heaps.HeapCallback');
 *
 *  var params = new HeapCallback.Params();
 *  params.arg = 1;
 *  HeapCallback.create(params);
 *
 *  HeapCallback.initInstFxn = '&userInitFxn';
 *  HeapCallback.createInstFxn = '&userCreateFxn';
 *  HeapCallback.deleteInstFxn = '&userDeleteFxn';
 *  HeapCallback.allocInstFxn = '&userAllocFxn';
 *  HeapCallback.freeInstFxn = '&userFreeFxn';
 *  HeapCallback.getStatsInstFxn = '&userGetStatsFxn';
 *  HeapCallback.isBlockingInstFxn = '&userIsBlockingFxn';
 *
 *  @p
 */
@ModuleStartup
@InstanceFinalize
module HeapCallback inherits xdc.runtime.IHeap {

    /*!  @_nodoc */
    metaonly struct BasicView {
        UArg        arg;
        UArg        context;
    }

    /*!  @_nodoc */
    metaonly struct ModuleView {
        String initInstFxn;
        String createInstFxn;
        String deleteInstFxn;
        String allocInstFxn;
        String freeInstFxn;
        String getStatsInstFxn;
        String isBlockingInstFxn;
    }

    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config xdc.rov.ViewInfo.Instance rovViewInfo =
        xdc.rov.ViewInfo.create({
            viewMap: [
                ['Basic',
                    {
                        type: ViewInfo.INSTANCE,
                        viewInitFxn: 'viewInitBasic',
                        structName: 'BasicView'
                    }
                ],
                ['Module',
                    {
                        type: xdc.rov.ViewInfo.MODULE,
                        viewInitFxn: 'viewInitModule',
                        structName: 'ModuleView'
                    }
                ]
            ]
        });

    /*!
     *  ======== AllocInstFxn ========
     *  Instance alloc callback function signature
     *
     *  This function takes the context return from createInstFxn(), the
     *  size to be allocated and the align value.  The return value from
     *  this function is a pointer to the allocated memory block.
     */
    typedef Ptr (*AllocInstFxn)(UArg, SizeT, SizeT);

    /*!
     *  ======== CreateInstFxn ========
     *  Instance create callback function signature
     *
     *  {@link HeapCallback#arg} is passed as an argument to this function.
     *  The return value from this function (context) will be passed as an
     *  argument to the other instance functions.
     */
    typedef UArg (*CreateInstFxn)(UArg);

    /*!
     *  ======== DeleteInstFxn ========
     *  Instance delete callback function signature
     *
     *  The context returned from createInstFxn() is passed as an argument to
     *  this function.
     */
    typedef Void (*DeleteInstFxn)(UArg);

    /*!
     *  ======== FreeInstFxn ========
     *  Instance free callback function signature
     *
     *  This function takes the context returned by createInstFxn() and a
     *  pointer to the buffer to be freed and the size to be freed.
     */
    typedef Void (*FreeInstFxn)(UArg, Ptr, SizeT);

    /*!
     *  ======== GetStatsInstFxn ========
     *  Instance getStats callback function signature
     *
     *  This function takes the context returned by createInstFxn() and a
     *  pointer to a memory stats object.
     */
    typedef Void (*GetStatsInstFxn)(UArg, Memory.Stats *);

    /*!
     *  ======== InitInstFxn ========
     *  Instance init callback function signature
     *
     *  `{@link HeapCallback#arg} is passed as an argument to this function.
     *  The return value from this function (context) will passed as an
     *  argument to the other instance functions.
     */
    typedef UArg (*InitInstFxn)(UArg);

    /*!
     *  ======== IsBlockingInstFxn ========
     *  Instance isblocking callback function signature
     *
     *  The context return from createInstFxn() is passed as an argument to
     *  this function. The return value is 'TRUE' or 'FALSE'.
     */
    typedef Bool (*IsBlockingInstFxn)(UArg);

    /*!
     *  ======== allocInstFxn ========
     *  User supplied instance alloc function
     *
     *  This function is called when the `{@link HeapCallback#alloc}` is
     *  called.
     *
     *  This parameter is configured with a default alloc function. The
     *  default alloc function returns NULL.
     */
    config AllocInstFxn allocInstFxn = '&ti_sysbios_heaps_HeapCallback_defaultAlloc';

    /*!
     *  ======== createInstFxn ========
     *  User supplied instance create function
     *
     *  This function is called when the `{@link HeapCallback#create}` is
     *  called.
     *
     *  This parameter is configured with a default create function.
     *  The default create function returns 0.
     */
    config CreateInstFxn createInstFxn = '&ti_sysbios_heaps_HeapCallback_defaultCreate';

    /*!
     *  ======== deleteInstFxn ========
     *  User supplied instance delete function
     *
     *  This function is called when the `{@link HeapCallback#delete}` is
     *  called.
     *
     *  This parameter is configured with a default delete function.
     */
    config DeleteInstFxn deleteInstFxn = '&ti_sysbios_heaps_HeapCallback_defaultDelete';

    /*!
     *  ======== freeInstFxn ========
     *  User supplied instance free function
     *
     *  This function is called when the `{@link HeapCallback#free}` is
     *  called.
     *
     *  This parameter is configured with a default free function.
     */
    config FreeInstFxn freeInstFxn = '&ti_sysbios_heaps_HeapCallback_defaultFree';

    /*!
     *  ======== getStatsInstFxn ========
     *  User supplied getStat function
     *
     *  This function is called when the `{@link HeapCallback#getStat}` is
     *  called.
     *
     *  This parameter is configured with a default getStats function.
     *  The default getStats function does not make any changes to stats
     *  structure.
     */
    config GetStatsInstFxn getStatsInstFxn = '&ti_sysbios_heaps_HeapCallback_defaultGetStats';

    /*!
     *  ======== initInstFxn ========
     *  User supplied init function
     *
     *  This function is called in Module startup for statically created
     *  instances.  The user-supplied function must not do any memory
     *  allocations or call any module create functions (e.g.,
     *  Semaphore_create()).
     *
     *  This parameter is configured with a default init function.  The
     *  default init function returns NULL.
     */
    config InitInstFxn initInstFxn = '&ti_sysbios_heaps_HeapCallback_defaultInit';

    /*!
     *  ======== isBlockingInstFxn ========
     *  User supplied isBlocking function
     *
     *  This function is called when the `{@link HeapCallback#isBlocking}` is
     *  called.
     *
     *  This parameter is configured with a default isBlocking function.  The
     *  default isBlocking returns FALSE.
     */
    config IsBlockingInstFxn isBlockingInstFxn = '&ti_sysbios_heaps_HeapCallback_defaultIsBlocking';

instance:

    /*!
     *  ======== create ========
     *  Create a `HeapCallback` Heap instance
     */
    create();

    /*!
     *  ======== getContext ========
     *  Get the context returned from user supplied create function
     */
    UArg getContext();

    /*!
     *  ======== arg ========
     *  User supplied argument for the user supplied create function.
     *
     *  This user supplied argument will be passed back as an argument to the
     *  `createInstFxn` function. It can be used by the
     *  `{@link HeapCallback#createInstFxn}` function at runtime to
     *  differentiate between the multiple Heap instances configured in the
     *  user config script.
     *
     *  The user can skip configuring this argument. In such a case, the
     *  default value `0` will be passed back as an argument to the
     *  `createInstFxn` function.
     */
    config UArg arg = 0;

internal:   /* not for client use */

    struct Instance_State {
        UArg context;       /* context returned from createInstFxn */
        UArg arg;           /* argument to createInstFxn */
    };
}
