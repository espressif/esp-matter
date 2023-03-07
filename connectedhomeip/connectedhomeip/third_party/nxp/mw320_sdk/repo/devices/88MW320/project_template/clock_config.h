/*
 * Copyright 2017, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    clock_config.h
 * @brief   Board clocks header file.
 */

/* This is an empty template for board specific configuration.*/

#ifndef _CLOCK_CONFIG_H_
#define _CLOCK_CONFIG_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 	Initialize board clocks.
 */
void BOARD_InitBootClocks(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CLOCK_CONFIG_H_ */
