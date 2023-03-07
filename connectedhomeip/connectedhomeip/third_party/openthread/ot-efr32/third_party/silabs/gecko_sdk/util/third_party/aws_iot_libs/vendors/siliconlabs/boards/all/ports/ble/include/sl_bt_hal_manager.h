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

#ifndef SL_BT_HAL_MANAGER_H
#define SL_BT_HAL_MANAGER_H

/* Standard library includes */
#include <stddef.h>

/* FreeRTOS BLE HAL includes */
#include "bt_hal_manager.h"

/* Silicon Labs includes */
#include "sl_bt_hal_config.h"
#include "sl_status.h"

/**
 * Enumeration listing the states of the Bluetooth stack
 */
typedef enum
{
  eSlBtHalBluetoothStopped  = 0, /**< Bluetooth stack is stopped */
  eSlBtHalBluetoothStarting = 1, /**< Bluetooth stack has been requested to start
                                      but has not completed yet */
  eSlBtHalBluetoothStarted  = 2, /**< Bluetooth stack has been successfully started  */
} SlBtHalState_t;

/**
 * @brief Structure to collect the properties of the device
 */
typedef struct
{
  uint8_t *      pucDeviceName;      /**< Device name. The memory is a heap allocation owned by this
                                          context and must be freed using sl_free() when not needed
                                          anymore. */
  size_t         xDeviceNameLen;     /**< Length of the device name */
  uint16_t       usMaxMtu;           /**< Max local ATT MTU configured to the Bluetooth stack */
  uint8_t        ucBondable;         /**< Bondable mode configured to the Bluetooth stack */
  uint8_t        ucSmConfigureFlags; /**< Flags configured in a call to sl_bt_sm_configure() */
  uint8_t        ucIoCapabilities;   /**< I/O capabilities configured to the Bluetooth stack */
  SlBtHalState_t eBtState;           /**< State of the Bluetooth stack */
} SlBtHalManager_t;

extern SlBtHalManager_t xSlBtHalManager;

/**
 * @brief Handle a new pairing event.
 *
 * @param[in] ucBondingHandle The bonding handle in Bluetooth stack
 * @param[in] pxAddress Pointer to the remote device address
 * @param[in] xSecurityLevel The security level of the pairing
 */
void prvBtHalManagerOnNewPairingEvent( uint8_t ucBondingHandle,
                                       BTBdaddr_t * pxAddress,
                                       BTSecurityLevel_t xSecurityLevel );

#endif /* SL_BT_HAL_MANAGER_H */
