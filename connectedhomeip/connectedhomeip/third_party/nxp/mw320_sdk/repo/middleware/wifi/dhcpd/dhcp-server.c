/** @file dhcp-server.c
 *
 *  @brief This file provides the DHCP Server
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

/** dhcp-server.c: The DHCP Server
 */
#include <string.h>

#include <wm_os.h>
#include <wm_net.h>
#include <dhcp-server.h>
#include <wlan.h>

#include "dhcp-bootp.h"
#include "dns.h"
#include "dhcp-priv.h"

#define DEFAULT_DHCP_ADDRESS_TIMEOUT (24U * 60U * 60U * 1U) /* 1 day */
#define CLIENT_IP_NOT_FOUND          0x00000000

uint32_t dhcp_address_timeout = DEFAULT_DHCP_ADDRESS_TIMEOUT;
static os_mutex_t dhcpd_mutex;
static int ctrl = -1;
#define CTRL_PORT 12679
static char ctrl_msg[16];

struct dhcp_server_data dhcps;
static void get_broadcast_addr(struct sockaddr_in *addr);
static int get_ip_addr_from_interface(uint32_t *ip, void *interface_handle);
static int get_netmask_from_interface(uint32_t *nm, void *interface_handle);
static int send_gratuitous_arp(uint32_t ip);
static bool ac_add(uint8_t *chaddr, uint32_t client_ip);
static uint32_t ac_lookup_mac(uint8_t *chaddr);
static uint8_t *ac_lookup_ip(uint32_t client_ip);
static bool ac_not_full();

static bool ac_add(uint8_t *chaddr, uint32_t client_ip)
{
    /* adds ip-mac mapping in cache */
    if (ac_not_full())
    {
        dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[0] = chaddr[0];
        dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[1] = chaddr[1];
        dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[2] = chaddr[2];
        dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[3] = chaddr[3];
        dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[4] = chaddr[4];
        dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[5] = chaddr[5];
        dhcps.ip_mac_mapping[dhcps.count_clients].client_ip     = client_ip;
        dhcps.count_clients++;
        return WM_SUCCESS;
    }
    return -WM_FAIL;
}

static uint32_t ac_lookup_mac(uint8_t *chaddr)
{
    /* returns ip address, if mac address is present in cache */
    int i;
    for (i = 0; i < dhcps.count_clients && i < MAC_IP_CACHE_SIZE; i++)
    {
        if ((dhcps.ip_mac_mapping[i].client_mac[0] == chaddr[0]) &&
            (dhcps.ip_mac_mapping[i].client_mac[1] == chaddr[1]) &&
            (dhcps.ip_mac_mapping[i].client_mac[2] == chaddr[2]) &&
            (dhcps.ip_mac_mapping[i].client_mac[3] == chaddr[3]) &&
            (dhcps.ip_mac_mapping[i].client_mac[4] == chaddr[4]) &&
            (dhcps.ip_mac_mapping[i].client_mac[5] == chaddr[5]))
        {
            return dhcps.ip_mac_mapping[i].client_ip;
        }
    }
    return CLIENT_IP_NOT_FOUND;
}

static uint8_t *ac_lookup_ip(uint32_t client_ip)
{
    /* returns mac address, if ip address is present in cache */
    int i;
    for (i = 0; i < dhcps.count_clients && i < MAC_IP_CACHE_SIZE; i++)
    {
        if ((dhcps.ip_mac_mapping[i].client_ip) == client_ip)
        {
            return dhcps.ip_mac_mapping[i].client_mac;
        }
    }
    return NULL;
}

static bool ac_not_full()
{
    /* returns true if cache is not full */
    return (dhcps.count_clients < MAC_IP_CACHE_SIZE);
}

static bool ac_valid_ip(uint32_t requested_ip)
{
    /* skip over our own address, the network address or the
     * broadcast address
     */
    if (requested_ip == ntohl(dhcps.my_ip) || (requested_ip == ntohl(dhcps.my_ip & dhcps.netmask)) ||
        (requested_ip == ntohl((dhcps.my_ip | (0xffffffff & ~dhcps.netmask)))))
    {
        return false;
    }
    if (ac_lookup_ip(htonl(requested_ip)) != NULL)
        return false;
    return true;
}

static void write_u32(char *dest, uint32_t be_value)
{
    *dest++ = be_value & 0xFF;
    *dest++ = (be_value >> 8) & 0xFF;
    *dest++ = (be_value >> 16) & 0xFF;
    *dest   = be_value >> 24;
}

/* Configure the DHCP dynamic IP lease time*/
int dhcp_server_lease_timeout(uint32_t val)
{
    if ((val == 0) || (val > (60U * 60U * 24U * 49700U)))
    {
        return -EINVAL;
    }
    else
    {
        dhcp_address_timeout = val;
        return WM_SUCCESS;
    }
}

/* calculate the address to give out to the next DHCP DISCOVER request
 *
 * DHCP clients will be assigned addresses in sequence in the subnet's address space.
 */
static unsigned int next_yiaddr()
{
    uint32_t new_ip;
    struct bootp_header *hdr = (struct bootp_header *)dhcps.msg;

    /* if device requesting for ip address is already registered,
     * if yes, assign previous ip address to it
     */
    new_ip = ac_lookup_mac(hdr->chaddr);
    if (new_ip == (CLIENT_IP_NOT_FOUND))
    {
        /* next IP address in the subnet */
        dhcps.current_ip = ntohl(dhcps.my_ip & dhcps.netmask) | ((dhcps.current_ip + 1) & ntohl(~dhcps.netmask));
        while (!ac_valid_ip(dhcps.current_ip))
        {
            dhcps.current_ip = ntohl(dhcps.my_ip & dhcps.netmask) | ((dhcps.current_ip + 1) & ntohl(~dhcps.netmask));
        }

        new_ip = htonl(dhcps.current_ip);

        if (ac_add(hdr->chaddr, new_ip) != WM_SUCCESS)
            dhcp_w("No space to store new mapping..");
    }

    return new_ip;
}

static unsigned int make_response(char *msg, enum dhcp_message_type type)
{
    struct bootp_header *hdr;
    struct bootp_option *opt;
    char *offset = msg;

    hdr         = (struct bootp_header *)offset;
    hdr->op     = BOOTP_OP_RESPONSE;
    hdr->htype  = 1;
    hdr->hlen   = 6;
    hdr->hops   = 0;
    hdr->ciaddr = 0;
    hdr->yiaddr = (type == DHCP_MESSAGE_ACK) ? dhcps.client_ip : 0;
    hdr->yiaddr = (type == DHCP_MESSAGE_OFFER) ? next_yiaddr() : hdr->yiaddr;
    hdr->siaddr = 0;
    hdr->riaddr = 0;
    offset += sizeof(struct bootp_header);

    opt                    = (struct bootp_option *)offset;
    opt->type              = BOOTP_OPTION_DHCP_MESSAGE;
    *(uint8_t *)opt->value = type;
    opt->length            = 1;
    offset += sizeof(struct bootp_option) + opt->length;

    if (type == DHCP_MESSAGE_NAK)
        return (unsigned int)(offset - msg);

    opt       = (struct bootp_option *)offset;
    opt->type = BOOTP_OPTION_SUBNET_MASK;
    write_u32(opt->value, dhcps.netmask);
    opt->length = 4;
    offset += sizeof(struct bootp_option) + opt->length;

    opt       = (struct bootp_option *)offset;
    opt->type = BOOTP_OPTION_ADDRESS_TIME;
    write_u32(opt->value, htonl(dhcp_address_timeout));
    opt->length = 4;
    offset += sizeof(struct bootp_option) + opt->length;

    opt       = (struct bootp_option *)offset;
    opt->type = BOOTP_OPTION_DHCP_SERVER_ID;
    write_u32(opt->value, dhcps.my_ip);
    opt->length = 4;
    offset += sizeof(struct bootp_option) + opt->length;

    opt       = (struct bootp_option *)offset;
    opt->type = BOOTP_OPTION_ROUTER;
    write_u32(opt->value, dhcps.my_ip);
    opt->length = 4;
    offset += sizeof(struct bootp_option) + opt->length;

    opt       = (struct bootp_option *)offset;
    opt->type = BOOTP_OPTION_NAMESERVER;
    write_u32(opt->value, dns_get_nameserver());
    opt->length = 4;
    offset += sizeof(struct bootp_option) + opt->length;

    opt       = (struct bootp_option *)offset;
    opt->type = BOOTP_END_OPTION;
    offset++;

    return (unsigned int)(offset - msg);
}

int dhcp_get_ip_from_mac(uint8_t *client_mac, uint32_t *client_ip)
{
    *client_ip = ac_lookup_mac(client_mac);
    if (*client_ip == CLIENT_IP_NOT_FOUND)
    {
        return -WM_FAIL;
    }
    return WM_SUCCESS;
}

int dhcp_send_response(int sock, struct sockaddr *addr, char *msg, int len)
{
    int nb;
    unsigned int sent = 0;
    while (sent < (unsigned int)len)
    {
        nb = sendto(sock, msg + sent, len - sent, 0, addr, sizeof(struct sockaddr_in));
        if (nb < 0)
        {
            dhcp_e("failed to send response");
            return -WM_E_DHCPD_RESP_SEND;
        }
        sent += nb;
    }

    dhcp_d("sent response, %d bytes %s", sent, inet_ntoa(((struct sockaddr_in *)addr)->sin_addr));
    return WM_SUCCESS;
}

static int process_dhcp_message(char *msg, int len)
{
    struct bootp_header *hdr;
    struct bootp_option *opt;
    uint8_t response_type = DHCP_NO_RESPONSE;
    unsigned int consumed = 0;
    bool got_ip           = 0;
    bool need_ip          = 0;
    int ret               = WM_SUCCESS;
    bool got_client_ip    = 0;
    uint32_t new_ip;

    if (!msg || (unsigned int)len < sizeof(struct bootp_header) + sizeof(struct bootp_option) + 1)
        return -WM_E_DHCPD_INVALID_INPUT;

    hdr = (struct bootp_header *)msg;

    switch (hdr->op)
    {
        case BOOTP_OP_REQUEST:
            dhcp_d("bootp request");
            break;
        case BOOTP_OP_RESPONSE:
            dhcp_d("bootp response");
            break;
        default:
            dhcp_e("invalid op code: %d", hdr->op);
            return -WM_E_DHCPD_INVALID_OPCODE;
    }

    if (hdr->htype != 1 || hdr->hlen != 6)
    {
        dhcp_e("invalid htype or hlen");
        return -WM_E_DHCPD_INCORRECT_HEADER;
    }

    dhcp_d("client MAC: %02X:%02X:%02X:%02X:%02X:%02X", hdr->chaddr[0], hdr->chaddr[1], hdr->chaddr[2], hdr->chaddr[3],
           hdr->chaddr[4], hdr->chaddr[5]);

    dhcp_d("magic cookie: 0x%X", hdr->cookie);

    len -= sizeof(struct bootp_header);
    opt = (struct bootp_option *)(msg + sizeof(struct bootp_header));
    while (len > 0 && opt->type != BOOTP_END_OPTION)
    {
        if (opt->type == BOOTP_OPTION_DHCP_MESSAGE && opt->length == 1)
        {
            dhcp_d("found DHCP message option");
            switch (*(uint8_t *)opt->value)
            {
                case DHCP_MESSAGE_DISCOVER:
                    dhcp_d("DHCP discover");
                    response_type = DHCP_MESSAGE_OFFER;
                    break;

                case DHCP_MESSAGE_REQUEST:
                    dhcp_d("DHCP request");
                    need_ip = 1;
                    if (hdr->ciaddr != 0x0000000)
                    {
                        dhcps.client_ip = hdr->ciaddr;
                        got_client_ip   = 1;
                    }
                    break;

                default:
                    dhcp_d("ignoring message type %d", *(uint8_t *)opt->value);
                    break;
            }
        }
        if (opt->type == BOOTP_OPTION_REQUESTED_IP && opt->length == 4)
        {
            dhcp_d("found REQUESTED IP option %hhu.%hhu.%hhu.%hhu", opt->value[0], opt->value[1], opt->value[2],
                   opt->value[3]);
            memcpy((uint8_t *)&dhcps.client_ip, (uint8_t *)opt->value, 4);
            got_client_ip = 1;
        }

        if (got_client_ip)
        {
            /* requested address outside of subnet */
            if ((dhcps.client_ip & dhcps.netmask) == (dhcps.my_ip & dhcps.netmask))
            {
                /* When client requests an IP address,
                 * DHCP-server checks if the valid
                 * IP-MAC entry is present in the
                 * ip-mac cache, if yes, also checks
                 * if the requested IP is same as the
                 * IP address present in IP-MAC entry,
                 * if yes, it allows the device to
                 * continue with the requested IP
                 * address.
                 */
                new_ip = ac_lookup_mac(hdr->chaddr);
                if (new_ip != (CLIENT_IP_NOT_FOUND))
                {
                    /* if new_ip is equal to requested ip */
                    if (new_ip == dhcps.client_ip)
                    {
                        got_ip = 1;
                    }
                    else
                    {
                        got_ip = 0;
                    }
                }
                else if (ac_valid_ip(ntohl(dhcps.client_ip)))
                {
                    /* When client requests with an IP
                     * address that is within subnet range
                     * and not assigned to any other client,
                     * then dhcp-server allows that device
                     * to continue with that IP address.
                     * And if IP-MAC cache is not full then
                     * adds this entry in cache.
                     */
                    if (ac_not_full())
                    {
                        ac_add(hdr->chaddr, dhcps.client_ip);
                    }
                    else
                    {
                        dhcp_w(
                            "No space to store new "
                            "mapping..");
                    }
                    got_ip = 1;
                }
            }
        }

        /* look at the next option (if any) */
        consumed = sizeof(struct bootp_option) + opt->length;
        len -= consumed;
        opt = (struct bootp_option *)((char *)opt + consumed);
        if (need_ip)
            response_type = got_ip ? DHCP_MESSAGE_ACK : DHCP_MESSAGE_NAK;
    }

    if (response_type != DHCP_NO_RESPONSE)
    {
        ret = make_response(msg, (enum dhcp_message_type)response_type);
        ret = SEND_RESPONSE(dhcps.sock, (struct sockaddr *)&dhcps.baddr, msg, ret);
        if (response_type == DHCP_MESSAGE_ACK)
            send_gratuitous_arp(dhcps.my_ip);
        return WM_SUCCESS;
    }

    dhcp_d("ignoring DHCP packet");
    return WM_SUCCESS;
}

static void dhcp_clean_sockets()
{
    int ret;

    if (ctrl != -1)
    {
        ret = net_close(ctrl);
        if (ret != 0)
        {
            dhcp_w("Failed to close control socket: %d", net_get_sock_error(ctrl));
        }
        ctrl = -1;
    }
    if (dhcps.sock != -1)
    {
        ret = net_close(dhcps.sock);
        if (ret != 0)
        {
            dhcp_w("Failed to close dhcp socket: %d", net_get_sock_error(dhcps.sock));
        }
        dhcps.sock = -1;
    }
}

void dhcp_server(os_thread_arg_t data)
{
    int ret;
    static int one = 1;
    struct sockaddr_in caddr;
    struct sockaddr_in ctrl_listen;
    int addr_len = 0;
    int max_sock;
    int len;
    socklen_t flen = sizeof(caddr);
    fd_set rfds;

    memset(&ctrl_listen, 0, sizeof(struct sockaddr_in));

    /* create listening control socket */
    ctrl = net_socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ctrl < 0)
    {
        ret = net_get_sock_error(ctrl);
        dhcp_e("Failed to create control socket: %d.", ret);
        goto done;
    }
    setsockopt(ctrl, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
    ctrl_listen.sin_family      = PF_INET;
    ctrl_listen.sin_port        = htons(CTRL_PORT);
    ctrl_listen.sin_addr.s_addr = net_inet_aton("127.0.0.1");
    addr_len                    = sizeof(struct sockaddr_in);
    ret                         = net_bind(ctrl, (struct sockaddr *)&ctrl_listen, addr_len);
    if (ret < 0)
    {
        dhcp_e("Failed to bind control socket", ctrl);
        dhcp_clean_sockets();
        dns_free_allocations();
        os_thread_self_complete(NULL);
    }

    os_mutex_get(&dhcpd_mutex, OS_WAIT_FOREVER);

    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(dhcps.sock, &rfds);
        FD_SET(ctrl, &rfds);
        max_sock = dns_get_maxsock(&rfds);

        max_sock = (max_sock > ctrl) ? max_sock : ctrl;

        ret = net_select(max_sock + 1, &rfds, NULL, NULL, NULL);

        /* Error in select? */
        if (ret < 0)
        {
            dhcp_e("select failed", -1);
            goto done;
        }

        /* check the control socket */
        if (FD_ISSET(ctrl, &rfds))
        {
            ret = recvfrom(ctrl, ctrl_msg, sizeof(ctrl_msg), 0, (struct sockaddr *)0, (socklen_t *)0);
            if (ret == -1)
            {
                dhcp_e(
                    "Failed to get control"
                    " message: %d\r\n",
                    ctrl);
            }
            else
            {
                if (strcmp(ctrl_msg, "HALT") == 0)
                {
                    goto done;
                }
            }
        }

        if (FD_ISSET(dhcps.sock, &rfds))
        {
            len = recvfrom(dhcps.sock, dhcps.msg, sizeof(dhcps.msg), 0, (struct sockaddr *)&caddr, &flen);
            if (len > 0)
            {
                dhcp_d("recved msg on dhcp sock len: %d", len);
                process_dhcp_message(dhcps.msg, len);
            }
        }

        dns_process_packet();
    }

done:
    dhcp_clean_sockets();
    dns_free_allocations();
    os_mutex_put(&dhcpd_mutex);
    os_thread_self_complete(NULL);
}

int dhcp_create_and_bind_udp_socket(struct sockaddr_in *address, void *intrfc_handle)
{
    int one = 1;
    int ret;
    struct ifreq req;

    memset(req.ifr_name, 0, sizeof(req.ifr_name));
    strncpy(req.ifr_name, "ua2", 3);

    int sock = net_socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        dhcp_e("failed to create a socket");
        return -WM_FAIL;
    }

    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));
    if (ret == -1)
    {
        /* This is unimplemented in lwIP, hence do not return */
        dhcp_e("failed to set SO_REUSEADDR");
    }

    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&one, sizeof(one)) == -1)
    {
        dhcp_e("failed to set SO_BROADCAST");
        net_close(sock);
        return -WM_FAIL;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, &req, sizeof(struct ifreq)) == -1)
    {
        dhcp_e("failed to set SO_BINDTODEVICE");
        net_close(sock);
        return -WM_FAIL;
    }

    net_socket_blocking(sock, NET_BLOCKING_OFF);

    ret = net_bind(sock, (struct sockaddr *)address, sizeof(struct sockaddr));

    if (ret)
    {
        dhcp_e("failed to bind server socket");
        dhcp_e("socket err: %d", net_get_sock_error(sock));
        net_close(sock);
        return -WM_FAIL;
    }
    return sock;
}

int dhcp_server_init(void *intrfc_handle)
{
    int ret = WM_SUCCESS;

    memset(&dhcps, 0, sizeof(dhcps));

    ret = os_mutex_create(&dhcpd_mutex, "dhcp", OS_MUTEX_INHERIT);
    if (ret != WM_SUCCESS)
        return -WM_E_DHCPD_MUTEX_CREATE;

    get_broadcast_addr(&dhcps.baddr);
    dhcps.baddr.sin_port = htons(DHCP_CLIENT_PORT);

    if (get_ip_addr_from_interface(&dhcps.my_ip, intrfc_handle) < 0)
    {
        dhcp_e("failed to look up our IP address from interface");
        ret = -WM_E_DHCPD_IP_ADDR;
        goto out;
    }

    if (get_netmask_from_interface(&dhcps.netmask, intrfc_handle) < 0)
    {
        dhcp_e("failed to look up our netmask from interface");
        ret = -WM_E_DHCPD_NETMASK;
        goto out;
    }

    dhcps.saddr.sin_family      = AF_INET;
    dhcps.saddr.sin_addr.s_addr = INADDR_ANY;
    dhcps.saddr.sin_port        = htons(DHCP_SERVER_PORT);
    dhcps.sock                  = dhcp_create_and_bind_udp_socket(&dhcps.saddr, intrfc_handle);

    if (dhcps.sock < 0)
    {
        ret = -WM_E_DHCPD_SOCKET;
        goto out;
    }

    ret = dns_server_init(intrfc_handle);
    if (ret != WM_SUCCESS)
    {
        dhcp_e("Failed to start dhcp server. Err: %d", ret);
        goto out;
    }

    return WM_SUCCESS;

out:
    os_mutex_delete(&dhcpd_mutex);
    return ret;
}

static int send_ctrl_msg(const char *msg)
{
    int ret, ctrl_tmp;
    struct sockaddr_in to_addr;

    /*
     * Create a temporary socket and send "HALT" message to control
     * socket listening on port CTRL_PORT (12679), this is done in order
     * to make sure that same socket does not get used from multiple
     * thread contexts.
     */
    ctrl_tmp = net_socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ctrl_tmp < 0)
    {
        ret = net_get_sock_error(ctrl_tmp);
        dhcp_e("failed to create socket");
        return ret;
    }

    memset((char *)&to_addr, 0, sizeof(to_addr));
    to_addr.sin_family      = PF_INET;
    to_addr.sin_port        = htons(CTRL_PORT);
    to_addr.sin_addr.s_addr = net_inet_aton("127.0.0.1");

    ret = sendto(ctrl_tmp, msg, strlen(msg) + 1, 0, (struct sockaddr *)&to_addr, sizeof(to_addr));
    if (ret == -1)
        ret = net_get_sock_error(ctrl_tmp);
    else
        ret = WM_SUCCESS;

    net_close(ctrl_tmp);
    return ret;
}

int dhcp_send_halt(void)
{
    int ret = WM_SUCCESS;

    ret = send_ctrl_msg("HALT");
    if (ret != 0)
    {
        dhcp_w("Failed to send HALT: %d.", ret);
        return -WM_FAIL;
    }

    ret = dhcp_free_allocations();
    return ret;
}

int dhcp_free_allocations(void)
{
    int ret;

    /* Wait for 10 seconds */
    ret = os_mutex_get(&dhcpd_mutex, os_msec_to_ticks(10000));
    if (ret != WM_SUCCESS)
        return ret;

    dhcp_clean_sockets();
    dns_free_allocations();

    ret = os_mutex_put(&dhcpd_mutex);
    if (ret != WM_SUCCESS)
        return ret;

    return os_mutex_delete(&dhcpd_mutex);
}

static int send_gratuitous_arp(uint32_t ip)
{
    int sock;
    struct arp_packet pkt;
    struct sockaddr_in to_addr;
    to_addr.sin_family      = AF_INET;
    to_addr.sin_addr.s_addr = ip;
    pkt.frame_type          = htons(ARP_FRAME_TYPE);
    pkt.hw_type             = htons(ETHER_HW_TYPE);
    pkt.prot_type           = htons(IP_PROTO_TYPE);
    pkt.hw_addr_size        = ETH_HW_ADDR_LEN;
    pkt.prot_addr_size      = IP_ADDR_LEN;
    pkt.op                  = htons(OP_ARP_REQUEST);

    write_u32(pkt.sndr_ip_addr, ip);
    write_u32(pkt.rcpt_ip_addr, ip);

    memset(pkt.targ_hw_addr, 0xff, ETH_HW_ADDR_LEN);
    memset(pkt.rcpt_hw_addr, 0xff, ETH_HW_ADDR_LEN);
    wlan_get_mac_address(pkt.sndr_hw_addr);
    wlan_get_mac_address(pkt.src_hw_addr);
    sock = net_socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        dhcp_e("Could not open socket to send Gratuitous ARP");
        return -WM_E_DHCPD_SOCKET;
    }
    memset(pkt.padding, 0, sizeof(pkt.padding));

    if (sendto(sock, (char *)&pkt, sizeof(pkt), 0, (struct sockaddr *)&to_addr, sizeof(to_addr)) < 0)
    {
        dhcp_e("Failed to send Gratuitous ARP");
        net_close(sock);
        return -WM_E_DHCPD_ARP_SEND;
    }
    dhcp_d("Gratuitous ARP sent");
    net_close(sock);
    return WM_SUCCESS;
}

static void get_broadcast_addr(struct sockaddr_in *addr)
{
    addr->sin_family = AF_INET;
    /* limited broadcast addr (255.255.255.255) */
    addr->sin_addr.s_addr = 0xffffffff;
    addr->sin_len         = sizeof(struct sockaddr_in);
}

static int get_ip_addr_from_interface(uint32_t *ip, void *interface_handle)
{
    return net_get_if_ip_addr(ip, interface_handle);
}

static int get_netmask_from_interface(uint32_t *nm, void *interface_handle)
{
    return net_get_if_ip_mask(nm, interface_handle);
}

void dhcp_stat()
{
    int i = 0;
    PRINTF("DHCP Server Lease Duration : %d seconds\r\n", (int)dhcp_address_timeout);
    if (dhcps.count_clients == 0)
    {
        PRINTF("No IP-MAC mapping stored\r\n");
    }
    else
    {
        PRINTF("Client IP\tClient MAC\r\n");
        for (i = 0; i < dhcps.count_clients && i < MAC_IP_CACHE_SIZE; i++)
        {
            PRINTF("%s\t%02X:%02X:%02X:%02X:%02X:%02X\r\n", inet_ntoa(dhcps.ip_mac_mapping[i].client_ip),
                   dhcps.ip_mac_mapping[i].client_mac[0], dhcps.ip_mac_mapping[i].client_mac[1],
                   dhcps.ip_mac_mapping[i].client_mac[2], dhcps.ip_mac_mapping[i].client_mac[3],
                   dhcps.ip_mac_mapping[i].client_mac[4], dhcps.ip_mac_mapping[i].client_mac[5]);
        }
    }
}
