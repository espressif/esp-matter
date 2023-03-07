/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave bootloader.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_BOOTLOADER_H_
#define ZPAL_BOOTLOADER_H_

#include <stdbool.h>
#include "zpal_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-bootloader
 * @brief
 * Defines a platform abstraction layer for the Z-Wave bootloader.
 *
 * How to use the bootloader API
 *
 * The ZPAL bootloader API is required if an application would like to store and use a new firmware
 * image.
 *
 * zpal_bootloader_init() and zpal_bootloader_get_info() must be implemented as Z-Wave always
 * invokes these. The remaining API functions are optional in case a bootloader is not present in
 * the ZPAL implementation.
 *
 * The following outlines an example of use:
 * 1. To enable the bootloader functionality, invoke zpal_bootloader_init().
 * 2. zpal_bootloader_get_info() must be invoked to check whether a bootloader is present.
 * 3. For every received fragment, zpal_bootloader_write_data() must be invoked to store the new
 *    firmware image.
 * 4. When all fragments are received, zpal_bootloader_verify_image() must be invoked to verify
 *    the integrity of the received firmware image.
 * 5. If the received firmware image is valid, invoke zpal_bootloader_reboot_and_install() install
 *    and boot from the new firmware image.
 *
 * Requirements:
 * - The ZPAL bootloader requires that a storage is available for storing a received firmware image.
 *   Z-Wave has no requirements on how and where the image is stored.
 *
 * @{
 */

/**
 * @brief Type of bootloader
 */
typedef enum
{
  ZPAL_BOOTLOADER_NOT_PRESENT = 0,  ///< No bootloader present.
  ZPAL_BOOTLOADER_PRESENT     = 1,  ///< Bootloader present.
} zpal_bootloader_type_t;

/**
 * @brief Information about the current bootloader
 */
typedef struct {
  zpal_bootloader_type_t type;  ///< The type of bootloader.
  uint32_t version;             ///< Version number of the bootloader. See \ref zpal-bootloader-info-version for masking
  uint32_t capabilities;        ///< Capability mask for the bootloader.
} zpal_bootloader_info_t;

/**
 * @brief Bootloader has the capability of storing data in an internal or
 * external storage medium
 */
#define ZPAL_BOOTLOADER_CAPABILITY_STORAGE                     (1 << 0)

/** \addtogroup zpal-bootloader-info-version 
 * Defines a platform abstraction layer for the Z-Wave bootloader.
 * @{
 */
/**
 * @brief Bootloader version major version shift value
 */
#define ZPAL_BOOTLOADER_VERSION_MAJOR_SHIFT (24U)
/**
 * @brief Bootloader version minor version shift value
 */
#define ZPAL_BOOTLOADER_VERSION_MINOR_SHIFT (16U)
/**
 * @brief Bootloader version bugfix version shift value
 */
#define ZPAL_BOOTLOADER_VERSION_BUGFIX_SHIFT (0U)
/**
 * @brief Bootloader version major version mask
 */
#define ZPAL_BOOTLOADER_VERSION_MAJOR_MASK  (0xFF000000U)
/**
 * @brief Bootloader version minor version mask
 */
#define ZPAL_BOOTLOADER_VERSION_MINOR_MASK  (0x00FF0000U)
/**
 * @brief Bootloader version bugfix version mask
 */
#define ZPAL_BOOTLOADER_VERSION_BUGFIX_MASK (0x0000FFFFU)
/**
 * @} //zpal-bootloader-info-version
 */

/**
 * @brief Get information about the bootloader on this device.
 * The information returned is fetched from the main bootloader
 * information table.
 *
 * @param[out] info Pointer to the bootloader information struct.
 */
void zpal_bootloader_get_info(zpal_bootloader_info_t *info);

/**
 * @brief Initialize components of the bootloader
 * so the app can use the interface. This typically includes initializing
 * serial peripherals for communication with external SPI flashes, and so on.
 *
 * @return Error code. @ref ZPAL_STATUS_OK on success, else @ref ZPAL_STATUS_FAIL
 */
zpal_status_t zpal_bootloader_init(void);

/**
 * @brief Reboot into the bootloader to install something.
 * If there is a storage component and a slot is marked for bootload, install
 * the image in that slot after verifying it.
 * If a communication component is present, open the communication channel and
 * receive an image to be installed.
 */
void zpal_bootloader_reboot_and_install(void);

/**
 * @brief Verify that the image is valid.
 *
 * @return @ref ZPAL_STATUS_OK if the image is valid, else @ref ZPAL_STATUS_FAIL.
 */
zpal_status_t zpal_bootloader_verify_image(void);

/**
 * @brief Writes a fragment of a firmware image to the bootloader storage.
 *
 * @param[in] offset Offset from the storage start.
 * @param[in] data Pointer to fragment of firmware image.
 * @param[in] length Length of firmware image fragment.
 * @return @ref ZPAL_STATUS_OK on success, and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_bootloader_write_data(uint32_t offset, uint8_t * data, uint16_t length);

/**
 * @brief Returns whether the software is running on the first boot after a new firmware image was written.
 * @param[in] updated_sucessfully Set to true if update was successfull, false otherwise. Valid only when 
 *                                it's the first boot.
 * @return True if it's the first boot, false otherwise.
 */
bool zpal_bootloader_is_first_boot(bool *updated_successfully);

/**
 * @} //zpal-bootloader
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_BOOTLOADER_H_ */
