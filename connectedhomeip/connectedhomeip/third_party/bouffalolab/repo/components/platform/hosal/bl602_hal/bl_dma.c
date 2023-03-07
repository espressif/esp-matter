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
#include <stdint.h>
#include <string.h>
#include <bl602_dma.h>

#include <FreeRTOS.h>
#include <task.h>

#include "bl_dma.h"
#include "bl_irq.h"

#include <blog.h>

#define DMA_CHANNEL_OFFSET              0x100
#define DMA_Get_Channel(ch)             (DMA_BASE+DMA_CHANNEL_OFFSET+(ch)*0x100)

/*please also change NVIC_SetPriority of DMA channel*/
#define DMA_DEFAULT_CHANNEL_COPY        (DMA_CH0)

#define MEM_UNIT_SIZE       1024
#define DTCM_ADDR_START     0X2014000
#define DTCM_ADDR_END       (DTCM_ADDR_START + (48 * 1024)) 
#define OCRAM_ADDR_START    0X2020000
#define OCRAM_ADDR_END      (OCRAM_ADDR_START + (64 * 1024))
#define WRAM_ADDR_START     0X2030000
#define WRAM_ADDR_END       (WRAM_ADDR_START + (112 * 1024))

#define AVAIBLE_ADDR_START    DTCM_ADDR_START
#define AVAIBLE_ADDR_END      WRAM_ADDR_END

struct dma_ctx {
    utils_dlist_t *pstqueue;
};

struct dma_node {
    utils_dlist_t dlist_item;
    int channel;
    void *tc_handler;
    void *interr_handler;
    void *ctx;
};

static struct utils_list dma_copy_list;

int bl_dma_int_clear(int ch)
{
    uint32_t tmpVal;
    uint32_t intClr;
    /* Get DMA register */
    uint32_t DMAChs = DMA_BASE;

    tmpVal = BL_RD_REG(DMAChs, DMA_INTTCSTATUS);
    if((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCSTATUS) & (1 << ch)) != 0) {
        /* Clear interrupt */
        tmpVal = BL_RD_REG(DMAChs, DMA_INTTCCLEAR);
        intClr = BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCCLEAR);
        intClr |= (1 << ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_INTTCCLEAR, intClr);
        BL_WR_REG(DMAChs, DMA_INTTCCLEAR, tmpVal);
    }

    tmpVal = BL_RD_REG(DMAChs, DMA_INTERRORSTATUS);
    if((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTERRORSTATUS) & (1 << ch)) != 0) {
        /*Clear interrupt */
        tmpVal = BL_RD_REG(DMAChs, DMA_INTERRCLR);
        intClr = BL_GET_REG_BITS_VAL(tmpVal, DMA_INTERRCLR);
        intClr |= (1 << ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_INTERRCLR, intClr);
        BL_WR_REG(DMAChs, DMA_INTERRCLR, tmpVal);
    }

    return 0;
}

void bl_dma_update_memsrc(uint8_t ch, uint32_t src, uint32_t len)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

    /* config channel config*/
    BL_WR_REG(DMAChs, DMA_SRCADDR, src);

    tmpVal = BL_RD_REG(DMAChs, DMA_CONTROL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_TRANSFERSIZE, len);
    BL_WR_REG(DMAChs, DMA_CONTROL, tmpVal);
}

void bl_dma_update_memdst(uint8_t ch, uint32_t dst, uint32_t len)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

    /* config channel config*/
    BL_WR_REG(DMAChs, DMA_DSTADDR, dst);

    tmpVal = BL_RD_REG(DMAChs, DMA_CONTROL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_TRANSFERSIZE, len);
    BL_WR_REG(DMAChs, DMA_CONTROL, tmpVal);
}

static void inline _dma_copy_trigger(struct bl_dma_item *first)
{
    //blog_info("------ DMA Trigger\r\n");
    DMA_LLI_Update(DMA_DEFAULT_CHANNEL_COPY, (uint32_t)&(first->src));
    DMA_Channel_Enable(DMA_DEFAULT_CHANNEL_COPY);
}

void bl_dma_copy(struct bl_dma_item *first)
{
    taskENTER_CRITICAL();
    if (utils_list_is_empty(&dma_copy_list)) {
        _dma_copy_trigger(first);
    }
    utils_list_push_back(&dma_copy_list, &(first->item));
    taskEXIT_CRITICAL();
}

void bl_dma_IRQHandler(void)
{
    struct bl_dma_item *first;

    //blog_info("------ Clear DMA now\r\n");
    bl_dma_int_clear(DMA_DEFAULT_CHANNEL_COPY);
    first = (struct bl_dma_item*)utils_list_pop_front(&dma_copy_list);
    if (NULL == first) {
        blog_info("[INT] ASSERT here for empty chain\r\n");
        while (1) {
        }
    }
    if (first->cb) {
        first->cb(first->arg);
    }

    /*prepare NEXT Transfer*/
    first = (struct bl_dma_item*)utils_list_pick(&dma_copy_list);
    if (first) {
        _dma_copy_trigger(first);
    }
}

void *bl_dma_mem_malloc(uint32_t size)
{
    void *ptr;
    uint32_t counts, piece, ptr_piece_num;
    uint32_t *p_heap_addr;
    uint32_t addr_start, addr_end;
    uint32_t left_size;

    addr_start = AVAIBLE_ADDR_START;
    addr_end = AVAIBLE_ADDR_END;
   
    ptr_piece_num = xPortGetFreeHeapSize() / MEM_UNIT_SIZE + 1;
    p_heap_addr = pvPortMalloc(ptr_piece_num * 4);
    memset(p_heap_addr, 0, ptr_piece_num * 4);
    if (p_heap_addr == NULL) {
        return NULL;
    }
    
    ptr = NULL;
    counts = 0;
    while (1) {
        left_size = xPortGetFreeHeapSize();
        if (left_size < size || left_size < MEM_UNIT_SIZE) {
            ptr = NULL;
            goto __exit;
        }

        ptr = pvPortMalloc(MEM_UNIT_SIZE);
        if (ptr == NULL) {
            goto __exit;
        }

        p_heap_addr[counts++] = (uint32_t)ptr;
        if ((uint32_t)((uint32_t)ptr & 0x0fffffff) >= addr_start) {
            if ((uint32_t)((uint32_t)ptr & 0x0fffffff) <= addr_end) {
                ptr = pvPortMalloc(size);
                break;
            }
        }
    }

__exit:
    for (piece = 0; piece < counts; piece++) {
        vPortFree((uint32_t *)p_heap_addr[piece]);
    }
    
    vPortFree(p_heap_addr);
    return ptr;
}

void bl_dma_mem_free(void *ptr)
{
    vPortFree(ptr);
}

static void bl_dma_int_process(void)
{
    int ch;
    uint32_t intclr;
    uint32_t tmpval;
    uint32_t interr_val; 
    struct dma_node *node;
    struct dma_ctx *pstctx;
    void (*handler)(void) = NULL; 
    int tc_flag, interr_flag;
   
    tmpval = BL_RD_REG(DMA_BASE, DMA_INTTCSTATUS);
    interr_val = BL_RD_REG(DMA_BASE, DMA_INTERRORSTATUS);
    bl_irq_ctx_get(DMA_ALL_IRQn, (void **)&pstctx); 
    for (ch = 0; ch < DMA_CH_MAX; ch++) {
        tc_flag = BL_GET_REG_BITS_VAL(tmpval, DMA_INTTCSTATUS) & (1 << ch);
        interr_flag = BL_GET_REG_BITS_VAL(interr_val, DMA_INTERRORSTATUS) & (1 << ch);
        
        if((tc_flag != 0) || (interr_flag != 0)) {
            if (tc_flag != 0) {
                /* tc int, clear interrupt */
                tmpval = BL_RD_REG(DMA_BASE, DMA_INTTCCLEAR);
                intclr = BL_GET_REG_BITS_VAL(tmpval, DMA_INTTCCLEAR);
                intclr |= (1 << ch);
                tmpval = BL_SET_REG_BITS_VAL(tmpval, DMA_INTTCCLEAR, intclr);
                BL_WR_REG(DMA_BASE, DMA_INTTCCLEAR, tmpval);
            }

            if (interr_flag != 0) {
                /* int error, clear interrupt */
                tmpval = BL_RD_REG(DMA_BASE, DMA_INTERRCLR);
                intclr = BL_GET_REG_BITS_VAL(tmpval, DMA_INTERRCLR);
                intclr |= (1 << ch);
                tmpval = BL_SET_REG_BITS_VAL(tmpval, DMA_INTERRCLR, intclr);
                BL_WR_REG(DMA_BASE, DMA_INTERRCLR, tmpval);
            }

            utils_dlist_for_each_entry(pstctx->pstqueue, node, struct dma_node, dlist_item) {
                if (ch == node->channel) {
                    if (node->tc_handler != NULL && tc_flag != 0) {
                        handler = (void(*)(void))node->tc_handler;
                        handler();
                    }

                    if (node->interr_handler != NULL && interr_flag != 0) {
                        handler = (void(*)(void))node->interr_handler;
                        handler();
                    }
                }               
            }
        }        
    }
    
    return;
}

int bl_dma_irq_register(int channel, void *tc_handler, void *interr_handler, void *ctx)
{
    struct dma_ctx *pstctx;
    struct dma_node *node;
    struct dma_node *pstnode;

    if ((channel > DMA_CH_MAX || channel < 0) || tc_handler == NULL) {
        blog_error("not valid para \r\n");

        return -1;
    }

    bl_irq_ctx_get(DMA_ALL_IRQn, (void **)&pstctx);
    utils_dlist_for_each_entry(pstctx->pstqueue, node, struct dma_node, dlist_item) {
        if (channel == node->channel) {
            blog_warn("channel %d already register \r\n", node->channel);
            return -1;
        }
    }
    pstnode = pvPortMalloc(sizeof(struct dma_node)); 
    if (pstnode == NULL) {
        blog_error("malloc dma node failed. \r\n");
    }
    pstnode->channel = channel;
    pstnode->tc_handler = tc_handler;
    pstnode->interr_handler = interr_handler;
    pstnode->ctx = ctx;
    utils_dlist_add(&(pstnode->dlist_item), pstctx->pstqueue);

    return -1;
}

void *bl_dma_find_node_by_channel(int channel)
{
    struct dma_ctx *pstctx;
    struct dma_node *node;

    bl_irq_ctx_get(DMA_ALL_IRQn, (void **)&pstctx);
    utils_dlist_for_each_entry(pstctx->pstqueue, node, struct dma_node, dlist_item) {
        if (node->channel == channel) {
            break;
        }
    }
    
    if (&(node->dlist_item) == pstctx->pstqueue) {
        blog_error("not find channel register. \r\n");

        return NULL;
    }
    
    return node;
}

void *bl_dma_find_ctx_by_channel(int channel)
{
    struct dma_node *node;
    void *ctx;

    node = bl_dma_find_node_by_channel(channel);
    ctx = node->ctx;

    return ctx;
}

int bl_dma_irq_unregister(int channel)
{
    struct dma_ctx *pstctx;
    struct dma_node *node;

    if (channel > DMA_CH_MAX || channel < 0) {
        blog_error("not valid para \r\n");

        return -1;
    }

    bl_irq_ctx_get(DMA_ALL_IRQn, (void **)&pstctx);
    utils_dlist_for_each_entry(pstctx->pstqueue, node, struct dma_node, dlist_item) {
        if (channel == node->channel) {
            utils_dlist_del(&(node->dlist_item));
            vPortFree(node);
            break;
        }
    }

    if (&node->dlist_item == pstctx->pstqueue) {
        blog_error("not find node \r\n");
        return -1;
    }
    
    return 0;
}

void bl_dma_init(void)
{
    struct dma_ctx *pstctx;
    //FIXME use DMA_CH4 as channel copy
    DMA_Chan_Type dmaCh = DMA_DEFAULT_CHANNEL_COPY;
    DMA_LLI_Cfg_Type lliCfg =
    {
        DMA_TRNS_M2M,
        DMA_REQ_NONE,
        DMA_REQ_NONE,
    };
    utils_list_init(&dma_copy_list);

    pstctx = pvPortMalloc(sizeof(struct dma_ctx));
    if (pstctx == NULL) {
        blog_error("malloc dma ctx failed \r\n");

        return;
    }

    pstctx->pstqueue = pvPortMalloc(sizeof(utils_dlist_t));
    if (pstctx->pstqueue == NULL) {
        blog_error("malloc dma pstqueue failed \r\n");

        return;
    }
    INIT_UTILS_DLIST_HEAD(pstctx->pstqueue);

    DMA_Enable();
    DMA_IntMask(dmaCh, DMA_INT_ALL, MASK);
    DMA_IntMask(dmaCh, DMA_INT_TCOMPLETED, UNMASK);
    DMA_IntMask(dmaCh, DMA_INT_ERR, UNMASK);
    DMA_LLI_Init(dmaCh, &lliCfg);
    bl_irq_register_with_ctx(DMA_ALL_IRQn, bl_dma_int_process, pstctx);
    bl_dma_irq_register(DMA_DEFAULT_CHANNEL_COPY, bl_dma_IRQHandler, NULL, NULL);
    bl_irq_enable(DMA_ALL_IRQn);
}

static void _cb_cmd(void *arg)
{
    struct bl_dma_item *first;

    first = (struct bl_dma_item*)arg;
    blog_info("[DMA] [TEST] Callback is working, arg is %p\r\n", arg);
    first->arg = NULL;
}

static void _dma_test_case1(void)
{
    struct bl_dma_item *first;
    uint32_t *src;
    uint32_t *dst;
    const int size = 68;

    first = pvPortMalloc(sizeof(struct bl_dma_item));
    src = pvPortMalloc(size);
    dst = pvPortMalloc(size);

    blog_info("[TEST] [DMA] first %p, src %p, dst %p\r\n",
            first,
            src,
            dst
    );

    memset(first, 0, sizeof(struct bl_dma_item));
    memset(src, 1, size);
    memset(dst, 0xFF, size);
    first->src = (uint32_t)src;
    first->dst = (uint32_t)dst;
    first->next = 0;
    first->ctrl =  BL_DMA_ITEM_CTRL_MAGIC_IRQ | (size >> 2);
    first->cb = _cb_cmd;
    first->arg = first;

    bl_dma_copy(first);

    /*Arg will be set in callback*/
    while (first->arg) {
        vTaskDelay(2);
    }

    vPortFree((void*)first->src);
    vPortFree((void*)first->dst);
    vPortFree(first);
}

void bl_dma_test(void)
{
    _dma_test_case1();
}
