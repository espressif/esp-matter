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
#ifndef __RWNX_UTILS_H__
#define __RWNX_UTILS_H__
#include "bl_defs.h"
#ifdef CONFIG_RWNX_DBG

/**
 ****************************************************************************************
 *
 * @file bl_utils.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#define RWNX_DBG os_printf
#else
#define RWNX_DBG(a...) do {} while (0)
#endif

#define RWNX_FN_ENTRY_STR ">>> %s()\r\n", __func__
#define RWNX_FN_LEAVE_STR "<<< %s()\r\n", __func__

#define RWNX_RXBUFF_PATTERN     (0xCAFEFADE)
#define RWNX_RXBUFF_VALID_IDX(idx) ((idx) < RWNX_RXBUFF_MAX)
/* Used to ensure that hostid set to fw is never 0 */
#define RWNX_RXBUFF_IDX_TO_HOSTID(idx) ((idx) + 1)
#define RWNX_RXBUFF_HOSTID_TO_IDX(hostid) ((hostid) - 1)
#define RWNX_RXBUFF_DMA_ADDR_GET(skbuff)                \
    skbuff->payload

/*
 * Structure used to store information regarding E2A msg buffers in the driver
 */
struct bl_e2amsg_elem {
    struct ipc_e2a_msg *msgbuf_ptr;
    u32 dma_addr;//XXX only for 32-bit addr
};

/*
 * Structure used to store information regarding Debug msg buffers in the driver
 */
struct bl_dbg_elem {
    struct ipc_dbg_msg *dbgbuf_ptr;
    u32 dma_addr;
};

/*
 * Structure used to store information regarding E2A radar events in the driver
 */
struct bl_e2aradar_elem {
    struct radar_pulse_array_desc *radarbuf_ptr;
    u32 dma_addr;
};

int bl_ipc_init(struct bl_hw *bl_hw, struct ipc_shared_env_tag *ipc_shared_mem);
uint32_t* bl_utils_pbuf_alloc(void);
void bl_utils_pbuf_free(uint32_t *p);
int bl_utils_idx_lookup(struct bl_hw *bl_hw, uint8_t *mac);
#endif
