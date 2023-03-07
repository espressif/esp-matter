/***************************************************************************//**
 * @file app.c
 * @brief Callbacks implementation and application specific code.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "led-protocol.h"

/* This sample application demostrates an NCP using a custom protocol to
 * communicate with the host. As an example protocol, the NCP has defined
 * commands so that the host can control an LED on the NCP's RCM.  See
 * led-protocol.h for details.
 *
 * The host sends custom EZSP commands to the NCP, and the NCP acts on them
 * based on the functionality in the code found below.
 * This sample application is meant to be paired with the xncp-led
 * sample application in the NCP Application Framework.
 */
static void sendLedProtocolCommand(uint8_t command, uint32_t possibleParam)
{
  EmberStatus status;
  uint8_t commandLength = 0;
  uint8_t commandPayload[LED_PROTOCOL_MAX_FRAME_LENGTH];
  uint8_t replyLength = LED_PROTOCOL_MAX_FRAME_LENGTH;
  uint8_t replyPayload[LED_PROTOCOL_MAX_FRAME_LENGTH];

  // Set the command byte.
  commandPayload[LED_PROTOCOL_COMMAND_INDEX] = command;
  commandLength++;

  // Conditionally set the parameter.
  if (command == LED_PROTOCOL_COMMAND_SET_FREQ) {
    emberAfCopyInt32u(commandPayload, 1, possibleParam);
    commandLength += sizeof(possibleParam);
  }

  // Send the command to the NCP.
  status = ezspCustomFrame(commandLength,
                           commandPayload,
                           &replyLength,
                           replyPayload);
  sl_zigbee_app_debug_print("Send custom frame: 0x%02X\n", status);

  // If we were expecting a response, display it.
  if (command == LED_PROTOCOL_COMMAND_GET_FREQ) {
    sl_zigbee_app_debug_print(" Response (frequency): %d\n",
                              emberAfGetInt32u(replyPayload,
                                               LED_PROTOCOL_RESPONSE_INDEX,
                                               replyLength));
  } else if (command == LED_PROTOCOL_COMMAND_GET_LED) {
    uint8_t ledState = replyPayload[LED_PROTOCOL_RESPONSE_INDEX];
    sl_zigbee_app_debug_print("  Response (state): %d (%s)\n",
                              ledState,
                              ledStateNames[ledState]);
  }
}

void getFrequencyCommand(sl_cli_command_arg_t *arguments)
{
  sendLedProtocolCommand(LED_PROTOCOL_COMMAND_GET_FREQ, 0); // no param
}

void setFrequencyCommand(sl_cli_command_arg_t *arguments)
{
  uint32_t frequency = sl_cli_get_argument_uint32(arguments, 0);
  sendLedProtocolCommand(LED_PROTOCOL_COMMAND_SET_FREQ, frequency);
}

void getLedCommand(sl_cli_command_arg_t *arguments)
{
  sendLedProtocolCommand(LED_PROTOCOL_COMMAND_GET_LED, 0); // no param
}

void setLedCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t command = sl_cli_get_argument_uint8(arguments, 0);
  if (command > LED_PROTOCOL_COMMAND_STROBE_LED) {
    sl_zigbee_app_debug_print("Invalid LED command: 0x%02X\n", command);
  } else {
    sendLedProtocolCommand(command, 0); // no param
  }
}

void getInfoCommand(sl_cli_command_arg_t *arguments)
{
  uint16_t version, manufacturerId;
  EmberStatus status;

  status = ezspGetXncpInfo(&manufacturerId, &version);

  sl_zigbee_app_debug_print("Get XNCP info: status: 0x%02X\n", status);
  sl_zigbee_app_debug_print("  manufacturerId: 0x%02X, version: 0x%02X\n",
                            manufacturerId, version);
}
