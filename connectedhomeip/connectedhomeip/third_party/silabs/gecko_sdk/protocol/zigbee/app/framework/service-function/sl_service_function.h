/***************************************************************************//**
 * @file sl_service_function.h
 * @brief Header for the service function module.
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

#ifndef __SL_SERVICE_FUNCTION__
#define __SL_SERVICE_FUNCTION__ 1
#include <stdint.h>
#include <stdbool.h>

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
/** @brief Helpful macro for the extension points, so people
 * can say: mfg_id: "NOT_MFG_SPECIFIC" instead of a magic 0xFFFF
 */
#define NOT_MFG_SPECIFIC 0xFFFF
#define SL_CLUSTER_SERVICE_SIDE_SERVER ZCL_DIRECTION_CLIENT_TO_SERVER
#define SL_CLUSTER_SERVICE_SIDE_CLIENT ZCL_DIRECTION_SERVER_TO_CLIENT
#endif // DOXYGEN_SHOULD_SKIP_THIS

// ========== Types.

/** @brief Operation code: provide the context in which the service function is
 * being called.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum sl_service_opcode_t
#else
typedef uint8_t sl_service_opcode_t;
enum
#endif
{
  SL_SERVICE_FUNCTION_TYPE_ZCL_COMMAND = 0x00,
};

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
/** @brief Key: each service function is hashed by a pair of these.
 * They should be clusterId/manufactureId.
 */
typedef uint32_t sl_service_key_t;
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @brief Context that is passed to service function. Depends on actual op code
 * which actual part is used.
 */
typedef union _sl_service_function_context_t {
  void *data;
} sl_service_function_context_t;

/** @brief Service function signature
 */
typedef uint32_t (*sl_service_function_t)(sl_service_opcode_t, sl_service_function_context_t*);

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
/** @brief Single service function entry.
 */
typedef struct {
  sl_service_opcode_t type;
  sl_service_key_t key;
  sl_service_key_t subkey;
  sl_service_function_t function;
} sl_service_function_entry_t;

/** @brief Struct that contains a block of entries and a possible non-NULL
 * pointer to the next block of entries.
 */
typedef struct _sl_service_function_block_t {
  uint16_t count;
  sl_service_function_entry_t *entries;
  struct _sl_service_function_block_t *next;
} sl_service_function_block_t;

/** @brief * Helpful macro to create a "function_block" out of an array of
 *  entries. Feel free to use it, but make sure you don't pass in
 *  the actual raw init block.
 *  Registration of individual block of service function.
 *  Note that the block should be allocated and owned by the caller.
 *  This function does NOT copy the contents anywhere, and simply inserts
 *  the block into the linked list. The caller needst to ensure that
 *  the memory allocation of block is safe, and not allocated on the stack
 *  or some other location that might get corrupt.
 *
 * @param block pointer to struct that contains block of entries
 */
void sl_service_function_register_block(sl_service_function_block_t *block);

/** @brief Returns the number of entries in the service registry.
 *
 * @return uint16_t number of entries in service registry
 */
uint16_t sl_service_function_entry_count(void);

/** @brief Returns a pointer to the first entry if it exists, NULL otherwise.
 *
 * @return sl_service_function_entry_t * pointer to the first entry if exists
 */
sl_service_function_entry_t *sl_service_function_get_first_entry(void);

/** @brief Returns a pointer to the entry after the passed one if it exists, NULL otherwise.
 *
 * @return sl_service_function_entry_t * pointer to the entry after the passed one if it exists
 */
sl_service_function_entry_t *sl_service_function_get_next_entry(sl_service_function_entry_t *current_entry);
#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // __SL_SERVICE_FUNCTION__
