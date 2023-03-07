/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LWIPOPTS_H_
#define LWIPOPTS_H_

/* this file contains user overrides and must be provided by the application */
#include "user_lwipopts.h"

/* features */

#ifndef LWIP_IPV4
#define LWIP_IPV4 1
#endif
#ifndef LWIP_IPV6
#define LWIP_IPV6 1
#endif
#ifndef LWIP_DHCP
#define LWIP_DHCP 1
#endif
#ifndef LWIP_DNS
#define LWIP_DNS 1
#endif
#ifndef LWIP_NETIF_LOOPBACK
#define LWIP_NETIF_LOOPBACK 1
#endif

#if (LWIP_IPV6 == 1)
#define RTE_Network_IPv6 1
#endif

/* TCP */

#ifndef TCP_MSS
#define TCP_MSS 1476
#endif
#ifndef TCP_SND_BUF
#define TCP_SND_BUF (24 * 1024)
#endif
#ifndef TCP_WND
#define TCP_WND (24 * 1024)
#endif
#ifndef LWIP_WND_SCALE
#define LWIP_WND_SCALE 1
#endif
#ifndef TCP_RCV_SCALE
#define TCP_RCV_SCALE 1
#endif

/* memory */

#ifndef MEM_SIZE
#define MEM_SIZE (10 * 1024)
#endif
#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT 4U
#endif
#ifndef MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB 8
#endif
#ifndef MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB 32
#endif
#ifndef MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN 32
#endif
#ifndef MEMP_NUM_TCP_SEG
#define MEMP_NUM_TCP_SEG 255
#endif
#ifndef MEMP_NUM_ARP_QUEUE
#define MEMP_NUM_ARP_QUEUE 8
#endif
#ifndef MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN 32
#endif
#ifndef PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE (1536 + 128)
#endif
#ifndef MEMP_NUM_NETDB
#define MEMP_NUM_NETDB 4
#endif

/* threads */
#ifndef IOTSDK_LWIP_TCPIP_THREAD_PRIORITY
#define IOTSDK_LWIP_TCPIP_THREAD_PRIORITY osPriorityHigh
#endif

#ifdef IOTSDK_LWIP_LOW_INPUT_LATENCY
#if IOTSDK_LWIP_TCPIP_THREAD_PRIORITY > IOTSDK_LWIP_INPUT_TASK_THREAD_PRIORITY
#error "IOTSDK_LWIP_LOW_INPUT_LATENCY set but IOTSDK_LWIP_TCPIP_THREAD_PRIORITY higher \
than IOTSDK_LWIP_INPUT_TASK_THREAD_PRIORITY"
#endif
#endif

#ifndef TCPIP_THREAD_NAME
#define TCPIP_THREAD_NAME "lwIP"
#endif
#ifndef TCPIP_THREAD_STACKSIZE
#define TCPIP_THREAD_STACKSIZE (512 * 4)
#endif
#ifndef TCPIP_THREAD_PRIO
#define TCPIP_THREAD_PRIO IOTSDK_LWIP_TCPIP_THREAD_PRIORITY
#endif
#ifndef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE 16
#endif
#ifndef DEFAULT_RAW_RECVMBOX_SIZE
#define DEFAULT_RAW_RECVMBOX_SIZE 16
#endif
#ifndef DEFAULT_UDP_RECVMBOX_SIZE
#define DEFAULT_UDP_RECVMBOX_SIZE 16
#endif
#ifndef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE 16
#endif
#ifndef DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE 16
#endif

/* internal */

/* if you already have the errno symbol defined, override this in the user options */
#ifndef LWIP_PROVIDE_ERRNO
#define LWIP_PROVIDE_ERRNO
#elif (LWIP_PROVIDE_ERRNO == 0)
/* code doesn't check the value, we allow the user to override it by defining it to 0 */
#undef LWIP_PROVIDE_ERRNO
#endif

#define LWIP_PROVIDE_ERRNO

#ifndef LWIP_NETBUF_RECVINFO
#define LWIP_NETBUF_RECVINFO 1
#endif
#ifndef ARP_QUEUEING
#define ARP_QUEUEING 1
#endif

#if defined(NO_SYS) && (NO_SYS != 0)
#warning "Port requires an OS. Ignoring user override of NO_SYS."
#endif
#undef NO_SYS
#define NO_SYS 0

#ifndef SO_REUSE
#define SO_REUSE 1
#else
#warning "Port relies on this option. Do not override unless you have modified lwIP to accommodate this."
#endif

#if defined(LWIP_NETIF_STATUS_CALLBACK) && (LWIP_NETIF_STATUS_CALLBACK == 0)
#warning "LWIP_NETIF_STATUS_CALLBACK cannot be disabled as the port requires it, overriding user option"
#endif
#undef LWIP_NETIF_STATUS_CALLBACK
#define LWIP_NETIF_STATUS_CALLBACK 1

#if defined(LWIP_NETIF_LINK_CALLBACK) && (LWIP_NETIF_LINK_CALLBACK == 0)
#warning "LWIP_NETIF_STATUS_CALLBACK cannot be disabled as the port requires it, overriding user option"
#endif
#undef LWIP_NETIF_LINK_CALLBACK
#define LWIP_NETIF_LINK_CALLBACK 1

#endif /* LWIPOPTS_H_ */
