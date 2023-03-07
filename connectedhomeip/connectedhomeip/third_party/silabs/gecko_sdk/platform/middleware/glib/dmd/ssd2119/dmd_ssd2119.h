/***************************************************************************//**
 * @file
 * @brief Dot matrix display driver for LCD controller SSD2119
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

#ifndef __DMD_SSD2119_H
#define __DMD_SSD2119_H

#include "dmd/dmd.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Frame update frequency of display */
#define DMD_FRAME_FREQUENCY    80
/** Horizontal size of the display */
#define DMD_HORIZONTAL_SIZE    320
/** Vertical size of the display */
#define DMD_VERTICAL_SIZE      240

EMSTATUS DMDIF_init(uint32_t cmdRegAddr, uint32_t dataRegAddr);

/** @endcond */

#ifdef __cplusplus
}
#endif

#endif
