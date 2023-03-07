/***************************************************************************//**
 * @brief CMSIS RTOS support code.
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
#include "sl_component_catalog.h"

#include "cmsis-rtos-ipc-config.h"
#include "cmsis-rtos-support.h"

#include "stack/include/api-rename.h"
#include "stack/include/ember.h"

//------------------------------------------------------------------------------
// Tasks variables and defines

osThreadId_t connectStackId;
osThreadId_t appFrameworkId;

osMutexId_t bufferSystemMutex;

//------------------------------------------------------------------------------
// Forward and external declarations.

#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
extern void App_OS_SetAllHooks(void);
#endif

//------------------------------------------------------------------------------
// Public APIs.

void emberAfPluginCmsisRtosIpcInit(void)
{
#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
  App_OS_SetAllHooks();
#endif

  emAfPluginCmsisRtosInitTasks();
}

void emberAfPluginCmsisRtosAcquireBufferSystemMutex(void)
{
  assert(osMutexAcquire(bufferSystemMutex,
                        osWaitForever) == osOK);
}

void emberAfPluginCmsisRtosReleaseBufferSystemMutex(void)
{
  assert(osMutexRelease(bufferSystemMutex) == osOK);
}

//------------------------------------------------------------------------------
// Internal APIs

osThreadId_t emAfPluginCmsisRtosGetStackTcb(void)
{
  return connectStackId;
}

void emAfPluginCmsisRtosInitTasks(void)
{
  // Create Connect task.
  osThreadAttr_t connectStackattribute = {
    "Connect Stask",
    osThreadDetached,
    NULL,
    0,
    NULL,
    (EMBER_AF_PLUGIN_CMSIS_RTOS_CONNECT_STACK_SIZE * sizeof(void *)) & 0xFFFFFFF8u,
    EMBER_AF_PLUGIN_CMSIS_RTOS_CONNECT_STACK_PRIO,
    0,
    0
  };

  connectStackId = osThreadNew(emAfPluginCmsisRtosStackTask,
                               NULL,
                               &connectStackattribute);
  assert(connectStackId != 0);

  bufferSystemMutex = osMutexNew(NULL);
  assert(bufferSystemMutex != NULL);

  emAfPluginCmsisRtosIpcInit();

  // Create App Framework task.
  osThreadAttr_t appFrameWorkattribute = {
    "App Framework",
    osThreadDetached,
    NULL,
    0,
    NULL,
    (EMBER_AF_PLUGIN_CMSIS_RTOS_APP_FRAMEWORK_STACK_SIZE * sizeof(void *)) & 0xFFFFFFF8u,
    EMBER_AF_PLUGIN_CMSIS_RTOS_APP_FRAMEWORK_PRIO,
    0,
    0
  };

  appFrameworkId = osThreadNew(emAfPluginCmsisRtosAppFrameworkTask,
                               NULL,
                               &appFrameWorkattribute);
  assert(appFrameworkId != 0);
}

//------------------------------------------------------------------------------
// Implemented callbacks

void emberAfPluginCmsisRtosStackIsr(void)
{
  emAfPluginCmsisRtosWakeUpConnectStackTask();
}

void emAcquireBufferSystemMutexHandler(void)
{
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
}

void emReleaseBufferSystemMutexHandler(void)
{
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
}

void emAfPluginCmsisStackIsrHandler(void)
{
  emAfPluginCmsisRtosWakeUpConnectStackTask();
}

// This should not fire when running within an OS.
bool emAfPluginCmsisStackIdleHandler(uint32_t *idleTimeMs)
{
  (void)idleTimeMs;

  assert(0);
  return false;
}
