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

#ifndef SL_BT_HAL_COMMON_H
#define SL_BT_HAL_COMMON_H

/* Standard library includes */
#include <stddef.h>
#include <inttypes.h>

/* FreeRTOS BLE HAL includes */
#include "bt_hal_manager_adapter_ble.h"

/* Silicon Labs includes */
#include "sl_bt_hal_config.h"
#include "sl_status.h"
#include "sl_bt_rtos_adaptation.h"

/** @brief Helper macro to set the format string for formatting a Bluetooth address */
#define LOG_BDADDR_FORMAT "%02x:%02x:%02x:%02x:%02x:%02x"

/** @brief Helper macro to extract the data bytes for logging a BTBdaddr_t */
#define LOG_BTBDADDR_T_VALUE(pAddr)  \
  (unsigned) (pAddr)->ucAddress[5],  \
  (unsigned) (pAddr)->ucAddress[4],  \
  (unsigned) (pAddr)->ucAddress[3],  \
  (unsigned) (pAddr)->ucAddress[2],  \
  (unsigned) (pAddr)->ucAddress[1],  \
  (unsigned) (pAddr)->ucAddress[0]

/** @brief Helper macro to extract the data bytes for logging a bd_addr */
#define LOG_BDADDR_VALUE(pAddr)      \
  (unsigned) (pAddr)->addr[5],       \
  (unsigned) (pAddr)->addr[4],       \
  (unsigned) (pAddr)->addr[3],       \
  (unsigned) (pAddr)->addr[2],       \
  (unsigned) (pAddr)->addr[1],       \
  (unsigned) (pAddr)->addr[0]

/* Level-specific wrappers to simplify logging */
#define SILABS_BLE_LOG_PRINT_ERROR(fmt, ...) \
  SILABS_BLE_LOG( SILABS_BLE_LOG_ERROR, SILABS_BLE_HAL, fmt, ##__VA_ARGS__ )
#define SILABS_BLE_LOG_PRINT_WARN(fmt, ...) \
  SILABS_BLE_LOG( SILABS_BLE_LOG_WARN, SILABS_BLE_HAL, fmt, ##__VA_ARGS__ )
#define SILABS_BLE_LOG_PRINT_INFO(fmt, ...) \
  SILABS_BLE_LOG( SILABS_BLE_LOG_INFO, SILABS_BLE_HAL, fmt, ##__VA_ARGS__ )
#define SILABS_BLE_LOG_PRINT_DEBUG(fmt, ...) \
  SILABS_BLE_LOG( SILABS_BLE_LOG_DEBUG, SILABS_BLE_HAL, fmt, ##__VA_ARGS__ )
#define SILABS_BLE_LOG_PRINT_FATAL(fmt, ...) \
  SILABS_BLE_LOG( SILABS_BLE_LOG_FATAL, SILABS_BLE_HAL, fmt, ##__VA_ARGS__ )

/*
 * @brief Helper macro to simplify the logging of function entry
 *
 * Intended usage:
 * @code
 * BTStatus_t prvExample(uint32_t ulParam1, uint32_t ulParam2)
 * {
 *   SILABS_BLE_LOG_FUNC_ENTRY( SILABS_BLE_LOG_INFO,
 *                              "ulParam1=%"PRIu32", ulParam2=%"PRIu32,
 *                              ulParam1, ulParam2 );
 *
 *   ...
 * }
 * @endcode
 *
 * Assuming parameter values 42 and 57 respectively, this will output something
 * like this:
 * @verbatim
 * SILABS_BLE_HAL: > prvExample( ulParam1=42, ulParam2=57 )
 * @endverbatim
 */
#define SILABS_BLE_LOG_FUNC_ENTRY( level, fmt, ... ) \
  SILABS_BLE_LOG( level, SILABS_BLE_HAL, "> %s( " fmt " )", __func__, ##__VA_ARGS__ )

/* Level-specific wrappers */
#define SILABS_BLE_LOG_FUNC_ENTRY_DEBUG( fmt, ... ) \
    SILABS_BLE_LOG_FUNC_ENTRY( SILABS_BLE_LOG_DEBUG, fmt, ##__VA_ARGS__ )
#define SILABS_BLE_LOG_FUNC_ENTRY_INFO( fmt, ... ) \
    SILABS_BLE_LOG_FUNC_ENTRY( SILABS_BLE_LOG_INFO, fmt, ##__VA_ARGS__ )
#define SILABS_BLE_LOG_FUNC_ENTRY_ERROR( fmt, ... ) \
    SILABS_BLE_LOG_FUNC_ENTRY( SILABS_BLE_LOG_ERROR, fmt, ##__VA_ARGS__ )

/*
 * @brief Helper macro to simplify the logging of function exit
 *
 * Intended usage:
 * @code
 * BTStatus_t prvExample(uint32_t ulParam1, uint21_t ulParam2)
 * {
 *   ...
 *   SILABS_BLE_LOG_FUNC_EXIT( SILABS_BLE_LOG_INFO, "status=%d", (int) status );
 *   return status;
 * }
 * @endcode
 *
 * Assuming return status 0, this will output something like this:
 * @verbatim
 * SILABS_BLE_HAL: < prvExample, status=0
 * @endverbatim
 */
#define SILABS_BLE_LOG_FUNC_EXIT( level, fmt, ... ) \
  SILABS_BLE_LOG( level, SILABS_BLE_HAL, "< %s, " fmt, __func__, ##__VA_ARGS__ )

/* Level-specific wrappers */
#define SILABS_BLE_LOG_FUNC_EXIT_DEBUG( fmt, ... ) \
    SILABS_BLE_LOG_FUNC_EXIT( SILABS_BLE_LOG_DEBUG, fmt, ##__VA_ARGS__ )
#define SILABS_BLE_LOG_FUNC_EXIT_INFO( fmt, ... ) \
    SILABS_BLE_LOG_FUNC_EXIT( SILABS_BLE_LOG_INFO, fmt, ##__VA_ARGS__ )
#define SILABS_BLE_LOG_FUNC_EXIT_ERROR( fmt, ... ) \
    SILABS_BLE_LOG_FUNC_EXIT( SILABS_BLE_LOG_ERROR, fmt, ##__VA_ARGS__ )

/*
 * @brief Helper macro to simplify the logging of calls to callbacks
 *
 * Intended usage:
 * @code
 * BTStatus_t prvExample( )
 * {
 *   SILABS_BLE_LOG_CB_CALL( SILABS_BLE_LOG_INFO, "pxExampleCb",
 *                           "ulCbParam1=%"PRIu32", ulCbParam1 );
 *   pxBleAdapterCallbacks->pxExampleCb(ulCbParam1);
 *   SILABS_BLE_LOG_CB_RETURN( SILABS_BLE_LOG_INFO, "pxExampleCb" );
 * }
 * @endcode
 *
 * Assuming callback parameter value 42, this will output something like this:
 * @verbatim
 * SILABS_BLE_HAL: > callback pxExampleCb( ulCbParam1=42 )
 * SILABS_BLE_HAL: < callback pxExampleCb
 * @endverbatim
 */
#define SILABS_BLE_LOG_CB_CALL( level, cb, fmt, ... )                   \
  SILABS_BLE_LOG( level, SILABS_BLE_HAL, "> callback " cb "( " fmt " )", ##__VA_ARGS__ )

/* Level-specific wrappers */
#define SILABS_BLE_LOG_CB_CALL_DEBUG( cb, fmt, ... ) \
    SILABS_BLE_LOG_CB_CALL( SILABS_BLE_LOG_DEBUG, cb, fmt, ##__VA_ARGS__ )
#define SILABS_BLE_LOG_CB_CALL_INFO( cb, fmt, ... ) \
    SILABS_BLE_LOG_CB_CALL( SILABS_BLE_LOG_INFO, cb, fmt, ##__VA_ARGS__ )
#define SILABS_BLE_LOG_CB_CALL_ERROR( cb, fmt, ... ) \
    SILABS_BLE_LOG_CB_CALL( SILABS_BLE_LOG_ERROR, cb, fmt, ##__VA_ARGS__ )

/*
 * @brief Helper macro to simplify the logging of returns from callbacks
 *
 * Intended usage:
 * @code
 * BTStatus_t prvExample( )
 * {
 *   SILABS_BLE_LOG_CB_CALL( SILABS_BLE_LOG_INFO, "pxExampleCb",
 *                           "ulCbParam1=%"PRIu32", ulCbParam1 );
 *   pxBleAdapterCallbacks->pxExampleCb(ulCbParam1);
 *   SILABS_BLE_LOG_CB_RETURN( SILABS_BLE_LOG_INFO, "pxExampleCb" );
 * }
 * @endcode
 *
 * Assuming callback parameter value 42, this will output something like this:
 * @verbatim
 * SILABS_BLE_HAL: > callback pxExampleCb( ulCbParam1=42 )
 * SILABS_BLE_HAL: < callback pxExampleCb
 * @endverbatim
 */
#define SILABS_BLE_LOG_CB_RETURN( level, cb ) \
  SILABS_BLE_LOG( level, SILABS_BLE_HAL, "< callback " cb )

/* Level-specific wrappers */
#define SILABS_BLE_LOG_CB_RETURN_DEBUG( cb ) \
    SILABS_BLE_LOG_CB_RETURN( SILABS_BLE_LOG_DEBUG, cb )
#define SILABS_BLE_LOG_CB_RETURN_INFO( cb ) \
    SILABS_BLE_LOG_CB_RETURN( SILABS_BLE_LOG_INFO, cb )
#define SILABS_BLE_LOG_CB_RETURN_ERROR( cb ) \
    SILABS_BLE_LOG_CB_RETURN( SILABS_BLE_LOG_ERROR, cb )

/*
 * @brief Helper macro to simplify the logging of unsupported functions
 *
 * Intended usage:
 * @code
 * BTStatus_t prvUnsupported(uint32_t ulParam1, uint32_t ulParam2)
 * {
 *   SILABS_BLE_LOG_FUNC_UNSUPPORTED(SILABS_BLE_LOG_WARN);
 *   ...
 *   return eBTStatusUnsupported;
 * }
 * @endcode
 *
 * This will output something like this:
 * @verbatim
 * SILABS_BLE_HAL: Function prvUnsupported is not supported
 * @endverbatim
 */
#define SILABS_BLE_LOG_FUNC_UNSUPPORTED( level ) \
  SILABS_BLE_LOG( level, SILABS_BLE_HAL, "Function %s is not supported", __func__ )

/**
 * @brief Structure to collect the data of one client registration.
 *
 * The BLE adapter, the GATT Server, and the GATT Client interfaces use a
 * registration mechanism that takes a UUID of the registrant and then use a
 * uint8_t handle to identify the client. This structure and the functions
 * associated with it implement a generic registration mechanism that the
 * mentioned interfaces can use.
 *
 * This structure is intended to be used as the first field in
 * interface-specific context structures.
 */
typedef struct SlBtBleHalClient
{
  BTUuid_t                  xAppUuid;  /**< The UUID that the client registered with */
  uint8_t                   ucHandle;  /**< Handle to identify this client */
  struct SlBtBleHalClient * pxNext;    /**< Pointer to the next registered client, if any */
} SlBtBleHalClient_t;

/** @brief Value used to indicate an invalid `ucHandle` */
#define SL_BT_INVALID_IF_HANDLE    ((uint8_t) 0)

/**
 * @brief Register a BLE HAL client
 *
 * @param[in] pxAppUuid UUID of the registrant
 * @param[in,out] ppxClientListHead Pointer to the head of the list of clients
 * @param[in] ucMaxClients Maximum number of concurrently registered clients
 * @param[in] xContextSize Size of API-specific context structure
 * @param[out] ppxClient On success, set to point to the allocated context
 *
 * @return eBTStatusSuccess if successful, otherwise an error code
 */
BTStatus_t prvRegisterBleHalClient( BTUuid_t * pxAppUuid,
                                    SlBtBleHalClient_t ** ppxClientListHead,
                                    uint8_t ucMaxClients,
                                    size_t xContextSize,
                                    SlBtBleHalClient_t ** ppxClient );

/**
 * @brief Free a BLE HAL client context
 *
 * @param[in,out] ppxClientListHead Pointer to the head of the list of clients
 * @param[in] pxClient The client context memory to free
 */
void prvFreeBleHalClient( SlBtBleHalClient_t ** ppxClientListHead,
                          SlBtBleHalClient_t * pxClient );

/**
 * @brief Find a BLE HAL client context based on its handle
 *
 * @param[in] ucHandle Handle of the client context
 * @param[in] pxFirstClient Pointer to the first client in the list
 *
 * @return Pointer to the client context, or NULL if the handle was not valid
 */
SlBtBleHalClient_t * prvGetBleHalClientByHandle( uint8_t ucHandle,
                                                 SlBtBleHalClient_t * pxFirstClient );

/**
 * @brief Map an sl_status_t value to a BTStatus_t.
 *
 * @param[in] sl_status One of the sl_status_t error codes
 *
 * @return Corresponding BTStatus_t error code
 */
BTStatus_t prvSlStatusToBTStatus( sl_status_t sl_status );

/**
 * @brief Map an sl_status_t value to a BTGattStatus_t.
 *
 * @param[in] sl_status One of the sl_status_t error codes
 *
 * @return Corresponding BTGattStatus_t error code
 */
BTGattStatus_t prvSlStatusToBTGattStatus( sl_status_t sl_status );

/**
 * @brief Check if two UUIDs match
 *
 * @param[in] pxUuid1 First UUID to compare
 * @param[in] pxUuid2 Second UUID to compare
 *
 * @return True if UUIDs match, false otherwise
 */
bool prvIsMatchingUuid( BTUuid_t * pxUuid1, BTUuid_t * pxUuid2 );

#endif /* SL_BT_HAL_COMMON_H */
