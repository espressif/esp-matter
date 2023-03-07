/***************************************************************************//**
 * @file
 * @brief Network Utility Library
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @defgroup NET_CORE_UTIL General Network Utilities API
 * @ingroup  NET_CORE
 * @brief    General Network Utilities API
 *
 * @addtogroup NET_CORE_UTIL
 * @{
 ********************************************************************************************************
 * @note     (1) NO compiler-supplied standard library functions are used by the network protocol suite.
 *               'net_util.*' implements ALL network-specific library functions.
 *               See also 'net.h  NETWORK INCLUDE FILES  Note #3'.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_UTIL_H_
#define  _NET_UTIL_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_type.h>

#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                              MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                           NETWORK WORD ORDER - TO - CPU WORD ORDER MACRO'S
 *
 * Description : Convert data values to & from network word order to host CPU word order.
 *
 * Argument(s) : val       Data value to convert (see Note #1).
 *
 * Return(s)   : Converted data value (see Note #1).
 *
 * Note(s)     : (1) 'val' data value to convert & any variable to receive the returned conversion MUST
 *                   start on appropriate CPU word-aligned addresses.  This is required because most word-
 *                   aligned processors are more efficient & may even REQUIRE that multi-octet words start
 *                   on CPU word-aligned addresses.
 *
 *                   (a) For 16-bit word-aligned processors, this means that
 *
 *                           all 16- & 32-bit words MUST start on addresses that are multiples of 2 octets
 *
 *                   (b) For 32-bit word-aligned processors, this means that
 *
 *                           all 16-bit       words MUST start on addresses that are multiples of 2 octets
 *                           all 32-bit       words MUST start on addresses that are multiples of 4 octets
 *
 *                   See also 'lib_mem.h  MEMORY DATA VALUE MACRO'S  Note #1a'
 *                          & 'lib_mem.h  ENDIAN WORD ORDER MACRO'S  Note #2'.
 *******************************************************************************************************/

#define  NET_UTIL_NET_TO_HOST_32(val)                   MEM_VAL_BIG_TO_HOST_32(val)
#define  NET_UTIL_NET_TO_HOST_16(val)                   MEM_VAL_BIG_TO_HOST_16(val)

#define  NET_UTIL_HOST_TO_NET_32(val)                   MEM_VAL_HOST_TO_BIG_32(val)
#define  NET_UTIL_HOST_TO_NET_16(val)                   MEM_VAL_HOST_TO_BIG_16(val)

#define  NET_UTIL_VAL_SWAP_ORDER_16(val)                MEM_VAL_BIG_TO_LITTLE_16(val)
#define  NET_UTIL_VAL_SWAP_ORDER_32(val)                MEM_VAL_BIG_TO_LITTLE_32(val)

/********************************************************************************************************
 *                                     NETWORK DATA VALUE MACRO'S
 *
 * Description : Encode/decode data values to & from any CPU memory addresses.
 *
 * Argument(s) : various.
 *
 * Return(s)   : various.
 *
 * Note(s)     : (1) Network data value macro's appropriately convert data words :
 *
 *                   (a) (1) From network  word order to host CPU word order
 *                       (2) From host CPU word order to network  word order
 *
 *                   (b) NO network-to-host word-order conversion performed
 *
 *               (2) (a) Some network data values MUST start on appropriate CPU word-aligned addresses :
 *
 *                       (1) Data values
 *                       (2) Variables to receive data values
 *
 *                   (b) Some network data values may start on any CPU address, word-aligned or not :
 *
 *                       (1) Addresses to         data values
 *                       (2) Addresses to receive data values
 *
 *                   See also 'lib_mem.h  MEMORY DATA VALUE MACRO'S  Note #1'.
 *******************************************************************************************************/
//                                                                 See Notes #1a1, #2a2, & #2b1.
#define  NET_UTIL_VAL_GET_NET_16(addr)                          MEM_VAL_GET_INT16U_BIG(addr)
#define  NET_UTIL_VAL_GET_NET_32(addr)                          MEM_VAL_GET_INT32U_BIG(addr)
//                                                                 See Notes #1a2, #2a1, & #2b2.
#define  NET_UTIL_VAL_SET_NET_16(addr, val)                     MEM_VAL_SET_INT16U_BIG(addr, val)
#define  NET_UTIL_VAL_SET_NET_32(addr, val)                     MEM_VAL_SET_INT32U_BIG(addr, val)

//                                                                 See Notes #1a1 & #2b.
#define  NET_UTIL_VAL_COPY_GET_NET_16(addr_dest, addr_src)      MEM_VAL_COPY_GET_INT16U_BIG(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_GET_NET_32(addr_dest, addr_src)      MEM_VAL_COPY_GET_INT32U_BIG(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_GET_NET(addr_dest, addr_src, size)   MEM_VAL_COPY_GET_INTU_BIG(addr_dest, addr_src, size)
//                                                                 See Notes #1a2 & #2b.
#define  NET_UTIL_VAL_COPY_SET_NET_16(addr_dest, addr_src)      MEM_VAL_COPY_SET_INT16U_BIG(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_SET_NET_32(addr_dest, addr_src)      MEM_VAL_COPY_SET_INT32U_BIG(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_SET_NET(addr_dest, addr_src, size)   MEM_VAL_COPY_SET_INTU_BIG(addr_dest, addr_src, size)

//                                                                 See Notes #1b, #2a2, & #2b1.
#define  NET_UTIL_VAL_GET_HOST_16(addr)                         MEM_VAL_GET_INT16U(addr)
#define  NET_UTIL_VAL_GET_HOST_32(addr)                         MEM_VAL_GET_INT32U(addr)
//                                                                 See Notes #1b, #2a1, & #2b2.
#define  NET_UTIL_VAL_SET_HOST_16(addr, val)                    MEM_VAL_SET_INT16U(addr, val)
#define  NET_UTIL_VAL_SET_HOST_32(addr, val)                    MEM_VAL_SET_INT32U(addr, val)

//                                                                 See Notes #1b & #2b.
#define  NET_UTIL_VAL_COPY_GET_HOST_16(addr_dest, addr_src)     MEM_VAL_COPY_GET_INT16U(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_GET_HOST_32(addr_dest, addr_src)     MEM_VAL_COPY_GET_INT32U(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_GET_HOST(addr_dest, addr_src, size)  MEM_VAL_COPY_GET_INTU(addr_dest, addr_src, size)
//                                                                 See Notes #1b & #2b.
#define  NET_UTIL_VAL_COPY_SET_HOST_16(addr_dest, addr_src)     MEM_VAL_COPY_SET_INT16U(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_SET_HOST_32(addr_dest, addr_src)     MEM_VAL_COPY_SET_INT32U(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_SET_HOST(addr_dest, addr_src, size)  MEM_VAL_COPY_SET_INTU(addr_dest, addr_src, size)

//                                                                 See Notes #1b & #2b.
#define  NET_UTIL_VAL_COPY_16(addr_dest, addr_src)              MEM_VAL_COPY_16(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_32(addr_dest, addr_src)              MEM_VAL_COPY_32(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY(addr_dest, addr_src, size)           MEM_VAL_COPY(addr_dest, addr_src, size)

#define  NET_UTIL_VAL_SWAP_ORDER(addr)                          MEM_VAL_BIG_TO_LITTLE_32(addr)

/********************************************************************************************************
 *                                    NETWORK IPv6 ADDRESS MACRO'S
 *
 * Description : Set and validate different type of IPv6 addresses.
 *
 * Argument(s) : various.
 *
 * Return(s)   : various.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  NET_UTIL_IPv6_ADDR_COPY(addr_src, addr_dest)           addr_dest.Addr[0] = addr_src.Addr[0]; \
  addr_dest.Addr[1] = addr_src.Addr[1];                                                               \
  addr_dest.Addr[2] = addr_src.Addr[2];                                                               \
  addr_dest.Addr[3] = addr_src.Addr[3];                                                               \
  addr_dest.Addr[4] = addr_src.Addr[4];                                                               \
  addr_dest.Addr[5] = addr_src.Addr[5];                                                               \
  addr_dest.Addr[6] = addr_src.Addr[6];                                                               \
  addr_dest.Addr[7] = addr_src.Addr[7];                                                               \
  addr_dest.Addr[8] = addr_src.Addr[8];                                                               \
  addr_dest.Addr[9] = addr_src.Addr[9];                                                               \
  addr_dest.Addr[10] = addr_src.Addr[10];                                                             \
  addr_dest.Addr[11] = addr_src.Addr[11];                                                             \
  addr_dest.Addr[12] = addr_src.Addr[12];                                                             \
  addr_dest.Addr[13] = addr_src.Addr[13];                                                             \
  addr_dest.Addr[14] = addr_src.Addr[14];                                                             \
  addr_dest.Addr[15] = addr_src.Addr[15];

#define  NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr)               addr.Addr[0] = DEF_BIT_NONE; \
  addr.Addr[1] = DEF_BIT_NONE;                                                               \
  addr.Addr[2] = DEF_BIT_NONE;                                                               \
  addr.Addr[3] = DEF_BIT_NONE;                                                               \
  addr.Addr[4] = DEF_BIT_NONE;                                                               \
  addr.Addr[5] = DEF_BIT_NONE;                                                               \
  addr.Addr[6] = DEF_BIT_NONE;                                                               \
  addr.Addr[7] = DEF_BIT_NONE;                                                               \
  addr.Addr[8] = DEF_BIT_NONE;                                                               \
  addr.Addr[9] = DEF_BIT_NONE;                                                               \
  addr.Addr[10] = DEF_BIT_NONE;                                                              \
  addr.Addr[11] = DEF_BIT_NONE;                                                              \
  addr.Addr[12] = DEF_BIT_NONE;                                                              \
  addr.Addr[13] = DEF_BIT_NONE;                                                              \
  addr.Addr[14] = DEF_BIT_NONE;                                                              \
  addr.Addr[15] = DEF_BIT_NONE;

#define  NET_UTIL_IPv6_ADDR_IS_UNSPECIFIED(addr)              ((addr.Addr[0] == DEF_BIT_NONE)     \
                                                               && (addr.Addr[1] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[2] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[3] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[4] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[5] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[6] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[7] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[8] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[9] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[10] == DEF_BIT_NONE) \
                                                               && (addr.Addr[11] == DEF_BIT_NONE) \
                                                               && (addr.Addr[12] == DEF_BIT_NONE) \
                                                               && (addr.Addr[13] == DEF_BIT_NONE) \
                                                               && (addr.Addr[14] == DEF_BIT_NONE) \
                                                               && (addr.Addr[15] == DEF_BIT_NONE))

#define  NET_UTIL_IPv6_ADDR_SET_LOOPBACK(addr)                  addr.Addr[0] = DEF_BIT_NONE; \
  addr.Addr[1] = DEF_BIT_NONE;                                                               \
  addr.Addr[2] = DEF_BIT_NONE;                                                               \
  addr.Addr[3] = DEF_BIT_NONE;                                                               \
  addr.Addr[4] = DEF_BIT_NONE;                                                               \
  addr.Addr[5] = DEF_BIT_NONE;                                                               \
  addr.Addr[6] = DEF_BIT_NONE;                                                               \
  addr.Addr[7] = DEF_BIT_NONE;                                                               \
  addr.Addr[8] = DEF_BIT_NONE;                                                               \
  addr.Addr[9] = DEF_BIT_NONE;                                                               \
  addr.Addr[10] = DEF_BIT_NONE;                                                              \
  addr.Addr[11] = DEF_BIT_NONE;                                                              \
  addr.Addr[12] = DEF_BIT_NONE;                                                              \
  addr.Addr[13] = DEF_BIT_NONE;                                                              \
  addr.Addr[14] = DEF_BIT_NONE;                                                              \
  addr.Addr[15] = DEF_BIT_00;

#define  NET_UTIL_IPv6_ADDR_IS_LOOPBACK(addr)                 ((addr.Addr[0] == DEF_BIT_NONE)     \
                                                               && (addr.Addr[1] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[2] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[3] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[4] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[5] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[6] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[7] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[8] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[9] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[10] == DEF_BIT_NONE) \
                                                               && (addr.Addr[11] == DEF_BIT_NONE) \
                                                               && (addr.Addr[12] == DEF_BIT_NONE) \
                                                               && (addr.Addr[13] == DEF_BIT_NONE) \
                                                               && (addr.Addr[14] == DEF_BIT_NONE) \
                                                               && (addr.Addr[15] == DEF_BIT_00))

#define  NET_UTIL_IPv6_ADDR_SET_MCAST_ALL_NODES(addr)           addr.Addr[0] = 0xFF; \
  addr.Addr[1] = 0x02;                                                               \
  addr.Addr[2] = DEF_BIT_NONE;                                                       \
  addr.Addr[3] = DEF_BIT_NONE;                                                       \
  addr.Addr[4] = DEF_BIT_NONE;                                                       \
  addr.Addr[5] = DEF_BIT_NONE;                                                       \
  addr.Addr[6] = DEF_BIT_NONE;                                                       \
  addr.Addr[7] = DEF_BIT_NONE;                                                       \
  addr.Addr[8] = DEF_BIT_NONE;                                                       \
  addr.Addr[9] = DEF_BIT_NONE;                                                       \
  addr.Addr[10] = DEF_BIT_NONE;                                                      \
  addr.Addr[11] = DEF_BIT_NONE;                                                      \
  addr.Addr[12] = DEF_BIT_NONE;                                                      \
  addr.Addr[13] = DEF_BIT_NONE;                                                      \
  addr.Addr[14] = DEF_BIT_NONE;                                                      \
  addr.Addr[15] = 0x01;

#define  NET_UTIL_IPv6_ADDR_IS_MCAST_ALL_NODES(addr)          ((addr.Addr[0] == 0xFF)             \
                                                               && (addr.Addr[1] == 0x02)          \
                                                               && (addr.Addr[2] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[3] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[4] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[5] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[6] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[7] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[8] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[9] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[10] == DEF_BIT_NONE) \
                                                               && (addr.Addr[11] == DEF_BIT_NONE) \
                                                               && (addr.Addr[12] == DEF_BIT_NONE) \
                                                               && (addr.Addr[13] == DEF_BIT_NONE) \
                                                               && (addr.Addr[14] == DEF_BIT_NONE) \
                                                               && (addr.Addr[15] == 0x01))

#define  NET_UTIL_IPv6_ADDR_SET_MCAST_ALL_ROUTERS(addr)         addr.Addr[0] = 0xFF; \
  addr.Addr[1] = 0x02;                                                               \
  addr.Addr[2] = DEF_BIT_NONE;                                                       \
  addr.Addr[3] = DEF_BIT_NONE;                                                       \
  addr.Addr[4] = DEF_BIT_NONE;                                                       \
  addr.Addr[5] = DEF_BIT_NONE;                                                       \
  addr.Addr[6] = DEF_BIT_NONE;                                                       \
  addr.Addr[7] = DEF_BIT_NONE;                                                       \
  addr.Addr[8] = DEF_BIT_NONE;                                                       \
  addr.Addr[9] = DEF_BIT_NONE;                                                       \
  addr.Addr[10] = DEF_BIT_NONE;                                                      \
  addr.Addr[11] = DEF_BIT_NONE;                                                      \
  addr.Addr[12] = DEF_BIT_NONE;                                                      \
  addr.Addr[13] = DEF_BIT_NONE;                                                      \
  addr.Addr[14] = DEF_BIT_NONE;                                                      \
  addr.Addr[15] = 0x02;

#define  NET_UTIL_IPv6_ADDR_IS_MCAST_ALL_ROUTERS(addr)        ((addr.Addr[0] == 0xFF)             \
                                                               && (addr.Addr[1] == 0x02)          \
                                                               && (addr.Addr[2] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[3] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[4] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[5] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[6] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[7] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[8] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[9] == DEF_BIT_NONE)  \
                                                               && (addr.Addr[10] == DEF_BIT_NONE) \
                                                               && (addr.Addr[11] == DEF_BIT_NONE) \
                                                               && (addr.Addr[12] == DEF_BIT_NONE) \
                                                               && (addr.Addr[13] == DEF_BIT_NONE) \
                                                               && (addr.Addr[14] == DEF_BIT_NONE) \
                                                               && (addr.Addr[15] == 0x02))

#define  NET_UTIL_IPv6_ADDR_IS_LINK_LOCAL(addr)               ((addr.Addr[0] == 0xFE) \
                                                               && (addr.Addr[1] == 0x80))

#define  NET_UTIL_IPv6_ADDR_IS_SITE_LOCAL(addr)               ((addr.Addr[0] == 0xFE) \
                                                               && (addr.Addr[1] == 0xC0))

#define  NET_UTIL_IPv6_ADDR_IS_MULTICAST(addr)                 (addr.Addr[0] == 0xFF)

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

NET_TS NetUtil_TS_Get(void);

NET_TS_MS NetUtil_TS_Get_ms(void);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_UTIL_H_
