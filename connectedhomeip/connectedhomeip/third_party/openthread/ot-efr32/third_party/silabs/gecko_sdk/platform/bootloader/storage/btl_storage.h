/***************************************************************************//**
 * @file
 * @brief Storage component for Silicon Labs Bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef BTL_STORAGE_H
#define BTL_STORAGE_H

#include "config/btl_config.h"

#include <stddef.h>
#include <stdbool.h>

#include "api/btl_interface.h"

#include "bootloadinfo/btl_storage_bootloadinfo.h"

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Storage
 * @{
 * @brief Storage component
 * @details
 *   This component provides the bootloader with multiple storage options. All
 *   storage implementations have to provide a slot-based API to access image
 *   files to be bootloaded.
 *
 *   Some storage implementations also support a raw storage API to access the
 *   underlying storage medium. This can be used by applications to store other
 *   data in parts of the storage medium that are not used for bootloading.
 *
 * @section storage_implementations Storage Implementations
 ******************************************************************************/

/// Version number for bootloader storage functions struct
#define BOOTLOADER_STORAGE_FUNCTIONS_VERSION 0x00000100

/// Information about the storage backend
typedef struct {
  /// Type of storage
  BootloaderStorageType_t storageType;
  /// Number of slots
  uint32_t numSlots;
  /// Array of slot information.
  BootloaderStorageSlot_t slot[];
} BootloaderStorageLayout_t;

/***************************************************************************//**
 * Initialize the storage component.
 *
 * @return Error code indicating success or failure.
 * @retval ::BOOTLOADER_OK on success
 * @retval ::BOOTLOADER_ERROR_INIT_STORAGE on failure
 ******************************************************************************/
int32_t storage_init(void);

/***************************************************************************//**
 * Main function for storage component.
 *
 * @return @ref BOOTLOADER_OK on success, else error code
 ******************************************************************************/
int32_t storage_main(void);

/***************************************************************************//**
 * Shutdown storage component.
 *
 * @return Error code indicating success or failure.
 * @retval ::BOOTLOADER_OK on success
 * @retval ::BOOTLOADER_ERROR_INIT_STORAGE on failure
 ******************************************************************************/
int32_t storage_shutdown(void);

/***************************************************************************//**
 * Get information about the storage component running on the device.
 *
 * @param[out] info Pointer to @ref BootloaderStorageInformation_t struct
 ******************************************************************************/
void storage_getInfo(BootloaderStorageInformation_t *info);

/***************************************************************************//**
 * Get information about the layout of a storage slot
 *
 * @param[in]  slotId Slot ID to get info about
 * @param[out] slot   Pointer to @ref BootloaderStorageSlot_t struct
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
int32_t storage_getSlotInfo(uint32_t                slotId,
                            BootloaderStorageSlot_t *slot);

/***************************************************************************//**
 * Get information about the contents of a storage slot
 *
 * @note @ref storage_initParseSlot <b>must</b> be called before calling this
 *       function to initialize the context.
 *
 * @note If the slot does not contain an application or a bootloader, the
 *       the corresponding values are set to zero.
 *
 * @param[in]  context Parsing context. Should be allocated by the application
 *                     and initialized by calling storage_initParseSlot before
 *                     calling this function.
 * @param[out] appInfo Pointer to @ref ApplicationData_t struct
 * @param[out] bootloaderVersion Pointer to an unsigned integer representing
 *                               the bootloader version number.
 *
 * @return @ref BOOTLOADER_OK on success
 ******************************************************************************/
int32_t storage_getSlotMetadata(BootloaderParserContext_t *context,
                                ApplicationData_t         *appInfo,
                                uint32_t                  *bootloaderVersion);

/***************************************************************************//**
 * Initialize the context variable for checking a slot and trying to parse
 * the image contained in it.
 *
 * @param[in] slotId      Slot to check for valid image
 * @param[in] context     Pointer to BootloaderParserContext_t struct
 * @param[in] contextSize Length of the context struct
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_PARSE_BASE range
 ******************************************************************************/
int32_t storage_initParseSlot(uint32_t                  slotId,
                              BootloaderParserContext_t *context,
                              size_t                    contextSize);

/***************************************************************************//**
 * Check the given slot for a valid image. Call this function continuously
 * until it stops returning.
 * @ref BOOTLOADER_ERROR_PARSE_CONTINUE.
 *
 * The function returns @ref BOOTLOADER_ERROR_PARSE_SUCCESS if the
 * image in the slot was successfully verified. For detailed information on the
 * parsed image, see imageProperties in the context variable.
 *
 * @param[in] context Pointer to BootloaderImageParsingContext_t struct
 * @param[in] metadataCallback Functionpointer which will be called with any
 *                             binary metadata that might be contained within
 *                             the image.
 *
 * @return @ref BOOTLOADER_ERROR_PARSE_CONTINUE if the parsing is not
 *         complete, @ref BOOTLOADER_ERROR_PARSE_SUCCESS on success.
 ******************************************************************************/
int32_t storage_verifySlot(BootloaderParserContext_t  *context,
                           BootloaderParserCallback_t metadataCallback);

/***************************************************************************//**
 * Upgrade SE using image contained in a slot.
 *
 * @note This function assumes the image located in the slotId has been
 *       verified first.
 *
 * @param slotId Slot ID to bootload from
 *
 * @return True if the operation succeeded
 ******************************************************************************/
bool storage_upgradeSeFromSlot(uint32_t slotId);

/***************************************************************************//**
 * Bootload a bootloader image contained in a slot
 *
 * @note This function assumes the image located in the slotId has been
 *       verified first.
 *
 * @param slotId Slot ID to bootload from
 * @param version Version number of new bootloader
 *
 * @return True if operation succeeded
 ******************************************************************************/
bool storage_bootloadBootloaderFromSlot(uint32_t slotId, uint32_t version);

/***************************************************************************//**
 * Bootload an image contained in a slot.
 *
 * @note This function assumes the image located in slotId has been
 *       verified first.
 *
 * @param slotId  Slot ID to bootload from
 * @param version Cached version number of the image contained in the slot
 *   (used for downgrade prevention)
 *
 * @return True if the operation succeeded
 ******************************************************************************/
bool storage_bootloadApplicationFromSlot(uint32_t slotId, uint32_t version);

/***************************************************************************//**
 * Erase the contents of a storage slot including all data and metadata.
 *
 * @param slotId ID of the slot.
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
int32_t storage_eraseSlot(uint32_t slotId);

/***************************************************************************//**
 * Read a number of words from a storage slot.
 *
 * @param slotId     ID of the slot.
 * @param offset     The offset into the slot in bytes.
 * @param buffer     Pointer to buffer to store read data in.
 * @param numBytes   Number of bytes to read.
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
int32_t storage_readSlot(uint32_t slotId,
                         uint32_t offset,
                         uint8_t  *buffer,
                         size_t   numBytes);

/***************************************************************************//**
 * Write a number of words to a storage slot.
 *
 * @param slotId   ID of the slot.
 * @param offset   The offset into the slot in bytes.
 * @param data     Pointer to data to write.
 * @param numBytes Length of data to write. Must be a multiple of 4.
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
int32_t storage_writeSlot(uint32_t slotId,
                          uint32_t offset,
                          uint8_t  *data,
                          size_t   numBytes);

/***************************************************************************//**
 * Read number of words from raw storage.
 *
 * @param address    The raw address of the storage.
 * @param buffer     Pointer to the buffer to store read data in.
 * @param numBytes   Number of bytes to read.
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
int32_t storage_readRaw(uint32_t address,
                        uint8_t  *buffer,
                        size_t   numBytes);

/***************************************************************************//**
 * Write a number of words to raw storage.
 *
 * @param address  The raw address of the storage.
 * @param data     Pointer to data to write.
 * @param numBytes Length of data to write. Must be a multiple of 4.
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
int32_t storage_writeRaw(uint32_t address,
                         uint8_t  *data,
                         size_t   numBytes);

/***************************************************************************//**
 * Get allocated DMA channel for MSC write
 *
 * @return A positive number channel. -1 if DMA-based MSC write
 *         is not enabled.
 ******************************************************************************/
int32_t storage_getDMAchannel(void);

/***************************************************************************//**
 * Erase the raw storage.
 *
 * @param address Start address of the region to erase
 * @param length  Number of bytes to erase
 *
 * @note Some devices, such as Flash-based storages, have restrictions on
 *       the alignment and size of erased regions. The details of the
 *       limitations of a particular storage can be found by reading
 *       the BootloaderStorageInformation_t struct using @ref storage_getInfo.
 *
 * @return @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
int32_t storage_eraseRaw(uint32_t address, size_t length);

/***************************************************************************//**
 * Poll the storage implementation and check whether it is busy.
 *
 * @return True if the storage is busy
 ******************************************************************************/
bool storage_isBusy(void);

/***************************************************************************//**
 * Get PPUSATD word of the (E)USART in use
 *
 * @param ppusatdNr PPUSATD register number
 *
 * @return Word representing the PPUSATD bit field of the (E)USART in use.
 *         0 if not found.
 ******************************************************************************/
uint32_t storage_getSpiUsartPPUSATD(uint32_t *ppusatdNr);

/** @} // addtogroup Storage */
/** @} addtogroup Components */
#endif // BTL_STORAGE_H
