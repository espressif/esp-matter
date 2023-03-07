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

#include "pw_blob_store/flat_file_system_entry.h"

#include <array>
#include <cstddef>
#include <cstring>

#include "gtest/gtest.h"
#include "pw_blob_store/blob_store.h"
#include "pw_kvs/crc16_checksum.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/test_key_value_store.h"
#include "pw_random/xor_shift.h"
#include "pw_span/span.h"
#include "pw_sync/mutex.h"

namespace pw::blob_store {
namespace {

class FlatFileSystemBlobStoreEntryTest : public ::testing::Test {
 protected:
  static constexpr char kBlobTitle[] = "TestBlobBlock";
  static constexpr size_t kBufferSize = 64;

  FlatFileSystemBlobStoreEntryTest()
      : flash_(kFlashAlignment),
        partition_(&flash_),
        metadata_buffer_(),
        source_buffer_(),
        checksum_(),
        blob_(kBlobTitle, partition_, &checksum_, kvs::TestKvs(), kBufferSize) {
  }

  void SetUp() override { ASSERT_EQ(OkStatus(), blob_.Init()); }

  void InitSourceBufferToRandom(uint64_t seed,
                                size_t init_size_bytes = kBlobDataSize) {
    ASSERT_LE(init_size_bytes, source_buffer_.size());
    random::XorShiftStarRng64 rng(seed);

    std::memset(source_buffer_.data(),
                static_cast<int>(flash_.erased_memory_content()),
                source_buffer_.size());
    rng.Get(span(source_buffer_).first(init_size_bytes));
  }

  // Fill the source buffer with random pattern based on given seed, written to
  // BlobStore in specified chunk size.
  void WriteTestBlock(std::string_view file_name, size_t write_size_bytes) {
    ASSERT_LE(write_size_bytes, source_buffer_.size());

    ConstByteSpan write_data = span(source_buffer_).first(write_size_bytes);

    BlobStore::BlobWriter writer(blob_, metadata_buffer_);
    EXPECT_EQ(OkStatus(), writer.Open());
    ASSERT_EQ(OkStatus(), writer.SetFileName(file_name));
    ASSERT_EQ(OkStatus(), writer.Write(write_data));
    EXPECT_EQ(OkStatus(), writer.Close());

    // Use reader to check for valid data.
    BlobStore::BlobReader reader(blob_);
    ASSERT_EQ(OkStatus(), reader.Open());
    Result<ConstByteSpan> result = reader.GetMemoryMappedBlob();
    ASSERT_TRUE(result.ok());
    EXPECT_EQ(write_size_bytes, result.value().size_bytes());
    EXPECT_EQ(OkStatus(), reader.Close());
  }

  static constexpr size_t kFlashAlignment = 16;
  static constexpr size_t kSectorSize = 2048;
  static constexpr size_t kSectorCount = 2;
  static constexpr size_t kBlobDataSize = (kSectorCount * kSectorSize);
  static constexpr size_t kMaxFileNameLength = 32;
  static constexpr size_t kMetadataBufferSize =
      BlobStore::BlobWriter::RequiredMetadataBufferSize(kMaxFileNameLength);

  kvs::FakeFlashMemoryBuffer<kSectorSize, kSectorCount> flash_;
  kvs::FlashPartition partition_;
  std::array<std::byte, kMetadataBufferSize> metadata_buffer_;
  std::array<std::byte, kBlobDataSize> source_buffer_;
  kvs::ChecksumCrc16 checksum_;
  BlobStoreBuffer<kBufferSize> blob_;
};

TEST_F(FlatFileSystemBlobStoreEntryTest, BasicProperties) {
  constexpr size_t kWrittenDataSizeBytes = 104;
  constexpr uint32_t kExpectedFileId = 0x731ACAC0;
  constexpr FlatFileSystemBlobStoreEntry::FilePermissions kExpectedPermissions =
      FlatFileSystemBlobStoreEntry::FilePermissions::READ;

  constexpr std::string_view kFileName("my_file_1.bin");
  InitSourceBufferToRandom(0x5C4CA189);
  WriteTestBlock(kFileName, kWrittenDataSizeBytes);
  std::array<char, kMaxFileNameLength> tmp_buffer = {};
  static_assert(kFileName.size() <= tmp_buffer.size());

  sync::VirtualMutex blob_store_mutex;
  FlatFileSystemBlobStoreEntry blob_store_file(
      kExpectedFileId, kExpectedPermissions, blob_, blob_store_mutex);

  StatusWithSize sws = blob_store_file.Name(tmp_buffer);
  ASSERT_EQ(OkStatus(), sws.status());

  const int comparison =
      memcmp(tmp_buffer.data(), kFileName.data(), sws.size());
  EXPECT_EQ(0, comparison);
  EXPECT_EQ(kWrittenDataSizeBytes, blob_store_file.SizeBytes());
  EXPECT_EQ(kExpectedPermissions, blob_store_file.Permissions());
  EXPECT_EQ(kExpectedFileId, blob_store_file.FileId());
}

TEST_F(FlatFileSystemBlobStoreEntryTest, Delete) {
  constexpr size_t kWrittenDataSizeBytes = 104;
  constexpr uint32_t kExpectedFileId = 0x87ED0EF2;
  constexpr FlatFileSystemBlobStoreEntry::FilePermissions kExpectedPermissions =
      FlatFileSystemBlobStoreEntry::FilePermissions::READ;

  constexpr std::string_view kFileName("my_file_1.bin");
  InitSourceBufferToRandom(0x5C4CA189);
  WriteTestBlock(kFileName, kWrittenDataSizeBytes);

  sync::VirtualMutex blob_store_mutex;
  FlatFileSystemBlobStoreEntry blob_store_file(
      kExpectedFileId, kExpectedPermissions, blob_, blob_store_mutex);

  ASSERT_EQ(OkStatus(), blob_store_file.Delete());

  BlobStore::BlobReader reader(blob_);
  // Failed precondition is the expected return value when a BlobStore is opened
  // for reading and is empty.
  ASSERT_EQ(Status::FailedPrecondition(), reader.Open());
}

TEST_F(FlatFileSystemBlobStoreEntryTest, NoData) {
  constexpr uint32_t kExpectedFileId = 0x1;
  constexpr FlatFileSystemBlobStoreEntry::FilePermissions kExpectedPermissions =
      FlatFileSystemBlobStoreEntry::FilePermissions::READ;

  // Ensure the BlobStore is erased.
  ASSERT_EQ(OkStatus(), partition_.Erase());

  sync::VirtualMutex blob_store_mutex;
  FlatFileSystemBlobStoreEntry blob_store_file(
      kExpectedFileId, kExpectedPermissions, blob_, blob_store_mutex);

  std::array<char, kMaxFileNameLength> tmp_buffer = {};
  StatusWithSize sws = blob_store_file.Name(tmp_buffer);
  EXPECT_EQ(Status::NotFound(), sws.status());
  EXPECT_EQ(0u, sws.size());
}

}  // namespace
}  // namespace pw::blob_store
