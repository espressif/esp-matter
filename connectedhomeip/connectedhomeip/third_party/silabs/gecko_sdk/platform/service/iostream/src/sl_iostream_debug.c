/***************************************************************************//**
 * @file
 * @brief IO Stream debug Component.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

#include "sli_iostream_swo_itm_8.h"
#include "sl_iostream.h"
#include "sl_iostream_debug.h"
#include "sl_status.h"
#include "sl_atomic.h"
#include "sl_assert.h"
#include "em_gpio.h"
#include "em_cmu.h"

/*******************************************************************************
 *************************   LOCAL DATA TYPES   ********************************
 ******************************************************************************/

#if defined(SL_CATALOG_KERNEL_PRESENT)
typedef struct {
  osMutexId_t lock;
  __ALIGNED(4) uint8_t lock_cb[osMutexCbSize];
} debug_context_t;
#endif

/*******************************************************************************
 ******************************   VARIABLES   **********************************
 ******************************************************************************/

// Public stream variable for debug io component

static sl_iostream_swo_itm_8_msg_type_t debug_type = EM_DEBUG_PRINTF;
static uint8_t seq_number = 0;

static sl_iostream_t sl_iostream_debug;

sl_iostream_t *sl_iostream_debug_handle = &sl_iostream_debug;

sl_iostream_instance_info_t sl_iostream_instance_debug_info = {
  .handle = &sl_iostream_debug,
  .name = "debug",
  .type = SL_IOSTREAM_TYPE_DEBUG_OUTPUT,
  .periph_id = 0,
  .init = sl_iostream_debug_init,
};

#if defined(SL_CATALOG_KERNEL_PRESENT)
// Internal context variable for SWO io component
debug_context_t debug_context;
#endif

static sl_status_t debug_write(void *context,
                               const void *buffer,
                               size_t buffer_length);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize debug stream component.
 ******************************************************************************/
sl_status_t sl_iostream_debug_init(void)
{
  void *context;
  // Configure SWO stimulus 8
  context = sli_iostream_swo_itm_8_init();
#if defined(SL_CATALOG_KERNEL_PRESENT)
  sl_iostream_debug.context = context;
#else
  (void)context;
#endif
  sl_iostream_debug.write = debug_write;
  sl_iostream_debug.read = NULL;

  // Set default IO Stream to debug
  sl_iostream_set_system_default(&sl_iostream_debug);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Set debug stream type
 ******************************************************************************/
sl_status_t sl_iostream_set_debug_type(sl_iostream_swo_itm_8_msg_type_t type)
{
  sl_atomic_store(debug_type, type);
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Internal stream write implementation
 ******************************************************************************/
static sl_status_t debug_write(void *context,
                               const void *buffer,
                               size_t buffer_length)
{
  sl_iostream_swo_itm_8_msg_type_t type;
  sl_atomic_load(type, debug_type);
  return sli_iostream_swo_itm_8_write(context, buffer, buffer_length, type, &seq_number);
}
