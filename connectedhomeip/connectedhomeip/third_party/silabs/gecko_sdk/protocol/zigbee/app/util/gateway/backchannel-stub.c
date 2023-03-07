/***************************************************************************//**
 * @file
 * @brief Stub code for simulating an ISA backchannel for EZSP host
 * applications.
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

#include PLATFORM_HEADER //compiler/micro specifics, types

#include "stack/include/ember-types.h"
#include "stack/include/error.h"

//------------------------------------------------------------------------------

const bool backchannelSupported = false;
bool backchannelEnable = false;
int backchannelSerialPortOffset = 0;

//------------------------------------------------------------------------------

EmberStatus backchannelStartServer(uint8_t port)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus backchannelStopServer(uint8_t port)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

int backchannelReceive(uint8_t port, char* data)
{
  return -1;
}

EmberStatus backchannelSend(uint8_t port, uint8_t * data, uint8_t length)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus backchannelGetConnection(uint8_t port,
                                     bool remapStdinStdout)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus backchannelCloseConnection(uint8_t port)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus backchannelMapStandardInputOutputToRemoteConnection(int port)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus backchannelServerPrintf(const char* formatString, ...)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus backchannelClientPrintf(uint8_t port, const char* formatString, ...)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus backchannelClientVprintf(uint8_t port,
                                     const char* formatString,
                                     va_list ap)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}
