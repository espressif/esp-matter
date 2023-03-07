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

#include "pb_decode.h"
#include "pb_encode.h"
#include "pw_assert/check.h"
#include "pw_bloat/bloat_this_binary.h"
#include "pw_log/log.h"
#include "pw_sys_io/sys_io.h"

int volatile* unoptimizable;

namespace my_product {

template <typename DecodeFunction>
struct NanopbTraits;

template <typename FieldsType>
struct NanopbTraits<bool(pb_istream_t*, FieldsType, void*)> {
  using Fields = FieldsType;
};

using Fields = typename NanopbTraits<decltype(pb_decode)>::Fields;

// Performs the core nanopb encode and decode operations so that those functions
// are included in the binary.
void DoNanopbStuff() {
  std::byte buffer[128];
  void* fields = &buffer;

  auto output = pb_ostream_from_buffer(reinterpret_cast<pb_byte_t*>(buffer),
                                       sizeof(buffer));
  pb_encode(&output, static_cast<Fields>(fields), buffer);

  auto input = pb_istream_from_buffer(
      reinterpret_cast<const pb_byte_t*>(buffer), sizeof(buffer));
  pb_decode(&input, static_cast<Fields>(fields), buffer);
}

}  // namespace my_product

int main() {
  pw::bloat::BloatThisBinary();
  my_product::DoNanopbStuff();

  // Ensure we are paying the cost for log and assert.
  PW_CHECK_INT_GE(*unoptimizable, 0, "Ensure this CHECK logic stays");
  PW_LOG_INFO("We care about optimizing: %d", *unoptimizable);

  std::byte packet_buffer[128];
  pw::sys_io::ReadBytes(packet_buffer);
  pw::sys_io::WriteBytes(packet_buffer);

  return static_cast<int>(packet_buffer[92]);
}
