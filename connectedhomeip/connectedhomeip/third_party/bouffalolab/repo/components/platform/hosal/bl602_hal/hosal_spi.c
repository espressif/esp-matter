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
#include <device/vfs_spi.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <aos/kernel.h>

#include <bl_dma.h>
#include <bl_gpio.h>
#include <bl602_spi.h>
#include <bl602_gpio.h>
#include <bl602_glb.h>
#include <bl602_dma.h>
#include <bl602.h>
#include <bl_irq.h>
#include <bl_dma.h>
#include <bl_gpio.h>
#include <hosal_dma.h>
#include <hosal_spi.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <event_groups.h>

#include <libfdt.h>
#include <utils_log.h>
#include <blog.h>

#define LLI_BUFF_SIZE       2048

#define EVT_GROUP_SPI_TX    (1<<0)
#define EVT_GROUP_SPI_RX    (1<<1)
#define EVT_GROUP_SPI_TR    (EVT_GROUP_SPI_TX | EVT_GROUP_SPI_RX)

static void hosal_spi_int_handler_tx(void *arg, uint32_t flag);
static void hosal_spi_int_handler_rx(void *arg, uint32_t flag);
static void spi_irq_process(void *p_arg);

typedef struct {
    uint8_t *tx_data;
    uint8_t *rx_data;
    uint32_t length;
    uint32_t tx_index;
    uint32_t rx_index;
    EventGroupHandle_t spi_event_group;
} spi_priv_t;

typedef struct {
    int8_t tx_dma_ch;
    int8_t rx_dma_ch;
    uint8_t rx_enable;
    EventGroupHandle_t spi_event_group;
} spi_dma_priv_t;

static void spi_basic_init(hosal_spi_dev_t *arg)
{
    hosal_spi_dev_t *hw_arg = arg;
    SPI_CFG_Type spicfg;
    SPI_FifoCfg_Type fifocfg;
    SPI_ID_Type spi_id; //TODO change SPI_ID_Type
    
    spi_id = hw_arg->port;

    SPI_SetClock(spi_id, hw_arg->config.freq);

    /* spi config */
    spicfg.deglitchEnable = DISABLE;
    spicfg.continuousEnable = ENABLE;
    spicfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST,
    spicfg.bitSequence = SPI_BIT_INVERSE_MSB_FIRST,
    spicfg.frameSize = SPI_FRAME_SIZE_8;

    if (hw_arg->config.polar_phase == 0) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_0;
        spicfg.clkPolarity = SPI_CLK_POLARITY_LOW;
    } else if (hw_arg->config.polar_phase == 1) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_1;
        spicfg.clkPolarity = SPI_CLK_POLARITY_LOW;
    } else if (hw_arg->config.polar_phase == 2) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_0;
        spicfg.clkPolarity = SPI_CLK_POLARITY_HIGH;
    } else if (hw_arg->config.polar_phase == 3) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_1;
        spicfg.clkPolarity = SPI_CLK_POLARITY_HIGH;
    } else {
        blog_error("node support polar_phase \r\n");
    }
    SPI_Init(0,&spicfg);

    if (hw_arg->config.mode == 0)
    {
        SPI_Disable(spi_id, SPI_WORK_MODE_MASTER);
    } else {
        SPI_Disable(spi_id, SPI_WORK_MODE_SLAVE);
    }

    SPI_IntMask(spi_id,SPI_INT_ALL,MASK);

    /* fifo */
    fifocfg.txFifoThreshold = 1;
    fifocfg.rxFifoThreshold = 1;
    if (hw_arg->config.dma_enable) {
        fifocfg.txFifoDmaEnable = ENABLE;
        fifocfg.rxFifoDmaEnable = ENABLE;
        SPI_FifoConfig(spi_id,&fifocfg);
    } else {
        fifocfg.txFifoDmaEnable = DISABLE;
        fifocfg.rxFifoDmaEnable = DISABLE;
        SPI_FifoConfig(spi_id,&fifocfg);
        bl_irq_register_with_ctx(SPI_IRQn, spi_irq_process, (void *)hw_arg);
        blog_info("spi no dma mode\r\n");
    }
}

static int lli_list_init(DMA_LLI_Ctrl_Type **pptxlli, DMA_LLI_Ctrl_Type **pprxlli, uint8_t *ptx_data, uint8_t *prx_data, uint32_t length)
{
    uint32_t i = 0;
    uint32_t count;
    uint32_t remainder;
    struct DMA_Control_Reg dmactrl;

    count = length / LLI_BUFF_SIZE;
    remainder = length % LLI_BUFF_SIZE;

    if (remainder != 0) {
        count = count + 1;
    }

    dmactrl.SBSize = DMA_BURST_SIZE_1;
    dmactrl.DBSize = DMA_BURST_SIZE_1;
    dmactrl.SWidth = DMA_TRNS_WIDTH_8BITS;
    dmactrl.DWidth = DMA_TRNS_WIDTH_8BITS;
    dmactrl.Prot = 0;
    dmactrl.SLargerD = 0;

    *pptxlli = pvPortMalloc(sizeof(DMA_LLI_Ctrl_Type) * count);
    if (*pptxlli == NULL) {
        blog_error("malloc lli failed. \r\n");

        return -1;
    }

    if (NULL != prx_data) {
        *pprxlli = pvPortMalloc(sizeof(DMA_LLI_Ctrl_Type) * count);
        if (*pprxlli == NULL) {
            blog_error("malloc lli failed.");
            vPortFree(*pptxlli);
            return -1;
        }
    }

    for (i = 0; i < count; i++) {
        if (remainder == 0) {
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
        
        if (NULL != ptx_data) {
            dmactrl.SI = DMA_MINC_ENABLE;
            dmactrl.DI = DMA_MINC_DISABLE;
            (*pptxlli)[i].srcDmaAddr = (uint32_t)(ptx_data + i * LLI_BUFF_SIZE);
            (*pptxlli)[i].destDmaAddr = (uint32_t)(SPI_BASE + SPI_FIFO_WDATA_OFFSET);
            (*pptxlli)[i].dmaCtrl = dmactrl;
            if (i != 0) {
                (*pptxlli)[i-1].nextLLI = (uint32_t)&(*pptxlli)[i];
            }
            (*pptxlli)[i].nextLLI = 0;
        }

        if (NULL != prx_data) {
            dmactrl.SI = DMA_MINC_DISABLE;
            dmactrl.DI = DMA_MINC_ENABLE;
            (*pprxlli)[i].srcDmaAddr = (uint32_t)(SPI_BASE + SPI_FIFO_RDATA_OFFSET);
            (*pprxlli)[i].destDmaAddr = (uint32_t)(prx_data + i * LLI_BUFF_SIZE);
            (*pprxlli)[i].dmaCtrl = dmactrl;
            if (i != 0) {
                (*pprxlli)[i-1].nextLLI = (uint32_t)&(*pprxlli)[i];
            }
            (*pprxlli)[i].nextLLI = 0;
        }
    }

    return 0;
}

static int hosal_spi_dma_trans(hosal_spi_dev_t *arg, uint8_t *TxData, uint8_t *RxData, uint32_t Len, uint32_t timeout)
{
    EventBits_t uxBits;
    DMA_LLI_Cfg_Type txllicfg;
    DMA_LLI_Cfg_Type rxllicfg;
    DMA_LLI_Ctrl_Type *ptxlli = NULL;
    DMA_LLI_Ctrl_Type *prxlli = NULL;
    spi_dma_priv_t *dma_arg = (spi_dma_priv_t*)arg->priv;
    int ret;

    if (!arg) {
        blog_error("arg err.\r\n");
        return -1;
    }

    if (TxData) {
        if (dma_arg->tx_dma_ch == -1) {
            dma_arg->tx_dma_ch = hosal_dma_chan_request(0);
            if (dma_arg->tx_dma_ch < 0) {
                blog_error("SPI TX DMA CHANNEL get failed!\r\n");
                return -1;
            }
        }
    } else {
        dma_arg->tx_dma_ch = -1;
    }
    if (RxData) {
        if (dma_arg->rx_dma_ch == -1) {
            dma_arg->rx_dma_ch = hosal_dma_chan_request(0);
            if (dma_arg->rx_dma_ch < 0) {
                blog_error("SPI RX DMA CHANNEL get failed!\r\n");
                return -1;
            }
        }
    } else {
        dma_arg->rx_dma_ch = -1;
    }

    txllicfg.dir = DMA_TRNS_M2P;
    txllicfg.srcPeriph = DMA_REQ_NONE; 
    txllicfg.dstPeriph = DMA_REQ_SPI_TX;

    rxllicfg.dir = DMA_TRNS_P2M;
    rxllicfg.srcPeriph = DMA_REQ_SPI_RX;
    rxllicfg.dstPeriph = DMA_REQ_NONE;


    xEventGroupClearBits(dma_arg->spi_event_group, EVT_GROUP_SPI_TR);


    if (arg->config.mode == 0) {
        SPI_Enable(arg->port, SPI_WORK_MODE_MASTER);
    } else {
        SPI_Enable(arg->port, SPI_WORK_MODE_SLAVE);
    }

    ret = lli_list_init(&ptxlli, &prxlli, TxData, RxData, Len);
    if (ret < 0) {
        blog_error("init lli failed. \r\n");

        return -1;
    }

    if (NULL != RxData) {
        hosal_dma_irq_callback_set(dma_arg->rx_dma_ch, hosal_spi_int_handler_rx, arg);
        DMA_LLI_Init(dma_arg->rx_dma_ch, &rxllicfg);
        DMA_LLI_Update(dma_arg->rx_dma_ch,(uint32_t)prxlli);
        hosal_dma_chan_start(dma_arg->rx_dma_ch);
    }

    if (NULL != TxData) {
        hosal_dma_irq_callback_set(dma_arg->tx_dma_ch, hosal_spi_int_handler_tx, arg);
        DMA_LLI_Init(dma_arg->tx_dma_ch, &txllicfg);
        DMA_LLI_Update(dma_arg->tx_dma_ch,(uint32_t)ptxlli);
        hosal_dma_chan_start(dma_arg->tx_dma_ch);
    }


    uxBits = xEventGroupWaitBits(dma_arg->spi_event_group,
                                     EVT_GROUP_SPI_TR,
                                     pdTRUE,
                                     pdTRUE,
                                     timeout);

    if ((uxBits & EVT_GROUP_SPI_TR) == EVT_GROUP_SPI_TR) {
        if (dma_arg->tx_dma_ch >= 0) {
            hosal_dma_chan_stop(dma_arg->tx_dma_ch);
            hosal_dma_chan_release(dma_arg->tx_dma_ch);
            dma_arg->tx_dma_ch = -1;
        }
        if (dma_arg->rx_dma_ch >= 0) {
            hosal_dma_chan_stop(dma_arg->rx_dma_ch);
            hosal_dma_chan_release(dma_arg->rx_dma_ch);
            dma_arg->rx_dma_ch = -1;
        }
        if (arg->cb) {
            arg->cb(arg->p_arg);
        }
    } else {
        blog_error(" trans timeout\r\n");
    }

    vPortFree(ptxlli);
    if (NULL != RxData) {
        vPortFree(prxlli);
    }
	return 0;
}

static void hosal_spi_int_handler_tx(void *arg, uint32_t flag)
{
    BaseType_t xResult = pdFAIL;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    hosal_spi_dev_t *dev = (hosal_spi_dev_t *)arg;
    if (NULL != dev) {
        if (dev->config.dma_enable) {
            spi_dma_priv_t *priv=  (spi_dma_priv_t *)dev->priv;
            bl_dma_int_clear(priv->tx_dma_ch);
            if (priv->spi_event_group != NULL) {
                xResult = xEventGroupSetBitsFromISR(priv->spi_event_group,
                                                    EVT_GROUP_SPI_TX,
                                                    &xHigherPriorityTaskWoken);

                if (priv->rx_dma_ch == -1) {
                    xEventGroupSetBitsFromISR(priv->spi_event_group,
                                                    EVT_GROUP_SPI_RX,
                                                    &xHigherPriorityTaskWoken);
                }

            }
     
            if(xResult != pdFAIL) {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        } 
    } else {
        blog_error("hosal_spi_int_handler_tx no clear isr.\r\n");
    }
    return;
}

static void hosal_spi_int_handler_rx(void *arg, uint32_t flag)
{
    BaseType_t xResult = pdFAIL;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    hosal_spi_dev_t *dev = (hosal_spi_dev_t *)arg;
    if (NULL != dev) {
        if (dev->config.dma_enable) {
            spi_dma_priv_t *priv=  (spi_dma_priv_t *)dev->priv;
            bl_dma_int_clear(priv->tx_dma_ch);
            if (priv->spi_event_group != NULL) {
                xResult = xEventGroupSetBitsFromISR(priv->spi_event_group, EVT_GROUP_SPI_RX, &xHigherPriorityTaskWoken);

                if (priv->tx_dma_ch == -1) {
                    xEventGroupSetBitsFromISR(priv->spi_event_group, EVT_GROUP_SPI_TX, &xHigherPriorityTaskWoken);
                }

            }
     
            if(xResult != pdFAIL) {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        } 
    } else {
        blog_error("hosal_spi_int_handler_tx no clear isr.\r\n");
    }
    return;
}

static void spi_irq_process(void *p_arg)
{
    BaseType_t xResult = pdFAIL;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    hosal_spi_dev_t *spi = (hosal_spi_dev_t *)p_arg;
    spi_priv_t *spi_priv = (spi_priv_t *)spi->priv;
    uint32_t tmpVal;
    uint32_t SPIx =SPI_BASE;
    hosal_spi_irq_t pfn;
    void *parg;
    pfn = spi->cb;
    parg = spi->p_arg;
    
    tmpVal = BL_RD_REG(SPIx,SPI_INT_STS);
    if(BL_IS_REG_BIT_SET(tmpVal,SPI_END_INT) && !BL_IS_REG_BIT_SET(tmpVal,SPI_CR_SPI_END_MASK)){
        BL_WR_REG(SPIx,SPI_INT_STS,BL_SET_REG_BIT(tmpVal,SPI_CR_SPI_END_CLR));
    }
    /* TX fifo ready interrupt(fifo count > fifo threshold) */
    if(BL_IS_REG_BIT_SET(tmpVal,SPI_TXF_INT) && !BL_IS_REG_BIT_SET(tmpVal,SPI_CR_SPI_TXF_MASK)){
        if (spi_priv->tx_data) {
            if (spi_priv->tx_index < spi_priv->length) {
                BL_WR_REG(SPIx, SPI_FIFO_WDATA, (uint32_t)spi_priv->tx_data[spi_priv->tx_index]);
                spi_priv->tx_index++;
            } else {
            }
        } else { /* send 0 while only recv */
            if (spi_priv->tx_index < spi_priv->length) {
                BL_WR_REG(SPIx,SPI_FIFO_WDATA, 0);
                spi_priv->tx_index++;
            } else {
            }
        }
    }
    /*  RX fifo ready interrupt(fifo count > fifo threshold) */
    if(BL_IS_REG_BIT_SET(tmpVal,SPI_RXF_INT) && !BL_IS_REG_BIT_SET(tmpVal,SPI_CR_SPI_RXF_MASK)){
        if (spi_priv->rx_data) {
            spi_priv->rx_data[spi_priv->rx_index] = (uint8_t)(BL_RD_REG(SPIx, SPI_FIFO_RDATA)&0xff);
            spi_priv->rx_index++;
            if (spi_priv->rx_index == spi_priv->length) {
                /* spi callback */
                if (pfn) {
                    pfn(parg);
                }
                bl_irq_disable(SPI_IRQn);
                xResult = xEventGroupSetBitsFromISR(spi_priv->spi_event_group, EVT_GROUP_SPI_TX, &xHigherPriorityTaskWoken);
                xEventGroupSetBitsFromISR(spi_priv->spi_event_group, EVT_GROUP_SPI_RX, &xHigherPriorityTaskWoken);
                if(xResult != pdFAIL) {
                    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                }
            }
        } else {
            spi_priv->rx_index++;
            if (spi_priv->rx_index == spi_priv->length) {
                /* spi callback */
                if (pfn) {
                    pfn(parg);
                }
                bl_irq_disable(SPI_IRQn);
                xResult = xEventGroupSetBitsFromISR(spi_priv->spi_event_group, EVT_GROUP_SPI_TX, &xHigherPriorityTaskWoken);
                xEventGroupSetBitsFromISR(spi_priv->spi_event_group, EVT_GROUP_SPI_RX, &xHigherPriorityTaskWoken);
                if(xResult != pdFAIL) {
                    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                }
            }
        }
    }
    /* Slave mode transfer time-out interrupt,triggered when bus is idle for the given value */
    if(BL_IS_REG_BIT_SET(tmpVal,SPI_STO_INT) && !BL_IS_REG_BIT_SET(tmpVal,SPI_CR_SPI_STO_MASK)){
        BL_WR_REG(SPIx,SPI_INT_STS,BL_SET_REG_BIT(tmpVal,SPI_CR_SPI_STO_CLR));
        blog_info(" slave timeout\r\n");
    }
    /* Slave mode tx underrun error interrupt,trigged when tx is not ready during transfer */
    if(BL_IS_REG_BIT_SET(tmpVal,SPI_TXU_INT) && !BL_IS_REG_BIT_SET(tmpVal,SPI_CR_SPI_TXU_MASK)){
        BL_WR_REG(SPIx,SPI_INT_STS,BL_SET_REG_BIT(tmpVal,SPI_CR_SPI_TXU_CLR));
        blog_info(" slave tx underrun error\r\n");
    }
    /* TX/RX fifo overflow/underflow interrupt */
    if(BL_IS_REG_BIT_SET(tmpVal,SPI_FER_INT) && !BL_IS_REG_BIT_SET(tmpVal,SPI_CR_SPI_FER_MASK)){
        blog_info(" tx/rx overfloe/underflow\r\n");
    }
}

static int hosal_spi_trans(hosal_spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data, uint32_t length, uint32_t timeout)
{
    if (!spi) {
        blog_error("arg error\r\n");
        return -1;
    }
    EventBits_t uxBits;
    spi_priv_t *spi_priv = (spi_priv_t *)spi->priv;
    blog_info("spi trans\r\n");
    uint32_t tmpVal;
    uint32_t SPIx = SPI_BASE;
    SPI_ID_Type spi_id; //TODO change SPI_ID_Type
    spi_id = spi->port;

    spi_priv->tx_data = tx_data;
    spi_priv->rx_data = rx_data;
    spi_priv->length  = length;
    spi_priv->tx_index  = 0;
    spi_priv->rx_index  = 0;
    
    /* Set valid width for each fifo entry */
    tmpVal = BL_RD_REG(SPIx,SPI_CONFIG);
    BL_WR_REG(SPIx,SPI_CONFIG,BL_SET_REG_BITS_VAL(tmpVal,SPI_CR_SPI_FRAME_SIZE,0));
    
    /* Disable rx ignore */
    tmpVal = BL_RD_REG(SPIx,SPI_CONFIG);
    BL_WR_REG(SPIx,SPI_CONFIG,BL_CLR_REG_BIT(tmpVal,SPI_CR_SPI_RXD_IGNR_EN));
    
    /* Clear tx and rx fifo */
    tmpVal = BL_RD_REG(SPIx,SPI_FIFO_CONFIG_0);
    tmpVal = BL_SET_REG_BIT(tmpVal,SPI_TX_FIFO_CLR);
    tmpVal = BL_SET_REG_BIT(tmpVal,SPI_RX_FIFO_CLR);
    BL_WR_REG(SPIx,SPI_FIFO_CONFIG_0,tmpVal);

    if (spi->config.mode == 0) {
        SPI_Enable(spi->port, SPI_WORK_MODE_MASTER);
    } else {
        SPI_Enable(spi->port, SPI_WORK_MODE_SLAVE);
    }

    SPI_IntMask(spi_id, SPI_INT_ALL,UNMASK);
    bl_irq_enable(SPI_IRQn);
    uxBits = xEventGroupWaitBits(spi_priv->spi_event_group, EVT_GROUP_SPI_TR, pdTRUE, pdTRUE, timeout);

    if ((uxBits & EVT_GROUP_SPI_TR) == EVT_GROUP_SPI_TR) {
        blog_info("recv all event group.\r\n");
    } else {
        blog_error(" transimission timeout\r\n");
    }
    return 0;

}

static void hosal_spi_gpio_init(hosal_spi_dev_t *arg)
{

    if (!arg) {
        blog_error("arg err.\r\n");
        return;
    }

    
        GLB_GPIO_Type gpiopins[4];
        gpiopins[0] = arg->config.pin_mosi + 1; //pin cs
        gpiopins[1] = arg->config.pin_clk;
        gpiopins[2] = arg->config.pin_mosi;
        gpiopins[3] = arg->config.pin_miso;
        GLB_GPIO_Func_Init(GPIO_FUN_SPI,gpiopins,sizeof(gpiopins)/sizeof(gpiopins[0]));

    if (arg->config.mode == 0) {
        GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_MASTER);
    } else {
        GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_SLAVE);
    }

    return;
}

int hosal_spi_init(hosal_spi_dev_t *spi)
{
    hosal_spi_dev_t *dev = spi;
    if (!spi) {
        blog_error("arg err.\r\n");
    }
    hosal_spi_gpio_init(dev);
    spi_basic_init(dev);
    if (dev->config.dma_enable) {
        spi_dma_priv_t *priv = (spi_dma_priv_t *)pvPortMalloc(sizeof(spi_dma_priv_t));
        priv->spi_event_group = xEventGroupCreate();
        priv->tx_dma_ch = -1;
        priv->rx_dma_ch = -1;
        dev->priv = priv;
    } else {
        spi_priv_t *priv = (spi_priv_t *)pvPortMalloc(sizeof(spi_priv_t));
        priv->spi_event_group = xEventGroupCreate();
        priv->tx_data = NULL;
        priv->rx_data = NULL;
        priv->length  = 0;
        priv->tx_index = 0;
        priv->rx_index = 0;
        dev->priv = priv;
    }
    return 0;
}

int hosal_spi_set_cs(uint8_t pin, uint8_t value)
{
    bl_gpio_enable_output(pin, 1, 0);
    bl_gpio_output_set(pin, value);
    return 0;
}

int hosal_spi_irq_callback_set(hosal_spi_dev_t *spi, hosal_spi_irq_t pfn, void *p_arg)
{
    if (spi == NULL) {
        return -1;
    }
    spi->cb = pfn;
    spi->p_arg = p_arg;
    return 0;
}

int hosal_spi_finalize(hosal_spi_dev_t *spi)
{
    if (spi == NULL) {
        return -1;
    }
    
    if (spi->config.dma_enable) {
        spi_dma_priv_t *spi_priv = (spi_dma_priv_t *)spi->priv;
        if (spi_priv->tx_dma_ch >= 0) {
            hosal_dma_chan_release(spi_priv->tx_dma_ch);
        }
        if (spi_priv->rx_dma_ch >= 0) {
            hosal_dma_chan_release(spi_priv->rx_dma_ch);
        }
        vPortFree(spi_priv);
    } else {
        spi_priv_t *spi_priv = (spi_priv_t *)spi->priv;
        vPortFree(spi_priv);
    }
    bl_irq_disable(SPI_IRQn);
    SPI_DeInit(0);
    return 0;
}

int hosal_spi_send(hosal_spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int ret;

    if (NULL == spi || data == NULL) {
        blog_error("not init.\r\n");
        return -1;
    }

    if (data == NULL) {
        blog_error("wrong para \r\n");
    }
    if (spi->config.dma_enable) {
        ret = hosal_spi_dma_trans(spi, (uint8_t *)data, NULL, size, timeout);
    } else {
        ret = hosal_spi_trans(spi, (uint8_t *)data, NULL, size, timeout);
    }
    return ret;
}

int hosal_spi_recv(hosal_spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
	int ret;

    if (NULL == spi || data == NULL) {
        blog_error("not init.\r\n");
        return -1;
    }

    if (data == NULL) {
        blog_error("wrong para \r\n");
    }
    if (spi->config.dma_enable) {
        ret = hosal_spi_dma_trans(spi, NULL, data, size, timeout);
    } else {
        ret = hosal_spi_trans(spi, NULL, data, size, timeout);
    }
    return ret;
}

int hosal_spi_send_recv(hosal_spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint32_t timeout)
{
	int ret;
    if (NULL == spi || tx_data == NULL || rx_data == NULL) {
        blog_error("not init.\r\n");
        return -1;
    }
    if (spi->config.dma_enable) {
        ret = hosal_spi_dma_trans(spi, (uint8_t *)tx_data, (uint8_t *)rx_data, size, timeout);
    } else {
        ret = hosal_spi_trans(spi, tx_data, rx_data, size, timeout);
    }
    return ret;
}
