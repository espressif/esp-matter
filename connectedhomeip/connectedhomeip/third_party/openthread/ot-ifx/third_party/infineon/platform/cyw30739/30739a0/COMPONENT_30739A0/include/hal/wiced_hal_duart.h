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
* DEBUG Universal Asynchronous Receiver/Transmitter (PUART) Driver.
*
*/

#ifndef __WICED_DUART_H__
#define __WICED_DUART_H__

#include "wiced_hal_gpio.h"

/****************************************************************************/
/**
 * @addtogroup  HardwareDrivers Hardware Drivers
 *
 * @{
 */
/****************************************************************************/
/**
 * Defines a driver to facilitate interfacing with the Debug UART hardware to
 * send bytes or a stream of bytes over the Debug UART hardware. Typical
 * use-cases involve printing messages over UART/RS-232.
 *
 * @addtogroup  DUARTDriver Debug UART (DUART)
 * @ingroup HardwareDrivers
 *
 * @{
 */
/****************************************************************************/

/*! @{ */

/******************************************************************************
*** Function prototypes.
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
/// Initialize the Debug UART interface.
///
/// \param tx_pin - target transmit pin
/// \param rx_pin - target receive pin
/// \param baudrate - target baud rate
///
/// \return WICED_TRUE if Success
///
/// Notices: 1. If the tx_pin belongs to the LHL GPIO (<= WICED_P39), the ARM GPIO 6
///             (WICED_GPIO_06) cannot be set to other purpose.
///          2. If the rx_pin belongs to the LHL GPIO (<= WICED_P39), the ARM GPIO 7
///             (WICED_GPIO_07) cannot be set to other purpose.
///          3. The receive function is not supported now.
///////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_hal_duart_init(wiced_bt_gpio_numbers_t tx_pin, wiced_bt_gpio_numbers_t rx_pin, uint32_t baudrate);

///////////////////////////////////////////////////////////////////////////////
/// Transmit data via Debug UART
///
/// \param p_buf - data to be sent
/// \param len - length of data in bytes
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_duart_write(uint8_t *p_buf, uint16_t len);


/* @} DUARTDriver */
/* @} HardwareDrivers */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_DUART_H__
