/***************************************************************************//**
 * @file    iot_gpio_drv.h
 * @brief   GPIO driver header file.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#ifndef _IOT_GPIO_DRV_H_
#define _IOT_GPIO_DRV_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

#include "em_core.h"
#include "em_device.h"
#include "em_gpio.h"

/*******************************************************************************
 *                               MACROS
 ******************************************************************************/

/* API and DEF macros */
#define IOT_GPIO_DRV_API(func_name)  (iot_gpio_drv_##func_name)
#define IOT_GPIO_DRV_DEF(defn_name)  (IOT_GPIO_DRV_##defn_name)

/* number of instances (5) */
#define IOT_GPIO_DRV_COUNT           (IOT_GPIO_CFG_MAX_COUNT)

/*******************************************************************************
 *                           Generic PINS/PORTS
 ******************************************************************************/

typedef enum {
  /* >> INVALID */
  /* 0x00 */ slInvalidPin0,
  /* 0x01 */ slInvalidPin1,
  /* 0x02 */ slInvalidPin2,
  /* 0x03 */ slInvalidPin3,
  /* 0x04 */ slInvalidPin4,
  /* 0x05 */ slInvalidPin5,
  /* 0x06 */ slInvalidPin6,
  /* 0x07 */ slInvalidPin7,
  /* 0x08 */ slInvalidPin8,
  /* 0x09 */ slInvalidPin9,
  /* 0x0A */ slInvalidPin10,
  /* 0x0B */ slInvalidPin11,
  /* 0x0C */ slInvalidPin12,
  /* 0x0D */ slInvalidPin13,
  /* 0x0E */ slInvalidPin14,
  /* 0x0F */ slInvalidPin15,

  /* >> PORT I */
  /* 0x10 */ slGpioPinI0,
  /* 0x11 */ slGpioPinI1,
  /* 0x12 */ slGpioPinI2,
  /* 0x13 */ slGpioPinI3,
  /* 0x14 */ slGpioPinI4,
  /* 0x15 */ slGpioPinI5,
  /* 0x16 */ slGpioPinI6,
  /* 0x17 */ slGpioPinI7,
  /* 0x18 */ slGpioPinI8,
  /* 0x19 */ slGpioPinI9,
  /* 0x1A */ slGpioPinI10,
  /* 0x1B */ slGpioPinI11,
  /* 0x1C */ slGpioPinI12,
  /* 0x1D */ slGpioPinI13,
  /* 0x1E */ slGpioPinI14,
  /* 0x1F */ slGpioPinI15,

  /* >> SPECIAL 0 */
  /* 0x20 */ slInputPinAVDD,
  /* 0x21 */ slInputPinBUVDD,
  /* 0x22 */ slInputPinDVDD,
  /* 0x23 */ slInputPinPAVDD,
  /* 0x24 */ slInputPinDECOUPLE,
  /* 0x25 */ slInputPinIOVDD,
  /* 0x26 */ slInputPinIOVDD1,
  /* 0x27 */ slInputPinVSP,
  /* 0x28 */ slInputPin68,
  /* 0x29 */ slInputPin69,
  /* 0x2A */ slInputPin6A,
  /* 0x2B */ slInputPin6B,
  /* 0x2C */ slInputPin6C,
  /* 0x2D */ slInputPin6D,
  /* 0x2E */ slInputPin6E,
  /* 0x2F */ slInputPin6F,

  /* >> SPECIAL 1 */
  /* 0x30 */ slInputPinPosRef,
  /* 0x31 */ slInputPinNegRef,
  /* 0x32 */ slInputPinOPA2,
  /* 0x33 */ slInputPinTEMP,
  /* 0x34 */ slInputPinDAC0OUT0,
  /* 0x35 */ slInputPinR5VOUT,
  /* 0x36 */ slInputPinSP1,
  /* 0x37 */ slInputPinSP2,
  /* 0x38 */ slInputPinDAC0OUT1,
  /* 0x39 */ slInputPinSUBLSB,
  /* 0x3A */ slInputPinOPA3,
  /* 0x3B */ slInputPinTEST,
  /* 0x3C */ slInputPinVRefDiv2,
  /* 0x3D */ slInputPinGND,
  /* 0x3E */ slInputPinVSSAUX,
  /* 0x3F */ slInputPinVSS,

  /* >> RESERVED 0 */
  /* 0x40 */ slReservedPin40,
  /* 0x41 */ slReservedPin41,
  /* 0x42 */ slReservedPin42,
  /* 0x43 */ slReservedPin43,
  /* 0x44 */ slReservedPin44,
  /* 0x45 */ slReservedPin45,
  /* 0x46 */ slReservedPin46,
  /* 0x47 */ slReservedPin47,
  /* 0x48 */ slReservedPin48,
  /* 0x49 */ slReservedPin49,
  /* 0x4A */ slReservedPin4A,
  /* 0x4B */ slReservedPin4B,
  /* 0x4C */ slReservedPin4C,
  /* 0x4D */ slReservedPin4D,
  /* 0x4E */ slReservedPin4E,
  /* 0x4F */ slReservedPin4F,

  /* >> RESERVED 1 */
  /* 0x50 */ slReservedPin50,
  /* 0x51 */ slReservedPin51,
  /* 0x52 */ slReservedPin52,
  /* 0x53 */ slReservedPin53,
  /* 0x54 */ slReservedPin54,
  /* 0x55 */ slReservedPin55,
  /* 0x56 */ slReservedPin56,
  /* 0x57 */ slReservedPin57,
  /* 0x58 */ slReservedPin58,
  /* 0x59 */ slReservedPin59,
  /* 0x5A */ slReservedPin5A,
  /* 0x5B */ slReservedPin5B,
  /* 0x5C */ slReservedPin5C,
  /* 0x5D */ slReservedPin5D,
  /* 0x5E */ slReservedPin5E,
  /* 0x5F */ slReservedPin5F,

  /* >> PORT G */
  /* 0x60 */ slGpioPinG0,
  /* 0x61 */ slGpioPinG1,
  /* 0x62 */ slGpioPinG2,
  /* 0x63 */ slGpioPinG3,
  /* 0x64 */ slGpioPinG4,
  /* 0x65 */ slGpioPinG5,
  /* 0x66 */ slGpioPinG6,
  /* 0x67 */ slGpioPinG7,
  /* 0x68 */ slGpioPinG8,
  /* 0x69 */ slGpioPinG9,
  /* 0x6A */ slGpioPinG10,
  /* 0x6B */ slGpioPinG11,
  /* 0x6C */ slGpioPinG12,
  /* 0x6D */ slGpioPinG13,
  /* 0x6E */ slGpioPinG14,
  /* 0x6F */ slGpioPinG15,

  /* >> PORT J */
  /* 0x70 */ slGpioPinJ0,
  /* 0x71 */ slGpioPinJ1,
  /* 0x72 */ slGpioPinJ2,
  /* 0x73 */ slGpioPinJ3,
  /* 0x74 */ slGpioPinJ4,
  /* 0x75 */ slGpioPinJ5,
  /* 0x76 */ slGpioPinJ6,
  /* 0x77 */ slGpioPinJ7,
  /* 0x78 */ slGpioPinJ8,
  /* 0x79 */ slGpioPinJ9,
  /* 0x7A */ slGpioPinJ10,
  /* 0x7B */ slGpioPinJ11,
  /* 0x7C */ slGpioPinJ12,
  /* 0x7D */ slGpioPinJ13,
  /* 0x7E */ slGpioPinJ14,
  /* 0x7F */ slGpioPinJ15,

  /* >> PORT H */
  /* 0x80 */ slGpioPinH0,
  /* 0x81 */ slGpioPinH1,
  /* 0x82 */ slGpioPinH2,
  /* 0x83 */ slGpioPinH3,
  /* 0x84 */ slGpioPinH4,
  /* 0x85 */ slGpioPinH5,
  /* 0x86 */ slGpioPinH6,
  /* 0x87 */ slGpioPinH7,
  /* 0x88 */ slGpioPinH8,
  /* 0x89 */ slGpioPinH9,
  /* 0x8A */ slGpioPinH10,
  /* 0x8B */ slGpioPinH11,
  /* 0x8C */ slGpioPinH12,
  /* 0x8D */ slGpioPinH13,
  /* 0x8E */ slGpioPinH14,
  /* 0x8F */ slGpioPinH15,

  /* >> PORT K */
  /* 0x90 */ slGpioPinK0,
  /* 0x91 */ slGpioPinK1,
  /* 0x92 */ slGpioPinK2,
  /* 0x93 */ slGpioPinK3,
  /* 0x94 */ slGpioPinK4,
  /* 0x95 */ slGpioPinK5,
  /* 0x96 */ slGpioPinK6,
  /* 0x97 */ slGpioPinK7,
  /* 0x98 */ slGpioPinK8,
  /* 0x99 */ slGpioPinK9,
  /* 0x9A */ slGpioPinK10,
  /* 0x9B */ slGpioPinK11,
  /* 0x9C */ slGpioPinK12,
  /* 0x9D */ slGpioPinK13,
  /* 0x9E */ slGpioPinK14,
  /* 0x9F */ slGpioPinK15,

  /* >> PORT A */
  /* 0xA0 */ slGpioPinA0,
  /* 0xA1 */ slGpioPinA1,
  /* 0xA2 */ slGpioPinA2,
  /* 0xA3 */ slGpioPinA3,
  /* 0xA4 */ slGpioPinA4,
  /* 0xA5 */ slGpioPinA5,
  /* 0xA6 */ slGpioPinA6,
  /* 0xA7 */ slGpioPinA7,
  /* 0xA8 */ slGpioPinA8,
  /* 0xA9 */ slGpioPinA9,
  /* 0xAA */ slGpioPinA10,
  /* 0xAB */ slGpioPinA11,
  /* 0xAC */ slGpioPinA12,
  /* 0xAD */ slGpioPinA13,
  /* 0xAE */ slGpioPinA14,
  /* 0xAF */ slGpioPinA15,

  /* >> PORT B */
  /* 0xB0 */ slGpioPinB0,
  /* 0xB1 */ slGpioPinB1,
  /* 0xB2 */ slGpioPinB2,
  /* 0xB3 */ slGpioPinB3,
  /* 0xB4 */ slGpioPinB4,
  /* 0xB5 */ slGpioPinB5,
  /* 0xB6 */ slGpioPinB6,
  /* 0xB7 */ slGpioPinB7,
  /* 0xB8 */ slGpioPinB8,
  /* 0xB9 */ slGpioPinB9,
  /* 0xBA */ slGpioPinB10,
  /* 0xBB */ slGpioPinB11,
  /* 0xBC */ slGpioPinB12,
  /* 0xBD */ slGpioPinB13,
  /* 0xBE */ slGpioPinB14,
  /* 0xBF */ slGpioPinB15,

  /* >> PORT C */
  /* 0xC0 */ slGpioPinC0,
  /* 0xC1 */ slGpioPinC1,
  /* 0xC2 */ slGpioPinC2,
  /* 0xC3 */ slGpioPinC3,
  /* 0xC4 */ slGpioPinC4,
  /* 0xC5 */ slGpioPinC5,
  /* 0xC6 */ slGpioPinC6,
  /* 0xC7 */ slGpioPinC7,
  /* 0xC8 */ slGpioPinC8,
  /* 0xC9 */ slGpioPinC9,
  /* 0xCA */ slGpioPinC10,
  /* 0xCB */ slGpioPinC11,
  /* 0xCC */ slGpioPinC12,
  /* 0xCD */ slGpioPinC13,
  /* 0xCE */ slGpioPinC14,
  /* 0xCF */ slGpioPinC15,

  /* >> PORT D */
  /* 0xD0 */ slGpioPinD0,
  /* 0xD1 */ slGpioPinD1,
  /* 0xD2 */ slGpioPinD2,
  /* 0xD3 */ slGpioPinD3,
  /* 0xD4 */ slGpioPinD4,
  /* 0xD5 */ slGpioPinD5,
  /* 0xD6 */ slGpioPinD6,
  /* 0xD7 */ slGpioPinD7,
  /* 0xD8 */ slGpioPinD8,
  /* 0xD9 */ slGpioPinD9,
  /* 0xDA */ slGpioPinD10,
  /* 0xDB */ slGpioPinD11,
  /* 0xDC */ slGpioPinD12,
  /* 0xDD */ slGpioPinD13,
  /* 0xDE */ slGpioPinD14,
  /* 0xDF */ slGpioPinD15,

  /* >> PORT E */
  /* 0xE0 */ slGpioPinE0,
  /* 0xE1 */ slGpioPinE1,
  /* 0xE2 */ slGpioPinE2,
  /* 0xE3 */ slGpioPinE3,
  /* 0xE4 */ slGpioPinE4,
  /* 0xE5 */ slGpioPinE5,
  /* 0xE6 */ slGpioPinE6,
  /* 0xE7 */ slGpioPinE7,
  /* 0xE8 */ slGpioPinE8,
  /* 0xE9 */ slGpioPinE9,
  /* 0xEA */ slGpioPinE10,
  /* 0xEB */ slGpioPinE11,
  /* 0xEC */ slGpioPinE12,
  /* 0xED */ slGpioPinE13,
  /* 0xEE */ slGpioPinE14,
  /* 0xEF */ slGpioPinE15,

  /* >> PORT F */
  /* 0xF0 */ slGpioPinF0,
  /* 0xF1 */ slGpioPinF1,
  /* 0xF2 */ slGpioPinF2,
  /* 0xF3 */ slGpioPinF3,
  /* 0xF4 */ slGpioPinF4,
  /* 0xF5 */ slGpioPinF5,
  /* 0xF6 */ slGpioPinF6,
  /* 0xF7 */ slGpioPinF7,
  /* 0xF8 */ slGpioPinF8,
  /* 0xF9 */ slGpioPinF9,
  /* 0xFA */ slGpioPinF10,
  /* 0xFB */ slGpioPinF11,
  /* 0xFC */ slGpioPinF12,
  /* 0xFD */ slGpioPinF13,
  /* 0xFE */ slGpioPinF14,
  /* 0xFF */ slGpioPinF15,
} slPin;

/*******************************************************************************
 *                           FUNCTION PROTOTYPES
 ******************************************************************************/

/* pin/port conversion functions */
GPIO_Port_TypeDef slGetPort(slPin pin);
uint32_t slGetPin(slPin pin);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_GPIO_DRV_H_ */
