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

#include <hosal_dma.h>
#include <bl602_dma.h>
#include <bl602.h>
#include <bl_dma.h>
#include "bl_irq.h"
#include <stdlib.h>
#include <blog.h>

static hosal_dma_dev_t *gp_hosal_dma_dev = NULL;

static void __dma_irq_process(void *p_arg)
{
    int ch;
    uint32_t intclr;
    uint32_t tmpval;
    uint32_t interr_val;
    int tc_flag, interr_flag;
    hosal_dma_dev_t *dma_dev = (hosal_dma_dev_t *)p_arg;
    hosal_dma_irq_t pfn;
    void *parg;

    tmpval = BL_RD_REG(DMA_BASE, DMA_INTTCSTATUS);
    interr_val = BL_RD_REG(DMA_BASE, DMA_INTERRORSTATUS);

    for (ch = 0; ch < dma_dev->max_chans; ch++) {
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

                pfn = gp_hosal_dma_dev->used_chan[ch].callback;
                parg = gp_hosal_dma_dev->used_chan[ch].p_arg;
                if (pfn) {
                    pfn(parg, HOSAL_DMA_INT_TRANS_COMPLETE);
                }
            }

            if (interr_flag != 0) {
                /* int error, clear interrupt */
                tmpval = BL_RD_REG(DMA_BASE, DMA_INTERRCLR);
                intclr = BL_GET_REG_BITS_VAL(tmpval, DMA_INTERRCLR);
                intclr |= (1 << ch);
                tmpval = BL_SET_REG_BITS_VAL(tmpval, DMA_INTERRCLR, intclr);
                BL_WR_REG(DMA_BASE, DMA_INTERRCLR, tmpval);

                pfn = gp_hosal_dma_dev->used_chan[ch].callback;
                parg = gp_hosal_dma_dev->used_chan[ch].p_arg;
                if (pfn) {
                    pfn(parg, HOSAL_DMA_INT_TRANS_ERROR);
                }
            }
        }
    }
}

/**
 * @brief Initialises a DMA interface
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_dma_init(void)
{
    int i;
    if (gp_hosal_dma_dev) {
        return 0;
    }

    gp_hosal_dma_dev = calloc(sizeof(hosal_dma_dev_t), 1);
    if (gp_hosal_dma_dev == NULL) {
        blog_error("no memory !!!\r\n");
    }

    gp_hosal_dma_dev->max_chans = DMA_CH_MAX;
    gp_hosal_dma_dev->used_chan = calloc(sizeof(struct hosal_dma_chan) * DMA_CH_MAX, 1);
    if (gp_hosal_dma_dev->used_chan == NULL) {
        blog_error("no memory !!!\r\n");
    }
    DMA_Enable();
    for (i = 0; i < gp_hosal_dma_dev->max_chans; i++) {
        DMA_Channel_Disable(i);
        DMA_IntMask(i, DMA_INT_ALL, MASK);
    }
    bl_irq_register_with_ctx(DMA_ALL_IRQn, __dma_irq_process, (void *)gp_hosal_dma_dev);
    bl_irq_enable(DMA_ALL_IRQn);
    return 0;
}

/**
 * @brief Request a DMA channel
 *
 * @return  < 0 : an error occurred with any step, otherwise is DMA channel number
 */
hosal_dma_chan_t hosal_dma_chan_request(int flag)
{
    int i;

    (void)flag;

    if (!gp_hosal_dma_dev) {
        blog_error("please hosal_dma_init !\r\n");
        return -1;
    }

    for (i = 0; i < gp_hosal_dma_dev->max_chans; i++) {
        if (!gp_hosal_dma_dev->used_chan[i].used) {
            gp_hosal_dma_dev->used_chan[i].used = 1;
            return i;
        }
    }
    return -1;
}

/**
 * @brief Release a DMA channel
 *
 * @param[in]  chan  DMA channel number
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_dma_chan_release(hosal_dma_chan_t chan)
{
    if (!gp_hosal_dma_dev) {
        blog_error("please hosal_dma_init !\r\n");
        return -1;
    }

    if (chan > gp_hosal_dma_dev->max_chans) {
        return -1;
    }
    hosal_dma_chan_stop(chan);
    gp_hosal_dma_dev->used_chan[chan].used = 0;
    gp_hosal_dma_dev->used_chan[chan].callback = NULL;
    DMA_IntMask(chan, DMA_INT_TCOMPLETED, MASK);
    DMA_IntMask(chan, DMA_INT_ERR, MASK);
    return 0;
}

/**
 * @brief DMA channel start
 *
 * @param[in]  chan  DMA channel number
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_dma_chan_start(hosal_dma_chan_t chan)
{
    if (!gp_hosal_dma_dev) {
        blog_error("please hosal_dma_init !\r\n");
        return -1;
    }

    DMA_Channel_Enable(chan);
    return 0;
}

/**
 * @brief DMA channel stop
 *
 * @param[in]  chan  DMA channel number
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_dma_chan_stop(hosal_dma_chan_t chan)
{
    if (!gp_hosal_dma_dev) {
        blog_error("please hosal_dma_init !\r\n");
        return -1;
    }

    bl_dma_int_clear(chan);
    DMA_Channel_Disable(chan);
    return 0;
}

/**
 * @brief DMA irq callback set
 *
 * @param[in] chan : DMA channel number
 * @param[in] pfn : callback function
 * @param[in] arg : callback function parameter
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_dma_irq_callback_set(hosal_dma_chan_t chan, hosal_dma_irq_t pfn, void *p_arg)
{
    if (!gp_hosal_dma_dev) {
        blog_error("please hosal_dma_init !\r\n");
        return -1;
    }
    if (chan > gp_hosal_dma_dev->max_chans) {
        return -1;
    }

    gp_hosal_dma_dev->used_chan[chan].callback = pfn;
    gp_hosal_dma_dev->used_chan[chan].p_arg = p_arg;
    DMA_IntMask(chan, DMA_INT_TCOMPLETED, UNMASK);
    DMA_IntMask(chan, DMA_INT_ERR, UNMASK);

    return 0;
}

/**
 * @brief Deinitialises a DMA interface
 *
 * @param[in]  DMA the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_dma_finalize(void)
{
    if (!gp_hosal_dma_dev) {
        blog_error("please hosal_dma_init !\r\n");
        return -1;
    }

    DMA_Disable();
    bl_irq_disable(DMA_ALL_IRQn);
    free(gp_hosal_dma_dev);
    return 0;
}

/* end of file */
