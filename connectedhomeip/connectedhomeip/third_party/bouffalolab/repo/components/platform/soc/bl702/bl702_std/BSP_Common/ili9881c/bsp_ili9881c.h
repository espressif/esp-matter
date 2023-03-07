/**
 * @file bsp_ili9881c.h
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#ifndef __ILI9881C_H__
#define __ILI9881C_H__

#include "hal_gpio.h"
/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  ILI9881C
 *  @{
 */

/** @defgroup  ILI9881C_Public_Types
 *  @{
 */

/*@} end of group ILI9881C_Public_Types */

/** @defgroup  ILI9881C_Public_Constants
 *  @{
 */

/*@} end of group ILI9881C_Public_Constants */

/** @defgroup  ILI9881C_Public_Macros
 *  @{
 */
#define LCD_CS_PIN GPIO_PIN_10
#define LCD_DC_PIN GPIO_PIN_22

#define CS1_HIGH gpio_write(LCD_CS_PIN, 1)
#define CS1_LOW  gpio_write(LCD_CS_PIN, 0)
#define DC_HIGH  gpio_write(LCD_DC_PIN, 1)
#define DC_LOW   gpio_write(LCD_DC_PIN, 0)
#define LCD_W    240 /* LCD width */
#define LCD_H    320 /* LCD height */
/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)
/* Calculate 32-bit or 16-bit absolute value */
#define ABS32(value) ((value ^ (value >> 31)) - (value >> 31))
#define ABS16(value) ((value ^ (value >> 15)) - (value >> 15))

/*@} end of group ILI9881C_Public_Macros */

/** @defgroup  ILI9881C_Public_Functions
 *  @{
 */

/**
 *  @brief ILI9881C Functions
 */

int display_prepare(void);
int display_enable(void);
int display_disable(void);
int display_unprepare(void);

/*@} end of group ILI9881C_Public_Functions */

/*@} end of group ILI9881C */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __ILI9881C_H__ */
