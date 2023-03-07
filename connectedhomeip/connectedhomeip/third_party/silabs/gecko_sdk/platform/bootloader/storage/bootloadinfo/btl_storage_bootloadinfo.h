/***************************************************************************//**
 * @file
 * @brief Bootload Info for Silicon Labs Bootloader.
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
#ifndef BTL_STORAGE_BOOTLOADINFO_H
#define BTL_STORAGE_BOOTLOADINFO_H

#include <stdint.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Storage
 * @{
 * @addtogroup BootloadInfo Bootload Info
 * @{
 * @brief Indicates which firmware update image should be bootloaded next.
 * @details
 *   This component provides the bootloader with support for storing multiple
 *   images and attempting to bootload a prioritized list. The Bootload
 *   Information  struct is placed at a known location in storage, and points
 *   to a list of images to attempt to bootload.
 *
 *   While the Bootload Information list of images to attempt to bootload has a
 *   compile-time configurable size, the bootloader is capable of handling lists
 *   of images with different sizes, e.g., if a bootloader upgrade changes the
 *   slot layout or if a storage device that was initialized on a different
 *   device is used.
 *
 *   If only one storage slot is available, the functions available in this API
 *   will do nothing and return applicable error codes (@ref BOOTLOADER_OK for
 *   @ref storage_getBootloadList and @ref storage_setBootloadList, and
 *   @ref BOOTLOADER_ERROR_BOOTLOAD_LIST_FULL for
 *   @ref storage_appendBootloadList).
 ******************************************************************************/

/// Magic word indicating @ref BootloadInfo_t struct
#define BTL_STORAGE_BOOTLOADINFO_MAGIC       0xE18F5239UL

/// Version number for the @ref BootloadInfo_t struct
#define BTL_STORAGE_BOOTLOADINFO_VERSION     0x00000001UL

/// Maximum number of items in the @ref BootloadInfo_t.bootloadList
#define BTL_STORAGE_BOOTLOAD_LIST_MAX_LENGTH 16UL

/// Definition of the Bootload Info struct.
///
/// The Bootload Info struct contains a prioritized list of firmware update
/// images that the
/// bootloader should attempt to bootload. The first image to pass verification
/// will be bootloaded.
typedef struct {
  /// @brief Magic word indicating that this is a Bootload Info struct
  ///        (@ref BTL_STORAGE_BOOTLOADINFO_MAGIC)
  uint32_t  magic;
  /// Struct version number
  uint32_t  structVersion;
  /// Size of the BootloadInfo_t struct
  uint32_t  length;
  /// List of addresses of slots to bootload from.
  int32_t  bootloadList[BTL_STORAGE_BOOTLOAD_LIST_MAX_LENGTH];
  /// Checksum of the struct
  uint32_t  crc32;
} BootloadInfo_t;

/***************************************************************************//**
 * Get list of firmware update images to attempt to bootload.
 *
 * @param[out] slotIds Pointer to array of integers to fill with slot IDs,
 *                     or -1 if list position does not contain a valid slot ID
 * @param[in]  length  Number of slot IDs to get
 *
 * @return Error code: BOOTLOADER_OK if list of slots was successfully filled
 *          else error code in @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
int32_t storage_getBootloadList(int32_t slotIds[], size_t length);

/***************************************************************************//**
 * Set list of firmware update images to attempt to bootload.
 *
 * @param[in]  slotIds Pointer to array of slot IDs to set
 * @param[in]  length  Number of slot IDs to set
 *
 * @return Error code: BOOTLOADER_OK if list of slots was successfully filled
 ******************************************************************************/
int32_t storage_setBootloadList(int32_t slotIds[], size_t length);

/***************************************************************************//**
 * Append a storage slot to the list of storage slots to try bootloading
 * from.
 *
 * @param[in]  slotId  ID of the slot
 *
 * @return Error code: BOOTLOADER_OK if slot was successfully appended
 ******************************************************************************/
int32_t storage_appendBootloadList(int32_t slotId);

/** @} // addtogroup BootloadInfo */
/** @} // addtogroup Storage */
/** @} // addtogroup Components */
#endif // BTL_STORAGE_BOOTLOADINFO_H
