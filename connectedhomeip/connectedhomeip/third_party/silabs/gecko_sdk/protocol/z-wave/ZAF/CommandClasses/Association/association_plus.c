/**
 * @file
 * Helper module for Command Class Association and Command Class Multi Channel Association.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <association_plus.h>
#include <stdbool.h>
#include "SizeOf.h"
#include "Assert.h"
#include <ZW_TransportLayer.h>
#include <CC_Association.h>
#include <CC_AssociationGroupInfo.h>
#include <string.h>
//#define DEBUGPRINT
#include "DebugPrint.h"
#include "ZAF_Common_interface.h"
#include <ZAF_file_ids.h>
#include <ZAF_nvm_app.h>

/****************************************************************************/
/*                             PARAMETER CHECK                              */
/****************************************************************************/

/**
 * The sum of the three values defined as:
 *
 *  MAX_ASSOCIATION_GROUPS,
 *  MAX_ASSOCIATION_IN_GROUP and
 *  NUMBER_OF_ENDPOINTS
 *
 * is a great indication as to how large the generated structures become
 * in memory and hence this max sum puts a limit on how large these
 * structures can become in memory.
 */
#ifndef ASSOCIATION_ALLOCATION_MAX // This may be defined by the build system
#define ASSOCIATION_ALLOCATION_MAX            18
#endif

#if ((MAX_ASSOCIATION_GROUPS + MAX_ASSOCIATION_IN_GROUP + NUMBER_OF_ENDPOINTS) > ASSOCIATION_ALLOCATION_MAX)
  #error "FATAL: The sum of MAX_ASSOCIATION_GROUPS, MAX_ASSOCIATION_IN_GROUP and NUMBER_OF_ENDPOINTS, may not exceed the value ASSOCIATION_ALLOCATION_MAX !!"
#endif  // ASSOCIATION_ALLOCATION_MAX check

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef enum _NVM_ACTION_
{
  NVM_ACTION_INIT_FORCE_CLEAR_MEM,         // This will reinitialize a whole new table for storing associations and clear the old if it exists.
  NVM_ACTION_INIT_CORRECT_INVALID_NODEID,  // Fetches and checks the stored associations and corrects invalid nodeIDs that were stored.
  NVM_ACTION_READ_DATA,
  NVM_ACTION_WRITE_DATA,
}
NVM_ACTION;

typedef struct _ASSOCIATION_NODE_LIST_
{
  uint8_t *                     pNodeId;           /*IN pointer to list of nodes*/
  uint8_t                       noOfNodes;         /*IN number of nodes in List*/
  MULTICHAN_DEST_NODE_ID_8bit * pMulChanNodeId;    /*IN pointer to list of multi channel nodes*/
  uint8_t                       noOfMulchanNodes;  /*IN number of  multi channel nodes in List*/
}
ASSOCIATION_NODE_LIST;

#define OFFSET_CLASSCMD                       0x00
#define OFFSET_CMD                            0x01
#define OFFSET_PARAM_1                        0x02
#define OFFSET_PARAM_2                        0x03
#define OFFSET_PARAM_3                        0x04
#define OFFSET_PARAM_4                        0x05

/*
 * These define the valid endpoint addresses that can be bit-addressed.
 * Both values are included in the range.
 */
#define ENDPOINT_VALUE_VALID_MAX              7
#define ENDPOINT_VALUE_VALID_MIN              1
#define IS_BIT_ADDRESSING_ENDPOINT(endpoint)  ((endpoint >= ENDPOINT_VALUE_VALID_MIN) && (endpoint <= ENDPOINT_VALUE_VALID_MAX))

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/* Default values */

static ASSOCIATION_GROUP groups[NUMBER_OF_ENDPOINTS + 1][MAX_ASSOCIATION_GROUPS];

uint8_t numberOfGroupMappingEntries = 0;

static zpal_nvm_handle_t pFileSystem;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

static bool isGroupIdValid(uint8_t groupId, uint8_t endpoint);
static void ExtractCmdClassNodeList(
    ASSOCIATION_NODE_LIST* plist,
    ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME* pCmd,
    uint8_t cmdLength,
    uint8_t commandClass);
static bool AssGroupMappingLookUp(uint8_t* pEndpoint, uint8_t* pGroupID);
static void AssociationStoreAll(void);
static void NVM_Action(NVM_ACTION action);
static void RemoveAssociationsFromGroup(
    uint8_t cmdClass,
    uint8_t ep,
    uint8_t groupId,
    ASSOCIATION_NODE_LIST * pListOfNodes);

static inline bool IsFree(destination_info_t * pNode)
{
  return (FREE_VALUE == pNode->node.nodeId);
}

static inline void Free(destination_info_t * pNode)
{
  pNode->node.nodeId = FREE_VALUE;
}

static inline bool HasEndpoint(destination_info_t * pNode)
{
  return (1 == pNode->nodeInfo.BitMultiChannelEncap);
}

/**
 * Returns a pointer to the node in association table.
 * @param endpoint Endpoint to which the association was made.
 * @param groupID Group in which the association was made.
 * @param index Index of the node in the group.
 * @return
 */
static inline destination_info_t * GetNode(uint8_t endpoint, uint8_t groupID, uint8_t index)
{
  return &groups[endpoint][groupID - 1].subGrp[index];
}

/**
 * @brief Reorders nodes in a given group.
 * @param groupIden Given group ID.
 * @param ep Given endpoint.
 * @param emptyIndx Location in index which must be filled up.
 */
static void
ReorderGroupAfterRemove(
  uint8_t groupIden,
  uint8_t ep,
  uint8_t emptyIndx)
{
  uint8_t move;

  destination_info_t * pNodeToMove;
  destination_info_t * pNode;
  const uint32_t iArraySize = sizeof_array(groups[ep][groupIden].subGrp);
  for(move = emptyIndx; move < (iArraySize - 1); move++)
  {
    pNode       = GetNode(ep, groupIden + 1, move);
    pNodeToMove = GetNode(ep, groupIden + 1, move + 1);
    if (IsFree(pNodeToMove)) break;
    *pNode = *pNodeToMove;
  }

  Free(&groups[ep][groupIden].subGrp[move]);
}

NODE_LIST_STATUS
handleAssociationGetnodeList(
  uint8_t groupId,
  uint8_t ep,
  destination_info_t ** ppList,
  uint8_t* pListLen)
{
  if((NUMBER_OF_ENDPOINTS + 1) < ep )
  {
    return NODE_LIST_STATUS_ERR_ENDPOINT_OUT_OF_RANGE;
  }

  if (NULL == ppList)
  {
    return NODE_LIST_STATUS_ERROR_LIST;
  }

  if (NULL == pListLen)
  {
    return NODE_LIST_STATUS_ERROR_LIST;
  }

  /*Check group number*/
  if (false == isGroupIdValid(groupId, ep))
  {
    return NODE_LIST_STATUS_ERR_GROUP_NBR_NOT_LEGAL; /*not legal number*/
  }

  // Find the rootGroupID and endpoint of this groupID
  AssGroupMappingLookUp(&ep, &groupId);

  *ppList = GetNode(ep, groupId, 0); // Get a pointer to the first node
  *pListLen = MAX_ASSOCIATION_IN_GROUP; /*default set to max*/

  for (uint8_t indx = 0; indx < MAX_ASSOCIATION_IN_GROUP; indx++)
  {
    if (IsFree(*ppList + indx))
    {
      *pListLen = indx; /*number of nodes in list*/
      break;  /* break out of loop*/
    }
    DPRINTF("\n nodeId = %u, endpoint = %u, BitAddress = %u",
        (*ppList + indx)->node.nodeId, (*ppList + indx)->node.endpoint, (*ppList + indx)->node.BitAddress);
  }
  if(0 == *pListLen)
  {
    return NODE_LIST_STATUS_ASSOCIATION_LIST_EMPTY;
  }
  return NODE_LIST_STATUS_SUCCESS;
}

/**
 * Returns whether a given group ID is valid or not.
 * @param groupId The group ID to check.
 * @param endpoint The endpoint to which the group belongs.
 * @return true if group ID is valid, false otherwise.
 */
static bool isGroupIdValid(uint8_t groupId, uint8_t endpoint)
{
  if ((NOT_VALID_GROUP_ID == groupId)                      // Group ID zero is invalid
      || (CC_AGI_groupCount_handler(endpoint) < groupId )) // Check with AGI.
  {
    return false; // Not a valid group ID
  }
  return true;
}

uint8_t
handleGetMaxNodesInGroup(
  uint8_t groupIden,
  uint8_t ep)
{
  // GroupID = 0 is interpreted as 1.
  if (groupIden == 0)
  {
    groupIden = 1;
  }

  // Only the endpoint defined in LIFELINE_ENDPOINT_ALLOWED is allowed for the lifeline, groupID = 1.
  if ((LIFELINE_GROUP_ID == groupIden) && (LIFELINE_ENDPOINT_ALLOWED != ep))
  {
    // Returning 0 because that's the max number of associations that is supported
    // for group 1 (LIFELINE) if the endpoint is other than LIFELINE_ENDPOINT_ALLOWED.
    return 0;
  }

  /*
   * Specific returns in case of being included as a LR endpoint node.
   */
  if (EINCLUSIONMODE_ZWAVE_LR == ZAF_GetInclusionMode())  // endpoint does not need to be checked due to the above.
  {
    return (LIFELINE_GROUP_ID == groupIden)?
        1:  // Only one association is accepted in LR.
        0;  // No association is accepted for any other group than the lifeline in LR.
  }

  return MAX_ASSOCIATION_IN_GROUP;  // Valid return for Z-Wave CLS.
}

uint8_t
handleGetMaxAssociationGroups(uint8_t endpoint)
{
  return CC_AGI_groupCount_handler(endpoint);
}

/**
 * Returns the result of comparing two nodes.
 *
 * The function compares node ID and endpoint, but has one twist: It considers a node with an
 * endpoint as a lower node than a node without an endpoint.
 *
 * NOTICE that it does matter which node is passed to which argument.
 * @param pNodeExisting Must be the existing node.
 * @param pNodeNew Must be a new node to insert in the list of existing nodes.
 * @return -1 means that the new node is bigger than the existing node, 1 is vice versa, and 0 is equal!
 */
static int32_t IsNewNodeGreater(destination_info_t * pNodeExisting, destination_info_t * pNodeNew)
{
  // Check to see if they are equal (memcmp() could not be used for this)
  if (   (pNodeExisting->node.nodeId     == pNodeNew->node.nodeId)
      && (pNodeExisting->node.endpoint   == pNodeNew->node.endpoint)
      && (pNodeExisting->node.BitAddress == pNodeNew->node.BitAddress))
  {
    return 0; // Equal/Identical
  }

  /*
   * In case of endpoints that are higher than 7, we have a special case where we no longer can use BitAddressing,
   * so we must treat these as none-endpoint node associations. Doing the following will push the new node further
   * down in the list of nodes during sorting in node addition.
   */
  if (pNodeNew->node.endpoint > ENDPOINT_VALUE_VALID_MAX)
  {
    if (pNodeExisting->node.endpoint < pNodeNew->node.endpoint)
    {
      return -1;  // The new node is bigger than the existing node
    }
    else if (pNodeExisting->node.endpoint > pNodeNew->node.endpoint)
    {
      return 1;  // The new node is smaller than the existing node
    }
  }

  // Check to see if the new node is smaller or bigger than pNodeExisting.
  if ((pNodeExisting->node.nodeId > pNodeNew->node.nodeId)
      && (pNodeExisting->node.endpoint >= ENDPOINT_VALUE_VALID_MIN)
      && (0 == pNodeNew->node.endpoint))
  {
    return -1;  // The new node is bigger than the existing node
  }
  else if ((pNodeExisting->node.nodeId < pNodeNew->node.nodeId)
      && (pNodeNew->node.endpoint >= ENDPOINT_VALUE_VALID_MIN)
      && (0 == pNodeExisting->node.endpoint))
  {
    return 1;  // The new node is smaller than the existing node
  }

  // As last resort, use memcmp() to get a comparison value
  return memcmp(&pNodeExisting->node, &pNodeNew->node, sizeof(MULTICHAN_DEST_NODE_ID));
}

bool
AssociationAddNode(
    uint8_t groupID,
    uint8_t endpoint,
    MULTICHAN_DEST_NODE_ID* pNodeToAdd,
    bool multiChannelAssociation)
{
  ASSERT(pNodeToAdd->nodeId <= ZW_MAX_NODES); // This makes this function LR aware!
  /*
   * Due to NVM not supporting 16 bit NodeIDs, can this function not process
   * node additions for NodeIDs higher than 232 (ZW_MAX_NODES).
   */
  if (pNodeToAdd->nodeId > ZW_MAX_NODES)
  {
    return false; // Do not process request of adding a node with higher than limit NodeID!
    // (Also a LR restriction. Controllers have nodeID less than ZW_MAX_NODES!)
  }

  if (ZAF_GetInclusionMode() == EINCLUSIONMODE_ZWAVE_LR)
  {
    /*
     * Z-Wave Long Range nodes are special cases for the use of associations and have the following limitation(s):
     *  - Only the Lifeline group is in use.
     *  - Only controllers can be added.
     */

    // Only accept the LIFELINE group ID! (LR nodes are not associating with each other or to other classic nodes.)
    if (groupID != LIFELINE_GROUP_ID)
    {
      return false; // Reject adding non-LIFELINE group additions.
    }

    MULTICHAN_NODE_ID* pList = NULL;
    uint8_t ListLen = 0;
    handleAssociationGetnodeList(groupID, endpoint, &pList, &ListLen);

    if (ListLen > 0) {
      // The controller is probably already included, and this one must be a classic node trying to be added to association.
      return false;
    }

    // Weak check for detecting controllers, but still relevant as these node's ID must be below ZW_MAX_NODES!
    if (pNodeToAdd->nodeId > ZW_MAX_NODES)
    {
      return false; // Unexpected Error! (Rejected: Controllers always have an ID equal or below ZW_MAX_NODES (232)!)
    }

    // A Controller is being associated to this Z-Wave LR node!
    DPRINT("Detected InclusionMode: EINCLUSIONMODE_ZWAVE_LR \n");
  }

  // Find the rootGroupID and endpoint of this groupID
  AssGroupMappingLookUp(&endpoint, &groupID);

  uint8_t maxNodesInGroup = handleGetMaxNodesInGroup(groupID, endpoint);

  destination_info_t * pCurrentNode;
  destination_info_t newNode;
  newNode.node.nodeId = pNodeToAdd->nodeId;
  newNode.node.endpoint = pNodeToAdd->endpoint;
  newNode.node.BitAddress = pNodeToAdd->BitAddress;
  newNode.nodeInfo.BitMultiChannelEncap = multiChannelAssociation;
  newNode.nodeInfo.security = GetHighestSecureLevel(ZAF_GetSecurityKeys());

  int32_t result;
  int32_t indx;

  // Search the existing associations to see if the node already exists.
  for (indx = 0; indx < maxNodesInGroup; indx++)
  {
    pCurrentNode = GetNode(endpoint, groupID, (uint8_t)indx);
    if (IsFree(pCurrentNode)) {
      break;
    }

    result = IsNewNodeGreater(pCurrentNode, &newNode);

    if (0 == result) {
      // Node exists already. Return true to indicate that it was added.
      return true;
    }
  }

  // Verify that we have at least one free entry.
  pCurrentNode = GetNode(endpoint, groupID, maxNodesInGroup - 1);
  if (!IsFree(pCurrentNode))
  {
    return false;
  }

  if (maxNodesInGroup > 1) {
    // Compare with the second last item because the last one will always be free at this point.
    indx = maxNodesInGroup - 2;
  } else {
    // There can only be one End Device in group.
    indx = 0;
  }

  /*
   * Compare the node with each existing node in the list, starting from the highest index and
   * successively move the new node up if it is found to be smaller than the existing node (pCurrentNode)
   * at that index.
   */
  do {
    pCurrentNode = GetNode(endpoint, groupID, (uint8_t)indx);
    if (IsFree(pCurrentNode)) {
      continue;  // Skip free nodes
    }

    // Positive results mean that the new node is smaller and must mode further up the list.
    result = IsNewNodeGreater(pCurrentNode, &newNode);

    if (0 < result) {
      // New node is smaller than current node => Move node in current entry down with one position.
      destination_info_t * pNewLocation = GetNode(endpoint, groupID, (uint8_t)indx + 1);
      *pNewLocation = *pCurrentNode;  // move the existing node at this index lower in the list.
    } else if (0 > result) {
      // We found a node that is smaller than the new. (The new node is bigger than the one in the list at this index.)
      break;
    }
  } while (--indx >= 0);

  /*
   * indx points to the node that is smaller than the new node. Hence, we increase it to the next index
   * that should be a free entry.
   */
  indx++;
  pCurrentNode = GetNode(endpoint, groupID, (uint8_t)indx);  // This returns an empty node.
  memcpy((uint8_t *)pCurrentNode, (uint8_t*)&newNode, sizeof(destination_info_t)); // Place the new node in list.
  return true;
}

e_cmd_handler_return_code_t
AssociationRemove(
  uint8_t groupId,
  uint8_t ep,
  ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_1BYTE_V2_FRAME* pCmd,
  uint8_t cmdLength)
{
  uint8_t j;
  uint8_t * pCmdByteWise = (uint8_t *)pCmd;
  ASSOCIATION_NODE_LIST list;
  uint8_t maxNumberOfGroups;

  /*Only setup lifeline for rootdevice*/
  if ((NUMBER_OF_ENDPOINTS + 1) < ep || ((1 == groupId) && (0 < ep)))
  {
    return E_CMD_HANDLER_RETURN_CODE_FAIL;
  }

  maxNumberOfGroups = CC_AGI_groupCount_handler(ep);

  if ((NOT_VALID_GROUP_ID < groupId) && (maxNumberOfGroups >= groupId))
  {
    // Find the rootGroupID and endpoint of this groupID
    AssGroupMappingLookUp(&ep, &groupId);

    if ((3 == cmdLength) || ((4 == cmdLength) && (0x00 == *(pCmdByteWise + 3))))
    {
      /*
       * The command is either [Class, Command, GroupID] or [Class, Command, GroupID, Marker].
       * In either case, we delete all nodes in the given group.
       */
      for (int8_t i = MAX_ASSOCIATION_IN_GROUP - 1; i >= 0; i--)
      {
        /*
         * The node can only be deleted according to the following truth table.
         * CCA   = Command Class Association Remove Command
         * CCMCA = Command Class Multi Channel Association Remove Command
         * MC    = Multi Channel associated node
         * T     = true
         * F     = false
         *      | CCA | CCMCA |
         * --------------------
         * MC=T |  V  |   V   |
         * --------------------
         * MC=F |  %  |   V   |
         * --------------------
         */
        destination_info_t * pCurrentNode = GetNode(ep, groupId, (uint8_t)i);
        if (!((COMMAND_CLASS_ASSOCIATION == *pCmdByteWise) && (true == HasEndpoint(pCurrentNode))))
        {
          Free(pCurrentNode);
          ReorderGroupAfterRemove(groupId-1, ep, (uint8_t)i);
        }
      }
    }
    else
    {
      // If this is the case, the command must be [Class, Command, GroupID, i3, i4, i5, ...].
      ExtractCmdClassNodeList(&list,
                              (ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME*)pCmd,
                              cmdLength,
                              COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3);
      RemoveAssociationsFromGroup(*pCmdByteWise, ep, groupId, &list);
    }
    AssociationStoreAll();
  }
  else if (0 == groupId)
  {
    /*
     * When the group ID equals zero, it is desired to remove all nodes from all groups or given
     * nodes from all groups.
     * If the length is larger than 3, it means that only given nodes must be removed from all
     * groups.
     */
    ExtractCmdClassNodeList(&list,
                            (ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME*)pCmd,
                            cmdLength,
                            COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3);

    for (ep = 0; ep < NUMBER_OF_ENDPOINTS + 1; ep++)
    {
      maxNumberOfGroups = CC_AGI_groupCount_handler(ep);
      for (j = 1; j <= maxNumberOfGroups; j++)
      {
        RemoveAssociationsFromGroup(*pCmdByteWise, ep, j, &list);
      }
    }
    AssociationStoreAll();
  }
  else
  {
    return E_CMD_HANDLER_RETURN_CODE_FAIL;
  }
  return E_CMD_HANDLER_RETURN_CODE_HANDLED;
}

/**
 * @brief Removes given associations or all associations from a given group.
 * @param cmdClass Command Class: Association or MultiChannel Association.
 * @param ep EndPoint to be removed, used in case of MultiChannel Association CC
 * @param groupId ID of the group from where the associations must be removed.
 * @param listOfNodes List of nodes/associations that must be removed.
 */
static void
RemoveAssociationsFromGroup(
    uint8_t cmdClass,
    uint8_t ep,
    uint8_t groupId,
    ASSOCIATION_NODE_LIST * pListOfNodes)
{
  uint8_t numberOfNodes;

  // We have to go through the loop once, even when the list is empty.
  numberOfNodes = ((0 == pListOfNodes->noOfNodes) ? 1 : pListOfNodes->noOfNodes);

  // Remove all Node ID Associations in the given list.
  for (uint8_t i = 0; i < numberOfNodes; i++)
  {
    for (int8_t indx = MAX_ASSOCIATION_IN_GROUP - 1; indx >= 0; indx--)
    {
      destination_info_t * pCurrentNode = GetNode(ep, groupId, (uint8_t)indx);
      if (!IsFree(pCurrentNode) &&
          (false == HasEndpoint(pCurrentNode)) &&
          ((0 == pListOfNodes->noOfNodes && 0 == pListOfNodes->noOfMulchanNodes) || pCurrentNode->node.nodeId == pListOfNodes->pNodeId[i]))
      {
        DPRINTF("sa: Remove: %u.%u\n",pCurrentNode->node.nodeId, pCurrentNode->node.endpoint);
        Free(pCurrentNode);
        /*
         * Do reorder after freeing each entry because the command might originate from
         * CC Association and then endpoint destinations will not be deleted. Hence, they must be
         * ordered.
         */
        ReorderGroupAfterRemove(groupId-1, ep, (uint8_t)indx);
      }
    }
  }

  if (COMMAND_CLASS_ASSOCIATION == cmdClass)
  {
    return;
  }

  numberOfNodes = ((0 == pListOfNodes->noOfMulchanNodes) ? 1 : pListOfNodes->noOfMulchanNodes);

  // Remove all Endpoint Node ID Associations in the given list.
  for (uint8_t i = 0; i < numberOfNodes; i++)
  {
    for (int8_t indx = MAX_ASSOCIATION_IN_GROUP - 1; indx >= 0; indx--)
    {
      destination_info_t * pCurrentNode = GetNode(ep, groupId, (uint8_t)indx);
      // Remove node if one of following conditions is met:
      // 1. List of simple and multichannel nodes is zero (remove everything)
      // 2. Remove single node with specified NodeId, EP and bitmask, and it is found in list of multichannel associations
      // 3.  Only EP was specified, but not NodeID. Then remove all associations with matching End Point.
      if (IsFree(pCurrentNode) || (false == HasEndpoint(pCurrentNode)))
      {
        // Current node is empty, or it's not multichannel node -> nothing to delete
        continue;
      }

      bool condition = false; // Condition to delete current node
      if (0 == pListOfNodes->noOfNodes && 0 == pListOfNodes->noOfMulchanNodes)
      {
        // List of simple and multichannel nodes is zero (remove everything)
        condition  = true;
      }
      else if (0 != pListOfNodes->noOfMulchanNodes)
      {
        // List of multichannel nodes is not empty -> check if this node should be deleted
        bool bMatchingNodeId = ((pCurrentNode->node.nodeId == pListOfNodes->pMulChanNodeId[i].nodeId)
            || (0 == pListOfNodes->pMulChanNodeId[i].nodeId));
        bool bMatchingEP = (pCurrentNode->node.endpoint == pListOfNodes->pMulChanNodeId[i].endpoint);
        bool bAddr = (pCurrentNode->node.BitAddress == pListOfNodes->pMulChanNodeId[i].BitAddress);

        condition = bMatchingNodeId && bMatchingEP && bAddr;
      }

      if (condition)
      {
        DPRINTF("mlc: Remove %u.%u\n", pCurrentNode->node.nodeId,
                pCurrentNode->node.endpoint);
        Free (pCurrentNode);

        /*
         * In case specific multi channel destinations are specified, we need to reorder those that
         * will not be removed.
         * If no multi channel destinations are specified, it means that all nodes in the group
         * will be removed. Hence, no need to reorder.
         */
        if (0 < pListOfNodes->noOfMulchanNodes)
        {
          ReorderGroupAfterRemove(groupId-1, ep, (uint8_t)indx);
        }
      }
    }
  }
}


/**
 * This function takes care of stepping through the entire association table that is in memory.
 * The input is used to identify the need for table initialization or checking for valid NodeID and correction.
 *
 * This function further writes the table to NVM.
 */
static void generateAssociationAndWrite(NVM_ACTION action, SAssociationInfo *pAssociationInfo)
{
  bool writeFile = false;

  /*
   * Notice the +1 for the endpoint expression. This ensures that this loop will run for the
   * root device.
   */
  for (uint8_t endpoint = 0; endpoint < (NUMBER_OF_ENDPOINTS + 1); endpoint++)
  {
    for (uint8_t group = 0; group < MAX_ASSOCIATION_GROUPS; group++)
    {
      for (uint8_t node = 0; node < MAX_ASSOCIATION_IN_GROUP; node++)
      {
        if ((action == NVM_ACTION_INIT_CORRECT_INVALID_NODEID
            && 0 == pAssociationInfo->Groups[endpoint][group].subGrp[node].node.nodeId)
            || action == NVM_ACTION_INIT_FORCE_CLEAR_MEM)
        {
          pAssociationInfo->Groups[endpoint][group].subGrp[node].node.nodeId = FREE_VALUE;  // Free
          writeFile = true;
        }
      }
    }
  }

  if(writeFile)
  {
    const zpal_status_t status = zpal_nvm_write(pFileSystem, ZAF_FILE_ID_ASSOCIATIONINFO, pAssociationInfo, sizeof(SAssociationInfo));
    ASSERT(ZPAL_STATUS_OK == status);
  }
}


/**
 * @brief Reads/Writes association data to the NVM.
 * @param action The action to take.
 */
static void
NVM_Action(NVM_ACTION action)
{
  uint8_t i,j,k;

  zpal_status_t status = ZPAL_STATUS_FAIL;
  size_t   dataLen = 0;
  bool     forceClearMem = false;
  SAssociationInfo associationInfo;  // This can become a large allocation on the stack
  SAssociationInfo* pSource = &associationInfo;

  pFileSystem = ZAF_GetFileSystemHandle();
  ASSERT(pFileSystem != 0);

  switch(action)
  {
    case NVM_ACTION_INIT_FORCE_CLEAR_MEM:
      forceClearMem = true;
      // Fall through
    case NVM_ACTION_INIT_CORRECT_INVALID_NODEID:

      // Fetch the stored data, since we are not clearing it.
      if (!forceClearMem)
      {
        status = zpal_nvm_get_object_size(pFileSystem, ZAF_FILE_ID_ASSOCIATIONINFO, &dataLen);
      }

      /*
       * If the stored NMV3 file structure for the Association Info is different than expected here,
       * erase it and create a new file.
       */
      if ((ZPAL_STATUS_OK != status) || (ZAF_FILE_SIZE_ASSOCIATIONINFO != dataLen) || (true == forceClearMem))
      {
        //Write default Association Info file
        memset(&associationInfo, 0 , sizeof(SAssociationInfo));

        generateAssociationAndWrite(action, &associationInfo);
      }
      else
      {
        // Make sure that free nodeIds are not set to legacy zero value
        zpal_nvm_read(pFileSystem, ZAF_FILE_ID_ASSOCIATIONINFO, &associationInfo, sizeof(SAssociationInfo));

        generateAssociationAndWrite(action, &associationInfo);
      }
      // Fall through
    case NVM_ACTION_READ_DATA:

      status = zpal_nvm_read(pFileSystem, ZAF_FILE_ID_ASSOCIATIONINFO, &associationInfo, sizeof(SAssociationInfo));
      ASSERT(ZPAL_STATUS_OK == status);

      for(i = 0; i < MAX_ASSOCIATION_GROUPS; i++)
      {
        for(j = 0; j < NUMBER_OF_ENDPOINTS + 1; j++)
        {
          for(k = 0; k < MAX_ASSOCIATION_IN_GROUP; k++)
          {
            groups[j][i].subGrp[k].node.nodeId     = pSource->Groups[j][i].subGrp[k].node.nodeId;     //1Byte
            groups[j][i].subGrp[k].node.endpoint   = pSource->Groups[j][i].subGrp[k].node.endpoint;   //7bits
            groups[j][i].subGrp[k].node.BitAddress = pSource->Groups[j][i].subGrp[k].node.BitAddress; //1bit

            /*
             * Ignore bitfield conversion warnings as there is no good solution other than stop
             * using bitfields.
             */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
            groups[j][i].subGrp[k].nodeInfo.BitMultiChannelEncap = pSource->Groups[j][i].subGrp[k].nodeInfoPacked.BitMultiChannelEncap; //1bit to uint8_t
            groups[j][i].subGrp[k].nodeInfo.security             = (security_key_t)pSource->Groups[j][i].subGrp[k].nodeInfoPacked.security;  //4bits to enum
#pragma GCC diagnostic pop
          }
        }
      }
      break;

    case NVM_ACTION_WRITE_DATA:

      for(i = 0; i < MAX_ASSOCIATION_GROUPS; i++)
      {
        for(j = 0; j < NUMBER_OF_ENDPOINTS + 1; j++)
        {
          for(k = 0; k < MAX_ASSOCIATION_IN_GROUP; k++)
          {
            associationInfo.Groups[j][i].subGrp[k].node.nodeId     = (uint8_t)groups[j][i].subGrp[k].node.nodeId;     //1Byte
            associationInfo.Groups[j][i].subGrp[k].node.endpoint   = groups[j][i].subGrp[k].node.endpoint;   //7bits
            associationInfo.Groups[j][i].subGrp[k].node.BitAddress = groups[j][i].subGrp[k].node.BitAddress; //1bit

            /*
             * Ignore bitfield conversion warnings as there is no good solution other than stop
             * using bitfields.
             */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
            associationInfo.Groups[j][i].subGrp[k].nodeInfoPacked.BitMultiChannelEncap = groups[j][i].subGrp[k].nodeInfo.BitMultiChannelEncap; //uint8_t to 1 bit
            associationInfo.Groups[j][i].subGrp[k].nodeInfoPacked.security             = (uint8_t)groups[j][i].subGrp[k].nodeInfo.security;  //enum to 4bits
#pragma GCC diagnostic pop

            DPRINTF("associationInfo.Groups[%d][%d].subGrp[%d].node.nodeId: %d\r\n", j,i,k, associationInfo.Groups[j][i].subGrp[k].node.nodeId);
          }
        }
      }

      status = zpal_nvm_write(pFileSystem, ZAF_FILE_ID_ASSOCIATIONINFO, &associationInfo, sizeof(SAssociationInfo));
      ASSERT(ZPAL_STATUS_OK == status);
      break;

    default:
      DPRINT("FATAL: Case not handled. Invalid input.");
      ASSERT(0);  // Invalid input
  }
}


/**
 * @brief Stores all associations in the NVM.
 */
static void
AssociationStoreAll(void)
{
  NVM_Action(NVM_ACTION_WRITE_DATA);
}


void
AssociationInit(bool forceClearMem, void * pFS)
{
  UNUSED(pFS);

  pFileSystem = ZAF_GetFileSystemHandle();

  NVM_ACTION action = forceClearMem ? NVM_ACTION_INIT_FORCE_CLEAR_MEM : NVM_ACTION_INIT_CORRECT_INVALID_NODEID;
  NVM_Action(action);
}


e_cmd_handler_return_code_t handleAssociationSet(
    uint8_t ep,
    ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME* pCmd,
    uint8_t cmdLength,
    uint8_t commandClass)
{
  uint8_t i = 0;
  ASSOCIATION_NODE_LIST list;

  // Set up lifeline for root device only.
  if (((NUMBER_OF_ENDPOINTS + 1) < ep
      || ((LIFELINE_GROUP_ID == pCmd->groupingIdentifier) && (ENDPOINT_ROOT < ep)))
      || (NOT_VALID_GROUP_ID == pCmd->groupingIdentifier)
      || (CC_AGI_groupCount_handler(ep) < pCmd->groupingIdentifier)) {
    return E_CMD_HANDLER_RETURN_CODE_FAIL;
  }

  ExtractCmdClassNodeList(&list, pCmd, cmdLength, commandClass);

  if ((list.noOfNodes + list.noOfMulchanNodes) > handleGetMaxNodesInGroup(pCmd->groupingIdentifier, ep)
      || 0 == (list.noOfNodes + list.noOfMulchanNodes)) {
    return E_CMD_HANDLER_RETURN_CODE_FAIL;
  }
  DPRINTF("\nnoOfNodes + noOfMulchanNodes = %u + %u\n", list.noOfNodes, list.noOfMulchanNodes);

  // NodeId Association /////////////////////////////////////////////////////

  bool allNodesAdded = true;

  for(i = 0; i < list.noOfNodes; i++) {
    MULTICHAN_DEST_NODE_ID node;
    node.nodeId = (node_id_t)(list.pNodeId[i]);
    node.endpoint = 0;
    node.BitAddress = 0;

    if (0 == node.nodeId) {
      allNodesAdded = false;
      continue;
    }

    if(false == AssociationAddNode( pCmd->groupingIdentifier, ep, &node, false)) {
      return E_CMD_HANDLER_RETURN_CODE_FAIL;
    }
  }

  // Multi Channel Association ///////////////////////////////////////////////

  bool multiChannelAssociation = (list.noOfMulchanNodes > 0) ? true : false;

  for(i = 0; i < list.noOfMulchanNodes; i++) {
    MULTICHAN_DEST_NODE_ID node;
    node.nodeId = (node_id_t)(list.pMulChanNodeId[i].nodeId);
    node.endpoint = list.pMulChanNodeId[i].endpoint;
    node.BitAddress = list.pMulChanNodeId[i].BitAddress;

    if (0 == list.pMulChanNodeId[i].nodeId) {
      allNodesAdded = false;
      continue;
    }
    if(false == AssociationAddNode(pCmd->groupingIdentifier,
                                   ep,
                                   &node,
                                   multiChannelAssociation)) {
      return E_CMD_HANDLER_RETURN_CODE_FAIL;
    }
  }

  AssociationStoreAll();
  // We report error for anything unexpected that happened during addition to the association table.
  return (true == allNodesAdded) ? E_CMD_HANDLER_RETURN_CODE_HANDLED : E_CMD_HANDLER_RETURN_CODE_FAIL;
}

/**
 * @brief Extracts nodes and endpoints from a given (Multi Channel) Association frame.
 * @param[out] plist
 * @param[in] pCmd
 * @param[in] cmdLength
 * @param[in] commandClass
 */
static void
ExtractCmdClassNodeList(
    ASSOCIATION_NODE_LIST* plist,
    ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME* pCmd,
    uint8_t cmdLength,
    uint8_t commandClass)
{
  plist->pNodeId          = &pCmd->nodeId1;
  plist->noOfNodes        = 0;
  plist->pMulChanNodeId   = NULL;
  plist->noOfMulchanNodes = 0;

  if (3 >= cmdLength)
  {
    /*
     * If the length is less than or equal to three, it means that it's a get or a remove. In the
     * first case we shouldn't end up here. In the second case, we must return, since the remove
     * command should remove all nodes.
     */
    plist->pNodeId = NULL;
    return;
  }

  cmdLength -= OFFSET_PARAM_2; /*calc length on node-Id's*/
  uint8_t * currentListElement = plist->pNodeId;
  for (uint8_t i = 0; i < cmdLength; i++, currentListElement++)
  {
    if ((COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3 == commandClass)
        && (MULTI_CHANNEL_ASSOCIATION_SET_MARKER_V2 == *currentListElement))
    {
      plist->noOfMulchanNodes = (uint8_t)(cmdLength - (uint8_t)(i + 1)) / 2;
      if (0 != plist->noOfMulchanNodes)
      {
        plist->pMulChanNodeId = (MULTICHAN_DEST_NODE_ID_8bit*)(currentListElement + 1); /*Point after the marker*/
        // all done, exit
        return;
      }
    }
    else
    {
      plist->noOfNodes = i + 1;
    }
  }
}

/**
 * @brief Finds a mapping between groupID and endpoint and returns the
 * mapping through the same pointer.
 *
 * @param[in/out] pEndpoint IN = Pointer to endpoint value. OUT = The mapped Endpoint to the root group ID.
 * @param[in/out] pRootGroupID IN = Pointer to a Root Group ID. OUT = The mapped Endpoint Group ID.
 * @return true if the mapping was found, false otherwise.
 */
static bool
AssGroupMappingLookUp(
    uint8_t* pEndpoint,
    uint8_t* pGroupID)
{

  if ((false == ((ENDPOINT_ROOT == *pEndpoint) && (LIFELINE_GROUP_ID != *pGroupID))) || (1 == handleGetMaxAssociationGroups(1)) ) {
    return false;
  }
  uint8_t grpTest = 1;
  uint8_t grpInput = *pGroupID;
 
  for (uint8_t ep = 1; ep <= NUMBER_OF_ENDPOINTS; ep++)  
  {
    uint8_t epGrpCount = handleGetMaxAssociationGroups(ep) - 1;
    grpTest += epGrpCount;
    if (grpInput <= grpTest) {
      *pGroupID = (uint8_t)((epGrpCount - (grpTest - grpInput)) + 1);
      *pEndpoint = ep;
      return true;
    }
  }
  return false;
}
void
AssociationGet(
    uint8_t endpoint,
    uint8_t * incomingFrame,
    uint8_t * outgoingFrame,
    size_t  * outgoingFrameLength)
{
  uint8_t nodeCount;
  uint8_t nodeCountMax;
  size_t  nodeCountNoEndpoint;
  size_t  nodeFieldCount;
  uint8_t mappedEndpoint;
  uint8_t mappedGroupID;
  MULTICHAN_NODE_ID * pCurrentNode;

  if ((*(incomingFrame + 2) > CC_AGI_groupCount_handler(endpoint)) || (0 == *(incomingFrame + 2)))
  {
    // If the group is invalid, we return group 1
    *(incomingFrame + 2) = 1;
  }

  mappedEndpoint = endpoint;
  mappedGroupID = *(incomingFrame + 2);
  if (0 == endpoint)
  {
    // Find the rootGroupID and endpoint of this groupID
    AssGroupMappingLookUp(&mappedEndpoint, &mappedGroupID);
  }

  nodeCountMax = handleGetMaxNodesInGroup(*(incomingFrame + 2), endpoint);

  *outgoingFrame = *incomingFrame; // Set the command class.

  *(outgoingFrame + 2) = *(incomingFrame + 2); // The group
  *(outgoingFrame + 3) = nodeCountMax;
  *(outgoingFrame + 4) = 0; // Number of reports to follow.

  // Add node IDs without endpoints if any.
  nodeCountNoEndpoint = 0;
  for (nodeCount = 0; nodeCount < nodeCountMax; nodeCount++)
  {
    pCurrentNode = GetNode(mappedEndpoint, mappedGroupID, nodeCount);

    if (IsFree(pCurrentNode)) break;

    if (false == HasEndpoint(pCurrentNode))
    {
      // No endpoints in the association
      *(outgoingFrame + 5 + nodeCountNoEndpoint) = (uint8_t)pCurrentNode->node.nodeId;
      nodeCountNoEndpoint++;
    }
  }

  switch (*(incomingFrame)) // Check command class.
  {
  case COMMAND_CLASS_ASSOCIATION:
    *(outgoingFrame + 1) = ASSOCIATION_REPORT_V2; // The response command.
    break;
  case COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3:
    *(outgoingFrame + 1) = MULTI_CHANNEL_ASSOCIATION_REPORT_V3; // The response command.

    // Add endpoint nodes if any.
    nodeFieldCount = 0;
    for (nodeCount = 0; nodeCount < nodeCountMax; nodeCount++)
    {
      pCurrentNode = GetNode(mappedEndpoint, mappedGroupID, nodeCount);

      // Since the group is ordered, we can break on the first free entry.
      if (IsFree(pCurrentNode)) break;

      if (true == HasEndpoint(pCurrentNode))
      {
        // The association contains endpoints.
        *(outgoingFrame + 6 + nodeCountNoEndpoint + nodeFieldCount++) = (uint8_t)(pCurrentNode->node.nodeId & 0x00FF);
        *(outgoingFrame + 6 + nodeCountNoEndpoint + nodeFieldCount++) = (uint8_t)((pCurrentNode->node.BitAddress << 7) | pCurrentNode->node.endpoint);
      }
    }

    if (nodeFieldCount)
    {
      *(outgoingFrame + 5 + nodeCountNoEndpoint) = MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V3;
      *outgoingFrameLength = sizeof(ZW_MULTI_CHANNEL_ASSOCIATION_REPORT_1BYTE_V3_FRAME) - 3 + nodeCountNoEndpoint + nodeFieldCount;

      // We return if there are endpoint associations.
      return;
    }
    break;
  default:
    // We should never get here, but if we do it means that we got an invalid command class.
    // Set the length to zero.
    *outgoingFrameLength = 0;
    break;
  }

  // If there are no endpoint associations we end up here.
  *outgoingFrameLength = sizeof(ZW_ASSOCIATION_REPORT_1BYTE_FRAME) - 1 + nodeCountNoEndpoint;
}


/***********************************************************************************
 * Functions used while doing transmission to association groups.
 **********************************************************************************/

static destination_info_t * associatedDestinationArray[MAX_ASSOCIATION_IN_GROUP];
static uint32_t singlecastDestIndex = 0;      // The index position in the associatedDestinationArray.
static uint32_t associatedDestinationsCount;  // Number of associations in the active association group for current multi-channel transmissions.
static uint32_t bitAddressedIndex;

/**********************************************************************************/

void AssociationGetDestinationInit(destination_info_t * pFirstDestination)
{
  bitAddressedIndex = 0;  // init

  // Find number of singlecasts
  associatedDestinationsCount = MAX_ASSOCIATION_IN_GROUP;
  for (uint32_t i = 0; i < MAX_ASSOCIATION_IN_GROUP; i++)
  {
    // Prepare the associatedDestinationArray
    if (IsFree(pFirstDestination + i)) {
      associatedDestinationsCount = i;
      break;
    }
    associatedDestinationArray[i] = pFirstDestination + i;
  }
}

/*
 * This functions like an iterator.
 * It iterates through all nodes in the associatedDestinationArray as initialized from the association list.
 * The initialization was done in AssociationGetDestinationInit().
 */
destination_info_t * AssociationGetNextSinglecastDestination()
{
  destination_info_t * pNode = NULL;

  for (uint8_t j = 0; j < MAX_ASSOCIATION_IN_GROUP; j++)
  {
    pNode = associatedDestinationArray[singlecastDestIndex % associatedDestinationsCount];  // The modulus of associatedDestinationsCount might no longer be needed.
    singlecastDestIndex++;
    if (NULL != pNode) {
      break;  // Return node
    }
  }
  ASSERT(pNode != NULL);  // We must never return NULL
  return pNode;
}

uint8_t AssociationGetSinglecastEndpointDestinationCount(void)
{
  uint8_t epCount = 0;
  destination_info_t * pNode;

  for (uint8_t i = 0; i < MAX_ASSOCIATION_IN_GROUP; i++)
  {
    pNode = associatedDestinationArray[i];
    if ((NULL != pNode) && IS_BIT_ADDRESSING_ENDPOINT(pNode->node.endpoint))
    {
      epCount++;
    }
  }
  return epCount;
}

uint32_t AssociationGetSinglecastNodeCount(void)
{
  uint32_t count = associatedDestinationsCount;
  for (uint32_t i = 0; i < associatedDestinationsCount; i++)
  {
    /* Search and remove all list-entries that are NULL, since those associations that were multi-channel
     * endpoint associations have been set to NULL in this list and already received their transmissions. */
    if (NULL == associatedDestinationArray[i]) {
      count--;
    }
  }
  return count;
}


bool AssociationGetBitAdressingDestination(destination_info_t ** ppNodeList,
                                           uint8_t * pListLength,
                                           destination_info_t * pNode)
{
  destination_info_t * pEntry = *ppNodeList;
  node_id_t previousNodeId = 0;

  uint8_t i;
  for (i = 0; i < *pListLength; i++, bitAddressedIndex++) {
    uint8_t endpoint = (pEntry + i)->node.endpoint;
    node_id_t nodeId = (pEntry + i)->node.nodeId;
    security_key_t security_key = (pEntry + i)->nodeInfo.security;

    /*
     * At the time of setting the node association, the associated nodes are sorted as they are being stored
     * in the non-volatile memory. The sorting algorithm sorts the nodes in such a way that the first
     * nodes in the list are those with endpoints within the valid range of bit-addressing. They are being
     * placed with ascending nodeID value.
     * Hereafter, are nodes that have a non-bit-addressable endpoint placed in the list.
     * Again hereafter are nodes without endpoint placed in the list.
     */
    if (0 == i) {
      // Switched to a new node ID (or it is the first iteration)
      if (IS_BIT_ADDRESSING_ENDPOINT(endpoint)) {
        // In this case, the first node in the list has and endpoint.
        // Hence, it must be the first association of MAYBE several with endpoints.
        memset((uint8_t *)pNode, 0, sizeof(destination_info_t));
        pNode->node.nodeId = nodeId;
        pNode->node.endpoint = (1 << (endpoint - 1));  // Set bits of endpoints
        // Grab the security level from the first node because all nodes have the same level.
        pNode->nodeInfo.security = security_key;
        /*
         * If there is only one element in the list, the node will not be Bit-addressed!
         */
      } else {
        // Current node has no endpoints. Since we know that our associations are sorted with
        // endpoint destinations first, we know that there are no more endpoint destinations.
        // Do not change list pointer or list length because we did not use anything from the list.
        pNode->node.BitAddress = 0;
        return false;
      }
    } else if (previousNodeId != nodeId || IS_BIT_ADDRESSING_ENDPOINT(endpoint) == false) {
      /*
       * The node ID has changed, or, while nodeID is the same, the endpoint is not valid for bit-addressing.
       * Terminate the aggregation of bit-addressed endpoint TX.
       * The nodes are sorted in the list, so when the first invalid endpoint is encountered,
       * nor further valid endpoints will be found in the list. The next if-statement results in returning false.
       */

      // We must point pList to this node and update list length.
      *ppNodeList = pEntry + i;  // Move the tip of the list to start from here.
      *pListLength -= i;         // Reduce size

      // Indicate further need of calling this function.
      if (IS_BIT_ADDRESSING_ENDPOINT(endpoint)) {
        // Next node also has endpoints.
        return true;  // Hence, this function needs to be called again.
      } else {
        // Set the index position of the last multi-channel endpoint transmitted association.
        // It will be used by the iterator (aka AssociationGetNextSinglecastDestination())
        singlecastDestIndex = i;
        return false;
      }
    } else {
      // If the node ID did not change, this association MUST be at least the SECOND one with an
      // endpoint. (the endpoint is between 1 and 7!)
      ASSERT( !(pNode->node.endpoint & (1 << (endpoint - 1)) ) );  // Make sure that the bit is not already set.
      /*
       * Ignore bitfield conversion warnings as there is no good solution other than stop
       * using bitfields.
       */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
      pNode->node.endpoint |= (1 << (endpoint - 1));
#pragma GCC diagnostic pop
      pNode->node.BitAddress = 1;
      // Mark this one and the previous one as used for bit addressing
      associatedDestinationArray[bitAddressedIndex - 1] = NULL;
      associatedDestinationArray[bitAddressedIndex] = NULL;
    }
    previousNodeId = nodeId;
  }

  // If we fall through the loop without a return it means that there a no more nodes in the list
  // and that the last node was with an endpoint.

  // Update list and list length
  *ppNodeList = NULL;  // List is empty. All nodes are handled.
  *pListLength = 0;    // All nodes in the list was with an endpoint.

  return false;
}
