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

#include "pw_kvs/internal/entry.h"

#include <string_view>

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_kvs/alignment.h"
#include "pw_kvs/checksum.h"
#include "pw_kvs/crc16_checksum.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/format.h"
#include "pw_span/span.h"

namespace pw::kvs::internal {
namespace {

using std::byte;
using std::string_view;

// For magic value always use a random 32 bit integer rather than a human
// readable 4 bytes. See pw_kvs/format.h for more information.
constexpr EntryFormat kFormat{0x961c2ff9, nullptr};

TEST(Entry, Size_RoundsUpToAlignment) {
  // Use FakeFlashMemory, rather than FakeFlashMemoryBuffer, so the class gets
  // tested/used directly.
  std::array<std::byte, 64 * 2> buffer;

  // Flash alignment needs to be 1 due to how the partition is used in this
  // test.
  FakeFlashMemory flash(buffer, 64, 2, 1);

  for (size_t alignment_bytes = 1; alignment_bytes <= 4096; ++alignment_bytes) {
    FlashPartition partition(&flash, 0, flash.sector_count(), alignment_bytes);
    const size_t align = AlignUp(alignment_bytes, Entry::kMinAlignmentBytes);

    for (size_t value : {size_t(0), align - 1, align, align + 1, 2 * align}) {
      Entry entry = Entry::Valid(
          partition, 0, kFormat, "k", {static_cast<byte*>(nullptr), value}, 0);

      ASSERT_EQ(AlignUp(sizeof(EntryHeader) + 1 /* key */ + value, align),
                entry.size());
    }

    Entry entry = Entry::Tombstone(partition, 0, kFormat, "k", 0);
    ASSERT_EQ(AlignUp(sizeof(EntryHeader) + 1 /* key */, align), entry.size());
  }
}

TEST(Entry, Construct_ValidEntry) {
  FakeFlashMemoryBuffer<64, 2> flash(16);
  FlashPartition partition(&flash, 0, flash.sector_count());

  auto entry =
      Entry::Valid(partition, 1, kFormat, "k", as_bytes(span("123")), 9876);

  EXPECT_FALSE(entry.deleted());
  EXPECT_EQ(entry.magic(), kFormat.magic);
  EXPECT_EQ(entry.value_size(), sizeof("123"));
  EXPECT_EQ(entry.transaction_id(), 9876u);
}

TEST(Entry, Construct_Tombstone) {
  FakeFlashMemoryBuffer<64, 2> flash(16);
  FlashPartition partition(&flash, 0, flash.sector_count());

  auto entry = Entry::Tombstone(partition, 1, kFormat, "key", 123);

  EXPECT_TRUE(entry.deleted());
  EXPECT_EQ(entry.magic(), kFormat.magic);
  EXPECT_EQ(entry.value_size(), 0u);
  EXPECT_EQ(entry.transaction_id(), 123u);
}

// For magic value always use a unique random 32 bit integer rather than a human
// readable 4 bytes. See pw_kvs/format.h for more information.
constexpr uint32_t kMagicWithChecksum = 0xad165142;
constexpr uint32_t kTransactionId1 = 0x96979899;

constexpr auto kKey1 = bytes::String("key45");
constexpr auto kValue1 = bytes::String("VALUE!");
constexpr auto kPadding1 = bytes::String("\0\0\0\0\0");

constexpr auto kHeader1 = bytes::Concat(kMagicWithChecksum,
                                        uint32_t(0x23aa),  // checksum (CRC16)
                                        uint8_t(1),        // alignment (32 B)
                                        uint8_t(kKey1.size()),     // key length
                                        uint16_t(kValue1.size()),  // value size
                                        kTransactionId1  // transaction ID
);

constexpr auto kEntryWithoutPadding1 = bytes::Concat(kHeader1, kKey1, kValue1);
constexpr auto kEntry1 = bytes::Concat(kEntryWithoutPadding1, kPadding1);
static_assert(kEntry1.size() == 32);

ChecksumCrc16 default_checksum;
constexpr EntryFormat kFormatWithChecksum{kMagicWithChecksum,
                                          &default_checksum};
constexpr internal::EntryFormats kFormats(kFormatWithChecksum);

class ValidEntryInFlash : public ::testing::Test {
 protected:
  ValidEntryInFlash() : flash_(kEntry1), partition_(&flash_) {
    EXPECT_EQ(OkStatus(), Entry::Read(partition_, 0, kFormats, &entry_));
  }

  FakeFlashMemoryBuffer<1024, 4> flash_;
  FlashPartition partition_;
  Entry entry_;
};

TEST_F(ValidEntryInFlash, PassesChecksumVerification) {
  EXPECT_EQ(OkStatus(), entry_.VerifyChecksumInFlash());
  EXPECT_EQ(OkStatus(), entry_.VerifyChecksum("key45", kValue1));
}

TEST_F(ValidEntryInFlash, HeaderContents) {
  EXPECT_EQ(entry_.magic(), kMagicWithChecksum);
  EXPECT_EQ(entry_.key_length(), 5u);
  EXPECT_EQ(entry_.value_size(), 6u);
  EXPECT_EQ(entry_.transaction_id(), kTransactionId1);
  EXPECT_FALSE(entry_.deleted());
}

TEST_F(ValidEntryInFlash, ReadKey) {
  Entry::KeyBuffer key = {};
  auto result = entry_.ReadKey(key);

  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(result.size(), entry_.key_length());
  EXPECT_STREQ(key.data(), "key45");
}

TEST_F(ValidEntryInFlash, ReadValue) {
  char value[32] = {};
  auto result = entry_.ReadValue(as_writable_bytes(span(value)));

  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(result.size(), entry_.value_size());
  EXPECT_STREQ(value, "VALUE!");
}

TEST_F(ValidEntryInFlash, ReadValue_BufferTooSmall) {
  char value[3] = {};
  auto result = entry_.ReadValue(as_writable_bytes(span(value)));

  ASSERT_EQ(Status::ResourceExhausted(), result.status());
  EXPECT_EQ(3u, result.size());
  EXPECT_EQ(value[0], 'V');
  EXPECT_EQ(value[1], 'A');
  EXPECT_EQ(value[2], 'L');
}

TEST_F(ValidEntryInFlash, ReadValue_WithOffset) {
  char value[3] = {};
  auto result = entry_.ReadValue(as_writable_bytes(span(value)), 3);

  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(3u, result.size());
  EXPECT_EQ(value[0], 'U');
  EXPECT_EQ(value[1], 'E');
  EXPECT_EQ(value[2], '!');
}

TEST_F(ValidEntryInFlash, ReadValue_WithOffset_BufferTooSmall) {
  char value[1] = {};
  auto result = entry_.ReadValue(as_writable_bytes(span(value)), 4);

  ASSERT_EQ(Status::ResourceExhausted(), result.status());
  EXPECT_EQ(1u, result.size());
  EXPECT_EQ(value[0], 'E');
}

TEST_F(ValidEntryInFlash, ReadValue_WithOffset_EmptyRead) {
  char value[16] = {'?'};
  auto result = entry_.ReadValue(as_writable_bytes(span(value)), 6);

  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(0u, result.size());
  EXPECT_EQ(value[0], '?');
}

TEST_F(ValidEntryInFlash, ReadValue_WithOffset_PastEnd) {
  char value[16] = {};
  auto result = entry_.ReadValue(as_writable_bytes(span(value)), 7);

  EXPECT_EQ(Status::OutOfRange(), result.status());
  EXPECT_EQ(0u, result.size());
}

TEST(ValidEntry, Write) {
  FakeFlashMemoryBuffer<1024, 4> flash;
  FlashPartition partition(&flash, 0, flash.sector_count(), 32);

  Entry entry = Entry::Valid(
      partition, 64, kFormatWithChecksum, "key45", kValue1, kTransactionId1);

  auto result = entry.Write("key45", kValue1);
  EXPECT_EQ(OkStatus(), result.status());
  EXPECT_EQ(32u, result.size());
  EXPECT_EQ(std::memcmp(&flash.buffer()[64], kEntry1.data(), kEntry1.size()),
            0);
}

constexpr auto kHeader2 = bytes::String(
    "\x42\x51\x16\xad"  // magic
    "\xba\xb3\x00\x00"  // checksum (CRC16)
    "\x00"              // alignment
    "\x01"              // key length
    "\xff\xff"          // value size
    "\x00\x01\x02\x03"  // transaction ID
);

constexpr auto kKeyAndPadding2 =
    bytes::String("K\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");

class TombstoneEntryInFlash : public ::testing::Test {
 protected:
  TombstoneEntryInFlash()
      : flash_(bytes::Concat(kHeader2, kKeyAndPadding2)), partition_(&flash_) {
    EXPECT_EQ(OkStatus(), Entry::Read(partition_, 0, kFormats, &entry_));
  }

  FakeFlashMemoryBuffer<1024, 4> flash_;
  FlashPartition partition_;
  Entry entry_;
};

TEST_F(TombstoneEntryInFlash, PassesChecksumVerification) {
  EXPECT_EQ(OkStatus(), entry_.VerifyChecksumInFlash());
  EXPECT_EQ(OkStatus(), entry_.VerifyChecksum("K", {}));
}

TEST_F(TombstoneEntryInFlash, HeaderContents) {
  EXPECT_EQ(entry_.magic(), kMagicWithChecksum);
  EXPECT_EQ(entry_.key_length(), 1u);
  EXPECT_EQ(entry_.value_size(), 0u);
  EXPECT_EQ(entry_.transaction_id(), 0x03020100u);
  EXPECT_TRUE(entry_.deleted());
}

TEST_F(TombstoneEntryInFlash, ReadKey) {
  Entry::KeyBuffer key = {};
  auto result = entry_.ReadKey(key);

  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(result.size(), entry_.key_length());
  EXPECT_STREQ(key.data(), "K");
}

TEST_F(TombstoneEntryInFlash, ReadValue) {
  char value[32] = {};
  auto result = entry_.ReadValue(as_writable_bytes(span(value)));

  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(0u, result.size());
}

TEST(TombstoneEntry, Write) {
  FakeFlashMemoryBuffer<1024, 4> flash;
  FlashPartition partition(&flash);
  ChecksumCrc16 checksum;

  Entry entry =
      Entry::Tombstone(partition, 16, kFormatWithChecksum, "K", 0x03020100);

  auto result = entry.Write("K", {});
  EXPECT_EQ(OkStatus(), result.status());
  EXPECT_EQ(32u, result.size());
  EXPECT_EQ(std::memcmp(&flash.buffer()[16],
                        bytes::Concat(kHeader2, kKeyAndPadding2).data(),
                        kEntry1.size()),
            0);
}

TEST(Entry, Checksum_NoChecksumRequiresZero) {
  FakeFlashMemoryBuffer<1024, 4> flash(kEntry1);
  FlashPartition partition(&flash);
  Entry entry;

  const EntryFormat format{kMagicWithChecksum, nullptr};
  const internal::EntryFormats formats(format);

  ASSERT_EQ(OkStatus(), Entry::Read(partition, 0, formats, &entry));

  EXPECT_EQ(Status::DataLoss(), entry.VerifyChecksumInFlash());
  EXPECT_EQ(Status::DataLoss(), entry.VerifyChecksum({}, {}));

  std::memset(&flash.buffer()[4], 0, 4);  // set the checksum field to 0
  ASSERT_EQ(OkStatus(), Entry::Read(partition, 0, formats, &entry));
  EXPECT_EQ(OkStatus(), entry.VerifyChecksumInFlash());
  EXPECT_EQ(OkStatus(), entry.VerifyChecksum({}, {}));
}

TEST(Entry, Checksum_ChecksPadding) {
  FakeFlashMemoryBuffer<1024, 4> flash(
      bytes::Concat(kHeader1, kKey1, kValue1, bytes::String("\0\0\0\0\1")));
  FlashPartition partition(&flash);
  Entry entry;
  ASSERT_EQ(OkStatus(), Entry::Read(partition, 0, kFormats, &entry));

  // Last byte in padding is a 1; should fail.
  EXPECT_EQ(Status::DataLoss(), entry.VerifyChecksumInFlash());

  // The in-memory verification fills in 0s for the padding.
  EXPECT_EQ(OkStatus(), entry.VerifyChecksum("key45", kValue1));

  flash.buffer()[kEntry1.size() - 1] = byte{0};
  EXPECT_EQ(OkStatus(), entry.VerifyChecksumInFlash());
}

TEST_F(ValidEntryInFlash, Update_SameFormat_TransactionIdIsUpdated) {
  ASSERT_EQ(OkStatus(),
            entry_.Update(kFormatWithChecksum, kTransactionId1 + 3));

  EXPECT_EQ(kFormatWithChecksum.magic, entry_.magic());
  EXPECT_EQ(0u, entry_.address());
  EXPECT_EQ(kTransactionId1 + 3, entry_.transaction_id());
  EXPECT_FALSE(entry_.deleted());
}

TEST_F(ValidEntryInFlash,
       Update_DifferentFormat_MagicAndTransactionIdAreUpdated) {
  ASSERT_EQ(OkStatus(), entry_.Update(kFormat, kTransactionId1 + 6));

  EXPECT_EQ(kFormat.magic, entry_.magic());
  EXPECT_EQ(0u, entry_.address());
  EXPECT_EQ(kTransactionId1 + 6, entry_.transaction_id());
  EXPECT_FALSE(entry_.deleted());
}

TEST_F(ValidEntryInFlash, Update_ReadError_WithChecksumIsError) {
  flash_.InjectReadError(FlashError::Unconditional(Status::Aborted()));

  EXPECT_EQ(Status::Aborted(),
            entry_.Update(kFormatWithChecksum, kTransactionId1 + 1));
}

// For magic value always use a random 32 bit integer rather than a human
// readable 4 bytes. See pw_kvs/format.h for more information.
constexpr EntryFormat kNoChecksumFormat{.magic = 0x721bad24,
                                        .checksum = nullptr};

TEST_F(ValidEntryInFlash, Update_ReadError_NoChecksumIsOkay) {
  flash_.InjectReadError(FlashError::Unconditional(Status::Aborted()));

  EXPECT_EQ(OkStatus(), entry_.Update(kNoChecksumFormat, kTransactionId1 + 1));
}

TEST_F(ValidEntryInFlash, Copy) {
  auto result = entry_.Copy(123);

  EXPECT_EQ(OkStatus(), result.status());
  EXPECT_EQ(entry_.size(), result.size());
  EXPECT_EQ(0,
            std::memcmp(
                &flash_.buffer().data()[123], kEntry1.data(), kEntry1.size()));
}

TEST_F(ValidEntryInFlash, Copy_ReadError) {
  flash_.InjectReadError(FlashError::Unconditional(Status::Unimplemented()));
  auto result = entry_.Copy(kEntry1.size());
  EXPECT_EQ(Status::Unimplemented(), result.status());
  EXPECT_EQ(0u, result.size());
}

constexpr uint32_t ByteSum(span<const byte> bytes, uint32_t value = 0) {
  for (byte b : bytes) {
    value += unsigned(b);
  }
  return value;
}

// Sums the bytes, adding one to each byte so that zeroes change the checksum.
class ChecksumSummation final : public ChecksumAlgorithm {
 public:
  ChecksumSummation() : ChecksumAlgorithm(as_bytes(span(&sum_, 1))), sum_(0) {}

  void Reset() override { sum_ = 0; }

  void Update(span<const byte> data) override {
    for (byte b : data) {
      sum_ += unsigned(b) + 1;  // Add 1 so zero-value bytes affect checksum.
    }
  }

 private:
  uint32_t sum_;
} sum_checksum;

// For magic value always use a random 32 bit integer rather than a human
// readable 4 bytes. See pw_kvs/format.h for more information.
constexpr uint32_t kMagicWithSum = 0x6093aadb;
constexpr EntryFormat kFormatWithSum{kMagicWithSum, &sum_checksum};
constexpr internal::EntryFormats kFormatsWithSum(kFormatWithSum);

template <size_t kAlignment>
constexpr auto MakeNewFormatWithSumEntry() {
  constexpr uint8_t alignment_units = (kAlignment + 15) / 16 - 1;
  constexpr size_t size = AlignUp(kEntryWithoutPadding1.size(), kAlignment);

  constexpr uint32_t checksum =
      ByteSum(bytes::Concat(kFormatWithSum.magic)) + 0 /* checksum */ +
      alignment_units + kKey1.size() + kValue1.size() +
      ByteSum(bytes::Concat(kTransactionId1 + 1)) + ByteSum(kKey1) +
      ByteSum(kValue1) + size /* +1 for each byte in the checksum */;

  constexpr auto kNewHeader1 =
      bytes::Concat(kFormatWithSum.magic,      // magic
                    checksum,                  // checksum (byte sum)
                    alignment_units,           // alignment (in 16 B units)
                    uint8_t(kKey1.size()),     // key length
                    uint16_t(kValue1.size()),  // value size
                    kTransactionId1 + 1);      // transaction ID
  constexpr size_t padding = Padding(kEntryWithoutPadding1.size(), kAlignment);
  return bytes::Concat(
      kNewHeader1, kKey1, kValue1, bytes::Initialized<padding>(0));
}

TEST_F(ValidEntryInFlash, UpdateAndCopy_DifferentFormatSmallerAlignment) {
  // Uses 16-bit alignment, smaller than the original entry's alignment.
  ASSERT_EQ(OkStatus(), entry_.Update(kFormatWithSum, kTransactionId1 + 1));

  StatusWithSize result = entry_.Copy(kEntry1.size());
  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(kEntry1.size(), result.size());

  constexpr auto new_data = MakeNewFormatWithSumEntry<16>();
  static_assert(new_data.size() == 32);

  EXPECT_EQ(
      0,
      std::memcmp(
          &flash_.buffer()[kEntry1.size()], new_data.data(), new_data.size()));
  Entry new_entry;
  ASSERT_EQ(OkStatus(),
            Entry::Read(partition_, 32, kFormatsWithSum, &new_entry));
  EXPECT_EQ(OkStatus(), new_entry.VerifyChecksumInFlash());
  EXPECT_EQ(kFormatWithSum.magic, new_entry.magic());
  EXPECT_EQ(kTransactionId1 + 1, new_entry.transaction_id());
}

TEST(Entry, UpdateAndCopy_DifferentFormatSameAlignment) {
  // Use 32-bit alignment, the same as the original entry's alignment.
  FakeFlashMemoryBuffer<1024, 4> flash(kEntry1);
  FlashPartition partition(&flash, 0, 4, 32);
  Entry entry;
  ASSERT_EQ(OkStatus(), Entry::Read(partition, 0, kFormats, &entry));

  ASSERT_EQ(OkStatus(), entry.Update(kFormatWithSum, kTransactionId1 + 1));

  StatusWithSize result = entry.Copy(32);
  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(AlignUp(kEntry1.size(), 32), result.size());

  constexpr auto new_data = MakeNewFormatWithSumEntry<32>();
  static_assert(new_data.size() == 32);

  EXPECT_EQ(0,
            std::memcmp(&flash.buffer()[32], new_data.data(), new_data.size()));

  Entry new_entry;
  ASSERT_EQ(OkStatus(),
            Entry::Read(partition, 32, kFormatsWithSum, &new_entry));
  EXPECT_EQ(OkStatus(), new_entry.VerifyChecksumInFlash());
  EXPECT_EQ(kTransactionId1 + 1, new_entry.transaction_id());
}

TEST(Entry, UpdateAndCopy_DifferentFormatLargerAlignment) {
  // Use 64-bit alignment, larger than the original entry's alignment.
  FakeFlashMemoryBuffer<1024, 4> flash(kEntry1);
  FlashPartition partition(&flash, 0, 4, 64);
  Entry entry;
  ASSERT_EQ(OkStatus(), Entry::Read(partition, 0, kFormats, &entry));

  ASSERT_EQ(OkStatus(), entry.Update(kFormatWithSum, kTransactionId1 + 1));

  StatusWithSize result = entry.Copy(64);
  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(AlignUp(kEntry1.size(), 64), result.size());

  constexpr auto new_data = MakeNewFormatWithSumEntry<64>();
  static_assert(new_data.size() == 64);

  EXPECT_EQ(0,
            std::memcmp(&flash.buffer()[64], new_data.data(), new_data.size()));

  Entry new_entry;
  ASSERT_EQ(OkStatus(),
            Entry::Read(partition, 64, kFormatsWithSum, &new_entry));
  EXPECT_EQ(OkStatus(), new_entry.VerifyChecksumInFlash());
  EXPECT_EQ(kTransactionId1 + 1, new_entry.transaction_id());
}

TEST_F(ValidEntryInFlash, UpdateAndCopy_NoChecksum_UpdatesToNewFormat) {
  // For magic value always use a random 32 bit integer rather than a human
  // readable 4 bytes. See pw_kvs/format.h for more information.
  constexpr EntryFormat no_checksum{.magic = 0x43fae18f, .checksum = nullptr};

  ASSERT_EQ(OkStatus(), entry_.Update(no_checksum, kTransactionId1 + 1));

  auto result = entry_.Copy(kEntry1.size());
  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(kEntry1.size(), result.size());

  constexpr auto kNewHeader1 =
      bytes::Concat(no_checksum.magic,  // magic
                    uint32_t(0),        // checksum (none)
                    uint8_t(0),         // alignment (changed to 16 B from 32)
                    uint8_t(kKey1.size()),     // key length
                    uint16_t(kValue1.size()),  // value size
                    kTransactionId1 + 1);      // transaction ID
  constexpr auto kNewEntry1 =
      bytes::Concat(kNewHeader1, kKey1, kValue1, kPadding1);

  EXPECT_EQ(0,
            std::memcmp(&flash_.buffer()[kEntry1.size()],
                        kNewEntry1.data(),
                        kNewEntry1.size()));
}

TEST_F(ValidEntryInFlash, UpdateAndCopyMultple_DifferentFormat) {
  ASSERT_EQ(OkStatus(), entry_.Update(kFormatWithSum, kTransactionId1 + 6));

  FlashPartition::Address new_address = entry_.size();

  for (int i = 0; i < 10; i++) {
    StatusWithSize copy_result = entry_.Copy(new_address + (i * entry_.size()));
    ASSERT_EQ(OkStatus(), copy_result.status());
    ASSERT_EQ(kEntry1.size(), copy_result.size());
  }

  for (int j = 0; j < 10; j++) {
    Entry entry;
    FlashPartition::Address read_address = (new_address + (j * entry_.size()));
    ASSERT_EQ(OkStatus(),
              Entry::Read(partition_, read_address, kFormatsWithSum, &entry));

    EXPECT_EQ(OkStatus(), entry.VerifyChecksumInFlash());
    EXPECT_EQ(kFormatWithSum.magic, entry.magic());
    EXPECT_EQ(read_address, entry.address());
    EXPECT_EQ(kTransactionId1 + 6, entry.transaction_id());
    EXPECT_FALSE(entry.deleted());
  }
}

TEST_F(ValidEntryInFlash, DifferentFormat_UpdatedCopy_FailsWithWrongMagic) {
  ASSERT_EQ(OkStatus(), entry_.Update(kFormatWithSum, kTransactionId1 + 6));

  FlashPartition::Address new_address = entry_.size();

  StatusWithSize copy_result = entry_.Copy(new_address);
  ASSERT_EQ(OkStatus(), copy_result.status());
  ASSERT_EQ(kEntry1.size(), copy_result.size());

  Entry entry;
  ASSERT_EQ(Status::DataLoss(),
            Entry::Read(partition_, new_address, kFormats, &entry));
}

TEST_F(ValidEntryInFlash, UpdateAndCopy_WriteError) {
  flash_.InjectWriteError(FlashError::Unconditional(Status::Cancelled()));

  ASSERT_EQ(OkStatus(), entry_.Update(kNoChecksumFormat, kTransactionId1 + 1));

  auto result = entry_.Copy(kEntry1.size());
  EXPECT_EQ(Status::Cancelled(), result.status());
  EXPECT_EQ(kEntry1.size(), result.size());
}

}  // namespace
}  // namespace pw::kvs::internal
