/**************************************************************************//**
 * Copyright 2022, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "sl_wfx_bus.h"
#include "sl_wfx_host_api.h"
#include "firmware/sl_wfx_registers.h"
#include "sl_wfx_configuration_defaults.h"
#include <stddef.h>
#include <string.h>

#ifndef SL_WFX_SDIO_BLOCK_MODE_THRESHOLD
#define SL_WFX_SDIO_BLOCK_MODE_THRESHOLD 0x200
#endif

static uint32_t            rx_buffer_id;
static uint32_t            tx_buffer_id;

sl_status_t sl_wfx_reg_read(sl_wfx_register_address_t address, void *buffer, uint32_t length)
{
  sl_status_t result;
  uint32_t buffer_id = 0;
  uint32_t reg_address;
  uint16_t control_register;
  uint32_t config_register;
  uint32_t current_transfer_size = (length >= SL_WFX_SDIO_BLOCK_MODE_THRESHOLD) ? SL_WFX_ROUND_UP(length, SL_WFX_SDIO_BLOCK_SIZE) : length;

  if (address == SL_WFX_IN_OUT_QUEUE_REG_ID) {
    buffer_id = rx_buffer_id++;
    if (rx_buffer_id > 4) {
      rx_buffer_id = 1;
    }
  }

  reg_address = (buffer_id << 7) | (address << 2);

  result = sl_wfx_host_sdio_transfer_cmd53(SL_WFX_BUS_READ, 1, reg_address, buffer, current_transfer_size);
  SL_WFX_ERROR_CHECK(result);

  if ( address == SL_WFX_IN_OUT_QUEUE_REG_ID ) {
    /* In block mode, the piggy_back value is at the end of the block. Append it at the end of the frame instead. */
    if (length > SL_WFX_SDIO_BLOCK_MODE_THRESHOLD) {
      memcpy( (uint8_t *)((uint8_t *)buffer + length - 2), (uint8_t *)((uint8_t *)buffer + current_transfer_size - 2), 2);
    }
    /* If the piggy-back value is null, acknowledge the received frame with a dummy configuration register read */
    control_register = sl_wfx_unpack_16bit_little_endian(((uint8_t *)buffer) + length - 2);
    if ((control_register & SL_WFX_CONT_NEXT_LEN_MASK) == 0) {
      result = sl_wfx_reg_read_32(SL_WFX_CONFIG_REG_ID, &config_register);
      SL_WFX_ERROR_CHECK(result);
      if ((config_register & SL_WFX_CONFIG_ERROR_MASK) != 0) {
        result = SL_STATUS_FAIL;
      }
    }
  }

  error_handler:
  return result;
}

sl_status_t sl_wfx_reg_write(sl_wfx_register_address_t address, const void *buffer, uint32_t length)
{
  uint32_t buffer_id = 0;
  uint32_t reg_address;
  uint32_t current_transfer_size = (length >= SL_WFX_SDIO_BLOCK_MODE_THRESHOLD) ? SL_WFX_ROUND_UP(length, SL_WFX_SDIO_BLOCK_SIZE) : length;

  if (address == SL_WFX_IN_OUT_QUEUE_REG_ID) {
    buffer_id = tx_buffer_id++;
    if (tx_buffer_id > 31) {
      tx_buffer_id = 0;
    }
  }

  reg_address = (buffer_id << 7) | (address << 2);

  return sl_wfx_host_sdio_transfer_cmd53(SL_WFX_BUS_WRITE, 1, reg_address, (void *)buffer, current_transfer_size);
}

sl_status_t sl_wfx_init_bus(void)
{
  sl_status_t result;
  uint32_t    value32;
  uint8_t     value_u8;

  rx_buffer_id = 1;
  tx_buffer_id = 0;

  sl_wfx_host_reset_chip();

  result = sl_wfx_host_init_bus();
  SL_WFX_ERROR_CHECK(result);

  result = sl_wfx_host_sdio_transfer_cmd52(SL_WFX_BUS_READ, 0,
                                           SL_WFX_SDIO_CCCR_IO_QUEUE_ENABLE,
                                           &value_u8);
  SL_WFX_ERROR_CHECK(result);

  // Enables Function 1
  value_u8 |= (1 << 1);
  result = sl_wfx_host_sdio_transfer_cmd52(SL_WFX_BUS_WRITE, 0,
                                           SL_WFX_SDIO_CCCR_IO_QUEUE_ENABLE,
                                           &value_u8);
  SL_WFX_ERROR_CHECK(result);

  // Enables Master and Function 1 interrupts
  result = sl_wfx_host_sdio_transfer_cmd52(SL_WFX_BUS_READ, 0,
                                           SL_WFX_SDIO_CCCR_IRQ_ENABLE,
                                           &value_u8);
  SL_WFX_ERROR_CHECK(result);
  value_u8 |= 0x1 | (1 << 1);
  result = sl_wfx_host_sdio_transfer_cmd52(SL_WFX_BUS_WRITE, 0,
                                           SL_WFX_SDIO_CCCR_IRQ_ENABLE,
                                           &value_u8);
  SL_WFX_ERROR_CHECK(result);

  // Set bus width to 4-bit
  result = sl_wfx_host_sdio_transfer_cmd52(SL_WFX_BUS_READ, 0,
                                           SL_WFX_SDIO_CCCR_BUS_INTERFACE_CONTROL,
                                           &value_u8);
  SL_WFX_ERROR_CHECK(result);
  value_u8 = (value_u8 & 0xFC) | 0x2;
  result = sl_wfx_host_sdio_transfer_cmd52(SL_WFX_BUS_WRITE, 0,
                                           SL_WFX_SDIO_CCCR_BUS_INTERFACE_CONTROL,
                                           &value_u8);
  SL_WFX_ERROR_CHECK(result);

  // Switch to HS mode
  result = sl_wfx_host_sdio_transfer_cmd52(SL_WFX_BUS_READ, 0,
                                           SL_WFX_SDIO_CCCR_HIGH_SPEED_ENABLE,
                                           &value_u8);
  SL_WFX_ERROR_CHECK(result);
  // Set Enable_High_Speed to 1
  value_u8 |= 0x2;
  result = sl_wfx_host_sdio_transfer_cmd52(SL_WFX_BUS_WRITE, 0,
                                           SL_WFX_SDIO_CCCR_HIGH_SPEED_ENABLE,
                                           &value_u8);
  SL_WFX_ERROR_CHECK(result);

  // Enabled SDIO high speed mode
  sl_wfx_host_sdio_enable_high_speed_mode();

  // Set function 1 block size
  value_u8 = SL_WFX_SDIO_BLOCK_SIZE & 0xff;
  result = sl_wfx_host_sdio_transfer_cmd52(SL_WFX_BUS_WRITE, 0,
                                           SL_WFX_SDIO_FBR1_BLOCK_SIZE_LSB,
                                           &value_u8);
  SL_WFX_ERROR_CHECK(result);

  value_u8 = (SL_WFX_SDIO_BLOCK_SIZE >> 8) & 0xff;
  result = sl_wfx_host_sdio_transfer_cmd52(SL_WFX_BUS_WRITE, 0,
                                           SL_WFX_SDIO_FBR1_BLOCK_SIZE_MSB,
                                           &value_u8);
  SL_WFX_ERROR_CHECK(result);

  result = sl_wfx_reg_read_32(SL_WFX_CONFIG_REG_ID, &value32);
  SL_WFX_ERROR_CHECK(result);

  if (value32 == 0 || value32 == 0xFFFFFFFF) {
    result = SL_STATUS_FAIL;
    SL_WFX_ERROR_CHECK(result);
  }

  error_handler:
  return result;
}

sl_status_t sl_wfx_deinit_bus(void)
{
  return sl_wfx_host_deinit_bus();
}
