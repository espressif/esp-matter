/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone non-secure interface dispatch functionality.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sli_tz_ns_interface.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT) \
  || defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
  #include "cmsis_os2.h"
  #include "sl_assert.h"
  #define RTOS_KERNEL_PRESENT
#endif

//------------------------------------------------------------------------------
// RTOS Support

#if defined(RTOS_KERNEL_PRESENT)

typedef struct tfm_threading_mutex {
  osMutexAttr_t mutex_attr;
  osMutexId_t   mutex_ID;
} tfm_threading_mutex_t;

static tfm_threading_mutex_t mutex = { 0 };

#define SLI_TFM_KERNEL_CRITICAL_SECTION_START                              \
  int32_t kernel_lock_state = 0;                                           \
  osKernelState_t kernel_state = osKernelGetState();                       \
  if (kernel_state != osKernelInactive && kernel_state != osKernelReady) { \
    kernel_lock_state = osKernelLock();                                    \
    if (kernel_lock_state < 0) {                                           \
      return PSA_ERROR_BAD_STATE;                                          \
    }                                                                      \
  }

#define SLI_TFM_KERNEL_CRITICAL_SECTION_END                                \
  if (kernel_state != osKernelInactive && kernel_state != osKernelReady) { \
    if (osKernelRestoreLock(kernel_lock_state) < 0) {                      \
      return PSA_ERROR_BAD_STATE;                                          \
    }                                                                      \
  }

#endif // RTOS_KERNEL_PRESENT

//------------------------------------------------------------------------------
// Global Functions

// TODO: PSEC-3170 - not called yet.
uint32_t sli_tz_ns_interface_init(void)
{
  #if defined(RTOS_KERNEL_PRESENT)
  SLI_TFM_KERNEL_CRITICAL_SECTION_START

  mutex.mutex_ID = osMutexNew(&mutex.mutex_attr);
  if (mutex.mutex_ID == NULL) {
    SLI_TFM_KERNEL_CRITICAL_SECTION_END
    // TODO: PSEC-3170 - should maybe not use PSA error codes.
    return PSA_ERROR_BAD_STATE;
  }
  SLI_TFM_KERNEL_CRITICAL_SECTION_END
  #endif // RTOS_KERNEL_PRESENT

  // TODO: PSEC-3170 - should maybe not use PSA error codes.
  return PSA_SUCCESS;
}

int32_t sli_tz_ns_interface_dispatch(sli_tz_veneer_fn fn,
                                     uint32_t arg0,
                                     uint32_t arg1,
                                     uint32_t arg2,
                                     uint32_t arg3)
{
  #if defined(RTOS_KERNEL_PRESENT)
  if (osKernelGetState() == osKernelRunning) {
    osStatus_t status = osMutexAcquire(mutex.mutex_ID, osWaitForever);
    if (status != osOK) {
      // TODO: PSEC-3170 - should maybe not use PSA error codes.
      return PSA_ERROR_BAD_STATE;
    }
  }
  #endif // RTOS_KERNEL_PRESENT

  int32_t result = fn((psa_invec *)arg0, arg1, (psa_outvec *)arg2, arg3);

  #if defined(RTOS_KERNEL_PRESENT)
  if (osKernelGetState() == osKernelRunning) {
    osStatus_t status = osMutexRelease(mutex.mutex_ID);
    if (status != osOK) {
      // TODO: PSEC-3170 - should maybe not use PSA error codes.
      return PSA_ERROR_BAD_STATE;
    }
  }
  #endif // RTOS_KERNEL_PRESENT

  return result;
}

uint32_t sli_tz_ns_interface_dispatch_simple(sli_tz_simple_veneer_fn fn,
                                             uint32_t arg)
{
  #if defined(RTOS_KERNEL_PRESENT)
  if (osKernelGetState() == osKernelRunning) {
    EFM_ASSERT(osMutexAcquire(mutex.mutex_ID, osWaitForever) == osOK);
  }
  #endif // RTOS_KERNEL_PRESENT

  uint32_t result = fn(arg);

  #if defined(RTOS_KERNEL_PRESENT)
  if (osKernelGetState() == osKernelRunning) {
    EFM_ASSERT(osMutexRelease(mutex.mutex_ID) == osOK);
  }
  #endif // RTOS_KERNEL_PRESENT

  return result;
}
