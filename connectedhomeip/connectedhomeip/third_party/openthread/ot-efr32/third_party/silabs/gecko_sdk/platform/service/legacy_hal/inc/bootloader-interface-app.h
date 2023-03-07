/***************************************************************************//**
 * @file
 * @brief See @ref app_bootload for documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup legacyhal
 * @{
 ******************************************************************************/

/** @addtogroup app_bootload Application Bootloader Interface
 * @brief Definition of the application bootloader interface
 *
 * Some functions in this file return an ::EmberStatus value. See
 * error-def.h for definitions of all ::EmberStatus return values.
 *
 * See bootloader-interface-app.h for source code.
 *@{
 */

#ifndef __BOOTLOADER_INTERFACE_APP_H__
#define __BOOTLOADER_INTERFACE_APP_H__

#include "bootloader-eeprom.h"

/** @brief This is the working unit of data for the app bootloader.  We want
 * it as big as possible, but it must be a factor of the NVM page size and
 * fit into a single Zigbee packet.  We choose  2^6 = 64 bytes.
 */
#define BOOTLOADER_SEGMENT_SIZE_LOG2  6

/** @brief This is the working unit of data for the app bootloader.  We want
 * it as big as possible, but it must be a factor of the NVM page size and
 * fit into a single Zigbee packet.  We choose  2^6 = 64 bytes.
 */
#define BOOTLOADER_SEGMENT_SIZE       (1 << BOOTLOADER_SEGMENT_SIZE_LOG2)

/** @brief Call this function as part of your application initialization
 *          to ensure the storage mechanism is ready to use.
 *         Note: some earlier drivers may assert instead of returning an error
 *          if initialization fails.
 *
 *  @return ::EEPROM_SUCCESS or ::EEPROM_ERR_INVALID_CHIP
 */
uint8_t halAppBootloaderInit(void);

/** @brief Call this function to get information about the attached storage
 *          device and its capabilities.
 *
 *  @return A pointer to a HalEepromInformationType data structure, or NULL
 *           if the driver does not support this API
 */
const HalEepromInformationType *halAppBootloaderInfo(void);

/** @brief Call this function when you are done accessing the storage mechanism
 *          to ensure that it is returned to its lowest power state.
 */
void halAppBootloaderShutdown(void);

/** @brief Call this function once before checking for a valid image to
 *  reset the call flag.
 */
void halAppBootloaderImageIsValidReset(void);

/** @brief Define a numerical value for checking image validity when calling
 * the image interface functions.
 */
#define BL_IMAGE_IS_VALID_CONTINUE  ((uint16_t)0xFFFF)

/// @brief Reads the app image out of storage, calculates the total file
/// CRC to verify the image is intact.
///
/// Caller should loop calling this function
/// while it returns ::BL_IMAGE_IS_VALID_CONTINUE to get final result. This
/// allows caller to service system needs during validation.
///
/// Call ::halAppBootloaderImageIsValidReset() before calling
/// ::halAppBootloaderImageIsValid() to reset the call flag.
///
/// Here is an example application call:
///
/// @code
/// halAppBootloaderImageIsValidReset();
/// while ( (pages = halAppBootloaderImageIsValid() ) == BL_IMAGE_IS_VALID_CONTINUE) {
///   // make app specific calls here, if any
///   emberTick();
/// }
/// @endcode
///
/// @return One of the following:
///    - Number of pages in a valid image
///    - 0 for an invalid image
///    - ::BL_IMAGE_IS_VALID_CONTINUE (-1) to continue to iterate for the final result.
///
uint16_t halAppBootloaderImageIsValid(void);

/** @brief Invokes the bootloader to install the application in storage.
 *  This function resets the device to start the bootloader code and
 *  does not return!
 */
EmberStatus halAppBootloaderInstallNewImage(void);

/** @brief Writes data to the specified raw storage address and length without
 *         being restricted to any page size
 *         Note: Not all storage implementations support accesses that are
 *               not page aligned, refer to the HalEepromInformationType
 *               structure for more information.
 *         Note: Some storage devices require contents to be erased before
 *               new data can be written, and will return an
 *               ::EEPROM_ERR_ERASE_REQUIRED error if write is called on a
 *               location that is not already erased. Refer to the
 *               HalEepromInformationType structure to see if the attached
 *               storage device requires erasing.
 *
 *  @param address  Address to start writing data
 *
 *  @param data     A pointer to the buffer of data to write.
 *
 *  @param len      Length of the data to write
 *
 *  @return ::EEPROM_SUCCESS or ::EEPROM_ERR.
 */
uint8_t halAppBootloaderWriteRawStorage(uint32_t address,
                                        const uint8_t *data,
                                        uint16_t len);

/** @brief Reads data from the specified raw storage address and length without
 *         being restricted to any page size
 *         Note: Not all storage implementations support accesses that are
 *               not page aligned, refer to the HalEepromInformationType
 *               structure for more information.
 *
 *  @param address  Address from which to start reading data
 *
 *  @param data     A pointer to a buffer where data should be read into
 *
 *  @param len      Length of the data to read
 *
 *  @return ::EEPROM_SUCCESS or ::EEPROM_ERR.
 */
uint8_t halAppBootloaderReadRawStorage(uint32_t address, uint8_t *data, uint16_t len);

/** @brief Erases the specified region of the storage device.
 *         Note: Most devices require the specified region to be page aligned,
 *          and will return an error if an unaligned region is specified.
 *         Note: Many devices take an extremely long time to perform an erase
 *          operation.  When erasing a large region, it may be preferable to
 *          make multiple calls to this API so that other application
 *          functionality can be performed while the erase is in progress.
 *          The ::halAppBootloaderStorageBusy() API may be used to determine
 *          when the last erase operation has completed.  Erase timing
 *          information can be found in the HalEepromInformationType structure.
 *
 *  @param address  Address to start erasing
 *
 *  @param len      Length of the region to be erased
 *
 *  @return ::EEPROM_SUCCESS or ::EEPROM_ERR.
 */
uint8_t halAppBootloaderEraseRawStorage(uint32_t address, uint32_t len);

/** @brief Determine if the attached storage device is still busy performing
 *          the last operation, such as a write or an erase.
 *
 *  @return true if still busy or false if not.
 */
bool halAppBootloaderStorageBusy(void);

/** @brief Converts pageToBeRead to an address and the calls storage read
 *          function.
 *         Note: This function is deprecated.  It has been replaced by
 *          halAppBootloaderReadRawStorage()
 *
 *  @param pageToBeRead   pass in the page to be read. This will be converted to the
 *  appropriate address. Pages are ::EEPROM_PAGE_SIZE long.
 *
 *  @param destRamBuffer  a pointer to the buffer to write to.
 *
 *  @return ::EEPROM_SUCCESS or ::EEPROM_ERR.
 */
uint8_t halAppBootloaderReadDownloadSpace(uint16_t pageToBeRead,
                                          uint8_t* destRamBuffer);

/** @brief Converts pageToBeWritten to an address and calls the storage write
 *          function.
 *         Note: This function is deprecated.  It has been replaced by
 *          halAppBootloaderWriteRawStorage()
 *
 * @param pageToBeWritten  pass in the page to be written. This will be
 * converted to the appropriate address. Pages are ::EEPROM_PAGE_SIZE long.
 *
 * @param RamPtr           a pointer to the data to be written.
 *
 * @return ::EEPROM_SUCCESS or ::EEPROM_ERR
 */
uint8_t halAppBootloaderWriteDownloadSpace(uint16_t pageToBeWritten,
                                           uint8_t* RamPtr);

/** @brief Read the application image data from storage
 *
 *  @param timestamp  write the image timestamp to this data pointer.
 *
 *  @param userData  write the user data field to this buffer.
 *
 *  @return ::EEPROM_SUCCESS or ::EEPROM_ERR
 */
uint8_t halAppBootloaderGetImageData(uint32_t *timestamp, uint8_t *userData);

/** @brief Returns the application bootloader version.
 */
uint16_t halAppBootloaderGetVersion(void);

/** @brief Returns the recovery image version.
 */
uint16_t halAppBootloaderGetRecoveryVersion(void);

/** @brief Return a value indicating whether the app bootloader supports IBRs
 *
 *  @return true if the app bootloader supports IBRs, false otherwise
 */
bool halAppBootloaderSupportsIbr(void);

#endif //__BOOTLOADER_INTERFACE_APP_H__

/** @} (end addtogroup app_bootload) */
/** @} (end addtogroup legacyhal) */
