/***************************************************************************//**
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER
#include "cmsis-rtos-ipc-config.h"

#include "stack/include/ember.h"
#include "cmsis-rtos-support.h"
#include "app_framework_common.h"

//------------------------------------------------------------------------------
// Forward and external declarations.

static void appFrameworkTaskYield(void);

extern EmberTaskId emAppTask;
extern const EmberEventData emAppEvents[];

extern osEventFlagsId_t emAfPluginCmsisRtosFlags;

//------------------------------------------------------------------------------
// Internal APIs.

void emAfPluginCmsisRtosAppFrameworkTask(void *p_arg)
{
  (void)p_arg;

  connect_app_framework_init();

  while (true) {
    connect_app_framework_tick();

    // Process incoming callback commands from the vNCP.
    if (!emAfPluginCmsisRtosProcessIncomingCallbackCommand()) {
      // Yield the Application Framework task if no callback message needs to be
      // processed.
      appFrameworkTaskYield();
    }
  }
}

void emAfPluginCmsisRtosWakeUpAppFrameworkTask(void)
{
  assert((osEventFlagsSet(emAfPluginCmsisRtosFlags,
                          FLAG_STACK_CALLBACK_PENDING) & CMSIS_RTOS_ERROR_MASK) == 0);
}

//------------------------------------------------------------------------------
// Static functions.

static void appFrameworkTaskYield(void)
{
  uint32_t idleTimeMs = emberMsToNextEvent(emAppEvents,
                                           EMBER_AF_PLUGIN_CMSIS_RTOS_APP_FRAMEWORK_YIELD_TIMEOUT_MS);

  if (idleTimeMs > 0) {
    uint32_t yieldTimeTicks = (osKernelGetTickFreq() * idleTimeMs) / 1000;

    osEventFlagsWait(emAfPluginCmsisRtosFlags,
                     FLAG_STACK_CALLBACK_PENDING,
                     osFlagsWaitAny,
                     yieldTimeTicks);
  }
}
