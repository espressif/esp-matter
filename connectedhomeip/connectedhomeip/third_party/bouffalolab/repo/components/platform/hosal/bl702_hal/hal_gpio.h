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
#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

typedef enum {
    GPIO_INT_TRIG_NEG_PULSE,            /*!< GPIO negedge pulse trigger interrupt */
    GPIO_INT_TRIG_POS_PULSE,            /*!< GPIO posedge pulse trigger interrupt */
    GPIO_INT_TRIG_NEG_LEVEL,            /*!< GPIO negedge level trigger interrupt (32k 3T) */
    GPIO_INT_TRIG_POS_LEVEL,            /*!< GPIO posedge level trigger interrupt (32k 3T) */
} hal_gpio_int_trig_type;

typedef enum {
    GPIO_INT_CONTROL_SYNC,              /*!< GPIO interrupt sync mode */
    GPIO_INT_CONTROL_ASYNC,             /*!< GPIO interrupt async mode */
} hal_gpio_int_ctl_type;

int hal_gpio_register_handler(void *func, int gpioPin, int intCtrlMod, int intTrgMod, void *arg);
int hal_gpio_init_from_dts(uint32_t fdt, uint32_t dtb_offset);
int hal_gpio_led_on(void);
int hal_gpio_led_off(void);

#endif
