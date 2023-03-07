/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "cslib_hwconfig.h"
#include "cslib.h"
#include "comm_routines.h"
#include "cslib_sensor_descriptors.h"
#include <inttypes.h>
#include <stdio.h>

#if defined(__DEBUG_TIMER_H__)
uint32_t serial_output_debug_time[DEF_NUM_SENSORS];
#define HEADER_TYPE_COUNT 10
#else
#define HEADER_TYPE_COUNT 9
#endif

/// @brief Defines each data type and count for that type
const HeaderStruct_t headerEntries[HEADER_TYPE_COUNT] =
{
  { "BASELINE", DEF_NUM_SENSORS },
  { "RAW", DEF_NUM_SENSORS },
  { "SINGACT", DEF_NUM_SENSORS },
  { "DEBACT", DEF_NUM_SENSORS },
  { "TDELTA", DEF_NUM_SENSORS },
  { "EXPVAL", DEF_NUM_SENSORS },
  { "NOISEEST", 1 },
  { "C_ACTTHR", DEF_NUM_SENSORS },
  { "C_INACTTHR", DEF_NUM_SENSORS },
//  { "TRST", 1},
#if defined(__DEBUG_TIMER_H__)
  { "DEBUGTIMER", DEF_NUM_SENSORS },
#endif
};

/// @brief One-shot flag triggering output of header line
uint8_t sendHeader = 1;

/// @brief Only send comm updates after LDMA has completed a transfer
uint8_t sendComms = true;

/// @brief  Generates and outputs a header describing the data in the stream
void printHeader(void);

/***************************************************************************//**
 * @brief
 *   Outputs | between columns when needed between data types in header
 *
 *   Exposed top-level debug output function.  Outputs one line of data output
 *   for each call.  Output controls are compile-time switches found in
 *   profiler_interface.h.  Function prints all of defined variable types in
 *   sequence before printing the next defined variable type.  For example,
 *   a configuration to print raw and baseline data for a system with two
 *   channels would print:
 *    <RAW_0> <RAW_1> <BASELINE_0> <BASELINE_1>
 *
 ******************************************************************************/
void CSLIB_commUpdate(void)
{
  uint16_t index;

  // If first execution, sendHeader will be set.  Clear and output header
  if (sendHeader == 1) {
    printHeader();
    sendHeader = 0;
  }

  if (!sendComms) {
    return;
  } else {
    sendComms = false;
  }

  // Output baselines
  for (index = 0; index < DEF_NUM_SENSORS; index++) {
    printf("%" PRIu32 " ", CSLIB_node[index].currentBaseline);
  }

  // Output raw data
  for (index = 0; index < DEF_NUM_SENSORS; index++) {
    printf("%" PRIu32 " ", CSLIB_node[index].rawBuffer[0]);
  }

  // Output single/candidate active state for sensors
  for (index = 0; index < DEF_NUM_SENSORS; index++) {
    if (CSLIB_isSensorSingleActive(index) == 1) {
      printf("1 ");
    } else {
      printf("0 ");
    }
  }

  // Output debounce/qualified touch state for each sensor
  for (index = 0; index < DEF_NUM_SENSORS; index++) {
    if (CSLIB_isSensorDebounceActive(index) == 1) {
      printf("1 ");
    } else {
      printf("0 ");
    }
  }

  // Output touch delta for each sensor
  for (index = 0; index < DEF_NUM_SENSORS; index++) {
    printf("%d ", CSLIB_getUnpackedTouchDelta(index));
  }

  // Output filtered data for each sensor
  for (index = 0; index < DEF_NUM_SENSORS; index++) {
    printf("%" PRIu32 " ", CSLIB_node[index].expValue.u32 >> 16);
  }

  // Output global noise estimation
  printf("%u ", CSLIB_systemNoiseAverage);

  // Output active threshold percentages
  for (index = 0; index < DEF_NUM_SENSORS; index++) {
    printf("%u ", CSLIB_activeThreshold[index]);
  }

  // Output inactive threshold percentages
  for (index = 0; index < DEF_NUM_SENSORS; index++) {
    printf("%u ", CSLIB_inactiveThreshold[index]);
  }

//  printf("%u ", indexTRST);

#if defined(__DEBUG_TIMER_H__)
  // Output debug timer info (not yet implemented)
#error "Debug timer not implemented!"
#else
#endif

  outputNewLine();

  // Wait until all bytes have been transmitted
  BlockWhileTX();
}

/***************************************************************************//**
 * @brief
 *   Outputs the header line used by Capacitive Sensing Profiler
 *
 ******************************************************************************/
void printHeader(void)
{
  uint8_t index;

  outputNewLine();

  // Outputs user-defined sensor names from sensor_descriptors.c
  outputsensorDescriptors();

  // Outputs protocol command showing that line contains the header
  outputBeginHeader();

  // For each defined data type, transmit header information and then
  // a break character
  for (index = 0; index < HEADER_TYPE_COUNT; index++) {
    outputHeaderCount(headerEntries[index]);
    outputBreak();
  }
  outputNewLine();
}
