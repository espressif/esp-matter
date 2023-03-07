/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave NVM.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_NVM_H_
#define ZPAL_NVM_H_

#include <stdint.h>
#include <stddef.h>
#include "zpal_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-nvm
 * @brief
 * Defines a platform abstraction layer for the Z-Wave NVM.
 *
 * How to use the entropy API
 *
 * The ZPAL NVM provides a way for an application to safely store and retrieve
 * variable size data in a page-based non-volatile memory (NVM).
 * Data are identified with 20-bit identifiers denoted as data_id.
 *
 * The following outlines an example of use:
 * 1. Initialize module with zpal_nvm_init().
 * 2. Invoke zpal_nvm_write() to store data.
 * 3. Invoke zpal_nvm_read() to get data.
 *
 * @{
 */

/**
 * @brief Defines the different areas of NVM storage.
 */
typedef enum
{
  ZPAL_NVM_AREA_APPLICATION,          ///< Reserved for application data.
  ZPAL_NVM_AREA_STACK,                ///< Reserved for stack data.
  ZPAL_NVM_AREA_MANUFACTURER_TOKENS,  ///< Reserved for manufacturer tokens.
} zpal_nvm_area_t;

/**
 * @brief Data id type. Only the 20 least significant bits are used.
 */
typedef uint32_t zpal_nvm_object_key_t;

/**
 * @brief Defines the NVM handle type.
 */
typedef void * zpal_nvm_handle_t;

/**
 * @brief Initializes the NVM for a given area.
 *
 * @param[in] area NVM area to initialize.
 * @return A valid NVM handle if successfully initialized. Otherwise it returns NULL.
 */
zpal_nvm_handle_t zpal_nvm_init(zpal_nvm_area_t area);

/**
 * @brief Reads object from a given area handle and given object key.
 *
 * @param[in]  handle         NVM area handle.
 * @param[in]  key            Object key.
 * @param[out] object         Address of array where object can be written to.
 * @param[in]  object_size    Size of the stored object.
 * @return @ref ZPAL_STATUS_OK if the object was successfully read and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_nvm_read(zpal_nvm_handle_t handle, zpal_nvm_object_key_t key, void *object, size_t object_size);

/**
 * @brief Reads part of object from a given area handle and given object key.
 *
 * @param[in]  handle       NVM area handle.
 * @param[in]  key          Object key.
 * @param[out] object       Address of array where object can be written to.
 * @param[in]  offset       The offset where object shall be read from.
 * @param[in]  object_size  Size of the stored object.
 * @return @ref ZPAL_STATUS_OK if the object was successfully read and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_nvm_read_object_part(zpal_nvm_handle_t handle, zpal_nvm_object_key_t key, void *object, size_t offset, size_t object_size);

/**
 * @brief Writes object to a given area handle and given object key.
 *
 * @param[in]  handle       NVM area handle.
 * @param[in]  key          Object key.
 * @param[out] object       Address of array of object that must be written.
 * @param[in]  object_size  Size of the object to be stored.
 * @return @ref ZPAL_STATUS_OK if the object was successfully written and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_nvm_write(zpal_nvm_handle_t handle, zpal_nvm_object_key_t key, const void *object, size_t object_size);

/**
 * @brief Erases everything in a given area.
 *
 * @param[in] handle NVM area handle.
 * @return @ref ZPAL_STATUS_OK if the all objects was successfully erased and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_nvm_erase_all(zpal_nvm_handle_t handle);

/**
 * @brief Delete an object from NVM.
 *
 * @param[in]   handle  Nvm storage handle.
 * @param[in]   key     Object key.
 * @return @ref ZPAL_STATUS_OK on success and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_nvm_erase_object(zpal_nvm_handle_t handle, zpal_nvm_object_key_t key);

/**
 * @brief Get the object size identified with a given key from NVM.
 *
 * @param[in]   handle  Nvm storage handle.
 * @param[in]   key     Object key.
 * @param[out]  len     Object size.
 * @return @ref ZPAL_STATUS_OK on success and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_nvm_get_object_size(zpal_nvm_handle_t handle, zpal_nvm_object_key_t key, size_t *len);

/**
 * @brief Create a list of key for valid objects in NVM.
 *
 * @param[in]   handle            Nvm storage handle.
 * @param[out]  key_list      A pointer to a buffer for the key list.
 * @param[in]   key_list_size The size of the key list buffer.
 * @param[in]   key_min       The lower search key.
 * @param[in]   key_max       The upper search key.
 * @return The number of keys written to the key list. This value is less than or equal to @p key_list_size.
 */
size_t zpal_nvm_enum_objects(zpal_nvm_handle_t handle,
                             zpal_nvm_object_key_t *key_list,
                             size_t key_list_size,
                             zpal_nvm_object_key_t key_min,
                             zpal_nvm_object_key_t key_max);

/**
 * @brief Opens the NVM for a backup/restore operation.
 *
 * @return @ref ZPAL_STATUS_OK on success and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_nvm_backup_open(void);

/**
 * @brief Closes the NVM after backup/restore operation.
 */
void zpal_nvm_backup_close(void);

/**
 * @brief Reads raw data from the NVM.
 *
 * @param[in]  offset       The offset where data shall be read from.
 * @param[out] data         Address of array where data can be written to.
 * @param[in]  data_length  Length of the stored data.
 * @return @ref ZPAL_STATUS_OK if the data was successfully read and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_nvm_backup_read(uint32_t offset, void *data, size_t data_length);

/**
 * @brief Writes raw data to the NVM.
 *
 * @param[in]  offset       The offset where data shall be written to.
 * @param[out] data         Address of array of data that must be written.
 * @param[in]  data_length  Length of the data to be stored.
 * @return @ref ZPAL_STATUS_OK if the data was successfully written and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_nvm_backup_write(uint32_t offset, const void *data, size_t data_length);

/**
 * @brief Get size of raw NVM area.
 *
 * @return Size of raw NVM area.
 */
uint32_t zpal_nvm_backup_get_size(void);

/**
 * @brief Set write protection for storage
 *
 * @param[in]   handle  Nvm storage handle.
 * @return @ref ZPAL_STATUS_OK if the lock was set successfully, @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_nvm_lock(zpal_nvm_handle_t handle);

/**
 * @} //zpal-nvm
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_NVM_H_ */
