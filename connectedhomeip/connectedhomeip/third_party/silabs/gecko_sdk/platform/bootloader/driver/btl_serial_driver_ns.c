/***************************************************************************//**
 * @file
 * @brief Non-secure universal UART driver.
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

#include "api/btl_interface.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

// -----------------------------------------------------------------------------
// NSC functions

extern void uart_nsc_init(void);
extern void uart_nsc_deinit(void);
extern int32_t uart_nsc_sendBuffer(uint8_t* buffer, size_t length, bool blocking);
extern int32_t uart_nsc_sendByte(uint8_t byte);
extern bool uart_nsc_isTxIdle(void);
extern size_t uart_nsc_getRxAvailableBytes(void);
extern int32_t uart_nsc_receiveBuffer(Bootloader_inOutVec_t *input_vec,
                                      size_t                input_len,
                                      Bootloader_inOutVec_t *output_vec,
                                      size_t                output_len);
extern int32_t uart_nsc_receiveByte(uint8_t *byte);
extern int32_t uart_nsc_receiveByteTimeout(uint8_t* byte, uint32_t timeout);
extern int32_t uart_nsc_flush(bool flushTx, bool flushRx);

// -----------------------------------------------------------------------------
// NS functions

void uart_init(void)
{
  uart_nsc_init();
}

void uart_deinit(void)
{
  uart_nsc_deinit();
}

int32_t uart_sendBuffer(uint8_t* buffer, size_t length, bool blocking)
{
  return uart_nsc_sendBuffer(buffer, length, blocking);
}

int32_t uart_sendByte(uint8_t byte)
{
  return uart_nsc_sendByte(byte);
}

bool uart_isTxIdle(void)
{
  return uart_nsc_isTxIdle();
}

size_t uart_getRxAvailableBytes(void)
{
  return uart_nsc_getRxAvailableBytes();
}

int32_t uart_receiveBuffer(uint8_t *buffer,
                           size_t requestedLength,
                           size_t *receivedLength,
                           bool blocking,
                           uint32_t timeout)
{
  Bootloader_inOutVec_t input_vec[2];
  Bootloader_inOutVec_t output_vec[2];

  output_vec[0].base = buffer;
  output_vec[0].len = requestedLength;
  output_vec[1].base = receivedLength;

  input_vec[0].base = &blocking;
  input_vec[1].base = &timeout;
  return uart_nsc_receiveBuffer(input_vec,
                                2u,
                                output_vec,
                                2u);
}

int32_t uart_receiveByte(uint8_t *byte)
{
  return uart_nsc_receiveByte(byte);
}

int32_t uart_receiveByteTimeout(uint8_t *byte, uint32_t timeout)
{
  return uart_nsc_receiveByteTimeout(byte, timeout);
}

int32_t uart_flush(bool flushTx, bool flushRx)
{
  return uart_nsc_flush(flushTx, flushRx);
}