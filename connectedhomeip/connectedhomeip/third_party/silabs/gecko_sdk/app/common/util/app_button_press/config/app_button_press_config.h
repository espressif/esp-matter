/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_BUTTON_PRESS_CONFIG_H_
#define APP_BUTTON_PRESS_CONFIG_H_

// <<< Use Configuration Wizard in Context Menu >>>

// <o SHORT_BUTTON_PRESS_DURATION> Duration of Short button presses
// <i> Default: 250
// <i> Any button press shorter than this value will be considered SHORT_BUTTON_PRESS.
#define SHORT_BUTTON_PRESS_DURATION   (250)

// <o MEDIUM_BUTTON_PRESS_DURATION> Duration of MEDIUM button presses
// <i> Default: 1000
// <i> Any button press shorter than this value and longer SHORT_BUTTON_PRESS than will
// be considered MEDIUM_BUTTON_PRESS.
#define MEDIUM_BUTTON_PRESS_DURATION   (1000)

// <o LONG_BUTTON_PRESS_DURATION> Duration of LONG button presses
// <i> Default: 5000
// <i> Any button press shorter than this value and longer MEDIUM_BUTTON_PRESS than will
// be consideredLONG_BUTTON_PRESS. Any button press longer than this value will be
// considered VERY_LONG_BUTTON_PRESS
#define LONG_BUTTON_PRESS_DURATION   (5000)

// <<< end of configuration section >>>

#endif // APP_BUTTON_PRESS_CONFIG_H_
