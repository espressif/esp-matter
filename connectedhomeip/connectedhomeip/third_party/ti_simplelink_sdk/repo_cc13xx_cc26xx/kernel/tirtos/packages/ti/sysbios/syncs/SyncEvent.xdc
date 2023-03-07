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
 *  ======== SyncEvent.xdc ========
 *
 */

import xdc.rov.ViewInfo;
import xdc.runtime.Assert;

import ti.sysbios.knl.Event;

/*!
 *  ======== SyncEvent ========
 *  Implements ISync using events.
 *
 *  Module that implements {@link xdc.runtime.knl.ISync} using
 *  {@link ti.sysbios.knl.Event}
 *
 *  This module uses {@link ti.sysbios.knl.Event#post} to implement
 *  {@link xdc.runtime.knl.ISync#signal}
 *
 *  The implementation of {@link xdc.runtime.knl.ISync#wait} is empty 
 *  and returns
 *  FALSE for timeout, since with events the pend call is made outside.
 *
 *  Modules like {@link ti.sdo.io.Stream} use an ISync.Handle for
 *  selecting the synchronization mechanism. This allows Stream to be
 *  independent of the synchronization mechanism.
 *
 *  An instance of SyncEvent requires an Event.Handle. When given a null
 *  Event.Handle, SynEvent will assert. 
 */

module SyncEvent inherits xdc.runtime.knl.ISync 
{

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String label;
        String EventHandle;
        UInt   EventId;
        String pendedTask;
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
                'Basic',
                {
                    type: ViewInfo.INSTANCE,
                    viewInitFxn: 'viewInitBasic',
                    structName: 'BasicView'
                }
            ],
            ]
        });

    /*! 
     *  Assert when baseHandle is null
     */
    config Assert.Id A_nullHandle  = {
        msg: "A_nullHandle: Null handle passed to create"
    };

instance:
    /*! 
     *  Event handle must be provided.
     */  
    config Event.Handle event = null;
    
    /*! Event id to be when signal() gets invoked. */
    config UInt eventId = Event.Id_00;

internal:

    /* -------- Internal Structures -------- */
    struct Instance_State {
        Event.Handle         evt;
        UInt                 evtId;
    };

}
