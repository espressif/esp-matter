// Copyright 2019 The Pigweed Authors
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
constexpr uint32_t kSystemCoreClock = 16000000;

// Base address for everything peripheral-related on the STM32F4xx.
constexpr uint32_t kPeripheralBaseAddr = 0x40000000u;
// Base address for everything AHB1-related on the STM32F4xx.
constexpr uint32_t kAhb1PeripheralBase = kPeripheralBaseAddr + 0x00020000U;
// Base address for everything APB2-related on the STM32F4xx.
constexpr uint32_t kApb2PeripheralBase = kPeripheralBaseAddr + 0x00010000U;

// Reset/clock configuration block (RCC).
// `reserved` fields are unimplemented features, and are present to ensure
// proper alignment of registers that are in use.
PW_PACKED(struct) RccBlock {
  uint32_t reserved1[12];
  uint32_t ahb1_config;
  uint32_t reserved2[4];
  uint32_t apb2_config;
};

// Mask for ahb1_config (AHB1ENR) to enable the "A" GPIO pins.
constexpr uint32_t kGpioAEnable = 0x1u;

// Mask for apb2_config (APB2ENR) to enable USART1.
constexpr uint32_t kUsart1Enable = 0x1u << 4;

// GPIO register block definition.
PW_PACKED(struct) GpioBlock {
  uint32_t modes;
  uint32_t out_type;
  uint32_t out_speed;
  uint32_t pull_up_down;
  uint32_t input_data;
  uint32_t output_data;
  uint32_t gpio_bit_set;
  uint32_t port_config_lock;
  uint32_t alt_low;
  uint32_t alt_high;
};

// Constants related to GPIO mode register masks.
constexpr uint32_t kGpio9PortModePos = 18;
constexpr uint32_t kGpio10PortModePos = 20;
constexpr uint32_t kGpioPortModeAlternate = 2;

// Constants related to GPIO port speed register masks.
constexpr uint32_t kGpio9PortSpeedPos = 18;
constexpr uint32_t kGpio10PortSpeedPos = 20;
constexpr uint32_t kGpioSpeedVeryHigh = 3;

// Constants related to GPIO pull up/down resistor type masks.
constexpr uint32_t kGpio9PullTypePos = 18;
constexpr uint32_t kGpio10PullTypePos = 20;
constexpr uint32_t kPullTypePullUp = 1;

// Constants related to GPIO port speed register masks.
constexpr uint32_t kGpio9AltModeHighPos = 4;
constexpr uint32_t kGpio10AltModeHighPos = 8;

// Alternate function for pins A9 and A10 that enable USART1.
constexpr uint8_t kGpioAlternateFunctionUsart1 = 0x07u;

// USART status flags.
constexpr uint32_t kTxRegisterEmpty = 0x1u << 7;

// USART configuration flags for config1 register.
// Note: a large number of configuration flags have been omitted as they default
// to reasonable values and we don't need to change them.
constexpr uint32_t kReceiveEnable = 0x1 << 2;
constexpr uint32_t kTransmitEnable = 0x1 << 3;
constexpr uint32_t kEnableUsart = 0x1 << 13;

// USART configuration flags for status register.
constexpr uint32_t kReadDataReady = 0x1u << 5;

// Layout of memory mapped registers for USART blocks.
PW_PACKED(struct) UsartBlock {
  uint32_t status;
  // Only the lower 8 bits are valid. Read for RX data, write to TX data.
  uint32_t data_register;
  uint32_t baud_rate;
  uint32_t config1;
  uint32_t config2;
  uint32_t config3;
  uint32_t config4;
};

// Sets the UART baud register using the peripheral clock and target baud rate.
// These calculations are specific to the default oversample by 16 mode.
// TODO(amontanez): Document magic calculations in full UART implementation.
uint32_t CalcBaudRegister(uint32_t clock, uint32_t target_baud) {
  uint32_t div_fac = (clock * 25) / (4 * target_baud);
  uint32_t mantissa = div_fac / 100;
  uint32_t fraction = ((div_fac - mantissa * 100) * 16 + 50) / 100;

  return (mantissa << 4) + (fraction & 0xFFu);
}

// Declare a reference to the memory mapped RCC block.
volatile RccBlock& platform_rcc =
    *reinterpret_cast<volatile RccBlock*>(kAhb1PeripheralBase + 0x3800U);

// Declare a reference to the 'A' GPIO memory mapped block.
volatile GpioBlock& gpio_a =
    *reinterpret_cast<volatile GpioBlock*>(kAhb1PeripheralBase + 0x0000U);

// Declare a reference to the memory mapped block for USART1.
volatile UsartBlock& usart1 =
    *reinterpret_cast<volatile UsartBlock*>(kApb2PeripheralBase + 0x1000U);

}  // namespace

extern "C" void pw_sys_io_stm32f429_Init() {
  // Enable 'A' GIPO clocks.
  platform_rcc.ahb1_config |= kGpioAEnable;

  // Enable Uart TX pin.
  // Output type defaults to push-pull (rather than open/drain).
  gpio_a.modes |= kGpioPortModeAlternate << kGpio9PortModePos;
  gpio_a.out_speed |= kGpioSpeedVeryHigh << kGpio9PortSpeedPos;
  gpio_a.pull_up_down |= kPullTypePullUp << kGpio9PullTypePos;
  gpio_a.alt_high |= kGpioAlternateFunctionUsart1 << kGpio9AltModeHighPos;

  // Enable Uart RX pin.
  // Output type defaults to push-pull (rather than open/drain).
  gpio_a.modes |= kGpioPortModeAlternate << kGpio10PortModePos;
  gpio_a.out_speed |= kGpioSpeedVeryHigh << kGpio10PortSpeedPos;
  gpio_a.pull_up_down |= kPullTypePullUp << kGpio10PullTypePos;
  gpio_a.alt_high |= kGpioAlternateFunctionUsart1 << kGpio10AltModeHighPos;

  // Initialize USART1. Initialized to 8N1 at the specified baud rate.
  platform_rcc.apb2_config |= kUsart1Enable;

  // Warning: Normally the baud rate register calculation is based off
  // peripheral 2 clock. For this code, the peripheral clock defaults to
  // the system core clock so it can be used directly.
  usart1.baud_rate = CalcBaudRegister(kSystemCoreClock, /*target_baud=*/115200);

  usart1.config1 = kEnableUsart | kReceiveEnable | kTransmitEnable;
}

namespace pw::sys_io {

// Wait for a byte to read on USART1. This blocks until a byte is read. This is
// extremely inefficient as it requires the target to burn CPU cycles polling to
// see if a byte is ready yet.
Status ReadByte(std::byte* dest) {
  while (true) {
    if (TryReadByte(dest).ok()) {
      return OkStatus();
    }
  }
}

// Wait for a byte to read on USART1. This blocks until a byte is read. This is
// extremely inefficient as it requires the target to burn CPU cycles polling to
// see if a byte is ready yet.
Status TryReadByte(std::byte* dest) {
  if (!(usart1.status & kReadDataReady)) {
    return Status::Unavailable();
  }
  *dest = static_cast<std::byte>(usart1.data_register);
  return OkStatus();
}

// Send a byte over USART1. Since this blocks on every byte, it's rather
// inefficient. At the default baud rate of 115200, one byte blocks the CPU for
// ~87 micro seconds. This means it takes only 10 bytes to block the CPU for
// 1ms!
Status WriteByte(std::byte b) {
  // Wait for TX buffer to be empty. When the buffer is empty, we can write
  // a value to be dumped out of UART.
  while (!(usart1.status & kTxRegisterEmpty)) {
  }
  usart1.data_register = static_cast<uint32_t>(b);
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
