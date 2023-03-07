/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform_retarget.h"
#include "platform_retarget_dev.h"
#include "tfm_plat_test.h"
#include "hal_gpt.h"
#include "hal_sej_gpt.h"
#include "hal_platform.h"

#if 0
#include "smm_mps2.h"
#endif

#define TIMER_RELOAD_VALUE (16*1024*1024)
#define USERLED_MASK       (0x3)
#define MPS2_USERPB0_BASE  (0x50302008)
#define MPS2_USERPB0_MASK  (0x1)
#define SEJ_GPT_PORT    (HAL_SEJ_GPT0)

static uint32_t sw_gpt_handle;
extern void TIMER1_Handler(void);

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
#if 0
    struct arm_mps2_fpgaio_t *fpgaio = SEC_MPS2_FPGAIO;
    return  fpgaio->LED;
#endif
}

void tfm_plat_test_set_led_status(uint32_t status)
{
#if 0
    struct arm_mps2_fpgaio_t *fpgaio = SEC_MPS2_FPGAIO;
    fpgaio->LED = status;
#endif
}

uint32_t tfm_plat_test_get_userled_mask(void)
{
    return USERLED_MASK;
}

void sej_gpt_callback(void *user_data)
{
    (void)user_data;
    TIMER1_Handler();
    printf("call TIMER1_Handler\r\n");
}

void tfm_plat_test_secure_timer_start(void)
{
    hal_sej_gpt_status_t ret_status;
    ret_status = hal_sej_gpt_init(SEJ_GPT_PORT);
    if (ret_status != HAL_SEJ_GPT_OK) {
        printf("ERROR : Secure GPT init Fail: %d\n", ret_status);
        return;
    }

    hal_sej_gpt_register_callback(SEJ_GPT_PORT, (hal_gpt_callback_t)sej_gpt_callback, NULL);
    ret_status = hal_sej_gpt_start_timer_ms(SEJ_GPT_PORT, 3000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
    if (ret_status != HAL_SEJ_GPT_OK) {
        printf("ERROR : Secure GPT start Fail: %d\n", ret_status);
        return;
    }
}

void tfm_plat_test_secure_timer_stop(void)
{
    hal_sej_gpt_status_t ret_status;
    ret_status = hal_sej_gpt_stop_timer(SEJ_GPT_PORT);
    if (ret_status != HAL_SEJ_GPT_OK) {
        printf("ERROR : Secure GPT stop Fail: %d\n", ret_status);
    }
    hal_sej_gpt_deinit(SEJ_GPT_PORT);
}

void tfm_plat_test_non_secure_timer_start(void)
{
    if (hal_gpt_sw_get_timer(&sw_gpt_handle))
    {
        printf("get sw gpt fail.\r\n");
    }

    if(hal_gpt_sw_start_timer_ms(sw_gpt_handle, 3000, sej_gpt_callback, NULL))
    {
        printf("start sw gpt fail.\r\n");
    }
}

void tfm_plat_test_non_secure_timer_stop(void)
{
    if (hal_gpt_sw_stop_timer_ms(sw_gpt_handle))
    {
        printf("stop gpt fail.\r\n");
    }
}
