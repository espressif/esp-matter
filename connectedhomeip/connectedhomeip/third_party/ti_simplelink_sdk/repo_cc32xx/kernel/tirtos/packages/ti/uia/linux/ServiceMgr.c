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
 *  ======== ServiceMgr.c ========
 */

//#include <Std.h>
#include <ti/syslink/Std.h>
#include <ti/syslink/utils/Memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ipc.h>

#include <ti/ipc/MessageQ.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MultiProc.h>

#include "UIAPacket.h"
#include "ServiceMgr.h"

#define ServiceMgr_TCPPORT 1234
#define ServiceMgr_UDPPORT 1235

#define ServiceMgr_NUMBROADCASTS 3

/*
 * Different message types used in UIA.
 * Must match the ti/uia/runtime/MultiCoreTypes.xdc file
 */
enum IpcTypes_Action {
    IpcTypes_Action_TOHOST = 0,
    IpcTypes_Action_FROMHOST,
    IpcTypes_Action_REGISTER,
    IpcTypes_Action_STOP,
    IpcTypes_Action_STOPACK
};

/*
 * Register message structure from the slaves.
 * Must match the ti/uia/runtime/MultiCoreTypes.xdc file
 */
typedef struct IpcTypes_RegisterMsg {
    MessageQ_MsgHeader msgHdr;
    UInt32             remoteMQ;
} IpcTypes_RegisterMsg;

/*
 * Names of UIA messages queues. The uiaSlave will have the MultiProc
 * id appended onto the end.
 * Must match the ti/uia/runtime/MultiCoreTypes.xdc file
 */
const String MultiCoreTypes_SLAVENAME = "uiaSlave";
const String MultiCoreTypes_MASTERNAME = "uiaMaster";
const String MultiCoreTypes_MASTERSTARTED = "uiaStarted";

/*
 * Internal helper functions.
 */
Void ServiceMgr_allocateResources();
Void ServiceMgr_cleanupResources();
Void ServiceMgr_handleMsg(MessageQ_Msg msg);
static inline UIAPacket_Hdr * ServiceMgr_msgToPacket(MessageQ_Msg msg);
Void ServiceMgr_prime(Ptr handle, Int size, Int count);
Void ServiceMgr_sendto(Ptr buf);
Void ServiceMgr_stopXferAgent(Ptr msg);
Ptr ServiceMgr_transferAgentFxn(Ptr arg);

ServiceMgr_Config ServiceMgr_config = {128, 2, 0, ""};

/*
 * Internal state structure.
 */
typedef struct ServiceMgr_State {
    MessageQ_Handle hostMsgMQ;
    MessageQ_Handle broadcastMsgMQ;
    MessageQ_Handle routerMQ;
    MessageQ_Handle startedMQ;
    MessageQ_QueueId masterMQ;
    Int transferAgentTid;
    Int rxThreadTid;
    MessageQ_QueueId replyMQ[8];
    Int msgSocket;
    Int eventSocket;
    Int clientSocket;
    struct sockaddr_in msgSin;
    struct sockaddr_in eventSin;
    struct sockaddr_in clientSin;
    MessageQ_Msg stopMsg;
    Ptr heap;
    Bool started;
    Bool shutdown;
    FILE *outFile;
} ServiceMgr_State;

/*
 * Internal state variable.
 */
ServiceMgr_State ServiceMgr_module;

/*
 *  ======== ServiceMgr_msgToPacket ========
 */
static inline UIAPacket_Hdr * ServiceMgr_msgToPacket(MessageQ_Msg msg)
{
    return (UIAPacket_Hdr *)((Char *)msg + sizeof(MessageQ_MsgHeader));
}

/*
 *  ======== ServiceMgr_msgToPacket ========
 */
static inline MessageQ_Msg ServiceMgr_packetToMsg(UIAPacket_Hdr *packet)
{
    return (MessageQ_Msg)((Char *)packet - sizeof(MessageQ_MsgHeader));
}

/*
 *  ======== ServiceMgr_allocateResources ========
 *  Allocate resources needed by the ServiceMgr threads
 */
Void ServiceMgr_allocateResources()
{
    Int allocSize = sizeof(MessageQ_MsgHeader);
    MessageQ_Params params;

    MessageQ_Params_init(&params);

    ServiceMgr_module.hostMsgMQ = MessageQ_create("ServiceMgr_inMsgs", &params);
    ServiceMgr_module.broadcastMsgMQ = MessageQ_create("ServiceMgr_outMsgs", &params);
    ServiceMgr_module.routerMQ = MessageQ_create(MultiCoreTypes_MASTERNAME, &params);

    /*
     *  Allocate a shutdown message. Not using a "free" msg since
     *  getting one might result in blocking
     */
    ServiceMgr_module.stopMsg = Memory_alloc(ServiceMgr_module.heap,
                                             allocSize, 0, NULL);
    if (ServiceMgr_module.stopMsg == NULL) {
        printf("ServiceMgr_allocateResources: Fatal error because Memory allocation failed\n");
        return;
    }

    /* No calls to MessageQ_alloc/free so we can use the staticMsgInit. */
    MessageQ_staticMsgInit(ServiceMgr_module.stopMsg, allocSize);
    MessageQ_setMsgId(ServiceMgr_module.stopMsg, IpcTypes_Action_STOP);

    /* Prime the incoming packets */
    ServiceMgr_prime(ServiceMgr_module.hostMsgMQ,
                     ServiceMgr_config.maxCtrlPacketSize,
                     ServiceMgr_config.numIncomingCtrlPacketBufs);

    /* Prime the broadcast packets */
    ServiceMgr_prime(ServiceMgr_module.broadcastMsgMQ,
                     ServiceMgr_config.maxCtrlPacketSize,
                     ServiceMgr_NUMBROADCASTS);

    /* Open the file if present */
    if (strlen(ServiceMgr_config.fileName) != 0) {
        ServiceMgr_module.outFile = fopen(ServiceMgr_config.fileName, "w");
    }

    ServiceMgr_module.startedMQ =
        MessageQ_create(MultiCoreTypes_MASTERSTARTED, &params);
}

/*
 *  ======== ServiceMgr_cleanupResources ========
 *  Clean up resources needed by the ServiceMgr threads
 */
Void ServiceMgr_cleanupResources()
{
    Int status;
    MessageQ_Msg msg;

    /* Free all the messages */
    while (TRUE) {

        status = MessageQ_get((MessageQ_Handle)(ServiceMgr_module.hostMsgMQ),
                               &msg, 0);
        if (status < 0) {
            break;
        }

        Memory_free(ServiceMgr_module.heap, msg,
            ServiceMgr_config.maxCtrlPacketSize + sizeof(MessageQ_MsgHeader));
    }

    /* Delete the queue now that it is empty */
    MessageQ_delete((MessageQ_Handle *)&(ServiceMgr_module.hostMsgMQ));

    /*
     *  Delete the router message queue. Nothing should be on this queue
     *  because part of the shutdown, the transfer agent communicates with the
     *  slaves and tells them to stop.
     */
    MessageQ_delete((MessageQ_Handle *)&(ServiceMgr_module.routerMQ));

    /* Free all the messages */
    while (TRUE) {

        status = MessageQ_get((MessageQ_Handle)(ServiceMgr_module.broadcastMsgMQ),
                               &msg, 0);
        if (status < 0) {
            break;
        }
        Memory_free(ServiceMgr_module.heap, msg,
            ServiceMgr_config.maxCtrlPacketSize + sizeof(MessageQ_MsgHeader));
    }
    MessageQ_delete((MessageQ_Handle *)&(ServiceMgr_module.broadcastMsgMQ));

    /* Free the stopMsg */
    Memory_free(ServiceMgr_module.heap, ServiceMgr_module.stopMsg,
                sizeof(MessageQ_MsgHeader));

    /* Close the file if present */
    if (strlen(ServiceMgr_config.fileName) != 0) {
        fclose(ServiceMgr_module.outFile);
    }
}

/*
 *  ======== ServiceMgr_getConfig ========
 */
Void ServiceMgr_getConfig(ServiceMgr_Config *config)
{
    config->maxCtrlPacketSize = ServiceMgr_config.maxCtrlPacketSize;
    config->numIncomingCtrlPacketBufs = ServiceMgr_config.numIncomingCtrlPacketBufs;
    config->sharedRegionId = ServiceMgr_config.sharedRegionId;
    strcpy(config->fileName, ServiceMgr_config.fileName);
}

/*
 *  ======== ServiceMgr_handleMsg ========
 */
Void ServiceMgr_handleMsg(MessageQ_Msg msg)
{
    Int i;
    UInt16 dstProcId;
    UIAPacket_Hdr *packet;
    UIAPacket_Hdr *remotePacket;
    MessageQ_Msg remoteMsg;
    Int status;
    UInt32 *nackCode;
    Char slaveName[16];
    MessageQ_Handle freeMQ;

    switch (MessageQ_getMsgId(msg)) {

        case IpcTypes_Action_TOHOST:
            /* Send out over Ethernet */
            ServiceMgr_sendto(msg);
            break;

        case IpcTypes_Action_FROMHOST:

            /* Point to the packet portion (skip MessageQ header) */
            packet = ServiceMgr_msgToPacket(msg);
            dstProcId = UIAPacket_getDestAdrs(packet);

            /* Check to see if this is a broadcast */
            if (dstProcId == UIAPacket_BROADCAST) {

                /* Send out to each registered slave */
                for (i = 0; i < MultiProc_getNumProcessors(); i++) {
                    if ((UInt16)ServiceMgr_module.replyMQ[i] !=
                        (UInt16)MessageQ_INVALIDMESSAGEQ) {
                        sprintf(slaveName, "%s%02d", MultiCoreTypes_SLAVENAME,
                                i);
                        status = MessageQ_open(slaveName,
                                     &(ServiceMgr_module.replyMQ[i]));
                    }

                    if ((UInt16)ServiceMgr_module.replyMQ[i] !=
                        (UInt16)MessageQ_INVALIDMESSAGEQ) {

                        freeMQ = (MessageQ_Handle)(ServiceMgr_module.broadcastMsgMQ);
                        status = MessageQ_get(freeMQ, &remoteMsg, MessageQ_FOREVER);
                        if (status < 0) {
                            printf("ServiceMgr_handleMsg: MessageQ_get failed: status = 0x%x\n", status);
                        }
                        else {
                            /* Copy contents into the message */
                            remotePacket = ServiceMgr_msgToPacket(remoteMsg);
                            memcpy(remotePacket, packet, UIAPacket_getMsgLength(packet));

                            /* Set up fields in the message and send it */
                            MessageQ_setReplyQueue(freeMQ, remoteMsg);
                            MessageQ_setMsgId(remoteMsg, IpcTypes_Action_FROMHOST);
                            status = MessageQ_put(ServiceMgr_module.replyMQ[i], remoteMsg);
                        }
                    }
                }
                status = MessageQ_put(MessageQ_getReplyQueue(msg), msg);
                if (status < 0) {
                    printf("ServiceMgr_handleMsg: MessageQ_put failed: status = 0x%x\n", status);
                }
            }
            else if ((UInt16)(ServiceMgr_module.replyMQ[dstProcId]) !=
                (UInt16)MessageQ_INVALIDMESSAGEQ) {
                 /* Send to the remote processor */
                status = MessageQ_put(ServiceMgr_module.replyMQ[dstProcId], msg);
                if (status < 0) {
                    printf("ServiceMgr_handleMsg: MessageQ_put failed: status = 0x%x\n", status);
                }
            }
            else {
                if (dstProcId == MultiProc_self()) {
                    printf("Logging on Linux not currently supported.\n");
                }
                else {
                    sprintf(slaveName, "%s%02d", MultiCoreTypes_SLAVENAME,
                            dstProcId);
                    status = MessageQ_open(slaveName,
                                 &(ServiceMgr_module.replyMQ[dstProcId]));
                    if (status >= 0) {
                        /* Send to the remote processor */
                        status = MessageQ_put(ServiceMgr_module.replyMQ[dstProcId], msg);
                        if (status < 0) {
                            printf("ServiceMgr_handleMsg: MessageQ_put failed: status = 0x%x\n", status);
                        }
                        return;
                    }
                }

                printf("ServiceMgr_handleMsg: %d processor not registered\n",
                       dstProcId);

                /* Point to the packet portion (skip MessageQ header) */
                packet = ServiceMgr_msgToPacket(msg);

                /* send NACK back. Re-use existing message. */
                UIAPacket_setMsgType(packet,
                    UIAPacket_MsgType_NACK_WITH_ERROR_CODE);
                UIAPacket_setMsgLength(packet, sizeof(UIAPacket_Hdr) +
                    sizeof(UIAPacket_NACKErrorCode));
                UIAPacket_setSenderAdrs(packet, dstProcId);
                UIAPacket_setDestAdrs(packet, UIAPacket_HOST);
                nackCode = (UInt32 *)((UInt)packet + sizeof(UIAPacket_Hdr));
                *nackCode = UIAPacket_swizzle(UIAPacket_NACKErrorCode_BAD_ENDPOINT_ADDRESS);

                ServiceMgr_sendto(msg);
            }
            break;

        case IpcTypes_Action_REGISTER:
            /* Fill in the replyMQ table */
            ServiceMgr_module.replyMQ[msg->srcProc] =
                ((IpcTypes_RegisterMsg *)msg)->remoteMQ;

            /* Send the message back */
            status = MessageQ_put(MessageQ_getReplyQueue(msg), msg);
            if (status < 0) {
                printf("ServiceMgr_handleMsg: MessageQ_put failed: status = 0x%x\n", status);
            }
            break;

        case IpcTypes_Action_STOP:
            ServiceMgr_stopXferAgent(msg);
            break;

        default:
            printf("ServiceMgr_handleMsg: invalid action = 0%d\n",
                   MessageQ_getMsgId(msg));
            break;
    }
}

/*
 *  ======== ServiceMgr_prime ========
 *  Prime the the MessageQ.
 */
Void ServiceMgr_prime(Ptr handle, Int size, Int count)
{
    Int i;
    Int allocSize = size + sizeof(MessageQ_MsgHeader);
    MessageQ_Msg msg;
    Int status;

    for (i = 0; i < count; i++) {

        /* Allocate the shared memory from the shared memory heap */
        msg = Memory_alloc(ServiceMgr_module.heap, allocSize, 0, NULL);
        if (msg == NULL) {
            printf("ServiceMgr_prime: Fatal error because Memory allocation failed\n");
            return;
        }

        /* No calls to MessageQ_alloc/free so we can use the staticMsgInit. */
        MessageQ_staticMsgInit(msg, allocSize);

        /* Place onto the "free" message queue */
        status = MessageQ_put(MessageQ_getQueueId(handle), msg);
        if (status < 0) {
            printf("ServiceMgr_prime: MessageQ_put failed: status = 0x%x\n", status);
        }
    }
}

/*
 *  ======== ServiceMgr_rxThreadFxn ========
 *  Task that receives incoming messages from the host via the Udp.
 *  These messages are then sent to the transfer agent.
 */
Void ServiceMgr_rxThreadAccept(struct timeval *tv)
{
    UInt clientLen;

    /* Wait for a client to connect */
    clientLen = sizeof(ServiceMgr_module.clientSin);
    while (ServiceMgr_module.shutdown == FALSE) {

        ServiceMgr_module.clientSocket = accept(ServiceMgr_module.msgSocket,
            (struct sockaddr *)&(ServiceMgr_module.clientSin), &clientLen);
        if (ServiceMgr_module.clientSocket >= 0) {
            /* Option to have recv only wait for 100ms */
            setsockopt (ServiceMgr_module.clientSocket, SOL_SOCKET, SO_RCVTIMEO,
                (char*)tv, sizeof tv);
            break;
        }
        else if ((ServiceMgr_module.clientSocket < 0) && (errno != EAGAIN)) {
            printf("ServiceMgr_rxThreadFxn: accept() failed with errno = %d\n",
                   errno);
            return;
        }
    }
    return;
}

/*
 *  ======== ServiceMgr_rxThreadFxn ========
 *  Task that receives incoming messages from the host via the Udp.
 *  These messages are then sent to the transfer agent.
 */
Ptr ServiceMgr_rxThreadFxn(Ptr arg)
{
    Int status;
    Int one = 1;
    MessageQ_Msg msg;
    Bool msgPresent = FALSE;
    UIAPacket_Hdr *packet = NULL;
    struct sockaddr_in *msgSin;
    socklen_t tmp = sizeof(struct sockaddr_in);
    MessageQ_Handle freeMQ;
    struct timeval tv;

    tv.tv_sec = 0;
        tv.tv_usec = 100;

    /* Set up the msgSocket */
    do {
        ServiceMgr_module.msgSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    while (ServiceMgr_module.msgSocket == -1);

    /* Option to handle fast reconnects */
    status = setsockopt(ServiceMgr_module.msgSocket, SOL_SOCKET, SO_REUSEADDR,
                        &one, sizeof(one));

    /* Initialize the socket addresses */
    msgSin = &(ServiceMgr_module.msgSin);
    memset((char *)msgSin, 0, sizeof(struct sockaddr_in));
    msgSin->sin_family      = AF_INET;
    msgSin->sin_port        = htons(1234);
    msgSin->sin_addr.s_addr = htonl(INADDR_ANY);

    /* bind the address information */
    status = bind(ServiceMgr_module.msgSocket, (struct sockaddr *)msgSin, tmp);
    if (status != 0) {
        printf("ServiceMgr_rxThreadFxn: bind() failed with errno = %d\n", errno);
        return (NULL);
    }

    /* Mark the socket so it will listen for 1 incoming connection */
    status = listen(ServiceMgr_module.msgSocket, 1);
    if (status != 0) {
        printf("ServiceMgr_rxThreadFxn: listen() failed with errno = %d\n", errno);
        return (NULL);
    }

    /* Option to have accept only wait for 100ms */
    setsockopt (ServiceMgr_module.msgSocket, SOL_SOCKET, SO_RCVTIMEO,
                (char*)&tv, sizeof tv);

    /* Wait for a client to connect */
    ServiceMgr_rxThreadAccept(&tv);

    /*
     *  This queue maintains the "free" buffers that will be
     *  filled in with data from the instrumentation host.
     *  These are will be routed to the destination processors.
     *  The remote processors are responsible for sending this
     *  back to the free queue (via the getReplyQueue feature
     *  of MessageQ).
     *  Using local variable to make code in the loop more readable.
     */
    freeMQ = (MessageQ_Handle)(ServiceMgr_module.hostMsgMQ);

    /*
     *  Loop to receive msgs from the instrumentation host
     */
    while (ServiceMgr_module.shutdown == FALSE) {

        /*
         *  Get a free buffer to be used in the recvfrom call.
         *  Only wait up to 100ms.
         */
        if (msgPresent == FALSE) {
            status = MessageQ_get(freeMQ, &msg, 100000);
            if (status < 0) {
                continue;
            }

            /* Point to the packet portion (skip MessageQ header) */
            packet = ServiceMgr_msgToPacket(msg);

            msgPresent = TRUE;
        }

        /* Get the incoming message from the instrumentation host */
        status = (int)recv(ServiceMgr_module.clientSocket, packet,
            (ServiceMgr_config.maxCtrlPacketSize - sizeof(MessageQ_MsgHeader)), 0);
        if (status > 0) {

            /* Validate the type of message */
            if (UIAPacket_getHdrType(packet) == UIAPacket_HdrType_Msg) {

                /* To allow receiver to know where to send it to */
                MessageQ_setReplyQueue(freeMQ, msg);

                /* To inform the remote processor of the action */
                MessageQ_setMsgId(msg, IpcTypes_Action_FROMHOST);

                /* Send the message to the remote processor */
                status = MessageQ_put(ServiceMgr_module.masterMQ, msg);
                if (status < 0) {
                    printf("ServiceMgr_rxThreadFxn: MessageQ_put failed. \
                            Dropped msg from host. status = 0x%x\n", status);
                }
                else {
                    msgPresent = FALSE;
                }

                /*
                 *  Set the event socket's dest IP address to the same as the
                 *  msg's socket. This allows changing the host to change IP address
                 *  or move to a different machine.
                 */
                ServiceMgr_module.eventSin.sin_addr.s_addr =
                    ServiceMgr_module.clientSin.sin_addr.s_addr;
            }
            else {
                printf("ServiceMgr_rxThreadFxn: received invalid packet HdrType = 0x%x\n",
                       UIAPacket_getHdrType(packet));
            }
        }
        else if ((errno != ETIMEDOUT) && (errno != EAGAIN)){
            /* Return to the free queue */
            status = MessageQ_put(MessageQ_getQueueId(freeMQ), msg);
            if (status < 0) {
                printf("ServiceMgr_rxThreadFxn: MessageQ_put failed: status = 0x%x\n", status);
            }

            /* Stop the events from being sent */
            ServiceMgr_module.eventSin.sin_addr.s_addr = htonl(INADDR_ANY);
            close(ServiceMgr_module.clientSocket);

            /* Wait for a client to connect */
            ServiceMgr_rxThreadAccept(&tv);
        }
    }

    /* Shutdown the socket */
    if (ServiceMgr_module.clientSocket >= 0) {
        close(ServiceMgr_module.clientSocket);
    }
    if (ServiceMgr_module.msgSocket >= 0) {
        close(ServiceMgr_module.msgSocket);
    }

    return (NULL);
}

/*
 *  ======== ServiceMgr_sendto ========
 *  Call the socket's sendto function.
 */
Void ServiceMgr_sendto(Ptr buf)
{
    Int sentBytes;
    Int status;
    UIAPacket_Hdr *packet;
    MessageQ_Msg msg = (MessageQ_Msg)buf;

    /* Point to the packet portion (skip MessageQ header) */
    packet = ServiceMgr_msgToPacket(msg);

    /*
     *  Determine the type of packet and handle accordingly:
     *    Msg:                        send via the clientSocket and put back
     *                                to free queue
     *    Event (IP address is set):  send via the eventSocket and put back
     *                                to free queue
     *    Event (IP address is !set): put back to free queue
     */
    if (UIAPacket_getHdrType(packet) == UIAPacket_HdrType_Msg) {
        sentBytes = sendto(ServiceMgr_module.clientSocket, packet,
                           UIAPacket_getMsgLength(packet), 0,
                           (struct sockaddr *)&(ServiceMgr_module.clientSin),
                           sizeof(struct sockaddr));
        if (sentBytes != UIAPacket_getMsgLength(packet)) {
            printf("ServiceMgr_sendto: Failed to send all of UIA Message\n");
        }
    }
    else if (strlen(ServiceMgr_config.fileName) != 0) {
        fwrite(packet, UIAPacket_getEventLength(packet), 1,
               ServiceMgr_module.outFile);
    }
    else if (ServiceMgr_module.eventSin.sin_addr.s_addr
             != htonl(INADDR_ANY)) {
        sentBytes = sendto(ServiceMgr_module.eventSocket, packet,
                        UIAPacket_getEventLength(packet), 0,
                        (struct sockaddr *)&(ServiceMgr_module.eventSin),
                        sizeof(struct sockaddr_in));
        if (sentBytes != UIAPacket_getEventLength(packet)) {
            printf("ServiceMgr_sendto: Failed to send all of UIA Event\n");
        }
    }

    /* Give the message back */
    status = MessageQ_put(MessageQ_getReplyQueue(msg), msg);
    if (status < 0) {
        printf("ServiceMgr_sendto: MessageQ_put failed: status = 0x%x\n", status);
    }
}

/*
 *  ======== ServiceMgr_setConfig ========
 */
Int ServiceMgr_setConfig(ServiceMgr_Config *config)
{
    Int status;

    if ((ServiceMgr_module.started == TRUE) ||
        (strlen(config->fileName) > (sizeof(ServiceMgr_config.fileName) - 1))) {
        status = ServiceMgr_FAILED;
    }
    else {
        ServiceMgr_config.maxCtrlPacketSize = config->maxCtrlPacketSize;
        ServiceMgr_config.numIncomingCtrlPacketBufs = config->numIncomingCtrlPacketBufs;
        ServiceMgr_config.sharedRegionId = config->sharedRegionId;
        strncpy(ServiceMgr_config.fileName, config->fileName, sizeof(ServiceMgr_config.fileName));
        status = ServiceMgr_SUCCESS;
    }
    return (status);
}

/*
 *  ======== ServiceMgr_signalHandler ========
 */
Void ServiceMgr_signalHandler(Int sig)
{
    /* Start the shutdown... */
    ServiceMgr_stop();
}

/*
 *  ======== ServiceMgr_start ========
 */
Void ServiceMgr_start()
{
    Int i;

    /* Allow the function to be called multiple times */
    if (ServiceMgr_module.started == TRUE) {
        return;
    }

    ServiceMgr_module.started = TRUE;
    ServiceMgr_module.shutdown = FALSE;

    ServiceMgr_module.heap = SharedRegion_getHeap(ServiceMgr_config.sharedRegionId);
    if (ServiceMgr_module.heap == NULL) {
        printf("ServiceMgr_start: SharedRegion_getHeap(%d) failed\n",
            ServiceMgr_config.sharedRegionId);
        return;
    }

    ServiceMgr_allocateResources();

    ServiceMgr_module.masterMQ = MessageQ_getQueueId(ServiceMgr_module.routerMQ);

    for (i = 0; i < MultiProc_getNumProcessors(); i++) {
        ServiceMgr_module.replyMQ[i] = MessageQ_INVALIDMESSAGEQ;
    }

    /* Create the transfer agent thread */
    if (pthread_create((pthread_t *)&(ServiceMgr_module.transferAgentTid),
                       NULL, ServiceMgr_transferAgentFxn, NULL)) {
        printf("Transfer Agent failed to create errno = %d\n", errno);
    }
    /* Create the rxThread thread */
    if (pthread_create((pthread_t *)&(ServiceMgr_module.rxThreadTid),
                       NULL, ServiceMgr_rxThreadFxn, NULL)) {
        printf("rxThread failed to create errno = %d\n", errno);
    }
}

/*
 *  ======== ServiceMgr_stop ========
 */
Void ServiceMgr_stop()
{
    Int status;

    /*
     *  Delete this first to prevent race conditions if the slaves are
     *  not restarted.
     */
    MessageQ_delete((MessageQ_Handle *)&(ServiceMgr_module.startedMQ));

    /*
     *  Send a message to the xferAgent to gracefully terminate
     *  and then join the thread. Since the msg was not allocated
     *  via MessageQ_alloc, the staticMsgInit must be called on it.
     */

    status = MessageQ_put(ServiceMgr_module.masterMQ, ServiceMgr_module.stopMsg);
    if (status < 0) {
        printf("ServiceMgr_stop: MessageQ_put failed: status = 0x%x\n", status);
    }

    /* Waits for the thread to terminate */
    pthread_join((pthread_t)(ServiceMgr_module.transferAgentTid), NULL);

    /*
     * Shutdown the rxThread's socket to gracefully terminate
     * and then join the thread.
     */
    ServiceMgr_module.shutdown = TRUE;

    pthread_join((pthread_t)(ServiceMgr_module.rxThreadTid), NULL);

    ServiceMgr_cleanupResources();

    ServiceMgr_module.started = FALSE;
}

/*
 *  ======== ServiceMgr_stopXferAgent ========
 *  Communicate a stop to all remote processors. Free all the
 *  outstanding messages, delete the transfer agent's message queue
 *  and finally terminate the thread.
 */
Void ServiceMgr_stopXferAgent(Ptr msg)
{
    Int status;
    UInt16 procId;

    /* Loop through each processor */
    for (procId = 0; procId < MultiProc_getNumProcessors(); procId++) {

        /* Skip processors that are not registered */
        if ((UInt16)(ServiceMgr_module.replyMQ[procId]) != (UInt16)MessageQ_INVALIDMESSAGEQ) {

            /* Send a stop message */
            MessageQ_setMsgId(msg, IpcTypes_Action_STOP);
            MessageQ_setReplyQueue(ServiceMgr_module.routerMQ, msg);
            status = MessageQ_put(ServiceMgr_module.replyMQ[procId], msg);
            if (status < 0) {
                printf("ServiceMgr_stopXferAgent: MessageQ_put failed: status = 0x%x\n", status);
            }

            /*
             *  Free all the messages except for the STOPACK. That message will
             *  be used to send to the next processor (or freed outside the loop).
             */
            while (TRUE) {

                status = MessageQ_get(ServiceMgr_module.routerMQ,
                                      (MessageQ_Msg *)&msg, MessageQ_FOREVER);
                if (status < 0) {
                    printf("ServiceMgr_stopXferAgent: MessageQ_get failed: status = 0x%x\n", status);
                }

                if (MessageQ_getMsgId(msg) == IpcTypes_Action_STOPACK) {
                    break;
                }

                status = MessageQ_put(MessageQ_getReplyQueue(msg), msg);
                if (status < 0) {
                    printf("ServiceMgr_stopXferAgent: MessageQ_put failed: status = 0x%x\n", status);
                }
            }
        }
    }

    close(ServiceMgr_module.eventSocket);
    pthread_exit(NULL);
}

/*
 *  ======== ServiceMgr_transferAgentFxn ========
 */
Ptr ServiceMgr_transferAgentFxn(Ptr arg)
{
    Int status;
    MessageQ_Msg msg;

    /* Set up the eventSocket */
    do {
        ServiceMgr_module.eventSocket = socket(AF_INET, SOCK_DGRAM, 0);
    }
    while (ServiceMgr_module.eventSocket == -1);

    /* Initialize the socket addresses */
    memset((char *)&ServiceMgr_module.eventSin, 0, sizeof(struct sockaddr_in));
    ServiceMgr_module.eventSin.sin_family      = AF_INET;
    ServiceMgr_module.eventSin.sin_port        = htons(ServiceMgr_UDPPORT);
    ServiceMgr_module.eventSin.sin_addr.s_addr = htonl(INADDR_ANY);

    /*
     *  Handle incoming messages from the instrumentation host (via
     *  the rxThread), the slave cores or the stop function.
     */
    while (TRUE) {

        /* Get the message */
        status = MessageQ_get(ServiceMgr_module.routerMQ, &msg, MessageQ_FOREVER);
        if (status >= 0) {
            ServiceMgr_handleMsg(msg);
        }
        else {
            printf("ServiceMgr_transferAgentFxn: MessageQ_get failed: status = 0x%x\n", status);
        }
    }

    return (NULL);
}
