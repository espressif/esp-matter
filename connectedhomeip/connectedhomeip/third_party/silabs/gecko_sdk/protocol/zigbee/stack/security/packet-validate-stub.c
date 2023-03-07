/***************************************************************************//**
 * @file
 * @brief Stub version of sanity check functions.
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
#include "stack/include/library.h"

const EmberLibraryStatus emPacketValidateLibrary = EMBER_LIBRARY_IS_STUB;

uint8_t emPacketValidateLibraryInfo(void)
{
  return EMBER_PACKET_VALIDATE_LIBRARY_DISABLED;
}

EmberStatus emberSetPacketValidateLibraryState(uint16_t state)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

bool emPacketValidateCheck(PacketValidateType type, PacketHeader header)
{
  return true;
}
