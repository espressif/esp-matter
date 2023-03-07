/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave miscellaneous
 * functions, not covered by other modules.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_MISC_H_
#define ZPAL_MISC_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-misc
 * @brief
 * Defines a platform abstraction layer for the Z-Wave miscellaneous
 * functions, not covered by other modules.
 *
 * @{
 */

/**
 * @brief Product id struct.
 */
typedef struct {
  uint16_t app_manufacturer_id;   ///< Manufacturer ID identifiers. (MFG_ID_XXX_XXX)
  uint16_t app_product_id;        ///< Product ID. (PRODUCT_ID_XXX_XXX)
  uint16_t app_product_type;      ///< Product type Id. (PRODUCT_TYPE_ID_ZWAVE_XXX_XXX)
  uint16_t app_icon_type;         ///< Z-Wave+ Icon Type identifiers. (ICON_TYPE_XXX_XXX)
  uint8_t generic_type;           ///< Generic Device Class identifier. (GENERIC_TYPE_XXX_XXX)
  uint8_t specyfic_type;          ///< Specific Device Class identifier. (SPECYFIC_TYPE_XXX_XXX)
  uint8_t requested_security_key; ///< Bitmask for security keys. (SECURITY_KEY_SX_XXX)
} zpal_product_id_t;

/**
 * @brief Perform a system reboot.
 */
void zpal_reboot(void);

/**
 * @brief Prepare for shutdown handler.
 */
void zpal_initiate_shutdown_handler(void);

/**
 * @brief Shutdown handler.
 */
void zpal_shutdown_handler(void);

/**
 * @brief Get serial number length.
 *
 * @return Serial number length.
 */
size_t zpal_get_serial_number_length(void);

/**
 * @brief Get serial number.
 *
 * @param[out] serial_number Serial number.
 */
void zpal_get_serial_number(uint8_t *serial_number);

/**
 * @brief Check if in ISR context.
 *
 * @return True if the CPU is in handler mode (currently executing an interrupt handler).
 *         False if the CPU is in thread mode.
 */
bool zpal_in_isr(void);

/**
 * @brief Get chip type.
 *
 * @return Chip type.
 */
uint8_t zpal_get_chip_type(void);

/**
 * @brief Get chip revision.
 *
 * @return Chip revision.
 */
uint8_t zpal_get_chip_revision(void);

/**
 * @brief Get application version.
 *
 * @return Application version.
 *
 * @note This function exists in PAL to allow use app version by external module (e.g. bootloader).
 */
uint32_t zpal_get_app_version(void);

/**
 * @brief Get major part of application version.
 *
 * @return Major part of application version.
 *
 * @note This function exists in PAL to allow use app version by external module (e.g. bootloader).
 */
uint8_t zpal_get_app_version_major(void);

/**
 * @brief Get minor part of application version.
 *
 * @return Minor part of application version.
 *
 * @note This function exists in PAL to allow use app version by external module (e.g. bootloader).
 */
uint8_t zpal_get_app_version_minor(void);

/**
 * @brief Get patch part of application version.
 *
 * @return Patch part of application version.
 *
 * @note This function exists in PAL to allow use app version by external module (e.g. bootloader).
 */
uint8_t zpal_get_app_version_patch(void);

/**
 * @brief Get product id.
 *
 * @param[out] product_id Product id.
 *
 * @note This function exists in PAL to allow use product id by external module (e.g. bootloader).
 */
void zpal_get_product_id(zpal_product_id_t *product_id);

/**
 * @brief Initialize debug output.
 */
void zpal_debug_init(void);

/**
 * @brief Output debug logs.
 *
 * @param[out] data   Pointer to debug data.
 * @param[in]  length Length of debug data.
 */
void zpal_debug_output(const uint8_t *data, uint32_t length);

/**
 * @brief Disable interrupts.
 */
void zpal_disable_interrupts(void);

/**
 * @} //zpal-misc
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_MISC_H_ */
