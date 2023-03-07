/***************************************************************************//**
 * @file
 * @brief Universal UART veneer driver.
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
#include "driver/btl_serial_driver.h"
#include "api/btl_interface.h"

// -----------------------------------------------------------------------------
// Static

// -----------------------------------------------------------------------------
// NSC functions

__attribute__((cmse_nonsecure_entry))
void uart_nsc_init(void)
{
  uart_init();
}

__attribute__((cmse_nonsecure_entry))
void uart_nsc_deinit(void)
{
  uart_deinit();
}

__attribute__((cmse_nonsecure_entry))
int32_t uart_nsc_sendBuffer(uint8_t* buffer, size_t length, bool blocking)
{
  if (!bl_verify_ns_memory_access(buffer, length)) {
    bl_fatal_assert_action();
  }

  return uart_sendBuffer(buffer, length, blocking);
}

__attribute__((cmse_nonsecure_entry))
int32_t uart_nsc_sendByte(uint8_t byte)
{
  return uart_sendByte(byte);
}

__attribute__((cmse_nonsecure_entry))
bool uart_nsc_isTxIdle(void)
{
  return uart_isTxIdle();
}

__attribute__((cmse_nonsecure_entry))
size_t uart_nsc_getRxAvailableBytes(void)
{
  return uart_getRxAvailableBytes();
}

__attribute__((cmse_nonsecure_entry))
int32_t uart_nsc_receiveBuffer(Bootloader_inOutVec_t *input_vec,
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

  return uart_receiveBuffer(buffer, requestedLength, receivedLength, *blocking, *timeout);
}

__attribute__((cmse_nonsecure_entry))
int32_t uart_nsc_receiveByte(uint8_t *byte)
{
  if (!bl_verify_ns_memory_access(byte, sizeof(byte))) {
    bl_fatal_assert_action();
  }

  return uart_receiveByte(byte);
}

__attribute__((cmse_nonsecure_entry))
int32_t uart_nsc_receiveByteTimeout(uint8_t* byte, uint32_t timeout)
{
  if (!bl_verify_ns_memory_access(byte, sizeof(byte))) {
    bl_fatal_assert_action();
  }

  return uart_receiveByteTimeout(byte, timeout);
}

__attribute__((cmse_nonsecure_entry))
int32_t uart_nsc_flush(bool flushTx, bool flushRx)
{
  return uart_flush(flushTx, flushRx);
}
