/***************************************************************************//**
 * @file
 * @brief capsense configuration parameters.
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

#ifndef __CAPSENSCONFIG_H
#define __CAPSENSCONFIG_H
#ifdef __cplusplus
extern "C" {
#endif

#define ACMP_CAPSENSE                           ACMP1
#define ACMP_CAPSENSE_CLKEN                     CMU_HFPERCLKEN0_ACMP1
#define PRS_CH_CTRL_SOURCESEL_ACMP_CAPSENSE     PRS_CH_CTRL_SOURCESEL_ACMP1
#define PRS_CH_CTRL_SIGSEL_ACMPOUT_CAPSENSE     PRS_CH_CTRL_SIGSEL_ACMP1OUT

#define ACMP_CHANNELS           8             /**< Number of channels for the Analog Comparator */

#define BUTTON_CHANNEL          5             /**< Button channel */

#define SLIDER_PART0_CHANNEL    0             /**< Touch slider channel Part 0 */
#define SLIDER_PART1_CHANNEL    1             /**< Touch slider channel Part 1 */
#define SLIDER_PART2_CHANNEL    2             /**< Touch slider channel Part 2 */
#define SLIDER_PART3_CHANNEL    3             /**< Touch slider channel Part 3 */

#define CAPSENSE_CH_IN_USE  { true, true, true, true, false, true, false, false }

#ifdef __cplusplus
}
#endif
#endif
