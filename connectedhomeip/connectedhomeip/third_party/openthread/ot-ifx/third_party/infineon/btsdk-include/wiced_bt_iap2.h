/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 *
 * This file provides definitions of the iAP2 implementation
 */

#ifndef __IAP2_API_H
#define __IAP2_API_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  wiced_bt_iap2_api_functions        iAP2 Protocol Library API
 * @ingroup     wicedbt
 *
 * iAP2 library of the AIROC BTSDK provides a simple method for an application to integrate iAP2
 * functionality.  A single External Accessory session is supported in the current version.
 * Application just needs to call API to connect/disconnect and send data.  Library in turn
 * indicates status of the connection and passes to the application received data.
 *
 * @{
 */

/// iAP2 Accessory Service
#define IAP2_ACCESSORY_UUID 0x00, 0x00, 0x00, 0x00, 0xde, 0xCA, 0xfA, 0xDE, 0xde, 0xca, 0xde, 0xaf, 0xDE, 0xca, 0xCA, 0xff

/// iAP2 Device Service
#define IAP2_DEVICE_UUID    0x00, 0x00, 0x00, 0x00, 0xde, 0xCA, 0xfA, 0xDE, 0xde, 0xca, 0xde, 0xaf, 0xDE, 0xca, 0xCA, 0xfe

// MAX packet size that can be sent over iAP2
#define IAP2_MAX_PACKET_SIZE    1003
#define PORT_PURGE_TXCLEAR     0x01
#define PORT_PURGE_RXCLEAR     0x02

/**
 * Data provided by the application and utilized by the library
 */
extern uint8_t LocalName[];                     /**<  zero terminating string with BT Local Name */
extern uint8_t ModelIdentifier[];               /**<  zero terminating string with Model Identifier */
extern uint8_t Manufacturer[];                  /**<  zero terminating string with Manufacturer Name */
extern uint8_t SerialNumber[];                  /**<  zero terminating string with device serial number */
extern uint8_t FirmwareVersion[];               /**<  zero terminating string with firmware version number */
extern uint8_t HardwareVersion[];               /**<  zero terminating string with firmware version number */
extern uint8_t ExternalAccessoryProtocolName[]; /**<  zero terminating string with the external accessory protocol name */

#define IAP2_MATCH_ACTION_NO_PROMPT                                 0
#define IAP2_MATCH_ACTION_DEVICE_MAY_PROMPT_AND_FIND_APP_BUTTON     1
#define IAP2_MATCH_ACTION_DEVICE_NO_PROMPT_AND_FIND_APP_BUTTON      2
extern uint8_t MatchAction;

extern uint8_t AppMatchTeamId[];                /**<  zero terminating string with the match team ID */
extern uint8_t CurrentLanguage[];               /**<  zero terminating string with the current language */
extern uint8_t SupportedLanguage[];             /**<  list of zero terminating strings with supported languages for example en\0fr\0\0 */
extern uint8_t ProductPlanUID[];                /**<  zero terminating string with sample ProductPlanUID value */

/**< Size of the buffer required to read chip information from the MFi chip */
#define AUTH_CHIP_INFO_DATA_SIZE                8

/*****************************************************************************
 *          Function Prototypes
 *****************************************************************************/

/**
 * Function         wiced_bt_iap2_connection_up_callback
 *
 * Application must implement connection up callback to be called by the library.  The
 * callback indicates to the application that External Accessory
 * session with a specified device has been successfully established.  This can
 * be a result of the wiced_bt_iap2_connect requested by the application, or due to an
 * iOS device establishing connection from its side.
 *
 * @param[in]       handle   : Handle that identifies the external accessory session
 * @param[in]       bd_addr  : Bluetooth Device address of the connected iOS device.
 * @return          Nothing
 */
typedef void (*wiced_bt_iap2_connection_up_callback_t)(uint16_t handle, uint8_t* bd_addr);

/**
 * Function         wiced_bt_iap2_connection_failed_callback
 *
 * Connection failed callback indicates to the application that the library
 * failed to establish RFCOMM connection with the peer device after connection
 * was requested by the wiced_bt_iap2_connect call.
 *
 * @return          Nothing
 */
typedef void (*wiced_bt_iap2_connection_failed_callback_t)(void);

/**
 * Function         wiced_bt_iap2_service_not_found_callback
 *
 * Service not found callback indicates wiced_bt_iap2_connect call requested to establish
 * connection to a device which is currently not present, or which is not running
 * IAP2 service.
 *
 * @return          Nothing
 */
typedef void (*wiced_bt_iap2_service_not_found_callback_t)(void);

/**
 * Function         wiced_bt_iap2_connection_down_callback
 *
 * Connection Down callback indicates that an active External Accessory session has
 * been terminated.
 *
 * @param[in]       handle   : Handle that identifies the external accessory session
 * @return          Nothing
 */
typedef void (*wiced_bt_iap2_connection_down_callback_t)(uint16_t handle);

/**
 * Function         wiced_bt_iap2_tx_complete_callback
 *
 * Transmission Complete callback indicates that application can send more data.
 *
 * @param[in]       handle   : Handle that identifies the external accessory session
 * @param[in]       result   : Result of tx operaion.  0 if success.
 * @return          Nothing
 */
typedef void (*wiced_bt_iap2_tx_complete_callback_t)(uint16_t handle, wiced_result_t result);

/**
 * Function         wiced_bt_iap2_rx_data_callback
 *
 * Rx Data callback passed to the application data received over the External Accessory
 * session.  The first 2 octets of the data have the handle of the session in the big
 * endian format.
 *
 * @param[in]       handle   : Handle that identifies the external accessory session
 * @param[in]       p_data  : Pointer to buffer with data to send. The first 2 octets of the p_data contain the handle
 *                            passed to the application in the wiced_bt_iap2_connection_up_callback in the big endian format.
 * @param[in]       len :     Length of the data + handle
 * @return          Nothing
 */
typedef wiced_bool_t (*wiced_bt_iap2_rx_data_callback_t)(uint16_t handle, uint8_t* data, uint32_t dataLen);

/*
 * iAP2, additional, optional, Events
 */
typedef enum
{
    WICED_BT_IAP2_EVENT_RFCOMM_CONNECTED,
    WICED_BT_IAP2_EVENT_RFCOMM_DISCONNECTED,
} wiced_bt_iap2_event_t;

/* Data associated with the WICED_BT_IAP2_EVENT_RFCOMM_CONNECTED event */
typedef struct
{
    wiced_bt_device_address_t bdaddr;
    uint16_t port_handle;
} wiced_bt_iap2_event_data_rfcomm_connected_t;

/* Data associated with the WICED_BT_IAP2_EVENT_RFCOMM_DISCONNECTED event */
typedef struct
{
    uint16_t port_handle;
} wiced_bt_iap2_event_data_rfcomm_disconnected_t;

/* Data (union) associated with the wiced_bt_iap2_event_t event */
typedef union
{
    wiced_bt_iap2_event_data_rfcomm_connected_t rfcomm_connected;
    wiced_bt_iap2_event_data_rfcomm_disconnected_t rfcomm_disconnected;
} wiced_bt_iap2_event_data_t;


/**
 * Function         wiced_bt_iap2_callback_t
 */
typedef void (wiced_bt_iap2_callback_t)(wiced_bt_iap2_event_t event,
        wiced_bt_iap2_event_data_t *p_data);

/**
 * Following structure is used to register application with wiced_bt_iap2 library
 */
typedef struct
{
    uint8_t                                     rfcomm_scn;                     /**< Application selects RFCOMM SCN that
                                                                                     it publishes in the SDP and need to
                                                                                     pass the same value for library to use. */
    uint16_t                                    rfcomm_mtu;                     /**< MTU to be be used by the RFCOMM layer */
    wiced_bt_iap2_connection_up_callback_t      p_connection_up_callback;       /**< iAP2 connection established */
    wiced_bt_iap2_connection_failed_callback_t  p_connection_failed_callback;   /**< iAP2 connection establishment failed */
    wiced_bt_iap2_service_not_found_callback_t  p_service_not_found_callback;   /**< iAP2 service not found */
    wiced_bt_iap2_connection_down_callback_t    p_connection_down_callback;     /**< iAP2 connection disconnected */
    wiced_bt_iap2_rx_data_callback_t            p_rx_data_callback;             /**< Data packet received */
    wiced_bt_iap2_callback_t                    *p_callback;
} wiced_bt_iap2_reg_t;

/**
 * Function         wiced_bt_iap2_get_auth_chip_info
 *
 * Read the auth chip info
 *
 * @param[out]      buffer  :Pointer to buffer to get the auth chip info
 * @param[out]      buffer_size  :buffer size
 * @return          Status
 */
 wiced_result_t wiced_bt_iap2_get_auth_chip_info(uint8_t* buffer, uint32_t buffer_size);

/**
 * Function         wiced_bt_iap2_startup
 *
 * Initialize IAP2 library and starts the RFCOMM service.
 *
 * @param[out]      p_reg  : Registration control block that includes RFCOMM SCN and callbacks
 * @return          Nothing
 */
void wiced_bt_iap2_startup(wiced_bt_iap2_reg_t *p_reg);

/**
 * Function         wiced_bt_iap2_connect
 *
 * Establish External Accessory connection to an iOS device.  Library will perform
 * Service Discovery.  If iAP2 service is running on the specified device RFCOMM
 * connection is established.  If connection is established, accessory initiates
 * iAP negotiations which includes accessory authentication and identification.
 * Application on the device is responsible to start External Accessory Session.
 * When session is established, library executes iap2_connection_up_callback.
 *
 * @param[out]      bd_addr  : Bluetooth Device address to connect to.
 * @return          Nothing
 */
void wiced_bt_iap2_connect( uint8_t *bd_addr );

/**
 * Function         wiced_bt_iap2_disconnect
 *
 * Disconnect External Accessory Session with the iOS device.  Bluetooth connection
 * is brought down as well.
 *
 * @param[out]      handle  : The handle returned by the application in the wiced_bt_iap2_connection_up_callback.
 * @return          Nothing
 */
void wiced_bt_iap2_disconnect( uint16_t handle );

/**
 * Function         wiced_bt_iap2_send_session_data
 *
 * Send data over the established External Accessory connection.  The session must
 * be IAP2_EA_SESSION_ID.  The first 2 octets of the p_data must be the handle
 * passed to the application in the wiced_bt_iap2_connection_up_callback in the big
 * endian format.
 *
 * @param[out]      session : Must always be IAP2_EA_SESSION_ID
 * @param[out]      p_data  : Pointer to buffer with data to send. The first 2 octets of the p_data must be the handle
 *                            passed to the application in the wiced_bt_iap2_connection_up_callback in the big endian format.
 * @param[out]      len :     Length of the data + handle
 * @return          WICED_TRUE: if data is scheduled for transmission, otherwise WICED_FALSE
 */
wiced_bool_t wiced_bt_iap2_send_session_data( uint16_t session, uint8_t *p_data, uint32_t len );

/**
 * Function         wiced_bt_iap2_rx_flow_enable
 *
 * IAP2 application may use this call to disable or reenable the RX data flow
 *
 * @param[out]      session : Must always be IAP2_EA_SESSION_ID
 * @param[out]      enable :  If true, data flow is enabled
 * @return          Nothing
 */

void wiced_bt_iap2_rx_flow_enable( uint16_t session, wiced_bool_t enable );

/**
 * Function         wiced_bt_iap2_can_send_more_data
 *
 * Returns TRUE if library can queue forward data, or FALSE if forward data queue
 * is full.
 */
wiced_bool_t wiced_bt_iap2_can_send_more_data(void);

/**
 * iAP2 application may use this function to discard all the data from the
 * output or input queues of the specified connection.
 *
 * @param[in]      handle : Connection handle indicated in the connection up callback
 * @param[in]      purge_flags - specify the action to take with PORT_PURGE_TXCLEAR
 *                 and/or PORT_PURGE_RXCLEAR.
 * @return         rfcomm port return code
 */
void wiced_bt_iap2_port_purge(uint16_t handle, uint8_t purge_flags);

/**
 * iAP2 application use this function to read the peer_mtu size when rfcomm connection made
 * @return         rfcomm port peer mtu size
 */
uint16_t wiced_bt_iap2_rfcomm_get_peer_mtu(uint16_t handle);

/**
 * iAP2 application use this function to get the connection state with the peer specified by btaddr

 * @return         connection state
 */
uint8_t wiced_bt_iap2_get_connection_state( uint8_t *bd_addr );

/**@} wiced_bt_iap2_api_functions */

#ifdef __cplusplus
}
#endif

#endif
