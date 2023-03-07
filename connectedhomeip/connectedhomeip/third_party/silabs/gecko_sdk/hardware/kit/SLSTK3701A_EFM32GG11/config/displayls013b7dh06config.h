/***************************************************************************//**
 * @file
 * @brief SLSTK3701A specific configuration for the display driver for
 *        the Sharp Memory LCD model LS013B7DH06.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef __SILICON_LABS_DISPLAYLS013B7DH06CONFIG_H__
#define __SILICON_LABS_DISPLAYLS013B7DH06CONFIG_H__

#include "displayconfigapp.h"
#include "em_gpio.h"

/* Display device name. */
#define SHARP_MEMLCD_DEVICE_NAME   "Sharp LS013B7DH06 #1"

/* LCD and SPI GPIO pin connections on the SLSTK3701A kit. */
#define LCD_PORT_SCLK             (gpioPortC)  /* EFM_DISP_SCLK on PC15 */
#define LCD_PIN_SCLK              (15)
#define LCD_PORT_SI               (gpioPortA)  /* EFM_DISP_MOSI on PA14 */
#define LCD_PIN_SI                (14)
#define LCD_PORT_SCS              (gpioPortC)  /* EFM_DISP_CS on PC14 */
#define LCD_PIN_SCS               (14)
#define LCD_PORT_EXTCOMIN         (gpioPortA)  /* EFM_DISP_COM on PA11 */
#define LCD_PIN_EXTCOMIN          (11)
#define LCD_PORT_DISP_SEL         (gpioPortA)  /* EFM_DISP_ENABLE on PA9 */
#define LCD_PIN_DISP_SEL          (9)

/* PRS settings for polarity inversion extcomin auto toggle.  */
#define LCD_AUTO_TOGGLE_PRS_CH    (11)  /* PRS channel 11.      */
#define LCD_AUTO_TOGGLE_PRS_ROUTELOC()  PRS->ROUTELOC2 = \
  ((PRS->ROUTELOC2 & ~_PRS_ROUTELOC2_CH11LOC_MASK) | PRS_ROUTELOC2_CH11LOC_LOC0)
#define LCD_AUTO_TOGGLE_PRS_ROUTEPEN    PRS_ROUTEPEN_CH11PEN

/*
 * Select how LCD polarity inversion should be handled:
 *
 * If POLARITY_INVERSION_EXTCOMIN is defined,
 * and the polarity inversion is armed for every rising edge of the EXTCOMIN
 * pin. The actual polarity inversion is triggered at the next transision of
 * SCS. This mode is recommended because it causes less CPU and SPI load than
 * the alternative mode, see below.
 * If POLARITY_INVERSION_EXTCOMIN is undefined,
 * the polarity inversion is toggled by sending an SPI command. This mode
 * causes more CPU and SPI load than using the EXTCOMIN pin mode.
 */
#define POLARITY_INVERSION_EXTCOMIN

/* Define POLARITY_INVERSION_EXTCOMIN_PAL_AUTO_TOGGLE if you want the PAL
 * (Platform Abstraction Layer interface) to automatically toggle the EXTCOMIN
 *  pin.
 * If the PAL_TIMER_REPEAT function is defined the EXTCOMIN toggling is handled
 * by a timer repeat system, therefore we must undefine
 * POLARITY_INVERSION_EXTCOMIN_PAL_AUTO_TOGGLE;
 */
#ifndef PAL_TIMER_REPEAT_FUNCTION
  #define POLARITY_INVERSION_EXTCOMIN_PAL_AUTO_TOGGLE
#endif

#endif /* __SILICON_LABS_DISPLAYLS013B7DH06CONFIG_H__ */
