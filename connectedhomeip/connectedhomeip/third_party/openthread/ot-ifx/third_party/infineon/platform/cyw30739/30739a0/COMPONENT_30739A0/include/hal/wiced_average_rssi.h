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
 * Average RSSI library
 *
 * Use this library to enable the average rssi events for ACL connections.
 * Add average_rss_lib.a in makefile to enable the feature as follows:
 * APP_PATCHES_AND_LIBS += average_rssi_lib.a
 *
 */

#include "wiced_bt_dev.h"

#define AVERAGE_RSSI_MAX_CONNECTIONS    3
#define BTHCI_EVENT_VS_AVERAGE_RSSI_EVENT_CODE      0x88

/* Structure of Average RSSI event */
typedef struct __attribute__((packed)) wiced_average_rssi_event
{
    uint8_t                             evt_code;   // The event code will be BTHCI_EVENT_VS_AVERAGE_RSSI_EVENT_CODE
    uint8_t                             status;     // Reserved for future. Always return 0 in current implementation
    uint8_t                             conn_num;   // The connection number for rssi, avg_rssi and conn_handle
    int8_t                              rssi[AVERAGE_RSSI_MAX_CONNECTIONS];     // The latest RSSI during the average interval
    int8_t                              avg_rssi[AVERAGE_RSSI_MAX_CONNECTIONS]; // The average RSSI during the average interval
    uint16_t                            conn_handle[AVERAGE_RSSI_MAX_CONNECTIONS]; // The connection handles, the same as in
                                                                                   // wiced_average_rssi_enable()
} wiced_average_rssi_event_t;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Function         wiced_average_rssi_enable
 *
 *                  Enable the average rssi event
 * @param[in]       enable              : 1: enable the event  0: disable the event (the same as wiced_average_rssi_disable)
 * @param[in]       coefficient         : The paramter will be divided by 1024 as the coefficient in the internal 1st order IIR filter
 *                                        y[n] = ax[n] + (1-a)y[n-1]
 * @param[in]       interval            : The timer interval in 100 ms between consecutive average RSSI events
 * @param[in]       elna_gain           : The RSSI will be reduced by elna_gain before filtering
 * @param[in]       conn_handle_num     : The connection number of conn_handle
 * @param[in]       conn_handle         : The array of connection handle
 *
 * @return
 *
 *                  WICED_BT_SUCCESS     : Success
 *                  WICED_BT_NO_RESOURCE : No resource to issue command
 */
wiced_result_t wiced_average_rssi_enable(uint8_t enable, uint16_t coefficient, uint16_t interval,
    uint8_t elna_gain, uint8_t conn_handle_num, uint16_t conn_handle[]);

/**
 * Function         wiced_average_rssi_disable
 *
 *                  Disable the average rssi event
 *
 * @return
 *
                   WICED_BT_SUCCESS     : Success
 *                 WICED_BT_NO_RESOURCE : No resource to issue command
 */
wiced_result_t wiced_average_rssi_disable(void);


#ifdef __cplusplus
} /* extern "C" */
#endif
