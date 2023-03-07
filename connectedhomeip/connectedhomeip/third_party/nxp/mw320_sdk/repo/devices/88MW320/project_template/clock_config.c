/*
 * Copyright 2017, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v4.0
* BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/**
 * @file    clock_config.c
 * @brief   Board clocks initialization file.
 */

/* This is a empty template for board specific configuration.*/

#include "fsl_common.h"
#include "clock_config.h"

/**
 * @brief Set up and initialize all required blocks and functions related to the board hardware.
 */
void BOARD_InitBootClocks(void)
{
    /* The user initialization should be placed here */

    /* Read core clock setting. */
    SystemCoreClockUpdate();
}
