/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <hosal_gpio.h>
#include <hosal_dma.h>
#include <blog.h>

static hosal_gpio_dev_t gp1;

void gpio_set_led_on(void)
{
    gp1.port = 5;
    gp1.config = OUTPUT_OPEN_DRAIN_NO_PULL;
    hosal_gpio_init(&gp1);
    hosal_gpio_output_set(&gp1, 1);
}

void gpio_set_led_off(void)
{
    hosal_gpio_output_set(&gp1, 0);
}

static hosal_gpio_dev_t key1;
static hosal_gpio_dev_t key2;
static uint8_t arg1 = 2, arg2 = 3;

void key1_irq(void *arg)
{
    uint8_t val1 = *(uint8_t *)arg;
    if (val1 == arg1) {
        blog_info("[HOSAL][GPIO] irq1 ok\r\n");
    }
}

void key2_irq(void *arg)
{
    uint8_t val2 = *(uint8_t *)arg;
    if (val2 == arg2) {
        blog_info("[HOSAL][GPIO] irq2 ok\r\n");
    }
}

void gpio_irq_test(void)
{

    key1.port = 3;
    key1.config = INPUT_PULL_UP;
    hosal_gpio_init(&key1);

    key2.port = 4;
    key2.config = INPUT_PULL_UP;
    hosal_gpio_init(&key2);

    hosal_gpio_irq_set(&key1, HOSAL_IRQ_TRIG_NEG_PULSE, key1_irq, &arg1);
    hosal_gpio_irq_set(&key2, HOSAL_IRQ_TRIG_NEG_PULSE, key2_irq, &arg2);
}

void key1_irq_mask_test(uint8_t mask)
{
    hosal_gpio_irq_mask(&key1, mask);
}

void gpio_finalize_test(void)
{
    hosal_gpio_finalize(&gp1);
    hosal_gpio_finalize(&key1);
    hosal_gpio_finalize(&key2);
}

