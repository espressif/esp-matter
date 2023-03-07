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
#include <bl_sys.h>
#include <bl_gpio.h>
#include <hosal_gpio.h>

#include <main.h>

#define LED1_PIN    22
#define LED2_PIN    29
#define LED3_PIN    30 
#define LED4_PIN    31
#define KEY_PIN     12

static const uint32_t   led_pins[] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};
hosal_gpio_dev_t gpio_led = {
    .config = OUTPUT_OPEN_DRAIN_NO_PULL,
    .priv = NULL
};
hosal_gpio_dev_t gpio_key = {
    .port = KEY_PIN,
    .config = INPUT_PULL_DOWN,
    .priv = NULL
};


void (*keyPress_isrCb) (void *);

static void keyPress_isr(void * p) {
	if (keyPress_isrCb) {
		keyPress_isrCb(p);
	}
    //hosal_gpio_clear_irq(&gpio_key);
}

void app_gpioInit(void (*keyPress_handler)(void *)) 
{
    for (uint32_t i = 0; i < sizeof(led_pins) / sizeof(led_pins[0]); i ++) {
        gpio_led.port = led_pins[i];
        hosal_gpio_init(&gpio_led);
    }

    keyPress_isrCb = keyPress_handler;
    hosal_gpio_init(&gpio_key);
    hosal_gpio_irq_set(&gpio_key, HOSAL_IRQ_TRIG_POS_PULSE, keyPress_isr, NULL);
}

void app_gpioLedOn(uint32_t index) 
{
    if (index < sizeof(led_pins) / sizeof(led_pins[0])) {
        gpio_led.port = led_pins[index];
        hosal_gpio_output_set(&gpio_led, 1);
    }
}

void app_gpioLedOff(uint32_t index) 
{
    if (index < sizeof(led_pins) / sizeof(led_pins[0])) {
        gpio_led.port = led_pins[index];
        hosal_gpio_output_set(&gpio_led, 0);
    }
} 

uint32_t app_gpioGetKeyValue(void) 
{
    uint8_t val = 0;

    hosal_gpio_input_get(&gpio_key, &val);

    return val;
}
