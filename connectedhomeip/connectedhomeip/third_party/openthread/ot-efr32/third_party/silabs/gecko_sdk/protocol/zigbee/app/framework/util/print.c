/***************************************************************************//**
 * @file
 * @brief Utilities and command line interface for printing, and enabling/disabling
 * printing to different areas.
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

//------------------------------------------------------------------------------
// Globals

// Enable this if you want area names printed. It proved annoying
// so we're disabling it here.
//#define EMBER_AF_PRINT_AREA_NAME

#ifdef EMBER_AF_PRINT_AREA_NAME
static void reallyPrintAreaName(uint16_t area);
  #define printAreaName(area) reallyPrintAreaName(area)
#else
  #define printAreaName(area)
#endif //EMBER_AF_PRINT_AREA_NAME

#ifdef EMBER_AF_PRINT_NAMES
static const char * areaNames[] = EMBER_AF_PRINT_NAMES;
#endif

#ifdef EMBER_AF_PRINT_BITS
static uint8_t enablementBytes[] = EMBER_AF_PRINT_BITS;
#endif

uint16_t emberAfPrintActiveArea = 0;

//------------------------------------------------------------------------------

#ifdef UC_BUILD
// In the first stage of Zigbee/UC there is no run-time enabling/disabling of
// debug prints
bool emberAfPrintEnabled(uint16_t area)
{
  return true;
}
#else

// Returns true if the area print is enabled
bool emberAfPrintEnabled(uint16_t area)
{
#ifndef EMBER_AF_PRINT_BITS
  return false;
#else
  uint8_t index = (uint8_t)(area >> 8);
  if ( area == 0xFFFF ) {
    return true;
  }
  if ( index >= sizeof(enablementBytes) ) {
    return false;
  } else {
    uint8_t byte = enablementBytes[index];
    return ( (byte & (uint8_t)(area & 0xFF) ) != 0x00);
  }
#endif // EMBER_AF_PRINT_BITS
}
#endif //UC_BUILD

static void printEnable(uint16_t area, bool on)
{
#ifdef EMBER_AF_PRINT_BITS
  uint8_t index = (uint8_t)(area >> 8);
  if ( index < sizeof(enablementBytes) ) {
    if ( on ) {
      enablementBytes[index] |= (uint8_t)(area & 0xFF);
    } else {
      enablementBytes[index] &= ~(uint8_t)(area & 0xFF);
    }
  }
#endif // EMBER_AF_PRINT_BITS
}

static uint16_t convertUserNumberAreaToInternalArea(uint16_t userNumberedArea)
{
  uint16_t index = userNumberedArea / 8;
  index = (uint16_t)(index << 8)
          + (uint16_t)( ((uint16_t)0x0001) << (userNumberedArea % 8) );
  return index;
}

#if defined EMBER_AF_PRINT_AREA_NAME
static void reallyPrintAreaName(uint16_t area)
{
#ifdef EMBER_AF_PRINT_NAMES
  uint8_t hi, lo, count;
  uint16_t index;

  hi = (uint8_t)(area >> 8);
  lo = (uint8_t)(area & 0xFF);
  count = 0;

  if ( lo != 0 ) {
    while ( !(lo & 0x01) ) {
      lo >>= 1;
      count++;
    }
  }
  index = ((8 * hi) + count);

  if (area != 0xFFFF
      && index < EMBER_AF_PRINT_NAME_NUMBER) {
    (void) emberSerialPrintf(EMBER_AF_PRINT_OUTPUT, "%p:", areaNames[index]);
  }
#endif // EMBER_AF_PRINT_NAMES
}
#endif //EMBER_AF_PRINT_AREA_NAME

// Prints the trace if trace is enabled
static void emAfPrintInternalVarArg(uint16_t area,
                                    bool newLine,
                                    const char * formatString,
                                    va_list ap)
{
  if ( !emberAfPrintEnabled(area) ) {
    return;
  }
  printAreaName(area);

  (void) emberSerialPrintfVarArg(EMBER_AF_PRINT_OUTPUT, formatString, ap);

  if (newLine) {
    (void) emberSerialPrintf(EMBER_AF_PRINT_OUTPUT, "\r\n");
  }
  emberAfPrintActiveArea = area;
}

void emberAfPrintln(uint16_t area, const char * formatString, ...)
{
  va_list ap = { 0 };
  va_start(ap, formatString);
  emAfPrintInternalVarArg(area, true, formatString, ap);
  va_end(ap);
}

void emberAfPrint(uint16_t area, const char * formatString, ...)
{
  va_list ap = { 0 };
  va_start(ap, formatString);
  emAfPrintInternalVarArg(area, false, formatString, ap);
  va_end(ap);
}

void emberAfPrintStatus(void)
{
#ifdef EMBER_AF_PRINT_NAMES
  uint8_t i;
  for (i = 0; i < EMBER_AF_PRINT_NAME_NUMBER; i++) {
    (void) emberSerialPrintfLine(EMBER_AF_PRINT_OUTPUT, "[%d] %p : %p",
                                 i,
                                 areaNames[i],
                                 (emberAfPrintEnabled(
                                    convertUserNumberAreaToInternalArea(i))
                                  ? "YES"
                                  : "no"));
    (void) emberSerialWaitSend(EMBER_AF_PRINT_OUTPUT);
  }
#endif // EMBER_AF_PRINT_NAMES
}

void emberAfPrintAllOn(void)
{
#ifdef EMBER_AF_PRINT_BITS
  MEMSET(enablementBytes, 0xFF, sizeof(enablementBytes));
#endif
}

void emberAfPrintAllOff(void)
{
#ifdef EMBER_AF_PRINT_BITS
  MEMSET(enablementBytes, 0x00, sizeof(enablementBytes));
#endif
}

// These are CLI functions where a user will supply a sequential numbered
// area; as opposed to the bitmask area number that we keep track
// of internally.

void emberAfPrintOn(uint16_t userArea)
{
  printEnable(convertUserNumberAreaToInternalArea(userArea),
              true);   // enable?
}

void emberAfPrintOff(uint16_t userArea)
{
  printEnable(convertUserNumberAreaToInternalArea(userArea),
              false);  // enable?
}
