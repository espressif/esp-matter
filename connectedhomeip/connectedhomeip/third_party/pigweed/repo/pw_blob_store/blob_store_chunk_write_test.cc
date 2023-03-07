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

#include <array>
#include <cstddef>
#include <cstring>

#include "gtest/gtest.h"
#include "pw_blob_store/blob_store.h"
#include "pw_kvs/crc16_checksum.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/test_key_value_store.h"
#include "pw_log/log.h"
#include "pw_random/xor_shift.h"
#include "pw_span/span.h"

namespace pw::blob_store {
namespace {

class BlobStoreChunkTest : public ::testing::Test {
 protected:
  BlobStoreChunkTest() : flash_(kFlashAlignment), partition_(&flash_) {}

  void InitFlashTo(span<const std::byte> contents) {
    ASSERT_EQ(OkStatus(), partition_.Erase());
    std::memcpy(flash_.buffer().data(), contents.data(), contents.size());
  }

  void InitSourceBufferToRandom(uint64_t seed) {
    ASSERT_EQ(OkStatus(), partition_.Erase());
    random::XorShiftStarRng64 rng(seed);
    rng.Get(source_buffer_);
  }

  void InitSourceBufferToFill(char fill) {
    ASSERT_EQ(OkStatus(), partition_.Erase());
    std::memset(source_buffer_.data(), fill, source_buffer_.size());
  }

  // Fill the source buffer with random pattern based on given seed, written to
  // BlobStore in specified chunk size.
  void ChunkWriteTest(size_t chunk_size) {
    constexpr size_t kBufferSize = 256;
    kvs::ChecksumCrc16 checksum;

    char name[16] = {};
    snprintf(name, sizeof(name), "Blob%u", static_cast<unsigned>(chunk_size));

    BlobStoreBuffer<kBufferSize> blob(
        name, partition_, &checksum, kvs::TestKvs(), kBufferSize);
    EXPECT_EQ(OkStatus(), blob.Init());

    BlobStore::BlobWriter writer(blob, metadata_buffer_);
    EXPECT_EQ(OkStatus(), writer.Open());
    EXPECT_EQ(OkStatus(), writer.Erase());

    ByteSpan source = source_buffer_;
    while (source.size_bytes() > 0) {
      const size_t write_size = std::min(source.size_bytes(), chunk_size);

      PW_LOG_DEBUG("Do write of %u bytes, %u bytes remain",
                   static_cast<unsigned>(write_size),
                   static_cast<unsigned>(source.size_bytes()));

      ASSERT_EQ(OkStatus(), writer.Write(source.first(write_size)));

      source = source.subspan(write_size);
    }

    EXPECT_EQ(OkStatus(), writer.Close());

    // Use reader to check for valid data.
    BlobStore::BlobReader reader(blob);
    ASSERT_EQ(OkStatus(), reader.Open());
    Result<ConstByteSpan> result = reader.GetMemoryMappedBlob();
    ASSERT_TRUE(result.ok());
    VerifyFlash(result.value());
    EXPECT_EQ(OkStatus(), reader.Close());
  }

  void VerifyFlash(ConstByteSpan verify_bytes) {
    // Should be defined as same size.
    EXPECT_EQ(source_buffer_.size(), flash_.buffer().size_bytes());

    // Can't allow it to march off the end of source_buffer_.
    ASSERT_LE(verify_bytes.size_bytes(), source_buffer_.size());

    for (size_t i = 0; i < verify_bytes.size_bytes(); i++) {
      EXPECT_EQ(source_buffer_[i], verify_bytes[i]);
    }
  }

  static constexpr size_t kFlashAlignment = 16;
  static constexpr size_t kSectorSize = 2048;
  static constexpr size_t kSectorCount = 2;
  static constexpr size_t kBlobDataSize = (kSectorCount * kSectorSize);
  static constexpr size_t kMetadataBufferSize =
      BlobStore::BlobWriter::RequiredMetadataBufferSize(0);

  kvs::FakeFlashMemoryBuffer<kSectorSize, kSectorCount> flash_;
  kvs::FlashPartition partition_;
  std::array<std::byte, kMetadataBufferSize> metadata_buffer_;
  std::array<std::byte, kBlobDataSize> source_buffer_;
};

TEST_F(BlobStoreChunkTest, ChunkWrite1) {
  InitSourceBufferToRandom(0x8675309);
  ChunkWriteTest(1);
}

TEST_F(BlobStoreChunkTest, ChunkWrite2) {
  InitSourceBufferToRandom(0x8675);
  ChunkWriteTest(2);
}

TEST_F(BlobStoreChunkTest, ChunkWrite3) {
  InitSourceBufferToFill(0);
  ChunkWriteTest(3);
}

TEST_F(BlobStoreChunkTest, ChunkWrite4) {
  InitSourceBufferToFill(1);
  ChunkWriteTest(4);
}

TEST_F(BlobStoreChunkTest, ChunkWrite5) {
  InitSourceBufferToFill(0xff);
  ChunkWriteTest(5);
}

TEST_F(BlobStoreChunkTest, ChunkWrite16) {
  InitSourceBufferToRandom(0x86);
  ChunkWriteTest(16);
}

TEST_F(BlobStoreChunkTest, ChunkWrite64) {
  InitSourceBufferToRandom(0x9);
  ChunkWriteTest(64);
}

TEST_F(BlobStoreChunkTest, ChunkWrite256) {
  InitSourceBufferToRandom(0x12345678);
  ChunkWriteTest(256);
}

TEST_F(BlobStoreChunkTest, ChunkWrite512) {
  InitSourceBufferToRandom(0x42);
  ChunkWriteTest(512);
}

TEST_F(BlobStoreChunkTest, ChunkWrite4096) {
  InitSourceBufferToRandom(0x89);
  ChunkWriteTest(4096);
}

TEST_F(BlobStoreChunkTest, ChunkWriteSingleFull) {
  InitSourceBufferToRandom(0x98765);
  ChunkWriteTest(kBlobDataSize);
}

}  // namespace
}  // namespace pw::blob_store
