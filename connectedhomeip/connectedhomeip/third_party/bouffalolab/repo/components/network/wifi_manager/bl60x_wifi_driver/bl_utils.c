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
#include <lwip/api.h>
#include <lwip/opt.h>
#include <lwip/def.h>
#include <lwip/mem.h>
#include <lwip/pbuf.h>
#include <lwip/netif.h>
#include <wifi_mgmr_ext.h>
#include <wifi_pkt_hooks.h>

#include "ipc_shared.h"
#include "ipc_host.h"
#include "bl_utils.h"
#include "bl_rx.h"
#include "bl_tx.h"
#include "bl_cmds.h"
#undef os_printf
#define os_printf(...) do {} while(0)

extern struct bl_hw wifi_hw;

#if 1
typedef struct bl_custom_pbuf
{
    struct pbuf_custom p;
    void *swdesc;
} bl_custom_pbuf_t;
#else
typedef struct bl_custom_pbuf
{
    struct pbuf_custom p;
    uint32_t payload[1800 >> 2];
} bl_custom_pbuf_t;
#undef LWIP_DECLARE_MEMORY_ALIGNED
#ifdef CONF_USER_ENABLE_PSRAM
#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[LWIP_MEM_ALIGN_BUFFER(size)] __attribute__((section(".wifi_psram")))
LWIP_MEMPOOL_DECLARE(RX_POOL, 100, sizeof(bl_custom_pbuf_t), "bl_zero_copy_rx");
#else
#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[LWIP_MEM_ALIGN_BUFFER(size)] __attribute__((section(".wifi_ram")))
LWIP_MEMPOOL_DECLARE(RX_POOL, 24, sizeof(bl_custom_pbuf_t), "bl_zero_copy_rx");
#endif
#endif

static void my_pbuf_free_custom(struct pbuf *p)
{
    bl_custom_pbuf_t* my_pbuf = (bl_custom_pbuf_t*)p;

void bl60x_firmwre_mpdu_free(void *swdesc);
    //bl_os_printf("--- cb free@%p\r\n", my_pbuf->swdesc);
    bl60x_firmwre_mpdu_free(my_pbuf->swdesc);
}

static void my_pbuf_free_custom_fake(struct pbuf *p)
{
    /*nothing needs to be done for tailed pbuf*/
}

static inline struct bl_vif *bl_rx_get_vif(int vif_idx)
{
    struct bl_vif *bl_vif = NULL;
    struct bl_hw *bl_hw = &wifi_hw;

    if (vif_idx == 0xFF) {
        list_for_each_entry(bl_vif, &bl_hw->vifs, list) {
            if (bl_vif->up)
                return bl_vif;
        }
        return NULL;
    } else if (vif_idx < NX_VIRT_DEV_MAX) {
        bl_vif = &(bl_hw->vif_table[vif_idx]);
        if (!bl_vif || !bl_vif->up)
            return NULL;
    }

    return bl_vif;
}

/**
 * bl_rx_mgmt - Process one 802.11 management frame
 *
 * @bl_hw: main driver data
 * @bl_vif: vif that received the buffer
 * @skb: skb received
 * @rxhdr: HW rx descriptor
 *
 * Process the management frame and free the corresponding skb
 */
static void bl_rx_mgmt(uint32_t *skb,  struct hw_rxhdr *hw_rxhdr, int len, bl_rx_info_t *info)
{
    struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)skb;
    static uint32_t counter;

#if 0
    if ( (0x01 == mgmt->sa[0] && 0x00 == mgmt->sa[1]) ||
         (0x01 == mgmt->da[0] && 0x00 == mgmt->da[1]) ||
         (0x01 == mgmt->bssid[0] && 0x00 == mgmt->bssid[1])) {

        bl_os_printf("[RX] %d, %08X %04X, DATA addr1: %02X:%02X:%02X:%02X:%02X:%02X addr2: %02X:%02X:%02X:%02X:%02X:%02X, addr3: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                len,
                (unsigned int)counter++,
                mgmt->frame_control,
                mgmt->da[0],
                mgmt->da[1],
                mgmt->da[2],
                mgmt->da[3],
                mgmt->da[4],
                mgmt->da[5],
                mgmt->sa[0],
                mgmt->sa[1],
                mgmt->sa[2],
                mgmt->sa[3],
                mgmt->sa[4],
                mgmt->sa[5],
                mgmt->bssid[0],
                mgmt->bssid[1],
                mgmt->bssid[2],
                mgmt->bssid[3],
                mgmt->bssid[4],
                mgmt->bssid[5]
        );
    }
#endif

    (void)counter;
	if (ieee80211_is_beacon(mgmt->frame_control)) {
#if 0
        os_printf("[RX] %04X BCN Received, freq %u, rssi %d\r\n",
                mgmt->frame_control,
                hw_rxhdr->phy_prim20_freq,
                hw_rxhdr->hwvect.rssi1
        );
#endif
    } else if ((ieee80211_is_deauth(mgmt->frame_control) ||
                ieee80211_is_disassoc(mgmt->frame_control)) &&
               (mgmt->u.deauth.reason_code == WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA ||
                mgmt->u.deauth.reason_code == WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA)) {
        if (ieee80211_is_deauth(mgmt->frame_control)) {
            os_printf("[RX] Deauth Received\r\n");
        } else if (ieee80211_is_disassoc(mgmt->frame_control)) {
            os_printf("[RX] Disassoc Received\r\n");
        }
    } else if ((ieee80211_is_action(mgmt->frame_control) &&
                (mgmt->u.action.category == 6))) {
        os_printf("[RX] FT EVENT\r\n");
    } else if (ieee80211_is_data(mgmt->frame_control)){
        os_printf("[RX] %08X %04X, DATA SA: %02X:%02X:%02X:%02X:%02X:%02X DA: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                (unsigned int)counter++,
                mgmt->frame_control,
                mgmt->sa[0],
                mgmt->sa[1],
                mgmt->sa[2],
                mgmt->sa[3],
                mgmt->sa[4],
                mgmt->sa[5],
                mgmt->da[0],
                mgmt->da[1],
                mgmt->da[2],
                mgmt->da[3],
                mgmt->da[4],
                mgmt->da[5]
        );
    } else if (ieee80211_is_data_qos(mgmt->frame_control)){
        bl_os_printf("[RX] %04X QOS DATA %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                mgmt->frame_control,
                mgmt->da[0],
                mgmt->da[1],
                mgmt->da[2],
                mgmt->da[3],
                mgmt->da[4],
                mgmt->da[5]
        );
    } else {
        os_printf("[RX] Upload MGMT Frame? %04X\r\n", mgmt->frame_control);
    }
}

struct wifi_pkt {
    uint32_t pkt[4];
    void *pbuf[4];
    uint16_t len[4];
};

static void dump_pkt_infor(struct hw_rxhdr *hw_rxhdr)
{
#define PACKET_TEST_INTERVAL     (4 * 1000)
    int16_t freq_offset = 0;
    static int32_t freq_offset_all = 0;
    uint16_t gain_status = 0;
    static uint32_t packets_num = 0;
    static uint32_t packets_lasttime = 0;

    gain_status = ((uint16_t)(hw_rxhdr->hwvect.rssi3)) | (((uint16_t)hw_rxhdr->hwvect.rssi4) << 8);

    if ((int32_t)bl_os_get_tick() - (int32_t)packets_lasttime > PACKET_TEST_INTERVAL) {
        packets_num = 0;
        freq_offset_all = 0;
    }
    packets_num++;
    packets_lasttime = bl_os_get_tick();

    if (hw_rxhdr->hwvect.format_mod >=2) {
        /*11n mode*/
        freq_offset = (((uint32_t) hw_rxhdr->hwvect.evm3) | (((uint32_t)hw_rxhdr->hwvect.evm4) << 8));
        freq_offset_all += ((int)(freq_offset * 20 / 2440));
        bl_os_printf("[11n] %04d bytes[%03lu], rssi %d, %04x, lna %02u, rbb %02u, dg %02d; evm3_4 %03d, freq_offset %d, ppm %f\r\n",
                hw_rxhdr->hwvect.len,
                packets_num,
                hw_rxhdr->hwvect.rssi1,
                gain_status,
                gain_status & 0xF,
                (gain_status >> 4) & 0x1F,
                (gain_status >> 9 & 0x7F),
                freq_offset,
                (int)(freq_offset * 20 / 2440),
                ((float) (freq_offset_all)) / packets_num
        );
    } else if (hw_rxhdr->hwvect.leg_rate > 3) {
        /*11g mode*/
        freq_offset = (((uint32_t) hw_rxhdr->hwvect.evm3) | (((uint32_t)hw_rxhdr->hwvect.evm4) << 8));
        freq_offset_all += ((int)(freq_offset * 20 / 2440));
        bl_os_printf("[11g] %04d bytes[%03lu], rssi %d, %04x, lna %02u, rbb %02u, dg %02d; evm3_4 %03d, freq_offset %d, ppm %f\r\n",
                hw_rxhdr->hwvect.len,
                packets_num,
                hw_rxhdr->hwvect.rssi1,
                gain_status,
                gain_status & 0xF,
                (gain_status >> 4) & 0x1F,
                (gain_status >> 9 & 0x7F),
                freq_offset,
                (int)(freq_offset * 20 / 2440),
                ((float)(freq_offset_all)) / packets_num
        );
    } else {
        /*11b mode*/
        freq_offset = ((int32_t)0) - (((int32_t)(hw_rxhdr->hwvect.evm3 << 24)) >> 24);
        freq_offset_all += ((int)(freq_offset * 0.7));
        bl_os_printf("[11b] %04d bytes[%03lu], fcs_err %d, rssi %d, %04x, lna %02u, rbb %02u, dg %02d; evm3 %04u:%03d, freq_offset %d, ppm %f\r\n",
                hw_rxhdr->hwvect.len,
                packets_num,
                hw_rxhdr->hwvect.fcs_err,
                hw_rxhdr->hwvect.rssi1,
                gain_status,
                gain_status & 0xF,
                (gain_status >> 4) & 0x1F,
                (gain_status >> 9 & 0x7F),
                hw_rxhdr->hwvect.evm3,
                freq_offset,
                (int)(freq_offset * 0.7),
                ((float)(freq_offset_all)) / packets_num
        );
    }
}

static inline struct pbuf *_handle_frame_from_stack_with_mempool(void *swdesc, unsigned int msdu_offset, struct wifi_pkt *pkt)
{
    struct pbuf *t, *h;
    int i = 0;

    h = pbuf_alloc(PBUF_RAW, pkt->len[0] - msdu_offset, PBUF_POOL);
    if (NULL == h) {
        printf("error mem1 ========================================== pbuf mem\r\n");
        return NULL;
    }
    pbuf_take(h, (uint8_t*)(pkt->pkt[0]) + msdu_offset, pkt->len[0] - msdu_offset);

    i = 1;//header is already set
    while (i < sizeof(pkt->pkt)/sizeof(pkt->pkt[0])) {
        if (0 == pkt->len[i]) {
            break;
        }
        t = pbuf_alloc(PBUF_RAW, pkt->len[i], PBUF_POOL);
        if (t) {
            pbuf_take(t, (uintptr_t*)pkt->pkt[i], pkt->len[i]);
            pbuf_cat(h, t);
            i++;
        } else {
            printf("error mem2 ====================================== pbuf mem\r\n");
            pbuf_free(h);
            return NULL;
        }
    }
    return h;
}

static inline struct pbuf *_handle_frame_from_stack_with_zerocopy(void *swdesc, unsigned int msdu_offset, struct wifi_pkt *pkt)
{
    struct pbuf *h, *t;
    int i;
    bl_custom_pbuf_t* my_pbuf;

    my_pbuf = (bl_custom_pbuf_t*)pkt->pbuf[0];
    memset(my_pbuf, 0, sizeof(bl_custom_pbuf_t));
    my_pbuf->p.custom_free_function = my_pbuf_free_custom;
    my_pbuf->swdesc = swdesc;
    h = pbuf_alloced_custom(
            PBUF_RAW,
            pkt->len[0] - msdu_offset,
            PBUF_REF,
            &my_pbuf->p,
            (uint8_t*)(pkt->pkt[0]) + msdu_offset,
            pkt->len[0] - msdu_offset
    );

    i = 1;//header is already set
    while (i < sizeof(pkt->pkt)/sizeof(pkt->pkt[0])) {
        if (0 == pkt->len[i]) {
            break;
        }
        my_pbuf = (bl_custom_pbuf_t*)pkt->pbuf[i];
        memset(my_pbuf, 0, sizeof(bl_custom_pbuf_t));
        my_pbuf->p.custom_free_function = my_pbuf_free_custom_fake;
        t = pbuf_alloced_custom(
                PBUF_RAW,
                pkt->len[i],
                PBUF_REF,
                &my_pbuf->p,
                (uint8_t*)(pkt->pkt[i]),
                pkt->len[i]
        );
        pbuf_cat(h, t);
        i++;
    }
    return h;
}

#define MAC_FMT "%02X%02X%02X%02X%02X%02X"
#define MAC_LIST(arr) (arr)[0], (arr)[1], (arr)[2], (arr)[3], (arr)[4], (arr)[5]

static int tcpip_src_addr_cmp(struct ethhdr *hdr, uint8_t addr[])
{
    int i;

    for (i = 0; i < 6; i++) {
        if ((uint8_t)(hdr->h_source[i]) != (uint8_t)(addr[i])) {
            return 1;
        }
    }

    return 0;
}

int tcpip_stack_input(void *swdesc, uint8_t status, void *hwhdr, unsigned int msdu_offset, struct wifi_pkt *pkt, uint8_t extra_status)
{
    struct hw_rxhdr *hw_rxhdr = (struct hw_rxhdr*)hwhdr;

    uint32_t *skb = (uint32_t*)(pkt->pkt[0]), *skb_payload;
    struct bl_vif *bl_vif;
    bool sniffer = false;
    bool free_by_lowlayer = true;
    bool zerocopy;
    struct pbuf *h;
    bl_rx_info_t info;

    /* Check if we need to forward the buffer */
    if (!(status & RX_STAT_FORWARD)) {
        goto end;
    }

    bl_vif = bl_rx_get_vif(hw_rxhdr->flags_vif_idx);
    skb_payload = (uint32_t*)((uint32_t)(skb) + msdu_offset);

    if (hw_rxhdr->flags_is_80211_mpdu) {
        sniffer = true;
    }
    if (!sniffer) {
        struct ethhdr *hdr = (struct ethhdr *)(skb_payload);
        (void)hdr;

        if (hw_rxhdr->flags_sta_idx != 0xff) {
            if (hw_rxhdr->flags_is_4addr) {
                bl_os_printf("[RX] Trigger 4addr unexpected frame\r\n");
            }
        }
        os_printf("********************ETH Start******************************\r\n");
        os_printf("  Eth Dst " MAC_FMT "\r\n", MAC_LIST(hdr->h_dest));
        os_printf("  Eth Src " MAC_FMT "\r\n", MAC_LIST(hdr->h_dest));
        os_printf("  Eth Proto %04X, %p:%p, len %u\r\n", hdr->h_proto, bl_vif, bl_vif ? bl_vif->dev : NULL, hw_rxhdr->hwvect.len);
        os_printf("********************ETH End******************************\r\n");
        if (wifi_mgmr_ext_dump_needed()) {
            dump_pkt_infor(hw_rxhdr);
        }
    }

    if (!sniffer && !bl_vif) {
        bl_os_printf("------ Frame received but no active vif (%d)\r\n", hw_rxhdr->flags_vif_idx);
        goto end;
    }

#if defined(CFG_CHIP_BL808)
    h = _handle_frame_from_stack_with_mempool(swdesc, msdu_offset, pkt);
    zerocopy = false;
#elif defined(CFG_CHIP_BL606P)
    h = _handle_frame_from_stack_with_mempool(swdesc, msdu_offset, pkt);
    zerocopy = false;
#else
    h = _handle_frame_from_stack_with_zerocopy(swdesc, msdu_offset, pkt);
    zerocopy = true;
#endif
    if (!h) {
        // wrapping in pbuf failed, free the packet
        goto end;
    }

    if (extra_status & BL_RX_STATUS_AMSDU) {
        h->flags |= PBUF_FLAG_AMSDU;
    }
    if (sniffer) {
        info.rssi = hw_rxhdr->hwvect.rssi1;
        info.leg_rate = hw_rxhdr->hwvect.leg_rate;
        info.format_mod = hw_rxhdr->hwvect.format_mod;
        //TODO fix splitted buff in zerocopy
        bl_rx_pkt_cb((uint8_t*)skb_payload, hw_rxhdr->hwvect.len, (void *)h, &info);
        bl_rx_mgmt(skb_payload, hw_rxhdr, hw_rxhdr->hwvect.len, &info);
        pbuf_free(h);
    } else {
#ifdef PKT_INPUT_HOOK
        if (bl_wifi_pkt_eth_input_hook) {
            bool is_sta = bl_vif->dev == wifi_mgmr_sta_netif_get();
            h = bl_wifi_pkt_eth_input_hook(is_sta, h, bl_wifi_pkt_eth_input_hook_arg);
            if (h == NULL) {
                // hook dropped the packet
                goto free;
            }
        }
#endif
        struct ethhdr *hdr = (struct ethhdr *)(skb_payload);
        if (bl_vif->dev && tcpip_src_addr_cmp(hdr, (bl_vif->dev)->hwaddr) && ERR_OK == bl_vif->dev->input(h, bl_vif->dev)) {
            //TCP/IP stack will take care of pbuf h
        } else {
            //No none need pbuf h anymore, so free it now
            pbuf_free(h);
        }
    }

    goto free; // In case of error that label free is defined but not used when PKT_INPUT_HOOK is disabled
free:
    if (zerocopy) {
        free_by_lowlayer = false;
    }
end:
    if (free_by_lowlayer) {
        return -1;
    } else {
        return 0;
    }
}

u8 bl_radarind(void *pthis, void *hostid)
{
    os_printf("%s: Enter\r\n", __func__);
    return 0;
}

u8 bl_msgackind(void *pthis, void *hostid)
{
    struct bl_hw *bl_hw = (struct bl_hw *)pthis;

    os_printf("[IPC] MSG ACKED @%p\r\n", hostid);
    bl_hw->cmd_mgr.llind(&bl_hw->cmd_mgr, (struct bl_cmd *)hostid);
    return 0;
}

u8 bl_dbgind(void *pthis, void *hostid)
{
    os_printf("%s: Enter\r\n", __func__);
    return 0;
}

void bl_prim_tbtt_ind(void *pthis)
{
    os_printf("%s: Enter\r\n", __func__);
    return;
}
void bl_sec_tbtt_ind(void *pthis)
{
    os_printf("%s: Enter\r\n", __func__);
    return;
}

//FIXME TODO use cache?
int bl_utils_idx_lookup(struct bl_hw *bl_hw, uint8_t *mac)
{
    int i;
    struct bl_sta *sta;

    for (i = 0; i < sizeof(bl_hw->sta_table)/sizeof(bl_hw->sta_table[0]); i++) {
        sta = &(bl_hw->sta_table[i]);
        if (0 == sta->is_used) {
            /*empty entry*/
            continue;
        }
        if (memcmp(sta->sta_addr.array, mac, 6)) {
            /*NOT match*/
            continue;
        } else {
            /*mac address found*/
            break;
        }
    }

    //FIXME use 0x0A for un-valid sta_idx?
    return (sizeof(bl_hw->sta_table)/sizeof(bl_hw->sta_table[0])) == i ? wifi_hw.ap_bcmc_idx : i;
}

static struct ipc_host_env_tag *ipc_env;

int bl_ipc_init(struct bl_hw *bl_hw, struct ipc_shared_env_tag *ipc_shared_mem)
{
    struct ipc_host_cb_tag cb;

    memset(&cb, 0, sizeof(cb));
    /* initialize the API interface */
    cb.recv_data_ind   = NULL;
    cb.recv_radar_ind  = bl_radarind;
    cb.recv_msg_ind    = NULL;
    cb.recv_msgack_ind = bl_msgackind;
    cb.recv_dbg_ind    = bl_dbgind;
    cb.send_data_cfm   = bl_txdatacfm;
    cb.prim_tbtt_ind   = bl_prim_tbtt_ind;
    cb.sec_tbtt_ind    = bl_sec_tbtt_ind;

    /* set the IPC environment */
    bl_hw->ipc_env = (struct ipc_host_env_tag *) bl_os_malloc(sizeof(struct ipc_host_env_tag));
    ipc_env = bl_hw->ipc_env;

    /* call the initialization of the IPC */
    ipc_host_init(bl_hw->ipc_env, &cb, ipc_shared_mem, bl_hw);

    bl_cmd_mgr_init(&bl_hw->cmd_mgr);
    return 0;
}

void bl_utils_dump(void)
{
    int i, cnt;
    struct pbuf *p;
    struct bl_txhdr *txhdr;

    bl_os_puts("---------- bl_utils_dump -----------\r\n");

    bl_os_printf("txdesc_free_idx: %lu(%lu)\r\n",
            ipc_env->txdesc_free_idx,
            ipc_env->txdesc_free_idx & (NX_TXDESC_CNT0 - 1)
    );
    bl_os_printf("txdesc_used_idx: %lu(%lu)\r\n",
            ipc_env->txdesc_used_idx,
            ipc_env->txdesc_used_idx & (NX_TXDESC_CNT0 - 1)
    );
    cnt = sizeof(ipc_env->tx_host_id0)/sizeof(ipc_env->tx_host_id0[0]);
    bl_os_printf("tx_host_id0 cnt: %d(used %ld)\r\n",
            cnt,
            (int32_t)ipc_env->txdesc_free_idx - (int32_t)ipc_env->txdesc_used_idx
    );
    bl_os_puts(  "  list:   pbuf    status ptr  status\r\n");
    for (i = 0; i < cnt; i++) {
        if (ipc_env->txdesc_used_idx + i == ipc_env->txdesc_free_idx) {
            /*break on empty*/
            break;
        }
        p = (struct pbuf*)(ipc_env->tx_host_id0[(ipc_env->txdesc_used_idx + i) & (NX_TXDESC_CNT0 - 1)]);
        txhdr = (struct bl_txhdr*)(((uint32_t)p->payload) + RWNX_HWTXHDR_ALIGN_PADS((uint32_t)p->payload));
        bl_os_printf("    [%lu]%p(%p:%08lX)\r\n",
                (ipc_env->txdesc_used_idx + i) & (NX_TXDESC_CNT0 - 1),
                p,
                p ? (void*)(txhdr->host.status_addr) : 0,
                p ? txhdr->status.value : 0
        );
    }
    bl_os_puts("========== bl_utils_dump End =======\r\n");
}
