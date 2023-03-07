/***************************************************************************//**
 * @file
 * @brief Commands for executing manufacturing-related tests.
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

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "stack/include/mfglib.h"
#include "app/framework/util/attribute-storage.h"
#include "app/framework/plugin/manufacturing-library-cli/manufacturing-library-cli-plugin.h"

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_BULB_UI_PRESENT
#include "bulb-ui.h"
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_BULB_UI
#include "app/framework/plugin-soc/bulb-ui/bulb-ui.h"
#define SL_CATALOG_ZIGBEE_BULB_UI_PRESENT
#endif
#endif // UC_BUILD

// -----------------------------------------------------------------------------
// Globals
enum {
  ACTIVE_COMMAND_NONE = 0x00,
  ACTIVE_COMMAND_TONE = 0x01,
  ACTIVE_COMMAND_STREAM = 0x02,
  ACTIVE_COMMAND_RX_MODE = 0x03
};

enum {
  KICKOFF_IDLE             = 0x00,
  KICKOFF_COMMAND_RECEIVED = 0x01,
  KICKOFF_LEAVING_NETWORK  = 0x02,
  KICKOFF_MFGLIB_ENABLED   = 0x03
};

#define KICKOFF_COMMAND_RECEIVED_WAIT_MS 2000
#define KICKOFF_LEAVINGNETWORK_WAIT_MS   1000
#define KIKOFF_MFGLIB_ENABLED_WAIT_MS    2000
#define TIMEOUT_DELAY_MFGLIB_STOP        1000

#ifdef UC_BUILD
sl_zigbee_event_t emberAfPluginManufacturingLibraryOtaKickoffEvent;
#define kickoffControl (&emberAfPluginManufacturingLibraryOtaKickoffEvent)
void emberAfPluginManufacturingLibraryOtaKickoffEventHandler(SLXU_UC_EVENT);
sl_zigbee_event_t emberAfPluginManufacturingLibraryOtaTimeoutEvent;
#define timeoutControl (&emberAfPluginManufacturingLibraryOtaTimeoutEvent)
void emberAfPluginManufacturingLibraryOtaTimeoutEventHandler(SLXU_UC_EVENT);
#else // !UC_BUILD
EmberEventControl emberAfPluginManufacturingLibraryOtaKickoffEventControl;
EmberEventControl emberAfPluginManufacturingLibraryOtaTimeoutEventControl;
#define kickoffControl emberAfPluginManufacturingLibraryOtaKickoffEventControl
#define timeoutControl emberAfPluginManufacturingLibraryOtaTimeoutEventControl
#endif // UC_BUILD

static uint8_t currentActiveCommand;
static uint8_t savedChannel;
static int8_t  savedPower;
static uint16_t savedTime;
static uint8_t savedEndpoint;
static uint8_t savedOnOff;
static uint8_t kickoffState;

// Forward declarations
void emAfMfglibSendCommand(void);

// Callbacks and functions

void emberAfMfglibClusterServerInitCallback(int8u endpoint)
{
  slxu_zigbee_event_init(kickoffControl,
                         emberAfPluginManufacturingLibraryOtaKickoffEventHandler);
  slxu_zigbee_event_init(timeoutControl,
                         emberAfPluginManufacturingLibraryOtaTimeoutEventHandler);

  currentActiveCommand = ACTIVE_COMMAND_NONE;
}

static void saveStateLeaveNetwork(uint8_t channel,
                                  int8_t power,
                                  uint16_t time)
{
  savedChannel = channel;
  savedPower = power;
  savedTime = time;
  savedEndpoint = emberAfCurrentEndpoint();

  emberAfReadServerAttribute(savedEndpoint,
                             ZCL_ON_OFF_CLUSTER_ID,
                             ZCL_ON_OFF_ATTRIBUTE_ID,
                             (uint8_t *)&savedOnOff,
                             sizeof(savedOnOff));

  // set up kickoff state machine
  kickoffState = KICKOFF_COMMAND_RECEIVED;
  slxu_zigbee_event_set_delay_ms(kickoffControl,
                                 KICKOFF_COMMAND_RECEIVED_WAIT_MS);
}

bool emberAfMfglibClusterToneCallback(uint8_t channel,
                                      int8_t power,
                                      uint16_t time)
{
  currentActiveCommand = ACTIVE_COMMAND_TONE;

  saveStateLeaveNetwork(channel, power, time);

  return TRUE;
}

bool emberAfMfglibClusterStreamCallback(uint8_t channel,
                                        int8_t power,
                                        uint16_t time)
{
  currentActiveCommand = ACTIVE_COMMAND_STREAM;

  saveStateLeaveNetwork(channel, power, time);

  return TRUE;
}

bool emberAfMfglibClusterRxModeCallback(uint8_t channel,
                                        int8_t power,
                                        uint16_t time)
{
  currentActiveCommand = ACTIVE_COMMAND_RX_MODE;

  saveStateLeaveNetwork(channel, power, time);

  return TRUE;
}

static void mfglibSetupGeneric(void)
{
#ifndef EMBER_TEST
  tokTypeMfgPhyConfig phyConfigToken, phyConfig;
  halCommonGetMfgToken(&phyConfigToken, TOKEN_MFG_PHY_CONFIG);
  phyConfig = ~phyConfigToken;
#else
  uint16_t phyConfig = 0;
#endif

  emberAfMfglibStart(true);
  mfglibSetChannel(savedChannel);
  mfglibSetPower(phyConfig, savedPower);
  (void) emberAfWriteAttribute(savedEndpoint,
                               ZCL_ON_OFF_CLUSTER_ID,
                               ZCL_ON_OFF_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               (uint8_t *)&savedOnOff,
                               ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}

void emberAfPluginManufacturingLibraryOtaStackStatusCallback(EmberStatus status)
{
  if (status != EMBER_NETWORK_DOWN) {
    return;
  }

  if (currentActiveCommand == ACTIVE_COMMAND_NONE) {
    return;
  }

  slxu_zigbee_event_set_delay_ms(kickoffControl,
                                 KICKOFF_LEAVINGNETWORK_WAIT_MS);
}

void emberAfPluginManufacturingLibraryOtaKickoffEventHandler(SLXU_UC_EVENT)
{
  EmberStatus status;

  slxu_zigbee_event_set_inactive(
    kickoffControl);

  switch (kickoffState) {
    case KICKOFF_IDLE:
      return;
      break;

    case KICKOFF_COMMAND_RECEIVED:
      emberLeaveNetwork();

      // Note:  I schedule the timeout after the stack status handler call.
      kickoffState = KICKOFF_LEAVING_NETWORK;
      break;

    case KICKOFF_LEAVING_NETWORK:
      mfglibSetupGeneric();
      slxu_zigbee_event_set_delay_ms(kickoffControl,
                                     KIKOFF_MFGLIB_ENABLED_WAIT_MS);
      kickoffState = KICKOFF_MFGLIB_ENABLED;

      break;

    case KICKOFF_MFGLIB_ENABLED:
      switch (currentActiveCommand) {
        case ACTIVE_COMMAND_TONE:
          status = mfglibStartTone();
          emberAfCorePrintln("Tone Start %x", status);
          break;
        case ACTIVE_COMMAND_STREAM:
          status = mfglibStartStream();
          emberAfCorePrintln("Stream Start %x", status);
          break;
        case ACTIVE_COMMAND_RX_MODE:
          // no calls required for mfglib for RX mode
          emberAfCorePrintln("RX Mode");
          break;
        default:
          break;
      }

      if (savedTime > 0) {
        // if saved time is 0, we want to enter mfglib mode forever.  If not,
        // we have a legit time, in milliseconds, after which we wish to exit.
        slxu_zigbee_event_set_delay_ms(
          timeoutControl,
          savedTime);
      }

      break;

    default:
      break;
  }
}

static void saveRxStatistics(void)
{
  uint16_t packetsReceived;
  int8_t   savedRssi;
  uint8_t  savedLqi;
  EmberStatus status;

  emberAfMfglibRxStatistics(&packetsReceived, &savedRssi, &savedLqi);

  status =
    emberAfWriteManufacturerSpecificServerAttribute(savedEndpoint,
                                                    ZCL_MFGLIB_CLUSTER_ID,
                                                    ZCL_PACKETS_RECEIVED_ATTRIBUTE_ID,
                                                    0x1002,
                                                    (uint8_t *) &packetsReceived,
                                                    ZCL_INT16U_ATTRIBUTE_TYPE);

  status =
    emberAfWriteManufacturerSpecificServerAttribute(savedEndpoint,
                                                    ZCL_MFGLIB_CLUSTER_ID,
                                                    ZCL_SAVED_RSSI_ATTRIBUTE_ID,
                                                    0x1002,
                                                    (uint8_t *) &savedRssi,
                                                    ZCL_INT8S_ATTRIBUTE_TYPE);

  status =
    emberAfWriteManufacturerSpecificServerAttribute(savedEndpoint,
                                                    ZCL_MFGLIB_CLUSTER_ID,
                                                    ZCL_SAVED_LQI_ATTRIBUTE_ID,
                                                    0x1002,
                                                    (uint8_t *) &savedLqi,
                                                    ZCL_INT8U_ATTRIBUTE_TYPE);
}

void emberAfPluginManufacturingLibraryOtaTimeoutEventHandler(SLXU_UC_EVENT)
{
  slxu_zigbee_event_set_inactive(timeoutControl);

  if (currentActiveCommand == ACTIVE_COMMAND_NONE) {
    emberAfMfglibStop();
    // Now time to kick off the join process, depending on the relevent UI
    // plugin
    return;
  }

  // Still have an active command.  We need to disable state and kick off a
  // delay to wait for the manufacturing library to stop.
  slxu_zigbee_event_set_delay_ms(timeoutControl, TIMEOUT_DELAY_MFGLIB_STOP);

  if (currentActiveCommand == ACTIVE_COMMAND_TONE) {
    mfglibStopTone();
  }

  if (currentActiveCommand == ACTIVE_COMMAND_STREAM) {
    mfglibStopStream();
  }

  if (currentActiveCommand == ACTIVE_COMMAND_RX_MODE) {
    saveRxStatistics();
  }

  currentActiveCommand = ACTIVE_COMMAND_NONE;
}
