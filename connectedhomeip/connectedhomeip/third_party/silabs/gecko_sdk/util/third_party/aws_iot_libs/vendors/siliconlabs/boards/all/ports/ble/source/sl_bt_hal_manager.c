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
#include "bt_hal_manager.h"

/* Silicon Labs includes */
#include "sl_bt_hal_manager.h"
#include "sl_bt_hal_manager_adapter_ble.h"
#include "sl_bt_hal_gatt_server.h"
#include "sl_bt_hal_event_handler.h"
#include "sl_bt_hal_common.h"
#include "sl_malloc.h"

/* HAL configuration */
#include "sl_bt_hal_config.h"

/** @brief Callbacks registered in the call to @ref prvBtManagerInit */
static const BTCallbacks_t * pxBtManagerCallbacks = NULL;

/* Flag bits used for "flags" argument of @ref sl_bt_sm_configure */
#define SM_CONFIGURE_FLAG_BONDING_REQUIRES_MITM_PROTECTION           ((uint8_t) 0x01)
#define SM_CONFIGURE_FLAG_ENCRYPTION_REQUIRES_BONDING                ((uint8_t) 0x02)
#define SM_CONFIGURE_FLAG_SECURE_CONNECTIONS_ONLY                    ((uint8_t) 0x04)
#define SM_CONFIGURE_FLAG_BONDING_REQUESTS_NEED_TO_BE_CONFIRMED      ((uint8_t) 0x08)
#define SM_CONFIGURE_FLAG_ALLOW_CONNECTIONS_ONLY_FROM_BONDED_DEVICES ((uint8_t) 0x10)

/**
 * @brief Default max MTU of the SL Bluetooth stack, as documented in @ref
 * sl_bt_gatt_set_max_mtu().
 */
#define SL_BT_DEFAULT_MAX_MTU ((uint16_t) 247)

/**
 * @brief The Bluetooth stack defaults to not being bondable. See @ref
 * sl_bt_sm_set_bondable_mode().
 */
#define SL_BT_DEFAULT_BONDABLE ((uint8_t) 0)

/**
 * @brief Default flags used for @ref sl_bt_sm_configure(). The default matches
 * Amazon's request to enable the "require bonding", "secure connections only",
 * and "bonding requests need to be confirmed" settings.
 */
#ifndef SL_BT_HAL_DEFAULT_SECURITY_CONFIG_BONDING_REQUIRES_MITM_PROTECTION
#define SL_BT_HAL_DEFAULT_SECURITY_CONFIG_BONDING_REQUIRES_MITM_PROTECTION      0
#endif
#ifndef SL_BT_HAL_DEFAULT_SECURITY_CONFIG_ENCRYPTION_REQUIRES_BONDING
#define SL_BT_HAL_DEFAULT_SECURITY_CONFIG_ENCRYPTION_REQUIRES_BONDING           1
#endif
#ifndef SL_BT_HAL_DEFAULT_SECURITY_CONFIG_SECURE_CONNECTIONS_ONLY
#define SL_BT_HAL_DEFAULT_SECURITY_CONFIG_SECURE_CONNECTIONS_ONLY               1
#endif
#ifndef SL_BT_HAL_DEFAULT_SECURITY_CONFIG_BONDING_REQUESTS_NEED_TO_BE_CONFIRMED
#define SL_BT_HAL_DEFAULT_SECURITY_CONFIG_BONDING_REQUESTS_NEED_TO_BE_CONFIRMED 1
#endif
#ifndef SL_BT_HAL_DEFAULT_SECURITY_CONFIG_CONNECTIONS_ONLY_FROM_BONDED_DEVICES
#define SL_BT_HAL_DEFAULT_SECURITY_CONFIG_CONNECTIONS_ONLY_FROM_BONDED_DEVICES  0
#endif
#define SL_BT_DEFAULT_SM_CONFIGURE_FLAGS                                                             \
  ( ((uint8_t) (SL_BT_HAL_DEFAULT_SECURITY_CONFIG_BONDING_REQUIRES_MITM_PROTECTION      << 0x00)) |  \
    ((uint8_t) (SL_BT_HAL_DEFAULT_SECURITY_CONFIG_ENCRYPTION_REQUIRES_BONDING           << 0x01)) |  \
    ((uint8_t) (SL_BT_HAL_DEFAULT_SECURITY_CONFIG_SECURE_CONNECTIONS_ONLY               << 0x02)) |  \
    ((uint8_t) (SL_BT_HAL_DEFAULT_SECURITY_CONFIG_BONDING_REQUESTS_NEED_TO_BE_CONFIRMED << 0x03)) |  \
    ((uint8_t) (SL_BT_HAL_DEFAULT_SECURITY_CONFIG_CONNECTIONS_ONLY_FROM_BONDED_DEVICES  << 0x04)) )

/**
 * @brief Default I/O capabilities used for @ref sl_bt_sm_configure().
 */
#ifndef SL_BT_HAL_DEFAULT_IO_CAPABILITIES
#define SL_BT_HAL_DEFAULT_IO_CAPABILITIES sm_io_capability_noinputnooutput
#endif
#define SL_BT_DEFAULT_IO_CAPABILITIES ((uint8_t) SL_BT_HAL_DEFAULT_IO_CAPABILITIES)

/**
 * @brief The maximum number of bondings to configure in a call to @ref
 *   sl_bt_sm_store_bonding_configuration().
 */
#ifndef SL_BT_HAL_SM_MAX_BONDING_COUNT
#define SL_BT_HAL_SM_MAX_BONDING_COUNT ((uint8_t) 8)
#endif

/**
 * @brief The policy flags to configure in a call to @ref
 *   sl_bt_sm_store_bonding_configuration().
 */
#ifndef SL_BT_HAL_SM_POLICY_FLAGS
#define SL_BT_HAL_SM_POLICY_FLAGS ((uint8_t) 2)
#endif

/**
 * @brief The value for confirming a pairing request or numeric value
 */
#define SL_BT_HAL_SM_COMFIRM_PAIRING ((uint8_t) 1)

/**
 * @brief The value for rejecting a pairing request or numeric value
 */
#define SL_BT_HAL_SM_REJECT_PAIRING ((uint8_t) 0)

/**
 * @brief The value for canceling the pairing during passkey entry stage
 */
#define SL_BT_HAL_SM_PASSKEY_ENTRY_CANCEL_PAIRING   (-1)

/**
 * @brief The minimum TX power to set to @ref sl_bt_system_set_tx_power()
 *
 * This constant specifies the minimum TX power level to set at
 * initialization time to @ref sl_bt_system_set_tx_power(). The value is
 * given in units of 0.1 dBm and defaults to 40, i.e. 4 dBm TX power.
 *
 * Note that the minimum TX power level is only used by LE power control and
 * the setting has no effect when LE power control is not used.
 */
#ifndef SL_BT_HAL_SYSTEM_MIN_TX_POWER
#define SL_BT_HAL_SYSTEM_MIN_TX_POWER ((int16_t) 40)
#endif

/**
 * @brief The maximum TX power to set to @ref sl_bt_system_set_tx_power()
 *
 * This constant specifies the maximum TX power level to set at initialization
 * time to @ref sl_bt_system_set_tx_power(). The value is given in units of
 * 0.1 dBm and defaults to 40, i.e. 4 dBm TX power.
 */
#ifndef SL_BT_HAL_SYSTEM_MAX_TX_POWER
#define SL_BT_HAL_SYSTEM_MAX_TX_POWER ((int16_t) 40)
#endif

/**
 * @brief Maximum Device Name length that can be queried.
 *
 * The Bluetooth specification (Bluetooth Core Specification version 5.2, Volume
 * 3, Part C, Section 3.2.2.3) allows up to 248 bytes. We default to a more
 * reasonable length of 40 bytes.
 */
#ifndef SL_BT_MAX_DEVICE_NAME_LENGTH
#define SL_BT_MAX_DEVICE_NAME_LENGTH ((size_t) 40)
#endif /* SL_BT_MAX_DEVICE_NAME_LENGTH */

/** @brief Properties of the device */
SlBtHalManager_t xSlBtHalManager = { 0 };

/** @brief Helper function to gets all bondings. */
static BTStatus_t prvGetBondings();

/**
 * @brief Initializes the interface and provides callback routines.
 *
 * This function implements pxBtManagerInit function in @ref BTInterface_t.
 */
static BTStatus_t prvBtManagerInit( const BTCallbacks_t * pxCallbacks )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "pxCallbacks=%p", pxCallbacks );

  /* Remember the callbacks */
  if( !pxCallbacks )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "NULL pxCallbacks, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }
  pxBtManagerCallbacks = pxCallbacks;

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Free up the memory.
 *
 * This function implements pxBtManagerCleanup function in @ref BTInterface_t.
 */
static BTStatus_t prvBtManagerCleanup( void )
{
  /* Nothing to cleanup here. */
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "%s", "" );
  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Initialize the HAL state and configure the Bluetooth stack accordingly.
 */
static BTStatus_t prvInitBluetoothState( void )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "%s", "" );

  /* Allocate the storage for device name attribute */
  xSlBtHalManager.pucDeviceName = sl_malloc( SL_BT_MAX_DEVICE_NAME_LENGTH );
  xSlBtHalManager.xDeviceNameLen = 0;
  if( !xSlBtHalManager.pucDeviceName )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to allocate %u bytes", SL_BT_MAX_DEVICE_NAME_LENGTH );
    return eBTStatusNoMem;
  }
  /* Set default max MTU */
  xSlBtHalManager.usMaxMtu = SL_BT_DEFAULT_MAX_MTU;
  sl_status_t sl_status = sl_bt_gatt_set_max_mtu( xSlBtHalManager.usMaxMtu,
                                                  &xSlBtHalManager.usMaxMtu );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_gatt_set_max_mtu(usMaxMtu=%"PRIu16") failed, sl_status=0x%x",
                                    xSlBtHalManager.usMaxMtu, sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }
  SILABS_BLE_LOG_PRINT_DEBUG( "  set usMaxMtu=%"PRIu16, xSlBtHalManager.usMaxMtu);

  /* Set default bondable mode */
  xSlBtHalManager.ucBondable = SL_BT_DEFAULT_BONDABLE;
  sl_status = sl_bt_sm_set_bondable_mode( xSlBtHalManager.ucBondable );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_sm_set_bondable_mode(ucBondable=%d) failed, sl_status=0x%x",
                                    ( int ) xSlBtHalManager.ucBondable, sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }
  SILABS_BLE_LOG_PRINT_DEBUG( "  set ucBondable=%d", ( int ) xSlBtHalManager.ucBondable );

  /* Set security manager configuration */
  xSlBtHalManager.ucSmConfigureFlags = SL_BT_DEFAULT_SM_CONFIGURE_FLAGS;
  xSlBtHalManager.ucIoCapabilities = SL_BT_DEFAULT_IO_CAPABILITIES;
  sl_status = sl_bt_sm_configure( xSlBtHalManager.ucSmConfigureFlags,
                                  xSlBtHalManager.ucIoCapabilities );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR(
      "sl_bt_sm_configure(ucSmConfigureFlags=0x%x, ucIoCapabilities=0x%x) failed, sl_status=0x%x",
      ( unsigned ) xSlBtHalManager.ucSmConfigureFlags,
      ( unsigned ) xSlBtHalManager.ucIoCapabilities,
      sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }
  SILABS_BLE_LOG_PRINT_DEBUG( "  set ucSmConfigureFlags=0x%x, ucIoCapabilities=0x%x",
                              ( unsigned ) xSlBtHalManager.ucSmConfigureFlags,
                              ( unsigned ) xSlBtHalManager.ucIoCapabilities );

  /* Store bonding configuration */
  uint8_t ucMaxBondingCount = SL_BT_HAL_SM_MAX_BONDING_COUNT;
  uint8_t ucPolicyFlags = SL_BT_HAL_SM_POLICY_FLAGS;
  sl_status = sl_bt_sm_store_bonding_configuration(ucMaxBondingCount, ucPolicyFlags);
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR(
      "sl_bt_sm_store_bonding_configuration(ucMaxBondingCount=%u, ucPolicyFlags=0x%x) failed, sl_status=0x%x",
      ( unsigned ) ucMaxBondingCount, ( unsigned ) ucPolicyFlags, sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }
  SILABS_BLE_LOG_PRINT_DEBUG("  set ucMaxBondingCount=%d, ucPolicyFlags=0x%x",
                             ( unsigned ) ucMaxBondingCount, ( unsigned ) ucPolicyFlags );

  /* Set the global maximum TX power to the highest power that advertising can use */
  int16_t sSlBtMaxPower = 0;
  BTStatus_t status = prvTxPowerIndexToSlBtPower( BT_HAL_BLE_ADV_TX_PWR_ULTRA_HIGH,
                                                  &sSlBtMaxPower );
  if( status != eBTStatusSuccess )
  {
    return status;
  }
  int16_t sRequestedMinPower = SL_BT_HAL_SYSTEM_MIN_TX_POWER;
  int16_t sRequestedMaxPower = SL_BT_HAL_SYSTEM_MAX_TX_POWER;
  int16_t sSetMinPower = 0;
  int16_t sSetMaxPower = 0;
  SILABS_BLE_LOG_PRINT_DEBUG( "  request TX power min=%"PRId16", max=%"PRId16,
                        sRequestedMinPower, sRequestedMaxPower );
  sl_status = sl_bt_system_set_tx_power( sRequestedMinPower, sRequestedMaxPower,
                                         &sSetMinPower, &sSetMaxPower );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR(
      "sl_bt_system_set_tx_power(sRequestedMinPower=%"PRId16", sRequestedMaxPower=%"PRId16") failed, sl_status=0x%x",
      sRequestedMinPower, sRequestedMaxPower, sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }
  SILABS_BLE_LOG_PRINT_DEBUG( "  set TX power min=%"PRId16", max=%"PRId16, sSetMinPower, sSetMaxPower );

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Enables Bluetooth module.
 *
 * This function implements pxEnable function in @ref BTInterface_t.
 */
static BTStatus_t prvEnable( uint8_t ucGuestMode )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "ucGuestMode=%d", ( int ) ucGuestMode );

  /* Unused parameter */
  (void) ucGuestMode;

  /* If the Bluetooth stack is already started, this is a redundant call. Just
  keep the current state, give the callback, and return success. */
  if( xSlBtHalManager.eBtState == eSlBtHalBluetoothStarted )
  {
    if( pxBtManagerCallbacks && pxBtManagerCallbacks->pxDeviceStateChangedCb )
    {
      SILABS_BLE_LOG_CB_CALL_INFO( "pxDeviceStateChangedCb", "state=%d", ( int ) eBTstateOn );
      pxBtManagerCallbacks->pxDeviceStateChangedCb( eBTstateOn );
      SILABS_BLE_LOG_CB_RETURN_INFO( "pxDeviceStateChangedCb" );
    }

    SILABS_BLE_LOG_FUNC_EXIT_INFO( "already started, status=%d", eBTStatusSuccess );
    return eBTStatusSuccess;
  }

  /* Commit to the starting state and start the Bluetooth stack */
  xSlBtHalManager.eBtState = eSlBtHalBluetoothStarting;
  sl_status_t sl_status = sl_bt_system_start_bluetooth( );
  if( sl_status != SL_STATUS_OK )
  {
    xSlBtHalManager.eBtState = eSlBtHalBluetoothStopped;
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_system_start_bluetooth() failed, sl_status=0x%x",
                                    sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }

  /* The Bluetooth stack is now starting up. We'll get either the boot event or
  the system error event to indicate the result later. */

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Disables Bluetooth module.
 *
 * This function implements pxDisable function in @ref BTInterface_t.
 */
static BTStatus_t prvDisable()
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "%s", "" );

  /* Consider the stack stopped */
  xSlBtHalManager.eBtState = eSlBtHalBluetoothStopped;

  /* Free the device name */
  if( xSlBtHalManager.pucDeviceName )
  {
    sl_free( xSlBtHalManager.pucDeviceName );
    xSlBtHalManager.pucDeviceName = NULL;
    xSlBtHalManager.xDeviceNameLen = 0;
  }

  /* Stop the Bluetooth stack */
  sl_status_t sl_status = sl_bt_system_stop_bluetooth( );
  SILABS_BLE_LOG_PRINT_DEBUG( "  sl_bt_system_stop_bluetooth() returned sl_status=0x%x", sl_status );

  /* We ignore any errors, as there's nothing we could or should do if an error
  is returned. The command commits to shutting down as much as it can, and the
  way to continue is to start the stack again, regardless of what happened
  at the time of stopping. */
  (void) sl_status;

  /* Give the application the state change callback */
  if( pxBtManagerCallbacks && pxBtManagerCallbacks->pxDeviceStateChangedCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxDeviceStateChangedCb", "state=%d", ( int ) eBTstateOff );
    pxBtManagerCallbacks->pxDeviceStateChangedCb( eBTstateOff );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxDeviceStateChangedCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Retrieves all properties of local device.
 *
 * This function implements pxGetAllDeviceProperties function in @ref BTInterface_t.
 */
static BTStatus_t prvGetAllDeviceProperties()
{
  /* This function is not supported */
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Convert SL Bluetooth I/O capabilities to BLE HAL value
 *
 * @param[in] ucIoCapabilities I/O capabilities configured to SL Bluetooth stack
 *
 * @return Corresponding @ref BTIOtypes_t value
 */
static BTIOtypes_t prvSlIoCapabilitiesToBTIotypes( uint8_t ucIoCapabilities )
{
  /* Convert the type */
  BTIOtypes_t xIoTypes = eBTIONone;
  switch( ucIoCapabilities )
  {
    case sm_io_capability_noinputnooutput:
      /* No IO. */
      xIoTypes = eBTIONone;
      break;

    case sm_io_capability_displayonly:
      /* No input, only display. */
      xIoTypes = eBTIODisplayOnly;
      break;

    case sm_io_capability_displayyesno:
      /* Display + yes/no input. */
      xIoTypes = eBTIODisplayYesNo;
      break;

    case sm_io_capability_keyboardonly:
      /* Only input, keyboard. */
      xIoTypes = eBTIOKeyboardOnly;
      break;

    case sm_io_capability_keyboarddisplay:
      /* Keyboard and Display. */
      xIoTypes = eBTIOKeyboardDisplay;
      break;

    default:
      xIoTypes = eBTIONone;
      break;
  }

  return xIoTypes;
}

/**
 * @brief Get Bluetooth Adapter property of 'type'.
 *
 * This function implements pxGetDeviceProperty function in @ref BTInterface_t.
 */
static BTStatus_t prvGetDeviceProperty( BTPropertyType_t xType )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "xType=%d", ( int ) xType );

  /* It is not meaningful to request properties without having provided the
   * associated callback */
  if( !pxBtManagerCallbacks || !pxBtManagerCallbacks->pxAdapterPropertiesCb )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "no callback provided, status%=d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  /* Initialize a property struct to use for the callback */
  BTDeviceType_t xDeviceType;
  BTBdaddr_t xBdaddr;
  bd_addr xSlBtBdAddr;
  uint8_t ucSlBtBdAddrType;
  uint32_t ulMaxMtu;
  bool bBoolValue;
  BTIOtypes_t xIoTypes;
  BTProperty_t xProperty;
  xProperty.xType = xType;
  xProperty.xLen = 0;
  xProperty.pvVal = NULL;

  BTStatus_t status = eBTStatusFail;
  sl_status_t sl_status = SL_STATUS_FAIL;
  switch( xType )
  {
    case eBTpropertyBdname:
      status = eBTStatusSuccess;
      if( !xSlBtHalManager.pucDeviceName )
      {
        status = eBTStatusFail;
      }
      else
      {
        xProperty.xLen = xSlBtHalManager.xDeviceNameLen;
        xProperty.pvVal = (void *) xSlBtHalManager.pucDeviceName;
        pxBtManagerCallbacks->pxAdapterPropertiesCb(status, 1, &xProperty);
      }
      break;

    case eBTpropertyBdaddr:
      sl_status = sl_bt_system_get_identity_address( &xSlBtBdAddr,
                                                     &ucSlBtBdAddrType );
      if( sl_status != SL_STATUS_OK )
      {
        status = prvSlStatusToBTStatus( sl_status );
      }
      else
      {
        memcpy(xBdaddr.ucAddress, xSlBtBdAddr.addr, sizeof(xBdaddr.ucAddress));
        xProperty.xLen = sizeof(xBdaddr);
        xProperty.pvVal = &xBdaddr;
        status = eBTStatusSuccess;
        pxBtManagerCallbacks->pxAdapterPropertiesCb(status, 1, &xProperty);
      }
      break;

    case eBTpropertyTypeOfDevice:
      xDeviceType = eBTdeviceDevtypeBle;
      xProperty.xLen = sizeof(xDeviceType);
      xProperty.pvVal = &xDeviceType;
      status = eBTStatusSuccess;
      pxBtManagerCallbacks->pxAdapterPropertiesCb(status, 1, &xProperty);
      break;

    case eBTpropertyAdapterBondedDevices:
      /* Callback is handled inside prvGetBondings() */
      status = prvGetBondings();
      break;

    case eBTpropertyLocalMTUSize:
      ulMaxMtu = xSlBtHalManager.usMaxMtu;
      xProperty.xLen = sizeof(ulMaxMtu);
      xProperty.pvVal = &ulMaxMtu;
      status = eBTStatusSuccess;
      pxBtManagerCallbacks->pxAdapterPropertiesCb(status, 1, &xProperty);
      break;

    case eBTpropertyBondable:
      bBoolValue = xSlBtHalManager.ucBondable != 0;
      xProperty.xLen = sizeof(bBoolValue);
      xProperty.pvVal = &bBoolValue;
      status = eBTStatusSuccess;
      pxBtManagerCallbacks->pxAdapterPropertiesCb(status, 1, &xProperty);
      break;

    case eBTpropertyIO:
      xIoTypes = prvSlIoCapabilitiesToBTIotypes(xSlBtHalManager.ucIoCapabilities);
      xProperty.xLen = sizeof(xIoTypes);
      xProperty.pvVal = &xIoTypes;
      status = eBTStatusSuccess;
      pxBtManagerCallbacks->pxAdapterPropertiesCb(status, 1, &xProperty);
      break;

    case eBTpropertySecureConnectionOnly:
      bBoolValue = (xSlBtHalManager.ucSmConfigureFlags & SM_CONFIGURE_FLAG_SECURE_CONNECTIONS_ONLY) != 0;
      xProperty.xLen = sizeof(bBoolValue);
      xProperty.pvVal = &bBoolValue;
      status = eBTStatusSuccess;
      pxBtManagerCallbacks->pxAdapterPropertiesCb(status, 1, &xProperty);
      break;

    default:
      SILABS_BLE_LOG_PRINT_ERROR( "get for unsupported property xType=%d", ( int ) xType );
      status = eBTStatusUnsupported;
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", status );
  return status;
}

/**
 * @brief Set the device name property
 *
 * @param[in] pxProperty The property struct for eBTpropertyBdname
 */
static BTStatus_t prvSetDeviceName( const BTProperty_t * pxProperty )
{
  if( !xSlBtHalManager.pucDeviceName )
  {
    return eBTStatusFail;
  }
  size_t xNameLen = pxProperty->xLen;
  /* If the given device name exceeds max device name length, return error */
  if( xNameLen > SL_BT_MAX_DEVICE_NAME_LENGTH )
  {
    return eBTStatusParamInvalid;
  }
  memcpy( xSlBtHalManager.pucDeviceName, pxProperty->pvVal, xNameLen );
  xSlBtHalManager.xDeviceNameLen = xNameLen;
  return eBTStatusSuccess;
}

/**
 * @brief Set the local MTU size
 *
 * @param[in] pxProperty The property struct for eBTpropertyLocalMTUSize
 */
static BTStatus_t prvSetLocalMtuSize( const BTProperty_t * pxProperty )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "%s", "" );

  uint16_t usMaxMtu;
  uint16_t usMaxMtuOut;
  uint32_t ulMaxMtu;

  /* A value must have been provided */
  if( !pxProperty->pvVal )
  {
    return eBTStatusParamInvalid;
  }

  /* The value must be stored as a uint16_t or a uint32_t */
  if( pxProperty->xLen == sizeof(uint16_t) )
  {
      ulMaxMtu = *( (uint16_t *) pxProperty->pvVal );
  }
  else if( pxProperty->xLen == sizeof(uint32_t) )
  {
      ulMaxMtu = *( (uint32_t *) pxProperty->pvVal );
  }
  else
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid xLen=%u", pxProperty->xLen );
    return eBTStatusParamInvalid;
  }

  /* A value that doesn't fit to a uint16_t is certainly invalid */
  if( ulMaxMtu > UINT16_MAX )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid ulMaxMtu=%"PRIu32, ulMaxMtu );
    return eBTStatusParamInvalid;
  }
  usMaxMtu = (uint16_t) ulMaxMtu;

  /* Set to the Bluetooth stack */
  sl_status_t sl_status = sl_bt_gatt_set_max_mtu( usMaxMtu, &usMaxMtuOut );
  if( sl_status != SL_STATUS_OK )
  {
    /* Nothing was set, so simply return an error */
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_gatt_set_max_mtu() failed, sl_status=0x%x", sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }

  /* Setting succeeded, but we might have ended up setting a smaller value than
   * what was asked */
  BTStatus_t status = eBTStatusSuccess;
  if( usMaxMtuOut < ulMaxMtu )
  {
    /* Try to restore the original value we had set before the attempt above.
     * This should not fail but if it did there's not much we could do about it,
     * so ignore any error. */
    sl_status = sl_bt_gatt_set_max_mtu( xSlBtHalManager.usMaxMtu, &usMaxMtuOut );
    (void) sl_status;

    /* Consider the parameter invalid */
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "max MTU usMaxMtuOut=%"PRIu16" < ulMaxMtu=%"PRIu32", status=%d",
                                    usMaxMtuOut, ulMaxMtu, eBTStatusParamInvalid );
    status = eBTStatusParamInvalid;
  }

  /* Update the stored max MTU to reflect what we set */
  xSlBtHalManager.usMaxMtu = usMaxMtuOut;

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d, usMaxMtuOut=%"PRIu16, status, usMaxMtuOut );
  return status;
}

/**
 * @brief Enable or disable bondable mode
 *
 * @param[in] pxProperty The property struct for eBTpropertyBondable
 */
static BTStatus_t prvSetBondableMode( const BTProperty_t * pxProperty )
{
  /* The value for property eBTpropertyBondable must be a bool */
  if( (!pxProperty->pvVal) || (pxProperty->xLen != sizeof(bool)) )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "bad property pvVal=%p, xLen=%u",
                                    pxProperty->pvVal, pxProperty->xLen );
    return eBTStatusParamInvalid;
  }
  bool bBondable = *( (bool *) pxProperty->pvVal );
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "bBondable=%d", ( int ) bBondable );

  /* Set to the Bluetooth stack */
  uint8_t ucBondable = bBondable ? 1 : 0;
  sl_status_t sl_status = sl_bt_sm_set_bondable_mode( ucBondable );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_sm_set_bondable_mode() failed, sl_status=0x%x",
                                    sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }

  /* Remember the state */
  xSlBtHalManager.ucBondable = ucBondable;

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Set I/O capabilities
 *
 * @param[in] pxProperty The property struct for eBTpropertyIO
 */
static BTStatus_t prvSetIoCapabilities( const BTProperty_t * pxProperty )
{
  /* The value for property eBTpropertyIO must be a BTIOtypes_t */
  if( (!pxProperty->pvVal) || (pxProperty->xLen != sizeof(BTIOtypes_t)) )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "bad property pvVal=%p, xLen=%u",
                                    pxProperty->pvVal, pxProperty->xLen );
    return eBTStatusParamInvalid;
  }
  BTIOtypes_t xIoTypes = *( (BTIOtypes_t *) pxProperty->pvVal );
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "xIoTypes=%d", ( int ) xIoTypes );

  /* Convert the type */
  uint8_t ucIoCapabilities = sm_io_capability_noinputnooutput;
  switch( xIoTypes )
  {
    case eBTIONone:
      /* No IO. */
      ucIoCapabilities = sm_io_capability_noinputnooutput;
      break;

    case eBTIODisplayOnly:
      /* No input, only display. */
      ucIoCapabilities = sm_io_capability_displayonly;
      break;

    case eBTIODisplayYesNo:
      /* Display + yes/no input. */
      ucIoCapabilities = sm_io_capability_displayyesno;
      break;

    case eBTIOKeyboardOnly:
      /* Only input, keyboard. */
      ucIoCapabilities = sm_io_capability_keyboardonly;
      break;

    case eBTIOKeyboardDisplay:
      /* Keyboard and Display. */
      ucIoCapabilities = sm_io_capability_keyboarddisplay;
      break;

    default:
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "invalid xIoTypes=%d", ( int ) xIoTypes );
      return eBTStatusParamInvalid;
  }

  /* Set to the Bluetooth stack */
  sl_status_t sl_status = sl_bt_sm_configure( xSlBtHalManager.ucSmConfigureFlags,
                                              ucIoCapabilities );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_sm_configure() failed, sl_status=0x%x",
                                    sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }

  /* Remember the setting */
  xSlBtHalManager.ucIoCapabilities = ucIoCapabilities;

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Set the eBTpropertySecureConnectionOnly property
 *
 * @param[in] pxProperty The property struct for eBTpropertySecureConnectionOnly
 */
static BTStatus_t prvSetSecureConnectionOnly( const BTProperty_t * pxProperty )
{
  /* The value for property eBTpropertySecureConnectionOnly must be abool */
  if( (!pxProperty->pvVal) || (pxProperty->xLen != sizeof(bool)) )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "bad property pvVal=%p, xLen=%u",
                                    pxProperty->pvVal, pxProperty->xLen );
    return eBTStatusParamInvalid;
  }
  bool bSecureConnectionsOnly = *( (bool *) pxProperty->pvVal );
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "bSecureConnectionsOnly=%d", ( int ) bSecureConnectionsOnly );

  /* Construct the new SM configuration flags */
  uint8_t ucSmConfigureFlags = xSlBtHalManager.ucSmConfigureFlags;
  if( bSecureConnectionsOnly )
  {
    ucSmConfigureFlags |= SM_CONFIGURE_FLAG_SECURE_CONNECTIONS_ONLY;
  }
  else
  {
    ucSmConfigureFlags &= ~SM_CONFIGURE_FLAG_SECURE_CONNECTIONS_ONLY;
  }

  /* Set to the Bluetooth stack */
  sl_status_t sl_status = sl_bt_sm_configure( ucSmConfigureFlags,
                                              xSlBtHalManager.ucIoCapabilities );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_sm_configure(ucSmConfigureFlags=0x%x, ucIoCapabilities=%d) failed, sl_status=0x%x",
                                    ucSmConfigureFlags, xSlBtHalManager.ucIoCapabilities, sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }

  /* Remember the setting */
  xSlBtHalManager.ucSmConfigureFlags = ucSmConfigureFlags;

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", eBTStatusSuccess );
  return eBTStatusSuccess;
}

/**
 * @brief Sets Bluetooth Adapter property of 'type'.
 *
 * This function implements pxSetDeviceProperty function in @ref BTInterface_t.
 */
static BTStatus_t prvSetDeviceProperty( const BTProperty_t * pxProperty )
{
  SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( "xType=%d", pxProperty != NULL ? ( int ) pxProperty->xType : 0 );

  /* Check parameters */
  if( !pxProperty )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "NULL property, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  BTStatus_t status = eBTStatusFail;
  switch( pxProperty->xType )
  {
    case eBTpropertyBdname:
      status = prvSetDeviceName(pxProperty);
      break;

    case eBTpropertyLocalMTUSize:
      status = prvSetLocalMtuSize(pxProperty);
      break;

    case eBTpropertyBondable:
      status = prvSetBondableMode(pxProperty);
      break;

    case eBTpropertyIO:
      status = prvSetIoCapabilities(pxProperty);
      break;

    case eBTpropertySecureConnectionOnly:
      status = prvSetSecureConnectionOnly(pxProperty);
      break;

    default:
      SILABS_BLE_LOG_PRINT_ERROR( "set for unsupported property xType=%d", ( int ) pxProperty->xType );
      status = eBTStatusUnsupported;
      break;
  }

  /* If successful, give the application the callback */
  if( ( status == eBTStatusSuccess ) && pxBtManagerCallbacks->pxAdapterPropertiesCb )
  {
    SILABS_BLE_LOG_CB_CALL_DEBUG( "pxAdapterPropertiesCb", "xType=%d", ( int ) pxProperty->xType );
    pxBtManagerCallbacks->pxAdapterPropertiesCb( status, 1, ( BTProperty_t * ) pxProperty );
    SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxAdapterPropertiesCb" );
  }

  SILABS_BLE_LOG_FUNC_EXIT_DEBUG( "status=%d", status );
  return status;
}

/**
 * @brief Retrieves all properties of a remote device.
 *
 * This function implements pxGetAllRemoteDeviceProperties function in @ref BTInterface_t.
 */
static BTStatus_t prvGetAllRemoteDeviceProperties( BTBdaddr_t * pxRemoteAddr )
{
  /* This function is not supported */
  (void) pxRemoteAddr;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Get Bluetooth property of 'type' associated with a remote device.
 *
 * This function implements pxGetRemoteDeviceProperty function in @ref BTInterface_t.
 */
static BTStatus_t prvGetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                              BTPropertyType_t xType )
{
  /* This function is not supported */
  (void) pxRemoteAddr;
  (void) xType;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Sets Bluetooth property of 'type' associated with a remote device.
 *
 * This function implements pxSetRemoteDeviceProperty function in @ref BTInterface_t.
 */
static BTStatus_t prvSetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                              const BTProperty_t * pxProperty )
{
  /* This function is not supported */
  (void) pxRemoteAddr;
  (void) pxProperty;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Initiates Bluetooth pairing to a remote device.
 *
 * This function implements pxPair function in @ref BTInterface_t.
 */
static BTStatus_t prvPair( const BTBdaddr_t * pxBdAddr,
                           BTTransport_t xTransport,
                           bool bCreateBond )
{
  /* This function is not supported */
  (void) pxBdAddr;
  (void) xTransport;
  (void) bCreateBond;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Sets out of band data for a device for pairing.
 *
 * This function implements pxCreateBondOutOfBand function in @ref BTInterface_t.
 */
static BTStatus_t prvCreateBondOutOfBand( const BTBdaddr_t * pxBdAddr,
                                          BTTransport_t xTransport,
                                          const BTOutOfBandData_t * pxOobData )
{
  /* This function is not supported */
  (void) pxBdAddr;
  (void) xTransport;
  (void) pxOobData;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Request authentication from the master.
 *
 * This function implements pxSendSlaveSecurityRequest function in @ref BTInterface_t.
 */
static BTStatus_t prvSendSlaveSecurityRequest( const BTBdaddr_t * pxBdAddr,
                                               BTSecurityLevel_t xSecurityLevel,
                                               bool bBonding )
{
  /* This function is not supported */
  (void) pxBdAddr;
  (void) xSecurityLevel;
  (void) bBonding;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Cancels an on going bonding procedure for the given device.
 *
 * This function implements pxCancelBond function in @ref BTInterface_t.
 */
static BTStatus_t prvCancelBond( const BTBdaddr_t * pxBdAddr )
{
  /* This function is not supported */
  (void) pxBdAddr;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Removes the bonding with a device.
 *
 * This function implements pxRemoveBond function in @ref BTInterface_t.
 */
static BTStatus_t prvRemoveBond( const BTBdaddr_t * pxBdAddr )
{
  if( pxBdAddr == NULL )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "NULL pxBdAddr, status=%d", eBTStatusParamInvalid );
    return eBTStatusParamInvalid;
  }

  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "pxBdAddr="LOG_BDADDR_FORMAT, LOG_BTBDADDR_T_VALUE(pxBdAddr) );

  uint8_t ucBondingHandles[4];
  uint32_t ulTotalBondings = 0;
  size_t xBondingsLen;
  sl_status_t sl_status = 
    sl_bt_sm_get_bonding_handles( 0, &ulTotalBondings, 
                                  sizeof(ucBondingHandles),
                                  &xBondingsLen, 
                                  &ucBondingHandles[0] );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_sm_get_bonding_handles() failed, sl_status=0x%x",
                                    sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }
  uint32_t ulBondingsLeft = ulTotalBondings;
  BTStatus_t status = eBTStatusSuccess;
  for( uint8_t ucBonding=0;
     ( ucBonding < sizeof(ucBondingHandles) * 8 ) && ( ulBondingsLeft > 0 ); 
      ucBonding++ )
  {
    uint32_t ulByteIndex = ucBonding / 8;
    uint32_t ulBondingMask = 1 << (ucBonding% 8);
    if( ucBondingHandles[ulByteIndex] & ulBondingMask )
    {
      BTBdaddr_t xAddress;
      status = prvLoadBondedDeviceAddress( ucBonding, &xAddress );
      if( status == eBTStatusSuccess )
      {
        if( memcmp( &xAddress, pxBdAddr, sizeof( BTBdaddr_t ) ) == 0 )
        {
          sl_status = sl_bt_sm_delete_bonding( ucBonding );
          if ( sl_status != SL_STATUS_OK )
          {
            return prvSlStatusToBTStatus( sl_status );
          }
        break;
        }
      }
      ulBondingsLeft--;
    } 
  }
  if( pxBtManagerCallbacks && pxBtManagerCallbacks->pxPairingStateChangedCb )
  {
    SILABS_BLE_LOG_CB_CALL_INFO( "pxPairingStateChangedCb", "status=%d", ( int ) status );
    pxBtManagerCallbacks->pxPairingStateChangedCb( eBTStatusSuccess,
                                                   (BTBdaddr_t *) &pxBdAddr[0],
                                                   eBTbondStateNone,
                                                   eBTSecLevelNoSecurity,
                                                   eBTauthSuccess );
    SILABS_BLE_LOG_CB_RETURN_INFO( "pxPairingStateChangedCb" );
  }
  return eBTStatusSuccess;
}

/**
 * @brief Gets all bondings.
 *
 * This helper function implements the eBTpropertyAdapterBondedDevices property.
 */
static BTStatus_t prvGetBondings()
{
  uint8_t ucBondingHandles[4];
  uint32_t ulTotalBondings = 0;
  size_t xBondingsLen;
  sl_status_t sl_status = 
    sl_bt_sm_get_bonding_handles( 0, &ulTotalBondings, 
                                  sizeof(ucBondingHandles),
                                  &xBondingsLen, 
                                  &ucBondingHandles[0] );
  if( sl_status != SL_STATUS_OK )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "sl_bt_sm_get_bonding_handles() failed, sl_status=0x%x",
                                    sl_status );
    return prvSlStatusToBTStatus( sl_status );
  }

  size_t xBondingMem = sizeof ( BTBdaddr_t ) * ulTotalBondings;
  BTBdaddr_t * xAddresses = (BTBdaddr_t *) sl_malloc( xBondingMem );
  memset( xAddresses, 0, xBondingMem );
  if( !xAddresses )
  {
    SILABS_BLE_LOG_FUNC_EXIT_ERROR( "failed to allocate %u bytes",  xBondingMem );
    return eBTStatusNoMem;
  }

  uint8_t ulNumBleHalBondings = 0;
  uint32_t ulBondingsLeft = ulTotalBondings;
  for( uint8_t ucBonding=0;
       ( ucBonding < sizeof(ucBondingHandles) * 8 ) && ( ulBondingsLeft > 0 ); 
       ucBonding++ )
  {
    uint32_t ulByteIndex = ucBonding / 8;
    uint32_t ulBondingMask = 1 << (ucBonding % 8);
    if( ucBondingHandles[ulByteIndex] & ulBondingMask )
    {
      BTStatus_t status = prvLoadBondedDeviceAddress( ucBonding, &xAddresses[ulNumBleHalBondings] );
      if( status == eBTStatusSuccess )
      {
        ulNumBleHalBondings++;
      }
      ulBondingsLeft--;
    }
  }

  BTProperty_t xProperty;
  xProperty.xType = eBTpropertyAdapterBondedDevices;
  /* ulNumBleHalBondings is always incremented after successful loading of bonded
   * address and represents actual amount of BLE HAL specific bonded addresses,
   * as non-BLE HAL specific address could possibly be stored at the NVM bonding 
   * handle address. */
  xProperty.xLen = ulNumBleHalBondings * sizeof ( BTBdaddr_t );
  xProperty.pvVal = xAddresses;
  SILABS_BLE_LOG_CB_CALL_DEBUG( "pxAdapterPropertiesCb",
                                "xType=eBTpropertyAdapterBondedDevices, status=%d, xLen=%u",
                                eBTStatusSuccess, xProperty.xLen );
  pxBtManagerCallbacks->pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xProperty);
  SILABS_BLE_LOG_CB_RETURN_DEBUG( "pxAdapterPropertiesCb" );
  sl_free(xAddresses);

  return eBTStatusSuccess;  
}

/**
 * @brief Retrieves connection status for a device.
 *
 * This function implements pxGetConnectionState function in @ref BTInterface_t.
 */
static BTStatus_t prvGetConnectionState( const BTBdaddr_t * pxBdAddr,
                                         bool * bConnectionState )
{
  /* This function is not supported */
  (void) pxBdAddr;
  (void) bConnectionState;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief BT Legacy PinKey Reply.
 *
 * This function implements pxPinReply function in @ref BTInterface_t.
 */
static BTStatus_t prvPinReply( const BTBdaddr_t * pxBdAddr,
                               uint8_t ucAccept,
                               uint8_t ucPinLen,
                               BTPinCode_t * pxPinCode )
{
  /* This function is not supported */
  (void) pxBdAddr;
  (void) ucAccept;
  (void) ucPinLen;
  (void) pxPinCode;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief BT SSP Reply - Just Works, Numeric Comparison and Passkey
 *
 * This function implements pxSspReply function in @ref BTInterface_t.
 */
static BTStatus_t prvSspReply( const BTBdaddr_t * pxBdAddr,
                               BTSspVariant_t xVariant,
                               uint8_t ucAccept,
                               uint32_t ulPasskey )
{
  uint8_t ucConnectionHandle;
  BTStatus_t xStatus = prvGetConnectionHandle( pxBdAddr, &ucConnectionHandle );
  if( xStatus != eBTStatusSuccess )
  {
    return eBTStatusParamInvalid;
  }

  uint8_t ucConfirm = ucAccept ? SL_BT_HAL_SM_COMFIRM_PAIRING : SL_BT_HAL_SM_REJECT_PAIRING;
  sl_status_t sl_status = SL_STATUS_INVALID_PARAMETER;
  switch( xVariant )
  {
    case eBTsspVariantPasskeyConfirmation:
      sl_status = sl_bt_sm_passkey_confirm( ucConnectionHandle, ucConfirm );
      break;

    case eBTsspVariantPasskeyEntry:
      {
        int32_t lPasskey = ucAccept ? (int32_t) ulPasskey : SL_BT_HAL_SM_PASSKEY_ENTRY_CANCEL_PAIRING;
        sl_status = sl_bt_sm_enter_passkey( ucConnectionHandle, lPasskey );
      }
      break;

    case eBTsspVariantConsent:
      sl_status = sl_bt_sm_bonding_confirm( ucConnectionHandle, ucConfirm );
      break;

    case eBTsspVariantPasskeyNotification:
      // TODO: Clarify the expected behavior. According to the Bluetooth spec,
      // it is not possible to have user interaction in passkey display method.
      break;

    default:
      break;
  }
  return prvSlStatusToBTStatus( sl_status );
}

/**
 * @brief Read Energy info details.
 *
 * This function implements pxReadEnergyInfo function in @ref BTInterface_t.
 */
static BTStatus_t prvReadEnergyInfo()
{
  /* This function is not supported */
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Enables/disables local device to DUT mode.
 *
 * This function implements pxDutModeConfigure function in @ref BTInterface_t.
 */
static BTStatus_t prvDutModeConfigure( bool bEnable )
{
  /* This function is not supported */
  (void) bEnable;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Send any test HCI (vendor-specific) command to the controller. Must be in DUT Mode.
 *
 * This function implements pxDutModeSend function in @ref BTInterface_t.
 */
static BTStatus_t prvDutModeSend( uint16_t usOpcode,
                                  uint8_t * pucBuf,
                                  size_t xLen )
{
  /* This function is not supported */
  (void) usOpcode;
  (void) pucBuf;
  (void) xLen;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Send LE test command (LE_Receiver_Test, LE_Transmitter_Test, LE_Test_End )
 *
 * This function implements pxLeTestMode function in @ref BTInterface_t.
 */
static BTStatus_t prvLeTestMode( uint16_t usOpcode,
                                 uint8_t * pucBuf,
                                 size_t xLen )
{
  /* This function is not supported */
  (void) usOpcode;
  (void) pucBuf;
  (void) xLen;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Enables/disables capturing of HCI (snoop) logs.
 *
 * This function implements pxConfigHCISnoopLog function in @ref BTInterface_t.
 */
static BTStatus_t prvConfigHCISnoopLog( bool bEnable )
{
  /* This function is not supported */
  (void) bEnable;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Clears the stack configuration cache. Equivalent to BT layer Factory reset.
 *
 * This function implements pxConfigClear function in @ref BTInterface_t.
 */
static BTStatus_t prvConfigClear()
{
  /* This function is not supported */
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Retrieves RSSI of a remote device.
 *
 * This function implements pxReadRssi function in @ref BTInterface_t.
 */
static BTStatus_t prvReadRssi( const BTBdaddr_t * pxBdAddr )
{
  /* This function is not supported */
  (void) pxBdAddr;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Retrieves TX power of a remote device.
 *
 * This function implements pxGetTxpower function in @ref BTInterface_t.
 */
static BTStatus_t prvGetTxpower( const BTBdaddr_t * pxBdAddr,
                                 BTTransport_t xTransport )
{
  /* This function is not supported */
  (void) pxBdAddr;
  (void) xTransport;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Retrieves the Classic interface.
 *
 * This function implements pxGetClassicAdapter function in @ref BTInterface_t.
 */
static const void * prvGetClassicAdapter( )
{
  /* This function is not supported */
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return NULL;
}

/**
 * @brief Retrieves last error number from the stack.
 *
 * This function implements pxGetLastError function in @ref BTInterface_t.
 */
static uint32_t prvGetLastError()
{
  /* This function is not supported */
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Get stack features supported.
 *
 * This function implements pxGetStackFeaturesSupport function in @ref BTInterface_t.
 */
static BTStatus_t prvGetStackFeaturesSupport( uint32_t * pulFeatureMask )
{
  /* This function is not supported */
  (void) pulFeatureMask;
  SILABS_BLE_LOG_FUNC_UNSUPPORTED( SILABS_BLE_LOG_WARN );
  return eBTStatusUnsupported;
}

/**
 * @brief Silicon Labs Bluetooth interface functions.
 */
static const BTInterface_t prvSilabsBTInterface =
{
  .pxBtManagerInit = prvBtManagerInit,
  .pxBtManagerCleanup = prvBtManagerCleanup,
  .pxEnable = prvEnable,
  .pxDisable = prvDisable,
  .pxGetAllDeviceProperties = prvGetAllDeviceProperties,
  .pxGetDeviceProperty = prvGetDeviceProperty,
  .pxSetDeviceProperty = prvSetDeviceProperty,
  .pxGetAllRemoteDeviceProperties = prvGetAllRemoteDeviceProperties,
  .pxGetRemoteDeviceProperty = prvGetRemoteDeviceProperty,
  .pxSetRemoteDeviceProperty = prvSetRemoteDeviceProperty,
  .pxPair = prvPair,
  .pxCreateBondOutOfBand = prvCreateBondOutOfBand,
  .pxSendSlaveSecurityRequest = prvSendSlaveSecurityRequest,
  .pxCancelBond = prvCancelBond,
  .pxRemoveBond = prvRemoveBond,
  .pxGetConnectionState = prvGetConnectionState,
  .pxPinReply = prvPinReply,
  .pxSspReply = prvSspReply,
  .pxReadEnergyInfo = prvReadEnergyInfo,
  .pxDutModeConfigure = prvDutModeConfigure,
  .pxDutModeSend = prvDutModeSend,
  .pxLeTestMode = prvLeTestMode,
  .pxConfigHCISnoopLog = prvConfigHCISnoopLog,
  .pxConfigClear = prvConfigClear,
  .pxReadRssi = prvReadRssi,
  .pxGetTxpower = prvGetTxpower,
  .pxGetClassicAdapter = prvGetClassicAdapter,
  .pxGetLeAdapter = prvGetLeAdapter,
  .pxGetLastError = prvGetLastError,
  .pxGetStackFeaturesSupport = prvGetStackFeaturesSupport
};

/**
 * @brief Return the Bluetooth interface functions
 */
const BTInterface_t * BTGetBluetoothInterface()
{
  return &prvSilabsBTInterface;
}

/**
 * @brief Triggered on system boot event
 */
static void prvOnSystemBoot( sl_bt_evt_system_boot_t * pxEvent )
{
  SILABS_BLE_LOG_FUNC_ENTRY_INFO( "eBtState=%d, pxEvent->version=%"PRIu16".%"PRIu16".%"PRIu16,
                                  ( int ) xSlBtHalManager.eBtState,
                                  pxEvent->major, pxEvent->minor, pxEvent->patch );

  /* Unused parameter */
  ( void ) pxEvent;

  /* If the Bluetooth stack was in the starting state, it has now started */
  if( xSlBtHalManager.eBtState == eSlBtHalBluetoothStarting )
  {
    /* Initialize to the configuration that the BLE HAL uses */
    BTState_t eBtState = eBTstateOn;
    BTStatus_t status = prvInitBluetoothState( );
    if( status != eBTStatusSuccess )
    {
      /* When failed, stop the Bluetooth stack to avoid getting into a partially
      initialized state */
      SILABS_BLE_LOG_FUNC_EXIT_ERROR( "prvInitBluetoothState() failed, status=%d", status );
      sl_bt_system_stop_bluetooth( );
      eBtState = eBTstateOff;
      xSlBtHalManager.eBtState = eSlBtHalBluetoothStopped;
    }
    else
    {
      /* Successfully started */
      xSlBtHalManager.eBtState = eSlBtHalBluetoothStarted;
    }

    /* Give the application the state change callback */
    if( pxBtManagerCallbacks && pxBtManagerCallbacks->pxDeviceStateChangedCb )
    {
      SILABS_BLE_LOG_CB_CALL_INFO( "pxDeviceStateChangedCb", "state=%d", ( int ) eBtState );
      pxBtManagerCallbacks->pxDeviceStateChangedCb( eBtState );
      SILABS_BLE_LOG_CB_RETURN_INFO( "pxDeviceStateChangedCb" );
    }
  }

  SILABS_BLE_LOG_FUNC_EXIT_INFO( "%s", "" );
}

/**
 * @brief Triggered on system error event
 */
static void prvOnSystemError( sl_bt_evt_system_error_t * pxEvent )
{
  SILABS_BLE_LOG_FUNC_ENTRY_ERROR( "eBtState=%d, reason=%"PRIu16,
                                   ( int ) xSlBtHalManager.eBtState, pxEvent->reason );

  /* Unused parameter if logs are disabled */
  ( void ) pxEvent;

  /* If the Bluetooth stack was in the starting state, the system error event
  indicates an error to start the stack */
  if( xSlBtHalManager.eBtState == eSlBtHalBluetoothStarting )
  {
    /* Give the application the state change callback to inform Bluetooth is still off */
    BTState_t eBtState = eBTstateOff;
    if( pxBtManagerCallbacks && pxBtManagerCallbacks->pxDeviceStateChangedCb )
    {
      SILABS_BLE_LOG_CB_CALL_INFO( "pxDeviceStateChangedCb", "state=%d", ( int ) eBtState );
      pxBtManagerCallbacks->pxDeviceStateChangedCb( eBtState );
      SILABS_BLE_LOG_CB_RETURN_INFO( "pxDeviceStateChangedCb" );
    }
  }

  SILABS_BLE_LOG_FUNC_EXIT_ERROR( "%s", "" );
}

static BTAuthFailureReason_t prvConvertSmpErrorCode( sl_status_t xStatus )
{
  switch( xStatus )
  {
    case SL_STATUS_BT_SMP_PASSKEY_ENTRY_FAILED:
      return eBTauthFailSmpPasskeyFail;

    case SL_STATUS_BT_SMP_OOB_NOT_AVAILABLE:
      return eBTauthFailSmpOobFail;

    case SL_STATUS_BT_SMP_CONFIRM_VALUE_FAILED:
      return eBTauthFailSmpCfrmValue;

    case SL_STATUS_BT_SMP_NUMERIC_COMPARISON_FAILED:
      return eBTauthFailSmpPasskeyFail;

    case SL_STATUS_BT_SMP_PAIRING_NOT_SUPPORTED:
      return eBTauthFailSmpPairNotSupport;

    case SL_STATUS_BT_SMP_UNSPECIFIED_REASON:
      return eBTauthFailUnspecified;

    default:
      return eBTauthFailSmp;
  }
}

/**
 * @brief Triggered when a pairing failed.
 *
 */
static void prvOnBondingFailed( sl_bt_evt_sm_bonding_failed_t * pxEvent )
{
  BTBdaddr_t *pxRemoteAddress = prvGetConnectionAddress( pxEvent->connection );
  if( pxRemoteAddress )
  {
    if( pxBtManagerCallbacks && pxBtManagerCallbacks->pxPairingStateChangedCb )
    {
      pxBtManagerCallbacks->pxPairingStateChangedCb( eBTStatusFail,
                                                     pxRemoteAddress,
                                                     eBTbondStateNone,
                                                     eBTSecLevelNoSecurity,
                                                     prvConvertSmpErrorCode( pxEvent->reason) );
    }
  }
}

/**
 * @brief Triggered when a pairing confirmation or passkey handling is requested.
 *
 */
static void prvOnPairingRequest( uint8_t ucConnectionHandle,
                                 BTSspVariant_t xsspVariant,
                                 uint32_t ulPasskey )
{
  BTBdaddr_t *pxRemoteAddress = prvGetConnectionAddress( ucConnectionHandle );
  if( pxRemoteAddress )
  {
    if( pxBtManagerCallbacks && pxBtManagerCallbacks->pxSspRequestCb )
    {
      pxBtManagerCallbacks->pxSspRequestCb( pxRemoteAddress,
                                            NULL, /* pxRemoteBdName not available */
                                            0, /* LE has no Class of Device */
                                            xsspVariant,
                                            ulPasskey );
    }
  }
}

/* Invoked on Bluetooth event to handle events the BT HAL Manager needs. */
static void prvBtHalManagerOnSlBtEvent( sl_bt_msg_t* evt )
{
  switch( SL_BT_MSG_ID( evt->header ) )
  {
    case sl_bt_evt_system_boot_id:
      prvOnSystemBoot( &evt->data.evt_system_boot );
      break;

    case sl_bt_evt_system_error_id:
      prvOnSystemError( &evt->data.evt_system_error );
      break;

    case sl_bt_evt_sm_bonding_failed_id:
      prvOnBondingFailed( &evt->data.evt_sm_bonding_failed );
      break;

    case sl_bt_evt_sm_confirm_bonding_id:
      prvOnPairingRequest(evt->data.evt_sm_confirm_bonding.connection,
                          eBTsspVariantConsent,
                          0);
      break;

    case sl_bt_evt_sm_passkey_display_id:
      prvOnPairingRequest(evt->data.evt_sm_passkey_display.connection,
                          eBTsspVariantPasskeyNotification,
                          evt->data.evt_sm_passkey_display.passkey);
      break;

    case sl_bt_evt_sm_passkey_request_id:
      prvOnPairingRequest(evt->data.evt_sm_passkey_request.connection,
                          eBTsspVariantPasskeyEntry,
                          0);
      break;

    case sl_bt_evt_sm_confirm_passkey_id:
      prvOnPairingRequest(evt->data.evt_sm_confirm_passkey.connection,
                          eBTsspVariantPasskeyConfirmation,
                          evt->data.evt_sm_confirm_passkey.passkey);
      break;

    /* Other events are ignored */
    default:
      break;
  }
}

/**
 * @brief Bluetooth stack event handler.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 */
void sl_bt_hal_on_event(sl_bt_msg_t *evt)
{
  /* Handle events in BT HAL Manager */
  prvBtHalManagerOnSlBtEvent( evt );

  /* Handle events in BLE adapter */
  prvBleAdapterOnSlBtEvent( evt );

  /* Handle events in GATT server */
  prvGattServerOnSlBtEvent( evt );
}

void prvBtHalManagerOnNewPairingEvent( uint8_t ucBondingHandle,
                                       BTBdaddr_t * pxAddress,
                                       BTSecurityLevel_t xSecurityLevel )
{
  if( pxBtManagerCallbacks && pxBtManagerCallbacks->pxPairingStateChangedCb )
  {
    /* eBTbondStateNone indicates a not-bonded pairing */
    BTBondState_t xBondState = ( ucBondingHandle == SL_BT_INVALID_BONDING_HANDLE )
        ? eBTbondStateNone : eBTbondStateBonded;
    pxBtManagerCallbacks->pxPairingStateChangedCb( eBTStatusSuccess,
                                                   pxAddress,
                                                   xBondState,
                                                   xSecurityLevel,
                                                   eBTauthSuccess );
  }
}
