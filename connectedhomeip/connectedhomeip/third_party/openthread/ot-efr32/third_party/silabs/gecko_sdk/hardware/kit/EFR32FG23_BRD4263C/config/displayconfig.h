/***************************************************************************//**
 * @file
 * @brief Configuration file for DISPLAY device driver interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef DISPLAYCONFIG_H
#define DISPLAYCONFIG_H

/* Include the application specific configuration file. */
#include "displayconfigapp.h"

/* Include support for the SHARP Memory LCD model LS013B7DH03 on the WSTK */
#define INCLUDE_DISPLAY_SHARP_LS013B7DH03

#include "displayls013b7dh03config.h"
#include "displayls013b7dh03.h"

/**
 * Maximum number of display devices the display module is configured
 * to support. This number may be increased if the system includes more than
 * one display device. However, the number should be kept low in order to
 * save memory.
 */
#define DISPLAY_DEVICES_MAX   (1)

/**
 * Geometry of display device #0 in the system (i.e. ls013b7dh03 on the WSTK)
 * These defines can be used to declare static framebuffers in order to save
 * extra memory consumed by malloc.
 */
#define DISPLAY0_WIDTH    (LS013B7DH03_WIDTH)
#define DISPLAY0_HEIGHT   (LS013B7DH03_HEIGHT)

/**
 * Define all display device driver initialization functions here.
 */
#define DISPLAY_DEVICE_DRIVER_INIT_FUNCTIONS \
  {                                          \
    DISPLAY_Ls013b7dh03Init,                 \
    NULL                                     \
  }

#endif /* DISPLAYCONFIG_H */
