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
#include <stdio.h>
#include <fdt.h>
#include <libfdt.h>
#include <blog.h>
#include <loopset.h>

#include <aos/kernel.h>
#include <aos/yloop.h>

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"

#include "blog.h"
#include "bl_gpio.h"
#include "bl_irq.h"
#include "hal_gpio.h"
#include "hal_button.h"
#include "hal_hbn.h"

/* scan time ms */
#define BUTTON_SCAN_TIME    20

#define BL_FDT32_TO_U8(addr, byte_offset)   ((uint8_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
#define BL_FDT32_TO_U16(addr, byte_offset)  ((uint16_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
#define BL_FDT32_TO_U32(addr, byte_offset)  ((uint32_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))

typedef struct _button_ctx_desc {
    void *private_ctx;
    TimerHandle_t psttimer_handler;

    int press_stage;
    int timer_count;

    int short_press_start_ms;
    int short_press_end_ms;
    int short_kevent;

    int long_press_start_ms;
    int long_press_end_ms;
    int long_kevent;

    int longlong_press_ms;
    int longlong_kevent;
    int dlong_entry_count;

    int debounce;
    int trig_level;
    int gpioPin;
} button_ctx_t;

static int accumulate_time(button_ctx_t *pstnode)
{
    int accu_time;

    if (NULL == pstnode) {
        blog_error("NULL pointer \r\n");
        return -1;
    }

    accu_time = pstnode->timer_count * BUTTON_SCAN_TIME + pstnode->debounce;

    return accu_time;
}

static int check_button_is_up(button_ctx_t *pstnode)
{
    int val;

    val = bl_gpio_input_get_value(pstnode->gpioPin);
    if (val != pstnode->trig_level) {
        return 0;
    }

    return 1;
}

static void button_int_umask(button_ctx_t *pstnode)
{
    bl_gpio_intmask(pstnode->gpioPin, 0);
}

static void clear_button_states(button_ctx_t *pstnode)
{
    pstnode->press_stage = 0;
    pstnode->timer_count = 0;
    pstnode->dlong_entry_count = 0;

    if (xTimerStop(pstnode->psttimer_handler, 0) != pdPASS) {
        blog_error("stop timer failed \r\n");
    }

    return;
}

static void button_process(xTimerHandle pxTimer)
{
    int ret;
    int accu_time;
    button_ctx_t *pstnode;

    pstnode = (button_ctx_t *)pvTimerGetTimerID(pxTimer);

    switch (pstnode->press_stage) {
        case 0:// for debounce
        {
            ret = check_button_is_up(pstnode);
            if (ret == 0) {
                blog_info("process not true pressed! \r\n");
                clear_button_states(pstnode);
                button_int_umask(pstnode);

                return;
            }

            if (xTimerChangePeriod(pstnode->psttimer_handler, 20 / portTICK_PERIOD_MS, 100) != pdPASS) {
                blog_error("change period failed \r\n");
                button_int_umask(pstnode);
                clear_button_states(pstnode);

                return;
            }
            pstnode->press_stage = 1;
        }
        break;

        case 1:// for short press
        {
            pstnode->timer_count++;
            accu_time = accumulate_time(pstnode);
            if (accu_time > pstnode->short_press_end_ms) {
                pstnode->press_stage = 2;

                break;
            }
            else if (accu_time >= pstnode->short_press_start_ms && accu_time < pstnode->short_press_end_ms) {
                ret = check_button_is_up(pstnode);
                if (ret == 0) {
                    blog_info("process short press \r\n");
                    aos_post_event(EV_KEY, pstnode->short_kevent, 0);
                    clear_button_states(pstnode);
                    button_int_umask(pstnode);

                    return;
                }
            }
            else if (accu_time < pstnode->short_press_start_ms) {
                ret = check_button_is_up(pstnode);
                if (ret == 0) {
                    blog_info("process not defined press time \r\n");
                    clear_button_states(pstnode);
                    button_int_umask(pstnode);
                    return;
                }
                else {
                    break;
                }
            }
            else {
                /* can not reach here */
            }
        }
        break;

        case 2:// for long press
        {
            pstnode->timer_count++;
            accu_time = accumulate_time(pstnode);
            if (accu_time > pstnode->long_press_end_ms) {
                pstnode->press_stage = 3;

                break;
            }
            else if (accu_time >= pstnode->long_press_start_ms && accu_time < pstnode->long_press_end_ms) {
                ret = check_button_is_up(pstnode);
                if (ret == 0) {
                    blog_info("process long press \r\n");
                    aos_post_event(EV_KEY, pstnode->long_kevent, 0);
                    clear_button_states(pstnode);
                    button_int_umask(pstnode);
                    return;
                }
            }
            else if (accu_time < pstnode->long_press_start_ms) {
                ret = check_button_is_up(pstnode);
                if (ret == 0) {
                    blog_info("process not defined press time \r\n");
                    clear_button_states(pstnode);
                    button_int_umask(pstnode);
                    return;
                }
                else {
                    break;
                }
            }
            else {
                 /* can not reach here */
            }
        }
        break;

        case 3:// for long long press
        {
            pstnode->timer_count++;
            accu_time = accumulate_time(pstnode);
            if (accu_time >= pstnode->longlong_press_ms && pstnode->dlong_entry_count == 0) {
                blog_info("process longlong press \r\n");
                aos_post_event(EV_KEY, pstnode->longlong_kevent, 0);
                pstnode->dlong_entry_count = 1;

                return ;
            }

            ret = check_button_is_up(pstnode);
            if (ret == 0) {
                clear_button_states(pstnode);
                button_int_umask(pstnode);

                return;
            }
        }
        break;
    }
}


static void button_callback(gpio_ctx_t *pstnode)
{
    int ret;
    BaseType_t xHigherPriorityTaskWoken;

    ret = xTimerStartFromISR(((button_ctx_t *)pstnode->arg)->psttimer_handler, &xHigherPriorityTaskWoken);
    if (ret != pdPASS) {
        blog_error("start timer failed \r\n");
        return ;
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    return;
}

static int hal_button_register_handler_with_dts(gpio_ctx_t *pstgpio)
{
    button_ctx_t *pstnode;
    char timer_name[20] = "buttontimer-";
    uint8_t level;

    if (NULL == pstgpio) {
        blog_error("pstgpio can not be NULL \r\n");
        return -1;
    }

    pstnode = pvPortMalloc(sizeof(button_ctx_t));

    pstnode->short_press_start_ms = ((button_ctx_t *)(pstgpio->arg))->short_press_start_ms;
    pstnode->short_press_end_ms = ((button_ctx_t *)(pstgpio->arg))->short_press_end_ms;
    pstnode->short_kevent = ((button_ctx_t *)(pstgpio->arg))->short_kevent;
    pstnode->long_press_start_ms = ((button_ctx_t *)(pstgpio->arg))->long_press_start_ms;
    pstnode->long_press_end_ms = ((button_ctx_t *)(pstgpio->arg))->long_press_end_ms;
    pstnode->long_kevent = ((button_ctx_t *)(pstgpio->arg))->long_kevent;
    pstnode->longlong_press_ms = ((button_ctx_t *)(pstgpio->arg))->longlong_press_ms;
    pstnode->longlong_kevent = ((button_ctx_t *)(pstgpio->arg))->longlong_kevent;
    pstnode->debounce = ((button_ctx_t *)(pstgpio->arg))->debounce;
    pstnode->trig_level = ((button_ctx_t *)(pstgpio->arg))->trig_level;
    pstnode->gpioPin = pstgpio->gpioPin;
    pstnode->press_stage = 0;
    pstnode->timer_count = 0;
    pstnode->dlong_entry_count = 0;


    sprintf(&timer_name[strlen(timer_name)], "%d", pstnode->gpioPin);
    pstnode->psttimer_handler = xTimerCreate(timer_name, pdMS_TO_TICKS(pstnode->debounce), pdTRUE, (void *)pstnode, button_process);

    if (!(pstnode->psttimer_handler)) {
        blog_error("create timer failed \r\n");
        return -1;
    }

    level = pstnode->trig_level ? 0 : 1;
    bl_gpio_enable_input(pstnode->gpioPin, level, !level);
    hal_gpio_register_handler(button_callback, pstnode->gpioPin, GPIO_INT_CONTROL_ASYNC, pstnode->trig_level ? GPIO_INT_TRIG_POS_LEVEL : GPIO_INT_TRIG_NEG_LEVEL, pstnode);

    return 0;
}

void fdt_button_module_init(const void *fdt, int button_offset)
{
#define GPIO_MODULE_MAX 5
    int offset1 = 0;
    int offset2 = 0;
    int offset3 = 0;
    int i;
    int lentmp = 0;
    char gpio_node[10] = "gpio";
    int countindex = 0;
    const char *result = 0;
    const uint32_t *addr_prop = 0;
    gpio_ctx_t stgpio;
    button_ctx_t stbutton;

    stgpio.arg = &stbutton;
    uint8_t *pinbuf;
    uint8_t pinbuf_size = 0;

    pinbuf = pvPortMalloc(GPIO_MODULE_MAX);
    if (!pinbuf) {
        blog_error("mem error.\r\n");
        return;
    }

    for (i = 0; i < GPIO_MODULE_MAX; i++) {
        memset(gpio_node, 0, sizeof(gpio_node));
        sprintf(gpio_node, "gpio%d", i);
        offset1 = fdt_subnode_offset(fdt, button_offset, gpio_node);
        if (0 > offset1) {
            //log_warn("gpio[%d] %s NULL. \r\n", i, gpio_node);
            continue;
        }

        countindex = fdt_stringlist_count(fdt, offset1, "status");
        if (countindex != 1) {
            blog_warn("gpio[%d] status_countindex = %d NULL. \r\n", i, countindex);
            continue;
        }
        result = fdt_stringlist_get(fdt, offset1, "status", 0, &lentmp);
        if ((lentmp != 4) || (memcmp("okay", result, 4) != 0)) {
            blog_warn("gpio[%d] status = %s\r\n", i, result);
            continue;
        }

        countindex = fdt_stringlist_count(fdt, offset1, "feature");
        if (countindex != 1) {
            blog_warn("gpio[%d] feature_countindex = %d NULL. \r\n", i, countindex);
            continue;
        }
        result = fdt_stringlist_get(fdt, offset1, "feature", 0, &lentmp);
        if ((lentmp != 6) || (memcmp("button", result, 6) != 0)) {
            blog_warn("gpio[%d] feature = %s\r\n", i, result);
            continue;
        }//not button continue

        countindex = fdt_stringlist_count(fdt, offset1, "mode");
        if (countindex != 1) {
            blog_warn("gpio[%d] mode = %d NULL. \r\n", i, countindex);
            continue;
        }
        result = fdt_stringlist_get(fdt, offset1, "mode", 0, &lentmp);
        if ((lentmp != 10) || (memcmp("multipress", result, 10) != 0)) {
            blog_warn("gpio[%d] multipress = %s\r\n", i, result);
            continue;
        }//not button continue

        addr_prop = fdt_getprop(fdt, offset1, "pin", &lentmp);
        if (addr_prop == NULL) {
            blog_warn("gpio[%d] pin NULL. \r\n", i);
            continue;
        }
        stgpio.gpioPin = BL_FDT32_TO_U32(addr_prop, 0);
        blog_info("i = %d, stgpio.gpioPin = %d\r\n", i, stgpio.gpioPin);

        result = fdt_stringlist_get(fdt, offset1, "hbn_use", 0, &lentmp);
        if ((lentmp == 4) && (memcmp("okay", result, 4) == 0)) {
            blog_warn("gpio[%d] status = %s\r\n", i, result);
            pinbuf[pinbuf_size++] = stgpio.gpioPin;
        }

        offset2 = fdt_subnode_offset(fdt, offset1, "button");
        if (0 >= offset2) {
            blog_warn("button feature NULL \r\n");
            continue;
        }
        addr_prop = fdt_getprop(fdt, offset2, "debounce", &lentmp);
        if (addr_prop == NULL) {
            blog_warn("debounce NULL. \r\n");
            continue;
        }
        blog_info("dehounce = %ld \r\n", BL_FDT32_TO_U32(addr_prop, 0));
        ((button_ctx_t*)(stgpio.arg))->debounce = BL_FDT32_TO_U32(addr_prop, 0);

        /* short press */
        offset3 = fdt_subnode_offset(fdt, offset2, "short_press_ms");
        if (0 >= offset3) {
            blog_warn("gpio[%d] short_press_ms feature NULL \r\n", i);
            continue;
        }
        addr_prop = fdt_getprop(fdt, offset3, "start", &lentmp);
        if (addr_prop == NULL) {
            blog_warn("press start  NULL. \r\n");
            continue;
        }
        ((button_ctx_t*)(stgpio.arg))->short_press_start_ms = BL_FDT32_TO_U32(addr_prop, 0);

        addr_prop = fdt_getprop(fdt, offset3, "end", &lentmp);
        if (addr_prop == NULL) {
            blog_warn("press end  NULL. \r\n");
            continue;
        }
        ((button_ctx_t*)(stgpio.arg))->short_press_end_ms = BL_FDT32_TO_U32(addr_prop, 0);

        addr_prop = fdt_getprop(fdt, offset3, "kevent", &lentmp);
        if (addr_prop == NULL) {
            blog_warn("gpio[%d] kevnet  NULL. \r\n", i);
            continue;
        }
        ((button_ctx_t*)(stgpio.arg))->short_kevent = BL_FDT32_TO_U32(addr_prop, 0);

        /* long press */
        offset3 = fdt_subnode_offset(fdt, offset2, "long_press_ms");
        if (0 >= offset3) {
            blog_warn("long_press_ms feature NULL \r\n");
        }
        addr_prop = fdt_getprop(fdt, offset3, "start", &lentmp);
        if (addr_prop == NULL) {
            blog_warn("press start pin NULL. \r\n");
            continue;
        }
        ((button_ctx_t*)(stgpio.arg))->long_press_start_ms = BL_FDT32_TO_U32(addr_prop, 0);

        addr_prop = fdt_getprop(fdt, offset3, "end", &lentmp);
        if (addr_prop == NULL) {
            blog_warn("press end pin NULL. \r\n");
            continue;
        }
        ((button_ctx_t*)(stgpio.arg))->long_press_end_ms = BL_FDT32_TO_U32(addr_prop, 0);

        addr_prop = fdt_getprop(fdt, offset3, "kevent", &lentmp);
        if (addr_prop == NULL) {
            blog_warn("gpio[%d] kevent NULL. \r\n", i);
            continue;
        }
        ((button_ctx_t*)(stgpio.arg))->long_kevent = BL_FDT32_TO_U32(addr_prop, 0);

        /* longlong press */
        offset3 = fdt_subnode_offset(fdt, offset2, "longlong_press_ms");
        if (0 >= offset3) {
            blog_warn("long_press_ms feature NULL \r\n");
        }
        addr_prop = fdt_getprop(fdt, offset3, "start", &lentmp);
        if (addr_prop == NULL) {
            blog_warn("press start pin NULL. \r\n");
            continue;
        }
        ((button_ctx_t*)(stgpio.arg))->longlong_press_ms = BL_FDT32_TO_U32(addr_prop, 0);

        addr_prop = fdt_getprop(fdt, offset3, "kevent", &lentmp);
        if (addr_prop == NULL) {
            blog_warn("gpio[%d] kevent NULL \r\n");
            continue;
        }
        ((button_ctx_t*)(stgpio.arg))->longlong_kevent = BL_FDT32_TO_U32(addr_prop, 0);

        countindex = fdt_stringlist_count(fdt, offset2, "trig_level");
        if (countindex != 1) {
            //log_warn("gpio[%d] trig_level = %d NULL. \r\n", i, countindex);
            continue;
        }
        result = fdt_stringlist_get(fdt, offset2, "trig_level", 0, &lentmp);
        if (lentmp != 2) {
            blog_warn("gpio[%d] trig_level = %s\r\n", i, result);
            continue;
        }
        if (memcmp("Hi", result, 2) == 0) {
            ((button_ctx_t*)(stgpio.arg))->trig_level = 1;
        } else if (memcmp("Lo", result, 2) == 0) {
            ((button_ctx_t*)(stgpio.arg))->trig_level = 0;
        } else {
            continue;
        }
        blog_info("gpio[%d] trig_level = %d\r\n", i, ((button_ctx_t*)(stgpio.arg))->trig_level);

        hal_button_register_handler_with_dts(&stgpio);
    }

    blog_buf(pinbuf, pinbuf_size);
    hal_hbn_init(pinbuf, pinbuf_size);
    vPortFree(pinbuf);
}

void hal_button_module_init(int pin, int short_press_end_ms, int long_press_end_ms, int longlong_press_ms)
{
    gpio_ctx_t stgpio;
    button_ctx_t stbutton;

    stgpio.arg = &stbutton;

    stgpio.gpioPin = pin;

    ((button_ctx_t *) (stgpio.arg))->debounce = 10;

    ((button_ctx_t *) (stgpio.arg))->short_press_start_ms = 100;

    ((button_ctx_t *) (stgpio.arg))->short_press_end_ms = short_press_end_ms;

    ((button_ctx_t *) (stgpio.arg))->short_kevent = 2;

    ((button_ctx_t *) (stgpio.arg))->long_press_start_ms = 1000;

    ((button_ctx_t *) (stgpio.arg))->long_press_end_ms = long_press_end_ms;

    ((button_ctx_t *) (stgpio.arg))->long_kevent = 3;

    ((button_ctx_t *) (stgpio.arg))->longlong_press_ms = longlong_press_ms;

    ((button_ctx_t *) (stgpio.arg))->longlong_kevent = 4;

    ((button_ctx_t *) (stgpio.arg))->trig_level = 1;

    hal_button_register_handler_with_dts(&stgpio);
}
