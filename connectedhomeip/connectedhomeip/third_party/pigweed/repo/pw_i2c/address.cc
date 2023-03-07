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

#include "pw_i2c/address.h"

#include "pw_assert/check.h"

namespace pw::i2c {

// Implemented in the source to produce a nicer CHECK message vs an ASSERT.
Address::Address(uint16_t address) : address_(address) {
  PW_CHECK_UINT_LE(address_, kMaxTenBitAddress);
}

// Implemented in the source to produce a nicer CHECK message vs an ASSERT.
uint8_t Address::GetSevenBit() const {
  PW_CHECK_UINT_LE(address_, kMaxSevenBitAddress);
  return address_;
}

}  // namespace pw::i2c
