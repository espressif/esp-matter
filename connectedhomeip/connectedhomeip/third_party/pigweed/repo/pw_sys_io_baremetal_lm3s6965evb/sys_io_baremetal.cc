// Copyright 2020 The Pigweed Authors
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

#include <cinttypes>

#include "pw_preprocessor/compiler.h"
#include "pw_sys_io/sys_io.h"

namespace {

// Default core clock. This is technically not a constant, but since this app
// doesn't change the system clock a constant will suffice.
constexpr uint32_t kSystemCoreClock = 12000000;

// UART status flags.
constexpr uint32_t kTxFifoEmptyMask = 0b10000000;
constexpr uint32_t kRxFifoFullMask = 0b100000;

// UART line control flags.
// Default: 8n1
constexpr uint32_t kDefaultLineControl = 0x60;

// UART control flags.
constexpr uint32_t kUartEnableMask = 0x1;

PW_PACKED(struct) UartBlock {
  uint32_t data_register;
  uint32_t receive_error;
  uint32_t reserved1[4];
  uint32_t status_flags;
  uint32_t reserved2;
  uint32_t low_power;
  uint32_t integer_baud;
  uint32_t fractional_baud;
  uint32_t line_control;
  uint32_t control;
  uint32_t interrupt_fifo_level;
  uint32_t interrupt_mask;
  uint32_t raw_interrupt;
  uint32_t masked_interrupt;
  uint32_t interrupt_clear;
};

// Declare a reference to the memory mapped block for UART0.
volatile UartBlock& uart0 = *reinterpret_cast<volatile UartBlock*>(0x4000C000U);

constexpr uint32_t kRcgcUart0EnableMask = 0x1;
volatile uint32_t& rcgc1 = *reinterpret_cast<volatile uint32_t*>(0x400FE104U);

// Calculate a baud rate multiplier such that we have 16 bits of precision for
// the integer portion and 6 bits for the fractional portion.
void SetBaudRate(uint32_t clock, uint32_t target_baud) {
  uint32_t divisor = target_baud * 16;
  uint32_t remainder = clock % divisor;
  uart0.integer_baud = (clock % divisor) & 0xffff;
  uart0.fractional_baud = (((remainder << 7) / divisor + 1) >> 1) & 0x3f;
}

}  // namespace

extern "C" void pw_sys_io_lm3s6965evb_Init() {
  rcgc1 |= kRcgcUart0EnableMask;
  for (volatile int i = 0; i < 3; ++i) {
    // We must wait after enabling uart.
  }
  // Set baud rate.
  SetBaudRate(kSystemCoreClock, /*target_baud=*/115200);
  uart0.line_control = kDefaultLineControl;
  uart0.control |= kUartEnableMask;
}

namespace pw::sys_io {

// Wait for a byte to read on UART0. This blocks until a byte is read. This is
// extremely inefficient as it requires the target to burn CPU cycles polling to
// see if a byte is ready yet.
Status ReadByte(std::byte* dest) {
  while (true) {
    if (TryReadByte(dest).ok()) {
      return OkStatus();
    }
  }
}

Status TryReadByte(std::byte* dest) {
  if (uart0.receive_error) {
    // Writing anything to this register clears all errors.
    uart0.receive_error = 0xff;
  }
  if (!(uart0.status_flags & kRxFifoFullMask)) {
    return Status::Unavailable();
  }
  *dest = static_cast<std::byte>(uart0.data_register);
  return OkStatus();
}

// Send a byte over UART0. Since this blocks on every byte, it's rather
// inefficient. At the default baud rate of 115200, one byte blocks the CPU for
// ~87 micro seconds. This means it takes only 10 bytes to block the CPU for
// 1ms!
Status WriteByte(std::byte b) {
  // Wait for TX buffer to be empty. When the buffer is empty, we can write
  // a value to be dumped out of UART.
  while (!(uart0.status_flags & kTxFifoEmptyMask)) {
  }
  uart0.data_register = static_cast<uint32_t>(b);
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

  return StatusWithSize(result.status(), chars_written);
}

}  // namespace pw::sys_io
