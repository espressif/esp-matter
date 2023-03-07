/***************************************************************************//**
* \file <wiced_bt_serial_gatt.h>
*
* \brief
* 	AIROC Serial GATT Service
* 	Provides definitions that can be used by a peer Serial GATT
*        applications to send and receive data from a peer device.
*//*****************************************************************************
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
*******************************************************************************/

/******************************************************************************
* \defgroup wiced_serial_gatt   AIROC Serial GATT Service
* \ingroup     wicedbt_gatt
*
*  AIROC serial over GATT service provides a method to use a serial pipe between
*  an embedded application and application on a peer device.
*
* @{
*
******************************************************************************/

#ifndef WICED_BT_SERIAL_GATT_SERVICE_H
#define WICED_BT_SERIAL_GATT_SERVICE_H

#ifdef _WIN32
static const GUID  GUID_SERIAL_GATT_SERVICE = { 0x695293b2, 0x059d, 0x47e0, { 0xa6, 0x3b, 0x7e, 0xbe, 0xf2, 0xfa, 0x60, 0x7e } };
#endif
#define UUID_SERIAL_GATT_SERVICE             0x7e, 0x60, 0xfa, 0xf2, 0xbe, 0x7e, 0x3b, 0xa6, 0xe0, 0x47, 0x9d, 0x05, 0xb2, 0x93, 0x52, 0x69

#ifdef _WIN32
static const GUID  GUID_SERIAL_GATT_SERIAL_DATA = { 0x614146e4, 0xef00, 0x42bc, { 0x87, 0x27, 0x90, 0x2d, 0x3c, 0xfe, 0x5e, 0x8b } };
#endif
#define UUID_SERIAL_GATT_SERIAL_DATA         0x8b, 0x5e, 0xfe, 0x3c, 0x2d, 0x90, 0x27, 0x87, 0xbc, 0x42, 0x00, 0xef, 0xe4, 0x46, 0x41, 0x61

/* Protocol definitions */
#define SERIAL_GATT_FLAGS_CREDIT_FIELD_PRESENT              0x01
#define SERIAL_GATT_FLAGS_MTU_FIELD_PRESENT                 0x02
#define SERIAL_GATT_FLAGS_DATA_FIELD_PRESENT                0x04

#define SERIAL_GATT_MAX_CREDITS                             255
#define SERIAL_GATT_DEFAULT_MTU                             23

#endif /* _WICED_BT_SERIAL_GATT_H_ */
/* @} */
