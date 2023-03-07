/***************************************************************************//**
 * @file    iot_efuse_cfg.h
 * @brief   Common I/O eFUSE module configurations.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#ifndef _IOT_EFUSE_CFG_H_
#define _IOT_EFUSE_CFG_H_

/*******************************************************************************
 *                        eFUSE Default Configs
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h> eFUSE General Options

#ifndef IOT_EFUSE_CFG_DRIVER
// <o IOT_EFUSE_CFG_DRIVER> eFUSE driver to use
// <1=> NVM3 Driver
// <2=> RAM Driver
// <i> NVM3 driver implements eFUSE using NVM3 driver (on internal flash).
// <i> RAM driver simulates eFUSE using RAM buffer.
// <i> Default: 1
#define IOT_EFUSE_CFG_DRIVER             1
#endif

#ifndef IOT_EFUSE_CFG_MAX_SIZE
// <o  IOT_EFUSE_CFG_MAX_SIZE> eFUSE size in bytes
// <i> For NVM3 driver, this represents max. possible number of NVM3 objects.
// <i> For RAM driver, this represents the size of the RAM buffer.
// <i> Default: 256
#define  IOT_EFUSE_CFG_MAX_SIZE          256
#endif

// </h>

// <<< end of configuration section >>>

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_EFUSE_CFG_H_ */
