/***************************************************************************//**
 * @file
 * @brief Utilities for sending and receiving ZigBee AMI InterPAN messages.
 * See @ref message for documentation.
 *
 * @deprecated The ami-inter-pan library is deprecated and will be removed in a
 * future release.  Similar functionality is available in the Inter-PAN plugin
 * in Application Framework.
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

/** @addtogroup message
 *
 * See also ami-inter-pan.h for source code.
 * @{
 */

#ifndef AMI_INTER_PAN_H
#define AMI_INTER_PAN_H

// The three types of inter-PAN messages.  The values are actually the
// corresponding APS frame controls.
//
// 0x03 is the special interPAN message type.  Unicast mode is 0x00,
// broadcast mode is 0x08, and multicast mode is 0x0C.
//

#define INTER_PAN_UNICAST   0x03
#define INTER_PAN_BROADCAST 0x0B
#define INTER_PAN_MULTICAST 0x0F

// Frame control, sequence, dest PAN ID, dest, source PAN ID, source.
#define MAX_INTER_PAN_MAC_SIZE (2 + 1 + 2 + 8 + 2 + 8)
// Short form has a short destination.

// NWK stub frame has two control bytes.
#define STUB_NWK_SIZE 2
#define STUB_NWK_FRAME_CONTROL 0x000B

// APS frame control, group ID, cluster ID, profile ID
#define MAX_STUB_APS_SIZE (1 + 2 + 2 + 2)
// Short form has no group ID.

#define MAX_INTER_PAN_HEADER_SIZE \
  (MAX_INTER_PAN_MAC_SIZE + STUB_NWK_SIZE + MAX_STUB_APS_SIZE)

/**
 * @brief A struct for keeping track of all of the header info.
 **/
typedef struct {
  uint8_t messageType;            // one of the INTER_PAN_...CAST values

  // MAC addressing
  // For outgoing messages this is the destination.  For incoming messages
  // it is the source, which always has a long address.
  uint16_t panId;
  bool hasLongAddress;       // always true for incoming messages
  EmberNodeId shortAddress;
  EmberEUI64 longAddress;

  // APS data
  uint16_t profileId;
  uint16_t clusterId;
  uint16_t groupId;               // only used for INTER_PAN_MULTICAST
} InterPanHeader;

/**
 * @brief Create an interpan message suitable for passing to emberSendRawMessage().
 */
EmberMessageBuffer makeInterPanMessage(InterPanHeader *headerData,
                                       EmberMessageBuffer payload);

/**
 * @brief Meant to be called on the message and offset values passed
 * to emberMacPassthroughMessageHandler(...).  The header is parsed and
 * the various fields are written to the InterPanHeader.  The returned
 * value is the offset of the payload in the message, or 0 if the
 * message is not a correctly formed AMI interPAN message.
 */
uint8_t parseInterPanMessage(EmberMessageBuffer message,
                             uint8_t startOffset,
                             InterPanHeader *headerData);

#endif // AMI_INTER_PAN_H

/** @} END addtogroup */
