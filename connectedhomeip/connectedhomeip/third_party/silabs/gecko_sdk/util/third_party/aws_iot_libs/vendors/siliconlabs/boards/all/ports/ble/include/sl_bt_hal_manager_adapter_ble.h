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

#ifndef SL_BT_HAL_MANAGER_ADAPTER_BLE_H
#define SL_BT_HAL_MANAGER_ADAPTER_BLE_H

/* Standard library includes */
#include <stddef.h>

/* FreeRTOS BLE HAL includes */
#include "bt_hal_manager_adapter_ble.h"

/* Silicon Labs includes */
#include "sl_bt_hal_config.h"
#include "sl_bt_api.h"

/**
 * @brief Retrieves the HAL LE interface.
 *
 * This function implements pxGetLeAdapter function in @ref BTInterface_t.
 */
const void * prvGetLeAdapter( );

/**
 * @brief Get pointer of BTBdaddr_t of the connection handle.
 *
 * @param[in] ucConnectionHandle The connection handle
 *
 * @return Pointer of BTBdaddr_t structure which stores the remote client address
 */
BTBdaddr_t * prvGetConnectionAddress( uint8_t ucConnectionHandle );

/**
 * @brief Read the address of a bonded device, if any, from non-volatile memory
 *
 * @param[in] ucBondingHandle The bonding handle
 * @param[out] pxBdAddr The Bluetooth address in the original connection, if found
 *
 * @return eBTStatusSuccess on success, otherwise an error code
 */
BTStatus_t prvLoadBondedDeviceAddress( uint8_t ucBondingHandle,
                                       BTBdaddr_t * pxBdAddr );

/**
 * @brief Get the connection handle by a Bluetooth device address.
 *
 * @param[in] pxRemoteAddress Pointer of BTBdaddr_t of the connection
 * @param[out] pucConnectionHandle The connection handle if the connection exists
 *
 * @return eBTStatusSuccess if the connection exists.
 */
BTStatus_t prvGetConnectionHandle( const BTBdaddr_t * pxRemoteAddress, uint8_t *pucConnectionHandle );

/**
 * @brief Map a BLE Adapter API power level index to SL BT API power value
 *
 * @param[in] ucTxPower BLE Adapter API power level index
 * @param[out] psSlBtPower Set to the corresponding SL BT API power value
 *
 * @return eBTStatusSuccess on success, otherwise an error code
 */
BTStatus_t prvTxPowerIndexToSlBtPower( uint8_t ucTxPower,
                                       int16_t * psSlBtPower );

/**
 * @brief Invoked on BTInterface_t's pxDisable() to cleanup adapter state.
 */
void prvBleAdapterOnPxDisable();

/**
 * @brief Invoked on Bluetooth event to handle events the BLE adapter needs.
 *
 * @param[in] evt The Bluetooth event
 */
void prvBleAdapterOnSlBtEvent(sl_bt_msg_t* evt);

#endif /* SL_BT_HAL_MANAGER_ADAPTER_BLE_H */
