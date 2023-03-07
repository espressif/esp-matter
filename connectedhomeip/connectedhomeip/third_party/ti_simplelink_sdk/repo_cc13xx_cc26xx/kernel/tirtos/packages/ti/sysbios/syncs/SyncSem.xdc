/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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
 *  ======== SyncSem.xdc ========
 *
 */

import xdc.rov.ViewInfo;

import ti.sysbios.knl.Semaphore;

/*!
 *  ======== SyncSem ========
 *  Implements ISyncs using semaphores.
 *
 *  Module that implements {@link xdc.runtime.knl.ISync} using
 *  {@link ti.sysbios.knl.Semaphore}
 *
 *  This module uses {@link ti.sysbios.knl.Semaphore#pend} to implement
 *  {@link xdc.runtime.knl.ISync#wait} and {@link ti.sysbios.knl.Semaphore#post}
 *  to implement {@link xdc.runtime.knl.ISync#signal}.
 *
 *  Modules like {@link ti.sdo.io.Stream} use an ISync.Handle for
 *  selecting the synchronization mechanism. This allows Stream to be
 *  independent of the synchronization mechanism.
 *
 *  An instance of SyncSem requires a Semaphore.Handle. When given a null
 *  Semaphore.Handle, SynSem will create a Semaphore for its use. Note that
 *  the underlying Semaphore has to be binary in nature and initialized as
 *  being unavailable.
 */

@InstanceFinalize
@InstanceInitError

module SyncSem inherits xdc.runtime.knl.ISync 
{

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String label;
        String SemaphoreHandle;
        String pendedTasks[];
    }
    
    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo = 
        ViewInfo.create({
            viewMap: [
                ['Basic', {type: ViewInfo.INSTANCE, viewInitFxn: 'viewInitBasic', structName: 'BasicView'}],
            ]
        });

instance:

    /*! 
     *  Semaphore handle to be used. When Semaphore.Handle is null SyncSem 
     *  will create a Semaphore instance.
     */  
    config Semaphore.Handle sem = null;
    
internal:

    /* -------- Internal Structures -------- */
    struct Instance_State {
        Bool                userSem;
        Semaphore.Handle    sem;
    };

}
