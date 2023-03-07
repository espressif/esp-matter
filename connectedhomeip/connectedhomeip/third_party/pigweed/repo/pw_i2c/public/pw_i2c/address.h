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
#pragma once

#include <cstdint>

namespace pw::i2c {

// The Address is a helper class which represents I2C addresses which is
// used by pw::i2c APIs.
//
// Example usage:
//   constexpr Address foo = Address::SevenBit<0x42>();
//   uint8_t foo_raw_address = foo.GetSevenBit();
//
//   const Address bar(0x200);  // 10 bit address.
//   uint16_t bar_raw_address = bar.GetTenBit();
//   // Note that bar.GetSevenBit() would assert here.
//
class Address {
 public:
  static constexpr uint8_t kMaxSevenBitAddress = (1 << 7) - 1;
  static constexpr uint16_t kMaxTenBitAddress = (1 << 10) - 1;

  // Helper constructor to ensure the address fits in the address space at
  // compile time, skipping the construction time assert.
  template <uint16_t kAddress>
  static constexpr Address TenBit() {
    static_assert(kAddress <= kMaxTenBitAddress);
    return Address(kAddress, kAlreadyCheckedAddress);
  }

  // Helper constructor to ensure the address fits in the address space at
  // compile time, skipping the construction time assert.
  template <uint8_t kAddress>
  static constexpr Address SevenBit() {
    static_assert(kAddress <= kMaxSevenBitAddress);
    return Address(kAddress, kAlreadyCheckedAddress);
  }

  // Precondition: The address is at least a valid ten bit address.
  explicit Address(uint16_t address);

  // Precondition: The address is a valid 7 bit address.
  uint8_t GetSevenBit() const;

  uint16_t GetTenBit() const { return address_; }

 private:
  enum AlreadyCheckedAddress { kAlreadyCheckedAddress };
  constexpr Address(uint16_t address, AlreadyCheckedAddress)
      : address_(address) {}

  uint16_t address_;
};

}  // namespace pw::i2c
