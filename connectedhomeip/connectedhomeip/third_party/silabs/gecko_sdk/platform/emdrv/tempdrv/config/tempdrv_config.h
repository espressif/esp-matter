/***************************************************************************//**
 * @file
 * @brief TEMPDRV configuration file.
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
#ifndef __SILICON_LABS_TEMPDRV_CONFIG_H__
#define __SILICON_LABS_TEMPDRV_CONFIG_H__

/***************************************************************************//**
 * @addtogroup tempdrv
 * @{
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Temperature Driver Configuration

#ifndef TEMPDRV_CUSTOM_CALLBACK_DEPTH
// <o TEMPDRV_CUSTOM_CALLBACK_DEPTH> Callback table depth (for high and low callbacks each)
// <i> Default: 5
#define TEMPDRV_CUSTOM_CALLBACK_DEPTH 5
#endif

#ifndef TEMPDRV_EM4WAKEUP
// <q TEMPDRV_EM4WAKEUP> Allow temperature sensor to wake the device up from EM4
// <i> Default: 0
#define TEMPDRV_EM4WAKEUP 0
#endif

#ifndef EMU_CUSTOM_IRQ_HANDLER
// <q EMU_CUSTOM_IRQ_HANDLER> Allow EMU_IRQ_Handler to be defined
// <i> elsewhere than in temperature driver.
// <i> Default: 0
#define EMU_CUSTOM_IRQ_HANDLER 0
#endif

// </h>

// <<< end of configuration section >>>

/** @} (end addtogroup tempdrv) */

#endif /* __SILICON_LABS_TEMPDRV_CONFIG_H__ */
