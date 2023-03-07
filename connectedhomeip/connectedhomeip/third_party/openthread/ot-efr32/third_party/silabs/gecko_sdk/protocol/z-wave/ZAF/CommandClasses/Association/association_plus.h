/**
 * @file association_plus.h
 * Helper module for Command Class Association and Command Class Multi Channel Association.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _ASSOCIATION_PLUS_H_
#define _ASSOCIATION_PLUS_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include "config_app.h"
#include <agi.h>


/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

#define FREE_VALUE                  0x00FF  // Valid for both Classic and LR variants!

/*
 * GroupID related definitions.
 */
#define NOT_VALID_GROUP_ID          0  ///< Invalid group ID
#define LIFELINE_GROUP_ID           1  ///< LIFELINE group ID
#define LIFELINE_ENDPOINT_ALLOWED   0  ///< The only allowed endpoint of the Lifeline group!

/**
 * Group id numbers
 */
typedef enum _ASSOCIATION_GROUP_ID_
{
  ASS_GRP_ID_1 = 1,
  ASS_GRP_ID_2,
  ASS_GRP_ID_3,
  ASS_GRP_ID_4,
  ASS_GRP_ID_5,
  ASS_GRP_ID_6,
  ASS_GRP_ID_7,
  ASS_GRP_ID_8,
  ASS_GRP_ID_9
}
ASSOCIATION_GROUP_ID;

/**
 * Structure for mapping root groups to endpoint groups
 */
typedef struct _ENDPOINT_ASSOCIATION_MAPPING_
{
  uint8_t rootGrpId;
  uint8_t endpoint;
  uint8_t endpointGrpId;
}
ASSOCIATION_ROOT_GROUP_MAPPING;

typedef struct _EEOFFS_NVM_TRANSPORT_CAPABILITIES_STRUCT_
{
  uint8_t security               : 4; /**< bit 0-3 of type security_key_t: 0-NON_KEY,1-S2_UNAUTHENTICATED,
                                           2-S2_AUTHENTICATED, 3-S2_ACCESS, 4-S0 (security_key_t)*/
  uint8_t unused                 : 1; /**< bit 4 */
  uint8_t BitMultiChannelEncap   : 1; /**< bit 5 */
  uint8_t unused2                : 1; /**< bit 6 */
  uint8_t unused3                : 1; /**< bit 7 */
}
EEOFFSET_TRANSPORT_CAPABILITIES_STRUCT;

typedef struct _ASSOCIATION_GROUP_
{
  MULTICHAN_NODE_ID subGrp[MAX_ASSOCIATION_IN_GROUP];
}
ASSOCIATION_GROUP;

typedef struct _ASSOCIATION_GROUP_PACKED_
{
  MULTICHAN_NODE_ID_PACKED subGrp[MAX_ASSOCIATION_IN_GROUP];
}
ASSOCIATION_GROUP_PACKED;

// Used to save association data to file system.
typedef struct SAssociationInfo
{
  ASSOCIATION_GROUP_PACKED Groups[NUMBER_OF_ENDPOINTS + 1][MAX_ASSOCIATION_GROUPS];  // +1 is for creating a rootdevice endpoint
} SAssociationInfo;

#define ZAF_FILE_SIZE_ASSOCIATIONINFO     (sizeof(SAssociationInfo))

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * @brief Initializes the Association module. This function must be called by devices not using
 * endpoints.
 * @param[in] forceClearMem If true, the association NVM will be cleared.
 * @param[in] pFS pointer to the file system.
 */
void
AssociationInit(bool forceClearMem, void * pFS);

#define AssociationInitEndpointSupport(forceClearMem, pMapping, nbrGrp, pFS)    \
do {                                                                            \
_Pragma("GCC diagnostic push")                                                  \
_Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")                        \
AssociationInit(forceClearMem, pFS);                                            \
_Pragma("GCC diagnostic pop")                                                   \
}while(0)

/**
 * Delivers a list of nodes in a given association group corresponding to a given endpoint.
 * @param[in] groupId Association group ID.
 * @param[in] ep Endpoint.
 * @param[out] ppListOfNodes is out double-pointer of type MULTICHAN_NODE_ID deliver node list
 * @param[out] pListLen length of list
 * @return enum type NODE_LIST_STATUS
 */
NODE_LIST_STATUS handleAssociationGetnodeList(
  uint8_t groupId,
  uint8_t ep,
  destination_info_t ** ppListOfNodes,
  uint8_t * pListLen);

/**
 * @brief Removes all nodes or given node(s) from all groups or a given group.
 * @details See Association CC and Multi Channel Association CC for details.
 * @param[in] groupId Association group ID.
 * @param[in] ep Multi Channel Endpoint.
 * @param[in] pCmd Pointer to the command containing the node IDs to remove.
 * @param[in] cmdLength Length of the command.
 * @return command handler return code
 */
e_cmd_handler_return_code_t
AssociationRemove(
  uint8_t groupId,
  uint8_t ep,
  ZW_MULTI_CHANNEL_ASSOCIATION_REMOVE_1BYTE_V2_FRAME* pCmd,
  uint8_t cmdLength);

/**
 * @brief Returns the number of association groups for a given endpoint.
 * @param[in] endpoint A given endpoint where 0 is the root device.
 * @return Number of association groups.
 */
uint8_t
handleGetMaxAssociationGroups(uint8_t endpoint);

/**
 * @brief Handles an incoming (Multi Channel) Association Get command and composes a (Multi Channel)
 * Association Report.
 * @param[in] endpoint The endpoint from which the associated nodes must be read.
 * @param[in] incomingFrame The incoming frame including CC and command.
 * @param[out] outgoingFrame The composed frame ready for transmission.
 * @param[out] outgoingFrameLength The total length of the outgoing frame.
 */
void
AssociationGet(
    uint8_t endpoint,
    uint8_t * incomingFrame,
    uint8_t * outgoingFrame,
    size_t  * outgoingFrameLength);

/**
 * @brief Associates a given node in the given group for a given endpoint.
 * @details The endpoint argument specifies the local endpoint for which the association is made.
 * E.g. if this device would be a Wall Controller/Switch with 4 endpoints, one for each switch, and
 * an association was to be made for endpoint 1. The associated node would receive something when
 * button 1 is pressed (if of course button one represents endpoint 1).
 * @param groupID ID of the group in which the association must be made.
 * @param endpoint Endpoint for which the association must be made.
 * @param pNode Pointer to a node with info about node ID, endpoint, etc.
 * @param multiChannelAssociation Specifies whether the associated node ID includes an endpoint or
 * not.
 * @return true if association is added, false otherwise.
 */
bool AssociationAddNode(
    uint8_t groupID,
    uint8_t endpoint,
    MULTICHAN_DEST_NODE_ID* pNode,
    bool multiChannelAssociation);

/***********************************************************************************
 * Functions used while doing transmission to association groups.
 **********************************************************************************/

/**
 * Initializes the fetching of nodes before e.g. a transmission. It places a destination_node_list into the system.
 *
 * MUST be invoked before
 * AssociationGetBitAdressingDestination(),
 * AssociationGetSinglecastNodeCount() and
 * AssociationGetNextSinglecastDestination().
 *
 * @param[in] pFirstDestination Address of the first association in a group. Retrieved by invoking
 *                              handleAssociationGetnodeList().
 */
void AssociationGetDestinationInit(destination_info_t * pFirstDestination);

/**
 * Outputs a node where associations to multiple endpoints of that node exist. The pNode [OUT] (list)
 * will contain all the endpoints using bit addressing, for endpoints equal or less than 7.
 *
 * Operation: For the first encounter of a nodeID and an endpoint, this function will search for the
 * same nodeID and add their endpoints to the endpoint-field as bit-flags. First time this function
 * encounters a node without a valid endpoint for bit-addressing, it will terminate and return false
 * because the list is sorted.
 *
 * If there are zero nodes with multiple endpoints associated, the function will return false
 * and write NULL to the pointer returned, pNode.
 *
 * If further nodes with multiple endpoints associated exist after the initial list is created,
 * the function will return true and indicate that this function is not finished.
 *
 * If the function returns true, the invoker must invoke the function again to get the next node.
 *
 * If there are no more nodes with multiple endpoints, the function will return false, as this
 * function is only designed to return node-destinations with endpoints.
 *
 *
 * Example with 6 associations in total:   (This is a pre-sorted list in accordance to requirements of this function!)
 * 3.1
 * 3.2
 * 4.1
 * 4.2
 * 5.1
 * 6.0
 *
 * @code
 *
 * destination_info_t * pNodeList    // Must point to the first node in an association group.
 * uint8_t              listLength;  // Must be set to the length of the node list.
 * destination_info_t   node;
 * bool                 moreNodes;
 *
 * moreNodes = AssociationGetBitAdressingDestination(&pNodeList, &listLength, &node);
 * // moreNodes is TRUE. (4.1 is still showing endpoint destination)
 * // pNodeList now points to node 4 endpoint 1.
 * // listLength now equals 4. (the two from the top with endpoints are removed from list)
 * // Bit-addressing is set to TRUE. (because 3.1 and 3.2 are bit-addressed)
 * // --> The prepared node contains the first nodeID (3) including the two endpoints as bit addressing.
 *
 * moreNodes = AssociationGetBitAdressingDestination(&pNodeList, &listLength, &node);
 * // moreNodes is TRUE. (5.1 is still showing endpoint destination)
 * // pNodeList now points to node 5 endpoint 1.
 * // listLength now equals 2. (the four destinations from the top with endpoints are removed from list)
 * // Bit-addressing is set to TRUE. (because 4.1 and 4.2 are bit-addressed)
 * // --> The prepared node contains the second nodeID (4) including the two endpoints as bit addressing.
 *
 * moreNodes = AssociationGetBitAdressingDestination(&pNodeList, &listLength, &node);
 * // moreNodes is FALSE. (6.0 is not having an endpoint destination)
 * // pNodeList now points to node 6 with no endpoints.
 * // listLength now equals 2. (the four destinations from the top remain the only removed from list)
 * // Bit-addressing is set to FALSE. (5.1 was the only endpoint destination of nodeID 5)
 * // // --> The prepared node is not good for multichannel TX with bit-addressing. This terminates multichannel TX.
 *
 * // The nodeID 6 destination is not processed for multichannel transmissions due to how the list is sorted!
 *
 * @endcode
 *
 *
 * @param[in,out] ppNodeList - Address of pointer to an element in the node list. The pointer will
 *                             be updated as this function travels through the node list.
 * @param[in,out] pListLength - Pointer to length of the node list. The value will be updated by this
 *                              function.
 * @param[out] pNode - Address of the memory to where the node is written.
 * @return Returns true if this function needs to be called again for a different nodeID with endpoints, otherwise false.
 */
bool AssociationGetBitAdressingDestination(destination_info_t ** ppNodeList,
                                           uint8_t * pListLength,
                                           destination_info_t * pNode);

/**
 * Returns the number of associations after the bit addressing destinations are removed.
 *
 * This function MUST be invoked AFTER AssociationGetBitAdressingDestination() has returned false.
 *
 * @return Returns the number of remaining nodes when there are no more bit addressing nodes.
 */
uint32_t AssociationGetSinglecastNodeCount(void);

/**
 * Returns the address of the next association like an iterator.
 * This function needs to know beforehand how many items it needs to iterate through.
 *
 * If the number of iterations are more than totalNodeCount, there will be repeated return of the same item.
 * If the number of iterations are less than totalNodeCount, it will impact the next transmission using this iterator.
 *
 * AssociationGetDestinationInit() MUST have been invoked before this function.
 *
 * This function must be invoked only AFTER AssociationGetBitAdressingDestination() has returned false.
 *
 * Continuously invoking this function will return the next association. If there are no more
 * associations in the group, the first one in the same group will be returned and so forth.
 *
 * If the group is empty, the function will ASSERT.
 *
 * @return Address of an association.
 */
destination_info_t * AssociationGetNextSinglecastDestination();

/**
 * Returns the number of endpoint destinations in the active node association list.
 *
 * AssociationGetDestinationInit() MUST have been invoked before this function.
 *
 * @return Number of destination endpoints found in list.
 */
uint8_t AssociationGetSinglecastEndpointDestinationCount(void);
#endif /* _ASSOCIATION_PLUS_H_ */
