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
 *  AIROC Generic Attribute (GATT) Application Programming Interface
 */
#pragma once


/*****************************************************************************
 *  External Function Declarations
 ****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup  wicedbt_gatt_utils   GATT Utilities
 * @ingroup     wicedbt_gatt
 *
 * Generic Attribute (GATT) Utility Functions.
 *
 * @{
 */

/**
 * Function     wiced_bt_util_set_gatt_client_config_descriptor
 *
 *              Set value of Client Configuration Descriptor
 *
 * @param[in]   conn_id   : GATT connection ID
 * @param[in]   handle    : Handle of the descriptor to modify
 * @param[in]   value     : Value to set
 *
 * @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *
 */
wiced_bt_gatt_status_t wiced_bt_util_set_gatt_client_config_descriptor(uint16_t conn_id, uint16_t handle, uint16_t value);

/**
 *
 * Function     wiced_bt_util_send_gatt_discover
 *
 *              Format and send GATT discover request
 *
 *  @param[in]  conn_id     : connection identifier.
 *  @param[in]  type        : GATT discovery type.
 *  @param[in]  uuid        : UUID of the attribute to search for.
 *  @param[in]  s_handle    : Start handle.
 *  @param[in]  e_handle    : Start handle.
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *
 */
wiced_bt_gatt_status_t wiced_bt_util_send_gatt_discover(uint16_t conn_id, wiced_bt_gatt_discovery_type_t type, uint16_t uuid, uint16_t s_handle, uint16_t e_handle);

/**
 * Function     wiced_bt_gatt_status_t wiced_bt_util_send_gatt_read_by_handle(uint16_t conn_id, uint16_t handle)
 *
 *              Format and send Read By Handle GATT request.
 *
 *  @param[in]  conn_id     : connection identifier.
 *  @param[in]  handle      : Attribute handle of the attribute to read.
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *
 */
wiced_bt_gatt_status_t wiced_bt_util_send_gatt_read_by_handle(uint16_t conn_id, uint16_t handle);

/**
 * Function       wiced_bt_util_send_gatt_read_by_type
 *
 *                Format and send Read by Type GATT request
 *
 *  @param[in]  conn_id     : Connection handle
 *  @param[in]  s_handle    : Start handle
 *  @param[in]  e_handle    : End handle
 *  @param[in]  uuid        : UUID of the attribute to read
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 */
wiced_bt_gatt_status_t wiced_bt_util_send_gatt_read_by_type(uint16_t conn_id, uint16_t s_handle, uint16_t e_handle, uint16_t uuid);

/**
 * Function       wiced_bt_util_uuid_cpy
 *
 *                This utility function copies an UUID
 *
 *  @param[out] p_dst       : Destination UUID
 *  @param[in]  p_src       : Source UUID
 *
 *  @return  int 0 if success, -1 if error
 */
int wiced_bt_util_uuid_cpy(wiced_bt_uuid_t *p_dst, wiced_bt_uuid_t *p_src);

/*
 * wiced_bt_util_uuid_cmp
 * This utility function Compares two UUIDs.
 * Note: This function can only compare UUIDs of same length
 * Return value: 0 if UUID are equal; 1 other with
 */
/**
 * Function       wiced_bt_util_uuid_cmp
 *
 *                This utility function Compares two UUIDs.
 *                Note: This function can only compare UUIDs of same length
 *
 *  @param[out] p_uuid1     : First UUID
 *  @param[in]  p_uuid2     : Second UUID
 *
 *  @return  int 0 if UUID are equal; -1 if error, 1 if UUIDs are different
 */
int wiced_bt_util_uuid_cmp(wiced_bt_uuid_t *p_uuid1, wiced_bt_uuid_t *p_uuid2);

/**@} wicedbt_gatt_utils */

#ifdef __cplusplus
}
#endif
