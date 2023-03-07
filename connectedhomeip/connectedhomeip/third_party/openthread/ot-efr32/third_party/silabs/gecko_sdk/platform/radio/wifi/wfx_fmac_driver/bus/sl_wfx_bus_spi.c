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
#include "sl_wfx_registers.h"

#define SET_WRITE 0x7FFF /* usage: and operation */
#define SET_READ 0x8000  /* usage: or operation */

sl_status_t sl_wfx_reg_read(sl_wfx_register_address_t address, void *buffer, uint32_t length)
{
  uint16_t header = SET_READ | (address << 12) | (length / 2);

  uint8_t header_as_bytes[2];
  sl_wfx_pack_16bit_big_endian(header_as_bytes, header);

  sl_wfx_host_spi_cs_assert();
  sl_wfx_host_spi_transfer_no_cs_assert(SL_WFX_BUS_READ, header_as_bytes, 2, (uint8_t *)buffer, length);
  sl_wfx_host_spi_cs_deassert();

  if (address == SL_WFX_CONFIG_REG_ID) {
    // Config always read/written in "word mode 0"
    // that is, its byte-order looks like that
    // B1 B0 B3 B2
    // here is this rearrangement
    uint8_t *tmp = (uint8_t *)buffer;
    uint8_t byte_swap = tmp[0];
    tmp[0] = tmp[1];
    tmp[1] = byte_swap;

    byte_swap = tmp[2];
    tmp[2] = tmp[3];
    tmp[3] = byte_swap;
  }

  return SL_STATUS_OK;
}

sl_status_t sl_wfx_reg_write(sl_wfx_register_address_t address, const void *buffer, uint32_t length)
{
  if (address == SL_WFX_CONFIG_REG_ID) {
    // Config always read/written in "word mode 0"
    // that is, its byte-order looks like that
    // B1 B0 B3 B2
    // here is this rearrangement
    uint8_t *tmp = (uint8_t *)buffer;
    uint8_t byte_swap = tmp[0];
    tmp[0] = tmp[1];
    tmp[1] = byte_swap;

    byte_swap = tmp[2];
    tmp[2] = tmp[3];
    tmp[3] = byte_swap;
  }

  uint16_t header = /* write flag = 0*/ (address << 12) | (length / 2);

  uint8_t header_as_bytes[2];
  sl_wfx_pack_16bit_big_endian(header_as_bytes, header);

  sl_wfx_host_spi_cs_assert();
  /* Note: sl_wfx_host_spi_transfer_no_cs_assert() does not modify the buffer when doing SL_WFX_BUS_WRITE */
  sl_wfx_host_spi_transfer_no_cs_assert(SL_WFX_BUS_WRITE, header_as_bytes, 2, (uint8_t *)buffer, length);
  sl_wfx_host_spi_cs_deassert();

  return SL_STATUS_OK;
}

sl_status_t sl_wfx_init_bus(void)
{
  sl_status_t status;
  uint32_t value32;

  status = sl_wfx_host_init_bus();
  SL_WFX_ERROR_CHECK(status);

  sl_wfx_host_reset_chip();

  status = sl_wfx_reg_read_32(SL_WFX_CONFIG_REG_ID, &value32);
  SL_WFX_ERROR_CHECK(status);

  if (value32 == 0 || value32 == 0xFFFFFFFF) {
    status = SL_STATUS_FAIL;
    SL_WFX_ERROR_CHECK(status);
  }

  // setting up "word mode = b10", this means that data on SPI bus
  // is sent as B0, B1, B2, B3 , ....
  value32 &= ~SL_WFX_CONFIG_WORD_MODE_BITS;
  value32 |= SL_WFX_CONFIG_WORD_MODE_2;

  /* Write CONFIG Register to configure SPI clock edge to support high frequency*/
  value32 |= 0x04045400;

  status = sl_wfx_reg_write_32(SL_WFX_CONFIG_REG_ID, value32);
  SL_WFX_ERROR_CHECK(status);

  error_handler:
  return status;
}

sl_status_t sl_wfx_deinit_bus(void)
{
  return sl_wfx_host_deinit_bus();
}
