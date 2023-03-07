/***************************************************************************//**
 * @file
 * @brief Graphics code for hall effect wheel demo
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "stddef.h"
#include "stdbool.h"
#include "em_device.h"

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

/**************************************************************************//**
 * Prototype declarations for Graphics screen updates from graphics.c
 *****************************************************************************/
void GRAPHICS_SetupGraphics(void);
void GRAPHICS_Demo_Menu(bool i2cError, uint8_t selection);
void GRAPHICS_WheelAngle(bool i2cError, int32_t u1, int32_t u2, int32_t angle);
void GRAPHICS_RevCounter(bool i2cError, int8_t revCount, uint8_t quad);
void GRAPHICS_PS_I2C(bool i2cError, bool scale, int32_t uTField);
void GRAPHICS_PS_Temp(bool i2cError, int32_t temp);
void GRAPHICS_PS_Switch(bool i2cError, uint8_t pin, int16_t tamper);
void GRAPHICS_PS_Analog(uint16_t voltageData, int32_t uTField);
void GRAPHICS_PS_PWM(int32_t uTField, uint32_t dCycle);
void GRAPHICS_PS_SENT(int32_t uTField, uint16_t nibbleData);

#endif /* GRAPHICS_H_ */
