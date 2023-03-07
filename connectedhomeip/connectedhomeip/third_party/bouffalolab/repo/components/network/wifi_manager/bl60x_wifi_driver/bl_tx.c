/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <stdio.h>
#include <netif/etharp.h>
#include <bl_os_private.h>

#include "bl_tx.h"
#include "bl_irqs.h"
#include "bl_utils.h"


struct utils_list tx_list_bl;

int internel_cal_size_tx_hdr = sizeof(struct bl_txhdr);

extern struct bl_hw wifi_hw;
static struct bl_hw *bl_hw_static = &wifi_hw;

#ifndef ARRAY_LEN
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
#endif

#if defined(CFG_CHIP_BL808) || defined(CFG_CHIP_BL606P)
void bl_tx_push(struct bl_hw *bl_hw, struct bl_txhdr *txhdr)
{
    volatile struct hostdesc *host;
    uint32_t* p = txhdr->p;
    uint32_t len;

    volatile struct txdesc_host *txdesc_host;

    //host = &(ipc_host_txdesc_get(bl_hw->ipc_env)->host);
    txdesc_host = ipc_host_txdesc_get(bl_hw->ipc_env);
    ASSERT_ERR(txdesc_host);//TODO protect when host is NULL
    host = &(txdesc_host->host);

    memcpy((void *)host, &txhdr->host, sizeof(*host));

    len = 0;
    for (size_t i = 0; i < ARRAY_LEN(host->pbuf_chained_len); ++i) {
        if (host->pbuf_chained_len[i] == 0) {
            break;
        }
        memcpy((uint8_t *)txdesc_host->eth_packet + len, (void *)host->pbuf_chained_ptr[i], host->pbuf_chained_len[i]);
        len += host->pbuf_chained_len[i];

        host->pbuf_chained_ptr[i] = 0;
        host->pbuf_chained_len[i] = 0;
    }
    host->pbuf_chained_ptr[0] = (uint32_t)(&(txdesc_host->eth_packet[0]));
    host->pbuf_chained_len[0] = len;

    ipc_host_txdesc_push(bl_hw->ipc_env, p);
#ifdef CFG_BL_STATISTIC
    bl_hw->stats.cfm_balance++;
#endif
}
#else
void bl_tx_push(struct bl_hw *bl_hw, struct bl_txhdr *txhdr)
{
    volatile struct hostdesc *host;
    uint32_t* p = txhdr->p;
    volatile struct txdesc_host *txdesc_host;

    txdesc_host = ipc_host_txdesc_get(bl_hw->ipc_env);
    ASSERT_ERR(txdesc_host);//TODO protect when host is NULL
    host = &(txdesc_host->host);

    memcpy((void *)host, &txhdr->host, sizeof(*host));

    ipc_host_txdesc_push(bl_hw->ipc_env, p);
#ifdef CFG_BL_STATISTIC
    bl_hw->stats.cfm_balance++;
#endif
}
#endif

#define TXHDR_HODLER_LEN (8)
#define TXHDR_HODLER_MSK (0x7)
struct bl_txhdr *(txhdr_hodler[8]);
uint32_t txhdr_pos_r = 0;
uint32_t txhdr_pos_w = 0;

void bl_tx_resend()
{
    bl_os_enter_critical();
    while (txhdr_pos_r != txhdr_pos_w) {
        if (NULL == ipc_host_txdesc_get(bl_hw_static->ipc_env)) {
            break;
        }
        txhdr_hodler[txhdr_pos_r & TXHDR_HODLER_MSK]->status.value = 0;
#if 0
        bl_os_printf("Push back %p\r\n", txhdr_hodler[txhdr_pos_r & TXHDR_HODLER_MSK]);
#endif
        bl_tx_push(bl_hw_static, txhdr_hodler[txhdr_pos_r & TXHDR_HODLER_MSK]);
        txhdr_pos_r++;
    }
    bl_os_exit_critical();
}

void bl_tx_try_flush()
{
    struct bl_txhdr *txhdr;

    bl_os_enter_critical();
    while (ipc_host_txdesc_get(bl_hw_static->ipc_env)) {
        txhdr = (struct bl_txhdr*)utils_list_pop_front(&tx_list_bl);
        if (NULL == txhdr) {
            break;
        }
        bl_tx_push(bl_hw_static, txhdr);
    }
    bl_os_exit_critical();
}

int bl_txdatacfm(void *pthis, void *host_id)
{
#define RETRY_LIMIT_REACHED_BIT (1 << 16)
    struct pbuf *p = (struct pbuf*)host_id;
    struct bl_txhdr *txhdr;
    union bl_hw_txstatus bl_txst;

    txhdr = (struct bl_txhdr*)(((uint32_t)p->payload) + RWNX_HWTXHDR_ALIGN_PADS((uint32_t)p->payload));


    /* Read status in the TX control header */
    bl_txst = txhdr->status;

    if (bl_txst.value == 0) {
        return -1;
    }
    if (bl_txst.value & RETRY_LIMIT_REACHED_BIT) {
#if 0
        bl_os_printf("TX STATUS %08lX", bl_txst.value);
        bl_os_printf(" Retry reached %p:%lu:%lu", txhdr, txhdr_pos_r, txhdr_pos_w);
#endif
        /*we don't pbuf_free here, because we will resend this packet*/
        if (((txhdr_pos_w + 1) & TXHDR_HODLER_MSK) != (txhdr_pos_r & TXHDR_HODLER_MSK)) {
            bl_os_log_warn(" push back\r\n");
            txhdr_hodler[txhdr_pos_w & TXHDR_HODLER_MSK] = txhdr;
            txhdr_pos_w++;
        } else {
            bl_os_log_warn(" NOT push back when no mem\r\n");
            pbuf_free(p);
        }
    } else {
        pbuf_free(p);
        return 1;
    }

    return 0;
}

#if 0
uint32_t drop_ack_now = 0;
uint32_t droped_ack;
#endif

err_t bl_output(struct bl_hw *bl_hw, struct netif *netif, struct pbuf *p, int is_sta, struct bl_custom_tx_cfm *custom_cfm)
{
    struct bl_txhdr *txhdr;
    struct pbuf *q;
    uint32_t *link_header;
    uint32_t *eth_header;
    struct ethhdr *eth;
    struct hostdesc *host;
    int loop = 0;
    u8 tid;
    uint16_t packet_len;
    struct bl_sta *sta;

    eth_header = (uint32_t*)p->payload;
    eth = (struct ethhdr *)(eth_header);

    if (NULL == bl_hw || (0 == (NETIF_FLAG_LINK_UP & netif->flags) && eth->h_proto != 0x8e88)) {//TODO avoid call output when Wi-Fi is not ready
        bl_os_printf("[TX] wifi is down, return now\r\n");
        return ERR_CONN;
    }
#if 0
    if (drop_ack_now) {
        drop_ack_now++;
        if (drop_ack_now & 0x02) {
            droped_ack++;
            return ERR_OK;
        }
    }
#endif

    bl_hw_static = bl_hw;
    packet_len = p->tot_len;

    /*Make room in the header for tx*/
    if (pbuf_header(p, PBUF_LINK_ENCAPSULATION_HLEN)) {
        bl_os_printf("[TX] Reserve room failed for header\r\n");
        return ERR_IF;
    }
    /*Use aligned link_header*/
    link_header = (uint32_t*)(((uint32_t)p->payload) + RWNX_HWTXHDR_ALIGN_PADS((uint32_t)p->payload));
    if (16 + sizeof(struct bl_txhdr) + RWNX_HWTXHDR_ALIGN_PADS((uint32_t)p->payload) > PBUF_LINK_ENCAPSULATION_HLEN) {
        bl_os_printf("link_header size is %ld vs header %u\r\n",
                sizeof(struct bl_txhdr) + RWNX_HWTXHDR_ALIGN_PADS((uint32_t)p->payload),
                PBUF_LINK_ENCAPSULATION_HLEN
        );
    }

    tid = 0;//XXX set TID to 0 for quick test purpose

    txhdr = (struct bl_txhdr *)(link_header);
    memset(txhdr, 0, sizeof(struct bl_txhdr));
    host = &(txhdr->host);

    txhdr->p         = (uint32_t*)p;//XXX pattention to this filed
    // Fill-in the descriptor
    memcpy(&host->eth_dest_addr, eth->h_dest, ETH_ALEN);
    memcpy(&host->eth_src_addr, eth->h_source, ETH_ALEN);
    host->pbuf_addr = (uint32_t)p;
    host->ethertype = eth->h_proto;
    host->vif_idx = is_sta; // vif_idx field is used as is-sta flag here, fixed in ipc_emb
    host->flags = 0;
    host->packet_len = packet_len - sizeof(*eth);

    (void)sta;
    (void)tid;
    /*
    // fixed in ipc_emb
    if (is_sta) {
        host->staid = bl_hw->sta_idx;
        sta = &(bl_hw->sta_table[bl_hw->sta_idx]);
        if(sta->qos) {
            tid = 0;//XXX set TID to 0 for quick test purpose
        } else {
            tid = 0xFF;
        }

    } else {
        //AP mode need update whether support qos
        host->staid = bl_utils_idx_lookup(bl_hw, eth->h_dest);
    }
    host->tid = tid;
    */

    loop = 0;
    for (q = p; q != NULL; q = q->next) {
        if (0 == loop) {
            /*The first pbuf*/
            host->pbuf_chained_ptr[loop] = (uint32_t)(((uint8_t*)q->payload) + sizeof(*eth)) + PBUF_LINK_ENCAPSULATION_HLEN;//eth header is skipped in the header
            host->pbuf_chained_len[loop] = q->len - sizeof(*eth) - PBUF_LINK_ENCAPSULATION_HLEN;//eth header is skipped in the header
        } else {
            /*Chained pbuf after*/
            if (loop >= 4) {
                /*exceed the limit for pbuf chained*/
                bl_os_printf("[TX] [PBUF] Please fix for bigger chained pbuf, total_len %d\r\n",
                        p->tot_len
                );
            }
            host->pbuf_chained_ptr[loop] = (uint32_t)(q->payload);
            host->pbuf_chained_len[loop] = q->len;
#if 0
            bl_os_printf("[%d] ptr %p, size %lu\r\n",
                    0,
                    (void*)host->pbuf_chained_ptr[0],
                    host->pbuf_chained_len[0]
            );
            bl_os_printf("[%d] ptr %p, size %lu\r\n",
                    loop,
                    (void*)host->pbuf_chained_ptr[loop],
                    host->pbuf_chained_len[loop]
            );
#endif
        }
        loop++;
    }
    if (loop > 2) {
        bl_os_printf("[TX] [LOOP] Chain Used %d\r\n", loop);
    }

    txhdr->status.value = 0;
    /* Fill-in TX descriptor */
    host->packet_addr = (uint32_t)(0x11111111);//FIXME we use this magic for unvaild packet_addr
    host->status_addr = (uint32_t)(&(txhdr->status));

    if (custom_cfm) {
        memcpy(&txhdr->custom_cfm, custom_cfm, sizeof(*custom_cfm));
    }
    /*Ref this pbuf to avoid pbuf release*/
    pbuf_ref(p);

    bl_os_enter_critical();
    utils_list_push_back(&tx_list_bl, &(txhdr->item));
    bl_os_exit_critical();

    bl_irq_handler();

    return ERR_OK;
}
