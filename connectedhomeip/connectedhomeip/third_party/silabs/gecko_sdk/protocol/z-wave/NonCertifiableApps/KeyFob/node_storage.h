/***************************************************************************//**
 * @file node_storage.h
 * @brief node_storage.h
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef NODE_STORAGE_H
#define NODE_STORAGE_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <ZW_typedefs.h>
#include <zpal_radio.h>
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

void node_storage_set_default_group(void);

void node_storage_init_group();

uint8_t node_storage_group_member_count();

void node_storage_add_group_member_nodeid(node_id_t member_nodeid_to_add);

void node_storage_remove_group_member_nodeid(node_id_t member_nodeid_to_remove);

bool node_storage_is_nodeid_group_member(node_id_t member_nodeid);

uint8_t node_storage_group_member_list_get(node_id_t* member_nodeid_list);

size_t node_storage_group_member_nodemask_get(uint8_t* member_nodeid_nodemask, size_t length);

#endif  // NODE_STORAGE_H
