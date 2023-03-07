/*
 * Copyright 2017, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    pin_mux.h
 * @brief   Board pins header file.
 */
 
/* This is an empty template for board specific configuration.*/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 	Initialize board pins.
 */
void BOARD_InitBootPins(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _PIN_MUX_H_ */


