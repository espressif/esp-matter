/***************************************************************************//**
 * @file
 * @brief EMSTATUS definitions.
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

#ifndef _EMSTATUS_H_
#define _EMSTATUS_H_

#include <stdint.h>
#include "em_types.h"          /* typedef uint32_t EMSTATUS; */

/** Common EMSTATUS codes: */
#define EMSTATUS_OK                   (0)            /**< Operation successful. */

/** EMSTATUS base codes for display modules. */
#define DISPLAY_EMSTATUS_BASE         (0x8011UL << 16) /**< EMSTATUS base code of DISPLAY driver interface. */
#define TEXTDISPLAY_EMSTATUS_BASE     (0x8012UL << 16) /**< EMSTATUS base code of TEXTDISPLAY module. */
#define PAL_EMSTATUS_BASE             (0x8013UL << 16) /**< EMSTATUS base code of PAL interface. */

#endif /*  _EMSTATUS_H_  */
