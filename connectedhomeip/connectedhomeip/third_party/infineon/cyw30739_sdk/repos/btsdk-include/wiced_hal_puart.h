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
* List of parameters and defined functions needed to access the
* Peripheral Universal Asynchronous Receiver/Transmitter (PUART) driver.
*
*/

#ifndef __WICED_PUART_H__
#define __WICED_PUART_H__

#include "wiced.h"

typedef enum{
    PARITY_ODD,
    PARITY_EVEN,
    PARITY_NONE
}parity_t;

typedef enum{
    STOP_BIT_1,
    STOP_BIT_2
}stop_bit_t;

/****************************************************************************/
/**
 * @addtogroup  HardwareDrivers Hardware Drivers
 *
 * @{
 */
/****************************************************************************/
/**
 * Defines a driver to facilitate interfacing with the UART hardware to send
 * and receive bytes or a stream of bytes over the UART hardware. Typical
 * use-cases involve printing messages over UART/RS-232, or to communicate
 * with peripheral devices.
 *
 * <b>Example Usage:</b>
 *
 * \code{.c}
 * void testPUARTDriver(void)
 * {
 *     uint8_t readbyte;
 *     uint8_t loopCtrl = 1;
 *     char printBuffer[50];
 *
 *     //Pins for PUART should be configured from wiced_platform_pin_config.c
 *     wiced_hal_puart_init();
 *
 *     wiced_hal_puart_flow_off();  //turn off flow control
 *     wiced_hal_puart_enable_tx();
 *     wiced_hal_puart_enable_rx();
 *
 *     while(loopCtrl)
 *     {
 *         while(wiced_hal_puart_read(&readbyte))
 *         {
 *             wiced_hal_puart_write(readbyte);
 *
 *             if(readbyte == 'S')
 *             {
 *                 wiced_hal_puart_print("\nYou typed 'S'.");
 *
 *                 sprintf(printBuffer, "\nThis message sprintf'ed here.");
 *                 wiced_hal_puart_print(printBuffer);
 *             }
 *
 *             if(readbyte == 'E') // End.
 *             {
 *                 loopCtrl = 0;
 *             }
 *         }
 *     }
 * }
 * \endcode
 *
 * @addtogroup  PUARTDriver Peripheral UART (PUART)
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
/// Initialize the Peripheral UART interface with the default configuration
/// parameters. This must be invoked once at boot before using any of PUART's
/// services.
///
/// Default baud rate is 115200 Bd. This can be changed by calling
/// "wiced_hal_puart_configuration()" as described later, after this
/// initialization function.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_init(void);

///////////////////////////////////////////////////////////////////////////////
/// Enable flow control.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_flow_on(void);


///////////////////////////////////////////////////////////////////////////////
/// Disable flow control.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_flow_off(void);


///////////////////////////////////////////////////////////////////////////////
/// Select the TX/RX and optional CTS/RTS pins (P<pin>) for the UART hardware
/// to use.
///
/// Please follow the guidelines set in the User Documentation when
/// selecting pins, as not all pins are avaliable for the PUART driver;
/// they depend on the specific hardware platform.
///
/// Remember that P# is the physical pin number on the board minus 1.
///         Ex: Board Pin 33 = P32.
///
/// \param rxdPin - RX Pin
/// \param txdPin - TX Pin
/// \param ctsPin - CTS Pin
/// \param rtsPin - RTS Pin
///
/// \return TRUE if pads were successfully set, FALSE if pads were not set.
/// If FALSE, make sure input port/pin parameters are correct.
///////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_hal_puart_select_uart_pads(uint8_t rxdPin, uint8_t txdPin,
                                        uint8_t ctsPin, uint8_t rtsPin);

///////////////////////////////////////////////////////////////////////////////
/// Print/send a string of characters via the TX line.
///
/// \param string - A string of characters to send.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_print(char * string);


///////////////////////////////////////////////////////////////////////////////
/// Print/send one byte via the TX line.
///
/// \param byte - Byte to send on the TX line.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_write(uint8_t byte);


///////////////////////////////////////////////////////////////////////////////
/// Read one byte via the RX line.
///
/// \param rxByte - Destination byte to hold received data byte from RX FIFO.
///
/// \return TRUE if data was successfully read, FALSE if
/// not (RX FIFO was empty).
///////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_hal_puart_read(uint8_t* rxByte);


///////////////////////////////////////////////////////////////////////////////
/// Disable transmit capability.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_disable_tx(void);


///////////////////////////////////////////////////////////////////////////////
/// Enable transmit capability.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_enable_tx(void);


///////////////////////////////////////////////////////////////////////////////
/// Enable receive capability (specifically, by enabling PUART RX interrupts
/// through the MIA driver).
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_enable_rx(void);

///////////////////////////////////////////////////////////////////////////////
/// Set the baud rate (Bd) to a value other than the default 115200 Bd.
///
/// \param baudrate - Desired rate in symbols per second, e.g. 9600.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_set_baudrate(uint32_t baudrate)__attribute__ ((deprecated("Please use wiced_hal_puart_configuration()")));


///////////////////////////////////////////////////////////////////////////////
/// Read in a set of bytes sequentially.
///
/// \param buffer - Destination buffer to hold incoming bytes.
/// \param lenth  - Number of bytes to read from the RX FIFO.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_synchronous_read(uint8_t* buffer, uint32_t length);


///////////////////////////////////////////////////////////////////////////////
/// Write a set of bytes sequentially.
///
/// \param buffer - Source buffer to hold outgoing bytes.
/// \param lenth  - Number of bytes to write to the TX FIFO.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_synchronous_write(uint8_t* buffer, uint32_t length);

///////////////////////////////////////////////////////////////////////////////
/// Check to see if there is any data ready in the RX FIFO.
///
/// \param none
///
/// \return TRUE if bytes are avaliable, FALSE if the FIFO is empty.
///////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_hal_puart_rx_fifo_not_empty(void);

///////////////////////////////////////////////////////////////////////////////
/// Clears and enables PUART interrupt.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_reset_puart_interrupt(void);

///////////////////////////////////////////////////////////////////////////////
/// Register Interrupt handler with PUART
///
/// \param puart_rx_cbk - Call back function to process rx bytes
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_register_interrupt(void (*puart_rx_cbk)(void*));

///////////////////////////////////////////////////////////////////////////////
/// updates the watermark level to the received value
///
/// \param watermark_level - watermark level in bytes
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_set_watermark_level(uint32_t watermark_level);

///////////////////////////////////////////////////////////////////////////////
/// updates the parity and stop bits as per received value
/// \param baudrate - Desired rate in symbols/sec ex. 9600
/// \param parity   - PARITY_ODD for odd parity, PARITY_EVEN for even parity,
///                   PARITY_NONE for no parity.
/// \param stop_bit - STOP_BIT_1 for 1 stop bit, STOP_BIT_2 for 2 stop bits.
///
/// \return wiced_bool_t - WICED_TRUE if the configuration was successfully set,
///                   otherwise WICED_FALSE. NOTE: For baudrate greater than
///                   2.727272 Mb/sec use of 2 stop bits is mandatory due to
///                   hardware limitation.
///////////////////////////////////////////////////////////////////////////////

wiced_bool_t wiced_hal_puart_configuration(uint32_t baudrate, parity_t parity,
                                           stop_bit_t stop_bit);

/* @} PUARTDriver */
/* @} HardwareDrivers */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_PUART_H__
