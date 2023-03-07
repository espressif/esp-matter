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
* Serial Flash interface driver.
*
*/

#ifndef __WICED_SFLASH_H__
#define __WICED_SFLASH_H__

/**  \addtogroup SerialFlashInterfaceDriver Serial Flash Interface
* \ingroup HardwareDrivers
* Defines a driver for the Serial Flash interface. The driver is responsible
* for interfacing with a Serial Flash memory module via the second SPI bus,
* with its main purpose being data handling operations. For instance, any
* unused space (from the system partition) can be used for any purpose to
* store and access data. It is a handy way for an app to save information to
* non-volatile storage.
*
*
* The software driver supports SPI Flashes with the following parameters:
* 1. Address Width - 24 bit addressing
* 2. Data Rate is specific to the IoT chip:
*       CYW207x9 supports SPI data rate upto 24MHz
* 3. Supports JEDEC's CFI (Common Flash Interface) compatible SPI Flash chips from
*    Cypress Semiconductors, SST, STMicroelectronics, Atmel, MXIC, Adesto, Winbond.
*
* Similar to the I2C EEPROM Interface Driver, this driver includes checks to
* ensure safe data handling operation--it will not allow any write or erase
* operations to take place within active sections (i.e., sections that the
* system currently uses for boot, etc). Note that read operations are
* unrestricted. Please reference the Kit Guide or HW User Manual for your device
* for more information regarding these active sections, their importance, and
* what roles they play in the system.
*
*/
/*! @{ */

/******************************************************************************
*** Function prototypes.
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
/// Initialize the serial flash control module.
///
/// This utility is used to substitute the original wiced_hal_sflash_init utility
/// since the original one set some IO pins to SPI pins.
/// However, the user may want to use different IO pins as the SPI interface.
///
/// The user may call wiced_hal_sflash_clk_rate_set utility to set the clock rate
/// before calling this utility. If the clock rate has not been set, the default
/// clock rate will be 12 MHz.
///
/// Note that the serial flash module uses SPIFFY2 (SPI2) as the interface.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_sflash_init_general(void);

///////////////////////////////////////////////////////////////////////////////
/// Set the clock rate used for communicating with serial flash.
///
/// \param clk_rate    : clock rate
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_sflash_clk_rate_set(uint32_t clk_rate);

///////////////////////////////////////////////////////////////////////////////
/// Initialize the SPI lines and low-level Serial Flash Interface Driver.
/// Call this before performing any SF operations.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
#define wiced_hal_sflash_init   wiced_hal_sflash_init_general

///////////////////////////////////////////////////////////////////////////////
/// Returns the installed SF module size. The low-level Serial Flash Interface
/// Driver sends specific commands to the chip, which then reports its size.
///
/// Note that this function is a good way to make sure that the SF module
/// is installed and is communicating correctly with the system.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_sflash_get_size(void);

///////////////////////////////////////////////////////////////////////////////
/// Load data from a certain location on the serial flash module into
/// memory. To have a better read performance, place the destination data
/// buffer at a word boundary.
///
/// \param addr - The starting source address on the serial flash.
/// \param len  - The number of bytes to read.
/// \param buf  - Pointer to destination data buffer.
///
/// \return The number of bytes read.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_sflash_read(uint32_t addr, uint32_t len, uint8_t *buf);

///////////////////////////////////////////////////////////////////////////////
/// Write data from memory to a certain location on the serial flash module.
///
/// (!) Please ensure that the address and (address + length) of data to be
/// written does not go beyond the size of the memory module. If they do,
/// the write operation will "wrap around" and start corrupting the starting
/// address of the memory (boot sector), rendering the device inoperable.
///
/// (!) Note that this function will not allow corruption of certain memory
/// locations, such as currently active sections (boot sectors) and sections
/// required for the proper function of the Bluetooth subsystem.
///
/// \param addr - The starting destination address on the serial flash.
/// \param len  - The number of bytes to write.
/// \param buf  - Pointer to source data buffer.
///
/// \return The number of bytes written.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_sflash_write(uint32_t addr, uint32_t len, uint8_t *buf);

///////////////////////////////////////////////////////////////////////////////
/// Erase len number of bytes from the serial flash. Depending on the starting
/// address and length, it calls sector or block erase to do the work.
///
/// (!) Please ensure that the address and (address + length) of data to be
/// written does not go beyond the size of the memory module. If they do,
/// the erase operation will "wrap around" and start erasing the starting
/// address of the memory (boot sector), rendering the device inoperable.
///
/// (!) Note that due to the nature of Serial Flash memory and the limitations
/// of sector and/or block boundaries, it is possible that the number of bytes
/// erased could be greater than len.
///
/// \param addr   - The starting erase address on the serial flash.
/// \param len    - The number of bytes to erase.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_sflash_erase(uint32_t addr, uint32_t len);

/* @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_SFLASH_H__
