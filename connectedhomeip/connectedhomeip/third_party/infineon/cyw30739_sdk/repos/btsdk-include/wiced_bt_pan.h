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
* \brief This file provides definitions of the persnoal access network(PAN)
* library interface.
*
******************************************************************************/

#ifndef WICED_BT_PAN_H
#define WICED_BT_PAN_H

#if defined(WICED_APP_PANU_INCLUDED) || defined(WICED_APP_PANNAP_INCLUDED)

#ifdef __cplusplus
extern "C" {
#endif

/**
* \addtogroup  wiced_bt_pan_api_functions        PAN Library API
* \ingroup     wicedbt
* @{
* PAN library of the AIROC BTSDK provide a simple method for an application to integrate PAN
* functionality. Application calls the library APIs to init/register/connect/disconnect and
* send data. Library in turn indicates status of the connection and passes to the application.
*/

#include "pan_api.h"

/*****************************************************************************
*                         Function Prototypes
*****************************************************************************/


/*****************************************************************************
*
* Function Name: wiced_bt_panu_sdp_init
*
***************************************************************************//**
*
* This function initializes the pan sdp unit. It should be called
* before accessing any other APIs to initialize the control block
*
* \param           none.
*
* \return          none.
*
*****************************************************************************/
void wiced_bt_panu_sdp_init(void);

/*****************************************************************************
*
* Function Name: wiced_bt_bnep_init
*
***************************************************************************//**
*
* This function initializes the BNEP unit. It should be called
* before accessing any other APIs to initialize the control block
*
* \param           none.
*
* \return          none.
*
*****************************************************************************/
void wiced_bt_bnep_init(void);

/*****************************************************************************
*
* Function Name: wiced_bt_pan_init
*
***************************************************************************//**
*
* This function initializes the PAN unit. It should be called
* before accessing any other APIs to initialize the control block.
*
* \param           none.
*
* \return          none.
*
*****************************************************************************/
void wiced_bt_pan_init(void);

/*****************************************************************************
*
* Function Name: wiced_bt_pan_register
*
***************************************************************************//**
*
* This function is called by the application to register
* its callbacks with PAN profile. The application then
* should set the PAN role explicitly.
*
* \param           p_register: contains all callback function pointers.
*
* \return          none
*
*****************************************************************************/
void wiced_bt_pan_register(tPAN_REGISTER *p_register);

/*****************************************************************************
*
* Function Name: wiced_bt_pan_setrole
*
***************************************************************************//**
*
* This function is called by the application to set the PAN
* profile role. This should be called after wiced_bt_pan_register.
* This can be called any time to change the PAN role.
*
* \param           role: is bit map of roles to be active
*                        PAN_ROLE_CLIENT is for PANU role
*                        PAN_ROLE_GN_SERVER is for GN role
*                        PAN_ROLE_NAP_SERVER is for NAP role.
*
* \return          Status of the PAN operation.
*
*****************************************************************************/
tPAN_RESULT wiced_bt_pan_setrole(uint8_t role);

/*****************************************************************************
*
* Function Name: wiced_bt_pan_writebuf
*
***************************************************************************//**
*
* This sends data over the PAN connections. If this is called
* on GN or NAP side and the packet is multicast or broadcast
* it will be sent on all the links. Otherwise the correct link
* is found based on the destination address and forwarded on it
* If the return value is not PAN_SUCCESS the application should
* take care of releasing the message buffer.
*
* \param           handle:  PAN connection handle.
* \param           dst:     MAC or BD Addr of the destination device.
* \param           src:     MAC or BD Addr of the source who sent this packet.
* \param           protocol:  protocol of the ethernet packet like IP or ARP.
* \param           p_buf:   pointer to the data buffer.
* \param           ext:     to indicate that extension headers present.
*
* \return          Status of the PAN operation.
*
*****************************************************************************/
tPAN_RESULT wiced_bt_pan_writebuf(uint16_t handle,
                                            BD_ADDR dst,
                                            BD_ADDR src,
                                            uint16_t protocol,
                                            BT_HDR *p_buf,
                                            BOOLEAN ext);

/*****************************************************************************
*
* Function Name: wiced_bt_pan_set_multicast_filters
*
***************************************************************************//**
*
* This function is used to set multicast filters on the peer.
*
* \param           handle:        handle for the connection.
* \param           num_filters:   number of multicast filter ranges.
* \param           p_start_array: Pointer to sequence of beginings of all
*                                 multicast address ranges.
* \param           p_end_array:   Pointer to sequence of ends of all
*                                 multicast address ranges.
*
* \return          Status of the PAN operation.
*
*****************************************************************************/
tBNEP_RESULT wiced_bt_pan_set_multicast_filters(uint16_t handle,
                                                     uint16_t num_mcast_filters,
                                                     uint8_t *p_start_array,
                                                     uint8_t *p_end_array);

/*****************************************************************************
*
* Function Name: wiced_bt_pan_set_protocol_filters
*
***************************************************************************//**
*
* This function is used to set protocol filters on the peer.
*
* \param           handle:        handle for the connection.
* \param           num_filters:   number of protocol filter ranges.
* \param           p_start_array: array of starting protocol numbers
* \param           p_end_array:   array of ending protocol numbers
*
* \return          Status of the PAN operation.
*
*****************************************************************************/
tPAN_RESULT wiced_bt_pan_set_protocol_filters(uint16_t handle,
                                                        uint16_t num_filters,
                                                        uint16_t *p_start_array,
                                                        uint16_t *p_end_array);

/*****************************************************************************
*
* Function Name: wiced_bt_pan_deregister
*
***************************************************************************//**
*
* This function is called by the application to de-register
* its callbacks with PAN profile. This will make the PAN to
* become inactive. This will deregister PAN services from SDP
* and close all active connections.
*
* \param           none
*
* \return          none
*
*****************************************************************************/
void wiced_bt_pan_deregister(void);

/*****************************************************************************
*
* Function Name: wiced_bt_pan_connect
*
***************************************************************************//**
*
* This function is called by the application to initiate a
* connection to the remote device.
*
* \param           rem_bda:  BD Addr of the remote device.
* \param           src_role: Role of the local device for the connection.
* \param           dst_role: Role of the remote device for the connection
* \param           handle:   Pointer for returning Handle to the connection
*
* \return          Status of the PAN operation.
*
*****************************************************************************/
tPAN_RESULT wiced_bt_pan_connect(BD_ADDR rem_bda, uint8_t src_role,
                                           uint8_t dst_role, uint16_t *handle);

/*****************************************************************************
*
* Function Name: wiced_bt_pan_disconnect
*
***************************************************************************//**
*
* This is used to disconnect the connection.
*
* \param           handle: handle for the connection
*
* \return          Status of the PAN operation.
*
*****************************************************************************/
tPAN_RESULT wiced_bt_pan_disconnect(uint16_t handle);

/*****************************************************************************
*
* Function Name: wiced_bt_panu_connect
*
***************************************************************************//**
*
* This function do sdp search and then connect request from panu to the NAP.
*
* \param           bd_addr:  BD Addr of the remote device.
*
* \return          none
*
*****************************************************************************/
void wiced_bt_panu_connect(BD_ADDR bd_addr);

/*****************************************************************************
*
* Function Name: wiced_bt_panu_disconnect
*
***************************************************************************//**
*
* This function disconnect request from panu to the NAP.
*
* \param           handle:  handle for the connection.
*
* \return          none
*
*****************************************************************************/
void wiced_bt_panu_disconnect(uint16_t handle);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* defined(WICED_APP_PANU_INCLUDED) || defined(WICED_APP_PANNAP_INCLUDED) */

#endif /* WICED_BT_PAN_H */
