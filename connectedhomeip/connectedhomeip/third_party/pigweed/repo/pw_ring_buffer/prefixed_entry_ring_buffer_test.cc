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

#include "pw_ring_buffer/prefixed_entry_ring_buffer.h"

#include <cstddef>
#include <cstdint>

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_containers/vector.h"

using std::byte;

namespace pw {
namespace ring_buffer {
namespace {
using Entry = PrefixedEntryRingBufferMulti::Entry;
using iterator = PrefixedEntryRingBufferMulti::iterator;

TEST(PrefixedEntryRingBuffer, NoBuffer) {
  PrefixedEntryRingBuffer ring(false);

  byte buf[32];
  size_t count;

  EXPECT_EQ(ring.EntryCount(), 0u);
  EXPECT_EQ(ring.SetBuffer(span<byte>(static_cast<byte*>(nullptr), 10u)),
            Status::InvalidArgument());
  EXPECT_EQ(ring.SetBuffer(span(buf, 0u)), Status::InvalidArgument());
  EXPECT_EQ(ring.FrontEntryDataSizeBytes(), 0u);

  EXPECT_EQ(ring.PushBack(buf), Status::FailedPrecondition());
  EXPECT_EQ(ring.EntryCount(), 0u);
  EXPECT_EQ(ring.PeekFront(buf, &count), Status::FailedPrecondition());
  EXPECT_EQ(count, 0u);
  EXPECT_EQ(ring.EntryCount(), 0u);
  EXPECT_EQ(ring.PeekFrontWithPreamble(buf, &count),
            Status::FailedPrecondition());
  EXPECT_EQ(count, 0u);
  EXPECT_EQ(ring.EntryCount(), 0u);
  EXPECT_EQ(ring.PopFront(), Status::FailedPrecondition());
  EXPECT_EQ(ring.EntryCount(), 0u);
}

// Single entry to write/read/pop over and over again.
constexpr byte single_entry_data[] = {byte(1),
                                      byte(2),
                                      byte(3),
                                      byte(4),
                                      byte(5),
                                      byte(6),
                                      byte(7),
                                      byte(8),
                                      byte(9)};
constexpr size_t single_entry_total_size = sizeof(single_entry_data) + 1;
constexpr size_t single_entry_test_buffer_size =
    (single_entry_total_size * 7) / 2;

// Make sure the single_entry_size is even so single_entry_buffer_Size gets the
// proper wrap/even behavior when getting to the end of the buffer.
static_assert((single_entry_total_size % 2) == 0u);
constexpr size_t kSingleEntryCycles = 300u;

// Repeatedly write the same data, read it, and pop it, done over and over
// again.
void SingleEntryWriteReadTest(bool user_data) {
  PrefixedEntryRingBuffer ring(user_data);
  byte test_buffer[single_entry_test_buffer_size];

  byte read_buffer[single_entry_total_size];

  // Set read_size to an unexpected value to make sure result checks don't luck
  // out and happen to see a previous value.
  size_t read_size = 500U;
  uint32_t user_preamble = 0U;

  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  EXPECT_EQ(ring.EntryCount(), 0u);
  EXPECT_EQ(ring.PopFront(), Status::OutOfRange());
  EXPECT_EQ(ring.EntryCount(), 0u);
  EXPECT_EQ(ring.PushBack(span(single_entry_data, sizeof(test_buffer) + 5)),
            Status::OutOfRange());
  EXPECT_EQ(ring.EntryCount(), 0u);
  EXPECT_EQ(ring.PeekFront(read_buffer, &read_size), Status::OutOfRange());
  EXPECT_EQ(read_size, 0u);
  read_size = 500U;
  EXPECT_EQ(ring.PeekFrontWithPreamble(read_buffer, &read_size),
            Status::OutOfRange());
  EXPECT_EQ(read_size, 0u);

  size_t user_preamble_bytes = (user_data ? 1 : 0);
  size_t data_size = sizeof(single_entry_data) - user_preamble_bytes;
  size_t data_offset = single_entry_total_size - data_size;

  byte expect_buffer[single_entry_total_size] = {};
  expect_buffer[user_preamble_bytes] = byte(data_size);
  memcpy(expect_buffer + data_offset, single_entry_data, data_size);

  for (size_t i = 0; i < kSingleEntryCycles; i++) {
    ASSERT_EQ(ring.FrontEntryDataSizeBytes(), 0u);
    ASSERT_EQ(ring.FrontEntryTotalSizeBytes(), 0u);

    // Limit the value of the preamble to a single byte, to ensure that we
    // retain a static `single_entry_buffer_size` during the test. Single
    // bytes are varint-encoded to the same value.
    uint32_t preamble_byte = i % 128;
    ASSERT_EQ(ring.PushBack(span(single_entry_data, data_size), preamble_byte),
              OkStatus());
    ASSERT_EQ(ring.FrontEntryDataSizeBytes(), data_size);
    ASSERT_EQ(ring.FrontEntryTotalSizeBytes(), single_entry_total_size);

    read_size = 500U;
    ASSERT_EQ(ring.PeekFront(read_buffer, &read_size), OkStatus());
    ASSERT_EQ(read_size, data_size);

    // ASSERT_THAT(span(expect_buffer).last(data_size),
    //            testing::ElementsAreArray(span(read_buffer, data_size)));
    ASSERT_EQ(
        memcmp(
            span(expect_buffer).last(data_size).data(), read_buffer, data_size),
        0);

    read_size = 500U;
    ASSERT_EQ(ring.PeekFrontWithPreamble(read_buffer, &read_size), OkStatus());
    ASSERT_EQ(read_size, single_entry_total_size);

    if (user_data) {
      expect_buffer[0] = byte(preamble_byte);
    }

    // ASSERT_THAT(span(expect_buffer),
    //            testing::ElementsAreArray(span(read_buffer)));
    ASSERT_EQ(memcmp(expect_buffer, read_buffer, single_entry_total_size), 0);

    if (user_data) {
      user_preamble = 0U;
      ASSERT_EQ(
          ring.PeekFrontWithPreamble(read_buffer, user_preamble, read_size),
          OkStatus());
      ASSERT_EQ(read_size, data_size);
      ASSERT_EQ(user_preamble, preamble_byte);
      ASSERT_EQ(memcmp(span(expect_buffer).last(data_size).data(),
                       read_buffer,
                       data_size),
                0);
    }

    ASSERT_EQ(ring.PopFront(), OkStatus());
  }
}

TEST(PrefixedEntryRingBuffer, SingleEntryWriteReadNoUserData) {
  SingleEntryWriteReadTest(false);
}

TEST(PrefixedEntryRingBuffer, SingleEntryWriteReadYesUserData) {
  SingleEntryWriteReadTest(true);
}

// TODO(b/234883746): Increase this to 5000 once we have a way to detect targets
// with more computation and memory oomph.
constexpr size_t kOuterCycles = 50u;
constexpr size_t kCountingUpMaxExpectedEntries =
    single_entry_test_buffer_size / single_entry_total_size;

// Write data that is filled with a byte value that increments each write. Write
// many times without read/pop and then check to make sure correct contents are
// in the ring buffer.
template <bool kUserData>
void CountingUpWriteReadTest() {
  PrefixedEntryRingBuffer ring(kUserData);
  byte test_buffer[single_entry_test_buffer_size];

  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());
  EXPECT_EQ(ring.EntryCount(), 0u);

  constexpr size_t kDataSize = sizeof(single_entry_data) - (kUserData ? 1 : 0);

  for (size_t i = 0; i < kOuterCycles; i++) {
    size_t seed = i;

    byte write_buffer[kDataSize];

    size_t j;
    for (j = 0; j < kSingleEntryCycles; j++) {
      memset(write_buffer, j + seed, sizeof(write_buffer));

      ASSERT_EQ(ring.PushBack(write_buffer), OkStatus());

      size_t expected_count = (j < kCountingUpMaxExpectedEntries)
                                  ? j + 1
                                  : kCountingUpMaxExpectedEntries;
      ASSERT_EQ(ring.EntryCount(), expected_count);
    }
    size_t final_write_j = j;
    size_t fill_val = seed + final_write_j - kCountingUpMaxExpectedEntries;

    for (j = 0; j < kCountingUpMaxExpectedEntries; j++) {
      byte read_buffer[sizeof(write_buffer)];
      size_t read_size;
      memset(write_buffer, fill_val + j, sizeof(write_buffer));
      ASSERT_EQ(ring.PeekFront(read_buffer, &read_size), OkStatus());

      ASSERT_EQ(memcmp(write_buffer, read_buffer, kDataSize), 0);

      ASSERT_EQ(ring.PopFront(), OkStatus());
    }
  }
}

TEST(PrefixedEntryRingBuffer, CountingUpWriteReadNoUserData) {
  CountingUpWriteReadTest<false>();
}

TEST(PrefixedEntryRingBuffer, CountingUpWriteReadYesUserData) {
  CountingUpWriteReadTest<true>();
}

// Create statically to prevent allocating a capture in the lambda below.
static pw::Vector<byte, single_entry_total_size> read_buffer;

// Repeatedly write the same data, read it, and pop it, done over and over
// again.
void SingleEntryWriteReadWithSectionWriterTest(bool user_data) {
  PrefixedEntryRingBuffer ring(user_data);
  byte test_buffer[single_entry_test_buffer_size];

  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  auto output = [](span<const byte> src) -> Status {
    for (byte b : src) {
      read_buffer.push_back(b);
    }
    return OkStatus();
  };

  size_t user_preamble_bytes = (user_data ? 1 : 0);
  size_t data_size = sizeof(single_entry_data) - user_preamble_bytes;
  size_t data_offset = single_entry_total_size - data_size;

  byte expect_buffer[single_entry_total_size] = {};
  expect_buffer[user_preamble_bytes] = byte(data_size);
  memcpy(expect_buffer + data_offset, single_entry_data, data_size);

  for (size_t i = 0; i < kSingleEntryCycles; i++) {
    ASSERT_EQ(ring.FrontEntryDataSizeBytes(), 0u);
    ASSERT_EQ(ring.FrontEntryTotalSizeBytes(), 0u);

    // Limit the value of the preamble to a single byte, to ensure that we
    // retain a static `single_entry_buffer_size` during the test. Single
    // bytes are varint-encoded to the same value.
    uint32_t preamble_byte = i % 128;
    ASSERT_EQ(ring.PushBack(span(single_entry_data, data_size), preamble_byte),
              OkStatus());
    ASSERT_EQ(ring.FrontEntryDataSizeBytes(), data_size);
    ASSERT_EQ(ring.FrontEntryTotalSizeBytes(), single_entry_total_size);

    read_buffer.clear();
    ASSERT_EQ(ring.PeekFront(output), OkStatus());
    ASSERT_EQ(read_buffer.size(), data_size);

    ASSERT_EQ(memcmp(span(expect_buffer).last(data_size).data(),
                     read_buffer.data(),
                     data_size),
              0);

    read_buffer.clear();
    ASSERT_EQ(ring.PeekFrontWithPreamble(output), OkStatus());
    ASSERT_EQ(read_buffer.size(), single_entry_total_size);
    ASSERT_EQ(ring.PopFront(), OkStatus());

    if (user_data) {
      expect_buffer[0] = byte(preamble_byte);
    }

    ASSERT_EQ(
        memcmp(expect_buffer, read_buffer.data(), single_entry_total_size), 0);
  }
}

TEST(PrefixedEntryRingBuffer, SingleEntryWriteReadWithSectionWriterNoUserData) {
  SingleEntryWriteReadWithSectionWriterTest(false);
}

TEST(PrefixedEntryRingBuffer,
     SingleEntryWriteReadWithSectionWriterYesUserData) {
  SingleEntryWriteReadWithSectionWriterTest(true);
}

constexpr size_t kEntrySizeBytes = 8u;
constexpr size_t kTotalEntryCount = 20u;
constexpr size_t kBufferExtraBytes = 5u;
constexpr size_t kTestBufferSize =
    (kEntrySizeBytes * kTotalEntryCount) + kBufferExtraBytes;

// Create statically to prevent allocating a capture in the lambda below.
static pw::Vector<byte, kTestBufferSize> actual_result;

void DeringTest(bool preload) {
  PrefixedEntryRingBuffer ring;

  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  // Entry data is entry size - preamble (single byte in this case).
  byte single_entry_buffer[kEntrySizeBytes - 1u];
  auto entry_data = span(single_entry_buffer);
  size_t i;

  // TODO(b/234883746): Increase this to 500 once we have a way to detect
  // targets with more computation and memory oomph.
  size_t loop_goal = preload ? 50 : 1;

  for (size_t main_loop_count = 0; main_loop_count < loop_goal;
       main_loop_count++) {
    if (preload) {
      // Prime the ringbuffer with some junk data to get the buffer
      // wrapped.
      for (i = 0; i < (kTotalEntryCount * (main_loop_count % 64u)); i++) {
        memset(single_entry_buffer, i, sizeof(single_entry_buffer));
        ASSERT_EQ(OkStatus(), ring.PushBack(single_entry_buffer));
      }
    }

    // Build up the expected buffer and fill the ring buffer with the test data.
    pw::Vector<byte, kTestBufferSize> expected_result;
    for (i = 0; i < kTotalEntryCount; i++) {
      // First component of the entry: the varint size.
      static_assert(sizeof(single_entry_buffer) < 127);
      expected_result.push_back(byte(sizeof(single_entry_buffer)));

      // Second component of the entry: the raw data.
      memset(single_entry_buffer, 'a' + i, sizeof(single_entry_buffer));
      for (byte b : entry_data) {
        expected_result.push_back(b);
      }

      // The ring buffer internally pushes the varint size byte.
      ASSERT_EQ(OkStatus(), ring.PushBack(single_entry_buffer));
    }

    // Check values before doing the dering.
    EXPECT_EQ(ring.EntryCount(), kTotalEntryCount);
    EXPECT_EQ(expected_result.size(), ring.TotalUsedBytes());

    ASSERT_EQ(ring.Dering(), OkStatus());

    // Check values after doing the dering.
    EXPECT_EQ(ring.EntryCount(), kTotalEntryCount);
    EXPECT_EQ(expected_result.size(), ring.TotalUsedBytes());

    // Read out the entries of the ring buffer.
    actual_result.clear();
    auto output = [](span<const byte> src) -> Status {
      for (byte b : src) {
        actual_result.push_back(b);
      }
      return OkStatus();
    };
    while (ring.EntryCount()) {
      ASSERT_EQ(ring.PeekFrontWithPreamble(output), OkStatus());
      ASSERT_EQ(ring.PopFront(), OkStatus());
    }

    // Ensure the actual result out of the ring buffer matches our manually
    // computed result.
    EXPECT_EQ(expected_result.size(), actual_result.size());
    ASSERT_EQ(memcmp(test_buffer, actual_result.data(), actual_result.size()),
              0);
    ASSERT_EQ(
        memcmp(
            expected_result.data(), actual_result.data(), actual_result.size()),
        0);
  }
}

TEST(PrefixedEntryRingBuffer, Dering) { DeringTest(true); }
TEST(PrefixedEntryRingBuffer, DeringNoPreload) { DeringTest(false); }

template <typename T>
Status PushBack(PrefixedEntryRingBufferMulti& ring,
                T element,
                uint32_t user_preamble = 0) {
  union {
    std::array<byte, sizeof(element)> buffer;
    T item;
  } aliased;
  aliased.item = element;
  return ring.PushBack(aliased.buffer, user_preamble);
}

template <typename T>
Status TryPushBack(PrefixedEntryRingBufferMulti& ring,
                   T element,
                   uint32_t user_preamble = 0) {
  union {
    std::array<byte, sizeof(element)> buffer;
    T item;
  } aliased;
  aliased.item = element;
  return ring.TryPushBack(aliased.buffer, user_preamble);
}

template <typename T>
T PeekFront(PrefixedEntryRingBufferMulti::Reader& reader,
            uint32_t* user_preamble_out = nullptr) {
  union {
    std::array<byte, sizeof(T)> buffer;
    T item;
  } aliased;
  size_t bytes_read = 0;
  uint32_t user_preamble = 0;
  PW_CHECK_OK(
      reader.PeekFrontWithPreamble(aliased.buffer, user_preamble, bytes_read));
  PW_CHECK_INT_EQ(bytes_read, sizeof(T));
  if (user_preamble_out) {
    *user_preamble_out = user_preamble;
  }
  return aliased.item;
}

template <typename T>
T GetEntry(span<const std::byte> lhs) {
  union {
    std::array<byte, sizeof(T)> buffer;
    T item;
  } aliased;
  std::memcpy(aliased.buffer.data(), lhs.data(), lhs.size_bytes());
  return aliased.item;
}

void EmptyDataPushBackTest(bool user_data) {
  PrefixedEntryRingBuffer ring(user_data);
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  // Push back an empty span and a non-empty span.
  EXPECT_EQ(ring.PushBack(span<std::byte>(), 1u), OkStatus());
  EXPECT_EQ(ring.EntryCount(), 1u);
  EXPECT_EQ(ring.PushBack(single_entry_data, 2u), OkStatus());
  EXPECT_EQ(ring.EntryCount(), 2u);

  // Confirm that both entries can be read back.
  byte entry_buffer[kTestBufferSize];
  uint32_t user_preamble = 0;
  size_t bytes_read = 0;
  // Read empty span.
  EXPECT_EQ(ring.PeekFrontWithPreamble(entry_buffer, user_preamble, bytes_read),
            OkStatus());
  EXPECT_EQ(user_preamble, user_data ? 1u : 0u);
  EXPECT_EQ(bytes_read, 0u);
  EXPECT_EQ(ring.PopFront(), OkStatus());
  EXPECT_EQ(ring.EntryCount(), 1u);
  // Read non-empty span.
  EXPECT_EQ(ring.PeekFrontWithPreamble(entry_buffer, user_preamble, bytes_read),
            OkStatus());
  EXPECT_EQ(user_preamble, user_data ? 2u : 0u);
  ASSERT_EQ(bytes_read, sizeof(single_entry_data));
  EXPECT_EQ(memcmp(entry_buffer, single_entry_data, bytes_read), 0);
  EXPECT_EQ(ring.PopFront(), OkStatus());
  EXPECT_EQ(ring.EntryCount(), 0u);
}

TEST(PrefixedEntryRingBuffer, EmptyDataPushBackTestWithPreamble) {
  EmptyDataPushBackTest(true);
}
TEST(PrefixedEntryRingBuffer, EmptyDataPushBackTestNoPreamble) {
  EmptyDataPushBackTest(false);
}

TEST(PrefixedEntryRingBuffer, TryPushBack) {
  PrefixedEntryRingBuffer ring;
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  // Fill up the ring buffer with a constant.
  int total_items = 0;
  while (true) {
    Status status = TryPushBack<int>(ring, 5);
    if (status.ok()) {
      total_items++;
    } else {
      EXPECT_EQ(status, Status::ResourceExhausted());
      break;
    }
  }
  EXPECT_EQ(PeekFront<int>(ring), 5);

  // Should be unable to push more items.
  for (int i = 0; i < total_items; ++i) {
    EXPECT_EQ(TryPushBack<int>(ring, 100), Status::ResourceExhausted());
    EXPECT_EQ(PeekFront<int>(ring), 5);
  }

  // Fill up the ring buffer with a constant.
  for (int i = 0; i < total_items; ++i) {
    EXPECT_EQ(PushBack<int>(ring, 100), OkStatus());
  }
  EXPECT_EQ(PeekFront<int>(ring), 100);
}

TEST(PrefixedEntryRingBuffer, Iterator) {
  PrefixedEntryRingBuffer ring;
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  // Fill up the ring buffer with a constant value.
  size_t entry_count = 0;
  while (TryPushBack<size_t>(ring, entry_count).ok()) {
    entry_count++;
  }

  // Iterate over all entries and confirm entry count.
  size_t validated_entries = 0;
  for (Result<const Entry> entry_info : ring) {
    EXPECT_TRUE(entry_info.status().ok());
    EXPECT_EQ(GetEntry<size_t>(entry_info.value().buffer), validated_entries);
    validated_entries++;
  }
  EXPECT_EQ(validated_entries, entry_count);
}

TEST(PrefixedEntryRingBufferMulti, TryPushBack) {
  PrefixedEntryRingBufferMulti ring;
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  PrefixedEntryRingBufferMulti::Reader fast_reader;
  PrefixedEntryRingBufferMulti::Reader slow_reader;

  EXPECT_EQ(ring.AttachReader(fast_reader), OkStatus());
  EXPECT_EQ(ring.AttachReader(slow_reader), OkStatus());

  // Fill up the ring buffer with an increasing count.
  int total_items = 0;
  while (true) {
    Status status = TryPushBack<int>(ring, total_items);
    if (status.ok()) {
      total_items++;
    } else {
      EXPECT_EQ(status, Status::ResourceExhausted());
      break;
    }
  }

  // Run fast reader twice as fast as the slow reader.
  size_t total_used_bytes = ring.TotalUsedBytes();
  for (int i = 0; i < total_items; ++i) {
    EXPECT_EQ(PeekFront<int>(fast_reader), i);
    EXPECT_EQ(fast_reader.PopFront(), OkStatus());
    EXPECT_EQ(ring.TotalUsedBytes(), total_used_bytes);
    if (i % 2 == 0) {
      EXPECT_EQ(PeekFront<int>(slow_reader), i / 2);
      EXPECT_EQ(slow_reader.PopFront(), OkStatus());
      EXPECT_TRUE(ring.TotalUsedBytes() < total_used_bytes);
    }
    total_used_bytes = ring.TotalUsedBytes();
  }
  EXPECT_EQ(fast_reader.PopFront(), Status::OutOfRange());
  EXPECT_TRUE(ring.TotalUsedBytes() > 0u);

  // Fill the buffer again, expect that the fast reader
  // only sees half the entries as the slow reader.
  size_t max_items = total_items;
  while (true) {
    Status status = TryPushBack<int>(ring, total_items);
    if (status.ok()) {
      total_items++;
    } else {
      EXPECT_EQ(status, Status::ResourceExhausted());
      break;
    }
  }
  EXPECT_EQ(slow_reader.EntryCount(), max_items);
  EXPECT_EQ(fast_reader.EntryCount(), total_items - max_items);

  for (int i = total_items - max_items; i < total_items; ++i) {
    EXPECT_EQ(PeekFront<int>(slow_reader), i);
    EXPECT_EQ(slow_reader.PopFront(), OkStatus());
    if (static_cast<size_t>(i) >= max_items) {
      EXPECT_EQ(PeekFront<int>(fast_reader), i);
      EXPECT_EQ(fast_reader.PopFront(), OkStatus());
    }
  }
  EXPECT_EQ(slow_reader.PopFront(), Status::OutOfRange());
  EXPECT_EQ(fast_reader.PopFront(), Status::OutOfRange());
  EXPECT_EQ(ring.TotalUsedBytes(), 0u);
}

TEST(PrefixedEntryRingBufferMulti, PushBack) {
  PrefixedEntryRingBufferMulti ring;
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  PrefixedEntryRingBufferMulti::Reader fast_reader;
  PrefixedEntryRingBufferMulti::Reader slow_reader;

  EXPECT_EQ(ring.AttachReader(fast_reader), OkStatus());
  EXPECT_EQ(ring.AttachReader(slow_reader), OkStatus());

  // Fill up the ring buffer with an increasing count.
  size_t total_items = 0;
  while (true) {
    Status status = TryPushBack<uint32_t>(ring, total_items);
    if (status.ok()) {
      total_items++;
    } else {
      EXPECT_EQ(status, Status::ResourceExhausted());
      break;
    }
  }
  EXPECT_EQ(slow_reader.EntryCount(), total_items);

  // The following test:
  //  - Moves the fast reader forward by one entry.
  //  - Writes a single entry that is guaranteed to be larger than the size of a
  //    single entry in the buffer (uint64_t entry > uint32_t entry).
  //  - Checks to see that both readers were moved forward.
  EXPECT_EQ(fast_reader.PopFront(), OkStatus());
  EXPECT_EQ(PushBack<uint64_t>(ring, 5u), OkStatus());
  // The readers have moved past values 0 and 1.
  EXPECT_EQ(PeekFront<uint32_t>(slow_reader), 2u);
  EXPECT_EQ(PeekFront<uint32_t>(fast_reader), 2u);
  // The readers have lost two entries, but gained an entry.
  EXPECT_EQ(slow_reader.EntryCount(), total_items - 1);
  EXPECT_EQ(fast_reader.EntryCount(), total_items - 1);
}

TEST(PrefixedEntryRingBufferMulti, ReaderAddRemove) {
  PrefixedEntryRingBufferMulti ring;
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  PrefixedEntryRingBufferMulti::Reader reader;
  PrefixedEntryRingBufferMulti::Reader transient_reader;

  EXPECT_EQ(ring.AttachReader(reader), OkStatus());

  // Fill up the ring buffer with a constant value.
  size_t total_items = 0;
  while (true) {
    Status status = TryPushBack<size_t>(ring, total_items);
    if (status.ok()) {
      total_items++;
    } else {
      EXPECT_EQ(status, Status::ResourceExhausted());
      break;
    }
  }
  EXPECT_EQ(reader.EntryCount(), total_items);

  // Add new reader after filling the buffer.
  EXPECT_EQ(ring.AttachReader(transient_reader), OkStatus());
  EXPECT_EQ(transient_reader.EntryCount(), total_items);

  // Confirm that the transient reader observes all values, even though it was
  // attached after entries were pushed.
  for (size_t i = 0; i < total_items; i++) {
    EXPECT_EQ(PeekFront<size_t>(transient_reader), i);
    EXPECT_EQ(transient_reader.PopFront(), OkStatus());
  }
  EXPECT_EQ(transient_reader.EntryCount(), 0u);

  // Confirm that re-attaching the reader resets it back to the oldest
  // available entry.
  EXPECT_EQ(ring.DetachReader(transient_reader), OkStatus());
  EXPECT_EQ(ring.AttachReader(transient_reader), OkStatus());
  EXPECT_EQ(transient_reader.EntryCount(), total_items);

  for (size_t i = 0; i < total_items; i++) {
    EXPECT_EQ(PeekFront<size_t>(transient_reader), i);
    EXPECT_EQ(transient_reader.PopFront(), OkStatus());
  }
  EXPECT_EQ(transient_reader.EntryCount(), 0u);
}

TEST(PrefixedEntryRingBufferMulti, SingleBufferPerReader) {
  PrefixedEntryRingBufferMulti ring_one;
  PrefixedEntryRingBufferMulti ring_two;
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring_one.SetBuffer(test_buffer), OkStatus());

  PrefixedEntryRingBufferMulti::Reader reader;
  EXPECT_EQ(ring_one.AttachReader(reader), OkStatus());
  EXPECT_EQ(ring_two.AttachReader(reader), Status::InvalidArgument());

  EXPECT_EQ(ring_one.DetachReader(reader), OkStatus());
  EXPECT_EQ(ring_two.AttachReader(reader), OkStatus());
  EXPECT_EQ(ring_one.AttachReader(reader), Status::InvalidArgument());
}

TEST(PrefixedEntryRingBufferMulti, IteratorEmptyBuffer) {
  PrefixedEntryRingBufferMulti ring;
  // Pick a buffer that can't contain any valid sections.
  byte test_buffer[1] = {std::byte(0xFF)};

  PrefixedEntryRingBufferMulti::Reader reader;
  EXPECT_EQ(ring.AttachReader(reader), OkStatus());
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  EXPECT_EQ(ring.begin(), ring.end());
}

TEST(PrefixedEntryRingBufferMulti, IteratorValidEntries) {
  PrefixedEntryRingBufferMulti ring;
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  PrefixedEntryRingBufferMulti::Reader reader;
  EXPECT_EQ(ring.AttachReader(reader), OkStatus());

  // Buffer only contains valid entries. This happens after populating
  // the buffer and no entries have been read.
  // E.g. [VALID|VALID|VALID|INVALID]

  // Fill up the ring buffer with a constant value.
  size_t entry_count = 0;
  while (TryPushBack<size_t>(ring, entry_count).ok()) {
    entry_count++;
  }

  // Iterate over all entries and confirm entry count.
  size_t validated_entries = 0;
  for (const Entry& entry_info : ring) {
    EXPECT_EQ(GetEntry<size_t>(entry_info.buffer), validated_entries);
    validated_entries++;
  }
  EXPECT_EQ(validated_entries, entry_count);
}

TEST(PrefixedEntryRingBufferMulti, IteratorValidEntriesWithPreamble) {
  PrefixedEntryRingBufferMulti ring(true);
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  PrefixedEntryRingBufferMulti::Reader reader;
  EXPECT_EQ(ring.AttachReader(reader), OkStatus());

  // Buffer only contains valid entries. This happens after populating
  // the buffer and no entries have been read.
  // E.g. [VALID|VALID|VALID|INVALID]

  // Fill up the ring buffer with a constant value.
  size_t entry_count = 0;
  while (TryPushBack<size_t>(ring, entry_count, entry_count).ok()) {
    entry_count++;
  }

  // Iterate over all entries and confirm entry count.
  size_t validated_entries = 0;
  for (const Entry& entry_info : ring) {
    EXPECT_EQ(GetEntry<size_t>(entry_info.buffer), validated_entries);
    EXPECT_EQ(entry_info.preamble, validated_entries);
    validated_entries++;
  }
  EXPECT_EQ(validated_entries, entry_count);
}

TEST(PrefixedEntryRingBufferMulti, IteratorStaleEntries) {
  PrefixedEntryRingBufferMulti ring;
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  // Buffer only contains stale, valid entries. This happens when after
  // populating the buffer, all entries are read. The buffer retains the
  // data but has an entry count of zero.
  // E.g. [STALE|STALE|STALE]
  PrefixedEntryRingBufferMulti::Reader trailing_reader;
  EXPECT_EQ(ring.AttachReader(trailing_reader), OkStatus());

  PrefixedEntryRingBufferMulti::Reader reader;
  EXPECT_EQ(ring.AttachReader(reader), OkStatus());

  // Push and pop all the entries.
  size_t entry_count = 0;
  while (TryPushBack<size_t>(ring, entry_count).ok()) {
    entry_count++;
  }

  while (reader.PopFront().ok()) {
  }

  // Iterate over all entries and confirm entry count.
  size_t validated_entries = 0;
  for (const Entry& entry_info : ring) {
    EXPECT_EQ(GetEntry<size_t>(entry_info.buffer), validated_entries);
    validated_entries++;
  }
  EXPECT_EQ(validated_entries, entry_count);
}

TEST(PrefixedEntryRingBufferMulti, IteratorValidStaleEntries) {
  PrefixedEntryRingBufferMulti ring;
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  // Buffer contains both valid and stale entries. This happens when after
  // populating the buffer, only some of the entries are read.
  // E.g. [VALID|INVALID|STALE|STALE]
  PrefixedEntryRingBufferMulti::Reader trailing_reader;
  EXPECT_EQ(ring.AttachReader(trailing_reader), OkStatus());

  PrefixedEntryRingBufferMulti::Reader reader;
  EXPECT_EQ(ring.AttachReader(reader), OkStatus());

  // Fill the buffer with entries.
  size_t entry_count = 0;
  while (TryPushBack<size_t>(ring, entry_count).ok()) {
    entry_count++;
  }

  // Pop roughly half the entries.
  while (reader.EntryCount() > (entry_count / 2)) {
    EXPECT_TRUE(reader.PopFront().ok());
  }

  // Iterate over all entries and confirm entry count.
  size_t validated_entries = 0;
  for (const Entry& entry_info : ring) {
    EXPECT_EQ(GetEntry<size_t>(entry_info.buffer), validated_entries);
    validated_entries++;
  }
  EXPECT_EQ(validated_entries, entry_count);
}

TEST(PrefixedEntryRingBufferMulti, IteratorBufferCorruption) {
  PrefixedEntryRingBufferMulti ring;
  byte test_buffer[kTestBufferSize];
  EXPECT_EQ(ring.SetBuffer(test_buffer), OkStatus());

  // Buffer contains partially written entries. This may happen if writing
  // is pre-empted (e.g. a crash occurs). In this state, we expect a series
  // of valid entries followed by an invalid entry.
  PrefixedEntryRingBufferMulti::Reader trailing_reader;
  EXPECT_EQ(ring.AttachReader(trailing_reader), OkStatus());

  // Add one entry to capture the second entry index.
  size_t entry_count = 0;
  EXPECT_TRUE(TryPushBack<size_t>(ring, entry_count++).ok());
  size_t entry_size = ring.TotalUsedBytes();

  // Fill the buffer with entries.
  while (TryPushBack<size_t>(ring, entry_count++).ok()) {
  }

  // Push another entry to move the write index forward and force the oldest
  // reader forward. This will require the iterator to dering.
  EXPECT_TRUE(PushBack<size_t>(ring, 0).ok());
  EXPECT_TRUE(ring.CheckForCorruption().ok());

  // The first entry is overwritten. Corrupt all data past the fifth entry.
  // Note that because the first entry has shifted, the entry_count recorded
  // in each entry is shifted by 1.
  constexpr size_t valid_entries = 5;
  size_t offset = valid_entries * entry_size;
  memset(test_buffer + offset, 0xFF, kTestBufferSize - offset);
  EXPECT_FALSE(ring.CheckForCorruption().ok());

  // Iterate over all entries and confirm entry count.
  size_t validated_entries = 0;
  iterator it = ring.begin();
  for (; it != ring.end(); it++) {
    EXPECT_EQ(GetEntry<size_t>(it->buffer), validated_entries + 1);
    validated_entries++;
  }
  // The final entry will fail to be read.
  EXPECT_EQ(it.status(), Status::DataLoss());
  EXPECT_EQ(validated_entries, valid_entries);
}

}  // namespace
}  // namespace ring_buffer
}  // namespace pw
