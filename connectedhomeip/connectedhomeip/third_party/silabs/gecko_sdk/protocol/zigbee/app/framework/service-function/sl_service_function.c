/***************************************************************************//**
 * @file sl_service_function.c
 * @brief Service function is an entry point. This file provides functions for
 * the API to manage and handle service functions.
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stddef.h>
#include "sl_service_function.h"

#ifndef SL_CLUSTER_SERVICE_TEST
#include "sl_cluster_service_gen.h"
#endif

/**
 * Registry of service function is made of blocks. Each block can be
 * any numbers of functions, but then it also has a next pointer
 * pointing to the next block, or NULL if there is no next block.
 * The memory allocation for the blocks is owned by the caller, so
 * the caller should make sure the allocate the function pointers
 * not on the stack, but on the heap, globally.
 *
 * Initial block is populated from the pregenerated entry if some exist
 * otherwise it remains empty.
 */
#if SL_CLUSTER_SERVICE_ENTRY_COUNT == 0
// There are no initial contributions, so we just start with a NULL pointer.

static sl_service_function_block_t *registry = NULL;

#else
// We have initial contributions generated for us. We lay them out into
// the initial block and make registry point to it.

static sl_service_function_block_t initBlock[] = {
  SL_CLUSTER_SERVICE_ENTRY_COUNT,
  (sl_service_function_entry_t *)sli_cluster_service_entries,
  NULL
};
static sl_service_function_block_t *registry = initBlock;

#endif

/**
 * Initializes the static global registry with the given entries.
 */
void sl_service_function_register_block(sl_service_function_block_t *block)
{
  block->next = registry;
  registry = block;
}

/**
 * Returns the number of entries.
 */
uint16_t sl_service_function_entry_count(void)
{
  uint16_t count = 0;
  sl_service_function_block_t *r = registry;
  while (r != NULL) {
    count += r->count;
    r = r->next;
  }
  return count;
}

sl_service_function_entry_t *sl_service_function_get_first_entry(void)
{
  if (registry == NULL) {
    return NULL;
  } else {
    return registry->entries;
  }
}

sl_service_function_entry_t *sl_service_function_get_next_entry(sl_service_function_entry_t *current_entry)
{
  sl_service_function_block_t * r = registry;
  bool current_entry_found = false;

  while (r != NULL) {
    for (uint16_t index_in_block = 0; index_in_block < r->count; index_in_block++) {
      sl_service_function_entry_t *entry = r->entries + index_in_block;
      if (current_entry_found) {
        return entry;
      } else {
        if (current_entry == entry) {
          current_entry_found = true;
        }
      }
    }
    r = r->next;
  }

  return NULL;
}
