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

/**************************************************************************//**
* \file
*
* \brief This file provides definitions of the Alert Notification Client (ANC)
* library interface.
*
******************************************************************************/

#ifndef ANC_CLIENT_H
#define ANC_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* \addtogroup  wiced_bt_anc_api_functions        ANC Library API
* \ingroup     wicedbt
* @{
* ANC library of the AIROC BTSDK provide a simple method for an application to integrate ANC
* service functionality. Application calls the library APIs to discover/enable/disable/control/read
* the alerts.
*/

#include "wiced_bt_anp.h"

/**
* \Brief ANC Events received by the applicaton's ANC callback (see \ref wiced_bt_anc_callback_t)
*
*/
typedef enum
{
    WICED_BT_ANC_DISCOVER_RESULT,                       /**< ANC Discovery Result */
    WICED_BT_ANC_READ_SUPPORTED_NEW_ALERTS_RESULT,      /**< ANC read Supported New Alert Result */
    WICED_BT_ANC_READ_SUPPORTED_UNREAD_ALERTS_RESULT,   /**< ANC read Supported Unread Alert Result */
    WICED_BT_ANC_CONTROL_ALERTS_RESULT,                 /**< ANC Control Alert Result */
    WICED_BT_ANC_ENABLE_NEW_ALERTS_RESULT,              /**< ANC Enable New Alert Notification Result */
    WICED_BT_ANC_DISABLE_NEW_ALERTS_RESULT,             /**< ANC Disable New Alert Notification Result */
    WICED_BT_ANC_ENABLE_UNREAD_ALERTS_RESULT,           /**< ANC Enable Unread Alert Notification Result */
    WICED_BT_ANC_DISABLE_UNREAD_ALERTS_RESULT,          /**< ANC Disable Unread Alert Notification Result */
    WICED_BT_ANC_EVENT_NEW_ALERT_NOTIFICATION,          /**< ANC New Alert Notification */
    WICED_BT_ANC_EVENT_UNREAD_ALERT_NOTIFICATION,       /**< ANC Unread Alert Notification */
} wiced_bt_anc_event_t;

/**
* \brief Data associated with WICED_BT_ANC_DISCOVER_RESULT.
*
*/
typedef struct
{
    uint16_t                conn_id;
    wiced_bt_gatt_status_t  status;
} wiced_bt_anc_discovery_result_t;

/**
* \brief Data associated with  WICED_BT_ANC_READ_SUPPORTED_NEW_ALERTS_RESULT.
*
*/
typedef struct
{
    uint16_t                                conn_id;
    wiced_bt_anp_alert_category_enable_t    supported_alerts;
    wiced_bt_gatt_status_t                  status;
} wiced_bt_anc_supported_new_alerts_result_t;

/**
* \brief Data associated with  WICED_BT_ANC_READ_SUPPORTED_UNREAD_ALERTS_RESULT.
*
*/
typedef struct
{
    uint16_t                                conn_id;
    wiced_bt_anp_alert_category_enable_t    supported_alerts;
    wiced_bt_gatt_status_t                  status;
} wiced_bt_anc_supported_unread_alerts_result_t;

/**
* \brief Data associated with  WICED_BT_ANC_CONTROL_ALERTS_RESULT.
*
*/
typedef struct
{
    uint16_t                             conn_id;
    wiced_bt_anp_alert_control_cmd_id_t  control_point_cmd_id;
    wiced_bt_anp_alert_category_id_t     category_id;
    wiced_bt_gatt_status_t               status;
} wiced_bt_anc_control_alerts_result_t;

/**
* \brief Data associated with WICED_BT_ANC_ENABLE_NEW_ALERTS_RESULT and  WICED_BT_ANC_DISABLE_NEW_ALERTS_RESULT.
*
*/
typedef struct
{
    uint16_t                conn_id;
    wiced_bt_gatt_status_t  status;
} wiced_bt_anc_enable_disable_alerts_result_t;

/**
* \brief Data associated with WICED_BT_ANC_EVENT_NEW_ALERT_NOTIFICATION
*
*/
typedef struct
{
    uint16_t                           conn_id;
    wiced_bt_anp_alert_category_id_t   new_alert_type;
    uint8_t                            new_alert_count;
    char                               *p_last_alert_data; /* Null terminated string */
} wiced_bt_anc_new_alert_notification_t;

/**
* \brief  Data associated with WICED_BT_ANC_EVENT_UNREAD_ALERT_NOTIFICATION
*
*/
typedef struct
{
    uint16_t                           conn_id;
    wiced_bt_anp_alert_category_id_t   unread_alert_type;
    uint8_t                            unread_count;
} wiced_bt_anc_unread_alert_notification_t;



/**
* \brief Union of data associated with ANC events. The ANC library calls the application's
* callback registered with a pointer on such structure.
*
*/
typedef union
{
    wiced_bt_anc_discovery_result_t                 discovery_result;
    wiced_bt_anc_supported_new_alerts_result_t      supported_new_alerts_result;
    wiced_bt_anc_supported_unread_alerts_result_t   supported_unread_alerts_result;
    wiced_bt_anc_control_alerts_result_t            control_alerts_result;
    wiced_bt_anc_enable_disable_alerts_result_t     enable_disable_alerts_result;
    wiced_bt_anc_new_alert_notification_t           new_alert_notification;
    wiced_bt_anc_unread_alert_notification_t        unread_alert_notification;
} wiced_bt_anc_event_data_t;

/**
* ANC Callback function type wiced_bt_anc_callback_t
*
*                  This function is called to send ANC events to the application.
*                  This function is registered with the \ref wiced_bt_anc_init function.
*
* \param[in]       event  : ANC Event.
* \param[in]       p_data : Data (pointer on union of structure) associated with the event.
*
* \return NONE.
*/
typedef void (wiced_bt_anc_callback_t)(wiced_bt_anc_event_t event, wiced_bt_anc_event_data_t *p_data);

/*****************************************************************************
*                         Function Prototypes
*****************************************************************************/

/*****************************************************************************
*
* Function Name: wiced_bt_anc_init
*
***************************************************************************//**
* The application calls this API on an application start to initialize the AIROC BTSDK ANS server
* library.
* Application registered callbacks gets called when requested operations completed.
* Once connection success, Application does ANS service discovery.
* If ANS service is discovered successfully, calls below for Characteristic discovery.
*
* \param           p_reg  : Registration control block that includes ANC application call backs.
*
* \return          WICED_SUCCESS if ANC initialized successfully, error otherwise.
*
*****************************************************************************/
wiced_result_t wiced_bt_anc_init(wiced_bt_anc_callback_t *p_callback);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_discover
*
***************************************************************************//**
*
* The application calls this API to Performs ANC characteristics discovery and characteristic
* descriptor discovery.
* Once discovery complete, registered application p_op_complete_callback is called with result of
* operation.
*
* \param           conn_id      : GATT connection ID.
* \param           start_handle : Start GATT handle of the ANC service.
* \param           end_handle   : End GATT handle of the ANC service.
*
* \return          Status of the GATT operation.
*
*****************************************************************************/
wiced_bt_gatt_status_t wiced_bt_anc_discover(uint16_t conn_id, uint16_t start_handle,
        uint16_t end_handle);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_discovery_result
*
***************************************************************************//**
*
* The application calls this API during GATT discovery to pass discovery results for the ANC
* service to the ANC Library.
* The library needs to find ANC service characteristics and associated characteristic client
* configuration descriptors.
*
* \param           p_data   : Discovery result data as passed from the stack.
*
* \return          none.
*
*****************************************************************************/
void wiced_bt_anc_discovery_result(wiced_bt_gatt_discovery_result_t *p_data);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_client_discovery_complete
*
***************************************************************************//**
*
* The application calls this API during GATT discovery to pass discovery complete information for
* the ANC service to the ANC Library.
* As the GATT discovery is performed in multiple steps this function initiates the next discovery
* request.
*
* \param           p_data   : Discovery complete data as passed from the stack.
*
* \return          none.
*
*****************************************************************************/
void wiced_bt_anc_client_discovery_complete(wiced_bt_gatt_discovery_complete_t *p_data);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_read_server_supported_new_alerts
*
***************************************************************************//**
*
* Once GATT discovery is complete, the Application calls this API to read the supported new alerts.
* Upon reception of the GATT operation result, the application must provides GATT operation result
* through wiced_bt_anc_read_rsp API.
*
* \param           conn_id: GATT connection id.
*
* \return          Status of the GATT operation.
*
*****************************************************************************/
wiced_bt_gatt_status_t wiced_bt_anc_read_server_supported_new_alerts(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_read_server_supported_unread_alerts
*
***************************************************************************//**
*
* Once GATT discovery is complete, the Application call to Read the Value of Supported Unread
* Alert Categories.
* Upon reception of the GATT operation result, the application must provides GATT operation result
* through wiced_bt_anc_read_rsp API.
*
* \param           conn_id: GATT connection id.
*
* \return          Status of the GATT operation.
*
*****************************************************************************/
wiced_bt_gatt_status_t wiced_bt_anc_read_server_supported_unread_alerts(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_control_required_alerts
*
***************************************************************************//**
*
* The application use this API to control notifications using Alert notification control point
* characteristic.
* Upon reception of the GATT operation result, the application must provides GATT operation result
* through wiced_bt_anc_read_rsp API.
*
* \param           conn_id  : GATT connection id.
* \param           cmd_id   : ANC alert command id.
* \param           category : ANC alert category id.
*
* \return          Status of the GATT operation.
*
*****************************************************************************/
wiced_bt_gatt_status_t wiced_bt_anc_control_required_alerts(uint16_t conn_id,
        wiced_bt_anp_alert_control_cmd_id_t cmd_id, wiced_bt_anp_alert_category_id_t category);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_enable_new_alerts
*
***************************************************************************//**
*
* The application use this API to write client characteristic configuration descriptor (to start
* receiving new alerts).
* Upon reception of the GATT operation result, the application must provides GATT operation result
* through wiced_bt_anc_read_rsp API.
*
* \param           conn_id: GATT connection id.
*
* \return          Status of the GATT operation.
*
*****************************************************************************/
wiced_bt_gatt_status_t wiced_bt_anc_enable_new_alerts(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_disable_new_alerts
*
***************************************************************************//**
*
* The application use this API to write client characteristic configuration descriptor (to stop
* receiving new alerts).
* Upon reception of the GATT operation result, the application must provides GATT operation result
* through wiced_bt_anc_read_rsp API.
*
* \param           conn_id: GATT connection id.
*
* \return          Status of the GATT operation.
*
*****************************************************************************/
wiced_bt_gatt_status_t wiced_bt_anc_disable_new_alerts(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_enable_unread_alerts
*
***************************************************************************//**
*
* The application use this API to write client characteristic configuration descriptor (to start
* receiving new unread alerts).
* Upon reception of the GATT operation result, the application must provides GATT operation result
* through wiced_bt_anc_read_rsp API.
*
* \param           conn_id: GATT connection id.
*
* \return          Status of the GATT operation.
*
*****************************************************************************/
wiced_bt_gatt_status_t wiced_bt_anc_enable_unread_alerts(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_disable_unread_alerts
*
***************************************************************************//**
*
* The application use this API to write client characteristic configuration descriptor (to stop
* receiving new unread alerts).
* Upon reception of the GATT operation result, the application must provides GATT operation result
* through wiced_bt_anc_read_rsp API.
*
* \param           conn_id: GATT connection id.
*
* \return          Status of the GATT operation.
*
*****************************************************************************/
wiced_bt_gatt_status_t wiced_bt_anc_disable_unread_alerts(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_recover_new_alerts_from_conn_loss
*
***************************************************************************//**
*
* The application use this API to enable new alerts and control alerts.
* This function sequentially call wiced_bt_anc_enable_new_alerts and
* wiced_bt_anc_control_required_alerts API inside the library.
*
* \param           conn_id: GATT connection id.
* \param           cmd_id:  ANC alert command id.
* \param           category: ANC alert category id.
*
* \return          Status of the GATT operation.
*
*****************************************************************************/
wiced_bt_gatt_status_t wiced_bt_anc_recover_new_alerts_from_conn_loss(uint16_t conn_id,
        wiced_bt_anp_alert_control_cmd_id_t cmd_id, wiced_bt_anp_alert_category_id_t category);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_recover_new_unread_alerts_from_conn_loss
*
***************************************************************************//**
*
* The application use this API to enable unread alerts and control alerts.
* This function sequentially call wiced_bt_anc_enable_unread_alerts an
*  wiced_bt_anc_control_required_alerts API inside the library.
*
* \param          conn_id: GATT connection id.
* \param          cmd_id:  ANC alert command id.
* \param          category: ANC alert category id.
*
* \return          Status of the GATT operation.
*
*****************************************************************************/
wiced_bt_gatt_status_t wiced_bt_anc_recover_new_unread_alerts_from_conn_loss(uint16_t conn_id,
        wiced_bt_anp_alert_control_cmd_id_t cmd_id, wiced_bt_anp_alert_category_id_t category);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_write_rsp
*
***************************************************************************//**
*
* The application should call this function when it receives GATT Write Response
* for the attribute handle which belongs to the ANC service.
*
* \param           p_data  : pointer to a GATT operation complete data structure.
*
* \return          none.
*
*****************************************************************************/
void wiced_bt_anc_write_rsp(wiced_bt_gatt_operation_complete_t *p_data);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_read_rsp
*
***************************************************************************//**
*
* The application should call this function when it receives GATT Read Response
* for the attribute handle which belongs to the ANC service.
* This function will parse the data and provide the result through p_op_complete_callback.
*
* \param           p_data  : pointer to a GATT operation complete data structure.
*
* \return          none.
*
*****************************************************************************/
void wiced_bt_anc_read_rsp(wiced_bt_gatt_operation_complete_t *p_data);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_client_connection_up
*
***************************************************************************//**
*
* The application should call this function when BLE connection with a peer
* device has been established.
*
* \param           p_conn_status  : pointer to a wiced_bt_gatt_connection_status_t which includes
*                                   the address and connection ID.
*
* \return          none
*
*****************************************************************************/
void wiced_bt_anc_client_connection_up(wiced_bt_gatt_connection_status_t *p_conn_status);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_client_connection_down
*
***************************************************************************//**
*
* The application should call this function when BLE connection with a peer
* device has been disconnected.
*
* \param           p_conn_status  : pointer to a wiced_bt_gatt_connection_status_t which includes
*                                   the address and connection ID.
* \return          none.
*
*****************************************************************************/
void wiced_bt_anc_client_connection_down(wiced_bt_gatt_connection_status_t *p_conn_status);

/*****************************************************************************
*
* Function Name: wiced_bt_anc_client_process_notification
*
***************************************************************************//**
*
* This function processes the ANC process Notification
*
* \param           p_data  : pointer to a GATT operation complete data structure.
*
* \return          none.
*
*****************************************************************************/
void wiced_bt_anc_client_process_notification(wiced_bt_gatt_operation_complete_t *p_data);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* ANC_CLIENT_H */
