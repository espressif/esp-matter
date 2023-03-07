/***************************************************************************//**
 * @file
 * @brief This file defines the interface to the host to send Ember proprietary
 * bootloader messages.
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

// *****************************************************************************
// * bootloader-message-host.c
// *
// *
// *
// * Copyright 2012 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include "app/framework/include/af.h"
#include "bootloader-protocol.h"

//------------------------------------------------------------------------------
// Globals

//------------------------------------------------------------------------------
// Functions

EmberStatus emAfSendBootloadMessage(bool isBroadcast,
                                    EmberEUI64 destEui64,
                                    uint8_t length,
                                    uint8_t* message)
{
  return ezspSendBootloadMessage(isBroadcast,
                                 destEui64,
                                 length,
                                 message);
}

void ezspIncomingBootloadMessageHandler(EmberEUI64 longId,
                                        uint8_t lastHopLqi,
                                        int8_t lastHopRssi,
                                        uint8_t messageLength,
                                        uint8_t* messageContents)
{
  if (messageLength > MAX_BOOTLOAD_MESSAGE_SIZE) {
    bootloadPrintln("Bootload message too long (%d > %d), dropping!",
                    messageLength,
                    MAX_BOOTLOAD_MESSAGE_SIZE);
    return;
  }

  emberAfPluginStandaloneBootloaderCommonIncomingMessageCallback(longId,
                                                                 messageLength,
                                                                 messageContents);
}

void ezspBootloadTransmitCompleteHandler(EmberStatus status,
                                         uint8_t messageLength,
                                         uint8_t *messageContents)
{
  if (status != EMBER_SUCCESS) {
    uint8_t commandId = 0xFF;
    if (messageLength >= 2) {
      commandId = messageContents[1];
    }
    bootloadPrintln("Bootload message (0x%X) send failed: 0x%X",
                    commandId,
                    status);
  }
}

void emAfStandaloneBootloaderClientEncrypt(uint8_t* block, uint8_t* key)
{
  uint8_t temp[EMBER_ENCRYPTION_KEY_SIZE];
  ezspAesEncrypt(block, key, temp);
  MEMMOVE(block, temp, EMBER_ENCRYPTION_KEY_SIZE);
}
