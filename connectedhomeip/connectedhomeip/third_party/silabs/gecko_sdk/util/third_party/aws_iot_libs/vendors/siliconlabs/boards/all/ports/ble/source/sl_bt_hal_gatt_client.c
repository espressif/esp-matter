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

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

/* Standard library includes */
#include <stddef.h>

/* FreeRTOS BLE HAL includes */
#include "bt_hal_gatt_client.h"
#include "sl_bt_hal_common.h"

/**
 * @brief Structure to collect the data of one GATT client
 */
typedef struct
{
  SlBtBleHalClient_t xClient; /**< Client UUID and handle */
} SlBtGattClientIf_t;

/**
 * @brief Head of the list of gattClientIf contexts
 */
static SlBtGattClientIf_t * pxGattClientIfs = NULL;

/** @brief Callbacks registered in the call to @ref prvGattClientInit */
static const BTGattClientCallbacks_t * pxGattClientCallbacks = NULL;

/**
 * @brief Get the gattClientIf context for the specified @p ucGattClientIf
 *
 * @param[in] ucGattClientIf The GATT client interface handle
 *
 * @return Pointer to the gattClientIf context, or NULL if ucGattClientIf was invalid
 */
static inline SlBtGattClientIf_t * prvGetGattClientIf( uint8_t ucGattClientIf )
{
  SlBtBleHalClient_t * pxClient =
    prvGetBleHalClientByHandle( ucGattClientIf, ( SlBtBleHalClient_t * ) pxGattClientIfs );
  return ( SlBtGattClientIf_t * ) pxClient;
}

/**
 * @brief Registers a GATT client application with the stack.
 *
 * This function implements pxRegisterClient function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvRegisterClient( BTUuid_t * pxUuid )
{
  /* Register using the common mechanism */
  SlBtGattClientIf_t * pxGattClientIf = NULL;
  BTStatus_t status = prvRegisterBleHalClient( pxUuid,
                                               ( SlBtBleHalClient_t ** ) &pxGattClientIfs,
                                               SL_BT_HAL_MAX_GATT_CLIENTS,
                                               sizeof(*pxGattClientIfs),
                                               ( SlBtBleHalClient_t ** ) &pxGattClientIf );
  if( status != eBTStatusSuccess )
  {
    return status;
  }

  /* Call the application */
  if( pxGattClientCallbacks && pxGattClientCallbacks->pxRegisterClientCb )
  {
    pxGattClientCallbacks->pxRegisterClientCb( eBTGattStatusSuccess,
                                               pxGattClientIf->xClient.ucHandle,
                                               pxUuid );
  }

  return eBTStatusSuccess;
}

/**
 * @brief  Unregister a client application from the stack.
 *
 * This function implements pxUnregisterClient function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvUnregisterClient( uint8_t ucClientIf )
{
  /* Get the GATT client context */
  SlBtGattClientIf_t * pxGattClientIf = prvGetGattClientIf( ucClientIf );
  if( !pxGattClientIf )
  {
    return eBTStatusParamInvalid;
  }

  /* Free the context */
  prvFreeBleHalClient( ( SlBtBleHalClient_t ** ) &pxGattClientIfs,
                       ( SlBtBleHalClient_t * ) pxGattClientIf );

  return eBTStatusSuccess;
}

/**
 * @brief Initializes the interface and provides callback routines.
 *
 * This function implements pxGattClientInit function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvGattClientInit( const BTGattClientCallbacks_t * pxCallbacks )
{
  while(pxGattClientIfs != NULL )
  {
    prvFreeBleHalClient( ( SlBtBleHalClient_t ** ) &pxGattClientIfs,
                       ( SlBtBleHalClient_t * ) pxGattClientIfs );
  }

  BTStatus_t status = eBTStatusParamInvalid;
  if( pxCallbacks )
  {
    pxGattClientCallbacks = pxCallbacks;
    status = eBTStatusSuccess;
  }

  return status;
}

/**
 * @brief Create a connection to a remote LE or dual-mode device.
 *
 * This function implements pxConnect function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvConnect( uint8_t ucClientIf,
                              const BTBdaddr_t * pxBdAddr,
                              bool bIsDirect,
                              BTTransport_t xTransport )
{
  /* This function is not supported */
  (void) ucClientIf;
  (void) pxBdAddr;
  (void) bIsDirect;
  (void) xTransport;

  return eBTStatusUnsupported;
}

/**
 * @brief Disconnect a remote device or cancel a pending connection.
 *
 * This function implements pxDisconnect function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvDisconnect( uint8_t ucClientIf,
                                 const BTBdaddr_t * pxBdAddr,
                                 uint16_t usConnId )
{
  /* This function is not supported */
  (void) ucClientIf;
  (void) pxBdAddr;
  (void) usConnId;

  return eBTStatusUnsupported;
}

/**
 * @brief Clear the attribute cache for a given device.
 *
 * This function implements pxRefresh function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvRefresh( uint8_t ucClientIf,
                              const BTBdaddr_t * pxBdAddr )
{
  /* This function is not supported */
  (void) ucClientIf;
  (void) pxBdAddr;

  return eBTStatusUnsupported;
}

/**
 * @brief Enumerate all GATT services on a connected device.
 *
 * This function implements pxSearchService function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvSearchService( uint16_t usConnId,
                                    BTUuid_t * pxFilterUuid )
{
  /* This function is not supported */
  (void) usConnId;
  (void) pxFilterUuid;

  return eBTStatusUnsupported;
}

/**
 * @brief Read a characteristic on a remote device.
 *
 * This function implements pxReadCharacteristic function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvReadCharacteristic( uint16_t usConnId,
                                         uint16_t usHandle,
                                         uint32_t ulAuthReq )
{
  /* This function is not supported */
  (void) usConnId;
  (void) usHandle;
  (void) ulAuthReq;

  return eBTStatusUnsupported;
}

/**
 * @brief Write a characteristic on a remote device.
 *
 * This function implements pxWriteCharacteristic function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvWriteCharacteristic( uint16_t usConnId,
                                          uint16_t usHandle,
                                          BTAttrWriteRequests_t xWriteType,
                                          size_t xLen,
                                          uint32_t ulAuthReq,
                                          char * pcValue )
{
  /* This function is not supported */
  (void) usConnId;
  (void) usHandle;
  (void) xWriteType;
  (void) xLen;
  (void) ulAuthReq;
  (void) pcValue;

  return eBTStatusUnsupported;
}

/**
 * @brief Read the descriptor for a given characteristic on a remote.
 *
 * This function implements pxReadDescriptor function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvReadDescriptor( uint16_t usConnId,
                                     uint16_t usHandle,
                                     uint32_t ulAuthReq )
{
  /* This function is not supported */
  (void) usConnId;
  (void) usHandle;
  (void) ulAuthReq;

  return eBTStatusUnsupported;
}

/**
 * @brief Write a remote descriptor for a given characteristic.
 *
 * This function implements pxWriteDescriptor function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvWriteDescriptor( uint16_t usConnId,
                                      uint16_t usHandle,
                                      BTAttrWriteRequests_t xWriteType,
                                      size_t xLen,
                                      uint32_t ulAuthReq,
                                      char * pcValue )
{
  /* This function is not supported */
  (void) usConnId;
  (void) usHandle;
  (void) xWriteType;
  (void) xLen;
  (void) ulAuthReq;
  (void) pcValue;

  return eBTStatusUnsupported;
}

/**
 * @brief Execute (or abort) a prepared write operation.
 *
 * This function implements pxExecuteWrite function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvExecuteWrite( uint16_t usConnId,
                                   bool bExecute )
{
  /* This function is not supported */
  (void) usConnId;
  (void) bExecute;

  return eBTStatusUnsupported;
}

/**
 * @brief Register to receive notifications or indications.
 *
 * This function implements pxRegisterForNotification function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvRegisterForNotification( uint8_t ucClientIf,
                                              const BTBdaddr_t * pxBdAddr,
                                              uint16_t usHandle )
{
  /* This function is not supported */
  (void) ucClientIf;
  (void) pxBdAddr;
  (void) usHandle;

  return eBTStatusUnsupported;
}

/**
 * @brief Deregister a previous request for notifications/indications.
 *
 * This function implements pxUnregisterForNotification function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvUnregisterForNotification( uint8_t ucClientIf,
                                                const BTBdaddr_t * pxBdAddr,
                                                uint16_t usHandle )
{
  /* This function is not supported */
  (void) ucClientIf;
  (void) pxBdAddr;
  (void) usHandle;

  return eBTStatusUnsupported;
}

/**
 * @brief Request RSSI for a given remote device.
 *
 * This function implements pxReadRemoteRssi function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvReadRemoteRssi( uint8_t ucClientIf,
                                     const BTBdaddr_t * pxBdAddr )
{
  /* This function is not supported */
  (void) ucClientIf;
  (void) pxBdAddr;

  return eBTStatusUnsupported;
}

/**
 * @brief Determine the type of the remote device (LE, BR/EDR, Dual-mode).
 *
 * This function implements pxGetDeviceType function in @ref BTGattClientInterface_t.
 */
static BTTransport_t prvGetDeviceType( const BTBdaddr_t * pxBdAddr )
{
  /* This function is not supported */
  (void) pxBdAddr;

  return BTTransportLe;
}

/**
 * @brief Configure the MTU for a given connection.
 *
 * This function implements pxConfigureMtu function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvConfigureMtu( uint16_t usConnId,
                                   uint16_t usMtu )
{
  /* This function is not supported */
  (void) usConnId;
  (void) usMtu;

  return eBTStatusUnsupported;
}

/**
 * @brief Test mode interface.
 *
 * This function implements pxTestCommand function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvTestCommand( uint32_t ulCommand,
                                  BTGattTestParams_t * pxParams )
{
  /* This function is not supported */
  (void) ulCommand;
  (void) pxParams;

  return eBTStatusUnsupported;
}

/**
 * @brief Get gatt db content.
 *
 * This function implements pxGetGattDb function in @ref BTGattClientInterface_t.
 */
static BTStatus_t prvGetGattDb( uint16_t usConnId )
{
  /* This function is not supported */
  (void) usConnId;

  return eBTStatusUnsupported;
}

/**
 * @brief Silicon Labs GATT Client interface functions.
 */
static const BTGattClientInterface_t xSilabsBTGattClientInterface =
{
  .pxRegisterClient = prvRegisterClient,
  .pxUnregisterClient = prvUnregisterClient,
  .pxGattClientInit = prvGattClientInit,
  .pxConnect = prvConnect,
  .pxDisconnect = prvDisconnect,
  .pxRefresh = prvRefresh,
  .pxSearchService = prvSearchService,
  .pxReadCharacteristic = prvReadCharacteristic,
  .pxWriteCharacteristic = prvWriteCharacteristic,
  .pxReadDescriptor = prvReadDescriptor,
  .pxWriteDescriptor = prvWriteDescriptor,
  .pxExecuteWrite = prvExecuteWrite,
  .pxRegisterForNotification = prvRegisterForNotification,
  .pxUnregisterForNotification = prvUnregisterForNotification,
  .pxReadRemoteRssi = prvReadRemoteRssi,
  .pxGetDeviceType = prvGetDeviceType,
  .pxConfigureMtu = prvConfigureMtu,
  .pxTestCommand = prvTestCommand,
  .pxGetGattDb = prvGetGattDb
};

/* Get the GATT client interface. */
const void * prvGetGattClientInterface( )
{
  return &xSilabsBTGattClientInterface;
}
