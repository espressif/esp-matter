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
* This file provides definitions of the Heart rate profile Client(HRC) library interface.
*
******************************************************************************/

#ifndef WICED_BT_HRC_H
#define WICED_BT_HRC_H
#include "wiced_bt_hrp.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \addtogroup  wiced_bt_hrc_api_functions        HRC Library API
* \ingroup     wicedbt
* @{
* HRC library of the AIROC BTSDK provides a simple method for an application to integrate Heart Rate
* Client functionality. Application Calls the Library APIs to:
*    Discover Heart Rate Service characteristics and descriptors whenever it finds Heart Rate Service in the connected device.
*    Start or Stop Heart Rate Notifications.
*    To Reset Energy expended during Heart Rate notification based on application request
* Application registers callback on application start to receive the result of above 3 operations.
*
*/

/**
* \Brief HRC Events received by the applicaton's HRC callback (see \ref wiced_bt_hrc_callback_t)
*
*/
typedef enum
{
    WICED_BT_HRC_EVENT_DISCOVERY,               /**< HRC Discovery event */
    WICED_BT_HRC_EVENT_START,                   /**< HRC Start event */
    WICED_BT_HRC_EVENT_STOP,                    /**< HRC Stop event */
    WICED_BT_HRC_EVENT_RESET_ENERGY_EXPENDED,   /**< HRC Reset Energy Expended event */
    WICED_BT_HRC_EVENT_NOTIFICATION_DATA        /**< HRC Notification event */
} wiced_bt_hrc_event_t;

/**
* \brief Data associated with WICED_BT_HRC_EVENT_DISCOVERY
*
*/
typedef struct
{
    uint16_t               conn_id;
    wiced_bt_gatt_status_t status;
} wiced_bt_hrc_discovery_t;

/**
* \brief Data associated with WICED_BT_HRC_EVENT_START
*
*/
typedef struct
{
    uint16_t               conn_id;
    wiced_bt_gatt_status_t status;
} wiced_bt_hrc_start_t;

/**
* \brief Data associated with WICED_BT_HRC_EVENT_STOP
*
*/
typedef struct
{
    uint16_t               conn_id;
    wiced_bt_gatt_status_t status;
} wiced_bt_hrc_stop_t;

/**
* \brief Data associated with WICED_BT_HRC_EVENT_RESET_ENERGY_EXPENDED
*
*/
typedef struct
{
    uint16_t               conn_id;
    wiced_bt_gatt_status_t status;
} wiced_bt_hrc_reset_energy_expended_t;

/**
* \brief Data associated with WICED_BT_HRC_EVENT_NOTIFICATION_DATA
*
*/
typedef struct
{
    uint16_t                conn_id;
    uint16_t                heart_rate;                 /**< Value in bpm, beats per minutes. */
    uint8_t                 energy_expended_present;    /**< 1 = present, 0= not present */
    uint16_t                energy_expended;            /**< Value in kilo Jouls Range is 0x0 to 0xffff*/

#ifdef SUPPORT_RR_INTERVALS
    uint8_t                 num_of_rr_intervals;        /* Number of RR intervals present */
    uint16_t                *rr_intervals_data;         /* pointer to RR intervals. Each RR interval is in terms of seconds */
#endif
} wiced_bt_hrc_notification_data_t;

/**
* \brief Union of data associated with HRC events. The HRC library calls the application's
* callback registered with a pointer on such structure.
*
*/
typedef union
{
    wiced_bt_hrc_discovery_t                discovery;
    wiced_bt_hrc_start_t                    start;
    wiced_bt_hrc_stop_t                     stop;
    wiced_bt_hrc_reset_energy_expended_t    reset_energy_expended;
    wiced_bt_hrc_notification_data_t        notification_data;
} wiced_bt_hrc_event_data_t;

/**
* HRC Callback function type wiced_bt_hrc_callback_t
*
*                  This function is called to send HRC events to the application.
*                  This function is registered with the wiced_bt_hrc_init function.
*
* \param[in]       event  : HRC Event.
* \param[in]       p_data : Data (pointer on union of structure) associated with the event.
*
* \return NONE.
*/
typedef void (wiced_bt_hcr_callback_t)(wiced_bt_hrc_event_t event, wiced_bt_hrc_event_data_t *p_data);

/*****************************************************************************
 *          Function Prototypes
 *****************************************************************************/

/*****************************************************************************
*
* Function Name: wiced_bt_hrc_init
*
***************************************************************************//**
*
* The application calls this function to initialize the HRC library and to
* register application callback.
*
* \param           p_callback  : application's HRC callback.
*
* \return          Status of the operation.
*/
wiced_result_t wiced_bt_hrc_init (wiced_bt_hcr_callback_t *p_callback);

/*****************************************************************************
*
* Function Name: wiced_bt_hrc_discover
*
***************************************************************************//**
*
* The application calls this function when it discovers the HRS service of the
* connected device.
* This function starts the GATT discovery of heart rate service characteristics.
*
* \param           conn_id  : GATT connection ID.
* \param           s_handle : Start GATT handle of the heart rate service.
* \param           e_handle : End GATT handle of the heart rate service.
*
* \return          Status of the GATT operation.
*/
wiced_bt_gatt_status_t wiced_bt_hrc_discover(uint16_t conn_id, uint16_t s_handle, uint16_t e_handle);

/*****************************************************************************
*
* Function Name: wiced_bt_hrc_discovery_result
*
***************************************************************************//**
*
* The application calls this API during GATT discovery to pass discovery results
* for the HRC service to the HRC Library.
* The library needs to find HRC service characteristics and associated characteristic
* client configuration descriptors.
*
* \param           p_data   : Discovery result data as passed from the stack.
*
* \return          none
*/
void wiced_bt_hrc_discovery_result(wiced_bt_gatt_discovery_result_t *p_data);

/*****************************************************************************
*
* Function Name: wiced_bt_hrc_discovery_result
*
***************************************************************************//**
*
* The application calls this API during GATT discovery to pass discovery complete
* information for the HRC service to the HRC Library.
* As the GATT discovery is performed in multiple steps this function initiates the
* next discovery request.
*
* \param           p_data   : Discovery complete data as passed from the stack.
*
* \return          None.
*/
void wiced_bt_hrc_discovery_complete(wiced_bt_gatt_discovery_complete_t *p_data);

/*****************************************************************************
*
* Function Name: wiced_bt_hrc_connection_up
*
***************************************************************************//**
*
* The application calls this function when BLE connection with a peer
* device has been established.
*
* \param           conn_id  : Connection Identifier.
*
* \return          None.
*/
void wiced_bt_hrc_connection_up(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_hrc_connection_down
*
***************************************************************************//**
*
* The application calls this function when BLE connection with a peer
* device has been released.
*
* \param           conn_id  : Connection Identifier.
*
* \return          None.
*/
void wiced_bt_hrc_connection_down(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_hrc_start
*
***************************************************************************//**
*
* The application calls this function whenever it is needed to collect Heart Rate.
* Discovery should be completed before this function is executed.
* The start function enables the server to send heart Rate notifications.
*
* \param           conn_id  : GATT connection ID.
*
* \return          Result of GATT operation.
*/
wiced_bt_gatt_status_t  wiced_bt_hrc_start(uint16_t conn_id);

/**
 * The application calls this function whenever it need to stop to collect Heart Rate.
 *
 * The stop function disables the server to send heart Rate notifications

 * \param           conn_id  : GATT connection ID.
 *
 * \return          Result of GATT operation.
 */

/*****************************************************************************
*
* Function Name: wiced_bt_hrc_stop
*
***************************************************************************//**
*
* The application calls this function whenever it need to stop to collect Heart Rate.
*
* \param           conn_id  : GATT connection ID.
*
* \return          Result of GATT operation.
*/
wiced_bt_gatt_status_t  wiced_bt_hrc_stop(uint16_t conn_id);


/*****************************************************************************
*
* Function Name: wiced_bt_hrc_reset_energy_expended
*
***************************************************************************//**
*
* The application calls this function to reset accumulated Energy expended value during
* heart rate measure process. Client can reset when it finds max energy expended value (i.e 0xffff)
* in heart rate notification.
*
* \param           conn_id  : GATT connection ID.
*
* \return          Result of GATT operation.
*/
wiced_bt_gatt_status_t wiced_bt_hrc_reset_energy_expended(uint16_t conn_id);

/*****************************************************************************
*
* Function Name: wiced_bt_hrc_gatt_op_complete
*
***************************************************************************//**
*
* The application calls this function when it receive GATT operation complete event on
* heart rate service characteristics.
* The Library parse the result and calls application registered callback.
*
* \param           conn_id  : GATT connection ID.
*
* \return          Result of GATT operation.
*/
void wiced_bt_hrc_gatt_op_complete(wiced_bt_gatt_operation_complete_t *p_data);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
