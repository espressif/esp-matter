/***************************************************************************//**
 * @file
 * @brief declarations for common app code.
 *
 * The common library is deprecated and will be removed in a future release.
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

#ifndef SILABS_APP_UTIL_COMMON_H
#define SILABS_APP_UTIL_COMMON_H

// TODO: EMZIGBEE-6322 Remove this file when doing the UC_BUILD cleanup works.
#include "app/util/common/uc-temp-macros.h"

extern uint8_t serialPort;
extern SerialBaudRate serialBaudRate;

// For identifying ourselves over the serial port.
extern const char * applicationString;

// Boilerplate
void configureSerial(uint8_t port, SerialBaudRate rate);
void toggleBlinker(void);

#if !defined EZSP_HOST
extern EmberEventControl blinkEvent;
#endif

void initialize(void);
void run(EmberEventData* events,
         void (* heartbeat)(void));

// Utilities
void printLittleEndianEui64(uint8_t port, EmberEUI64 eui64);
void printBigEndianEui64(uint8_t port, EmberEUI64 eui64);
#define printEui64(port, eui64) printBigEndianEui64((port), (eui64))
void printHexByteArray(uint8_t port, uint8_t *byteArray, uint8_t length);
#define printEncryptionKey(port, keyData) \
  printHexByteArray(port, keyData, EMBER_ENCRYPTION_KEY_SIZE)

uint8_t asciiHexToByteArray(uint8_t *bytesOut, uint8_t* asciiIn, uint8_t asciiCharLength);

void hexToEui64(uint8_t *contents, uint8_t length, EmberEUI64 returnEui64);
void hexToKeyData(uint8_t *contents, uint8_t length, EmberKeyData* returnKeyData);
uint8_t hexDigitValue(uint8_t digit);

// Host / Non-host specific implementations
bool setSecurityLevel(uint8_t level);
bool generateRandomBytes(uint8_t* result, uint8_t length);
bool getNetworkParameters(EmberNodeType* nodeType,
                          EmberNetworkParameters* networkParams);
void initializeEmberStack(void);
EmberStatus getOnlineNodeParameters(uint8_t* childCountReturn,
                                    uint8_t* routerCountReturn,
                                    EmberNodeType* nodeTypeReturn,
                                    EmberNodeId* parentNodeIdReturn,
                                    EmberEUI64 parentEuiReturn,
                                    EmberNetworkParameters* networkParamReturn);
EmberStatus getOfflineNodeParameters(EmberNodeId *myNodeIdReturn,
                                     EmberNodeType *myNodeTypeReturn,
                                     uint8_t* stackProfileReturn);
void runEvents(EmberEventData* events);

#define generateRandomKey(result) \
  (EMBER_SUCCESS == emberGenerateRandomKey(result))

void createMulticastBinding(uint8_t index, uint8_t *multicastGroup, uint8_t endpoint);
bool findEui64Binding(EmberEUI64 key, uint8_t *index);
void printCommandStatus(EmberStatus status, const char * good, const char * bad);
void printCommandStatusWithPrefix(EmberStatus status,
                                  const char * prefix,
                                  const char * good,
                                  const char * bad);
void printOperationStatus(EmberStatus status, const char * operation);
void setCommissionParameters(SL_CLI_COMMAND_ARG);
EmberStatus joinNetwork(void);

// Common commands
void helpCommand(SL_CLI_COMMAND_ARG);
void statusCommand(SL_CLI_COMMAND_ARG);
void stateCommand(SL_CLI_COMMAND_ARG);
void rebootCommand(SL_CLI_COMMAND_ARG);
void radioCommand(SL_CLI_COMMAND_ARG);
void formNetworkCommand(SL_CLI_COMMAND_ARG);
void multiPhyStartCommand(SL_CLI_COMMAND_ARG);
void multiPhyStopCommand(SL_CLI_COMMAND_ARG);
void setExtPanIdCommand(SL_CLI_COMMAND_ARG);
void setJoinMethod(SL_CLI_COMMAND_ARG);
void joinNetworkCommand(SL_CLI_COMMAND_ARG);
void rejoinCommand(SL_CLI_COMMAND_ARG);
void networkInitCommand(SL_CLI_COMMAND_ARG);
void leaveNetworkCommand(SL_CLI_COMMAND_ARG);
void leaveWithOptionsNetworkCommand(SL_CLI_COMMAND_ARG);
void addressRequestCommand(SL_CLI_COMMAND_ARG);
void permitJoiningCommand(SL_CLI_COMMAND_ARG);
void setSecurityCommand(SL_CLI_COMMAND_ARG);
void setNetworkCommand(SL_CLI_COMMAND_ARG);
void getCurrentNwkIndexCommand(SL_CLI_COMMAND_ARG);
void getRadioParameters(SL_CLI_COMMAND_ARG);

void printCarriageReturn(void);
void printErrorMessage(const char * message);
void printErrorMessage2(const char * message, const char * message2);

#define MULTI_PHY_COMMON_COMMANDS                                       \
  emberCommandEntryAction("multi_start", multiPhyStartCommand, "uusu*", \
                          "Start second phy"),                          \
  emberCommandEntryAction("multi_stop", multiPhyStopCommand, "u*",      \
                          "Stop multi phy interface"),

#define COMMON_COMMANDS                                                              \
  emberCommandEntryAction("status", statusCommand, "",                               \
                          "Print the node's current status"),                        \
  emberCommandEntryAction("state", stateCommand, "",                                 \
                          "Print the node's current state"),                         \
  emberCommandEntryAction("reboot", rebootCommand, "",                               \
                          "Reboot the node"),                                        \
                                                                                     \
  /* Parameters: security bitmask, preconfigured key, network key, */                \
  /*   network key sequence */                                                       \
  /* If security bitmask is 0x8000 then security is turned off, */                   \
  /*   nothing else is set. */                                                       \
  /* If security bitmask is 0xFFFF then security is turned on  */                    \
  /*   without setting any other security settings. */                               \
  /* If security bitmask is anything else, security is turned on, */                 \
  /*   the bitmask is set and so are the other values. */                            \
  /*   (as appropriate for the bitmask) */                                           \
  /*   The higher 16-bits of the bitmask are written into the */                     \
  /*  extended security bitmask, while the lower 16-bits are written */              \
  /*  into the security bitmask. */                                                  \
  emberCommandEntryAction("set_security", setSecurityCommand, "wbbu",                \
                          "Set the security parameters"),                            \
                                                                                     \
  /* channel, pan id, tx power */                                                    \
  emberCommandEntryAction("form", formNetworkCommand, "uvs",                         \
                          "Form a network"),                                         \
  /* channel, pan id, tx power, extended pan id */                                   \
  emberCommandEntryAction("form_ext", formNetworkCommand, "uvsb",                    \
                          "Form a network with extended settings"),                  \
                                                                                     \
  /* extended pan id */                                                              \
  emberCommandEntryAction("set_ext_pan", setExtPanIdCommand, "b",                    \
                          "Set extended PAN ID for forming/joining"),                \
                                                                                     \
  /* Set the join method: MAC Associate (0), */                                      \
  /*   NWK Rejoin w/out security (1), */                                             \
  /*   NWK Rejoin with Security (2) */                                               \
  /*   NWK commission, i.e. join quietly (3) */                                      \
  emberCommandEntryAction("join_method", setJoinMethod, "u",                         \
                          "Set the method used for joining"),                        \
                                                                                     \
  /* Set the Commissioning parameters: */                                            \
  /*   coordinator bool (1 byte) */                                                  \
  /*   nwkManagerId (2 bytes) */                                                     \
  /*   nwkUpdateId  (1 byte) */                                                      \
  /*   channels  (lower 16-bits of 32-bit value) */                                  \
  /*   channels  (upper 16-bits of 32-bit value) */                                  \
  /*   trust center eui64 */                                                         \
  emberCommandEntryAction("commission", setCommissionParameters, "uvuvvb",           \
                          "Set the commission parameters"),                          \
                                                                                     \
  /* channel, pan id, tx power */                                                    \
  emberCommandEntryAction("join", joinNetworkCommand, "uvs",                         \
                          "Join a network as router"),                               \
  emberCommandEntryAction("join_end", joinNetworkCommand, "uvs",                     \
                          "Join a network as RxOnWhenIdle end  device"),             \
  emberCommandEntryAction("join_sleepy", joinNetworkCommand, "uvs",                  \
                          "Join a network as a sleepy end device"),                  \
                                                                                     \
  emberCommandEntryAction("nwk_init", networkInitCommand, "v",                       \
                          "Execute emberNetworkInit()"),                             \
                                                                                     \
  emberCommandEntryAction("leave", leaveNetworkCommand, "",                          \
                          "Tell local node to Leave the network"),                   \
                                                                                     \
  emberCommandEntryAction("leave_with_options", leaveWithOptionsNetworkCommand, "u", \
                          "Tell local node to Leave the network with options"),      \
                                                                                     \
  /* params: */                                                                      \
  /*   - have current network key? (secure vs. insecure) */                          \
  /*   - channel mask (0 = search current channel only) */                           \
  emberCommandEntryAction("rejoin", rejoinCommand, "uw",                             \
                          "Execute a rejoin operation"),                             \
                                                                                     \
  /* target network id, include kids */                                              \
  emberCommandEntryAction("ieee_address", addressRequestCommand, "vu",               \
                          "Send a ZDO IEEE address request"),                        \
                                                                                     \
  /* target EUI64, include kids */                                                   \
  emberCommandEntryAction("nwk_address", addressRequestCommand, "bu",                \
                          "Send a ZDO NWK address request"),                         \
                                                                                     \
  /* duration in seconds, 0 to prohibit, 0xff to allow */                            \
  emberCommandEntryAction("permit_joins", permitJoiningCommand, "u",                 \
                          "Change the MAC permit join flag"),                        \
                                                                                     \
  /* help commands */                                                                \
  emberCommandEntryAction("help", helpCommand, "",                                   \
                          "Print the list of commands"),                             \
                                                                                     \
  /* Parameter 1 -  Network index */                                                 \
  emberCommandEntryAction("set_network", setNetworkCommand, "u",                     \
                          "Set the network index"),                                  \
                                                                                     \
  /* no params */                                                                    \
  emberCommandEntryAction("get_curr_nwk_index", getCurrentNwkIndexCommand, "",       \
                          "Get the current network index"),                          \
                                                                                     \
  /* Optional parameter 1 -  Phy index */                                            \
  emberCommandEntryAction("radio_params", getRadioParameters, "u*",                  \
                          "Print the radio parameters"),

#endif // SILABS_APP_UTIL_COMMON_H
