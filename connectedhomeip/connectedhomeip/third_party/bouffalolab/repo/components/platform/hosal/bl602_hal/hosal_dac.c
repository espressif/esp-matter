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

#include "hosal_dac.h"
#include "hosal_dma.h"
#include "bl602_dac.h"
#include "bl602_gpio.h"
#include "bl602_common.h"
#include "bl602_dma.h"
#include "bl602_glb.h"
#include "bl602.h"
#include "blog.h"
#include "bl_irq.h"

#define LLI_BUFF_SIZE           2048

#define DAC_RANGE_MIN           200000
#define DAC_RANGE_MAX           1800000   /* for 602 ,dac is 10 bits , data range is 0 ～ 1023 ---> 0.2V ~ 1.8V */

int hosal_dac_init(hosal_dac_dev_t *dac)
{
    uint8_t dac_pin;

    GLB_GPIO_Cfg_Type gpio_cfg = {
        .gpioPin = GLB_GPIO_PIN_13,
        .gpioFun = GPIO13_FUN_GPIP_CH3,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive = 1,
        .smtCtrl = 1
    };

    GLB_GPIP_DAC_Cfg_Type dac_cfg = {
        .refSel = GLB_DAC_REF_SEL_INTERNAL,         /*602 only support GLB_DAC_REF_SEL_INTERNAL*/
        .resetChanA = ENABLE,
        .resetChanB = ENABLE,
        .mod = GPIP_DAC_MOD_32K,
        .dmaEn = DISABLE,
        .dmaFmt = GPIP_DAC_DMA_FORMAT_0
    };

    if (NULL == dac || dac->port != 0) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    dac->dma_chan = -1;
    dac->priv = NULL;
    dac->cb = NULL;
    dac->arg = NULL;

    dac_pin = dac->config.pin;

    if (GLB_GPIO_PIN_13 == dac_pin) {
        gpio_cfg.gpioPin = GLB_GPIO_PIN_13;
        gpio_cfg.gpioFun = GPIO13_FUN_GPIP_CH3;
    }
    if (GLB_GPIO_PIN_14 == dac_pin) {
        gpio_cfg.gpioPin = GLB_GPIO_PIN_14;
        gpio_cfg.gpioFun = GPIO14_FUN_GPIP_CH2;
    }

    if (8000 == dac->config.freq) {
        dac_cfg.mod = GPIP_DAC_MOD_8K;
    } else if (16000 == dac->config.freq) {
        dac_cfg.mod = GPIP_DAC_MOD_16K;
    } else if (32000 == dac->config.freq) {
        dac_cfg.mod = GPIP_DAC_MOD_32K;
    } else if (512000 == dac->config.freq) {
        dac_cfg.mod = GPIP_DAC_MOD_512K;
    } else {
        blog_error("not support this freq!\r\n");
        return -1;
    }

    dac_cfg.dmaEn = dac->config.dma_enable;

    GLB_GPIO_Init(&gpio_cfg);
    GLB_GPIP_DAC_Init(&dac_cfg);
    GLB_Set_DAC_CLK(ENABLE, GLB_DAC_CLK_32M, 0x40);

    return 0;
}

int hosal_dac_start(hosal_dac_dev_t *dac)
{
    GLB_GPIP_DAC_ChanA_Cfg_Type chA_cfg = {
        .chanCovtEn = ENABLE,
        .outputEn = ENABLE,
        .chanEn = ENABLE,
        .src = GPIP_DAC_ChanA_SRC_REG
    };

    GLB_GPIP_DAC_ChanB_Cfg_Type chB_cfg = {
        .chanCovtEn = ENABLE,
        .outputEn = ENABLE,
        .chanEn = ENABLE,
        .src = GPIP_DAC_ChanB_SRC_REG
    };

    if (NULL == dac || dac->port != 0) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    if (GLB_GPIO_PIN_13 == dac->config.pin) {
        GPIP_Set_DAC_ChanA_SRC_SEL(GPIP_DAC_ChanA_SRC_REG);
    } else if (GLB_GPIO_PIN_14 == dac->config.pin) {
        GPIP_Set_DAC_ChanB_SRC_SEL(GPIP_DAC_ChanB_SRC_REG);
    } else
    {}

    GLB_GPIP_DAC_Set_ChanA_Config(&chA_cfg);
    GLB_GPIP_DAC_Set_ChanB_Config(&chB_cfg);

    return 0;
}

int hosal_dac_set_value(hosal_dac_dev_t *dac, uint32_t data)
{
    float k, b;
    uint32_t y_data;

    if (NULL == dac || dac->port != 0 || data > DAC_RANGE_MAX || data < DAC_RANGE_MIN) {
        blog_error("parameter is error!\r\n");
        return -1;
    }
    
    k = 1023.0 / (DAC_RANGE_MAX - DAC_RANGE_MIN);   /* y = kx + b,x-->data, y-->y_data */
    b = 1023.0 - DAC_RANGE_MAX * k;

    y_data = (uint32_t)(data * k + b);

    /* for 602, dac is 10 bits, data only Low 16 bits are valid */
    if (GLB_GPIO_PIN_13 == dac->config.pin) {
        GLB_DAC_Set_ChanA_Value(y_data);
    } else if (GLB_GPIO_PIN_14 == dac->config.pin) {
        GLB_DAC_Set_ChanB_Value(y_data);
    } else
    {}

    return 0;
}

int hosal_dac_get_value(hosal_dac_dev_t *dac)
{
    int tmp_val, data;
    float k, b;

    if (NULL == dac || dac->port != 0) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    tmp_val = BL_RD_REG(GLB_BASE,GLB_GPDAC_DATA);

    if (GLB_GPIO_PIN_13 == dac->config.pin) {
        tmp_val = BL_GET_REG_BITS_VAL(tmp_val,GLB_GPDAC_A_DATA);
    } else if (GLB_GPIO_PIN_14 == dac->config.pin) {
        tmp_val = BL_GET_REG_BITS_VAL(tmp_val,GLB_GPDAC_B_DATA);
    } else
    {}

    b = DAC_RANGE_MIN;                                          /* DAC_RANGE_MIN */
    k = (DAC_RANGE_MAX - DAC_RANGE_MIN) / 1023.0;
    data = (int)(k * tmp_val + b);

    return data;
}

int hosal_dac_stop(hosal_dac_dev_t *dac)
{
    if (NULL == dac || dac->port != 0) {
        blog_error("parameter is error!\r\n");
        return -1;
    }
 
    GPIP_DAC_ChanA_Disable();
    GPIP_DAC_ChanB_Disable();
   
    return 0;
}

void dac_dma_irq_handler(void *p_arg, uint32_t flag)
{
    hosal_dac_dev_t *dac;

    dac = (hosal_dac_dev_t *)p_arg;

    if (flag == HOSAL_DMA_INT_TRANS_COMPLETE) {
        if (dac->cb) {
            dac->cb(dac->arg);
        }
    } else if (flag == HOSAL_DMA_INT_TRANS_ERROR) {
        blog_error("dma tran error!\r\n");
    }

    if (NULL != dac->priv) {
        vPortFree(dac->priv);
        dac->priv = NULL;
    }
}

int hosal_dac_dma_cb_reg(hosal_dac_dev_t *dac, hosal_dac_cb_t callback, void *arg)
{
    if (NULL == dac) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    dac->cb = callback;
    dac->arg = arg;

    return 0;
}

int hosal_dac_dma_start(hosal_dac_dev_t *dac, uint32_t *data, uint32_t size)
{
    uint32_t i;
    uint32_t count;
    uint32_t remainder;
    struct DMA_Control_Reg dmactrl;
    DMA_LLI_Ctrl_Type *plli_list;
    
    GLB_GPIP_DAC_ChanA_Cfg_Type chA_cfg = {
        .chanCovtEn = ENABLE,
        .outputEn = ENABLE,
        .chanEn = ENABLE,
        .src = GPIP_DAC_ChanA_SRC_DMA
    };

    GLB_GPIP_DAC_ChanB_Cfg_Type chB_cfg = {
        .chanCovtEn = ENABLE,
        .outputEn = ENABLE,
        .chanEn = ENABLE,
        .src = GPIP_DAC_ChanB_SRC_A
    };

    DMA_LLI_Cfg_Type lli_cfg = {
        DMA_TRNS_M2P,
        DMA_REQ_NONE,
        DMA_REQ_GPADC1,
    };

    if (NULL == dac || NULL == data || size < 1 || dac->port != 0) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    if (dac->config.dma_enable && (dac->dma_chan < 0)) {
        dac->dma_chan = hosal_dma_chan_request(0);
        if (dac->dma_chan < 0 ) {
            blog_error("dma channel is error!\r\n");
            return -1;
        }
    }

    hosal_dma_chan_stop(dac->dma_chan);
    hosal_dma_irq_callback_set(dac->dma_chan, dac_dma_irq_handler, dac);

    /* for dma Linked list */
    count = size / 4 / LLI_BUFF_SIZE;
    remainder = size % LLI_BUFF_SIZE;

    if (remainder != 0) {
        count = count + 1;
    }

    dmactrl.SBSize = DMA_BURST_SIZE_1;
    dmactrl.DBSize = DMA_BURST_SIZE_1;
    dmactrl.SWidth = DMA_TRNS_WIDTH_32BITS;
    dmactrl.DWidth = DMA_TRNS_WIDTH_32BITS;
    dmactrl.Prot = 0;
    dmactrl.SLargerD = 0;

    plli_list = pvPortMalloc(sizeof(DMA_LLI_Ctrl_Type) * count);
    memset(plli_list, 0 , sizeof(DMA_LLI_Ctrl_Type) * count);
    
    dac->priv = plli_list;

    if (NULL == plli_list) {
        blog_error("malloc lli failed. \r\n");
        return -1;
    }

    for (i = 0; i < count; i++) {
        if (0 == remainder) {
            dmactrl.TransferSize = LLI_BUFF_SIZE;
        } else {
            if (i == count - 1) {
                dmactrl.TransferSize = remainder;
            } else {
                dmactrl.TransferSize = LLI_BUFF_SIZE;
            }
        }

        if (i == count - 1) {
            dmactrl.I = 1;
        } else {
            dmactrl.I = 0;
        }

        dmactrl.SI = DMA_MINC_ENABLE;
        dmactrl.DI = DMA_MINC_DISABLE;
        plli_list[i].srcDmaAddr = (uint32_t)(data + i * LLI_BUFF_SIZE);
        plli_list[i].destDmaAddr = 0x40002048;
        plli_list[i].dmaCtrl = dmactrl;
        if (i != 0) {
            plli_list[i-1].nextLLI = (uint32_t)&plli_list[i];
        }
        //plli_list[count - 1].nextLLI = (uint32_t)&plli_list[0];
    }
    plli_list[i - 1].nextLLI = 0;

    DMA_LLI_Init(dac->dma_chan, &lli_cfg);
    /*we must start dma before DMA_LLI_Update,otherwise dma will Interrupt first*/
    hosal_dma_chan_start(dac->dma_chan);
    DMA_LLI_Update(dac->dma_chan, (uint32_t)plli_list);

    if (GLB_GPIO_PIN_13 == dac->config.pin) {
        /* set src before set config */
        GPIP_Set_DAC_ChanA_SRC_SEL(GPIP_DAC_ChanA_SRC_DMA);
    } else if (GLB_GPIO_PIN_14 == dac->config.pin) {
        GPIP_Set_DAC_ChanB_SRC_SEL(GPIP_DAC_ChanB_SRC_A);
    } else{}
    
    GLB_GPIP_DAC_Set_ChanA_Config(&chA_cfg);
    GLB_GPIP_DAC_Set_ChanB_Config(&chB_cfg);
    GPIP_Set_DAC_DMA_TX_FORMAT_SEL(GPIP_DAC_DMA_FORMAT_0);
    GPIP_Set_DAC_DMA_TX_Enable();

    return 0;
}

int hosal_dac_dma_stop(hosal_dac_dev_t *dac)
{
    if (NULL == dac || dac->port != 0) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    GPIP_DAC_ChanA_Disable();
    GPIP_DAC_ChanB_Disable();
    
    if (dac->config.dma_enable) {
        GPIP_Set_DAC_DMA_TX_Disable();
        hosal_dma_chan_stop(dac->dma_chan);
    }

    return 0;
}

int hosal_dac_finalize(hosal_dac_dev_t *dac)
{
    if (NULL == dac || dac->port != 0) {
        blog_error("parameter is error!\r\n");
        return -1;
    }
   
    if (NULL != dac->priv) {
        vPortFree(dac->priv);
        dac->priv = NULL;
    }

    if (dac->dma_chan >= 0) {
        hosal_dma_chan_release(dac->dma_chan);
        dac->dma_chan = -1;
    }

    return 0;
}
