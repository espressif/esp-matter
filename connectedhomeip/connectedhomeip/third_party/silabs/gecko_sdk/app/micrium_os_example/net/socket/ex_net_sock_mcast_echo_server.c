/***************************************************************************//**
 * @file
 * @brief Network Core Example - Multicast Echo Server
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

#include  <cpu/include/cpu.h>
#include  <kernel/include/os.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

#include  <net/include/net_type.h>
#include  <net/include/net_if.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_util.h>
#include  <net/include/net_igmp.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      Ex_Net_SockMCastEchoServer()
 *
 * @brief  Create an UDP Multicast echo server using IPv4 string address.
 *
 * @param  p_name        String containing interface controller's name.
 *
 * @param  p_group_addr  String with IPv4 multicast address of group to use.
 *
 * @param  port          Network port number to use.
 *******************************************************************************************************/
void Ex_Net_SockMCastEchoServer(CPU_CHAR     *p_name,
                                CPU_CHAR     *p_group_addr,
                                NET_PORT_NBR port)
{
#if ((NET_IPv4_CFG_EN == DEF_ENABLED) \
  && (NET_MCAST_CFG_IPv4_RX_EN == DEF_ENABLED))
  NET_IF_NBR         if_nbr;
  NET_IPv4_ADDR      addr;
  NET_SOCK_ID        sock_id;
  NET_SOCK_ADDR_IPv4 addr_local;
  RTOS_ERR           err;

  //                                                               --- RECOVER INTERFACE NUMBER FROM INTEFACE NAME ----
  if_nbr = NetIF_NbrGetFromName(p_name);
  APP_RTOS_ASSERT_CRITICAL((if_nbr != NET_IF_NBR_NONE),; );

  addr = NetASCII_Str_to_IPv4(p_group_addr, &err);              // Convert Group IP address.

  NetIGMP_HostGrpJoin(if_nbr, addr, &err);                      // Join the multicast group.
  if (err.Code != RTOS_ERR_NONE) {
    return;
  }

  sock_id = NetSock_Open(NET_SOCK_PROTOCOL_FAMILY_IP_V4,        // Open an UDP socket.
                         NET_SOCK_TYPE_DATAGRAM,
                         NET_SOCK_PROTOCOL_UDP,
                         &err);
  if (err.Code != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Set Socket address structure.
  addr_local.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;           // IPv4.
  addr_local.Addr = NET_UTIL_HOST_TO_NET_32(NET_IPv4_ADDR_ANY); // Any IPv4 addresses.
  addr_local.Port = NET_UTIL_HOST_TO_NET_16(port);              // Multicast Port to listen on.

  NetSock_Bind(sock_id,                                         // Bind the socket.
               (NET_SOCK_ADDR *)&addr_local,
               sizeof(addr_local),
               &err);
  if (err.Code != RTOS_ERR_NONE) {
    goto exit_close;
  }

  while (1) {
    CPU_CHAR          *p_buf;
    CPU_CHAR          buf[1472u];
    NET_SOCK_ADDR     sock_addr;
    NET_SOCK_ADDR_LEN sock_addr_len = sizeof(sock_addr);
    NET_SOCK_RTN_CODE data_len;
    NET_SOCK_RTN_CODE tx_size;

    p_buf = &buf[0];

    data_len = NetSock_RxDataFrom(sock_id,                      // Receive Data from the multicast group.
                                  p_buf,
                                  1472u,
                                  NET_SOCK_FLAG_NONE,
                                  &sock_addr,
                                  &sock_addr_len,
                                  DEF_NULL,
                                  DEF_NULL,
                                  DEF_NULL,
                                  &err);
    switch (err.Code) {
      case RTOS_ERR_NONE:
        do {
          tx_size = NetSock_TxDataTo(sock_id,                   // Transmit received data to the sender.
                                     p_buf,
                                     data_len,
                                     NET_SOCK_FLAG_NONE,
                                     &sock_addr,
                                     sock_addr_len,
                                     &err);
          switch (err.Code) {
            case RTOS_ERR_NONE:
              data_len -= tx_size;
              p_buf = p_buf + tx_size;
              break;

            case RTOS_ERR_POOL_EMPTY:
            case RTOS_ERR_NET_IF_LINK_DOWN:
            case RTOS_ERR_TIMEOUT:
            case RTOS_ERR_WOULD_BLOCK:
              OSTimeDlyHMSM(0, 0, 0, 5, OS_OPT_TIME_DLY, &err);
              break;

            default:
              goto exit_close;
          }
        } while (data_len > 0);
        break;

      case RTOS_ERR_WOULD_BLOCK:
      case RTOS_ERR_TIMEOUT:
        OSTimeDlyHMSM(0, 0, 0, 5, OS_OPT_TIME_DLY, &err);
        break;

      default:
        goto exit_close;
    }
  }

exit_close:
  NetSock_Close(sock_id, &err);                                 // Close the socket.

#else
  PP_UNUSED_PARAM(p_name);
  PP_UNUSED_PARAM(p_group_addr);
  PP_UNUSED_PARAM(port);
  APP_RTOS_ASSERT_CRITICAL_FAIL(RTOS_ERR_NOT_AVAIL);
#endif
}
