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

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>

#include "gtest/gtest.h"
#include "public/pw_kvs/flash_memory.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs_private/config.h"
#include "pw_log/log.h"
#include "pw_random/xor_shift.h"
#include "pw_span/span.h"

#if PW_CXX_STANDARD_IS_SUPPORTED(17)

#ifndef PW_FLASH_TEST_ALIGNMENT
#define PW_FLASH_TEST_ALIGNMENT 1
#endif

namespace pw::kvs {
namespace {

class FlashStreamTest : public ::testing::Test {
 protected:
  FlashStreamTest() : flash_(kFlashAlignment), partition_(&flash_) {}

  void InitBufferToFill(ByteSpan buffer_span, char fill) {
    std::memset(buffer_span.data(), fill, buffer_span.size_bytes());
  }

  void InitBufferToRandom(ByteSpan buffer_span, uint64_t seed) {
    random::XorShiftStarRng64 rng(seed);

    std::memset(buffer_span.data(),
                static_cast<int>(flash_.erased_memory_content()),
                buffer_span.size());
    rng.Get(buffer_span);
  }

  void VerifyFlash(ConstByteSpan verify_bytes, size_t offset = 0) {
    // Should be defined as same size.
    EXPECT_EQ(source_buffer_.size(), flash_.buffer().size_bytes());

    // Can't allow it to march off the end of source_buffer_.
    ASSERT_LE((verify_bytes.size_bytes() + offset), source_buffer_.size());

    for (size_t i = 0; i < verify_bytes.size_bytes(); i++) {
      ASSERT_EQ(source_buffer_[i + offset], verify_bytes[i]);
    }
  }

  void VerifyFlashContent(ConstByteSpan verify_bytes, size_t offset = 0) {
    // Can't allow it to march off the end of source_buffer_.
    ASSERT_LE((verify_bytes.size_bytes() + offset),
              flash_.buffer().size_bytes());

    for (size_t i = 0; i < verify_bytes.size_bytes(); i++) {
      ASSERT_EQ(flash_.buffer()[i + offset], verify_bytes[i]);
    }
  }

  void DoWriteInChunks(size_t chunk_write_size_bytes, uint64_t seed) {
    InitBufferToRandom(span(source_buffer_), seed);
    ConstByteSpan write_data = span(source_buffer_);

    ASSERT_EQ(OkStatus(), partition_.Erase());

    FlashPartition::Writer writer(partition_);

    while (write_data.size_bytes() > 0) {
      size_t offset_before_write = writer.Tell();
      size_t write_chunk_size =
          std::min(chunk_write_size_bytes, write_data.size_bytes());

      ConstByteSpan write_chunk = write_data.first(write_chunk_size);
      ASSERT_EQ(OkStatus(), writer.Write(write_chunk));
      VerifyFlashContent(write_chunk, offset_before_write);

      write_data = write_data.subspan(write_chunk_size);

      ASSERT_EQ(writer.ConservativeWriteLimit(), write_data.size_bytes());
    }

    VerifyFlashContent(span(source_buffer_));
  }

  void DoReadInChunks(size_t chunk_read_size_bytes,
                      uint64_t seed,
                      size_t start_offset,
                      size_t bytes_to_read) {
    InitBufferToRandom(flash_.buffer(), seed);

    ASSERT_LE((start_offset + bytes_to_read), flash_.buffer().size_bytes());

    FlashPartition::Reader reader(partition_);
    ASSERT_EQ(reader.ConservativeReadLimit(), flash_.buffer().size_bytes());

    ASSERT_EQ(reader.Seek(start_offset), OkStatus());
    ASSERT_EQ(reader.ConservativeReadLimit(),
              flash_.buffer().size_bytes() - start_offset);

    while (bytes_to_read > 0) {
      ASSERT_EQ(start_offset, reader.Tell());

      size_t chunk_size = std::min(chunk_read_size_bytes, bytes_to_read);

      ByteSpan read_chunk = span(source_buffer_).first(chunk_size);
      InitBufferToFill(read_chunk, 0);
      ASSERT_EQ(read_chunk.size_bytes(), chunk_size);

      auto result = reader.Read(read_chunk);
      ASSERT_EQ(result.status(), OkStatus());
      ASSERT_EQ(result.value().size_bytes(), chunk_size);
      VerifyFlashContent(read_chunk, start_offset);

      start_offset += chunk_size;
      bytes_to_read -= chunk_size;

      ASSERT_EQ(reader.ConservativeReadLimit(),
                flash_.buffer().size_bytes() - start_offset);
    }
  }

  static constexpr size_t kFlashAlignment = PW_FLASH_TEST_ALIGNMENT;
  static constexpr size_t kSectorSize = 2048;
  static constexpr size_t kSectorCount = 2;
  static constexpr size_t kFPDataSize = (kSectorCount * kSectorSize);

  FakeFlashMemoryBuffer<kSectorSize, kSectorCount> flash_;
  FlashPartition partition_;
  std::array<std::byte, kFPDataSize> source_buffer_;
  size_t size_bytes_;
};

TEST_F(FlashStreamTest, Write_1_Byte_Chunks) {
  // Write in 1 byte chunks.
  DoWriteInChunks(1, 0xab1234);
}

TEST_F(FlashStreamTest, Write_5_Byte_Chunks) {
  // Write in 5 byte chunks.
  DoWriteInChunks(5, 0xdc2274);
}

TEST_F(FlashStreamTest, Write_16_Byte_Chunks) {
  // Write in 16 byte chunks.
  DoWriteInChunks(16, 0xef8224);
}

TEST_F(FlashStreamTest, Write_255_Byte_Chunks) {
  // Write in 255 byte chunks.
  DoWriteInChunks(255, 0xffe1348);
}

TEST_F(FlashStreamTest, Write_256_Byte_Chunks) {
  // Write in 256 byte chunks.
  DoWriteInChunks(256, 0xe11234);
}

TEST_F(FlashStreamTest, Read_1_Byte_Chunks) {
  // Read in 1 byte chunks.
  DoReadInChunks(1, 0x7643ff, 0, flash_.buffer().size_bytes());
}

TEST_F(FlashStreamTest, Read_16_Byte_Chunks) {
  // Read in 16 byte chunks.
  DoReadInChunks(16, 0x61e234, 0, flash_.buffer().size_bytes());
}

TEST_F(FlashStreamTest, Read_255_Byte_Chunks) {
  // Read in 256 byte chunks.
  DoReadInChunks(255, 0xe13514, 0, flash_.buffer().size_bytes());
}

TEST_F(FlashStreamTest, Read_256_Byte_Chunks) {
  // Read in 256 byte chunks.
  DoReadInChunks(256, 0xe11234, 0, flash_.buffer().size_bytes());
}

TEST_F(FlashStreamTest, Read_256_Byte_Chunks_With_Offset) {
  // Read in 256 byte chunks.
  DoReadInChunks(256, 0xfffe34, 1024, (flash_.buffer().size_bytes() - 1024));
}

TEST_F(FlashStreamTest, Read_Multiple_Seeks) {
  static const size_t kSeekReadSizeBytes = 512;
  static const size_t kSeekReadIterations = 4;
  ASSERT_GE(flash_.buffer().size_bytes(),
            (kSeekReadIterations * (2 * kSeekReadSizeBytes)));

  InitBufferToRandom(flash_.buffer(), 0xffde176);
  FlashPartition::Reader reader(partition_);

  for (size_t i = 0; i < kSeekReadIterations; i++) {
    size_t start_offset = kSeekReadSizeBytes + (i * 2 * kSeekReadSizeBytes);
    ASSERT_EQ(reader.Seek(start_offset), OkStatus());
    ASSERT_EQ(start_offset, reader.Tell());

    ByteSpan read_chunk = span(source_buffer_).first(kSeekReadSizeBytes);
    InitBufferToFill(read_chunk, 0);

    auto result = reader.Read(read_chunk);
    ASSERT_EQ(result.status(), OkStatus());
    ASSERT_EQ(result.value().size_bytes(), kSeekReadSizeBytes);
    VerifyFlashContent(read_chunk, start_offset);
    ASSERT_EQ(start_offset + kSeekReadSizeBytes, reader.Tell());
  }
}

TEST_F(FlashStreamTest, Read_Seek_Forward_and_Back) {
  static const size_t kSeekReadSizeBytes = 256;
  static const size_t kTotalIterations = 3;
  static const size_t kSeekReadIterations =
      flash_.buffer().size_bytes() / (2 * kSeekReadSizeBytes);

  InitBufferToRandom(flash_.buffer(), 0xffde176);
  FlashPartition::Reader reader(partition_);

  for (size_t outer_count = 0; outer_count < kTotalIterations; outer_count++) {
    // Seek and read going forward.
    for (size_t i = 0; i < kSeekReadIterations; i++) {
      size_t start_offset = kSeekReadSizeBytes + (i * 2 * kSeekReadSizeBytes);
      ASSERT_EQ(reader.Seek(start_offset), OkStatus());
      ASSERT_EQ(start_offset, reader.Tell());

      ByteSpan read_chunk = span(source_buffer_).first(kSeekReadSizeBytes);
      InitBufferToFill(read_chunk, 0);

      auto result = reader.Read(read_chunk);
      ASSERT_EQ(result.status(), OkStatus());
      ASSERT_EQ(result.value().size_bytes(), kSeekReadSizeBytes);
      VerifyFlashContent(read_chunk, start_offset);
      ASSERT_EQ(start_offset + kSeekReadSizeBytes, reader.Tell());
    }

    // Seek and read going backward.
    for (size_t j = (kSeekReadIterations * 2); j > 0; j--) {
      size_t start_offset = (j - 1) * kSeekReadSizeBytes;
      ASSERT_EQ(reader.Seek(start_offset), OkStatus());
      ASSERT_EQ(start_offset, reader.Tell());
      ASSERT_GE(reader.ConservativeReadLimit(), kSeekReadSizeBytes);

      ByteSpan read_chunk = span(source_buffer_).first(kSeekReadSizeBytes);
      InitBufferToFill(read_chunk, 0);

      auto result = reader.Read(read_chunk);
      ASSERT_EQ(result.status(), OkStatus());
      ASSERT_EQ(result.value().size_bytes(), kSeekReadSizeBytes);
      VerifyFlashContent(read_chunk, start_offset);
      ASSERT_EQ(start_offset + kSeekReadSizeBytes, reader.Tell());
    }
  }
}

TEST_F(FlashStreamTest, Read_Past_End) {
  InitBufferToRandom(flash_.buffer(), 0xcccde176);
  FlashPartition::Reader reader(partition_);

  static const size_t kBytesForFinalRead = 50;

  ByteSpan read_chunk =
      span(source_buffer_).first(source_buffer_.size() - kBytesForFinalRead);

  auto result = reader.Read(read_chunk);
  ASSERT_EQ(result.status(), OkStatus());
  ASSERT_EQ(result.value().size_bytes(), read_chunk.size_bytes());
  ASSERT_EQ(reader.Tell(), read_chunk.size_bytes());
  ASSERT_EQ(reader.ConservativeReadLimit(), kBytesForFinalRead);
  ASSERT_EQ(result.value().data(), read_chunk.data());
  VerifyFlashContent(read_chunk);

  result = reader.Read(read_chunk);
  ASSERT_EQ(result.status(), OkStatus());
  ASSERT_EQ(result.value().size_bytes(), kBytesForFinalRead);
  ASSERT_EQ(reader.Tell(), flash_.buffer().size_bytes());
  ASSERT_EQ(reader.ConservativeReadLimit(), 0U);
  ASSERT_EQ(result.value().data(), read_chunk.data());
  VerifyFlashContent(result.value(), read_chunk.size_bytes());
}

TEST_F(FlashStreamTest, Read_Past_End_After_Seek) {
  InitBufferToRandom(flash_.buffer(), 0xddcde176);
  FlashPartition::Reader reader(partition_);

  static const size_t kBytesForFinalRead = 50;
  size_t start_offset = flash_.buffer().size_bytes() - kBytesForFinalRead;
  ASSERT_EQ(reader.Seek(start_offset), OkStatus());

  ASSERT_EQ(start_offset, reader.Tell());
  ASSERT_EQ(reader.ConservativeReadLimit(), kBytesForFinalRead);

  ByteSpan read_chunk = span(source_buffer_);
  auto result = reader.Read(read_chunk);
  ASSERT_EQ(result.status(), OkStatus());
  ASSERT_EQ(result.value().size_bytes(), kBytesForFinalRead);
  ASSERT_EQ(reader.Tell(), flash_.buffer().size_bytes());
  ASSERT_EQ(reader.ConservativeReadLimit(), 0U);
  ASSERT_EQ(result.value().data(), read_chunk.data());
  VerifyFlashContent(result.value(), start_offset);
}

TEST_F(FlashStreamTest, Read_Out_Of_Range) {
  InitBufferToRandom(flash_.buffer(), 0x531de176);
  FlashPartition::Reader reader(partition_);

  ByteSpan read_chunk = span(source_buffer_);

  auto result = reader.Read(read_chunk);
  ASSERT_EQ(result.status(), OkStatus());
  ASSERT_EQ(result.value().size_bytes(), read_chunk.size_bytes());
  ASSERT_EQ(reader.Tell(), read_chunk.size_bytes());
  ASSERT_EQ(reader.ConservativeReadLimit(), 0U);
  ASSERT_EQ(result.value().data(), read_chunk.data());
  VerifyFlashContent(read_chunk);

  result = reader.Read(read_chunk);
  ASSERT_EQ(result.status(), Status::OutOfRange());
  ASSERT_EQ(reader.Tell(), flash_.buffer().size_bytes());
  ASSERT_EQ(reader.ConservativeReadLimit(), 0U);
}

TEST_F(FlashStreamTest, Read_Out_Of_Range_After_Seek) {
  InitBufferToRandom(flash_.buffer(), 0x8c94566);
  FlashPartition::Reader reader(partition_);

  ByteSpan read_chunk = span(source_buffer_);

  ASSERT_EQ(reader.Seek(flash_.buffer().size_bytes()), OkStatus());
  ASSERT_EQ(reader.Tell(), flash_.buffer().size_bytes());
  ASSERT_EQ(reader.ConservativeReadLimit(), 0U);

  auto result = reader.Read(read_chunk);
  ASSERT_EQ(result.status(), Status::OutOfRange());
  ASSERT_EQ(reader.Tell(), flash_.buffer().size_bytes());
  ASSERT_EQ(reader.ConservativeReadLimit(), 0U);
}

TEST_F(FlashStreamTest, Reader_Seek_Ops) {
  size_t kPartitionSizeBytes = flash_.buffer().size_bytes();
  FlashPartition::Reader reader(partition_);

  // Seek from 0 to past end.
  ASSERT_EQ(reader.Seek(kPartitionSizeBytes + 5), Status::OutOfRange());
  ASSERT_EQ(reader.Tell(), 0U);

  // Seek to end then seek again going past end.
  ASSERT_EQ(reader.Seek(0), OkStatus());
  ASSERT_EQ(reader.Tell(), 0U);
  ASSERT_EQ(reader.ConservativeReadLimit(), kPartitionSizeBytes);

  ASSERT_EQ(reader.Seek(kPartitionSizeBytes,
                        FlashPartition::Reader::Whence::kCurrent),
            OkStatus());
  ASSERT_EQ(reader.Tell(), kPartitionSizeBytes);
  ASSERT_EQ(reader.ConservativeReadLimit(), 0U);

  ASSERT_EQ(reader.Seek(5, FlashPartition::Reader::Whence::kCurrent),
            Status::OutOfRange());
  ASSERT_EQ(reader.Tell(), kPartitionSizeBytes);
  ASSERT_EQ(reader.ConservativeReadLimit(), 0U);

  // Seek to beginning then seek backwards going past start.
  ASSERT_EQ(reader.Seek(0), OkStatus());
  ASSERT_EQ(reader.Seek(-5, FlashPartition::Reader::Whence::kCurrent),
            Status::OutOfRange());
  ASSERT_EQ(reader.Tell(), 0U);
  ASSERT_EQ(reader.ConservativeReadLimit(), kPartitionSizeBytes);
}

TEST_F(FlashStreamTest, Invald_Ops) {
  FlashPartition::Reader reader(partition_);
  ASSERT_EQ(reader.ConservativeWriteLimit(), 0U);

  FlashPartition::Writer writer(partition_);
  ASSERT_EQ(writer.ConservativeReadLimit(), 0U);
}

}  // namespace
}  // namespace pw::kvs

#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)
