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

#include "gmock/gmock.h"
#include "pw_i2c/initiator.h"

namespace pw::i2c {

class GmockInitiator : public Initiator {
 public:
  MOCK_METHOD(Status,
              DoWriteReadFor,
              (Address device_address,
               ConstByteSpan tx_buffer,
               ByteSpan rx_buffer,
               chrono::SystemClock::duration timeout),
              (override));
};

}  // namespace pw::i2c
