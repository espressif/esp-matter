/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform_description.h"
#include "device_definition.h"
#include "systimer_armv8-m_drv.h"
#include "syscounter_armv8-m_cntrl_drv.h"
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
    syscounter_armv8_m_cntrl_init(&SYSCOUNTER_CNTRL_ARMV8_M_DEV_S);

    systimer_armv8_m_init(&SYSTIMER0_ARMV8_M_DEV_S);
    /* If already initialized, call results in a fall through. */
    systimer_armv8_m_set_autoinc_reload(
                            &SYSTIMER0_ARMV8_M_DEV_S, TIMER_RELOAD_VALUE);
    systimer_armv8_m_enable_autoinc(&SYSTIMER0_ARMV8_M_DEV_S);
    systimer_armv8_m_clear_autoinc_interrupt(&SYSTIMER0_ARMV8_M_DEV_S);
    systimer_armv8_m_enable_interrupt(&SYSTIMER0_ARMV8_M_DEV_S);
}

void tfm_plat_test_secure_timer_stop(void)
{
    systimer_armv8_m_uninit(&SYSTIMER0_ARMV8_M_DEV_S);
    systimer_armv8_m_clear_autoinc_interrupt(&SYSTIMER0_ARMV8_M_DEV_S);
}

void tfm_plat_test_non_secure_timer_start(void)
{
    systimer_armv8_m_init(&SYSTIMER1_ARMV8_M_DEV_NS);
    /* If already initialized, call results in a fall through. */
    systimer_armv8_m_set_autoinc_reload(
                                &SYSTIMER1_ARMV8_M_DEV_NS, TIMER_RELOAD_VALUE);
    systimer_armv8_m_enable_autoinc(&SYSTIMER1_ARMV8_M_DEV_NS);
    systimer_armv8_m_clear_autoinc_interrupt(&SYSTIMER1_ARMV8_M_DEV_NS);
    systimer_armv8_m_enable_interrupt(&SYSTIMER1_ARMV8_M_DEV_NS);
}

void tfm_plat_test_non_secure_timer_stop(void)
{
    systimer_armv8_m_uninit(&SYSTIMER1_ARMV8_M_DEV_NS);
    systimer_armv8_m_clear_autoinc_interrupt(&SYSTIMER1_ARMV8_M_DEV_NS);
}
