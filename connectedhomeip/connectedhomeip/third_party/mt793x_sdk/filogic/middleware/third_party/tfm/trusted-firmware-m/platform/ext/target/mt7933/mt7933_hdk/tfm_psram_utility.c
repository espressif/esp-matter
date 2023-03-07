/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file includes extra functions that needed in the PSRAM init flow.
 * These functions are added the prefix sysram_, it means these only used in PSRAM init flow.
 */

#include "hal_platform.h"
#include "hal_clock.h"
#include "hal_asic_mpu.h"
#include "hal_psram.h"
//#include "hw_uart.h"

void sysram_hw_uart_printf(char *str, ...) __attribute__((weak));
hal_clock_status_t sysram_hal_clock_enable(hal_clock_cg_id clock_id) __attribute__((weak));
// from "hal_asic_mpu.h" and add sysram_ prefix by linker
extern hal_asic_mpu_status_t sysram_hal_asic_mpu_set_region_apc(hal_asic_mpu_type_t mpu_type, unsigned int region, int domain, unsigned int apc);
// from "hal_boot.h" and add sysram_ prefix by linker
extern uint16_t sysram_hal_boot_get_hw_ver(void);

void Enable_all_clk(void)
{
    hal_clock_cg_id cid;

    for (cid = 0; cid < HAL_CLOCK_CG_MAX; cid++)
    {
        sysram_hal_clock_enable(cid);
    }
}

void sysram_log_hal_error_internal(const char *func, int line, const char *message, ...)
{
        sysram_hw_uart_printf((char*)message);
}

void sysram_log_hal_warning_internal(const char *func, int line, const char *message, ...)
{
        sysram_hw_uart_printf((char*)message);
}

void sysram_log_hal_info_internal(const char *func, int line, const char *message, ...)
{
        sysram_hw_uart_printf((char*)message);
}

void sysram_log_hal_dump_internal(const char *func, int line, const char *message, const void *data, int length, ...)
{
    sysram_hw_uart_printf((char*)message);
}

void sysram_log_hal_debug_internal(const char *func, int line, const char *message, const void *data, int length, ...)
{
    sysram_hw_uart_printf((char*)message);
}
