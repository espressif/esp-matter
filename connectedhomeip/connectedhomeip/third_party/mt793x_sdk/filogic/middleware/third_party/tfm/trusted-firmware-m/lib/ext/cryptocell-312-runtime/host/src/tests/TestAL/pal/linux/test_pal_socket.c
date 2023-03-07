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
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>

#include "test_pal_socket.h"
#include "test_pal_mem.h"

/******************************************************************************/
uint32_t Test_PalSocket(tp_socket *s, enum tp_sock_domain domain,
        enum tp_sock_type type, enum tp_sock_protocol protocol,
        const uint32_t recvTimeout_ms)
{
    int sfd;
    int xDomain;
    int xType;
    int xProtocol;
    (void)recvTimeout_ms;

    if (s == NULL)
        return 1;

    /* initial the socket parameters */
    xDomain = (domain == TP_AF_INET) ? AF_INET : 0;
    xType   = (type == TP_SOCK_DGRAM) ? SOCK_DGRAM : SOCK_STREAM;
    xProtocol = (protocol == TP_IPPROTO_UDP) ? IPPROTO_UDP : IPPROTO_TCP;
    /* Attempt to open the socket. */
    sfd = socket(xDomain, xType, xProtocol);
    if (sfd == -1)
        return 1;
    *s = sfd;
    return 0;
}

/******************************************************************************/
uint32_t Test_PalCloseSocket(tp_socket s)
{
    int sfd = (int)s;

    close(sfd);
    return 0;
}

/******************************************************************************/
uint32_t Test_PalConnect(tp_socket s, const uint8_t *addr, uint32_t port)
{
    struct addrinfo hints;
    struct addrinfo *srv;
    int sfd = (int)s;
    char   portstr[10];
    int32_t rc;

    sprintf(portstr, "%d", port);
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    /* Set the IP address and port of the remote socket
    to which this client socket will transmit. */
    if (getaddrinfo((const char *)addr, portstr, &hints, &srv) != 0) {
        /* failed to initiate the address */
        return 1;
    }

    /* Connect to the remote socket.  The socket has not previously been
    bound to a local port number so will get automatically bound to a
    local port inside function. */
    rc = connect(sfd, srv->ai_addr, srv->ai_addrlen);
    return (rc == -1) ? 1 : 0;
}

/******************************************************************************/
uint32_t Test_PalBind(tp_socket s, uint32_t port)
{
    struct sockaddr_in saddr;
    int sfd = (int)s;

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    /* indicate to use the local ip addr */
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Set the listening port  */
    saddr.sin_port = htons(port);
    /* bind the local port & IP to the socket */
    if (bind(sfd, (struct sockaddr *) &saddr,
        sizeof(saddr)) < 0) {
        return 1;
    }
    return 0;
}

/******************************************************************************/
uint32_t Test_PalListen(tp_socket s, uint32_t backlog)
{
    int rc;
    int sockfd = (int)s;

    rc = listen(sockfd, (int)backlog);
    return (rc == 0) ? 0 : 1;
}

/******************************************************************************/
uint32_t Test_PalAccept(tp_socket s, tp_socket *acptS, uint8_t *addr,
             uint32_t *port)
{
    struct sockaddr_in si;
    int sfd = (int)s;
    int acceptedSocket;
    socklen_t silen = sizeof(si);
    char *addrstr;

    acceptedSocket = accept(sfd, (struct sockaddr *)&si, &silen);

    if (acceptedSocket > 0) {
        /* Data was received from the socket.
        copy IP address converting it to a string. */
        addrstr = inet_ntoa(si.sin_addr);
        if (addrstr == NULL)
            return 0;
        strcpy((char *)addr, addrstr);
        /* update the received port number */
        *port = ntohs(si.sin_port);
        *acptS = acceptedSocket;
        return 0;
    }
    return 1;
}

/******************************************************************************/
tp_socket Test_PalShutdown(tp_socket s)
{
    uint32_t rc = 0;
    /* shutdown active close before close */
    rc = shutdown(s, SHUT_RDWR);
    return (rc == 0) ? 0 : 1;
}

/******************************************************************************/
uint32_t Test_PalSend(tp_socket s, const uint8_t *buf,
            size_t len)
{
    uint32_t byteSent = 0;
    int sfd = (int)s;
    /* Send message size */
    byteSent = (uint32_t)send(sfd, buf,
                len, 0);
    return byteSent;
}

/******************************************************************************/
uint32_t Test_PalSendTo(tp_socket s, const uint8_t *buf,
            size_t len, const uint8_t *addr, uint32_t port)
{
    uint32_t byteSent = 0;
    int sfd = (int)s;
    /* Send message size */
    struct sockaddr_in xDestAddr;
    socklen_t xDestAddrLen = sizeof(xDestAddr);
    /* Fill in the destination address and port number */
    inet_aton((const char *)addr, &xDestAddr.sin_addr);
    xDestAddr.sin_port = htons(port);
    /* Send the buffer with ulFlags set to 0 */
    byteSent = sendto(sfd, buf, len,
                0, (const struct sockaddr *)(&xDestAddr),
                xDestAddrLen);
    /* check that byte had been successfully queued for sending */
    return (byteSent == len) ? byteSent : 0;
}

/******************************************************************************/
uint32_t Test_PalRecvFrom(tp_socket s, const uint8_t *buf,
            size_t len, uint8_t *addr, uint32_t *port)
{
    struct sockaddr_in si;
    int32_t byteRecv;
    int sfd = (int)s;
    socklen_t silen = sizeof(si);
    char *addrstr;

    /* Receive into the buffer with ulFlags set to 0,
    so the FREERTOS_ZERO_COPY bit is clear. */
    byteRecv = recvfrom(sfd, (void *)buf, len,
            0, (struct sockaddr *)&si, &silen);

    if (byteRecv > 0) {
        /* Data was received from the socket.
        copy IP address converting it to a string. */
        addrstr = inet_ntoa(si.sin_addr);
        if (addrstr == NULL)
            return 0;
        strcpy((char *)addr, addrstr);
        /* update the received port number */
        *port = ntohs(si.sin_port);
        return byteRecv;
    }
    return 0;
}

/******************************************************************************/
uint32_t Test_PalRecv(tp_socket s, const uint8_t *buf,
              size_t len)
{
    int32_t byteRecv;
    int sfd = (int)s;

    /* Receive into the buffer with ulFlags set to 0,
    so the FREERTOS_ZERO_COPY bit is clear. */
    byteRecv = recv(sfd, (void *)buf, len,
            0);
    /* in case of error return 0 */
    return (byteRecv > 0) ? byteRecv : 0;
}

/******************************************************************************/
uint32_t Test_PalHtonl(uint32_t val)
{
    return htonl(val);
}

/******************************************************************************/
uint16_t Test_PalHtons(uint16_t val)
{
    return htons(val);
}

/******************************************************************************/
uint32_t Test_PalNtohl(uint32_t val)
{
    return ntohl(val);
}

/******************************************************************************/
uint16_t Test_PalNtohs(uint16_t val)
{
    return ntohs(val);
}
