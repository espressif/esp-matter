/***************************************************************************//**
 * @file
 * @brief ZigBee Device Object (ZDO) functions not provided by the stack.
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
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "zigbee-device-common.h"
#include "zigbee-device-library.h"

static EmberStatus sendZigDevRequestBuffer(EmberNodeId destination,
                                           uint16_t clusterId,
                                           EmberApsOption options,
                                           EmberMessageBuffer request);

EmberStatus emberMatchDescriptorsRequest(EmberNodeId target,
                                         uint16_t profile,
                                         EmberMessageBuffer inClusters,
                                         EmberMessageBuffer outClusters,
                                         EmberApsOption options)
{
  uint8_t inCount = (inClusters == EMBER_NULL_MESSAGE_BUFFER
                     ? 0
                     : emberMessageBufferLength(inClusters)
                     / 2); // Clusters are 2 bytes long
  uint8_t outCount = (outClusters == EMBER_NULL_MESSAGE_BUFFER
                      ? 0
                      : emberMessageBufferLength(outClusters)
                      / 2); // Clusters are 2 bytes long
  EmberMessageBuffer request;
  uint8_t offset = ZDO_MESSAGE_OVERHEAD + 5;
  uint8_t contents[ZDO_MESSAGE_OVERHEAD + 5];  // Add 2 bytes for NWK Address
                                               // Add 2 bytes for Profile Id
                                               // Add 1 byte for in Cluster Count

  EmberStatus result;
  uint8_t i;

  contents[0] = emberNextZigDevRequestSequence();
  contents[1] = LOW_BYTE(target);
  contents[2] = HIGH_BYTE(target);
  contents[3] = LOW_BYTE(profile);
  contents[4] = HIGH_BYTE(profile);
  contents[5] = inCount;

  request = emberFillLinkedBuffers(contents, offset);
  if (request == EMBER_NULL_MESSAGE_BUFFER) {
    return EMBER_NO_BUFFERS;
  }

  if (emberSetLinkedBuffersLength(request,
                                  offset
                                  + (inCount * 2) // Times 2 for 2 byte Clusters
                                  + 1             // Out Cluster Count
                                  + (outCount * 2)) // Times 2 for 2 byte Clusters
      != EMBER_SUCCESS) {
    return EMBER_NO_BUFFERS;
  }
  for (i = 0; i < inCount; i++) {
    emberSetLinkedBuffersByte(request,
                              (i * 2) + offset,
                              emberGetLinkedBuffersByte(inClusters, (i * 2)));
    emberSetLinkedBuffersByte(request,
                              (i * 2) + offset + 1,
                              emberGetLinkedBuffersByte(inClusters,
                                                        (i * 2) + 1));
  }
  offset += (inCount * 2);
  emberSetLinkedBuffersByte(request, offset, outCount);
  offset++;
  for (i = 0; i < outCount; i++) {
    emberSetLinkedBuffersByte(request,
                              (i * 2) + offset,
                              emberGetLinkedBuffersByte(outClusters, (i * 2)));
    emberSetLinkedBuffersByte(request,
                              (i * 2) + offset + 1,
                              emberGetLinkedBuffersByte(outClusters,
                                                        (i * 2) + 1));
  }

  result = sendZigDevRequestBuffer(target,
                                   MATCH_DESCRIPTORS_REQUEST,
                                   options,
                                   request);
  emberReleaseMessageBuffer(request);
  return result;
}

EmberStatus emberEndDeviceBindRequest(uint8_t endpoint,
                                      EmberApsOption options)
{
  EmberMessageBuffer request;
  EmberStatus result;
  uint8_t contents[ZDO_MESSAGE_OVERHEAD + 14];
  EmberEndpointDescription descriptor;
  uint8_t i;

  if (!emberGetEndpointDescription(endpoint, &descriptor)) {
    return EMBER_INVALID_ENDPOINT;
  }

  contents[0]  = emberNextZigDevRequestSequence();
  contents[1]  = LOW_BYTE(emberGetNodeId());
  contents[2]  = HIGH_BYTE(emberGetNodeId());
  MEMMOVE(contents + 3, emberGetEui64(), 8);
  contents[11] = endpoint;
  contents[12] = LOW_BYTE(descriptor.profileId);
  contents[13] = HIGH_BYTE(descriptor.profileId);
  contents[14] = descriptor.inputClusterCount;

  request = emberFillLinkedBuffers(contents, sizeof(contents));
  if (request == EMBER_NULL_MESSAGE_BUFFER) {
    return EMBER_NO_BUFFERS;
  }
  for (i = 0; i < descriptor.inputClusterCount; i++) {
    uint16_t clusterId = emberGetEndpointCluster(endpoint,
                                                 EMBER_INPUT_CLUSTER_LIST,
                                                 i);
    uint8_t temp = LOW_BYTE(clusterId);
    emberAppendToLinkedBuffers(request,
                               &temp,
                               1);
    temp = HIGH_BYTE(clusterId);
    emberAppendToLinkedBuffers(request,
                               &temp,
                               1);
  }
  emberAppendToLinkedBuffers(request,
                             &descriptor.outputClusterCount,
                             1);
  for (i = 0; i < descriptor.outputClusterCount; i++) {
    uint16_t clusterId = emberGetEndpointCluster(endpoint,
                                                 EMBER_OUTPUT_CLUSTER_LIST,
                                                 i);
    uint8_t temp = LOW_BYTE(clusterId);
    emberAppendToLinkedBuffers(request,
                               &temp,
                               1);
    temp = HIGH_BYTE(clusterId);
    emberAppendToLinkedBuffers(request,
                               &temp,
                               1);
  }
  result = sendZigDevRequestBuffer(EMBER_ZIGBEE_COORDINATOR_ADDRESS,
                                   END_DEVICE_BIND_REQUEST,
                                   options,
                                   request);

  if (request != EMBER_NULL_MESSAGE_BUFFER) {
    emberReleaseMessageBuffer(request);
  }
  return result;
}

EmberStatus emberSendZigDevRequest(EmberNodeId destination,
                                   uint16_t clusterId,
                                   EmberApsOption options,
                                   uint8_t *contents,
                                   uint8_t length)
{
  EmberMessageBuffer message;
  EmberStatus result;

  contents[0] = emberNextZigDevRequestSequence();

  message = emberFillLinkedBuffers(contents, length);

  if (message == EMBER_NULL_MESSAGE_BUFFER) {
    return EMBER_NO_BUFFERS;
  } else {
    result = sendZigDevRequestBuffer(destination,
                                     clusterId,
                                     options,
                                     message);
    emberReleaseMessageBuffer(message);
    return result;
  }
}

static EmberStatus sendZigDevRequestBuffer(EmberNodeId destination,
                                           uint16_t clusterId,
                                           EmberApsOption options,
                                           EmberMessageBuffer request)
{
  EmberApsFrame apsFrame;

  apsFrame.sourceEndpoint = EMBER_ZDO_ENDPOINT;
  apsFrame.destinationEndpoint = EMBER_ZDO_ENDPOINT;
  apsFrame.clusterId = clusterId;
  apsFrame.profileId = EMBER_ZDO_PROFILE_ID;
  apsFrame.options = options;

  if (destination == EMBER_BROADCAST_ADDRESS
      || destination == EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS
      || destination == EMBER_SLEEPY_BROADCAST_ADDRESS) {
    return emberSendBroadcast(destination,
                              &apsFrame,
                              emberGetZigDevRequestRadius(),
                              request);
  } else {
    return emberSendUnicast(EMBER_OUTGOING_DIRECT,
                            destination,
                            &apsFrame,
                            request);
  }
}

EmberNodeId emberDecodeAddressResponse(EmberMessageBuffer response,
                                       EmberEUI64 eui64Return)
{
  uint8_t contents[11] = { 0 };
  if (emberMessageBufferLength(response) < ZDO_MESSAGE_OVERHEAD + 11) {
    return EMBER_NULL_NODE_ID;
  }
  emberCopyFromLinkedBuffers(response, ZDO_MESSAGE_OVERHEAD, contents, 11);
  if (contents[0] == EMBER_ZDP_SUCCESS) {
    MEMMOVE(eui64Return, contents + 1, EUI64_SIZE);
    return HIGH_LOW_TO_INT(contents[10], contents[9]);
  } else {
    return EMBER_NULL_NODE_ID;
  }
}
