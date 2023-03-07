/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <setjmp.h>
#include "stack/include/ember.h"
#include "em2xx-reset-defs.h"

extern jmp_buf gResetJump;

// platform/base/hal/micro/unix/simulation/micro.c
void halStackProcessBootCount(void)
{
  // Note:  We need to add the increment call in order to test the lighting
  // sample applications.

#if defined(CREATOR_STACK_BOOT_COUNTER)
#ifndef EMBER_SCRIPTED_TEST
  halCommonIncrementCounterToken(TOKEN_STACK_BOOT_COUNTER);
#endif
#endif
}

// Referenced from serialOkToBootload in ash-ncp.c
bool halInternalUartTxIsIdle(uint8_t port)
{
  (void)port;
  return true;
}

// Referenced from processEzspCommand in command-handlers.c
uint16_t halGetStandaloneBootloaderVersion(void)
{
  return 0xFFFF;
}

// Referenced from checkBootloaderLaunch in ncp-common.c
EmberStatus halLaunchStandaloneBootloader(uint8_t mode)
{
  return EMBER_ERR_FATAL;
}

// Removed platform/base/hal/micro/unix/host/micro.c so placing stubs here
void halInternalResetWatchDog(void)
{
}

uint8_t halGetResetInfo(void)
{
  return EM2XX_RESET_SOFTWARE;
  //return EM2XX_RESET_POWERON;
}

void halReboot(void)
{
  longjmp(gResetJump, 1);
  assert(false); //Should never reach here
}

// Referenced from emberStopScan in zigbee-scan.c
// Stub in platform/radio/rail_lib/chip/simulation/simulation_stub.c
void halStackSymbolDelayAIsr(void)
{
}

// Referenced from emNetworkCheckIncomingQueue in network.c
// Stub in platform/base/hal/micro/generic/led-stub.c
void halStackIndicateActivity(bool turnOn)
{
}

// Needed by zigbee-stack.c but not needed by legacy_host/src/token.c
// so it can be left empty. If there is a failure during token
// initialization, unix/host/token.c will raise an assert
EmberStatus halStackInitTokens()
{
  return EMBER_SUCCESS;
}

void halResetWatchdog(void)
{
  //stub
}

void halInternalAssertFailed(const char * filename, int linenumber)
{
  //stub
}
