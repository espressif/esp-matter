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

#include "blog.h"
#include "bl_irq.h"
#include "bl_timer.h"
#include "bl602_gpio.h"
#include "bl602_adc.h"
#include "bl602_dma.h"
#include "bl602_glb.h"
#include "hosal_adc.h"
#include "hosal_dma.h"

#define ADC_CLOCK_FREQ    96000000
#define ADC_GPIO_NUM        10

#ifdef CONF_ADC_ENABLE_TSEN
#define ADC_CHANNEL_MAX     11          /* channel14 for tsen*/
#else
#define ADC_CHANNEL_MAX     10
#endif

typedef struct adc_ctx {
    uint32_t *channel_data;
    void *adc_lli;
    void *llibuf;
    int lli_flag;
#ifdef CONF_ADC_ENABLE_TSEN
    uint32_t tsen_v0;
    uint32_t tsen_v1;
    uint16_t tsen_offset;
    int tsen_data;
    uint8_t tsen_flag;
#endif
    uint32_t chan_init_table;
    uint32_t data_size;
}hosal_adc_ctx_t;

static hosal_adc_dev_t *pgdevice;

#ifdef CONF_ADC_ENABLE_TSEN
static void update_tsen_v(hosal_adc_ctx_t *pstctx, uint8_t flag)
{
    uint16_t i = 0;
    uint32_t value = 0;

    for (i = 0; i < ADC_CHANNEL_MAX; i++) {
        if (pstctx->channel_data[i] >> 21 == ADC_CHAN_TSEN_P) {
            value = (pstctx->channel_data[i] & 0xffff);
        }
    }

    if (flag == 0) {
        pstctx->tsen_v0 = value;
    } else {
        pstctx->tsen_v1 = value;
    }
}

static void update_tsen_value(hosal_adc_ctx_t *pstctx)
{
    uint32_t v0, v1;

    v0 = pstctx->tsen_v0 & 0xffff;
    v1 = pstctx->tsen_v1 & 0xffff;
    
    if (v0 > v1) {
        pstctx->tsen_data = (((float)v0 - (float)v1) - (float)(pstctx->tsen_offset)) / 7.753;
    } else {
        pstctx->tsen_data = (((float)v1 - (float)v0) - (float)(pstctx->tsen_offset)) / 7.753;
    }
}
#endif

void adc_data_update (void *p_arg, uint32_t flag)
{
    hosal_adc_dev_t *adc = (hosal_adc_dev_t *)p_arg;
    hosal_adc_ctx_t *pstctx = (hosal_adc_ctx_t *)adc->priv;

    if (pstctx->lli_flag == 0) {
        pstctx->channel_data = (uint32_t *)((DMA_LLI_Ctrl_Type *)(pstctx->adc_lli))[0].destDmaAddr;
        pstctx->lli_flag = 1;

#ifdef CONF_ADC_ENABLE_TSEN 
        if (pstctx->tsen_flag == 0) {
            update_tsen_v(pstctx, 0);
            ADC_SET_TSVBE_HIGH();
        }

        if (pstctx->tsen_flag == 1) {
            update_tsen_v(pstctx, 1);
            update_tsen_value(pstctx);
            ADC_SET_TSVBE_LOW();
        }
        pstctx->tsen_flag = !pstctx->tsen_flag;
#endif

    } else {
        pstctx->channel_data = (uint32_t *)((DMA_LLI_Ctrl_Type *)(pstctx->adc_lli))[1].destDmaAddr;
        pstctx->lli_flag = 0;
    }
}

static int adc_check_gpio_valid(GLB_GPIO_Type pin)
{
    int i;
    
    GLB_GPIO_Type gpio_arr[ADC_GPIO_NUM] =
    {
        GLB_GPIO_PIN_4, GLB_GPIO_PIN_5, GLB_GPIO_PIN_6,
        GLB_GPIO_PIN_9, GLB_GPIO_PIN_10, GLB_GPIO_PIN_11,
        GLB_GPIO_PIN_12, GLB_GPIO_PIN_13, GLB_GPIO_PIN_14,
        GLB_GPIO_PIN_15,
    };

    for (i = 0; i < ADC_GPIO_NUM; i++) {
        if (pin == gpio_arr[i]) {
            return 0;
        }
    }

    log_error("gpio %d can not used as adc\r\n", pin);

    return -1;
}

static int adc_get_channel_by_gpio(GLB_GPIO_Type pin)
{
    int channel = -1;

    switch (pin) {
        case GLB_GPIO_PIN_4:
            channel = 1;
            break;
        case GLB_GPIO_PIN_5:
            channel = 4;
            break;
        case GLB_GPIO_PIN_6:
            channel = 5;
            break;
        case GLB_GPIO_PIN_9:
            channel = 7;
            break;
        case GLB_GPIO_PIN_10:
            channel = 9;
            break;
        case GLB_GPIO_PIN_11:
            channel = 10;
            break;
        case GLB_GPIO_PIN_12:
            channel = 0;
            break;
        case GLB_GPIO_PIN_13:
            channel = 3;
            break;
        case GLB_GPIO_PIN_14:
            channel = 2;
            break;
        case GLB_GPIO_PIN_15:
            channel = 11;
            break;

        default :
            channel = -1;
            break;
    }

    return channel;
}

static void adc_freq_init(hosal_adc_sample_mode_t mode, uint32_t freq)
{
    uint32_t div;
    uint32_t source_freq;
    uint32_t mode_freq;

    if (mode == HOSAL_ADC_ONE_SHOT) {
        mode_freq = ADC_CHANNEL_MAX;
    } else {
        mode_freq = 1;
    }

    source_freq = ADC_CLOCK_FREQ / (256 * 20 * mode_freq);

    div = source_freq / freq;
    if (((div + 1) * freq - source_freq) < (source_freq - freq * div)) {
        div = div + 1;
    }

    if (div > 64) {
        div = 64; 
    }

    /*adc clk can not more than 2M*/
    /*when adc work at scan mode, adc clk can not more than 1M*/
    if (mode == HOSAL_ADC_ONE_SHOT) {
        if (div < 5) {
            div = 5;
        }
    } else {
        if (div < 3) {
            div = 3;
        }
    }   

    log_info("ADC freq: %ldHz. div:%lu\r\n", (int)(source_freq / div), div);

    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_96M, div - 1);
}

static void adc_dma_lli_init(DMA_LLI_Ctrl_Type *pstlli, uint32_t *buf, uint32_t data_num)
{
    struct DMA_Control_Reg dma_ctrl_reg;

    dma_ctrl_reg.TransferSize = data_num;
    dma_ctrl_reg.SBSize = DMA_BURST_SIZE_1;
    dma_ctrl_reg.DBSize = DMA_BURST_SIZE_1;
    dma_ctrl_reg.SWidth = DMA_TRNS_WIDTH_32BITS;
    dma_ctrl_reg.DWidth = DMA_TRNS_WIDTH_32BITS;
    dma_ctrl_reg.SI = DMA_MINC_DISABLE;
    dma_ctrl_reg.DI = DMA_MINC_ENABLE;
    dma_ctrl_reg.I = 1;
    dma_ctrl_reg.SLargerD = 0;
    dma_ctrl_reg.Prot = 0;

    pstlli[0].srcDmaAddr = 0x40002000+0x4;
    pstlli[0].destDmaAddr = (uint32_t)&buf[0];
    pstlli[0].nextLLI = (uint32_t)&pstlli[1]; 
    pstlli[0].dmaCtrl= dma_ctrl_reg;

    pstlli[1].srcDmaAddr = 0x40002000+0x4;
    pstlli[1].destDmaAddr = (uint32_t)&buf[ADC_CHANNEL_MAX];
    pstlli[1].nextLLI = (uint32_t)&pstlli[0];
    pstlli[1].dmaCtrl= dma_ctrl_reg;
}

static int adc_dma_init(hosal_adc_dev_t *adc, uint32_t data_num)
{
    DMA_LLI_Ctrl_Type *pstlli;
    uint32_t *llibuf;
    DMA_LLI_Cfg_Type llicfg;
    hosal_adc_ctx_t *pstctx = (hosal_adc_ctx_t *)adc->priv;

    if (data_num < 1) {
        blog_error("illegal para. \r\n");

        return -1;
    }

    adc->dma_chan = hosal_dma_chan_request(0);

    pstlli = pvPortMalloc(sizeof(DMA_LLI_Ctrl_Type) * 2);
    if (NULL == pstlli) {
        blog_error("malloc lli failed. \r\n");

        return -1;
    }

    llibuf = pvPortMalloc(sizeof(uint32_t) * data_num * 2);
    if (NULL == llibuf) {
        blog_error("malloc lli buf failed. \r\n");

        return -1;
    }

    llicfg.dir = DMA_TRNS_P2M;
    llicfg.srcPeriph = DMA_REQ_GPADC0;
    llicfg.dstPeriph = DMA_REQ_NONE;
    DMA_Channel_Disable(adc->dma_chan);

    adc_dma_lli_init(pstlli, llibuf, data_num);
    DMA_LLI_Init(adc->dma_chan, &llicfg);
    DMA_LLI_Update(adc->dma_chan, (uint32_t)&(pstlli[0]));

    pstctx->llibuf = llibuf;
    pstctx->adc_lli = pstlli;
    pstctx->lli_flag = 0;
    pstctx->chan_init_table = 0;
    pstctx->channel_data = NULL;
    pstctx->data_size = data_num;

#ifdef CONF_ADC_ENABLE_TSEN
    pstctx->tsen_data = 0;
    pstctx->tsen_v0 = 0;
    pstctx->tsen_v1 = 0;
    pstctx->tsen_flag = 0;
#endif

    hosal_dma_irq_callback_set(adc->dma_chan, adc_data_update, adc);

    return 0;
}

//mode = 0, for normal adc. freq 27HZ~340HZ
static void adc_init(hosal_adc_dev_t *adc)
{
    int i, chan;
#ifdef CONF_ADC_ENABLE_TSEN
    uint8_t channel_table[ADC_CHANNEL_MAX] = {0, 1, 2, 3, 4, 5, 7, 9, 10, 11, 14};
#else
    uint8_t channel_table[ADC_CHANNEL_MAX] = {0, 1, 2, 3, 4, 5, 7, 9, 10, 11};
#endif
    hosal_adc_sample_mode_t mode = adc->config.mode;
    GLB_GPIO_Type pin = adc->config.pin;

    ADC_CFG_Type adccfg;
    ADC_Chan_Type pos_chlist_single[ADC_CHANNEL_MAX];
    ADC_Chan_Type neg_chlist_single[ADC_CHANNEL_MAX];
    ADC_FIFO_Cfg_Type adc_fifo_cfg;

    adccfg.v18Sel=ADC_V18_SEL_1P82V;
    adccfg.v11Sel=ADC_V11_SEL_1P1V;
    adccfg.clkDiv=ADC_CLK_DIV_20;
    adccfg.resWidth=ADC_DATA_WIDTH_16_WITH_256_AVERAGE;

    /* one shot mode */
    if (mode == HOSAL_ADC_ONE_SHOT) {

#ifdef CONF_ADC_ENABLE_TSEN
        adccfg.gain1=ADC_PGA_GAIN_1;
        adccfg.gain2=ADC_PGA_GAIN_1;
        adccfg.chopMode=ADC_CHOP_MOD_AZ_ON;
#else
        adccfg.gain1=ADC_PGA_GAIN_NONE;
        adccfg.gain2=ADC_PGA_GAIN_NONE;
        adccfg.chopMode=ADC_CHOP_MOD_AZ_ON;
#endif

    } else {
        adccfg.gain1=ADC_PGA_GAIN_1;
        adccfg.gain2=ADC_PGA_GAIN_1;
        adccfg.chopMode=ADC_CHOP_MOD_AZ_PGA_ON;
    }

    adccfg.biasSel=ADC_BIAS_SEL_MAIN_BANDGAP;
    adccfg.vcm=ADC_PGA_VCM_1V;

#ifdef CONF_ADC_ENABLE_TSEN
    adccfg.vref=ADC_VREF_2V;
#else
    adccfg.vref=ADC_VREF_3P2V;
#endif

    adccfg.inputMode=ADC_INPUT_SINGLE_END;
    adccfg.offsetCalibEn=0;
    adccfg.offsetCalibVal=0;

    ADC_Disable();
    ADC_Enable();
    ADC_Reset();

    ADC_Init(&adccfg);
 
    if (mode == HOSAL_ADC_ONE_SHOT) {
        for (i = 0; i < ADC_CHANNEL_MAX; i++) {
            pos_chlist_single[i] = channel_table[i];;
            neg_chlist_single[i] = ADC_CHAN_GND;
        }

        ADC_Scan_Channel_Config(pos_chlist_single, neg_chlist_single, ADC_CHANNEL_MAX, ENABLE);
    } 
    else {
        chan = adc_get_channel_by_gpio(pin);
        ADC_Channel_Config(chan, ADC_CHAN_GND, ENABLE);
        ADC_Mic_Init(NULL);
    }
    
    adc_fifo_cfg.fifoThreshold = ADC_FIFO_THRESHOLD_1;
    adc_fifo_cfg.dmaEn = ENABLE;
    ADC_FIFO_Cfg(&adc_fifo_cfg);
    //ADC_Tsen_Init(ADC_TSEN_MOD_INTERNAL_DIODE);
    ADC_SET_TSVBE_LOW();
}

static void adc_dma_start(hosal_dma_chan_t chan)
{
    DMA_IntMask(chan, DMA_INT_ALL, MASK);
    DMA_IntMask(chan, DMA_INT_TCOMPLETED, UNMASK);
    DMA_IntMask(chan, DMA_INT_ERR, UNMASK);

    ADC_Start();
    DMA_Enable();
    DMA_Channel_Enable(chan);
}

#ifdef CONF_ADC_ENABLE_TSEN
static int adc_tsen_offset_get(uint16_t *tsen_offset)
{
    if (NULL == tsen_offset) {
        log_error("parm is error\r\n");
        return -1;
    }

    ADC_CFG_Type adcCfg = {
        .v18Sel = ADC_V18_SEL_1P82V,                /*!< ADC 1.8V select */
        .v11Sel = ADC_V11_SEL_1P1V,                 /*!< ADC 1.1V select */
        .clkDiv = ADC_CLK_DIV_32,                   /*!< Clock divider */
        .gain1 = ADC_PGA_GAIN_1,                 /*!< PGA gain 1 */
        .gain2 = ADC_PGA_GAIN_1,                 /*!< PGA gain 2 */
        .chopMode = ADC_CHOP_MOD_AZ_PGA_ON,           /*!< ADC chop mode select */
        .biasSel = ADC_BIAS_SEL_MAIN_BANDGAP,       /*!< ADC current form main bandgap or aon bandgap */
        .vcm = ADC_PGA_VCM_1V,                      /*!< ADC VCM value */
        .vref = ADC_VREF_2V,                      /*!< ADC voltage reference */
        .inputMode = ADC_INPUT_SINGLE_END,          /*!< ADC input signal type */
        .resWidth = ADC_DATA_WIDTH_16_WITH_256_AVERAGE,              /*!< ADC resolution and oversample rate */
        .offsetCalibEn = 0,                         /*!< Offset calibration enable */
        .offsetCalibVal = 0,                        /*!< Offset calibration value */
    };

    ADC_FIFO_Cfg_Type adcFifoCfg = {
        .fifoThreshold = ADC_FIFO_THRESHOLD_1,
        .dmaEn = DISABLE,
    };

    GLB_Set_ADC_CLK(ENABLE,GLB_ADC_CLK_96M, 7);

    ADC_Disable();
    ADC_Enable();

    ADC_Reset();

    ADC_Init(&adcCfg);
    ADC_Channel_Config(ADC_CHAN_TSEN_P, ADC_CHAN_GND, 0);
    ADC_Tsen_Init(ADC_TSEN_MOD_INTERNAL_DIODE);

    ADC_FIFO_Cfg(&adcFifoCfg);

    if (ADC_Trim_TSEN(tsen_offset) == ERROR) {
        log_error("read efuse data failed\r\n");
        return -1;
    } else {
        log_info("offset = %d\r\n", *tsen_offset);
    }

    return 0;
}
#endif

static int adc_parse_data(uint32_t *parr, int data_size, int channel)
{
    int i;
    int32_t data;

    for (i = 0; i < data_size; i++) {
        if (parr[i] >> 21 == channel) {
            data = parr[i] & 0xFFFF;

#ifdef CONF_ADC_ENABLE_TSEN
            data = (data * 2000) >> 16;
#else
            data = (data * 3200) >> 16;
#endif

            return data;
        }
    }   
    log_error("error!\r\n");
    return -1;
}

int hosal_adc_init(hosal_adc_dev_t *adc)
{
    int res = -1;
    int freq;
    GLB_GPIO_Type pin;
    hosal_adc_ctx_t *pstctx;

    if (NULL == adc) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    freq = adc->config.sampling_freq;
    pin = (GLB_GPIO_Type)adc->config.pin;

    /* check adc pin*/
    res = adc_check_gpio_valid(pin);
    if (res) {
        blog_error("pin is error!\r\n");
        return -1;
    }
    
    pstctx = (hosal_adc_ctx_t *)pvPortMalloc(sizeof(hosal_adc_ctx_t));
    if (NULL == pstctx) {
        blog_error("not have enough memory!\r\n");
        return -1;
    }

    memset(pstctx, 0, sizeof(hosal_adc_ctx_t));
    adc->priv = pstctx;

    if (adc->config.mode == HOSAL_ADC_ONE_SHOT) {
        if (freq < 27 || freq > 340) {
            blog_error("illegal freq. for mode0, freq 27HZ ~ 340HZ \r\n");
            return -1;
        }
#ifdef CONF_ADC_ENABLE_TSEN
        uint16_t offset;
        adc_tsen_offset_get(&offset);
        pstctx->tsen_offset = offset;
        ADC_Stop();
#endif
        /* init freq */
        adc_freq_init(adc->config.mode, freq);
        adc_init(adc);
        adc_dma_init(adc, ADC_CHANNEL_MAX);
        adc_dma_start(adc->dma_chan);
    } else {
        log_error("not support continue mode!\r\n");
        return -1;
    }

    pgdevice = adc;
    
    return 0;
}

int hosal_adc_add_channel(hosal_adc_dev_t *adc, uint32_t channel)
{
    hosal_adc_ctx_t *pstctx = (hosal_adc_ctx_t *)adc->priv;

    if (NULL == adc) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    if (channel > 11) {
        blog_error("channel is error!");
        return -1;
    }
    pstctx->chan_init_table |= 1 << channel;

    return 0;
}

int hosal_adc_remove_channel(hosal_adc_dev_t *adc, uint32_t channel)
{
    hosal_adc_ctx_t *pstctx = (hosal_adc_ctx_t *)adc->priv;

    if (NULL == adc) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    if (channel > 11) {
        blog_error("channel is error!\r\n");
        return -1;
    }

    pstctx->chan_init_table &= ~(1 << channel);

    return 0;
}

hosal_adc_dev_t *hosal_adc_device_get(void)
{
    if (NULL == pgdevice) {
        blog_error("please init adc first!\r\n");
        return NULL;
    }

    return pgdevice;
}

int hosal_adc_value_get(hosal_adc_dev_t *adc, uint32_t channel, uint32_t timeout)
{
    int val = -1;
    hosal_adc_ctx_t *pstctx = (hosal_adc_ctx_t *)adc->priv;
 
    if (NULL == adc) {
        blog_error("parameter is error!\r\n");
        return -1;
    }
    
    if (channel > 11) {
        blog_error("channel is error!\r\n");
        return -1;
    }
    
    if (((1 << channel) & pstctx->chan_init_table) == 0) {
        blog_error("channel = %d  not init as adc \r\n", channel);
        return -1;
    }
    
    if (pstctx->channel_data == NULL) {
        blog_error("adc sampling not finish. \r\n");
        return -1;
    }

    while ((val = adc_parse_data(pstctx->channel_data, ADC_CHANNEL_MAX, channel)) == -1) {
        if (timeout-- == 0) {
            return -1;
        }
        vTaskDelay(1);
    }
    
    return val;
}

#ifdef CONF_ADC_ENABLE_TSEN
int hosal_adc_tsen_value_get(hosal_adc_dev_t *adc)
{
    hosal_adc_ctx_t *pstctx = (hosal_adc_ctx_t *)adc->priv;
 
    return pstctx->tsen_data;
}
#endif

int hosal_adc_sample_cb_reg(hosal_adc_dev_t *adc, hosal_adc_cb_t cb)
{
    log_error("not support now!\r\n");
    return -1;
}

int hosal_adc_start(hosal_adc_dev_t *adc, void *data, uint32_t size)
{
    log_error("not support now!\r\n");
    return -1;
}

int hosal_adc_stop(hosal_adc_dev_t *adc)
{
   return 0; 
}

int hosal_adc_finalize(hosal_adc_dev_t *adc)
{
    hosal_adc_ctx_t *pstctx = (hosal_adc_ctx_t *)adc->priv;

    if (NULL == adc) {
        log_error("parm error!\r\n");
        return -1;
    }
    
    vPortFree(pstctx->llibuf);
    vPortFree(pstctx->adc_lli);
    vPortFree(pstctx);
    ADC_Stop();
    hosal_dma_chan_stop(adc->dma_chan);
    hosal_dma_chan_release(adc->dma_chan);

    return 0;
}


