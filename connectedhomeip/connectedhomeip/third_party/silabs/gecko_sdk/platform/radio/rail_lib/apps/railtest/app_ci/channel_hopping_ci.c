/***************************************************************************//**
 * @file
 * @brief This file implements the autoack commands in RAILtest apps.
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
#include "rail_features.h"
#include "app_common.h"
#include "rail_zwave.h"

#if RAIL_SUPPORTS_CHANNEL_HOPPING

static RAIL_RxChannelHoppingConfigEntry_t channelHoppingEntries[MAX_NUMBER_CHANNELS];
static RAIL_RxChannelHoppingConfigMultiMode_t multiModeParams[MAX_NUMBER_CHANNELS];
static RAIL_RxChannelHoppingConfig_t channelHoppingConfig = {
  .entries = channelHoppingEntries,
  .bufferLength = CHANNEL_HOPPING_BUFFER_SIZE,
  .numberOfChannels = MAX_NUMBER_CHANNELS
};
// Enable RX duty cycle with power manager schedule wakeup
static bool enableRxDutyCycleWithSchedWakeup = false;
// RX Duty cycle delay to schedule the periodic wakeup
static RAIL_Time_t rxDutyCycleSchedWakeupDelayUs = 0U;

void configRxChannelHopping(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  RAIL_Status_t status;
  uint8_t i = 0U;
  channelHoppingConfig.buffer = channelHoppingBuffer;

  if (sl_cli_get_argument_count(args) == 0) {
#if RAIL_SUPPORTS_PROTOCOL_ZWAVE
    // If no arguments are provided, use the Z-Wave timings set by the calculator
    channelHoppingConfig.entries = channelHoppingEntries;
    channelHoppingConfig.bufferLength = CHANNEL_HOPPING_BUFFER_SIZE;
    channelHoppingConfig.numberOfChannels = MAX_NUMBER_CHANNELS;
    status = RAIL_ZWAVE_ConfigRxChannelHopping(railHandle, &channelHoppingConfig);
    responsePrint(sl_cli_get_command_string(args, 0),
                  "numberOfChannels:%d,paramCh0:%d,paramCh1:%d,paramCh2:%d,paramCh3:%d,buffer:0x%x,Success:%s",
                  channelHoppingConfig.numberOfChannels,
                  channelHoppingConfig.entries[0].parameter,
                  channelHoppingConfig.entries[1].parameter,
                  channelHoppingConfig.entries[2].parameter,
                  channelHoppingConfig.entries[3].parameter,
                  channelHoppingConfig.buffer,
                  status == RAIL_STATUS_NO_ERROR ? "True" : "False");
#else
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "Insufficient arguments. Must provide channel hopping sequence.");
#endif
    return;
  } else {
    for (i = 0U; (i + 1) * 4 < (sl_cli_get_argument_count(args) + 1); i++) {
      if (sl_cli_get_argument_uint32(args, ((i + 1) * 4) - 4) > (uint32_t)UINT16_MAX) {
        responsePrintError(sl_cli_get_command_string(args, 0), 0x16, "Channel must be a 16 bit value.");
        return;
      }
      channelHoppingEntries[i].channel   = sl_cli_get_argument_uint32(args, ((i + 1) * 4) - 4);
      channelHoppingEntries[i].mode      = sl_cli_get_argument_uint32(args, ((i + 1) * 4) - 3);
      if ((channelHoppingEntries[i].mode & ~RAIL_RX_CHANNEL_HOPPING_MODES_WITH_OPTIONS_BASE)
          == RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE) {
        channelHoppingEntries[i].parameter = (uint32_t)(void *)(&multiModeParams[i]);
      } else {
        channelHoppingEntries[i].parameter = sl_cli_get_argument_uint32(args, ((i + 1) * 4) - 2);
      }
      channelHoppingEntries[i].delay     = sl_cli_get_argument_uint32(args, ((i + 1) * 4) - 1);
      channelHoppingEntries[i].delayMode = RAIL_RX_CHANNEL_HOPPING_DELAY_MODE_STATIC;
    }
    channelHoppingConfig.numberOfChannels = i;
    status = RAIL_ConfigRxChannelHopping(railHandle, &channelHoppingConfig);
    responsePrint(sl_cli_get_command_string(args, 0), "numberOfChannels:%d,buffer:0x%x,Success:%s",
                  i,
                  channelHoppingConfig.buffer,
                  status == RAIL_STATUS_NO_ERROR ? "True" : "False");
  }
}

void configChannelHoppingOptions(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  uint8_t index = sl_cli_get_argument_uint8(args, 0);
  if (index >= COUNTOF(channelHoppingEntries)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x16,
                       "Index must be < %u", COUNTOF(channelHoppingEntries));
    return;
  }

  RAIL_RxChannelHoppingOptions_t options
    = (RAIL_RxChannelHoppingOptions_t)sl_cli_get_argument_uint8(args, 1);
  int8_t rssiThresholdDbm = channelHoppingEntries[index].rssiThresholdDbm;

  channelHoppingEntries[index].options = options;

  if (sl_cli_get_argument_count(args) >= 3) {
    rssiThresholdDbm = sl_cli_get_argument_int8(args, 2);
    channelHoppingEntries[index].rssiThresholdDbm = rssiThresholdDbm;
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "index:%u,options:%u,rssiThreshold:%d",
                index, options, rssiThresholdDbm);
}

void configChannelHoppingMulti(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  uint8_t index = sl_cli_get_argument_uint8(args, 0);
  if (index >= COUNTOF(channelHoppingEntries)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x17,
                       "Index must be < %u", COUNTOF(channelHoppingEntries));
    return;
  }

  multiModeParams[index].syncDetect    = sl_cli_get_argument_uint32(args, 1);
  multiModeParams[index].preambleSense = sl_cli_get_argument_uint32(args, 2);
  multiModeParams[index].timingSense   = sl_cli_get_argument_uint32(args, 3);
  multiModeParams[index].timingReSense = sl_cli_get_argument_uint32(args, 4);
  multiModeParams[index].status        = 0U;

  responsePrint(sl_cli_get_command_string(args, 0),
                "index:%u,syncDetect:%u,preambleSense:%u,timingSense:%u,timingReSense:%u",
                index,
                multiModeParams[index].syncDetect,
                multiModeParams[index].preambleSense,
                multiModeParams[index].timingSense,
                multiModeParams[index].timingReSense);
}

void enableRxChannelHopping(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  bool enable = !!sl_cli_get_argument_uint8(args, 0);

  bool reset = false;
  if (sl_cli_get_argument_count(args) >= 2) {
    reset = !!sl_cli_get_argument_uint8(args, 1);
  }

  RAIL_Status_t status = RAIL_EnableRxChannelHopping(railHandle, enable, reset);
  responsePrint(sl_cli_get_command_string(args, 0), "Success:%s", status == RAIL_STATUS_NO_ERROR ? "True" : "False");
}

void getChannelHoppingRssi(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint8_t channelIndex = sl_cli_get_argument_uint8(args, 0);

  int16_t result = RAIL_GetChannelHoppingRssi(railHandle, channelIndex);

  char bufRssi[10];

  // The lowest negative value is used to indicate an error reading the RSSI
  if (result == RAIL_RSSI_INVALID) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x08, "Could not read RSSI. Ensure channel hopping was configured and enabled.");
    return;
  }

  sprintfFloat(bufRssi, sizeof(bufRssi), ((float) result / 4), 2);

  responsePrint(sl_cli_get_command_string(args, 0), "rssi:%s", bufRssi);
}

void configRxDutyCycle(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  RAIL_RxChannelHoppingMode_t mode;
  int32_t parameters[4] = { 0, };
  int32_t delay;
  RAIL_RxChannelHoppingOptions_t options = RAIL_RX_CHANNEL_HOPPING_OPTIONS_NONE;
  int8_t rssiThresholdDbm = RAIL_RSSI_INVALID_DBM;
  uint32_t currentArg = 0U;
  uint32_t argCount = sl_cli_get_argument_count(args);
  uint32_t neededArgCount = 3U; // mode parameter delay minimally needed

  mode = (RAIL_RxChannelHoppingMode_t) sl_cli_get_argument_uint32(args, currentArg++);
  bool modeIsMulti = ((mode & ~RAIL_RX_CHANNEL_HOPPING_MODES_WITH_OPTIONS_BASE)
                      == RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE);
  if (modeIsMulti) {
    neededArgCount += 3U; // needs timingReSense preambleSense syncDetect params
  }
  if ((mode & RAIL_RX_CHANNEL_HOPPING_MODES_WITH_OPTIONS_BASE) != 0U) {
    neededArgCount += 1U; // needs options
  }
  if (argCount < neededArgCount) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "Insufficient arguments for specified mode");
    return;
  }
  parameters[0] = sl_cli_get_argument_int32(args, currentArg++);
  if (modeIsMulti) {
    parameters[1] = sl_cli_get_argument_int32(args, currentArg++);
    parameters[2] = sl_cli_get_argument_int32(args, currentArg++);
    parameters[3] = sl_cli_get_argument_int32(args, currentArg++);
  }
  delay = sl_cli_get_argument_int32(args, currentArg++);
  if ((mode & RAIL_RX_CHANNEL_HOPPING_MODES_WITH_OPTIONS_BASE) != 0U) {
    options = (RAIL_RxChannelHoppingOptions_t) sl_cli_get_argument_int32(args, currentArg++);
    if ((options & RAIL_RX_CHANNEL_HOPPING_OPTION_RSSI_THRESHOLD)
        != RAIL_RX_CHANNEL_HOPPING_OPTIONS_NONE) {
      if (currentArg >= argCount) {
        responsePrintError(sl_cli_get_command_string(args, 0), 0x13,
                           "Missing rssiThreshold argument for specified option");
        return;
      }
      rssiThresholdDbm = (int8_t) sl_cli_get_argument_int32(args, currentArg++);
    }
  }
  if (currentArg != argCount) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x14,
                       "Unexpected extra arguments for specified mode/options");
    return;
  }

  multiModeParams[0].syncDetect    = (uint32_t)parameters[0];
  multiModeParams[0].preambleSense = (uint32_t)parameters[1];
  multiModeParams[0].timingSense   = (uint32_t)parameters[2];
  multiModeParams[0].timingReSense = (uint32_t)parameters[3];
  multiModeParams[0].status        = 0U;
  RAIL_RxDutyCycleConfig_t config = {
    .mode = mode,
    .parameter = (modeIsMulti
                  ? ((uint32_t)(void *)&multiModeParams[0])
                  : ((uint32_t)parameters[0])),
    .delay = (uint32_t)delay,
    .delayMode = RAIL_RX_CHANNEL_HOPPING_DELAY_MODE_STATIC,
    .options = options,
    .rssiThresholdDbm = rssiThresholdDbm,
  };

  rxDutyCycleSchedWakeupDelayUs = delay;

  RAIL_Status_t status = RAIL_ConfigRxDutyCycle(railHandle, &config);

  responsePrint(sl_cli_get_command_string(args, 0),
                "Success:%s,Mode:0x%x,Parameter:%u,Delay:%d,Options:%u,rssiThreshold:%d",
                status == RAIL_STATUS_NO_ERROR ? "True" : "False",
                config.mode,
                config.parameter,
                config.delay,
                config.options,
                config.rssiThresholdDbm);
}

void getDefaultRxDutyCycleConfig(sl_cli_command_arg_t *args)
{
  RAIL_RxDutyCycleConfig_t config = { 0 };

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));

  RAIL_Status_t status = RAIL_GetDefaultRxDutyCycleConfig(railHandle,
                                                          &config);
  responsePrint(sl_cli_get_command_string(args, 0),
                "Success:%s,Mode:0x%x,Parameter:%u,Delay:%d,Options:%u,rssiThreshold:%d",
                status == RAIL_STATUS_NO_ERROR ? "True" : "False",
                config.mode,
                config.parameter,
                config.delay,
                config.options,
                config.rssiThresholdDbm);
}

void enableRxDutyCycle(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  enableRxDutyCycleWithSchedWakeup = false;
  serEvent = false;
  bool enable = !!sl_cli_get_argument_uint8(args, 0);

  if (sl_cli_get_argument_count(args) >= 2) {
    enableRxDutyCycleWithSchedWakeup = !!(sl_cli_get_argument_uint8(args, 1));
  }

  RAIL_Status_t status = RAIL_EnableRxDutyCycle(railHandle, enable);
  responsePrint(sl_cli_get_command_string(args, 0), "Success:%s", status == RAIL_STATUS_NO_ERROR ? "True" : "False");
}

bool getRxDutyCycleSchedWakeupEnable(RAIL_Time_t *sleepInterval)
{
  bool schedWakeupEnable = false;

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  if (sleepInterval != NULL) {
    *sleepInterval = rxDutyCycleSchedWakeupDelayUs;
    schedWakeupEnable = enableRxDutyCycleWithSchedWakeup;
  }

  CORE_EXIT_CRITICAL();

  return schedWakeupEnable;
}

RAIL_ChannelMetadata_t channelMetadata[MAX_NUMBER_CHANNELS];
#define TIME_PER_CHANNEL (10000UL)
void spectrumAnalyzer(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  uint16_t channelMetadataLength = MAX_NUMBER_CHANNELS;

  uint16_t minChannel = 0U;
  if (sl_cli_get_argument_count(args) >= 2) {
    minChannel = sl_cli_get_argument_uint16(args, 1);
  }

  uint16_t maxChannel = UINT16_MAX;
  if (sl_cli_get_argument_count(args) >= 3) {
    maxChannel = sl_cli_get_argument_uint16(args, 2);
  }

  RAIL_Status_t status = RAIL_GetChannelMetadata(railHandle,
                                                 channelMetadata,
                                                 &channelMetadataLength,
                                                 minChannel,
                                                 maxChannel);

  if (channelMetadataLength == 0) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x1, "No channels in range found");
    return;
  }
  bool graphics = false;
  if (sl_cli_get_argument_count(args) >= 1) {
    graphics = !!sl_cli_get_argument_uint8(args, 0);
  }

  if (status == RAIL_STATUS_INVALID_STATE) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x20, "Error in RAIL_GetChannelMetadata, channel config invalid.");
    return;
  }

  uint8_t length = MAX_NUMBER_CHANNELS < channelMetadataLength
                   ? MAX_NUMBER_CHANNELS : channelMetadataLength;
  for (uint16_t i = 0; i < length; i++) {
    channelHoppingEntries[i].channel = channelMetadata[i].channel;
    channelHoppingEntries[i].mode = RAIL_RX_CHANNEL_HOPPING_MODE_TIMEOUT;
    channelHoppingEntries[i].parameter = TIME_PER_CHANNEL;
    channelHoppingEntries[i].delay = 0;
    channelHoppingEntries[i].delayMode = RAIL_RX_CHANNEL_HOPPING_DELAY_MODE_STATIC;
  }
  channelHoppingConfig.numberOfChannels = channelMetadataLength;
  channelHoppingConfig.buffer = channelHoppingBuffer;

  RAIL_ConfigRxChannelHopping(railHandle, &channelHoppingConfig);
  RAIL_EnableRxChannelHopping(railHandle, true, true);
  RAIL_StartRx(railHandle, channelMetadata[0].channel, NULL);

  // Make sure we get into receive and then have enough time to
  // cycle through all the channels (time per channel * (num(channels) + safety buffer))
  while (RAIL_GetRadioState(railHandle) != RAIL_RF_STATE_RX) ;
  RAIL_DelayUs(TIME_PER_CHANNEL * (channelMetadataLength + 5));

  responsePrintHeader(sl_cli_get_command_string(args, 0), "channelIndex:%u,channel:%u,frequency:%u,rssi:%s");
  int16_t min = INT16_MAX;
  int16_t max = INT16_MIN;
  int16_t result[MAX_NUMBER_CHANNELS];
  for (uint16_t i = 0; i < channelMetadataLength; i++) {
    result[i] = RAIL_GetChannelHoppingRssi(railHandle, i);
    if (result[i] > max) {
      max = result[i];
    }
    if (result[i] < min) {
      min = result[i];
    }
    if (result[i] == RAIL_RSSI_INVALID) {
      continue;
    }
    char bufRssi[10];
    sprintfFloat(bufRssi, sizeof(bufRssi), ((float) result[i] / 4), 2);
    responsePrintMulti("channelIndex:%u,channel:%u,frequency:%u,rssi:%s",
                       i, channelMetadata[i].channel,
                       channelMetadata[i].frequency, bufRssi);
  }

  RAIL_Idle(railHandle, RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS, true);

  if (!graphics) {
    return;
  }

  printf("\n");
  printf("+----------------------------+\n");
  printf("| RSSI dBm by Channel Number |\n");
  printf("+----------------------------+\n");
  char bufRssi[10];
  for (uint8_t i = 0; i <= 20; i++) {
    for (uint8_t j = 0; j < channelMetadataLength; j++) {
      if (i != 20) {
        for (uint8_t k = 0; k < 4; k++) {
          if (result[j] >= (max - (((max - min) / 20) * i))) {
            printf("#");
          } else {
            printf(".");
          }
        }
      } else {
        printf("%-4d", channelMetadata[j].channel);
      }
    }
    sprintfFloat(bufRssi, sizeof(bufRssi), ((float) (max - (((max - min) / 20) * i)) / 4), 2);
    printf(" %s dBm\n", bufRssi);
  }
  printf("Channel Number\n");
}

#else

static void channelHoppingNotSupported(sl_cli_command_arg_t *args)
{
  responsePrintError(sl_cli_get_command_string(args, 0), 0x17, "Channel hopping not suppported on this chip");
}

void configRxChannelHopping(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  channelHoppingNotSupported(args);
}

void enableRxChannelHopping(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  channelHoppingNotSupported(args);
}

void getChannelHoppingRssi(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  channelHoppingNotSupported(args);
}

void configRxDutyCycle(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  channelHoppingNotSupported(args);
}

void enableRxDutyCycle(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  channelHoppingNotSupported(args);
}

void getDefaultRxDutyCycleConfig(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  channelHoppingNotSupported(args);
}

void spectrumAnalyzer(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  channelHoppingNotSupported(args);
}

void configChannelHoppingOptions(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  channelHoppingNotSupported(args);
}

void configChannelHoppingMulti(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  channelHoppingNotSupported(args);
}
bool getRxDutyCycleSchedWakeupEnable(RAIL_Time_t *sleepInterval)
{
  (void)sleepInterval;
  return false;
}

#endif

void setNextTxRepeat(sl_cli_command_arg_t *args)
{
  RAIL_TxRepeatConfig_t repeats = {
    .iterations = sl_cli_get_argument_uint16(args, 0),
    .repeatOptions = RAIL_TX_REPEAT_OPTIONS_NONE,
    .delayOrHop.delay = RAIL_TRANSITION_TIME_KEEP,
  };
  uint32_t argc = sl_cli_get_argument_count(args);
  char *command = sl_cli_get_command_string(args, 0);

  if (argc < 2U) {
    // repeats already setup appropriately
  } else if (argc <= 2U) {
    repeats.delayOrHop.delay = sl_cli_get_argument_uint32(args, 1);
  } else { // argc > 2U
#if RAIL_SUPPORTS_CHANNEL_HOPPING
    RAIL_TxChannelHoppingConfigEntry_t *txChannelHoppingEntries =
      (RAIL_TxChannelHoppingConfigEntry_t *) channelHoppingEntries;
    uint32_t i = 0U;
    for (uint32_t arg = 2U; arg < argc; arg += 2U) {
      uint32_t channel = sl_cli_get_argument_uint32(args, arg);
      if (channel > (uint32_t)UINT16_MAX) {
        responsePrintError(command, 0x16, "Channel must be a 16 bit value.");
        return;
      }
      txChannelHoppingEntries[i].delay = sl_cli_get_argument_uint32(args, arg - 1U);
      txChannelHoppingEntries[i].channel = (uint16_t)channel;
      i++;
    }
    repeats.repeatOptions = RAIL_TX_REPEAT_OPTION_HOP;
    repeats.delayOrHop.channelHopping.numberOfChannels = i;
    repeats.delayOrHop.channelHopping.buffer = channelHoppingBuffer;
    repeats.delayOrHop.channelHopping.bufferLength = CHANNEL_HOPPING_BUFFER_SIZE;
    repeats.delayOrHop.channelHopping.entries = txChannelHoppingEntries;
#else
    responsePrintError(command, 0x16, "Hopping not supported on this platform.");
    return;
#endif
  }

  RAIL_Status_t ret = RAIL_SetNextTxRepeat(railHandle, &repeats);
  if (ret != RAIL_STATUS_NO_ERROR) {
    repeats.iterations = 0;
  }
  txRepeatCount = repeats.iterations;
  responsePrint(command, "RepeatedTxCount:%u", repeats.iterations);
}

void setNextBleTxRepeat(sl_cli_command_arg_t *args)
{
  RAIL_BLE_TxRepeatConfig_t repeats = {
    .iterations = sl_cli_get_argument_uint16(args, 0),
    .repeatOptions = RAIL_TX_REPEAT_OPTIONS_NONE,
    .delayOrHop.delay = RAIL_TRANSITION_TIME_KEEP,
  };
  uint32_t argc = sl_cli_get_argument_count(args);
  char *command = sl_cli_get_command_string(args, 0);

  if (argc < 2U) {
    // repeats already setup appropriately
  } else if (argc <= 2U) {
    repeats.delayOrHop.delay = sl_cli_get_argument_uint32(args, 1);
  } else if (argc <= 5U) {
    responsePrintError(command, 0x17, "Need at least 6 parameters for BLE hop config.");
    return;
  } else { // argc > 5U
#if RAIL_SUPPORTS_CHANNEL_HOPPING
    // Default parameters to not exceed CLI parameter limits
    uint32_t crcInit = 0x00555555UL;
    bool disableWhitening = false;

    RAIL_BLE_TxChannelHoppingConfigEntry_t *txChannelHoppingEntries =
      (RAIL_BLE_TxChannelHoppingConfigEntry_t *) channelHoppingEntries;
    uint32_t i = 0U;
    for (uint32_t arg = 2U; arg < argc; arg += 5U) {
      uint32_t railChannel    = sl_cli_get_argument_uint32(args, arg + 1U);
      uint32_t logicalChannel = sl_cli_get_argument_uint32(args, arg + 2U);
      if (railChannel >= 40U) {
        responsePrintError(command, 0x27, "RAIL channel must be a valid BLE channel.");
      }
      if (logicalChannel >= 40U) {
        responsePrintError(command, 0x28, "Logical channel must be a valid BLE channel.");
      }

      txChannelHoppingEntries[i].delay            = sl_cli_get_argument_uint32(args, arg - 1U);
      txChannelHoppingEntries[i].phy              = sl_cli_get_argument_uint32(args, arg);
      txChannelHoppingEntries[i].railChannel      = (uint8_t)railChannel;
      txChannelHoppingEntries[i].logicalChannel   = (uint8_t)logicalChannel;
      txChannelHoppingEntries[i].accessAddress    = sl_cli_get_argument_uint32(args, arg + 3U);

      txChannelHoppingEntries[i].disableWhitening = disableWhitening;
      txChannelHoppingEntries[i].crcInit          = crcInit;
      i++;
    }
    repeats.repeatOptions = RAIL_TX_REPEAT_OPTION_HOP;
    repeats.delayOrHop.channelHopping.numberOfChannels = i;
    repeats.delayOrHop.channelHopping.buffer = channelHoppingBuffer;
    repeats.delayOrHop.channelHopping.bufferLength = CHANNEL_HOPPING_BUFFER_SIZE;
    repeats.delayOrHop.channelHopping.entries = txChannelHoppingEntries;
#else
    responsePrintError(command, 0x16, "Hopping not supported on this platform.");
    return;
#endif
  }

  RAIL_Status_t ret = RAIL_BLE_SetNextTxRepeat(railHandle, &repeats);
  if (ret != RAIL_STATUS_NO_ERROR) {
    repeats.iterations = 0;
  }
  txRepeatCount = repeats.iterations;
  responsePrint(command, "RepeatedTxCount:%u", repeats.iterations);
}
