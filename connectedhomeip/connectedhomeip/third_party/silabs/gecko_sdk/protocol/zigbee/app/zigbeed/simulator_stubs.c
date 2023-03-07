#include "stack/include/ember.h"
#include "hal.h"
#include <stdio.h>

//----------------------------------------------------------------
// From tool/simulator/child/child-main.c

// debugPrintTextAndHex, vSimPrint, simPrint, scriptTestCheckpoint and
// debugSimPrint are from child-main.c. However, they are referenced
// from many other zigbee stack source files. Once EMBER_TEST is removed,
// ember-stack.h will define an empty macro to allow compilation.
// sendLogEvent will go away after EMBER_TEST is removed
#ifdef EMBER_TEST

uint16_t simulatorId;
uint16_t rebootCount = 0;
bool quiet = true;

void debugPrintTextAndHex(const char* text,
                          const uint8_t* hexData,
                          uint8_t length,
                          uint8_t spaceEveryXChars,
                          bool finalCr)
{
}

void vSimPrint(char *format, va_list argPointer)
{
  if (quiet) {
    return;
  }
  fprintf(stderr, "[%f ", halCommonGetInt32uMillisecondTick() / 1000.0);
  if (rebootCount == 0) {
    fprintf(stderr, "%X: ", simulatorId);
  } else {
    fprintf(stderr, "%X.%d: ", simulatorId, rebootCount);
  }
  vfprintf(stderr, format, argPointer);
  putc(']', stderr);
  putc('\n', stderr);
}

bool usingScript = true;

void simPrint(char* format, ...)
{
  va_list argPointer;
  va_start(argPointer, format);
  vSimPrint(format, argPointer);
  va_end(argPointer);
}

void debugSimPrint(char* format, ...)
{
  if (usingScript) {
    va_list argPointer;
    va_start(argPointer, format);
    vSimPrint(format, argPointer);
    va_end(argPointer);
  }
}

void scriptTestCheckpoint(char* string, ...)
{
}

// Referenced from emLinkReportCosts in neighbor.c
void sendLogEvent(char *types, char *data)
{
}

#endif // EMBER_TEST

#ifdef EMBER_TEST

void simulatedTimePasses(void)
{
}

#endif

#include "sl_status.h"
#include "sl_iostream.h"

// Referenced from emSourceRouteUpdateEventHandler in source-route-update.c
EmberStatus emberSerialPrintfLine(uint8_t port, PGM_P formatString, ...)
{
  return 0;
}

void emberSetOrGetEzspTokenCommandHandler(bool isSet)
{
}

void emberAfCounterHandler(EmberCounterType type, EmberCounterInfo info)
{
}

// Referenced from stream_putchar in iostream_printf.o
//   from putchar in iostream_printf.o
sl_status_t sl_iostream_putchar(sl_iostream_t *stream,
                                char c)
{
  //EmberStatus status = emberSerialWriteByte(APP_SERIAL, c);
  EmberStatus status = EMBER_SUCCESS;
  if (status == EMBER_SUCCESS) {
    return SL_STATUS_OK;
  } else {
    return SL_STATUS_FAIL;
  }
}
