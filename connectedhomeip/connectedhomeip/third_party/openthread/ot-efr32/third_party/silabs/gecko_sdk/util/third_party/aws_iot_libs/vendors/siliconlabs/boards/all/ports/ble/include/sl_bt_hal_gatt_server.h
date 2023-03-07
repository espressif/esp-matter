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

#ifndef SL_BT_HAL_GATT_SERVER_H
#define SL_BT_HAL_GATT_SERVER_H

/* Standard library includes */
#include <stddef.h>

/* FreeRTOS BLE HAL includes */
#include "bt_hal_manager_adapter_ble.h"

/* Silicon Labs includes */
#include "sl_bt_hal_config.h"
#include "sl_bt_api.h"

/**
 * @brief Get the GATT server interface.
 *
 * This function implements ppvGetGattServerInterface function in @ref BTBleAdapter_t.
 */
const void * prvGetGattServerInterface( );

/**
 * @brief Invoked on Bluetooth event to handle events the GATT Server needs
 *
 * @param[in] evt The Bluetooth event
 */
void prvGattServerOnSlBtEvent(sl_bt_msg_t* evt);

/**
 * @brief Invoked by Adapter BLE to handle opened connection event by GATT Server
 *
 * @param[in] ucConnectionHandle Connection handle
 *
 * @param[in] address Pointer to BTBdaddr_t structure
 */
void prvGattServerOpenConnectionEventCb( uint8_t ucConnectionHandle, BTBdaddr_t * pBTAddress );

/**
 * @brief Invoked by Adapter BLE to handle closed connection event by GATT Server
 *
 * @param[in] ucConnectionHandle Connection handle
 *
 * @param[in] address Pointer to BTBdaddr_t structure
 */
void prvGattServerCloseConnectionEventCb( uint8_t ucConnectionHandle, BTBdaddr_t * pBTAddress );

#endif /* SL_BT_HAL_GATT_SERVER_H */
