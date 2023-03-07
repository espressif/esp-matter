/***************************************************************************//**
 * @file
 * @brief IO Stream VUART Component.
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
// Define module name for Power Manager debug feature
#define CURRENT_MODULE_NAME    "IOSTREAM_VUART"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

#include "sli_iostream_swo_itm_8.h"
#include "sl_iostream_vuart_config.h"
#include "sl_iostream_vuart.h"
#include "sl_iostream.h"
#include "sl_iostream_swo.h"
#include "sl_status.h"
#include "sl_assert.h"
#include "em_gpio.h"
#include "em_cmu.h"

#include "SEGGER_RTT.h"

/*******************************************************************************
 *************************   LOCAL DATA TYPES   ********************************
 ******************************************************************************/

#if defined(SL_CATALOG_KERNEL_PRESENT)
typedef struct {
  osMutexId_t lock;
  __ALIGNED(4) uint8_t lock_cb[osMutexCbSize];
} vuart_context_t;
#endif

/*******************************************************************************
 ******************************   VARIABLES   **********************************
 ******************************************************************************/

// Public stream variable for VUART io component
static sl_iostream_t sl_iostream_vuart;

sl_iostream_t *sl_iostream_vuart_handle = &sl_iostream_vuart;

sl_iostream_instance_info_t sl_iostream_instance_vuart_info = {
  .handle = &sl_iostream_vuart,
  .name = "vuart",
  .type = SL_IOSTREAM_TYPE_VUART,
  .periph_id = 0,
  .init = sl_iostream_vuart_init,
};

// RTT down channel number
static int rtt_buffer_index = -1;

// RTT down channel ring buffer
static char rtt_buffer[SL_IOSTREAM_VUART_RX_BUFFER_SIZE];

// Packet TX sequence number
static uint8_t seq_number = 0;

#if defined(SL_CATALOG_KERNEL_PRESENT)
// Internal context variable for SWO io component
vuart_context_t vuart_context;
#endif

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static sl_status_t vuart_write(void *context,
                               const void *buffer,
                               size_t buffer_length);

static sl_status_t vuart_read(void *context,
                              void *buffer,
                              size_t buffer_length,
                              size_t *bytes_read);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize vuart stream component.
 ******************************************************************************/
sl_status_t sl_iostream_vuart_init(void)
{
  void *context;
  // Configure SWO stimulus 8
  context = sli_iostream_swo_itm_8_init();
  sl_iostream_vuart.context = context;

  // Configure RTT
  SEGGER_RTT_Init();
  // The "silabsvuartdown" RTT channel name is used by the debugger to find the
  // location of the RTT buffer in RAM, and can therefore not be changed without
  // updating the board controller firmware.
  rtt_buffer_index = SEGGER_RTT_AllocDownBuffer("silabsvuartdown",
                                                rtt_buffer,
                                                SL_IOSTREAM_VUART_RX_BUFFER_SIZE,
                                                SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  EFM_ASSERT(rtt_buffer_index > 0);
  sl_iostream_vuart.write = vuart_write;
  sl_iostream_vuart.read = vuart_read;

  // Set default IO Stream to VUART
  sl_iostream_set_system_default(&sl_iostream_vuart);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  // RTT requires EM1
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Internal stream write implementation
 ******************************************************************************/
static sl_status_t vuart_write(void *context,
                               const void *buffer,
                               size_t buffer_length)
{
  return sli_iostream_swo_itm_8_write(context, buffer, buffer_length, EM_DEBUG_VIRTUAL_UART_TX, &seq_number);
}

/***************************************************************************//**
 * Internal stream read implementation
 ******************************************************************************/
static sl_status_t vuart_read(void *context,
                              void *buffer,
                              size_t buffer_length,
                              size_t *bytes_read)
{
  (void) context;

  *bytes_read = SEGGER_RTT_Read(rtt_buffer_index, buffer, buffer_length);
  if (*bytes_read == 0) {
    return SL_STATUS_EMPTY;
  }

  return SL_STATUS_OK;
}
