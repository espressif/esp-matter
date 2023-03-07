/**************************************************************************/ /**
 * @file
 * @brief OS abstraction primitives for the SE Manager for bare metal apps
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SE_MANAGER_OSAL_BAREMETAL_H
#define SE_MANAGER_OSAL_BAREMETAL_H

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Defines

/// In order to wait forever in blocking functions the user can pass the
/// following value.
#define SE_MANAGER_OSAL_WAIT_FOREVER  (-1)
/// In order to return immediately in blocking functions the user can pass the
/// following value.
#define SE_MANAGER_OSAL_NON_BLOCKING  (0)

/// Priority to use for SEMBRX IRQ
#if defined(SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY)
  #define SE_MANAGER_SEMBRX_IRQ_PRIORITY SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY
#else
  #define SE_MANAGER_SEMBRX_IRQ_PRIORITY (0)
#endif

// -----------------------------------------------------------------------------
// Typedefs

/// Completion type used to wait for and signal end of operation.
typedef volatile unsigned int se_manager_osal_completion_t;

/// SE manager mutex definition for Baremetal.
typedef volatile unsigned int se_manager_osal_mutex_t;

// -----------------------------------------------------------------------------
// Globals

#if defined(SE_MANAGER_OSAL_TEST)
/// Global variable to keep track of ticks in bare metal test apps.
extern unsigned int sli_se_manager_test_ticks;
#endif

// -----------------------------------------------------------------------------
// Functions

/// Initialize a completion object.
__STATIC_INLINE
sl_status_t se_manager_osal_init_completion(se_manager_osal_completion_t *p_comp)
{
  *p_comp = 0;
  return SL_STATUS_OK;
}

/// Free a completion object.
__STATIC_INLINE
sl_status_t se_manager_osal_free_completion(se_manager_osal_completion_t *p_comp)
{
  *p_comp = 0;
  return SL_STATUS_OK;
}

/// Wait for completion event.
__STATIC_INLINE sl_status_t
se_manager_osal_wait_completion(se_manager_osal_completion_t *p_comp, int ticks)
{
  int ret;
  if (ticks == SE_MANAGER_OSAL_WAIT_FOREVER) {
    while ( *p_comp == 0 ) {
#if defined(SE_MANAGER_OSAL_TEST)
      sli_se_manager_test_ticks++;
#endif
    }
    *p_comp = 0;
    ret = 0;
  } else {
    while ((*p_comp == 0) && (ticks > 0)) {
      ticks--;
#if defined(SE_MANAGER_OSAL_TEST)
      sli_se_manager_test_ticks++;
#endif
    }
    if (*p_comp == 1) {
      *p_comp = 0;
      ret = 0;
    } else {
      ret = SL_STATUS_TIMEOUT;
    }
  }

  return ret;
}

/// Signal completion event.
__STATIC_INLINE
sl_status_t se_manager_osal_complete(se_manager_osal_completion_t* p_comp)
{
  *p_comp = 1;
  return SL_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif // SE_MANAGER_OSAL_BAREMETAL_H
