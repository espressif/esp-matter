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
#include <bl602_timer.h>
#include <bl602_common.h>
#include <bl602.h>
#include <bl_irq.h>
#include <hosal_timer.h>
#include <blog.h>

static void timer_process(void *ctx)
{
	hosal_timer_dev_t *tim= (hosal_timer_dev_t *)ctx;
    void *arg;
	hosal_timer_cb_t handle;
    
    handle = tim->config.cb;
    arg = tim->config.arg;

    if (tim->port == 0) {
        TIMER_IntMask(TIMER_CH0, TIMER_INT_ALL, MASK);
        TIMER_ClearIntStatus(TIMER_CH0, TIMER_COMP_ID_0);
        if (tim->config.reload_mode == TIMER_RELOAD_ONCE) {
            TIMER_Disable(TIMER_CH0);
        }
    } else if (tim->port == 1) {
        TIMER_IntMask(TIMER_CH1, TIMER_INT_ALL, MASK);
        TIMER_ClearIntStatus(TIMER_CH1, TIMER_COMP_ID_0);
        if (tim->config.reload_mode == TIMER_RELOAD_ONCE) {
            TIMER_Disable(TIMER_CH1);
        }
    }

    if (handle) {
        handle(arg);
    }

    if (tim->port == 0) {
        TIMER_IntMask(TIMER_CH0, TIMER_INT_COMP_0, UNMASK);
    } else if (tim->port == 1) {
        TIMER_IntMask(TIMER_CH1, TIMER_INT_COMP_0, UNMASK);
    }
}

int hosal_timer_init(hosal_timer_dev_t *tim)
{
	TIMER_CFG_Type timer_cfg =
    {
        TIMER_CH1,
        TIMER_CLKSRC_XTAL,
        TIMER_PRELOAD_TRIG_COMP0,
        TIMER_COUNT_PRELOAD,
        39,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0,
    };
    if (tim->port == 0) {
        timer_cfg.timerCh = 0;
    } else if (tim->port == 1) {
        timer_cfg.timerCh = 1;
    } else {
        blog_error("timer channel %d not exists\r\n", tim->port);
		return -1;
    }
	timer_cfg.matchVal0 = tim->config.period;
    TIMER_IntMask(timer_cfg.timerCh, TIMER_INT_ALL, MASK);
    TIMER_Disable(timer_cfg.timerCh);
    TIMER_Init(&timer_cfg);

    /* Clear interrupt status*/
    TIMER_ClearIntStatus(timer_cfg.timerCh, TIMER_COMP_ID_0);
    TIMER_ClearIntStatus(timer_cfg.timerCh, TIMER_COMP_ID_1);
    TIMER_ClearIntStatus(timer_cfg.timerCh, TIMER_COMP_ID_2);

    /* Enable timer match interrupt */
    TIMER_IntMask(timer_cfg.timerCh, TIMER_INT_COMP_0, UNMASK);
    TIMER_IntMask(timer_cfg.timerCh, TIMER_INT_COMP_1, MASK);
    TIMER_IntMask(timer_cfg.timerCh, TIMER_INT_COMP_2, MASK);
   
    if (tim->port == 0) {
		bl_irq_register_with_ctx(TIMER_CH0_IRQn, timer_process, tim);
    } else {
		bl_irq_register_with_ctx(TIMER_CH1_IRQn, timer_process, tim);
    }

    return 0;
}


int hosal_timer_start(hosal_timer_dev_t *tim)
{
    if (tim->port == 0) {
		bl_irq_enable(TIMER_CH0_IRQn);
		TIMER_Enable(TIMER_CH0);
    } else if (tim->port == 1) {
		bl_irq_enable(TIMER_CH1_IRQn);
		TIMER_Enable(TIMER_CH1);
    } else {
        blog_error("timer channel %d not exists\r\n", tim->port);
		return -1;
    }
    return 0;
}

void hosal_timer_stop(hosal_timer_dev_t *tim)
{
    if (tim->port == 0) {
		bl_irq_disable(TIMER_CH0_IRQn);
		TIMER_Disable(TIMER_CH0);
    } else if (tim->port == 1) {
		bl_irq_disable(TIMER_CH1_IRQn);
		TIMER_Disable(TIMER_CH1);
    } else {
        blog_info("timer channel %d not exists\r\n", tim->port);
		return;
    }
}

int hosal_timer_finalize(hosal_timer_dev_t *tim)
{
    if (tim->port == 0) {
		bl_irq_disable(TIMER_CH0_IRQn);
        bl_irq_unregister(TIMER_CH0_IRQn, timer_process);
        TIMER_IntMask(TIMER_CH0, TIMER_INT_ALL, MASK);
		TIMER_Disable(TIMER_CH0);
    } else if (tim->port == 1) {
		bl_irq_disable(TIMER_CH1_IRQn);
        bl_irq_unregister(TIMER_CH1_IRQn, timer_process);
        TIMER_IntMask(TIMER_CH1, TIMER_INT_ALL, MASK);
		TIMER_Disable(TIMER_CH1);
    } else {
        blog_error("timer channel %d not exists\r\n", tim->port);
		return -1;
    }
    return 0;
}
