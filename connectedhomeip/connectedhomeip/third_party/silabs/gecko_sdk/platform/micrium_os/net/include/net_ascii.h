/***************************************************************************//**
 * @file
 * @brief Network Ascii Library
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
 * @defgroup NET_CORE_ASCII ASCII Functions API
 * @ingroup  NET_CORE
 * @brief    ASCII Functions API
 *
 * @addtogroup NET_CORE_ASCII
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_ASCII_H_
#define  _NET_ASCII_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_type.h>
#include  <net/include/net_ip.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_ASCII_CHAR_LEN_DOT                            1u
#define  NET_ASCII_CHAR_LEN_HYPHEN                         1u
#define  NET_ASCII_CHAR_LEN_COLON                          1u
#define  NET_ASCII_CHAR_LEN_NUL                            1u

/********************************************************************************************************
 *                                    NETWORK ASCII ADDRESS DEFINES
 *
 * Note(s) : (1) ONLY supports 48-bit Ethernet MAC addresses.
 *******************************************************************************************************/

#define  NET_ASCII_NBR_OCTET_ADDR_MAC                      6u   // See Note #1.
#define  NET_ASCII_CHAR_MAX_OCTET_ADDR_MAC                 2u

#define  NET_ASCII_LEN_MAX_ADDR_MAC                    ((NET_ASCII_NBR_OCTET_ADDR_MAC       * NET_ASCII_CHAR_MAX_OCTET_ADDR_MAC) \
                                                        + ((NET_ASCII_NBR_OCTET_ADDR_MAC - 1u) * NET_ASCII_CHAR_LEN_HYPHEN)      \
                                                        + NET_ASCII_CHAR_LEN_NUL)

//                                                                 IPv4 addresses.
#define  NET_ASCII_CHAR_MAX_OCTET_ADDR_IPv4                3u
#define  NET_ASCII_VAL_MAX_OCTET_ADDR_IPv4               255u

#define  NET_ASCII_NBR_OCTET_ADDR_IPv4           (sizeof(NET_IPv4_ADDR))

#define  NET_ASCII_LEN_MAX_ADDR_IPv4                   ((NET_ASCII_NBR_OCTET_ADDR_IPv4      * NET_ASCII_CHAR_MAX_OCTET_ADDR_IPv4) \
                                                        + ((NET_ASCII_NBR_OCTET_ADDR_IPv4 - 1) * NET_ASCII_CHAR_LEN_DOT)          \
                                                        + NET_ASCII_CHAR_LEN_NUL)

//                                                                 IPv6 addresses.
#define  NET_ASCII_CHAR_MAX_OCTET_ADDR_IPv6                2u
#define  NET_ASCII_CHAR_MAX_DIG_ADDR_IPv6               (NET_ASCII_CHAR_MAX_OCTET_ADDR_IPv6 * 2)
#define  NET_ASCII_CHAR_MIN_COLON_IPv6                     2u
#define  NET_ASCII_CHAR_MAX_COLON_IPv6                     7u
#define  NET_ASCII_VAL_MAX_OCTET_ADDR_IPv6               255u

#define  NET_ASCII_NBR_OCTET_ADDR_IPv6           (sizeof(NET_IPv6_ADDR))

#define  NET_ASCII_LEN_MAX_ADDR_IPv6                   (((NET_ASCII_CHAR_MAX_DIG_ADDR_IPv6 * 4)      * NET_ASCII_CHAR_MAX_OCTET_ADDR_IPv6) \
                                                        + (((NET_ASCII_CHAR_MAX_DIG_ADDR_IPv6 * 2) - 1) * NET_ASCII_CHAR_LEN_COLON)        \
                                                        + NET_ASCII_CHAR_LEN_NUL)

#ifdef   NET_IPv4_MODULE_EN
#undef   NET_ASCII_LEN_MAX_ADDR_IP
#define  NET_ASCII_LEN_MAX_ADDR_IP                        NET_ASCII_LEN_MAX_ADDR_IPv4
#endif

#ifdef   NET_IPv6_MODULE_EN
#undef   NET_ASCII_LEN_MAX_ADDR_IP
#define  NET_ASCII_LEN_MAX_ADDR_IP                        NET_ASCII_LEN_MAX_ADDR_IPv6
#endif

#define  NET_ASCII_LEN_MAX_PORT                           DEF_INT_16U_NBR_DIG_MAX

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void NetASCII_Str_to_MAC(CPU_CHAR   *p_addr_mac_ascii,
                         CPU_INT08U *p_addr_mac,
                         RTOS_ERR   *p_err);

void NetASCII_MAC_to_Str(CPU_INT08U  *p_addr_mac,
                         CPU_CHAR    *p_addr_mac_ascii,
                         CPU_BOOLEAN hex_lower_case,
                         CPU_BOOLEAN hex_colon_sep,
                         RTOS_ERR    *p_err);

NET_IP_ADDR_FAMILY NetASCII_Str_to_IP(CPU_CHAR   *p_addr_ip_ascii,
                                      void       *p_addr,
                                      CPU_INT08U addr_max_len,
                                      RTOS_ERR   *p_err);

NET_IPv4_ADDR NetASCII_Str_to_IPv4(CPU_CHAR *p_addr_ip_ascii,
                                   RTOS_ERR *p_err);

NET_IPv6_ADDR NetASCII_Str_to_IPv6(CPU_CHAR *p_addr_ip_ascii,
                                   RTOS_ERR *p_err);

void NetASCII_IPv4_to_Str(NET_IPv4_ADDR addr_ip,
                          CPU_CHAR      *p_addr_ip_ascii,
                          CPU_BOOLEAN   lead_zeros,
                          RTOS_ERR      *p_err);

void NetASCII_IPv6_to_Str(NET_IPv6_ADDR *p_addr_ip,
                          CPU_CHAR      *p_addr_ip_ascii,
                          CPU_BOOLEAN   hex_lower_case,
                          CPU_BOOLEAN   lead_zeros,
                          RTOS_ERR      *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                            MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_ASCII_H_
