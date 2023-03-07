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

/* Standard library includes */
#include <stddef.h>

/* FreeRTOS BLE HAL includes */
#include "bt_hal_manager_adapter_ble.h"

/* Silicon Labs includes */
#include "sl_bt_hal_gatt_client.h"
#include "sl_bt_hal_gatt_server.h"
#include "sl_bt_hal_manager.h"
#include "sl_bt_hal_common.h"
#include "sl_bt_api.h"
#include "sl_bluetooth_config.h"
#include "sl_malloc.h"

/* Values used for @p phys parameter of @ref sl_bt_scanner_set_timing, @p phys
 * parameter of @ref sl_bt_scanner_set_mode, and @p scanning_phy parameter of
 * @ref sl_bt_scanner_start */
#define SCANNER_PHYS_1M            ((uint8_t) 0x01)
#define SCANNER_PHYS_CODED         ((uint8_t) 0x04)
#define SCANNER_PHYS_1M_AND_CODED  ((uint8_t) 0x05)

/* Values used for @p scan_mode param of @ref sl_bt_scanner_set_mode */
#define SCANNER_MODE_PASSIVE  ((uint8_t) 0x00)
#define SCANNER_MODE_ACTIVE   ((uint8_t) 0x01)

/* Scan report packet type mask and values */
#define SCAN_REPORT_PACKET_TYPE_MASK                           ((uint8_t) 0x07)
#define SCAN_REPORT_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED_ADV  ((uint8_t) 0x00)
#define SCAN_REPORT_TYPE_CONNECTABLE_UNDIRECTED_ADV            ((uint8_t) 0x01)
#define SCAN_REPORT_TYPE_SCANNABLE_UNDIRECTED_ADV              ((uint8_t) 0x02)
#define SCAN_REPORT_TYPE_UNDIRECTED_ADV                        ((uint8_t) 0x03)
#define SCAN_REPORT_TYPE_SCAN_RESPONSE                         ((uint8_t) 0x04)

/* Scan report packet type mask and values to indicate completeness */
#define SCAN_REPORT_COMPLETENESS_MASK   ((uint8_t) 0x60)
#define SCAN_REPORT_COMPLETE            ((uint8_t) 0x00)
#define SCAN_REPORT_INCOMPLETE_MORE     ((uint8_t) 0x20)
#define SCAN_REPORT_INCOMPLETE_NO_MORE  ((uint8_t) 0x40)

/* Scan report packet type flag that indicates extended advertising */
#define SCAN_REPORT_EXTENDED_PDUS_FLAG  ((uint8_t) 0x80)

/** @brief Maximum length of legacy advertisement data */
#define MAX_LEGACY_ADV_DATA_LEN 31

/** @brief Buffer size for combined advertisement and scan response data */
#define ADV_DATA_AND_SCAN_RESPONSE_BUF_SIZE 62

/* Range of NVM keys used for storing the original device addresses for each bonded device */
#define BONDED_DEVICE_ADDRESS_NVM_KEY_MIN ((uint16_t) 0x4800)
#define BONDED_DEVICE_ADDRESS_NVM_KEY_MAX ((uint16_t) 0x4FFF)

/**
 * @brief Scanning context used for active scanning
 *
 * When active scanning is used, we need to temporarily store the
 * advertisement data so that we can combine it with the scan response data
 * that may arrive in a separate event.
 */
typedef struct
{
  uint8_t ucScanMode;   /**< SL BT API scanning mode */
  BTBdaddr_t xAddress;  /**< Address of the advertiser that sent the stored data */
  int8_t cRssi;         /**< Signal strength indicator (RSSI) in the received data */
  uint8_t ucDataLength; /**< Length of the data stored or 0 if the context is empty */
  uint8_t ucAdvData[ADV_DATA_AND_SCAN_RESPONSE_BUF_SIZE];
} SlBtScanContext_t;

/**
 * @brief Scanning context instance
 *
 * The scanning context is allocated when scanning starts, and freed when scanning is ended.
 */
SlBtScanContext_t * pxScanContext = NULL;

/* Type of the API that the advertiser is using */
typedef enum
{
  SlBtHalAdvApiNone = 0, /**< Advertising is not in use */
  SlBtHalAdvApiBasic,    /**< The basic advertising API (pxStartAdv(), pxStopAdv(),
                              pxSetAdvData()) is used */
  SlBtHalAdvApiMultiAdv, /**< The MultiAdv*() API is in use */
} SlBtHalAdvApi_t;

/**
 * @brief Structure to collect the data of one application/adapterIf
 */
typedef struct
{
  SlBtBleHalClient_t xClient;   /**< Client UUID and handle */
  SlBtHalAdvApi_t xAdvApiType;  /**< Type of the advertisement API that's being used */
  uint8_t ucAdvSetHandle;       /**< SL BT API advertiser set handle */
  uint8_t ucAdvConnectableMode; /**< SL BT API advertiser connectable mode */
  int16_t sSetAdvPower;         /**< Power level that was set for advertising */
  bool bAdvActive;              /**< True when advertising is active */
} SlBtAdapterIf_t;

/**
 * @brief Head of the list of adapterIf contexts
 */
static SlBtAdapterIf_t * pxAdapterIfs = NULL;

/* Flag indicating a new pairing on the connection just happened. */
#define CONNECTION_FLAG_NEW_PAIRING    ((uint8_t) 0x01)

/* Valid range for connection interval. See sl_bt_api.h, function
 * sl_bt_connection_set_parameters() */
#define MIN_CONNECTION_INTERVAL ((uint16_t) 0x0006)
#define MAX_CONNECTION_INTERVAL ((uint16_t) 0x0c80)

/* Valid range for connection timeout. See sl_bt_api.h, function
 * sl_bt_connection_set_parameters() */
#define MIN_CONNECTION_TIMEOUT ((uint16_t) 0x000a)
#define MAX_CONNECTION_TIMEOUT ((uint16_t) 0x0c80)

/* Maximum value for slave latency. See sl_bt_api.h, function
 * sl_bt_connection_set_parameters() */
#define MAX_SLAVE_LATENCY ((uint16_t) 0x01f4)

/**
 * @brief Structure to save BT connection informations
 */
typedef struct
{
  uint8_t ucAdapterIf;        /**< ucAdapterIf of application which opened the connection */
  uint8_t ucConnectionHandle; /**< Handle of connection */
  uint8_t ucBondingHandle;    /**< The bonding handle of the remote device */
  uint8_t ucSecurityLevel;    /**< The security level of the connection */
  uint16_t usMinInterval;     /**< Minimum connection interval currently requested for this connection,
                                   or 0 if no local parameter update request is pending */
  uint16_t usMaxInterval;     /**< Maximum connection interval currently requested for this connection,
                                   or 0 if no local parameter update request is pending */
  BTBdaddr_t xRemoteAddress;  /**< Remote BT address used to identify this device in the BLE HAL API.
                                   For bonded devices the original address is stored in non-volatile
                                   memory and retrieved when the device reconnects. */
  uint8_t ucFlags;            /**< Flags currently set on this connection */
} SlBtConnections_t;

/**
 * @brief BT connection array contains BT address, connection handle values of open connections
 */
static SlBtConnections_t xBtConnections[SL_BT_CONFIG_MAX_CONNECTIONS] = { 0 };

/** @brief Callbacks registered in the call to @ref prvBleAdapterInit */
static const BTBleAdapterCallbacks_t * pxBleAdapterCallbacks = NULL;

/**
 * @brief Static random address used for advertisers
 *
 * This static random Bluetooth address is used by all advertisers that specify the
 * `BTAddrTypeStaticRandom` address type in their configuration. The address is generated when it is
 * first needed and remains unchanged until device reboot.
 */
static bd_addr xAdvStaticRandomAddr = { 0 };
static bool bHaveAdvStaticRandomAddr = false;

/**
 * @brief Structure to collect advertiser parameters used in SL BT API
 */
typedef struct
{
  uint32_t ulMinInterval;
  uint32_t ulMaxInterval;
  uint16_t usTimeout;
  int16_t  usRequestedPower;
  uint8_t  ucPrimaryPhy;   /* Uses values of enum gap_phy_type_t */
  uint8_t  ucSecondaryPhy; /* Uses values of enum gap_phy_type_t */
  uint8_t  ucChannelMap;
  uint8_t  ucAddressType;  /* Uses values of enum gap_address_type_t */
} SlBtAdvParams_t;

/**
 * @brief Convert Silabs security mode to a value of BTSecurityLevel_t type.
 */
static BTSecurityLevel_t prvConvertSecurityMode(sl_bt_connection_security_t ucSecurityMode)
{
  switch (ucSecurityMode)
  {
    case sl_bt_connection_mode1_level1:
      return eBTSecLevelNoSecurity;

    case sl_bt_connection_mode1_level2:
      return eBTSecLevelUnauthenticatedPairing;

    case sl_bt_connection_mode1_level3:
      return eBTSecLevelAuthenticatedPairing;

    case sl_bt_connection_mode1_level4:
      return eBTSecLevelSecureConnect;

    default:
      return eBTSecLevelNoSecurity;
  }
}

/**
 * @brief Save the newly established connection handle, address information and adapterIf
 *
 * @param[in] ucAdapterIf The adapterIf
 * @param[in] ucConnectionHandle The connection handle
 * @param[in] pxRemoteAddress Bluetooth address of the remote device
 *
 * @return Pointer to the SlBtConnections_t structure where the information is saved
 */
static SlBtConnections_t * prvSaveNewConnection( uint8_t ucAdapterIf,
                                                 uint8_t ucConnectionHandle,
                                                 uint8_t ucBondingHandle,
                                                 const BTBdaddr_t * pxRemoteAddress )
{
  SlBtConnections_t * pConnection = NULL;
  for ( uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i ++ )
  {
    /* Find a free memory slot for new connection. */
    if ( (xBtConnections[i].ucAdapterIf == SL_BT_INVALID_IF_HANDLE) ||
         (xBtConnections[i].ucConnectionHandle == ucConnectionHandle) )
    {
      pConnection = &xBtConnections[i];
      break;
    }
  }
  if ( pConnection != NULL )
  {
    pConnection->ucAdapterIf = ucAdapterIf;
    pConnection->ucConnectionHandle = ucConnectionHandle;
    pConnection->ucBondingHandle = ucBondingHandle;
    pConnection->ucSecurityLevel = eBTSecLevelNoSecurity;
    pConnection->usMinInterval = 0; /* No parameter update request is pending yet */
    pConnection->usMaxInterval = 0;
    pConnection->ucFlags = 0;
    pConnection->xRemoteAddress = *pxRemoteAddress;
  }
  return pConnection;
}

/**
 * @brief Get the information of saved connection
 *
 * @param[in] ucConnectionHandle The connection handle
 *
 * @return Pointer of SlBtConnections_t structure where the connection is saved
 */
static SlBtConnections_t * prvGetConnection( uint8_t ucConnectionHandle )
{
  for ( uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i ++ )
  {
    if ( (xBtConnections[i].ucAdapterIf != SL_BT_INVALID_IF_HANDLE) &&
         (xBtConnections[i].ucConnectionHandle == ucConnectionHandle) )
    {
      return &xBtConnections[i];
    }
  }
  return NULL;
}

/* Get pointer of BTBdaddr_t of the connection handle */
BTBdaddr_t * prvGetConnectionAddress( uint8_t ucConnectionHandle )
{
  SlBtConnections_t * pConnection =  prvGetConnection( ucConnectionHandle );
  if ( pConnection != NULL )
  {
    return &pConnection->xRemoteAddress;
  }
  return NULL;
}

/**
 * @brief Get the information of saved connection by the Bluetooth address
 *
 * @param[in] pxRemoteAddress Bluetooth address of the remote device
 *
 * @return Pointer of SlBtConnections_t structure where the connection is saved
 */
static SlBtConnections_t * prvGetConnectionByAddress( const BTBdaddr_t * pxRemoteAddress )
{
  if( pxRemoteAddress == NULL )
  {
    return NULL;
  }

  for( uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++ )
  {
    if ( ( xBtConnections[i].ucAdapterIf != SL_BT_INVALID_IF_HANDLE ) &&
         ( memcmp( xBtConnections[i].xRemoteAddress.ucAddress,
                   pxRemoteAddress->ucAddress,
                   sizeof( pxRemoteAddress->ucAddress ) ) == 0 ) )
    {
      return &xBtConnections[i];
    }
  }

  return NULL;
}

/* Get the connection handle by a Bluetooth device address. */
BTStatus_t prvGetConnectionHandle( const BTBdaddr_t * pxRemoteAddress, uint8_t *pucConnectionHandle )
{
  /* Find the connection */
  SlBtConnections_t * pxConnection = prvGetConnectionByAddress( pxRemoteAddress );
  if( pxConnection == NULL )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to find connection for pxRemoteAddress="LOG_BDADDR_FORMAT", status=%d",
                                    LOG_BTBDADDR_T_VALUE( pxRemoteAddress ), eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  *pucConnectionHandle = pxConnection->ucConnectionHandle;
  return eBTStatusSuccess;
}

/**
 * @brief Get the adapterIf context for the specified @p ucAdapterIf
 *
 * @param[in] ucAdapterIf The adapter interface handle
 *
 * @return Pointer to the adapterIf context, or NULL if ucAdapterIf was invalid
 */
static inline SlBtAdapterIf_t * prvGetAdapterIf( uint8_t ucAdapterIf )
{
  SlBtBleHalClient_t * pxClient =
    prvGetBleHalClientByHandle( ucAdapterIf, ( SlBtBleHalClient_t * ) pxAdapterIfs );
  return ( SlBtAdapterIf_t * ) pxClient;
}

/**
 * @brief Get the adapterIf context for the specified @p ucAdvSetHandle
 *
 * @param[in] ucAdvSetHandle Advertisement set handle
 *
 * @return Pointer to the adapterIf context, or NULL if no matching adapterIf was found
 */
static SlBtAdapterIf_t * prvGetAdapterIfByAdvSetHandle( uint8_t ucAdvSetHandle )
{
  /* Find the adapter context, if any, that has this advertiser set */
  SlBtAdapterIf_t * pxAdapterIf = pxAdapterIfs;
  while( pxAdapterIf != NULL )
  {
    if( (pxAdapterIf->xClient.ucHandle != SL_BT_INVALID_IF_HANDLE) &&
        (pxAdapterIf->ucAdvSetHandle != SL_BT_INVALID_ADVERTISING_SET_HANDLE) &&
        (pxAdapterIf->ucAdvSetHandle == ucAdvSetHandle) )
    {
      return pxAdapterIf;
    }

    pxAdapterIf = ( SlBtAdapterIf_t * ) pxAdapterIf->xClient.pxNext;
  }

  return NULL;
}

/**
 * @brief Get the adapterIf context and create an advertiser set if not done yet
 *
 * @param[in] ucAdapterIf The adapter interface handle
 * @param[in] xAdvApiType The API type that the advertiser set will be used for
 * @param[out] ppxAdapterIf On success set, set to point to the adapterIf context
 *
 * @return eBTStatusSuccess on success, otherwise an error code
 */
static BTStatus_t prvGetAdapterIfWithAdvSet( uint8_t ucAdapterIf,
                                             SlBtHalAdvApi_t xAdvApiType,
                                             SlBtAdapterIf_t ** ppxAdapterIf )
{
  /* Get the adapterIf context */
  SlBtAdapterIf_t * pxAdapterIf = prvGetAdapterIf( ucAdapterIf );
  if( pxAdapterIf == NULL )
  {
    return eBTStatusParamInvalid;
  }
  *ppxAdapterIf = NULL;

  /* If the adapter context is already reserved for use with another
   * advertisement API, return an error. */
  if( ( pxAdapterIf->xAdvApiType != SlBtHalAdvApiNone ) && ( pxAdapterIf->xAdvApiType != xAdvApiType ) )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "adapter has xAdvApiType=%d, requested xAdvApiType=%d",
                                    pxAdapterIf->xAdvApiType, xAdvApiType );
    return eBTStatusBusy;
  }

  /* If we don't yet have an advertiser set, create one now */
  if( pxAdapterIf->ucAdvSetHandle == SL_BT_INVALID_ADVERTISING_SET_HANDLE )
  {
    sl_status_t sl_status = sl_bt_advertiser_create_set( &pxAdapterIf->ucAdvSetHandle );
    if( sl_status != SL_STATUS_OK )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to create advertiser, sl_status=0x%x", sl_status );
      return prvSlStatusToBTStatus( sl_status );
    }

    /* Commit the context to the given API */
    pxAdapterIf->xAdvApiType = xAdvApiType;
  }

  *ppxAdapterIf = pxAdapterIf;
  return eBTStatusSuccess;
}

/* Advertisement PHY selection values specified in Bluetooth Core Specification
 * version 5.2, Volume 4, Part E, Section 7.8.53. */
#define ADVERTISEMENT_PHY_1M    ((uint8_t) 0x01)
#define ADVERTISEMENT_PHY_2M    ((uint8_t) 0x02)
#define ADVERTISEMENT_PHY_CODED ((uint8_t) 0x03)

/**
 * @brief Map a BLE Adapter API phy type to a gap_phy_type_t
 *
 * @param[in] ucPhy Phy selection of BLE Adapter API
 *
 * @return Corresponding gap_phy_type_t value
 */
static gap_phy_type_t prvBlePhyToSlGapPhyType( uint8_t ucPhy )
{
  /* Amazon FreeRTOS Bluetooth API does not specify the values used for the phy
   * selection, but seems to use value 0 in tests. We must assume that 0 means
   * default and any concrete values are the values used in the HCI standard
   * (Bluetooth Core Specification version 5.2, Volume 4, Part E, Section
   * 7.8.53). */
  gap_phy_type_t gapPhy = gap_1m_phy;
  switch( ucPhy )
  {
    case ADVERTISEMENT_PHY_1M:    gapPhy = gap_1m_phy;    break;
    case ADVERTISEMENT_PHY_2M:    gapPhy = gap_2m_phy;    break;
    case ADVERTISEMENT_PHY_CODED: gapPhy = gap_coded_phy; break;
    default:                      gapPhy = gap_1m_phy;    break;
  }

  return gapPhy;
}

/**
 * @brief Map a BLE Adapter API advertising event properties to
 * advertiser_connectable_mode_t
 *
 * @param[in] ucPhy Phy selection of BLE Adapter API
 *
 * @return Corresponding gap_phy_type_t value
 */
static BTStatus_t prvAdvEventPropsToSlAdvConnectableMode(
  BTAdvProperties_t usAdvEventProps,
  uint8_t * pucSlBtConnectableMode )
{
  BTStatus_t status = eBTStatusParamInvalid;
  switch ( usAdvEventProps )
  {
    case BTAdvInd:
      /* Undirected connectable scannable */
      *pucSlBtConnectableMode = sl_bt_advertiser_connectable_scannable;
      status = eBTStatusSuccess;
      break;

    case BTAdvDirectInd:
      /* Directed connectable. Not supported by Silicon Labs stack. */
      status = eBTStatusUnsupported;
      break;

    case BTAdvNonconnInd:
      /* Non-connectable non-scannable */
      *pucSlBtConnectableMode = sl_bt_advertiser_non_connectable;
      status = eBTStatusSuccess;
      break;

    default:
      status = eBTStatusParamInvalid;
      break;
  }

  return status;
}

/**
 * @brief Array to map BT_HAL_BLE_ADV_TX_PWR_* index to SL BT API power value
 *
 * TX power is represented in the SL BT API with units of 0.1 dBm.
 */
static const int16_t xTxPowerLevels[] =
{
  -210, /* BT_HAL_BLE_ADV_TX_PWR_ULTRA_LOW,  -21 dBm */
  -150, /* BT_HAL_BLE_ADV_TX_PWR_LOW,        -15 dBm */
   -70, /* BT_HAL_BLE_ADV_TX_PWR_MEDIUM,      -7 dBm */
    40, /* BT_HAL_BLE_ADV_TX_PWR_HIGH,         4 dBm */
    90, /* BT_HAL_BLE_ADV_TX_PWR_ULTRA_HIGH,   9 dBm */
};

/* Map a BLE Adapter API power level index to SL BT API power value */
BTStatus_t prvTxPowerIndexToSlBtPower( uint8_t ucTxPower,
                                       int16_t * psSlBtPower )
{
  BTStatus_t status = eBTStatusParamInvalid;
  if( ucTxPower < (sizeof(xTxPowerLevels) / sizeof(xTxPowerLevels[0])) )
  {
    *psSlBtPower = xTxPowerLevels[ucTxPower];
    status = eBTStatusSuccess;
  }
  else
  {
    /* Out of range, use the lowest power level */
    status = eBTStatusParamInvalid;
    *psSlBtPower = xTxPowerLevels[0];
  }

  return status;
}

/**
 * @brief Registers a BLE application with the stack.
 *
 * This function implements pxRegisterBleApp function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvRegisterBleApp( BTUuid_t * pxAppUuid )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "pxAppUuid=%p", pxAppUuid );

  /* Register using the common mechanism */
  SlBtAdapterIf_t * pxAdapterIf = NULL;
  BTStatus_t status = prvRegisterBleHalClient( pxAppUuid,
                                               ( SlBtBleHalClient_t ** ) &pxAdapterIfs,
                                               SL_BT_HAL_MAX_BLE_APPS,
                                               sizeof(*pxAdapterIfs),
                                               ( SlBtBleHalClient_t ** ) &pxAdapterIf );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "status=%d", status );
    return status;
  }

  /* Initialize the context */
  pxAdapterIf->ucAdvSetHandle = SL_BT_INVALID_ADVERTISING_SET_HANDLE;
  pxAdapterIf->bAdvActive = false;

  /* Call the application */
  if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxRegisterBleAdapterCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxRegisterBleAdapterCb", "ucHandle=0x%x",
                                  pxAdapterIf->xClient.ucHandle );
    pxBleAdapterCallbacks->pxRegisterBleAdapterCb( eBTStatusSuccess,
                                                   pxAdapterIf->xClient.ucHandle,
                                                   pxAppUuid );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxRegisterBleAdapterCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d, ucAdapterIf=%d",
                                  eBTStatusSuccess, (int) pxAdapterIf->xClient.ucHandle );
  return eBTStatusSuccess;
}

/**
 * @brief Unregister a BLE application from the stack.
 *
 * This function implements pxUnregisterBleApp function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvUnregisterBleApp( uint8_t ucAdapterIf )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "ucAdapterIf=%d", (int) ucAdapterIf );

  /* Get the adapter context */
  SlBtAdapterIf_t * pxAdapterIf = prvGetAdapterIf( ucAdapterIf );
  if( !pxAdapterIf )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* Release resources */
  BTStatus_t status = eBTStatusSuccess;
  if( pxAdapterIf->ucAdvSetHandle != SL_BT_INVALID_ADVERTISING_SET_HANDLE )
  {
    sl_status_t sl_status = sl_bt_advertiser_delete_set( pxAdapterIf->ucAdvSetHandle );
    if( sl_status != SL_STATUS_OK )
    {
      status = prvSlStatusToBTStatus( sl_status );
    }
    pxAdapterIf->ucAdvSetHandle = SL_BT_INVALID_ADVERTISING_SET_HANDLE;
  }
  pxAdapterIf->xClient.ucHandle = SL_BT_INVALID_IF_HANDLE;

  /* Free the context */
  prvFreeBleHalClient( ( SlBtBleHalClient_t ** ) &pxAdapterIfs,
                       ( SlBtBleHalClient_t * ) pxAdapterIf );

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", status );
  return status;
}

/**
 * @brief Initializes the interface and provides callback routines.
 *
 * This function implements pxBleAdapterInit function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "pxCallbacks=%p", pxCallbacks );

  /* Free all adapters on initialisation */
  while( pxAdapterIfs != NULL )
  {
    prvFreeBleHalClient( ( SlBtBleHalClient_t ** ) &pxAdapterIfs,
                       ( SlBtBleHalClient_t * ) pxAdapterIfs );
  }

  BTStatus_t status = eBTStatusParamInvalid;
  if( pxCallbacks )
  {
    pxBleAdapterCallbacks = pxCallbacks;
    status = eBTStatusSuccess;
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", status );
  return status;
}

/**
 * @brief Get Bluetooth LE Adapter property of 'type'.
 *
 * This function implements pxGetBleAdapterProperty function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvGetBleAdapterProperty( BTBlePropertyType_t xType )
{
  /* This function is not supported */
  (void) xType;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Set Bluetooth LE Adapter property of 'type'.
 *
 * This function implements pxSetBleAdapterProperty function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvSetBleAdapterProperty( const BTBleProperty_t * pxProperty )
{
  /* This function is not supported */
  (void) pxProperty;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Get all Remote Device properties.
 *
 * This function implements pxGetallBleRemoteDeviceProperties function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvGetallBleRemoteDeviceProperties( BTBdaddr_t * pxRremoteAddr )
{
  /* This function is not supported */
  (void) pxRremoteAddr;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Get Remote Device property of 'type'.
 *
 * This function implements pxGetBleRemoteDeviceProperty function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvGetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                                 BTBleProperty_t xType )
{
  /* This function is not supported */
  (void) pxRemoteAddr;
  (void) xType;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Set Remote Device property of 'type' .
 *
 * This function implements pxSetBleRemoteDeviceProperty function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvSetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                                 const BTBleProperty_t * pxProperty )
{
  /* This function is not supported */
  (void) pxRemoteAddr;
  (void) pxProperty;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Start or stop LE device scanning.
 *
 * This function implements pxScan function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvScan( bool bStart )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "bStart=%d", (int) bStart );

  /* Start or stop depending on the ask */
  sl_status_t sl_status = SL_STATUS_FAIL;
  if( bStart )
  {
    /* Allocate the scanning context if we don't have it already */
    if( pxScanContext == NULL )
    {
      pxScanContext = sl_malloc( sizeof( *pxScanContext ) );
      if( pxScanContext == NULL )
      {
        SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to allocate context, status=%d", eBTStatusNoMem );
        return eBTStatusNoMem;
      }
      memset( pxScanContext, 0, sizeof( *pxScanContext ) );
    }

    /* The BLE HAL API does not provide a method for choosing the scanning mode.
    We use active scanning always. */
    pxScanContext->ucScanMode = SCANNER_MODE_ACTIVE;

    /* Set the scanning mode and start scanning */
    sl_status = sl_bt_scanner_set_mode( SCANNER_PHYS_1M, pxScanContext->ucScanMode );
    if( sl_status == SL_STATUS_OK )
    {
      sl_status = sl_bt_scanner_start( SCANNER_PHYS_1M,
                                       sl_bt_scanner_discover_observation );
    }

    /* If we failed to start, free the scanning context */
    if( sl_status != SL_STATUS_OK )
    {
      sl_free( pxScanContext );
      pxScanContext = NULL;
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to start, sl_status=0x%x", sl_status );
      return prvSlStatusToBTStatus( sl_status );
    }
  }
  else
  {
    /* When scanner is stopped, always free the scanning context to make sure we
    stop delivering scan reports to the app even if the stopping in the stack
    somehow fails. */
    if( pxScanContext != NULL )
    {
      sl_free( pxScanContext );
      pxScanContext = NULL;
    }

    /* Stop the scanner in the Bluetooth stack */
    sl_status = sl_bt_scanner_stop( );
    if( sl_status != SL_STATUS_OK )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to stop, sl_status=0x%x", sl_status );
      return prvSlStatusToBTStatus( sl_status );
    }
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Create a connection to a remote LE or dual-mode device.
 *
 * This function implements pxConnect function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvConnect( uint8_t ucAdapterIf,
                              const BTBdaddr_t * pxBdAddr,
                              bool bIsDirect,
                              BTTransport_t ulTransport )
{
  /* This function is not supported */
  (void) ucAdapterIf;
  (void) pxBdAddr;
  (void) bIsDirect;
  (void) ulTransport;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Disconnect a remote device or cancel a pending connection.
 *
 * This function implements pxDisconnect function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvDisconnect( uint8_t ucAdapterIf,
                                 const BTBdaddr_t * pxBdAddr,
                                 uint16_t usConnId )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucAdapterIf=%d, usConnId=%PRIu16",
                                  (int) ucAdapterIf, usConnId );

  (void) pxBdAddr;
  /* Get the adapterIf context */
  BTStatus_t status = eBTStatusParamInvalid;
  SlBtAdapterIf_t * pxAdapterIf = prvGetAdapterIf( ucAdapterIf );
  if( pxAdapterIf )
  {
    status = eBTStatusSuccess;
    sl_status_t sl_status = sl_bt_connection_close( usConnId );
    if( sl_status != SL_STATUS_OK )
    {
      status = prvSlStatusToBTStatus( sl_status );
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to close, status=%d", status );
      return status;
    }
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", eBTStatusSuccess );
  return status;
}

/**
 * @brief Start advertisements to listen for incoming connections.
 *
 * This function implements pxStartAdv function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvStartAdv( uint8_t ucAdapterIf )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucAdapterIf=%d", (int) ucAdapterIf );

  /* Get the adapterIf context and create its advertiser set if necessary */
  SlBtAdapterIf_t * pxAdapterIf = NULL;
  BTStatus_t status = prvGetAdapterIfWithAdvSet( ucAdapterIf, SlBtHalAdvApiBasic, &pxAdapterIf );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to get adapter with advertiser, status=%d", status );
    return status;
  }

  /* Start advertising with user-defined data */
  sl_status_t sl_status = sl_bt_advertiser_start( pxAdapterIf->ucAdvSetHandle,
                                                  sl_bt_advertiser_user_data,
                                                  pxAdapterIf->ucAdvConnectableMode );
  if( sl_status != SL_STATUS_OK )
  {
    status = prvSlStatusToBTStatus( sl_status );
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to start, status=%d", status );
    return status;
  }

  /* On success, remember the status and give the callback */
  pxAdapterIf->bAdvActive = true;
  if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxAdvStatusCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxAdvStatusCb", "ucHandle=0x%x", pxAdapterIf->xClient.ucHandle );
    pxBleAdapterCallbacks->pxAdvStatusCb( eBTStatusSuccess,
                                          pxAdapterIf->xClient.ucHandle,
                                          true );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxAdvStatusCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", status );
  return status;
}

/**
 * @brief Stop advertisements to listen for incoming connections.
 *
 * This function implements pxStopAdv function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvStopAdv( uint8_t ucAdapterIf )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucAdapterIf=%d", (int) ucAdapterIf );

  /* Get the adapterIf context */
  SlBtAdapterIf_t * pxAdapterIf = prvGetAdapterIf( ucAdapterIf );
  if( !pxAdapterIf )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to get adapter, status=%d",
                                    eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* Stop advertising if we have a handle and are currently active */
  BTStatus_t status = eBTStatusSuccess;
  if( (pxAdapterIf->ucAdvSetHandle != SL_BT_INVALID_ADVERTISING_SET_HANDLE) &&
      pxAdapterIf->bAdvActive )
  {
    sl_status_t sl_status = sl_bt_advertiser_stop( pxAdapterIf->ucAdvSetHandle );
    if( sl_status != SL_STATUS_OK )
    {
      SILABS_BLE_LOG_PRINT_ERROR( "failed to stop advertiser, sl_status=0x%x", sl_status );
      status = prvSlStatusToBTStatus( sl_status );
    }
  }

  pxAdapterIf->bAdvActive = false;

  /* Give the callback */
  if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxAdvStatusCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxAdvStatusCb", "ucHandle=0x%x", pxAdapterIf->xClient.ucHandle );
    pxBleAdapterCallbacks->pxAdvStatusCb( eBTStatusSuccess,
                                          pxAdapterIf->xClient.ucHandle,
                                          false );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxAdvStatusCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", status );
  return status;
}

/**
 * @brief Delete the advertiser set of a multi-advertiser
 *
 * @param[in] pxAdapterIf Adapter IF context of the multi-advertiser
 */
static void prvDeleteMultiAdvertiserAdvSet( SlBtAdapterIf_t * pxAdapterIf )
{
  /* Delete the advertiser set, if any */
  if( ( pxAdapterIf->ucAdvSetHandle != SL_BT_INVALID_ADVERTISING_SET_HANDLE ) &&
      ( pxAdapterIf->xAdvApiType == SlBtHalAdvApiMultiAdv ) )
  {
    /* We ignore any errors here, as there's nothing we could do to recover. */
    sl_status_t sl_status = sl_bt_advertiser_delete_set( pxAdapterIf->ucAdvSetHandle );
    (void) sl_status;
    pxAdapterIf->ucAdvSetHandle = SL_BT_INVALID_ADVERTISING_SET_HANDLE;
    pxAdapterIf->xAdvApiType = SlBtHalAdvApiNone;
    pxAdapterIf->bAdvActive = false;

    /* Call the application */
    if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxMultiAdvDisableCb )
    {
      SILABS_BLE_LOG_CB_CALL_DEBUG( "pxMultiAdvDisableCb", "ucHandle=0x%x", pxAdapterIf->xClient.ucHandle );
      pxBleAdapterCallbacks->pxMultiAdvDisableCb( pxAdapterIf->xClient.ucHandle, eBTStatusSuccess );
      SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxMultiAdvDisableCb" );
    }
  }
}

/**
 * @brief Handle the implicit stopping of an advertiser
 *
 * This function is called when advertising is stopped implicitly without
 * application action, for example when the advertisiment timed out, or was
 * stopped because a connection was established. This function checks the type
 * of the API that was being used and handles the implicit stopping
 * approriately.
 *
 * @param[in] pxAdapterIf Adapter IF context of the advertiser
 */
static void prvStopAdvertiserImplicitly( SlBtAdapterIf_t * pxAdapterIf )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "ucAdapterIf=%d", ( int ) pxAdapterIf->xClient.ucHandle );

  /* If the app is advertising with the basic advertising APIs, stop it */
  if( ( pxAdapterIf->xAdvApiType == SlBtHalAdvApiBasic ) && ( pxAdapterIf->bAdvActive ) )
  {
    /* Stopping the basic advertiser will also make the callback to the app */
    prvStopAdv( pxAdapterIf->xClient.ucHandle );
  }

  /* If the context is used by a multi-advertiser, delete the advertiser */
  if( ( pxAdapterIf->xAdvApiType == SlBtHalAdvApiMultiAdv ) )
  {
    /* Deleting the multi-advertiser will also make the callback to the app */
    prvDeleteMultiAdvertiserAdvSet( pxAdapterIf );
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "%s", "" );
}

/**
 * @brief Triggered when advertiser has timed out
 */
static void prvOnAdvertiserTimeout( sl_bt_evt_advertiser_timeout_t * pxEvent )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "advertiser=%d", ( int ) pxEvent->handle );


  /* Get the adapterIf context */
  SlBtAdapterIf_t * pxAdapterIf = prvGetAdapterIfByAdvSetHandle( pxEvent->handle );
  if( pxAdapterIf )
  {
    /* Handle the implicit stop of the advertiser */
    prvStopAdvertiserImplicitly( pxAdapterIf );
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "%s", "" );
}

/**
 * @brief Append data to the scanning context
 *
 * @param[out] pxScanContext Scan context to append to
 * @param[in] pxScanReport Scan report to read data from
 */
static void prvAppendScanReportData( SlBtScanContext_t * pxScanContext,
                                     sl_bt_evt_scanner_scan_report_t * pxScanReport )
{
  /* Make sure we never overflow the buffer */
  size_t xSpaceRemaining = 0;
  if( sizeof( pxScanContext->ucAdvData ) > pxScanContext->ucDataLength )
  {
    xSpaceRemaining = sizeof( pxScanContext->ucAdvData ) - pxScanContext->ucDataLength;
  }
  uint8_t ucBytesToCopy = pxScanReport->data.len;
  if( ucBytesToCopy > xSpaceRemaining )
  {
    ucBytesToCopy = xSpaceRemaining;
  }

  /* Append the data */
  memcpy( &pxScanContext->ucAdvData[pxScanContext->ucDataLength],
          pxScanReport->data.data,
          ucBytesToCopy );
  pxScanContext->ucDataLength += ucBytesToCopy;

  /* If the buffer is not full, ensure termination with a 0 byte */
  if( pxScanContext->ucDataLength < sizeof( pxScanContext->ucAdvData ) )
  {
    pxScanContext->ucAdvData[pxScanContext->ucDataLength] = 0;
  }

  /* Store the RSSI and the address */
  pxScanContext->cRssi = pxScanReport->rssi;
  memcpy( pxScanContext->xAddress.ucAddress,
          pxScanReport->address.addr,
          sizeof( pxScanContext->xAddress.ucAddress ) );
}

/**
 * @brief Send data to application and clear the scanning context
 *
 * @param[in,out] pxScanContext Scan context to send and clear
 */
static void prvSendAndClearScanReportData( SlBtScanContext_t * pxScanContext )
{
  /* Send the data to the app */
  if( ( pxBleAdapterCallbacks != NULL ) && ( pxBleAdapterCallbacks->pxScanResultCb != NULL ) )
  {
    pxBleAdapterCallbacks->pxScanResultCb( &pxScanContext->xAddress,
                                           (uint32_t) pxScanContext->cRssi, /* RSSI is reported in a uint32_t */
                                           pxScanContext->ucAdvData );
  }

  /* Clear the data */
  memset( &pxScanContext->xAddress, 0, sizeof( pxScanContext->xAddress ) );
  memset( &pxScanContext->ucAdvData, 0, sizeof( pxScanContext->ucAdvData ) );
  pxScanContext->ucDataLength = 0;
}

/**
 * @brief Triggered when a scan report event is received
 */
static void prvOnScanReportEvent( sl_bt_evt_scanner_scan_report_t * pxScanReport )
{
  /* If the application has no callback or is not actively scanning, we can
  ignore the event */
  if( ( pxBleAdapterCallbacks == NULL ) ||
      ( pxBleAdapterCallbacks->pxScanResultCb == NULL ) ||
      ( pxScanContext == NULL ) )
  {
    return;
  }

  /* The BLE HAL API is only prepared for legacy advertising. We ignore extended
  advertising or data that is too long or incomplete. */
  if( ( pxScanReport->packet_type & SCAN_REPORT_EXTENDED_PDUS_FLAG ) ||
      ( pxScanReport->data.len > MAX_LEGACY_ADV_DATA_LEN ) ||
      ( ( pxScanReport->packet_type & SCAN_REPORT_COMPLETENESS_MASK ) != SCAN_REPORT_COMPLETE ) )
  {
    return;
  }

  /* If active scanning is being used, we need to evaluate whether to store
  or forward the data immediately */
  if( pxScanContext->ucScanMode == SCANNER_MODE_ACTIVE )
  {
    /* See if the scan report we have is a scan response for the same device
    that we already have advertisement data from */
    uint8_t ucPacketType = pxScanReport->packet_type & SCAN_REPORT_PACKET_TYPE_MASK;
    if( ( ucPacketType == SCAN_REPORT_TYPE_SCAN_RESPONSE ) &&
        ( pxScanContext->ucDataLength > 0 ) &&
        ( memcmp( pxScanContext->xAddress.ucAddress,
                  pxScanReport->address.addr,
                  sizeof( pxScanContext->xAddress.ucAddress ) ) == 0 ) )
    {
      /* Append and send to the app */
      prvAppendScanReportData( pxScanContext, pxScanReport );
      prvSendAndClearScanReportData( pxScanContext );
      return;
    }

    /* Data was not appended, so this is not continuation of previous data.
    If we already have previously received data, send and clear that. */
    if( pxScanContext->ucDataLength > 0 )
    {
      prvSendAndClearScanReportData( pxScanContext );
    }

    /* If the scan report is scannable advertisement, the Bluetooth stack
    will send a scan request and we may later receive a scan response that
    we need to append to the advertisement data. */
    if( ( ucPacketType == SCAN_REPORT_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED_ADV ) ||
        ( ucPacketType == SCAN_REPORT_TYPE_SCANNABLE_UNDIRECTED_ADV ) )
    {
      /* Advertisement is scannable. Store the advertisement data. */
      prvAppendScanReportData( pxScanContext, pxScanReport );
      return;
    }

    /* The event is one that should be reported immediately. Continue to
    reporting it below. */
  }

  /* The BLE HAL API does not take the length of the advertisement data. It
  seems to assume that the data ends with an item that is 0 bytes long, i.e.
  that the first byte after the advertisement items is 0. We must take a
  copy so that we can ensure that termination. We have already verified the
  data size above, so this is guaranteed to fit in the buffer. We borrow the
  buffer for the copy but don't mean to store this event as we deliver it
  immediately. Make sure the stored length is cleared. */
  pxScanContext->ucDataLength = 0;
  memcpy( pxScanContext->ucAdvData, pxScanReport->data.data, pxScanReport->data.len );
  pxScanContext->ucAdvData[pxScanReport->data.len] = 0;
  pxBleAdapterCallbacks->pxScanResultCb( (BTBdaddr_t *) &pxScanReport->address,
                                         (uint32_t) pxScanReport->rssi, /* RSSI is reported in a uint32_t */
                                         pxScanContext->ucAdvData );
}

/**
 * @brief Map a bonding handle to an NVM key
 *
 * @param[in] ucBondingHandle The bonding handle
 *
 * @return The NVM key used to store information of this bonding
 */
static inline uint16_t prvBondingHandleToNvmKey( uint8_t ucBondingHandle )
{
  uint16_t usNvmKey = BONDED_DEVICE_ADDRESS_NVM_KEY_MIN + ucBondingHandle;
  return usNvmKey;
}

/**
 * @brief Save the address of a bonded device to non-volatile memory
 *
 * @param[in] ucBondingHandle The bonding handle
 * @param[in] pxBdAddr The Bluetooth address in the original connection
 *
 * @return eBTStatusSuccess on success, otherwise an error code
 */
static BTStatus_t prvSaveBondedDeviceAddress( uint8_t ucBondingHandle,
                                              const BTBdaddr_t * pxBdAddr )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucBondingHandle=%u, pxBdAddr="LOG_BDADDR_FORMAT,
                                  ( unsigned ) ucBondingHandle,
                                  LOG_BTBDADDR_T_VALUE( pxBdAddr ) );

  sl_status_t sl_status = sl_bt_nvm_save( prvBondingHandleToNvmKey( ucBondingHandle ),
                                          sizeof( *pxBdAddr ),
                                          ( uint8_t * ) pxBdAddr );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_nvm_save failed, sl_status=0x%x", sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/* Read the address of a bonded device, if any, from non-volatile memory */
BTStatus_t prvLoadBondedDeviceAddress( uint8_t ucBondingHandle,
                                       BTBdaddr_t * pxBdAddr )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucBondingHandle=%u", ( unsigned ) ucBondingHandle );

  size_t xExpectedLen = sizeof( *pxBdAddr );
  size_t xValueLen = 0;
  sl_status_t sl_status = sl_bt_nvm_load( prvBondingHandleToNvmKey( ucBondingHandle ),
                                          xExpectedLen,
                                          &xValueLen,
                                          ( uint8_t * ) pxBdAddr );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_nvm_load failed, sl_status=0x%x", sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }
  if( xValueLen < xExpectedLen )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_nvm_load returned %u bytes, expected %u",
                                    ( unsigned ) xValueLen, ( unsigned ) xExpectedLen );
    return eBTStatusFail;
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d, pxBdAddr="LOG_BDADDR_FORMAT,
                                 eBTStatusSuccess,
                                 LOG_BTBDADDR_T_VALUE( pxBdAddr ) );
  return eBTStatusSuccess;
}

/**
 * @brief Triggered when a new connection has been opened
 */
static void prvOnConnectionOpened( sl_bt_evt_connection_opened_t * pxEvent )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "advertiser=%d, connection=%d, bonding=%d",
                                  ( int ) pxEvent->advertiser,
                                  ( int ) pxEvent->connection,
                                  ( int ) pxEvent->bonding);

  /* Get the adapterIf context */
  SlBtAdapterIf_t * pxAdapterIf = prvGetAdapterIfByAdvSetHandle( pxEvent->advertiser );
  if( !pxAdapterIf )
  {
    SILABS_BLE_LOG_FUNC_EXIT_INFO( "No adapter for advertiser=%d", ( int ) pxEvent->advertiser );
    return;
  }

  /* If the device was bonded, load the original address that it connected with */
  BTBdaddr_t xRemoteAddr;
  BTStatus_t xAddressStatus = eBTStatusNotReady;
  if( pxEvent->bonding != SL_BT_INVALID_BONDING_HANDLE )
  {
    xAddressStatus = prvLoadBondedDeviceAddress( pxEvent->bonding, &xRemoteAddr );
  }

  /* If we didn't already get an address from a stored bonding, use the adress
  the device connected with on this connection */
  if( xAddressStatus != eBTStatusSuccess )
  {
    memcpy( xRemoteAddr.ucAddress, &pxEvent->address, sizeof( xRemoteAddr.ucAddress ) );
  }

  /* Save the new connection before callback */
  prvSaveNewConnection( pxAdapterIf->xClient.ucHandle,
                        pxEvent->connection,
                        pxEvent->bonding,
                        &xRemoteAddr );

  /* Call the application */
  if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxOpenCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxOpenCb", "connection=%d, ucHandle=0x%x, xRemoteAddr="LOG_BDADDR_FORMAT,
                                 ( int ) pxEvent->connection,
                                 pxAdapterIf->xClient.ucHandle,
                                 LOG_BTBDADDR_T_VALUE(&xRemoteAddr));
    pxBleAdapterCallbacks->pxOpenCb( pxEvent->connection,
                                     eBTGattStatusSuccess,
                                     pxAdapterIf->xClient.ucHandle,
                                     &xRemoteAddr );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxOpenCb" );
  }

  /* Let the GATT Server call the corresponding callback */
  prvGattServerOpenConnectionEventCb( pxEvent->connection, &xRemoteAddr );

  /* Handle the implicit stop of the advertiser */
  prvStopAdvertiserImplicitly( pxAdapterIf );

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", eBTStatusSuccess );
}

/**
 * @brief Handle a new pairing event.
 *
 */
static void prvOnNewPairingEvent( SlBtConnections_t *pConnection )
{
  if( ( pConnection->ucFlags & CONNECTION_FLAG_NEW_PAIRING ) != 0 )
  {
    pConnection->ucFlags &= ~CONNECTION_FLAG_NEW_PAIRING;

    /* If the device was bonded, save the address that it connected with */
    if( pConnection->ucBondingHandle != SL_BT_INVALID_BONDING_HANDLE )
    {
      /* We ignore any errors from saving the address. If we failed, we
      won't be able to report the same address in future connections, but
      there's no need to fail this connection. */
      prvSaveBondedDeviceAddress( pConnection->ucBondingHandle,
                                  &pConnection->xRemoteAddress );
    }

    prvBtHalManagerOnNewPairingEvent( pConnection->ucBondingHandle,
                                      &pConnection->xRemoteAddress,
                                      pConnection->ucSecurityLevel);
  }
  else
  {
    pConnection->ucFlags |= CONNECTION_FLAG_NEW_PAIRING;
  }
}

/**
 * @brief Triggered when parameters on a connection has changed
 *
 * See @ref prvOnBonded for new pairing detection and handling
 */
static void prvOnConnectionParameters( sl_bt_evt_connection_parameters_t * pxEvent )
{
  SlBtConnections_t * pConnection = prvGetConnection( pxEvent->connection );
  if( !pConnection )
  {
    return;
  }

  /* If the change was originated by the peer, we won't have an active
  connection parameter update request pending. In this case we'll default to
  reporting min and max with the same value as the selected interval, as we
  don't have the actual min and max that the peer requested. */
  uint16_t usMinInterval = pxEvent->interval;
  uint16_t usMaxInterval = pxEvent->interval;
  if( ( pConnection->usMinInterval != 0 ) &&
      ( pConnection->usMaxInterval != 0 ) )
  {
    /* A local request was pending. Use the min and max range that was stored
    and clear the values to indicate the request is no longer pending. */
    usMinInterval = pConnection->usMinInterval;
    usMaxInterval = pConnection->usMaxInterval;
    pConnection->usMinInterval = 0;
    pConnection->usMaxInterval = 0;
  }

  /* Report the parameter update to the application */
  if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxConnParameterUpdateCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxConnParameterUpdateCb", "xRemoteAddress="LOG_BDADDR_FORMAT", "
                                  "ulMinInterval=%u, ulMaxInterval=%u, latency=%u, interval=%u, timeout=%u",
                                  LOG_BTBDADDR_T_VALUE( &pConnection->xRemoteAddress ),
                                  ( unsigned ) usMinInterval,
                                  ( unsigned ) usMaxInterval,
                                  ( unsigned ) pxEvent->latency,
                                  ( unsigned ) pxEvent->interval,
                                  ( unsigned ) pxEvent->timeout );
    pxBleAdapterCallbacks->pxConnParameterUpdateCb( eBTStatusSuccess,
                                                    &pConnection->xRemoteAddress,
                                                    usMinInterval,
                                                    usMaxInterval,
                                                    pxEvent->latency,
                                                    pxEvent->interval,
                                                    pxEvent->timeout );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxConnParameterUpdateCb" );
  }

  /* Check if this is a new pairing. See @ref prvOnBonded for the algorithm */
  BTSecurityLevel_t xNewSecurityLevel = prvConvertSecurityMode( pxEvent->security_mode );
  bool bNewPairing =
    ( pConnection->ucSecurityLevel == eBTSecLevelNoSecurity &&
      xNewSecurityLevel > eBTSecLevelNoSecurity );

  if( bNewPairing )
  {
    pConnection->ucSecurityLevel = xNewSecurityLevel;
    prvOnNewPairingEvent( pConnection );
  }
}

/**
 * @brief Triggered when a connection has been closed
 */
static void prvOnConnectionClosed( sl_bt_evt_connection_closed_t * pxEvent )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "connection=%d", ( int ) pxEvent->connection);

  SlBtConnections_t * pConnection = prvGetConnection( pxEvent->connection );
  if ( pConnection != NULL )
  {
    /* Give the app the callback */
    if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxCloseCb )
    {
      SILABS_BLE_LOG_CB_CALL_DEBUG( "pxCloseCb", "connection=%d, event->reason=%d, ucHandle=0x%x, xRemoteAddress="LOG_BDADDR_FORMAT,
                                   ( int ) pxEvent->connection,
                                   ( int ) pxEvent->reason,
                                   pConnection->ucAdapterIf,
                                   LOG_BTBDADDR_T_VALUE(&pConnection->xRemoteAddress));
      pxBleAdapterCallbacks->pxCloseCb( pxEvent->connection,
                                        prvSlStatusToBTGattStatus(pxEvent->reason),
                                        pConnection->ucAdapterIf,
                                        &pConnection->xRemoteAddress );
      SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxCloseCb" );
    }

    SILABS_BLE_LOG(SILABS_BLE_LOG_DEBUG,
                       SILABS_BLE_HAL,
                       "%s: connection closed", __func__);

    /* Let the GATT Server call the corresponding callback */
    prvGattServerCloseConnectionEventCb( pxEvent->connection, &pConnection->xRemoteAddress );

    /* Delete the saved connection after callback */
    memset( pConnection, 0, sizeof( SlBtConnections_t ) );
    pConnection->ucAdapterIf = SL_BT_INVALID_IF_HANDLE;
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", eBTStatusSuccess );
}

/**
 * @brief Triggered when a new pairing completes.
 *
 * Algorithm of detecting a new pairing and fetching all necessary data
 * required by BTPairingStateChangedCallback_t:
 *
 *   1) When sl_bt_evt_connection_opened event arrives, set the security level
 *   to 0 and update the bonding handle value in cache.
 *
 *   2) When sl_bt_evt_sm_bonded event arrives, update the bonding handle in
 *   cache. If new-paired flag is not set, set the flag. Otherwise, clear the
 *   flag and call prvBtHalManagerOnNewPairingEvent.
 *
 *   3) When sl_bt_evt_connection_parameters arrives, if security level increases
 *   from 0, and if either the bonding handle in cache is invalid bonding or
 *   new-paired flag is already set, this is a new pairing. In this case,
 *   update the security level in cache. If new-paired flag is not set, set the
 *   flag. Otherwise, clear the flag and call prvBtHalManagerOnNewPairingEvent.
 */
static void prvOnBonded( sl_bt_evt_sm_bonded_t * pxEvent )
{
  SlBtConnections_t * pConnection = prvGetConnection( pxEvent->connection );
  if( pConnection )
  {
    pConnection->ucBondingHandle = pxEvent->bonding;
    prvOnNewPairingEvent( pConnection );
  }
}

/**
 * @brief Request RSSI for a given remote device.
 *
 * This function implements pxReadRemoteRssi function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvReadRemoteRssi( uint8_t ucAdapterIf,
                                     const BTBdaddr_t * pxBdAddr )
{
  /* This function is not supported */
  (void) ucAdapterIf;
  (void) pxBdAddr;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Setup scan filter params.
 *
 * This function implements pxScanFilterParamSetup function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvScanFilterParamSetup( BTGattFiltParamSetup_t xFiltParam )
{
  /* This function is not supported */
  (void) xFiltParam;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Configure a scan filter condition.
 *
 * This function implements pxScanFilterAddRemove function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvScanFilterAddRemove( uint8_t ucAdapterIf,
                                          uint32_t ulAction,
                                          uint32_t ulFiltType,
                                          uint32_t ulFiltIndex,
                                          uint32_t ulCompanyId,
                                          uint32_t ulCompanyIdMask,
                                          const BTUuid_t * pxUuid,
                                          const BTUuid_t * pxUuidMask,
                                          const BTBdaddr_t * pxBdAddr,
                                          char cAddrType,
                                          size_t xDataLen,
                                          char * pcData,
                                          size_t xMaskLen,
                                          char * pcMask )
{
  /* This function is not supported */
  (void) ucAdapterIf;
  (void) ulAction;
  (void) ulFiltType;
  (void) ulFiltIndex;
  (void) ulCompanyId;
  (void) ulCompanyIdMask;
  (void) pxUuid;
  (void) pxUuidMask;
  (void) pxBdAddr;
  (void) cAddrType;
  (void) xDataLen;
  (void) pcData;
  (void) xMaskLen;
  (void) pcMask;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Clear all scan filter conditions for specific filter index.
 *
 * This function implements pxScanFilterClear function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvScanFilterClear( uint8_t ucAdapterIf,
                                      uint32_t ulFiltIndex )
{
  /* This function is not supported */
  (void) ucAdapterIf;
  (void) ulFiltIndex;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Enable / disable scan filter feature.
 *
 * This function implements pxScanFilterEnable function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvScanFilterEnable( uint8_t ucAdapterIf,
                                       bool bEnable )
{
  /* This function is not supported */
  (void) ucAdapterIf;
  (void) bEnable;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Determine the type of the remote device (LE, BR/EDR, Dual-mode).
 *
 * This function implements pxGetDeviceType function in @ref BTBleAdapter_t.
 */
static BTTransport_t prvGetDeviceType( const BTBdaddr_t * pxBdAddr )
{
  /* This function is not supported */
  (void) pxBdAddr;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return BTTransportLe;
}

/**
 * @brief Convert advertiser parameters to types used in SL BT API.
 *
 * @param[in] pxParams Advertising parameters received from use of BLE HAL
 * @param[out] pxSlBtAdvParams Filled with corresponding parameters for SL BT API
 *
 *
 */
static BTStatus_t prvConvertAdvParams( BTGattAdvertismentParams_t * pxParams,
                                       SlBtAdvParams_t * pxSlBtAdvParams )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "%s", "" );

  /* Log the input parameters */
  SILABS_BLE_LOG_PRINT_DEBUG(
    "  usMinAdvInterval=%"PRIu16", usMaxAdvInterval=%"PRIu16", usTimeout=%"PRIu16,
    pxParams->usMinAdvInterval, pxParams->usMaxAdvInterval, pxParams->usTimeout );

  SILABS_BLE_LOG_PRINT_DEBUG(
    "  ucChannelMap=0x%x, ucPrimaryAdvertisingPhy=%u, ucSecondaryAdvertisingPhy=%u",
    ( unsigned ) pxParams->ucChannelMap,
    ( unsigned ) pxParams->ucPrimaryAdvertisingPhy,
    ( unsigned ) pxParams->ucSecondaryAdvertisingPhy );

  SILABS_BLE_LOG_PRINT_DEBUG("  ucTxPower=%u, xAddrType=%u",
                             ( unsigned ) pxParams->ucTxPower,
                             ( unsigned ) pxParams->xAddrType );

  /* Clear the output */
  memset(pxSlBtAdvParams, 0, sizeof(*pxSlBtAdvParams));

  /* Use stack-specific default intervals if the input params do not specify them */
  pxSlBtAdvParams->ulMinInterval = 100;
  pxSlBtAdvParams->ulMaxInterval = 200;
  if( pxParams->usMinAdvInterval > 0 )
  {
    pxSlBtAdvParams->ulMinInterval = pxParams->usMinAdvInterval;
  }
  if( pxParams->usMaxAdvInterval > 0 )
  {
    pxSlBtAdvParams->ulMaxInterval = pxParams->usMaxAdvInterval;
  }

  /* Both APIs specify timeout in units of 10 ms and use 0 for infinite */
  pxSlBtAdvParams->usTimeout = pxParams->usTimeout;

  /* Default to all channels if none are specified */
  pxSlBtAdvParams->ucChannelMap = 7;
  if( pxParams->ucChannelMap != 0 )
  {
    pxSlBtAdvParams->ucChannelMap = pxParams->ucChannelMap;
  }

  /* Convert the PHY selection */
  pxSlBtAdvParams->ucPrimaryPhy = prvBlePhyToSlGapPhyType(pxParams->ucPrimaryAdvertisingPhy);
  pxSlBtAdvParams->ucSecondaryPhy = prvBlePhyToSlGapPhyType(pxParams->ucSecondaryAdvertisingPhy);

  /* Convert the power level */
  BTStatus_t status = prvTxPowerIndexToSlBtPower( pxParams->ucTxPower,
                                                  &pxSlBtAdvParams->usRequestedPower );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "prvTxPowerIndexToSlBtPower(ucTxPower=%d) failed, status=%d",
                                    ( int ) pxParams->ucTxPower, status );
    return status;
  }

  /* Convert the address type */
  switch( pxParams->xAddrType )
  {
    case BTAddrTypePublic:
      pxSlBtAdvParams->ucAddressType = sl_bt_gap_public_address;
      break;

    case BTAddrTypeStaticRandom:
      pxSlBtAdvParams->ucAddressType = sl_bt_gap_static_address;
      break;

    case BTAddrTypeRandom:
      pxSlBtAdvParams->ucAddressType = sl_bt_gap_random_nonresolvable_address;
      break;

    case BTAddrTypeResolvable:
      pxSlBtAdvParams->ucAddressType = sl_bt_gap_random_resolvable_address;
      break;

    default:
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid address type %d", ( int ) pxParams->xAddrType );
      return eBTStatusParamInvalid;
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/*
 * Index and type bit mask for the most-significant byte of a Bluetooth address.
 * See Bluetooth Core Specification version 5.2, Volume 6, Part B, Section
 * 1.3.2.2 Private device address generation.
 */
#define BD_ADDR_TYPE_BYTE_INDEX             (5)
#define BD_ADDR_TYPE_MASK                   ((uint8_t) 0xC0)
#define BD_ADDR_TYPE_NON_RESOLVABLE_PRIVATE ((uint8_t) 0x00)
#define BD_ADDR_TYPE_STATIC_RANDOM          ((uint8_t) 0xC0)

/**
 * @brief Generate a random address of the specified type.
 *
 * @param[out] pxAddress Filled with the generated random address
 * @param[in] xAddressType The address type to generate
 *
 * @return SL_STATUS_OK if successful, otherwise an error code
 */
static sl_status_t prvGenerateRandomAddress(bd_addr * pxAddress,
                                            sl_bt_gap_address_type_t xAddressType)
{
  /* Get random bytes to construct a random address */
  size_t dataLen = 0;
  sl_status_t sl_status = sl_bt_system_get_random_data( sizeof(pxAddress->addr),
                                                        sizeof(pxAddress->addr),
                                                        &dataLen,
                                                        pxAddress->addr );

  /* Make sure we got all the bytes we requested */
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to get random data, sl_status=0x%x", sl_status );
    return sl_status;
  }
  if( dataLen < sizeof( pxAddress->addr ) )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to get enough random data, %u < %u",
                                    dataLen, sizeof( pxAddress->addr ) );
    return SL_STATUS_FAIL;
  }

  /* Set the type bits to indicate the correct type */
  pxAddress->addr[BD_ADDR_TYPE_BYTE_INDEX] &= ~BD_ADDR_TYPE_MASK;
  if( xAddressType == sl_bt_gap_static_address )
  {
    pxAddress->addr[BD_ADDR_TYPE_BYTE_INDEX] |= BD_ADDR_TYPE_STATIC_RANDOM;
  }
  else if( xAddressType == sl_bt_gap_random_nonresolvable_address )
  {
    pxAddress->addr[BD_ADDR_TYPE_BYTE_INDEX] |= BD_ADDR_TYPE_NON_RESOLVABLE_PRIVATE;
  }
  else
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid address type %d", ( int ) xAddressType );
    return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

/**
 * @brief Set advertiser parameters to SL BT API.
 *
 * @param[in] ucAdvSetHandle Advertisement set handle
 * @param[in] pxSlBtAdvParams Advertising parameters to set
 * @param[out] psSetTxPower Set TX power level, in the units used in the SL BT API
 *
 * @return SL_STATUS_OK if succesful, otherwise an error code
 */
static sl_status_t prvSlBtSetAdvParams( uint8_t ucAdvSetHandle,
                                        const SlBtAdvParams_t * pxSlBtAdvParams,
                                        int16_t * psSetTxPower )
{
  bd_addr address = { 0 };
  bd_addr addressOut = { 0 };
  sl_status_t sl_status = SL_STATUS_FAIL;

  /* Set the address type */
  if( pxSlBtAdvParams->ucAddressType == sl_bt_gap_public_address )
  {
    /* Clear the random address in order to use the default advertiser address
     * which is either the public device address programmed at production or the
     * address written into persistent storage using @ref
     * sl_bt_system_set_identity_address command. */
    SILABS_BLE_LOG_PRINT_DEBUG( "  clear random address%s", "" );
    sl_status = sl_bt_advertiser_clear_random_address( ucAdvSetHandle );
    if( sl_status != SL_STATUS_OK )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_advertiser_clear_random_address() failed, sl_status=0x%x",
                                      sl_status );
      return sl_status;
    }
  }
  else
  {
    /* The address is one of the random address types. See which one. */
    if( pxSlBtAdvParams->ucAddressType == sl_bt_gap_static_address )
    {
      /* Advertisers with static random address use the same shared address.
       * Generate it now if we don't have it already. */
      sl_status = SL_STATUS_OK;
      if( !bHaveAdvStaticRandomAddr )
      {
        sl_status = prvGenerateRandomAddress(&xAdvStaticRandomAddr, sl_bt_gap_static_address);
        if( sl_status != SL_STATUS_OK )
        {
          SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to generate static random address, sl_status=0x%x",
                                          sl_status );
          return sl_status;
        }
        bHaveAdvStaticRandomAddr = true;
      }

      /* Copy the shared address */
      memcpy( &address, &xAdvStaticRandomAddr, sizeof( address ) );
      SILABS_BLE_LOG_PRINT_DEBUG( "  use static random address "LOG_BDADDR_FORMAT,
                                  LOG_BDADDR_VALUE(&address) );
    }
    else if( pxSlBtAdvParams->ucAddressType == sl_bt_gap_random_nonresolvable_address )
    {
      /* Advertisers that use a random non-resolvable address get a fresh
       * random address */
      sl_status = prvGenerateRandomAddress(&address, sl_bt_gap_random_nonresolvable_address);
      if( sl_status != SL_STATUS_OK )
      {
        SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to generate non-resolvable random address, sl_status=0x%x",
                                        sl_status );
        return sl_status;
      }
      SILABS_BLE_LOG_PRINT_DEBUG( "  use random non-resolvable address "LOG_BDADDR_FORMAT,
                                  LOG_BDADDR_VALUE(&address) );
    }
    else
    {
      /* The type is a private resolvable random address. The Bluetooth stack
       * will generate the address internally and ignores the passed address. */
      sl_status = SL_STATUS_OK;
      SILABS_BLE_LOG_PRINT_DEBUG( "  let the stack generate private resolvable address%s", "" );
    }

    /* Set random address for this advertiser */
    sl_status = sl_bt_advertiser_set_random_address( ucAdvSetHandle,
                                                     pxSlBtAdvParams->ucAddressType,
                                                     address, &addressOut);
    if( sl_status != SL_STATUS_OK )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_advertiser_set_random_address() failed, sl_status=0x%x",
                                      sl_status );
      return sl_status;
    }

    SILABS_BLE_LOG_PRINT_INFO( "Current random address is xx:xx:xx:xx:%02x:%02x",
                               (unsigned)addressOut.addr[1], (unsigned)addressOut.addr[0]);
  }

  /* Set timing parameters */
  sl_status = sl_bt_advertiser_set_timing( ucAdvSetHandle,
                                           pxSlBtAdvParams->ulMinInterval,
                                           pxSlBtAdvParams->ulMaxInterval,
                                           pxSlBtAdvParams->usTimeout, 0 );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_advertiser_set_timing() failed, sl_status=0x%x",
                                    sl_status );
    return sl_status;
  }

  /* Set the advertising phy */
  sl_status = sl_bt_advertiser_set_phy( ucAdvSetHandle,
                                        pxSlBtAdvParams->ucPrimaryPhy,
                                        pxSlBtAdvParams->ucSecondaryPhy );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_advertiser_set_phy() failed, sl_status=0x%x",
                                    sl_status );
    return sl_status;
  }

  /* Set the channel map */
  sl_status = sl_bt_advertiser_set_channel_map( ucAdvSetHandle,
                                                pxSlBtAdvParams->ucChannelMap );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_advertiser_set_channel_map() failed, sl_status=0x%x",
                                    sl_status );
    return sl_status;
  }

  /* Set the power level */
  sl_status = sl_bt_advertiser_set_tx_power( ucAdvSetHandle,
                                             pxSlBtAdvParams->usRequestedPower,
                                             psSetTxPower );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_advertiser_set_tx_power() failed, sl_status=0x%x",
                                    sl_status );
    return sl_status;
  }

  return SL_STATUS_OK;
}

/* Advertisement data types specified at
 * https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile */
#define ADV_DATA_TYPE_FLAGS                      ((uint8_t) 0x01)
#define ADV_DATA_TYPE_INCOMPLETE_16BIT_UUIDS     ((uint8_t) 0x02)
#define ADV_DATA_TYPE_COMPLETE_16BIT_UUIDS       ((uint8_t) 0x03)
#define ADV_DATA_TYPE_INCOMPLETE_32BIT_UUIDS     ((uint8_t) 0x04)
#define ADV_DATA_TYPE_COMPLETE_32BIT_UUIDS       ((uint8_t) 0x05)
#define ADV_DATA_TYPE_INCOMPLETE_128BIT_UUIDS    ((uint8_t) 0x06)
#define ADV_DATA_TYPE_COMPLETE_128BIT_UUIDS      ((uint8_t) 0x07)
#define ADV_DATA_TYPE_SHORTENED_LOCAL_NAME       ((uint8_t) 0x08)
#define ADV_DATA_TYPE_COMPLETE_LOCAL_NAME        ((uint8_t) 0x09)
#define ADV_DATA_TYPE_TX_POWER_LEVEL             ((uint8_t) 0x0A)
#define ADV_DATA_TYPE_APPEARANCE                 ((uint8_t) 0x19)
#define ADV_DATA_TYPE_CONNECTION_INTERVAL_RANGE  ((uint8_t) 0x12)
#define ADV_DATA_TYPE_SERVICE_DATA_16BIT_UUID    ((uint8_t) 0x16)
#define ADV_DATA_TYPE_MANUFACTURER_DATA          ((uint8_t) 0xFF)

/*
 * Flag values from Bluetooth Core Specification Supplement, Part A, section 1.3.
 */
#define ADV_FLAG_GENERAL_DISCOVERABLE ((uint8_t) 0x02)
#define ADV_FLAG_BR_EDR_NOT_SUPPORTED ((uint8_t) 0x04)

/**
 * @brief Macro to set a uint16_t data item to advertisement data
 */
#define PRV_SET_ADV_DATA_UINT16(ptr, value) \
  do {                                      \
    uint16_t _value = (value);              \
    (ptr)[0] = (uint8_t) (_value & 0xFF);   \
    (ptr)[1] = (uint8_t) (_value >> 8);     \
  } while( 0 )

/* Packet type values for @ref sl_bt_advertiser_set_data() */
#define ADV_PACKET_TYPE_ADVERTISEMENT ((uint8_t) 0) /* Advertising packets */
#define ADV_PACKET_TYPE_SCAN_RESPONSE ((uint8_t) 1) /* Scan response packets */

/**
 * @brief Append advertisement data to a buffer
 *
 * @param[in,out] ppBufPos Position in the output buffer
 * @param[in,out] pSizeRemaining Number of bytes remaining in the output buffer
 * @param[in] ucDataType Type of the data, one of ADV_DATA_TYPE_* defines
 * @param[in] ucDataLen Length of the data
 * @param[in] pData Data bytes to append
 *
 * @return eBTStatusSuccess on success, otherwise an error code
 */
static BTStatus_t prvAppendAdvData( uint8_t ** ppBufPos,
                                    uint32_t * pSizeRemaining,
                                    uint8_t ucDataType,
                                    uint8_t ucDataLen,
                                    const void * pData )
{
  /* Make sure the data fits. We need one extra byte for type and another for length. */
  uint32_t entrySize = ucDataLen + 1 + 1;
  if( *pSizeRemaining < entrySize )
  {
    return eBTStatusParamInvalid;
  }

  /* Set the length, type, and data */
  uint8_t * pBufPos = *ppBufPos;
  pBufPos[0] = ucDataLen + 1; /* + 1 byte for the type */
  pBufPos[1] = ucDataType;
  if( ucDataLen > 0 )
  {
    memcpy(&pBufPos[2], pData, ucDataLen);
  }

  /* Update the outputs */
  (*ppBufPos) += entrySize;
  (*pSizeRemaining) -= entrySize;

  return eBTStatusSuccess;
}

/** @brief Structure to collect the properties of one type of service UUID */
typedef struct
{
  uint8_t ucDataTypeIncomplete; /**< Adv data type value to indicate an incomplete list */
  uint8_t ucDataTypeComplete;   /**< Adv data type value to indicate a complete list */
  uint8_t ucLength;             /**< Length of the UUID */
} SlBtServiceUuid_t;

/* Service UUID indexes */
#define SERVICE_UUID_16BIT_INDEX  0
#define SERVICE_UUID_32BIT_INDEX  1
#define SERVICE_UUID_128BIT_INDEX 2
#define NUM_SERVICE_UUID_TYPES    3

/** @brief Structure to collect the properties of one type of service UUID */
static const SlBtServiceUuid_t xServiceUuidProps[NUM_SERVICE_UUID_TYPES] =
{
  { ADV_DATA_TYPE_INCOMPLETE_16BIT_UUIDS,  ADV_DATA_TYPE_COMPLETE_16BIT_UUIDS,   2 },
  { ADV_DATA_TYPE_INCOMPLETE_32BIT_UUIDS,  ADV_DATA_TYPE_COMPLETE_32BIT_UUIDS,   4 },
  { ADV_DATA_TYPE_INCOMPLETE_128BIT_UUIDS, ADV_DATA_TYPE_COMPLETE_128BIT_UUIDS, 16 },
};

/**
 * @brief Generate advertisement data.
 *
 * See @ref pxSetAdvData function in @ref BTBleAdapter_t for description of the parameters.
 */
static BTStatus_t prvGenerateAdvertisementData( BTGattAdvertismentParams_t * pxParams,
                                                bool bIncludeAdvFlags,
                                                uint16_t usManufacturerLen,
                                                char * pcManufacturerData,
                                                uint16_t usServiceDataLen,
                                                char * pcServiceData,
                                                BTUuid_t * pxServiceUuid,
                                                size_t xNbServices,
                                                int16_t usSlBtSetPower,
                                                uint8_t * pucBuffer,
                                                size_t xBufferSize,
                                                size_t * pxAdvDataLen )
{
  uint8_t * pBufPos = pucBuffer;
  uint32_t sizeRemaining = xBufferSize;
  BTStatus_t status = eBTStatusSuccess;

  /* Optionally append advertisement flags */
  if( bIncludeAdvFlags )
  {
    uint8_t flags = ADV_FLAG_GENERAL_DISCOVERABLE | ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    status = prvAppendAdvData( &pBufPos, &sizeRemaining, ADV_DATA_TYPE_FLAGS,
                               sizeof(flags), &flags );
    if( status != eBTStatusSuccess )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to add flags, status=%d", status );
      return status;
    }
  }

  /* Optionally append device name */
  if( pxParams->ucName.xType != BTGattAdvNameNone )
  {
    /* Get the complete name length */
    uint8_t nameType = ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
    uint8_t completeNameLen = (uint8_t) xSlBtHalManager.xDeviceNameLen;
    uint8_t usedNameLen = completeNameLen;

    /* Shorten the name if needed */
    if( pxParams->ucName.xType == BTGattAdvNameComplete )
    {
      /* Already OK */
    }
    else if( pxParams->ucName.xType == BTGattAdvNameShort )
    {
      if( pxParams->ucName.ucShortNameLen < completeNameLen )
      {
        usedNameLen = pxParams->ucName.ucShortNameLen;
        nameType = ADV_DATA_TYPE_SHORTENED_LOCAL_NAME;
      }
    }
    else
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid name type %d", ( int ) pxParams->ucName.xType );
      return eBTStatusParamInvalid;
    }

    /* Append the name */
    if( usedNameLen > 0 )
    {
      status = prvAppendAdvData( &pBufPos, &sizeRemaining, nameType,
                                 usedNameLen, xSlBtHalManager.pucDeviceName );
      if( status != eBTStatusSuccess )
      {
        SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to add name, status=%d", status );
        return status;
      }
    }
  }

  /* Optionally append appearance */
  if( pxParams->ulAppearance != 0 )
  {
    /* Verify validity */
    if( pxParams->ulAppearance > UINT16_MAX )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid appearance value %"PRIu32, pxParams->ulAppearance );
      return eBTStatusParamInvalid;
    }

    /* Appearance item is two octets long */
    uint8_t appearanceBuf[2];
    PRV_SET_ADV_DATA_UINT16( appearanceBuf, (uint16_t) pxParams->ulAppearance );

    status = prvAppendAdvData( &pBufPos, &sizeRemaining, ADV_DATA_TYPE_APPEARANCE,
                               sizeof(appearanceBuf), appearanceBuf );
    if( status != eBTStatusSuccess )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to add appearance, status=%d", status );
      return status;
    }
  }

  /* Optionally append TX power level */
  if( pxParams->bIncludeTxPower )
  {
    /* SL BT API defines power levels in units of 0.1 dBm. If we have a value in the valid range,
     * append the power level data item. */
    if( (usSlBtSetPower >= -1270) &&
        (usSlBtSetPower <= 1270) )
    {
      int8_t txPowerLevel = usSlBtSetPower / 10;
      status = prvAppendAdvData( &pBufPos, &sizeRemaining, ADV_DATA_TYPE_TX_POWER_LEVEL,
                                 sizeof(txPowerLevel), &txPowerLevel );
      if( status != eBTStatusSuccess )
      {
        SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to add TX power level, status=%d", status );
        return status;
      }
    }
  }

  /* Optionally append connection interval range */
  if( (pxParams->ulMinInterval != 0) && (pxParams->ulMaxInterval != 0) )
  {
    /* Verify validity */
    if( (pxParams->ulMinInterval > UINT16_MAX) || (pxParams->ulMaxInterval > UINT16_MAX) )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid ulMinInterval=%"PRIu32" or ulMaxInterval=%"PRIu32,
                                      pxParams->ulMinInterval, pxParams->ulMaxInterval );
      return eBTStatusParamInvalid;
    }

    /* The interval range data is 4 octets long (Bluetooth Core Specification Supplement, Part A,
     * section 1.9). */
    uint8_t intervalRangeBuf[4];
    PRV_SET_ADV_DATA_UINT16( &intervalRangeBuf[0], (uint16_t) pxParams->ulMinInterval );
    PRV_SET_ADV_DATA_UINT16( &intervalRangeBuf[2], (uint16_t) pxParams->ulMaxInterval );

    status = prvAppendAdvData( &pBufPos, &sizeRemaining, ADV_DATA_TYPE_CONNECTION_INTERVAL_RANGE,
                               sizeof(intervalRangeBuf), intervalRangeBuf );
    if( status != eBTStatusSuccess )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to add connection interval range, status=%d", status );
      return status;
    }
  }

  /* Optionally append service UUIDs */
  if( ( xNbServices > 0 ) && ( pxServiceUuid != NULL ) )
  {
    /* Get the number of UUIDs per type and the UUID to include in the advertisement */
    uint8_t numUuids[NUM_SERVICE_UUID_TYPES] = { 0 };
    const void * uuidToInclude[NUM_SERVICE_UUID_TYPES] = { NULL };

    for( uint32_t i = 0; i < xNbServices; i++ )
    {
      /* Map the type to an index */
      uint32_t typeIndex = 0;
      switch( pxServiceUuid[i].ucType )
      {
        case eBTuuidType16:  typeIndex = SERVICE_UUID_16BIT_INDEX;  break;
        case eBTuuidType32:  typeIndex = SERVICE_UUID_32BIT_INDEX;  break;
        case eBTuuidType128: typeIndex = SERVICE_UUID_128BIT_INDEX; break;
        default:
          SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid UUID type %d", ( int ) pxServiceUuid[i].ucType );
          return eBTStatusParamInvalid;
      }

      /* Remember the pointer to the first UUID of this type */
      if( numUuids[typeIndex] == 0 )
      {
        uuidToInclude[typeIndex] = &pxServiceUuid[i].uu;
      }
      numUuids[typeIndex]++;
    }

    /* Append the data for each type */
    for( uint32_t i = 0; i < NUM_SERVICE_UUID_TYPES; i++ )
    {
      if( numUuids[i] > 0 )
      {
        uint8_t ucDataType = numUuids[i] > 1 ?
          xServiceUuidProps[i].ucDataTypeIncomplete : xServiceUuidProps[i].ucDataTypeComplete;
        status = prvAppendAdvData( &pBufPos, &sizeRemaining, ucDataType,
                                   xServiceUuidProps[i].ucLength, uuidToInclude[i] );
        if( status != eBTStatusSuccess )
        {
          SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to add service UUIDs, status=%d", status );
          return status;
        }
      }
    }
  }

  /* Optionally append service data */
  if( ( usServiceDataLen > 0 ) && ( pcServiceData != NULL ) )
  {
    status = prvAppendAdvData( &pBufPos, &sizeRemaining, ADV_DATA_TYPE_SERVICE_DATA_16BIT_UUID,
                               usServiceDataLen, pcServiceData );
    if( status != eBTStatusSuccess )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to add service data, status=%d", status );
      return status;
    }
  }

  /* Optionally append manufacturer data */
  if( ( usManufacturerLen > 0 ) && ( pcManufacturerData != NULL ) )
  {
    status = prvAppendAdvData( &pBufPos, &sizeRemaining, ADV_DATA_TYPE_MANUFACTURER_DATA,
                               usManufacturerLen, pcManufacturerData );
    if( status != eBTStatusSuccess )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to add manufacturer data, status=%d", status );
      return status;
    }
  }

  /* Set the final size */
  *pxAdvDataLen = xBufferSize - sizeRemaining;

  return eBTStatusSuccess;
}

/**
 * @brief Configure the advertiser in the given adapter context.
 *
 * @param[in] pxAdapterIf The adapter context to configure
 * @param[in] pxParams Advertising parameters to use
 *
 * @return eBTStatusSuccess on success, otherwise an error code
 */
static BTStatus_t prvConfigureAdapterAdvertiser( SlBtAdapterIf_t * pxAdapterIf,
                                                 BTGattAdvertismentParams_t * pxParams )
{
  /* Remember the advertising connectable mode */
  BTStatus_t status = prvAdvEventPropsToSlAdvConnectableMode( pxParams->usAdvertisingEventProperties,
                                                              &pxAdapterIf->ucAdvConnectableMode );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to convert connectable mode %d, status=%d",
                                    ( int ) pxParams->usAdvertisingEventProperties, status );
    return status;
  }

  /* Convert the advertising parameters */
  SlBtAdvParams_t xSlBtAdvParams;
  status = prvConvertAdvParams( pxParams, &xSlBtAdvParams );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to convert advertisement params, status=%d", status );
    return status;
  }

  /* Set the advertising parameters */
  pxAdapterIf->sSetAdvPower = 0;
  sl_status_t sl_status = prvSlBtSetAdvParams( pxAdapterIf->ucAdvSetHandle,
                                               &xSlBtAdvParams,
                                               &pxAdapterIf->sSetAdvPower );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to set advertisement params, sl_status=0x%x", sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }

  return eBTStatusSuccess;
}

/**
 * @brief Set the advertisement data to SL BT stack.
 *
 * See @ref pxSetAdvData function in @ref BTBleAdapter_t for description of the parameters.
 */
static BTStatus_t prvSetAdvertisementData( uint8_t ucAdvSetHandle,
                                           BTGattAdvertismentParams_t * pxParams,
                                           uint16_t usManufacturerLen,
                                           char * pcManufacturerData,
                                           uint16_t usServiceDataLen,
                                           char * pcServiceData,
                                           BTUuid_t * pxServiceUuid,
                                           size_t xNbServices,
                                           int16_t usSlBtSetPower )
{
  /* Generate the advertisement data */
  uint8_t advBuf[MAX_LEGACY_ADV_DATA_LEN];
  size_t advDataLen = 0;
  bool bIncludeAdvFlags = !pxParams->bSetScanRsp;
  BTStatus_t status = prvGenerateAdvertisementData( pxParams, bIncludeAdvFlags,
                                                    usManufacturerLen, pcManufacturerData,
                                                    usServiceDataLen, pcServiceData,
                                                    pxServiceUuid, xNbServices, usSlBtSetPower,
                                                    advBuf, sizeof(advBuf),
                                                    &advDataLen );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to generate advertisement data, status=%d", status );
    return status;
  }

  /* Set the user data to the Bluetooth stack */
  uint8_t ucPacketType = pxParams->bSetScanRsp ?
    ADV_PACKET_TYPE_SCAN_RESPONSE : ADV_PACKET_TYPE_ADVERTISEMENT;
  sl_status_t sl_status = sl_bt_advertiser_set_data( ucAdvSetHandle, ucPacketType, advDataLen, advBuf );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_advertiser_set_data() failed, sl_status=0x%x", sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }

  return eBTStatusSuccess;
}

/**
 * @brief Set the advertising data or scan response data.
 *
 * This function implements pxSetAdvData function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvSetAdvData( uint8_t ucAdapterIf,
                                 BTGattAdvertismentParams_t * pxParams,
                                 uint16_t usManufacturerLen,
                                 char * pcManufacturerData,
                                 uint16_t usServiceDataLen,
                                 char * pcServiceData,
                                 BTUuid_t * pxServiceUuid,
                                 size_t xNbServices )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucAdapterIf=%d", (int) ucAdapterIf );

  /* Get the adapterIf context and create its advertiser set if necessary */
  SlBtAdapterIf_t * pxAdapterIf = NULL;
  BTStatus_t status = prvGetAdapterIfWithAdvSet( ucAdapterIf, SlBtHalAdvApiBasic, &pxAdapterIf );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to get adapter with advertiser, status=%d", status );
    return status;
  }

  /* Check parameters */
  if( pxParams == NULL )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "NULL params, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* Configure the advertiser in this adapter context */
  status = prvConfigureAdapterAdvertiser( pxAdapterIf, pxParams );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to configure, status=%d", status );
    return status;
  }

  /* Set the advertisement data */
  status = prvSetAdvertisementData( pxAdapterIf->ucAdvSetHandle,
                                    pxParams, usManufacturerLen, pcManufacturerData,
                                    usServiceDataLen, pcServiceData,
                                    pxServiceUuid, xNbServices,
                                    pxAdapterIf->sSetAdvPower );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to set data, status=%d", status );
    return status;
  }

  /* Give the callback to the application */
  if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxSetAdvDataCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxSetAdvDataCb", "status=%d", eBTStatusSuccess );
    pxBleAdapterCallbacks->pxSetAdvDataCb( eBTStatusSuccess );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxSetAdvDataCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Set the advertising data of complete 31 bytes or scan response data.
 *
 * This function implements pxSetAdvRawData function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvSetAdvRawData( uint8_t ucAdapterIf,
                                    uint8_t * pucData,
                                    uint8_t ucLen )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucAdapterIf=%d", (int) ucAdapterIf );

  /* Get the adapterIf context and create its advertiser set if necessary */
  SlBtAdapterIf_t * pxAdapterIf = NULL;
  BTStatus_t status = prvGetAdapterIfWithAdvSet( ucAdapterIf, SlBtHalAdvApiBasic, &pxAdapterIf );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to get adapter with advertiser, status=%d", status );
    return status;
  }

  /* This function is not supported */
  (void) pucData;
  (void) ucLen;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Request a connection parameter update.
 *
 * This function implements pxConnParameterUpdateRequest function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvConnParameterUpdateRequest( const BTBdaddr_t * pxBdAddr,
                                                 uint32_t ulMinInterval,
                                                 uint32_t ulMaxInterval,
                                                 uint32_t ulLatency,
                                                 uint32_t ulTimeout )
{
  /* Verify the address pointer first to avoid trying to log with NULL pointer */
  if( pxBdAddr == NULL )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "NULL pxBdAddr, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "pxBdAddr="LOG_BDADDR_FORMAT", "
                                  "ulMinInterval=%"PRIu32", ulMaxInterval=%"PRIu32", "
                                  "ulLatency=%"PRIu32", ulTimeout=%"PRIu32,
                                  LOG_BTBDADDR_T_VALUE( pxBdAddr ),
                                  ulMinInterval, ulMaxInterval, ulLatency, ulTimeout );

  /* Find the connection */
  SlBtConnections_t * pxConnection = prvGetConnectionByAddress( pxBdAddr );
  if ( pxConnection == NULL )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to find connection, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* Verify that the values are in valid range */
  if( ( ulMinInterval < MIN_CONNECTION_INTERVAL ) || ( ulMinInterval > MAX_CONNECTION_INTERVAL ) ||
      ( ulMaxInterval < MIN_CONNECTION_INTERVAL ) || ( ulMaxInterval > MAX_CONNECTION_INTERVAL ) ||
      ( ulLatency > MAX_SLAVE_LATENCY ) ||
      ( ulTimeout < MIN_CONNECTION_TIMEOUT ) || ( ulTimeout > MAX_CONNECTION_TIMEOUT ) )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid parameter, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* Remember the parameters requested for this connection */
  pxConnection->usMinInterval = (uint16_t) ulMinInterval;
  pxConnection->usMaxInterval = (uint16_t) ulMaxInterval;

  /* Set connection parameters with no limits on CE length */
  uint16_t usMinCeLength = 0;
  uint16_t usMaxCeLength = 0xFFFF;
  sl_status_t sl_status = sl_bt_connection_set_parameters(pxConnection->ucConnectionHandle,
                                                          (uint16_t) ulMinInterval,
                                                          (uint16_t) ulMaxInterval,
                                                          (uint16_t) ulLatency,
                                                          (uint16_t) ulTimeout,
                                                          usMinCeLength,
                                                          usMaxCeLength );
  if( sl_status != SL_STATUS_OK )
  {
    /* When failed, no local request for these parameters is pending. Clear the values. */
    pxConnection->usMinInterval = 0;
    pxConnection->usMaxInterval = 0;

    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_connection_set_parameters() failed, sl_status=0x%x",
                                    sl_status);
    return prvSlStatusToBTStatus( sl_status );
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", eBTStatusSuccess);
  return eBTStatusSuccess;
}

/**
 * @brief Sets the LE scan interval and window in units of N*0.625 msec.
 *
 * This function implements pxSetScanParameters function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvSetScanParameters( uint8_t ucAdapterIf,
                                        uint32_t ulScanInterval,
                                        uint32_t ulScanWindow )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG(
    "ucAdapterIf=%d, ulScanInterval=%"PRIu32", ulScanWindow=%"PRIu32,
    (int) ucAdapterIf, ulScanInterval, ulScanWindow );

  /* Get the adapter context just to verify it */
  SlBtAdapterIf_t * pxAdapterIf = prvGetAdapterIf( ucAdapterIf );
  if( !pxAdapterIf )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to get adapter, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* Verify that the values fit to the stack's 16-bit parameters */
  if( ( ulScanInterval > UINT16_MAX ) || ( ulScanWindow > UINT16_MAX ) )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid parameters, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* The stack function uses the same units and has the same behaviour as
   * expected of the BLE HAL, i.e. the parameter change takes effect the next
   * time scanning is started. */
  sl_status_t sl_status = sl_bt_scanner_set_timing( SCANNER_PHYS_1M,
                                                    (uint16_t) ulScanInterval,
                                                    (uint16_t) ulScanWindow );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to set timing, sl_status=0x%x", sl_status);
    return prvSlStatusToBTStatus( sl_status );
  }

  /* Give the callback to the application */
  if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxScanParameterSetupCompletedCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxScanParameterSetupCompletedCb", "ucAdapterIf=%d, status=%d",
                                  ucAdapterIf, eBTStatusSuccess );
    pxBleAdapterCallbacks->pxScanParameterSetupCompletedCb( ucAdapterIf,
                                                            eBTStatusSuccess );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxScanParameterSetupCompletedCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess);
  return eBTStatusSuccess;
}

/**
 * @brief Setup the parameters as per spec, user manual specified values and enable multi ADV.
 *
 * This function implements pxMultiAdvEnable function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvMultiAdvEnable( uint8_t ucAdapterIf,
                                     BTGattAdvertismentParams_t * xAdvParams )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "ucAdapterIf=%d", (int) ucAdapterIf );

  /* Check parameters */
  if( xAdvParams == NULL )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "NULL xAdvParams, status=%d",
                              eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* Get the adapterIf context and create its advertiser set if necessary */
  SlBtAdapterIf_t * pxAdapterIf = NULL;
  BTStatus_t status = prvGetAdapterIfWithAdvSet( ucAdapterIf, SlBtHalAdvApiMultiAdv, &pxAdapterIf );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to get adapter with advertiser, status=%d", status );
    return status;
  }

  /* If advertisement is already active, return an error */
  if( pxAdapterIf->bAdvActive )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "already active, status=%d", eBTStatusBusy );
    return eBTStatusBusy;
  }

  /* Configure the advertiser in this adapter context */
  status = prvConfigureAdapterAdvertiser( pxAdapterIf, xAdvParams );
  if( status != eBTStatusSuccess )
  {
    /* Delete the advertiser set that we failed to configure */
    sl_bt_advertiser_delete_set( pxAdapterIf->ucAdvSetHandle );
    pxAdapterIf->ucAdvSetHandle = SL_BT_INVALID_ADVERTISING_SET_HANDLE;
    pxAdapterIf->xAdvApiType = SlBtHalAdvApiNone;
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to configure, status=%d", status );
    return status;
  }

  /* Call the application */
  if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxMultiAdvEnableCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxMultiAdvEnableCb", "ucAdapterIf=%d, status=%d",
                                  ucAdapterIf, eBTStatusSuccess );
    pxBleAdapterCallbacks->pxMultiAdvEnableCb( ucAdapterIf, eBTStatusSuccess );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxMultiAdvEnableCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Update the parameters as per spec, user manual specified values and restart multi ADV.
 *
 * This function implements pxMultiAdvUpdate function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvMultiAdvUpdate( uint8_t ucAdapterIf,
                                     BTGattAdvertismentParams_t * advParams )
{
  /* This function is not supported */
  (void) ucAdapterIf;
  (void) advParams;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Setup the raw data for the specified instance.
 *
 * This function implements pxMultiAdvSetInstRawData function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvMultiAdvSetInstRawData( uint8_t ucAdapterIf,
                                             bool bSetScanRsp,
                                             uint8_t * pucData,
                                             size_t xDataLen )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "ucAdapterIf=%d, bSetScanRsp=%d, xDataLen=%u",
                                  ( int ) ucAdapterIf, ( int ) bSetScanRsp, xDataLen );

  /* We must have an adapter and it must already have an advertiser set for a
   * multi-advertiser */
  SlBtAdapterIf_t * pxAdapterIf = prvGetAdapterIf( ucAdapterIf );
  if( ( pxAdapterIf == NULL ) ||
      ( pxAdapterIf->ucAdvSetHandle == SL_BT_INVALID_ADVERTISING_SET_HANDLE ) ||
      ( pxAdapterIf->xAdvApiType != SlBtHalAdvApiMultiAdv ) )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid ucAdapterIf, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* If advertisement is already active, return an error */
  if( pxAdapterIf->bAdvActive )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "already active, status=%d", eBTStatusBusy );
    return eBTStatusBusy;
  }

  /* If we're not setting a scan response, prepend advertisement flags */
  uint8_t ucAdvBuf[MAX_LEGACY_ADV_DATA_LEN];
  if( !bSetScanRsp )
  {
    /* Include the flags */
    size_t xAdvDataLen = 0;
    ucAdvBuf[xAdvDataLen++] = 2; /* Length */
    ucAdvBuf[xAdvDataLen++] = ADV_DATA_TYPE_FLAGS; /* Type */
    ucAdvBuf[xAdvDataLen++] = ADV_FLAG_GENERAL_DISCOVERABLE | ADV_FLAG_BR_EDR_NOT_SUPPORTED; /* Value */

    /* Include the user data */
    if( xDataLen > sizeof( ucAdvBuf ) - xAdvDataLen )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "could not fit user data, xDataLen=%d, status=%d",
                                (int) xDataLen, eBTStatusParamInvalid );
      return eBTStatusParamInvalid;
    }
    memcpy( &ucAdvBuf[xAdvDataLen], pucData, xDataLen );
    xDataLen += xAdvDataLen;
    pucData = ucAdvBuf;
  }

  /* Set the user data to the Bluetooth stack */
  uint8_t ucPacketType = bSetScanRsp ?
    ADV_PACKET_TYPE_SCAN_RESPONSE : ADV_PACKET_TYPE_ADVERTISEMENT;
  sl_status_t sl_status = sl_bt_advertiser_set_data( pxAdapterIf->ucAdvSetHandle,
                                                     ucPacketType, xDataLen, pucData );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to set data, sl_status=0x%x", sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }

  /* Advertisement is started when the scan response is set, or immediately when
  the advertiser is not scannable */
  bool bIsScannable =
    ( pxAdapterIf->ucAdvConnectableMode == sl_bt_advertiser_connectable_scannable ) ||
    ( pxAdapterIf->ucAdvConnectableMode == sl_bt_advertiser_scannable_non_connectable );
  if( bSetScanRsp || ( !bIsScannable ) )
  {
    /* Start advertising with user-defined data */
    sl_status = sl_bt_advertiser_start( pxAdapterIf->ucAdvSetHandle,
                                        sl_bt_advertiser_user_data,
                                        pxAdapterIf->ucAdvConnectableMode );
    if( sl_status != SL_STATUS_OK )
    {
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to start advertising, sl_status=0x%x",
                                sl_status );
      return prvSlStatusToBTStatus( sl_status );
    }
    pxAdapterIf->bAdvActive = true;
  }

  /* Call the application */
  if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxMultiAdvDataCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxMultiAdvDataCb", "ucAdapterIf=%d, status=%d",
                                  ucAdapterIf, eBTStatusSuccess );
    pxBleAdapterCallbacks->pxMultiAdvDataCb( ucAdapterIf, eBTStatusSuccess );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxMultiAdvDataCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Setup the data for the specified instance.
 *
 * This function implements pxMultiAdvSetInstData function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvMultiAdvSetInstData( uint8_t ucAdapterIf,
                                          bool bSetScanRsp,
                                          bool bIncludeName,
                                          bool bInclTxpower,
                                          uint32_t ulAppearance,
                                          size_t xManufacturerLen,
                                          char * pcManufacturerData,
                                          size_t xServiceDataLen,
                                          char * pcServiceData,
                                          BTUuid_t * pxServiceUuid,
                                          size_t xNbServices )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "ucAdapterIf=%d, bSetScanRsp=%d",
                             (int) ucAdapterIf, (int) bSetScanRsp );

  /* We must have an adapter and it must already have an advertiser set for a
   * multi-advertiser */
  SlBtAdapterIf_t * pxAdapterIf = prvGetAdapterIf( ucAdapterIf );
  if( ( pxAdapterIf == NULL ) ||
      ( pxAdapterIf->ucAdvSetHandle == SL_BT_INVALID_ADVERTISING_SET_HANDLE ) ||
      ( pxAdapterIf->xAdvApiType != SlBtHalAdvApiMultiAdv ) )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid ucAdapterIf, status=%d",
                              eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* Initialize the advertisement parameters needed by prvGenerateAdvertisementData */
  BTGattAdvertismentParams_t xParams;
  memset( &xParams, 0, sizeof(xParams) );
  xParams.bSetScanRsp = bSetScanRsp;
  if( bIncludeName )
  {
    xParams.ucName.xType = BTGattAdvNameComplete;
  }
  else
  {
    xParams.ucName.xType = BTGattAdvNameNone;
  }
  xParams.bIncludeTxPower = bInclTxpower;
  xParams.ulAppearance = ulAppearance;

  /* Generate the advertisement data. We never include advertisement flags, as
  prvMultiAdvSetInstRawData() will include them automatically when relevant. */
  uint8_t advBuf[MAX_LEGACY_ADV_DATA_LEN];
  size_t advDataLen = 0;
  bool bIncludeAdvFlags = false;
  BTStatus_t status = prvGenerateAdvertisementData( &xParams, bIncludeAdvFlags,
                                                    xManufacturerLen, pcManufacturerData,
                                                    xServiceDataLen, pcServiceData,
                                                    pxServiceUuid, xNbServices,
                                                    pxAdapterIf->sSetAdvPower,
                                                    advBuf, sizeof(advBuf),
                                                    &advDataLen );
  if( status != eBTStatusSuccess )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to generate data, status=%d", status );
    return status;
  }

  /* Set as raw data */
  status = prvMultiAdvSetInstRawData( ucAdapterIf, bSetScanRsp, advBuf, advDataLen );

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", status );
  return status;
}

/**
 * @brief Disable the multi adv instance.
 *
 * This function implements pxMultiAdvDisable function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvMultiAdvDisable( uint8_t ucAdapterIf )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "ucAdapterIf=%d", (int) ucAdapterIf );

  /* We must have a multi-advertiser adapter */
  SlBtAdapterIf_t * pxAdapterIf = prvGetAdapterIf( ucAdapterIf );
  if( ( pxAdapterIf == NULL ) ||
      ( pxAdapterIf->xAdvApiType != SlBtHalAdvApiMultiAdv ) )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid ucAdapterIf, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* Delete the set. This will also make the callback to the app. */
  prvDeleteMultiAdvertiserAdvSet( pxAdapterIf );

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Configure the batchscan storage.
 *
 * This function implements pxBatchscanCfgStorage function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvBatchscanCfgStorage( uint8_t ucAdapterIf,
                                          uint32_t ulBatchScanFullMax,
                                          uint32_t ulBatchScanTruncMax,
                                          uint32_t ulBatchScanNotifyThreshold )
{
  /* This function is not supported */
  (void) ucAdapterIf;
  (void) ulBatchScanFullMax;
  (void) ulBatchScanTruncMax;
  (void) ulBatchScanNotifyThreshold;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Enable batchscan.
 *
 * This function implements pxBatchscanEndBatchScan function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvBatchscanEndBatchScan( uint8_t ucAdapterIf,
                                            uint32_t ulScanMode,
                                            uint32_t ulScanInterval,
                                            uint32_t ulScanWindow,
                                            uint32_t ulAddrType,
                                            uint32_t ulDiscardRule )
{
  /* This function is not supported */
  (void) ucAdapterIf;
  (void) ulScanMode;
  (void) ulScanInterval;
  (void) ulScanWindow;
  (void) ulAddrType;
  (void) ulDiscardRule;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Disable batchscan.
 *
 * This function implements pxBatchscanDisBatchScan function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvBatchscanDisBatchScan( uint8_t ucAdapterIf )
{
  /* This function is not supported */
  (void) ucAdapterIf;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief  Read out batchscan reports.
 *
 * This function implements pxBatchscanReadReports function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvBatchscanReadReports( uint8_t ucAdapterIf,
                                           uint32_t ulScanMode )
{
  /* This function is not supported */
  (void) ucAdapterIf;
  (void) ulScanMode;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Set preferred PHY for a connection.
 *
 * This function implements pxSetPreferredPhy function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvSetPreferredPhy( uint16_t usConnId,
                                      uint8_t ucTxPhy,
                                      uint8_t ucRxPhy,
                                      uint16_t usPhyOptions )
{
  /* This function is not supported */
  (void) usConnId;
  (void) ucTxPhy;
  (void) ucRxPhy;
  (void) usPhyOptions;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Read the PHY used for a connection.
 *
 * This function implements pxReadPhy function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvReadPhy( uint16_t usConnId,
                              BTReadClientPhyCallback_t xCb )
{
  /* This function is not supported */
  (void) usConnId;
  (void) xCb;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Sets the LE background scan interval and window in units of N*0.625 msec.
 *
 * This function implements pxSetBgScanParameters function in @ref BTBleAdapter_t.
 */
static BTStatus_t prvSetBgScanParameters( uint8_t ucAdapterIf,
                                          uint32_t ulScanIntervalTime,
                                          uint32_t ulScanWindowTime )
{
  /* This function is not supported */
  (void) ucAdapterIf;
  (void) ulScanIntervalTime;
  (void) ulScanWindowTime;

  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Silicon Labs BLE adapter interface functions.
 */
static const BTBleAdapter_t xSilabsBTBleAdapter =
{
  .pxRegisterBleApp = prvRegisterBleApp,
  .pxUnregisterBleApp = prvUnregisterBleApp,
  .pxBleAdapterInit = prvBleAdapterInit,
  .pxGetBleAdapterProperty = prvGetBleAdapterProperty,
  .pxSetBleAdapterProperty = prvSetBleAdapterProperty,
  .pxGetallBleRemoteDeviceProperties = prvGetallBleRemoteDeviceProperties,
  .pxGetBleRemoteDeviceProperty = prvGetBleRemoteDeviceProperty,
  .pxSetBleRemoteDeviceProperty = prvSetBleRemoteDeviceProperty,
  .pxScan = prvScan,
  .pxConnect = prvConnect,
  .pxDisconnect = prvDisconnect,
  .pxStartAdv = prvStartAdv,
  .pxStopAdv = prvStopAdv,
  .pxReadRemoteRssi = prvReadRemoteRssi,
  .pxScanFilterParamSetup = prvScanFilterParamSetup,
  .pxScanFilterAddRemove = prvScanFilterAddRemove,
  .pxScanFilterClear = prvScanFilterClear,
  .pxScanFilterEnable = prvScanFilterEnable,
  .pxGetDeviceType = prvGetDeviceType,
  .pxSetAdvData = prvSetAdvData,
  .pxSetAdvRawData = prvSetAdvRawData,
  .pxConnParameterUpdateRequest = prvConnParameterUpdateRequest,
  .pxSetScanParameters = prvSetScanParameters,
  .pxMultiAdvEnable = prvMultiAdvEnable,
  .pxMultiAdvUpdate = prvMultiAdvUpdate,
  .pxMultiAdvSetInstData = prvMultiAdvSetInstData,
  .pxMultiAdvDisable = prvMultiAdvDisable,
  .pxBatchscanCfgStorage = prvBatchscanCfgStorage,
  .pxBatchscanEndBatchScan = prvBatchscanEndBatchScan,
  .pxBatchscanDisBatchScan = prvBatchscanDisBatchScan,
  .pxBatchscanReadReports = prvBatchscanReadReports,
  .pxSetPreferredPhy = prvSetPreferredPhy,
  .pxReadPhy = prvReadPhy,
  .ppvGetGattClientInterface = prvGetGattClientInterface,
  .ppvGetGattServerInterface = prvGetGattServerInterface,
  .pxMultiAdvSetInstRawData = prvMultiAdvSetInstRawData,
  .pxSetBgScanParameters = prvSetBgScanParameters,
};

/* Retrieves the HAL LE interface. */
const void * prvGetLeAdapter( )
{
  return &xSilabsBTBleAdapter;
}

/* Invoked on BTInterface_t's pxDisable() to cleanup adapter state */
void prvBleAdapterOnPxDisable()
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "%s", "" );

  /* Stop and cleanup advertiser sets */
  SlBtAdapterIf_t * pxAdapterIf = pxAdapterIfs;
  while( pxAdapterIf != NULL )
  {
    if( ( pxAdapterIf->xClient.ucHandle != SL_BT_INVALID_IF_HANDLE ) )
    {
      /* If we have an advertiser set, clean that up */
      if( pxAdapterIf->ucAdvSetHandle != SL_BT_INVALID_ADVERTISING_SET_HANDLE )
      {
        /* Handle the implicit stop of the advertiser */
        prvStopAdvertiserImplicitly( pxAdapterIf );

        /* If we still have a set (wasn't deleted already for a
         * multi-advertiser), delete it now */
        if( pxAdapterIf->ucAdvSetHandle != SL_BT_INVALID_ADVERTISING_SET_HANDLE )
        {
          sl_bt_advertiser_delete_set( pxAdapterIf->ucAdvSetHandle );
          pxAdapterIf->ucAdvSetHandle = SL_BT_INVALID_ADVERTISING_SET_HANDLE;
        }

        pxAdapterIf->xAdvApiType = SlBtHalAdvApiNone;
      }
    }

    pxAdapterIf = ( SlBtAdapterIf_t * ) pxAdapterIf->xClient.pxNext;
  }

  /* Close all connections */
  for( uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i ++ )
  {
    SlBtConnections_t * pxConnection = &xBtConnections[i];
    if( pxConnection->ucAdapterIf != SL_BT_INVALID_IF_HANDLE )
    {
      /* Close the connection and ignore the status, as there's nothing we could do to recover here */
      sl_status_t sl_status = sl_bt_connection_close( pxConnection->ucConnectionHandle );
      (void) sl_status;

      /* Simulate an immediate disconnection by giving the callback already here */
      if( pxBleAdapterCallbacks && pxBleAdapterCallbacks->pxCloseCb )
      {
        SILABS_BLE_LOG_CB_CALL_INFO( "pxCloseCb", "connection=%d, reason=%d, ucHandle=0x%x",
                                     ( int ) pxConnection->ucConnectionHandle,
                                     ( int ) eBTGattStatusLocalHostTerminatedConnection,
                                     ( int ) pxConnection->ucAdapterIf );
        pxBleAdapterCallbacks->pxCloseCb( pxConnection->ucConnectionHandle,
                                          eBTGattStatusLocalHostTerminatedConnection,
                                          pxConnection->ucAdapterIf,
                                          &pxConnection->xRemoteAddress );
        SILABS_BLE_LOG_CB_RETURN_INFO( "pxCloseCb" );
      }

      /* Delete the saved connection */
      memset( pxConnection, 0, sizeof( SlBtConnections_t ) );
      pxConnection->ucAdapterIf = SL_BT_INVALID_IF_HANDLE;
    }
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", eBTStatusSuccess );
}

/* Invoked on Bluetooth event to handle events the BLE adapter needs. */
void prvBleAdapterOnSlBtEvent( sl_bt_msg_t* evt )
{
  switch( SL_BT_MSG_ID( evt->header ) ) {
    case sl_bt_evt_advertiser_timeout_id:
      prvOnAdvertiserTimeout( &evt->data.evt_advertiser_timeout );
      break;

    case sl_bt_evt_connection_opened_id:
      prvOnConnectionOpened( &evt->data.evt_connection_opened );
      break;

    case sl_bt_evt_connection_parameters_id:
      prvOnConnectionParameters( &evt->data.evt_connection_parameters );
      break;

    case sl_bt_evt_connection_closed_id:
      prvOnConnectionClosed( &evt->data.evt_connection_closed );
      break;

    case sl_bt_evt_scanner_scan_report_id:
      prvOnScanReportEvent( &evt->data.evt_scanner_scan_report );
      break;

    case sl_bt_evt_sm_bonded_id:
      prvOnBonded( &evt->data.evt_sm_bonded );
      break;

    /* Other events are ignored */
    default:
      break;
  }
}
