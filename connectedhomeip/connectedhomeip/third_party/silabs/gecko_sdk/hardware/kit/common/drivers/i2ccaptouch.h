/***************************************************************************//**
 * @file
 * @brief I2C touch support on Touch Display
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

#ifndef I2CCAPTOUCH_H
#define I2CCAPTOUCH_H

#ifndef SL_SUPRESS_DEPRECATION_WARNINGS_SDK_3_1
#warning "The I2C touch driver is deprecated and marked for removal in a later release."
#endif

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                                                                            */
/* Error code definitions                                                     */
/*                                                                            */
/******************************************************************************/

#define CAPT_OK                          (0x00000000)
#define CAPT_ERROR_BUFFER_TOO_SMALL      (0x00000001)
#define CAPT_ERROR_I2C_ERROR             (0x00000002)
#define CAPT_ERROR_PARSE_ERROR           (0x00000003)

/******************************************************************************/
/*                                                                            */
/* Other definitions                                                          */
/*                                                                            */
/******************************************************************************/

#define CAPT_STATUS_NEW                  (0x0)
#define CAPT_STATUS_ACTIVE               (0x2)
#define CAPT_STATUS_INACTIVE             (0x1)

/******************************************************************************/
/*                                                                            */
/* Type definitions                                                           */
/*                                                                            */
/******************************************************************************/

typedef struct _CAPT_Touch {
  uint8_t id;
  uint8_t status;
  float   x;
  float   y;
} CAPT_Touch;

/******************************************************************************/
/*                                                                            */
/* Global function declarations                                               */
/*                                                                            */
/******************************************************************************/

uint32_t CAPT_init     (void);
uint32_t CAPT_enable   (bool enable, bool wait);
uint32_t CAPT_getTouches (CAPT_Touch *touchBuffer, uint8_t len, uint8_t *nTouches);

#endif /* I2CCAPTOUCH_H */
