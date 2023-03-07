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

#include "pw_stream/memory_stream.h"

#include "gtest/gtest.h"
#include "pw_preprocessor/compiler.h"

namespace pw::stream {
namespace {

// Size of the in-memory buffer to use for this test.
constexpr size_t kSinkBufferSize = 1013;

struct TestStruct {
  uint8_t day;
  uint8_t month;
  uint16_t year;
};

constexpr TestStruct kExpectedStruct = {.day = 18, .month = 5, .year = 2020};

class MemoryWriterTest : public ::testing::Test {
 protected:
  MemoryWriterTest() : memory_buffer_{} {}
  std::array<std::byte, kSinkBufferSize> memory_buffer_;
};

TEST_F(MemoryWriterTest, BytesWritten) {
  MemoryWriter memory_writer(memory_buffer_);
  EXPECT_EQ(memory_writer.bytes_written(), 0u);
  Status status =
      memory_writer.Write(&kExpectedStruct, sizeof(kExpectedStruct));
  EXPECT_EQ(status, OkStatus());
  EXPECT_EQ(memory_writer.bytes_written(), sizeof(kExpectedStruct));
}

TEST_F(MemoryWriterTest, BytesWrittenOnConstruction) {
  constexpr size_t bytes_written = kSinkBufferSize / 2;
  std::memset(memory_buffer_.data(), 1u, bytes_written);
  MemoryWriter memory_writer(memory_buffer_, bytes_written);
  EXPECT_EQ(memory_writer.bytes_written(), bytes_written);
  EXPECT_EQ(memcmp(memory_writer.data(), memory_buffer_.data(), bytes_written),
            0);
}

TEST_F(MemoryWriterTest, ValidateContents) {
  MemoryWriter memory_writer(memory_buffer_);
  EXPECT_TRUE(
      memory_writer.Write(&kExpectedStruct, sizeof(kExpectedStruct)).ok());

  span<const std::byte> written_data = memory_writer.WrittenData();
  EXPECT_EQ(written_data.size_bytes(), sizeof(kExpectedStruct));
  TestStruct temp;
  std::memcpy(&temp, written_data.data(), written_data.size_bytes());
  EXPECT_EQ(memcmp(&temp, &kExpectedStruct, sizeof(kExpectedStruct)), 0);
}

TEST_F(MemoryWriterTest, MultipleWrites) {
  constexpr size_t kTempBufferSize = 72;
  std::byte buffer[kTempBufferSize] = {};

  for (std::byte& value : memory_buffer_) {
    value = std::byte(0);
  }
  MemoryWriter memory_writer(memory_buffer_);

  size_t counter = 0;
  while (memory_writer.ConservativeWriteLimit() >= kTempBufferSize) {
    for (size_t i = 0; i < sizeof(buffer); ++i) {
      buffer[i] = std::byte(counter++);
    }
    EXPECT_EQ(memory_writer.Write(span(buffer)), OkStatus());
  }

  EXPECT_GT(memory_writer.ConservativeWriteLimit(), 0u);
  EXPECT_LT(memory_writer.ConservativeWriteLimit(), kTempBufferSize);

  EXPECT_EQ(memory_writer.Write(span(buffer)), Status::ResourceExhausted());
  EXPECT_EQ(memory_writer.bytes_written(), counter);

  counter = 0;
  for (const std::byte& value : memory_writer) {
    EXPECT_EQ(value, std::byte(counter++));
  }
}

TEST_F(MemoryWriterTest, FullWriter) {
  constexpr size_t kTempBufferSize = 32;
  std::byte buffer[kTempBufferSize] = {};
  const int fill_byte = 0x25;
  memset(buffer, fill_byte, sizeof(buffer));

  for (std::byte& value : memory_buffer_) {
    value = std::byte(0);
  }
  MemoryWriter memory_writer(memory_buffer_);

  while (memory_writer.ConservativeWriteLimit() > 0) {
    size_t bytes_to_write =
        std::min(sizeof(buffer), memory_writer.ConservativeWriteLimit());
    EXPECT_EQ(memory_writer.Write(span(buffer, bytes_to_write)), OkStatus());
  }

  EXPECT_EQ(memory_writer.ConservativeWriteLimit(), 0u);

  EXPECT_EQ(memory_writer.Write(span(buffer)), Status::OutOfRange());
  EXPECT_EQ(memory_writer.bytes_written(), memory_buffer_.size());

  for (const std::byte& value : memory_writer) {
    EXPECT_EQ(value, std::byte(fill_byte));
  }
}

TEST_F(MemoryWriterTest, EmptyData) {
  std::byte buffer[5] = {};

  MemoryWriter memory_writer(memory_buffer_);
  EXPECT_EQ(memory_writer.Write(buffer, 0), OkStatus());
  EXPECT_EQ(memory_writer.bytes_written(), 0u);
}

TEST_F(MemoryWriterTest, ValidateContents_SingleByteWrites) {
  MemoryWriter memory_writer(memory_buffer_);
  EXPECT_TRUE(memory_writer.Write(std::byte{0x01}).ok());
  EXPECT_EQ(memory_writer.bytes_written(), 1u);
  EXPECT_EQ(memory_writer.data()[0], std::byte{0x01});

  EXPECT_TRUE(memory_writer.Write(std::byte{0x7E}).ok());
  EXPECT_EQ(memory_writer.bytes_written(), 2u);
  EXPECT_EQ(memory_writer.data()[1], std::byte{0x7E});
}

TEST_F(MemoryWriterTest, OverlappingBuffer) {
  constexpr std::string_view kTestString("This is staged into the same buffer");
  // Write at a five-byte offset from the start of the destination buffer.
  std::byte* const kOverlappingStart = memory_buffer_.data() + 5;
  std::memcpy(kOverlappingStart, kTestString.data(), kTestString.size());
  MemoryWriter memory_writer(memory_buffer_);
  EXPECT_TRUE(memory_writer.Write(kOverlappingStart, kTestString.size()).ok());
  EXPECT_TRUE(memory_writer.Write(std::byte(0)).ok());
  EXPECT_EQ(memory_writer.bytes_written(), kTestString.size() + 1);

  EXPECT_STREQ(reinterpret_cast<const char*>(memory_writer.data()),
               kTestString.data());
}

TEST_F(MemoryWriterTest, Clear) {
  MemoryWriter writer(memory_buffer_);
  EXPECT_EQ(OkStatus(), writer.Write(std::byte{1}));
  ASSERT_FALSE(writer.empty());
  writer.clear();
  EXPECT_TRUE(writer.empty());

  EXPECT_EQ(OkStatus(), writer.Write(std::byte{99}));
  EXPECT_EQ(writer[0], std::byte{99});
}

TEST_F(MemoryWriterTest, Seek_To0) {
  MemoryWriter writer(memory_buffer_);
  EXPECT_EQ(OkStatus(), writer.Seek(0));
}

TEST_F(MemoryWriterTest, Tell_StartsAt0) {
  MemoryWriter writer(memory_buffer_);
  EXPECT_EQ(0u, writer.Tell());
}

TEST_F(MemoryWriterTest, Tell_UpdatesOnSeek) {
  MemoryWriter writer(memory_buffer_);
  ASSERT_EQ(OkStatus(), writer.Seek(2, Stream::kCurrent));
  EXPECT_EQ(2u, writer.Tell());
}

TEST_F(MemoryWriterTest, Tell_UpdatesOnRead) {
  MemoryWriter writer(memory_buffer_);
  std::byte buffer[4] = {};
  ASSERT_EQ(OkStatus(), writer.Write(buffer));
  EXPECT_EQ(4u, writer.Tell());
}

#define TESTING_CHECK_FAILURES_IS_SUPPORTED 0
#if TESTING_CHECK_FAILURES_IS_SUPPORTED

// TODO(amontanez): Ensure that this test triggers an assert.
TEST_F(MemoryWriterTest, NullPointer) {
  MemoryWriter memory_writer(memory_buffer_);
  memory_writer.Write(nullptr, 21);
}

// TODO(davidrogers): Ensure that this test triggers an assert.
TEST(MemoryReader, NullSpan) {
  std::byte memory_buffer[32];
  ByteSpan dest(nullptr, 5);
  MemoryReader memory_reader(memory_buffer);
  memory_reader.Read(dest);
}

// TODO(davidrogers): Ensure that this test triggers an assert.
TEST(MemoryReader, NullPointer) {
  std::byte memory_buffer[32];
  MemoryReader memory_reader(memory_buffer);
  memory_reader.Read(nullptr, 21);
}

#endif  // TESTING_CHECK_FAILURES_IS_SUPPORTED

TEST(MemoryReader, SingleFullRead) {
  constexpr size_t kTempBufferSize = 32;

  std::array<std::byte, kTempBufferSize> source;
  std::array<std::byte, kTempBufferSize> dest;

  uint8_t counter = 0;
  for (std::byte& value : source) {
    value = std::byte(counter++);
  }

  MemoryReader memory_reader(source);

  // Read exactly the available bytes.
  EXPECT_EQ(memory_reader.ConservativeReadLimit(), dest.size());
  Result<ByteSpan> result = memory_reader.Read(dest);
  EXPECT_EQ(result.status(), OkStatus());
  EXPECT_EQ(result.value().size_bytes(), dest.size());

  ASSERT_EQ(source.size(), result.value().size_bytes());
  for (size_t i = 0; i < source.size(); i++) {
    EXPECT_EQ(source[i], result.value()[i]);
  }

  // Shoud be no byte remaining.
  EXPECT_EQ(memory_reader.ConservativeReadLimit(), 0u);
  result = memory_reader.Read(dest);
  EXPECT_EQ(result.status(), Status::OutOfRange());
}

TEST(MemoryReader, EmptySpanRead) {
  constexpr size_t kTempBufferSize = 32;
  std::array<std::byte, kTempBufferSize> source;

  // Use a span with nullptr and zero length;
  ByteSpan dest;
  EXPECT_EQ(dest.size_bytes(), 0u);

  MemoryReader memory_reader(source);

  // Read exactly the available bytes.
  Result<ByteSpan> result = memory_reader.Read(dest);
  EXPECT_EQ(result.status(), OkStatus());
  EXPECT_EQ(result.value().size_bytes(), 0u);
  EXPECT_EQ(result.value().data(), dest.data());

  // Shoud be original bytes remaining.
  EXPECT_EQ(memory_reader.ConservativeReadLimit(), source.size());
}

TEST(MemoryReader, SinglePartialRead) {
  constexpr size_t kTempBufferSize = 32;
  std::array<std::byte, kTempBufferSize> source;
  std::array<std::byte, kTempBufferSize * 2> dest;

  uint8_t counter = 0;
  for (std::byte& value : source) {
    value = std::byte(counter++);
  }

  MemoryReader memory_reader(source);

  // Try and read double the bytes available. Use the pointer/size version of
  // the API.
  Result<ByteSpan> result = memory_reader.Read(dest.data(), dest.size());
  EXPECT_EQ(result.status(), OkStatus());
  EXPECT_EQ(result.value().size_bytes(), source.size());

  ASSERT_EQ(source.size(), result.value().size_bytes());
  for (size_t i = 0; i < source.size(); i++) {
    EXPECT_EQ(source[i], result.value()[i]);
  }

  // Shoud be no byte remaining.
  EXPECT_EQ(memory_reader.ConservativeReadLimit(), 0u);
  result = memory_reader.Read(dest);
  EXPECT_EQ(result.status(), Status::OutOfRange());
}

TEST(MemoryReader, MultipleReads) {
  constexpr size_t kTempBufferSize = 32;

  std::array<std::byte, kTempBufferSize * 5> source;
  std::array<std::byte, kTempBufferSize> dest;

  uint8_t counter = 0;

  for (std::byte& value : source) {
    value = std::byte(counter++);
  }

  MemoryReader memory_reader(source);

  size_t source_chunk_base = 0;

  while (memory_reader.ConservativeReadLimit() > 0) {
    size_t read_limit = memory_reader.ConservativeReadLimit();

    // Try and read a chunk of bytes.
    Result<ByteSpan> result = memory_reader.Read(dest);
    EXPECT_EQ(result.status(), OkStatus());
    EXPECT_EQ(result.value().size_bytes(), dest.size());
    EXPECT_EQ(memory_reader.ConservativeReadLimit(),
              read_limit - result.value().size_bytes());

    // Verify the chunk of byte that was read.
    for (size_t i = 0; i < result.value().size_bytes(); i++) {
      EXPECT_EQ(source[source_chunk_base + i], result.value()[i]);
    }
    source_chunk_base += result.value().size_bytes();
  }
}

TEST(MemoryReader, Seek) {
  constexpr std::string_view data = "0123456789";
  MemoryReader reader(as_bytes(span(data)));

  char buffer[5] = {};  // Leave a null terminator at the end.
  ASSERT_EQ(OkStatus(), reader.Read(buffer, sizeof(buffer) - 1).status());
  EXPECT_STREQ(buffer, "0123");

  ASSERT_EQ(OkStatus(), reader.Seek(1));
  ASSERT_EQ(OkStatus(), reader.Read(buffer, sizeof(buffer) - 1).status());
  EXPECT_STREQ(buffer, "1234");

  ASSERT_EQ(OkStatus(), reader.Seek(0));
  ASSERT_EQ(OkStatus(), reader.Read(buffer, sizeof(buffer) - 1).status());
  EXPECT_STREQ(buffer, "0123");
}

TEST(MemoryReader, Tell_StartsAt0) {
  MemoryReader reader(as_bytes(span("\3\2\1")));
  EXPECT_EQ(0u, reader.Tell());
}

TEST(MemoryReader, Tell_UpdatesOnSeek) {
  MemoryReader reader(as_bytes(span("\3\2\1")));
  ASSERT_EQ(OkStatus(), reader.Seek(2, Stream::kCurrent));
  EXPECT_EQ(2u, reader.Tell());
}

TEST(MemoryReader, Tell_UpdatesOnRead) {
  MemoryReader reader(as_bytes(span("\3\2\1")));
  std::byte buffer[4];
  ASSERT_EQ(OkStatus(), reader.Read(buffer).status());
  EXPECT_EQ(4u, reader.Tell());
}

}  // namespace
}  // namespace pw::stream
