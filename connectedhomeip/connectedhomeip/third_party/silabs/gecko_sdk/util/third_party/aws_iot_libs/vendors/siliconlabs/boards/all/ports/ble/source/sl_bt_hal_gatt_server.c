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
#include "bt_hal_gatt_server.h"

/* Silicon Labs includes */
#include "sl_bt_hal_common.h"
#include "sl_bt_hal_manager_adapter_ble.h"
#include "sl_bt_api.h"

/**
 * @brief Structure to collect the data of one Gatt server
 */
typedef struct
{
  uint8_t ucServerIf;
  BTUuid_t xAppUuid;
} SlBtGattServerIf_t;

/**
 * @brief Singleton gattServerIf context
 */
static SlBtGattServerIf_t xGattServerIfContext = { 0 };
#define SL_BT_SINGLETON_GATT_SERVER_IF    ((uint8_t) 1)
#define SL_BT_INVALID_GATT_SERVER_IF      ((uint8_t) 0)

/** @brief Callbacks registered in the call to @ref prvGattServerInit */
static const BTGattServerCallbacks_t * pxGattServerCallbacks = NULL;

/** @defgroup BLE_GATTS_TRAN_TYPES GATT Server Transaction Types
 * @{ */
#define BLE_GATTS_TRAN_TYPE_INVALID    0x00  /**< Invalid Type. */
#define BLE_GATTS_TRAN_TYPE_READ       0x01  /**< Read Operation. */
#define BLE_GATTS_TRAN_TYPE_WRITE      0x02  /**< Write Request Operation. */
#define BLE_GATTS_TRAN_TYPE_PREP_WRITE 0x03  /**< Write Request Operation. */

#define bt16BIT_UUID_LEN    2

/**
 * @brief Some commonly used UUIDs
 */
static const uint8_t pucUuidPrimaryService[] = { 0x00, 0x28 };
static const uint8_t pucUuidSecondaryService[] = { 0x01, 0x28 };
static const uint8_t pucUuidChr[] = { 0x03, 0x28 };

/**
 * @brief Check whether the service interface is valid
 *
 * @param[in] ucServerIf Server interface
 *
 * @return true if the valid one, otherwise false
 */
static bool prvCheckServerInterface( uint8_t ucServerIf )
{
  /* We only support one GATT server. Make sure this one has registered. */
  if( ( ucServerIf == SL_BT_INVALID_GATT_SERVER_IF ) ||
      ( ucServerIf != xGattServerIfContext.ucServerIf ) )
  {
    return false;
  }
  return true;
}

/**
 * @brief Convert BTUuid_t to length and data
 *
 * @param[in] pxUuid pointer to BTUuid_t instance
 * @param[out] usUuidLen UUID data length
 *
 * @return pointer to UUID data
 */
static uint8_t* prvBTUuidToData( BTUuid_t * pxUuid, uint16_t *pusUuidLen)
{
  static uint8_t pucUuid16[bt16BIT_UUID_LEN] = { 0 };
  if ( pxUuid->ucType == eBTuuidType16 )
  {
    pucUuid16[0] = pxUuid->uu.uu16 & 0xff;
    pucUuid16[1] = pxUuid->uu.uu16 >> 8;
    *pusUuidLen = bt16BIT_UUID_LEN;
    return pucUuid16;
  } else if ( pxUuid->ucType == eBTuuidType128 ) {
    *pusUuidLen = bt128BIT_UUID_LEN;
    return pxUuid->uu.uu128;
  }
  *pusUuidLen = 0;
  return NULL;
}

/**
  * @brief Start a new GATT database update session.
  *
  * @param[out] usSessionId The database update session ID.
  *
  * @return SL_STATUS_OK on success, otherwise an error code
  */
static sl_status_t prvGattServerStartChange( uint16_t * usSessionId )
{
  if( usSessionId == NULL )
  {
    return SL_STATUS_INVALID_PARAMETER;
  }
  SILABS_BLE_LOG_PRINT_DEBUG( "new_session(%s)", " " );
  return sl_bt_gattdb_new_session( usSessionId );
}

/**
  * @brief Save all changes performed in current session and close the session.
  *
  * @param[in] usSessionId The database update session ID.
  *
  * @return SL_STATUS_OK on success, otherwise an error code
  */
static sl_status_t prvGattServerCommitChange( uint16_t usSessionId )
{
  SILABS_BLE_LOG_PRINT_DEBUG( "commit( usSessionId = %d )", usSessionId );
  return sl_bt_gattdb_commit( usSessionId );
}

/**
  * @brief Cancel all changes performed in current session and close the session.
  *
  * @param[in] usSessionId The database update session ID.
  *
  * @return SL_STATUS_OK on success, otherwise an error code
  */
static sl_status_t prvGattServerAbortChange( uint16_t usSessionId )
{
  SILABS_BLE_LOG_PRINT_DEBUG( "abort( usSessionId = %d )", usSessionId );
  return sl_bt_gattdb_abort( usSessionId );
}

/**
 * @brief Registers a GATT server application with the stack.
 *
 * This function implements pxRegisterServer function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvRegisterServer( BTUuid_t * pxUuid )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "%s", "" );
  /*
   * The current implementation supports only one GATT server.
   * Unless GATT server has already registered, we initialize the
   * singleton context and provide that to the caller.
   */
  /* Check parameters */
  if( !pxUuid )
  {
    return eBTStatusParamInvalid;
  }

  /* We only support one GATT server */
  if( xGattServerIfContext.ucServerIf != SL_BT_INVALID_GATT_SERVER_IF )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "xStatus=%d", eBTStatusNoMem );
    return eBTStatusNoMem;
  }

  /* Assign the ID and initialize */
  SlBtGattServerIf_t * gattServerIfContext = &xGattServerIfContext;
  gattServerIfContext->ucServerIf = SL_BT_SINGLETON_GATT_SERVER_IF;
  gattServerIfContext->xAppUuid = *pxUuid;

  /* Call the application */
  if( pxGattServerCallbacks && pxGattServerCallbacks->pxRegisterServerCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxRegisterServerCb", "xStatus = %d, ucServerIf = %d ",
                                  eBTStatusSuccess, gattServerIfContext->ucServerIf );
    pxGattServerCallbacks->pxRegisterServerCb( eBTStatusSuccess,
                                               gattServerIfContext->ucServerIf,
                                               pxUuid );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxRegisterServerCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Unregister a server application from the stack.
 *
 * This function implements pxUnregisterServer function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvUnregisterServer( uint8_t ucServerIf )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "%s", "" );
  if( !prvCheckServerInterface( ucServerIf ) )
  {
    return eBTStatusParamInvalid;
  }

  /* Release resources and mark the context free */
  SlBtGattServerIf_t * gattServerIfContext = &xGattServerIfContext;
  gattServerIfContext->ucServerIf = SL_BT_INVALID_GATT_SERVER_IF;

  /* Call the application */
  if( pxGattServerCallbacks && pxGattServerCallbacks->pxUnregisterServerCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxUnregisterServerCb", "xStatus = %d, ucServerIf = %d ",
                                  eBTStatusSuccess, ucServerIf );
    pxGattServerCallbacks->pxUnregisterServerCb( eBTStatusSuccess,
                                                 ucServerIf);
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxUnregisterServerCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Initializes the interface and provides callback routines.
 *
 * This function implements pxGattServerInit function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvGattServerInit( const BTGattServerCallbacks_t * pxCallbacks )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "%s", "" );
  BTStatus_t xStatus = eBTStatusParamInvalid;
  if( pxCallbacks )
  {
    pxGattServerCallbacks = pxCallbacks;
    xStatus = eBTStatusSuccess;
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", xStatus );
  return xStatus;
}

/**
 * @brief Create a connection to a remote peripheral.
 *
 * This function implements pxConnect function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvConnect( uint8_t ucServerIf,
                              const BTBdaddr_t * pxBdAddr,
                              bool bIsDirect,
                              BTTransport_t xTransport )
{
  /* This function is not supported */
  (void) ucServerIf;
  (void) pxBdAddr;
  (void) bIsDirect;
  (void) xTransport;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Disconnect an established connection or cancel a pending one.
 *
 * This function implements pxDisconnect function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvDisconnect( uint8_t ucServerIf,
                                 const BTBdaddr_t * pxBdAddr,
                                 uint16_t usConnId )
{
  /* This function is not supported */
  (void) ucServerIf;
  (void) pxBdAddr;
  (void) usConnId;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Create a new service with all its components (descriptors/characteristic/included services, etc..).
 *
 * This function implements pxAddServiceBlob function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvAddServiceBlob( uint8_t ucServerIf,
                                     BTService_t * pxService )
{
  /* This function is not supported */
  (void) ucServerIf;
  (void) pxService;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Create a new service.
 *
 * This function implements pxAddService function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvAddService( uint8_t ucServerIf,
                                 BTGattSrvcId_t * pxSrvcId,
                                 uint16_t usNumHandles )
{
  (void) usNumHandles;
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucServerIf =  %d, UUIDType = %d",
                                   ucServerIf, pxSrvcId->xId.xUuid.ucType );
  if(  !pxSrvcId || !prvCheckServerInterface( ucServerIf ) )
  {
    return eBTStatusParamInvalid;
  }

  BTStatus_t xStatus = eBTStatusSuccess;
  sl_status_t xSlStatus = SL_STATUS_OK;
  uint16_t usSessionId = 0;
  uint16_t usService = 0;
  xSlStatus = prvGattServerStartChange( &usSessionId );
  if( xSlStatus == SL_STATUS_OK )
  {
    uint8_t ucType = ( pxSrvcId->xServiceType == eBTServiceTypePrimary ) ?
                       sl_bt_gattdb_primary_service :
                       sl_bt_gattdb_secondary_service;
    uint16_t usUuidLen = 0;
    uint8_t *pucUuidData = prvBTUuidToData( &pxSrvcId->xId.xUuid, &usUuidLen );

    SILABS_BLE_LOG_PRINT_DEBUG( "add_service( session = %d, type = %d, uuid_len = %d )",
                                usSessionId, ucType, usUuidLen );

    xSlStatus = sl_bt_gattdb_add_service( usSessionId,
                                          ucType,
                                          0,
                                          usUuidLen,
                                          pucUuidData,
                                          &usService );
  }
  if( xSlStatus == SL_STATUS_OK )
  {
    xSlStatus = prvGattServerCommitChange( usSessionId );
  } else {
    prvGattServerAbortChange( usSessionId );
  }
  if( xSlStatus != SL_STATUS_OK )
  {
    xStatus = prvSlStatusToBTStatus( xSlStatus );
  }

  /* Call the application */
  if ( pxGattServerCallbacks && pxGattServerCallbacks->pxServiceAddedCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxServiceAddedCb", "xStatus = %d, ucServerIf = %d, usServiceHandle = 0x%x ",
                                  xStatus, ucServerIf, usService );
    pxGattServerCallbacks->pxServiceAddedCb( xStatus,
                                             ucServerIf,
                                             pxSrvcId,
                                             usService );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxServiceAddedCb" );
  }
  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", xStatus );
  return xStatus;
}

/**
 * @brief Assign an included service to it's parent service.
 *
 * This function implements pxAddIncludedService function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvAddIncludedService( uint8_t ucServerIf,
                                         uint16_t usServiceHandle,
                                         uint16_t usIncludedHandle )
{
  /* This function is not supported */
  (void) ucServerIf;
  (void) usServiceHandle;
  (void) usIncludedHandle;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

static uint16_t prvBTCharPermissionsToSl( BTCharPermissions_t xPermissions )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "xPermissions = %d", xPermissions );
  uint16_t usPermissions = 0;
  if( xPermissions & eBTPermReadEncrypted ) {
    usPermissions |= SL_BT_GATTDB_ENCRYPTED_READ;
  }
  if( xPermissions & eBTPermReadEncryptedMitm ) {
    usPermissions |= SL_BT_GATTDB_AUTHENTICATED_READ;
  }
  if( xPermissions & eBTPermWriteEncrypted ) {
    usPermissions |= SL_BT_GATTDB_ENCRYPTED_WRITE;
  }
  if( xPermissions & eBTPermWriteEncryptedMitm ) {
    usPermissions |= SL_BT_GATTDB_AUTHENTICATED_WRITE;
  }
  /**
   * eBTPermRead, eBTPermWrite are taken from properties
   * eBTPermWriteSigned, eBTPermWriteSignedMitm are not supported
   */
  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "usPermissions=%d", usPermissions );
  return usPermissions;
}

/**
 * @brief Add a characteristic to a service. Note: Characteristic declaration is added automatically.
 *
 * This function implements pxAddCharacteristic function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvAddCharacteristic( uint8_t ucServerIf,
                                        uint16_t usServiceHandle,
                                        BTUuid_t * pxUuid,
                                        BTCharProperties_t xProperties,
                                        BTCharPermissions_t xPermissions )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucServerIf = %d, usServiceHandle = %d, UUIDType = %d, xProperties = %d, xPermissions = %d ",
                                   ucServerIf, usServiceHandle, pxUuid->ucType, xProperties, xPermissions );
  if( !pxUuid ||
      ( usServiceHandle == 0 ) ||
      !prvCheckServerInterface( ucServerIf ) )
  {
    return eBTStatusParamInvalid;
  }

  BTStatus_t xStatus = eBTStatusSuccess;
  sl_status_t xSlStatus = SL_STATUS_OK;
  uint16_t usSessionId = 0;
  uint16_t usPermissions = prvBTCharPermissionsToSl( xPermissions );
  uint16_t usCharacteristic = 0;
  xSlStatus = prvGattServerStartChange( &usSessionId );
  if( xSlStatus == SL_STATUS_OK )
  {
    if ( pxUuid->ucType == eBTuuidType16 )
    {
      sl_bt_uuid_16_t uuid;
      uuid.data[0] = pxUuid->uu.uu16 & 0xff;
      uuid.data[1] = pxUuid->uu.uu16 >> 8;

      SILABS_BLE_LOG_PRINT_DEBUG( "add_uuid16_characteristic( session = %d, service = %d, property = %d, security = %d )",
                                  usSessionId, usServiceHandle, xProperties, usPermissions );

      xSlStatus = sl_bt_gattdb_add_uuid16_characteristic( usSessionId,
                                                          usServiceHandle,
                                                          xProperties,
                                                          usPermissions,
                                                          SL_BT_GATTDB_NO_AUTO_CCCD, /* Do not create client-config automatically */
                                                          uuid,
                                                          sl_bt_gattdb_user_managed_value,
                                                          0, 0, NULL, /* Ignored parameters when value type is user_managed */
                                                          &usCharacteristic );
    } else if ( pxUuid->ucType == eBTuuidType128 ) {
      uuid_128 uuid;
      memcpy( uuid.data, pxUuid->uu.uu128, bt128BIT_UUID_LEN);

      SILABS_BLE_LOG_PRINT_DEBUG( "add_uuid128_characteristic( session = %d, service = %d, property = %d, security = %d )",
                                  usSessionId, usServiceHandle, xProperties, usPermissions );

      xSlStatus = sl_bt_gattdb_add_uuid128_characteristic( usSessionId,
                                                           usServiceHandle,
                                                           xProperties,
                                                           usPermissions,
                                                           SL_BT_GATTDB_NO_AUTO_CCCD, /* Do not create client-config automatically */
                                                           uuid,
                                                           sl_bt_gattdb_user_managed_value,
                                                           0, 0, NULL, /* Ignored parameters when value type is user_managed */
                                                           &usCharacteristic );
    } else  {
      return eBTStatusParamInvalid;
    }
  }
  if( xSlStatus == SL_STATUS_OK )
  {
    xSlStatus = prvGattServerCommitChange( usSessionId );
  } else {
    prvGattServerAbortChange( usSessionId );
  }
  if( xSlStatus != SL_STATUS_OK )
  {
    xStatus = prvSlStatusToBTStatus( xSlStatus );
  }

  /* Call the application */
  if ( pxGattServerCallbacks && pxGattServerCallbacks->pxCharacteristicAddedCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxCharacteristicAddedCb", "xStatus = %d, ucServerIf = %d, usServiceHandle = 0x%x, usCharHandle = 0x%x ",
                                  xStatus, ucServerIf, usServiceHandle, usCharacteristic );
    pxGattServerCallbacks->pxCharacteristicAddedCb( xStatus,
                                                    ucServerIf,
                                                    pxUuid,
                                                    usServiceHandle,
                                                    usCharacteristic );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxCharacteristicAddedCb" );
  }
  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", xStatus );
  return xStatus;
}

/**
 * @brief Set the primitive/complete values only.
 *
 * This function implements pxSetVal function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvSetVal( BTGattResponse_t * pxValue )
{
  /* This function is not supported */
  (void) pxValue;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

static uint16_t prvBTCharPermissionsToSlDescriptorProperties( BTCharPermissions_t ulPermissions )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "ulPermissions = %d", ulPermissions );
  uint16_t usProperties = 0;
  if( ( ulPermissions & eBTPermRead ) ||
      ( ulPermissions & eBTPermReadEncrypted )  ||
      ( ulPermissions & eBTPermReadEncryptedMitm ) )
  {
    usProperties |= SL_BT_GATTDB_DESCRIPTOR_READ;
  }
  if( ( ulPermissions & eBTPermWrite ) ||
      ( ulPermissions & eBTPermWriteEncrypted )  ||
      ( ulPermissions & eBTPermWriteEncryptedMitm ) )
  {
    usProperties |= SL_BT_GATTDB_DESCRIPTOR_WRITE;
  }
  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "usProperties=%d", usProperties );
  return usProperties;
}

static uint16_t prvFindServiceEndHandle( uint16_t usServiceHandle )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "usServiceHandle = %d", usServiceHandle );
  uint16_t usNextPriSrv = 0;
  uint16_t usNextSecSrv = 0;
  SILABS_BLE_LOG_PRINT_DEBUG( "find_attribute( start = %d, type_len = %d, pucUuidPrimaryService )",
                              usServiceHandle, bt16BIT_UUID_LEN );
  sl_bt_gatt_server_find_attribute( usServiceHandle,
                                    bt16BIT_UUID_LEN,
                                    pucUuidPrimaryService,
                                    &usNextPriSrv );
  SILABS_BLE_LOG_PRINT_DEBUG( "find_attribute( start = %d, type_len = %d, pucUuidSecondaryService )",
                              usServiceHandle, bt16BIT_UUID_LEN );
  sl_bt_gatt_server_find_attribute( usServiceHandle,
                                    bt16BIT_UUID_LEN,
                                    pucUuidSecondaryService,
                                    &usNextPriSrv );
  /* Return whichever the smaller, 0 means not found */
  uint16_t usSrvEnd = ( usNextPriSrv > usNextSecSrv ) ? usNextSecSrv : usNextPriSrv;
  if (usSrvEnd > 0)
  {
    /* Found service, minus 1 to get last handle of previous service */
    usSrvEnd --;
  }
  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "usSrvEnd=%d", usSrvEnd );
  return usSrvEnd;
}

static uint16_t prvFindLastCharacteristicHandle( uint16_t usServiceHandle )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "usServiceHandle = %d", usServiceHandle );
  uint16_t usServiceEnd = prvFindServiceEndHandle( usServiceHandle );
  if( usServiceEnd == 0 )
  {
    usServiceEnd = 0xffff; /* If there is no following service, set ending to 0xffff */
  }
  uint16_t usCharacteristic = 0;
  uint16_t usStart = usServiceHandle;
  uint16_t usNextChar = 0;
  sl_status_t xSlStatus = SL_STATUS_OK;
  do {
    SILABS_BLE_LOG_PRINT_DEBUG( "find_attribute( start = %d, type_len = %d, pucUuidChr )",
                                usStart, bt16BIT_UUID_LEN );
    xSlStatus = sl_bt_gatt_server_find_attribute( usStart,
                                                  bt16BIT_UUID_LEN,
                                                  pucUuidChr,
                                                  &usNextChar );
    if( xSlStatus != SL_STATUS_OK || usNextChar == 0 || usNextChar > usServiceEnd )
    {
      /* if no characteristic is found or found characteristic beyond the service, exit */
      break;
    }
    ++usNextChar; /* Convert to characteristic value handle */
    usStart = usNextChar;
    usCharacteristic = usNextChar;
  } while ( xSlStatus == SL_STATUS_OK );
  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "usCharacteristic=%d", usCharacteristic );
  return usCharacteristic;
}

/**
 * @brief Add a descriptor to a given service.
 *
 * This function implements pxAddDescriptor function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvAddDescriptor( uint8_t ucServerIf,
                                    uint16_t usServiceHandle,
                                    BTUuid_t * pxUuid,
                                    BTCharPermissions_t ulPermissions )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucServerIf = %d, usServiceHandle = %d, UUIDType = %d, xPermissions = %d ",
                                   ucServerIf, usServiceHandle, pxUuid->ucType, ulPermissions );
  if( !pxUuid ||
      ( usServiceHandle == 0 ) ||
      !prvCheckServerInterface( ucServerIf ) )
  {
    return eBTStatusParamInvalid;
  }

  uint16_t usCharacteristic = prvFindLastCharacteristicHandle( usServiceHandle );
  if( usCharacteristic == 0 )
  {
    return eBTStatusParamInvalid;
  }
  BTStatus_t xStatus = eBTStatusSuccess;
  sl_status_t xSlStatus = SL_STATUS_OK;
  uint16_t usSessionId = 0;
  uint16_t usProperties = prvBTCharPermissionsToSlDescriptorProperties( ulPermissions );
  uint16_t usPermissions = prvBTCharPermissionsToSl( ulPermissions );
  uint16_t usDescriptor = 0;
  xSlStatus = prvGattServerStartChange( &usSessionId );
  if( xSlStatus == SL_STATUS_OK )
  {
    if ( pxUuid->ucType == eBTuuidType16 )
    {
      sl_bt_uuid_16_t uuid;
      uuid.data[0] = pxUuid->uu.uu16 & 0xff;
      uuid.data[1] = pxUuid->uu.uu16 >> 8;

      SILABS_BLE_LOG_PRINT_DEBUG( "add_uuid16_descriptor( session = %d, characteristic = %d, property = %d, security = %d )",
                                  usSessionId, usCharacteristic, usProperties, usPermissions );

      xSlStatus = sl_bt_gattdb_add_uuid16_descriptor( usSessionId,
                                                      usCharacteristic,
                                                      usProperties,
                                                      usPermissions,
                                                      uuid,
                                                      sl_bt_gattdb_user_managed_value,
                                                      0, 0, NULL,
                                                      &usDescriptor );
    } else if ( pxUuid->ucType == eBTuuidType128 ) {
      uuid_128 uuid;
      memcpy( uuid.data, pxUuid->uu.uu128, bt128BIT_UUID_LEN);

      SILABS_BLE_LOG_PRINT_DEBUG( "add_uuid128_descriptor( session = %d, characteristic = %d, property = %d, security = %d )",
                                  usSessionId, usCharacteristic, usProperties, usPermissions );

      xSlStatus = sl_bt_gattdb_add_uuid128_descriptor( usSessionId,
                                                       usCharacteristic,
                                                       usProperties,
                                                       usPermissions,
                                                       uuid,
                                                       sl_bt_gattdb_user_managed_value,
                                                       0, 0, NULL,
                                                       &usDescriptor );
    } else  {
      return eBTStatusParamInvalid;
    }
  }
  if( xSlStatus == SL_STATUS_OK )
  {
    xSlStatus = prvGattServerCommitChange( usSessionId );
  } else {
    prvGattServerAbortChange( usSessionId );
  }
  if( xSlStatus != SL_STATUS_OK )
  {
    xStatus = prvSlStatusToBTStatus( xSlStatus );
  }

  /* Call the application */
  if ( pxGattServerCallbacks && pxGattServerCallbacks->pxDescriptorAddedCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxDescriptorAddedCb", "xStatus = %d, ucServerIf = %d, usServiceHandle = 0x%x, usDescrHandle = 0x%x ",
                                  xStatus, ucServerIf, usServiceHandle, usDescriptor );
    pxGattServerCallbacks->pxDescriptorAddedCb( xStatus,
                                                ucServerIf,
                                                pxUuid,
                                                usServiceHandle,
                                                usDescriptor );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxDescriptorAddedCb" );
  }
  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", xStatus );
  return xStatus;
}

/**
 * @brief Starts a local service.
 *
 * This function implements pxStartService function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvStartService( uint8_t ucServerIf,
                                   uint16_t usServiceHandle,
                                   BTTransport_t xTransport )
{
  (void) xTransport;
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucServerIf =  %d, usServiceHandle = %d ",
                                    ucServerIf, usServiceHandle );
  if( ( usServiceHandle == 0 ) || !prvCheckServerInterface( ucServerIf ) )
  {
    return eBTStatusParamInvalid;
  }

  BTStatus_t xStatus = eBTStatusSuccess;
  sl_status_t xSlStatus = SL_STATUS_OK;
  uint16_t usSessionId = 0;
  xSlStatus = prvGattServerStartChange( &usSessionId );
  if( xSlStatus == SL_STATUS_OK )
  {
    SILABS_BLE_LOG_PRINT_DEBUG( "start_service( session = %d, service = %d )",
                                usSessionId, usServiceHandle );
    xSlStatus = sl_bt_gattdb_start_service( usSessionId,
                                            usServiceHandle );
  }
  if( xSlStatus == SL_STATUS_OK )
  {
    xSlStatus = prvGattServerCommitChange( usSessionId );
  } else {
    prvGattServerAbortChange( usSessionId );
  }
  if( xSlStatus != SL_STATUS_OK )
  {
    xStatus = prvSlStatusToBTStatus( xSlStatus );
  }

  /* Call the application */
  if ( pxGattServerCallbacks && pxGattServerCallbacks->pxServiceStartedCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxServiceStartedCb", "xStatus = %d, ucServerIf = %d, usServiceHandle = 0x%x ",
                                  xStatus, ucServerIf, usServiceHandle );
    pxGattServerCallbacks->pxServiceStartedCb( xStatus,
                                               ucServerIf,
                                               usServiceHandle );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxServiceStartedCb" );
  }
  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", xStatus );
  return xStatus;
}

/**
 * @brief Stops a local service if supported by the underlying stack.
 *
 * This function implements pxStopService function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvStopService( uint8_t ucServerIf,
                                  uint16_t usServiceHandle )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucServerIf =  %d, usServiceHandle = %d ",
                                   ucServerIf, usServiceHandle);
  if( ( usServiceHandle == 0 ) || !prvCheckServerInterface( ucServerIf ) )
  {
    return eBTStatusParamInvalid;
  }

  BTStatus_t xStatus = eBTStatusSuccess;
  sl_status_t xSlStatus = SL_STATUS_OK;
  uint16_t usSessionId = 0;
  xSlStatus = prvGattServerStartChange( &usSessionId );
  if( xSlStatus == SL_STATUS_OK )
  {
    SILABS_BLE_LOG_PRINT_DEBUG( "stop_service( session = %d, service = %d )",
                                usSessionId, usServiceHandle );
    xSlStatus = sl_bt_gattdb_stop_service( usSessionId,
                                           usServiceHandle );
  }
  if( xSlStatus == SL_STATUS_OK )
  {
    xSlStatus = prvGattServerCommitChange( usSessionId );
  } else {
    prvGattServerAbortChange( usSessionId );
  }
  if( xSlStatus != SL_STATUS_OK )
  {
    xStatus = prvSlStatusToBTStatus( xSlStatus );
  }

  /* Call the application */
  if ( pxGattServerCallbacks && pxGattServerCallbacks->pxServiceStoppedCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxServiceStoppedCb", "xStatus = %d, ucServerIf = %d, usServiceHandle = 0x%x ",
                                  xStatus, ucServerIf, usServiceHandle );
    pxGattServerCallbacks->pxServiceStoppedCb( xStatus,
                                               ucServerIf,
                                               usServiceHandle );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxServiceStoppedCb" );
  }
  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", xStatus );
  return xStatus;
}

/**
 * @brief Delete a local service.
 *
 * This function implements pxDeleteService function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvDeleteService( uint8_t ucServerIf,
                                    uint16_t usServiceHandle )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucServerIf =  %d, usServiceHandle = %d ",
                                    ucServerIf, usServiceHandle );
  if( ( usServiceHandle == 0 ) || !prvCheckServerInterface( ucServerIf ) )
  {
    return eBTStatusParamInvalid;
  }

  BTStatus_t xStatus = eBTStatusSuccess;
  sl_status_t xSlStatus = SL_STATUS_OK;
  uint16_t usSessionId = 0;
  xSlStatus = prvGattServerStartChange( &usSessionId );
  if( xSlStatus == SL_STATUS_OK )
  {
    SILABS_BLE_LOG_PRINT_DEBUG( "remove_service( session = %d, service = %d )",
                                usSessionId, usServiceHandle );
    xSlStatus = sl_bt_gattdb_remove_service( usSessionId,
                                             usServiceHandle );
  }
  if( xSlStatus == SL_STATUS_OK )
  {
    xSlStatus = prvGattServerCommitChange( usSessionId );
  } else {
    prvGattServerAbortChange( usSessionId );
  }
  if( xSlStatus != SL_STATUS_OK )
  {
    xStatus = prvSlStatusToBTStatus( xSlStatus );
  }

  /* Call the application */
  if ( pxGattServerCallbacks && pxGattServerCallbacks->pxServiceDeletedCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxServiceDeletedCb", "xStatus = %d, ucServerIf = %d, usServiceHandle = 0x%x ",
                                  xStatus, ucServerIf, usServiceHandle );
    pxGattServerCallbacks->pxServiceDeletedCb( xStatus,
                                               ucServerIf,
                                               usServiceHandle );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxServiceDeletedCb" );
  }
  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", xStatus );
  return xStatus;
}

/**
 * @brief Send value indication to a remote device.
 *
 * This function implements pxSendIndication function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvSendIndication( uint8_t ucServerIf,
                                     uint16_t usAttributeHandle,
                                     uint16_t usConnId,
                                     size_t xLen,
                                     uint8_t * pucValue,
                                     bool bConfirm )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucServerIf = %d, usAttributeHandle = %d, usConnId = %d, bConfirm = % ",
                                   ucServerIf, usAttributeHandle, usConnId, bConfirm );
  /* Check parameters */
  if( !pucValue || !prvCheckServerInterface( ucServerIf ) )
  {
    return eBTStatusParamInvalid;
  }

  BTStatus_t xStatus = eBTStatusSuccess;
  sl_status_t xSlStatus = SL_STATUS_OK;
  if( bConfirm )
  {
    SILABS_BLE_LOG_PRINT_DEBUG( "send_indication(connection = %d, characteristic = %d)",
                                usConnId, usAttributeHandle );

    /* Indications need confirmation */
    xSlStatus = sl_bt_gatt_server_send_indication( usConnId,
                                                   usAttributeHandle,
                                                   xLen,
                                                   pucValue );
    /* Indications need to wait until confirmation of indication is received
     * before calling the application. */
  } else {
    SILABS_BLE_LOG_PRINT_DEBUG( "send_notification(connection = %d, characteristic = %d)",
                                usConnId, usAttributeHandle );

    /* Notification does not need confirmation*/
    xSlStatus = sl_bt_gatt_server_send_notification( usConnId,
                                                     usAttributeHandle,
                                                     xLen,
                                                     pucValue );
    /* Call the application */
    if( pxGattServerCallbacks && pxGattServerCallbacks->pxIndicationSentCb )
    {
      pxGattServerCallbacks->pxIndicationSentCb( usConnId,
                                                 prvSlStatusToBTStatus( xSlStatus ) );
    }
  }
  if( xSlStatus != SL_STATUS_OK )
  {
    xStatus = prvSlStatusToBTStatus( xSlStatus );
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", xStatus );
  return xStatus;
}

/**
 * @brief Send a response to a read/write operation.
 *
 * This function implements pxSendResponse function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvSendResponse( uint16_t usConnId,
                                   uint32_t ulTransId,
                                   BTStatus_t xStatus,
                                   BTGattResponse_t * pxResponse )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "usConnId = %d, ulTransId = %d, xStatus = %d",
                                   usConnId, ulTransId, xStatus );
  BTStatus_t xRetStatus = eBTStatusSuccess;
  sl_status_t xSlStatus = SL_STATUS_OK;
  uint8_t ucAttErrorCode = 0;

  if(!usConnId || !pxResponse )
      xRetStatus = eBTStatusParamInvalid;

  if( xRetStatus == eBTStatusSuccess )
  {
      switch ( ulTransId )
      {
        case BLE_GATTS_TRAN_TYPE_WRITE:
          {
            if (xStatus != eBTStatusSuccess)
              ucAttErrorCode = 0x03; /* att_error_code_write_not_permitted */

            /* Send response to remote */
            xSlStatus = sl_bt_gatt_server_send_user_write_response( usConnId,
                                                                    pxResponse->usHandle,
                                                                    ucAttErrorCode );

            SILABS_BLE_LOG_PRINT_DEBUG( "send_user_write_response( xSlStatus = 0x%X, usConnId = %d, usHandle = %d, ucAttErrorCode = %d",
                                        xSlStatus, usConnId, pxResponse->usHandle, ucAttErrorCode );
            if (xSlStatus != SL_STATUS_OK)
              xRetStatus = eBTStatusFail;
          }
          break;

        case BLE_GATTS_TRAN_TYPE_PREP_WRITE:
          {
            if (xStatus != eBTStatusSuccess)
              ucAttErrorCode = 0x03; /* att_error_code_write_not_permitted */

            /* Send response to remote */
            xSlStatus = sl_bt_gatt_server_send_user_prepare_write_response( usConnId,
                                                                            pxResponse->usHandle,
                                                                            ucAttErrorCode,
                                                                            pxResponse->xAttrValue.usOffset,
                                                                            pxResponse->xAttrValue.xLen,
                                                                            &pxResponse->xAttrValue.pucValue[0] );

            if (xSlStatus != SL_STATUS_OK)
              xRetStatus = eBTStatusFail;

            SILABS_BLE_LOG_PRINT_DEBUG( "send_user_prepare_write_response( xSlStatus = 0x%X, usConnId = %d, usHandle = %d, ucAttErrorCode = %d, offset = %d, Sent Length = %d ",
                                        xSlStatus, usConnId, pxResponse->usHandle, ucAttErrorCode, pxResponse->xAttrValue.usOffset, pxResponse->xAttrValue.xLen );
          }
          break;

        case BLE_GATTS_TRAN_TYPE_READ:
          {
            if (xStatus != eBTStatusSuccess)
              ucAttErrorCode = 0x02; /* att_error_code_read_not_permitted */

            uint16_t usSentLen;

            /* Check MTU size */
            uint16_t rspValLen;
            xSlStatus = sl_bt_gatt_server_get_mtu( usConnId, &rspValLen );

            SILABS_BLE_LOG_PRINT_DEBUG( "get_mtu( xSlStatus = 0x%X, usConnId = %d, MTU = %d",
                                        xSlStatus, usConnId, rspValLen );
            if (xSlStatus != SL_STATUS_OK)
            {
              xRetStatus = eBTStatusFail;
              break;
            }

            /* Compare MTU and the length of the unsent Attribute value*/
            if (rspValLen > pxResponse->xAttrValue.xLen)
              rspValLen = pxResponse->xAttrValue.xLen;

            /* Send response to remote */
            xSlStatus = sl_bt_gatt_server_send_user_read_response( usConnId,
                                                                   pxResponse->usHandle,
                                                                   ucAttErrorCode,
                                                                   rspValLen,
                                                                   &pxResponse->xAttrValue.pucValue[0],
                                                                   &usSentLen );

            if (xSlStatus != SL_STATUS_OK)
              xRetStatus = eBTStatusFail;

            SILABS_BLE_LOG_PRINT_DEBUG( "send_user_read_response( xSlStatus = 0x%X, usConnId = %d, \
                                        usHandle = %d, ucAttErrorCode = %d, Sent Length = %d",
                                        xSlStatus, usConnId, pxResponse->usHandle, ucAttErrorCode, usSentLen );
          }
          break;

          case BLE_GATTS_TRAN_TYPE_INVALID:
            SILABS_BLE_LOG_PRINT_ERROR( "BLE_GATTS_TRAN_TYPE_INVALID, ucAttErrorCode = %d", ucAttErrorCode );
            ucAttErrorCode = 0x01; /* att_error_code_invalid_handle */
            break;

          default:
            break;
      }

      if(xRetStatus == eBTStatusSuccess && ucAttErrorCode != 0x00)
      {
          xRetStatus = eBTStatusFail;
      }

      if( pxGattServerCallbacks->pxResponseConfirmationCb != NULL )
      {
        SILABS_BLE_LOG_CB_CALL_DEBUG( "pxResponseConfirmationCb", "xStatus = %d, usHandle = 0x%x",
                                     xRetStatus, pxResponse->usHandle );
        pxGattServerCallbacks->pxResponseConfirmationCb( xRetStatus, pxResponse->usHandle );
        SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxResponseConfirmationCb" );
      }
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "xStatus=%d", xRetStatus );
  return xRetStatus;
}

/**
 * @brief Start directed advertising to the device address passed.
 *
 * This function implements pxReconnect function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvReconnect( uint8_t ucServerIf,
                                const BTBdaddr_t * pxBdAddr )
{
  /* This function is not supported */
  (void) ucServerIf;
  (void) pxBdAddr;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Add devices to whitelist.
 *
 * This function implements pxAddDevicesToWhiteList function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvAddDevicesToWhiteList( uint8_t ucServerIf,
                                            const BTBdaddr_t * pxBdAddr,
                                            uint32_t ulNumberOfDevices )
{
  /* This function is not supported */
  (void) ucServerIf;
  (void) pxBdAddr;
  (void) ulNumberOfDevices;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Remove device from whitelist and stops any ongoing directed advertisements.
 *
 * This function implements pxRemoveDevicesFromWhiteList function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvRemoveDevicesFromWhiteList( uint8_t ucServerIf,
                                                 const BTBdaddr_t * pxBdAddr,
                                                 uint32_t ulNumberOfDevices )
{
  /* This function is not supported */
  (void) ucServerIf;
  (void) pxBdAddr;
  (void) ulNumberOfDevices;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Configure the MTU for gatt server.
 *
 * This function implements pxConfigureMtu function in @ref BTGattServerInterface_t.
 */
static BTStatus_t prvConfigureMtu( uint8_t ucServerIf,
                                   uint16_t usMtu )
{
  /* This function is not supported */
  (void) ucServerIf;
  (void) usMtu;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Silicon Labs GATT Server interface functions.
 */
static const BTGattServerInterface_t xSilabsBTGattServerInterface =
{
  .pxRegisterServer = prvRegisterServer,
  .pxUnregisterServer = prvUnregisterServer,
  .pxGattServerInit = prvGattServerInit,
  .pxConnect = prvConnect,
  .pxDisconnect = prvDisconnect,
  .pxAddServiceBlob = prvAddServiceBlob,
  .pxAddService = prvAddService,
  .pxAddIncludedService = prvAddIncludedService,
  .pxAddCharacteristic = prvAddCharacteristic,
  .pxSetVal = prvSetVal,
  .pxAddDescriptor = prvAddDescriptor,
  .pxStartService = prvStartService,
  .pxStopService = prvStopService,
  .pxDeleteService = prvDeleteService,
  .pxSendIndication = prvSendIndication,
  .pxSendResponse = prvSendResponse,
  .pxReconnect = prvReconnect,
  .pxAddDevicesToWhiteList = prvAddDevicesToWhiteList,
  .pxRemoveDevicesFromWhiteList = prvRemoveDevicesFromWhiteList,
  .pxConfigureMtu = prvConfigureMtu
};

/* Get the GATT server interface. */
const void * prvGetGattServerInterface( )
{
  return &xSilabsBTGattServerInterface;
}

void prvGattServerOnSlBtEvent(sl_bt_msg_t* evt)
{
  switch( SL_BT_MSG_ID(evt->header) ) {
    case sl_bt_evt_gatt_server_characteristic_status_id:
      if( evt->data.evt_gatt_server_characteristic_status.status_flags == gatt_server_confirmation )
      {
        /* Call the application as a result of prvSendIndication as confirmation of
           indication has been received. */
        if( pxGattServerCallbacks && pxGattServerCallbacks->pxIndicationSentCb )
        {
          SILABS_BLE_LOG_CB_CALL_DEBUG( "pxIndicationSentCb", "usConnId = %d, xStatus = %d",
                                        evt->data.evt_gatt_server_characteristic_status.connection, eBTStatusSuccess );
          pxGattServerCallbacks->pxIndicationSentCb( evt->data.evt_gatt_server_characteristic_status.connection,
                                                     eBTStatusSuccess );
          SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxIndicationSentCb" );
        }
      }
      break;
    case sl_bt_evt_gatt_server_indication_timeout_id:
      /* Call the application as a result of prvSendIndication as confirmation of
         indication has been timeout. */
      if( pxGattServerCallbacks && pxGattServerCallbacks->pxIndicationSentCb )
      {
        SILABS_BLE_LOG_CB_CALL_DEBUG( "pxIndicationSentCb", "usConnId = %d, xStatus = %d",
                                      evt->data.evt_gatt_server_characteristic_status.connection, eBTStatusFail );
        pxGattServerCallbacks->pxIndicationSentCb( evt->data.evt_gatt_server_indication_timeout.connection,
                                                   eBTStatusFail );
        SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxIndicationSentCb" );
      }
      break;
    case sl_bt_evt_gatt_server_user_write_request_id:
      {
        bool bIsNeedRsp = false;

        if( ( evt->data.evt_gatt_server_user_write_request.att_opcode == gatt_write_request ) ||
          ( evt->data.evt_gatt_server_user_write_request.att_opcode == gatt_prepare_write_request) )
        {
            bIsNeedRsp = true;
        }

        SILABS_BLE_LOG_PRINT_DEBUG( "user_write_request_id: Opcode:%d",
                                    evt->data.evt_gatt_server_user_write_request.att_opcode );

        switch( evt->data.evt_gatt_server_user_write_request.att_opcode)
        {
            case gatt_write_request:
            case gatt_write_command:
                {
                  if( pxGattServerCallbacks->pxRequestWriteCb != NULL )
                  {
                      SILABS_BLE_LOG_CB_CALL_DEBUG( "pxRequestWriteCb", "Connection = %d, Characteristic = %d, bIsNeedRsp: %d ",
                                                    evt->data.evt_gatt_server_user_write_request.connection,
                                                    evt->data.evt_gatt_server_user_write_request.characteristic,
                                                    bIsNeedRsp);
                      BTBdaddr_t * pAddress = prvGetConnectionAddress( evt->data.evt_gatt_server_user_write_request.connection );
                      pxGattServerCallbacks->pxRequestWriteCb( evt->data.evt_gatt_server_user_write_request.connection,
                                                               BLE_GATTS_TRAN_TYPE_WRITE,
                                                               pAddress,
                                                               evt->data.evt_gatt_server_user_write_request.characteristic,
                                                               evt->data.evt_gatt_server_user_write_request.offset,
                                                               evt->data.evt_gatt_server_user_write_request.value.len,
                                                               bIsNeedRsp,
                                                               false,
                                                               evt->data.evt_gatt_server_user_write_request.value.data );
                      SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxRequestWriteCb" );
                  }
                }
                break;

            case gatt_prepare_write_request:
                {
                  if( pxGattServerCallbacks->pxRequestWriteCb != NULL )
                  {
                      SILABS_BLE_LOG_CB_CALL_DEBUG( "pxRequestWriteCb", "Connection =  %d, Characteristic = %d, bIsNeedRsp: %d ",
                                                    evt->data.evt_gatt_server_user_write_request.connection,
                                                    evt->data.evt_gatt_server_user_write_request.characteristic,
                                                    bIsNeedRsp);
                      BTBdaddr_t * pAddress = prvGetConnectionAddress( evt->data.evt_gatt_server_user_write_request.connection );
                      pxGattServerCallbacks->pxRequestWriteCb( evt->data.evt_gatt_server_user_write_request.connection,
                                                               BLE_GATTS_TRAN_TYPE_PREP_WRITE,
                                                               pAddress,
                                                               evt->data.evt_gatt_server_user_write_request.characteristic,
                                                               evt->data.evt_gatt_server_user_write_request.offset,
                                                               evt->data.evt_gatt_server_user_write_request.value.len,
                                                               bIsNeedRsp,
                                                               true,
                                                               evt->data.evt_gatt_server_user_write_request.value.data );
                      SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxRequestWriteCb" );
                  }
                }
                break;

            case gatt_execute_write_request:
                if( pxGattServerCallbacks->pxRequestExecWriteCb != NULL )
                {
                    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxRequestExecWriteCb", "Connection = %d ",
                                                  evt->data.evt_gatt_server_user_write_request.connection );
                    BTBdaddr_t * pAddress = prvGetConnectionAddress( evt->data.evt_gatt_server_user_write_request.connection );
                    pxGattServerCallbacks->pxRequestExecWriteCb( evt->data.evt_gatt_server_user_write_request.connection,
                                                                 BLE_GATTS_TRAN_TYPE_WRITE,
                                                                 pAddress,
                                                                 true);
                    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxRequestExecWriteCb" );
                }
                break;

            default:
                break;
        }
      }
      break;

    case  sl_bt_evt_gatt_server_user_read_request_id:
      {
        if( pxGattServerCallbacks->pxRequestReadCb != NULL )
        {
            SILABS_BLE_LOG_CB_CALL_DEBUG( "pxRequestReadCb", "Connection = %d, Characteristic = %d",
                                          evt->data.evt_gatt_server_user_read_request.connection,
                                          evt->data.evt_gatt_server_user_read_request.characteristic );
            BTBdaddr_t * pAddress = prvGetConnectionAddress( evt->data.evt_gatt_server_user_read_request.connection );
            pxGattServerCallbacks->pxRequestReadCb( evt->data.evt_gatt_server_user_read_request.connection,
                                                    BLE_GATTS_TRAN_TYPE_READ,
                                                    pAddress,
                                                    evt->data.evt_gatt_server_user_read_request.characteristic,
                                                    evt->data.evt_gatt_server_user_read_request.offset);
            SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxRequestReadCb" );
        }
      }
      break;

    case sl_bt_evt_gatt_mtu_exchanged_id:
      {
        if( pxGattServerCallbacks->pxMtuChangedCb != NULL )

        {
            SILABS_BLE_LOG_CB_CALL_DEBUG( "pxMtuChangedCb", "Connection = %d",
                                          evt->data.evt_gatt_mtu_exchanged.connection );

            pxGattServerCallbacks->pxMtuChangedCb( evt->data.evt_gatt_mtu_exchanged.connection,
                                                   evt->data.evt_gatt_mtu_exchanged.mtu);
            SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxMtuChangedCb" );
        }
      }
      break;


    default:
      break;
  }
}

void prvGattServerOpenConnectionEventCb( uint8_t ucConnectionHandle, BTBdaddr_t * pBTAddress )
{
  if( pxGattServerCallbacks->pxConnectionCb )
  {
    SlBtGattServerIf_t * gattServerIfContext = &xGattServerIfContext;
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxConnectionCb", "usConnId = 0x%x, bConnected = %b", ucConnectionHandle, true );
    pxGattServerCallbacks->pxConnectionCb( ucConnectionHandle,
                                           gattServerIfContext->ucServerIf,
                                           true,
                                           pBTAddress );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxConnectionCb" );
  }
}

void prvGattServerCloseConnectionEventCb( uint8_t ucConnectionHandle, BTBdaddr_t * pBTAddress )
{
  if( pxGattServerCallbacks->pxConnectionCb )
  {
    SlBtGattServerIf_t * gattServerIfContext = &xGattServerIfContext;
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxConnectionCb", "usConnId = 0x%x, bConnected = %b", ucConnectionHandle, false );
    pxGattServerCallbacks->pxConnectionCb( ucConnectionHandle,
                                           gattServerIfContext->ucServerIf,
                                           false,
                                           pBTAddress );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxConnectionCb" );
  }
}
