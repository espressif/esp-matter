/***************************************************************************//**
 * @file    iot_i2c_drv.h
 * @brief   I2C driver header file
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

#ifndef _IOT_I2C_DRV_H_
#define _IOT_I2C_DRV_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

/* I2C driver init */
sl_status_t iot_i2c_drv_driver_init(void *pvHndl);
sl_status_t iot_i2c_drv_driver_deinit(void *pvHndl);

/* I2C set/get config */
sl_status_t iot_i2c_drv_config_set(void *pvHndl,
                                      uint32_t ulTimeout,
                                      uint32_t ulFreq);
sl_status_t iot_i2c_drv_config_get(void *pvHndl,
                                      uint32_t *pulTimeout,
                                      uint32_t *pulFreq);

/* I2C h/w control */
sl_status_t iot_i2c_drv_hw_enable(void *pvHndl);
sl_status_t iot_i2c_drv_hw_disable(void *pvHndl);

/* I2C transfer */
sl_status_t iot_i2c_drv_transfer_start(void *pvHndl,
                                          uint16_t usAddress,
                                          uint8_t ucDirection,
                                          uint8_t *pucBuffer,
                                          uint32_t ulCount);
sl_status_t iot_i2c_drv_transfer_step(void *pvHndl);
sl_status_t iot_i2c_drv_transfer_finish(void *pvHndl);
sl_status_t iot_i2c_drv_transfer_status(void *pvHndl);

/* I2C IRQ control */
sl_status_t iot_i2c_drv_irq_enable(void *pvHndl);
sl_status_t iot_i2c_drv_irq_disable(void *pvHndl);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_I2C_DRV_H_ */
