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
#include <bl_gpio.h>
#include <bl602_glb.h>
#include <bl602_gpio.h>
#include <hosal_gpio.h>
#include <bl602_common.h>
#include <blog.h>
#include <bl602.h>
#include <bl_irq.h>

#define GPIP_INT_STATE_OFFSET    (0x1a8)
static hosal_gpio_ctx_t *gpio_head = NULL;

int hosal_gpio_init(hosal_gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    switch (gpio->config) {
    case INPUT_PULL_DOWN:
        bl_gpio_enable_input(gpio->port, 0, 1);
        break;
    case INPUT_PULL_UP:
        bl_gpio_enable_input(gpio->port, 1, 0);
        break;
    case INPUT_HIGH_IMPEDANCE:
        bl_gpio_enable_input(gpio->port, 0, 0);
        break;
    case OUTPUT_PUSH_PULL:
        bl_gpio_enable_output(gpio->port, 1, 0);
        break;
    case OUTPUT_OPEN_DRAIN_NO_PULL:
        bl_gpio_enable_output(gpio->port, 0, 0);
        break;
    case OUTPUT_OPEN_DRAIN_PULL_UP:
        bl_gpio_enable_output(gpio->port, 0, 0);
        break;
    default:
        return -1;
    }
    return 0;
}

int hosal_gpio_output_set(hosal_gpio_dev_t *gpio, uint8_t value)
{
    if (gpio == NULL) {
        return -1;
    }
    GLB_GPIO_Write((GLB_GPIO_Type)gpio->port, value ? 1 : 0);
    return 0;
}

int hosal_gpio_input_get(hosal_gpio_dev_t *gpio, uint8_t  *value)
{
    if (gpio == NULL) {
        return -1;
    }

    *value = GLB_GPIO_Read((GLB_GPIO_Type)gpio->port);
    return 0;
}

static int check_gpio_is_interrupt(int gpioPin)
{
    int bitcount = 0;
    int reg_val = 0;

    bitcount = 1 << gpioPin;
    reg_val = *(int32_t *)(GLB_BASE + GPIP_INT_STATE_OFFSET);

    if ((bitcount & reg_val) == bitcount) {
        return 0;
    }
    return -1;
}

static int exec_gpio_handler(hosal_gpio_ctx_t *node)
{
    bl_gpio_intmask(node->pin, 1);

    if (node->handle) {
        node->handle(node->arg);
        bl_gpio_intmask(node->pin, 0);
        return 0;
    }
    bl_gpio_intmask(node->pin, 0);

    return -1;
}

static void gpio_interrupt_entry(hosal_gpio_ctx_t *pstnode)
{
    int ret;
    hosal_gpio_ctx_t *node  = pstnode;

    while (node) {
        ret = check_gpio_is_interrupt(node->pin);
        if (ret == 0) {
            exec_gpio_handler(node);
        }

        node = node->next;
    }
    return;
}

int hosal_gpio_irq_set(hosal_gpio_dev_t *gpio, hosal_gpio_irq_trigger_t trigger_type, hosal_gpio_irq_handler_t handler, void *arg)
{
    if (NULL == gpio || gpio->port > GLB_GPIO_PIN_MAX || trigger_type > 3 || trigger_type < 0) {
        printf("hosal irq register paraments is not correct! \r\n");
    }
    hosal_gpio_ctx_t *node = NULL;
    hosal_gpio_ctx_t *node_f = NULL;
    node = (hosal_gpio_ctx_t *)pvPortMalloc(sizeof(hosal_gpio_ctx_t));
    if (!node) {
        printf("hosal irq ctx malloc failed \r\n");
        return -1;
    }
    node->handle     = handler;
    node->arg        = arg;
    node->pin        = gpio->port;
    node->intCtrlMod = GLB_GPIO_INT_CONTROL_ASYNC;
    node->intTrigMod = trigger_type;
    if (!gpio_head) {
        gpio_head = node;
        node->next = NULL;
    } else {
        for (node_f = gpio_head; node_f != NULL; node_f = node_f->next) {
            if (node_f->pin == node->pin) {
#if 0
                memcpy(node_f, node, sizeof(hosal_gpio_ctx_t));// will crash beacause next field is NULL
#endif
                node_f->handle = node->handle;
                node_f->arg    = node->arg;
                node_f->intCtrlMod = node->intCtrlMod;
                node_f->intTrigMod = node->intTrigMod;
                vPortFree(node);
                break;
            }
        }
        if (node_f == NULL) {
            node->next = gpio_head;
            gpio_head = node;
        }
    }

    bl_gpio_intmask(gpio->port, 1);
    bl_set_gpio_intmod(gpio->port, GLB_GPIO_INT_CONTROL_ASYNC, trigger_type);
    bl_irq_register_with_ctx(GPIO_INT0_IRQn, gpio_interrupt_entry, gpio_head);
    bl_gpio_intmask(gpio->port, 0);
    bl_irq_enable(GPIO_INT0_IRQn);
    return 0;
}

int hosal_gpio_irq_mask(hosal_gpio_dev_t *gpio, uint8_t mask)
{
    if (gpio  == NULL) {
        return -1;
    }
    bl_gpio_intmask(gpio->port, mask ? 1 : 0);

    return 0;
}

int hosal_gpio_finalize(hosal_gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }
    hosal_gpio_ctx_t *node = NULL;
    for (node = gpio_head; node != NULL; node = node->next) {
        if (node->pin == gpio->port) {
            vPortFree(node);
        }
    }
    
    bl_gpio_intmask(gpio->port, 1);
    bl_gpio_int_clear(gpio->port, 0);
    return 0;    
}



