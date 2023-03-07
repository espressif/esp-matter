/***************************************************************************//**
 * @file
 * @brief Tag database handler module, responsible for storing states of tags.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "aoa_db.h"
#include "sl_common.h"

// -----------------------------------------------------------------------------
// Type definitions.

// Forward declarations
typedef struct aoa_db_node aoa_db_node_t;
typedef struct aoa_db_allow_node aoa_db_allow_node_t;

struct aoa_db_node{
  aoa_db_entry_t entry;
  aoa_db_node_t *next;
};

struct aoa_db_allow_node{
  uint8_t address[ADR_LEN];
  aoa_db_allow_node_t *next;
};

// -----------------------------------------------------------------------------
// Module variables.

// Linked list heads
static aoa_db_node_t *head_conn = NULL;
static aoa_db_allow_node_t *head_allow = NULL;

// -----------------------------------------------------------------------------
// Public function definitions.

/**************************************************************************//**
 * Add a tag to the database list.
 *****************************************************************************/
sl_status_t aoa_db_add_tag(uint16_t handle,
                           bd_addr *address,
                           uint8_t address_type,
                           aoa_db_entry_t **tag)
{
  aoa_db_node_t *new = (aoa_db_node_t *)malloc(sizeof(aoa_db_node_t));
  if (NULL == new) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // Store the connection handle, and the server address
  new->entry.handle = handle;
  new->entry.address = *address;
  new->entry.address_type = address_type;
  new->entry.connection_state = DISCOVER_SERVICES;
  new->entry.sequence = -1;
  new->next = head_conn;
  head_conn = new;
  *tag = &(new->entry);

  aoa_db_on_tag_added(*tag);

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Removes a tag from the database list.
 *****************************************************************************/
sl_status_t aoa_db_remove_tag(uint16_t handle)
{
  aoa_db_node_t *current = head_conn;
  aoa_db_node_t *previous = NULL;

  if (head_conn == NULL) {
    return SL_STATUS_EMPTY;
  }

  while (NULL != current) {
    if (current->entry.handle == handle) {
      if (previous != NULL) {
        previous->next = current->next;
      } else {
        head_conn = current->next;
      }
      aoa_db_on_tag_removed(&current->entry);
      free(current);
      return SL_STATUS_OK;
    }
    previous = current;
    current = current->next;
  }

  return SL_STATUS_NOT_FOUND;
}

/**************************************************************************//**
 * Returns a tag from the tag list by its handle.
 *****************************************************************************/
sl_status_t aoa_db_get_tag_by_handle(uint16_t handle,
                                     aoa_db_entry_t **tag)
{
  aoa_db_node_t *current = head_conn;

  if (NULL == head_conn) {
    return SL_STATUS_NOT_FOUND;
  }

  while (NULL != current) {
    if (current->entry.handle == handle) {
      *tag = &(current->entry);
      return SL_STATUS_OK;
    }
    current = current->next;
  }

  return SL_STATUS_NOT_FOUND;
}

/**************************************************************************//**
 * Returns a tag from the tag list by its index.
 *****************************************************************************/
sl_status_t aoa_db_get_tag_by_index(uint32_t index,
                                    aoa_db_entry_t **tag)
{
  aoa_db_node_t *current = head_conn;
  uint32_t i = 0;

  if (NULL == head_conn) {
    return SL_STATUS_NOT_FOUND;
  }

  while (NULL != current) {
    if (index == i) {
      *tag = &(current->entry);
      return SL_STATUS_OK;
    }
    current = current->next;
    i++;
  }

  return SL_STATUS_NOT_FOUND;
}

/**************************************************************************//**
 * Returns a tag from the tag list by its address.
 *****************************************************************************/
sl_status_t aoa_db_get_tag_by_address(bd_addr *address,
                                      aoa_db_entry_t **entry)
{
  aoa_db_node_t *current = head_conn;

  if (NULL == head_conn) {
    return SL_STATUS_NOT_FOUND;
  }

  while (NULL != current) {
    if (0 == memcmp(address, &(current->entry.address), sizeof(bd_addr))) {
      *entry = &(current->entry);
      return SL_STATUS_OK;
    }
    current = current->next;
  }

  return SL_STATUS_NOT_FOUND;
}

/**************************************************************************//**
 * Returns the number of tags on the list.
 *****************************************************************************/
size_t aoa_db_get_number_of_tags(void)
{
  size_t i = 0;
  aoa_db_node_t *current = head_conn;

  while (NULL != current) {
    i++;
    current = current->next;
  }

  return i;
}

/**************************************************************************//**
 * Adds an address to the allowlist.
 *****************************************************************************/
sl_status_t aoa_db_allowlist_add(uint8_t address[ADR_LEN])
{
  aoa_db_allow_node_t *new = NULL;

  if (SL_STATUS_OK == aoa_db_allowlist_find(address)) {
    return SL_STATUS_ALREADY_EXISTS;
  } else {
    new = (aoa_db_allow_node_t *)malloc(sizeof(aoa_db_allow_node_t));
    if (NULL == new) {
      return SL_STATUS_ALLOCATION_FAILED;
    }
    for (uint32_t i = 0; i < ADR_LEN; i++) {
      new->address[i] = address[i];
    }
  }
  new->next = head_allow;
  head_allow = new;

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Removes all tags from the allowlist.
 *****************************************************************************/
void aoa_db_allowlist_reset(void)
{
  aoa_db_allow_node_t *current;
  aoa_db_allow_node_t *next;

  for (current = head_allow; current != NULL; current = next) {
    next = current->next;
    free(current);
  }

  head_allow = NULL;
}

/**************************************************************************//**
 * Removes an address from the allowlist.
 *****************************************************************************/
sl_status_t aoa_db_allowlist_remove(uint8_t address[ADR_LEN])
{
  aoa_db_allow_node_t *current = head_allow;
  aoa_db_allow_node_t *previous = NULL;

  if (head_allow == NULL) {
    return SL_STATUS_EMPTY;
  }

  if (0 == memcmp(head_allow->address, address, ADR_LEN)) {
    head_allow = head_allow->next;
    free(current);
    return SL_STATUS_OK;
  }

  while (NULL != current) {
    if (memcmp(current->address, address, ADR_LEN) == 0) {
      previous->next = current->next;
      free(current);
      return SL_STATUS_OK;
    }
    previous = current;
    current = current->next;
  }

  return SL_STATUS_NOT_FOUND;
}

/**************************************************************************//**
 * Finds an address on the allowlist
 *****************************************************************************/
sl_status_t aoa_db_allowlist_find(uint8_t address[ADR_LEN])
{
  aoa_db_allow_node_t *current = head_allow;

  if (NULL == head_allow) {
    // Allowlist is empty, every tag is allowed
    return SL_STATUS_EMPTY;
  }

  while (NULL != current) {
    if (0 == memcmp(address, current->address, ADR_LEN)) {
      return SL_STATUS_OK;
    }
    current = current->next;
  }

  return SL_STATUS_NOT_FOUND;
}

/**************************************************************************//**
 * Returns the number of tags on the allow list.
 *****************************************************************************/
size_t aoa_db_allowlist_get_size(void)
{
  size_t i = 0;
  aoa_db_allow_node_t *current = head_allow;

  while (NULL != current) {
    i++;
    current = current->next;
  }

  return i;
}

/**************************************************************************//**
 * Deletes every tags.
 *****************************************************************************/
void aoa_db_remove_all(void)
{
  aoa_db_node_t *current;
  aoa_db_node_t *next;

  for (current = head_conn; current != NULL; current = next) {
    next = current->next;
    aoa_db_on_tag_removed(&current->entry);
    free(current);
  }

  head_conn = NULL;
}

/**************************************************************************//**
 * Weak implementation of tag added callback.
 *****************************************************************************/
SL_WEAK void aoa_db_on_tag_added(aoa_db_entry_t *tag)
{
  // Implement this in the application.
}

/**************************************************************************//**
 * Weak implementation of tag removed callback.
 *****************************************************************************/
SL_WEAK void aoa_db_on_tag_removed(aoa_db_entry_t *tag)
{
  // Implement this in the application.
}
