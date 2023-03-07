/***************************************************************************//**
 * @file
 * @brief Programmable NCP stubs.
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

#include "xncp.h"
#include "app/util/ezsp/ezsp-frame-utilities.h"

bool emXNcpAllowIncomingEzspCommand(void)
{
  return true;
}

bool emXNcpAllowOutgoingEzspCallback(uint8_t *callback,
                                     uint8_t callbackLength)
{
  (void)callback;
  (void)callbackLength;
  return true;
}

bool emAfPluginXncpIncomingMessageCallback(EmberIncomingMessageType type,
                                           EmberApsFrame *apsFrame,
                                           EmberMessageBuffer message)
{
  (void)type;
  (void)apsFrame;
  (void)message;
  return false;
}

void emXNcpHandleIncomingCustomEzspMessage(void)
{
  appendInt8u(EMBER_INVALID_CALL);
  appendInt8u(0); // Bug 14838: We always need to append the reply length
}
void emXNcpHandleGetInfoCommand(void)
{
  appendInt8u(EMBER_INVALID_CALL);
}

void emberAfPluginXncpGetXncpInformation(uint16_t *manufacturerId,
                                         uint16_t *versionNumber)
{
  *versionNumber = *manufacturerId = 0;
}
