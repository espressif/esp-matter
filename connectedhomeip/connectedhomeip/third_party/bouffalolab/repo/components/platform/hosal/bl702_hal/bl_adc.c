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
#include <bl702_glb.h>
#include <bl702_adc.h>
#include <bl702_dma.h>
#include <bl_adc.h>
#include <bl_irq.h>
#include <bl_efuse.h>


#define TSEN_SAMPLE_CNT            16

static int16_t tsen_refcode;


int bl_adc_tsen_init(void)
{
    ADC_CFG_Type adcCfg = {
        .v18Sel=ADC_V18_SEL_1P82V,                      /*!< ADC 1.8V select */
        .v11Sel=ADC_V11_SEL_1P1V,                       /*!< ADC 1.1V select */
        .clkDiv=ADC_CLK_DIV_16,                         /*!< Clock divider */
        .gain1=ADC_PGA_GAIN_1,                          /*!< PGA gain 1 */
        .gain2=ADC_PGA_GAIN_1,                          /*!< PGA gain 2 */
        .chopMode=ADC_CHOP_MOD_AZ_PGA_ON,               /*!< ADC chop mode select */
        .biasSel=ADC_BIAS_SEL_MAIN_BANDGAP,             /*!< ADC current form main bandgap or aon bandgap */
        .vcm=ADC_PGA_VCM_1V,                            /*!< ADC VCM value */
        .vref=ADC_VREF_2P0V,                            /*!< ADC voltage reference */
        .inputMode=ADC_INPUT_SINGLE_END,                /*!< ADC input signal type */
        .resWidth=ADC_DATA_WIDTH_16_WITH_256_AVERAGE,   /*!< ADC resolution and oversample rate */
        .offsetCalibEn=0,                               /*!< Offset calibration enable */
        .offsetCalibVal=0,                              /*!< Offset calibration value */
    };
    
    ADC_FIFO_Cfg_Type adcFifoCfg = {
        .fifoThreshold = ADC_FIFO_THRESHOLD_1,
        .dmaEn = DISABLE,
    };
    
    // Set ADC clock to 32M
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 0);
    
    // Initialize TSEN
    ADC_Disable();
    ADC_Reset();
    ADC_Enable();
    ADC_Init(&adcCfg);
    ADC_Tsen_Init(ADC_TSEN_MOD_INTERNAL_DIODE);
    ADC_Channel_Config(ADC_CHAN_TSEN_P, ADC_CHAN_GND, 0);  // single-ended mode
    ADC_FIFO_Cfg(&adcFifoCfg);
    
    // Skip the first bad sample
    ADC_Start();
    while(ADC_Get_FIFO_Count() == 0);
    ADC_Read_FIFO();
    
    // Get TSEN ref code
    return bl_efuse_read_tsen_refcode(&tsen_refcode);
}

int16_t bl_adc_tsen_get_val(void)
{
    int i;
    int32_t sum;
    int16_t v0, v1, vdelta, temperature;
    
    // Get V0
    ADC_SET_TSVBE_LOW();
    sum = 0;
    for(i=0; i<TSEN_SAMPLE_CNT; i++){
        ADC_Start();
        while(ADC_Get_FIFO_Count() == 0);
        sum += (int16_t)ADC_Read_FIFO();
    }
    v0 = sum / TSEN_SAMPLE_CNT;
    
    // Get V1
    ADC_SET_TSVBE_HIGH();
    sum = 0;
    for(i=0; i<TSEN_SAMPLE_CNT; i++){
        ADC_Start();
        while(ADC_Get_FIFO_Count() == 0);
        sum += (int16_t)ADC_Read_FIFO();
    }
    v1 = sum / TSEN_SAMPLE_CNT;
    
    // Get Vdelta
    vdelta = v0 - v1;
    
    // Calculate temperature
    temperature = (vdelta - tsen_refcode) / 7.753;
    
    return temperature;
}


static uint8_t tsen_dma_ch = 0xFF;
static bl_adc_tsen_callback_t tsen_event;
static int16_t tsen_buf[TSEN_SAMPLE_CNT];
static volatile int tsen_fsm = 0;

static void tsen_dma_callback(void)
{
    int i;
    int32_t sum;
    static int16_t v0;
    int16_t v1, vdelta, temperature;
    
    BL_WR_REG(DMA_BASE, DMA_INTTCCLEAR, 1U << tsen_dma_ch);
    
    switch(tsen_fsm)
    {
        case 1:
            ADC_Stop();
            ADC_FIFO_Clear();
            
            // Get V0 (skip the first bad sample)
            sum = 0;
            for(i=1; i<TSEN_SAMPLE_CNT; i++){
                sum += tsen_buf[i];
            }
            v0 = sum / (TSEN_SAMPLE_CNT - 1);
            
            DMA_Channel_Update_DstMemcfg(tsen_dma_ch, (uint32_t)tsen_buf, TSEN_SAMPLE_CNT);
            DMA_Channel_Enable(tsen_dma_ch);
            
            ADC_SET_TSVBE_HIGH();
            ADC_Start();
            
            tsen_fsm = 2;
        break;
        
        case 2:
            ADC_Stop();
            ADC_FIFO_Clear();
            
            // Get V1 (skip the first bad sample)
            sum = 0;
            for(i=1; i<TSEN_SAMPLE_CNT; i++){
                sum += tsen_buf[i];
            }
            v1 = sum / (TSEN_SAMPLE_CNT - 1);
            
            // Get Vdelta
            vdelta = v0 - v1;
            
            // Calculate temperature
            temperature = (vdelta - tsen_refcode) / 7.753;
            
            if(tsen_event != NULL){
                tsen_event(temperature);
            }
            
            tsen_fsm = 0;
        break;
        
        default:
        break;
    }
}

static void tsen_adc_init(void)
{
    ADC_CFG_Type adcCfg = {
        .v18Sel=ADC_V18_SEL_1P82V,                      /*!< ADC 1.8V select */
        .v11Sel=ADC_V11_SEL_1P1V,                       /*!< ADC 1.1V select */
        .clkDiv=ADC_CLK_DIV_16,                         /*!< Clock divider */
        .gain1=ADC_PGA_GAIN_1,                          /*!< PGA gain 1 */
        .gain2=ADC_PGA_GAIN_1,                          /*!< PGA gain 2 */
        .chopMode=ADC_CHOP_MOD_AZ_PGA_ON,               /*!< ADC chop mode select */
        .biasSel=ADC_BIAS_SEL_MAIN_BANDGAP,             /*!< ADC current form main bandgap or aon bandgap */
        .vcm=ADC_PGA_VCM_1V,                            /*!< ADC VCM value */
        .vref=ADC_VREF_2P0V,                            /*!< ADC voltage reference */
        .inputMode=ADC_INPUT_SINGLE_END,                /*!< ADC input signal type */
        .resWidth=ADC_DATA_WIDTH_16_WITH_256_AVERAGE,   /*!< ADC resolution and oversample rate */
        .offsetCalibEn=0,                               /*!< Offset calibration enable */
        .offsetCalibVal=0,                              /*!< Offset calibration value */
    };
    
    ADC_FIFO_Cfg_Type adcFifoCfg = {
        .fifoThreshold = ADC_FIFO_THRESHOLD_1,
        .dmaEn = ENABLE,
    };
    
    // Set ADC clock to 32M
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 0);
    
    // Initialize TSEN
    ADC_Disable();
    ADC_Reset();
    ADC_Enable();
    ADC_Init(&adcCfg);
    ADC_Tsen_Init(ADC_TSEN_MOD_INTERNAL_DIODE);
    ADC_Channel_Config(ADC_CHAN_TSEN_P, ADC_CHAN_GND, 1);  // continuous mode
    ADC_FIFO_Cfg(&adcFifoCfg);
}

static void tsen_dma_init(bl_adc_tsen_cfg_t *cfg)
{
    DMA_Channel_Cfg_Type dmaChCfg = {
        0x40002004,                /* Source address of DMA transfer */
        0,                         /* Destination address of DMA transfer */
        0,                         /* Transfer length, 0~4095, this is burst count */
        DMA_TRNS_P2M,              /* Transfer dir control. 0: Memory to Memory, 1: Memory to peripheral, 2: Peripheral to memory */
        cfg->tsen_dma_ch,          /* Channel select 0-7 */
        DMA_TRNS_WIDTH_16BITS,     /* Transfer width. 0: 8  bits, 1: 16  bits, 2: 32  bits */
        DMA_TRNS_WIDTH_16BITS,     /* Transfer width. 0: 8  bits, 1: 16  bits, 2: 32  bits */
        DMA_BURST_SIZE_1,          /* Number of data items for burst transaction length. Each item width is as same as tansfer width. 0: 1 item, 1: 4 items, 2: 8 items, 3: 16 items */
        DMA_BURST_SIZE_1,          /* Number of data items for burst transaction length. Each item width is as same as tansfer width. 0: 1 item, 1: 4 items, 2: 8 items, 3: 16 items */
        DISABLE,
        DISABLE,
        0,
        DMA_PINC_DISABLE,          /* Source address increment. 0: No change, 1: Increment */
        DMA_MINC_ENABLE,           /* Destination address increment. 0: No change, 1: Increment */
        DMA_REQ_GPADC0,            /* Source peripheral select */
        DMA_REQ_NONE,              /* Destination peripheral select */
    };
    
    bl_irq_register(DMA_ALL_IRQn, tsen_dma_callback);
    bl_irq_enable(DMA_ALL_IRQn);
    
    DMA_Enable();
    DMA_IntMask(cfg->tsen_dma_ch, DMA_INT_ALL, MASK);
    DMA_IntMask(cfg->tsen_dma_ch, DMA_INT_TCOMPLETED, UNMASK);
    DMA_Channel_Init(&dmaChCfg);
}

int bl_adc_tsen_dma_init(bl_adc_tsen_cfg_t *cfg)
{
    if(cfg == NULL){
        return -1;
    }
    
    if(cfg->tsen_dma_ch > 7){
        return -1;
    }
    
    tsen_adc_init();
    tsen_dma_init(cfg);
    
    tsen_dma_ch = cfg->tsen_dma_ch;
    tsen_event = cfg->tsen_event;
    
    // Get TSEN ref code
    return bl_efuse_read_tsen_refcode(&tsen_refcode);
}

int bl_adc_tsen_dma_trigger(void)
{
    if(tsen_dma_ch == 0xFF){
        return -1;
    }
    
    if(tsen_fsm != 0){
        return -1;
    }
    
    DMA_Channel_Update_DstMemcfg(tsen_dma_ch, (uint32_t)tsen_buf, TSEN_SAMPLE_CNT);
    DMA_Channel_Enable(tsen_dma_ch);
    
    ADC_SET_TSVBE_LOW();
    ADC_Start();
    
    tsen_fsm = 1;
    
    return 0;
}

int bl_adc_tsen_dma_is_busy(void)
{
    return (tsen_fsm != 0);
}


static uint8_t adc_dma_ch = 0xFF;
static uint16_t pcm_frame_size;
static int16_t *pcm_frame_buf[2];
static bl_adc_voice_callback_t pcm_frame_event;
static int pcm_frame_idx;

static void voice_dma_callback(void)
{
    int idx = pcm_frame_idx;
    
    BL_WR_REG(DMA_BASE, DMA_INTTCCLEAR, 1U << adc_dma_ch);
    
    pcm_frame_idx = !pcm_frame_idx;
    DMA_Channel_Update_DstMemcfg(adc_dma_ch, (uint32_t)pcm_frame_buf[pcm_frame_idx], pcm_frame_size);
    DMA_Channel_Enable(adc_dma_ch);
    
    if(pcm_frame_event != NULL){
        pcm_frame_event(idx);
    }
}

static void voice_gpio_init(bl_adc_voice_cfg_t *cfg)
{
    GLB_GPIO_Cfg_Type gpioCfg;
    
    gpioCfg.gpioFun = 10;
    gpioCfg.gpioMode = GPIO_MODE_AF;
    gpioCfg.pullType = GPIO_PULL_NONE;
    gpioCfg.drive = 1;
    gpioCfg.smtCtrl = 1;
    
    gpioCfg.gpioPin = cfg->adc_pos_pin;
    GLB_GPIO_Init(&gpioCfg);
    
    gpioCfg.gpioPin = cfg->adc_neg_pin;
    GLB_GPIO_Init(&gpioCfg);
}

static void voice_adc_init(bl_adc_voice_cfg_t *cfg)
{
    // Sample Rate = 15.625kHz (32M / 16 / 128)
    ADC_CFG_Type adcCfg = {
        .v18Sel=ADC_V18_SEL_1P82V,                      /*!< ADC 1.8V select */
        .v11Sel=ADC_V11_SEL_1P1V,                       /*!< ADC 1.1V select */
        .clkDiv=ADC_CLK_DIV_16,                         /*!< Clock divider */
        .gain1=ADC_PGA_GAIN_8,                          /*!< PGA gain 1 */
        .gain2=ADC_PGA_GAIN_4,                          /*!< PGA gain 2 */
        .chopMode=ADC_CHOP_MOD_AZ_ON,                   /*!< ADC chop mode select */
        .biasSel=ADC_BIAS_SEL_MAIN_BANDGAP,             /*!< ADC current form main bandgap or aon bandgap */
        .vcm=ADC_PGA_VCM_1P4V,                          /*!< ADC VCM value */
        .vref=ADC_VREF_3P2V,                            /*!< ADC voltage reference */
        .inputMode=ADC_INPUT_DIFF,                      /*!< ADC input differential type */
        .resWidth=ADC_DATA_WIDTH_16_WITH_128_AVERAGE,   /*!< ADC resolution and oversample rate */
        .offsetCalibEn=0,                               /*!< Offset calibration enable */
        .offsetCalibVal=0,                              /*!< Offset calibration value */
    };
    
    ADC_FIFO_Cfg_Type adcFifoCfg = {
        .fifoThreshold = ADC_FIFO_THRESHOLD_1,
        .dmaEn = ENABLE,
    };
    
    uint8_t adcPins[] = {8, 15, 17, 11, 12, 14, 7, 9, 18, 19, 20, 21};
    ADC_Chan_Type adcPosCh, adcNegCh;
    
    // Get positive channel
    for(adcPosCh = ADC_CHAN0; adcPosCh <= ADC_CHAN11; adcPosCh++){
        if(adcPins[adcPosCh] == cfg->adc_pos_pin){
            break;
        }
    }
    
    // Get negative channel
    for(adcNegCh = ADC_CHAN0; adcNegCh <= ADC_CHAN11; adcNegCh++){
        if(adcPins[adcNegCh] == cfg->adc_neg_pin){
            break;
        }
    }
    
    // Set ADC clock to 32M
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 0);
    
    ADC_Disable();
    ADC_Reset();
    ADC_Enable();
    ADC_Init(&adcCfg);
    ADC_Tsen_Disable();
    ADC_Channel_Config(adcPosCh, adcNegCh, 1);
    ADC_FIFO_Cfg(&adcFifoCfg);
    
    // Set pga_vcmi_en = 1 for mic
    ADC_PGA_Config(1, 1);
}

static void voice_dma_init(bl_adc_voice_cfg_t *cfg)
{
    DMA_Channel_Cfg_Type dmaChCfg = {
        0x40002004,                /* Source address of DMA transfer */
        0,                         /* Destination address of DMA transfer */
        0,                         /* Transfer length, 0~4095, this is burst count */
        DMA_TRNS_P2M,              /* Transfer dir control. 0: Memory to Memory, 1: Memory to peripheral, 2: Peripheral to memory */
        cfg->adc_dma_ch,           /* Channel select 0-7 */
        DMA_TRNS_WIDTH_16BITS,     /* Transfer width. 0: 8  bits, 1: 16  bits, 2: 32  bits */
        DMA_TRNS_WIDTH_16BITS,     /* Transfer width. 0: 8  bits, 1: 16  bits, 2: 32  bits */
        DMA_BURST_SIZE_1,          /* Number of data items for burst transaction length. Each item width is as same as tansfer width. 0: 1 item, 1: 4 items, 2: 8 items, 3: 16 items */
        DMA_BURST_SIZE_1,          /* Number of data items for burst transaction length. Each item width is as same as tansfer width. 0: 1 item, 1: 4 items, 2: 8 items, 3: 16 items */
        DISABLE,
        DISABLE,
        0,
        DMA_PINC_DISABLE,          /* Source address increment. 0: No change, 1: Increment */
        DMA_MINC_ENABLE,           /* Destination address increment. 0: No change, 1: Increment */
        DMA_REQ_GPADC0,            /* Source peripheral select */
        DMA_REQ_NONE,              /* Destination peripheral select */
    };
    
    bl_irq_register(DMA_ALL_IRQn, voice_dma_callback);
    bl_irq_enable(DMA_ALL_IRQn);
    
    DMA_Enable();
    DMA_IntMask(cfg->adc_dma_ch, DMA_INT_ALL, MASK);
    DMA_IntMask(cfg->adc_dma_ch, DMA_INT_TCOMPLETED, UNMASK);
    DMA_Channel_Init(&dmaChCfg);
}

int bl_adc_voice_init(bl_adc_voice_cfg_t *cfg)
{
    if(cfg == NULL){
        return -1;
    }
    
    if(cfg->adc_pos_pin < 7 || cfg->adc_pos_pin == 10 || cfg->adc_pos_pin == 13 || cfg->adc_pos_pin == 16 || cfg->adc_pos_pin > 21){
        return -1;
    }
    
    if(cfg->adc_neg_pin < 7 || cfg->adc_neg_pin == 10 || cfg->adc_neg_pin == 13 || cfg->adc_neg_pin == 16 || cfg->adc_neg_pin > 21){
        return -1;
    }
    
    if(cfg->adc_pos_pin == cfg->adc_neg_pin){
        return -1;
    }
    
    if(cfg->adc_dma_ch > 7){
        return -1;
    }
    
    if(cfg->pcm_frame_size < 1 || cfg->pcm_frame_size > 4095){
        return -1;
    }
    
    if(cfg->pcm_frame_buf[0] == NULL){
        return -1;
    }
    
    if(cfg->pcm_frame_buf[1] == NULL){
        return -1;
    }
    
    voice_gpio_init(cfg);
    voice_adc_init(cfg);
    voice_dma_init(cfg);
    
    adc_dma_ch = cfg->adc_dma_ch;
    pcm_frame_size = cfg->pcm_frame_size;
    pcm_frame_buf[0] = cfg->pcm_frame_buf[0];
    pcm_frame_buf[1] = cfg->pcm_frame_buf[1];
    pcm_frame_event = cfg->pcm_frame_event;
    
    return 0;
}

int bl_adc_voice_start(void)
{
    if(adc_dma_ch == 0xFF){
        return -1;
    }
    
    pcm_frame_idx = 0;
    DMA_Channel_Update_DstMemcfg(adc_dma_ch, (uint32_t)pcm_frame_buf[pcm_frame_idx], pcm_frame_size);
    DMA_Channel_Enable(adc_dma_ch);
    
    ADC_Start();
    
    return 0;
}

int bl_adc_voice_stop(void)
{
    if(adc_dma_ch == 0xFF){
        return -1;
    }
    
    DMA_Channel_Disable(adc_dma_ch);
    
    ADC_Stop();
    ADC_FIFO_Clear();
    
    return 0;
}
