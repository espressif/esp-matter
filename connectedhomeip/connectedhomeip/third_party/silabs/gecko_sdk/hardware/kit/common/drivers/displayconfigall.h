/***************************************************************************//**
 * @file
 * @brief Main configuration file for the DISPLAY driver software stack.
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

#ifndef __DISPLAYCONFIGALL_H
#define __DISPLAYCONFIGALL_H

#ifdef HAL_CONFIG
#include "displayhalconfig.h"
#else
/*
 * First, we list the default INCLUDE_XXX #defines which may be excluded later
 * by the kit or application specific configuration files.
 */
#define INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE

/* Then include the kit specific display configuration files which also includes
   the application specific configuration file and further selects which modules
   modules to include below. */

#include "displaypalconfig.h"
#include "displayconfig.h"
#endif

#ifdef INCLUDE_DISPLAY_SHARP_LS013B7DH03
#include "displayls013b7dh03config.h"
#endif

#ifdef INCLUDE_DISPLAY_SHARP_LS013B7DH06
#include "displayls013b7dh06config.h"
#endif

#ifdef INCLUDE_TEXTDISPLAY_SUPPORT
#include "textdisplayconfig.h"
#include "retargettextdisplayconfig.h"
#endif

#endif /* __DISPLAYCONFIGALL_H */
