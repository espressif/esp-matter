/***************************************************************************//**
 * @file
 * @brief Network Igmp Layer - (Internet Group Management Protocol)
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
 * @defgroup NET_CORE_IGMP IGMP API
 * @ingroup  NET_CORE
 * @brief    IGMP API
 *
 * @addtogroup NET_CORE_IGMP
 * @{
 ********************************************************************************************************
 * @note     (1) Internet Group Management Protocol ONLY required for network interfaces that require
 *               reception of IP class-D (multicast) packets (see RFC #1112, Section 3 'Levels of
 *               Conformance : Level 2').
 *             - (a) IGMP is NOT required for the transmission of IP class-D (multicast) packets
 *                   (see RFC #1112, Section 3 'Levels of Conformance : Level 1').
 *
 * @note     (2) Supports Internet Group Management Protocol version 1, as described in RFC #1112
 *               with the following restrictions/constraints :
 *             - (a) Only one socket may receive datagrams for a specific host group address & port
 *                   number at any given time.
 *             - (b) Since sockets do NOT automatically leave IGMP host groups when closed,
 *                   it is the application's responsibility to leave each host group once it is
 *                   no longer needed by the application.
 *             - (c) Transmission of IGMP Query Messages NOT currently supported.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_IGMP_H_
#define  _NET_IGMP_H_

#include  "net_cfg_net.h"

#ifdef   NET_IGMP_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//                                                                 ------------- GRP MEMBERSHIP FNCTS -------------
CPU_BOOLEAN NetIGMP_HostGrpJoin(NET_IF_NBR    if_nbr,
                                NET_IPv4_ADDR addr_grp,
                                RTOS_ERR      *p_err);

CPU_BOOLEAN NetIGMP_HostGrpLeave(NET_IF_NBR    if_nbr,
                                 NET_IPv4_ADDR addr_grp,
                                 RTOS_ERR      *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IGMP_MODULE_EN
#endif // _NET_IGMP_H_
