/***************************************************************************//**
 * @file node_storage.c
 * @brief node_storage.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stddef.h>
#include "node_storage.h"
#define DEBUGPRINT
#include "DebugPrint.h"
#include <NodeMask.h>
#include <ZAF_file_ids.h>
#include <Assert.h>
#include <string.h>
#include <zpal_nvm.h>
#include "ZAF_nvm_app.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define KEY_FOB_NODES_HEAD (0)

typedef struct
{
  uint8_t members;
  NODE_MASK_TYPE membermask;
} NODE_GROUP;

#define MAX_NODE_ID (sizeof(NODE_MASK_TYPE) * 8)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static bool write_data_to_nvm(zpal_nvm_object_key_t file_id, void* data, size_t size);

static bool read_data_from_nvm(zpal_nvm_object_key_t file_id, void* data, size_t size);

static void writeNodeGroupCache(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static zpal_nvm_handle_t pFileSystemApplication;

static NODE_GROUP nodeGroupCache;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

// Initialises the nvm file system, and read BASIC Set Association group setting
void node_storage_init_group()
{
  // Init the NVM filesystem
  ApplicationFileSystemInit(&pFileSystemApplication);
  if (NULL == pFileSystemApplication)
  {
    pFileSystemApplication = ZAF_GetFileSystemHandle();
  }

  size_t   dataLen;
  if (ZPAL_STATUS_OK != zpal_nvm_get_object_size(pFileSystemApplication, ZAF_FILE_ID_KEY_FOB_NODES, &dataLen))
  {
    // if nodeGroup file not found initialize it with an empty group
    node_storage_set_default_group();
  }
  // read group setting into nodeGroupCache
  read_data_from_nvm(ZAF_FILE_ID_KEY_FOB_NODES, (void*)&nodeGroupCache, sizeof(nodeGroupCache));
  if (nodeGroupCache.members > (MAX_NODE_ID - 1))
  {
    // Invalid number of members, only (MAX_NODE_ID - 1) allowed - reinitialize
    node_storage_set_default_group();
  }
}


// Add nodeID to Basic Set Association Group
void node_storage_add_group_member_nodeid(node_id_t member_nodeid_to_add)
{
  if ((0 < member_nodeid_to_add) && (MAX_NODE_ID >= member_nodeid_to_add) && (false == node_storage_is_nodeid_group_member(member_nodeid_to_add)))
  {
    ZW_NodeMaskSetBit(nodeGroupCache.membermask, (uint8_t)member_nodeid_to_add);
    nodeGroupCache.members++;
    writeNodeGroupCache();
  }
}


// Remove nodeID from Basic Set Association Group
void node_storage_remove_group_member_nodeid(node_id_t member_nodeid_to_remove)
{
  if ((0 < member_nodeid_to_remove) && (MAX_NODE_ID >= member_nodeid_to_remove) && (true == node_storage_is_nodeid_group_member(member_nodeid_to_remove)))
  {
    ZW_NodeMaskClearBit(nodeGroupCache.membermask, (uint8_t)member_nodeid_to_remove);
    if (0 != nodeGroupCache.members)
    {
      nodeGroupCache.members--;
    }
    writeNodeGroupCache();
  }

}


bool node_storage_is_nodeid_group_member(node_id_t member_nodeid)
{
  return ((0 < member_nodeid) && (MAX_NODE_ID >= member_nodeid) &&  (true == ZW_NodeMaskNodeIn(nodeGroupCache.membermask, (uint8_t)member_nodeid)));
}


uint8_t node_storage_group_member_list_get(node_id_t* member_nodeid_list)
{
  uint8_t numberOfMembersInList = 0;
  uint8_t currentNode = 0;
  while (nodeGroupCache.members > numberOfMembersInList)
  {
    currentNode = ZW_NodeMaskGetNextNode(currentNode, nodeGroupCache.membermask);
    if ((0 == currentNode) && (MAX_NODE_ID < currentNode))
    {
      break;
    }
    member_nodeid_list[numberOfMembersInList] = (node_id_t)currentNode;
    numberOfMembersInList++;
  }
  return numberOfMembersInList;
}


size_t node_storage_group_member_nodemask_get(uint8_t* member_nodeid_nodemask, size_t length)
{
  memcpy((void*)member_nodeid_nodemask, nodeGroupCache.membermask, length);
  DPRINT("Node mask: 0x");
  for (unsigned int i = 0 ; i < sizeof(nodeGroupCache.membermask) ; i++)
  {
    DPRINTF("%X ", member_nodeid_nodemask[i]);
  }
  DPRINT("\n");
  return (size_t)nodeGroupCache.members;
}

uint8_t node_storage_group_member_count()
{
  return nodeGroupCache.members;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static bool write_data_to_nvm(zpal_nvm_object_key_t file_id, void* data, size_t size)
{
  zpal_status_t nvm_return_value = ZPAL_STATUS_FAIL;

  if ((pFileSystemApplication != NULL) && (data != NULL) && (size > 0))
  {
    nvm_return_value = zpal_nvm_write( pFileSystemApplication,
                                        file_id,
                                        data,
                                        size);
  }
  return (nvm_return_value == ZPAL_STATUS_OK) ? true : false;
}


static bool read_data_from_nvm(zpal_nvm_object_key_t file_id, void* data, size_t size)
{
  zpal_status_t nvm_return_value = ZPAL_STATUS_FAIL;

  if ((pFileSystemApplication != NULL) && (data != NULL) && (size > 0))
  {
    nvm_return_value = zpal_nvm_read(pFileSystemApplication,
                                     file_id,
                                     data,
                                     size);
  }
  return (nvm_return_value == ZPAL_STATUS_OK) ? true : false;
}


static void writeNodeGroupCache(void)
{
  bool retval = write_data_to_nvm(ZAF_FILE_ID_KEY_FOB_NODES, (void*)&nodeGroupCache, sizeof(nodeGroupCache));
  if(false == retval)
  {
    ASSERT(0);
  }
}


void node_storage_set_default_group(void)
{
  ZW_NodeMaskClear(nodeGroupCache.membermask, sizeof(nodeGroupCache.membermask));
  nodeGroupCache.members = 0;
  writeNodeGroupCache();
}
