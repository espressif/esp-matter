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

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "xncp.h"
#include "hal.h"
#include "zigbee_app_framework_common.h"
#include "led-protocol.h"

#if defined(SL_CATALOG_LED0_PRESENT)
#include "sl_led.h"
#include "sl_simple_led_instances.h"
#define led_turn_on(led) sl_led_turn_on(led)
#define led_turn_off(led) sl_led_turn_off(led)
#define led_toggle(led) sl_led_toggle(led)
#define LED (&sl_led_led0)
#else // !SL_CATALOG_LED0_PRESENT
#define led_turn_on(led)
#define led_turn_off(led)
#define led_toggle(led)
#endif // SL_CATALOG_LED0_PRESENT

/* This sample application demostrates an NCP using a custom protocol to
 * communicate with the host. As an example protocol, the NCP has defined
 * commands so that the host can control an LED on the NCP's RCM.  See
 * led-protocol.h for details.
 *
 * The host sends custom EZSP commands to the NCP, and the NCP acts on them
 * based on the functionality in the code found below.
 * This sample application is meant to be paired with the XncpSensorHost
 * sample application in the ZCL Application Framework.
 */

#define XNCP_VERSION_NUMBER  0x1234
#define XNCP_MANUFACTURER_ID 0xABCD

static uint8_t led_state;
static uint32_t led_period = MILLISECOND_TICKS_PER_SECOND;
static sl_zigbee_event_t led_event;

//---------------
// Event handlers

static void led_event_handler(sl_zigbee_event_t *event)
{
  switch (led_state) {
    case LED_PROTOCOL_COMMAND_CLEAR_LED:
      sl_led_turn_off(LED);
      break;
    case LED_PROTOCOL_COMMAND_SET_LED:
      sl_led_turn_on(LED);
      break;
    case LED_PROTOCOL_COMMAND_STROBE_LED:
      sl_led_toggle(LED);
      break;
    default:
      ; // TODO: handler error.
  }

  sl_zigbee_event_set_delay_ms(&led_event, led_period);
}

//----------------------
// Implemented Callbacks

/** @brief Main Init
 *
 * This function is called when the application starts and can be used to
 * perform any additional initialization required at system startup.
 */
void emberAfMainInitCallback(void)
{
  sl_zigbee_event_init(&led_event, led_event_handler);
  sl_zigbee_event_set_active(&led_event);
}

/** @brief Incoming Custom EZSP Message Callback
 *
 * This function is called when the NCP receives a custom EZSP message from the
 * HOST.  The message length and payload is passed to the callback in the first
 * two arguments.  The implementation can then fill in the replyPayload and set
 * the replayPayloadLength to the number of bytes in the replyPayload.
 * See documentation for the function ezspCustomFrame on sending these messages
 * from the HOST.
 *
 * @param messageLength The length of the messagePayload.
 * @param messagePayload The custom message that was sent from the HOST.
 * Ver.: always
 * @param replyPayloadLength The length of the replyPayload.  This needs to be
 * set by the implementation in order for a properly formed respose to be sent
 * back to the HOST. Ver.: always
 * @param replyPayload The custom message to send back to the HOST in respose
 * to the custom message. Ver.: always
 *
 * @return An ::EmberStatus indicating the result of the custom message
 * handling.  This returned status is always the first byte of the EZSP
 * response.
 */
EmberStatus emberAfPluginXncpIncomingCustomFrameCallback(uint8_t messageLength,
                                                         uint8_t *messagePayload,
                                                         uint8_t *replyPayloadLength,
                                                         uint8_t *replyPayload)
{
  // First byte is the command ID.
  uint8_t commandId = messagePayload[LED_PROTOCOL_COMMAND_INDEX];
  EmberStatus status = EMBER_SUCCESS;
  *replyPayloadLength = 0;

  switch (commandId) {
    case LED_PROTOCOL_COMMAND_SET_LED:
      led_state = LED_PROTOCOL_COMMAND_SET_LED;
      break;
    case LED_PROTOCOL_COMMAND_CLEAR_LED:
      led_state = LED_PROTOCOL_COMMAND_CLEAR_LED;
      break;
    case LED_PROTOCOL_COMMAND_STROBE_LED:
      led_state = LED_PROTOCOL_COMMAND_STROBE_LED;
      break;
    case LED_PROTOCOL_COMMAND_GET_LED:
      replyPayload[0] = led_state;
      *replyPayloadLength += sizeof(led_state);
      break;
    case LED_PROTOCOL_COMMAND_GET_FREQ:
      emberStoreLowHighInt32u(replyPayload, led_period);
      *replyPayloadLength += sizeof(led_period);
      break;
    case LED_PROTOCOL_COMMAND_SET_FREQ: {
      // Get the frequency parameter from one index past the command.
      // A frequency of 0 is invalid, as mentioned in the protocol.
      // The message should be longer than 4 bytes, since it should consist of
      // a command and a 32-bit integer.
      uint32_t frequency = emberFetchLowHighInt32u(messagePayload + 1);
      if (frequency || messageLength < 4) {
        led_period = frequency;
      } else {
        status = EMBER_BAD_ARGUMENT;
      }
    }
    break;
    default:
      status = EMBER_INVALID_CALL;
  }

  return status;
}

/** @brief Get XNCP Information
 *
 * This callback enables users to communicate the version number and
 * manufacturer ID of their NCP application to the framework. This information
 * is needed for the EZSP command frame called getXncpInfo. This callback will
 * be called when that frame is received so that the application can report
 * its version number and manufacturer ID to be sent back to the HOST.
 *
 * @param versionNumber The version number of the NCP application.
 * @param manufacturerId The manufacturer ID of the NCP application.
 */
void emberAfPluginXncpGetXncpInformation(uint16_t *manufacturerId,
                                         uint16_t *versionNumber)
{
  *versionNumber = XNCP_VERSION_NUMBER;
  *manufacturerId = XNCP_MANUFACTURER_ID;
}

/** @brief
 *
 * Application framework equivalent of ::emberRadioNeedsCalibratingHandler
 */
void emberAfRadioNeedsCalibratingCallback(void)
{
  sl_mac_calibrate_current_channel();
}
