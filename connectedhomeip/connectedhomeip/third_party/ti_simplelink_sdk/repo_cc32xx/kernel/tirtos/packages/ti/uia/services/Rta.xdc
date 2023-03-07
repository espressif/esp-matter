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
 *  ======== Rta.xdc ========
 *  UIA RealTime Analysis module
 *
 *  Responsible for collecting Log events and sending them to
 *  an instrumentation host. Rta only services loggers that inherit from
 *  the {@link ti.uia.runtime.IUIATransfer} interface (e.g.
 *   {@link ti.uia.runtime.LoggerCircBuf}.
 *
 *  Rta sends records to the host from one logger instance at a
 *  time--it will finish one logger before moving on to the next one.
 *
 *  The Rta service runs in the framework of the UIA ServiceMgr framework.
 *  Configuration of the how the logs are actually moved off the target
 *  is managed by the {@link ti.uia.runtime.ServiceMgr} modules.
 *
 *  Before sending the records to the host, the agent will copy them into
 *  a transfer buffer. The size of this transfer buffer is configurable using
 *  the {@link ti.uia.runtime.ServiceMgr} module.
 *
 *  Because the application may generate logs faster than the agent can send
 *  them to the host, there is a limit to how many records the agent will send
 *  from one logger before moving on to the next one. The limit is
 *  approximately the size of the logger instance.
 *
 *  The rate at which the logs are sent up depends on two configuration
 *  parameters, {@link #period} and
 *  {@link ti.uia.runtime.ServiceMgr#transferAgentPriority}.
 *  The period does not guarantee that the collection will run
 *  at this rate. Even if the period has expired, the collection
 *  will not occur until the current running Task has yielded and there
 *  are no other higher priority Tasks ready.
 *
 *  The Rta module also responds to commands from the host to configure logging
 *  dynamically on the target, such as changing modules' diagnostic masks and
 *  enabling or disabling loggers.
 *
 *  To enable Rta, simply do an xdc.useModule in your configuration file.
 *  @p(code)
 *  var Rta = xdc.useModule('ti.uia.services.Rta');
 *  @p
 *  The ServiceMgr is brought in automatically be this statement. If the
 *  configuration of the ServiceMgr needs to be changed, the application must
 *  do an xdc.useModule on ti.uia.runtime.ServiceMgr and configured it as
 *  needed.
 */

import xdc.runtime.Diags;
import xdc.runtime.Log;
import ti.uia.runtime.ServiceMgr;
import ti.uia.runtime.UIAPacket;
import ti.uia.runtime.IUIATransfer;
import xdc.rov.ViewInfo;

module Rta
{
    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        Int         serviceId;
        Bool        enabled;
        Bool        snapshotMode;
        Int         period;
        UInt        numLoggers;
        String      loggers[];
        Bits16      sequence;
        UInt        totalPacketsSent;
    }

    /*!
     *  @_nodoc
     *  ======== rovViewInfo ========
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [['Module',   {type: ViewInfo.MODULE,
                                    viewInitFxn: 'viewInitModule',
                                    structName: 'ModuleView'}
                      ]]
        });

    /*! Logged on every packet is sent call from the Rta */
    config Log.Event LD_recordsSent = {
        mask: Diags.USER2,
        msg: "LD_recordsSent: Sent %d bytes from logger [%d] 0x%x"
    };

    /*! Logged when the Rta receives a command */
    config Log.Event LD_cmdRcvd = {
        mask: Diags.USER2,
        msg: "LD_cmdRcvd: Received command: %d, arg0: 0x%x, arg1: 0x%x"
    };

    /*! Logged when a diags mask is changed */
    config Log.Event LD_writeMask = {
        mask: Diags.USER2,
        msg: "LD_writeMask: Mask addres: 0x%x, New mask value: 0x%x"
    };

    /*!
     *  ======== periodInMs ========
     *  Period in miliseconds of the RTA Transfer Agent
     *
     *  Configures how often the RTA should collect events. The minimum
     *  value is 100ms.
     *
     *  This value does not guarantee that the collection will run
     *  at this rate. Even if the period has expired, the collection
     *  will not occur until the current running Task has yielded and there
     *  are no other higher priority Tasks ready.
     *
     *  Setting the period to 0, disables all collection of events. There
     *  must be a setPeriod message sent from an instrumentation host to
     *  Rta to enable it.
     *
     *  Default is 100 milliseconds.
     */
    config Int periodInMs = 100;

    /*! @_nodoc
     *  ======== processCallback ========
     *  Function registered with the ServiceMgr module
     */
    Void processCallback(ServiceMgr.Reason reason, UIAPacket.Hdr *cmd);

    /*! @_nodoc
     *  ======== registerLoggers ========
     *  Register all loggers instances with the Agent so that it can service
     *  them on the target.
     */
    metaonly function registerLoggers(modObj);

    /*!
     *  ======== disableAllLogs ========
     *  Function to disable all the logs being processed by Rta
     *
     *  Runtime function to disable all the logs that are being
     *  processed/read by Rta. When disabled, all new Log records
     *  are discarded.
     *
     *  Please realize that external instrumentation host (e.g.
     *  System Analyzer) might be sending down similar requests.
     */
    Void disableAllLogs();

    /*!
     *  ======== enableAllLogs ========
     *  Function to enable all the logs being processed by Rta
     *
     *  Runtime function to enable disable all the logs that are being
     *  processed/read by Rta.
     *
     *  Please realize that external instrumentation host (e.g.
     *  System Analyzer) might be sending down similar requests
     */
    Void enableAllLogs();

    /*!
     *  ======== snapshotAllLogs ========
     *  Function to delay processing of the Rta service
     *
     *  This function informs Rta to delay for the specified waitPeriod (in ms).
     *  After the waitPeriod has expired, Rta will process all the loggers
     *  that it manages. The state of Rta (e.g. started or stopped) will
     *  be maintained after the waitPeriod is expired and all the logs
     *  processed.
     *
     *  The reset flag determines whether to reset all the loggers at the
     *  start of the waitPeriod (true -> reset). The state of the loggers
     *  (e.g. enabled or disabled) is not changed by this flag.
     *
     *  @param(reset)   Flag to denote whether to reset the loggers or not.
     *                  TRUE means reset all the loggers processed by Rta.
     *                  FALSE means do not reset any of the loggers processed
     *                  by Rta.
     *
     *  @param(waitPeriod) Duration in milliseconds to wait to run the Rta
     *                     service.
     */
    Void snapshotAllLogs(UArg reset, UArg waitPeriod);

    /*!
     *  ======== resetAllLogs ========
     *  Function to resets enable all the logs being processed by Rta
     *
     *  Runtime function to enable resets all the logs that are being
     *  processed/read by Rta. All records in the logs are discarded.
     *  The state of the logger (e.g. enabled or disabled) is not changed.
     *
     *  Please realize that external instrumentation host (e.g.
     *  System Analyzer) might be sending down similar requests
     */
    Void resetAllLogs();

    /*!
     *  ======== startDataTx ========
     *  Function to start the Rta service
     *
     *  This function allows the Rta service to be turned on.
     *
     *  Please realize that external instrumentation host (e.g.
     *  System Analyzer) might be sending down similar requests
     */
    Void startDataTx();

    /*!
     *  ======== stopDataTx ========
     *  Function to stop the Rta service
     *
     *  This function allows the Rta service to be turned off.
     *
     *  Please realize that external instrumentation host (e.g.
     *  System Analyzer) might be sending down similar requests
     */
    Void stopDataTx();

internal:

    /*
     *  ======== SERVICEID ========
     *  Method to obtain serviceId
     */
    readonly config ServiceMgr.ServiceId SERVICEID;


    /*
     *  ======== sendEvents ========
     *  Send out events
     */
    Void sendEvents();

    /*
     *  ======== processMsg ========
     *  Process an incoming message
     */
    Void processMsg(UIAPacket.Hdr *cmd);

    /*
     *  ======== flushLogger ========
     *  Flushes a logger.
     * @param(logger) the handle of the logger to flush
     * @param(loggerNum) the logger's InstanceId
     */
    Void flushLogger(IUIATransfer.Handle logger, UInt loggerNum);

    /* Control APIs */
    Void acknowledgeCmd(Packet *resp);
    UIAPacket.MsgType readMask(Packet *resp, UArg addr);
    UIAPacket.MsgType writeMask(Packet *resp, UArg addr, UArg val);
    Void enableLog(UArg log);
    Void disableLog(UArg log);
    Void getCpuSpeed(Packet *resp);
    Void resetLog(UArg log);
    Void changePeriod(UArg period);

    /* Command ids */
    enum Command {
        Command_READ_MASK = 0,
        Command_WRITE_MASK = 1,
        Command_LOGGER_OFF = 2,
        Command_LOGGER_ON = 3,
        Command_GET_CPU_SPEED = 4,
        Command_RESET_LOGGER = 5,
        Command_CHANGE_PERIOD = 6,
        Command_START_TX = 7,
        Command_STOP_TX = 8,
        Command_LOGGER_OFF_ALL = 9,
        Command_LOGGER_ON_ALL = 10,
        Command_RESET_LOGGER_ALL = 11,
        Command_SNAPSHOT_ALL = 12
    };

    enum ErrorCode {
        ErrorCode_NULLPOINTER = 0
    };

    /* Structure of response packet sent back to host */
    struct Packet {
        UIAPacket.Hdr hdr;
        Bits32        arg0;
        Bits32        arg1;
    }

    struct Module_State {
        IUIATransfer.Handle  loggers[];
        UInt                 numLoggers;
        UInt                 totalPacketsSent;
        Int                  period;
        Bits16               seq;
        Bool                 txData;
        Bool                 snapshot;
    };
}
