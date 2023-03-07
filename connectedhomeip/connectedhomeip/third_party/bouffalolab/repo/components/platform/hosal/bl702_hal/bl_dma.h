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
#ifndef __BL_DMA_H__
#define __BL_DMA_H__

#include <utils_list.h>

#define BL_DMA_ITEM_CTRL_MAGIC_IRQ           (0x8c49b000)
#define BL_DMA_ITEM_CTRL_MAGIC_NOIRQ         (0x0c49b000)
#define BL_DMA_ITEM_CTRL_MAGIC_IRQ_CLR       (0x7FFFFFFF)
#define BL_DMA_ITEM_CTRL_MAGIC_IRQ_SET       (0x80000000)

#define BL_DMA_ITEM_BITS_SRC_BURST_COUNT_01         (0 << 12)
#define BL_DMA_ITEM_BITS_SRC_BURST_COUNT_04         (1 << 12)
#define BL_DMA_ITEM_BITS_SRC_BURST_COUNT_08         (2 << 12)
#define BL_DMA_ITEM_BITS_SRC_BURST_COUNT_16         (3 << 12)
#define BL_DMA_ITEM_BITS_DST_BURST_COUNT_01         (0 << 15)
#define BL_DMA_ITEM_BITS_DST_BURST_COUNT_04         (1 << 15)
#define BL_DMA_ITEM_BITS_DST_BURST_COUNT_08         (2 << 15)
#define BL_DMA_ITEM_BITS_DST_BURST_COUNT_16         (3 << 15)
#define BL_DMA_ITEM_BITS_SRC_WIDTH_1BYTE            (0 << 18)
#define BL_DMA_ITEM_BITS_SRC_WIDTH_2BYTE            (1 << 18)
#define BL_DMA_ITEM_BITS_SRC_WIDTH_4BYTE            (2 << 18)
#define BL_DMA_ITEM_BITS_DST_WIDTH_1BYTE            (0 << 21)
#define BL_DMA_ITEM_BITS_DST_WIDTH_2BYTE            (1 << 21)
#define BL_DMA_ITEM_BITS_DST_WIDTH_4BYTE            (2 << 21)
#define BL_DMA_ITEM_BITS_SRC_AUTO_INCR_ENABLE       (1 << 26)
#define BL_DMA_ITEM_BITS_DST_AUTO_INCR_ENABLE       (1 << 27)
#define BL_DMA_ITEM_BITS_IRQ_ENABLE                 (1 << 31)
struct bl_dma_item {
    struct utils_list_hdr item;
    void (*cb)(void *arg);
    void *arg;
    /*the following fields is for hardware access, strict aligment and memory is required*/
    uint32_t src;
    uint32_t dst;
    uint32_t next;
    uint32_t ctrl;
};


void bl_dma_copy(struct bl_dma_item *item);
void bl_dma_init(void);
void bl_dma_test(void);
int bl_dma_int_clear(int ch);
void bl_dma_update_memsrc(uint8_t ch, uint32_t src, uint32_t len);
void bl_dma_update_memdst(uint8_t ch, uint32_t dst, uint32_t len);
int bl_dma_irq_register(int channel, void *tc_handler, void *interr_handler, void *ctx);
int bl_dma_irq_unregister(int channel);
void *bl_dma_find_node_by_channel(int channel);
void *bl_dma_find_ctx_by_channel(int channel);
void *bl_dma_mem_malloc(uint32_t size);
void bl_dma_mem_free(void *ptr);


#endif
