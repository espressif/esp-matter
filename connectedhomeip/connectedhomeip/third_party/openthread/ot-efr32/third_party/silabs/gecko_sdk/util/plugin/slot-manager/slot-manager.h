/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef __SLOT_MANAGER_H__
#define __SLOT_MANAGER_H__

#include "api/btl_interface.h"
#include "api/btl_interface_storage.h"
#include "sl_status.h"

// Defines
typedef struct {
  BootloaderStorageSlot_t slotStorageInfo;
  ApplicationData_t       slotAppInfo;
} SlotManagerSlotInfo_t;

// Prototypes
void      emberAfPluginSlotManagerInitCallback(void);

/***************************************************************************//**
 * Print information about the external flash, if applicable.
 ******************************************************************************/
void      emberAfPluginSlotManagerPrintExternalFlashInfo(void);

/***************************************************************************//**
 * Read data from the external flash.
 *
 * @param[in] address The address to begin reading from
 * @param[out] data The buffer to fill in
 * @param[in] length The number of bytes to read
 *
 * @return SL_STATUS_OK if successful, else an applicable error code.
 ******************************************************************************/
sl_status_t   emberAfPluginSlotManagerReadExtFlash(uint32_t address,
                                                   uint8_t  *data,
                                                   uint8_t  length);

/***************************************************************************//**
 * Verify image in slot and bootload it if valid.
 *
 * @param[in] slotId The slot to verify and potentially bootload
 *
 * @return SL_STATUS_OK if successful, else an applicable error code.
 *
 * @note This function is for demonstrative purposes only. It holds off other
 *       tasks until image validation is complete. Users should call
 *       emberAfPluginSlotManagerImageIsValidReset and
 *       emberAfPluginSlotManagerImageIsValid instead of this function in
 *       production code.
 ******************************************************************************/
sl_status_t   emberAfPluginSlotManagerVerifyAndBootloadSlot(uint32_t slotId);

/***************************************************************************//**
 * Retrieve address, length, and image information about a slot.
 *
 * @param[in] slotId The slot to retrieve the information for
 * @param[out] slotInfo The SlotManagerInfo_t struct to fill in
 *
 * @return SL_STATUS_OK if successful, else an applicable error code.
 ******************************************************************************/
sl_status_t   emberAfPluginSlotManagerGetSlotInfo(uint32_t              slotId,
                                                  SlotManagerSlotInfo_t *slotInfo);

/***************************************************************************//**
 * Write data to slot.
 *
 * @param[in] slotId The slot ID to write to
 * @param[in] offset The address offset within the slot to write to
 * @param[in] buffer The data to write
 * @param[in] length The number of bytes to write
 *
 * @return SL_STATUS_OK if successful, else an applicable error code.
 ******************************************************************************/
sl_status_t   emberAfPluginSlotManagerWriteToSlot(uint32_t slotId,
                                                  uint32_t offset,
                                                  uint8_t  *buffer,
                                                  size_t   length);

/***************************************************************************//**
 * Erase slot contents.
 *
 * @param[in] slotId The slot ID to erase
 *
 * @return SL_STATUS_OK if successful, else an applicable error code.
 *
 * @note This function is for demonstrative purposes only. It holds off other
 *       tasks until slot erasure is complete. It should not be used in
 *       production code.
 ******************************************************************************/
sl_status_t   emberAfPluginSlotManagerEraseSlot(uint32_t slotId);

/***************************************************************************//**
 * Fetch the number of slots in storage.
 *
 * @param[out] numSlots A buffer to fill in with the number of slots in storage
 *
 * @return SL_STATUS_OK if successful, else an applicable error code.
 ******************************************************************************/
sl_status_t   emberAfPluginSlotManagerGetNumberOfSlots(uint32_t *numSlots);

/***************************************************************************//**
 * Begin image validation in slot.
 *
 * @param[in] slotId The slot ID to validate the image
 *
 * @return SL_STATUS_OK if successful, else an applicable error code.
 *
 * @note Upon calling this function, if SL_STATUS_OK is returned, users
 *       should continuously call emberAfPluginSlotManagerImageIsValid until
 *       either SL_STATUS_OK or an applicable error code is returned.
 ******************************************************************************/
sl_status_t   emberAfPluginSlotManagerImageIsValidReset(uint32_t slotId);

/***************************************************************************//**
 * Continue image validation in a slot.
 *
 * @return SL_STATUS_OK if successful, SL_STATUS_IN_PROGRESS if image is
 *         still being validated, else an applicable error code.
 ******************************************************************************/
sl_status_t   emberAfPluginSlotManagerImageIsValid();

/***************************************************************************//**
 * Boot the image in a slot.
 *
 * @param[in] slotId The slot ID to boot
 *
 * @return SL_STATUS_OK if successful, else an applicable error code.
 *
 * @note Though programmatically this function returns a good status code
 *       if successful, this function will actually not return control to the
 *       caller if so. The bootloader code will begin to boot the image.
 ******************************************************************************/
sl_status_t   emberAfPluginSlotManagerBootSlot(uint32_t slotId);

/***************************************************************************//**
 * Fetch the metadata tags in a GBL image in a slot.
 *
 * @param[in] slotId The slot ID to fetch the metadata tags
 * @param[in] callbackFunction A callback to call when a metadata tag is found
 *
 * @return SL_STATUS_OK if successful, else an applicable error code.
 *
 * @note One metadata tag can cause callbackFunction to be called multiple times
 *       if its data length is longer than a code block used to inspect the
 *       image. The callbackFunction prototype offers an address variable to
 *       indicate that the same metadata tag is being processed.
 ******************************************************************************/
sl_status_t   emberAfPluginSlotManagerGetMetadataTagsFromGbl(
  uint32_t slotId,
  BootloaderParserCallback_t callbackFunction);

#endif  // __SLOT_MANAGER_H__
