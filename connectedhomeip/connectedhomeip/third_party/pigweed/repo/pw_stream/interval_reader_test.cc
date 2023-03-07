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

#include "pw_stream/interval_reader.h"

#include "gtest/gtest.h"
#include "pw_result/result.h"
#include "pw_stream/memory_stream.h"

namespace pw::stream {
namespace {

TEST(IntervalReader, IntervalReaderRead) {
  std::uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 9, 10};
  stream::MemoryReader reader(as_bytes(span(data)));
  IntervalReader reader_first_half(reader, 0, 5);
  IntervalReader reader_second_half(reader, 5, 10);

  // Read second half
  std::byte read_buf[5];
  Result<ByteSpan> res = reader_second_half.Read(read_buf);
  ASSERT_EQ(res.status(), OkStatus());
  ASSERT_EQ(res.value().size(), sizeof(read_buf));
  ASSERT_EQ(memcmp(read_buf, data + 5, 5), 0);
  ASSERT_EQ(reader_second_half.Read(read_buf).status(), Status::OutOfRange());

  // Read first half. They should be independent and do not interfere each
  // other.
  res = reader_first_half.Read(read_buf);
  ASSERT_EQ(res.status(), OkStatus());
  ASSERT_EQ(res.value().size(), sizeof(read_buf));
  ASSERT_EQ(memcmp(read_buf, data, 5), 0);
  ASSERT_EQ(reader_first_half.Read(read_buf).status(), Status::OutOfRange());

  // Reset the cursor and the reader should read from the beginning.
  res = reader_second_half.Reset().Read(read_buf);
  ASSERT_EQ(res.status(), OkStatus());
  ASSERT_EQ(res.value().size(), sizeof(read_buf));
  ASSERT_EQ(memcmp(read_buf, data + 5, 5), 0);
  ASSERT_EQ(reader_second_half.Read(read_buf).status(), Status::OutOfRange());
}

TEST(IntervalReader, IntervalReaderSeek) {
  std::uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 9, 10};
  stream::MemoryReader reader(as_bytes(span(data)));
  IntervalReader interval_reader(reader, 0, 10);

  // Absolute seeking.
  std::byte read_buf[5];
  ASSERT_EQ(interval_reader.Seek(5), OkStatus());
  Result<ByteSpan> res = interval_reader.Read(read_buf);
  ASSERT_EQ(res.status(), OkStatus());
  ASSERT_EQ(res.value().size(), sizeof(read_buf));
  ASSERT_EQ(memcmp(read_buf, data + 5, 5), 0);
  ASSERT_EQ(interval_reader.Read(read_buf).status(), Status::OutOfRange());

  // Relative seek.
  ASSERT_EQ(interval_reader.Seek(-10, stream::Stream::kCurrent), OkStatus());
  res = interval_reader.Read(read_buf);
  ASSERT_EQ(res.status(), OkStatus());
  ASSERT_EQ(res.value().size(), sizeof(read_buf));
  ASSERT_EQ(memcmp(read_buf, data, 5), 0);

  // Seeking from the end.
  ASSERT_EQ(interval_reader.Seek(-5, stream::Stream::kEnd), OkStatus());
  res = interval_reader.Read(read_buf);
  ASSERT_EQ(res.status(), OkStatus());
  ASSERT_EQ(res.value().size(), sizeof(read_buf));
  ASSERT_EQ(memcmp(read_buf, data + 5, 5), 0);
  ASSERT_EQ(interval_reader.Read(read_buf).status(), Status::OutOfRange());

  // Seeking to the end is allowed
  ASSERT_EQ(interval_reader.Seek(0, stream::Stream::kEnd), OkStatus());
  ASSERT_EQ(interval_reader.Read(read_buf).status(), Status::OutOfRange());
}

}  // namespace
}  // namespace pw::stream
