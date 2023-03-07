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
#include <stdio.h>
#include <string.h>
#include <utils_list.h>
#include <lwip/pbuf.h>

#include "ipc_host.h"
#include "reg_ipc_app.h"
#include "bl_cmds.h"
#include "bl_utils.h"
#include "bl_tx.h"
#include "bl_os_private.h"

#define REG_SW_SET_PROFILING(env, value)   do{  }while(0)
#define REG_SW_CLEAR_PROFILING(env, value)   do{  }while(0)
#define REG_SW_CLEAR_HOSTBUF_IDX_PROFILING(env)   do{  }while(0)
#define REG_SW_SET_HOSTBUF_IDX_PROFILING(env, val)   do{  }while(0)
#define REG_SW_SET_PROFILING_CHAN(env, bit)            do {} while (0)
#define REG_SW_CLEAR_PROFILING_CHAN(env, bit)          do {} while (0)

#undef os_printf
#define os_printf(...) do {} while(0)

extern struct utils_list tx_list_bl;

static const int nx_txdesc_cnt[] =
{
    NX_TXDESC_CNT0,
    NX_TXDESC_CNT1,
    NX_TXDESC_CNT2,
    NX_TXDESC_CNT3,
    #if NX_TXQ_CNT == 5
    NX_TXDESC_CNT4,
    #endif
};

static const int nx_txdesc_cnt_msk[] =
{
    NX_TXDESC_CNT0 - 1,
    NX_TXDESC_CNT1 - 1,
    NX_TXDESC_CNT2 - 1,
    NX_TXDESC_CNT3 - 1,
    #if NX_TXQ_CNT == 5
    NX_TXDESC_CNT4 - 1,
    #endif
};

void ipc_host_init(struct ipc_host_env_tag *env,
                  struct ipc_host_cb_tag *cb,
                  struct ipc_shared_env_tag *shared_env_ptr,
                  void *pthis)
{

extern int internel_cal_size_tx_desc;
extern int internel_cal_size_tx_hdr;
    bl_os_printf("[IPC] [TX] Low level size %d, driver size %d, total size %d\r\n",
            internel_cal_size_tx_desc,
            internel_cal_size_tx_hdr,
            internel_cal_size_tx_desc + internel_cal_size_tx_hdr
    );
    utils_list_init(&tx_list_bl);
#if 0
    
/**
 ****************************************************************************************
 *
 * @file ipc_host.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

    memset(shared_env_ptr, 0, sizeof(struct ipc_shared_env_tag));
#else
#if 0
    unsigned int * dst;
    unsigned int size;
    dst = (unsigned int *)shared_env_ptr;
    size = (unsigned int)sizeof(struct ipc_shared_env_tag);
    for (i=0; i < size; i+=4)
    {
        *dst++ = 0;
    }
#endif
#endif
    // Reset the IPC Host environment
    memset(env, 0, sizeof(struct ipc_host_env_tag));

    // Initialize the shared environment pointer
    env->shared = shared_env_ptr;

    // Save the callbacks in our own environment
    env->cb = *cb;

    // Save the pointer to the register base
    env->pthis = pthis;

    // Initialize buffers numbers and buffers sizes needed for DMA Receptions
    env->rx_bufnb = IPC_RXBUF_CNT;
    env->rx_bufsz = IPC_RXBUF_SIZE;
    env->rxdesc_nb = IPC_RXDESC_CNT;
    env->ipc_e2amsg_bufnb = IPC_MSGE2A_BUF_CNT;
    env->ipc_e2amsg_bufsz = sizeof(struct ipc_e2a_msg);

    // Initialize the pointers to the hostid arrays
    env->tx_host_id = env->tx_host_id0;

    // Initialize the pointers to the TX descriptor arrays
    env->txdesc = shared_env_ptr->txdesc0;
    memset((void*)&(shared_env_ptr->txdesc0), 0, sizeof(shared_env_ptr->txdesc0));
}

int ipc_host_msg_push(struct ipc_host_env_tag *env, void *msg_buf, uint16_t len)
{
    int i;
    uint32_t *src, *dst;

    REG_SW_SET_PROFILING(env->pthis, SW_PROF_IPC_MSGPUSH);

    ASSERT_ERR(!env->msga2e_hostid);
    ASSERT_ERR(round_up(len, 4) <= sizeof(env->shared->msg_a2e_buf.msg));

    // Copy the message into the IPC MSG buffer
#if 1
    src = (uint32_t*)((struct bl_cmd *)msg_buf)->a2e_msg;
#else
    src = (uint32_t*) msg_buf;
#endif
    dst = (uint32_t*)&(env->shared->msg_a2e_buf.msg);

    // Copy the message in the IPC queue
    for (i=0; i<len; i+=4)
    {
        *dst++ = *src++;
    }

    env->msga2e_hostid = msg_buf;

    // Trigger the irq to send the message to EMB
    ipc_app2emb_trigger_set(IPC_IRQ_A2E_MSG);

    REG_SW_CLEAR_PROFILING(env->pthis, SW_PROF_IPC_MSGPUSH);

    return 0;
}

void ipc_host_patt_addr_push(struct ipc_host_env_tag *env, uint32_t addr)
{
    struct ipc_shared_env_tag *shared_env_ptr = env->shared;

    // Copy the address
    shared_env_ptr->pattern_addr = addr;
}

uint32_t ipc_host_get_status(struct ipc_host_env_tag *env)
{
    uint32_t status;

    status = ipc_emb2app_status_get(env->shared);

    return status;
} 

uint32_t ipc_host_get_rawstatus(struct ipc_host_env_tag *env)
{
    uint32_t status;

    status = ipc_emb2app_rawstatus_get(env->shared);

    return status;
} 

static void ipc_host_msgack_handler(struct ipc_host_env_tag *env)
{
    void *hostid = env->msga2e_hostid;

    ASSERT_ERR(hostid);
    ASSERT_ERR(env->msga2e_cnt == (((struct lmac_msg *)(&env->shared->msg_a2e_buf.msg))->src_id & 0xFF));

    env->msga2e_hostid = NULL;
    env->msga2e_cnt++;
    env->cb.recv_msgack_ind(env->pthis, hostid);
}

static void ipc_host_tx_cfm_handler(struct ipc_host_env_tag *env, const int queue_idx, const int user_pos)
{
    int ret;
    bl_custom_tx_callback_t custom_tx_callback;
    void *custom_tx_callback_arg;
    // TX confirmation descriptors have been received
    REG_SW_SET_PROFILING(env->pthis, SW_PROF_IRQ_E2A_TXCFM);
    while (1) {
        uint32_t used_idx = env->txdesc_used_idx;
        void *host_id = env->tx_host_id[used_idx & nx_txdesc_cnt_msk[queue_idx]];
        struct pbuf *p = (struct pbuf*)host_id;
        struct bl_txhdr *txhdr;

        // bl_os_log_info("[IRQ-BH] CFM handler host id: %p\r\n", host_id);
        if (host_id == 0) {
            // bl_os_log_info("[IRQ-BH] ipc_host_tx_cfm_handler break\r\n");
            break;
        }

        // bl_os_log_info("[IRQ-BH] send_data_cfm cb\r\n");

        txhdr = (struct bl_txhdr*)(((uint32_t)p->payload) + RWNX_HWTXHDR_ALIGN_PADS((uint32_t)p->payload));
        custom_tx_callback = txhdr->custom_cfm.cb;
        custom_tx_callback_arg = txhdr->custom_cfm.cb_arg;

        ret = env->cb.send_data_cfm(env->pthis, host_id);
        if (ret < 0) {
            break;
        }
        // Reset the host id in the array
        env->tx_host_id[used_idx & nx_txdesc_cnt_msk[queue_idx]] = 0;
        /*current txdesc is confirmed, so increase the idx now*/
        env->txdesc_used_idx++;

        /*Notify tx status*/
        if (custom_tx_callback) {
            custom_tx_callback(custom_tx_callback_arg, ret > 0);
        }

        REG_SW_SET_PROFILING_CHAN(env->pthis, SW_PROF_CHAN_CTXT_CFM_HDL_BIT);
        REG_SW_CLEAR_PROFILING_CHAN(env->pthis, SW_PROF_CHAN_CTXT_CFM_HDL_BIT);
    }

    REG_SW_CLEAR_PROFILING(env->pthis, SW_PROF_IRQ_E2A_TXCFM);
}

static void ipc_host_radar_handler(struct ipc_host_env_tag *env)
{
    /*empty here*/
}

static void ipc_host_dbg_handler(struct ipc_host_env_tag *env)
{
    // For profiling
    REG_SW_SET_PROFILING(env->pthis, SW_PROF_IRQ_E2A_DBG);

    // LMAC has triggered an IT saying that a DBG message has been sent to upper layer.
    // Then we first need to check the validity of the current buffer, and the validity
    // of the next buffers too, because it is likely that several buffers have been
    // filled within the time needed for this irq handling
    // call the external function to indicate that a RX packet is received
    while(env->cb.recv_dbg_ind(env->pthis,
            env->ipc_host_dbgbuf_array[env->ipc_host_dbg_idx].hostid) == 0)
        ;

    // For profiling
    REG_SW_CLEAR_PROFILING(env->pthis, SW_PROF_IRQ_E2A_DBG);
}

#if 1
uint32_t used_issue  = 0;
#endif

volatile struct txdesc_host *ipc_host_txdesc_get(struct ipc_host_env_tag *env)
{
    volatile struct txdesc_host *txdesc_free;
    uint32_t used_idx = env->txdesc_used_idx;
    uint32_t free_idx = env->txdesc_free_idx;

    os_printf("free_idx is %u(%u), used_idx is %u(%u), cnt is %u\r\n",
        free_idx, 
        free_idx & nx_txdesc_cnt_msk[0],
        used_idx,
        used_idx & nx_txdesc_cnt_msk[0],
        nx_txdesc_cnt[0]
    );
    if (used_idx > free_idx) {
        os_printf("[TX] used_idx case found, used:free %u:%u\r\n", used_idx, free_idx);
        used_issue++;
    }

    // Check if a free descriptor is available
    if (free_idx != (used_idx + nx_txdesc_cnt[0]))
    {
        // Get the pointer to the first free descriptor
        txdesc_free = env->txdesc + (free_idx & nx_txdesc_cnt_msk[0]);
        if ((free_idx - used_idx) <= nx_txdesc_cnt[0]) {
            /*empty here*/
        } else {
            /*idx error*/
            while (1) {
                os_printf("TX IPC HOST race condition\r\rn");
            }
        }
    }
    else
    {
        txdesc_free = NULL;
    }
    os_printf("txdesc_free is %p\r\n", txdesc_free);

    return txdesc_free;
}

int ipc_host_txdesc_left(struct ipc_host_env_tag *env, const int queue_idx, const int user_pos)
{
    uint32_t used_idx = env->txdesc_used_idx;
    uint32_t free_idx = env->txdesc_free_idx;

    return nx_txdesc_cnt[queue_idx] - (free_idx - used_idx);
}

void ipc_host_txdesc_push(struct ipc_host_env_tag *env, void *host_id)
{
    uint32_t free_idx = env->txdesc_free_idx & nx_txdesc_cnt_msk[0];
    volatile struct txdesc_host *txdesc_pushed = env->txdesc + free_idx;


    // Descriptor is now ready
    txdesc_pushed->ready = 0xFFFFFFFF;

    // Save the host id in the environment
    env->tx_host_id[free_idx] = host_id;

    // Increment the index
    env->txdesc_free_idx++;

    os_printf("use free_idx %u\r\n", free_idx);

    // trigger interrupt!!!
    //REG_SW_SET_PROFILING(env->pthis, CO_BIT(queue_idx+SW_PROF_IRQ_A2E_TXDESC_FIRSTBIT));
    ipc_app2emb_trigger_setf(CO_BIT(IPC_IRQ_A2E_TXDESC_FIRSTBIT));
}

void ipc_host_irq(struct ipc_host_env_tag *env, uint32_t status)
{
    // Acknowledge the pending interrupts
    ipc_emb2app_ack_clear(status);
    // And re-read the status, just to be sure that the acknowledgment is
    // effective when we start the interrupt handling
    // XXX status reload
    status |= ipc_emb2app_status_get();

    // Optimized for only one IRQ at a time
    if (status & IPC_IRQ_E2A_TXCFM)
    {
        int i;

        os_printf("[IRQ-BH] IPC_IRQ_E2A_TXCFM\r\n");
#if 0
        spin_lock(&((struct bl_hw *)env->pthis)->tx_lock);
#endif
        // handle the TX confirmation reception
        for (i = 0; i < IPC_TXQUEUE_CNT; i++) {
            uint32_t q_bit = CO_BIT(i + IPC_IRQ_E2A_TXCFM_POS);
            if (status & q_bit) {
                // handle the confirmation
                os_printf("[IRQ-BH] CFM handler\r\n");
                ipc_host_tx_cfm_handler(env, i, 0);
            }
        }
#if 0
        spin_unlock(&((struct bl_hw *)env->pthis)->tx_lock);
#endif
    }
    void bl_tx_resend(void);
    bl_tx_resend();

    if (status & IPC_IRQ_E2A_RXDESC) {
        os_printf("[IRQ-BH] IPC_IRQ_E2A_RXDESC\r\n");
    }
    if (status & IPC_IRQ_E2A_MSG_ACK) {
        os_printf("[IRQ-BH] IPC_IRQ_E2A_MSG_ACK\r\n");
        ipc_host_msgack_handler(env);
    }
    if (status & IPC_IRQ_E2A_RADAR) {
        os_printf("[IRQ-BH] IPC_IRQ_E2A_RADAR\r\n");
        ipc_host_radar_handler(env);
    }
    if (status & IPC_IRQ_E2A_DBG) {
        os_printf("[IRQ-BH] IPC_IRQ_E2A_DBG\r\n");
        ipc_host_dbg_handler(env);
    }
    if (status & IPC_IRQ_E2A_TBTT_PRIM) {
        os_printf("[IRQ-BH] IPC_IRQ_E2A_TBTT_PRIM\r\n");
        env->cb.prim_tbtt_ind(env->pthis);
    }
    if (status & IPC_IRQ_E2A_TBTT_SEC) {
        os_printf("[IRQ-BH] IPC_IRQ_E2A_TBTT_SEC\r\n");
        env->cb.sec_tbtt_ind(env->pthis);
    }
}

void ipc_host_enable_irq(struct ipc_host_env_tag *env, uint32_t value)
{
    // Enable the handled interrupts
    ipc_emb2app_unmask_set(value);
}

void ipc_host_disable_irq(struct ipc_host_env_tag *env, uint32_t value)
{
    // Enable the handled interrupts
    ipc_emb2app_unmask_clear(value);
}

void ipc_host_disable_irq_e2a(void)
{
    ipc_emb2app_unmask_clear(IPC_IRQ_E2A_ALL);
}

