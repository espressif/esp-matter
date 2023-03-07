/***************************************************************************//**
 * @file
 * @brief Silicon Labs implementation of FreeRTOS Bluetooth Low Energy library.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_BT_HAL_GATT_CLIENT_H
#define SL_BT_HAL_GATT_CLIENT_H

/* Standard library includes */
#include <stddef.h>

/* FreeRTOS BLE HAL includes */
#include "bt_hal_manager_adapter_ble.h"

/* Silicon Labs includes */
#include "sl_bt_hal_config.h"

/**
 * @brief Get the GATT client interface.
 *
 * This function implements ppvGetGattClientInterface function in @ref BTBleAdapter_t.
 */
const void * prvGetGattClientInterface( );

#endif /* SL_BT_HAL_GATT_CLIENT_H */
