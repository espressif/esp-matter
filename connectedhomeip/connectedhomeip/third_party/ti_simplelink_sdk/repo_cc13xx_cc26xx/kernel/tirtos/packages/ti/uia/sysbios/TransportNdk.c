/*
 * Copyright (c) 2012-2018, Texas Instruments Incorporated
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
 *  ======== TransportNdk.c ========
 */

/* XDC include files */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/knl/Task.h>

/* UIA include files */
#include <ti/uia/runtime/UIAPacket.h>

#include <ti/uia/runtime/Transport.h>

/* NDK headers. */
#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/_stack.h>

UInt16 TransportNdk_tcpPort = 1234;
UInt16 TransportNdk_udpPort = 1235;

static SOCKET TransportNdk_clientSocket;
static SOCKET TransportNdk_msgSocket;
static SOCKET TransportNdk_eventSocket;
static struct sockaddr_in TransportNdk_eventSin;
static struct sockaddr_in TransportNdk_clientAddr;
Int TransportNdk_failedUDPPacket = 0;
Int TransportNdk_failedTCPPacket = 0;

/*
 *  ======== TransportNdk_init ========
 */
Void TransportNdk_init()
{
}

/*
 *  ======== TransportNdk_start ========
 */
Ptr TransportNdk_start(UIAPacket_HdrType hdrType)
{
    Int status;
    Ptr retVal;
    struct sockaddr_in msgSin;
    Int clientLen;
    Int one = 1;

    /* Open the session as required by the NDK */
    fdOpenSession(TaskSelf());

    /* If control, create a TCP socket. If event, create a UDP socket */
    if (hdrType == UIAPacket_HdrType_Msg) {

        /* Create socket for incoming connections */
        do {
            TransportNdk_msgSocket = socket(AF_INET, SOCK_STREAM,
                                            IPPROTO_TCP);
            if (TransportNdk_msgSocket == INVALID_SOCKET) {
                Task_yield();
            }
        }
        while (TransportNdk_msgSocket == INVALID_SOCKET);

        /*
         *  TCP server so makes sure to set the SO_REUSEPORT to insure bind
         *  works on a reconnect.
         */
        status = setsockopt(TransportNdk_msgSocket, SOL_SOCKET, SO_REUSEPORT,
                            &one, sizeof(one));
        if (status != 0) {
            System_printf("setsockopt failed: fdError() = %d\n", fdError());
            return (NULL);
        }

        /* Construct local address structure */
        memset((char *)&msgSin, 0, sizeof(struct sockaddr_in));
        msgSin.sin_family      = AF_INET;
        msgSin.sin_addr.s_addr = NDK_htonl(INADDR_ANY);
        msgSin.sin_port        = NDK_htons(TransportNdk_tcpPort);

        /* Bind to the local address */
        status = bind(TransportNdk_msgSocket, (struct sockaddr *)&msgSin,
                      sizeof(struct sockaddr_in));
        if (status != 0) {
            System_printf("bind failed: fdError = %d\n", fdError());
            return (NULL);
        }

        /* Mark the socket so it will listen for 1 incoming connection */
        status = listen(TransportNdk_msgSocket, 1);
        if (status != 0) {
            System_printf("listen failed: fdError() = %d\n", fdError());
            return (NULL);
        }

        /* Wait for a client to connect */
        clientLen = sizeof(TransportNdk_clientAddr);
        TransportNdk_clientSocket = accept(TransportNdk_msgSocket,
            (struct sockaddr *)&TransportNdk_clientAddr, &clientLen);

        retVal = (Ptr)TransportNdk_clientSocket;
    }
    else if (hdrType == UIAPacket_HdrType_EventPkt) {
        /* Set up the TransportNdk_eventSocket */
        do {
            TransportNdk_eventSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }
        while (TransportNdk_eventSocket == (SOCKET)-1);

        /* Construct local address structure */
        memset((char *)&TransportNdk_eventSin, 0, sizeof(struct sockaddr_in));

        TransportNdk_eventSin.sin_family = AF_INET;
        TransportNdk_eventSin.sin_port = NDK_htons(TransportNdk_udpPort);
        TransportNdk_eventSin.sin_addr.s_addr = NDK_htonl(INADDR_ANY);
        retVal = (Ptr)TransportNdk_eventSocket;
    }
    else {
       System_printf("Invalid type = %d\n", hdrType);
       retVal = NULL;
    }

    return (retVal);
}

/*
 *  ======== TransportNdk_recv ========
 */
SizeT TransportNdk_recv(Ptr handle, UIAPacket_Hdr **packet, SizeT size)
{
    Int status;

    /* Receive incoming messages */
    status = recv((SOCKET)handle, *packet, size, 0);
    if (status > 0) {
        /*
         *  Set the event socket's dest IP address to the same as the
         *  msg's socket. This allows changing the host to change IP address
         *  or move to a different machine.
         */
        TransportNdk_eventSin.sin_addr.s_addr = TransportNdk_clientAddr.sin_addr.s_addr;
    }
    else {
        /* Problem with the msg socket. Do not send the events. */
        TransportNdk_eventSin.sin_addr.s_addr = NDK_htonl(INADDR_ANY);
    }

    return (status);
}

/*
 *  ======== TransportNdk_send ========
 */
Bool TransportNdk_send(Ptr handle, UIAPacket_Hdr **packet)
{
    Int status;
    Bool successFlag = TRUE;

    /*
     *  Send the messages via the TCP socket.
     *  Send the events via the UDP socket if the address is set.
     */
    if (UIAPacket_getHdrType(*packet) == UIAPacket_HdrType_Msg) {
        status = send(TransportNdk_clientSocket, *packet,
                     UIAPacket_getMsgLength(*packet), 0);
        if (status <= 0) {
            /* Useful in debugging */
            TransportNdk_failedTCPPacket++;
            successFlag = FALSE;
        }
    }
    else if (((struct sockaddr_in *)&TransportNdk_eventSin)->sin_addr.s_addr
             != NDK_htonl(INADDR_ANY)) {
        status = sendto(TransportNdk_eventSocket, *packet,
                     UIAPacket_getEventLength(*packet), 0,
                     (struct sockaddr *)&TransportNdk_eventSin,
                     sizeof(struct sockaddr_in));
        if (status <= 0) {
            /* Useful in debugging */
            TransportNdk_failedUDPPacket++;
            successFlag = FALSE;
        }
    }

    return (successFlag);
}

/*
 *  ======== TransportNdk_stop ========
 */
Void TransportNdk_stop(Ptr handle)
{
    fdCloseSession(TaskSelf());
    fdClose(handle);
    if (handle == (Ptr)TransportNdk_clientSocket) {
        fdClose(TransportNdk_msgSocket);
    }
}

/*
 *  ======== TransportNdk_exit ========
 */
Void TransportNdk_exit()
{
}
