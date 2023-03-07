/***************************************************************************//**
 * @file
 * @brief Commands for executing manufacturing related tests
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
#include "app/framework/util/attribute-storage.h"

#ifndef EMBER_TEST
#include "token.h"
#endif

// -----------------------------------------------------------------------------
// Globals

// The max packet size for 802.15.4 is 128, minus 1 byte for the length, and 2 bytes for the CRC.
#define MAX_BUFFER_SIZE 125

// the saved information for the first packet
static uint8_t savedPktLength = 0;
static int8_t savedRssi = 0;
static uint8_t savedLinkQuality = 0;
static uint8_t savedPkt[MAX_BUFFER_SIZE];

static uint16_t mfgCurrentPacketCounter = 0;

static bool inReceivedStream = false;

static bool mfgLibRunning = false;
static bool mfgToneTestRunning = false;
static bool mfgStreamTestRunning = false;

static uint16_t  mfgTotalPacketCounter = 0;

// Add 1 for the length byte which is at the start of the buffer.
ALIGNMENT(4) // efr32xg22 parts and later need 32-bit alignment
static uint8_t   sendBuff[MAX_BUFFER_SIZE + 1];

#define PLUGIN_NAME "Mfglib"

#define MIN_CLI_MESSAGE_SIZE 3
#define MAX_CLI_MESSAGE_SIZE 16

#ifdef UC_BUILD
sl_zigbee_event_t emberAfPluginManufacturingLibraryCliCheckSendCompleteEvent;
#define checkSendCompleteEventControl (&emberAfPluginManufacturingLibraryCliCheckSendCompleteEvent)
void emberAfPluginManufacturingLibraryCliCheckSendCompleteEventHandler(SLXU_UC_EVENT);

#else
EmberEventControl emberAfPluginManufacturingLibraryCliCheckSendCompleteEventControl;
#define checkSendCompleteEventControl emberAfPluginManufacturingLibraryCliCheckSendCompleteEventControl
#endif

static uint16_t savedPacketCount = 0;

#define CHECK_SEND_COMPLETE_DELAY_QS 2

// -----------------------------------------------------------------------------
// Forward Declarations

// -----------------------------------------------------------------------------
// External APIs
// Function to determine whether the manufacturing library functionality is
// running.  This is used by the network manager and bulb ui plugins to
// determine if it is safe to kick off joining behavoir.
bool emberAfMfglibRunning(void)
{
  return mfgLibRunning;
}

// Some joining behavior kicks off before the device can receive a CLI command
// to start the manufacturing library.  Or in the case of devices that use
// UART for CLI access, they may be asleep.  In this case, we need to set a
// token that gives the manufacturing test a window of opportunity to enable
// the manufacturin library.  The idea is that fresh devices can more easily
// allow entry into the manufacturing test modes.  It is also intended to
// disable this token via CLI command at the end of manufacturing test so
// the end customer is not exposed to this functionality.
bool emberAfMfglibEnabled(void)
{
  uint8_t enabled;

#ifndef EMBER_TEST
  halCommonGetToken(&enabled, TOKEN_MFG_LIB_ENABLED);
#else
  // no op
  enabled = true;
#endif

  (void) emberSerialPrintf(APP_SERIAL,
                           "MFG_LIB Enabled %x\r\n", enabled);

  return enabled;
}

// -----------------------------------------------------------------------------

void emAfPluginManufacturingLibraryCliInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  slxu_zigbee_event_init(checkSendCompleteEventControl,
                         emberAfPluginManufacturingLibraryCliCheckSendCompleteEventHandler);
}

// This is unfortunate but there is no callback indicating when sending is complete
// for all packets.  So we must create a timer that checks whether the packet count
// has increased within the last second.

void emberAfPluginManufacturingLibraryCliCheckSendCompleteEventHandler(SLXU_UC_EVENT)
{
  slxu_zigbee_event_set_inactive(checkSendCompleteEventControl);
  if (!inReceivedStream) {
    return;
  }

  if (savedPacketCount == mfgTotalPacketCounter) {
    inReceivedStream = false;
    emberAfCorePrintln("%p Send Complete %d packets",
                       PLUGIN_NAME,
                       mfgCurrentPacketCounter);
    emberAfCorePrintln("First packet: lqi %d, rssi %d, len %d",
                       savedLinkQuality,
                       savedRssi,
                       savedPktLength);
    mfgCurrentPacketCounter = 0;
  } else {
    savedPacketCount = mfgTotalPacketCounter;
    slxu_zigbee_event_set_delay_qs(checkSendCompleteEventControl,
                                   CHECK_SEND_COMPLETE_DELAY_QS);
  }
}

static void fillBuffer(uint8_t* buff, uint8_t length, bool random)
{
  uint8_t i;
  // length byte does not include itself. If the user asks for 10
  // bytes of packet this means 1 byte length, 7 bytes, and 2 bytes CRC
  // this example will have a length byte of 9, but 10 bytes will show
  // up on the receive side
  buff[0] = length;

  for (i = 1; i < length; i += 2) {
    // Two buffer elements per iteration to use both random bytes.
    if (random) {
      uint16_t randomNumber = emberGetPseudoRandomNumber();
      buff[i] = (uint8_t)(randomNumber & 0xFF);
      buff[i + 1] = (uint8_t)((randomNumber >> 8)) & 0xFF;
    } else {
      // Test pattern is ascending integers starting from 1.
      buff[i] = i;
      buff[i + 1] = i + 1;
    }
  }
}

void ezspMfglibRxHandler(uint8_t linkQuality,
                         int8_t rssi,
                         uint8_t packetLength,
                         uint8_t *packetContents)
{
  // This increments the total packets for the whole mfglib session
  // this starts when mfglibStart is called and stops when mfglibEnd
  // is called.
  mfgTotalPacketCounter++;

  mfgCurrentPacketCounter++;

  // If this is the first packet of a transmit group then save the information
  // of the current packet. Don't do this for every packet, just the first one.
  if (!inReceivedStream) {
    inReceivedStream = true;
    mfgCurrentPacketCounter = 1;
    savedRssi = rssi;
    savedLinkQuality = linkQuality;
    savedPktLength = packetLength;
    MEMMOVE(savedPkt, packetContents, savedPktLength);
    slxu_zigbee_event_set_active(checkSendCompleteEventControl);
  }
}

void emberAfMfglibRxStatistics(uint16_t* packetsReceived,
                               int8_t* savedRssiReturn,
                               uint8_t* savedLqiReturn)
{
  *packetsReceived = mfgTotalPacketCounter;
  *savedRssiReturn = savedRssi;
  *savedLqiReturn = savedLinkQuality;
}

void emberAfMfglibStart(bool wantCallback)
{
  EmberStatus status = ezspMfglibStart(wantCallback);
  emberAfCorePrintln("%p start, status 0x%X",
                     PLUGIN_NAME,
                     status);
  if (status == EMBER_SUCCESS) {
    mfgLibRunning = true;
    mfgTotalPacketCounter = 0;
  }
}

void emAfMfglibStartCommand(SL_CLI_COMMAND_ARG)
{
  bool wantCallback = (bool)emberUnsignedCommandArgument(0);

  emberAfMfglibStart(wantCallback);
}

void emberAfMfglibStop(void)
{
  EmberStatus status = mfglibEnd();
  emberAfCorePrintln("%s end, status 0x%02X",
                     PLUGIN_NAME,
                     status);
  emberAfCorePrintln("rx %d packets while in mfg mode", mfgTotalPacketCounter);
  if (status == EMBER_SUCCESS) {
    mfgLibRunning = false;
  }
}

void emAfMfglibStopCommand(SL_CLI_COMMAND_ARG)
{
  emberAfMfglibStop();
}

void emAfMfglibToneStartCommand(SL_CLI_COMMAND_ARG)
{
  EmberStatus status = mfglibStartTone();
  emberAfCorePrintln("%s start tone 0x%02X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgToneTestRunning = true;
  }
}

void emAfMfglibToneStopCommand(SL_CLI_COMMAND_ARG)
{
  EmberStatus status = mfglibStopTone();
  emberAfCorePrintln("%s stop tone 0x%02X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgToneTestRunning = false;
  }
}

void emAfMfglibStreamStartCommand(SL_CLI_COMMAND_ARG)
{
  EmberStatus status = mfglibStartStream();
  emberAfCorePrintln("%s start stream 0x%02X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgStreamTestRunning = true;
  }
}

void emAfMfglibStreamStopCommand(SL_CLI_COMMAND_ARG)
{
  EmberStatus status = mfglibStopStream();
  emberAfCorePrintln("%s stop stream 0x%02X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgStreamTestRunning = false;
  }
}

static EmberStatus sendPacket(uint8_t *buffer, uint16_t numPackets)
{
  EmberStatus returnStatus = EMBER_SUCCESS;
  uint16_t i;
  uint16_t now, sendTime;

  // send the packet(s)
  for (i = 0; i < numPackets; i++) {
    EmberStatus status = mfglibSendPacket(buffer[0], &buffer[1]);

    // Add 10 ms delay in between sending messages. On UART hosts sometimes
    // the receiver can't keep up with the sender if the sender doesn't use
    // any delay. This helps prevent false failures - devices can hear fine
    // but they can't keep up with the sender.
    now = sendTime = halCommonGetInt32uMillisecondTick();
    while ((uint16_t)(now - sendTime) < 10) {
      now = halCommonGetInt32uMillisecondTick();
    }

    // print an error on failure
    if (status != EMBER_SUCCESS) {
      emberAfCorePrintln("mfg send err 0x%x index 0x%x\r\n\r\n", status, i);
      if (returnStatus == EMBER_SUCCESS) {
        returnStatus = status;
      }
    }
  }

  return returnStatus;
}

void emAfMfglibSendCommand(SL_CLI_COMMAND_ARG)
{
  #ifdef UC_BUILD
  bool random = (sl_cli_get_command_string(arguments, 3)[0] == 'r');
  #else
  bool random = (emberCommandName()[0] == 'r');
  #endif

  uint16_t numPackets = (uint16_t)emberUnsignedCommandArgument(0);
  uint8_t length = (uint16_t)emberUnsignedCommandArgument(1);

  if (length > MAX_BUFFER_SIZE) {
    emberAfCorePrintln("Error: Length cannot be bigger than %d", MAX_BUFFER_SIZE);
    return;
  }

  if (numPackets == 0) {
    emberAfCorePrintln("Error: Number of packets cannot be 0.");
    return;
  }

  fillBuffer(sendBuff, length, random);

  EmberStatus status = sendPacket(sendBuff, numPackets);
  emberAfCorePrintln("%p send packet, status 0x%X", PLUGIN_NAME, status);
}

void emAfMfglibSendMessageCommand(SL_CLI_COMMAND_ARG)
{
  uint8_t length = 0;
  uint8_t *message = emberStringCommandArgument(0, &length);
  uint16_t numPackets = (uint16_t)emberUnsignedCommandArgument(1);

  if (length < MIN_CLI_MESSAGE_SIZE) {
    emberAfCorePrintln("Error: Minimum length is %d bytes.", MIN_CLI_MESSAGE_SIZE);
    return;
  }

  if (length > MAX_CLI_MESSAGE_SIZE) {
    emberAfCorePrintln("Error: Maximum length is %d bytes.", MAX_CLI_MESSAGE_SIZE);
    return;
  }

  if (numPackets == 0) {
    emberAfCorePrintln("Error: Number of packets cannot be 0.");
    return;
  }

  sendBuff[0] = length + 2; // message length plus 2-byte CRC
  MEMMOVE(sendBuff + 1, message, length);
  EmberStatus status = sendPacket(sendBuff, numPackets);
  emberAfCorePrintln("%s send message, status 0x%02X", PLUGIN_NAME, status);
}

void emAfMfglibStatusCommand(SL_CLI_COMMAND_ARG)
{
  uint8_t channel = mfglibGetChannel();
  int8_t power = mfglibGetPower();
  uint16_t powerMode;
  ezspGetConfigurationValue(EZSP_CONFIG_TX_POWER_MODE, &powerMode);
  uint8_t optionsLength, options = 0;
  ezspGetValue(EZSP_VALUE_MFGLIB_OPTIONS, &optionsLength, &options);
  emberAfCorePrintln("Channel: %d", channel);
  emberAfCorePrintln("Power: %d", power);
  emberAfCorePrintln("Power Mode: 0x%04X", powerMode);
  emberAfCorePrintln("Options: 0x%02X", options);
  emberAfCorePrintln("%s running: %s", PLUGIN_NAME, (mfgLibRunning ? "yes" : "no"));
  emberAfCorePrintln("%s tone test running: %s", PLUGIN_NAME, (mfgToneTestRunning ? "yes" : "no"));
  emberAfCorePrintln("%s stream test running: %s", PLUGIN_NAME, (mfgStreamTestRunning ? "yes" : "no"));
  emberAfCorePrintln("Total %s packets received: %d", PLUGIN_NAME, mfgTotalPacketCounter);
}

void emAfMfglibSetChannelCommand(SL_CLI_COMMAND_ARG)
{
  uint8_t channel = (uint8_t)emberUnsignedCommandArgument(0);
  EmberStatus status = mfglibSetChannel(channel);
  emberAfCorePrintln("%s set channel, status 0x%02X", PLUGIN_NAME, status);
}

void emAfMfglibSetPowerAndModeCommand(SL_CLI_COMMAND_ARG)
{
  int8_t power = (int8_t)emberSignedCommandArgument(0);
  uint16_t mode = (uint16_t)emberUnsignedCommandArgument(1);
  EmberStatus status = mfglibSetPower(mode, power);
  emberAfCorePrintln("%s set power and mode, status 0x%02X", PLUGIN_NAME, status);
}

void emAfMfglibSleepCommand(SL_CLI_COMMAND_ARG)
{
  emberAfCorePrintln("%s sleep not supported", PLUGIN_NAME);
}

// Function to program a custom EUI64 into the chip.
// Example:
// plugin mfglib programEui { 01 02 03 04 05 06 07 08 }
// Note:  this command is OTP.  It only works once.  To re-run, you
// must erase the chip.
void emAfMfglibProgramEuiCommand(SL_CLI_COMMAND_ARG)
{
  EmberEUI64 eui64;

  emberAfCopyBigEndianEui64Argument(0, eui64);

  // potentially verify first few bytes for customer OUI

#ifndef EMBER_TEST
  // OK, we verified the customer OUI.  Let's program it here.
  ezspSetMfgToken(EZSP_MFG_CUSTOM_EUI_64, EUI64_SIZE, (uint8_t *) &eui64);
#endif
}

void emAfMfglibEnableMfglib(SL_CLI_COMMAND_ARG)
{
#ifndef EMBER_TEST
  uint8_t enabled = (uint8_t) emberSignedCommandArgument(0);

  halCommonSetToken(TOKEN_MFG_LIB_ENABLED, &enabled);
#endif
}

void emAfMfglibSetOptions(SL_CLI_COMMAND_ARG)
{
  uint8_t options = (uint8_t)emberUnsignedCommandArgument(0);
  EzspStatus status = ezspSetValue(EZSP_VALUE_MFGLIB_OPTIONS, 1, &options);
  emberAfCorePrintln("%s set options, status 0x%02X", PLUGIN_NAME, status);
}
