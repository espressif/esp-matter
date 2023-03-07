/***************************************************************************//**
 * @file
 * @brief This file implements commands for configuring BLE RAIL options
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
#include <string.h>

#include "response_print.h"

#include "rail.h"
#include "rail_ble.h"

#include "app_common.h"

static const uint8_t blePacket[] = {
  0x02, 0x1A, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x02,
  0x01, 0x06, 0x10, 0x08, 'S', 'i', 'l', 'a', 'b', 's',
  ' ', 'R', 'A', 'I', 'L', 'T', 'E', 'S', 'T',
};
PhySwitchToRx_t phySwitchToRx;

void bleStatus(sl_cli_command_arg_t *args);

void bleEnable(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);

  // Turn BLE mode on or off as requested
  if (enable) {
    disableIncompatibleProtocols(RAIL_PTI_PROTOCOL_BLE);
    RAIL_BLE_Init(railHandle);
  } else {
    RAIL_BLE_Deinit(railHandle);
  }

  // Report the current status of BLE mode
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  bleStatus(args);
}

void bleStatus(sl_cli_command_arg_t *args)
{
  bool enabled = RAIL_BLE_IsEnabled(railHandle);

  // Report the current enabled status for BLE
  responsePrint(sl_cli_get_command_string(args, 0), "BLE:%s", enabled ? "Enabled" : "Disabled");
}

void bleSet1MbpsPhy(sl_cli_command_arg_t *args)
{
  RAIL_Status_t status;
  bool isViterbi;
  if (sl_cli_get_argument_count(args) == 1) {
    isViterbi = !!sl_cli_get_argument_uint8(args, 0);
  } else {
    isViterbi = 0;
  }

  if (isViterbi) {
    status = RAIL_BLE_ConfigPhy1MbpsViterbi(railHandle);
  } else {
    status = RAIL_BLE_ConfigPhy1Mbps(railHandle);
  }
  responsePrint(sl_cli_get_command_string(args, 0), "Status:%s", getStatusMessage(status));
}

void bleSet2MbpsPhy(sl_cli_command_arg_t *args)
{
  RAIL_Status_t status;
  bool isViterbi;
  if (sl_cli_get_argument_count(args) == 1) {
    isViterbi = !!sl_cli_get_argument_uint8(args, 0);
  } else {
    isViterbi = 0;
  }

  if (isViterbi) {
    status = RAIL_BLE_ConfigPhy2MbpsViterbi(railHandle);
  } else {
    status = RAIL_BLE_ConfigPhy2Mbps(railHandle);
  }
  responsePrint(sl_cli_get_command_string(args, 0), "Status:%s", getStatusMessage(status));
}

void bleSetCoding(sl_cli_command_arg_t *args)
{
  // Make sure BLE mode is enabled so that the call below can succeed
  if (!RAIL_BLE_IsEnabled(railHandle)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x31, "BLE mode not enabled");
    return;
  }

  RAIL_BLE_Coding_t coding = (RAIL_BLE_Coding_t) sl_cli_get_argument_uint8(args, 0);
  RAIL_Status_t status = RAIL_BLE_ConfigPhyCoded(railHandle, coding);
  responsePrint(sl_cli_get_command_string(args, 0), "Status:%s", getStatusMessage(status));
}

void bleSetSimulscan(sl_cli_command_arg_t *args)
{
  // Make sure BLE mode is enabled so that the call below can succeed
  if (!RAIL_BLE_IsEnabled(railHandle)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x31, "BLE mode not enabled");
    return;
  }

  RAIL_Status_t status = RAIL_BLE_ConfigPhySimulscan(railHandle);
  responsePrint(sl_cli_get_command_string(args, 0), "Status:%s", getStatusMessage(status));
}

// channel, accessAddress, crcInit, whitening
void bleSetChannelParams(sl_cli_command_arg_t *args)
{
  // Default to the parameters for advertising channels
  uint32_t accessAddress = 0x8E89BED6UL;
  uint32_t crcInit = 0x00555555UL;
  uint8_t logicalChannel = 37;
  bool disableWhitening = false;
  RAIL_Status_t res;

  // Make sure BLE mode is enabled so that the call below can succeed
  if (!RAIL_BLE_IsEnabled(railHandle)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x31, "BLE mode not enabled");
    return;
  }

  if (sl_cli_get_argument_count(args) >= 1) {
    logicalChannel = sl_cli_get_argument_uint8(args, 0);
  }
  if (sl_cli_get_argument_count(args) >= 2) {
    accessAddress = sl_cli_get_argument_uint32(args, 1);
  }
  if (sl_cli_get_argument_count(args) >= 3) {
    crcInit = sl_cli_get_argument_uint32(args, 2);
  }
  if (sl_cli_get_argument_count(args) >= 4) {
    disableWhitening = !!sl_cli_get_argument_uint8(args, 3);
  }

  res = RAIL_BLE_ConfigChannelRadioParams(railHandle,
                                          crcInit,
                                          accessAddress,
                                          logicalChannel,
                                          disableWhitening);
  if (res == RAIL_STATUS_NO_ERROR) {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "LogicalChannel:%d,"
                  "AccessAddress:0x%0.8X,"
                  "CrcInit:0x%0.8X,"
                  "Whitening:%s",
                  logicalChannel,
                  accessAddress,
                  crcInit,
                  disableWhitening ? "Disabled" : "Enabled");
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x32, "Setting channel parameters failed");
  }
}

void bleAdvertisingConfig(sl_cli_command_arg_t *args)
{
  uint8_t advChannel = sl_cli_get_argument_uint8(args, 0);
  RAIL_Status_t res;

  // Make sure BLE mode is enabled so that the call below can succeed
  if (!RAIL_BLE_IsEnabled(railHandle)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x31, "BLE mode not enabled");
    return;
  }

  if ((advChannel < 37) || (advChannel > 39)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x30, "Invalid advertising channel");
    return;
  }

  // Configure RAIL for BLE advertising on channel 37
  res = RAIL_BLE_ConfigChannelRadioParams(railHandle,  // RAIL instance
                                          0x00555555UL, // CRC Init
                                          0x8E89BED6UL, // Access Address
                                          advChannel,  // Channel
                                          false);      // Disable Whitening
  if (res != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x32, "Setting channel parameters failed");
    return;
  }

  if (advChannel == 37) {
    changeChannel(0); // Logical channel 37 is physical channel 0
  } else if (advChannel == 38) {
    changeChannel(12); // Logical channel 38 is physical channel 12
  } else if (advChannel == 39) {
    changeChannel(39); // Logical channel 39 is physical channel 39
  }

  responsePrint(sl_cli_get_command_string(args, 0), "AdvertisingChannel:%d", advChannel);

  // Load up a suitable advertising packet
  memcpy(txData, blePacket, sizeof(blePacket));
  txDataLen = sizeof(blePacket);
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  printTxPacket(args);
}

void blePhySwitchToRx(sl_cli_command_arg_t *args)
{
  phySwitchToRx.enable = !!sl_cli_get_argument_uint8(args, 0);
  if (phySwitchToRx.enable) {
    phySwitchToRx.phy = RAIL_BLE_1Mbps;
    phySwitchToRx.physicalChannel = 0U;
    phySwitchToRx.timeDelta = 1000U;
    phySwitchToRx.crcInit = 0x00555555UL;
    phySwitchToRx.accessAddress = 0x8E89BED6UL;
    phySwitchToRx.logicalChannel = 37U;
    phySwitchToRx.disableWhitening = false;
    phySwitchToRx.extraDelayUs = 0U;

    if (sl_cli_get_argument_count(args) >= 2) {
      phySwitchToRx.phy = sl_cli_get_argument_uint8(args, 1);
    }
    if (sl_cli_get_argument_count(args) >= 3) {
      phySwitchToRx.timeDelta = sl_cli_get_argument_uint32(args, 2);
    }
    if (sl_cli_get_argument_count(args) >= 4) {
      phySwitchToRx.physicalChannel = sl_cli_get_argument_uint16(args, 3);
    }
    if (sl_cli_get_argument_count(args) >= 5) {
      phySwitchToRx.logicalChannel = sl_cli_get_argument_uint16(args, 4);
    }
    if (sl_cli_get_argument_count(args) >= 6) {
      phySwitchToRx.accessAddress = sl_cli_get_argument_uint32(args, 5);
    }
    if (sl_cli_get_argument_count(args) >= 7) {
      phySwitchToRx.crcInit = sl_cli_get_argument_uint32(args, 6);
    }
    if (sl_cli_get_argument_count(args) >= 8) {
      phySwitchToRx.disableWhitening = !!sl_cli_get_argument_uint8(args, 7);
    }
    if (sl_cli_get_argument_count(args) >= 9) {
      phySwitchToRx.extraDelayUs = sl_cli_get_argument_uint8(args, 8);
    }
  }

  char *enabled = phySwitchToRx.enable ? "Enabled" : "Disabled";
  responsePrint(sl_cli_get_command_string(args, 0), "PhySwitchToRx:%s", enabled);
}

void enableBleSignalIdentifier(sl_cli_command_arg_t *args)
{
#if RAIL_BLE_SUPPORTS_SIGNAL_IDENTIFIER
  RAIL_Status_t status;
  RAIL_BLE_SignalIdentifierMode_t siMode = (RAIL_BLE_SignalIdentifierMode_t)sl_cli_get_argument_uint8(args, 0);
  bool enable = (siMode != RAIL_BLE_SIGNAL_IDENTIFIER_MODE_DISABLE);

  if (RAIL_BLE_IsEnabled(railHandle)) {
    status = RAIL_BLE_ConfigSignalIdentifier(railHandle, siMode);
    if (status == RAIL_STATUS_NO_ERROR) {
      status = RAIL_BLE_EnableSignalDetection(railHandle, enable);
    }
  } else {
    // Disable the signal identifier just to be safe. Ignore the return type
    // as we know it would be invalid call if it was not configured.
    (void) RAIL_BLE_ConfigSignalIdentifier(railHandle, RAIL_BLE_SIGNAL_IDENTIFIER_MODE_DISABLE);
    status = RAIL_STATUS_INVALID_CALL;
  }
  responsePrint(sl_cli_get_command_string(args, 0), "Result:%s",
                ((status == RAIL_STATUS_NO_ERROR) ? "Success"
                 : (status == RAIL_STATUS_INVALID_CALL) ? "Invalid Call"
                 : (status == RAIL_STATUS_INVALID_PARAMETER) ? "Invalid Parameter"
                 : "Failure"
                ));
#else
  responsePrint(sl_cli_get_command_string(args, 0),
                "Signal identifier unsupported");
#endif
}
