/***************************************************************************//**
 * @file    iot_efuse_drv.h
 * @brief   eFUSE Driver header file
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

#ifndef _IOT_EFUSE_DRV_H_
#define _IOT_EFUSE_DRV_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "iot_efuse_drv_nvm.h"
#include "iot_efuse_drv_ram.h"
#include "iot_efuse_drv_tok.h"
#include "iot_efuse_cfg.h"

/*******************************************************************************
 *                               MACROS
 ******************************************************************************/

/* Token Manager Driver */
#if (IOT_EFUSE_CFG_DRIVER == IOT_EFUSE_DRV_TOK_DRIVER_ID)
#define IOT_EFUSE_DRV_API(func_name)  (iot_efuse_drv_tok_##func_name)
#endif

/* NVM3 Driver */
#if (IOT_EFUSE_CFG_DRIVER == IOT_EFUSE_DRV_NVM_DRIVER_ID)
#define IOT_EFUSE_DRV_API(func_name)  (iot_efuse_drv_nvm_##func_name)
#endif

/* RAM Driver */
#if (IOT_EFUSE_CFG_DRIVER == IOT_EFUSE_DRV_RAM_DRIVER_ID)
#define IOT_EFUSE_DRV_API(func_name)  (iot_efuse_drv_ram_##func_name)
#endif

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_EFUSE_DRV_H_ */
