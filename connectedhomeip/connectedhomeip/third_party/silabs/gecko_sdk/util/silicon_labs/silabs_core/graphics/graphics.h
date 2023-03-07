/***************************************************************************//**
 * @file
 * @brief Draws the graphics on the display
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include "glib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/
void GRAPHICS_Init(void);
void GRAPHICS_Sleep(void);
void GRAPHICS_Wakeup(void);
void GRAPHICS_Update(void);
void GRAPHICS_AppendString(char *str);
void GRAPHICS_Clear(void);
void GRAPHICS_InsertTriangle(uint32_t x,
                             uint32_t y,
                             uint32_t size,
                             bool up,
                             int8_t fillPercent);

#ifdef __cplusplus
}
#endif

#endif /* __GRAHPHICS_H */
