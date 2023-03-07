// Copyright 2022 The Pigweed Authors
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

#include "mss_gpio/mss_gpio.h"
#include "mss_uart/mss_uart.h"
#include "pw_preprocessor/concat.h"
#include "pw_status/status.h"
#include "pw_sys_io/sys_io.h"
#include "pw_sys_io_emcraft_sf2_private/config.h"

namespace {

// LEDs GPIOs

constexpr mss_gpio_id_t kDs3LedGPIO = MSS_GPIO_1;
constexpr mss_gpio_id_t kDs4LEDGPIO = MSS_GPIO_2;
constexpr uint32_t kDs3LedMask = MSS_GPIO_1_MASK;
constexpr uint32_t kDs4LedMask = MSS_GPIO_2_MASK;

constexpr uint32_t kReadDataReady = 0x1u;

}  // namespace

extern "C" void pw_sys_io_Init() {
  // Configure MSS GPIOs.
#if SF2_MSS_NO_BOOTLOADER
  MSS_GPIO_init();
#endif

  MSS_GPIO_config(kDs3LedGPIO, MSS_GPIO_OUTPUT_MODE);
  MSS_GPIO_config(kDs4LEDGPIO, MSS_GPIO_OUTPUT_MODE);
  // Set LEDs to initial app state
  MSS_GPIO_set_outputs(MSS_GPIO_get_outputs() | kDs4LedMask);

  // Initialize the UART0 controller (57600, 8N1)
  // Due to a HW eratta in SF2, we need to run at 57600 for
  // in-system-programming mode. If we are not upgrading FPGA or flash then we
  // can use a faster BAUD.
  MSS_UART_init(
      &g_mss_uart0,
      MSS_UART_57600_BAUD,
      MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);
}

// This whole implementation is very inefficient because it uses the synchronous
// polling UART API and only reads / writes 1 byte at a time.
namespace pw::sys_io {

Status ReadByte(std::byte* dest) {
  while (true) {
    if (TryReadByte(dest).ok()) {
      return OkStatus();
    }
  }
}

Status TryReadByte(std::byte* dest) {
  if (!(g_mss_uart0.hw_reg->LSR & kReadDataReady)) {
    return Status::Unavailable();
  }

  *dest = static_cast<std::byte>(g_mss_uart0.hw_reg->RBR);
  return OkStatus();
}

Status WriteByte(std::byte b) {
  // Wait for TX buffer to be empty. When the buffer is empty, we can write
  // a value to be dumped out of UART.
  const uint8_t pbuff = (uint8_t)b;

  MSS_UART_polled_tx(&g_mss_uart0, &pbuff, 1);
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
