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

#include "pw_bloat/bloat_this_binary.h"
#include "pw_ring_buffer/prefixed_entry_ring_buffer.h"
#include "pw_status/status.h"

constexpr size_t kRingBufferSize = 1024;
constexpr size_t kReaderCount = 4;
constexpr std::byte kValue = (std::byte)0xFF;
constexpr std::byte kData[1] = {kValue};

int main() {
  pw::bloat::BloatThisBinary();

  pw::ring_buffer::PrefixedEntryRingBufferMulti ring(true /* user_preamble */);
  std::byte buffer[kRingBufferSize];

  pw::Status status = ring.SetBuffer(buffer);
  if (!status.ok()) {
    return 1;
  }

  // Attach readers.
  pw::ring_buffer::PrefixedEntryRingBufferMulti::Reader readers[kReaderCount];
  for (auto& reader : readers) {
    ring.AttachReader(reader)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }

  // Push entries until the buffer is full.
  size_t total_entries = 0;
  while (true) {
    status = ring.TryPushBack(kData);
    if (status == pw::Status::ResourceExhausted()) {
      break;
    } else if (!status.ok()) {
      return 2;
    }
    total_entries++;
  }

  // Forcefully push an entry.
  status = ring.PushBack(kData);
  if (!status.ok()) {
    return 3;
  }

  // Dering the buffer.
  status = ring.Dering();
  if (!status.ok()) {
    return 4;
  }

  // Peek and pop all entries.
  __attribute__((unused)) std::byte value[1];
  __attribute__((unused)) size_t value_size;
  for (size_t i = 0; i < total_entries; ++i) {
    for (auto& reader : readers) {
      status = reader.PeekFront(value, &value_size);
      if (!status.ok()) {
        return 5;
      }
      status = reader.PeekFrontWithPreamble(value, &value_size);
      if (!status.ok()) {
        return 6;
      }
      if (reader.FrontEntryDataSizeBytes() == 0) {
        return 7;
      }
      if (reader.FrontEntryTotalSizeBytes() == 0) {
        return 8;
      }
      if (reader.EntryCount() == 0) {
        return 9;
      }
      status = reader.PopFront();
      if (!status.ok()) {
        return 10;
      }
    }
  }

  for (auto& reader : readers) {
    ring.DetachReader(reader)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }
  ring.Clear();
  return 0;
}
