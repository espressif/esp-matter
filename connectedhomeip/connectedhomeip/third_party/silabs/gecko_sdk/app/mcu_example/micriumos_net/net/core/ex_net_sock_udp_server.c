/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/*
*********************************************************************************************************
*
*                                       NETWORK CORE EXAMPLE
*                                        SOCKET UDP SERVER
*
* File : ex_net_socket_udp_server.c
* Note(s) : (1) This example shows how to create an UDP server with IPv4 and IPv6.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <net_cfg.h>

#include  <net/include/net_sock.h>
#include  <net/include/net_app.h>
#include  <net/include/net_ipv6.h>

#include  <cpu/include/cpu.h>
#include  <kernel/include/os.h>
#include  <common/include/rtos_err.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  UDP_SERVER_PORT  10001
#define  RX_BUF_SIZE         15


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        Ex_Net_SockUDP_ServerIPv4()
*
* Description : UDP Echo server:
*
*                   (a) Open a socket.
*                   (b) Configure socket's address.
*                   (c) Bind that socket.
*                   (d) Receive data on the socket.
*                   (e) Transmit to source the data received.
*                   (f) Close socket on fatal fault error.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/
#if (NET_IPv4_CFG_EN == DEF_ENABLED)
void  Ex_Net_SockUDP_ServerIPv4 (void)
{
    NET_SOCK_ID          sock;
    NET_SOCK_ADDR_IPv4   server_sock_addr_ip;
    NET_SOCK_ADDR_IPv4   client_sock_addr_ip;
    NET_SOCK_ADDR_LEN    client_sock_addr_ip_size;
    NET_SOCK_RTN_CODE    rx_size;
    NET_SOCK_RTN_CODE    tx_size;
    NET_SOCK_DATA_SIZE   tx_rem;
    CPU_CHAR             rx_buf[RX_BUF_SIZE];
    CPU_INT32U           addr_any = NET_IPv4_ADDR_ANY;
    CPU_INT08U          *p_buf;
    CPU_BOOLEAN          fault_err;
    RTOS_ERR             err;


                                                                /* ----------------- OPEN IPV4 SOCKET ----------------- */
    sock = NetSock_Open(NET_SOCK_PROTOCOL_FAMILY_IP_V4,
                        NET_SOCK_TYPE_DATAGRAM,
                        NET_SOCK_PROTOCOL_UDP,
                       &err);
    if (err.Code != RTOS_ERR_NONE) {
        return;
    }


                                                                /* ------------ CONFIGURE SOCKET'S ADDRESS ------------ */
    NetApp_SetSockAddr((NET_SOCK_ADDR *)&server_sock_addr_ip,
                                         NET_SOCK_ADDR_FAMILY_IP_V4,
                                         UDP_SERVER_PORT,
                       (CPU_INT08U    *)&addr_any,
                                         NET_IPv4_ADDR_SIZE,
                                        &err);
    if (err.Code != RTOS_ERR_NONE) {
        NetSock_Close(sock, &err);
        return;
    }

                                                                /* ------------------- BIND SOCKET -------------------- */
    NetSock_Bind(                  sock,
                 (NET_SOCK_ADDR *)&server_sock_addr_ip,
                                   NET_SOCK_ADDR_SIZE,
                                  &err);
    if (err.Code != RTOS_ERR_NONE) {
        NetSock_Close(sock, &err);
        return;
    }


    fault_err = DEF_NO;

    do {
                                                                /* ----- WAIT UNTIL RECEIVING DATA FROM A CLIENT ------ */
        client_sock_addr_ip_size = sizeof(client_sock_addr_ip);

        rx_size = NetSock_RxDataFrom(                  sock,
                                                       rx_buf,
                                                       RX_BUF_SIZE,
                                                       NET_SOCK_FLAG_NONE,
                                     (NET_SOCK_ADDR *)&client_sock_addr_ip,
                                                      &client_sock_addr_ip_size,
                                                       DEF_NULL,
                                                       DEF_NULL,
                                                       DEF_NULL,
                                                      &err);
        switch (err.Code) {
            case RTOS_ERR_NONE:
                 tx_rem = rx_size;
                 p_buf  = (CPU_INT08U *)rx_buf;
                                                                /* ----- TRANSMIT THE DATA RECEIVED TO THE CLIENT ----- */
                 do {
                     tx_size = NetSock_TxDataTo(                  sock,
                                                                  p_buf,
                                                                  tx_rem,
                                                                  NET_SOCK_FLAG_NONE,
                                                (NET_SOCK_ADDR *)&client_sock_addr_ip,
                                                                  client_sock_addr_ip_size,
                                                                 &err);
                     switch (err.Code) {
                         case RTOS_ERR_NONE:
                              tx_rem -=  tx_size;
                              p_buf   = (CPU_INT08U *)(p_buf + tx_size);
                              break;

                         case RTOS_ERR_POOL_EMPTY:
                         case RTOS_ERR_NET_IF_LINK_DOWN:
                         case RTOS_ERR_TIMEOUT:
                         case RTOS_ERR_WOULD_BLOCK:
                              OSTimeDlyHMSM(0, 0, 0, 5, OS_OPT_TIME_DLY, &err);
                              break;

                         default:
                              fault_err = DEF_YES;
                              break;
                     }

                 } while (tx_rem > 0);
                 break;


            case RTOS_ERR_WOULD_BLOCK:
            case RTOS_ERR_TIMEOUT:
                 break;


            default:
                fault_err = DEF_YES;
                break;
        }

    } while (fault_err == DEF_NO);


                                                                /* ------------- FATAL FAULT SOCKET ERROR ------------- */
    NetSock_Close(sock, &err);                                  /* This function should be reached only when a fatal ...*/
                                                                /* fault error has occurred.                            */
}
#endif



/*
*********************************************************************************************************
*                                        Ex_Net_SockUDP_ServerIPv6()
*
* Description : UDP Echo server:
*
*                   (a) Open a socket.
*                   (b) Configure socket's address.
*                   (c) Bind that socket.
*                   (d) Receive data on the socket.
*                   (e) Transmit to source the data received.
*                   (f) Close socket on fatal fault error.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/
#if (NET_IPv6_CFG_EN == DEF_ENABLED)
void  Ex_Net_SockUDP_ServerIPv6 (void)
{
    NET_SOCK_ID          sock;
    NET_SOCK_ADDR_IPv6   server_sock_addr_ip;
    NET_SOCK_ADDR_IPv6   client_sock_addr_ip;
    NET_SOCK_ADDR_LEN    client_sock_addr_ip_size;
    NET_SOCK_RTN_CODE    rx_size;
    NET_SOCK_RTN_CODE    tx_size;
    NET_SOCK_DATA_SIZE   tx_rem;
    CPU_CHAR             rx_buf[RX_BUF_SIZE];
    CPU_INT08U          *p_buf;
    CPU_BOOLEAN          fault_err;
    RTOS_ERR             err;



                                                                /* ----------------- OPEN IPV6 SOCKET ----------------- */
    sock = NetSock_Open(NET_SOCK_PROTOCOL_FAMILY_IP_V6,         /* IPv6 Socket family.                                  */
                        NET_SOCK_TYPE_DATAGRAM,                 /* Datagram socket.                                     */
                        NET_SOCK_PROTOCOL_UDP,                  /* UDP protocol.                                        */
                       &err);
    if (err.Code != RTOS_ERR_NONE) {
        return;
    }



                                                                /* ------------ CONFIGURE SOCKET'S ADDRESS ------------ */
                                                                /* Populate the NET_SOCK_ADDR_IP structure for the ...  */
                                                                /* server address and port, and convert it to ...       */
                                                                /* network order.                                       */
    NetApp_SetSockAddr((NET_SOCK_ADDR *)&server_sock_addr_ip,
                                         NET_SOCK_ADDR_FAMILY_IP_V6,
                                         UDP_SERVER_PORT,
                       (CPU_INT08U    *)&NetIPv6_AddrAny,
                                         NET_IPv6_ADDR_SIZE,
                                        &err);
    if (err.Code != RTOS_ERR_NONE) {
        NetSock_Close(sock, &err);
        return;
    }



                                                                /* ------------------- BIND SOCKET -------------------- */
    NetSock_Bind(                  sock,                        /* Bind the newly created socket to the address and ... */
                 (NET_SOCK_ADDR *)&server_sock_addr_ip,         /* port specified by server_sock_addr_ip.               */
                                   NET_SOCK_ADDR_SIZE,
                                  &err);
    if (err.Code != RTOS_ERR_NONE) {
        NetSock_Close(sock, &err);
        return;
    }


    fault_err = DEF_NO;

    do {
                                                                /* ----- WAIT UNTIL RECEIVING DATA FROM A CLIENT ------ */
        client_sock_addr_ip_size = sizeof(client_sock_addr_ip);
        rx_size = NetSock_RxDataFrom(                  sock,    /* Receive data from any host on port UDP_SERVER_PORT.  */
                                     (void           *)rx_buf,
                                                       RX_BUF_SIZE,
                                                       NET_SOCK_FLAG_NONE,
                                     (NET_SOCK_ADDR *)&client_sock_addr_ip,
                                                      &client_sock_addr_ip_size,
                                                       DEF_NULL,
                                                       DEF_NULL,
                                                       DEF_NULL,
                                                      &err);
        switch (err.Code) {
            case RTOS_ERR_NONE:
                 tx_rem =  rx_size;
                 p_buf  = (CPU_INT08U *)rx_buf;
                 do {
                                                                 /* ------- TRANSMIT RECEIVED DATA TO THE CLIENT ------- */
                                                                 /* Transmit data to IP address and port of the client.  */
                     tx_size = NetSock_TxDataTo(                  sock,
                                                (void           *)p_buf,
                                                                  tx_rem,
                                                                  NET_SOCK_FLAG_NONE,
                                                (NET_SOCK_ADDR *)&client_sock_addr_ip,
                                                                  client_sock_addr_ip_size,
                                                                 &err);
                     switch (err.Code) {
                         case RTOS_ERR_NONE:
                              tx_rem -=  tx_size;
                              p_buf   = (CPU_INT08U *)(p_buf + tx_size);
                              break;

                         case RTOS_ERR_POOL_EMPTY:
                         case RTOS_ERR_NET_IF_LINK_DOWN:
                         case RTOS_ERR_TIMEOUT:
                         case RTOS_ERR_WOULD_BLOCK:
                              OSTimeDlyHMSM(0, 0, 0, 5, OS_OPT_TIME_DLY, &err);
                              break;

                         default:
                              fault_err = DEF_YES;
                              break;
                     }

                 } while (tx_rem > 0);
                 break;

            case RTOS_ERR_WOULD_BLOCK:
            case RTOS_ERR_TIMEOUT:
                 break;

            default:
                fault_err = DEF_YES;
                break;
        }

    } while (fault_err == DEF_NO);


                                                                /* ------------- FATAL FAULT SOCKET ERROR ------------- */
    NetSock_Close(sock, &err);                                  /* This function should be reached only when a fatal ...*/
                                                                /* fault error has occurred.                            */
}
#endif

