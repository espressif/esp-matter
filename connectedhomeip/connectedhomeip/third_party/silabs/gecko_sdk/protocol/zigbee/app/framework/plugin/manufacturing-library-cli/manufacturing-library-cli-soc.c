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
#include "stack/include/mfglib.h"
#include "app/framework/util/attribute-storage.h"
#ifdef HAL_CONFIG
#include "hal-config.h"
#include "ember-hal-config.h"
#endif
// -----------------------------------------------------------------------------
// Globals
extern EmberStatus mfglibStart(void (*mfglibRxCallback)(uint8_t *packet, uint8_t linkQuality, int8_t rssi));

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
#else // !UC_BUILD
EmberEventControl emberAfPluginManufacturingLibraryCliCheckSendCompleteEventControl;
#define checkSendCompleteEventControl emberAfPluginManufacturingLibraryCliCheckSendCompleteEventControl
#endif // UC_BUILD

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

// Some joining behavoir kicks off before the device can receive a CLI command
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
  return false;
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

static void mfglibRxHandler(uint8_t *packet,
                            uint8_t linkQuality,
                            int8_t rssi)
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
    savedPktLength = *packet;
    MEMMOVE(savedPkt, (packet + 1), savedPktLength);
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
  EmberStatus status = mfglibStart(wantCallback ? mfglibRxHandler : NULL);
  emberAfCorePrintln("%p start, status 0x%X",
                     PLUGIN_NAME,
                     status);
  if (status == EMBER_SUCCESS) {
    mfgLibRunning = true;
    mfgTotalPacketCounter = 0;
  }
}

void emberAfMfglibStop(void)
{
  EmberStatus status = mfglibEnd();
  emberAfCorePrintln("%p end, status 0x%X",
                     PLUGIN_NAME,
                     status);
  emberAfCorePrintln("rx %d packets while in mfg mode", mfgTotalPacketCounter);
  if (status == EMBER_SUCCESS) {
    mfgLibRunning = false;
  }
}

#ifdef UC_BUILD

#include "app/util/serial/sl_zigbee_command_interpreter.h"

void emAfMfglibStartCommand(sl_cli_command_arg_t *arguments)
{
  bool wantCallback = sl_cli_get_argument_uint8(arguments, 0);

  emberAfMfglibStart(wantCallback);
}

void emAfMfglibStopCommand(sl_cli_command_arg_t *arguments)
{
  emberAfMfglibStop();
}

void emAfMfglibToneStartCommand(sl_cli_command_arg_t *arguments)
{
  EmberStatus status = mfglibStartTone();
  emberAfCorePrintln("%p start tone 0x%X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgToneTestRunning = true;
  }
}

void emAfMfglibToneStopCommand(sl_cli_command_arg_t *arguments)
{
  EmberStatus status = mfglibStopTone();
  emberAfCorePrintln("%p stop tone 0x%X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgToneTestRunning = false;
  }
}

void emAfMfglibStreamStartCommand(sl_cli_command_arg_t *arguments)
{
  EmberStatus status = mfglibStartStream();
  emberAfCorePrintln("%p start stream 0x%X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgStreamTestRunning = true;
  }
}

void emAfMfglibStreamStopCommand(sl_cli_command_arg_t *arguments)
{
  EmberStatus status = mfglibStopStream();
  emberAfCorePrintln("%p stop stream 0x%X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgStreamTestRunning = false;
  }
}

void emAfMfglibSendCommand(sl_cli_command_arg_t *arguments)
{
  bool random = memcmp(sl_cli_get_command_string(arguments, 3 /*arguments->arg_ofs - 1 */),
                       "random", strlen("random")) == 0;
  uint16_t numPackets = sl_cli_get_argument_uint16(arguments, 0);
  uint8_t length = sl_cli_get_argument_uint8(arguments, 1);

  if (length > MAX_BUFFER_SIZE) {
    emberAfCorePrintln("Error: Length cannot be bigger than %d", MAX_BUFFER_SIZE);
    return;
  }

  if (numPackets == 0) {
    emberAfCorePrintln("Error: Number of packets cannot be 0.");
    return;
  }

  fillBuffer(sendBuff, length, random);

  // The second parameter to the mfglibSendPacket() is the
  // number of "repeats", therefore we decrement numPackets by 1.
  numPackets--;
  EmberStatus status = mfglibSendPacket(sendBuff, numPackets);
  emberAfCorePrintln("%p send packet, status 0x%X", PLUGIN_NAME, status);
}

void emAfMfglibSendMessageCommand(sl_cli_command_arg_t *arguments)
{
  size_t length = 0;
  uint8_t *message = sl_cli_get_argument_hex(arguments, 0, &length);
  uint16_t numPackets = sl_cli_get_argument_uint16(arguments, 1);

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
  numPackets--;
  EmberStatus status = mfglibSendPacket(sendBuff, numPackets);
  emberAfCorePrintln("%p send message, status 0x%X", PLUGIN_NAME, status);
}

void emAfMfglibStatusCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t channel = mfglibGetChannel();
  int8_t power = mfglibGetPower();
  uint16_t powerMode = emberGetTxPowerMode();
  uint8_t options = mfglibGetOptions();
  emberAfCorePrintln("Channel: %d", channel);
  emberAfCorePrintln("Power: %d", power);
  emberAfCorePrintln("Power Mode: 0x%2X", powerMode);
  emberAfCorePrintln("Options: 0x%X", options);
  emberAfCorePrintln("%p running: %p", PLUGIN_NAME, (mfgLibRunning ? "yes" : "no"));
  emberAfCorePrintln("%p tone test running: %p", PLUGIN_NAME, (mfgToneTestRunning ? "yes" : "no"));
  emberAfCorePrintln("%p stream test running: %p", PLUGIN_NAME, (mfgStreamTestRunning ? "yes" : "no"));
  emberAfCorePrintln("Total %p packets received: %d", PLUGIN_NAME, mfgTotalPacketCounter);
}

void emAfMfglibSetChannelCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t channel = sl_cli_get_argument_uint8(arguments, 0);
  EmberStatus status = mfglibSetChannel(channel);
  emberAfCorePrintln("%p set channel, status 0x%X", PLUGIN_NAME, status);
}

void emAfMfglibSetPowerAndModeCommand(sl_cli_command_arg_t *arguments)
{
  int8_t power = sl_cli_get_argument_uint8(arguments, 0);
  uint16_t mode = sl_cli_get_argument_uint16(arguments, 1);
  EmberStatus status = mfglibSetPower(mode, power);
  emberAfCorePrintln("%p set power and mode, status 0x%X", PLUGIN_NAME, status);
}

void emAfMfglibSleepCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  // In UC, Zigbee sits on top of the Legacy HAL component instead of the old
  // Ember HAL (previously located in platform/base). Whereas the Ember HAL
  // would allow applications to essentially force sleep, the Power Manager
  // component does not. We could try requesting it here (by calling
  // sl_power_manager_sleep), but there's no guarantee it would succeed (in
  // fact, it will likely fail under many circumstances). Furthermore, even
  // whether it succeeded requires additional code and increases complexity,
  // leaving aside how to handle the case where it fails. The best course of
  // action is to deprecate this command. I am leaving in place with an error
  // message for now to notify anyone who used it in the past.
  emberAfCorePrintln("Error: %p no longer supports forced sleep", PLUGIN_NAME);
}

// Function to program a custom EUI64 into the chip.
// Example:
// plugin mfglib programEui { 01 02 03 04 05 06 07 08 }
// Note:  this command is OTP.  It only works once.  To re-run, you
// must erase the chip.
void emAfMfglibProgramEuiCommand(sl_cli_command_arg_t *arguments)
{
  EmberEUI64 eui64;

  sl_zigbee_copy_eui64_arg(arguments, 0, eui64, true);

  // potentially verify first few bytes for customer OUI

#ifndef EMBER_TEST
  // OK, we verified the customer OUI.  Let's program it here.
  halInternalSetMfgTokenData(TOKEN_MFG_CUSTOM_EUI_64, (uint8_t *) &eui64, EUI64_SIZE);
#endif
}

void emAfMfglibEnableMfglib(sl_cli_command_arg_t *arguments)
{
#ifndef EMBER_TEST
  uint8_t enabled = sl_cli_get_argument_uint8(arguments, 0);

  halCommonSetToken(TOKEN_MFG_LIB_ENABLED, &enabled);
#endif
}

void emAfMfglibSetOptions(sl_cli_command_arg_t *arguments)
{
  uint8_t options = sl_cli_get_argument_uint8(arguments, 0);
  EmberStatus status = mfglibSetOptions(options);
  emberAfCorePrintln("%p set options, status 0x%X", PLUGIN_NAME, status);
}

#else // !UC_BUILD

// -----------------------------------------------------------------------------
// Forward Declarations

void emAfMfglibStartCommand(void)
{
  bool wantCallback = (bool)emberUnsignedCommandArgument(0);

  emberAfMfglibStart(wantCallback);
}

void emAfMfglibStopCommand(void)
{
  emberAfMfglibStop();
}

void emAfMfglibToneStartCommand(void)
{
  EmberStatus status = mfglibStartTone();
  emberAfCorePrintln("%p start tone 0x%X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgToneTestRunning = true;
  }
}

void emAfMfglibToneStopCommand(void)
{
  EmberStatus status = mfglibStopTone();
  emberAfCorePrintln("%p stop tone 0x%X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgToneTestRunning = false;
  }
}

void emAfMfglibStreamStartCommand(void)
{
  EmberStatus status = mfglibStartStream();
  emberAfCorePrintln("%p start stream 0x%X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgStreamTestRunning = true;
  }
}

void emAfMfglibStreamStopCommand(void)
{
  EmberStatus status = mfglibStopStream();
  emberAfCorePrintln("%p stop stream 0x%X", PLUGIN_NAME, status);
  if (status == EMBER_SUCCESS) {
    mfgStreamTestRunning = false;
  }
}

void emAfMfglibSendCommand(void)
{
  bool random = (emberCommandName()[0] == 'r');
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

  // The second parameter to the mfglibSendPacket() is the
  // number of "repeats", therefore we decrement numPackets by 1.
  numPackets--;
  EmberStatus status = mfglibSendPacket(sendBuff, numPackets);
  emberAfCorePrintln("%p send packet, status 0x%X", PLUGIN_NAME, status);
}

void emAfMfglibSendMessageCommand(void)
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
  numPackets--;
  EmberStatus status = mfglibSendPacket(sendBuff, numPackets);
  emberAfCorePrintln("%p send message, status 0x%X", PLUGIN_NAME, status);
}

void emAfMfglibStatusCommand(void)
{
  uint8_t channel = mfglibGetChannel();
  int8_t power = mfglibGetPower();
  uint16_t powerMode = emberGetTxPowerMode();
  uint8_t options = mfglibGetOptions();
  emberAfCorePrintln("Channel: %d", channel);
  emberAfCorePrintln("Power: %d", power);
  emberAfCorePrintln("Power Mode: 0x%2X", powerMode);
  emberAfCorePrintln("Options: 0x%X", options);
  emberAfCorePrintln("%p running: %p", PLUGIN_NAME, (mfgLibRunning ? "yes" : "no"));
  emberAfCorePrintln("%p tone test running: %p", PLUGIN_NAME, (mfgToneTestRunning ? "yes" : "no"));
  emberAfCorePrintln("%p stream test running: %p", PLUGIN_NAME, (mfgStreamTestRunning ? "yes" : "no"));
  emberAfCorePrintln("Total %p packets received: %d", PLUGIN_NAME, mfgTotalPacketCounter);
}

void emAfMfglibSetChannelCommand(void)
{
  uint8_t channel = (uint8_t)emberUnsignedCommandArgument(0);
  EmberStatus status = mfglibSetChannel(channel);
  emberAfCorePrintln("%p set channel, status 0x%X", PLUGIN_NAME, status);
}

void emAfMfglibSetPowerAndModeCommand(void)
{
  int8_t power = (int8_t)emberSignedCommandArgument(0);
  uint16_t mode = (uint16_t)emberUnsignedCommandArgument(1);
  EmberStatus status = mfglibSetPower(mode, power);
  emberAfCorePrintln("%p set power and mode, status 0x%X", PLUGIN_NAME, status);
}

void emAfMfglibSleepCommand(void)
{
  uint32_t sleepDurationMS = (uint32_t)emberUnsignedCommandArgument(0);

  // turn off the radio
  emberStackPowerDown();

  ATOMIC(
    // turn off board and peripherals
    halPowerDown();
    // turn micro to power save mode - wakes on external interrupt
    // or when the time specified expires
    halSleepForMilliseconds(&sleepDurationMS);
    // power up board and peripherals
    halPowerUp();
    );
  // power up radio
  emberStackPowerUp();

  emberAfEepromNoteInitializedStateCallback(false);

  // Allow the stack to time out any of its events and check on its
  // own network state.
  emberTick();
}

// Function to program a custom EUI64 into the chip.
// Example:
// plugin mfglib programEui { 01 02 03 04 05 06 07 08 }
// Note:  this command is OTP.  It only works once.  To re-run, you
// must erase the chip.
void emAfMfglibProgramEuiCommand(void)
{
  EmberEUI64 eui64;

  emberAfCopyBigEndianEui64Argument(0, eui64);

  // potentially verify first few bytes for customer OUI

#ifndef EMBER_TEST
  // OK, we verified the customer OUI.  Let's program it here.
  halInternalSetMfgTokenData(TOKEN_MFG_CUSTOM_EUI_64, (uint8_t *) &eui64, EUI64_SIZE);
#endif
}

void emAfMfglibEnableMfglib(void)
{
#ifndef EMBER_TEST
  uint8_t enabled = (uint8_t) emberSignedCommandArgument(0);

  halCommonSetToken(TOKEN_MFG_LIB_ENABLED, &enabled);
#endif
}

void emAfMfglibSetOptions(void)
{
  uint8_t options = (uint8_t)emberUnsignedCommandArgument(0);
  EmberStatus status = mfglibSetOptions(options);
  emberAfCorePrintln("%p set options, status 0x%X", PLUGIN_NAME, status);
}

#endif // UC_BUILD
