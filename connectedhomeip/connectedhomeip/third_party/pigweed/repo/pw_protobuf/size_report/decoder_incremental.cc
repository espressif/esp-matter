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

#include <cstring>

#include "pw_bloat/bloat_this_binary.h"
#include "pw_protobuf/decoder.h"

namespace {
// clang-format off
constexpr uint8_t encoded_proto[] = {
  // type=int32, k=1, v=42
  0x08, 0x2a,
  // type=sint32, k=2, v=-13
  0x10, 0x19,
};
// clang-format on
}  // namespace

int* volatile non_optimizable_pointer;

int main() {
  pw::bloat::BloatThisBinary();

  int32_t test_int32, test_sint32;
  std::string_view str;
  float f;
  double d;
  uint32_t uint;

  pw::protobuf::Decoder decoder(pw::as_bytes(pw::span(encoded_proto)));
  while (decoder.Next().ok()) {
    switch (decoder.FieldNumber()) {
      case 1:
        decoder.ReadInt32(&test_int32);
        break;
      case 2:
        decoder.ReadSint32(&test_sint32);
        break;
      case 3:
        decoder.ReadString(&str);
        break;
      case 4:
        decoder.ReadFloat(&f);
        break;
      case 5:
        decoder.ReadDouble(&d);
        break;

      // Extra fields over decoder_full.
      case 21:
        decoder.ReadInt32(&test_int32);
        break;
      case 22:
        decoder.ReadUint32(&uint);
        break;
      case 23:
        decoder.ReadSint32(&test_sint32);
        break;
    }
  }

  *non_optimizable_pointer = test_int32 + test_sint32;

  return 0;
}
