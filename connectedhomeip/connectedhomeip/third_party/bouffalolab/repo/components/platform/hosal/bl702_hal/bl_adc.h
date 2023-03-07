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

#include <stdint.h>

typedef void(*bl_adc_tsen_callback_t)(int16_t tsen_val);

typedef struct bl_adc_tsen_cfg
{
    uint8_t tsen_dma_ch;
    bl_adc_tsen_callback_t tsen_event;
}bl_adc_tsen_cfg_t;

typedef void(*bl_adc_voice_callback_t)(int buf_idx);

typedef struct bl_adc_voice_cfg
{
    uint8_t adc_pos_pin;
    uint8_t adc_neg_pin;
    uint8_t adc_dma_ch;
    uint16_t pcm_frame_size;
    int16_t *pcm_frame_buf[2];
    bl_adc_voice_callback_t pcm_frame_event;
}bl_adc_voice_cfg_t;

int bl_adc_tsen_init(void);
int16_t bl_adc_tsen_get_val(void);

int bl_adc_tsen_dma_init(bl_adc_tsen_cfg_t *cfg);
int bl_adc_tsen_dma_trigger(void);
int bl_adc_tsen_dma_is_busy(void);

int bl_adc_voice_init(bl_adc_voice_cfg_t *cfg);
int bl_adc_voice_start(void);
int bl_adc_voice_stop(void);

#endif
