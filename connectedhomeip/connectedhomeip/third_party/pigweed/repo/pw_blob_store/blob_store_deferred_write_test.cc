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

class DeferredWriteTest : public ::testing::Test {
 protected:
  DeferredWriteTest() : flash_(kFlashAlignment), partition_(&flash_) {}

  void InitFlashToErased() { ASSERT_EQ(OkStatus(), partition_.Erase()); }

  void InitFlashToRandom(uint64_t seed) {
    random::XorShiftStarRng64 rng(seed);
    rng.Get(flash_.buffer());
  }

  void InitBufferToRandom(uint64_t seed) {
    random::XorShiftStarRng64 rng(seed);
    rng.Get(buffer_);
  }

  void InitBufferToFill(char fill) {
    ASSERT_EQ(OkStatus(), partition_.Erase());
    std::memset(buffer_.data(), fill, buffer_.size());
  }

  // Fill the source buffer with random pattern based on given seed, written to
  // BlobStore in specified chunk size.
  void ChunkWriteTest(size_t chunk_size,
                      size_t flush_interval,
                      bool explicit_discard = false,
                      bool explicit_erase = false) {
    constexpr size_t kWriteSize = 64;
    kvs::ChecksumCrc16 checksum;

    size_t bytes_since_flush = 0;

    char name[16] = {};
    snprintf(name, sizeof(name), "Blob%u", static_cast<unsigned>(chunk_size));

    BlobStoreBuffer<kBufferSize> blob(
        name, partition_, &checksum, kvs::TestKvs(), kWriteSize);
    EXPECT_EQ(OkStatus(), blob.Init());

    BlobStore::DeferredWriterWithBuffer writer(blob);
    EXPECT_EQ(OkStatus(), writer.Open());

    if (explicit_discard) {
      EXPECT_EQ(OkStatus(), writer.Discard());
    }

    if (explicit_erase) {
      EXPECT_EQ(OkStatus(), writer.Erase());
    }

    ByteSpan source = buffer_;
    while (source.size_bytes() > 0) {
      const size_t write_size = std::min(source.size_bytes(), chunk_size);

      PW_LOG_DEBUG("Do write of %u bytes, %u bytes remain",
                   static_cast<unsigned>(write_size),
                   static_cast<unsigned>(source.size_bytes()));

      ASSERT_EQ(OkStatus(), writer.Write(source.first(write_size)));
      // TODO(davidrogers): Add check that the write did not go to flash yet.

      source = source.subspan(write_size);
      bytes_since_flush += write_size;

      if (bytes_since_flush >= flush_interval) {
        bytes_since_flush = 0;
        ASSERT_EQ(OkStatus(), writer.Flush());
      }
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
    EXPECT_EQ(buffer_.size(), flash_.buffer().size_bytes());

    // Can't allow it to march off the end of buffer_.
    ASSERT_LE(verify_bytes.size_bytes(), buffer_.size());

    for (size_t i = 0; i < verify_bytes.size_bytes(); i++) {
      EXPECT_EQ(buffer_[i], verify_bytes[i]);
    }
  }

  static constexpr size_t kFlashAlignment = 16;
  static constexpr size_t kSectorSize = 1024;
  static constexpr size_t kSectorCount = 4;
  static constexpr size_t kBufferSize = 2 * kSectorSize;

  kvs::FakeFlashMemoryBuffer<kSectorSize, kSectorCount> flash_;
  kvs::FlashPartition partition_;
  std::array<std::byte, kSectorCount * kSectorSize> buffer_;
};

TEST_F(DeferredWriteTest, ChunkWrite1) {
  InitFlashToErased();
  InitBufferToRandom(0x8675309);
  ChunkWriteTest(1, 16);
}

TEST_F(DeferredWriteTest, ChunkWrite2) {
  InitFlashToRandom(0x2283);
  InitBufferToRandom(0x8675);
  ChunkWriteTest(2, 16);
}

TEST_F(DeferredWriteTest, ChunkWrite3) {
  InitFlashToErased();
  InitBufferToFill(0);
  ChunkWriteTest(3, 16);
}

TEST_F(DeferredWriteTest, ChunkWrite4) {
  InitFlashToErased();
  InitBufferToFill(1);
  ChunkWriteTest(4, 64);
}

TEST_F(DeferredWriteTest, ChunkWrite5) {
  InitFlashToErased();
  InitBufferToFill(0xff);
  ChunkWriteTest(5, 64);
}

TEST_F(DeferredWriteTest, ChunkWrite16) {
  InitFlashToErased();
  InitBufferToRandom(0x86);
  ChunkWriteTest(16, 128);
}

TEST_F(DeferredWriteTest, ChunkWrite64) {
  InitFlashToRandom(0x9223);
  InitBufferToRandom(0x9);
  ChunkWriteTest(64, 128);
}

TEST_F(DeferredWriteTest, ChunkWrite64FullBufferFill) {
  InitFlashToErased();
  InitBufferToRandom(0x9);
  ChunkWriteTest(64, kBufferSize);
}

TEST_F(DeferredWriteTest, ChunkWrite256) {
  InitFlashToErased();
  InitBufferToRandom(0x12345678);
  ChunkWriteTest(256, 256);
}

TEST_F(DeferredWriteTest, ChunkWriteDiscard16) {
  InitFlashToErased();
  InitBufferToRandom(0x86);

  // Test with a discard of an invalid blob and erased flash.
  ChunkWriteTest(16, 128, true);

  // Test with a discard of a valid blob.
  ChunkWriteTest(16, 128, true);

  // Test with a discard of an current blob with corrupted date.
  InitFlashToRandom(0x9223);
  ChunkWriteTest(16, 128, true);
}

TEST_F(DeferredWriteTest, ChunkWriteErase16) {
  InitFlashToErased();
  InitBufferToRandom(0x1286);

  // Test with an erase of an invalid blob and erased flash.
  ChunkWriteTest(16, 128, false, true);

  // Test with an erase of a valid blob.
  ChunkWriteTest(16, 128, false, true);

  // Test with an erase of an current blob with corrupted date.
  InitFlashToRandom(0x9223);
  ChunkWriteTest(16, 128, false, true);
}

}  // namespace
}  // namespace pw::blob_store
