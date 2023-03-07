/***************************************************************************//**
 * @file    iot_efuse_drv_nvm.h
 * @brief   eFUSE Driver Using NVM3 Storage header file
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

#ifndef _IOT_EFUSE_DRV_NVM_H_
#define _IOT_EFUSE_DRV_NVM_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "em_core.h"
#include "em_device.h"

#include "sl_status.h"

/*******************************************************************************
 *                                MACROS
 ******************************************************************************/

#define IOT_EFUSE_DRV_NVM_DRIVER_ID    1

/*******************************************************************************
 *                               PROTOTYPES
 ******************************************************************************/

/* driver control */
sl_status_t iot_efuse_drv_nvm_driver_init();
sl_status_t iot_efuse_drv_nvm_driver_deinit();

/* access function */
sl_status_t iot_efuse_drv_nvm_byte_write(uint32_t ulAddr, uint8_t *pucData);
sl_status_t iot_efuse_drv_nvm_byte_read(uint32_t ulAddr, uint8_t *pucData);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_EFUSE_DRV_NVM_H_ */
