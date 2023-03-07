/***************************************************************************//**
 * @brief Connect Poll component configuration header.
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Connect Poll configuration

// <o EMBER_AF_PLUGIN_POLL_SHORT_POLL_INTERVAL_QS> Short Poll Interval in quarter seconds<1-255>
// <i> Default: 4
// <i> The amount of time (in quarter seconds) the device will wait between MAC data polls when it is expecting data.
#define EMBER_AF_PLUGIN_POLL_SHORT_POLL_INTERVAL_QS        (4)

// <o EMBER_AF_PLUGIN_POLL_LONG_POLL_INTERVAL_S> Long Poll Interval in quarter seconds<1-65535>
// <i> Default: 600
// <i> The amount of time (in seconds) the device will wait between MAC data polls to ensure connectivity with its parent.
#define EMBER_AF_PLUGIN_POLL_LONG_POLL_INTERVAL_S          (600)

// </h>

// <<< end of configuration section >>>
