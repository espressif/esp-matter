/***************************************************************************//**
 * @file
 * @brief stubs for the external interface of the binding-table
 * code for use in applications which don't use the binding-table
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
#include "stack/include/library.h"

const EmberLibraryStatus emBindingTableLibraryStatus = EMBER_LIBRARY_IS_STUB;

void emForgetBindingShortId(EmberNodeId shortId)
{
  (void)shortId;
}

EmberNodeId emUpdateBindingRemoteNode(EmberEUI64 longId, EmberNodeId shortId)
{
  (void)longId;
  (void)shortId;
  return EMBER_NULL_NODE_ID;
}

void emberSetBindingRemoteNodeId(uint8_t i, EmberNodeId shortId)
{
  (void)i;
  (void)shortId;
}

void emUpdateBindingFlags(EmberEUI64 longId, uint8_t flags)
{
  (void)longId;
  (void)flags;
}

EmberStatus emberGetBinding(uint8_t index, EmberBindingTableEntry *result)
{
  (void)index;
  (void)result;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberSetBinding(uint8_t index, EmberBindingTableEntry *value)
{
  (void)index;
  (void)value;
  return EMBER_LIBRARY_NOT_PRESENT;
}

uint8_t emFindMulticastBinding(uint8_t start, EmberMulticastId multicastId)
{
  (void)start;
  (void)multicastId;
  return EMBER_NULL_BINDING;
}

uint8_t emFindUnicastBinding(EmberNodeId shortId,
                             uint8_t *longId,
                             bool ignoreEndpoints,
                             uint8_t localEndpoint,
                             uint8_t remoteEndpoint)
{
  (void)shortId;
  (void)longId;
  (void)ignoreEndpoints;
  (void)localEndpoint;
  (void)remoteEndpoint;
  return EMBER_NULL_BINDING;
}

void emGetZigbeeBinding(uint8_t index,
                        EmberBindingTableEntry *binding,
                        EmberEUI64 sourceLongId)
{
  (void)index;
  (void)binding;
  (void)sourceLongId;
}

bool emRemoteModifyBinding(EmberBindingTableEntry *binding,
                           EmberEUI64 sourceLongId,
                           bool isSet,
                           bool *success,
                           EmberZdoStatus *status)
{
  (void)binding;
  (void)sourceLongId;
  (void)isSet;
  (void)success;
  (void)status;
  return false;
}

void emInitializeBindingTable(void)
{
}

bool emHandleBindUnbindRequests(EmberNodeId source,
                                uint8_t *contents,
                                uint16_t clusterId)
{
  (void)source;
  (void)contents;
  (void)clusterId;
  return false;
}

bool emHandleBindingTableRequest(EmberApsOption options,
                                 EmberNodeId source,
                                 uint8_t *contents)
{
  (void)options;
  (void)source;
  (void)contents;
  return false;
}

EmberNodeId emberGetBindingRemoteNodeId(uint8_t index)
{
  (void)index;
  return EMBER_NULL_NODE_ID;
}

EmberStatus emberDeleteBinding(uint8_t index)
{
  (void)index;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberClearBindingTable(void)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}
