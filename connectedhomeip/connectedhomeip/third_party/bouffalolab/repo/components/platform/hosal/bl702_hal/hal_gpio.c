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
//#include <loopset.h>

#include "hal_gpio.h"
#include "bl_gpio.h"

#include <blog.h>

#define USER_UNUSED(a) ((void)(a))
#define GPIO_MAX_NUM        31
#define GPIO_MAX_NUM_STR    "max_num"
#define BL_FDT32_TO_U8(addr, byte_offset)   ((uint8_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
#define BL_FDT32_TO_U16(addr, byte_offset)  ((uint16_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
#define BL_FDT32_TO_U32(addr, byte_offset)  ((uint32_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
struct gpio_feature_config {
    int pin;
#define GPIO_VALID_NOT              (0)
#define GPIO_VALID_OK               (1)
    uint8_t valid;
#define GPIO_FEATURE_CONFIG_LED     (0)
    uint8_t feature;//0: LED
#define GPIO_ACTIVE_LO              (0)
#define GPIO_ACTIVE_HI              (1)
    uint8_t active;//0: Lo, 1:Hi
#define GPIO_MODE_BLINK             (0)
#define GPIO_MODE_HEARTBEAT          (1)
#define GPIO_MODE_ONOFF             (2)
    uint8_t mode;//0:blink, 1:heartbeat
    unsigned int time;
};

struct
{
    int pin;
    uint8_t valid;
    uint8_t active;
} led_onoff_config;


static int _get_gpio_max(const void* fdt, uint32_t dtb_offset)
{
    int lentmp = 0;
    const uint32_t *addr_prop = 0;
    uint32_t max_num;

    addr_prop = fdt_getprop(fdt, dtb_offset, GPIO_MAX_NUM_STR, &lentmp);
    if (NULL == addr_prop) {
        return -1;
    }
    max_num = BL_FDT32_TO_U32(addr_prop, 0);
    return max_num;
}

static int _get_gpio_config(const void* fdt, uint32_t dtb_offset, const char *name, struct gpio_feature_config *gpio_config)
{
    int offset1;
    int lentmp = 0;
    const uint32_t *addr_prop = 0;
    const char *result = 0;


    /*
     *  gpio1 {
     *  status = "okay";
     *  pin  = <5>;
     *  feature = "led";
     *  active = "Hi"; //Hi or Lo
     *  mode = "blink"; //blink or hearbeat
     *
     * */

    offset1 = fdt_subnode_offset(fdt, dtb_offset, name);
    if (offset1 < 0) {
        blog_info("%s NOT found\r\n", name);
        return -1;
    }

    memset(gpio_config, 0, sizeof(struct gpio_feature_config));
    gpio_config->valid  = GPIO_VALID_NOT;

    result = fdt_stringlist_get(fdt, offset1, "status", 0, &lentmp);
    if ((lentmp != 4) || (memcmp("okay", result, 4) != 0)) {
        blog_info("[%s] status = %s\r\n", name, result);
        return 0;
    }

    addr_prop = fdt_getprop(fdt, offset1, "pin", &lentmp);
    if (addr_prop == NULL) {
        blog_error("no pin found for %s\r\n", name);
        return 0;
    }
    gpio_config->pin = BL_FDT32_TO_U32(addr_prop, 0);

    result = fdt_stringlist_get(fdt, offset1, "feature", 0, &lentmp);
    if (3 == lentmp && memcmp("led", result, 3) == 0) {
        gpio_config->feature = GPIO_FEATURE_CONFIG_LED;
    } else {
        blog_error("%s: unvalid GPIO config %3s\r\n", name, result);
        return 0;
    }

    result = fdt_stringlist_get(fdt, offset1, "active", 0, &lentmp);
    if (2 == lentmp && memcmp("Hi", result, 2) == 0) {
        gpio_config->active = GPIO_ACTIVE_HI;
    } else if (2 == lentmp && memcmp("Lo", result, 2) == 0) {
        gpio_config->active = GPIO_ACTIVE_LO;
    } else {
        blog_error("%s: unvalid GPIO config %3s\r\n", name, result);
        return 0;
    }

    result = fdt_stringlist_get(fdt, offset1, "mode", 0, &lentmp);
    if (5 == lentmp && memcmp("blink", result, 5) == 0) {
        gpio_config->mode = GPIO_MODE_BLINK;
    } else if (9 == lentmp && memcmp("heartbeat", result, 9) == 0) {
        gpio_config->mode = GPIO_MODE_HEARTBEAT;
    } else if (5 == lentmp && memcmp("onoff", result, 5) == 0) {
        gpio_config->mode = GPIO_MODE_ONOFF;
    } else {
        blog_error("%s: unvalid GPIO config %3s\r\n", name, result);
        return 0;
    }

    addr_prop = fdt_getprop(fdt, offset1, "time", &lentmp);
    if (addr_prop == NULL) {
        blog_error("%s: unvalid GPIO config\r\n", name);
        return 0;
    }
    gpio_config->time = BL_FDT32_TO_U32(addr_prop, 0);

    /*scan is done, so config is valid now*/
    gpio_config->valid = GPIO_VALID_OK;

    return 0;
}

static void _dump_gpio_conf(struct gpio_feature_config *config)
{
    blog_info("New CONF: GPIO%02u: feature %u, active %u, mode %u, time %u\r\n",
            config->pin,
            config->feature,
            config->active,
            config->mode,
            config->time
    );
}

static void _apply_gpio_config(struct gpio_feature_config *config)
{
    if(config->mode == GPIO_MODE_ONOFF){
        led_onoff_config.pin = config->pin;
        led_onoff_config.valid = 1;
        led_onoff_config.active = config->active;
        
        bl_gpio_enable_output(led_onoff_config.pin, 0, 0);
        bl_gpio_output_set(led_onoff_config.pin, !led_onoff_config.active);
        return;
    }
    
//    loopset_led_trigger(config->pin, config->time);
}

int hal_gpio_init_from_dts(uint32_t fdt, uint32_t dtb_offset)
{
    int i, num;
    char node[12];
    struct gpio_feature_config gpio_config;

    memset(&led_onoff_config,0,sizeof(led_onoff_config));
    num = _get_gpio_max((const void*)fdt, dtb_offset);
    blog_info("[HAL] [GPIO] Max num is %d\r\n", num);

    USER_UNUSED(i);
    USER_UNUSED(node);
    USER_UNUSED(gpio_config);
    for (i = 0; i < num; i++) {
        snprintf(node, sizeof(node) - 1, "gpio%u", i);
        node[sizeof(node) - 1] = '\0';
        if (_get_gpio_config((const void*)fdt, dtb_offset, node, &gpio_config) < 0) {
            break;
        } else if (GPIO_VALID_OK == gpio_config.valid) {
            _dump_gpio_conf(&gpio_config);
            _apply_gpio_config(&gpio_config);
        } else if (GPIO_VALID_NOT == gpio_config.valid) {
            blog_error("%s: unvalid GPIO config\r\n", node);
        }
    }

    return 0;
}

static gpio_ctx_t *pstgpio_head = NULL;
int hal_gpio_register_handler(void *func, int gpioPin, int intCtrlMod, int intTrgMod, void *arg)
{
    if (NULL == func || gpioPin > GPIO_MAX_NUM || intCtrlMod > 1 || intTrgMod > 3) {
        printf("register paraments is not correct! \r\n");
        return -1;
    }

    gpio_ctx_t *pstnode;

    pstnode  = pvPortMalloc(sizeof(gpio_ctx_t));
    if (!pstnode) {
        printf("Malloc failed \r\n");
        return -1;
    }

    pstnode->gpioPin = gpioPin;
    pstnode->intCtrlMod = intCtrlMod;
    pstnode->intTrgMod = intTrgMod;
    pstnode->gpio_handler = func;
    pstnode->arg = arg;

    if (!pstgpio_head) {
        pstgpio_head = pstnode;
        pstnode->next = NULL;
    }
    else {
        pstnode->next = pstgpio_head;
        pstgpio_head = pstnode;
    }

    bl_gpio_register(pstgpio_head);

    return 0;

}

int hal_gpio_led_on(void)
{
    if(led_onoff_config.valid){
        bl_gpio_output_set(led_onoff_config.pin, led_onoff_config.active);
        return 0;
    }else{
        return -1;
    }
}

int hal_gpio_led_off(void)
{
    if(led_onoff_config.valid){
        bl_gpio_output_set(led_onoff_config.pin, !led_onoff_config.active);
        return 0;
    }else{
        return -1;
    }
}
