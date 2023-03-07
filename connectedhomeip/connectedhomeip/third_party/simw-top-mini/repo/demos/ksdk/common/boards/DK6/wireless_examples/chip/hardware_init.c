/*
* Copyright 2020 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "board.h"
#include "clock_config.h"

void BOARD_InitClocks(void)
{
    BOARD_BootClockRUN();
}

void hardware_init(void)
{
    BOARD_common_hw_init();
}
