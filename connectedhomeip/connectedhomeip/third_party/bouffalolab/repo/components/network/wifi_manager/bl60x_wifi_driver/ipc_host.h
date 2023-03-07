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
#ifndef __IPC_HOST_H__
#define __IPC_HOST_H__
#include <stdint.h>
#include "ipc_shared.h"

enum ipc_host_desc_status
{
    /// Descriptor is IDLE
    IPC_HOST_DESC_IDLE      = 0,
    /// Data can be forwarded
    IPC_HOST_DESC_FORWARD,
    /// Data has to be kept in UMAC memory
    IPC_HOST_DESC_KEEP,
    /// Delete stored packet
    IPC_HOST_DESC_DELETE,
    /// Update Frame Length status
    IPC_HOST_DESC_LEN_UPDATE,
};


/**
 ****************************************************************************************
 *
 * @file ipc_host.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

struct ipc_host_cb_tag
{
    /// WLAN driver call-back function: send_data_cfm
    int (*send_data_cfm)(void *pthis, void *host_id);

    /// WLAN driver call-back function: recv_data_ind
    uint8_t (*recv_data_ind)(void *pthis, void *host_id);

    /// WLAN driver call-back function: recv_radar_ind
    uint8_t (*recv_radar_ind)(void *pthis, void *host_id);

    /// WLAN driver call-back function: recv_msg_ind
    uint8_t (*recv_msg_ind)(void *pthis, void *host_id);

    /// WLAN driver call-back function: recv_msgack_ind
    uint8_t (*recv_msgack_ind)(void *pthis, void *host_id);

    /// WLAN driver call-back function: recv_dbg_ind
    uint8_t (*recv_dbg_ind)(void *pthis, void *host_id);

    /// WLAN driver call-back function: prim_tbtt_ind
    void (*prim_tbtt_ind)(void *pthis);

    /// WLAN driver call-back function: sec_tbtt_ind
    void (*sec_tbtt_ind)(void *pthis);

};

/*
 * Struct used to store information about host buffers (DMA Address and local pointer)
 */
struct ipc_hostbuf
{
    void    *hostid;     ///< ptr to hostbuf client (ipc_host client) structure
    uint32_t dma_addr;   ///< ptr to real hostbuf dma address
};


/// Definition of the IPC Host environment structure.
struct ipc_host_env_tag
{
    /// Structure containing the callback pointers
    struct ipc_host_cb_tag cb;

    /// Pointer to the shared environment
    struct ipc_shared_env_tag *shared;

    // Array used to store the descriptor addresses
    struct ipc_hostbuf ipc_host_rxdesc_array[IPC_RXDESC_CNT];
    // Index of the host RX descriptor array (ipc_shared environment)
    uint8_t ipc_host_rxdesc_idx;
    /// Store the number of RX Descriptors
    uint8_t rxdesc_nb;

    // Index used for ipc_host_rxbuf_array to point to current buffer
    uint8_t ipc_host_rxbuf_idx;
    // Store the number of Rx Data buffers
    uint32_t rx_bufnb;
    // Store the size of the Rx Data buffers
    uint32_t rx_bufsz;

    // Index used that points to the first free TX desc
    uint32_t txdesc_free_idx;
    // Index used that points to the first used TX desc
    uint32_t txdesc_used_idx;
    // Array storing the currently pushed host ids for the BK queue
    void *tx_host_id0[NX_TXDESC_CNT0];
    // Pointer to the different host ids arrays, per IPC queue
    void **tx_host_id;
    // Pointer to the different TX descriptor arrays, per IPC queue
    volatile struct txdesc_host *txdesc;

    /// Fields for Emb->App MSGs handling
    // Global array used to store the hostid and hostbuf addresses for msg/ind
    struct ipc_hostbuf ipc_host_msgbuf_array[IPC_MSGE2A_BUF_CNT];
    // Index of the MSG E2A buffers array to point to current buffer
    uint8_t ipc_host_msge2a_idx;
    // Store the number of E2A MSG buffers
    uint32_t ipc_e2amsg_bufnb;
    // Store the size of the E2A MSG buffers
    uint32_t ipc_e2amsg_bufsz;

    /// E2A ACKs of A2E MSGs
    uint8_t msga2e_cnt;
    void *msga2e_hostid;

    /// Fields for Debug MSGs handling
    // Global array used to store the hostid and hostbuf addresses for Debug messages
    struct ipc_hostbuf ipc_host_dbgbuf_array[IPC_DBGBUF_CNT];
    // Index of the Debug messages buffers array to point to current buffer
    uint8_t ipc_host_dbg_idx;
    // Store the number of Debug messages buffers
    uint32_t ipc_dbg_bufnb;
    // Store the size of the Debug messages buffers
    uint32_t ipc_dbg_bufsz;

    /// Pointer to the attached object (used in callbacks and register accesses)
    void *pthis;
};

void ipc_host_init(struct ipc_host_env_tag *env,
                  struct ipc_host_cb_tag *cb,
                  struct ipc_shared_env_tag *shared_env_ptr,
                  void *pthis);
int ipc_host_msg_push(struct ipc_host_env_tag *env, void *msg_buf, uint16_t len);
uint32_t ipc_host_get_status(struct ipc_host_env_tag *env);
uint32_t ipc_host_get_rawstatus(struct ipc_host_env_tag *env);
volatile struct txdesc_host *ipc_host_txdesc_get(struct ipc_host_env_tag *env);
void ipc_host_txdesc_push(struct ipc_host_env_tag *env, void *host_id);
void ipc_host_irq(struct ipc_host_env_tag *env, uint32_t status);
void ipc_host_enable_irq(struct ipc_host_env_tag *env, uint32_t value);
void ipc_host_disable_irq(struct ipc_host_env_tag *env, uint32_t value);

/**
 ******************************************************************************
 * @brief Push a pre-allocated buffer descriptor for IPC MSGs (host side)
 *
 * This function is called at Init time to initialize all Emb2App messages
 * buffers. Then each time embedded send a IPC message, this function is used
 * to push back the same buffer once it has been handled.
 *
 * @param[in]   env         Pointer to the IPC host environment
 * @param[in]   hostid      Address of buffer for host
 * @param[in]   hostbuf     Address of buffer for embedded
 *                          The length of this buffer should be predefined
 *                          between host and emb statically.
 *
 ******************************************************************************
 */
int ipc_host_msgbuf_push(struct ipc_host_env_tag *env, void *hostid, uint32_t hostbuf);

void ipc_host_patt_addr_push(struct ipc_host_env_tag *env, uint32_t addr);

/**
 ******************************************************************************
 * @brief Push a pre-allocated buffer descriptor for Rx packet (host side)
 *
 * This function should be called by the host IRQ handler to supply the
 * embedded side with new empty buffer.
 *
 * @param[in]   env         Pointer to the IPC host environment
 * @param[in]   hostid      Packet ID used by the host (skbuff pointer on Linux)
 * @param[in]   hostbuf     Pointer to the start of the buffer payload in the
 *                          host memory (this may be inferred from the skbuff?)
 *                          The length of this buffer should be predefined
 *                          between host and emb statically (constant needed?).
 *
 ******************************************************************************
 */
int ipc_host_rxbuf_push(struct ipc_host_env_tag *env, uint32_t hostid, uint32_t hostbuf);

/**
 ******************************************************************************
 * @brief Push a pre-allocated Descriptor
 *
 * This function should be called by the host IRQ handler to supply the
 * embedded side with new empty buffer.
 *
 * @param[in]   env         Pointer to the IPC host environment
 * @param[in]   hostid      Address of packet for host
 * @param[in]   hostbuf     Pointer to the start of the buffer payload in the
 *                          host memory. The length of this buffer should be
 *                          predefined between host and emb statically.
 *
 ******************************************************************************
 */
int ipc_host_rxdesc_push(struct ipc_host_env_tag *env, void *hostid, uint32_t hostbuf);
int ipc_host_txdesc_left(struct ipc_host_env_tag *env, const int queue_idx, const int user_pos);
#endif
