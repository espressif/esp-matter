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
 *  ======== Adaptor.xdc ========
 */

package ti.uia.sysbios;

import xdc.runtime.Assert;
import xdc.rov.ViewInfo;
import ti.uia.runtime.UIAPacket;
import ti.sysbios.knl.Clock;
import ti.sysbios.knl.Task;
import ti.sysbios.knl.Event;
import ti.sysbios.knl.Semaphore;
import ti.sysbios.knl.Queue;
import ti.sysbios.syncs.SyncEvent;

/*!
 *  ======== Adaptor ========
 *  Single core IServiceMgrSupport implementation
 *
 *  This module implements the IServiceMgrSupport interface
 *  and is used by the ServiceMgr on single cores. This module
 *  routes UIA data between the instrumentation host and the services
 *  via the {@link Transport} implementations. It maintains lists of
 *  free events and contrl message buffers via BIOS Semaphores and Queues.
 *
 *  There is a Transfer Agent task (transferAgentTaskFxn). This task
 *  sends data to the instrumentation host. This task uses the
 *  {@link ServiceMgr#transportFxns} functions communicate to the host. These functions
 *  are setup by default based on the device.
 *
 *  If {@link ti.uia.runtime.ServiceMgr#supportControl} is true, this module
 *  also creates the Receive Task (rxTaskFxn). The Receive Task receives
 *  control messages from the instrumentation host via the
 *  {@link ServiceMgr#transportFxns} functions. This task is not needed if no control
 *  messages are coming from the host.
 *
 *  The majority of the configuration parameters are defined in the ServiceMgr.
 *  For example, the {@link ti.uia.runtime.ServiceMgr#transferAgentPriority}
 *  dictates priority of the Adaptor's Transfer Agent task. Here is the list of
 *  the ServiceMgr parameter used by the Adaptor module:
 *  @p(blist)
 *  - numEventPacketBufs
 *  - maxEventPacketSize
 *  - transferAgentStackSection
 *  - transferAgentStackSize
 *  - transferAgentPriority
 *  - supportControl
 *  - maxCtrlPacketSize
 *  - numIncomingCtrlPacketBufs
 *  - numOutgoingCtrlPacketBufs
 *  - rxTaskStackSize
 *  - rxTaskStackSection
 *  - rxTaskPriority
 *  @p
 */

@ModuleStartup      /* Initialize static instances */

module Adaptor inherits ti.uia.runtime.IServiceMgrSupport
{

    /*!
     *  ======== packetSection ========
     *  Memory section for UIA packets.
     *
     *  If this parameter is not set then the
     *  ServiceMgr.transferAgentStackSection parameter is used for the events
     *  and the ServiceMgr.rxTaskStackSection parameter is used for the control
     *  messages.
     */
    metaonly config String packetSection;

    /*!
     *  @_nodoc
     *  ======== rxTaskFxn ========
     *  Function used for the Receive Task.
     */
    @DirectCall
    Void rxTaskFxn(UArg arg0, UArg arg1);

    /*!
     *  @_nodoc
     *  ======== transferAgentFxn ========
     *  Function used for the transfer agent Task.
     */
    @DirectCall
    Void transferAgentTaskFxn(UArg arg0, UArg arg1);

    /*!
     *  @_nodoc
     *  ======== clockFxn ========
     *  Function used to drive transfer agent's event polling
     */
    @DirectCall
    Void clockFxn(UArg arg0);

internal:

    /*!
     *  ======== Entry ========
     *  Structure used to place packets on a free queues
     */
    struct Entry {
        Queue.Elem     elem;
        UIAPacket.Hdr  packet;
    }

    /*!
     *  ======== eventBuf ========
     *  Buffer of event packets
     */
    config Char eventBuf[];

    /*!
     *  ======== msgBuf ========
     *  Buffer of control message packets
     */
    config Char msgBuf[];

    /*!
     *  ======== giveEnergy ========
     *  Internal helper function
     */
    @DirectCall
    Void giveEnergy();

    /*!
     *  ======== sendToHost ========
     *  Internal helper function
     */
    @DirectCall
    Bool sendToHost(UIAPacket.Hdr *packet);

    /*!
     *  ======== sendToService ========
     *  Internal helper function
     */
    @DirectCall
    Void sendToService(Entry *entry);

    /*!
     *  ======== runScheduledServices ========
     *  Internal helper function
     */
    @DirectCall
    Void runScheduledServices();

    /*!
     *  ======== Module_State ========
     *  The four queues are used as following:
     *  freeEventQ: holds "free" event packets
     *  freeMsgQ:   holds "free" control message packets (both
     *              incoming and outgoing ones)
     *  incomingQ:  holds all filled-in incoming messages
     *  outgoingQ:  holds all filled-in outgoing messages
     */
    struct Module_State {
        Event.Handle            event;
        Clock.Handle            clock;
        Semaphore.Handle        freeEventSem;
        Semaphore.Handle        freeMsgSem;
        Semaphore.Handle        incomingSem;
        Queue.Handle            freeEventQ;
        Queue.Handle            freeMsgQ;
        Queue.Handle            incomingQ;
        Queue.Handle            outgoingQ;
        SyncEvent.Handle        syncEvent01;
        Task.Handle             transferAgentHandle;
        Ptr                     transportMsgHandle;
        Ptr                     transportEventHandle;
        Int                     numMsgPacketsSent;
        Int                     numMsgPacketsFailed;
        Int                     numEventPacketsSent;
        Int                     numEventPacketsFailed;
        UInt32                  period[];
        UInt32                  scheduled[];
        Bool                    reqEnergy[];
    };
}
