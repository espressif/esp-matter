/***************************************************************************//**
 * @file    iot_tsensor_drv.h
 * @brief   Temp sensor driver header file
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

#ifndef _IOT_TSENSOR_DRV_H_
#define _IOT_TSENSOR_DRV_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

/*******************************************************************************
 *                               MACROS
 ******************************************************************************/

/* API and DEF macros */
#define IOT_TSENSOR_DRV_API(func_name)  (iot_tsensor_drv_##func_name)
#define IOT_TSENSOR_DRV_DEF(defn_name)  (IOT_TSENSOR_DRV_##defn_name)

/* number of instances (1) */
#define IOT_TSENSOR_DRV_COUNT           (1)

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_TSENSOR_DRV_H_ */
