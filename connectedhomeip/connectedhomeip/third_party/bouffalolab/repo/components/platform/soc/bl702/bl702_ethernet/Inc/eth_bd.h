#ifndef __ETH_BD_H__
#define __ETH_BD_H__
#include <lwip/netif.h>
#include <bl_emac.h>
#include <utils_list.h>


#define ETH_RXNB  6
#define ETH_TXNB  4
struct netif eth_mac;
#define EMAC_TX_COMMON_FLAGS   (EMAC_BD_FIELD_MSK(TX_RD)  | \
                                EMAC_BD_FIELD_MSK(TX_IRQ) | \
                                EMAC_BD_FIELD_MSK(TX_PAD) | \
                                EMAC_BD_FIELD_MSK(TX_CRC) )

#define EMAC_RX_COMMON_FLAGS   (ETH_MAX_PACKET_SIZE << 16) | \
                                EMAC_BD_FIELD_MSK(RX_IRQ) )

typedef struct eth_pbuf_custom{
    struct pbuf_custom p;
    uint32_t idx;
    void *bd;
}eth_pbuf_custom_t;

struct unsent_item{
    struct utils_list_hdr hdr;
    struct pbuf *p;
};

typedef struct {
    struct utils_list unsent;
    uint8_t Tx_free_bd_num;
    uint8_t Rx_free_bd_num;
    volatile uint16_t unsent_num;
    uint32_t tx_pkt_cnt;
    uint32_t rx_pkt_cnt;
    uint32_t out_tmr;
    uint32_t done_tmr;
}eth_context;

typedef enum {
    ETH_INIT_STEP_LINKUP,
    ETH_INIT_STEP_READY,
    ETH_INIT_STEP_LINKDOWN,
}eth_link_state;


typedef int (*eth_callback)(eth_link_state val);

int ethernet_init(eth_callback cb);
#endif
