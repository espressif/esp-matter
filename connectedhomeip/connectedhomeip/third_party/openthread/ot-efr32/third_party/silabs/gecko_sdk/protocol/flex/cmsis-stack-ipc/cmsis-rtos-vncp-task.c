/***************************************************************************//**
 * @brief CMSIS RTOS vncp code.
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
// Define module name for Power Manager debuging feature.
#define CURRENT_MODULE_NAME    "FLEX_RTOS_VNCP_TASK"

#include PLATFORM_HEADER
#include "cmsis-rtos-ipc-config.h"
#include "sl_component_catalog.h"

#include "stack/include/api-rename.h"
#include "stack/include/ember.h"
#include "stack/include/api-rename-undef.h"

#include "hal.h"

#include "app_framework_common.h"

#include "cmsis-rtos-support.h"

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  #include "sl_power_manager.h"
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

// Some large value still manageable by the OS and the BSP tick code (in case
// sleep is enabled).
#define MAX_VNCP_YIELD_TIME_MS (1000000)

//------------------------------------------------------------------------------
// Forward declarations and external declarations

static void connectStackTaskYield(void);

extern osEventFlagsId_t emAfPluginCmsisRtosFlags;

//------------------------------------------------------------------------------
// Internal APIs

// The stack task main loop.
void emAfPluginCmsisRtosStackTask(void *p_arg)
{
  (void)p_arg;

  connect_stack_init();

  while (true) {
    connect_stack_tick();

    // Process IPC commands from application tasks.
    emAfPluginCmsisRtosProcessIncomingApiCommand();
    // Yield the Connect stack task if possible.
    connectStackTaskYield();
  }
}

// This can be called from ISR.
void emAfPluginCmsisRtosWakeUpConnectStackTask(void)
{
  assert((osEventFlagsSet(emAfPluginCmsisRtosFlags,
                          FLAG_STACK_ACTION_PENDING) & CMSIS_RTOS_ERROR_MASK) == 0);
}

//------------------------------------------------------------------------------
// Static functions

static void connectStackTaskYield(void)
{
  uint16_t currentStackTasks;
  uint32_t idleTimeMs = emApiStackIdleTimeMs(&currentStackTasks);

  if (idleTimeMs > 0) {
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    bool stackTaskDeepSleepAllowed =
      ((currentStackTasks & EMBER_HIGH_PRIORITY_TASKS) == 0);
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

    if (idleTimeMs > MAX_VNCP_YIELD_TIME_MS) {
      idleTimeMs = MAX_VNCP_YIELD_TIME_MS;
    }

    uint32_t yieldTimeTicks = (osKernelGetTickFreq() * idleTimeMs) / 1000;

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    if (!stackTaskDeepSleepAllowed) {
      sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
    }
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

    // Pend on a stack action.
    osEventFlagsWait(emAfPluginCmsisRtosFlags,
                     FLAG_STACK_ACTION_PENDING,
                     osFlagsWaitAny,
                     yieldTimeTicks);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    if (!stackTaskDeepSleepAllowed) {
      sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
    }
#endif // SL_CATALOG_POWER_MANAGER_PRESENT
  }
}
