/*
 * Copyright (c) 2012-2019, Texas Instruments Incorporated
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
 *  ======== Rta.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Error.h>

#include <ti/uia/runtime/IUIATransfer.h>
#include <ti/uia/runtime/UIAPacket.h>
#include <ti/uia/runtime/ServiceMgr.h>
#include <ti/uia/runtime/LogSync.h>

#include "package/internal/Rta.xdc.h"

#ifdef xdc_target__bigEndian
#define  ENDIANNESS UIAPacket_PayloadEndian_BIG
#else
#define  ENDIANNESS UIAPacket_PayloadEndian_LITTLE
#endif

/*
 *  ======== Rta_swizzle ========
 *  Perform 32 bit swizzle if the endianness of payload in the packet
 *  is different than the target.
 */
UArg Rta_swizzle(UIAPacket_Hdr *packet, UArg arg)
{
    if (UIAPacket_getPayloadEndianness(packet) == ENDIANNESS) {
        return (arg);
    }

    return (UIAPacket_swizzle(arg));
}

/*
 *  ======== Rta_sendEvents ========
 */
Void Rta_sendEvents()
{
    UInt i;
    Bool prevState = FALSE;
    IUIATransfer_Handle logger;
    Int priority;

    /*
     *  Only send data to the host if the "start" command has been issued
     *  or if the period was configured to a non-zero value.
     */
    if ((Rta_module->txData == TRUE) || (Rta_module->snapshot == TRUE)) {

        /*
         * If the target has been suspended or halted since the last time
         * an event packet was sent to the host, log a new sync point event
         * to record the current correlation info between the local timestamp
         * and the global timestamp.
         * Always log a sync point if this is the end of a snapshot delay.
         */
        if (LogSync_isSyncEventRequired() || (Rta_module->snapshot == TRUE)) {
            LogSync_writeSyncPoint();
        }

        /* Send up the logs in order of priority, from highest to lowest */
        for (priority = (Int)IUIATransfer_Priority_SYNC;
             priority >= (Int)IUIATransfer_Priority_LOW; priority--) {
            for (i = 0; i < Rta_module->numLoggers; i++) {
                logger = Rta_module->loggers[i];

                /*
                 *  If a SNAPSHOT was requested, disable the logger and
                 *  get all the events out. Then restore the state afterwards.
                 */
                if (Rta_module->snapshot == TRUE) {
                    prevState = IUIATransfer_disable(logger);
                }

                /*
                 *  Transfer the data.
                 */
                if (IUIATransfer_getPriority(logger) == priority) {
                    Rta_flushLogger(logger, IUIATransfer_getInstanceId(logger));
                }

                /*
                 * If the logger was enabled, re-enable it now that it is empty.
                 */
                if ((Rta_module->snapshot == TRUE) &&
                    (prevState == TRUE)) {
                    IUIATransfer_enable(logger);
                }
            }
        }

        /* Reset the previos period */
        if (Rta_module->snapshot == TRUE) {
            Rta_module->snapshot = FALSE;
            ServiceMgr_setPeriod(Rta_SERVICEID, Rta_module->period);
        }
    }
}

/*
 *  ======== Rta_flushLogger ========
 *  This helper function sends up all of the records from one IUIATransfer
 *  instance.
 *
 *  This function also keeps track of the number of packets being sent, and
 *  returns if the number of packets sent is >= to the number of packets in the
 *  instance. Because the target can continue to log events while the buffer
 *  is being flushed, there may still be more records to send, but we want to
 *  limit how many records we send from one buffer so that we can continue on
 *  to the other buffers.
 */
Void Rta_flushLogger(IUIATransfer_Handle logger, UInt loggerNum)
{
    SizeT bytes;
    SizeT txBufLen = ServiceMgr_maxEventPacketSize - sizeof(UIAPacket_Hdr);
    Bool moreRecords;
    UIAPacket_Hdr *packet;
    Char *txBuf;
    Int16 maxNumPackets;
    Int16 totalPackets = 0;
    UInt maxLength = IUIATransfer_getMaxLength(logger);
    Bool status;

    /* No sense doing anything if it is empty */
    if (IUIATransfer_isEmpty(logger) == TRUE) {
        return;
    }

    /* Limit how many packets will be sent for this logger */
    maxNumPackets = (maxLength / ServiceMgr_maxEventPacketSize) + 1;

    while (TRUE) {
        /* Get a packet. */
        packet = ServiceMgr_getFreePacket(UIAPacket_HdrType_EventPkt,
                                       ServiceMgr_WAIT_FOREVER);

        /* Point to the payload portion of the packet */
        txBuf = (Char *)(packet);

        /* Fill in the packet. */
        moreRecords = IUIATransfer_getContents(logger, txBuf, txBufLen, &bytes);
        if (bytes != 0) {

            /* Initialize the UIA packager header */
            UIAPacket_initEventRecHdr(packet, ENDIANNESS, bytes, Rta_module->seq,
                IUIATransfer_getPriority(logger),
                ti_uia_runtime_IUIATransfer_Module_id(
                    IUIATransfer_Handle_to_Module(logger)),
                    loggerNum, UIAPacket_HOST, (~0));

            /* Send the the packet. */
            totalPackets++;
            status = ServiceMgr_sendPacket(packet);
            if (status == TRUE) {
                /* Track how many records sent for ROV. */
                Rta_module->totalPacketsSent++;

                /* Increment for the next packet */
                (Rta_module->seq)++;

                /* Log the transmission. */
                Log_write3(Rta_LD_recordsSent, bytes, loggerNum, (IArg)logger);
            }
            else {
                ServiceMgr_freePacket(packet);
            }
        }
        else {
            /*
             *  Nothing to send. Free the packet.
             *  This should not happen since the logger was not empty.
             */
            ServiceMgr_freePacket(packet);
        }

        /*
         * If there are no more records to send, or if maxNumPackets have been
         * sent, just break out of the loop and return.
         */
        if ((moreRecords == FALSE) || (totalPackets == maxNumPackets)) {
            break;
        }
    }
}

/*
 *  ======== Rta_processCallback ========
 */
Void Rta_processCallback(ServiceMgr_Reason reason, UIAPacket_Hdr *packet)
{
    if (reason == ServiceMgr_Reason_PERIODEXPIRED) {
        Rta_sendEvents();
    }
    else if (reason == ServiceMgr_Reason_INCOMINGMSG) {
        Rta_processMsg(packet);
    }
}

/*
 *  ======== Rta_processMsg ========
 */
Void Rta_processMsg(UIAPacket_Hdr *packet)
{
    Bool status;
    Rta_Command cmdId;
    UArg arg0;
    UArg arg1;
    Rta_Packet *resp;
    Rta_Packet *cmd = (Rta_Packet *)packet;
    UIAPacket_MsgType msgType = UIAPacket_MsgType_ACK;

    /* Grab a free packet */
    resp = (Rta_Packet *)ServiceMgr_getFreePacket(UIAPacket_HdrType_Msg,
                                                  ServiceMgr_WAIT_FOREVER);

    /* Extract the actual Rta command */
    cmdId = (Rta_Command)UIAPacket_getCmdId(packet);

    arg0 = Rta_swizzle(packet, cmd->arg0);
    arg1 = Rta_swizzle(packet, cmd->arg1);

    /* Log receipt of a command. */
    Log_write3(Rta_LD_cmdRcvd, cmdId, arg0, arg1);

    /* Handle the command. */
    switch(cmdId) {
        case Rta_Command_READ_MASK:
            msgType = Rta_readMask(resp, arg0);
            break;
        case Rta_Command_WRITE_MASK:
            msgType = Rta_writeMask(resp, arg0, arg1);
            break;
        case Rta_Command_LOGGER_OFF:
            Rta_disableLog(arg0);
            break;
        case Rta_Command_LOGGER_ON:
            Rta_enableLog(arg0);
            break;
        case Rta_Command_GET_CPU_SPEED:
            Rta_getCpuSpeed(resp);
            break;
        case Rta_Command_RESET_LOGGER:
            Rta_resetLog(arg0);
            break;
        case Rta_Command_CHANGE_PERIOD:
            Rta_changePeriod(arg0);
            break;
        case Rta_Command_START_TX:
            Rta_startDataTx();
            break;
        case Rta_Command_STOP_TX:
            Rta_stopDataTx();
            break;
        case Rta_Command_LOGGER_OFF_ALL:
            Rta_disableAllLogs();
            break;
        case Rta_Command_LOGGER_ON_ALL:
            Rta_enableAllLogs();
            break;
        case Rta_Command_RESET_LOGGER_ALL :
            Rta_resetAllLogs();
            break;
        case Rta_Command_SNAPSHOT_ALL:
            Rta_snapshotAllLogs(arg0, arg1);
            break;
        default:
            msgType = UIAPacket_MsgType_NACK_BAD_DATA;
            break;
    }

    /* ServiceMgr module fills in the src addresses */
    UIAPacket_initMsgHdr((UIAPacket_Hdr *)resp, ENDIANNESS,
                         msgType, sizeof(Rta_Packet),
                         Rta_SERVICEID, UIAPacket_getSequenceCount(packet),
                         cmdId, UIAPacket_getTag(packet), UIAPacket_HOST, (~0));

    /* Send the response back to the host */
    status = ServiceMgr_sendPacket((UIAPacket_Hdr *)resp);
    if (status == FALSE) {
        ServiceMgr_freePacket((UIAPacket_Hdr *)resp);
    }
}

/*
 *  ======== Rta_readMask ========
 * The address passed in is the address of diagsMask__C, which holds the
 * address of the actual diagsMask in the module state structure. So
 * the address passed in must be dereferenced twice.
 * The diagsMask is a Bits16.
 */
UIAPacket_MsgType Rta_readMask(Rta_Packet *resp, UArg addr)
{
    Bits16 *maskAddr;

    /* Make sure we don't have any NULL pointers */
    if ((addr == (UArg)NULL) || (*(Bits16 *)addr == 0)) {
        resp->arg0 = Rta_ErrorCode_NULLPOINTER;
        return (UIAPacket_MsgType_NACK_WITH_ERROR_CODE);
    }

    maskAddr = *((Bits16 **) addr);

    resp->arg0 = *maskAddr;

    return (UIAPacket_MsgType_ACK);
}

/*
 *  ======== Rta_writeMask ========
 * The address passed in is the address of diagsMask__C, which holds the
 * address of the actual diagsMask in the module state structure. So
 * the address passed in must be dereferenced twice.
 * The diagsMask is a Bits16.
 */
UIAPacket_MsgType Rta_writeMask(Rta_Packet *resp, UArg addr, UArg val)
{
    Bits16 *maskAddr;

    /* Make sure we don't have any NULL pointers */
    if ((addr == (UArg)NULL) || (*(Bits16 *)addr == 0)) {
        resp->arg0 = Rta_ErrorCode_NULLPOINTER;
        return (UIAPacket_MsgType_NACK_WITH_ERROR_CODE);
    }

    maskAddr = *((Bits16 **) addr);

    /* The diagsMask is a Bits16. */
    *maskAddr = (Bits16) val;

    Log_write2(Rta_LD_writeMask, (UArg)maskAddr, (Bits16)val);

    return (UIAPacket_MsgType_ACK);
}

/*
 *  ======== Rta_enableAllLogs ========
 */
Void Rta_enableAllLogs()
{
    UInt i;

    for (i = 0; i < Rta_module->numLoggers; i++) {
        Rta_enableLog(i);
    }
}

/*
 *  ======== Rta_snapshotAllLogs ========
 */
Void Rta_snapshotAllLogs(UArg reset, UArg waitPeriod)
{
    UInt logNum;
    IUIATransfer_Handle logger;

    Rta_module->snapshot = TRUE;

    if ((Bool)reset == TRUE) {
        for (logNum = 0; logNum < Rta_module->numLoggers; logNum++) {

            /* Retrieve the logger from the Agent's list. */
            logger = Rta_module->loggers[logNum];
            IUIATransfer_reset(logger);
        }
    }

    LogSync_writeSyncPoint();

    ServiceMgr_setPeriod(Rta_SERVICEID, (UInt)waitPeriod);
}

/*
 *  ======== Rta_enableLog ========
 */
Void Rta_enableLog(UArg log)
{
    UInt32 logNum = (UInt32) log;
    IUIATransfer_Handle logger;

    if (logNum < Rta_module->numLoggers) {

        /* Retrieve the logger from the Agent's list. */
        logger = Rta_module->loggers[logNum];

        IUIATransfer_enable(logger);
    }
}

/*
 *  ======== Rta_disableAllLogs ========
 */
Void Rta_disableAllLogs()
{
    UInt i;

    for (i = 0; i < Rta_module->numLoggers; i++) {
        Rta_disableLog(i);
    }
}

/*
 *  ======== Rta_disableLog ========
 */
Void Rta_disableLog(UArg log)
{
    UInt32 logNum = (UInt32) log;
    IUIATransfer_Handle logger;

    if (logNum < Rta_module->numLoggers) {

        /* Retrieve the logger from the Agent's list. */
        logger = Rta_module->loggers[logNum];

        IUIATransfer_disable(logger);
    }
}

/*
 *  ======== Rta_resetAllLogs ========
 */
Void Rta_resetAllLogs()
{
    UInt i;

    for (i = 0; i < Rta_module->numLoggers; i++) {
        Rta_resetLog(i);
    }
}

/*
 *  ======== Rta_resetLog ========
 */
Void Rta_resetLog(UArg log)
{
    UInt32 logNum = (UInt32) log;
    IUIATransfer_Handle logger;

    if (logNum < Rta_module->numLoggers) {

        /* Retrieve the logger from the Agent's list. */
        logger = Rta_module->loggers[logNum];

        /* Reset the logger */
        IUIATransfer_reset(logger);

        if (logger == ti_uia_runtime_LogSync_Module__loggerObj__C){
            LogSync_writeSyncPoint();
        }
    }
}

/*
 *  ======== Rta_changePeriod ========
 */
Void Rta_changePeriod(UArg period)
{
    /* Store in module state */
    Rta_module->period = period;

    /* Update the ServiceMgr module if Rta_Command_START_TX was done. */
    if (Rta_module->txData == TRUE) {
        ServiceMgr_setPeriod(Rta_SERVICEID, (UInt)period);
    }
}

/*
 *  ======== Rta_startDataTx ========
 */
Void Rta_startDataTx()
{
    Rta_module->txData = TRUE;
    ServiceMgr_setPeriod(Rta_SERVICEID, Rta_module->period);

    /*
     * Log a sync point event to ensure that there
     * is enough info to interpret the event timestamps
     * properly.
     */
    LogSync_writeSyncPoint();
}

/*
 *  ======== Rta_stopDataTx ========
 */
Void Rta_stopDataTx()
{
    Rta_module->txData = FALSE;

    /*
     * Reset to initial value. If a different host starts a session,
     * it should not inherit the previous host's period request.
     */
    Rta_module->period = Rta_periodInMs;
    ServiceMgr_setPeriod(Rta_SERVICEID, 0);

    /*
     * Log a sync point event so that all events logged
     * from this point on will have enough info to
     * interpret their timestamps properly
     */
    LogSync_writeSyncPoint();
}

/*
 *  ======== Rta_getCpuSpeed ========
 */
Void Rta_getCpuSpeed(Rta_Packet *resp)
{
    Types_FreqHz freq;

    /* Get the Timestamp frequency. */
    Timestamp_getFreq(&freq);

    resp->arg0 = freq.hi;
    resp->arg1 = freq.lo;
}
