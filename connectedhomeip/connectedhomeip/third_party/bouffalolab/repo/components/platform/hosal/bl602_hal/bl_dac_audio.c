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
#include "bl_dac_audio.h"

#include "bl602_common.h"
#include "bl602_glb.h"
#include "bl602_hbn.h"
#include "bl602_dma.h"
#include "bl_irq.h"
#include "bl_dma.h"

static bl_audio_dac_dev_t *gp_audio_dac_dev = NULL;

#define BL_DAC_BITS                    10

#define DAC_DMA_CHANNLE                DMA_CH0

static struct DMA_Control_Reg dmaCtrlRegVal={
    .TransferSize=1000,
    .SBSize=DMA_BURST_SIZE_1,
    .DBSize=DMA_BURST_SIZE_1,
    .SWidth=DMA_TRNS_WIDTH_32BITS,
    .DWidth=DMA_TRNS_WIDTH_32BITS,
    .SI=DMA_PINC_ENABLE,
    .DI=DMA_PINC_DISABLE,
    .I=1,
};

static DMA_LLI_Cfg_Type lliCfg={
    DMA_TRNS_M2P,
    DMA_REQ_NONE,
    DMA_REQ_GPADC1,
};

static GLB_GPIO_Cfg_Type gpioCfg[2] = {
    [0] = {
        .gpioPin = GLB_GPIO_PIN_13,
        .gpioFun = GPIO13_FUN_GPIP_CH3,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive = 1,
        .smtCtrl = 1
    },
    [1] = {
        .gpioPin = GLB_GPIO_PIN_14,
        .gpioFun = GPIO14_FUN_GPIP_CH2,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive = 1,
        .smtCtrl = 1
    }
};
static GLB_GPIP_DAC_Cfg_Type dacCfg = {
    .refSel = GLB_DAC_REF_SEL_INTERNAL,
    .resetChanA = ENABLE,
    .resetChanB = ENABLE,
    .mod = GPIP_DAC_MOD_16K,
    .dmaEn = ENABLE,
    .dmaFmt = GPIP_DAC_DMA_FORMAT_1
};
static GLB_GPIP_DAC_ChanA_Cfg_Type chACfg = {
    .chanCovtEn = ENABLE,
    .outputEn = ENABLE,
    .chanEn = ENABLE,
    .src = GPIP_DAC_ChanA_SRC_DMA
};
static GLB_GPIP_DAC_ChanB_Cfg_Type chBCfg = {
    .chanCovtEn = ENABLE,
    .outputEn = ENABLE,
    .chanEn = ENABLE,
    .src = GPIP_DAC_ChanB_SRC_A
};

static void __dac_clock_init(void)
{
    GLB_Set_DAC_CLK(ENABLE, GLB_DAC_CLK_32M, 0x40);
}

static void __dac_gpio_init(void)
{
    GLB_GPIO_Init(&gpioCfg[0]);
}

static void __dac_lli_init(bl_audio_dac_dev_t *p_dev)
{
    p_dev->lli_list[0].srcDmaAddr = (uint32_t)p_dev->lli_tx_buffer;
    p_dev->lli_list[0].destDmaAddr = 0x40002048;
    p_dev->lli_list[0].nextLLI = (uint32_t)&p_dev->lli_list[1];
    dmaCtrlRegVal.TransferSize = (p_dev->lli_tx_buffer_size >> 2) >> 1;
    p_dev->lli_list[0].dmaCtrl= dmaCtrlRegVal;

    p_dev->lli_list[1].srcDmaAddr = ((uint32_t)p_dev->lli_tx_buffer) + (p_dev->lli_tx_buffer_size >> 1);
    p_dev->lli_list[1].destDmaAddr = 0x40002048;
    p_dev->lli_list[1].nextLLI=(uint32_t)&p_dev->lli_list[0];
    dmaCtrlRegVal.TransferSize = (p_dev->lli_tx_buffer_size >> 2) >> 1;
    p_dev->lli_list[1].dmaCtrl= dmaCtrlRegVal;

    DMA_LLI_Init(DAC_DMA_CHANNLE, &lliCfg);
    DMA_LLI_Update(DAC_DMA_CHANNLE, (uint32_t)&gp_audio_dac_dev->lli_list);
}

static int __dma_int_clear(int ch)
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

static void __dma_irq_handler(void)
{
    uint32_t tmpVal;
    int ret, size;

    if ( NULL == gp_audio_dac_dev) {
        return;
    }

    tmpVal = BL_RD_REG(DMA_BASE, DMA_INTTCSTATUS);
    if((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCSTATUS) & (1 << DAC_DMA_CHANNLE)) != 0) {

        size = gp_audio_dac_dev->lli_tx_buffer_size >> 1;

        if (gp_audio_dac_dev->usr_cb) {
            if (++gp_audio_dac_dev->pingpang >= 2) {
                gp_audio_dac_dev->pingpang = 0;
            }
            if (gp_audio_dac_dev->pingpang) {
                ret = gp_audio_dac_dev->usr_cb(gp_audio_dac_dev->p_usr_arg,
                                               gp_audio_dac_dev->lli_tx_buffer,
                                               size,
                                               gp_audio_dac_dev->is_underrun);
            } else {
                ret = gp_audio_dac_dev->usr_cb(gp_audio_dac_dev->p_usr_arg,
                                               gp_audio_dac_dev->lli_tx_buffer + size,
                                               size,
                                               gp_audio_dac_dev->is_underrun);
            }
            if (ret != 0) {
                gp_audio_dac_dev->is_underrun = 1;
            } else {
                gp_audio_dac_dev->is_underrun = 0;
            }
        }
    }
    __dma_int_clear(DAC_DMA_CHANNLE);
}

int bl_audio_dac_samplerate_set (bl_audio_dac_dev_t *p_dev, GPIP_DAC_MOD_Type samplerate)
{
    dacCfg.mod = samplerate;
    return 0;
}

int bl_audio_dac_tx_ready_config(bl_audio_dac_dev_t *p_dev,
                              audio_callback_tx_ready_t cb,
                              void *p_arg)
{
    if (NULL == p_dev) {
        return -1;
    }
    p_dev->usr_cb = cb;
    p_dev->p_usr_arg = p_arg;
    return 0;
}

int bl_audio_dac_rx_ready_config(bl_audio_dac_dev_t *p_dev,
                              audio_callback_rx_ready_t cb,
                              void *p_arg)
{
    return 0;
}

int bl_audio_dac_tx_buffer_config (bl_audio_dac_dev_t *p_dev, void **ptr_mem, uint32_t bufsize)
{
    int i;
    uint16_t *p_16;

    if (NULL == p_dev->lli_tx_buffer) {
        p_dev->lli_tx_buffer = pvPortMalloc(bufsize);
    }

    if (NULL == p_dev->lli_tx_buffer) {
        return -1;
    }
    *ptr_mem = p_dev->lli_tx_buffer;
    p_dev->lli_tx_buffer_size = bufsize;

    p_16 = (uint16_t *)p_dev->lli_tx_buffer;
    for (i = 0; i < (p_dev->lli_tx_buffer_size >> 1); i++) {
        p_16[i] = 1UL << (BL_DAC_BITS - 1);
    }

    return 0;
}

int bl_audio_dac_rx_buffer_config (bl_audio_dac_dev_t *p_dev,
                                  void **ptr_mem,
                                  uint32_t bufsize)
{
    return 0;
}

int bl_audio_dac_start (bl_audio_dac_dev_t *p_dev)
{
    if (NULL == p_dev) {
        return -1;
    }

    DMA_Enable();
    DMA_Channel_Disable(DAC_DMA_CHANNLE);

    DMA_IntMask(DAC_DMA_CHANNLE, DMA_INT_ALL, MASK);
    DMA_IntMask(DAC_DMA_CHANNLE, DMA_INT_TCOMPLETED, UNMASK);
    bl_irq_register(DMA_ALL_IRQn, __dma_irq_handler);
    bl_irq_enable(DMA_ALL_IRQn);

    GLB_GPIP_DAC_Init(&dacCfg);
    GPIP_Set_DAC_DMA_TX_FORMAT_SEL(GPIP_DAC_DMA_FORMAT_1);
    GPIP_Set_DAC_DMA_TX_Enable();
    GPIP_DAC_ChanA_Enable();
    GPIP_DAC_ChanB_Enable();

    __dac_lli_init(p_dev);

    DMA_Channel_Enable(DAC_DMA_CHANNLE);

    return 0;
}

int bl_audio_dac_stop (bl_audio_dac_dev_t *p_dev)
{
    gp_audio_dac_dev->pingpang = 0;

    DMA_Disable();
    DMA_Channel_Disable(DAC_DMA_CHANNLE);
    bl_irq_disable(DMA_ALL_IRQn);

    return 0;
}

int bl_audio_dac_init (bl_audio_dac_dev_t *p_dev)
{
    if (NULL == p_dev) {
        return -1;
    }
    memset(p_dev, 0, sizeof(bl_audio_dac_dev_t));
    gp_audio_dac_dev = p_dev;

    __dac_gpio_init();
    __dac_clock_init();

    GLB_GPIP_DAC_Set_ChanA_Config(&chACfg);
    GLB_GPIP_DAC_Set_ChanB_Config(&chBCfg);

    return 0;
}

int bl_audio_dac_deinit (bl_audio_dac_dev_t *p_dev)
{
    vPortFree(p_dev->lli_tx_buffer);
    p_dev->lli_tx_buffer =  NULL;

    return 0;
}
