/***************************************************************************//**
 * @file
 * @brief Dot matrix display driver for DISPLAY device driver interface.
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

#ifndef __DMD_ETM043010EDH6_H__
#define __DMD_ETM043010EDH6_H__

#include "emstatus.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DMD_HORIZONTAL_SIZE         (480)
#define DMD_VERTICAL_SIZE           (272)
#define DMD_DISPLAY_BACKLIGHT_PORT  (gpioPortI)
#define DMD_DISPLAY_BACKLIGHT_PIN   (6)
#define DMD_DISPLAY_ENABLE_PORT     (gpioPortI)
#define DMD_DISPLAY_ENABLE_PIN      (1)

EMSTATUS DMD_startDrawing(void);
EMSTATUS DMD_stopDrawing(void);

#ifdef __cplusplus
}
#endif

#endif /* __DMD_ETM043010EDH6_H__ */
