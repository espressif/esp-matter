/**
 * @file
 * @brief Functions used to manipulate bits (Node ID) in a NodeMask array.
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef _ZW_NODEMASK_API_H_
#define _ZW_NODEMASK_API_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_transport_api.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/* Number of nodes per node mask byte */
#define NODEMASK_NODES_PER_BYTE   8

/* The max length of a node mask*/
#define MAX_NODEMASK_LENGTH       (ZW_MAX_NODES/NODEMASK_NODES_PER_BYTE)     // 29 bytes
#define MAX_LR_NODEMASK_LENGTH    (ZW_MAX_NODES_LR/NODEMASK_NODES_PER_BYTE)  // 128 bytes

typedef uint8_t NODE_MASK_TYPE[MAX_NODEMASK_LENGTH];
typedef uint8_t LR_NODE_MASK_TYPE[MAX_LR_NODEMASK_LENGTH];

typedef struct {
  NODE_MASK_TYPE      nodeMask;     /// List of Z-wAve nodes.
  LR_NODE_MASK_TYPE   nodeMask_LR;  /// List of Z-Wave LR nodes.
} NODE_MASK_TYPE_ALL;

/****************************  NodeMask  ************************************
** Functions used to manipulate bits (Node ID) in a byte array (NodeMask array)
**
*****************************************************************************/

#define ZW_NODE_MASK_SET_BIT(pMask, bNodeID) ZW_NodeMaskSetBit(pMask, bNodeID)
#define ZW_NODE_MASK_CLEAR_BIT(pMask, bNodeID) ZW_NodeMaskClearBit(pMask, bNodeID)
#define ZW_NODE_MASK_CLEAR(pMask, bLength) ZW_NodeMaskClear(pMask, bLength)
#define ZW_NODE_MASK_BITS_IN(pMask, bLength) ZW_NodeMaskBitsIn(pMask, bLength)
#define ZW_NODE_MASK_NODE_IN(pMask, bNode) ZW_NodeMaskNodeIn(pMask, bNode)


/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/***
 * Set the node bit in a node bitmask
 * @param pMask Nodemask
 * @param bNodeID Node ID that should be set in the mask
 */
extern void
ZW_NodeMaskSetBit(
uint8_t* pMask,
uint8_t bNodeID);

/***
 * Set the node bit in a Long Range node bitmask
 * @param pMask Long Range Nodemask
 * @param nodeID Node ID that should be set in the mask
 */
extern void
ZW_LR_NodeMaskSetBit(
uint8_t* pMask,
node_id_t nodeID);

/**
 * Clear the node bit in a node bitmask
 * @param pMask Nodemask
 * @param bNodeID Node to clear in nodemask
 */
extern void
ZW_NodeMaskClearBit(
uint8_t* pMask,
uint8_t bNodeID);

/**
 * Clear the node bit in a Long Range node bitmask
 * @param pMask Long Range Nodemask
 * @param nodeID Node to clear in nodemask
 */
extern void
ZW_LR_NodeMaskClearBit(
uint8_t* pMask,
node_id_t nodeID);

/**
 * Clear all bits in a nodemask
 * @param pMask Nodemask that should be cleared
 * @param bLength Length of nodemask to be cleared
 */
extern void
ZW_NodeMaskClear(
uint8_t* pMask,
uint8_t bLength);


/**
 * Check and count number of bit that is set in a nodemask
 * @param pMask Pointer to nodemask that should be counted
 * @param bLength Length of nodemask to count
 * @return Number of bits set in nodemask
 */
extern uint8_t
ZW_NodeMaskBitsIn(
uint8_t* pMask,
uint8_t bLength);

/**
 * Check if a node is in a nodemask
 * @param pMask Pointer to nodemask to check for bNode
 * @param bNode Bit number that should be checked
 * @return 0 if not in nodemask, non-zero if found in nodemask
 */
extern uint8_t
ZW_NodeMaskNodeIn(
uint8_t* pMask,
uint8_t bNode);

/**
 * Check if a node is in a Long Range nodemask.
 * @param pMask Pointer to Long Range nodemask to check for bNode
 * @param nodeID Bit number that should be checked
 * @return 0 if not in nodemask, non-zero if found in nodemask
 */
extern uint8_t
ZW_LR_NodeMaskNodeIn(
uint8_t* pMask,
node_id_t nodeID);

/**
 * Find the next NodeId that is set in a nodemask
 * @param currentNodeId Last NodeId found (0 for first call)
 * @param pMask Nodemask that should be searched
 * @return Next NodeId from the nodemask if found, or 0 if not found.
 */
extern uint8_t
ZW_NodeMaskGetNextNode(
  uint8_t currentNodeId,
  uint8_t* pMask);

#endif /* _ZW_NODEMASK_API_H_ */
