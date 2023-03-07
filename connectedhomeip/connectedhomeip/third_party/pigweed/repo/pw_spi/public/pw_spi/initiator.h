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

#pragma once

#include <cstdint>
#include <type_traits>

#include "pw_assert/assert.h"
#include "pw_bytes/span.h"
#include "pw_status/status.h"

namespace pw::spi {

// ClockPolarity is a configuration parameter that specifies whether a SPI
// bus clock signal is active low, or active high.
enum class ClockPolarity : uint8_t {
  kActiveHigh,  // Corresponds to CPOL = 0
  kActiveLow,   // Corresponds to CPOL = 1
};

// ClockPhase is a configuration parameter that specifies whether the
// phase of the SPI bus clock is rising edge or falling edge.
enum class ClockPhase : uint8_t {
  kRisingEdge,   // Corresponds to CPHA = 0
  kFallingEdge,  // Corresponds to CPHA = 1
};

// Configuration parameter, specifying the bit order for data clocked over the
// SPI bus; whether least-significant bit first, or most-significant bit first
enum class BitOrder : uint8_t {
  kLsbFirst,
  kMsbFirst,
};

// Configuration object used to represent the number of bits in a SPI
// data word. Devices typically use 8-bit words, although values of 3-32
// are sometimes specified for bus-level optimizations.  Values outside
// this range are considered an error.
class BitsPerWord {
 public:
  constexpr BitsPerWord(uint8_t data_bits) : data_bits_(data_bits) {
    PW_ASSERT(data_bits_ >= 3 && data_bits_ <= 32);
  }

  uint8_t operator()() const { return data_bits_; }

 private:
  uint8_t data_bits_;
};

// This struct contains the necessary configuration details required to
// initialize a SPI bus for communication with a target device.
struct Config {
  ClockPolarity polarity;
  ClockPhase phase;
  BitsPerWord bits_per_word;
  BitOrder bit_order;
};
static_assert(sizeof(Config) == sizeof(uint32_t),
              "Ensure that the config struct fits in 32-bits");

// The Initiator class provides an abstract interface used to configure and
// transmit data using a SPI bus.
class Initiator {
 public:
  virtual ~Initiator() = default;

  // Configure the SPI bus to communicate with peripherals using a given set of
  // properties, including the clock polarity, clock phase, bit-order, and
  // bits-per-word.
  // Returns OkStatus() on success, and implementation-specific values on
  // failure.
  virtual Status Configure(const Config& config) = 0;

  // Perform a synchronous read/write operation on the SPI bus.  Data from the
  // `write_buffer` object is written to the bus, while the `read_buffer` is
  // populated with incoming data on the bus.  The operation will ensure that
  // all requested data is written-to and read-from the bus. In the event the
  // read buffer is smaller than the write buffer (or zero-size), any additional
  // input bytes are discarded. In the event the write buffer is smaller than
  // the read buffer (or zero size), the output is padded with 0-bits for the
  // remainder of the transfer.
  // Returns OkStatus() on success, and implementation-specific values on
  // failure.
  virtual Status WriteRead(ConstByteSpan write_buffer,
                           ByteSpan read_buffer) = 0;
};

}  // namespace pw::spi
