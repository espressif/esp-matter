/***************************************************************************//**
 * @file
 * @brief LwIP DHCP server implementation
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
#include <string.h>
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/prot/dhcp.h"
#include "lwip/etharp.h"
#include "app_webpage.h"
#include "dhcp_server.h"

#if LWIP_UDP && LWIP_DHCP

/// LwIP pcb for dhcp server.
static struct udp_pcb * dhcp_pcb = 0;

#define DHCP_RESPONSE_DEFAULT_SIZE 1024
#define DHCPS_DBG 0

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68

/// Place to store mac addresses of dhcp clients.
static struct eth_addr saved_mac[DHCPS_MAX_CLIENT];

#define UDP_DATA_OFS    0
// DHCP message item offsets and length
#define DHCP_MSG_OFS (UDP_DATA_OFS)
  #define DHCP_OP_OFS (DHCP_MSG_OFS + 0)
  #define DHCP_HTYPE_OFS (DHCP_MSG_OFS + 1)
  #define DHCP_HLEN_OFS (DHCP_MSG_OFS + 2)
  #define DHCP_HOPS_OFS (DHCP_MSG_OFS + 3)
  #define DHCP_XID_OFS (DHCP_MSG_OFS + 4)
  #define DHCP_SECS_OFS (DHCP_MSG_OFS + 8)
  #define DHCP_FLAGS_OFS (DHCP_MSG_OFS + 10)
  #define DHCP_CIADDR_OFS (DHCP_MSG_OFS + 12)
  #define DHCP_YIADDR_OFS (DHCP_MSG_OFS + 16)
  #define DHCP_SIADDR_OFS (DHCP_MSG_OFS + 20)
  #define DHCP_GIADDR_OFS (DHCP_MSG_OFS + 24)
  #define DHCP_CHADDR_OFS (DHCP_MSG_OFS + 28)

#define DHCP_COOKIE_OFS (DHCP_MSG_OFS + DHCP_MSG_LEN)
#define UDP_DHCP_OPTIONS_OFS (DHCP_MSG_OFS + DHCP_MSG_LEN + 4)

#define LEASE_TIME_1DAY     (86400)

/***************************************************************************//**
 * Remove mac address from list of clients.
 *
 * @param mac MAC address to remove.
 ******************************************************************************/
void dhcpserver_remove_mac(struct eth_addr *mac)
{
  for (uint32_t i = 0; i < DHCPS_MAX_CLIENT; ++i) {
    if (saved_mac[i].addr[0] == mac->addr[0] && saved_mac[i].addr[1] == mac->addr[1]
        && saved_mac[i].addr[2] == mac->addr[2] && saved_mac[i].addr[3] == mac->addr[3]
        && saved_mac[i].addr[4] == mac->addr[4] && saved_mac[i].addr[5] == mac->addr[5]) {
      saved_mac[i].addr[0] = 0;
      saved_mac[i].addr[1] = 0;
      saved_mac[i].addr[2] = 0;
      saved_mac[i].addr[3] = 0;
      saved_mac[i].addr[4] = 0;
      saved_mac[i].addr[5] = 0;
    }
  }
}

/***************************************************************************//**
 * Save mac address in list of clients.
 ******************************************************************************/
static void dhcpserver_save_mac(struct eth_addr *mac)
{
  // Remove previous listing of MAC.
  dhcpserver_remove_mac(mac);

  for (uint8_t i = 0; i < DHCPS_MAX_CLIENT; ++i) {
    if (saved_mac[i].addr[0] == 0 && saved_mac[i].addr[1] == 0
        && saved_mac[i].addr[2] == 0 && saved_mac[i].addr[3] == 0
        && saved_mac[i].addr[4] == 0 && saved_mac[i].addr[5] == 0) {
      saved_mac[i].addr[0] = mac->addr[0];
      saved_mac[i].addr[1] = mac->addr[1];
      saved_mac[i].addr[2] = mac->addr[2];
      saved_mac[i].addr[3] = mac->addr[3];
      saved_mac[i].addr[4] = mac->addr[4];
      saved_mac[i].addr[5] = mac->addr[5];
      break;
    }
  }
}

/***************************************************************************//**
 * Get IP associated to MAC address.
 *
 * @param mac MAC address of client.
 * @returns IP address
 ******************************************************************************/
ip_addr_t dhcpserver_get_ip(struct eth_addr *mac)
{
  ip_addr_t offer_ip = { 0 };
  for (uint32_t i = 0; i < DHCPS_MAX_CLIENT; ++i) {
    if (saved_mac[i].addr[0] == mac->addr[0] && saved_mac[i].addr[1] == mac->addr[1]
        && saved_mac[i].addr[2] == mac->addr[2] && saved_mac[i].addr[3] == mac->addr[3]
        && saved_mac[i].addr[4] == mac->addr[4] && saved_mac[i].addr[5] == mac->addr[5]) {
      /* index is used to increment IP address. */
      offer_ip.addr = ((10 + i) << 24) + ap_ip_addr0 + (ap_ip_addr1 << 8) + (ap_ip_addr2 << 16);
      return offer_ip;
    }
  }
  return offer_ip;
}

/***************************************************************************//**
 * Get mac address in client list.
 *
 * @param client Number of client.
 * @param mac mac address result
 * @returns mac address (all zeros if none)
 ******************************************************************************/
void dhcpserver_get_mac(uint8_t client, struct eth_addr *mac)
{
  mac->addr[0] = saved_mac[client].addr[0];
  mac->addr[1] = saved_mac[client].addr[1];
  mac->addr[2] = saved_mac[client].addr[2];
  mac->addr[3] = saved_mac[client].addr[3];
  mac->addr[4] = saved_mac[client].addr[4];
  mac->addr[5] = saved_mac[client].addr[5];
}

/***************************************************************************//**
 * Clear stored mac addresses in client list.
 ******************************************************************************/
void dhcpserver_clear_stored_mac(void)
{
  //clear saved mac addresses
  for (uint32_t i = 0; i < DHCPS_MAX_CLIENT; ++i) {
    saved_mac[i].addr[0] = 0;
    saved_mac[i].addr[1] = 0;
    saved_mac[i].addr[2] = 0;
    saved_mac[i].addr[3] = 0;
    saved_mac[i].addr[4] = 0;
    saved_mac[i].addr[5] = 0;
  }
}

/***************************************************************************//**
 * Find a requested IP address in DHCP packet.
 *
 * @param pbuf_in The pbuf containing the a DHCP packet.
 * @param ipaddr IP address result.
 * @returns 1 if found, 0 if not found.
 ******************************************************************************/
static uint16_t dhcpserver_find_requested_ip(struct pbuf * pbuf_in, uint8_t * ipaddr)
{
  uint32_t index = UDP_DHCP_OPTIONS_OFS;
  uint8_t optionID = DHCP_OPTION_REQUESTED_IP;
  uint16_t found = 0;
  uint8_t val = 0;
  uint8_t size;

  while ((val != 0xff) && (index < pbuf_in->tot_len)) {
    val = pbuf_get_at(pbuf_in, index);
    if (val == optionID) {
      index++;
      size = pbuf_get_at(pbuf_in, index);
      index++;
      pbuf_copy_partial(pbuf_in, ipaddr, size, index);
      index += size;
      found = 1;
    }
    index++;
    index += ((pbuf_get_at(pbuf_in, index)) + 1);
  }

  return found;
}

/***************************************************************************//**
 * DHCP server main function.
 ******************************************************************************/
static void dhcpserver_fn(void *arg, struct udp_pcb *dhcp_pcb_recv,
                          struct pbuf *pbuf_in, const ip_addr_t *client_addr, uint16_t port)
{
  (void)arg;
  (void)dhcp_pcb_recv;
  (void)port;
  (void)client_addr;
  struct pbuf * pbuf_out = 0;
  uint32_t i;
  uint8_t flags = 0;
  struct eth_addr ethaddr;

  uint32_t options_offset = 0;
  uint32_t client_requested_addr = 0;
  uint8_t val = 0;
  ip_addr_t client_ip_addr;
  ip_addr_t r;

  if ((NULL == pbuf_in) || ((pbuf_in->tot_len) <= UDP_DHCP_OPTIONS_OFS)) {
    goto end_of_fcn;
  }

  pbuf_out = pbuf_alloc(PBUF_TRANSPORT, DHCP_RESPONSE_DEFAULT_SIZE, PBUF_POOL);

  if ((NULL == pbuf_out) || ((pbuf_out->tot_len) < UDP_DHCP_OPTIONS_OFS)) {
    goto end_of_fcn;
  }
  pbuf_copy(pbuf_out, pbuf_in);

#if DHCPS_DBG
  printf("mac ");
#endif
  // Read MAC address.
  for (i = 0; i < 6; ++i) {
    val = pbuf_get_at(pbuf_in, DHCP_CHADDR_OFS + i);
    ethaddr.addr[i] = val;
#if DHCPS_DBG
    printf("%X ", val);
#endif
  }
#if DHCPS_DBG
  printf("\r\n");
#endif
  // Register MAC address and get an IP address.
  dhcpserver_save_mac(&ethaddr);
  client_ip_addr = dhcpserver_get_ip(&ethaddr);
  if (0 == client_ip_addr.addr) {
    goto end_of_fcn;
  }
#if DHCPS_DBG
  printf("ip %d.%d.%d.%d\r\n", client_ip_addr.addr & 0xff, (client_ip_addr.addr >> 8) & 0xff, (client_ip_addr.addr >> 16) & 0xff, (client_ip_addr.addr >> 24) & 0xff);
#endif
  etharp_add_static_entry(&client_ip_addr, &ethaddr);   //add ARP table entry
  /* request type. */
  val = pbuf_get_at(pbuf_in, UDP_DHCP_OPTIONS_OFS + 2);

  switch (val) {
    case DHCP_DISCOVER:
#if DHCPS_DBG
      printf("DHCP Discover\r\n");
#endif
      pbuf_put_at(pbuf_out, DHCP_OP_OFS, DHCP_BOOTREPLY);
      pbuf_put_at(pbuf_out, DHCP_SECS_OFS, 0);
      pbuf_put_at(pbuf_out, DHCP_FLAGS_OFS, 0x0);       //unicast

      // set the client IP address.
      pbuf_put_at(pbuf_out, DHCP_YIADDR_OFS, client_ip_addr.addr & 0xff);
      pbuf_put_at(pbuf_out, DHCP_YIADDR_OFS + 1, (client_ip_addr.addr >> 8) & 0xff);
      pbuf_put_at(pbuf_out, DHCP_YIADDR_OFS + 2, (client_ip_addr.addr >> 16) & 0xff);
      pbuf_put_at(pbuf_out, DHCP_YIADDR_OFS + 3, (client_ip_addr.addr >> 24) & 0xff);
      // next server address.
      pbuf_put_at(pbuf_out, DHCP_SIADDR_OFS, 0);
      pbuf_put_at(pbuf_out, DHCP_SIADDR_OFS + 1, 0);
      pbuf_put_at(pbuf_out, DHCP_SIADDR_OFS + 2, 0);
      pbuf_put_at(pbuf_out, DHCP_SIADDR_OFS + 3, 0);
      // DHCP cookie
      pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS, (DHCP_MAGIC_COOKIE >> 24) & 0xff);
      pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS + 1, (DHCP_MAGIC_COOKIE >> 16) & 0xff);
      pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS + 2, (DHCP_MAGIC_COOKIE >> 8) & 0xff);
      pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS + 3, (DHCP_MAGIC_COOKIE) & 0xff);
      options_offset = UDP_DHCP_OPTIONS_OFS;
      // Response is a DHCP Offer Packet.
      pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_MESSAGE_TYPE);
      options_offset++;
      pbuf_put_at(pbuf_out, options_offset, 1);
      options_offset++;
      pbuf_put_at(pbuf_out, options_offset, DHCP_OFFER);
      options_offset++;
      // Subnet Mask
      pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_SUBNET_MASK);
      options_offset++;
      pbuf_put_at(pbuf_out, options_offset, 4);
      options_offset++;
      IP4_ADDR(&r, ap_netmask_addr0, ap_netmask_addr1, ap_netmask_addr2, ap_netmask_addr3);
      pbuf_put_at(pbuf_out, options_offset, r.addr & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 1, (r.addr >> 8) & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 2, (r.addr >> 16) & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 3, (r.addr >> 24) & 0xff);
      options_offset += 4;
      //Set router info.
      pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_ROUTER);
      options_offset++;
      pbuf_put_at(pbuf_out, options_offset, 4);
      options_offset++;
      IP4_ADDR(&r, ap_ip_addr0, ap_ip_addr1, ap_ip_addr2, ap_ip_addr3);
      pbuf_put_at(pbuf_out, options_offset, r.addr & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 1, (r.addr >> 8) & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 2, (r.addr >> 16) & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 3, (r.addr >> 24) & 0xff);
      options_offset += 4;
      // lease time.
      pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_LEASE_TIME);
      options_offset++;
      pbuf_put_at(pbuf_out, options_offset, 4);
      options_offset++;
      pbuf_put_at(pbuf_out, options_offset, (LEASE_TIME_1DAY >> 24) & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 1, (LEASE_TIME_1DAY >> 16) & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 2, (LEASE_TIME_1DAY >> 8) & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 3, (LEASE_TIME_1DAY) & 0xff);
      options_offset += 4;
      // server ID
      pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_SERVER_ID);
      options_offset++;
      pbuf_put_at(pbuf_out, options_offset, 4);
      options_offset++;
      IP4_ADDR(&r, ap_ip_addr0, ap_ip_addr1, ap_ip_addr2, ap_ip_addr3);
      pbuf_put_at(pbuf_out, options_offset, r.addr & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 1, (r.addr >> 8) & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 2, (r.addr >> 16) & 0xff);
      pbuf_put_at(pbuf_out, options_offset + 3, (r.addr >> 24) & 0xff);
      options_offset += 4;

      pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_END);
      options_offset++;

      pbuf_realloc(pbuf_out, options_offset);

      if (ERR_OK != udp_sendto(dhcp_pcb, pbuf_out, &client_ip_addr, DHCP_CLIENT_PORT)) {
        goto end_of_fcn;
      }

      break;

    case DHCP_REQUEST:
#if DHCPS_DBG
      printf("DHCP Request\r\n");
#endif
      pbuf_put_at(pbuf_out, DHCP_OP_OFS, DHCP_BOOTREPLY);
      pbuf_put_at(pbuf_out, DHCP_SECS_OFS, 0);

      // broadcast flag
      flags = 0;
      flags = pbuf_get_at(pbuf_in, DHCP_FLAGS_OFS);
      if (flags & 0x80) {
        flags = pbuf_get_at(pbuf_out, DHCP_FLAGS_OFS);
        flags |= 0x80;
        pbuf_put_at(pbuf_out, DHCP_FLAGS_OFS, flags);
      } else {
        flags = pbuf_get_at(pbuf_out, DHCP_FLAGS_OFS);
        flags &= ~0x80;
        pbuf_put_at(pbuf_out, DHCP_FLAGS_OFS, flags);
      }

      // Check requested IP address
      if (0 == dhcpserver_find_requested_ip(pbuf_in, (uint8_t *)&client_requested_addr)) {
        goto end_of_fcn;
      }

      if (client_requested_addr == client_ip_addr.addr) {
        // set client IP address.
        pbuf_put_at(pbuf_out, DHCP_YIADDR_OFS, client_ip_addr.addr & 0xff);
        pbuf_put_at(pbuf_out, DHCP_YIADDR_OFS + 1, (client_ip_addr.addr >> 8) & 0xff);
        pbuf_put_at(pbuf_out, DHCP_YIADDR_OFS + 2, (client_ip_addr.addr >> 16) & 0xff);
        pbuf_put_at(pbuf_out, DHCP_YIADDR_OFS + 3, (client_ip_addr.addr >> 24) & 0xff);

        // next server address.
        pbuf_put_at(pbuf_out, DHCP_SIADDR_OFS, 0);
        pbuf_put_at(pbuf_out, DHCP_SIADDR_OFS + 1, 0);
        pbuf_put_at(pbuf_out, DHCP_SIADDR_OFS + 2, 0);
        pbuf_put_at(pbuf_out, DHCP_SIADDR_OFS + 3, 0);

        pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS, (DHCP_MAGIC_COOKIE >> 24) & 0xff);
        pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS + 1, (DHCP_MAGIC_COOKIE >> 16) & 0xff);
        pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS + 2, (DHCP_MAGIC_COOKIE >> 8) & 0xff);
        pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS + 3, (DHCP_MAGIC_COOKIE) & 0xff);
        options_offset = UDP_DHCP_OPTIONS_OFS;

        pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_MESSAGE_TYPE);
        options_offset++;
        pbuf_put_at(pbuf_out, options_offset, 1);
        options_offset++;
        pbuf_put_at(pbuf_out, options_offset, DHCP_ACK);
        options_offset++;

        // Subnet Mask.
        pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_SUBNET_MASK);
        options_offset++;
        pbuf_put_at(pbuf_out, options_offset, 4);
        options_offset++;
        IP4_ADDR(&r, ap_netmask_addr0, ap_netmask_addr1, ap_netmask_addr2, ap_netmask_addr3);
        pbuf_put_at(pbuf_out, options_offset, r.addr & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 1, (r.addr >> 8) & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 2, (r.addr >> 16) & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 3, (r.addr >> 24) & 0xff);
        options_offset += 4;
        // Router
        pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_ROUTER);
        options_offset++;
        pbuf_put_at(pbuf_out, options_offset, 4);
        options_offset++;
        IP4_ADDR(&r, ap_ip_addr0, ap_ip_addr1, ap_ip_addr2, ap_ip_addr3);
        pbuf_put_at(pbuf_out, options_offset, r.addr & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 1, (r.addr >> 8) & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 2, (r.addr >> 16) & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 3, (r.addr >> 24) & 0xff);
        options_offset += 4;

        // lease time.
        pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_LEASE_TIME);
        options_offset++;
        pbuf_put_at(pbuf_out, options_offset, 4);
        options_offset++;
        pbuf_put_at(pbuf_out, options_offset, (LEASE_TIME_1DAY >> 24) & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 1, (LEASE_TIME_1DAY >> 16) & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 2, (LEASE_TIME_1DAY >> 8) & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 3, (LEASE_TIME_1DAY) & 0xff);
        options_offset += 4;

        // Server ID
        pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_SERVER_ID);
        options_offset++;
        pbuf_put_at(pbuf_out, options_offset, 4);
        options_offset++;
        IP4_ADDR(&r, ap_ip_addr0, ap_ip_addr1, ap_ip_addr2, ap_ip_addr3);
        pbuf_put_at(pbuf_out, options_offset, r.addr & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 1, (r.addr >> 8) & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 2, (r.addr >> 16) & 0xff);
        pbuf_put_at(pbuf_out, options_offset + 3, (r.addr >> 24) & 0xff);
        options_offset += 4;

        pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_END);
        options_offset++;
      } else {
        pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS, (DHCP_MAGIC_COOKIE >> 24) & 0xff);
        pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS + 1, (DHCP_MAGIC_COOKIE >> 16) & 0xff);
        pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS + 2, (DHCP_MAGIC_COOKIE >> 8) & 0xff);
        pbuf_put_at(pbuf_out, DHCP_COOKIE_OFS + 3, (DHCP_MAGIC_COOKIE) & 0xff);
        options_offset = UDP_DHCP_OPTIONS_OFS;

        pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_MESSAGE_TYPE);
        options_offset++;
        pbuf_put_at(pbuf_out, options_offset, 1);
        options_offset++;
        pbuf_put_at(pbuf_out, options_offset, DHCP_NAK);
        options_offset++;

        pbuf_put_at(pbuf_out, options_offset, DHCP_OPTION_END);
        options_offset++;
      }

      pbuf_realloc(pbuf_out, options_offset);

      if (ERR_OK != udp_sendto(dhcp_pcb, pbuf_out, &client_ip_addr, DHCP_CLIENT_PORT)) {
        goto end_of_fcn;
      }

      break;
    // do nothing if not defined above
    default:
      break;
  }

  end_of_fcn:
  if (pbuf_out) {
    pbuf_free(pbuf_out);
  }
  pbuf_free(pbuf_in);
}

/***************************************************************************//**
 * Start DHCP server callback.
 ******************************************************************************/
static void dhcpserver_start_prv(void * arg)
{
  (void)arg;
  //clear saved mac addresses
  for (uint32_t i = 0; i < DHCPS_MAX_CLIENT; ++i) {
    saved_mac[i].addr[0] = 0;
    saved_mac[i].addr[1] = 0;
    saved_mac[i].addr[2] = 0;
    saved_mac[i].addr[3] = 0;
    saved_mac[i].addr[4] = 0;
    saved_mac[i].addr[5] = 0;
  }

  if (NULL == dhcp_pcb) {
    dhcp_pcb = udp_new();
    if (NULL == dhcp_pcb) {
      return;
    }

    udp_bind(dhcp_pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
    udp_bind_netif(dhcp_pcb, &ap_netif);
    udp_recv(dhcp_pcb, dhcpserver_fn, 0);
  }
}

/***************************************************************************//**
 * Start DHCP server.
 ******************************************************************************/
void dhcpserver_start(void)
{
  tcpip_callback(dhcpserver_start_prv, 0);
}

/***************************************************************************//**
 * Stop DHCP server callback.
 ******************************************************************************/
static void dhcpserver_stop_prv(void * arg)
{
  (void)arg;
  if (dhcp_pcb != NULL) {
    dhcpserver_clear_stored_mac();
    udp_disconnect(dhcp_pcb);
    udp_remove(dhcp_pcb);
    dhcp_pcb = NULL;
  }
}

/***************************************************************************//**
 * Stop DHCP server.
 ******************************************************************************/
void dhcpserver_stop(void)
{
  tcpip_callback(dhcpserver_stop_prv, 0);
}
#endif
