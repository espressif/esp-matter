/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform_retarget.h"
#include "platform_retarget_dev.h"
#include "timer_cmsdk_drv.h"
#include "tfm_plat_test.h"

#include "smm_mps2.h"

#define TIMER_RELOAD_VALUE (16*1024*1024)
#define USERLED_MASK       (0x3)
#define MPS2_USERPB0_BASE  (0x50302008)
#define MPS2_USERPB0_MASK  (0x1)

void tfm_plat_test_wait_user_button_pressed(void)
{
    volatile uint32_t *p_btn = (volatile uint32_t *) MPS2_USERPB0_BASE;

    /* Wait until user button 0 is pressed */
    while (!(*p_btn & MPS2_USERPB0_MASK)) {
      ;
    }
}

void tfm_plat_test_wait_user_button_released(void)
{
    volatile uint32_t *p_btn = (volatile uint32_t *) MPS2_USERPB0_BASE;

    /* Wait until user button 0 is released */
    while ((*p_btn & MPS2_USERPB0_MASK)) {
      ;
    }
}

uint32_t tfm_plat_test_get_led_status(void)
{
    struct arm_mps2_fpgaio_t *fpgaio = SEC_MPS2_FPGAIO;
    return  fpgaio->LED;
}

void tfm_plat_test_set_led_status(uint32_t status)
{
    struct arm_mps2_fpgaio_t *fpgaio = SEC_MPS2_FPGAIO;
    fpgaio->LED = status;
}

uint32_t tfm_plat_test_get_userled_mask(void)
{
    return USERLED_MASK;
}

void tfm_plat_test_secure_timer_start(void)
{
    if (!timer_cmsdk_is_initialized(&CMSDK_TIMER0_DEV_S)) {
        timer_cmsdk_init(&CMSDK_TIMER0_DEV_S);
    }
    timer_cmsdk_set_reload_value(&CMSDK_TIMER0_DEV_S, TIMER_RELOAD_VALUE);
    timer_cmsdk_enable(&CMSDK_TIMER0_DEV_S);
    timer_cmsdk_enable_interrupt(&CMSDK_TIMER0_DEV_S);
}

void tfm_plat_test_secure_timer_stop(void)
{
    timer_cmsdk_disable(&CMSDK_TIMER0_DEV_S);
    timer_cmsdk_disable_interrupt(&CMSDK_TIMER0_DEV_S);
    timer_cmsdk_clear_interrupt(&CMSDK_TIMER0_DEV_S);
}

void tfm_plat_test_non_secure_timer_start(void)
{
    if (!timer_cmsdk_is_initialized(&CMSDK_TIMER1_DEV_NS)) {
        timer_cmsdk_init(&CMSDK_TIMER1_DEV_NS);
    }
    timer_cmsdk_set_reload_value(&CMSDK_TIMER1_DEV_NS, TIMER_RELOAD_VALUE);
    timer_cmsdk_enable(&CMSDK_TIMER1_DEV_NS);
    timer_cmsdk_enable_interrupt(&CMSDK_TIMER1_DEV_NS);
}

void tfm_plat_test_non_secure_timer_stop(void)
{
    timer_cmsdk_disable(&CMSDK_TIMER1_DEV_NS);
    timer_cmsdk_disable_interrupt(&CMSDK_TIMER1_DEV_NS);
    timer_cmsdk_clear_interrupt(&CMSDK_TIMER1_DEV_NS);
}
