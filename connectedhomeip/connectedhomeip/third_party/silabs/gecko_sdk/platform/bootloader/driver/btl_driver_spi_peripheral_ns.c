/***************************************************************************//**
 * @file
 * @brief Non-secure universal SPI peripheral driver.
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
#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>

// -----------------------------------------------------------------------------
// NSC functions

extern void spi_nsc_peripheral_init(void);
extern void spi_nsc_peripheral_deinit(void);
extern int32_t spi_nsc_peripheral_sendBuffer(uint8_t* buffer, size_t length);
extern int32_t spi_nsc_peripheral_sendByte(uint8_t byte);
extern size_t spi_nsc_peripheral_getTxBytesLeft(void);
extern void spi_nsc_peripheral_enableTransmitter(bool enable);
extern void spi_nsc_peripheral_enableReceiver(bool enable);
extern size_t spi_nsc_peripheral_getRxAvailableBytes(void);
extern int32_t spi_nsc_peripheral_receiveBuffer(Bootloader_inOutVec_t *input_vec,
                                                size_t                input_len,
                                                Bootloader_inOutVec_t *output_vec,
                                                size_t                output_len);
extern int32_t spi_nsc_peripheral_receiveByte(uint8_t* byte);
extern void spi_nsc_peripheral_flush(bool flushTx, bool flushRx);

// -----------------------------------------------------------------------------
// NS functions

void spi_peripheral_init(void)
{
  spi_nsc_peripheral_init();
}

void spi_peripheral_deinit(void)
{
  spi_nsc_peripheral_deinit();
}

int32_t spi_peripheral_sendBuffer(uint8_t* buffer, size_t length)
{
  return spi_nsc_peripheral_sendBuffer(buffer, length);
}

int32_t spi_peripheral_sendByte(uint8_t byte)
{
  return spi_nsc_peripheral_sendByte(byte);
}

size_t spi_peripheral_getTxBytesLeft(void)
{
  return spi_nsc_peripheral_getTxBytesLeft();
}

void spi_peripheral_enableTransmitter(bool enable)
{
  spi_nsc_peripheral_enableTransmitter(enable);
}

void spi_peripheral_enableReceiver(bool enable)
{
  spi_nsc_peripheral_enableReceiver(enable);
}

size_t spi_peripheral_getRxAvailableBytes(void)
{
  return spi_nsc_peripheral_getRxAvailableBytes();
}

int32_t spi_peripheral_receiveBuffer(uint8_t *buffer,
                                     size_t requestedLength,
                                     size_t *receivedLength,
                                     bool blocking,
                                     uint32_t timeout)
{
  Bootloader_inOutVec_t input_vec[2];
  Bootloader_inOutVec_t output_vec[2];

  input_vec[0].base = &blocking;
  input_vec[1].base = &timeout;

  output_vec[0].base = buffer;
  output_vec[0].len = requestedLength;
  output_vec[1].base = receivedLength;
  output_vec[1].len = sizeof(receivedLength);

  return spi_nsc_peripheral_receiveBuffer(input_vec,
                                          2u,
                                          output_vec,
                                          2u);
}

int32_t spi_peripheral_receiveByte(uint8_t* byte)
{
  return spi_nsc_peripheral_receiveByte(byte);
}

void spi_peripheral_flush(bool flushTx, bool flushRx)
{
  spi_nsc_peripheral_flush(flushTx, flushRx);
}