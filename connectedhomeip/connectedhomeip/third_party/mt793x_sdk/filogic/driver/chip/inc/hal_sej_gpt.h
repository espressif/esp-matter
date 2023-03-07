/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2021. All rights reserved.
 */
#ifndef _HAL_SEJ_GPT_INTERNAL_H_
#define _HAL_SEJ_GPT_INTERNAL_H_

#ifdef HAL_SEJ_GPT_MODULE_ENABLED
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "mt7933.h"

#ifdef __cplusplus
extern "C"
{
#endif

//#define SEJ_GPT_DEBUG_LOG
#define HAL_SEJ_GPT_MAX_MS  (330382)
#define OVERFLOW_MAX_VAL     (0xFFFFFFFFUL)

#define SEJ_GPT_BASE        (0xC0090200)
#define SEJ_GPT_IRQEN       (SEJ_GPT_BASE + 0x0)
#define SEJ_GPT_IRQSTA      (SEJ_GPT_BASE + 0x4)
#define SEJ_GPT_IRQACK      (SEJ_GPT_BASE + 0x8)

#define SEJ_GPT0_CON        (SEJ_GPT_BASE + 0x10)
#define SEJ_GPT0_PRESCALE   (SEJ_GPT_BASE + 0x14)
#define SEJ_GPT0_COUNTER    (SEJ_GPT_BASE + 0x18)
#define SEJ_GPT0_COMPARE    (SEJ_GPT_BASE + 0x1C)

#define SEJ_GPT1_CON        (SEJ_GPT_BASE + 0x20)
#define SEJ_GPT1_PRESCALE   (SEJ_GPT_BASE + 0x24)
#define SEJ_GPT1_COUNTER0   (SEJ_GPT_BASE + 0x28)
#define SEJ_GPT1_COUNTER1   (SEJ_GPT_BASE + 0x2C)
#define SEJ_GPT1_COMPARE0   (SEJ_GPT_BASE + 0x30)
#define SEJ_GPT1_COMPARE1   (SEJ_GPT_BASE + 0x34)

typedef enum {
    HAL_SEJ_GPT_PORT_INVALID = -1,
    HAL_SEJ_GPT0 = 0,
    HAL_SEJ_GPT1 = 1,
    HAL_SEJ_GPT_MAX_PORT
} hal_sej_gpt_port_t;

typedef enum {
    SEJ_GPT_ONE_SHOT = 0x0,
    SEJ_GPT_REPEAT   = 0x1,
    SEJ_GPT_KEEP_GO  = 0x2,
    SEJ_GPT_FREE_RUN = 0x3
} sej_gpt_mode_t;

typedef enum {
    SEJ_GPT_DIVIDE_1  = 0,
    SEJ_GPT_DIVIDE_2  = 1,
    SEJ_GPT_DIVIDE_3  = 2,
    SEJ_GPT_DIVIDE_4  = 3,
    SEJ_GPT_DIVIDE_5  = 4,
    SEJ_GPT_DIVIDE_6  = 5,
    SEJ_GPT_DIVIDE_7  = 6,
    SEJ_GPT_DIVIDE_8  = 7,
    SEJ_GPT_DIVIDE_9  = 8,
    SEJ_GPT_DIVIDE_10 = 9,
    SEJ_GPT_DIVIDE_11 = 10,
    SEJ_GPT_DIVIDE_12 = 11,
    SEJ_GPT_DIVIDE_13 = 12,
    SEJ_GPT_DIVIDE_16 = 13,
    SEJ_GPT_DIVIDE_32 = 14,
    SEJ_GPT_DIVIDE_64 = 15
} sej_gpt_clkdiv_t;

typedef enum {
    HAL_SEJ_GPT_ERROR_CLOCK       = -5,/**< SEJ_GPT clock enabling failed. */
    HAL_SEJ_GPT_ERROR_PORT_USED   = -4,/**< The timer has beed used. */
    HAL_SEJ_GPT_ERROR             = -3,/**< SEJ_GPT function error occurred. */
    HAL_SEJ_GPT_ERROR_PORT        = -2,/**< A wrong SEJ_GPT port is set. */
    HAL_SEJ_GPT_INVALID_PARAMETER = -1,/**< An invalid parameter. */
    HAL_SEJ_GPT_OK   = 0,              /**< No error occurred during the function call.*/
} hal_sej_gpt_status_t;

typedef enum {
    HAL_SEJ_GPT_STOPPED = 0,
    HAL_SEJ_GPT_RUNNING = 1
} hal_sej_gpt_running_status_t;

typedef void(* hal_sej_gpt_callback_t)(void *user_data);

typedef struct {
    hal_sej_gpt_callback_t callback;
    void *user_data;
} sej_gpt_callback_t;

typedef struct {
    hal_sej_gpt_running_status_t running_status;
    sej_gpt_callback_t   callback_context;
    bool                 has_initilized;
    bool                 is_gpt_locked_sleep;
} sej_gpt_context_t;

hal_sej_gpt_status_t hal_sej_gpt_init(hal_sej_gpt_port_t gpt_port);
hal_sej_gpt_status_t hal_sej_gpt_deinit(hal_sej_gpt_port_t gpt_port);
//hal_sej_gpt_status_t hal_sej_gpt_get_running_status(hal_sej_gpt_port_t gpt_port, hal_sej_gpt_running_status_t *running_status);
hal_sej_gpt_status_t hal_sej_gpt_register_callback(hal_sej_gpt_port_t gpt_port, hal_sej_gpt_callback_t callback, void *user_data);
//hal_sej_gpt_status_t hal_sej_gpt_get_free_run_count(hal_gpt_clock_source_t clock_source, uint32_t *count);
hal_sej_gpt_status_t hal_sej_gpt_start_timer_ms(hal_sej_gpt_port_t gpt_port, uint32_t timeout_time_ms, sej_gpt_mode_t timer_type);
hal_sej_gpt_status_t hal_sej_gpt_stop_timer(hal_sej_gpt_port_t gpt_port);
#if 0
hal_sej_gpt_status_t hal_sej_gpt_delay_ms(uint32_t ms);
#ifdef HAL_GPT_FEATURE_US_TIMER
hal_sej_gpt_status_t hal_sej_gpt_start_timer_us(hal_sej_gpt_port_t gpt_port, uint32_t timeout_time_us, hal_gpt_timer_type_t timer_type);
#endif
hal_sej_gpt_status_t hal_sej_gpt_delay_us(uint32_t us);
hal_sej_gpt_status_t hal_sej_gpt_get_duration_count(uint32_t start_count, uint32_t end_count, uint32_t *duration_count);
#endif

#ifdef __cplusplus
}
#endif

#endif /*HAL_SEJ_GPT_MODULE_ENABLED*/
#endif /*_HAL_GPT_SEJ_INTERNAL_H_*/

