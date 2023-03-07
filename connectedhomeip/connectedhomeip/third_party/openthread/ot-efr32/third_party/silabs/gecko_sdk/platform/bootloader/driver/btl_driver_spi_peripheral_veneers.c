/***************************************************************************//**
 * @file
 * @brief Universal SPI peripheral veneer driver.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Includes

#include "core/btl_tz_utils.h"
#include "driver/btl_driver_spi_peripheral.h"
#include "api/btl_interface.h"

// -----------------------------------------------------------------------------
// NSC functions

__attribute__((cmse_nonsecure_entry))
void spi_nsc_peripheral_init(void)
{
  spi_peripheral_init();
}

__attribute__((cmse_nonsecure_entry))
void spi_nsc_peripheral_deinit(void)
{
  spi_peripheral_deinit();
}

__attribute__((cmse_nonsecure_entry))
int32_t spi_nsc_peripheral_sendBuffer(uint8_t* buffer, size_t length)
{
  if (!bl_verify_ns_memory_access(buffer, length)) {
    bl_fatal_assert_action();
  }
  return spi_peripheral_sendBuffer(buffer, length);
}

__attribute__((cmse_nonsecure_entry))
int32_t spi_nsc_peripheral_sendByte(uint8_t byte)
{
  return spi_peripheral_sendByte(byte);
}

__attribute__((cmse_nonsecure_entry))
size_t spi_nsc_peripheral_getTxBytesLeft(void)
{
  return spi_peripheral_getTxBytesLeft();
}

__attribute__((cmse_nonsecure_entry))
void spi_nsc_peripheral_enableTransmitter(bool enable)
{
  spi_peripheral_enableTransmitter(enable);
}

__attribute__((cmse_nonsecure_entry))
void spi_nsc_peripheral_enableReceiver(bool enable)
{
  spi_peripheral_enableReceiver(enable);
}

__attribute__((cmse_nonsecure_entry))
size_t spi_nsc_peripheral_getRxAvailableBytes(void)
{
  return spi_peripheral_getRxAvailableBytes();
}

__attribute__((cmse_nonsecure_entry))
int32_t spi_nsc_peripheral_receiveBuffer(Bootloader_inOutVec_t *input_vec,
                                         size_t                input_len,
                                         Bootloader_inOutVec_t *output_vec,
                                         size_t                output_len)
{
  if (input_vec == NULL || output_vec == NULL || input_len != 2u || output_len != 2u) {
    return BOOTLOADER_ERROR_UART_ARGUMENT;
  }

  if (!bl_verify_ns_memory_access(input_vec, sizeof(Bootloader_inOutVec_t) * input_len)) {
    bl_fatal_assert_action();
  }
  if (!bl_verify_ns_memory_access(output_vec, sizeof(Bootloader_inOutVec_t) * output_len)) {
    bl_fatal_assert_action();
  }

  uint8_t *buffer = output_vec[0].base;
  size_t requestedLength = output_vec[0].len;
  size_t *receivedLength = output_vec[1].base;
  bool *blocking = input_vec[0].base;
  uint32_t *timeout = input_vec[1].base;

  if (!bl_verify_ns_memory_access(buffer, requestedLength)
      || !bl_verify_ns_memory_access(receivedLength, sizeof(receivedLength))
      || !bl_verify_ns_memory_access(blocking, sizeof(blocking))
      || !bl_verify_ns_memory_access(timeout, sizeof(timeout))) {
    bl_fatal_assert_action();
  }

  return spi_peripheral_receiveBuffer(buffer, requestedLength, receivedLength, *blocking, *timeout);
}

__attribute__((cmse_nonsecure_entry))
int32_t spi_nsc_peripheral_receiveByte(uint8_t* byte)
{
  if (!bl_verify_ns_memory_access(byte, sizeof(byte))) {
    bl_fatal_assert_action();
  }
  return spi_peripheral_receiveByte(byte);
}

__attribute__((cmse_nonsecure_entry))
void spi_nsc_peripheral_flush(bool flushTx, bool flushRx)
{
  spi_peripheral_flush(flushTx, flushRx);
}
