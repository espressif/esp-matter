#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <lwip/pbuf.h>
#include <lwip/udp.h>
#include <lwip/netifapi.h>
#include <dns_server.h>
#include <utils_dns.h>

#define SIZEOF_DNSANS_HDR    12
#define SIZEOF_DNSANS_HDRQUE 20

#define DNS_SERVER_ID        0x0000
#define DNS_SERVER_FLAGS     0x8180
#define DNS_SERVER_NUMQUE    0x0001
#define DNS_SERVER_ANSRRS    0x0001
#define DNS_SERVER_AUTRRS    0x0000
#define DNS_SERVER_ADDRRS    0x0000
#define DNS_SERVER_TYPE      0x0001
#define DNS_SERVER_CLASS     0x0001
#define DNS_POINAME          0xC00C
#define DNS_SERVER_ANSTYPE   0x0001
#define DNS_SERVER_ANSTYPEE  0x0001
#define DNS_SERVER_DATALEN   0x0000
#define DNS_SERVER_ANSTIME   0x003c0004
#define DNS_SERVER_ADRESS    0xc0a8a901

struct dns_server_ctx {
    uint8_t name[256];
    uint16_t txid;
    uint16_t nquestions;
    uint16_t nanswers;
    struct udp_pcb *upcb1;
    const ip_addr_t *addr1;
    u16_t port1;
};

struct dns_ans_hdr {
    PACK_STRUCT_FIELD(uint16_t id);
    PACK_STRUCT_FIELD(uint16_t flag);
    PACK_STRUCT_FIELD(uint16_t numquestions);
    PACK_STRUCT_FIELD(uint16_t numanswers);
    PACK_STRUCT_FIELD(uint16_t numauthrr);
    PACK_STRUCT_FIELD(uint16_t numextrarr);
}PACK_STRUCT_STRUCT;

struct dns_ans_ans {
    uint16_t typ;
    uint16_t cls;
    uint16_t point;
    uint16_t antyp;
    uint16_t antypp;
    uint16_t len;
    uint32_t time;
    uint32_t addr;
};

struct dns_table_entry {
    uint16_t txid;
    uint16_t flags;
    uint16_t numque;
    uint16_t ansrrs;
    uint16_t autrrs;
    uint16_t addrrs;
    uint16_t type;
    uint16_t class;
    uint16_t poiname;
    uint16_t anstype;
    uint16_t anstypee;
    uint16_t datalen;
    uint32_t anstime;
    uint32_t adress;
};

static void dns_server_send(struct dns_server_ctx *dns_ctx)
{
    struct pbuf *rp = NULL;
    struct dns_ans_hdr hdr;
    struct dns_ans_ans qry;
    uint8_t n;
    uint16_t query_idx, copy_len;
    const char *hostname, *hostname_part;
    struct dns_table_entry dns_server_table = {
        .txid     = DNS_SERVER_ID,
        .flags    = DNS_SERVER_FLAGS,
        .numque   = DNS_SERVER_NUMQUE,
        .ansrrs   = DNS_SERVER_ANSRRS,
        .autrrs   = DNS_SERVER_AUTRRS,
        .addrrs   = DNS_SERVER_ADDRRS,
        .type     = DNS_SERVER_TYPE,
        .class    = DNS_SERVER_CLASS,
        .poiname  = DNS_POINAME,
        .anstype  = DNS_SERVER_ANSTYPE,
        .anstypee = DNS_SERVER_ANSTYPEE,
        .datalen  = DNS_SERVER_DATALEN,
        .anstime  = DNS_SERVER_ANSTIME,
        .adress   = DNS_SERVER_ADRESS
    };
    struct dns_table_entry* entry = &dns_server_table;

    rp =  pbuf_alloc(PBUF_TRANSPORT, 512, PBUF_RAM);
    if (rp != NULL) {
        memset (&hdr, 0, SIZEOF_DNSANS_HDR);
        /* fill dns_ans header */
        hdr.id           = htons(dns_ctx->txid);
        hdr.flag         = htons(entry->flags);
        hdr.numquestions = htons(entry->numque);
        hdr.numanswers   = htons(entry->ansrrs);
        hdr.numauthrr    = htons(entry->autrrs);
        hdr.numextrarr   = htons(entry->addrrs);
        pbuf_take(rp, &hdr, SIZEOF_DNSANS_HDR);
        /* convert hostname into suitable query format. */
        hostname = (char*)dns_ctx->name;
        --hostname;
        query_idx = SIZEOF_DNSANS_HDR;
        do {
            ++hostname;
            hostname_part = hostname;
            for (n = 0; *hostname != '.' && *hostname != 0; ++hostname) {
                ++n;
            }
            copy_len = (u16_t)(hostname - hostname_part);
            pbuf_put_at(rp, query_idx, n);
            pbuf_take_at(rp, hostname_part, copy_len, query_idx + 1);
            query_idx += n + 1;
        } while (*hostname != 0);
        pbuf_put_at(rp, query_idx, 0);
        query_idx++;
        /* fill dns ans */
        qry.typ    = htons(entry->type);
        qry.cls    = htons(entry->class);
        qry.point  = htons(entry->poiname);
        qry.antyp  = htons(entry->anstype);
        qry.antypp = htons(entry->anstypee);
        qry.len    = htons(entry->datalen);
        qry.time   = htonl(entry->anstime);
        qry.addr   = htonl(entry->adress);
        pbuf_take_at(rp, &qry, SIZEOF_DNSANS_HDRQUE, query_idx);

        pbuf_realloc(rp, query_idx + SIZEOF_DNSANS_HDRQUE);//shrink to the real size
        udp_sendto(dns_ctx->upcb1, rp, dns_ctx->addr1, dns_ctx->port1);
        pbuf_free(rp);
    }
}

void get_dns_request(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    struct dns_ans_hdr hdr;
    struct dns_server_ctx *dns_ctx = (struct dns_server_ctx*)arg;
    int len;

    dns_ctx->addr1 = addr;
    dns_ctx->port1 = port;

    if (p->tot_len < SIZEOF_DNSANS_HDR) {
        LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: pbuf too small\r\n"));
         /* free pbuf and return */
    } else {
        pbuf_copy_partial(p, &hdr, SIZEOF_DNSANS_HDR, 0);
        dns_ctx->txid = ntohs(hdr.id);
        dns_ctx->nquestions = ntohs(hdr.numquestions);
        pbuf_copy_partial(
            p,
            dns_ctx->name,
            len = p->tot_len > sizeof(dns_ctx->name) - 1 ? sizeof(dns_ctx->name) - 1 : p->tot_len,
            SIZEOF_DNSANS_HDR
        );
        if (0 == utils_dns_domain_get(dns_ctx->name, dns_ctx->name, &len)) {
            if (len > 0 && '.' == dns_ctx->name[len - 1]) {
                len--;
            }
            dns_ctx->name[len] = '\0';
        }
    }
    pbuf_free(p);  //check this

    dns_server_send(dns_ctx);
}

static err_t dns_server_init_internal(struct netif *arg)
{
    err_t ret = ERR_OK;
    struct udp_pcb *upcb;
    struct dns_server_ctx *server = (struct dns_server_ctx *)arg;

    upcb = udp_new();
    if (NULL == upcb) {
        ret = ERR_MEM;
        goto out;
    }
    if (ERR_OK != udp_bind(upcb, IP_ADDR_ANY, 53)) {
        ret = ERR_USE;
        goto err;
    }
    udp_recv(upcb, get_dns_request, server);
    server->upcb1 = upcb;
    goto out;

err:
    udp_remove(upcb);
out:
    return ret;
}

void *dns_server_init(void)
{
    struct dns_server_ctx *server;

    server = pvPortMalloc(sizeof(struct dns_server_ctx));
    if (NULL == server) {
        goto out;
    }
    if (ERR_OK != netifapi_netif_common((struct netif *)server, NULL, dns_server_init_internal)) {
        vPortFree(server);
        server = NULL;
    }
out:
    return server;
}

static void dns_server_deinit_internal(struct netif *arg)
{
    struct dns_server_ctx *server = (struct dns_server_ctx *)arg;
    udp_remove(server->upcb1);
}

void dns_server_deinit(void *server)
{
    if (!server) {
        return;
    }
    netifapi_netif_common((struct netif *)server, dns_server_deinit_internal, NULL);
    vPortFree(server);
}
