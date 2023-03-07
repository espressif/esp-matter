/*
 * Copyright (c) 2012-2016, Texas Instruments Incorporated
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
 * */

/*
 *  ======== IpcMP.xdc ========
 */

package ti.uia.sysbios;

import xdc.runtime.Assert;
import ti.uia.runtime.UIAPacket;
import ti.sysbios.knl.Semaphore;
import ti.sysbios.knl.Clock;
import ti.sysbios.knl.Task;
import ti.sysbios.knl.Event;
import ti.sysbios.syncs.SyncEvent;

/*!
 *  ======== IpcMP ========
 */

@ModuleStartup      /* Initialize static instances */

module IpcMP inherits ti.uia.runtime.IServiceMgrSupport
{
    /*!
     *  Assert raised IpcMP interaction gets unexpected failure
     */
    config Assert.Id A_IpcMPFailure  = {
        msg: "A_IpcMPFailure: Unexpected failure with the IpcMP"
    };

    /*!
     *  SharedRegion used to allocate messages
     */
    config Int sharedRegionId = 0;

    // -------- Module Functions --------

    /*!
     *  @_nodoc
     *  ======== rxTaskFxn ========
     *  Function used for the transfer agent Task.
     */
    @DirectCall
    Void rxTaskFxn(UArg arg0, UArg arg1);

    /*!
     *  @_nodoc
     *  ======== transferAgentFxn ========
     *  Function used for the transfer agent Task.
     */
    @DirectCall
    Void transferAgentFxn(UArg arg0, UArg arg1);

    /*!
     *  @_nodoc
     *  ======== clockFxn ========
     *  Function used for the RTA Agent's Clock instance.
     *
     *  The Agent C code does not contain any references to this function, so
     *  this function has been made public and nodoc'd, rather than being made
     *  internal, so that it does not fall away in a ROM build.
     */
    @DirectCall
    Void clockFxn(UArg arg0);

    /*!
     *  @_nodoc
     *  ======== start ========
     */
    @DirectCall
    Int start(UArg arg, UInt16 value);

    /*!
     *  ======== doNotPlugIpc ========
     *  Work-around for Syslink bug SDOCM00077375
     *
     *  Only call if you are using Syslink version 2.00.00.66
     */
    metaonly Void doNotPlugIpc();

internal:
    /*!
     *  ======== createRxTask ========
     */
    config Bool createRxTask = false;

    /*!
     *  @_nodoc
     *  ======== start ========
     */
    @DirectCall
    Int startOrig(Ptr *ptr, UInt16 value);

    /*!
     *  ======== giveEnergy ========
     */
    @DirectCall
    Void giveEnergy();

    /*!
     *  ======== handleMsg ========
     */
    @DirectCall
    Void handleMsg(Ptr msg);

    /*!
     *  ======== prime ========
     */
    @DirectCall
    Void prime(Ptr handle, Int size, Int count);

    /*!
     *  ======== registerWithMaster ========
     */
    @DirectCall
    Void registerWithMaster();

    /*!
     *  ======== requestEvents ========
     */
    @DirectCall
    Void requestEvents();

    /*!
     *  ======== Module_State ========
     */
    struct Module_State {
        Event.Handle            event;
        Clock.Handle            clock;
        Ptr                     freeEventMQ;
        Ptr                     freeMsgMQ;
        Ptr                     routerMQ;
        Ptr                     startedMQ;
        UInt32                  masterMQ;
        Ptr                     incomingMsgMQ;
        UInt32                  replyMQ[];
        SyncEvent.Handle        syncEvent01;
        Task.Handle             transferAgentHandle;
        Ptr                     transportMsgHandle;
        Ptr                     transportEventHandle;
        Semaphore.Handle        releaseTasksSem;
        Int                     numMSGPacketsSent;
        Int                     numEventPacketsSent;
        Bool                    master;
        Bool                    masterRunning;
        UInt32                  period[];
        UInt32                  scheduled[];
        Bool                    reqEnergy[];

    };
}
