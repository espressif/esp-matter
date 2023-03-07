/***************************************************************************//**
 * @file
 * @brief IO Stream RTT Component.
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

#include "sl_iostream_rtt.h"
#include "SEGGER_RTT.h"
#include "sl_status.h"

#if !defined(IOSTREAM_RTT_UP_MODE)
#define IOSTREAM_RTT_UP_MODE   SEGGER_RTT_MODE_NO_BLOCK_TRIM
#endif

#if !defined(IOSTREAM_RTT_DOWN_MODE)
#define IOSTREAM_RTT_DOWN_MODE   SEGGER_RTT_MODE_NO_BLOCK_TRIM
#endif

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static sl_status_t rtt_write(void *context,
                             const void *buffer,
                             size_t buffer_length);

static sl_status_t rtt_read(void *context,
                            void *buffer,
                            size_t buffer_length,
                            size_t *bytes_read);

/*******************************************************************************
 ******************************   VARIABLES   **********************************
 ******************************************************************************/

static sl_iostream_t sl_iostream_rtt = {
  .read = rtt_read,
  .write = rtt_write,
  .context = NULL
};

sl_iostream_t *sl_iostream_rtt_handle = &sl_iostream_rtt;

sl_iostream_instance_info_t sl_iostream_instance_rtt_info = {
  .handle = &sl_iostream_rtt,
  .name = "rtt",
  .type = SL_IOSTREAM_TYPE_RTT,
  .periph_id = 0,
  .init = sl_iostream_rtt_init,
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * RTT Stream init.
 ******************************************************************************/
sl_status_t sl_iostream_rtt_init(void)
{
  SEGGER_RTT_Init();
  SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, IOSTREAM_RTT_UP_MODE);
  SEGGER_RTT_ConfigDownBuffer(0, NULL, NULL, 0, IOSTREAM_RTT_DOWN_MODE);
  sl_iostream_set_system_default(&sl_iostream_rtt);

  return SL_STATUS_OK;
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * Internal RTT stream write implementation
 ******************************************************************************/
static sl_status_t rtt_write(void *context,
                             const void *buffer,
                             size_t buffer_length)
{
  uint32_t ret = 0;
  sl_status_t status;
  (void)context;
  (void)ret;

  ret = SEGGER_RTT_Write(0, buffer, buffer_length);

#if ((IOSTREAM_RTT_UP_MODE == SEGGER_RTT_MODE_NO_BLOCK_TRIM) \
  || (IOSTREAM_RTT_UP_MODE == SEGGER_RTT_MODE_NO_BLOCK_SKIP))
  status = SL_STATUS_OK; // Ignore error
#else
  if (ret > 0) {
    status = SL_STATUS_OK;
  } else {
    status = SL_STATUS_IO;
  }
#endif

  return status;
}

/***************************************************************************//**
 * Internal RTT stream read implementation
 ******************************************************************************/
static sl_status_t rtt_read(void *context,
                            void *buffer,
                            size_t buffer_length,
                            size_t *bytes_read)
{
  sl_status_t status;
  (void)context;

  *bytes_read = SEGGER_RTT_Read(0, buffer, buffer_length);

  if (*bytes_read > 0) {
    status = SL_STATUS_OK;
  } else {
    status = SL_STATUS_EMPTY;
  }

  return status;
}
