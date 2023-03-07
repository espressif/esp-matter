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

#define PW_TRACE_MODULE_NAME "TST"

#include "pw_trace_tokenized/trace_buffer.h"

#include "gtest/gtest.h"
#include "pw_trace/trace.h"

TEST(TokenizedTrace, Simple) {
  PW_TRACE_SET_ENABLED(true);
  pw::trace::ClearBuffer();

  PW_TRACE_INSTANT("Test");

  // Check results
  pw::ring_buffer::PrefixedEntryRingBuffer* buf = pw::trace::GetBuffer();

  EXPECT_EQ(buf->EntryCount(), 1u);
  const size_t expected_min_bytes_used =
      1 /*varint size */ + 4 /*token*/ + 1 /* min varint time */;
  const size_t expected_max_bytes_used = 1 /*varint size */ + 4 /*token*/ +
                                         sizeof(PW_TRACE_TIME_TYPE) +
                                         1 /* max varint time */;

  EXPECT_GE(buf->TotalUsedBytes(), expected_min_bytes_used);
  EXPECT_LE(buf->TotalUsedBytes(), expected_max_bytes_used);
}

TEST(TokenizedTrace, TraceId) {
  PW_TRACE_SET_ENABLED(true);
  pw::trace::ClearBuffer();

  uint32_t trace_id = 256;
  PW_TRACE_INSTANT("Test", "group", trace_id);

  // Check results
  pw::ring_buffer::PrefixedEntryRingBuffer* buf = pw::trace::GetBuffer();

  EXPECT_EQ(buf->EntryCount(), 1u);
  const size_t expected_min_bytes_used = 1 /*varint size */ + 4 /*token*/ +
                                         1 /* min varint time */ +
                                         2 /*varint trace id*/;
  const size_t expected_max_bytes_used =
      1 /*varint size */ + 4 /*token*/ + sizeof(PW_TRACE_TIME_TYPE) +
      1 /* max varint time */ + 2 /*varint trace id*/;

  EXPECT_GE(buf->TotalUsedBytes(), expected_min_bytes_used);
  EXPECT_LE(buf->TotalUsedBytes(), expected_max_bytes_used);
}

TEST(TokenizedTrace, Data) {
  PW_TRACE_SET_ENABLED(true);
  pw::trace::ClearBuffer();

  char data[] = "some test data";
  PW_TRACE_INSTANT_DATA("Test", "test_data", data, sizeof(data));

  // Check results
  pw::ring_buffer::PrefixedEntryRingBuffer* buf = pw::trace::GetBuffer();

  EXPECT_EQ(buf->EntryCount(), 1u);
  const size_t expected_min_bytes_used =
      1 /*varint size */ + 4 /*token*/ + 1 /* min varint time */ + sizeof(data);
  const size_t expected_max_bytes_used = 1 /*varint size */ + 4 /*token*/ +
                                         sizeof(PW_TRACE_TIME_TYPE) +
                                         1 /* max varint time */ + sizeof(data);

  EXPECT_GE(buf->TotalUsedBytes(), expected_min_bytes_used);
  EXPECT_LE(buf->TotalUsedBytes(), expected_max_bytes_used);

  std::byte value[expected_max_bytes_used];
  size_t bytes_read = 0;
  EXPECT_EQ(buf->PeekFront(pw::span<std::byte>(value), &bytes_read),
            pw::OkStatus());

  // read size is minus 1, since doesn't include varint size
  EXPECT_GE(bytes_read, expected_min_bytes_used - 1);
  EXPECT_LE(bytes_read, expected_max_bytes_used - 1);
  EXPECT_STREQ(data,
               reinterpret_cast<char*>(&value[bytes_read - sizeof(data)]));
}

TEST(TokenizedTrace, Clear) {
  PW_TRACE_SET_ENABLED(true);
  pw::trace::ClearBuffer();
  pw::ring_buffer::PrefixedEntryRingBuffer* buf = pw::trace::GetBuffer();

  PW_TRACE_INSTANT("Test");
  EXPECT_EQ(buf->EntryCount(), 1u);
  pw::trace::ClearBuffer();
  EXPECT_EQ(buf->EntryCount(), 0u);
}

TEST(TokenizedTrace, Overflow) {
  PW_TRACE_SET_ENABLED(true);
  pw::trace::ClearBuffer();
  pw::ring_buffer::PrefixedEntryRingBuffer* buf = pw::trace::GetBuffer();

  // Add samples until entry count stops increasing.
  uint32_t last_entry_count = 0;
  size_t count = 0;
  while (buf->EntryCount() == 0 || buf->EntryCount() > last_entry_count) {
    last_entry_count = buf->EntryCount();
    PW_TRACE_INSTANT_DATA("Test", "count", &count, sizeof(count));
    count++;
  }
  // CHECK
  size_t expected_count = 1;  // skip first (since it was lost)
  while (buf->EntryCount() > 0) {
    std::byte value[PW_TRACE_BUFFER_MAX_BLOCK_SIZE_BYTES];
    size_t bytes_read = 0;
    EXPECT_EQ(buf->PeekFront(pw::span<std::byte>(value), &bytes_read),
              pw::OkStatus());
    EXPECT_EQ(buf->PopFront(), pw::OkStatus());
    size_t entry_count;
    memcpy(
        &entry_count, &value[bytes_read - sizeof(size_t)], sizeof(entry_count));
    EXPECT_EQ(entry_count, expected_count);
    expected_count++;
  }

  EXPECT_EQ(count, expected_count);
}

TEST(TokenizedTrace, BlockTooLarge) {
  PW_TRACE_SET_ENABLED(true);
  pw::trace::ClearBuffer();
  pw::ring_buffer::PrefixedEntryRingBuffer* buf = pw::trace::GetBuffer();
  uint8_t data[PW_TRACE_BUFFER_MAX_BLOCK_SIZE_BYTES];
  PW_TRACE_INSTANT_DATA("Test", "data", data, sizeof(data));
  EXPECT_EQ(buf->EntryCount(), 0u);
}

TEST(TokenizedTrace, DeringAndViewRawBuffer) {
  PW_TRACE_SET_ENABLED(true);
  pw::trace::ClearBuffer();

  // Should be empty span
  pw::ConstByteSpan buf = pw::trace::DeringAndViewRawBuffer();
  EXPECT_EQ(buf.size(), 0u);

  // Should now have data
  PW_TRACE_INSTANT("Test");
  buf = pw::trace::DeringAndViewRawBuffer();
  EXPECT_GT(buf.size(), 0u);

  // Should now have more data
  PW_TRACE_INSTANT("Test");
  size_t size_start = buf.size();
  buf = pw::trace::DeringAndViewRawBuffer();
  EXPECT_GT(buf.size(), size_start);
}
