/*
 * Copyright (c) 2012-2014, Texas Instruments Incorporated
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
 *  ======== Transport.xdc ========
 */

package ti.uia.runtime;

/*!
 *  @_nodoc
 *  ======== Transport ========
 *  Transport function specification module
 *
 *  This module defines the function prototypes for the transport functions
 *  that can be plugged into the ServiceMgr. UIA ships several
 *  implementations of this interface in the ti/uia/sysbios directory.
 *
 *  The implementations do not have to be XDC modules. They are simply standard
 *  'C' functions (i.e look at ti/uia/sysbios/TransportNdk.c). Only one
 *  transport set can be used on a target and it needs to be set up at build
 *  time via the {@link ti.uia.runtime.ServiceMgr#transportFxns} parameter. The
 *  ServiceMgr plugs the transportFxns automatically based on the TransportType
 *  {@link ti.uia.runtime.ServiceMgr#transportType}.
 *
 *  If someone writes an new transport
 *  (e.g. RapidIO), they can be plugged in by setting the TransportType
 *  to {@link ti.uia.runtime.ServiceMgr#TransportType_USER} and then plugging
 *  the transportFxns manually. It must also set up the following parameters
 *  as directed by the new transport developer.
 *  @p(blist)
 *  -ServiceMgr.supportControl: does the transport support receiving messages
 *                 from the host. For example TransportFile does not.
 *  -ServiceMgr.maxEventPacketSize: Max size of an outgoing event packet. For
 *                 example TransportNdk uses 1472 (emac size minus headers)
 *  -ServiceMgr.maxCtrlPacketSize: Max size of the message packets. This can
 *                 be zero if supportControl is false.
 *  @p
 *
 *  Here is an example of plugging the transport XYZ into the ServiceMgr:
 *  @p(code)
 *  var ServiceMgr = xdc.useModule('ti.uia.runtime.ServiceMgr');
 *  ServiceMgr.transportType = ServiceMgr.TransportType_USER;
 *  var xyzTransport = {
 *      initFxn: '&TransportXYZ_init',
 *      startFxn: '&TransportXYZ_start',
 *      recvFxn: '&TransportXYZ_recv',
 *      sendFxn: '&TransportXYZ_send',
 *      stopFxn: '&TransportXYZ_stop',
 *      exitFxn: '&TransportXYZ_exit',
 *  };
 *  ServiceMgr.transportFxns = xyzTransport;
 *
 *  ServiceMgr.maxEventPacketSize = 1024
 *  ServiceMgr.maxCtrlPacketSize  = 1024;
 *  ServiceMgr.supportControl     = true;
 *  @p
 *
 *  @p(html)
 *  <a name="transportfxn"></a>
 *
 *  @a(Transport Functions)
 *
 *  The following are the transport functions. Note
 *  all of these functions
 *  are called by the ServiceMgr. The application should not be calling
 *  these functions directly.
 *  @p(blist)
 *  -initFxn:  Called during module startup (which is before main()). Minimal
 *             actions can take place here since there are no interrupts and
 *             the state of the application is just starting up. Generally only
 *             internal initialization is done in this function.
 *
 *  -startFxn: The start function is called at once or twice after the SYS/BIOS
 *             tasks have started to run. The start
 *             function is called with UIAPacket_HdrType_EventPkt before any
 *             events are sent. This allows the transport to initialize
 *             anything needed for event transmission. The function returns a
 *             handle to a transport specific structure (or NULL if not needed).
 *             This handle is passed into the sendFxn and stopFxn.
 *
 *             If the transport supports control messages from a host, the
 *             start function is called with UIAPacket_HdrType_Msg.
 *             This allows the transport to initialize anything needed for
 *             msg transmission (both sending and receiving). Again, the
 *             transport can return a transport specific structure. This
 *             structure can be different from the one returned in the
 *             UIAPacket_HdrType_EventPkt start.
 *
 *  -recvFxn:  The recv function is called to receive incoming messages
 *             from the host. The handle returned from the start is passed
 *             into the recv. Also passed in is a buffer and its size.
 *             The buffer is passed in as a double pointer. This allows
 *             the transport to double-buffer. For example, the recv
 *             function can return a different buffer than what was
 *             passed in. This potentially reduces extra copies of the data.
 *
 *             The recv can be a blocking call.
 *
 *             The recv returns the actual number of bytes that are placed into
 *             the buffer. If the transport does not
 *             support control messages, this function can simply return zero.
 *
 *  -sendFxn:  The send function is called to send either events or msgs. If
 *             send is called to transmit a event, the first parameter is the
 *             handle returned from the start(UIAPacket_HdrType_EventPkt).
 *             Similiarily, if a message is being sent, the first parameter is
 *             the handle returned from the start(UIAPacket_HdrType_Msg).
 *             The size of the packet is maintained in the UIAPacket_Hdr.
 *
 *             The send can be a blocking call.
 *
 *             This function returns whether the send was successful or not.
 *
 *             Again a double pointer is used to allow the transport to return
 *             a different buffer to allow double-buffering.
 *
 *  -stopFxn:  The stop function is to counterpart to the start function. The
 *             stop will be called the same number of times as the start. The
 *             calls will contain handles returned from the start.
 *
 *  -exitFxn:  The exit function is to counterpart to the init function.
 *  @p
 *
 *  Transport are allowed to have additional functions that can be directly
 *  called by the application. For example in ti/uia/sysbiosTransportFile,
 *  there is a TransportFile_setFile function. The downside to the extended
 *  functions is portability.
 */
module Transport
{
    /*!
     *  Task hook set type definition.
     *
     *  See {@link #transportfxn Transport Functions} for details.
     */
    struct FxnSet {
        Void  (*initFxn)();
        Ptr   (*startFxn)(UIAPacket.HdrType);
        SizeT (*recvFxn)(Ptr, UIAPacket.Hdr **, SizeT);
        Bool  (*sendFxn)(Ptr, UIAPacket.Hdr **);
        Void  (*stopFxn)(Ptr);
        Void  (*exitFxn)(Void);
    };
}
