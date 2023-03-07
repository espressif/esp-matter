/***************************************************************************//**
 * @file
 * @brief
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
#include "stack/include/ember-types-internal.h"
#include "stack/include/byte-utilities.h"
#include "mac-header.h"
#include "mac-packet-header.h"

//-----------------------------------------------------------------------------

EmMacFrameInfoElementParseResult emMacParseInfoElementsInPacket(PacketHeader header,
                                                                EmMacInfoElementField* infoElementsArray,
                                                                uint8_t  maxInfoElementCount,
                                                                uint8_t* macInfoElementsLength)
{
  (void)infoElementsArray;
  (void)maxInfoElementCount;
  uint8_t* macPacket;
  uint16_t frameControl;

  macPacket = sl_mac_payload_pointer(header);
  frameControl = emberFetchLowHighInt16u(macPacket);

  *macInfoElementsLength = 0;

  if (!(frameControl & MAC_FRAME_FLAG_IE_LIST_PRESENT)) {
    return EM_MAC_FRAME_INFO_ELEMENTS_NOT_PRESENT;
  }

  return EM_MAC_FRAME_INFO_ELEMENTS_PRESENT_WITH_ERRORS;
}

bool emMacHeaderGetInfoElementsLength(PacketHeader header,
                                      uint8_t* returnMacInfoElementLength)
{
  EmMacFrameInfoElementParseResult result
    = emMacParseInfoElementsInPacket(header,
                                     NULL,
                                     0,
                                     returnMacInfoElementLength);

  if (EM_MAC_FRAME_INFO_ELEMENTS_PRESENT_WITH_ERRORS == result) {
    return false;
  }

  return true;
}
