/***************************************************************************//**
 * @file
 * @brief This file implements commands for configuring 802.15.4 options
 *   relevant to receiving packets
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "response_print.h"

#include "rail.h"
#include "rail_ieee802154.h"
#include "app_common.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

bool ieee802154EnhAckEnabled = false;
uint8_t ieee802154PhrLen = 1U; // Default is 1-byte PHY Header (length byte)
bool setFpByDefault = false;
uint32_t dataReqLatencyUs = 0U;

RAIL_Handle_t emPhyRailHandle;

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
extern RAIL_Status_t sl_rail_util_ieee802154_config_radio(RAIL_Handle_t railHandle);
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

extern RAIL_LbtConfig_t *lbtConfig;
extern RAIL_CsmaConfig_t *csmaConfig;
static int8_t ccaThreshold = RAIL_RSSI_INVALID_DBM;

void emRadioHoldOffIsr(bool active)
{
  if (active) {
    if (txType == TX_TYPE_CSMA) {
      ccaThreshold = csmaConfig->ccaThreshold;
      RAIL_SetCcaThreshold(railHandle, RAIL_RSSI_INVALID_DBM);
    } else if (txType == TX_TYPE_LBT) {
      ccaThreshold = lbtConfig->lbtThreshold;
      RAIL_SetCcaThreshold(railHandle, RAIL_RSSI_INVALID_DBM);
    }
  } else if (ccaThreshold != RAIL_RSSI_INVALID_DBM) {
    RAIL_SetCcaThreshold(railHandle, ccaThreshold);
  } else {
    // MISRA compliance
  }
}

void sl_railtest_update_154_radio_config(void)
{
#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
  if (RAIL_IEEE802154_IsEnabled(railHandle)) {
    RAIL_RadioState_t currentState = RAIL_GetRadioState(railHandle);
    RAIL_Idle(railHandle, RAIL_IDLE_ABORT, false);
    sl_rail_util_ieee802154_config_radio(railHandle);
    if (currentState != RAIL_RF_STATE_IDLE) {
      RAIL_StartRx(railHandle, channel, NULL);
    }
  }
#endif //SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
}

void ieee802154Enable(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  RAIL_StateTiming_t timings = {
    .idleToTx = 100,
    .idleToRx = 100,
    .rxToTx = 192,
    // Make txToRx slightly lower than desired to make sure we get to
    // RX in time
    .txToRx = 192 - 10,
    .rxSearchTimeout = 0,
    .txToRxSearchTimeout = 0,
    .txToTx = 0,
  };

  RAIL_IEEE802154_Config_t config = {
    .addresses = NULL,
    .ackConfig = {
      .enable = true,
      .ackTimeout = 672,
      .rxTransitions = {
        .success = RAIL_RF_STATE_RX,
        .error = RAIL_RF_STATE_RX // ignored
      },
      .txTransitions = {
        .success = RAIL_RF_STATE_RX,
        .error = RAIL_RF_STATE_RX // ignored
      }
    },
    .timings = timings,
    .framesMask = RAIL_IEEE802154_ACCEPT_STANDARD_FRAMES,
    .promiscuousMode = false,
    .isPanCoordinator = false,
    .defaultFramePendingInOutgoingAcks = false
  };

  if (memcmp(sl_cli_get_argument_string(args, 0), "idle", 4) == 0) {
    config.ackConfig.rxTransitions.success = RAIL_RF_STATE_IDLE;
    config.ackConfig.txTransitions.success = RAIL_RF_STATE_IDLE;
  } else if (memcmp(sl_cli_get_argument_string(args, 0), "rx", 2) == 0) {
    config.ackConfig.rxTransitions.success = RAIL_RF_STATE_RX;
    config.ackConfig.txTransitions.success = RAIL_RF_STATE_RX;
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x20, "Unknown auto ack default state.");
    return;
  }

  RAIL_TransitionTime_t timing = (RAIL_TransitionTime_t)sl_cli_get_argument_uint16(args, 1);
  if (timing > RAIL_MAXIMUM_TRANSITION_US) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x21, "Invalid idle timing.");
    return;
  } else {
    config.timings.idleToTx = timing;
    config.timings.idleToRx = timing;
  }

  timing = (RAIL_TransitionTime_t)sl_cli_get_argument_uint16(args, 2);
  if (timing > RAIL_MAXIMUM_TRANSITION_US) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x22, "Invalid turnaround timing");
    return;
  } else {
    config.timings.rxToTx = timing;
    config.timings.txToRx = timing - 10U;
  }

  timing = (RAIL_TransitionTime_t)sl_cli_get_argument_uint16(args, 3);
  config.ackConfig.ackTimeout = (uint16_t)timing;

  if (sl_cli_get_argument_count(args) >= 5) {
    setFpByDefault = !!(sl_cli_get_argument_uint8(args, 4));
    config.defaultFramePendingInOutgoingAcks  = setFpByDefault;
  }
  disableIncompatibleProtocols(RAIL_PTI_PROTOCOL_ZIGBEE);
  RAIL_Status_t status = RAIL_IEEE802154_Init(railHandle, &config);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), status, "Call to RAIL_IEEE802154_Init returned an error");
  } else {
    emPhyRailHandle = railHandle;
    responsePrint(sl_cli_get_command_string(args, 0),
                  "802.15.4:%s,"
                  "rxDefaultState:%s,"
                  "txDefaultState:%s,"
                  "idleTiming:%d,"
                  "turnaroundTime:%d,"
                  "ackTimeout:%d,"
                  "defaultFramePending:%s",
                  status ? "Disabled" : "Enabled",
                  getRfStateName(config.ackConfig.txTransitions.success),
                  getRfStateName(config.ackConfig.rxTransitions.success),
                  config.timings.idleToTx,
                  config.timings.rxToTx,
                  config.ackConfig.ackTimeout,
                  config.defaultFramePendingInOutgoingAcks ? "True" : "False");
  }
}

#define RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_ANTDIV_SHIFT    (0U)
#define RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_COEX_SHIFT      (1U)
#define RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_FEM_SHIFT       (2U)
#define RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_ANTDIV          (1U << RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_ANTDIV_SHIFT)
#define RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_COEX            (1U << RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_COEX_SHIFT)
#define RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_FEM             (1U << RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_FEM_SHIFT)
#if RAIL_IEEE802154_SUPPORTS_CUSTOM1_PHY
#define RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_CUSTOM1_PHY_SHIFT   (3U)
#define RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_CUSTOM1_PHY         (1U << RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_CUSTOM1_PHY_SHIFT)
#endif

typedef RAIL_Status_t (*RAIL_IEEE802154_2p4GHzRadioConfig_t)(RAIL_Handle_t railHandle);
static RAIL_IEEE802154_2p4GHzRadioConfig_t ieee802154Configs[] = {
  &RAIL_IEEE802154_Config2p4GHzRadio,
  &RAIL_IEEE802154_Config2p4GHzRadioAntDiv,
  &RAIL_IEEE802154_Config2p4GHzRadioCoex,
  &RAIL_IEEE802154_Config2p4GHzRadioAntDivCoex,
  &RAIL_IEEE802154_Config2p4GHzRadioFem,
  &RAIL_IEEE802154_Config2p4GHzRadioAntDivFem,
  &RAIL_IEEE802154_Config2p4GHzRadioCoexFem,
  &RAIL_IEEE802154_Config2p4GHzRadioAntDivCoexFem,
#if RAIL_IEEE802154_SUPPORTS_CUSTOM1_PHY
  &RAIL_IEEE802154_Config2p4GHzRadioCustom1,
#endif
};

void config2p4Ghz802154(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Status_t status;
  uint8_t ieee802154Config = 0U;

  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  disableIncompatibleProtocols(RAIL_PTI_PROTOCOL_ZIGBEE);

  if ((sl_cli_get_argument_count(args) >= 1)
      && (sl_cli_get_argument_uint8(args, 0) != 0U)) {
    ieee802154Config |= RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_ANTDIV;
  }
  if ((sl_cli_get_argument_count(args) >= 2)
      && (sl_cli_get_argument_uint8(args, 1) != 0U)) {
    ieee802154Config |= RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_COEX;
  }
  if ((sl_cli_get_argument_count(args) >= 3)
      && (sl_cli_get_argument_uint8(args, 2) != 0U)) {
    ieee802154Config |= RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_FEM;
  }
#if RAIL_IEEE802154_SUPPORTS_CUSTOM1_PHY
  if ((sl_cli_get_argument_count(args) >= 4)
      && (sl_cli_get_argument_uint8(args, 3) != 0U)) {
    ieee802154Config |= RAIL_IEEE802154_CONFIG_2P4GHZ_RADIO_CUSTOM1_PHY;
  }
  if (ieee802154Config >= COUNTOF(ieee802154Configs)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 1,
                       "Cannot set antdiv, coex or fem options when custom is set");
  }
#endif //RAIL_IEEE802154_SUPPORTS_CUSTOM1_PHY
  status = (*ieee802154Configs[ieee802154Config])(railHandle);
  if (status == RAIL_STATUS_NO_ERROR) {
    ieee802154PhrLen = 1U;
    changeChannel(11);
  }
  responsePrint(sl_cli_get_command_string(args, 0), "802.15.4:%s", status ? "Disabled" : "Enabled");
}

void config863Mhz802154(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  disableIncompatibleProtocols(RAIL_PTI_PROTOCOL_ZIGBEE);

  RAIL_Status_t status = RAIL_IEEE802154_ConfigGB863MHzRadio(railHandle);
  if (status == RAIL_STATUS_NO_ERROR) {
    ieee802154PhrLen = 2U;
    changeChannel(0x80);
    // Hack to make a default 2-byte PHR in txData[]
    if ((txDataLen == 16) && (txData[0] == 0x0F)) { // default tx packet
      txData[0] = 0x18; // enable whitening, 2-byte CRC
      txData[1] = 0x70; // length indicator = 14, bit-reversed
    }
  }
  responsePrint(sl_cli_get_command_string(args, 0), "802.15.4:%s", status ? "Disabled" : "Enabled");
}

void config915Mhz802154(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  disableIncompatibleProtocols(RAIL_PTI_PROTOCOL_ZIGBEE);

  RAIL_Status_t status = RAIL_IEEE802154_ConfigGB915MHzRadio(railHandle);
  if (status == RAIL_STATUS_NO_ERROR) {
    ieee802154PhrLen = 2U;
    changeChannel(0xE0);
    // Hack to make a default 2-byte PHR in txData[]
    if ((txDataLen == 16) && (txData[0] == 0x0F)) { // default tx packet
      txData[0] = 0x18; // enable whitening, 2-byte CRC
      txData[1] = 0x70; // length indicator = 14, bit-reversed
    }
  }
  responsePrint(sl_cli_get_command_string(args, 0), "802.15.4:%s", status ? "Disabled" : "Enabled");
}

void ieee802154AcceptFrames(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint8_t framesEnable = 0;
  bool commandFrame = !!sl_cli_get_argument_uint8(args, 0);
  bool ackFrame = !!sl_cli_get_argument_uint8(args, 1);
  bool dataFrame = !!sl_cli_get_argument_uint8(args, 2);
  bool beaconFrame = !!sl_cli_get_argument_uint8(args, 3);
  bool multipurposeFrame = false;

  if (sl_cli_get_argument_count(args) >= 5) {
    multipurposeFrame = !!sl_cli_get_argument_uint8(args, 4);
  }

  // Command
  if (commandFrame) {
    framesEnable |= RAIL_IEEE802154_ACCEPT_COMMAND_FRAMES;
  }
  if (ackFrame) {
    framesEnable |= RAIL_IEEE802154_ACCEPT_ACK_FRAMES;
  }
  if (dataFrame) {
    framesEnable |= RAIL_IEEE802154_ACCEPT_DATA_FRAMES;
  }
  if (beaconFrame) {
    framesEnable |= RAIL_IEEE802154_ACCEPT_BEACON_FRAMES;
  }
  if (multipurposeFrame) {
    framesEnable |= RAIL_IEEE802154_ACCEPT_MULTIPURPOSE_FRAMES;
  }

  RAIL_Status_t status = RAIL_IEEE802154_AcceptFrames(railHandle, framesEnable);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x23, "Failed to set which frames to accept.");
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "CommandFrame:%s,"
                  "AckFrame:%s,"
                  "DataFrame:%s,"
                  "BeaconFrame:%s,"
                  "MultiPurposeFrame:%s",
                  commandFrame ? "Enabled" : "Disabled",
                  ackFrame ? "Enabled" : "Disabled",
                  dataFrame ? "Enabled" : "Disabled",
                  beaconFrame ? "Enabled" : "Disabled",
                  multipurposeFrame ? "Enabled" : "Disabled");
  }
}

void ieee802154SetPromiscuousMode(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  bool promiscuous = !!sl_cli_get_argument_uint8(args, 0);
  RAIL_Status_t status = RAIL_IEEE802154_SetPromiscuousMode(railHandle, promiscuous);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x24, "Failed to (un)set promiscuous mode.");
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "PromiscuousMode:%s",
                  promiscuous ? "Enabled" : "Disabled");
  }
}

void ieee802154SetPanCoordinator(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  bool panCoord = !!sl_cli_get_argument_uint8(args, 0);
  RAIL_Status_t status = RAIL_IEEE802154_SetPanCoordinator(railHandle, panCoord);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x24, "Failed to (un)set PAN Coordinator.");
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "PanCoordinator:%s",
                  panCoord ? "Enabled" : "Disabled");
  }
}

void ieee802154SetPanId(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint16_t panId = sl_cli_get_argument_uint16(args, 0);
  uint8_t index = (sl_cli_get_argument_count(args) >= 2) ? sl_cli_get_argument_uint8(args, 1) : 0;
  RAIL_Status_t status = RAIL_IEEE802154_SetPanId(railHandle, panId, index);
  responsePrint(sl_cli_get_command_string(args, 0), "802.15.4PanId:%s", getStatusMessage(status));
}

void ieee802154SetShortAddress(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint16_t shortAddr = sl_cli_get_argument_uint16(args, 0);
  uint8_t index = (sl_cli_get_argument_count(args) >= 2) ? sl_cli_get_argument_uint8(args, 1) : 0;
  RAIL_Status_t status = RAIL_IEEE802154_SetShortAddress(railHandle,
                                                         shortAddr,
                                                         index);
  responsePrint(sl_cli_get_command_string(args, 0), "802.15.4ShortAddress:%s", getStatusMessage(status));
}

void ieee802154SetLongAddress(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint8_t longAddr[8];
  for (int i = 0; i < 8; i++) {
    longAddr[i] = sl_cli_get_argument_uint8(args, i);
  }
  uint8_t index = (sl_cli_get_argument_count(args) >= 9) ? sl_cli_get_argument_uint8(args, 8) : 0;
  RAIL_Status_t status = RAIL_IEEE802154_SetLongAddress(railHandle,
                                                        longAddr,
                                                        index);
  responsePrint(sl_cli_get_command_string(args, 0), "802.15.4LongAddress:%s", getStatusMessage(status));
}

void ieee802154SetAddresses(sl_cli_command_arg_t *args)
{
  // Any subsequent optional inputs after the 1st set of panID, shortAddr and LongAddr
  // will be input as string hence need to be handled differently.
  #define GET_ARG(args, argCount) (argCount >= 2)                 \
  ? strtoull(sl_cli_get_argument_string(args, argCount), NULL, 0) \
  : sl_cli_get_argument_uint16(args, argCount);

  RAIL_IEEE802154_AddrConfig_t addresses = {
    { 0xFFFF, 0xFFFF, 0xFFFF },
    { 0xFFFF, 0xFFFF, 0xFFFF },
    { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }
  };

  uint8_t argCount = 0;
  for (uint8_t i = 0; i < RAIL_IEEE802154_MAX_ADDRESSES; i++) {
    if (argCount >= sl_cli_get_argument_count(args)) {
      break;
    }
    addresses.panId[i] = GET_ARG(args, argCount);
    argCount++;

    if (argCount >= sl_cli_get_argument_count(args)) {
      break;
    }
    addresses.shortAddr[i] = GET_ARG(args, argCount);
    argCount++;

    if (argCount >= sl_cli_get_argument_count(args)) {
      break;
    }
    uint64_t longAddr = GET_ARG(args, argCount);
    for (int j = 0; j < 8; j++) {
      addresses.longAddr[i][j] = (longAddr >> (i * 8)) & 0xFF;
    }
    argCount++;
  }
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Status_t status = RAIL_IEEE802154_SetAddresses(railHandle, &addresses);
  responsePrint(sl_cli_get_command_string(args, 0), "802.15.4Addresses:%s", getStatusMessage(status));
}

void ieee802154SetDataReqLatency(sl_cli_command_arg_t *args)
{
  dataReqLatencyUs = sl_cli_get_argument_uint32(args, 0);
  responsePrint(sl_cli_get_command_string(args, 0), "DataReqLatency:%u", dataReqLatencyUs);
}

void ieee802154SetE(sl_cli_command_arg_t *args)
{
  RAIL_IEEE802154_EOptions_t options = (RAIL_IEEE802154_EOptions_t) sl_cli_get_argument_uint32(args, 0);
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Status_t status = RAIL_IEEE802154_ConfigEOptions(railHandle,
                                                        RAIL_IEEE802154_E_OPTIONS_ALL,
                                                        options);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 31, "802.15.4E:Failed");
    return;
  }
  ieee802154EnhAckEnabled = ((options & RAIL_IEEE802154_E_OPTION_ENH_ACK) != 0U);
  responsePrint(sl_cli_get_command_string(args, 0),
                "15.4E_GB868:%s,"
                "15.4E_EnhAck:%s,"
                "15.4E_ImplicitBroadcast:%s",
                (options & RAIL_IEEE802154_E_OPTION_GB868) ? "True" : "False",
                (options & RAIL_IEEE802154_E_OPTION_ENH_ACK) ? "True" : "False",
                (options & RAIL_IEEE802154_E_OPTION_IMPLICIT_BROADCAST) ? "True" : "False");
}

void ieee802154SetG(sl_cli_command_arg_t *args)
{
  RAIL_IEEE802154_GOptions_t options = (RAIL_IEEE802154_GOptions_t) sl_cli_get_argument_uint32(args, 0);
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Status_t status = RAIL_IEEE802154_ConfigGOptions(railHandle,
                                                        RAIL_IEEE802154_G_OPTIONS_ALL,
                                                        options);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 31, "802.15.4G:Failed");
    return;
  } else {
    ieee802154PhrLen = (((options & RAIL_IEEE802154_G_OPTION_GB868) != 0U)
                        ? 2U : 1U);
  }
  responsePrint(sl_cli_get_command_string(args, 0),
                "15.4G_GB868:%s,"
                "15.4G_DynamicFEC:%s,"
                "Wi-SUN_ModeSwitch:%s",
                (options & RAIL_IEEE802154_G_OPTION_GB868) ? "True" : "False",
                (options & RAIL_IEEE802154_G_OPTION_DYNFEC) ? "True" : "False",
                (options & RAIL_IEEE802154_G_OPTION_WISUN_MODESWITCH) ? "True" : "False");
}

void ieee802154SetFpMode(sl_cli_command_arg_t *args)
{
  bool earlyFp = !!sl_cli_get_argument_uint8(args, 0);
  bool dataFp = !!sl_cli_get_argument_uint8(args, 1);

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Status_t earlyStatus = RAIL_IEEE802154_EnableEarlyFramePending(railHandle, earlyFp);
  const char *earlyDisplay;
  if (earlyStatus != RAIL_STATUS_NO_ERROR) {
    earlyDisplay = "Unsupported";
  } else {
    earlyDisplay = (earlyFp) ? "Enabled" : "Disabled";
  }

  RAIL_Status_t dataStatus = RAIL_IEEE802154_EnableDataFramePending(railHandle, dataFp);
  const char *dataDisplay;
  if (dataStatus != RAIL_STATUS_NO_ERROR) {
    dataDisplay = "Unsupported";
  } else {
    dataDisplay = (dataFp) ? "Enabled" : "Disabled";
  }

  responsePrint(sl_cli_get_command_string(args, 0), "EarlyFp:%s,DataFp:%s", earlyDisplay, dataDisplay);
}

//PHR types known by ieee802154SetPHR()
RAIL_ENUM(IEEE802154_PHRType_t) {
  /* Legacy PHRs */
  MISC_MOD_PHR1BYTE,
  SUNFSK_PHR2BYTES,
  /* New Modem PHRs are 4 bytes long */
  SUNOFDM_PHR4BYTES,
  SUNOQPSK_PHR4BYTES,
  SUNFSK_PHR4BYTES,
  LEGOQPSK_PHR4BYTE, //802.15.4 O-QPSK (other than SUN O-QPSK)
  IEEE802154_NB_PHR_TYPE // Must be last
};

void ieee802154SetPHR(sl_cli_command_arg_t *args)
{
  /*
     This function sets the PHR for 802.15.4 frames. It must be used with the Tx buffer size
     (set by setTxLength command). It does NOT take into account the PHY loaded.
   */
  uint8_t param1, param2;
  uint8_t phrType = sl_cli_get_argument_uint8(args, 0);
  uint8_t phrSizeByte = 0;
  uint16_t frameLength = 0;
  uint8_t fcsSizeByte = 0;
  uint32_t phr = 0;

  if (IEEE802154_NB_PHR_TYPE <= phrType) {
    responsePrintError(sl_cli_get_command_string(args, 0), 31, "invalid PHR type: %d", phrType);
    return;
  }

  //no other input params for MISC_MOD_PHR1BYTE
  //and 2 for other modes
  if (MISC_MOD_PHR1BYTE != phrType) {
    if (sl_cli_get_argument_count(args) >= 3) {
      param1 = sl_cli_get_argument_uint8(args, 1);
      param2 = sl_cli_get_argument_uint8(args, 2);
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 31, "invalid number of arguments");
      return;
    }
  }

  if (MISC_MOD_PHR1BYTE == phrType) { // All modulations in PHR 1 byte
    // Non SUN modulation: no need to flip the PHR bits
    phrSizeByte = 1;
    fcsSizeByte = 2;
    phr = (txDataLen - phrSizeByte + fcsSizeByte) & 0x7F;
  } else if (SUNFSK_PHR2BYTES == phrType) { // This is Legacy demod case
    phrSizeByte = 2;
    bool fcsType = (bool) param1;
    bool whitening = (bool) param2;
    fcsSizeByte = fcsType ? 2 : 4; //FCS type = 0 => fcsSizeByte = 4
    frameLength = (txDataLen - phrSizeByte + fcsSizeByte) & 0x7FF;
    phr = ((fcsType << 12) | (whitening << 11) | frameLength);
    // Flip bits of the 2 bytes PHR as it is a SUN modulation
    phr = (uint16_t) (__RBIT(phr) >> 16);
  } else { // If the new modem is used
    /*
       If the new modem is used, assume that txDataLen contains the PHR AND the FCS (2 or 4 bytes).
       Then the frame length programmed is just txDataLen - PhrSizeByte.
       Note that in normal cases, frame length is programmed as txDataLen - PhrSizeByte + fcsSizeByte.
       However, the fcs type is unknown at this stage in many cases (OFDM, SUN O-QPSK).
     */
    phrSizeByte = 4;
    frameLength = (txDataLen - phrSizeByte) & 0x7FF; // FrameLength in byte

    switch (phrType) {
      case SUNOFDM_PHR4BYTES:
      {
        uint8_t rate = (param1 & 0x1F);     // rate: 5 bits wide
        uint8_t scrambler = (param2 & 0x3); // scrambler: 2 bits wide

        phr = (rate << 19) | (frameLength << 7) | (scrambler << 3);
        break;
      }
      case SUNOQPSK_PHR4BYTES:
      {
        bool spreadingMode = (bool) param1;
        uint8_t rateMode = (param2 & 0x3); // rateMode: 2 bits wide

        phr = (spreadingMode << 15) | (rateMode << 13) | frameLength;
        break;
      }
      case SUNFSK_PHR4BYTES:
      {
        bool fcsType   = (bool) param1;
        bool whitening = (bool) param2;

        phr = (fcsType << 12) | (whitening << 11) | frameLength;
        break;
      }
      case LEGOQPSK_PHR4BYTE:
        phr = (frameLength & 0x7F) << 24;
        break;
      default:
        break;
    }

    // Only Legacy O-QPSK modulation should not be swapped
    if (LEGOQPSK_PHR4BYTE != phrType) {
      // Flip the 32 bits for all SUN modulations
      phr = __RBIT(phr);
    }
  }

  if (phrSizeByte != 0) {
    // Write the phr in the payload
    for (uint8_t index = 0; index < phrSizeByte; index++) {
      txData[index] = ((phr & (0xFF << index * 8)) >> index * 8);
    }

    if (railDataConfig.txMethod == PACKET_MODE) {
      RAIL_WriteTxFifo(railHandle, txData, txDataLen, true);
    }

    responsePrint(sl_cli_get_command_string(args, 0), "PhrSize:%d,PHR:0x%x", phrSizeByte, phr);
    printTxPacket(args);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 31, "PHR write failed");
  }
}

#if RAIL_IEEE802154_SUPPORTS_G_MODESWITCH && defined(WISUN_MODESWITCHPHRS_ARRAY_SIZE)

uint16_t modeSwitchNewChannel = 0xFFFFU;
uint16_t modeSwitchBaseChannel = 0xFFFFU;
uint32_t modeSwitchSequenceIterations = 1;
uint32_t modeSwitchSequenceId = 0;
#define MSPHR_LENGTH 2
uint8_t txData_2B[2] = { 0, 0 };
uint8_t MSphr[2] = { 0, 0 };
uint8_t txCountAfterModeSwitch;
ModeSwitchState_t modeSwitchState = IDLE;
uint32_t modeSwitchDelayUs = 0;
RAIL_MultiTimer_t modeSwitchMultiTimer;
uint8_t txCountAfterModeSwitchId = 0;

void trigModeSwitchTx(sl_cli_command_arg_t *args)
{
  modeSwitchSequenceIterations = 1;
  txCountAfterModeSwitchId = 0;
  modeSwitchSequenceId = 0;
  modeSwitchDelayUs = 0;
  uint8_t newPhyModeId = sl_cli_get_argument_uint32(args, 0);
  txCountAfterModeSwitch = sl_cli_get_argument_uint32(args, 1);
  uint8_t argCount = sl_cli_get_argument_count(args);
  if (argCount > 2) {
    modeSwitchSequenceIterations = sl_cli_get_argument_uint32(args, 2);
    if (argCount > 3) {
      modeSwitchDelayUs = sl_cli_get_argument_uint32(args, 3) * 1000; // Argument is in milliseconds but multitimer takes microseconds
      if (modeSwitchDelayUs > 0) {
        if (!RAIL_ConfigMultiTimer(true)) {
          responsePrintError(sl_cli_get_command_string(args, 0), 0x10, "Unable to configure the mode switch multiTimer.");
          return;
        }
      }
    }
  }
  uint8_t i;
  uint8_t numPhrsInArray = sizeof(wisun_modeSwitchPhrs) / sizeof(RAIL_IEEE802154_ModeSwitchPhr_t);

// Retrieve PHR to send from RAIL config table
  for (i = 0; i < numPhrsInArray; i++) {
    if (wisun_modeSwitchPhrs[i].phyModeId == newPhyModeId) {
      break;
    }
  }

  if (i < numPhrsInArray) {
    // Compute channel to switch to
    modeSwitchNewChannel = 0xFFFFU;
    RAIL_Status_t status = RAIL_IEEE802154_ComputeChannelFromPhyModeId(railHandle, newPhyModeId, &modeSwitchNewChannel);

    if (status == RAIL_STATUS_NO_ERROR) {
      txData_2B[0] = txData[0];
      txData_2B[1] = txData[1];
      memcpy(txData, &(wisun_modeSwitchPhrs[i].phr), MSPHR_LENGTH);
      memcpy(MSphr, &(wisun_modeSwitchPhrs[i].phr), MSPHR_LENGTH);
      modeSwitchBaseChannel = channel;
      modeSwitchState = TX_MS_PACKET;
      // Sends Mode Switch packet
      radioTransmit(1, "txModeSwitch");
      // Proper channel switch + sending packets on the new PHY moved to RAILCb_TxPacketSent()
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 31, "Mode switch failed: same PhyModeId or no channel found");
    }
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 31, "Mode switch failed: no PHR found");
  }
}

bool modeSwitchLifeReturn = false;
void modeSwitchLife(sl_cli_command_arg_t *args)
{
  modeSwitchLifeReturn = !!sl_cli_get_argument_uint32(args, 0);
  if (modeSwitchLifeReturn) {
    if (!RAIL_ConfigMultiTimer(true)) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x10, "Unable to configure the mode switch multiTimer.");
      return;
    }
  }
  responsePrint(sl_cli_get_command_string(args, 0), "ModeSwitchLife:%s", modeSwitchLifeReturn ? "Return to base PHY" : "Stay on new PHY");
}

#else//!RAIL_IEEE802154_SUPPORTS_G_MODESWITCH

void trigModeSwitchTx(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  responsePrintError(sl_cli_get_command_string(args, 0), 31, "Mode switching not supported on this chip or missing PHR array");
}

void modeSwitchLife(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  responsePrintError(sl_cli_get_command_string(args, 0), 31, "Mode switching not supported on this chip or missing PHR array");
}

#endif//RAIL_IEEE802154_SUPPORTS_G_MODESWITCH

// 802.15.4-2015 Frame Control Field definitions for Beacon, Ack, Data, Command
#define MAC_FRAME_SOURCE_MODE_MASK          0xC000U
#define MAC_FRAME_SOURCE_MODE_LONG          0xC000U
#define MAC_FRAME_SOURCE_MODE_SHORT         0x8000U
#define MAC_FRAME_SOURCE_MODE_RESERVED      0x4000U
#define MAC_FRAME_SOURCE_MODE_NONE          0x0000U
#define MAC_FRAME_VERSION_MASK              0x3000U
#define MAC_FRAME_VERSION_RESERVED          0x3000U
#define MAC_FRAME_VERSION_2012              0x2000U
#define MAC_FRAME_VERSION_2006              0x1000U
#define MAC_FRAME_VERSION_2003              0x0000U
#define MAC_FRAME_DESTINATION_MODE_MASK     0x0C00U
#define MAC_FRAME_DESTINATION_MODE_LONG     0x0C00U
#define MAC_FRAME_DESTINATION_MODE_SHORT    0x0800U
#define MAC_FRAME_DESTINATION_MODE_RESERVED 0x0400U
#define MAC_FRAME_DESTINATION_MODE_NONE     0x0000U
#define MAC_FRAME_FLAG_IE_LIST_PRESENT      0x0200U
#define MAC_FRAME_FLAG_SEQ_SUPPRESSION      0x0100U
#define MAC_FRAME_FLAG_RESERVED             0x0080U
#define MAC_FRAME_FLAG_PANID_COMPRESSION    0x0040U
#define MAC_FRAME_FLAG_ACK_REQUIRED         0x0020U
#define MAC_FRAME_FLAG_FRAME_PENDING        0x0010U
#define MAC_FRAME_FLAG_SECURITY_ENABLED     0x0008U
#define MAC_FRAME_TYPE_MASK                 0x0007U
#define MAC_FRAME_TYPE_BEACON               0x0000U
#define MAC_FRAME_TYPE_DATA                 0x0001U
#define MAC_FRAME_TYPE_ACK                  0x0002U
#define MAC_FRAME_TYPE_CONTROL              0x0003U
// 802.15.4-2015 Frame Control Field definitions for Multipurpose
#define MAC_FRAME_TYPE_MULTIPURPOSE         0x0005U
#define MAC_MP_FRAME_FLAG_LONG_FCF          0x0008U
#define MAC_MP_FRAME_DESTINATION_MODE_MASK  0x0030U
#define MAC_MP_FRAME_SOURCE_MODE_MASK       0x00C0U
#define MAC_MP_FRAME_FLAG_PANID_PRESENT     0x0100U
#define MAC_MP_FRAME_FLAG_SECURITY_ENABLED  0x0200U
#define MAC_MP_FRAME_FLAG_SEQ_SUPPRESSION   0x0400U
#define MAC_MP_FRAME_FLAG_FRAME_PENDING     0x0800U
#define MAC_MP_FRAME_FRAME_VERSION_MASK     MAC_FRAME_VERSION_MASK
#define MAC_MP_FRAME_FLAG_ACK_REQUIRED      0x4000U
#define MAC_MP_FRAME_FLAG_IE_LIST_PRESENT   0x8000U

// This table is derived from 802.15.4-2015 Section 7.2.1.5 PAN ID
// Compression field and Table 7-2 for both 2003/2006 and 2015
// frame versions.  It is indexed by 6 bits of the MacFCF:
//   SrcAdrMode FrameVer<msbit> DstAdrMode PanIdCompression
// and each address' length is encoded in a nibble:
//    15:12  11:8     7:4     3:0
//   SrcAdr  SrcPan  DstAdr  DstPan
// Illegal combinations are indicated by 0xFFFFU.
#define ADDRSIZE_DST_PAN_SHIFT 0
#define ADDRSIZE_DST_PAN_MASK  (0x0FU << ADDRSIZE_DST_PAN_SHIFT)
#define ADDRSIZE_DST_ADR_SHIFT 4
#define ADDRSIZE_DST_ADR_MASK  (0x0FU << ADDRSIZE_DST_ADR_SHIFT)
#define ADDRSIZE_SRC_PAN_SHIFT 8
#define ADDRSIZE_SRC_PAN_MASK  (0x0FU << ADDRSIZE_SRC_PAN_SHIFT)
#define ADDRSIZE_SRC_ADR_SHIFT 12
#define ADDRSIZE_SRC_ADR_MASK  (0x0FU << ADDRSIZE_SRC_ADR_SHIFT)

static const uint16_t ieee802154Table7p2[64] = {
  0x0000U, 0x0000U, 0xFFFFU, 0xFFFFU, 0x0022U, 0x0022U, 0x0082U, 0x0082U,
  0x0000U, 0x0002U, 0xFFFFU, 0xFFFFU, 0x0022U, 0x0020U, 0x0082U, 0x0080U,
  0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU,
  0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU,
  0x2200U, 0x2200U, 0xFFFFU, 0xFFFFU, 0x2222U, 0x2022U, 0x2282U, 0x2082U,
  0x2200U, 0x2000U, 0xFFFFU, 0xFFFFU, 0x2222U, 0x2022U, 0x2282U, 0x2082U,
  0x8200U, 0x8200U, 0xFFFFU, 0xFFFFU, 0x8222U, 0x8022U, 0x8282U, 0x8082U,
  0x8200U, 0x8000U, 0xFFFFU, 0xFFFFU, 0x8222U, 0x8022U, 0x8082U, 0x8080U,
};

void RAILCb_IEEE802154_DataRequestCommand(RAIL_Handle_t railHandle)
{
  RAIL_IEEE802154_Address_t address;
  bool setFramePending = false;
  if (dataReqLatencyUs > 0U) {
    (void) RAIL_DelayUs(dataReqLatencyUs);
  }
  if (ieee802154EnhAckEnabled) {
    // This callback occurs after the address fields of an incoming
    // ACK-requesting CMD or DATA frame have been received and we
    // can do a frame pending check.  We must also figure out what
    // kind of ACK is being requesting -- Immediate or Enhanced.
    // For an Enhanced ACK, we need to generate that ourselves;
    // RAIL will generate an Immediate ACK for us, though we can
    // tell it to go out with its FramePending bit set.
    // An 802.15.4 packet from RAIL should look like:
    // 1/2 |   1/2  | 0/1  |  0/2   | 0/2/8  |  0/2   | 0/2/8  | ...
    // PHR | MacFCF | Seq# | DstPan | DstAdr | SrcPan | SrcAdr | ...
    #define MaxExpectedBytes (2U + 2U + 1U + 2U + 8U + 2U + 8U)
    RAIL_RxPacketInfo_t packetInfo;
    uint8_t pkt[MaxExpectedBytes];
    uint8_t pktOffset = ieee802154PhrLen; // No need to parse the PHR byte(s)
    RAIL_GetRxIncomingPacketInfo(railHandle, &packetInfo);
    if (packetInfo.packetBytes < (pktOffset + 2U)) {
      counters.ackTxFpAddrFail++;
      return;
    }
    // Only extract what we care about
    if (packetInfo.packetBytes > MaxExpectedBytes) {
      packetInfo.packetBytes = MaxExpectedBytes;
      if (packetInfo.firstPortionBytes >= MaxExpectedBytes) {
        packetInfo.firstPortionBytes = MaxExpectedBytes;
        packetInfo.lastPortionData = NULL;
      }
    }
    RAIL_CopyRxPacket(pkt, &packetInfo);
    uint16_t macFcf = pkt[pktOffset++];
    if ((macFcf & MAC_FRAME_TYPE_MASK) == MAC_FRAME_TYPE_MULTIPURPOSE) {
      // Multipurpose frames have an arcane FCF structure
      if ((macFcf & MAC_MP_FRAME_FLAG_LONG_FCF) != 0U) {
        macFcf |= (pkt[pktOffset++] << 8);
      }
      // Map Multipurpose FCF to a 'normal' Version 2012 FCF
      // as best we can.
      macFcf = (MAC_FRAME_TYPE_MULTIPURPOSE
                | ((macFcf & (MAC_MP_FRAME_FLAG_SECURITY_ENABLED
                              | MAC_MP_FRAME_FLAG_IE_LIST_PRESENT)) >> 6)
                | ((macFcf & MAC_MP_FRAME_FLAG_FRAME_PENDING) >> 7)
                | ((macFcf & MAC_MP_FRAME_FLAG_ACK_REQUIRED) >> 9)
                | ((macFcf & (MAC_MP_FRAME_FLAG_PANID_PRESENT
                              | MAC_MP_FRAME_FLAG_SEQ_SUPPRESSION)) >> 2)
                | ((macFcf & MAC_MP_FRAME_DESTINATION_MODE_MASK) << 6)
                | MAC_FRAME_VERSION_2012
                | ((macFcf & MAC_MP_FRAME_SOURCE_MODE_MASK) << 8)
                );
      // MultiPurpose's PANID_PRESENT is not equivalent to 2012/5's
      // PANID_COMPRESSION so we map it best we can by flipping it
      // in the following address-combination situations:
      uint16_t addrCombo = (macFcf & (MAC_FRAME_SOURCE_MODE_MASK | MAC_FRAME_DESTINATION_MODE_MASK));
      if ((addrCombo == (MAC_FRAME_SOURCE_MODE_NONE | MAC_FRAME_DESTINATION_MODE_NONE))
          || (addrCombo == (MAC_FRAME_SOURCE_MODE_SHORT | MAC_FRAME_DESTINATION_MODE_SHORT))
          || (addrCombo == (MAC_FRAME_SOURCE_MODE_SHORT | MAC_FRAME_DESTINATION_MODE_LONG))
          || (addrCombo == (MAC_FRAME_SOURCE_MODE_LONG | MAC_FRAME_DESTINATION_MODE_SHORT))
          ) {
        // 802.15.4-2015 PANID_COMPRESSION = MP PANID_PRESENT
      } else {
        // 802.15.4-2015 PANID_COMPRESSION = !MP PANID_PRESENT
        macFcf ^= MAC_FRAME_FLAG_PANID_COMPRESSION; // Flip it
      }
    } else {
      macFcf |= (pkt[pktOffset++] << 8);
    }
    bool enhAck = ((macFcf & MAC_FRAME_VERSION_MASK) == MAC_FRAME_VERSION_2012);
    // Compress MAC FCF to index into 64-entry address-length table:
    // SrcAdrMode FrameVer<msbit> DstAdrMode PanIdCompression
    uint16_t index = (((macFcf & (MAC_FRAME_SOURCE_MODE_MASK | MAC_FRAME_VERSION_2012)) >> 10)
                      | ((macFcf & MAC_FRAME_DESTINATION_MODE_MASK) >> 9)
                      | ((macFcf & MAC_FRAME_FLAG_PANID_COMPRESSION) >> 6));
    uint16_t addrSizes = ieee802154Table7p2[index];
    // Illegal combinations mean illegal packets which we ignore
    if (addrSizes == 0xFFFFU) {
      counters.ackTxFpAddrFail++;
      return;
    }
    uint8_t seqNo = ((enhAck && ((macFcf & MAC_FRAME_FLAG_SEQ_SUPPRESSION) != 0U))
                     ? 0U : pkt[pktOffset++]); // Seq#
    if (enhAck) {
      // Enhanced ACK -- need to construct it since RAIL cannot.
      // First extract addresses from incoming packet since we may
      // need to reflect them in a different order in the outgoing ACK.
      // Use byte[0] to hold each one's length.
      uint8_t dstPan[3] = { 0, }; // Initialized only to eliminate false gcc warning
      dstPan[0] = ((addrSizes & ADDRSIZE_DST_PAN_MASK) >> ADDRSIZE_DST_PAN_SHIFT);
      if ((dstPan[0] + pktOffset) > packetInfo.packetBytes) {
        counters.ackTxFpAddrFail++;
        return;
      }
      if (dstPan[0] > 0U) {
        dstPan[1] = pkt[pktOffset++];
        dstPan[2] = pkt[pktOffset++];
      }
      uint8_t dstAdr[9];
      dstAdr[0] = ((addrSizes & ADDRSIZE_DST_ADR_MASK) >> ADDRSIZE_DST_ADR_SHIFT);
      if ((dstAdr[0] + pktOffset) > packetInfo.packetBytes) {
        counters.ackTxFpAddrFail++;
        return;
      }
      for (uint8_t i = 1U; i <= dstAdr[0]; i++) {
        dstAdr[i] = pkt[pktOffset++];
      }
      uint8_t srcPan[3];
      srcPan[0] = ((addrSizes & ADDRSIZE_SRC_PAN_MASK) >> ADDRSIZE_SRC_PAN_SHIFT);
      if ((srcPan[0] + pktOffset) > packetInfo.packetBytes) {
        counters.ackTxFpAddrFail++;
        return;
      }
      if (srcPan[0] > 0U) {
        srcPan[1] = pkt[pktOffset++];
        srcPan[2] = pkt[pktOffset++];
      }
      uint8_t srcAdr[9];
      srcAdr[0] = ((addrSizes & ADDRSIZE_SRC_ADR_MASK) >> ADDRSIZE_SRC_ADR_SHIFT);
      if ((srcAdr[0] + pktOffset) > packetInfo.packetBytes) {
        counters.ackTxFpAddrFail++;
        return;
      }
      for (uint8_t i = 1U; i <= srcAdr[0]; i++) {
        srcAdr[i] = pkt[pktOffset++];
      }

      // Reuse packet[] buffer for outgoing Enhanced ACK.
      // Phr1 Phr2 FcfL FcfH [Seq#] [DstPan] [DstAdr] [SrcPan] [SrcAdr]
      // Will fill in PHR later.
      // MAC Fcf:
      // - Frame Type = ACK
      // - Security Enabled = 0 in this implementation
      // - Frame Pending = 0 or as appropriate
      // - ACK Request = 0
      // - PanId compression = incoming packet's
      // - Seq# suppression = incoming packet's
      // - IE Present = 0 in this implementation
      // - DstAdrMode = SrcAdrMode of incoming packet's
      // - Frame Version = 2 (154E)
      // - SrcAdrMode = DstAdrMode of incoming packet's (for convenience)
      uint16_t ackFcf = (MAC_FRAME_TYPE_ACK
                         | (macFcf & MAC_FRAME_FLAG_PANID_COMPRESSION)
                         | (macFcf & MAC_FRAME_FLAG_SEQ_SUPPRESSION)
                         | MAC_FRAME_VERSION_2012
                         | ((macFcf & MAC_FRAME_SOURCE_MODE_MASK) >> 4)
                         | ((macFcf & MAC_FRAME_DESTINATION_MODE_MASK) << 4)
                         );
      // Do frame-pending check now
      if ((srcAdr[0] > 0U) && (srcAdr[1] == 0xAAU)) {
        setFramePending = true;
      }
      if (setFramePending) {
        ackFcf |= MAC_FRAME_FLAG_FRAME_PENDING;
      }
      pktOffset = ieee802154PhrLen;
      pkt[pktOffset++] = (uint8_t)ackFcf;
      pkt[pktOffset++] = (uint8_t)(ackFcf >> 8);

      if ((macFcf & MAC_FRAME_FLAG_SEQ_SUPPRESSION) == 0U) {
        pkt[pktOffset++] = seqNo;
      }
      // Determine outgoing ACK's address field sizes
      index = (((ackFcf & (MAC_FRAME_SOURCE_MODE_MASK | MAC_FRAME_VERSION_2012)) >> 10)
               | ((ackFcf & MAC_FRAME_DESTINATION_MODE_MASK) >> 9)
               | ((ackFcf & MAC_FRAME_FLAG_PANID_COMPRESSION) >> 6));
      addrSizes = ieee802154Table7p2[index];
      if (addrSizes == 0xFFFFU) {
        // Uh-oh! Enh-ACK would be malformed?!  Something funky happened!
        // Possibly a latency-induced issue.
        counters.ackTxFpAddrFail++;
        return;
      }
      // DstPan = SrcPan of incoming if avail otherwise DstPan of incoming
      if ((addrSizes & ADDRSIZE_DST_PAN_MASK) != 0U) {
        if (srcPan[0] > 0U) {
          pkt[pktOffset++] = srcPan[1];
          pkt[pktOffset++] = srcPan[2];
        } else if (dstPan[0] > 0U) {
          pkt[pktOffset++] = dstPan[1];
          pkt[pktOffset++] = dstPan[2];
        } else {
          // Uh-oh! Outgoing packet needs a DstPanId but incoming had neither!
          // Possibly a latency-induced issue.
          counters.ackTxFpAddrFail++;
          return;
        }
      }
      // DstAdr = SrcAdr of incoming packet -- their sizes should match
      if ((addrSizes & ADDRSIZE_DST_ADR_MASK) != 0U) {
        for (uint8_t i = 1U; i <= srcAdr[0]; i++) {
          pkt[pktOffset++] = srcAdr[i];
        }
      }
      // SrcPan = DstPan of incoming if avail otherwise SrcPan of incoming
      if ((addrSizes & ADDRSIZE_SRC_PAN_MASK) != 0U) {
        if (dstPan[0] > 0U) {
          pkt[pktOffset++] = dstPan[1];
          pkt[pktOffset++] = dstPan[2];
        } else if (srcPan[0] > 0U) {
          pkt[pktOffset++] = srcPan[1];
          pkt[pktOffset++] = srcPan[2];
        } else {
          // Uh-oh! Outgoing packet needs a SrcPanId but incoming had neither!
          // Possibly a latency-induced issue.
          counters.ackTxFpAddrFail++;
          return;
        }
      }
      // SrcAdr = DstAdr of incoming packet -- their sizes should match
      if ((addrSizes & ADDRSIZE_SRC_ADR_MASK) != 0U) {
        for (uint8_t i = 1U; i <= dstAdr[0]; i++) {
          pkt[pktOffset++] = dstAdr[i];
        }
      }

      // Fill in PHR now that we know Enh-ACK's length
      if (ieee802154PhrLen == 2U) {
        pkt[0] = (0x08U /*FCS=2byte*/ | 0x10U /*Whiten=enabled*/);
        pkt[1] = (uint8_t)(__RBIT(pktOffset - 2U /*PHRLen*/ + 2U /*FCS*/) >> 24);
      } else {
        pkt[0] = (pktOffset - 1U /*PHRLen*/ + 2U /*FCS*/);
      }
      if (RAIL_IEEE802154_WriteEnhAck(railHandle, pkt, pktOffset)
          == RAIL_STATUS_NO_ERROR) {
        if (setFramePending) {
          counters.ackTxFpSet++;
        }
      } else if (setFramePending) {
        counters.ackTxFpFail++;
      }
      return;
    }

    // Immediate ACK -- RAIL will send, but may need to do FP lookup.
    uint16_t srcAdrLen = ((addrSizes & ADDRSIZE_SRC_ADR_MASK) >> ADDRSIZE_SRC_ADR_SHIFT);
    if (srcAdrLen == 0U) {
      // No source address info to do FP lookup
      counters.ackTxFpAddrFail++;
      return;
    }
    // Skip over DstPan, DstAdr, SrcPan to get to SrcAdr
    pktOffset += (((addrSizes & ADDRSIZE_DST_PAN_MASK) >> ADDRSIZE_DST_PAN_SHIFT)
                  + ((addrSizes & ADDRSIZE_DST_ADR_MASK) >> ADDRSIZE_DST_ADR_SHIFT)
                  + ((addrSizes & ADDRSIZE_SRC_PAN_MASK) >> ADDRSIZE_SRC_PAN_SHIFT));
    // At this point we expect the addressing info to be there
    if ((srcAdrLen + pktOffset) > packetInfo.packetBytes) {
      counters.ackTxFpAddrFail++;
      return; // Oops, maybe latency caused us to see a later incoming frame
    }
    // Placeholder validation for when a data request should have the frame
    // pending bit set in the ACK.
    // First byte of short or long address being 0xAA has frame pending
    setFramePending = (pkt[pktOffset] == 0xAAU);
  } else {
    if (RAIL_IEEE802154_GetAddress(railHandle, &address)
        != RAIL_STATUS_NO_ERROR) {
      counters.ackTxFpAddrFail++;
      return;
    }
    // Placeholder validation for when a data request should have the frame
    // pending bit set in the ACK.
    setFramePending = (((address.length == RAIL_IEEE802154_LongAddress)
                        && (address.longAddress[0] == 0xAA))
                       || ((address.shortAddress & 0xFF) == 0xAA));
  }
  if (setFramePending != setFpByDefault) {
    if (RAIL_IEEE802154_ToggleFramePending(railHandle) == RAIL_STATUS_NO_ERROR) {
      counters.ackTxFpSet++;
    } else {
      counters.ackTxFpFail++;
    }
  }
}

void set802154CcaMode(sl_cli_command_arg_t *args)
{
  RAIL_IEEE802154_CcaMode_t ccaMode = sl_cli_get_argument_uint8(args, 0);
  if (RAIL_IEEE802154_ConfigCcaMode(railHandle, ccaMode)
      == RAIL_STATUS_NO_ERROR) {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "ccaMode:%u",
                  ccaMode);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "CCA mode unsupported");
  }
}

void enable802154SignalIdentifier(sl_cli_command_arg_t *args)
{
#if RAIL_IEEE802154_SUPPORTS_SIGNAL_IDENTIFIER
  RAIL_Status_t status;
  bool enable = sl_cli_get_argument_uint8(args, 0);
  if (RAIL_IEEE802154_IsEnabled(railHandle)) {
    RAIL_IEEE802154_PtiRadioConfig_t radioConfig = RAIL_IEEE802154_GetPtiRadioConfig(railHandle);
    if (radioConfig < RAIL_IEEE802154_PTI_RADIO_CONFIG_863MHZ_GB868) {
      status = RAIL_IEEE802154_ConfigSignalIdentifier(railHandle, (RAIL_IEEE802154_SignalIdentifierMode_t)enable);
      if (status == RAIL_STATUS_NO_ERROR) {
        status = RAIL_IEEE802154_EnableSignalDetection(railHandle, enable);
      }
    } else {
      RAIL_IEEE802154_ConfigSignalIdentifier(railHandle, RAIL_IEEE802154_SIGNAL_IDENTIFIER_MODE_DISABLE);
      status = RAIL_STATUS_INVALID_CALL;
    }
  } else {
    RAIL_IEEE802154_ConfigSignalIdentifier(railHandle, RAIL_IEEE802154_SIGNAL_IDENTIFIER_MODE_DISABLE);
    status = RAIL_STATUS_INVALID_CALL;
  }
  responsePrint(sl_cli_get_command_string(args, 0), "Result:%s",
                ((status == RAIL_STATUS_NO_ERROR) ? "Success"
                 : (status == RAIL_STATUS_INVALID_CALL) ? "Invalid Call"
                 : "Failure"
                ));
#else
  responsePrint(sl_cli_get_command_string(args, 0),
                "Signal identifier unsupported");
#endif
}
