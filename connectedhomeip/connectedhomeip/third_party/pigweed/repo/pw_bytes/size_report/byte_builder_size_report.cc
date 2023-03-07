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

// This size report uses either ByteBuilder or manual bytes manipulation
// depending on whether "USE_BYTE_BUILDER" is set to true/false.
// Building the file: ninja -C out build_me

#include <array>
#include <cstdint>
#include <cstdio>

#include "pw_bytes/bit.h"
#include "pw_bytes/byte_builder.h"

#if !defined(USE_BYTE_BUILDER)
#error "USE_BYTE_BUILDER must be defined"
#endif  // !defined(USE_BYTE_BUILDER)

namespace pw::bytes {

#if USE_BYTE_BUILDER

ByteBuffer<8> bb;

void PutBytes() {
  bb.PutUint32(0x482B3D9E);
  bb.PutInt32(0x482B3D9E, endian::big);
}

void ReadBytes() {
  auto it = bb.begin();

  std::printf("%u\n", static_cast<unsigned>(it.ReadUint32()));
  std::printf("%d\n", static_cast<int>(it.ReadInt32(endian::big)));
}

#else  // !USE_BYTE_BUILDER

std::byte b_array[8];

void PutBytes() {
  uint32_t kVal1 = 0x482B3D9E;
  int32_t kVal2 = 0x482B3D9E;

  if (endian::native == endian::little) {
    std::memcpy(b_array, &kVal1, sizeof(kVal1));

    kVal2 = int32_t(((kVal2 & 0x000000FF) << 3 * 8) |  //
                    ((kVal2 & 0x0000FF00) << 1 * 8) |  //
                    ((kVal2 & 0x00FF0000) >> 1 * 8) |  //
                    ((kVal2 & 0xFF000000) >> 3 * 8));
    std::memcpy(b_array + 4, &kVal2, sizeof(kVal2));
  } else {
    kVal1 = uint32_t(((kVal1 & 0x000000FF) << 3 * 8) |  //
                     ((kVal1 & 0x0000FF00) << 1 * 8) |  //
                     ((kVal1 & 0x00FF0000) >> 1 * 8) |  //
                     ((kVal1 & 0xFF000000) >> 3 * 8));
    std::memcpy(b_array, &kVal1, sizeof(kVal1));

    std::memcpy(b_array + 4, &kVal2, sizeof(kVal2));
  }
}

void ReadBytes() {
  uint32_t kVal1;
  int32_t kVal2;

  if (endian::native == endian::little) {
    std::memcpy(&kVal1, b_array, sizeof(kVal1));
    std::memcpy(&kVal2, b_array + 4, sizeof(kVal2));
    kVal2 = int32_t(((kVal2 & 0x000000FF) << 3 * 8) |  //
                    ((kVal2 & 0x0000FF00) << 1 * 8) |  //
                    ((kVal2 & 0x00FF0000) >> 1 * 8) |  //
                    ((kVal2 & 0xFF000000) >> 3 * 8));
  } else {
    std::memcpy(&kVal1, b_array, sizeof(kVal1));
    std::memcpy(&kVal2, b_array + 4, sizeof(kVal2));

    kVal1 = uint32_t(((kVal1 & 0x000000FF) << 3 * 8) |  //
                     ((kVal1 & 0x0000FF00) << 1 * 8) |  //
                     ((kVal1 & 0x00FF0000) >> 1 * 8) |  //
                     ((kVal1 & 0xFF000000) >> 3 * 8));
  }

  std::printf("%u\n", static_cast<unsigned>(kVal1));
  std::printf("%d\n", static_cast<int>(kVal2));
}

#endif  // USE_BYTE_BUILDER

}  // namespace pw::bytes

int main() {
  pw::bytes::PutBytes();
  pw::bytes::ReadBytes();
  return 0;
}
