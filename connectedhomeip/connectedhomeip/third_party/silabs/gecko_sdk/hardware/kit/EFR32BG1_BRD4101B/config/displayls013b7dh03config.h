/***************************************************************************//**
 * @file
 * @brief BRD4101B specific configuration for the display driver for
 *        the Sharp Memory LCD model LS013B7DH03.
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

#ifndef DISPLAY_LS013B7DH03_CONFIG_H
#define DISPLAY_LS013B7DH03_CONFIG_H

#include "displayconfigapp.h"

/* Display device name. */
#define SHARP_MEMLCD_DEVICE_NAME   "Sharp LS013B7DH03 #1"

/* LCD and SPI GPIO pin connections on the BRD4101B. */
#define LCD_PORT_SCLK             (gpioPortB)  /* EFM_DISP_SCLK on PB11 */
#define LCD_PIN_SCLK              (11)
#define LCD_PORT_SI               (gpioPortA)  /* EFM_DISP_MOSI on PA0 */
#define LCD_PIN_SI                (0)
#define LCD_PORT_SCS              (gpioPortC)  /* EFM_DISP_CS on PC6 */
#define LCD_PIN_SCS               (6)

/* The EFM8 I/O-expander will take care of display EXTCOMIN toggling. */
/* The following two defines will achieve this.                       */
#define POLARITY_INVERSION_EXTCOMIN_PAL_AUTO_TOGGLE
#define POLARITY_INVERSION_EXTCOMIN_MANUAL

#endif /* DISPLAY_LS013B7DH03_CONFIG_H */
