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
* Inter-Integrated Circuit (I2C, IIC) Electrically Erasable
* Programmable Read-Only Memory (EEPROM) interface driver.
*
*/

#ifndef __WICED_I2CEEPROM_H__
#define __WICED_I2CEEPROM_H__

#include "wiced.h"

/**  \addtogroup I2CEEPROMInterfaceDriver I2C EEPROM Interface
* \ingroup I2CDriver
* @{
*
* Defines a driver for the I2C EEPROM interface. The driver is responsible for
* interfacing with an EEPROM module via the I2C bus, with its main purpose
* being data handling operations. For instance, any unused space (from the
* system partition) can be used for any purpose to store and access data. It
* is a handy way for an app to save information to non-volatile storage.
*
* Similar to the Serial Flash Interface Driver, this driver includes checks to
* ensure safe data handling operation--it will not allow any write operations
* to take place within active sections (i.e., sections that the system
* currently uses for boot, etc). Note that read operations are unrestricted.
* Please reference the Kit Guide or HW User Manual for your device for more
* information regarding these active sections, their importance, and what roles
* they play in the system.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/

/**
// Status for EEPROM operations.
#define DRV_STATUS_SUCCESS           0x00
#define DRV_STATUS_NO_ACK            0x01
#define DRV_STATUS_INVALID_ADDR      0xFF
**/

/******************************************************************************
*** Function prototypes.
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
/// Initialize the I2C lines and low-level EEPROM Driver.
/// Call this before performing any operations.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_ieeprom_init(void);

///////////////////////////////////////////////////////////////////////////////
/// Sets the size of the EEPROM module in bytes, used for bounds checking.
/// The default is 32768 bytes (256 kBit)
///
/// \param size - Size of the installed module in bytes.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_ieeprom_set_size(uint32_t size);

///////////////////////////////////////////////////////////////////////////////
/// Load data from a certain location on the EEPROM module into
/// memory.
///
/// \param eepromAddress - The starting source address on the EEPROM.
/// \param readCount     - The number of bytes to read.
/// \param buffer        - Pointer to destination data buffer.
/// \param deviceAddress - The source slave address.
///
/// \return Success[0] or no ack[1].
///////////////////////////////////////////////////////////////////////////////
uint8_t wiced_hal_ieeprom_read(uint16_t eepromAddress,
                             uint16_t readCount,
                             uint8_t* buffer,
                             uint8_t  deviceAddress);


///////////////////////////////////////////////////////////////////////////////
/// Write data from memory to a certain location on the EEPROM module.
///
/// (!) Please ensure that the address and (address + length) of data to be
/// written does not go beyond the size of the memory module.
///
/// (!) Note that this function will not allow corruption of certain memory
/// locations, such as currently active sections (boot sectors) and sections
/// required for the proper function of the Bluetooth subsystem.
///
/// \param eepromAddress - The starting destination address on the EEPROM.
/// \param writeCount    - The number of bytes to write.
/// \param buffer        - Pointer to destination data buffer.
/// \param deviceAddress - The destination slave address.
///
/// \return Success[0x00] or no ack[0x01] or invalid eepromAddress/invalid
/// length[0xFF].
///////////////////////////////////////////////////////////////////////////////
uint8_t wiced_hal_ieeprom_write(uint16_t eepromAddress,
                              uint16_t writeCount,
                              uint8_t* buffer,
                              uint8_t  deviceAddress);

/** @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_I2CEEPROM_H__
