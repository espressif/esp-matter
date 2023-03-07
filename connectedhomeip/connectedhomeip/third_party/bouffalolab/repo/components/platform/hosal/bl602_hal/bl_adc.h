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
#ifndef __BL_ADC_H__
#define __BL_ADC_H__

#define ADC_DMA_CHANNEL    1
#define ADC_CHANNEL_MAX    12

typedef void (*bl_adc_cb_t)(int mode, uint32_t *data_ptr, uint32_t data_size);
typedef struct adc_ctx {
    int mode;
    uint32_t *channel_data;
    void *adc_lli;
    int lli_flag;
    uint32_t chan_init_table;
    uint32_t data_size;
    bl_adc_cb_t cb;
}adc_ctx_t;

int test_adc_init(void);
int test_adc_get(int16_t *tmp);
int test_adc_test(void);
int bl_tsen_adc_get(int16_t *temp, uint8_t log_flag);

//int bl_adc_clock_init(int sampling_ms);
int bl_adc_init(int mode, int gpio_num);
int bl_adc_dma_init(int mode, uint32_t data_num);
int bl_adc_start(void);
int bl_adc_gpio_init(int gpio_num);
int bl_adc_get_channel_by_gpio(int gpio_num);
int bl_adc_freq_init(int mode, uint32_t freq);
int32_t bl_adc_parse_data(uint32_t *parr, int data_size, int channel, int raw_flag);

#endif
