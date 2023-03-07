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
* \brief Provides definitions of the Alert Notification Server (ANS) library interface.
*
******************************************************************************/

#ifndef WICED_BT_ANS_H
#define WICED_BT_ANS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
*
* \addtogroup  wiced_bt_ans_api_functions        ANS Library API
* \ingroup     wicedbt
* @{
*
* The ANS library of the AIROC BTSDK provide a simple method for an application to integrate the ANS
* service functionality. The application calls the library APIs to configure supported alerts,
* to process alert notification client GATT read, write requests for enable/disable/control the alerts.
*
*/
#include "wiced_bt_anp.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_types.h"

/**
* \brief List of Handles of an Alert
*/
typedef struct
{
    uint16_t supported_category;                        /**< Alert Supported Category Value handle */
    uint16_t value;                                     /**< Alert Value handle */
    uint16_t configuration;                             /**< Alert Configuration Value handle */
} wiced_bt_ans_gatt_alert_handles_t;

/**
* \brief List of Handles of the Alert Service
*/
typedef struct
{
    wiced_bt_ans_gatt_alert_handles_t new_alert;        /**< New Alert handles */
    wiced_bt_ans_gatt_alert_handles_t unread_alert;     /**< Unread Alert handles */
    uint16_t notification_control;                      /**< Alert Notification Control handle */
} wiced_bt_ans_gatt_handles_t;

/******************************************************************************
*          Function Prototypes
******************************************************************************/

/******************************************************************************
*
* Function Name: wiced_bt_ans_init
*
***************************************************************************//**
*
* The application calls this API on an application start to initialize the AIROC BTSDK ANS server library.
* The ANS GATT Handles are defined in the application. These handles must be passed to
* the ANS library t initialization time.
*
* \param           p_gatt_handles : Pointer on a structure containing the Service Handles
*
* \return          None.
*
******************************************************************************/
wiced_result_t wiced_bt_ans_init(wiced_bt_ans_gatt_handles_t *p_gatt_handles);

/******************************************************************************
*
* Function Name: wiced_bt_ans_connection_up
*
***************************************************************************//**
*
* The application calls this API when the application is connected with the alert notification client.
*
* \param           conn_id : GATT connection ID
*
* \return          None.
*
******************************************************************************/
void wiced_bt_ans_connection_up(uint16_t conn_id);

/******************************************************************************
*
* Function Name: wiced_bt_ans_connection_down
*
***************************************************************************//**
*
* The application calls this API when the application is disconnected from the client.
*
* \param           conn_id : GATT connection ID
*
* \return          None.
*
******************************************************************************/
void wiced_bt_ans_connection_down(uint16_t conn_id);

/******************************************************************************
*
* Function Name: wiced_bt_ans_set_supported_new_alert_categories
*
***************************************************************************//**
*
* The application calls this API to set Server supported new alert categories. This API should be
* called before the application connects to the alert notification client. Supported new alerts static
* during the connection.
*
* \param           conn_id : GATT connection ID
* \param           supported_new_alert_cat  : Server-supported new alert categories.
*                                             Each category is represented by a bit.
*                                             The bit is positioned by the category ID value.
*                                             \ref ANP_ALERT_CATEGORY_ENABLE."Alert category enable bit mask".
*
* \return          None.
*
******************************************************************************/
void wiced_bt_ans_set_supported_new_alert_categories(uint16_t conn_id, wiced_bt_anp_alert_category_enable_t supported_new_alert_cat);

/******************************************************************************
*
* Function Name: wiced_bt_ans_set_supported_unread_alert_categories
*
***************************************************************************//**
*
* The application calls this API to set Server supported unread alert categories. This API should be
* called before the application connects to the alert notification client. Supported unread alerts static
* during the connection.
*
* \param           conn_id                     : GATT connection ID
* \param           supported_unread_alert_cat  : Server-supported unread alert categories.
*                                                Each category is represented by a bit.
*                                                The bit is positioned by the category ID value.
*                                                \ref ANP_ALERT_CATEGORY_ENABLE."Alert category enable bit mask".
*
* \return          None.
*
******************************************************************************/
void wiced_bt_ans_set_supported_unread_alert_categories(uint16_t conn_id, wiced_bt_anp_alert_category_enable_t supported_unread_alert_cat);

/******************************************************************************
*
* Function Name: wiced_bt_ans_process_gatt_read_req
*
***************************************************************************//**
*
* The application calls this API to process alert notification client GATT read requests.
* The alert notification client uses the GATT read procedure to know server-supported new alerts,
* unread alerts and to check GATT notification configuration enabled/disabled for new alerts and/or unread alerts.
*
* \param           conn_id : GATT connection ID
* \param           p_read  : GATT read request.
*
* \return          Status of the GATT read.operation
*
******************************************************************************/
wiced_bt_gatt_status_t wiced_bt_ans_process_gatt_read_req(uint16_t conn_id, wiced_bt_gatt_read_t *p_read);

/******************************************************************************
*
* Function Name: wiced_bt_ans_process_gatt_write_req
*
***************************************************************************//**
*
* The application calls this API to process alert notification client GATT write requests.
* The alert notification client uses the GATT write procedure to configure, enable/disable and to control the alerts.
*
* \param           conn_id  : GATT connection ID
* \param           p_write  : GATT write request.
*
* \return          Status of the GATT write.operation.
*
******************************************************************************/
wiced_bt_gatt_status_t wiced_bt_ans_process_gatt_write_req(uint16_t conn_id, wiced_bt_gatt_write_t *p_write);

/******************************************************************************
*
* Function Name: wiced_bt_ans_process_and_send_new_alert
*
***************************************************************************//**
*
* The application calls this API to process and send a new alert.on given alert category ID.
* The library increments the new alert count of the specified category.
* The library sends a new alert to 5the alert notification client if the client already configured to receive
* notification of the given category, otherwise the server send a new alert wheneven theclient enables.
* The count will be cleared when the alert is sent to the client or when theapplication asks to clear the alerts
*
* \param           conn_id : GATT connection ID
* \param           category_id  : New Alert category ID. \ref ANP_ALERT_CATEGORY_ID."Alert category ID".
*
* \return          Status of the GATT notification.
*
******************************************************************************/
wiced_bt_gatt_status_t wiced_bt_ans_process_and_send_new_alert(uint16_t conn_id, wiced_bt_anp_alert_category_id_t category_id);

/******************************************************************************
*
* Function Name: wiced_bt_ans_process_and_send_unread_alert
*
***************************************************************************//**
*
* The application calls this API to process and send the unread alert.on given alert category ID.
* The library increments the unread alert count of the specified category.
* The library sends an unread alert to thealert notification client if client already configured to receive
* notification of the given category, otherwise server send the unread alert wheneven theclient enables.
* The count will be cleared when the alert is sent to theclient or when theapplication asks to clear the alerts
*
* \param           conn_id      : GATT connection ID
* \param           category_id  : Unread Alert category ID. \ref ANP_ALERT_CATEGORY_ID. "Alert category ID".
*
* \return          Status of the GATT notification.
*
******************************************************************************/
wiced_bt_gatt_status_t wiced_bt_ans_process_and_send_unread_alert(uint16_t conn_id, wiced_bt_anp_alert_category_id_t category_id);

/******************************************************************************
*
* Function Name: wiced_bt_ans_clear_alerts
*
***************************************************************************//**
*
* The application calls this API to clear the new alert and unread alert count.of the specified category.
* The library clears the new alert count and unread alert count of a given category.
*
* \param           conn_id      : GATT connection ID
* \param           category_id  : Unread Alert category ID. see @ref ANP_ALERT_CATEGORY_ID. "Alert category ID".
*
* \return          WICED_TRUE   : On success.
*                  WICED_FALSE  : On the invalid category ID.
*
******************************************************************************/
wiced_bool_t wiced_bt_ans_clear_alerts(uint16_t conn_id, wiced_bt_anp_alert_category_id_t category_id);

#ifdef __cplusplus
}
#endif

/** @} wiced_bt_ans_api_functions */

#endif /* WICED_BT_ANS_H */
