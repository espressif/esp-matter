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
 * Bluetooth Synchronous Connection Oriented Channel Application Programming Interface
 *
 */
#pragma once

#include "wiced.h"
/**
 *  @addtogroup  sco_api_functions       Synchronous Connection Oriented (SCO) Channel
 *  @ingroup     wicedbt_av
 *
 *  SCO Definitions and Functions
 *
 *  SCO logical transport, is a symmetric, point-to-point transport between
 *  the central and a specific peripheral. The SCO logical transport reserves slots
 *  and can therefore be considered as a circuit-switched connection between
 *  the central and the peripheral. The central may support up to three SCO links to
 *  the same peripheral or to different peripherals. A peripheral may support up to three SCO
 *  links from the same central, or two SCO links if the links originate from
 *  different centrals. SCO packets are never retransmitted.
 *
 *  @{
 */

/******************************************************
 *              Constants
 ******************************************************/
#define WICED_BT_SCO_CONNECTION_ACCEPT              0x00
#define WICED_BT_SCO_CONNECTION_REJECT_RESOURCES    0x0D
#define WICED_BT_SCO_CONNECTION_REJECT_SECURITY     0x0E
#define WICED_BT_SCO_CONNECTION_REJECT_DEVICE       0x0F

#ifndef WICED_SCO_PKT_TYPES_MASK
#define WICED_INVALID_SCO_INDEX           0xFFFF
#define WICED_SCO_LINK_ALL_PKT_MASK       0x003F
#define WICED_SCO_LINK_ONLY_MASK          0x0007
#define WICED_SCO_PKT_TYPES_MASK_HV3      0x0004
#define WICED_SCO_PKT_TYPES_MASK_EV3      0x0008
#define WICED_SCO_PKT_TYPES_MASK_EV4      0x0010
#define WICED_SCO_PKT_TYPES_MASK_EV5      0x0020
#define WICED_SCO_PKT_TYPES_MASK_NO_2_EV3 0x0040
#define WICED_SCO_PKT_TYPES_MASK_NO_3_EV3 0x0080
#define WICED_SCO_PKT_TYPES_MASK_NO_2_EV5 0x0100
#define WICED_SCO_PKT_TYPES_MASK_NO_3_EV5 0x0200
#endif

/** SCO route path */
typedef enum
{
    WICED_BT_SCO_OVER_PCM = 0,   /**< [DEFAULT] PCM data config for routing over I2S/PCM interface */
}wiced_bt_sco_route_path_t;

/******************************************************
 *              Type Definitions
 ******************************************************/
#define  WICED_BT_SCO_DATA_CB_GET_LENGTH(ltch_len)   ((ltch_len>>8)&0xff)	/**< SCO data callback length */

/** Subset for the enhanced setup/accept synchronous connection paramters. See BT 4.1 or later HCI spec for details */
typedef struct
{
    uint16_t max_latency;                   /**< Maximum latency (0x4-0xFFFE in msecs) */
    uint16_t packet_types;                  /**< Packet Types */
    uint8_t retrans_effort;                 /**< 0x00-0x02, 0xFF don't care */
    wiced_bool_t use_wbs;                   /**< True to use wide band, False to use narrow band */

} wiced_bt_sco_params_t;

/** Call back function for pcm data transfer, ltch_len = (length)<<8|(sco_channel) */
typedef void (wiced_bt_sco_data_cb_t) (uint32_t ltch_len, uint8_t *p_data);

/** SCO path config */
typedef struct
{
    wiced_bt_sco_route_path_t    path;           /**< sco routing path  0:pcm/i2s; 1: app*/
    wiced_bt_sco_data_cb_t       *p_sco_data_cb; /**< If not NULL and route is APP_CB, callback function called for incoming pcm data */
}wiced_bt_voice_path_setup_t;

/******************************************************
 *              Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Function         wiced_bt_sco_create_as_initiator
 *
 * Creates a synchronous connection oriented connection as initiator.
 *
 *  @param[in]  bd_addr                 : Peer bd_addr
 *  @param[in]  p_params                : Pointer to the SCO parameter structure
 *  @param[out] p_sco_index             : SCO index returned
 *
 *  @return     <b> WICED_BT_UNKNOWN_ADDR </b>      : Create connection failed, ACL connection is not up
 *              <b> WICED_BT_BUSY </b>              : Create connection failed, another SCO is being
 *                                                    conncted to the same BD address
 *              <b> WICED_BT_WRONG_MODE </b>        : Create connection failed, wrong mode
 *              <b> WICED_BT_NO_RESOURCES </b>      : Create connection failed, max SCO limit has been
 *                                                    reached
 *              <b> WICED_BT_PENDING </b>            : Create connection successfully, "p_sco_index" is returned
 */
wiced_bt_dev_status_t wiced_bt_sco_create_as_initiator (wiced_bt_device_address_t bd_addr,
                                                        uint16_t *p_sco_index,
                                                        wiced_bt_sco_params_t *p_params);

/**
 * Function         wiced_bt_sco_create_as_acceptor
 *
 *                  Creates a synchronous connection oriented connection as acceptor.
 *
 *  @param[out] p_sco_index             : SCO index returned
 *
 *  @return     <b> WICED_BT_UNKNOWN_ADDR </b>      : Create connection failed, ACL connection is not up or
 *                                                    address is invalid
 *              <b> WICED_BT_BUSY </b>              : Create connection failed, a SCO connection is already
 *                                                    conncted to the same BD address
 *              <b> WICED_BT_WRONG_MODE </b>        : Create connection failed, link in park mode or
 *                                                    automatic un-park is not supported
 *              <b> WICED_BT_NO_RESOURCES </b>      : Create connection failed, max SCO limit has been
 *                                                    reached
 *              <b> WICED_BT_PENDING </b>            : Create connection successfully, "p_sco_index" is returned
 */
wiced_bt_dev_status_t wiced_bt_sco_create_as_acceptor (uint16_t *p_sco_index);

/**
 * Function         wiced_bt_sco_create_as_acceptor
 *
 *                  Creates a synchronous connection oriented connection as acceptor with specifi
 *                  Audio Gateway.
 *  @param[in]  ag_addr                 : Target Audio Gateway's address
 *  @param[out] p_sco_index             : SCO index returned
 *
 *  @return     <b> WICED_BT_UNKNOWN_ADDR </b>      : Create connection failed, ACL connection is not up or
 *                                                    address is invalid
 *              <b> WICED_BT_BUSY </b>              : Create connection failed, a SCO connection is already
 *                                                    conncted to the same BD address
 *              <b> WICED_BT_WRONG_MODE </b>        : Create connection failed, link in park mode or
 *                                                    automatic un-park is not supported
 *              <b> WICED_BT_NO_RESOURCES </b>      : Create connection failed, max SCO limit has been
 *                                                    reached
 *              <b> BTM_CMD_STARTED </b>            : Create connection successfully, "p_sco_index" is returned
 */
wiced_bt_dev_status_t wiced_bt_sco_create_as_acceptor_with_specific_ag(wiced_bt_device_address_t ag_addr,
                                                                       uint16_t *p_sco_index);

/**
 * Function         wiced_bt_sco_remove
 *
 *                  Removes a specific synchronous connection oriented connection.
 *
 *  @param[in]  sco_index           : SCO index to remove
 *
 *  @return     <b> WICED_BT_UNKNOWN_ADDR </b>      : Remove connection failed, invalid SCO index
 *              <b> WICED_BT_NO_RESOURCES </b>      : Remove connection failed, no resource
 *              <b> WICED_BT_SUCCESS </b>           : Remove connection successfully, device is still
 *                                                    listening for incoming connection
 *              <b> WICED_BT_PENDING </b>            : Remove connection successfully
 */
wiced_bt_dev_status_t wiced_bt_sco_remove (uint16_t sco_index);

/**
 * Function         wiced_bt_sco_accept_connection
 *
 * Called to handle (e)SCO connection request event (wiced_bt_sco_connect_request_event).
 *
 *
 *  @param[in]  sco_index           : SCO index
 *
 *  @param[in]  HCI status code     : WICED_BT_SCO_CONNECTION_ACCEPT              0x00
 *                                    WICED_BT_SCO_CONNECTION_REJECT_RESOURCES    0x0D
 *                                    WICED_BT_SCO_CONNECTION_REJECT_SECURITY     0x0E
 *                                    WICED_BT_SCO_CONNECTION_REJECT_DEVICE       0x0F
 *  @param[in]  p_params            : Pointer to the SCO parameter structure
 *
 */
void wiced_bt_sco_accept_connection (uint16_t sco_index, uint8_t hci_status,
                                     wiced_bt_sco_params_t *p_params);


/**
 * Function         wiced_bt_sco_setup_voice_path
 *
 * Configure the SCO routing path.
 *
 *  @param[in]  pData                 : To setup SCO routing path
 *
 * @return
 *              WICED_BT_SUCCESS    : Config success.
 *              WICED_BT_PENDING    : Command sent. Waiting for command complete event.
 *              WICED_BT_BUSY       : Command not sent. Waiting for command complete event for prior command.
 */
wiced_bt_dev_status_t wiced_bt_sco_setup_voice_path(wiced_bt_voice_path_setup_t *pData);



/**
 * Function         wiced_bt_sco_output_stream
 *
 * Send pcm data to internal audio pipe.
 *
 *
 *  @param[in]  sco_index     : SCO index to send the stream
 *  @param[in]  p_pcmsrc        : Audio stream source.
 *  @param[in]  len             : Length of stream.
 *
 *  @return                     : Return the legth of non transmited stream.
 */
uint16_t wiced_bt_sco_output_stream( uint16_t sco_index, uint8_t* p_pcmsrc,uint16_t len );

/**
 * Function         wiced_bt_sco_turn_off_pcm_clock
 *
 *                  To turn off the PCM/I2S hardware clock
 *                  This function needs to be called after the application turns off (or mutes audio data in/out of) the codec
 *
 *  @return                     : None
 */

void  wiced_bt_sco_turn_off_pcm_clock( void );


#ifdef __cplusplus
}
#endif
/** @} wicedbt_sco */
