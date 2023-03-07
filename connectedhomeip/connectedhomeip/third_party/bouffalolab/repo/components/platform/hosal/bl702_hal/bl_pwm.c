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
#include <stdint.h>
#include <bl702_pwm.h>
#include <bl702_glb.h>

#include "bl_pwm.h"
#include "bl_irq.h"

#include <blog.h>

static void gpio_init(uint8_t pin)
{
	GLB_GPIO_Cfg_Type cfg;

	cfg.drive = 0;
	cfg.smtCtrl = 1;
	cfg.gpioMode = GPIO_MODE_OUTPUT;
	cfg.pullType = GPIO_PULL_DOWN;
    cfg.gpioPin = pin;
    cfg.gpioFun = 8;

    GLB_GPIO_Init(&cfg);
}

static int32_t pwm_init(uint8_t id, uint32_t freq)
{
    PWM_CH_CFG_Type pwmCfg = {
        .ch = PWM_CH0,
        .clk = PWM_CLK_BCLK,        //PWM_CLK_BCLK PWM_CLK_XCLK
        .stopMode = PWM_STOP_ABRUPT,//PWM_STOP_ABRUPT:default PWM_STOP_GRACEFUL:no change 
        .pol = PWM_POL_NORMAL,      //first low
        .clkDiv = 0,                //32/1 = 32M
        .period = 100,
        .threshold1 = 0,
        .threshold2 = 0,
        .intPulseCnt = 0,
    };

    /* 2k ~ 800K */
    if ((freq < 2000) || (freq > 800000)) {
        return -1;
    }

    pwmCfg.period = BL_PWM_CLK/freq;
    pwmCfg.ch = id;

    PWM_Channel_Disable(id);
    PWM_Channel_Init(&pwmCfg);

    return 0;
}

int32_t bl_pwm_init(uint8_t id, uint8_t pin, uint32_t freq)
{
    if (GLB_GPIO_PIN_MAX <= pin) {
        return -1;
    }

    /* 2k ~ 800K */
    if ((freq < 2000) || (freq > 800000)) {
        blog_error("arg error. bl_pwm_init freq = %ld\r\n", freq);
        return -1;
    }

    /* init gpio */
    gpio_init(pin);
    pwm_init(id, freq);

    return 0;
}

int32_t bl_pwm_start(uint8_t id)
{
    PWM_Channel_Enable(id);
    return 0;
}

int32_t bl_pwm_stop(uint8_t id)
{
    PWM_Channel_Disable(id);
    return 0;
}

int32_t bl_pwm_set_freq(uint8_t id, uint32_t freq)
{
    PWM_Channel_Disable(id);

    uint16_t period = BL_PWM_CLK/freq;
    uint16_t threshold1 = 0;
    uint16_t threshold2 = 0;

    PWM_Channel_Update(id, period, threshold1, threshold2);
    PWM_Channel_Enable(id);

    return 0;
}

int32_t bl_pwm_set_duty(uint8_t id, float duty)
{
    uint16_t period;
    uint16_t threshold1;
    uint16_t threshold2;

    PWM_Channel_Get(id, &period, &threshold1, &threshold2);

    threshold1 = 0;
    threshold2 = (uint16_t)(period/100 * duty);

    PWM_Channel_Set_Threshold1(id, threshold1);
    PWM_Channel_Set_Threshold2(id, threshold2);
    return 0;
}

int32_t bl_pwm_get_duty(uint8_t id, float *p_duty)
{
    uint16_t period;
    uint16_t threshold1;
    uint16_t threshold2;

    if (NULL == p_duty) {
        return -1;
    }
    PWM_Channel_Get(id, &period, &threshold1, &threshold2);

    *p_duty = (float)threshold2 * 100 / period;

    return 0;
}


