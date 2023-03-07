/***************************************************************************//**
 * @file
 * @brief stubs for the external interface of the end-device bind
 * code for use in applications which don't use end-device bind.
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

const EmberLibraryStatus emEndDeviceBindLibraryStatus =
  EMBER_LIBRARY_IS_STUB;

bool emHandleEndDeviceBindRequest(EmberNodeId source,
                                  EmberApsOption options,
                                  uint8_t sequence,
                                  PacketHeader payloadBuffer,
                                  uint8_t payloadIndex,
                                  uint8_t length)
{
  (void)source;
  (void)options;
  (void)sequence;
  (void)payloadBuffer;
  (void)payloadIndex;
  (void)length;
  return false;
}

void emZigbeeDeviceIdTableUpdated(EmberEUI64 longId, EmberNodeId shortId)
{
  (void)longId;
  (void)shortId;
}

void emZigbeeDeviceBindUnbindResponse(bool bind,
                                      EmberNodeId source,
                                      uint8_t status)
{
  (void)bind;
  (void)source;
  (void)status;
}
