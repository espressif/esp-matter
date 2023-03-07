/** @file dhcp-priv.h
 *
 *  @brief DHCP PVT Header
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

#ifndef __DHCP_PRIV_H__
#define __DHCP_PRIV_H__

#include <wmlog.h>

#define dhcp_e(...) wmlog_e("dhcp", ##__VA_ARGS__)
#define dhcp_w(...) wmlog_w("dhcp", ##__VA_ARGS__)

#define dhcp_d(...)

#define SERVER_BUFFER_SIZE        1024
#define MAC_IP_CACHE_SIZE         8
#define SEND_RESPONSE(w, x, y, z) dhcp_send_response(w, x, y, z)

struct client_mac_cache
{
    uint8_t client_mac[6]; /* mac address of the connected device */
    uint32_t client_ip;    /* ip address of the connected device */
};

struct dhcp_server_data
{
    int sock;
    int count_clients; /* to keep count of cached devices */
    char msg[SERVER_BUFFER_SIZE];
    struct sockaddr_in saddr; /* dhcp server address */
    struct sockaddr_in baddr; /* broadcast address */
    struct client_mac_cache ip_mac_mapping[MAC_IP_CACHE_SIZE];
    uint32_t netmask;    /* network order */
    uint32_t my_ip;      /* network order */
    uint32_t client_ip;  /* last address that was requested, network
                          * order */
    uint32_t current_ip; /* keep track of assigned IP addresses */
};

int dhcp_server_init(void *intrfc_handle);
void dhcp_server(os_thread_arg_t data);
int dhcp_send_halt(void);
int dhcp_free_allocations(void);
int dhcp_create_and_bind_udp_socket(struct sockaddr_in *address, void *intrfc_handle);
int dhcp_send_response(int sock, struct sockaddr *addr, char *msg, int len);
#endif
