/***************************************************************************//**
 * @file
 * @brief ZigBee Device Object (ZDO) functions available on all platforms.
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

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#include "hal/hal.h"
#include "zigbee-device-common.h"

static uint8_t zigDevRequestSequence = 0;
uint8_t zigDevRequestRadius = 255;

// Application generated ZDO messages use sequence numbers 0-127, and the stack
// uses sequence numbers 128-255.  This simplifies life by eliminating the need
// for coordination between the two entities, and allows both to send ZDO
// messages with non-conflicting sequence numbers.
#define APPLICATION_ZDO_SEQUENCE_MASK (0x7F)

uint8_t emberGetLastAppZigDevRequestSequence(void)
{
  return (zigDevRequestSequence & APPLICATION_ZDO_SEQUENCE_MASK);
}

uint8_t emberNextZigDevRequestSequence(void)
{
  return ((++zigDevRequestSequence) & APPLICATION_ZDO_SEQUENCE_MASK);
}

EmberStatus emberSendZigDevRequestTarget(EmberNodeId target,
                                         uint16_t clusterId,
                                         EmberApsOption options)
{
  uint8_t contents[ZDO_MESSAGE_OVERHEAD + 2];
  uint8_t *payload = contents + ZDO_MESSAGE_OVERHEAD;
  payload[0] = LOW_BYTE(target);
  payload[1] = HIGH_BYTE(target);
  return emberSendZigDevRequest(target,
                                clusterId,
                                options,
                                contents,
                                sizeof(contents));
}

EmberStatus emberSimpleDescriptorRequest(EmberNodeId target,
                                         uint8_t targetEndpoint,
                                         EmberApsOption options)
{
  uint8_t contents[ZDO_MESSAGE_OVERHEAD + 3];
  uint8_t *payload = contents + ZDO_MESSAGE_OVERHEAD;
  payload[0] = LOW_BYTE(target);
  payload[1] = HIGH_BYTE(target);
  payload[2] = targetEndpoint;
  return emberSendZigDevRequest(target,
                                SIMPLE_DESCRIPTOR_REQUEST,
                                options,
                                contents,
                                sizeof(contents));
}

EmberStatus emberSendZigDevBindRequest(EmberNodeId target,
                                       uint16_t bindClusterId,
                                       EmberEUI64 source,
                                       uint8_t sourceEndpoint,
                                       uint16_t clusterId,
                                       uint8_t type,
                                       EmberEUI64 destination,
                                       EmberMulticastId groupAddress,
                                       uint8_t destinationEndpoint,
                                       EmberApsOption options)
{
  uint8_t contents[ZDO_MESSAGE_OVERHEAD + 21];
  uint8_t *payload = contents + ZDO_MESSAGE_OVERHEAD;
  uint8_t length;
  MEMMOVE(payload, source, 8);
  payload[8] = sourceEndpoint;
  payload[9] = LOW_BYTE(clusterId);
  payload[10] = HIGH_BYTE(clusterId);
  payload[11] = type;
  switch (type) {
    case UNICAST_BINDING:
      MEMMOVE(payload + 12, destination, 8);
      payload[20] = destinationEndpoint;
      length = ZDO_MESSAGE_OVERHEAD + 21;
      break;
    case MULTICAST_BINDING:
      payload[12] = LOW_BYTE(groupAddress);
      payload[13] = HIGH_BYTE(groupAddress);
      length = ZDO_MESSAGE_OVERHEAD + 14;
      break;
    default:
      return EMBER_ERR_FATAL;
  }
  return emberSendZigDevRequest(target,
                                bindClusterId,
                                options,
                                contents,
                                length);
}

#define emberLqiTableRequest(target, startIndex, options) \
  (emberTableRequest(LQI_TABLE_REQUEST, (target), (startIndex), (options)))
#define emberRoutingTableRequest(target, startIndex, options) \
  (emberTableRequest(ROUTING_TABLE_REQUEST, (target), (startIndex), (options)))
#define emberBindingTableRequest(target, startIndex, options) \
  (emberTableRequest(BINDING_TABLE_REQUEST, (target), (startIndex), (options)))

EmberStatus emberTableRequest(uint16_t clusterId,
                              EmberNodeId target,
                              uint8_t startIndex,
                              EmberApsOption options)
{
  uint8_t contents[ZDO_MESSAGE_OVERHEAD + 1];
  contents[ZDO_MESSAGE_OVERHEAD] = startIndex;
  return emberSendZigDevRequest(target,
                                clusterId,
                                options,
                                contents,
                                sizeof(contents));
}

EmberStatus emberLeaveRequest(EmberNodeId target,
                              EmberEUI64 deviceAddress,
                              uint8_t leaveRequestFlags,
                              EmberApsOption options)
{
  uint8_t contents[ZDO_MESSAGE_OVERHEAD + 9];
  MEMMOVE(contents + ZDO_MESSAGE_OVERHEAD, deviceAddress, 8);
  contents[ZDO_MESSAGE_OVERHEAD + 8] = leaveRequestFlags;
  return emberSendZigDevRequest(target,
                                LEAVE_REQUEST,
                                options,
                                contents,
                                sizeof(contents));
}

EmberStatus emberPermitJoiningRequest(EmberNodeId target,
                                      uint8_t duration,
                                      uint8_t authentication,
                                      EmberApsOption options)
{
  uint8_t contents[ZDO_MESSAGE_OVERHEAD + 2];
  contents[ZDO_MESSAGE_OVERHEAD] = duration;
  contents[ZDO_MESSAGE_OVERHEAD + 1] = authentication;
  return emberSendZigDevRequest(target,
                                PERMIT_JOINING_REQUEST,
                                options,
                                contents,
                                sizeof(contents));
}
