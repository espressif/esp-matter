/** @file ping.c
 *
 *  @brief  This file provides the support for network utility ping
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

/* ping.c: This file contains the support for network utility ping */

#include <string.h>
#include <wm_os.h>
#include <wm_net.h>
#include <cli.h>
#include <cli_utils.h>
#include <wlan.h>
#include <ping.h>

int wlan_get_ipv4_addr(unsigned int *ipv4_addr);

/* Display the final result of ping */
static void display_ping_result(ip_addr_t *addr, int total, int recvd)
{
    int dropped = total - recvd;
    PRINTF("\r\n--- %s ping statistics ---\r\n", inet_ntoa(*addr));
    PRINTF("%d packets transmitted, %d received,", total, recvd);
    if (dropped != 0)
        PRINTF(" +%d errors,", dropped);
    PRINTF(" %d%% packet loss\r\n", (dropped * 100) / total);
}

/* Display the statistics of the current iteration of ping */
static void display_ping_stats(int status, uint32_t size, ip_addr_t *ipaddr, int seqno, int ttl, uint32_t time)
{
    if (status == WM_SUCCESS)
        PRINTF("%u bytes from %s: icmp_req=%u ttl=%u time=%u ms\r\n", size, inet_ntoa(*ipaddr), seqno, ttl, time);
    else
        PRINTF("From %s icmp_seq=%u Destination Host Unreachable\r\n", inet_ntoa(*ipaddr), seqno);
}

/* Display the usage of ping */
static void display_ping_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF(
        "\tping [-s <packet_size>] [-c <packet_count>] "
        "[-W <timeout in sec>] <ip_address>\r\n");
    PRINTF("Default values:\r\n");
    PRINTF(
        "\tpacket_size: %u\r\n\tpacket_count: %u"
        "\r\n\ttimeout: %u sec\r\n",
        PING_DEFAULT_SIZE, PING_DEFAULT_COUNT, PING_DEFAULT_TIMEOUT_SEC);
}

/* Handle the ICMP echo response and extract required parameters */
static int ping_recv(int s, uint16_t seq_no, int *ttl)
{
    char buf[64];
    int fromlen = 0, len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;

    while ((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&from, (socklen_t *)&fromlen)) > 0)
    {
        /* Received length should be greater than size of IP header and
         * size of ICMP header */
        if (len >= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr)))
        {
            iphdr = (struct ip_hdr *)buf;
            /* Calculate the offset of ICMP header */
            iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));

            /* Verify that the echo response is for the echo request
             * we sent by checking PING_ID and sequence number */
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(seq_no)))
            {
                /* Extract TTL and send back so that it can be
                 * displayed in ping statistics */
                *ttl = iphdr->_ttl;
                return WM_SUCCESS;
            }
        }
    }
    /* Either len < 0 or the echo response verification unsuccessful */
    return -WM_FAIL;
}

/* Prepare a ICMP echo request */
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, uint16_t len, uint16_t seq_no)
{
    uint32_t i;
    uint32_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(seq_no);

    /* Fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++)
    {
        ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

/* Send an ICMP echo request, receive its response and print its statistics and
 * result */
static int ping(unsigned int count, unsigned short size, unsigned int r_timeout, ip_addr_t *addr)
{
    unsigned int i = 1;
    int ret = WM_SUCCESS, s, recvd = 0;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    unsigned int src_ip, ping_time, ping_size;
    ip_addr_t *ip_addr;
    struct timeval timeout;

    PRINTF("PING %s (%s) %u(%u) bytes of data\r\n", inet_ntoa(*addr), inet_ntoa(*addr), size,
           size + sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr));

    /* Create a raw socket */
    s = socket(AF_INET, SOCK_RAW, 1);
    if (s < 0)
    {
        ping_e("Failed to create raw socket for ping %d", s);
        return -WM_FAIL;
    }
    /* Convert timeout to milliseconds */
    timeout.tv_sec  = r_timeout;
    timeout.tv_usec = 0;

    /* Set the socket timeout */
    ret = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if (ret < 0)
    {
        ping_e("Failed to set socket options for ping");
        ret = -WM_FAIL;
        goto end;
    }

    /* Get the source IP address */
    wlan_get_ipv4_addr(&src_ip);

    /* Ping size is: size of ICMP header + size of payload */
    ping_size = sizeof(struct icmp_echo_hdr) + size;

    iecho = (struct icmp_echo_hdr *)os_mem_alloc(ping_size);
    if (!iecho)
    {
        ping_e("Failed to allocate memory for ping packet");
        ret = -WM_FAIL;
        goto end;
    }

    ping_prepare_echo(iecho, (uint16_t)ping_size, i);

    while (i <= count)
    {
        iecho->seqno  = htons(i);
        iecho->chksum = 0;
        iecho->chksum = inet_chksum(iecho, (uint16_t)ping_size);

        to.sin_len    = sizeof(to);
        to.sin_family = AF_INET;
        inet_addr_from_ip4addr(&to.sin_addr, ip_2_ip4(addr));

        /* Send the ICMP echo request */
        ret = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr *)&to, sizeof(to));

        /* Get the current ticks as the start time */
        ping_time = os_ticks_get();

        if (ret > 0)
        {
            int ttl = 0;
            /* Receive the ICMP echo response */
            ret = ping_recv(s, i, &ttl);

            /* Calculate the round trip time */
            ping_time = os_ticks_get() - ping_time;

            if (ret == WM_SUCCESS)
            {
                /* Increment the receive counter */
                recvd++;
                /* To display successful ping stats, destination
                 * IP address is required */
                ip_addr = addr;
            }
            else
            {
                /* To display unsuccessful ping stats, source
                 * IP address is required */
                ip_addr = (ip_addr_t *)&src_ip;
            }

            display_ping_stats(ret, ping_size, ip_addr, i, ttl, ping_time);
        }
        else
        {
            ping_e("Failed to send ping echo request");
            os_mem_free(iecho);
            ret = -WM_FAIL;
            goto end;
        }
        i++;
        os_thread_sleep(os_msec_to_ticks(PING_INTERVAL));
    }
    os_mem_free(iecho);
    display_ping_result((ip_addr_t *)&src_ip, count, recvd);
end:
    close(s);
    return ret;
}

void cmd_ping(int argc, char **argv)
{
    ip_addr_t addr;
    int c;
    uint16_t size    = PING_DEFAULT_SIZE;
    uint32_t count   = PING_DEFAULT_COUNT, temp;
    uint32_t timeout = PING_DEFAULT_TIMEOUT_SEC;

    /* If number of arguments is odd then print error */
    if (argc & 0x01)
        goto end;

    cli_optind = 1;
    while ((c = cli_getopt(argc, argv, "c:s:W:")) != -1)
    {
        switch (c)
        {
            case 'c':
                count = strtoul(cli_optarg, NULL, 10);
                break;
            case 's':
                temp = strtoul(cli_optarg, NULL, 10);
                if (temp > PING_MAX_SIZE)
                {
                    PRINTF(
                        "ping: packet size too large: %u."
                        " Maximum is %u\r\n",
                        temp, PING_MAX_SIZE);
                    return;
                }
                size = temp;
                break;
            case 'W':
                timeout = strtoul(cli_optarg, NULL, 10);
                break;
            default:
                goto end;
        }
    }
    if (cli_optind == argc)
        goto end;

    /* Extract the destination IP address. This function returns non zero on
     * success, zero on failure */
    if (inet_aton(argv[cli_optind], &addr))
    {
        ping(count, size, timeout, &addr);
        return;
    }
end:
    PRINTF("Incorrect usage\r\n");
    display_ping_usage();
}

static struct cli_command ping_cli[] = {
    {"ping", "[-s <packet_size>] [-c <packet_count>] [-W <timeout in sec>] <ip_address>", cmd_ping},
};

int ping_cli_init(void)
{
    unsigned int i;
    for (i = 0; i < sizeof(ping_cli) / sizeof(struct cli_command); i++)
        if (cli_register_command(&ping_cli[i]))
            return -WM_FAIL;
    return WM_SUCCESS;
}

int ping_cli_deinit(void)
{
    unsigned int i;

    for (i = 0; i < sizeof(ping_cli) / sizeof(struct cli_command); i++)
        if (cli_unregister_command(&ping_cli[i]))
            return -WM_FAIL;
    return WM_SUCCESS;
}
