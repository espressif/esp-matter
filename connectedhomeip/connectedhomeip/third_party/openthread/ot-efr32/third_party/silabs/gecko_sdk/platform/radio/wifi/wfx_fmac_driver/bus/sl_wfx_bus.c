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

#include "sl_wfx.h"

#define CONFIG_PREFETCH_BIT (1 << 13)

static sl_status_t sl_wfx_bus_access(sl_wfx_host_bus_transfer_type_t type,
                                     sl_wfx_register_address_t address,
                                     void *buffer,
                                     uint32_t length);

sl_status_t sl_wfx_reg_read_16(sl_wfx_register_address_t address, uint16_t *value_out)
{
  uint8_t tmp[4];
  sl_status_t result = sl_wfx_bus_access(SL_WFX_BUS_READ, address, tmp, sizeof(tmp));

  *value_out = sl_wfx_unpack_16bit_little_endian(tmp);

#if (SL_WFX_DEBUG_MASK & SL_WFX_DEBUG_RX_REG)
  sl_wfx_host_log("RX_REG> addr:%02X, %02X%02X\r\n",
                  address,
                  (*value_out >> 8) & 0xFF,
                  *value_out & 0xFF);
#endif

  return result;
}

sl_status_t sl_wfx_reg_write_16(sl_wfx_register_address_t address, uint16_t value_in)
{
  uint8_t tmp[4];

#if (SL_WFX_DEBUG_MASK & SL_WFX_DEBUG_TX_REG)
  sl_wfx_host_log("TX_REG> addr:%02X, %02X%02X\r\n",
                  address,
                  (value_in >> 8) & 0xFF,
                  value_in & 0xFF);
#endif

  sl_wfx_pack_16bit_little_endian(tmp, value_in);
  tmp[2] = 0;
  tmp[3] = 0;

  return sl_wfx_bus_access(SL_WFX_BUS_WRITE, address, tmp, sizeof(tmp));
}

sl_status_t sl_wfx_reg_read_32(sl_wfx_register_address_t address, uint32_t *value_out)
{
  sl_status_t result = sl_wfx_bus_access(SL_WFX_BUS_READ, address, value_out, sizeof(*value_out));

  *value_out = sl_wfx_htole32(*value_out);

#if (SL_WFX_DEBUG_MASK & SL_WFX_DEBUG_RX_REG)
  sl_wfx_host_log("RX_REG> addr:%02X, %02X%02X%02X%02X\r\n",
                  address,
                  (*value_out >> 24) & 0xFF,
                  (*value_out >> 16) & 0xFF,
                  (*value_out >> 8) & 0xFF,
                  *value_out & 0xFF);
#endif

  return result;
}

sl_status_t sl_wfx_reg_write_32(sl_wfx_register_address_t address, uint32_t value_in)
{
#if (SL_WFX_DEBUG_MASK & SL_WFX_DEBUG_TX_REG)
  sl_wfx_host_log("TX_REG> addr:%02X, %02X%02X%02X%02X\r\n",
                  address,
                  (value_in >> 24) & 0xFF,
                  (value_in >> 16) & 0xFF,
                  (value_in >> 8) & 0xFF,
                  value_in & 0xFF);
#endif

  value_in = sl_wfx_htole32(value_in);

  return sl_wfx_bus_access(SL_WFX_BUS_WRITE, address, &value_in, sizeof(value_in));
}

sl_status_t sl_wfx_data_read(void *buffer, uint32_t length)
{
  sl_status_t result;

  result = sl_wfx_bus_access(SL_WFX_BUS_READ, SL_WFX_IN_OUT_QUEUE_REG_ID, buffer, length);

#if (SL_WFX_DEBUG_MASK & (SL_WFX_DEBUG_RX | SL_WFX_DEBUG_RX_RAW))
  sl_wfx_host_log("RX> %02X%02X %02X %02X ",
                  ((uint8_t *)buffer)[0],
                  ((uint8_t *)buffer)[1],
                  ((uint8_t *)buffer)[2],
                  ((uint8_t *)buffer)[3]);
#if (SL_WFX_DEBUG_MASK & SL_WFX_DEBUG_RX_RAW)
  for (uint16_t i = 4; i < length; i++) {
    sl_wfx_host_log("%02X", ((uint8_t *)buffer)[i]);
  }
#endif
  sl_wfx_host_log("\r\n");
#endif

  return result;
}

sl_status_t sl_wfx_data_write(const void *buffer, uint32_t length)
{
  sl_status_t result;

#if (SL_WFX_DEBUG_MASK & (SL_WFX_DEBUG_TX | SL_WFX_DEBUG_TX_RAW))
  sl_wfx_host_log("TX> %02X%02X %02X %02X ",
                  ((uint8_t *)buffer)[0],
                  ((uint8_t *)buffer)[1],
                  ((uint8_t *)buffer)[2],
                  ((uint8_t *)buffer)[3]);
#if (SL_WFX_DEBUG_MASK & SL_WFX_DEBUG_TX_RAW)
  for (uint16_t i = 4; i < length; i++) {
    sl_wfx_host_log("%02X", ((uint8_t *)buffer)[i]);
  }
#endif
  sl_wfx_host_log("\r\n");
#endif

  result = sl_wfx_bus_access(SL_WFX_BUS_WRITE, SL_WFX_IN_OUT_QUEUE_REG_ID, (void*) buffer, length);

  return result;
}

sl_status_t sl_wfx_apb_write(uint32_t address, const void *buffer, uint32_t length)
{
  sl_status_t result;
  if (length / 2 >= 0x1000) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  result = sl_wfx_reg_write_32(SL_WFX_SRAM_BASE_ADDR_REG_ID, address);

  if (result == SL_STATUS_OK) {
    result = sl_wfx_bus_access(SL_WFX_BUS_WRITE, SL_WFX_SRAM_DPORT_REG_ID, (void*) buffer, length);
  }

  return result;
}

sl_status_t sl_wfx_apb_write_32(uint32_t address, uint32_t value_in)
{
  sl_status_t result;

  result = sl_wfx_reg_write_32(SL_WFX_SRAM_BASE_ADDR_REG_ID, address);

  if (result == SL_STATUS_OK) {
    result = sl_wfx_reg_write_32(SL_WFX_SRAM_DPORT_REG_ID, value_in);
  }

  return result;
}

sl_status_t sl_wfx_apb_read_32(uint32_t address, uint32_t *value_out)
{
  uint32_t value32;
  sl_status_t result;

  // write address
  result = sl_wfx_reg_write_32(SL_WFX_SRAM_BASE_ADDR_REG_ID, address);

  // set the "prefetch" bit
  result = sl_wfx_reg_read_32(SL_WFX_CONFIG_REG_ID, &value32);
  result = sl_wfx_reg_write_32(SL_WFX_CONFIG_REG_ID, value32 | CONFIG_PREFETCH_BIT);

  // and wait for the prefetch bit to clear
  for (uint32_t i = 0; i < 20; i++) {
    result = sl_wfx_reg_read_32(SL_WFX_CONFIG_REG_ID, &value32);
    if ((value32 & CONFIG_PREFETCH_BIT) == 0) {
      break;
    }

    sl_wfx_host_wait(1);
  }

  // and data is ready
  result = sl_wfx_reg_read_32(SL_WFX_SRAM_DPORT_REG_ID, value_out);

  return result;
}

static sl_status_t sl_wfx_bus_access(sl_wfx_host_bus_transfer_type_t type,
                                     sl_wfx_register_address_t address,
                                     void *buffer,
                                     uint32_t length)
{
  sl_status_t result;

  /* If the WFx is sleeping, wake it up */
  if (sl_wfx_context->state & SL_WFX_SLEEPING) {
    result = sl_wfx_host_set_wake_up_pin(1);
    SL_WFX_ERROR_CHECK(result);
    /* If the command is of read type, consider the WFx awake */
    if (type == SL_WFX_BUS_WRITE) {
      result = sl_wfx_host_wait_for_wake_up();
      SL_WFX_ERROR_CHECK(result);
    }
    sl_wfx_context->state &= ~SL_WFX_SLEEPING;

#if (SL_WFX_DEBUG_MASK & SL_WFX_DEBUG_SLEEP)
    sl_wfx_host_log("WFx awake\r\n");
#endif
  }

  /* Send the communication on the bus */
  if (type == SL_WFX_BUS_READ) {
    result = sl_wfx_reg_read(address, buffer, length);
  } else if (type == SL_WFX_BUS_WRITE) {
    result = sl_wfx_reg_write(address, buffer, length);
  } else {
    result = SL_STATUS_INVALID_PARAMETER;
  }
  SL_WFX_ERROR_CHECK(result);

  /* If the power save is active and there is no confirmation pending, put
     the WFx back to sleep */
  if ((sl_wfx_context->state & SL_WFX_POWER_SAVE_ACTIVE)
      && (type == SL_WFX_BUS_READ)
      && ((address != SL_WFX_CONTROL_REG_ID)
          || ((address == SL_WFX_CONTROL_REG_ID) && ((*(uint16_t *)buffer & SL_WFX_CONT_NEXT_LEN_MASK) == 0)))
      && (sl_wfx_context->used_buffers <= 1)
      && ((*((uint16_t *)((uint8_t *)buffer + length - SL_WFX_CONT_REGISTER_SIZE)) & SL_WFX_CONT_NEXT_LEN_MASK) == 0)) {
    /* Ask the host opinion on whether the WFx should be put back to sleep or
       not*/
    if (sl_wfx_host_sleep_grant(type, address, length) == SL_STATUS_WIFI_SLEEP_GRANTED) {
      sl_wfx_context->state |= SL_WFX_SLEEPING;
      result = sl_wfx_host_set_wake_up_pin(0);
      SL_WFX_ERROR_CHECK(result);

#if (SL_WFX_DEBUG_MASK & SL_WFX_DEBUG_SLEEP)
      sl_wfx_host_log("WFx sleeping\r\n");
#endif
    }
  }

  error_handler:
  return result;
}
