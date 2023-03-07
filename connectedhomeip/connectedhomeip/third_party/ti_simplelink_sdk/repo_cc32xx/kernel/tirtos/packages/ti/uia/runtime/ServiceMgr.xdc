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

/*
 *  ======== ServiceMgr.xdc ========
 */

import xdc.runtime.Assert;
import xdc.rov.ViewInfo;

/*!
 *  ======== ServiceMgr ========
 *  Module that manages UIA Services
 *
 *  The ServiceMgr module manages all the UIA Services. The ServiceMgr module
 *  itself can be plugged with different low level transport functions to
 *  support different methods for getting information between the target
 *  and instrumentation host. Some of these methods (e.g. File) only support
 *  sending information from the target to the host. They do not support
 *  sending information from the host to the target.
 *
 *  The ServiceMgr module also supports two different topologies.
 *  @p(blist)
 *      - Topology_SINGLECORE: Each core on the device communicates
 *        directly the instrumentation host.
 *      - Topology_MULTICORE: Only one core (i.e. master)
 *        communicates with the instrumentation host. Communication
 *        with other cores are routed to the via the master core.
 *        The master core is determined by the {@link #masterProcId}
 *        parameter. The routing is done via Ipc's MessageQ module.
 *        Note: ServiceMgr is a user of Ipc and depends on the
 *        application to configure and initialize Ipc.
 *  @p
 *
 *  There are two different uses of the ServiceMgr module:
 *  @p(blist)
 *      - Applications (configuration only)
 *      - Services
 *  @p
 *
 *  @p(html)
 *  <B>Applications</B>
 *  @p
 *  The ServiceMgr module contains the generic configuration.
 *  Many of these configuration values change
 *  depending on the device. The ServiceMgr module uses the
 *  ti.uia.family.Settings module to fill in values if they are not set by
 *  the application.
 *
 *  The ServiceMgr does expose many configuration parameters to allow users
 *  to modify the default behavior.
 *
 *  @p(blist)
 *      - Topology: UIA is shipped with support for two different topologies:
 *  @p(blist)
 *          - Topology_SINGLECORE: Each core on the device communicates
 *               directly the instrumentation host.
 *          - Topology_MULTICORE: Only one core (i.e. master)
 *               communicates with the instrumentation host. Communication
 *               with other cores are routed to the via the master core.
 *               The master core is determined by the {@link #masterProcId}
 *               parameter. The routing is done via Ipc's MessageQ module.
 *               Note: ServiceMgr is a user of Ipc and depends on the
 *               application to configure and initialize Ipc.
 *  @p
 *
 *      - Transport: The ServiceMgr has  pluggable transport functions. The
 *               determination of the transport is accomplished via the
 *               {@link #transportType} parameter. The parameter is defaulted
 *               based on the device is not explicitly set.
 *
 *      - Packets: UIA makes a distinction between event and messages packets.
 *                 Events packets are generally bigger to hold many event
 *                 records. The size and number of these packets are defaulted
 *                 by UIA based on the transport and device. However the number
 *                 of event packets might need to be increased on a multicore
 *                 device with heavy event record load.
 *                 {@link #numEventPacketBufs} can be increased in this case.
 *
 *      - Tasks: The ServiceMgr module might have up to two tasks:
 *               transfer agent task and receive task. The receive task only
 *               recieves control messages from the instrumentation host and
 *               forwards it on the the transfer agent task.
 *               The transfer agent task is responsible for everything else
 *               (e.g. period management, event collection, communicating with
 *               remote cores in a multicore configuration, sending UIA Packets
 *               to the instrumentation host, etc.).
 *
 *               The application can specify the stack sizes and placement of
 *               these task via the ServiceMgr.
 *
 *               If there is only one task needed, the parameters for the
 *               receive task is ignored.
 *  @p
 *
 *  @p(html)
 *  <B>Services</B>
 *  @p
 *  All services must register with the ServiceMgr module via the
 *  {@link #register} function. Each service
 *  must have a unique service id (the first 3 are reserved). The service id
 *  is obtained via the encoded type ServiceIdDesc.
 *
 *  From the service's standpoint, the ServiceMgr module manages all the
 *  buffers for sending and receive messages and events. The services need
 *  to obtain packets from the ServiceMgr via the {@link #getFreePacket}
 *  function. To send a packet, the service must call the
 *  {@link #sendPacket} function.
 *
 *  All data send and received by a service must be contained in a
 *  {@link UIAPacket} header. Refer to this module for more details.
 *
 *  From the service's standpoint, the ServiceMgr module also manages the
 *  the period of service. Each service can request the interval at which
 *  the ServiceMgr calls the service's {@link #ProcessCallback} to send out
 *  events. This interval can be set by the service via the {@link #register}
 *  function. If during runtime the service wants to change it period, it
 *  can call {@link #setPeriod}.
 */
@CustomHeader
module ServiceMgr
{
    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        Int        periodInMs;
        Bool       supportControl;
        String     topology;
        Int        numServices;
        Int        masterProcId;
        Int        runCount;
        //string     masterRunning; //TODO SDOCM00077324
    }

    /*!
     *  @_nodoc
     *  ======== PacketView ========
     */
    metaonly struct PacketView {
        SizeT      maxEventPacketSize;
        Int        numEventPacketBufs;
        SizeT      maxCtrlPacketSize;
        Int        numOutgoingCtrlPacketBufs;
        Int        numIncomingCtrlPacketBufs;
    }

    /*!
     *  @_nodoc
     *  ======== TransportView ========
     */
    metaonly struct TransportView {
        String     initFxn;
        String     startFxn;
        String     recvFxn;
        String     sendFxn;
        String     stopFxn;
        String     exitFxn;
    }

    /*!
     *  @_nodoc
     *  ======== StatisticsView ========
     */
    metaonly struct StatisticsView {
        Int     numEventPacketsSent;
        Int     numEventPacketsFailed;
        Int     numMsgPacketsSent;
        Int     numMsgPacketsFailed;
    }

    /*!
     *  @_nodoc
     *  ======== rovViewInfo ========
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                ['Module',
                    {
                        type: ViewInfo.MODULE,
                        viewInitFxn: 'viewInitModule',
                        structName: 'ModuleView'
                    }
                ],
                // SDOCM00077324
                // ServiceMgr's ROV should obtain information from proxies
                //['Statistics',
                //    {
                //        type: ViewInfo.MODULE,
                //        viewInitFxn: 'viewInitStats',
                //        structName: 'StatisticsView'
                //    }
                //],
                ['Transport',
                    {
                        type: ViewInfo.MODULE,
                        viewInitFxn: 'viewInitTransport',
                        structName: 'TransportView'
                    }
                ],
                ['Packet',
                    {
                        type: ViewInfo.MODULE,
                        viewInitFxn: 'viewInitPacket',
                        structName: 'PacketView'
                    }
                ],
            ]
        });

    /*!
     *  ======== Reason ========
     *  Used in the ProcessCallback to denote the reason
     *
     *  ServiceMgr_Reason_PERIODEXPIRED: the {@link #ProcessCallback} is being
     *  called because it is time to collect events and send them.
     *
     *  ServiceMgr_Reason_REQUESTENERGY: the {@link #ProcessCallback} is being
     *  called because the service requested energy to perform some action.
     *
     *  ServiceMgr_Reason_INCOMINGMSG: the {@link #ProcessCallback} is being
     *  called because there is an incoming message for the service.
     */
    enum Reason {
        Reason_PERIODEXPIRED,
        Reason_REQUESTENERGY,
        Reason_INCOMINGMSG
    };

    /*!
     *  ======== Topology ========
     *  Used to define UIA topology.
     *
     *  If UIA is configured for multicore, all events and messages are routed
     *  via the master core to the non-master cores. Choosing multicore requires
     *  setting the {@link #master} config.
     *
     *  If UIA is configured for Topology_SINGLECORE, each core on the device
     *  communicates with the instrumentation host directly.
     */
    enum Topology {
        Topology_SINGLECORE,
        Topology_MULTICORE
    };

    /*!
     *  ======== TransportType ========
     *  Used to specify the type of transport for UIA to use.
     *
     *  TransportType defines what the underlying transport will be used. In
     *  a multi-core topology, this parameter is only relevant for the
     *  {@link #masterProcId} core. The masterProcId core uses the TransportType
     *  to determine how to get the data off the target.
     *
     *  @p(blist)
     *  -TransportType_ETHERNET: Use the Ethernet transport. On a SYS/BIOS
     *      system, this uses the NDK. The application is responsible for
     *      adding the NDK into the application and initializing it.
     *  -TransportType_FILE: Use the File transport.
     *      On a SYS/BIOS system, this transport requires a JTAG connection.
     *  -TransportType_USER: This allows the application to specify their
     *      own transport functions.
     *  -TransportType_NULL: No transport functions are needed. This should
     *      only be used on the non-masterProcId cores.
     *  @p
     */
    metaonly enum TransportType {
        TransportType_ETHERNET,
        TransportType_FILE,
        TransportType_USER,
        TransportType_NULL
    };

    /*!
     *  ======== ProcessCallback ========
     *  Function prototype for the processMsg callback
     *
     *  A ProcessCallback function must be supplied by every service.
     *  The function is provided in the {@link #register} function.
     *
     *  For a description of the ServiceMgr_Reason parameter, please refer
     *  to {@link #Reason}.
     *
     *  The UIPacket_Hdr field is only used in the {@link #Reason_INCOMINGMSG}
     *  case. For any other reason, this value is NULL.
     */
    typedef Void (*ProcessCallback)(Reason, UIAPacket.Hdr *);

    /*!
     *  ======== WAIT_FOREVER ========
     *  Wait forever constant that can be used in ServiceMgr_getFreePacket.
     */
    const UInt WAIT_FOREVER = ~(0);

    /*! @_nodoc */
    metaonly struct ServiceIdDesc { Bits16 val; };

    /*!
     *  ======== ServiceId ========
     *  Used by services to generate a serviceId
     *
     *  Services needs to define a ServiceId in their xdc file.
     *  Then the ServiceMgr module will assign a unique value to it during
     *  build time.
     *
     *  For example in ti.uia.service.Rta.xdc there is the following line:
     *  @p(code)
     *  readonly config ServiceMgr.ServiceId SERVICEID;
     *  @p
     */
    @Encoded typedef ServiceIdDesc ServiceId;     /*! Control command type */

    /*!
     *  Assert raised when calling API with invalid ServiceId
     */
    config Assert.Id A_invalidServiceId  = {
        msg: "A_invalidServiceId: ServiceId out of range"
    };

    /*!
     *  Assert raised invalid processCallbackFxn is supplied
     */
    config Assert.Id A_invalidProcessCallbackFxn  = {
        msg: "A_invalidProcessCallbackFxn: Callback cannot be NULL"
    };

    /*!
     *  ======== customTransportType ========
     *  Custom transport used to send the records to an instrumentation host
     */
    metaonly config String customTransportType = null;

    /*!
     *  ======== transportFxns ========
     *  Transport functions used to communicate to the instrumentation host
     *
     *  These functions are setup by default based on the device.
     *  The user can explicitly set this parameter if the default is not
     *  appropriate.
     */
    config Transport.FxnSet transportFxns;

    /*!
     *  ======== topology ========
     *  Used to define UIA topology.
     *
     *  If `Topology_MULTICORE` is chosen, the ServiceMgr will use Ipc to
     *  discover the core configuration and to communicate between the cores.
     *  UIA will route the outbound packets from each core through the master
     *  core. UIA will also route messages received by the master core to
     *  their intended recipient.
     *
     *  If UIA is configured for Topology_SINGLECORE, each core on the device
     *  communicates with the instrumentation host directly.
     *
     *  The default is Topology_SINGLECORE.
     */
    config Topology topology = Topology_SINGLECORE;

    /*!
     *  ======== transportType ========
     *  Determines the transport that UIA will be configured for.
     *
     *  For a given transport type, UIA picks an appropriate transport
     *  implementation to use based on your device. This is specified by the
     *  ti.uia.family.Settings module. Refer to the examples for configuring
     *  the actual transport implementation.
     *
     *  If someone writes an new transport (e.g. RapidIO),
     *  they can be plugged in by setting the TransportType
     *  to `TransportType_USER` and then plugging
     *  the transportFxns manually. It must also set up the following parameters
     *  as directed by the new transport developer.
     *  @p(blist)
     *  -ServiceMgr.supportControl: does the transport support receiving
     *               messages from the host. For example TransportFile does not.
     *  -ServiceMgr.maxEventPacketSize: Max size of an outgoing event packet. For
     *               example TransportNdk uses 1472 (emac size minus headers)
     *  -ServiceMgr.maxCtrlPacketSize: Max size of the message packets. This can
     *               be zero if supportControl is false.
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
     *      exitFxn: '&TransportXYZ_exit'
     *  };
     *  ServiceMgr.transportFxns = xyzTransport;
     *
     *  ServiceMgr.maxEventPacketSize = 1024
     *  ServiceMgr.maxCtrlPacketSize  = 1024;
     *  ServiceMgr.supportControl     = true;
     *  @p
     */
    metaonly config TransportType transportType;

    /*!
     *  ======== periodInMs ========
     *  Period in miliseconds of ServiceMgr's transfer agent
     *
     *  The transfer agent runs at the configured period. It checks to see
     *  if a service's period has expired. If it has expired, the service's
     *  {@link #ProcessCallback} is called with the
     *  {@link #Reason_PERIODEXPIRED} reason.
     *
     *  A service should not set it's period to a value less than the ServiceMgr
     *  module'speriod. A service's period should be a multiple of the
     *  ServiceMgr module's period. If it is not, it will called at the rounded
     *  up period. For example, if ServiceMgr.periodInMs = 100 and a service
     *  sets its period to 250. That service will be called every 300
     *  milliseconds.
     *
     *  This value does not guarantee that the transfer agent will run at this
     *  rate. Even if the period has expired, the transfer agent will not run
     *  until the current running Task has yielded and there are no other higher
     *  priority Tasks ready.
     *
     *  Default is 100ms.
     */
    config Int periodInMs = 100;

    /*!
     *  ======== maxEventPacketSize  ========
     *  Size of Event packets in bytes
     *
     *  This size includes the UIAPacket header. This value's default
     *  depends on the device.
     */
    config SizeT maxEventPacketSize;

    /*!
     *  ======== numEventPacketBufs  ========
     *  Number of UIAPacket events on the processor
     */
    config Int numEventPacketBufs  = 2;

    /*!
     *  ======== maxCtrlPacketSize  ========
     *  Size of control message packets in bytes
     *
     *  This size includes the UIAPacket header. This value's default
     *  depends on the device.
     */
    config SizeT maxCtrlPacketSize;

    /*!
     *  ======== numOutgoingCtrlPacketBufs  ========
     *  Number of outgoing Ctrl buffers on the processor
     */
    config Int numOutgoingCtrlPacketBufs  = 2;

    /*!
     *  ======== numIncomingCtrlPacketBufs  ========
     *  Number of incoming Ctrl buffers on the master processor
     */
    config Int numIncomingCtrlPacketBufs  = 2;

    /*!
     *  ======== supportControl ========
     *  Configure whether control messages are supported.
     *
     *  Default is determined based on the device and transport type.
     *
     *  The application should only set this if {@link #transportType}
     *  is TransportType_USER and it is plugging in a new set
     *  of transport functions. The transport function package should
     *  specify how to set this parameter.
     */
    config Bool supportControl;

    /*!
     *  ======== transferAgentPriority ========
     *  Priority of the Transfer Agent Task.
     *
     *  Default is 1, the lowest priority.
     */
    config Int transferAgentPriority = 1;

    /*!
     *  ======== transferAgentStackSize ========
     *  Transfer Agent Task stack size in MAUs.
     *
     *  The recommended size is 2048 bytes.
     */
    config SizeT transferAgentStackSize = 2048;

    /*!
     *  ======== transferAgentStackSection ========
     *  Memory section for Transfer Agent Task's stack.
     *
     *  If this parameter is not set then the Task.defaultStackSection is used.
     *  See the Task module for instructions on creating a stack section in
     *  a different memory segment.
     */
    metaonly config String transferAgentStackSection = null;

    /*!
     *  ======== rxTaskPriority ========
     *  Priority of the Transfer Agent Task.
     *
     *  Default is 1, the lowest priority.
     */
    config Int rxTaskPriority = 1;

    /*!
     *  ======== rxTaskStackSize ========
     *  Transfer Agent Task stack size in MAUs.
     *
     *  The recommended size is 2048 bytes.
     */
    config SizeT rxTaskStackSize = 2048;

    /*!
     *  ======== rxTaskStackSection ========
     *  Memory section for Receiving Task's stack.
     *
     *  If this parameter is not set then the Task.defaultStackSection is used.
     *  See the Task module for instructions on creating a stack section in
     *  a different memory segment.
     */
    metaonly config String rxTaskStackSection = null;

    /*!
     *  @_nodoc
     *  ======== SupportProxy ========
     *  The implementation module of the low-level ServiceMgr functions
     */
    proxy SupportProxy inherits IServiceMgrSupport;

    /*!
     *  ======== masterProcId ========
     *  Processor that communicates to the instrumentation host
     *
     *  This value denotes which core in a multiple core topology is
     *  the master core. All routing of UIA data to the instrumentation
     *  host is done via this core.
     *
     *  The procId corresponds to Ipc's MultiProc value.
     *
     *  For single processor systems, or where there is no routing of
     *  data via an intermediate core, this value is ignored.
     */
    config UInt16 masterProcId = 0;

    /*!
     *  ======== freePacket ========
     *  Function to return an unused packet back to the ServiceMgr module
     *
     *  This function can be used to return an unused packet back to the
     *  ServiceMgr module. It must only return packets that were obtained via
     *  the {@link #getFreePacket} function.
     *
     *  @param(packet)  Pointer to a UIAPacket
     */
    Void freePacket(UIAPacket.Hdr *packet);

    /*!
     *  ======== getFreePacket ========
     *  Function to obtain a free UIA packet
     *
     *  The service can specify what type of packet it wants with the
     *  first parameter. Currently only UIAPacket_HdrType_Msg and
     *  UIAPacket_HdrType_EventPkt are supported.
     *
     *  The function fills in the HdrType field of the packet automatically
     *  for the service. All other fields are un-initialized.
     *
     *  @param(type)    Requested type of packet
     *  @param(timeout) Return after this many system time units
     *
     *  @b(returns)     Pointer to a packet if successful. NULL if timeout.
     */
    UIAPacket.Hdr *getFreePacket(UIAPacket.HdrType type, UInt timeout);

    /*!
     *  ======== getNumServices ========
     *  Returns the number of services present in the system
     *
     *  @b(returns)     Number of services
     */
    Int getNumServices();

    /*!
     *  @_nodoc
     *  ======== processCallback ========
     *  Callback function called by the support proxy
     *
     *  This function is called by the support proxy when it needs to
     *  call a service's ProcessCallback function.
     *
     *  This function should not be called by a service.
     *
     *  @param(id)      Service id of the service
     *  @param(reason)  Reason for calling the service's ProcessCallback
     *                  function
     *  @param(packet)  If the reason is {@link #Reason_INCOMINGMSG}, this
     *                  parameter points to the incoming msg. Otherwise it is
     *                  NULL. The service does not own this packet. It should
     *                  NOT re-use it. Internally the ServiceMgr module will
     *                  return it to an internal queue after the
     *                  processCallback returns.
     */
    Void processCallback(ServiceId id, Reason reason, UIAPacket.Hdr *packet);

    /*!
     *  ======== register ========
     *  Register a services with the ServiceMgr module
     *
     *  All service's must register with the ServiceMgr module statically.
     *
     *  Refer to {@link #periodInMs} for a description of the period parameter.
     *
     *  @param(id)      Service id of the service (refer to
     *                  {@link #ServiceId}).
     *
     *  @param(processCallbackFxn)  Service's callback function.
     *
     *  @param(periodInMs) Period of the service.
     */
    metaonly Int register(ServiceId id, ProcessCallback processCallbackFxn,
                          UInt32 periodInMs);

    /*!
     *  ======== requestEnergy ========
     *  Function to request energy for a service
     *
     *  Generally services do not maintain an active thread.
     *  They may request the ServiceMgr module to call the
     *  {@link #ProcessCallback} in the context of the transfer agent.
     *  This can be accomplished via this function.
     *
     *  @param(id)     Service id of the service
     */
    Void requestEnergy(ServiceId id);

    /*!
     *  ======== sendPacket ========
     *  Send a UIAPacket to the instrumentation host
     *
     *  All UIAPacket fields except for SenderAdrs must be filled in.
     *
     *  The caller loses ownership of the packet once it is successfully sent.
     *  If this function fails, the caller still owns the packet. It can re-use
     *  it or free it via the {@link #freePacket} function.
     *
     *  @param(packet)  UIAPacket to be sent
     *
     *  @b(returns)     TRUE denotes success and the packet is
     *                  no longer owned by the caller. FALSE denotes
     *                  failure and the packet is still owned by the caller.
     */
    Bool sendPacket(UIAPacket.Hdr *packet);

    /*!
     *  ======== setPeriod ========
     *  Allows services to set their event collection period
     *
     *  ServiceMgr's period should be a multiple of the ServiceMgr's period
     *  ({@link #periodInMs}). If it is not, they will called at the rounded
     *  up period. For example, if ServiceMgr.periodInMs = 100 and a service sets
     *  its period to 250. That service will be called every 300 milliseconds.
     *
     *  @param(id)         Service id of the service
     *
     *  @param(periodInMs) Requested period in milliseconds
     */
    Void setPeriod(ServiceId id, UInt32 periodInMs);

internal:

    /*!
     *  ======== ServiceMgr Information ========
     *  The following arrays contain information about each service
     */
    config ProcessCallback processCallbackFxn[];

    struct Module_State {
        Int runCount;
        Int numServices;
    };
}
