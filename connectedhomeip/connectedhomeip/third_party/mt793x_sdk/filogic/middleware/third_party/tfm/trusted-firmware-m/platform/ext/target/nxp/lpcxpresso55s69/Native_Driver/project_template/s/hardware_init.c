/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pin_mux.h"
#include "board.h"
#include "clock_config.h"

void BOARD_InitHardware(void)
{
    /* attach main clock divide to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitPins();

    /* Flash operations (erase, blank check, program) and reading single word can only be performed for CPU frequencies
    of up to 100 MHz. Cannot be performed for frequencies above 100 MHz. */
    BOARD_BootClockFROHF96M();

    BOARD_InitDebugConsole();
}

void SystemInitHook(void)
{
/* Coprocessor Access Control Register */
#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
    SCB_NS->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10, CP11 Full Access */
#endif                                                    /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */

    SCB_NS->CPACR |= ((3UL << 0 * 2) | (3UL << 1 * 2)); /* set CP0, CP1 Full Access (enable PowerQuad) */

    BOARD_InitHardware();
}
