/***************************************************************************//**
 * @file
 * @brief Network Core Example - Socket TCP Client
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net_cfg.h>

#include  <net/include/net_type.h>
#include  <net/include/net_ip.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_app.h>
#include  <net/include/net_ascii.h>

#include  <cpu/include/cpu.h>
#include  <kernel/include/os.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  TCP_SERVER_PORT  10001
#define  TX_BUF_SIZE         16

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         Ex_Net_SockTCP_Client()
 *
 * @brief  TCP Echo client:
 *           - (a) Open a socket.
 *           - (b) Configure socket's address.
 *           - (c) Connect socket.
 *           - (d) Transmit data to the server.
 *           - (e) Receive echo response from the server
 *           - (f) Close socket.
 *
 * @param  p_ip_addr  Pointer to a string that contains the IP address of the server.
 *
 * @return  DEF_OK,   No error, message sent and echo received.
 *          DEF_FAIL, Otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_Net_SockTCP_Client(CPU_CHAR *p_ip_addr)
{
#if (NET_IPv6_CFG_EN == DEF_ENABLED)
  NET_IPv6_ADDR server_addr;
#elif (NET_IPv4_CFG_EN == DEF_ENABLED)
  NET_IPv4_ADDR server_addr;
#endif
  NET_SOCK_ADDR            server_sock_addr;
  NET_SOCK_ADDR_LEN        server_sock_addr_len;
  NET_IP_ADDR_FAMILY       ip_family;
  NET_SOCK_PROTOCOL_FAMILY protocol_family;
  NET_SOCK_ADDR_FAMILY     sock_addr_family;
  NET_IP_ADDR_LEN          ip_addr_len;
  NET_SOCK_ID              sock;
  NET_SOCK_DATA_SIZE       tx_size;
  NET_SOCK_DATA_SIZE       tx_rem;
  CPU_CHAR                 tx_buf[] = "This is a TCP message";
  NET_SOCK_RTN_CODE        rx_size;
  NET_SOCK_DATA_SIZE       rx_rem;
  CPU_INT08U               *p_buf;
  RTOS_ERR                 err;

  //                                                               ---------------- CONVERT IP ADDRESS ----------------
  ip_family = NetASCII_Str_to_IP(p_ip_addr,
                                 &server_addr,
                                 sizeof(server_addr),
                                 &err);
  if (err.Code != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  switch (ip_family) {
    case NET_IP_ADDR_FAMILY_IPv4:
      sock_addr_family = NET_SOCK_ADDR_FAMILY_IP_V4;
      protocol_family = NET_SOCK_PROTOCOL_FAMILY_IP_V4;
      ip_addr_len = NET_IPv4_ADDR_SIZE;
      break;

    case NET_IP_ADDR_FAMILY_IPv6:
      sock_addr_family = NET_SOCK_ADDR_FAMILY_IP_V6;
      protocol_family = NET_SOCK_PROTOCOL_FAMILY_IP_V6;
      ip_addr_len = NET_IPv6_ADDR_SIZE;
      break;

    case NET_IP_ADDR_FAMILY_UNKNOWN:
    default:
      return (DEF_FAIL);
  }

  //                                                               ------------------- OPEN SOCKET --------------------
  sock = NetSock_Open(protocol_family,
                      NET_SOCK_TYPE_STREAM,
                      NET_SOCK_PROTOCOL_TCP,
                      &err);
  if (err.Code != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  //                                                               ------------ CONFIGURE SOCKET'S ADDRESS ------------
  NetApp_SetSockAddr(&server_sock_addr,
                     sock_addr_family,
                     TCP_SERVER_PORT,
                     (void *)&server_addr,
                     ip_addr_len,
                     &err);
  if (err.Code != RTOS_ERR_NONE) {
    NetSock_Close(sock, &err);
    return (DEF_FAIL);
  }

  //                                                               ------------------ CONNECT SOCKET ------------------
  NetSock_Conn(sock, &server_sock_addr, sizeof(server_sock_addr), &err);
  if (err.Code != RTOS_ERR_NONE) {
    NetSock_Close(sock, &err);
    return (DEF_FAIL);
  }

  p_buf = (CPU_INT08U *)tx_buf;
  tx_rem = sizeof(tx_buf);
  do {
    tx_size = NetSock_TxDataTo(sock,
                               (void *)p_buf,
                               tx_rem,
                               NET_SOCK_FLAG_NONE,
                               (NET_SOCK_ADDR *)&server_sock_addr,
                               NET_SOCK_ADDR_SIZE,
                               &err);
    switch (err.Code) {
      case RTOS_ERR_NONE:
        tx_rem -= tx_size;
        p_buf = (CPU_INT08U *)(p_buf + tx_size);
        break;

      case RTOS_ERR_POOL_EMPTY:                                     // Transitory Errors
      case RTOS_ERR_TIMEOUT:
      case RTOS_ERR_WOULD_BLOCK:
      case RTOS_ERR_NET_ADDR_UNRESOLVED:
      case RTOS_ERR_NET_IF_LINK_DOWN:
        OSTimeDlyHMSM(0, 0, 0, 5, OS_OPT_TIME_DLY, &err);
        break;

      default:
        NetSock_Close(sock, &err);
        return (DEF_FAIL);
    }
  } while (tx_rem > 0);

  rx_rem = TX_BUF_SIZE;

  //                                                               ----- WAIT UNTIL RECEIVING DATA FROM A CLIENT ------
  rx_rem = TX_BUF_SIZE;
  p_buf = (CPU_INT08U *)tx_buf;
  do {
    server_sock_addr_len = sizeof(server_sock_addr);
    rx_size = NetSock_RxDataFrom(sock,
                                 (void *)p_buf,
                                 rx_rem,
                                 NET_SOCK_FLAG_NONE,
                                 &server_sock_addr,
                                 &server_sock_addr_len,
                                 DEF_NULL,
                                 DEF_NULL,
                                 DEF_NULL,
                                 &err);
    switch (err.Code) {
      case RTOS_ERR_NONE:
        rx_rem -= rx_size;
        p_buf = (CPU_INT08U *)(p_buf + rx_size);
        break;

      case RTOS_ERR_WOULD_BLOCK:                                    // Transitory rx err(s).
      case RTOS_ERR_TIMEOUT:
        OSTimeDlyHMSM(0, 0, 0, 5, OS_OPT_TIME_DLY, &err);
        break;

      case RTOS_ERR_NET_CONN_CLOSED_FAULT:                          // Conn closed by peer.
      case RTOS_ERR_NET_CONN_CLOSE_RX:
        NetSock_Close(sock, &err);
        return (DEF_FAIL);

      default:
        NetSock_Close(sock, &err);
        return (DEF_FAIL);
    }
  } while (tx_rem > 0);

  //                                                               ------------------- CLOSE SOCKET -------------------
  NetSock_Close(sock, &err);

  return (DEF_OK);
}
