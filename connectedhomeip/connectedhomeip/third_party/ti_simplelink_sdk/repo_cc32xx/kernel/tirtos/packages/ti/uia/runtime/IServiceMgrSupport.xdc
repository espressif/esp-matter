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
 * */

/*!
 *  ======== IServiceMgrSupport ========
 *  Interface defining the ServiceMgr's proxy
 *
 *  This module defines the interface that the ServiceMgr
 *  support proxy must adhere to.
 */
interface IServiceMgrSupport  {

    /*!
     *  ======== newService ========
     *  Function called within ServiceMgr_register
     *
     *  All services must call ServiceMgr.register. The ServiceMgr
     *  manages the processFxn callback. It then calls the proxy's
     *  newService function.
     *
     *  @param(id)         Id of the new service
     *  @param(periodInMs) Period requested by the service (in millisecond)
     */
    metaonly Void newService(Int id, UInt32 periodInMs);

    /*!
     *  ======== freePacket ========
     *  Function called within ServiceMgr_freePacket
     *
     *  This function can be used to return an unused packet back to the
     *  module. It must only return packets that were obtained via
     *  the {@link #getFreePacket} function.
     *
     *  @param(packet)  Pointer to a UIAPacket
     */
    @DirectCall
    Void freePacket(UIAPacket.Hdr *packet);

    /*!
     *  ======== getFreePacket ========
     *  Function called within ServiceMgr_getFreePacket
     *
     *  The service can specify what type of packet it wants with the
     *  first parameter.
     *
     *  The function fills in the HdrType field of the packet automatically
     *  for the service. All other fields are un-initialized.
     *
     *  @param(type)    Requested type of packet
     *  @param(timeout) return after this many system time units
     *
     *  @b(returns)     Point to the free UIA packet. NULL if not successful.
     */
    @DirectCall
    UIAPacket.Hdr *getFreePacket(UIAPacket.HdrType type, UInt timeout);

    /*!
     *  ======== requestEnergy ========
     *  Function called within ServiceMgr_requestEnergy
     *
     *  Generally services do not maintain an active thread. Services may
     *  request the ServiceMgr module to call the {@link #ProcessCallback}
     *  in the context of the transfer agent. This can be accomplished via
     *  this function.
     *
     *  @param(id)     Id of the service
     */
     @DirectCall
     Void requestEnergy(Int id);

    /*!
     *  ======== sendPacket ========
     *  Function called within ServiceMgr_sendPacket
     *
     *  All UIAPacket fields except for SenderAdrs must be filled in.
     *
     *  The caller loses ownership of the packet once it is successfully sent.
     *  If this function fails, the caller still owns the packet.
     *
     *  @param(packet)  UIAPacket to be sent
     *
     *  @b(returns)     TRUE denotes success and the packet is
     *                  no longer owned by the caller. FALSE denotes
     *                  failure and the packet is still owned by the caller.
     */
    @DirectCall
    Bool sendPacket(UIAPacket.Hdr *packet);

    /*!
     *  ======== setPeriod ========
     *  Function called within ServiceMgr_setPeriod
     *
     *  Services period should be a multiple of the ServiceMgr's period
     *  ({@link #periodInMs}). If it is not, they will called at the rounded
     *  up period. For example, if ServiceMgr.periodInMs = 100 and a service sets
     *  its period to 250. That service will be called every 300 milliseconds.
     *
     *  @param(id)         Service id of the service
     *
     *  @param(periodInMs) Requested period in milliseconds
     */
     @DirectCall
     Void setPeriod(Int id, UInt32 periodInMs);
}
