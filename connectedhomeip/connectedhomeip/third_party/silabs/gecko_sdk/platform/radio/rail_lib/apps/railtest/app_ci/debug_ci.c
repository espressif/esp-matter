/***************************************************************************//**
 * @file
 * @brief This file implements the debug commands for RAILtest applications.
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

#if !defined(__ICCARM__)
// IAR doesn't have strings.h and puts those declarations in string.h
#include <strings.h>
#endif

#include "response_print.h"
#include "rail_types.h"

#include "rail.h"
#include "app_common.h"
#include "app_trx.h"
#include "hal_common.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "rail_features.h"
#include "sl_rail_util_init.h"

#include "em_prs.h"

#if SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
  #include "rail_config.h"
#endif // SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE

uint32_t rxOverflowDelay = 10 * 1000000; // 10 seconds
uint32_t thermistorResistance = 0;

#define MAX_DEBUG_BYTES (128)
static char debugPrintBuffer[MAX_DEBUG_BYTES];
static uint8_t debugDataBuffer[MAX_DEBUG_BYTES];

/*
 * setFrequency
 *
 * Allows the user to set an arbitrary frequency if
 * the frequency override debug mode is enabled.
 */
void setFrequency(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint32_t newFrequency = sl_cli_get_argument_uint32(args, 0);

  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  if ((RAIL_GetDebugMode(railHandle) & RAIL_DEBUG_MODE_FREQ_OVERRIDE) == RAIL_DEBUG_MODE_FREQ_OVERRIDE) {
    if (!RAIL_OverrideDebugFrequency(railHandle, newFrequency)) {
      responsePrint(sl_cli_get_command_string(args, 0), "NewFrequency:%u", newFrequency);
    } else {
      // This won't take effect until we parse divider ranges.
      responsePrintError(sl_cli_get_command_string(args, 0), 0x14, "%u Hz is out of range and cannot be "
                                                                   "set as the frequency", newFrequency);
    }
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x13, "Not currently in frequency override debug mode.");
  }
}

char * lookupDebugModeString(uint32_t debugMode)
{
  if (debugMode == 0) {
    return "Disabled";
  }
  if (debugMode & RAIL_DEBUG_MODE_FREQ_OVERRIDE) {
    return "Frequency Override";
  }

  return "NULL";
}
/*
 * setDebugMode()
 *
 * Configure RAIL into a debug mode.
 *
 * Current debug modes:
 * RAIL_DEBUG_MODE_FREQ_OVERRIDE - Disable RAIL's channel scheme and
 * uses a specific frequency defined by the user.
 */
void setDebugMode(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint32_t debugMode;

  debugMode = sl_cli_get_argument_uint32(args, 0);
  if (!RAIL_SetDebugMode(railHandle, debugMode)) {
    responsePrint(sl_cli_get_command_string(args, 0), "DebugMode:%s", lookupDebugModeString(debugMode));
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x15, "%d is an invalid debug mode!", debugMode);
  }
}

void getMemWord(sl_cli_command_arg_t *args)
{
  uint32_t *address = (uint32_t*)sl_cli_get_argument_uint32(args, 0);
  uint32_t count = 1;
  uint32_t value;

  // If there was a length given then read it out
  if (sl_cli_get_argument_count(args) >= 2) {
    count = sl_cli_get_argument_uint32(args, 1);
  }

  // Check for alignment
  if (((uint32_t)address % 4) != 0) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Address 0x%.8X is not 32bit aligned", address);
    return;
  }

  responsePrintHeader(sl_cli_get_command_string(args, 0), "address:0x%.8x,value:0x%.8x");
  for (uint32_t i = 0; i < count; i++) {
    value = address[i];
    responsePrintMulti("address:0x%.8x,value:0x%.8x", address + i, value);
  }
}

void setMemWord(sl_cli_command_arg_t *args)
{
  uint32_t *address = (uint32_t*)sl_cli_get_argument_uint32(args, 0);
  int count = 0;
  char lengthStr[12];

  // Check for alignment
  if (((uint32_t)address % 4) != 0) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Address 0x%.8X is not 32bit aligned", address);
    return;
  }

  // Write each word given sequentially
  for (int i = 2; i < (sl_cli_get_argument_count(args) + 1); i++) {
    address[count] = sl_cli_get_argument_uint32(args, i - 1);
    count++;
  }

  // Make getMemWord print out everything we just wrote to
  sprintf(lengthStr, "%d", count);
  args->argc = 3;
  args->argv[2] = lengthStr;
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getMemWord(args);
}

void setTxUnderflow(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  enableAppMode(TX_UNDERFLOW, enable, sl_cli_get_command_string(args, 0));
}

void setRxOverflow(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  if (sl_cli_get_argument_count(args) >= 2) {
    rxOverflowDelay = sl_cli_get_argument_uint32(args, 1);
  } else {
    // 10 seconds should be enough to trigger an overflow
    rxOverflowDelay = 10 * 1000000;
  }
  enableAppMode(RX_OVERFLOW, enable, sl_cli_get_command_string(args, 0));
}

void setCalibrations(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  skipCalibrations = !enable;
  responsePrint(sl_cli_get_command_string(args, 0), "Calibrations:%s", enable ? "Enabled" : "Disabled");
}

void txCancel(sl_cli_command_arg_t *args)
{
  int32_t delay = sl_cli_get_argument_uint32(args, 0);
  txCancelDelay = delay;
  txCancelMode = RAIL_STOP_MODES_NONE; // Default to using RAIL_Idle()
  if (sl_cli_get_argument_count(args) >= 2) {
    txCancelMode = sl_cli_get_argument_uint8(args, 1);
  }

  enableAppMode(TX_CANCEL, delay >= 0, sl_cli_get_command_string(args, 0)); // Pends transmit to cancel
}

void configHFXOThermistor(sl_cli_command_arg_t *args)
{
#if RAIL_FEAT_EXTERNAL_THERMISTOR
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  const RAIL_HFXOThermistorConfig_t hfxoThermistorConfig = {
    .port = GPIO_THMSW_EN_PORT,
    .pin = GPIO_THMSW_EN_PIN
  };

  RAIL_Status_t status = RAIL_ConfigHFXOThermistor(railHandle, &hfxoThermistorConfig);
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint(sl_cli_get_command_string(args, 0), "Configuration:Success");
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Error during configuration");
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif
  return;
}

void startThermistorMeasurement(sl_cli_command_arg_t *args)
{
#if RAIL_FEAT_EXTERNAL_THERMISTOR
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Status_t status = RAIL_StartThermistorMeasurement(railHandle);
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint(sl_cli_get_command_string(args, 0), "Thermistor measurement:Started.");
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Ongoing thermistor measurement or unconfigured modem.");
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif
  return;
}

void getThermistorImpedance(sl_cli_command_arg_t *args)
{
#if RAIL_FEAT_EXTERNAL_THERMISTOR
  RAIL_Status_t status;
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  status = RAIL_GetThermistorImpedance(railHandle, &thermistorResistance);

  if (status == RAIL_STATUS_NO_ERROR) {
    if (thermistorResistance != 0U) {
      int16_t thermistorTemperatureC;
      status = RAIL_ConvertThermistorImpedance(railHandle, thermistorResistance, &thermistorTemperatureC);

      if (status == RAIL_STATUS_NO_ERROR) {
        // Convert temperature (originally in eighth of Celsius degrees) in Celsius
        responsePrint(sl_cli_get_command_string(args, 0),
                      "Ohms:%u,DegreesC:%d",
                      thermistorResistance, thermistorTemperatureC / 8);
      } else {
        responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Ohms:%u,DegreesC:255", thermistorResistance);
      }
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Ohms:%u", thermistorResistance);
    }
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Thermistor measurement not done yet.");
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif
  return;
}

void getHFXOPPMError(sl_cli_command_arg_t *args)
{
#if RAIL_FEAT_EXTERNAL_THERMISTOR
  RAIL_Status_t status;
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));

  if (thermistorResistance != 0U) {
    int16_t thermistorTemperatureC;
    status = RAIL_ConvertThermistorImpedance(railHandle, thermistorResistance, &thermistorTemperatureC);

    if (status == RAIL_STATUS_NO_ERROR) {
      int8_t crystalPPMError;
      // Temperature is in eighth of celsius degrees
      RAIL_ComputeHFXOPPMError(railHandle, thermistorTemperatureC / 8, &crystalPPMError);
      responsePrint(sl_cli_get_command_string(args, 0),
                    "ErrorPpm:%d", crystalPPMError);
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Ohms:%u,DegreesC:255", thermistorResistance);
    }
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Ohms:%u", thermistorResistance);
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif
  return;
}

void getTemperature(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool reset = false;
    int16_t tempBuffer[RAIL_TEMP_MEASURE_COUNT];

    if (sl_cli_get_argument_count(args) > 0) {
      reset = (bool) sl_cli_get_argument_uint8(args, 0);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetTemperature(railHandle, tempBuffer, reset);

    if (status == RAIL_STATUS_NO_ERROR) {
      for (uint8_t i = 0; i < RAIL_TEMP_MEASURE_COUNT; i++) {
        tempBuffer[i] = ((tempBuffer[i] != 0) && (tempBuffer[i] != 0x7FFF)) ? (tempBuffer[i] - 273) : 0xFF;
      }

      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintContinue("chipTempTxStreamC:%d,"
                            "chipTempPreTxC:%d,"
                            "chipTempPostTxC:%d",
                            tempBuffer[0],
                            tempBuffer[1],
                            tempBuffer[2]);
      responsePrintContinue("effTempPreTxC:%d,"
                            "effTempPostTxC:%d,"
                            "effTempPreTxMinC:%d,"
                            "effTempPostTxMinC:%d,"
                            "effTempPreTxMaxC:%d,"
                            "effTempPostTxMaxC:%d,"
                            "effTempPreTxAvgC:%d,"
                            "effTempPostTxAvgC:%d",
                            tempBuffer[3],
                            tempBuffer[4],
                            tempBuffer[5],
                            tempBuffer[6],
                            tempBuffer[7],
                            tempBuffer[8],
                            tempBuffer[9],
                            tempBuffer[10]);
      responsePrintEnd("xtalTempPreTxK:%d,"
                       "xtalTempPostTxK:%d",
                       tempBuffer[11],
                       tempBuffer[12]);
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get temperature metrics.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetFemProtectionConfig(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    RAIL_Status_t status = RAIL_STATUS_NO_ERROR;

    // Only update protection configuration if an argument is given,
    // otherwise print the current configuration
    if (sl_cli_get_argument_count(args) > 0) {
      femConfig.txDutyCycle = sl_cli_get_argument_uint32(args, 0);

      if (sl_cli_get_argument_count(args) == 2) {
        femConfig.PMaxContinuousTx = sl_cli_get_argument_uint32(args, 1);
      }
      // Check and possibly apply the configuration
      (void) RAIL_SetFemProtectionConfig(railHandle, &femConfig);
    }

    // Get FEM parameters after update
    status = RAIL_GetFemProtectionConfig(railHandle, &femConfig);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrint(sl_cli_get_command_string(args, 0),
                    "TxDutyCycle:%u,PMaxContinuousTx:%d",
                    femConfig.txDutyCycle, femConfig.PMaxContinuousTx);
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 31,
                         "Incorrect configuration");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetEffControl(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool reset = false;
    uint16_t tempBuffer[RAIL_EFF_CONTROL_SIZE / sizeof(uint16_t)];

    _Static_assert(COUNTOF(tempBuffer) == 26,
                   "CLPC Control size changed, must update getSetEffControl");

    if (sl_cli_get_argument_count(args) > 0) {
      reset = (bool) sl_cli_get_argument_uint8(args, 0);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetEffControl(railHandle, tempBuffer, reset);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintContinue("EFF_CLPC_STATUS:%d,"
                            "RX_BYPASS_SENSE_VDDB:%d,"
                            "RX_BYPASS_SENSE_TEMPB:%d,"
                            "PA_SAWA:%d,"
                            "PA_SAWB:%d,"
                            "PA_SAWC:%d,"
                            "PA_SAWD:%d,"
                            "PA_SAWE:%d,"
                            "PA_SAWF:%d,"
                            "PA_SAWG:%d,"
                            "PA_SAWH:%d",
                            ((uint32_t)tempBuffer[1] << 16) | tempBuffer[0],
                            tempBuffer[2],
                            tempBuffer[3],
                            tempBuffer[4],
                            tempBuffer[5],
                            tempBuffer[6],
                            tempBuffer[7],
                            tempBuffer[8],
                            tempBuffer[9],
                            tempBuffer[10],
                            tempBuffer[11]
                            );
      responsePrintEnd("PA_SENSE_ANT_VOLTAGEA:%d,"
                       "PA_SENSE_ANT_CURRENTA:%d,"
                       "PA_SENSE_ANT_VOLTAGEB:%d,"
                       "PA_SENSE_ANT_CURRENTB:%d,"
                       "PA_SENSE_VDD_SENSE:%d,"
                       "RX_BYPASS_SENSE_TEMPA:%d,"
                       "TIMESTAMP0:%d,"
                       "TIMESTAMP1:%d,"
                       "TIMESTAMP2:%d,"
                       "TIMESTAMP3:%d",
                       tempBuffer[12],
                       tempBuffer[13],
                       tempBuffer[14],
                       tempBuffer[15],
                       tempBuffer[16],
                       tempBuffer[17],
                       ((uint32_t)tempBuffer[19] << 16) | tempBuffer[18],
                       ((uint32_t)tempBuffer[21] << 16) | tempBuffer[20],
                       ((uint32_t)tempBuffer[23] << 16) | tempBuffer[22],
                       ((uint32_t)tempBuffer[25] << 16) | tempBuffer[24]
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get EFF Control values.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetEffMode(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool changeMode = false;
    uint8_t newMode;

    if (sl_cli_get_argument_count(args) > 0) {
      changeMode = true;
      newMode = sl_cli_get_argument_uint8(args, 0);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetEffMode(railHandle, &newMode, changeMode);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintEnd("EFF Mode:%d",
                       newMode
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get EFF mode.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetEffRuralUrban(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool changeTrip = false;
    uint16_t newTrip;

    if (sl_cli_get_argument_count(args) > 0) {
      changeTrip = true;
      newTrip = sl_cli_get_argument_uint16(args, 0);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetEffRuralUrbanMv(railHandle, &newTrip, changeTrip);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintEnd("EFF rural to urban trip (mV):%d",
                       newTrip
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get Rural to Urban trip.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetEffUrbanBypass(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool changeTrip = false;
    uint16_t newTrip;

    if (sl_cli_get_argument_count(args) > 0) {
      changeTrip = true;
      newTrip = sl_cli_get_argument_uint16(args, 0);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetEffUrbanBypassMv(railHandle, &newTrip, changeTrip);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintEnd("EFF urban to bypass trip (mV):%d",
                       newTrip
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get Urban to Bypass trip.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetEffUrbanDwellTime(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool changeDwellTime = false;
    uint32_t newDwellTime;

    if (sl_cli_get_argument_count(args) > 0) {
      changeDwellTime = true;
      newDwellTime = sl_cli_get_argument_uint32(args, 0);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetEffUrbanDwellTimeMs(railHandle, &newDwellTime, changeDwellTime);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintEnd("EFF urban dwell time (ms):%d",
                       newDwellTime
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get Urban dwell time.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetEffBypassDwellTime(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool changeDwellTime = false;
    uint32_t newDwellTime;

    if (sl_cli_get_argument_count(args) > 0) {
      changeDwellTime = true;
      newDwellTime = sl_cli_get_argument_uint32(args, 0);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetEffBypassDwellTimeMs(railHandle, &newDwellTime, changeDwellTime);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintEnd("EFF bypass dwell time (ms):%d",
                       newDwellTime
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get Bypass dwell time.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetClpcSlowLoop(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool changeValues = false;
    uint16_t newTarget;
    uint16_t newSlope;

    if (sl_cli_get_argument_count(args) > 1) {
      changeValues = true;
      newTarget = sl_cli_get_argument_uint16(args, 0);
      newSlope = sl_cli_get_argument_uint16(args, 1);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetClpcSlowLoop(railHandle, &newTarget, &newSlope, changeValues);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintEnd("EFF CLPC slow loop target (mW):%d,"
                       "EFF CLPC slow loop slope (GAINDIG/mW):%d",
                       newTarget,
                       newSlope
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get CLPC slow loop values.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetClpcFastLoop(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool changeValues = false;
    uint16_t newTarget;
    uint16_t newSlope;

    if (sl_cli_get_argument_count(args) > 1) {
      changeValues = true;
      newTarget = sl_cli_get_argument_uint16(args, 0);
      newSlope = sl_cli_get_argument_uint16(args, 1);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetClpcFastLoop(railHandle, &newTarget, &newSlope, changeValues);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintEnd("EFF CLPC fast loop target (mV):%d,"
                       "EFF CLPC fast loop slope (GAINDIG/mV):%d",
                       newTarget,
                       newSlope
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get CLPC fast loop values.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetClpcEnable(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool changeClpcEnable = false;
    uint8_t newClpcEnable;

    if (sl_cli_get_argument_count(args) > 0) {
      changeClpcEnable = true;
      newClpcEnable = sl_cli_get_argument_uint8(args, 0);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetClpcEnable(railHandle, &newClpcEnable, changeClpcEnable);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintEnd("EFF CLPC Enable:%d",
                       newClpcEnable
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get CLPC Enable.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetEffTempThreshold(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool changeThreshold = false;
    uint16_t newThreshold;

    if (sl_cli_get_argument_count(args) > 0) {
      changeThreshold = true;
      newThreshold = sl_cli_get_argument_uint16(args, 0);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetEffTempThreshold(railHandle, &newThreshold, changeThreshold);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintEnd("EFF temperature threshold (K):%d",
                       newThreshold
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get EFF temperature threshold.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getSetInternalTempThreshold(sl_cli_command_arg_t *args)
{
#ifdef SL_RAIL_UTIL_EFF_DEVICE
  if (SL_RAIL_UTIL_EFF_DEVICE != RAIL_EFF_DEVICE_NONE) {
    bool changeThreshold = false;
    uint16_t newThreshold;

    if (sl_cli_get_argument_count(args) > 0) {
      changeThreshold = true;
      newThreshold = sl_cli_get_argument_uint16(args, 0);
    }

    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    RAIL_Status_t status = RAIL_GetSetInternalTempThreshold(railHandle, &newThreshold, changeThreshold);

    if (status == RAIL_STATUS_NO_ERROR) {
      responsePrintStart(sl_cli_get_command_string(args, 0));
      responsePrintEnd("Internal temperature threshold (K):%d",
                       newThreshold
                       );
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Could not get internal temperature threshold.");
    }
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif // SL_RAIL_UTIL_EFF_DEVICE
}

void getLogLevels(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0), "Peripherals:%s,Notifications:%s",
                ((logLevel & PERIPHERAL_ENABLE) ? "Enabled" : "Disabled"),
                ((logLevel & ASYNC_RESPONSE) ? "Enabled" : "Disabled"));
}

void setPeripheralEnable(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  logLevel = enable ? (logLevel | PERIPHERAL_ENABLE)
             : (logLevel & ~(PERIPHERAL_ENABLE));
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getLogLevels(args);

  // Actually turn on/off the peripherals as requested to save power
  if (enable) {
    PeripheralEnable();
  } else {
    PeripheralDisable();
  }
}

void setNotifications(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  logLevel = enable ? (logLevel | ASYNC_RESPONSE)
             : (logLevel & ~(ASYNC_RESPONSE));
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getLogLevels(args);
}

void resetChip(sl_cli_command_arg_t *args)
{
  (void)args;
  // Wait for any serial traffic to be done before resetting so we don't
  // output garbage characters
  serialWaitForTxIdle();

  // Use the NVIC to reset the chip
  NVIC_SystemReset();
}

void printDataRates(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  responsePrint(sl_cli_get_command_string(args, 0),
                "Symbolrate:%d,Bitrate:%d",
                RAIL_GetSymbolRate(railHandle),
                RAIL_GetBitRate(railHandle));
}

void getRandom(sl_cli_command_arg_t *args)
{
  uint16_t length = sl_cli_get_argument_uint16(args, 0);
  bool hidden = false;
  uint16_t result, offset = 0;
  int i;

  // Read out the hidden option if specified
  if (sl_cli_get_argument_count(args) >= 2) {
    hidden = !!sl_cli_get_argument_uint8(args, 1);
  }

  if (length > MAX_DEBUG_BYTES) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x10,
                       "Cannot collect more than %d random bytes.",
                       MAX_DEBUG_BYTES);
    return;
  }

  // Collect the random data
  result = RAIL_GetRadioEntropy(railHandle, debugDataBuffer, length);
  if (result != length) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x11, "Error collecting random data.");
  }

  if (!hidden) {
    for (i = 0; i < length; i++) {
      int n = snprintf(debugPrintBuffer + offset,
                       sizeof(debugPrintBuffer) - offset,
                       "%.2x",
                       debugDataBuffer[i]);
      if (n < 0) {
        responsePrintError(sl_cli_get_command_string(args, 0), 0x12, "Error printing random data.");
        return;
      }
      offset += n;

      // Make sure we don't try to print too much data
      if (offset >= sizeof(debugPrintBuffer)) {
        break;
      }
    }

    responsePrint(sl_cli_get_command_string(args, 0), "Length:%u,Data:%s", result, debugPrintBuffer);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "Length:%u,DataPtr:0x%.8x",
                  result,
                  &debugDataBuffer);
  }
}

// configs 1, 2, 3, and 5 behave the same, but 4 has some extra channels
#if defined(_SILICON_LABS_32B_SERIES_2)

#define FIRST_PAB_CHANNEL 0
#define FIRST_PCD_CHANNEL 6
#define LAST_PCD_CHANNEL 11

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
  #define LAST_PAB_CHANNEL 15
#else
  #define LAST_PAB_CHANNEL 5
#endif

char pinForPRSChannel[PRS_ASYNC_CHAN_COUNT][5];

static void printDebugSignalHelp(char *cmdName,
                                 const debugSignal_t *signals,
                                 uint32_t numSignals)
{
  uint32_t i;
  bool isFirstInstance = true;

  RAILTEST_PRINTF("%s [pin] [signal] [options]\n", cmdName);
  #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
  RAILTEST_PRINTF("\nPins: Any pin from PA, PB, PC, and PD available for debug, but only 6 total from PA and PB, and 6 total from PC and PD\n");
  #else
  RAILTEST_PRINTF("\nPins: Any pin from PA, PB, PC, and PD available for debug, but only 10 total from PA and PB, and 6 total from PC and PD\n");
  #endif
  RAILTEST_PRINTF("\nPins in Use: \n");
  for (i = 0; i < PRS_ASYNC_CHAN_COUNT; i++) {
    if (pinForPRSChannel[i][0] != '\0') {
      if (!isFirstInstance) {
        RAILTEST_PRINTF(", ");
      }

      isFirstInstance = false;

      RAILTEST_PRINTF("%s", pinForPRSChannel[i]);
    }
  }

  // Print information about the supported debug signals
  RAILTEST_PRINTF("\n\nSignals: \n");
  RAILTEST_PRINTF("  OFF\n  CUSTOM_PRS <source> <signal>\n  CUSTOM_LIB <event>\n");
  for (i = 0; i < numSignals; i++) {
    RAILTEST_PRINTF("  %s\n", signals[i].name);
  }
}

static RAIL_Status_t getPinAndChannelFromInput(debugPin_t *pin, char *pinArg, bool disablePin, const debugSignal_t *signal)
{
  // instead of referring to a table
  uint32_t i;
  uint32_t pinNumber;

  pin->name = pinArg;

  // Determine port from pinArg
  pin->gpioPort = (pinArg[1] - ((pinArg[1] >= 'a') ? 'a' : 'A'));
  // Skip the first two chars from the string and convert the remainder into an integer
  pinNumber = strtoul(&pinArg[2], NULL, 10);

  if (((pinArg[0] != 'P') && (pinArg[0] != 'p'))
      || (strlen(pinArg) >= 5)
      || (!GPIO_PORT_VALID(pin->gpioPort))
      || (!GPIO_PORT_PIN_VALID(pin->gpioPort, pinNumber))) {
    responsePrintError(pin->name, 0x50, "Not a valid pin name");
    return RAIL_STATUS_INVALID_PARAMETER;
  }

  pin->gpioPin = (uint8_t)pinNumber;

  // If disabling the pin, search for the PRS channel that is being used by that pin so that it can be disabled
  if (disablePin) {
    // Search for what PRS channel the pin uses it and remove the reference
    for (i = 0; i < PRS_ASYNC_CHAN_COUNT; i++) {
      if (strcasecmp(pinForPRSChannel[i], pin->name) == 0) {
        pin->prsChannel = i;
        pinForPRSChannel[i][0] = '\0';
        break;
      }
    }

    if (i == PRS_ASYNC_CHAN_COUNT) {
      responsePrint(pin->name, "\nPin not in use");
      return RAIL_STATUS_INVALID_PARAMETER;
    }
  } else if (signal->isPrs) {
    // If the signal is a PRS signal, search for an available PRS channel
    // On series 2 there are many PRS channels that can go to each GPIO, so choose the first valid available one
    // XG24 parts have 4 extra channels available for ports A and B, so do some extra logic to include them in the search
    if (pin->gpioPort < 2U) { // gpioPortA or gpioPortB
      for (i = FIRST_PAB_CHANNEL; i <= LAST_PAB_CHANNEL; i++) {
        #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
        if (((i < FIRST_PCD_CHANNEL) || (i > LAST_PCD_CHANNEL)) && (pinForPRSChannel[i][0] == '\0'))
        #else
        if (pinForPRSChannel[i][0] == '\0')
        #endif
        {
          pin->prsChannel = i;
          strcpy(pinForPRSChannel[i], pin->name);
          break;
        }
      }
      if (i > LAST_PAB_CHANNEL) {
        responsePrintError(pin->name, 0x50, "No more PRS channels available for PA/PB");
        return RAIL_STATUS_INVALID_STATE;
      }
    }
    // ports C and D
    else {
      for (i = FIRST_PCD_CHANNEL; i <= LAST_PCD_CHANNEL; i++) {
        if (pinForPRSChannel[i][0] == '\0') {
          pin->prsChannel = i;
          strcpy(pinForPRSChannel[i], pin->name);
          break;
        }
      }
      if (i > LAST_PCD_CHANNEL) {
        responsePrintError(pin->name, 0x50, "No more PRS channels available for PC/PD");
        return RAIL_STATUS_INVALID_PARAMETER;
      }
    }
  }

  // Location doesn't matter on series 2, but is still used by halEnablePrs, so set to 0
  pin->prsLocation = 0;

  return RAIL_STATUS_NO_ERROR;
}

#else // !_SILICON_LABS_32B_SERIES_2
static void printDebugSignalHelp(char *cmdName,
                                 const debugPin_t *pins,
                                 const debugSignal_t *signals,
                                 uint32_t numPins,
                                 uint32_t numSignals)
{
  uint32_t i;

  RAILTEST_PRINTF("%s [pin] [signal] [options]\n", cmdName);
  RAILTEST_PRINTF("Pins: ");
  for (i = 0; i < numPins; i++) {
    if (i != 0) {
      RAILTEST_PRINTF(", ");
    }
    RAILTEST_PRINTF("%s", pins[i].name);
  }

  // Print information about the supported debug signals
  RAILTEST_PRINTF("\nSignals: \n");
  RAILTEST_PRINTF("  OFF\n  CUSTOM_PRS <source> <signal>\n  CUSTOM_LIB <event>\n");
  for (i = 0; i < numSignals; i++) {
    RAILTEST_PRINTF("  %s\n", signals[i].name);
  }
}

static RAIL_Status_t getPinAndChannelFromInput(debugPin_t *debugPin, char *pinArg)
{
  uint32_t numPins;
  uint32_t i;

  const debugPin_t *pin = NULL, *pinList;

  // Get the debug pins for this chip
  pinList = halGetDebugPins(&numPins);

  // Make sure the pin they're trying to use is valid for this chip
  for (i = 0; i < numPins; i++) {
    if (strcasecmp(pinList[i].name, pinArg) == 0) {
      pin = &pinList[i];
    }
  }
  if (pin == NULL) {
    responsePrintError(pinArg, 0x50, "Not a valid pin name");
    //debugPin->name = NULL;
    return RAIL_STATUS_INVALID_PARAMETER;
  }

  debugPin->name = pin->name;
  debugPin->prsChannel = pin->prsChannel;
  debugPin->prsLocation = pin->prsLocation;
  debugPin->gpioPort = pin->gpioPort;
  debugPin->gpioPin = pin->gpioPin;
  return RAIL_STATUS_NO_ERROR;
}

#endif // _SILICON_LABS_32B_SERIES_2

void setDebugSignal(sl_cli_command_arg_t * args)
{
  const debugSignal_t *signal = NULL, *signalList;
  debugSignal_t customSignal;
  uint32_t i;
  uint32_t numSignals;
  bool disablePin = false;
  char *pinArg = sl_cli_get_argument_string(args, 0);
  debugPin_t defaultPin = {
    "name",
    0,
    0,
    gpioPortA,
    0
  };
  debugPin_t *pin = &defaultPin;
  RAIL_Status_t status;

  // Get the debug signals
  signalList = halGetDebugSignals(&numSignals);

  // Provide information about the pins and signals supported by this chip if
  // the help command is given. @TODO: It would be nice if this ignored the next
  // parameter somehow...
  if (strncasecmp(pinArg, "help", 4) == 0) {
    #if defined(_SILICON_LABS_32B_SERIES_2)
    printDebugSignalHelp(pinArg, signalList, numSignals);
    #else
    uint32_t numPins;
    const debugPin_t *pinList;
    pinList = halGetDebugPins(&numPins);
    printDebugSignalHelp(pinArg, pinList, signalList, numPins, numSignals);
    #endif
    return;
  }

  // Make sure the signal they're trying to use is valid for this chip
  if (strcasecmp("CUSTOM_LIB", sl_cli_get_argument_string(args, 1)) == 0) {
    // Make sure the correct parameters were given for this command
    if (!(sl_cli_get_argument_count(args) == 3)) {
      responsePrintError(sl_cli_get_command_string(args, 0),
                         0x51,
                         "Invalid parameters passed to CUSTOM_LIB");
      return;
    }
    customSignal.name = "CUSTOM_LIB";
    customSignal.isPrs = false;
    customSignal.loc.debugEventNum = sl_cli_get_argument_uint16(args, 2);
    signal = &customSignal;
  } else if (strcasecmp("CUSTOM_PRS", sl_cli_get_argument_string(args, 1)) == 0) {
    // Make sure that the right arguments were given for this command
    if ((!(sl_cli_get_argument_count(args) == 4))
        || (sl_cli_get_argument_uint16(args, 2) > 0xFF)
        || (sl_cli_get_argument_uint16(args, 3) > 0xFF)) {
      responsePrintError(sl_cli_get_command_string(args, 0),
                         0x53,
                         "Invalid parameters passed to CUSTOM_PRS");
      return;
    }
    customSignal.name = "CUSTOM_PRS";
    customSignal.isPrs = true;
    customSignal.loc.prs.source = (uint8_t)sl_cli_get_argument_uint16(args, 2);
    customSignal.loc.prs.signal = (uint8_t)sl_cli_get_argument_uint16(args, 3);
    signal = &customSignal;
  } else if (strcasecmp("OFF", sl_cli_get_argument_string(args, 1)) == 0) {
    disablePin = true;
  } else {
    // Search through the list of known signals for the requested one
    for (i = 0; i < numSignals; i++) {
      if (strcasecmp(signalList[i].name, sl_cli_get_argument_string(args, 1)) == 0) {
        signal = &signalList[i];
      }
    }
    if (signal == NULL) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x54, "%s is not a valid signal name", sl_cli_get_argument_string(args, 1));
      return;
    }
  }

  // Determine the pin, port, and prsChannel from the input arguments
  #if defined(_SILICON_LABS_32B_SERIES_2)
  status = getPinAndChannelFromInput(pin, pinArg, disablePin, signal);
  #else
  status = getPinAndChannelFromInput(pin, pinArg);
  #endif

  // return if invalid pin, port, or prs channel
  if (status != RAIL_STATUS_NO_ERROR) {
    return;
  }

  // Turn on the GPIO clock since we're going to need that
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Disable the GPIO while configuring it
  GPIO_PinModeSet(pin->gpioPort, pin->gpioPin, gpioModeDisabled, 0);

  // If this is a disable command then just turn everything off for this debug
  // pin to stop it from outputting.
  if (disablePin) {
    // Turn off the PRS output on this pin's channel
    halDisablePrs(pin->prsChannel);
    // @TODO: Turn off the RAIL debug event for this pin
    responsePrint(sl_cli_get_command_string(args, 0), "Pin:%s,Signal:OFF", pin->name);

    return;
  }

  if (signal == NULL) {
    return;
  }
  // Configure the PRS or library signal as needed
  if (signal->isPrs) {
    // Enable this PRS signal
    halEnablePrs(pin->prsChannel,
                 pin->prsLocation,
                 pin->gpioPort,
                 pin->gpioPin,
                 signal->loc.prs.source,
                 signal->loc.prs.signal);
  } else {
    // Turn on the RAIL debug event for this signal
  }
  responsePrint(sl_cli_get_command_string(args, 0), "Pin:%s,Signal:%s,Channel:%d", pin->name, signal->name, pin->prsChannel);
}

void forceAssert(sl_cli_command_arg_t * args)
{
  uint32_t errorCode = sl_cli_get_argument_uint32(args, 0);

  responsePrint(sl_cli_get_command_string(args, 0), "code:%d", errorCode);
  RAILCb_AssertFailed(railHandle, errorCode);
}

void configPrintEvents(sl_cli_command_arg_t * args)
{
  if (sl_cli_get_argument_count(args) == 0) {
    responsePrintHeader(sl_cli_get_command_string(args, 0), "name:%s,shift:%u,mask:%x%08x,printEnabled:%s");
    for (uint32_t i = 0; i < numRailEvents; i++) {
      uint64_t mask = 1ULL << i;
      responsePrintMulti("name:RAIL_EVENT_%s,shift:%u,mask:0x%x%08x,printEnabled:%s",
                         eventNames[i],
                         i,
                         (uint32_t)(mask >> 32),
                         (uint32_t)(mask),
                         (mask & enablePrintEvents) ? "True" : "False");
    }
    return;
  }

  RAIL_Events_t printEvents = sl_cli_get_argument_uint32(args, 0);
  RAIL_Events_t printEventsMask = RAIL_EVENTS_ALL;

  if (sl_cli_get_argument_count(args) >= 2) {
    printEvents |= (((RAIL_Events_t)sl_cli_get_argument_uint32(args, 1)) << 32);
  }
  // Read out the optional mask bits
  if (sl_cli_get_argument_count(args) == 3) {
    printEventsMask = 0xFFFFFFFF00000000ULL | sl_cli_get_argument_uint32(args, 2);
  } else if (sl_cli_get_argument_count(args) >= 4) {
    printEventsMask = ((RAIL_Events_t)sl_cli_get_argument_uint32(args, 3)) << 32;
    printEventsMask |= (RAIL_Events_t)sl_cli_get_argument_uint32(args, 2);
  }
  // Modify only the requested events
  enablePrintEvents = (enablePrintEvents & ~printEventsMask)
                      | (printEvents & printEventsMask);

  responsePrint(sl_cli_get_command_string(args, 0), "enablePrintEvents:0x%x%08x",
                (uint32_t)(enablePrintEvents >> 32),
                (uint32_t)(enablePrintEvents));
}

void printTxAcks(sl_cli_command_arg_t * args)
{
  printTxAck = !!sl_cli_get_argument_uint8(args, 0);

  responsePrint(sl_cli_get_command_string(args, 0), "printTxAcks:%s",
                printTxAck ? "True" : "False");
}

void printRxErrors(sl_cli_command_arg_t * args)
{
  printRxErrorPackets = !!sl_cli_get_argument_uint8(args, 0);

  responsePrint(sl_cli_get_command_string(args, 0), "printRxErrors:%s",
                printRxErrorPackets ? "True" : "False");
}

void printRxFreqOffsets(sl_cli_command_arg_t * args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  printRxFreqOffsetData = !!sl_cli_get_argument_uint8(args, 0);

  if (printRxFreqOffsetData) {
    // Enable the first sync word and second sync word events
    RAIL_ConfigEvents(railHandle,
                      (RAIL_EVENT_RX_SYNC1_DETECT | RAIL_EVENT_RX_SYNC2_DETECT),
                      (RAIL_EVENT_RX_SYNC1_DETECT | RAIL_EVENT_RX_SYNC2_DETECT));
  } else {
    rxFreqOffset = RAIL_FREQUENCY_OFFSET_INVALID;
  }

  responsePrint(sl_cli_get_command_string(args, 0), "printRxFreqOffsets:%s",
                printRxFreqOffsetData ? "True" : "False");
}

void setPrintingEnable(sl_cli_command_arg_t * args)
{
  printingEnabled = !!sl_cli_get_argument_uint8(args, 0);
  responsePrintEnable(printingEnabled);
  responsePrint(sl_cli_get_command_string(args, 0), "printingEnabled:%s",
                printingEnabled ? "True" : "False");
}

void getAppMode(sl_cli_command_arg_t * args)
{
  responsePrint(sl_cli_get_command_string(args, 0), "appMode:%s", appModeNames(currentAppMode()));
}

void getRadioState(sl_cli_command_arg_t * args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  responsePrint(sl_cli_get_command_string(args, 0), "radioState:%s,radioStateDetail:%s",
                getRfStateName(RAIL_GetRadioState(railHandle)),
                getRfStateDetailName(RAIL_GetRadioStateDetail(railHandle), debugPrintBuffer));
}

static bool verifyFirstTime = true;
static bool verifyUseOverride = false;
static bool verifyUseCallback = false;
static uint32_t verifyCbCounter = 0;         // Number of times the callback is called.

static bool RAILCb_VerificationApproval(uint32_t address,
                                        uint32_t expectedValue,
                                        uint32_t actualValue)
{
  // true = change approved (Do this to see a list of all registers that are
  //   different for the current radio state.)
  // false = change unapproved (This is the default behavior when no approval
  //   callback is provided.)
  bool approveDifference = true;
  // Print out all addresses that contain differences.
  responsePrint("verifyRadioCb",
                "encodedAddress:0x%08x,"
                "expectedValue:0x%08x,"
                "actualValue:0x%08x,"
                "differenceApproved:%s",
                address,
                expectedValue,
                actualValue,
                (approveDifference ? "true" : "false"));
  verifyCbCounter++;
  return approveDifference;
}

void verifyRadio(sl_cli_command_arg_t * args)
{
  char *answer;
  uint32_t durationUs = sl_cli_get_argument_uint32(args, 0);
  bool restart = !!sl_cli_get_argument_uint8(args, 1);
  bool useOverride = !!sl_cli_get_argument_uint8(args, 2);
  bool useCallback = !!sl_cli_get_argument_uint8(args, 3);
  uint32_t *radioConfig;
  RAIL_VerifyCallbackPtr_t cb;
  uint32_t timeBefore;
  uint32_t timeAfter;
  RAIL_Status_t retVal;

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));

  // Only run RAIL_ConfigVerification when we have to.
  if (verifyFirstTime
      || (useOverride != verifyUseOverride)
      || (useCallback != verifyUseCallback)) {
    verifyFirstTime = false;
    verifyUseOverride = useOverride;
    verifyUseCallback = useCallback;

    if (useOverride) {
#if SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
      // Provide a custom radio config.
      radioConfig = (uint32_t *)(channelConfigs[configIndex]->phyConfigBase);
#else // !SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
      // Restore variable to default value so this error always occurs.
      verifyFirstTime = true;
      responsePrintError(sl_cli_get_command_string(args, 0), 0x22, "External radio config support not enabled");
      return;
#endif // SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
    } else {
      radioConfig = NULL;
    }
    if (useCallback) {
      // Provide an approval callback to the application.
      cb = RAILCb_VerificationApproval;
    } else {
      cb = NULL;
    }

    RAIL_ConfigVerification(railHandle, &configVerify, radioConfig, cb);
  }

  // Clear the callback counter when restarting verification.
  if (restart) {
    verifyCbCounter = 0;
  }

  timeBefore = RAIL_GetTime();
  retVal = RAIL_Verify(&configVerify, durationUs, restart);
  timeAfter = RAIL_GetTime();
  switch (retVal) {
    case RAIL_STATUS_NO_ERROR:
    {
      answer = "success, done";
      break;
    }
    case RAIL_STATUS_SUSPENDED:
    {
      answer = "success, operation suspended";
      break;
    }
    case RAIL_STATUS_INVALID_PARAMETER:
    {
      answer = "invalid input parameter";
      break;
    }
    case RAIL_STATUS_INVALID_STATE:
    default:
    {
      answer = "data corruption";
    }
  }
  if (useCallback) {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "verification:%s,testDurationUs:%d,callbackCounter:%d",
                  answer,
                  timeAfter - timeBefore,
                  verifyCbCounter);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "verification:%s,testDurationUs:%d",
                  answer,
                  timeAfter - timeBefore);
  }
  // Clear the callback counter on success after we output the value.
  if (RAIL_STATUS_NO_ERROR == retVal) {
    verifyCbCounter = 0;
  }
}

void setVerifyConfig(sl_cli_command_arg_t * args)
{
  verifyConfigEnabled = !!sl_cli_get_argument_uint8(args, 0);
  responsePrint(sl_cli_get_command_string(args, 0),
                "verify config enabled:%d,"
                "Status:Success",
                verifyConfigEnabled);
}

void getVerifyConfig(sl_cli_command_arg_t * args)
{
  responsePrint(sl_cli_get_command_string(args, 0),
                "verify config enabled:%d",
                verifyConfigEnabled);
}
