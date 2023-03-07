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

#include <cstring>

#include "pw_bloat/bloat_this_binary.h"
#include "pw_bytes/span.h"
#include "pw_checksum/crc32.h"
#include "pw_hdlc/decoder.h"
#include "pw_hdlc/encoder.h"
#include "pw_log/log.h"
#include "pw_preprocessor/compiler.h"
#include "pw_result/result.h"
#include "pw_span/span.h"
#include "pw_stream/memory_stream.h"
#include "pw_varint/varint.h"

namespace pw::size_report {

PW_NO_INLINE
Result<int> ForceResultInBloat() { return 43; }

std::byte buffer[128];
std::byte* volatile get_buffer = buffer;
volatile unsigned get_size;

PW_NO_INLINE
ByteSpan GetBufferSpan() {
  if (ForceResultInBloat().ok()) {
    PW_LOG_INFO("Got result!");
  }
  // Trick the optimizer and also satisfy the type checker.
  get_size = sizeof(buffer);
  std::byte* local_buffer = get_buffer;
  unsigned local_size = get_size;
  return span(local_buffer, local_size);
}

#ifdef ENABLE_CRC
PW_NO_INLINE
unsigned RunChecksum() {
  // Trigger varint
  varint::Encode(55, GetBufferSpan());
  uint64_t decoded_number;
  size_t num_bytes = varint::Decode(GetBufferSpan(), &decoded_number);

  // Calculate the checksum and stash it in a volatile variable so the compiler
  // can't optimize it out.
  ::pw::checksum::Crc32 checksum;
  checksum.Update(GetBufferSpan());
  uint32_t value = static_cast<uint32_t>(checksum.value());
  value += num_bytes + static_cast<uint32_t>(decoded_number);
  *get_buffer = static_cast<std::byte>(value);
  return 0;
}
#endif

PW_NO_INLINE
void HdlcSizeReport() {
  // Create a buffer with some data to bloat the binary with stream and related.
  std::array<std::byte, 100> data;
  std::fill(data.begin(), data.end(), std::byte(0));
  stream::MemoryWriterBuffer<250> destination;
  destination.Write(data);
  if (destination.Write(GetBufferSpan()).ok()) {
    PW_LOG_INFO("Wrote successfully");
  }

#ifdef ENABLE_CRC
  RunChecksum();
#endif

#ifdef ENABLE_ENCODE
  hdlc::WriteUIFrame(123, data, destination);
#endif

#ifdef ENABLE_DECODE
  hdlc::Decoder decoder(data);
  Result<hdlc::Frame> frame = decoder.Process(std::byte('~'));
  // Force use of nodiscard frame.
  if (frame.ok()) {
    get_size++;
  }
  decoder.Clear();
#endif
}

}  // namespace pw::size_report

int main() {
  pw::bloat::BloatThisBinary();
  pw::size_report::HdlcSizeReport();
  return 0;
}
