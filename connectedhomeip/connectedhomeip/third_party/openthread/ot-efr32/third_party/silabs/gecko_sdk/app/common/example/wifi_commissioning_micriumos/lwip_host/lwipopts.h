/***************************************************************************//**
 * @file
 * @brief LwIP configuration header file.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define LWIP_DEBUG LWIP_DBG_OFF
#define TCP_DEBUG LWIP_DBG_OFF
#define DHCP_DEBUG LWIP_DBG_OFF
#define HTTPD_DEBUG LWIP_DBG_OFF

#define NO_SYS                  0

/* Memory options */
#define MEM_ALIGNMENT           4

/* the size of the heap memory.  */
#define MEM_SIZE                (20 * 1024)

/* the number of memp struct pbufs. */
#define MEMP_NUM_PBUF           10
/* the number of UDP protocol control blocks. One per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        6
/* the number of simultaneously active TCP connections. */
#define MEMP_NUM_TCP_PCB        10
/* the number of listening TCP connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 5
/* the number of simultaneously queued TCP segments. */
#define MEMP_NUM_TCP_SEG        16
/*  the number of simultaneously active timeouts. */
#define MEMP_NUM_SYS_TIMEOUT    10

// pbuf options
/* the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE          10

/* the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE       1582

/* TCP options  */
#define LWIP_TCP                1
#define TCP_TTL                 255

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP Maximum segment size. */
#define TCP_MSS                 (1500 - 40)

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             (8 * TCP_MSS)

/*  TCP sender buffer space (pbufs). This must be at least
   as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */
#define TCP_SND_QUEUELEN        (2 * TCP_SND_BUF / TCP_MSS)

/* TCP receive window. */
#define TCP_WND                 (8 * TCP_MSS)

/* ICMP options */
#define LWIP_ICMP                       1

/* DHCP options */
#define LWIP_DHCP               1
#define ETHARP_SUPPORT_STATIC_ENTRIES 1

/* UDP options */
#define LWIP_UDP                1
#define UDP_TTL                 255

/* Statistics options */
#define LWIP_STATS 0

/* Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_API                  1

// Checksum options

#define CHECKSUM_BY_HARDWARE 0

/* Generate checksums in software for outgoing IP packets.*/
#define CHECKSUM_GEN_IP                 1
/* Generate checksums in software for outgoing UDP packets.*/
#define CHECKSUM_GEN_UDP                1
/* Generate checksums in software for outgoing TCP packets.*/
#define CHECKSUM_GEN_TCP                1
/* Check checksums in software for incoming IP packets.*/
#define CHECKSUM_CHECK_IP               1
/* Check checksums in software for incoming UDP packets.*/
#define CHECKSUM_CHECK_UDP              1
/* Check checksums in software for incoming TCP packets.*/
#define CHECKSUM_CHECK_TCP              1
/* Check checksums by hardware for incoming ICMP packets.*/
#define CHECKSUM_GEN_ICMP               1

// Enable/disable Netconn API (require to use api_lib.c)
#define LWIP_NETCONN                    1

// Enable/disable Socket API (require to use sockets.c)
#define LWIP_SOCKET                     0

// httpd settings
/** Set this to 1 to include "fsdata_custom.c" instead of "fsdata.c" for the
 * file system (to prevent changing the file included in LwIP) */
#define HTTPD_USE_CUSTOM_FSDATA   0
#define HTTPD_FSDATA_FILE      "httpfsdata.h"
#define LWIP_HTTPD_CGI 1
#define LWIP_HTTPD_SSI 1
#define LWIP_HTTPD_SSI_INCLUDE_TAG 0
#define LWIP_HTTPD_MAX_TAG_NAME_LEN 16
/* LwIP Stack Parameters (modified compared to initialization value in opt.h) -*/
#define LWIP_HTTPD_DYNAMIC_HEADERS 1
#define LWIP_HTTPD_MAX_TAG_INSERT_LEN 4096

// OS related options
#define TCPIP_THREAD_NAME              "TCP/IP"
#define TCPIP_THREAD_STACKSIZE          1000
#define TCPIP_MBOX_SIZE                 10
#define DEFAULT_UDP_RECVMBOX_SIZE       10
#define DEFAULT_TCP_RECVMBOX_SIZE       10
#define DEFAULT_ACCEPTMBOX_SIZE         10
#define DEFAULT_THREAD_STACKSIZE        500
#define TCPIP_THREAD_PRIO               16u

#endif /* __LWIPOPTS_H__ */
