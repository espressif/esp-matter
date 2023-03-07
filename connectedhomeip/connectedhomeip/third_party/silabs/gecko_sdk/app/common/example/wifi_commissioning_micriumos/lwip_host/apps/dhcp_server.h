/**************************************************************************//**
 * Copyright 2019, Silicon Laboratories Inc.
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
#ifndef DHCP_SERVER_H
#define DHCP_SERVER_H

#include "lwip/ip_addr.h"
#include "lwip/prot/ethernet.h"

#define DHCPS_MAX_CLIENT 8 /// Max number of dhcp clients.

#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************//**
 * Start DHCP server.
 ******************************************************************************/
void dhcpserver_start(void);

/***************************************************************************//**
 * Stop DHCP server.
 ******************************************************************************/
void dhcpserver_stop(void);
/***************************************************************************//**
 * Remove mac address from list of clients.
 *
 * @param mac MAC address to remove.
 ******************************************************************************/
void dhcpserver_remove_mac(struct eth_addr *mac);
/***************************************************************************//**
 * Get mac address in client list.
 *
 * @param client Number of client.
 * @param mac mac address result
 * @returns mac address (all zeros if none)
 ******************************************************************************/
void dhcpserver_get_mac(uint8_t client, struct eth_addr *mac);
/***************************************************************************//**
 * Get IP associated to MAC address.
 *
 * @param mac MAC address of client.
 * @returns IP address
 ******************************************************************************/
ip_addr_t dhcpserver_get_ip(struct eth_addr *mac);

/***************************************************************************//**
 * Clear stored mac addresses in client list.
 ******************************************************************************/
void dhcpserver_clear_stored_mac(void);
#ifdef __cplusplus
}
#endif
#endif
