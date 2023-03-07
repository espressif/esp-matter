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
* Peripheral SPI driver.
*
*/

#ifndef __WICED_PSPI_DRIVER_H__
#define __WICED_PSPI_DRIVER_H__

#include "wiced.h"

/**  \addtogroup PeripheralSpiDriver Peripheral SPI
* \ingroup HardwareDrivers
* @{
*
* Defines an SPI driver to facilitate communication with other devices on an
* SPI bus (such as a temperature sensor, etc). The driver is capable of
* assuming either a master or a slave role on said bus. Applications use
* this driver to obtain the status from and control the behavior of the SPI
* hardware. This driver only offers services for clock control, mode control
* and data transfer operations. The application is responsible for
* generating the slave/chip select signals (if it's in a master role).
* This could be done by mapping a GPIO pin for each slave the application
* wants to control (See wiced_hal_gpio.h for more information on driving a
* GPIO pin).
*
* All master data transfer operations (half/full duplex)
* provided by this driver assume that the desired
* slave has already been selected and will remain selected
* throughout the duration of the transaction.
*
* Please note that this driver cannot access supplied off-chip memory
* (if installed; serial flash, etc). Please use the drivers found in
* wiced_hal_sflash.h or wiced_hal_seeprom.h to access those modules, as those
* drivers deal with a separate SPI bus and include checks to ensure safe data
* handling operations. This driver is intended only to interface with other
* devices on the peripheral SPI bus, such as a motion sensor. Of course,
* this restriction does not preclude the user from adding their own memory
* module to the peripheral bus, and using this driver with it.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the SPI driver or define
*** return status. They are not modifiable.
******************************************************************************/

/// Type Definitions

/** SPI Interfaces */
typedef enum
{
    SPI1 = 0, /**< SPI1 Interface */
    SPI2  /**< SPI2 Interface */
}spi_interface_t;

/** SPI Device Role */
enum
{
    SPI_MASTER = 1, /**< SPI Device in the Master role */
    SPI_SLAVE  = 2, /**< SPI Device in the Slave role */
};

/// Clock polarity and phase
/// If CPOL=0, base value of the clock is zero
/// If CPOL=1, base value of the clock is one
/// If CPHA=0, sample on leading (first) clock edge
/// If CPHA=1, sample on trailing (second) clock edge
typedef enum SPI_MODE
{
    /// CPOL = 0, CPHA = 0
    /// Data read on clock's rising edge, data changed on a falling edge
    SPI_MODE_0,
    /// CPOL = 0, CPHA = 1
    /// Data read on clock's falling edge, data changed on a rising edge
    SPI_MODE_1,
    /// CPOL = 1, CPHA = 0
    /// Data read on clock's falling edge, data changed on a rising edge
    SPI_MODE_2,
    /// CPOL = 1, CPHA = 1
    /// Data read on clock's rising edge, data changed on a falling edge
    SPI_MODE_3
} SPI_MODE;
/// Slave select polarity (output from master)
typedef enum SPI_SS_POLARITY
{
    /// Slave select active low
    SPI_SS_ACTIVE_LOW,
    /// Slave select active high
    SPI_SS_ACTIVE_HIGH,
} SPI_SS_POLARITY;

/// SPI Endian: Direction of bit data flow (MSB or LSB first).
typedef enum SPI_ENDIAN
{
    /// Transmit most significant bit first
    SPI_MSB_FIRST,
    /// Transmit least significant bit first
    SPI_LSB_FIRST
} SPI_ENDIAN;

///
/// For all the available GPIO config combinations (spiGpioCfg) for master
/// and slave modes, please reference the Kit Guide or HW User Manual for
/// your device, since the values vary by hardware platform.
///


/// Slave Select mode (output from master)
typedef enum SPI_SS_MODE
{
    /// Slave select normal
    SPI_SS_NORMAL,
    /// Slave select goes inactive between bytes
    SPI_SS_INACTIVE_BTW_BYTES
} SPI_SS_MODE;

// Pull configure for input-pin
// - Master input pin
//      -- MISO
//
// - Slave input pin
//      -- CLOCK
//      -- MOSI
//      -- CS
enum
{
    // pull up for MISO if master mode, for MOSI if Slave mode
    INPUT_PIN_PULL_UP   = 0x0400,
    // pull DOWN for MISO if master mode, for MOSI if Slave mode
    INPUT_PIN_PULL_DOWN = 0x0200,
    // FLOAT for MISO if master mode, for MOSI if Slave mode
    INPUT_PIN_FLOATING  = 0x0,
};

// Return values.
typedef enum
{
    SPIFFY_SUCCESS,
    SPIFFY_FAIL,
    SPIFFY_SLAVE_NOT_ENOUGH_RX_FIFO_BYTES
} SPIFFY_STATUS;


/******************************************************************************
*** Function prototypes.
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
/// Initialize the SPI driver with the given parameters. Please reference
/// the various parameters above.
///
/// In this function, the hardware block will be reset. The driver will
/// be configured before the first transaction with a device.
///
/// Please not that the SPI pin selection must be done using
/// Supermux array (wiced_platform_pin_config.c).
///
/// \param spi              - SPI interface.( SPI1 or SPI2 ).
/// \param clkSpeed         - Clock speed (non-zero for master, zero for slave).
/// \param endian           - Direction of bit data flow (MSB or LSB first).
/// \param polarity         - Active high or active low for chip select line.
/// \param mode,            - SPI mode (0-3).
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pspi_init(spi_interface_t           spi,
                                   uint32_t        clkSpeed,
                                   SPI_ENDIAN      endian,
                                   SPI_SS_POLARITY polarity,
                                   SPI_MODE        mode);

///////////////////////////////////////////////////////////////////////////////
/// Send/transmit data over SPI as the master. Assumes that the slave/chip
/// select line will be active throughout the transaction.
///
/// \param spi   - SPI interface.( SPI1 or SPI2 ).
/// \param txLen - The number of bytes-to-send that this buffer contains.
/// \param txBuf - Pointer to the data buffer to transmit.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pspi_tx_data(spi_interface_t spi, uint32_t txLen, const uint8_t* txBuf);

///////////////////////////////////////////////////////////////////////////////
/// Receive data over SPI as the master. Assumes that the slave/chip select
/// line will be active throughout the transaction.
///
/// \param spi   - SPI interface.( SPI1 or SPI2 ).
/// \param txLen - Length of the data buffer to receive.
/// \param txBuf - Pointer to the data buffer which will receive data.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pspi_rx_data(spi_interface_t spi, uint32_t rxLen, uint8_t* rxBuf);


///////////////////////////////////////////////////////////////////////////////
/// Reset and bring the SPI driver to a known good state with default
/// configuration. Note that the hardware block will be reset.
///
/// \param spi  - SPI interface.( SPI1 or SPI2 ).
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pspi_reset(spi_interface_t spi);


///////////////////////////////////////////////////////////////////////////////
/// Transmit one buffer of data while simultaneously receiving data
/// (as the master). Assumes that the slave/chip select line
/// will be active throughout this transaction.
///
/// \param spi   - SPI interface.( SPI1 or SPI2 ).
/// \param len   - The number of bytes to transmit and receive.
/// \param txBuf - Pointer to the data buffer to transmit.
/// \param rxBuf - Pointer to the buffer where the read data will be stored.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pspi_exchange_data(spi_interface_t spi,uint32_t len, const uint8_t* txBuf, uint8_t* rxBuf);


///////////////////////////////////////////////////////////////////////////////
/// Enable the tx fifo so any data in it will be transmitted when the
/// SPI master clocks it out.
///
/// \param spi  - SPI interface.( SPI1 or SPI2 ).
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pspi_slave_enable_tx(spi_interface_t spi);


///////////////////////////////////////////////////////////////////////////////
/// Disable the tx fifo.
///
/// \param spi  - SPI interface.( SPI1 or SPI2 ).
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pspi_slave_disable_tx(spi_interface_t spi);


///////////////////////////////////////////////////////////////////////////////
/// Enable the rx fifo for incoming data when the SPI master clocks it in.
///
/// \param spi  - SPI interface.( SPI1 or SPI2 ).
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pspi_slave_enable_rx(spi_interface_t spi);


///////////////////////////////////////////////////////////////////////////////
/// Disable the rx fifo.
///
/// \param spi  - SPI interface.( SPI1 or SPI2 ).
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pspi_slave_disable_rx(spi_interface_t spi);


///////////////////////////////////////////////////////////////////////////////
/// Send/transmit data over SPI as a slave. If the tx fifo is enabled, the
/// data in the tx fifo will be transmitted when the SPI master clocks it out.
///
/// \param spi   - SPI interface.( SPI1 or SPI2 ).
/// \param txLen - The number of bytes-to-send that this buffer contains.
/// \param txBuf - Pointer to the data buffer to transmit.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pspi_slave_tx_data(spi_interface_t spi, uint32_t txLen, const uint8_t* txBuf);


///////////////////////////////////////////////////////////////////////////////
/// Receive data over SPI as a slave. If the rx fifo is enabled, pull data
/// from the rx fifo if there are at least rxLen bytes in the rx fifo.
///
/// \param spi   - SPI interface.( SPI1 or SPI2 ).
/// \param txLen - Length of the data buffer to receive.
/// \param txBuf - Pointer to the data buffer which will receive data.
///
/// \return SUCCESS if bytes were received or NOT_ENOUGH_RX_FIFO_BYTES if fail.
///////////////////////////////////////////////////////////////////////////////
SPIFFY_STATUS wiced_hal_pspi_slave_rx_data(spi_interface_t spi, uint32_t rxLen, uint8_t* rxBuf);


///////////////////////////////////////////////////////////////////////////////
/// Get the number of bytes in the slave tx fifo.
///
/// \param spi  - SPI interface.( SPI1 or SPI2 ).
///
/// \return Number of bytes in the tx fifo.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_pspi_slave_get_tx_fifo_count(spi_interface_t spi);


///////////////////////////////////////////////////////////////////////////////
/// Get the number of bytes in the slave rx fifo.
///
/// \param spi  - SPI interface.( SPI1 or SPI2 ).
///
/// \return Number of bytes in the rx fifo.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_pspi_slave_get_rx_fifo_count(spi_interface_t spi);


/* @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_PSPI_DRIVER_H__
