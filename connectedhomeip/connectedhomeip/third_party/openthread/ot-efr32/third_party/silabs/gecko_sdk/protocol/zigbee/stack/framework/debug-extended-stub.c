/***************************************************************************//**
 * @file
 * @brief This file contains stubs for full debug functionality. It
 * returns only failure.
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

#include PLATFORM_HEADER
#include "include/error.h"

//------------------------------------------------------------------------------
// Forward declarations.

#ifndef DEBUG_STRIPPED  // TODO: EMZIGBEE-6992 for cleanup

bool emberDebugReportOff(void)
{
  return false;
}

void emberDebugReportRestore(bool state)
{
  (void)state;
}

void emberDebugPrintf(const char * formatString, ...)
{
  (void)formatString;
}

void emberDebugError(EmberStatus code)
{
  (void)code;
}

void emDebugClearStats(void)
{
}

void emDebugPrintStats(void)
{
}

void emDebugStats(uint8_t index)
{
  (void)index;
}

void emDebugApiTrace(uint16_t debugType, const char * formatString, ...)
{
  (void)debugType;
  (void)formatString;
}

bool emDebugShouldIgnoreTraceForType(uint16_t debugType)
{
  (void)debugType;
  return false;
}

bool emDebugProcessIncomingFullDebugMessages(uint16_t debugType, uint8_t *data, uint8_t length)
{
  (void)debugType;
  (void)data;
  (void)length;
  return false;
}

#endif
