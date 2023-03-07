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
#include <bl602_ir.h>
#include <bl602_glb.h>
#include <bl_irq.h>
#include <bl_ir.h>
#include <looprt.h> 
#include <loopset.h>
#include <loopset_ir.h>
#include <stdio.h>
#include <bl_gpio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <cli.h>
#include <string.h>
#include <hal/soc/spi.h>
#include <aos/kernel.h>
#include <bl602_spi.h>
#include <bl602_gpio.h>
#include <bl602_dma.h>
#include <bl_dma.h>
#include <libfdt.h>
#include <utils_log.h>
#include <blog.h>

#define ONE_FRAME_BYTESIZE      3       /* chip 24bits */
#define EVT_GROUP_SPI_DMA_TX    (1<<0)
#define PINGPONG_BUF_SIZE       4080
#define WS2812B_RESET_BYTES     26
#define UCS1903_RESET_BYTES     13
#define WS2812B_COEFFICIENT     15
#define UCS1903_COEFFICIENT     30
#define WS2812B_CONVERT_SIZE    5
#define UCS1903_CONVERT_SIZE    10
#define BASE_FREQ               40000000

static void bl_spi_ir_dma_int_handler_tx(void);

struct ir_rx_ctx {
    uint32_t data_check;
}; 

static void ir_gpio_init(int pin)
{
    GLB_GPIO_FUNC_Type gpioFuns = GPIO_FUN_SWGPIO;
    GLB_GPIO_Func_Init(gpioFuns, (GLB_GPIO_Type*)&pin, 1);

    return;
}

static void ir_init(int pin, int ctrltype)
{
    IR_RxCfg_Type rxcfg = {
        IR_RX_NEC,      /* Set ir rx mode NEC */
        ENABLE,         /* Disable signal of input inverse */
        9000,           /* Pulse width threshold to trigger end condition, 4.5ms @2MHz source clock */
        3400,           /* Pulse width threshold for logic 0/1 detection, 1.7ms */
        DISABLE,        /* Disable input de-glitch function */
        0               /* De-glitch function cycle count */// TODO use De-Glitch
    };

    ir_gpio_init(pin);
    GLB_IR_LED_Driver_Enable();
    GLB_IR_RX_GPIO_Sel(pin);

    IR_Disable(IR_TXRX);
    IR_RxInit((IR_RxCfg_Type*)&rxcfg);
}

uint32_t bl_receivedata(void)
{ 
    return IR_ReceiveData(IR_WORD_0);
}

uint32_t bl_get_bitcount(void)
{
    return IR_GetRxDataBitCount();
}

void bl_enable_rx_int(void)
{
    IR_Enable(IR_RX);
    IR_IntMask(IR_INT_RX, UNMASK);

    return;
}

static int check_cmd(uint32_t data)
{
    uint16_t hdata = 0;

    hdata = ((data >> 24) & 0xff) ^ ((data >> 16) & 0xff);

    if (hdata == 0xff) {
        return 0;
    } else {
        return -1;
    }
}

static int check_addr(uint32_t data)
{
    uint16_t ldata = 0;

    ldata = ((data >> 8) & 0xff) ^ (data & 0xff);

    if (ldata == 0xff) {
        return 0;
    } else {
        return -1;
    }
}

int bl_ir_data_check_config(uint32_t data_check)
{
    struct ir_rx_ctx *pstctx;
   
    bl_irq_ctx_get(IRRX_IRQn, (void **)&pstctx);
    if (pstctx == NULL) {
        return -1;
    }

    pstctx->data_check = data_check;

    return 0;
}

static void ir_interrupt_entry(void)
{
    uint32_t data;
    int flag_cmd = 0;
    int flag_addr = 0;
    struct ir_rx_ctx *pstctx;
    
    bl_irq_ctx_get(IRRX_IRQn, (void **)&pstctx);
    IR_Disable(IR_RX);
    IR_IntMask(IR_INT_RX, MASK);
    IR_ClrIntStatus(IR_INT_RX);

    data = bl_receivedata();

    if (data == 0) {
        ir_async_post();
        return;
    }

    if (pstctx->data_check & BL_IR_CHECK_CMD) {
        flag_cmd = check_cmd(data);
    }
 
    if (pstctx->data_check & BL_IR_CHECK_ADDR) {
        flag_addr = check_addr(data);
    }

    if (flag_cmd != 0 || flag_addr != 0) {
        bl_enable_rx_int();
        return;
    } 

    //TODO use with irq context
    ir_async_post();
}

int bl_ir_init(int pin, int ctrltype, int data_check)
{
    struct ir_rx_ctx *pstctx;
    
    pstctx = pvPortMalloc(sizeof(struct ir_rx_ctx));
    if (pstctx == NULL) {
        return -1;
    }

    pstctx->data_check = data_check; 
    ir_init(pin, ctrltype);
    bl_irq_register_with_ctx(IRRX_IRQn, ir_interrupt_entry, pstctx);
    bl_irq_enable(IRRX_IRQn);
    IR_Enable(IR_RX);
    IR_IntMask(IR_INT_RX, UNMASK);

    return 0;
}

/*
 *  spi clock select 4M, 
 *  two kinds chip WS2812B and UCS1903 ,default chip is WS2812B ,5 spi bits mean WS2812B 0 or 1, 10 spi bits mean UCS1903 0 or 1 
 *
 * */
static int ir_convert_ws2812b(uint8_t *pbuf0_tx, uint8_t *data, uint32_t nbytes)
{
    int i, status, n = 0;
    uint32_t nbits, code;
    /*data is 24bits Integer multiple*/
    if (nbytes % ONE_FRAME_BYTESIZE != 0) {
        blog_error("param error, nbytes = %ld \r\n", nbytes);
        return -1;
    }

    nbits = nbytes * 8;

    int a = 0;
    for (i = 0; i < nbits; i++) {
        /* 5 spi bits mean WS2812B 0 or 1,we let each group 8 WS2812B bits,so one group just right need 5 spi bytes*/
        status = i % 8;
        /*move point*/
        if (i != 0 && i % 8 == 0) {
            data++;
            a++;
            n = 3 * a;
        }
        code = ((*data >> (7 - (i % 8))) & 0x01);
        switch (status) {
            case 0:
            {
                /*0x00 or 0x01 means chip WS2812B bit*/
                if (code == 0x00) {
                    pbuf0_tx[i-n] = 0xC0;
                } else {
                    pbuf0_tx[i-n] = 0xE0;
                }
            }
            break;
            case 1:
            {
                if (code == 0x00) {
                    pbuf0_tx[i-n-1] = (pbuf0_tx[i-n-1] | 0x06);
                    pbuf0_tx[i-n] = 0x00;
                } else {
                    pbuf0_tx[i-n-1] = (pbuf0_tx[i-n-1] | 0x07);
                    pbuf0_tx[i-n] = 0x00;
                }
            }
            break;
            case 2:
            {   
                if (code == 0x00) {
                    pbuf0_tx[i-n-1] = (pbuf0_tx[i-n-1] | 0x30);
                } else {
                    pbuf0_tx[i-n-1] = (pbuf0_tx[i-n-1] | 0x38);
                }    
            }
            break;
            case 3:
            {
                if (code == 0x00) {
                    pbuf0_tx[i-n-2] = (pbuf0_tx[i-n-2] | 0x01);
                    pbuf0_tx[i-n-1] = 0x80;
                } else {
                    pbuf0_tx[i-n-2] = (pbuf0_tx[i-n-2] | 0x01);
                    pbuf0_tx[i-n-1] = 0xC0;
                }
            }
            break;
            case 4:
            {
                if (code == 0x00) {
                    
                    pbuf0_tx[i-n-2] = (pbuf0_tx[i-n-2] | 0x0C);
                    pbuf0_tx[i-n-1] = 0x00;
                } else {
                    pbuf0_tx[i-n-2] = (pbuf0_tx[i-n-2] | 0x0E);
                    pbuf0_tx[i-n-1] = 0x00;
                }
            }
            break;
            case 5:
            {
                if (code == 0x00) {
                    pbuf0_tx[i-n-2] = (pbuf0_tx[i-n-2] | 0x60);
                } else {
                    pbuf0_tx[i-n-2] = (pbuf0_tx[i-n-2] | 0x70);
                }
            }     
            break;
            case 6:
            {
                if (code == 0x00) {
                    pbuf0_tx[i-n-3] = (pbuf0_tx[i-n-3] | 0x03);
                    pbuf0_tx[i-n-2] = 0x00;
                } else {
                    pbuf0_tx[i-n-3] = (pbuf0_tx[i-n-3] | 0x03);
                    pbuf0_tx[i-n-2] = 0x80;
                }
            }     
            break;
            case 7:
            {
                if (code == 0x00) {
                    pbuf0_tx[i-n-3] = (pbuf0_tx[i-n-3] | 0x18);
                } else {
                    pbuf0_tx[i-n-3] = (pbuf0_tx[i-n-3] | 0x1C);
                }
            }           
            break;
            default:
            {
                blog_error("ERROR\r\n");
                return -1;
            }
        }
    }

    return 0;
}

static int ir_convert_usc1903(uint8_t *pbuf0_tx, uint8_t *data, uint32_t nbytes)
{
    int i, status, n = 0;
    uint32_t nbits, code;
    /*data is 24bits Integer multiple*/
    if (nbytes % ONE_FRAME_BYTESIZE != 0) {
        blog_error("param error, nbytes = %ld \r\n", nbytes);
        return -1;
    }

    nbits = nbytes * 8;

    for (i = 0; i < nbits; i++) {
        /* 10 spi bits mean UCS1903 0 or 1,we let each group 4 UCS1903 bits,so one group just right need 5 spi bytes*/
        status = i % 4;
        /*move point*/
        if (i != 0 && i % 8 == 0) {
            data++;
        }
        if (i != 0 && i % 4 == 0) {
            n++;
        }
        code = ((*data >> (7 - (i % 8))) & 0x01);
        switch (status) {
            case 0:
            {
                if (code == 0x00) {
                    pbuf0_tx[i+n] = 0xC0;
                    pbuf0_tx[i+n+1] = 0x00;
                } else {
                    pbuf0_tx[i+n] = 0xFF;
                    pbuf0_tx[i+n+1] = 0x00;
                }
            }
            break;
            case 1:
            {
                if (code == 0x00) {
                    pbuf0_tx[i+n] = (pbuf0_tx[i+n] | 0x30);
                    pbuf0_tx[i+n+1] = 0x00;
                } else {
                    
                    pbuf0_tx[i+n] = (pbuf0_tx[i+n] | 0x3F);
                    pbuf0_tx[i+n+1] = 0xC0;
                }
            }
            break;
            case 2:
            {   
                if (code == 0x00) {
                    pbuf0_tx[i+n] = (pbuf0_tx[i+n] | 0x0C);
                    pbuf0_tx[i+n+1] = 0x00;
                } else {
                    pbuf0_tx[i+n] = (pbuf0_tx[i+n] | 0x0F);
                    pbuf0_tx[i+n+1] = 0xF0;
                }    
            }
            break;
            case 3:
            {
                if (code == 0x00) {
                    pbuf0_tx[i+n] = (pbuf0_tx[i+n] | 0x03);
                    pbuf0_tx[i+n+1] = 0x00;
                } else {
                    pbuf0_tx[i+n] = (pbuf0_tx[i+n] | 0x03);
                    pbuf0_tx[i+n+1] = 0xFC;
                }
            }
            break;
            default:
            {
                blog_error("ERROR\r\n");
                return -1;
            }
        }
    }

    return 0;
}

static void ir_data_convert(uint8_t *dst, uint32_t *src, uint32_t size, int type)
{
    uint8_t tmp_buf[300];
    uint32_t i;
    uint32_t count;
    uint32_t or_idx;
   
    count = 0;
    or_idx = 0;
    for (i = 0; i < size; i++) {
        tmp_buf[3 * count + 2] = src[i] & 0xff;
        tmp_buf[3 * count + 1] = (src[i] >> 8) & 0xff;
        tmp_buf[3 * count] = (src[i] >> 16) & 0xff;
        count++;
        if (count == 100) {
            if (type == 0) {
                ir_convert_ws2812b(dst + or_idx * WS2812B_CONVERT_SIZE, tmp_buf, 100 * 3); 
            } else {
                ir_convert_usc1903(dst + or_idx * UCS1903_CONVERT_SIZE, tmp_buf, 100 * 3);
            }
            count = 0;
            or_idx = or_idx + 300;
        } else if (i == size - 1){
            if (type == 0) {
                ir_convert_ws2812b(dst + or_idx * WS2812B_CONVERT_SIZE, tmp_buf, (size - or_idx / 3) * 3); 
            } else {
                ir_convert_usc1903(dst + or_idx * UCS1903_CONVERT_SIZE, tmp_buf, (size - or_idx / 3) * 3);
            } 
        }
    }
    
    return;
}

int bl_spi_hw_init(private_ir_data_t *pstctx)
{
    GLB_GPIO_Type gpiopins[2];
    spi_ir_hw_t *arg;

    if (!pstctx) {
        blog_error("ctx err.\r\n");
        return -1;
    }
    
    arg = &(pstctx->dev_spi);
    gpiopins[0] = arg->pin_clk;
    gpiopins[1] = arg->pin_mosi;
   
    GLB_GPIO_Func_Init(GPIO_FUN_SPI,gpiopins, sizeof(gpiopins) / sizeof(gpiopins[0]));
    if (arg->mode == HAL_SPI_MODE_MASTER) {
        GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_MASTER);
    } else {
        GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_SLAVE);
    }
    
    arg->spi_dma_ir_event_group = xEventGroupCreate();
    pstctx->end_flag = 0;
    pstctx->buf_flag = 0;
    pstctx->data_idx = 0;
    pstctx->chip_coe = 0;

    pstctx->pp_buf0 = bl_dma_mem_malloc(PINGPONG_BUF_SIZE + WS2812B_RESET_BYTES);
    if (pstctx->pp_buf0 == NULL) {
        blog_error("not avaible ram, malloc failed. \r\n");

        return -1;
    }

    pstctx->pp_buf1 = bl_dma_mem_malloc(PINGPONG_BUF_SIZE + WS2812B_RESET_BYTES);
    if (pstctx->pp_buf1 == NULL) {
        blog_error("not enouth DTCM, malloc failed. \r\n");
        vPortFree(pstctx->pp_buf0);

        return -1;
    }

    return 0;
}

static int lli_list_init(private_ir_data_t *pstctx, uint32_t *ptx_data, uint32_t length)
{
    struct DMA_Control_Reg dmactrl;
    uint32_t size;

    dmactrl.SBSize = DMA_BURST_SIZE_1;
    dmactrl.DBSize = DMA_BURST_SIZE_1;
    dmactrl.SWidth = DMA_TRNS_WIDTH_8BITS;
    dmactrl.DWidth = DMA_TRNS_WIDTH_8BITS;
    dmactrl.Prot = 0;
    dmactrl.SLargerD = 0;

    pstctx->ptxlli = pvPortMalloc(sizeof(DMA_LLI_Ctrl_Type) * 2);
    if (pstctx->ptxlli == NULL) {
        blog_error("malloc lli failed. \r\n");

        return -1;
    }

    dmactrl.SI = DMA_MINC_ENABLE;
    dmactrl.DI = DMA_MINC_DISABLE; 
    dmactrl.I = 1;

    if (pstctx->chip_type == CHIP_WS2812B) {
        pstctx->chip_coe = WS2812B_COEFFICIENT;
        pstctx->reset_size = WS2812B_RESET_BYTES;
    } else {
        pstctx->chip_coe = UCS1903_COEFFICIENT;
        pstctx->reset_size = UCS1903_RESET_BYTES;
    }

    size = pstctx->chip_coe * length;
    if (size < PINGPONG_BUF_SIZE || size == PINGPONG_BUF_SIZE) {
        ir_data_convert(pstctx->pp_buf0, ptx_data, length, pstctx->chip_type);
        memset(pstctx->pp_buf0 + size, 0, pstctx->reset_size);
        dmactrl.TransferSize = size + pstctx->reset_size;
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[0].srcDmaAddr = (uint32_t)(pstctx->pp_buf0);
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[0].destDmaAddr = (uint32_t)(SPI_BASE+SPI_FIFO_WDATA_OFFSET);
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[0].dmaCtrl = dmactrl;
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[0].nextLLI = 0;
        pstctx->left_size = 0;
        pstctx->end_flag = 0;
        pstctx->data_idx = 0;
    } else {
        ir_data_convert(pstctx->pp_buf0, ptx_data, PINGPONG_BUF_SIZE / pstctx->chip_coe, pstctx->chip_type);
        dmactrl.TransferSize = PINGPONG_BUF_SIZE; 
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[0].srcDmaAddr = (uint32_t)(pstctx->pp_buf0);
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[0].destDmaAddr = (uint32_t)(SPI_BASE+SPI_FIFO_WDATA_OFFSET);
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[0].dmaCtrl = dmactrl;
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[0].nextLLI = (uint32_t)&((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[1];
        
        pstctx->left_size = length - PINGPONG_BUF_SIZE / pstctx->chip_coe;
        if (pstctx->left_size  * pstctx->chip_coe < PINGPONG_BUF_SIZE || pstctx->left_size  * pstctx->chip_coe == PINGPONG_BUF_SIZE) {
            dmactrl.TransferSize = pstctx->left_size * pstctx->chip_coe + pstctx->reset_size;
            ir_data_convert(pstctx->pp_buf1, &(ptx_data[PINGPONG_BUF_SIZE / pstctx->chip_coe]), pstctx->left_size, pstctx->chip_type);
            memset(pstctx->pp_buf1 + pstctx->left_size * pstctx->chip_coe, 0, pstctx->reset_size);
            pstctx->left_size = 0;
        } else {
            dmactrl.TransferSize = PINGPONG_BUF_SIZE;
            ir_data_convert(pstctx->pp_buf1, &(ptx_data[PINGPONG_BUF_SIZE / pstctx->chip_coe]), PINGPONG_BUF_SIZE / pstctx->chip_coe, pstctx->chip_type);
            pstctx->left_size = pstctx->left_size - PINGPONG_BUF_SIZE / pstctx->chip_coe;
            pstctx->data_idx = PINGPONG_BUF_SIZE / pstctx->chip_coe * 2;
        }

        pstctx->end_flag = 1; 
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[1].srcDmaAddr = (uint32_t)(pstctx->pp_buf1);
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[1].destDmaAddr = (uint32_t)(SPI_BASE+SPI_FIFO_WDATA_OFFSET);
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[1].dmaCtrl = dmactrl;
        if (pstctx->left_size == 0) {
            ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[1].nextLLI = 0;
        } else {
            ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[1].nextLLI = (uint32_t)&((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[0];
        }
    }

    return 0;
}

void bl_spi_dma_init(private_ir_data_t *pstctx)
{
    spi_ir_hw_t *hw_arg = &(pstctx->dev_spi);
    SPI_CFG_Type spicfg;
    SPI_ClockCfg_Type clockcfg;
    SPI_FifoCfg_Type fifocfg;
    SPI_ID_Type spi_id;
    uint8_t clk_div;
    
    //spi_id = hw_arg->ssp_id;
    spi_id = 0;
    /* clock */
    /*1  --->  40 Mhz
     *2  --->  20 Mhz
     *5  --->  8  Mhz
     *6  --->  6.66 Mhz
     *10 --->  4 Mhz
     * */
    clk_div = (uint8_t)(BASE_FREQ / hw_arg->freq);
    GLB_Set_SPI_CLK(ENABLE,0);
    clockcfg.startLen = clk_div;
    clockcfg.stopLen = clk_div;
    clockcfg.dataPhase0Len = clk_div;
    clockcfg.dataPhase1Len = clk_div;
    clockcfg.intervalLen = clk_div;
    SPI_ClockConfig(0, &clockcfg);

    /* spi config */
    spicfg.deglitchEnable = DISABLE;
    spicfg.continuousEnable = ENABLE;
    spicfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST;
    spicfg.bitSequence = SPI_BIT_INVERSE_MSB_FIRST;
    spicfg.frameSize = SPI_FRAME_SIZE_8;

    if (hw_arg->polar_phase == 0) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_0;
        spicfg.clkPolarity = SPI_CLK_POLARITY_LOW;
    } else if (hw_arg->polar_phase == 1) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_1;
        spicfg.clkPolarity = SPI_CLK_POLARITY_LOW;
    } else if (hw_arg->polar_phase == 2) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_0;
        spicfg.clkPolarity = SPI_CLK_POLARITY_HIGH;
    } else if (hw_arg->polar_phase == 3) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_1;
        spicfg.clkPolarity = SPI_CLK_POLARITY_HIGH;
    } else {
        blog_error("node support polar_phase \r\n");
    }
    SPI_Init(0, &spicfg);

    if (hw_arg->mode == HAL_SPI_MODE_MASTER)
    {
        SPI_Disable(0, SPI_WORK_MODE_MASTER);
    } else {
        SPI_Disable(0, SPI_WORK_MODE_SLAVE);
    }

    SPI_IntMask(spi_id,SPI_INT_ALL,MASK);

    /* fifo */
    fifocfg.txFifoThreshold = 1;
    fifocfg.rxFifoThreshold = 1;
    fifocfg.txFifoDmaEnable = ENABLE;
    fifocfg.rxFifoDmaEnable = ENABLE;
    SPI_FifoConfig(spi_id,&fifocfg);

    DMA_Disable();
    DMA_IntMask(hw_arg->tx_dma_ch, DMA_INT_ALL, MASK);
    DMA_IntMask(hw_arg->tx_dma_ch, DMA_INT_TCOMPLETED, UNMASK);
    DMA_IntMask(hw_arg->tx_dma_ch, DMA_INT_ERR, UNMASK);
   
    bl_irq_enable(DMA_ALL_IRQn);
    bl_dma_irq_register(hw_arg->tx_dma_ch, bl_spi_ir_dma_int_handler_tx, NULL, pstctx);

    return;
}

int bl_spi_dma_trans(private_ir_data_t *pstctx, uint32_t *TxData, uint32_t Len)
{
    DMA_LLI_Cfg_Type txllicfg;
    spi_ir_hw_t *arg = &(pstctx->dev_spi);
    int ret = 0;
    EventBits_t uxBits;

    txllicfg.dir = DMA_TRNS_M2P;
    txllicfg.srcPeriph = DMA_REQ_NONE; 
    txllicfg.dstPeriph = DMA_REQ_SPI_TX;
    DMA_Channel_Disable(arg->tx_dma_ch);
    bl_dma_int_clear(arg->tx_dma_ch);
    DMA_Enable();

    if (arg->mode == HAL_SPI_MODE_MASTER) {
        SPI_Enable(0, SPI_WORK_MODE_MASTER);
    } else {
        SPI_Enable(0, SPI_WORK_MODE_SLAVE);
    }

    pstctx->end_flag = 0;
    pstctx->buf_flag = 0;
    pstctx->data_idx = 0;

    ret = lli_list_init(pstctx, TxData, Len);
    if (ret < 0) {
        blog_error("init lli failed. \r\n");

        return -1;
    }
    
    xEventGroupClearBits(arg->spi_dma_ir_event_group, EVT_GROUP_SPI_DMA_TX);

    DMA_LLI_Init(arg->tx_dma_ch, &txllicfg);
    DMA_LLI_Update(arg->tx_dma_ch,(uint32_t)((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli)));
    DMA_Channel_Enable(arg->tx_dma_ch);
    
    uxBits = xEventGroupWaitBits(arg->spi_dma_ir_event_group, EVT_GROUP_SPI_DMA_TX, pdTRUE, pdTRUE, portMAX_DELAY);
    if ((uxBits & EVT_GROUP_SPI_DMA_TX) == EVT_GROUP_SPI_DMA_TX) {
        blog_info("recv all event group.\r\n");
    }

    vPortFree(pstctx->ptxlli);

    return ret;
}

static int update_pingpong_buf(private_ir_data_t *pstctx)
{
    uint32_t len;

    if (pstctx->left_size * pstctx->chip_coe < PINGPONG_BUF_SIZE || pstctx->left_size * pstctx->chip_coe == PINGPONG_BUF_SIZE) {
        len = pstctx->left_size * pstctx->chip_coe;
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[pstctx->buf_flag].nextLLI = 0;
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[pstctx->buf_flag].dmaCtrl.TransferSize = len + pstctx->reset_size;
        if (pstctx->buf_flag == 0) {
            ir_data_convert(pstctx->pp_buf0, &((pstctx->p_data)[pstctx->data_idx]), pstctx->left_size, pstctx->chip_type);
            memset(pstctx->pp_buf0 + len, 0, pstctx->reset_size);
        } else {
            ir_data_convert(pstctx->pp_buf1, &((pstctx->p_data)[pstctx->data_idx]), pstctx->left_size, pstctx->chip_type);
            memset(pstctx->pp_buf1 + len, 0, pstctx->reset_size);
        }

        pstctx->left_size = 0;
        pstctx->data_idx = 0;
    } else {
        pstctx->left_size = pstctx->left_size - PINGPONG_BUF_SIZE / pstctx->chip_coe;
        len = PINGPONG_BUF_SIZE;
        ((DMA_LLI_Ctrl_Type *)(pstctx->ptxlli))[pstctx->buf_flag].dmaCtrl.TransferSize = len;
        if (pstctx->buf_flag == 0) {
            ir_data_convert(pstctx->pp_buf0, &(pstctx->p_data[pstctx->data_idx]), PINGPONG_BUF_SIZE / pstctx->chip_coe, pstctx->chip_type);
        } else {
            ir_data_convert(pstctx->pp_buf1, &(pstctx->p_data[pstctx->data_idx]), PINGPONG_BUF_SIZE / pstctx->chip_coe, pstctx->chip_type);
        }

        pstctx->data_idx = pstctx->data_idx + len / pstctx->chip_coe;
    }

    return 0;
}

static void bl_spi_ir_dma_int_handler_tx(void)
{
    BaseType_t xResult = pdFAIL;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    private_ir_data_t *pstctx;

    pstctx =  bl_dma_find_ctx_by_channel(IR_DMA_CHANNEL);
    if (pstctx == NULL) {
        blog_error("get ctx by dma channel failed. \r\n");

        return;
    }
 
    if (pstctx->end_flag == 0) {
        bl_dma_int_clear((pstctx->dev_spi).rx_dma_ch);
        if ((pstctx->dev_spi).spi_dma_ir_event_group != NULL) {
            xResult = xEventGroupSetBitsFromISR((pstctx->dev_spi).spi_dma_ir_event_group, EVT_GROUP_SPI_DMA_TX, &xHigherPriorityTaskWoken);
        }

        if (xResult != pdFAIL) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    } else {
        if (pstctx->left_size == 0) {
            pstctx->end_flag = 0;   
        } else {
            update_pingpong_buf(pstctx);
        }
    }

    if (pstctx->buf_flag == 0) {
        pstctx->buf_flag = 1;
    } else {
        pstctx->buf_flag = 0;
    }

    return;
}
