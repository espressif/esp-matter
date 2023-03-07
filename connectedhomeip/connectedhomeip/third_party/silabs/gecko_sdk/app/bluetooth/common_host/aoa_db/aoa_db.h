/***************************************************************************//**
 * @file
 * @brief Tag database handler header file
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef AOA_DB_H
#define AOA_DB_H

#include <stdint.h>
#include "sl_bt_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Defines

// Bluetooth address length
#define ADR_LEN 6

// -----------------------------------------------------------------------------
// Type definitions.

/// Connection state, used only in connection oriented mode
typedef enum {
  DISCOVER_SERVICES,
  DISCOVER_CHARACTERISTICS,
  ENABLE_CTE,
  RUNNING
} aoa_db_state_t;

typedef struct {
  uint16_t handle;                  // Connection handle or sync handle in connectionless mode
  bd_addr address;
  uint8_t address_type;
  uint32_t cte_service_handle;      // Connection only
  uint16_t cte_enable_char_handle;  // Connection only
  aoa_db_state_t connection_state;  // Connection only
  int32_t sequence;                 // RTL lib only
  void *user_data;
} aoa_db_entry_t;

// -----------------------------------------------------------------------------
// Function Declarations

/**************************************************************************//**
 * Add a tag to the database list.
 *
 * @param[in] handle Connection or sync handle.
 * @param[in] address Bluetooth address in reverse byte order.
 * @param[in] address_type Address type.
 * @param[out] tag Pointer to the created tag properties structure.
 *
 * @retval SL_STATUS_ALLOCATION_FAILED - Memory allocation failure.
 * @retval SL_STATUS_OK - Tag added.
 *****************************************************************************/
sl_status_t aoa_db_add_tag(uint16_t handle,
                           bd_addr *address,
                           uint8_t address_type,
                           aoa_db_entry_t** tag);

/**************************************************************************//**
 * Removes a tag from the database list.
 *
 * @param[in] handle Connection or sync handle.
 *
 * @retval SL_STATUS_EMPTY - No tag in the list.
 * @retval SL_STATUS_OK - Tag removed.
 * @retval SL_STATUS_NOT_FOUND - Tag is not on the list.
 *****************************************************************************/
sl_status_t aoa_db_remove_tag(uint16_t handle);

/**************************************************************************//**
 * Returns a tag from the tag list by its handle.
 *
 * @param[in] handle Connection or sync handle.
 * @param[out] tag Pointer to tag structure.
 *
 * @retval SL_STATUS_OK - Tag found.
 * @retval SL_STATUS_NOT_FOUND - Tag is not on the list.
 *****************************************************************************/
sl_status_t aoa_db_get_tag_by_handle(uint16_t handle,
                                     aoa_db_entry_t **tag);

/**************************************************************************//**
 * Returns a tag from the tag list by its handle.
 *
 * @param[in] index index of the tag.
 * @param[out] tag Pointer to tag structure.
 *
 * @retval SL_STATUS_OK - Tag found.
 * @retval SL_STATUS_NOT_FOUND - Tag is not on the list.
 *****************************************************************************/
sl_status_t aoa_db_get_tag_by_index(uint32_t index,
                                    aoa_db_entry_t **tag);

/**************************************************************************//**
 * Returns a tag from the tag list by its address.
 *
 * @param[in] address Bluetooth address in reverse byte order.
 * @param[out] tag Pointer to tag structure.
 *
 * @retval SL_STATUS_EMPTY - No tag in the list.
 * @retval SL_STATUS_OK - Tag found.
 * @retval SL_STATUS_NOT_FOUND - Tag is not on the list.
 *****************************************************************************/
sl_status_t aoa_db_get_tag_by_address(bd_addr* address,
                                      aoa_db_entry_t **tag);

/**************************************************************************//**
 * Returns the number of tags on the list.
 *
 * @return Number of tags in the list.
 *****************************************************************************/
size_t aoa_db_get_number_of_tags(void);

/**************************************************************************//**
 * Adds an address to the allowlist.
 *
 * @param[in] address Address to be added to the allowlist.
 *
 * @retval SL_STATUS_ALREADY_EXISTS - Address already on the list.
 * @retval SL_STATUS_ALLOCATION_FAILED - Allocation error.
 * @retval SL_STATUS_OK - Address added to the allowlist.
 *****************************************************************************/
sl_status_t aoa_db_allowlist_add(uint8_t address[ADR_LEN]);

/**************************************************************************//**
 * Removes all tags from the allowlist.
 *****************************************************************************/
void aoa_db_allowlist_reset(void);

/**************************************************************************//**
 * Removes an address from the allowlist.
 *
 * @param[in] address Address to be removed from the allowlist.
 *
 * @retval SL_STATUS_EMPTY - Allowlist is empty.
 * @retval SL_STATUS_NOT_FOUND - Address is not on the allowlist.
 * @retval SL_STATUS_OK - Address removed from the allowlist.
 *****************************************************************************/
sl_status_t aoa_db_allowlist_remove(uint8_t address[ADR_LEN]);

/**************************************************************************//**
 * Finds an address on the allowlist.
 *
 * @param[in] address Address to be found in the allowlist.
 *
 * @retval SL_STATUS_NOT_FOUND - Address is not on the allowlist.
 * @retval SL_STATUS_EMPTY - Allowlist is empty.
 * @retval SL_STATUS_OK - Address is on the allowlist.
 *****************************************************************************/
sl_status_t aoa_db_allowlist_find(uint8_t address[ADR_LEN]);

/**************************************************************************//**
 * Returns the number of tags on the allow list.
 *
 * @retval Number of tags on the allowlist.
 *****************************************************************************/
size_t aoa_db_allowlist_get_size(void);

/**************************************************************************//**
 * Deletes every tags.
 *****************************************************************************/
void aoa_db_remove_all(void);

/**************************************************************************//**
 * Tag added callback.
 * @note To be implemented in user code.
 *
 * @param[in] tag Pointer to tag.
 *****************************************************************************/
void aoa_db_on_tag_added(aoa_db_entry_t *tag);

/**************************************************************************//**
 * Tag removed callback.
 * @note To be implemented in user code.
 *
 * @param[in] tag Pointer to tag.
 *****************************************************************************/
void aoa_db_on_tag_removed(aoa_db_entry_t *tag);

#ifdef __cplusplus
};
#endif

#endif /* AOA_DB_H */
