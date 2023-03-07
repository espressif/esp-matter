
#include <stdio.h>
#include <stdint.h>

#include "lwip/opt.h"
#include "lwip/def.h"

#include "lwip/prot/ieee.h"
#include "lwip/prot/ethernet.h"
#include "lwip/prot/etharp.h"
#include "lwip/prot/ip.h"
#include "lwip/prot/ip4.h"
#include "lwip/prot/ip6.h"
#include "lwip/prot/udp.h"
#include "lwip/prot/tcp.h"

static inline void dump_print_level(unsigned int level, const char *prefix)
{
  while (level--) {
    printf(prefix);
  }
  printf(" ");
}

static void dump_udp(unsigned int level, unsigned char *pkt)
{
  struct udp_hdr *udphdr = (struct udp_hdr *)pkt;

  dump_print_level(level, ">");

  printf("udp: src port:%" U16_F ", dst port:%" U16_F ", len:%" U16_F "\r\n",
    lwip_ntohs(udphdr->src), lwip_ntohs(udphdr->dest), lwip_ntohs(udphdr->len));
}

void dump_tcp(unsigned int level, unsigned char *pkt)
{
  struct tcp_hdr *tcphdr = (struct tcp_hdr *)pkt;
  unsigned char flags;

  dump_print_level(level, ">");
  printf("tcp: src port:%" U16_F "(%" U16_F "), dst port:%" U16_F "(%" U16_F "), seq:%" U32_F "(%" U32_F "), ack:%" U32_F "(%" U32_F ")\r\n",
    lwip_ntohs(tcphdr->src), tcphdr->src, lwip_ntohs(tcphdr->dest), tcphdr->dest,
    lwip_ntohl(tcphdr->seqno), tcphdr->seqno, lwip_ntohl(tcphdr->ackno), tcphdr->ackno);

  dump_print_level(level, ">");
  printf("tcp: win:%" U16_F "(%" U16_F "), flags:", lwip_ntohs(tcphdr->wnd), tcphdr->wnd);

  flags = TCPH_FLAGS(tcphdr);
  if (flags & TCP_FIN) {
    printf("FIN ");
  }
  if (flags & TCP_SYN) {
    printf("SYN ");
  }
  if (flags & TCP_RST) {
    printf("RST ");
  }
  if (flags & TCP_PSH) {
    printf("PSH ");
  }
  if (flags & TCP_ACK) {
    printf("ACK ");
  }
  if (flags & TCP_URG) {
    printf("URG ");
  }
  if (flags & TCP_ECE) {
    printf("ECE ");
  }
  if (flags & TCP_CWR) {
    printf("CWR ");
  }

  printf("\r\n");
}

static void dump_ip4(unsigned int level, unsigned char *pkt)
{
  struct ip_hdr *iphdr = (struct ip_hdr *)pkt;

  dump_print_level(level, ">");
  printf("ip: src:%" U16_F ".%" U16_F ".%" U16_F ".%" U16_F ", dst:%" U16_F ".%" U16_F ".%" U16_F ".%" U16_F ", proto:%" U16_F "\r\n",
      ip4_addr1_16_val(iphdr->src), ip4_addr2_16_val(iphdr->src), ip4_addr3_16_val(iphdr->src), ip4_addr4_16_val(iphdr->src),
      ip4_addr1_16_val(iphdr->dest), ip4_addr2_16_val(iphdr->dest), ip4_addr3_16_val(iphdr->dest), ip4_addr4_16_val(iphdr->dest),
      (uint16_t)IPH_PROTO(iphdr));

  dump_print_level(level, ">");
  printf("ip: ttl:%" U16_F ", len:%" U16_F ", offset:%" U16_F ", hlen:%" U16_F "\r\n",
      (uint16_t)IPH_TTL(iphdr), lwip_ntohs(IPH_LEN(iphdr)), (uint16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) & IP_OFFMASK), (uint16_t)IPH_HL(iphdr));

  switch (IPH_PROTO(iphdr)) {
    case IP_PROTO_UDP:
      dump_udp(++level, pkt + IPH_HL_BYTES(iphdr));
      break;
    case IP_PROTO_TCP:
      dump_tcp(++level, pkt + IPH_HL_BYTES(iphdr));
      break;
  }
}

static void dump_ip6(unsigned int level, unsigned char *pkt)
{
  // TODO
}

void lwip_dump_ether_pkt(unsigned char *pkt)
{
  int level = 1;
  struct eth_hdr *ethhdr = (struct eth_hdr *)pkt;

  if (ethhdr == NULL) {
    return;
  }

  dump_print_level(level, ">");
  printf("ethernet: dst:%" X8_F ":%" X8_F ":%" X8_F ":%" X8_F ":%" X8_F ":%" X8_F ", src:%" X8_F ":%" X8_F ":%" X8_F ":%" X8_F ":%" X8_F ":%" X8_F ", type:%" X16_F "\r\n",
         (unsigned char)ethhdr->dest.addr[0], (unsigned char)ethhdr->dest.addr[1], (unsigned char)ethhdr->dest.addr[2],
         (unsigned char)ethhdr->dest.addr[3], (unsigned char)ethhdr->dest.addr[4], (unsigned char)ethhdr->dest.addr[5],
         (unsigned char)ethhdr->src.addr[0], (unsigned char)ethhdr->src.addr[1], (unsigned char)ethhdr->src.addr[2],
         (unsigned char)ethhdr->src.addr[3], (unsigned char)ethhdr->src.addr[4], (unsigned char)ethhdr->src.addr[5],
         lwip_htons(ethhdr->type));

  switch (ethhdr->type) {
  case PP_HTONS(ETHTYPE_IP):
    dump_ip4(++level, pkt + SIZEOF_ETH_HDR);
    break;
  case PP_HTONS(ETHTYPE_IPV6):
    dump_ip6(++level, pkt + SIZEOF_ETH_HDR);
    break;
  case PP_HTONS(ETHTYPE_ARP):
    break;
  }
}
