/***************************************************************************//**
 * @file
 * @brief IO Stream SWO Component.
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

#include "sl_iostream.h"
#include "sl_iostream_swo.h"
#include "sl_status.h"
#include "sl_debug_swo.h"
#include "sl_iostream_swo_config.h"

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

/*******************************************************************************
 *************************   LOCAL DATA TYPES   ********************************
 ******************************************************************************/

#if defined(SL_CATALOG_KERNEL_PRESENT)
typedef struct {
  osMutexId_t lock;
  __ALIGNED(4) uint8_t lock_cb[osMutexCbSize];
} swo_stream_context_t;
#endif

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static sl_status_t swo_write(void  *context,
                             const void *buffer,
                             size_t buffer_length);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
static void sli_iosteam_swo_em_transition(sl_power_manager_em_t from,
                                          sl_power_manager_em_t to);
#endif
/*******************************************************************************
 ******************************   VARIABLES   **********************************
 ******************************************************************************/

// Public stream variable for SWO io component
static sl_iostream_t sl_iostream_swo = {
  .write   = swo_write,
  .read    = NULL,
  .context = NULL
};

sl_iostream_t *sl_iostream_swo_handle = &sl_iostream_swo;

sl_iostream_instance_info_t sl_iostream_instance_swo_info = {
  .handle = &sl_iostream_swo,
  .name = "swo",
  .type = SL_IOSTREAM_TYPE_SWO,
  .periph_id = 0,
  .init = sl_iostream_swo_init,
};

#if defined(SL_CATALOG_KERNEL_PRESENT)
// Internal context variable for SWO io component
swo_stream_context_t swo_context;
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
static sl_power_manager_em_transition_event_handle_t event_handle;
static sl_power_manager_em_transition_event_info_t event_info = {
  .event_mask = SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0,
  .on_event = sli_iosteam_swo_em_transition,
};
#endif

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize SWO stream component.
 *
 * @return  Status Code:
 *            - SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_iostream_swo_init(void)
{
  // Configure SWO module
  sl_status_t ret = sl_debug_swo_enable_itm(0U);
  if (ret != SL_STATUS_OK) {
    return ret;
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  // Create OS Mutex for lock
  osMutexAttr_t m_attr;
  m_attr.attr_bits = 0u;
  m_attr.cb_mem = swo_context.lock_cb;
  m_attr.cb_size = osMutexCbSize;
  m_attr.name = "SWO Component Lock";

  swo_context.lock = osMutexNew(&m_attr);
  EFM_ASSERT(swo_context.lock != NULL);
  // Save context reference in SWO stream object
  sl_iostream_swo.context = &swo_context;
#endif

  // Set default IO Stream to SWO
  sl_iostream_set_system_default(&sl_iostream_swo);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_subscribe_em_transition_event(&event_handle, &event_info);
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * De-initialize SWO stream component.
 *
 * @return  Status Code:
 *            - SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_iostream_swo_deinit(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  // Acquire Mutex
  if (osMutexAcquire(swo_context.lock, osWaitForever) != osOK) {
    return SL_STATUS_FAIL;
  }

  // Delete Mutex
  EFM_ASSERT(osMutexDelete(swo_context.lock) == osOK);
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_unsubscribe_em_transition_event(&event_handle);
#endif

  // De-configure ITM
  // We cannot fully deconfigure the debug/trace system, since
  // it may be in use by others.
  return sl_debug_swo_disable_itm(0U);
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * SWO stream component write function.
 ******************************************************************************/
static sl_status_t swo_write(void *context,
                             const void *buffer,
                             size_t buffer_length)
{
  sl_status_t ret = SL_STATUS_OK;
  uint8_t *buf_8 = (uint8_t *)buffer;
#if (SL_IOSTREAM_SWO_WRITE_BYTE_PER_BYTE == 0)
  uint32_t *buf_32 = (uint32_t*)(buffer);
  uint16_t *buf_16;
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
  swo_stream_context_t *swo_context = (swo_stream_context_t *)context;
  if (osKernelGetState() == osKernelRunning) {
    // Bypass lock if we print before the kernel is running
    if (osMutexAcquire(swo_context->lock, osWaitForever) != osOK) {
      return SL_STATUS_FAIL;
    }
  }
#else
  (void)context;
#endif

#if (SL_IOSTREAM_SWO_WRITE_BYTE_PER_BYTE == 1)
  // Write buffer
  for (size_t i = 0; i < buffer_length; i++) {
    ret = sl_debug_swo_write_u8(0, buf_8[i]);
    if (ret != SL_STATUS_OK) {
      goto early_return;
    }
  }
#else
  // Write the maximum number of words
  while (buffer_length >= sizeof(uint32_t)) {
    ret = sl_debug_swo_write_u32(0, *buf_32);
    if (ret != SL_STATUS_OK) {
      goto early_return;
    }
    buf_32++;
    buffer_length -= sizeof(uint32_t);
  }

  // Write a last half-word if remaining
  buf_16 = (uint16_t*)buf_32;
  if (buffer_length >= sizeof(uint16_t)) {
    ret = sl_debug_swo_write_u16(0, *buf_16);
    if (ret != SL_STATUS_OK) {
      goto early_return;
    }
    buf_16++;
    buffer_length -= sizeof(uint16_t);
  }

  // Write the last byte if remaining
  buf_8 = (uint8_t*)buf_16;
  if (buffer_length == sizeof(uint8_t)) {
    ret = sl_debug_swo_write_u8(0, *buf_8);
  }
#endif

  early_return:
#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (osKernelGetState() == osKernelRunning) {
    // Bypass lock if we print before the kernel is running
    EFM_ASSERT(osMutexRelease(swo_context->lock) == osOK);
  }
#endif

  return ret;
}

/***************************************************************************//**
 * wait for the ITM busy bit to clear before going to sleep
 ******************************************************************************/
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
static void sli_iosteam_swo_em_transition(sl_power_manager_em_t from,
                                          sl_power_manager_em_t to)
{
  (void)to;
  EFM_ASSERT(from == SL_POWER_MANAGER_EM0);
  while (ITM->TCR & ITM_TCR_BUSY_Msk) ;
}
#endif
