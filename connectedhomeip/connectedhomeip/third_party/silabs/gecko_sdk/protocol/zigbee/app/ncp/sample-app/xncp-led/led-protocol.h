/***************************************************************************//**
 * @file
 * @brief This is the protocol used in the xncp-uart sample application to control
 * and read the state of an LED on an NCP.
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

#ifndef __LED_PROTOCOL_H__
#define __LED_PROTOCOL_H__

// The LED protocol custom EZSP frames are no longer than 5 bytes.
// These consist of 1 command byte and possibly 4 parameter bytes.
#define LED_PROTOCOL_MAX_FRAME_LENGTH (5)

// Each custom EZSP frame starts with a command.
#define LED_PROTOCOL_COMMAND_INDEX (0)

// If an LED protocol command has a response, it will be at the beginning of
// the response buffer.
#define LED_PROTOCOL_RESPONSE_INDEX (0)

// Each command is a byte.
enum {
  LED_PROTOCOL_COMMAND_SET_LED    = 0x00,
  LED_PROTOCOL_COMMAND_CLEAR_LED  = 0x01,
  LED_PROTOCOL_COMMAND_STROBE_LED = 0x02,

  // The response to this command contains a byte corresponding to one of the
  // three enum values above.
  LED_PROTOCOL_COMMAND_GET_LED    = 0x03,

  // The response to this command contains an unsigned 32-bit integer
  // which is the frequency, in ticks, of the LED in the strobe state.
  LED_PROTOCOL_COMMAND_GET_FREQ   = 0x04,

  // This command takes an unsigned 32-bit integer as a parameter.
  // This parameter is the frequency, in ticks, of the LED when in
  // a STROBE state.
  // A frequency of 0 is invalid.
  LED_PROTOCOL_COMMAND_SET_FREQ   = 0x05,
};
typedef uint8_t LedProtocolCommand;

const char * ledStateNames[] = {
  "ON",      /* LED_PROTOCOL_COMMAND_SET_LED state name */
  "OFF",     /* LED_PROTOCOL_COMMAND_CLEAR_LED state name */
  "STROBE",  /* LED_PROTOCOL_COMMAND_STROBE_LED state name */
};

#endif /* __LED_PROTOCOL_H__ */
