/***************************************************************************//**
 * @file
 * @brief Configuration file for DISPLAY device driver interface.
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
#ifndef __SILICON_LABS_DISPLAYCONFIG_H__
#define __SILICON_LABS_DISPLAYCONFIG_H__

/* Include the application specific configuration file. */
#include "displayconfigapp.h"

/* Include support for the SHARP Memory LCD model LS013B7DH06 */
#define INCLUDE_DISPLAY_SHARP_LS013B7DH06

#include "displayls013b7dh06config.h"
#include "displayls013b7dh06.h"

/** Display color mode */
#define DISPLAY_COLOUR_MODE_IS_RGB_3BIT

/**
 * Maximum number of display devices the display module is configured
 * to support. This number may be increased if the system includes more than
 * one display device. However, the number should be kept low in order to
 * save memory.
 */
#define DISPLAY_DEVICES_MAX   (1)

/**
 * Geometry of display device #0 in the system. Display device #0 on this kit
 * is the SHARP Memory LCD LS013B7DH06 which has 128x128 pixels.
 * These defines can be used to declare static framebuffers in order to save
 * extra memory consumed by malloc.
 */
#define DISPLAY0_WIDTH          (LS013B7DH06_WIDTH)
#define DISPLAY0_HEIGHT         (LS013B7DH06_HEIGHT)
#define DISPLAY0_BITS_PER_PIXEL (LS013B7DH06_BITS_PER_PIXEL)

/**
 * Define all display device driver initialization functions here.
 */
#define DISPLAY_DEVICE_DRIVER_INIT_FUNCTIONS \
  {                                          \
    DISPLAY_Ls013b7dh06Init,                 \
    NULL                                     \
  }

#endif /* __SILICON_LABS_DISPLAYCONFIG_H__ */
