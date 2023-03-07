/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "timer_cmsdk_drv.h"
#include "platform/include/tfm_plat_defs.h"
#include "tfm_plat_test.h"
#include "device_definition.h"

#define USERLED_MASK                (0x3)
#define BTN_WAIT_INIT_COUNTER_VALUE (10000u)
#define TIMER_RELOAD_VALUE          (16*1024*1024)

/**
 * \brief Store the state of the mocked LED
 *
 * This variable have to be linked to the data section of the partition
 * TFM_SP_CORE_TEST so that in case of in case of isolation within the secure
 * domain the Core Test service can access it.
 */
uint32_t led_status
    TFM_LINK_SET_RW_IN_PARTITION_SECTION("TFM_SP_CORE_TEST")
    = 0x02u;

/**
 * \brief Simulate user reaction time
 */
static void busy_wait_to_simulate_user(void)
{
    volatile uint32_t counter = BTN_WAIT_INIT_COUNTER_VALUE;
    while (counter)
    {
        --counter;
    }
}

void tfm_plat_test_wait_user_button_pressed(void)
{
    busy_wait_to_simulate_user();
}

void tfm_plat_test_wait_user_button_released(void)
{
    busy_wait_to_simulate_user();
}

uint32_t tfm_plat_test_get_led_status(void)
{
    return led_status;
}

void tfm_plat_test_set_led_status(uint32_t status)
{
    led_status = status & USERLED_MASK;
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
