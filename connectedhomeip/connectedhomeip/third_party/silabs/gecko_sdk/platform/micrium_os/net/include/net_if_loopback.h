/***************************************************************************//**
 * @file
 * @brief Network Interface Layer - Loopback
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_IF_LOOPBACK_H_
#define  _NET_IF_LOOPBACK_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_if.h>
#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                          NETWORK LOOPBACK INTERFACE CONFIGURATION DATA TYPE
 *
 * @brief Net Loopback interface configuration
 *
 * @note    (1) The network loopback interface configuration data type is a specific instantiation of a
 *               network device configuration data type.  ALL specific network device configuration data
 *               types MUST be defined with ALL of the generic network device configuration data type's
 *               configuration parameters, synchronized in both the sequential order & data type of each
 *               parameter.
 *               @n
 *               Thus ANY modification to the sequential order or data types of generic configuration
 *               parameters MUST be appropriately synchronized between the generic network device
 *               configuration data type & the network loopback interface configuration data type.
 *               @n
 *               See also 'net_if.h  GENERIC NETWORK DEVICE CONFIGURATION DATA TYPE  Note #1'.
 *******************************************************************************************************/

//                                                                 --------------------- NET LOOPBACK IF CFG ----------------------
typedef  struct  net_if_cfg_loopback {
  //                                                               ---------------- GENERIC  LOOPBACK CFG MEMBERS -----------------
  /** Rx buf mem pool type :
     NET_IF_MEM_TYPE_MAIN        bufs alloc'd from main      mem
     NET_IF_MEM_TYPE_DEDICATED   bufs alloc'd from dedicated mem    */
  NET_IF_MEM_TYPE RxBufPoolType;
  NET_BUF_SIZE    RxBufLargeSize;                   ///< Size  of loopback IF large buf data areas     (in octets).
  NET_BUF_QTY     RxBufLargeNbr;                    ///< Nbr   of loopback IF large buf data areas.
  NET_BUF_SIZE    RxBufAlignOctets;                 ///< Align of loopback IF       buf data areas     (in octets).
  NET_BUF_SIZE    RxBufIxOffset;                    ///< Offset from base ix to rx data into data area (in octets).

  /** Tx buf mem pool type :
     NET_IF_MEM_TYPE_MAIN        bufs alloc'd from main      mem
     NET_IF_MEM_TYPE_DEDICATED   bufs alloc'd from dedicated mem    */
  NET_IF_MEM_TYPE   TxBufPoolType;
  NET_BUF_SIZE      TxBufLargeSize;                 ///< Size  of loopback IF large buf data areas     (in octets).
  NET_BUF_QTY       TxBufLargeNbr;                  ///< Nbr   of loopback IF large buf data areas.
  NET_BUF_SIZE      TxBufSmallSize;                 ///< Size  of loopback IF small buf data areas     (in octets).
  NET_BUF_QTY       TxBufSmallNbr;                  ///< Nbr   of loopback IF small buf data areas.
  NET_BUF_SIZE      TxBufAlignOctets;               ///< Align of loopback IF       buf data areas     (in octets).
  NET_BUF_SIZE      TxBufIxOffset;                  ///< Offset from base ix to tx data from data area (in octets).

  CPU_ADDR          MemAddr;                        ///< Base addr of (loopback IF's) dedicated mem, if avail.
  CPU_ADDR          MemSize;                        ///< Size      of (loopback IF's) dedicated mem, if avail.

  NET_DEV_CFG_FLAGS Flags;                          ///< Opt'l bit flags.

  //                                                               ---------------- SPECIFIC LOOPBACK CFG MEMBERS -----------------
} NET_IF_CFG_LOOPBACK;

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_IF_LOOPBACK_H_
