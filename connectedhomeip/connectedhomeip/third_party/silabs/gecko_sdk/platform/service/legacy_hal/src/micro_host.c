/***************************************************************************//**
 * @file
 * @brief PC host files for the HAL.
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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "include/error.h"
#include "micro-common.h"

#if defined(EZSP_HOST) && !defined(EZSP_ASH) && !defined(EZSP_USB) && !defined(EZSP_CPC)
  #include "spi-protocol-common.h"
  #define initSpiNcp() halNcpSerialInit()
#else
  #define initSpiNcp()
#endif

static void (*microRebootHandler)(void) = NULL;

void halInit(void)
{
  initSpiNcp();
}

void halResetWatchdog(void)
{
  //stub
}

void halInternalAssertFailed(const char * filename, int linenumber)
{
  printf("\nFailed assert on line %d of %s, rebooting.\n", linenumber, filename);
  halReboot();
}

void otaServerSetClientDelayUnits(void)
{
  //stub
}

void setMicroRebootHandler(void (*handler)(void))
{
  microRebootHandler = handler;
}

void halReboot(void)
{
  if (microRebootHandler) {
    microRebootHandler();
  }
  printf("\nReboot not supported.  Exiting instead.\n");
  exit(0);
}

uint8_t halGetResetInfo(void)
{
  return RESET_SOFTWARE;
}

PGM_P halGetResetString(void)
{
  static PGM_P resetString = "SOFTWARE";
  return (resetString);
}

// Ideally this should not be necessary, but the serial code references
// this for all platforms.
void simulatedSerialTimePasses(void)
{
}

void halPowerDown(void)
{
}

void halPowerUp(void)
{
}

// void halSleep(SleepModes sleepMode)
// {
// }

// Implement this to catch incorrect HAL calls for this platform.
void halCommonSetSystemTime(uint32_t time)
{
  (void)time;
  fprintf(stderr, "FATAL:  halCommonSetSystemTime() not supported!\n");
  assert(0);
}

void halInternalResetWatchDog(void)
{
}

void halCommonDelayMicroseconds(uint16_t usec)
{
  if (usec == 0) {
    return;
  }

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = usec;
  assert(select(0, NULL, NULL, NULL, &timeout) >= 0);
}

void halCommonDelayMilliseconds(uint16_t msec)
{
  uint16_t cnt = msec;

  if (msec == 0) {
    return;
  }

  while (cnt-- > 0)
    halCommonDelayMicroseconds(1000);
}

// stubs to enable full compilation.
CORE_irqState_t CORE_EnterCritical(void)
{
  return 0;
}
void CORE_ExitCritical(CORE_irqState_t irqState)
{
  (void) irqState;
}

CORE_irqState_t CORE_EnterAtomic(void)
{
  return (0u);
}

void CORE_ExitAtomic(CORE_irqState_t irqState)
{
  (void)irqState;
}
