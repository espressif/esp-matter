/***************************************************************************//**
 * @file mac-child.h
 *
 * @brief MAC layer child interface
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef MAC_CHILD_H
#define MAC_CHILD_H

#include "sl_status.h"
#include "mac-types.h"
#include "buffer_manager/buffer-management.h"

#define SL_MAC_CHILD_IS_PRESENT                             0x01
#define SL_MAC_CHILD_IS_RX_ON_WHEN_IDLE                     0x02 // Set by stack
#define SL_MAC_CHILD_HAS_PENDING_SHORT_INDIRECT_MESSAGE     0x04
#define SL_MAC_CHILD_HAS_PENDING_LONG_INDIRECT_MESSAGE      0x08
#define SL_MAC_CHILD_EXPECTING_JIT_MESSAGE                  0x10 // Set by stack
#define SL_MAC_CHILD_HAS_SECURE_DATA_REQUESTS               0x20 // Set by stack
#define SL_MAC_CHILD_INVALID_INDEX                          0xFF
// Pick out the flags that indicate that a MAC indirect message is pending.
#define SL_MAC_CHILD_HAS_PENDING_MESSAGE           \
  (SL_MAC_CHILD_HAS_PENDING_SHORT_INDIRECT_MESSAGE \
   | SL_MAC_CHILD_HAS_PENDING_LONG_INDIRECT_MESSAGE)

typedef uint64_t sl_mac_child_status_flags_t;
#define SL_MAC_MAX_CHILD_TABLE_SIZE 64

typedef struct {
  uint8_t mac_index;
  EmberNodeId short_id;
  EmberEUI64 long_id;
  sl_mac_child_status_flags_t info;
} sl_mac_child_entry_t;

extern uint8_t sl_mac_child_table_size;

sl_status_t sl_mac_child_table_init(sl_mac_child_entry_t *childTable,
                                    uint16_t *childStatus,
                                    uint8_t childTableSize);

sl_status_t sl_mac_add_child(uint8_t macIndex,
                             uint8_t childIndex,
                             EmberNodeId shortId,
                             EmberEUI64 longId,
                             sl_mac_child_status_flags_t info);

sl_status_t sl_mac_remove_child(uint8_t childIndex);

sl_status_t sl_mac_update_child(uint8_t macIndex,
                                uint8_t childIndex,
                                EmberNodeId shortId,
                                EmberEUI64 longId,
                                sl_mac_child_status_flags_t info);

EmberNodeId sl_mac_child_short_id(uint8_t childIndex);
uint8_t *sl_mac_child_long_id(uint8_t childIndex);

EmberNodeId sl_mac_find_child_short_id(EmberEUI64 longId);
uint8_t sl_mac_child_find_long_index(uint8_t startIndex, EmberEUI64 longId);
#define sl_mac_child_find_free_index(startIndex) \
  (sl_mac_child_find_long_index((startIndex), NULL))

bool sl_mac_child_lookup_eui64_by_id(EmberNodeId id, EmberEUI64 eui64);

// Legacy APIs emberChildIndex / emberChildLongIndex will be replaced by these
// MAC layer functions
uint8_t sl_mac_child_index(EmberNodeId shortId);
uint8_t sl_mac_child_long_index(EmberEUI64 longId);

sl_status_t sl_mac_set_child_id(uint8_t childIndex, EmberNodeId nodeId);
sl_status_t sl_mac_set_child_long_id(uint8_t childIndex, EmberEUI64 longId);

uint8_t sl_mac_header_outgoing_child_index(PacketHeader header);
uint8_t sl_mac_header_incoming_child_index(PacketHeader header);

bool sl_mac_child_has_pending_message(uint8_t childIndex);
bool sl_mac_child_expecting_jit_message(uint8_t childIndex);

uint8_t sl_mac_get_child_mac_index(uint8_t childIndex);
bool sl_mac_set_child_mac_index(uint8_t childIndex, uint8_t macIndex);

sl_mac_child_status_flags_t sl_mac_get_child_info_flags(uint8_t childIndex);

// Set or clear 'mask' bits for specific children.
sl_status_t sl_mac_set_child_flag(uint8_t childIndex, sl_mac_child_status_flags_t flags, bool set);

// Set or clear 'mask' bits for all sleepy children.
sl_status_t sl_mac_set_sleepy_children_flags(sl_mac_child_status_flags_t mask, bool set);

// True if any child has all of the given flags set.
bool sl_mac_check_any_child_flags(sl_mac_child_status_flags_t flags);

#define sl_mac_have_pending_jit_messages() \
  (sl_mac_check_any_child_flags(SL_MAC_CHILD_EXPECTING_JIT_MESSAGE))

#define sl_mac_have_child(childIndex) \
  (sl_mac_get_child_info_flags(childIndex) & SL_MAC_CHILD_IS_PRESENT)

#define sl_mac_child_is_sleepy(childIndex) \
  (sl_mac_have_child(childIndex)           \
   && !(sl_mac_get_child_info_flags(childIndex) & SL_MAC_CHILD_IS_RX_ON_WHEN_IDLE))

bool sl_mac_child_id_is_sleepy(EmberNodeId id);
bool sl_mac_child_id_is_valid(EmberNodeId id);

extern sl_mac_child_entry_t *sl_mac_child_table;
extern uint16_t *sl_mac_child_status;
#define sl_mac_change_child_table(childTable, childStatus) \
  do {                                                     \
    sl_mac_child_table = (childTable);                     \
    sl_mac_child_status = (childStatus);                   \
  } while (0)
#endif // MAC_CHILD_H
