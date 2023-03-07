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

#include "pw_checksum/crc16_ccitt.h"
#include "pw_kvs/checksum.h"
#include "pw_span/span.h"

namespace pw::kvs {

class ChecksumCrc16 final : public ChecksumAlgorithm {
 public:
  ChecksumCrc16() : ChecksumAlgorithm(as_bytes(span<uint16_t>(&crc_, 1))) {}

  void Reset() override { crc_ = checksum::Crc16Ccitt::kInitialValue; }

  void Update(span<const std::byte> data) override {
    crc_ = checksum::Crc16Ccitt::Calculate(data, crc_);
  }

 private:
  uint16_t crc_ = checksum::Crc16Ccitt::kInitialValue;
};

}  // namespace pw::kvs
