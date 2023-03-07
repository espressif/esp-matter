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

#include "pw_blob_store/blob_store.h"

#include <array>
#include <cstddef>
#include <cstring>

#include "gtest/gtest.h"
#include "pw_kvs/crc16_checksum.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/test_key_value_store.h"
#include "pw_log/log.h"
#include "pw_random/xor_shift.h"
#include "pw_span/span.h"

#ifndef PW_FLASH_TEST_ALIGNMENT
#define PW_FLASH_TEST_ALIGNMENT 1
#endif

namespace pw::blob_store {
namespace {

class BlobStoreTest : public ::testing::Test {
 protected:
  static constexpr char kBlobTitle[] = "TestBlobBlock";

  BlobStoreTest() : flash_(kFlashAlignment), partition_(&flash_) {}

  void InitFlashTo(span<const std::byte> contents) {
    ASSERT_EQ(OkStatus(), partition_.Erase());
    std::memcpy(flash_.buffer().data(), contents.data(), contents.size());
  }

  void InitSourceBufferToRandom(uint64_t seed,
                                size_t init_size_bytes = kBlobDataSize) {
    ASSERT_LE(init_size_bytes, source_buffer_.size());
    random::XorShiftStarRng64 rng(seed);

    std::memset(source_buffer_.data(),
                static_cast<int>(flash_.erased_memory_content()),
                source_buffer_.size());
    rng.Get(span(source_buffer_).first(init_size_bytes));
  }

  void InitSourceBufferToFill(char fill,
                              size_t fill_size_bytes = kBlobDataSize) {
    ASSERT_LE(fill_size_bytes, source_buffer_.size());
    std::memset(source_buffer_.data(),
                static_cast<int>(flash_.erased_memory_content()),
                source_buffer_.size());
    std::memset(source_buffer_.data(), fill, fill_size_bytes);
  }

  // Fill the source buffer with random pattern based on given seed, written to
  // BlobStore in specified chunk size.
  void WriteTestBlock(size_t write_size_bytes = kBlobDataSize) {
    ASSERT_LE(write_size_bytes, source_buffer_.size());
    constexpr size_t kBufferSize = 256;
    kvs::ChecksumCrc16 checksum;

    ConstByteSpan write_data = span(source_buffer_).first(write_size_bytes);

    BlobStoreBuffer<kBufferSize> blob(
        kBlobTitle, partition_, &checksum, kvs::TestKvs(), kBufferSize);
    EXPECT_EQ(OkStatus(), blob.Init());

    BlobStore::BlobWriterWithBuffer writer(blob);
    EXPECT_EQ(OkStatus(), writer.Open());
    ASSERT_EQ(OkStatus(), writer.Write(write_data));
    EXPECT_EQ(OkStatus(), writer.Close());

    // Use reader to check for valid data.
    BlobStore::BlobReader reader(blob);
    ASSERT_EQ(OkStatus(), reader.Open());
    Result<ConstByteSpan> result = reader.GetMemoryMappedBlob();
    ASSERT_TRUE(result.ok());
    EXPECT_EQ(write_size_bytes, result.value().size_bytes());
    VerifyFlash(result.value().first(write_size_bytes));
    VerifyFlash(flash_.buffer().first(write_size_bytes));
    EXPECT_EQ(OkStatus(), reader.Close());
  }

  // Open a new blob instance and read the blob using the given read chunk size.
  void ChunkReadTest(size_t read_chunk_size) {
    kvs::ChecksumCrc16 checksum;

    VerifyFlash(flash_.buffer());

    constexpr size_t kBufferSize = 16;
    BlobStoreBuffer<kBufferSize> blob(
        kBlobTitle, partition_, &checksum, kvs::TestKvs(), kBufferSize);
    EXPECT_EQ(OkStatus(), blob.Init());

    // Use reader to check for valid data.
    BlobStore::BlobReader reader1(blob);
    ASSERT_EQ(OkStatus(), reader1.Open());
    Result<ConstByteSpan> possible_blob = reader1.GetMemoryMappedBlob();
    ASSERT_TRUE(possible_blob.ok());
    VerifyFlash(possible_blob.value());
    EXPECT_EQ(OkStatus(), reader1.Close());

    BlobStore::BlobReader reader(blob);
    ASSERT_EQ(OkStatus(), reader.Open());

    std::array<std::byte, kBlobDataSize> read_buffer;

    ByteSpan read_span = read_buffer;
    while (read_span.size_bytes() > 0) {
      size_t read_size = std::min(read_span.size_bytes(), read_chunk_size);

      PW_LOG_DEBUG("Do write of %u bytes, %u bytes remain",
                   static_cast<unsigned>(read_size),
                   static_cast<unsigned>(read_span.size_bytes()));

      ASSERT_EQ(read_span.size_bytes(), reader.ConservativeReadLimit());
      auto result = reader.Read(read_span.first(read_size));
      ASSERT_EQ(result.status(), OkStatus());
      read_span = read_span.subspan(read_size);
    }
    EXPECT_EQ(OkStatus(), reader.Close());

    VerifyFlash(read_buffer);
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

  static constexpr size_t kFlashAlignment = PW_FLASH_TEST_ALIGNMENT;
  static constexpr size_t kSectorSize = 2048;
  static constexpr size_t kSectorCount = 2;
  static constexpr size_t kBlobDataSize = (kSectorCount * kSectorSize);

  kvs::FakeFlashMemoryBuffer<kSectorSize, kSectorCount> flash_;
  kvs::FlashPartition partition_;
  std::array<std::byte, kBlobDataSize> source_buffer_;
};

TEST_F(BlobStoreTest, Init_Ok) {
  // TODO(davidrogers): Do init test with flash/kvs explicitly in the different
  // possible entry states.
  constexpr size_t kBufferSize = 256;
  BlobStoreBuffer<kBufferSize> blob(
      "Blob_OK", partition_, nullptr, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());
}

TEST_F(BlobStoreTest, Writer_ConservativeLimits) {
  constexpr size_t kBufferSize = 256;
  BlobStoreBuffer<kBufferSize> blob(
      "Blob_OK", partition_, nullptr, kvs::TestKvs(), kBufferSize);
  ASSERT_EQ(OkStatus(), blob.Init());

  BlobStore::BlobWriterWithBuffer writer(blob);
  ASSERT_EQ(OkStatus(), writer.Open());
  EXPECT_EQ(writer.ConservativeReadLimit(), 0u);
  EXPECT_EQ(writer.ConservativeWriteLimit(), kSectorSize * kSectorCount);
  ASSERT_EQ(OkStatus(), writer.Close());

  BlobStore::DeferredWriterWithBuffer deferred_writer(blob);
  ASSERT_EQ(OkStatus(), deferred_writer.Open());
  EXPECT_EQ(deferred_writer.ConservativeReadLimit(), 0u);
  EXPECT_EQ(deferred_writer.ConservativeWriteLimit(), kBufferSize);
}

// Write to the blob using a flash_write_size_bytes smaller than the
// buffer size. Use Write operations smaller than flash_write_size_bytes
// to ensure it checks the internal buffering path.
TEST_F(BlobStoreTest, OversizedWriteBuffer) {
  size_t write_size_bytes = 8;
  ASSERT_LE(write_size_bytes, source_buffer_.size());
  constexpr size_t kBufferSize = 256;
  kvs::ChecksumCrc16 checksum;

  InitSourceBufferToRandom(0x123d123);

  ConstByteSpan write_data = span(source_buffer_);
  ConstByteSpan original_source = span(source_buffer_);

  EXPECT_EQ(OkStatus(), partition_.Erase());

  BlobStoreBuffer<kBufferSize> blob(
      kBlobTitle, partition_, &checksum, kvs::TestKvs(), 64);
  EXPECT_EQ(OkStatus(), blob.Init());

  BlobStore::BlobWriterWithBuffer writer(blob);
  EXPECT_EQ(OkStatus(), writer.Open());
  while (write_data.size_bytes() > 0) {
    ASSERT_EQ(OkStatus(), writer.Write(write_data.first(write_size_bytes)));
    write_data = write_data.subspan(write_size_bytes);
  }
  EXPECT_EQ(OkStatus(), writer.Close());

  // Use reader to check for valid data.
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());
  Result<ConstByteSpan> result = reader.GetMemoryMappedBlob();
  ASSERT_TRUE(result.ok());
  EXPECT_EQ(original_source.size_bytes(), result.value().size_bytes());
  VerifyFlash(result.value());
  VerifyFlash(flash_.buffer());
  EXPECT_EQ(OkStatus(), reader.Close());
}

TEST_F(BlobStoreTest, Reader_ConservativeLimits) {
  InitSourceBufferToRandom(0x11309);
  WriteTestBlock();

  kvs::ChecksumCrc16 checksum;
  constexpr size_t kBufferSize = 16;
  BlobStoreBuffer<kBufferSize> blob(
      "TestBlobBlock", partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());
  EXPECT_TRUE(blob.HasData());
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());

  EXPECT_EQ(kBlobDataSize, reader.ConservativeReadLimit());
  EXPECT_EQ(0u, reader.ConservativeWriteLimit());
}

TEST_F(BlobStoreTest, IsOpen) {
  constexpr size_t kBufferSize = 256;
  BlobStoreBuffer<kBufferSize> blob(
      "Blob_open", partition_, nullptr, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());

  BlobStore::DeferredWriterWithBuffer deferred_writer(blob);
  EXPECT_EQ(false, deferred_writer.IsOpen());
  EXPECT_EQ(OkStatus(), deferred_writer.Open());
  EXPECT_EQ(true, deferred_writer.IsOpen());
  EXPECT_EQ(OkStatus(), deferred_writer.Close());
  EXPECT_EQ(false, deferred_writer.IsOpen());

  BlobStore::BlobWriterWithBuffer writer(blob);
  EXPECT_EQ(false, writer.IsOpen());
  EXPECT_EQ(OkStatus(), writer.Open());
  EXPECT_EQ(true, writer.IsOpen());

  EXPECT_FALSE(blob.HasData());

  // Need to write something, so the blob reader is able to open.
  std::array<std::byte, 64> tmp_buffer = {};
  EXPECT_EQ(OkStatus(), writer.Write(tmp_buffer));
  EXPECT_EQ(OkStatus(), writer.Close());
  EXPECT_EQ(false, writer.IsOpen());

  EXPECT_TRUE(blob.HasData());
  BlobStore::BlobReader reader(blob);
  EXPECT_EQ(false, reader.IsOpen());
  ASSERT_EQ(OkStatus(), reader.Open());
  EXPECT_EQ(true, reader.IsOpen());
  EXPECT_EQ(OkStatus(), reader.Close());
  EXPECT_EQ(false, reader.IsOpen());
}

// Write to the blob using no write buffer size. Write operations must be
// multiples of flash_write_size_bytes.
TEST_F(BlobStoreTest, NoWriteBuffer_1Alignment) {
  if (kFlashAlignment > 1) {
    // Test not valid for flash alignments greater than 1.
    return;
  }

  const size_t kWriteSizeBytes = 1;
  kvs::ChecksumCrc16 checksum;

  InitSourceBufferToRandom(0xaabd123);

  ConstByteSpan write_data = span(source_buffer_);
  ConstByteSpan original_source = span(source_buffer_);

  EXPECT_EQ(OkStatus(), partition_.Erase());

  BlobStore blob(kBlobTitle,
                 partition_,
                 &checksum,
                 kvs::TestKvs(),
                 span<std::byte>(),
                 kWriteSizeBytes);
  EXPECT_EQ(OkStatus(), blob.Init());

  BlobStore::BlobWriterWithBuffer writer(blob);
  EXPECT_EQ(OkStatus(), writer.Open());

  size_t test_write_size[] = {1, 1, 2, 4, 32, 128};

  for (size_t size : test_write_size) {
    ASSERT_EQ(OkStatus(), writer.Write(write_data.first(size)));
    write_data = write_data.subspan(size);
  }

  while (write_data.size_bytes() > 0) {
    const size_t finish_write_size = 8;
    ASSERT_EQ(OkStatus(), writer.Write(write_data.first(finish_write_size)));
    write_data = write_data.subspan(finish_write_size);
  }
  EXPECT_EQ(write_data.size_bytes(), 0U);
  EXPECT_EQ(OkStatus(), writer.Close());

  // Use reader to check for valid data.
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());
  Result<ConstByteSpan> result = reader.GetMemoryMappedBlob();
  ASSERT_TRUE(result.ok());
  EXPECT_EQ(original_source.size_bytes(), result.value().size_bytes());
  VerifyFlash(result.value());
  VerifyFlash(flash_.buffer());
  EXPECT_EQ(OkStatus(), reader.Close());
}

// Write to the blob using no write buffer size. Write operations must be
// multiples of flash_write_size_bytes.
TEST_F(BlobStoreTest, NoWriteBuffer_16Alignment) {
  if (kFlashAlignment > 16) {
    // Test not valid for flash alignments greater than 16.
    return;
  }

  const size_t kWriteSizeBytes = 16;
  kvs::ChecksumCrc16 checksum;

  InitSourceBufferToRandom(0x6745d123);

  ConstByteSpan write_data = span(source_buffer_);
  ConstByteSpan original_source = span(source_buffer_);

  EXPECT_EQ(OkStatus(), partition_.Erase());

  BlobStore blob(kBlobTitle,
                 partition_,
                 &checksum,
                 kvs::TestKvs(),
                 span<std::byte>(),
                 kWriteSizeBytes);
  EXPECT_EQ(OkStatus(), blob.Init());

  BlobStore::BlobWriterWithBuffer writer(blob);
  EXPECT_EQ(OkStatus(), writer.Open());
  ASSERT_EQ(Status::InvalidArgument(), writer.Write(write_data.first(1)));
  ASSERT_EQ(Status::InvalidArgument(),
            writer.Write(write_data.first(kWriteSizeBytes / 2)));

  ASSERT_EQ(OkStatus(), writer.Write(write_data.first(4 * kWriteSizeBytes)));
  write_data = write_data.subspan(4 * kWriteSizeBytes);

  ASSERT_EQ(Status::InvalidArgument(), writer.Write(write_data.first(1)));
  ASSERT_EQ(Status::InvalidArgument(),
            writer.Write(write_data.first(kWriteSizeBytes / 2)));

  while (write_data.size_bytes() > 0) {
    ASSERT_EQ(OkStatus(), writer.Write(write_data.first(kWriteSizeBytes)));
    write_data = write_data.subspan(kWriteSizeBytes);
  }
  EXPECT_EQ(OkStatus(), writer.Close());

  // Use reader to check for valid data.
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());
  Result<ConstByteSpan> result = reader.GetMemoryMappedBlob();
  ASSERT_TRUE(result.ok());
  EXPECT_EQ(original_source.size_bytes(), result.value().size_bytes());
  VerifyFlash(result.value());
  VerifyFlash(flash_.buffer());
  EXPECT_EQ(OkStatus(), reader.Close());
}

TEST_F(BlobStoreTest, FileName) {
  InitSourceBufferToRandom(0x8675309);
  WriteTestBlock();
  constexpr std::string_view kFileName("my_file_1.bin");
  std::array<std::byte, 64> tmp_buffer = {};
  static_assert(kFileName.size() <= tmp_buffer.size());
  kvs::ChecksumCrc16 checksum;
  constexpr size_t kBufferSize = 256;
  {
    // Create/init a blob store in a nested scope so it can be re-initialized
    // later when checking the read.
    BlobStoreBuffer<kBufferSize> blob(
        kBlobTitle, partition_, &checksum, kvs::TestKvs(), kBufferSize);
    EXPECT_EQ(OkStatus(), blob.Init());

    BlobStore::BlobWriterWithBuffer<kFileName.size()> writer(blob);

    EXPECT_EQ(OkStatus(), writer.Open());
    EXPECT_EQ(OkStatus(), writer.SetFileName(kFileName));
    EXPECT_EQ(OkStatus(), writer.Write(tmp_buffer));
    EXPECT_EQ(OkStatus(), writer.Close());
    EXPECT_EQ(OkStatus(),
              kvs::TestKvs().acquire()->Get(kBlobTitle, tmp_buffer).status());
  }

  BlobStoreBuffer<kBufferSize> blob(
      kBlobTitle, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());

  // Ensure the file name can be read from a reader.
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());

  memset(tmp_buffer.data(), 0, tmp_buffer.size());
  StatusWithSize sws = reader.GetFileName(
      {reinterpret_cast<char*>(tmp_buffer.data()), tmp_buffer.size()});

  EXPECT_EQ(OkStatus(), sws.status());
  ASSERT_EQ(kFileName.size(), sws.size());
  EXPECT_EQ(memcmp(kFileName.data(), tmp_buffer.data(), kFileName.size()), 0);
}

TEST_F(BlobStoreTest, FileNameUndersizedRead) {
  InitSourceBufferToRandom(0x8675309);
  WriteTestBlock();
  constexpr std::string_view kFileName("my_file_1.bin");
  std::array<std::byte, 4> tmp_buffer = {};
  static_assert(kFileName.size() > tmp_buffer.size());

  kvs::ChecksumCrc16 checksum;
  constexpr size_t kBufferSize = 256;
  BlobStoreBuffer<kBufferSize> blob(
      kBlobTitle, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());

  BlobStore::BlobWriterWithBuffer<kFileName.size()> writer(blob);

  EXPECT_EQ(OkStatus(), writer.Open());
  EXPECT_EQ(OkStatus(), writer.SetFileName(kFileName));
  EXPECT_EQ(OkStatus(), writer.Write(as_bytes(span("some interesting data"))));
  EXPECT_EQ(OkStatus(), writer.Close());

  // Ensure the file name can be read from a reader.
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());

  StatusWithSize sws = reader.GetFileName(
      {reinterpret_cast<char*>(tmp_buffer.data()), tmp_buffer.size()});
  EXPECT_EQ(Status::ResourceExhausted(), sws.status());
  ASSERT_EQ(tmp_buffer.size(), sws.size());
  EXPECT_EQ(memcmp(kFileName.data(), tmp_buffer.data(), sws.size()), 0);
}

TEST_F(BlobStoreTest, FileNameUndersizedSet) {
  InitSourceBufferToRandom(0x8675309);
  WriteTestBlock();
  constexpr std::string_view kFileName("my_file_1.bin");

  kvs::ChecksumCrc16 checksum;
  constexpr size_t kBufferSize = 256;
  BlobStoreBuffer<kBufferSize> blob(
      kBlobTitle, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());

  BlobStore::BlobWriterWithBuffer<2> writer(blob);

  EXPECT_EQ(OkStatus(), writer.Open());
  EXPECT_EQ(Status::ResourceExhausted(), writer.SetFileName(kFileName));
  EXPECT_EQ(OkStatus(), writer.Close());
}

TEST_F(BlobStoreTest, FileNameInvalidation) {
  InitSourceBufferToRandom(0x8675309);
  WriteTestBlock();

  constexpr std::string_view kFileName("sliced_cheese.png");
  std::array<std::byte, 64> tmp_buffer = {};
  static_assert(kFileName.size() <= tmp_buffer.size());

  kvs::ChecksumCrc16 checksum;
  constexpr size_t kBufferSize = 256;
  BlobStoreBuffer<kBufferSize> blob(
      kBlobTitle, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());

  BlobStore::BlobWriterWithBuffer<kFileName.size()> writer(blob);

  EXPECT_EQ(OkStatus(), writer.Open());
  EXPECT_EQ(OkStatus(), writer.SetFileName(kFileName));
  EXPECT_EQ(OkStatus(), writer.Write(tmp_buffer));
  EXPECT_EQ(OkStatus(), writer.Discard());
  EXPECT_EQ(OkStatus(), writer.Write(tmp_buffer));
  EXPECT_EQ(OkStatus(), writer.Close());

  // Check that the file name was discarded by Discard().
  memset(tmp_buffer.data(), 0, tmp_buffer.size());
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());
  StatusWithSize sws = reader.GetFileName(
      {reinterpret_cast<char*>(tmp_buffer.data()), tmp_buffer.size()});
  EXPECT_EQ(Status::NotFound(), sws.status());
  ASSERT_EQ(0u, sws.size());
}

TEST_F(BlobStoreTest, NoFileName) {
  InitSourceBufferToRandom(0x8675309);
  WriteTestBlock();

  std::array<std::byte, 64> tmp_buffer = {};
  kvs::ChecksumCrc16 checksum;
  constexpr size_t kBufferSize = 256;
  BlobStoreBuffer<kBufferSize> blob(
      kBlobTitle, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());

  // Ensure blobs with no file names work as expected.
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());

  StatusWithSize sws = reader.GetFileName(
      {reinterpret_cast<char*>(tmp_buffer.data()), tmp_buffer.size()});
  EXPECT_EQ(Status::NotFound(), sws.status());
  ASSERT_EQ(0u, sws.size());
}

TEST_F(BlobStoreTest, V1MetadataBackwardsCompatible) {
  constexpr size_t kWriteSize = 25;
  WriteTestBlock(kWriteSize);

  kvs::ChecksumCrc16 checksum;
  constexpr size_t kBufferSize = 16;
  BlobStoreBuffer<kBufferSize> blob(
      kBlobTitle, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());

  // Read the written data in the current format.
  internal::BlobMetadataHeader current_metadata;
  ASSERT_EQ(OkStatus(),
            kvs::TestKvs().acquire()->Get(kBlobTitle, &current_metadata));

  // Re-save only the V1 metadata contents.
  ASSERT_EQ(
      OkStatus(),
      kvs::TestKvs().acquire()->Put(kBlobTitle, current_metadata.v1_metadata));

  // Ensure the BlobStore's contents aren't invalid.
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());
  ASSERT_EQ(kWriteSize, reader.ConservativeReadLimit());
  ASSERT_EQ(current_metadata.v1_metadata.data_size_bytes,
            reader.ConservativeReadLimit());
}

TEST_F(BlobStoreTest, Discard) {
  InitSourceBufferToRandom(0x8675309);
  WriteTestBlock();
  constexpr char blob_title[] = "TestBlobBlock";
  std::array<std::byte, 64> tmp_buffer = {};

  kvs::ChecksumCrc16 checksum;

  // TODO(davidrogers): Do this test with flash/kvs in the different entry state
  // combinations.

  constexpr size_t kBufferSize = 256;
  BlobStoreBuffer<kBufferSize> blob(
      blob_title, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());

  EXPECT_TRUE(blob.HasData());

  BlobStore::BlobWriterWithBuffer writer(blob);

  EXPECT_EQ(OkStatus(), writer.Open());
  EXPECT_EQ(OkStatus(), writer.Write(tmp_buffer));

  // Blob should NOT be valid to read, because the write data was only buffered,
  // and has not been written to flash yet.
  EXPECT_FALSE(blob.HasData());

  // The write does an implicit erase so there should be no key for this blob.
  EXPECT_EQ(Status::NotFound(),
            kvs::TestKvs().acquire()->Get(blob_title, tmp_buffer).status());
  EXPECT_EQ(OkStatus(), writer.Close());

  EXPECT_TRUE(blob.HasData());

  EXPECT_EQ(OkStatus(),
            kvs::TestKvs().acquire()->Get(blob_title, tmp_buffer).status());

  EXPECT_EQ(OkStatus(), writer.Open());
  EXPECT_EQ(OkStatus(), writer.Discard());
  EXPECT_EQ(OkStatus(), writer.Close());

  EXPECT_FALSE(blob.HasData());

  EXPECT_EQ(Status::NotFound(),
            kvs::TestKvs().acquire()->Get(blob_title, tmp_buffer).status());
}

TEST_F(BlobStoreTest, MultipleErase) {
  constexpr size_t kBufferSize = 256;
  BlobStoreBuffer<kBufferSize> blob(
      "Blob_OK", partition_, nullptr, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());

  BlobStore::BlobWriterWithBuffer writer(blob);
  EXPECT_EQ(OkStatus(), writer.Open());

  EXPECT_EQ(OkStatus(), writer.Erase());
  EXPECT_EQ(OkStatus(), writer.Erase());
  EXPECT_EQ(OkStatus(), writer.Erase());
}

TEST_F(BlobStoreTest, OffsetRead) {
  InitSourceBufferToRandom(0x11309);
  WriteTestBlock();

  constexpr size_t kOffset = 10;
  ASSERT_LT(kOffset, kBlobDataSize);

  kvs::ChecksumCrc16 checksum;

  char name[16] = "TestBlobBlock";
  constexpr size_t kBufferSize = 16;
  BlobStoreBuffer<kBufferSize> blob(
      name, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open(kOffset));

  std::array<std::byte, kBlobDataSize - kOffset> read_buffer;
  ByteSpan read_span = read_buffer;
  ASSERT_EQ(read_span.size_bytes(), reader.ConservativeReadLimit());

  auto result = reader.Read(read_span);
  ASSERT_EQ(result.status(), OkStatus());
  EXPECT_EQ(OkStatus(), reader.Close());
  VerifyFlash(read_buffer, kOffset);
}

TEST_F(BlobStoreTest, SeekOffsetRead) {
  InitSourceBufferToRandom(0x11309);
  WriteTestBlock();

  constexpr size_t kOffset = 10;
  ASSERT_LT(kOffset, kBlobDataSize);

  kvs::ChecksumCrc16 checksum;

  char name[16] = "TestBlobBlock";
  constexpr size_t kBufferSize = 16;
  BlobStoreBuffer<kBufferSize> blob(
      name, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());
  ASSERT_EQ(OkStatus(), reader.Seek(kOffset));

  std::array<std::byte, kBlobDataSize - kOffset> read_buffer;
  ByteSpan read_span = read_buffer;
  ASSERT_EQ(read_span.size_bytes(), reader.ConservativeReadLimit());

  auto result = reader.Read(read_span);
  ASSERT_EQ(result.status(), OkStatus());
  EXPECT_EQ(OkStatus(), reader.Close());
  VerifyFlash(read_buffer, kOffset);
}

TEST_F(BlobStoreTest, InvalidReadOffset) {
  InitSourceBufferToRandom(0x11309);
  WriteTestBlock();

  constexpr size_t kOffset = kBlobDataSize;

  kvs::ChecksumCrc16 checksum;

  char name[16] = "TestBlobBlock";
  constexpr size_t kBufferSize = 16;
  BlobStoreBuffer<kBufferSize> blob(
      name, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(Status::InvalidArgument(), reader.Open(kOffset));
}

TEST_F(BlobStoreTest, ReadSeekClosedReader) {
  InitSourceBufferToRandom(0x11309);
  WriteTestBlock();

  kvs::ChecksumCrc16 checksum;

  char name[16] = "TestBlobBlock";
  constexpr size_t kBufferSize = 16;
  BlobStoreBuffer<kBufferSize> blob(
      name, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());
  ASSERT_EQ(OkStatus(), reader.Close());

  EXPECT_EQ(Status::FailedPrecondition(), reader.Seek(0));

  std::byte read_buffer[32];
  EXPECT_EQ(Status::FailedPrecondition(), reader.Read(read_buffer).status());
}

TEST_F(BlobStoreTest, InvalidSeekOffset) {
  InitSourceBufferToRandom(0x11309);
  WriteTestBlock();

  constexpr size_t kOffset = kBlobDataSize;

  kvs::ChecksumCrc16 checksum;

  char name[16] = "TestBlobBlock";
  constexpr size_t kBufferSize = 16;
  BlobStoreBuffer<kBufferSize> blob(
      name, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());
  ASSERT_EQ(Status::OutOfRange(), reader.Seek(kOffset));
}

// Write a block to blob and close with part of a write buffer with unflushed
// data.
TEST_F(BlobStoreTest, WriteBufferWithRemainderInBuffer) {
  InitSourceBufferToRandom(0x11309);

  kvs::ChecksumCrc16 checksum;
  constexpr size_t kBufferSize = 256;
  BlobStoreBuffer<kBufferSize> blob(
      "TestBlobBlock", partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());

  const size_t write_size_bytes = kBlobDataSize - 10;
  ConstByteSpan write_data = span(source_buffer_).first(write_size_bytes);

  BlobStore::BlobWriterWithBuffer writer(blob);
  EXPECT_EQ(OkStatus(), writer.Open());
  ASSERT_EQ(OkStatus(), writer.Write(write_data));
  EXPECT_EQ(OkStatus(), writer.Close());

  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());
  EXPECT_EQ(write_size_bytes, reader.ConservativeReadLimit());
}

// Test reading with a read buffer larger than the available data in the blob.
TEST_F(BlobStoreTest, ReadBufferIsLargerThanData) {
  InitSourceBufferToRandom(0x57326);

  constexpr size_t kWriteBytes = 64;
  WriteTestBlock(kWriteBytes);

  kvs::ChecksumCrc16 checksum;

  char name[16] = "TestBlobBlock";
  constexpr size_t kBufferSize = 16;
  BlobStoreBuffer<kBufferSize> blob(
      name, partition_, &checksum, kvs::TestKvs(), kBufferSize);
  EXPECT_EQ(OkStatus(), blob.Init());
  BlobStore::BlobReader reader(blob);
  ASSERT_EQ(OkStatus(), reader.Open());
  EXPECT_EQ(kWriteBytes, reader.ConservativeReadLimit());

  std::array<std::byte, kWriteBytes + 10> read_buffer;
  ByteSpan read_span = read_buffer;

  auto result = reader.Read(read_span);
  ASSERT_EQ(result.status(), OkStatus());
  EXPECT_EQ(OkStatus(), reader.Close());
}

TEST_F(BlobStoreTest, ChunkRead1) {
  InitSourceBufferToRandom(0x8675309);
  WriteTestBlock();
  ChunkReadTest(1);
}

TEST_F(BlobStoreTest, ChunkRead3) {
  InitSourceBufferToFill(0);
  WriteTestBlock();
  ChunkReadTest(3);
}

TEST_F(BlobStoreTest, ChunkRead4) {
  InitSourceBufferToFill(1);
  WriteTestBlock();
  ChunkReadTest(4);
}

TEST_F(BlobStoreTest, ChunkRead5) {
  InitSourceBufferToFill(0xff);
  WriteTestBlock();
  ChunkReadTest(5);
}

TEST_F(BlobStoreTest, ChunkRead16) {
  InitSourceBufferToRandom(0x86);
  WriteTestBlock();
  ChunkReadTest(16);
}

TEST_F(BlobStoreTest, ChunkRead64) {
  InitSourceBufferToRandom(0x9);
  WriteTestBlock();
  ChunkReadTest(64);
}

TEST_F(BlobStoreTest, ChunkReadFull) {
  InitSourceBufferToRandom(0x9);
  WriteTestBlock();
  ChunkReadTest(kBlobDataSize);
}

TEST_F(BlobStoreTest, PartialBufferThenClose) {
  // Do write of only a partial chunk, which will only have bytes in buffer
  // (none written to flash) at close.
  size_t data_bytes = 12;
  InitSourceBufferToRandom(0x111, data_bytes);
  WriteTestBlock(data_bytes);

  // Do write with several full chunks and then some partial.
  data_bytes = 158;
  InitSourceBufferToRandom(0x3222, data_bytes);
}

// Test to do write/close, write/close multiple times.
TEST_F(BlobStoreTest, MultipleWrites) {
  InitSourceBufferToRandom(0x1121);
  WriteTestBlock();
  InitSourceBufferToRandom(0x515);
  WriteTestBlock();
  InitSourceBufferToRandom(0x4321);
  WriteTestBlock();
}

}  // namespace
}  // namespace pw::blob_store
