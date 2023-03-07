// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include "pw_sys_io/sys_io.h"

#include <cinttypes>

#include "pw_preprocessor/concat.h"
#include "pw_status/status.h"
#include "pw_sys_io_stm32cube_private/config.h"
#include "stm32cube/stm32cube.h"

// These macros remap config options to the various STM32Cube HAL macro names.

// USART_INSTANCE defined to USARTn, where n is the USART peripheral index.
#define USART_INSTANCE \
  PW_CONCAT(PW_SYS_IO_STM32CUBE_USART_PREFIX, PW_SYS_IO_STM32CUBE_USART_NUM)

// USART_GPIO_ALTERNATE_FUNC defined to GPIO_AFm_USARTn, where m is the
// alternate function index and n is the USART peripheral index.
#define USART_GPIO_ALTERNATE_FUNC             \
  PW_CONCAT(GPIO_AF,                          \
            PW_SYS_IO_STM32CUBE_GPIO_AF,      \
            _,                                \
            PW_SYS_IO_STM32CUBE_USART_PREFIX, \
            PW_SYS_IO_STM32CUBE_USART_NUM)

// USART_GPIO_PORT defined to GPIOx, where x is the GPIO port letter that the
// TX/RX pins are on.
#define USART_GPIO_TX_PORT PW_CONCAT(GPIO, PW_SYS_IO_STM32CUBE_GPIO_TX_PORT)
#define USART_GPIO_RX_PORT PW_CONCAT(GPIO, PW_SYS_IO_STM32CUBE_GPIO_RX_PORT)
#define USART_GPIO_TX_PIN PW_CONCAT(GPIO_PIN_, PW_SYS_IO_STM32CUBE_GPIO_TX_PIN)
#define USART_GPIO_RX_PIN PW_CONCAT(GPIO_PIN_, PW_SYS_IO_STM32CUBE_GPIO_RX_PIN)

// USART_GPIO_PORT_ENABLE defined to __HAL_RCC_GPIOx_CLK_ENABLE, where x is the
// GPIO port letter that the TX/RX pins are on.
#define USART_GPIO_TX_PORT_ENABLE \
  PW_CONCAT(__HAL_RCC_GPIO, PW_SYS_IO_STM32CUBE_GPIO_TX_PORT, _CLK_ENABLE)

#define USART_GPIO_RX_PORT_ENABLE \
  PW_CONCAT(__HAL_RCC_GPIO, PW_SYS_IO_STM32CUBE_GPIO_RX_PORT, _CLK_ENABLE)

// USART_ENABLE defined to __HAL_RCC_USARTn_CLK_ENABLE, where n is the USART
// peripheral index.
#define USART_ENABLE                          \
  PW_CONCAT(__HAL_RCC_,                       \
            PW_SYS_IO_STM32CUBE_USART_PREFIX, \
            PW_SYS_IO_STM32CUBE_USART_NUM,    \
            _CLK_ENABLE)

static UART_HandleTypeDef uart;

extern "C" void pw_sys_io_Init() {
  GPIO_InitTypeDef GPIO_InitStruct = {};

  USART_ENABLE();
  USART_GPIO_TX_PORT_ENABLE();
  USART_GPIO_RX_PORT_ENABLE();

  GPIO_InitStruct.Pin = USART_GPIO_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = USART_GPIO_ALTERNATE_FUNC;
  HAL_GPIO_Init(USART_GPIO_TX_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = USART_GPIO_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = USART_GPIO_ALTERNATE_FUNC;
  HAL_GPIO_Init(USART_GPIO_RX_PORT, &GPIO_InitStruct);

  uart.Instance = USART_INSTANCE;
  uart.Init.BaudRate = 115200;
  uart.Init.WordLength = UART_WORDLENGTH_8B;
  uart.Init.StopBits = UART_STOPBITS_1;
  uart.Init.Parity = UART_PARITY_NONE;
  uart.Init.Mode = UART_MODE_TX_RX;
  uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&uart);
}

// This whole implementation is very inefficient because it uses the synchronous
// polling UART API and only reads / writes 1 byte at a time.
namespace pw::sys_io {
Status ReadByte(std::byte* dest) {
  if (HAL_UART_Receive(
          &uart, reinterpret_cast<uint8_t*>(dest), 1, HAL_MAX_DELAY) !=
      HAL_OK) {
    return Status::ResourceExhausted();
  }
  return OkStatus();
}

Status TryReadByte(std::byte* dest) { return Status::Unimplemented(); }

Status WriteByte(std::byte b) {
  if (HAL_UART_Transmit(
          &uart, reinterpret_cast<uint8_t*>(&b), 1, HAL_MAX_DELAY) != HAL_OK) {
    return Status::ResourceExhausted();
  }
  return OkStatus();
}

// Writes a string using pw::sys_io, and add newline characters at the end.
StatusWithSize WriteLine(const std::string_view& s) {
  size_t chars_written = 0;
  StatusWithSize result = WriteBytes(as_bytes(span(s)));
  if (!result.ok()) {
    return result;
  }
  chars_written += result.size();

  // Write trailing newline.
  result = WriteBytes(as_bytes(span("\r\n", 2)));
  chars_written += result.size();

  return StatusWithSize(OkStatus(), chars_written);
}

}  // namespace pw::sys_io
