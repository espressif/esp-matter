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
* @file
*
* \brief This file provides definitions of the Heart rate profile Server(HRS)
* library interface.
*
******************************************************************************/

#ifndef WICED_BT_HRS_H
#define WICED_BT_HRS_H

#include "wiced_bt_hrp.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @addtogroup  wiced_bt_hrs_api_functions        HRS Library API
* @ingroup     wicedbt
* @{
* HRS library of the AIROC BTSDK provides a simple method for an application to integrate Heart Rate
* server functionality. Application calls library APIs to process Heart Rate Client(HRC)
* GATT read, write requests to configure Heart Rate services. Application notified with
* simple events to start or stop Heart rate notification based on client configuration.
* Formats Heart rate notification as per spec and sends to client when application sends
* Heart rate data data. see @ref wiced_bt_heart_rate_data_t
*
*/

/**
* \brief List of Handles of the Heart Rate Service
*/
typedef struct
{
    uint16_t value;                         /**< Measurement Value handle */
    uint16_t configuration;                 /**< Measurement Configuration Value handle */
    uint16_t control;                       /**< Reset Energy Expended Control Value handle */
    uint16_t location;                      /**< Body Sensor Location Value handle */
} wiced_bt_hrs_handles_t;

/**
* \Brief HRC Events received by the applicaton's HRS callback (see \ref wiced_bt_hrs_callback_t)
*
*/
typedef enum
{
    WICED_BT_HRS_EVENT_HEART_RATE_NOTIFICATIONS_ENABLED,   /**< Client Registered for for Heart rate notifications */
    WICED_BT_HRS_EVENT_HEART_RATE_NOTIFICATIONS_DISABLED,  /**< Client Un-registered Heart rate notifications */
    WICED_BT_HRS_RESET_ENERGY_EXPENDED_VALUE,              /**< Reset Energy expended during heart rate measurement since last reset */
} wiced_bt_hrs_event_t;

/**
* \brief Data associated with WICED_BT_HRS_EVENT_HEART_RATE_NOTIFICATIONS_ENABLED
 */
typedef struct
{
    uint16_t    conn_id;
} wiced_bt_hrs_notification_enabled_t;

/**
* \brief Data associated with WICED_BT_HRS_EVENT_HEART_RATE_NOTIFICATIONS_DISABLED
*/
typedef struct
{
    uint16_t    conn_id;
} wiced_bt_hrs_notification_disabled_t;

/**
* \brief Data associated with WICED_BT_HRS_RESET_ENERGY_EXPENDED_VALUE
*/
typedef struct
{
    uint16_t    conn_id;
} wiced_bt_hrs_reset_energy_expended_t;

/**
* \brief Heart Rate Notification data.
* HRS Notification data sent to the Heart Rate Client device.
*/
typedef struct
{
    uint16_t    heart_rate;                 /**< Value in bpm, beats per minutes. */
    uint8_t     energy_expended_present;    /**< 1 = present, 0= not present */
    uint16_t    energy_expended;            /**< Value in kilo Jouls Range is 0x0 to 0xffff*/

#ifdef SUPPORT_RR_INTERVALS
    uint8_t     num_of_rr_intervals; /* Number of RR intervals present */
    uint16_t    *rr_intervals_data; /* pointer to RR intervals. Each RR interval is in terms of seconds */
#endif
} wiced_bt_hrs_notification_data_t;


/**
* \brief Union of data associated with HRS events. The HRS library calls the application's
* callback registered with a pointer on such structure.
*/
typedef union
{
    wiced_bt_hrs_notification_enabled_t  notification_enabled;  /**< Notifications Enabled data */
    wiced_bt_hrs_notification_disabled_t notification_disabled; /**< Notifications Disabled data */
    wiced_bt_hrs_reset_energy_expended_t reset_energy_expended; /**< Reset Energy Expended data */
} wiced_bt_hrs_event_data_t;

/**
* HRS Callback function wiced_bt_hrs_event_cback_t
*
*                  This callback is executed when HRS library completes HRC
*                  requested configuration (Client requests to start/stop/reset
*                  energy expended in heart rate using GATT write operation).
*
* \param           conn_id : GATT connection ID.
* \param           event   : Event to application to act accordingly.
*
* \return          None.
*/
typedef void (wiced_bt_hrs_event_cback_t)(wiced_bt_hrs_event_t event, wiced_bt_hrs_event_data_t *p_data);

/*****************************************************************************
 *          Function Prototypes
 *****************************************************************************/

/*****************************************************************************
*
* Function Name: wiced_bt_hrs_init
*
***************************************************************************//**
*
* The application calls this function on start up to initialize HRS library.
* Through callback application notified with different events (see wiced_bt_hrs_event_cback_t)
* based on heart rate client actions.
*
* \param           p_callback : Application callback pointer to receive events from library.
*
* \return          None.
*/
wiced_result_t wiced_bt_hrs_init(wiced_bt_hrs_event_cback_t *p_callback, wiced_bt_hrs_handles_t *p_gatt_handles);

/*****************************************************************************
*
* Function Name: wiced_bt_hrs_connection_up
*
***************************************************************************//**
*
* The application calls this function when BLE connection with a peer device
* has been established.
*
* \param           conn_id  : GATT connection ID.
*
* \return          None.
*/
void wiced_bt_hrs_connection_up(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_hrs_connection_down
*
***************************************************************************//**
*
* The application calls this function when BLE connection with a peer device
* has been disconnected.
*
* \param           conn_id  : GATT connection ID.
*
* \return          None.
*/
 void wiced_bt_hrs_connection_down(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_hrs_process_client_read_req
*
***************************************************************************//**
*
* The application calls this function when it receives GATT read request on
* Heart rate service characteristic and descriptors.
*
* \param           conn_id  : GATT connection ID.
* \param           p_read   :  GATT read request.
*
* \return          Status of GATT Read operation
*/
wiced_bt_gatt_status_t wiced_bt_hrs_process_client_read_req(uint16_t conn_id, wiced_bt_gatt_read_t *p_read);

/*****************************************************************************
*
* Function Name: wiced_bt_hrs_process_client_write_req
*
***************************************************************************//**
*
* The application calls this function when it receives GATT write request on
* Heart rate service characteristic and descriptors.
*
* \param           conn_id  : GATT connection ID.
* \param           p_write  : GATT write request.
*
* \return          Status of GATT Write operation (Note: WICED_BT_HRP_CONTROL_POINT_WRITE_UNSUPPORTED_VALUE
                       value returns when client provides invalid value in reset energy expended request.)
*/
wiced_bt_gatt_status_t wiced_bt_hrs_process_client_write_req(uint16_t conn_id, wiced_bt_gatt_write_t *p_write);

/*****************************************************************************
*
* Function Name: wiced_bt_hrs_send_heart_rate
*
***************************************************************************//**
*
* The application calls this function to send heart rate notification.
*
* \param           conn_id         : GATT connection ID.
* \param           heart_rate_data : Heart rate data.See @ref wiced_bt_heart_rate_data_t
*
* \return          Status of GATT notification operation
*/
wiced_bt_gatt_status_t wiced_bt_hrs_send_heart_rate(uint16_t conn_id, wiced_bt_hrs_notification_data_t *p_heart_rate_data);

/*****************************************************************************
*
* Function Name: wiced_bt_hrs_set_previous_connection_client_notification_configuration
*
***************************************************************************//**
*
* The application should call this API on successful encryption with Heart Rate client.
* Library is required to know whether the client enabled notifications.
*
* \param           conn_id               : GATT connection ID.
* \param           notifications_enabled : Notifications status. WICED_TRUE:Enabled, WICED_FALSE:disabled
*
* \return          None.
*/
void wiced_bt_hrs_set_previous_connection_client_notification_configuration(uint16_t conn_id, wiced_bool_t notifications_enabled);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
