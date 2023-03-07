/** @file dns-server.c
 *
 *  @brief This file provides the DNS Server
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

/** dns-server.c: The DNS Server
 */
#include <string.h>

#include <wm_os.h>
#include <wm_net.h>
#include <dhcp-server.h>
#include <wlan.h>

#include "dhcp-bootp.h"
#include "dns.h"
#include "dhcp-priv.h"

struct dns_server_data dnss;
int (*dhcp_dns_server_handler)(char *msg, int len, struct sockaddr_in *fromaddr);
extern struct dhcp_server_data dhcps;

/* take a domain name and convert it into a DNS QNAME format, i.e.
 * foo.rats.com. -> 03 66 6f 6f 04 72 61 74 73 03 63 6f 6d 00
 *
 * The size of the QNAME will be one byte longer than the original string.
 */
static void format_qname(char *domain_name, char *dns_qname)
{
    int i   = 0, len;
    char *s = domain_name;
    char *d = dns_qname + 1;

    len = strlen(s);
    s += len - 1;
    d += len - 1;

    while (s >= domain_name)
    {
        if (*s != '.')
        {
            *d = *s;
            i++;
        }
        else
        {
            *d = (char)i;
            i  = 0;
        }
        s--;
        d--;
    }
    dns_qname[0] = i;
}

static unsigned int make_answer_rr(char *base, char *query, char *dst)
{
    struct dns_question *q;
    struct dns_rr *rr = (struct dns_rr *)dst;
    char *query_start = query;

    rr->name_ptr = htons(((uint16_t)(query - base) | 0xC000));

    /* skip past the qname (label) field */
    do
    {
        if (*query > 0)
            query += *query + 1;
    } while (*query > 0);
    query++;

    q = (struct dns_question *)query;
    query += sizeof(struct dns_question);

    rr->type     = q->type;
    rr->class    = q->class;
    rr->ttl      = htonl(60U * 60U * 1U); /* 1 hour */
    rr->rdlength = htons(4);
    rr->rd       = dhcps.my_ip;

    return (unsigned int)(query - query_start);
}

static char *parse_questions(unsigned int num_questions, uint8_t *pos, int *found)
{
    uint8_t *base = pos;
    int i;

    pos += sizeof(struct dns_header);

    for (; num_questions > 0; num_questions--)
    {
        if (!*found)
        {
            for (i = 0; i < dnss.count_qnames; i++)
            {
                *found = !strncmp(dnss.list_qnames[i].qname, (char *)pos, (base + SERVER_BUFFER_SIZE - pos));
                if (*found)
                    break;
            }
        }
        do
        {
            if (*pos > 0)
                pos += *pos + 1;
            if (pos >= base + SERVER_BUFFER_SIZE)
                return NULL;
        } while (*pos > 0);
        pos += 1 + sizeof(struct dns_question);
    }
    return (char *)pos;
}

#define ERROR_REFUSED 5
int process_dns_message(char *msg, int len, struct sockaddr_in *fromaddr)
{
    struct dns_header *hdr;
    char *pos;
    char *outp = msg + len;
    int found  = 0, nq, i;

    if ((unsigned int)len < sizeof(struct dns_header))
    {
        dhcp_e("DNS request is not complete, hence ignoring it");
        return -WM_E_DHCPD_DNS_IGNORE;
    }

    hdr = (struct dns_header *)msg;

    dhcp_d("DNS transaction id: 0x%x", htons(hdr->id));

    if (hdr->flags.fields.qr)
    {
        dhcp_e("ignoring this dns message (not a query)");
        return -WM_E_DHCPD_DNS_IGNORE;
    }

    nq = ntohs(hdr->num_questions);
    dhcp_d("we were asked %d questions", nq);

    if (nq <= 0)
    {
        dhcp_e("ignoring this dns msg (not a query or 0 questions)");
        return -WM_E_DHCPD_DNS_IGNORE;
    }

    outp = parse_questions(nq, (uint8_t *)msg, &found);
    if (found && outp)
    {
        pos = msg + sizeof(struct dns_header);
        for (i = 0; i < nq; i++)
        {
            if (outp + sizeof(struct dns_rr) >= msg + SERVER_BUFFER_SIZE)
            {
                dhcp_d("no room for more answers, refusing");
                break;
            }
            pos += make_answer_rr(msg, pos, outp);
            outp += sizeof(struct dns_rr);
        }
        hdr->flags.fields.qr    = 1;
        hdr->flags.fields.aa    = 1;
        hdr->flags.fields.rcode = 0;
        hdr->flags.num          = htons(hdr->flags.num);
        hdr->answer_rrs         = htons(i);
        /* the response consists of:
         * - 1 x DNS header
         * - num_questions x query fields from the message we're parsing
         * - num_answers x answer fields that we've appended
         */
        return SEND_RESPONSE(dnss.dnssock, (struct sockaddr *)fromaddr, msg, outp - msg);
    }

    /* make the header represent a response */
    hdr->flags.fields.qr     = 1;
    hdr->flags.fields.opcode = 0;
    /* Errors are never authoritative (unless they are
       NXDOMAINS, which this is not) */
    hdr->flags.fields.aa    = 0;
    hdr->flags.fields.tc    = 0;
    hdr->flags.fields.rd    = 1;
    hdr->flags.fields.ra    = 0;
    hdr->flags.fields.rcode = ERROR_REFUSED;
    hdr->flags.num          = htons(hdr->flags.num);
    /* number of entries in questions section */
    hdr->num_questions  = htons(0x01);
    hdr->answer_rrs     = 0; /* number of resource records in answer section */
    hdr->authority_rrs  = 0;
    hdr->additional_rrs = 0;
    SEND_RESPONSE(dnss.dnssock, (struct sockaddr *)fromaddr, msg, outp - msg);

    return -WM_E_DHCPD_DNS_IGNORE;
}

void dhcp_enable_dns_server(char **domain_names)
{
    if (dhcp_dns_server_handler || dnss.list_qnames)
        return;

    int i;
    /* To reduce footprint impact, dns server support is kept optional */
    dhcp_dns_server_handler = process_dns_message;
    if (domain_names != NULL)
    {
        while (domain_names[dnss.count_qnames] != NULL)
            dnss.count_qnames++;
        dnss.list_qnames = os_mem_alloc(dnss.count_qnames * sizeof(struct dns_qname));

        for (i = 0; i < dnss.count_qnames; i++)
        {
            memset(dnss.list_qnames[i].qname, 0, sizeof(struct dns_qname));
            format_qname(domain_names[i], dnss.list_qnames[i].qname);
        }
    }
}

int dns_server_init(void *intrfc_handle)
{
    if (dhcp_dns_server_handler == NULL)
        return WM_SUCCESS;

    dnss.dnsaddr.sin_family      = AF_INET;
    dnss.dnsaddr.sin_addr.s_addr = INADDR_ANY;
    dnss.dnsaddr.sin_port        = htons(NAMESERVER_PORT);
    dnss.dnssock                 = dhcp_create_and_bind_udp_socket(&dnss.dnsaddr, intrfc_handle);
    if (dnss.dnssock < 0)
        return -WM_E_DHCPD_SOCKET;

    return WM_SUCCESS;
}

void dns_process_packet()
{
    if (!dhcp_dns_server_handler)
        return;

    struct sockaddr_in caddr;
    socklen_t flen = sizeof(caddr);
    int len;
    len = recvfrom(dnss.dnssock, dhcps.msg, sizeof(dhcps.msg), 0, (struct sockaddr *)&caddr, &flen);
    if (len > 0)
    {
        dhcp_d("recved msg on dns sock len: %d", len);
        dhcp_dns_server_handler(dhcps.msg, len, &caddr);
    }
}

uint32_t dns_get_nameserver()
{
    if (dhcp_dns_server_handler)
        return dhcps.my_ip;
    return 0;
}

int dns_get_maxsock(fd_set *rfds)
{
    if (!dhcp_dns_server_handler)
        return dhcps.sock;

    int max_sock;
    FD_SET(dnss.dnssock, rfds);
    max_sock = (dhcps.sock > dnss.dnssock ? dhcps.sock : dnss.dnssock);
    return max_sock;
}

void dns_free_allocations()
{
    if (!dhcp_dns_server_handler)
        return;

    if (dnss.list_qnames)
    {
        dnss.count_qnames = 0;
        os_mem_free(dnss.list_qnames);
        dnss.list_qnames = NULL;
    }
    if (dnss.dnssock != -1)
    {
        if (net_close(dnss.dnssock) != 0)
        {
            dhcp_w("Failed to close dns socket: %d", net_get_sock_error(dnss.dnssock));
        }
        dnss.dnssock = -1;
    }
    dhcp_dns_server_handler = NULL;
}
