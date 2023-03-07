/***************************************************************************//**
 * @file
 * @brief IO Stream SWO ITM Functions
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
#include <stdbool.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

#include "sli_iostream_swo_itm_8.h"
#include "sl_iostream.h"
#include "sl_enum.h"
#include "sl_status.h"
#include "sl_assert.h"
#include "sl_debug_swo.h"

#include "sl_atomic.h"

#define LOW_BYTE(n)                     ((uint8_t)((n) & 0xFF))
#define HIGH_BYTE(n)                    ((uint8_t)(LOW_BYTE((n) >> 8)))

/*******************************************************************************
 *************************   LOCAL DATA TYPES   ********************************
 ******************************************************************************/

#if defined(SL_CATALOG_KERNEL_PRESENT)
typedef struct {
  osMutexId_t lock;
  __ALIGNED(4) uint8_t lock_cb[osMutexCbSize];
} swo_itm_8_context_t;
#endif

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
static void sli_iosteam_swo_itm_em_transition(sl_power_manager_em_t from,
                                              sl_power_manager_em_t to);
#endif

/*******************************************************************************
 ******************************   VARIABLES   **********************************
 ******************************************************************************/

static uint32_t swo_itm_initialized = 0;

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
static sl_power_manager_em_transition_event_handle_t event_handle;
static sl_power_manager_em_transition_event_info_t event_info = {
  .event_mask = SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0,
  .on_event = sli_iosteam_swo_itm_em_transition,
};
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
static swo_itm_8_context_t context;
#endif

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize swo stimulus 8
 ******************************************************************************/
void *sli_iostream_swo_itm_8_init(void)
{
  bool init;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  void *return_context = &context;
#else
  void *return_context = NULL;
#endif

  sl_atomic_load(init, swo_itm_initialized);
  if (init) {
    return return_context;
  }

  sl_atomic_store(swo_itm_initialized, true);

  // Configure SWO module
  sl_debug_swo_enable_itm(8U);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  // Create OS Mutex for lock
  osMutexAttr_t m_attr;
  m_attr.cb_mem = context.lock_cb;
  m_attr.cb_size = osMutexCbSize;
  m_attr.name = "vuart lock";
  m_attr.attr_bits = 0u;
  context.lock = osMutexNew(&m_attr);
  EFM_ASSERT(context.lock != NULL);
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_subscribe_em_transition_event(&event_handle, &event_info);
#endif

  return return_context;
}

/***************************************************************************//**
 * write data on vuart interface (WSTK-port 4900)
 ******************************************************************************/
sl_status_t sli_iostream_swo_itm_8_write(void *context,
                                         const void *buffer,
                                         size_t buffer_length,
                                         sl_iostream_swo_itm_8_msg_type_t type,
                                         uint8_t *seq_nbr)
{
  uint8_t *buf = (uint8_t *)buffer;
  uint32_t packet_length;
  uint32_t i;
  uint8_t  output_byte;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  swo_itm_8_context_t *vuart_context = (swo_itm_8_context_t *)context;
  if (osKernelGetState() == osKernelRunning) {
    // Bypass lock if we print before the kernel is running
    if (osMutexAcquire(vuart_context->lock, osWaitForever) != osOK) {
      return SL_STATUS_FAIL;
    }
  }
#else
  (void)context;
#endif

  // Full length is 2 square braces, 1 byte length and 2 byte crc
  packet_length = ( (uint32_t)buffer_length) + 9;

  for ( i = 0; i < packet_length; ++i ) {
    if ( i == 0 ) {
      // Frame start
      output_byte = '[';
    } else if ( i == 1 ) {
      // Including special byte, type and sequence number
      output_byte = buffer_length + 4;
    } else if ( i == 2 ) {
      // Special EDM byte
      output_byte = 0xD1;
    } else if ( i == 3 ) {
      // VUART TX Type byte 1
      output_byte = LOW_BYTE((uint16_t)type);
    } else if ( i == 4 ) {
      // VUART TX Type byte 2
      output_byte = HIGH_BYTE((uint16_t)type);
    } else if ( i == 5 ) {
      // Sequence number
      output_byte = *seq_nbr++;
    } else if ( i == packet_length - 3 ) {
      // CRC first byte
      // Ignored by FW - so we also skip it
      output_byte = 0x5A;
    } else if ( i == packet_length - 2 ) {
      // CRC second byte
      // Ignored by FW - so we also skip it
      output_byte = 0x5A;
    } else if ( i == packet_length - 1 ) {
      // Frame end
      output_byte = ']';
    } else {
      // Data
      output_byte = buf[i - 6];
    }

    sl_debug_swo_write(8U, output_byte);
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (osKernelGetState() == osKernelRunning) {
    // Bypass lock if we print before the kernel is running
    EFM_ASSERT(osMutexRelease(vuart_context->lock) == osOK);
  }
#endif
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * wait for the ITM busy bit to clear before going to sleep
 ******************************************************************************/
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
static void sli_iosteam_swo_itm_em_transition(sl_power_manager_em_t from,
                                              sl_power_manager_em_t to)
{
  (void)to;
  EFM_ASSERT(from == SL_POWER_MANAGER_EM0);
  while (ITM->TCR & ITM_TCR_BUSY_Msk) ;
}
#endif
