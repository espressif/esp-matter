/*******************************************************************************
* The confidential and proprietary information contained in this file may      *
* only be used by a person authorised under and to the extent permitted        *
* by a subsisting licensing agreement from ARM Limited or its affiliates.      *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.                   *
*       ALL RIGHTS RESERVED                                                    *
* This entire notice must be reproduced on all copies of this file             *
* and copies of this file may only be made by a person if such person is       *
* permitted to do so under the terms of a subsisting license agreement         *
* from ARM Limited or its affiliates.                                          *
*******************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include <stdint.h>

#include "test_pal_socket.h"
#include "test_pal_mem.h"

/* Sending timeout */
#define TIMEOUT_MS  20000

/******************************************************************************/
uint32_t Test_PalSocket(tp_socket *s, enum tp_sock_domain domain,
        enum tp_sock_type type, enum tp_sock_protocol protocol,
        const uint32_t recvTimeout_ms)
{
    const TickType_t xTimeout = TIMEOUT_MS/portTICK_PERIOD_MS;
    const TickType_t xReceiveTimeout = (recvTimeout_ms == MAX_DELAY) ?\
            portMAX_DELAY:(recvTimeout_ms/portTICK_PERIOD_MS);

    Socket_t sfd;
    BaseType_t xDomain;
    BaseType_t xType;
    BaseType_t xProtocol;

    if (s == NULL)
        return 1;

    /* initial the socket parameters */
    xDomain = (domain == TP_AF_INET) ? FREERTOS_AF_INET : 0;
    xType   = (type == TP_SOCK_DGRAM) ? FREERTOS_SOCK_DGRAM :
                        FREERTOS_SOCK_STREAM;
    xProtocol = (protocol == TP_IPPROTO_UDP) ? FREERTOS_IPPROTO_UDP :
                        FREERTOS_IPPROTO_TCP;

    /* Attempt to open the socket. */
    sfd = FreeRTOS_socket(xDomain, xType, xProtocol);

    /* Check if socket was created. */
    configASSERT(sfd != FREERTOS_INVALID_SOCKET);
    *s = (tp_socket)sfd;
    /* If FREERTOS_SO_RCVBUF or FREERTOS_SO_SNDBUF are to be used with
    FreeRTOS_setsockopt() to change the buffer sizes from their default
    then do it here!.  (see the FreeRTOS_setsockopt() documentation. */

    /* If ipconfigUSE_TCP_WIN is set to 1 and FREERTOS_SO_WIN_PROPERTIES is
    be used with FreeRTOS_setsockopt() to change the sliding window size
    from to its default then do it here! (see the FreeRTOS_setsockopt()
    documentation. */

    /* Set send and receive time outs. */
    FreeRTOS_setsockopt(sfd,
            0,
            FREERTOS_SO_RCVTIMEO,
            &xReceiveTimeout,
            sizeof(xReceiveTimeout));

    FreeRTOS_setsockopt(sfd,
            0,
            FREERTOS_SO_SNDTIMEO,
            &xTimeout,
            sizeof(xTimeout));
    return 0;
}

/******************************************************************************/
uint32_t Test_PalCloseSocket(tp_socket s)
{
    Socket_t sfd = (Socket_t)s;

    FreeRTOS_closesocket(sfd);
    return 0;
}

/******************************************************************************/
uint32_t Test_PalConnect(tp_socket s, const uint8_t *addr, uint32_t port)
{
    struct freertos_sockaddr xRemoteAddress;
    Socket_t sfd = (Socket_t)s;

    /* Set the IP address and port of the remote socket
    to which this client socket will transmit. */
    xRemoteAddress.sin_port = FreeRTOS_htons(port);
    xRemoteAddress.sin_addr = FreeRTOS_inet_addr((const char *)addr);

    /* Connect to the remote socket.  The socket has not previously been
    bound to a local port number so will get automatically bound to a
    local port inside the FreeRTOS_connect() function. */
    return FreeRTOS_connect(sfd, &xRemoteAddress,
                sizeof(xRemoteAddress));

}

/******************************************************************************/
uint32_t Test_PalBind(tp_socket s, uint32_t port)
{
    struct freertos_sockaddr xBindAddress;
    Socket_t sfd = (Socket_t)s;
    BaseType_t rc;
    /* Set the listening port  */
    xBindAddress.sin_port = FreeRTOS_htons((uint16_t)port);

    /* Bind the socket to the port number */
    rc = FreeRTOS_bind(sfd, &xBindAddress,
            sizeof(xBindAddress));
    return (rc == 0) ? 0 : 1;
}

/******************************************************************************/
uint32_t Test_PalListen(tp_socket s, uint32_t backlog)
{
    BaseType_t rc;
    Socket_t sfd = (Socket_t)s;

    rc = FreeRTOS_listen(sfd, backlog);
    return (rc == 0) ? 0 : 1;
}

/******************************************************************************/
uint32_t Test_PalAccept(tp_socket s, tp_socket *acptS, uint8_t *addr,
             uint32_t *port)
{
    struct freertos_sockaddr xSourceAddress;
    Socket_t sfd = (Socket_t)s;
    Socket_t xConnectedSfd;
    socklen_t xAddressLength = 0;


    xConnectedSfd = FreeRTOS_accept(sfd, &xSourceAddress, &xAddressLength);
    if ((xConnectedSfd == FREERTOS_INVALID_SOCKET) ||
        (xConnectedSfd == NULL)) {
        /* failed to accept new connection  */
        return 1;
    }
    *acptS = (tp_socket)xConnectedSfd;
    /* copy IP address converting it to a string. */
    FreeRTOS_inet_ntoa(xSourceAddress.sin_addr,
                addr);
    /* update the received port number */
    *port = FreeRTOS_ntohs(xSourceAddress.sin_port);
    return 0;
}

/******************************************************************************/
tp_socket Test_PalShutdown(tp_socket s)
{
    uint32_t rc = 0;
    Socket_t sfd = (Socket_t)s;

    /* shutdown active close before close */
    rc = FreeRTOS_shutdown(sfd, FREERTOS_SHUT_RDWR);
    return (rc == 0) ? 0 : 1;
}

/******************************************************************************/
uint32_t Test_PalSend(tp_socket s, const uint8_t *buf,
              size_t len)
{
    uint32_t byteSent = 0;
    Socket_t sfd = (Socket_t)s;
    /* Send message size */
    byteSent = (uint32_t)FreeRTOS_send(sfd, buf,
                        len, 0);
    return byteSent;
}

/******************************************************************************/
uint32_t Test_PalSendTo(tp_socket s, const uint8_t *buf,
            size_t len, const uint8_t *addr, uint32_t port)
{
    uint32_t byteSent = 0;
    Socket_t sfd = (Socket_t)s;
    /* Send message size */
    struct freertos_sockaddr xDestinationAddress;
    /* Fill in the destination address and port number */
    xDestinationAddress.sin_addr = FreeRTOS_inet_addr((const char *)addr);
    xDestinationAddress.sin_port = FreeRTOS_htons(port);
    /* Send the buffer with ulFlags set to 0, so the
    FREERTOS_ZERO_COPY bit is clear. */
    byteSent = FreeRTOS_sendto(sfd, buf, len,
                0, &xDestinationAddress,
                sizeof(xDestinationAddress));
    /* check that byte had been successfully queued for sending */
    return (byteSent == len) ? byteSent : 0;
}

/******************************************************************************/
uint32_t Test_PalRecvFrom(tp_socket s, const uint8_t *buf,
            size_t len, uint8_t *addr, uint32_t *port)
{
    struct freertos_sockaddr xSourceAddress;
    int32_t byteRecv;
    Socket_t sfd = (Socket_t)s;
    socklen_t xAddressLength = 0;

    /* Receive into the buffer with ulFlags set to 0,
    so the FREERTOS_ZERO_COPY bit is clear. */
    byteRecv = FreeRTOS_recvfrom(sfd, (void *)buf, len,
                    0, &xSourceAddress, &xAddressLength);

    if (byteRecv > 0) {
        /* Data was received from the socket.
        copy IP address converting it to a string. */
        FreeRTOS_inet_ntoa(xSourceAddress.sin_addr,
                    addr);
        /* update the received port number */
        *port = FreeRTOS_ntohs(xSourceAddress.sin_port);
        return byteRecv;
    }
    return 0;
}

/******************************************************************************/
uint32_t Test_PalRecv(tp_socket s, const uint8_t *buf,
              size_t len)
{
    int32_t byteRecv;
    Socket_t sfd = (Socket_t)s;

    /* Receive into the buffer with ulFlags set to 0,
    so the FREERTOS_ZERO_COPY bit is clear. */
    byteRecv = FreeRTOS_recv(sfd, (void *)buf, len,
                    0);
    /* in case of error return 0 */
    return (byteRecv > 0) ? byteRecv : 0;
}

/******************************************************************************/
uint32_t Test_PalHtonl(uint32_t val)
{
    return FreeRTOS_htonl(val);
}

/******************************************************************************/
uint16_t Test_PalHtons(uint16_t val)
{
    return FreeRTOS_htons(val);
}

/******************************************************************************/
uint32_t Test_PalNtohl(uint32_t val)
{
    return FreeRTOS_ntohl(val);
}

/******************************************************************************/
uint16_t Test_PalNtohs(uint16_t val)
{
    return FreeRTOS_ntohs(val);
}

