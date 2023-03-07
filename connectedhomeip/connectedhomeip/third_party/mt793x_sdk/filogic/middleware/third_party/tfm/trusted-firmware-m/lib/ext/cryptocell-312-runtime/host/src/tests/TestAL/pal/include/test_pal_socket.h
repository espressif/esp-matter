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

#ifndef TP_SOCKET_H_
#define TP_SOCKET_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DELAY 0xFFFFFFFF

typedef uint32_t tp_socket;

/** tp_sock_domain - communication domain
 *
 * @TP_AF_INET - internet IP protocol
 */
enum tp_sock_domain {
    TP_AF_INET  = 1
};


/** tp_sock_type - Socket types
 *
 * @TP_SOCK_DGRAM - datagram (conn.less) socket
 * @TP_SOCK_STREAM - stream (connection) socket
 */
enum tp_sock_type {
    TP_SOCK_DGRAM   = 1,
    TP_SOCK_STREAM  = 2
};

/** test_pal_sock_protocol - protocol types
 *
 * @TP_IPPROTO_TCP - TCP socket
 * @TP_IPPROTO_UDP - UDP socket
 */
enum tp_sock_protocol {
    TP_IPPROTO_TCP  = 1,
    TP_IPPROTO_UDP  = 2
};

/******************************************************************************/
/*
 * @brief This function creates an endpoint for communication.
 *
 * @param[in]
 * domain   - specifies a communication domain.
 * type     - indicated type, which specifies the communication semantics.
 * protocol - specifies a particular protocol to be used with the socket.
 * recvTimeout_ms - specifies receive timeout in milliseconds. MAX_DELAY is
 * blocking.
 *
 * @param[out]
 * s - Socket structure pointer.
 *
 * @return - 0 for success, 1 for failure.
 */
uint32_t Test_PalSocket(tp_socket *s, enum tp_sock_domain domain,
            enum tp_sock_type type, enum tp_sock_protocol protocol,
            const uint32_t recvTimeout_ms);

/******************************************************************************/
/*
 * @brief This function close an endpoint for communication
 *
 * @param[in]
 * s   - socket handler to be closed.
 *
 * @param[out]
 *
 * @return - 0 for success, 1 for failure.
 */
uint32_t Test_PalCloseSocket(tp_socket s);

/******************************************************************************/
/*
 * @brief This function initiate a connection on a socket
 *
 * @param[in]
 * s      - Socket handler.
 * addr   - destination IP address.
 * port   - destination port number.
 *
 * @param[out]
 *
 * @return - 0 for success, 1 for failure.
 */
uint32_t Test_PalConnect(tp_socket s, const uint8_t *addr, uint32_t port);

/******************************************************************************/
/*
 * @brief This function assigns the local  address to the socket
 *
 * @param[in]
 * s      - Socket handler.
 * port   - socket local port number
 *
 * @param[out]
 *
 * @return - 0 for success, 1 for failure.
 */
uint32_t Test_PalBind(tp_socket s, uint32_t port);

/******************************************************************************/
/*
 * @brief This function listen for incoming connection on socket
 *
 * @param[in]
 * s       - Socket handler.
 * backlog - puts a limit on the number of simultaneously connected clients
 *
 * @param[out]
 *
 * @return - 0 for success, 1 for failure.
 */
uint32_t Test_PalListen(tp_socket s, uint32_t backlog);

/******************************************************************************/
/*
 * @brief This function accept connection on a socket
 *
 * @param[in]
 * s      - the listening socket on which new connections are to be accepted.
 *
 * @param[out]
 * acptS  - handle of the accepted socket created
 * addr   - IP Addr of the socket from which a connection was accepted.
 * port   - port number of the socket from which a connection was accepted.
 *
 * @return - 0 for success, 1 for failure.
 */
uint32_t Test_PalAccept(tp_socket s, tp_socket *acptS, uint8_t *addr,
             uint32_t *port);

/******************************************************************************/
/*
 * @brief This function disable reads and writes on a connected TCP socket
 *
 * @param[in]
 * s      - Socket handler.
 *
 * @param[out]
 *
 * @return - 0 for success, 1 for failure.
 */
uint32_t Test_PalShutdown(tp_socket s);

/******************************************************************************/
/**
 * @brief This function sends messages via a specified socket.
 *
 * @param[in]
 * s   - active connected socket-descriptor.
 * buf - pointer to data buffer prepared by user.
 * len - buffer size.
 *
 * @param[out]
 *
 * @return - number of bytes sent for success, 0 for failure.
 */
uint32_t Test_PalSend(tp_socket s, const uint8_t *buf,
              size_t len);

/******************************************************************************/
/**
 * @brief This function sends messages via a specified socket.
 *
 * @param[in]
 * s   - active connected socket-descriptor.
 * buf - pointer to data buffer prepared by user.
 * len - buffer size.
 *
 * @param[out]
 *
 * @return - number of bytes sent for success, 0 for failure.
 */
uint32_t Test_PalSendTo(tp_socket s, const uint8_t *buf,
            size_t len, const uint8_t *addr, uint32_t port);

/******************************************************************************/
/**
 * @brief This function sends messages via a specified socket.
 *
 * @param[in]
 * s   - active socket-descriptor.
 * buf - pointer to data buffer prepared by user.
 * len - buffer size.
 *
 * @param[out]
 * addr   - received IP address.
 * port   - received port number.
 *
 * @return - number of bytes recv for success, 0 for failure.
 */
uint32_t Test_PalRecvFrom(tp_socket s, const uint8_t *buf,
            size_t len, uint8_t *addr, uint32_t *port);

/******************************************************************************/
/**
 * @brief This function sends messages via a specified socket.
 *
 * @param[in]
 * s   - active socket-descriptor.
 * buf - pointer to data buffer prepared by user.
 * len - buffer size.
 *
 * @param[out]
 *
 * @return - number of bytes recv for success, 0 for failure.
 */
uint32_t Test_PalRecv(tp_socket s, const uint8_t *buf,
              size_t len);

/******************************************************************************/
/**
 * @brief This function set the Byte Order and Endian of the host
 * to network long.
 *
 * @param[in]
 * val - value to convert.
 *
 * @param[out]
 *
 * @return - the converted value.
 */
uint32_t Test_PalHtonl(uint32_t val);

/******************************************************************************/
/**
 * @brief This function set the Byte Order and Endian of the host
 * to network short.
 *
 * @param[in]
 * val - value to convert.
 *
 * @param[out]
 *
 * @return - the converted value.
 */
uint16_t Test_PalHtons(uint16_t val);

/******************************************************************************/
/**
 * @brief This function set the Byte Order and Endian of the network
 * to host long.
 *
 * @param[in]
 * val - value to convert.
 *
 * @param[out]
 *
 * @return - the converted value.
 */
uint32_t Test_PalNtohl(uint32_t val);

/******************************************************************************/
/**
 * @brief This function set the Byte Order and Endian of the network
 * to host short.
 *
 * @param[in]
 * val - value to convert.
 *
 * @param[out]
 *
 * @return - the converted value.
 */
uint16_t Test_PalNtohs(uint16_t val);

#ifdef __cplusplus
}
#endif

#endif /* TP_SOCKET_H_ */
