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
#include <cstdio>
#include <cstring>

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_bytes/array.h"
#include "pw_checksum/crc16_ccitt.h"
#include "pw_kvs/crc16_checksum.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/flash_test_partition.h"
#include "pw_kvs/internal/entry.h"
#include "pw_kvs/key_value_store.h"
#include "pw_log/log.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_string/string_builder.h"

namespace pw::kvs {
namespace {

using internal::EntryHeader;
using std::byte;

constexpr size_t kMaxEntries = 256;
constexpr size_t kMaxUsableSectors = 1024;

FlashPartition& test_partition = FlashTestPartition();

std::array<byte, 512> buffer;

size_t RoundUpForAlignment(size_t size) {
  return AlignUp(size, test_partition.alignment_bytes());
}

// This class gives attributes of KVS that we are testing against
class KvsAttributes {
 public:
  KvsAttributes(size_t key_size, size_t data_size)
      : chunk_header_size_(RoundUpForAlignment(sizeof(EntryHeader))),
        data_size_(RoundUpForAlignment(data_size)),
        key_size_(RoundUpForAlignment(key_size)),
        erase_size_(chunk_header_size_ + key_size_),
        min_put_size_(
            RoundUpForAlignment(chunk_header_size_ + key_size_ + data_size_)) {}

  size_t ChunkHeaderSize() { return chunk_header_size_; }
  size_t DataSize() { return data_size_; }
  size_t KeySize() { return key_size_; }
  size_t EraseSize() { return erase_size_; }
  size_t MinPutSize() { return min_put_size_; }

 private:
  const size_t chunk_header_size_;
  const size_t data_size_;
  const size_t key_size_;
  const size_t erase_size_;
  const size_t min_put_size_;
};

constexpr std::array<const char*, 3> keys{"TestKey1", "Key2", "TestKey3"};

ChecksumCrc16 checksum;
// For KVS magic value always use a random 32 bit integer rather than a
// human readable 4 bytes. See pw_kvs/format.h for more information.
constexpr EntryFormat default_format{.magic = 0x5b9a341e,
                                     .checksum = &checksum};

class EmptyInitializedKvs : public ::testing::Test {
 protected:
  EmptyInitializedKvs() : kvs_(&test_partition, default_format) {
    EXPECT_EQ(OkStatus(), test_partition.Erase());
    PW_CHECK_OK(kvs_.Init());
  }

  // Intention of this is to put and erase key-val to fill up sectors. It's a
  // helper function in testing how KVS handles cases where flash sector is full
  // or near full.
  void FillKvs(const char* key, size_t size_to_fill) {
    constexpr size_t kTestDataSize = 8;
    KvsAttributes kvs_attr(std::strlen(key), kTestDataSize);
    const size_t kMaxPutSize =
        buffer.size() + kvs_attr.ChunkHeaderSize() + kvs_attr.KeySize();

    ASSERT_GE(size_to_fill, kvs_attr.MinPutSize() + kvs_attr.EraseSize());

    // Saving enough space to perform erase after loop
    size_to_fill -= kvs_attr.EraseSize();
    // We start with possible small chunk to prevent too small of a Kvs.Put() at
    // the end.
    size_t chunk_len =
        std::max(kvs_attr.MinPutSize(), size_to_fill % buffer.size());
    std::memset(buffer.data(), 0, buffer.size());
    while (size_to_fill > 0) {
      // Changing buffer value so put actually does something
      buffer[0] = static_cast<byte>(static_cast<uint8_t>(buffer[0]) + 1);
      ASSERT_EQ(OkStatus(),
                kvs_.Put(key,
                         span(buffer.data(),
                              chunk_len - kvs_attr.ChunkHeaderSize() -
                                  kvs_attr.KeySize())));
      size_to_fill -= chunk_len;
      chunk_len = std::min(size_to_fill, kMaxPutSize);
    }
    ASSERT_EQ(OkStatus(), kvs_.Delete(key));
  }

  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs_;
};

}  // namespace

TEST_F(EmptyInitializedKvs, Put_SameKeySameValueRepeatedly_AlignedEntries) {
  std::array<char, 8> value{'v', 'a', 'l', 'u', 'e', '6', '7', '\0'};

  for (int i = 0; i < 1000; ++i) {
    ASSERT_EQ(OkStatus(), kvs_.Put("The Key!", as_bytes(span(value))));
  }
}

TEST_F(EmptyInitializedKvs, Put_SameKeySameValueRepeatedly_UnalignedEntries) {
  std::array<char, 7> value{'v', 'a', 'l', 'u', 'e', '6', '\0'};

  for (int i = 0; i < 1000; ++i) {
    ASSERT_EQ(OkStatus(), kvs_.Put("The Key!", as_bytes(span(value))));
  }
}

TEST_F(EmptyInitializedKvs, Put_SameKeyDifferentValuesRepeatedly) {
  std::array<char, 10> value{'v', 'a', 'l', 'u', 'e', '6', '7', '8', '9', '\0'};

  for (int i = 0; i < 100; ++i) {
    for (unsigned size = 0; size < value.size(); ++size) {
      ASSERT_EQ(OkStatus(), kvs_.Put("The Key!", i));
    }
  }
}

TEST_F(EmptyInitializedKvs, PutAndGetByValue_ConvertibleToSpan) {
  constexpr float input[] = {1.0, -3.5};
  ASSERT_EQ(OkStatus(), kvs_.Put("key", input));

  float output[2] = {};
  ASSERT_EQ(OkStatus(), kvs_.Get("key", &output));
  EXPECT_EQ(input[0], output[0]);
  EXPECT_EQ(input[1], output[1]);
}

TEST_F(EmptyInitializedKvs, PutAndGetByValue_Span) {
  float input[] = {1.0, -3.5};
  ASSERT_EQ(OkStatus(), kvs_.Put("key", span(input)));

  float output[2] = {};
  ASSERT_EQ(OkStatus(), kvs_.Get("key", &output));
  EXPECT_EQ(input[0], output[0]);
  EXPECT_EQ(input[1], output[1]);
}

TEST_F(EmptyInitializedKvs, PutAndGetByValue_NotConvertibleToSpan) {
  struct TestStruct {
    float a;
    bool b;
  };
  const TestStruct input{-1234.5, true};

  ASSERT_EQ(OkStatus(), kvs_.Put("key", input));

  TestStruct output;
  ASSERT_EQ(OkStatus(), kvs_.Get("key", &output));
  EXPECT_EQ(input.a, output.a);
  EXPECT_EQ(input.b, output.b);
}

TEST_F(EmptyInitializedKvs, Get_Simple) {
  ASSERT_EQ(OkStatus(), kvs_.Put("Charles", as_bytes(span("Mingus"))));

  char value[16];
  auto result = kvs_.Get("Charles", as_writable_bytes(span(value)));
  EXPECT_EQ(OkStatus(), result.status());
  EXPECT_EQ(sizeof("Mingus"), result.size());
  EXPECT_STREQ("Mingus", value);
}

TEST_F(EmptyInitializedKvs, Get_WithOffset) {
  ASSERT_EQ(OkStatus(), kvs_.Put("Charles", as_bytes(span("Mingus"))));

  char value[16];
  auto result = kvs_.Get("Charles", as_writable_bytes(span(value)), 4);
  EXPECT_EQ(OkStatus(), result.status());
  EXPECT_EQ(sizeof("Mingus") - 4, result.size());
  EXPECT_STREQ("us", value);
}

TEST_F(EmptyInitializedKvs, Get_WithOffset_FillBuffer) {
  ASSERT_EQ(OkStatus(), kvs_.Put("Charles", as_bytes(span("Mingus"))));

  char value[4] = {};
  auto result = kvs_.Get("Charles", as_writable_bytes(span(value, 3)), 1);
  EXPECT_EQ(Status::ResourceExhausted(), result.status());
  EXPECT_EQ(3u, result.size());
  EXPECT_STREQ("ing", value);
}

TEST_F(EmptyInitializedKvs, Get_WithOffset_PastEnd) {
  ASSERT_EQ(OkStatus(), kvs_.Put("Charles", as_bytes(span("Mingus"))));

  char value[16];
  auto result =
      kvs_.Get("Charles", as_writable_bytes(span(value)), sizeof("Mingus") + 1);
  EXPECT_EQ(Status::OutOfRange(), result.status());
  EXPECT_EQ(0u, result.size());
}

TEST_F(EmptyInitializedKvs, GetValue) {
  ASSERT_EQ(OkStatus(), kvs_.Put("key", uint32_t(0xfeedbeef)));

  uint32_t value = 0;
  EXPECT_EQ(OkStatus(), kvs_.Get("key", &value));
  EXPECT_EQ(uint32_t(0xfeedbeef), value);
}

TEST_F(EmptyInitializedKvs, GetValue_TooSmall) {
  ASSERT_EQ(OkStatus(), kvs_.Put("key", uint32_t(0xfeedbeef)));

  uint8_t value = 0;
  EXPECT_EQ(Status::InvalidArgument(), kvs_.Get("key", &value));
  EXPECT_EQ(0u, value);
}

TEST_F(EmptyInitializedKvs, GetValue_TooLarge) {
  ASSERT_EQ(OkStatus(), kvs_.Put("key", uint32_t(0xfeedbeef)));

  uint64_t value = 0;
  EXPECT_EQ(Status::InvalidArgument(), kvs_.Get("key", &value));
  EXPECT_EQ(0u, value);
}

TEST_F(EmptyInitializedKvs, Delete_GetDeletedKey_ReturnsNotFound) {
  ASSERT_EQ(OkStatus(), kvs_.Put("kEy", as_bytes(span("123"))));
  ASSERT_EQ(OkStatus(), kvs_.Delete("kEy"));

  EXPECT_EQ(Status::NotFound(), kvs_.Get("kEy", {}).status());
  EXPECT_EQ(Status::NotFound(), kvs_.ValueSize("kEy").status());
}

TEST_F(EmptyInitializedKvs, Delete_AddBackKey_PersistsAfterInitialization) {
  ASSERT_EQ(OkStatus(), kvs_.Put("kEy", as_bytes(span("123"))));
  ASSERT_EQ(OkStatus(), kvs_.Delete("kEy"));

  EXPECT_EQ(OkStatus(), kvs_.Put("kEy", as_bytes(span("45678"))));
  char data[6] = {};
  ASSERT_EQ(OkStatus(), kvs_.Get("kEy", &data));
  EXPECT_STREQ(data, "45678");

  // Ensure that the re-added key is still present after reinitialization.
  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> new_kvs(&test_partition,
                                                              default_format);
  ASSERT_EQ(OkStatus(), new_kvs.Init());

  EXPECT_EQ(OkStatus(), new_kvs.Put("kEy", as_bytes(span("45678"))));
  char new_data[6] = {};
  EXPECT_EQ(OkStatus(), new_kvs.Get("kEy", &new_data));
  EXPECT_STREQ(data, "45678");
}

TEST_F(EmptyInitializedKvs, Delete_AllItems_KvsIsEmpty) {
  ASSERT_EQ(OkStatus(), kvs_.Put("kEy", as_bytes(span("123"))));
  ASSERT_EQ(OkStatus(), kvs_.Delete("kEy"));

  EXPECT_EQ(0u, kvs_.size());
  EXPECT_TRUE(kvs_.empty());
}

TEST_F(EmptyInitializedKvs, Collision_WithPresentKey) {
  // Both hash to 0x19df36f0.
  constexpr std::string_view key1 = "D4";
  constexpr std::string_view key2 = "dFU6S";

  ASSERT_EQ(OkStatus(), kvs_.Put(key1, 1000));

  EXPECT_EQ(Status::AlreadyExists(), kvs_.Put(key2, 999));

  int value = 0;
  EXPECT_EQ(OkStatus(), kvs_.Get(key1, &value));
  EXPECT_EQ(1000, value);

  EXPECT_EQ(Status::NotFound(), kvs_.Get(key2, &value));
  EXPECT_EQ(Status::NotFound(), kvs_.ValueSize(key2).status());
  EXPECT_EQ(Status::NotFound(), kvs_.Delete(key2));
}

TEST_F(EmptyInitializedKvs, Collision_WithDeletedKey) {
  // Both hash to 0x4060f732.
  constexpr std::string_view key1 = "1U2";
  constexpr std::string_view key2 = "ahj9d";

  ASSERT_EQ(OkStatus(), kvs_.Put(key1, 1000));
  ASSERT_EQ(OkStatus(), kvs_.Delete(key1));

  // key2 collides with key1's tombstone.
  EXPECT_EQ(Status::AlreadyExists(), kvs_.Put(key2, 999));

  int value = 0;
  EXPECT_EQ(Status::NotFound(), kvs_.Get(key1, &value));

  EXPECT_EQ(Status::NotFound(), kvs_.Get(key2, &value));
  EXPECT_EQ(Status::NotFound(), kvs_.ValueSize(key2).status());
  EXPECT_EQ(Status::NotFound(), kvs_.Delete(key2));
}

TEST_F(EmptyInitializedKvs, Iteration_Empty_ByReference) {
  for (const KeyValueStore::Item& entry : kvs_) {
    FAIL();  // The KVS is empty; this shouldn't execute.
    static_cast<void>(entry);
  }
}

TEST_F(EmptyInitializedKvs, Iteration_Empty_ByValue) {
  for (KeyValueStore::Item entry : kvs_) {
    FAIL();  // The KVS is empty; this shouldn't execute.
    static_cast<void>(entry);
  }
}

TEST_F(EmptyInitializedKvs, Iteration_OneItem) {
  ASSERT_EQ(OkStatus(), kvs_.Put("kEy", as_bytes(span("123"))));

  for (KeyValueStore::Item entry : kvs_) {
    EXPECT_STREQ(entry.key(), "kEy");  // Make sure null-terminated.

    char temp[sizeof("123")] = {};
    EXPECT_EQ(OkStatus(), entry.Get(&temp));
    EXPECT_STREQ("123", temp);
  }
}

TEST_F(EmptyInitializedKvs, Iteration_GetWithOffset) {
  ASSERT_EQ(OkStatus(), kvs_.Put("key", as_bytes(span("not bad!"))));

  for (KeyValueStore::Item entry : kvs_) {
    char temp[5];
    auto result = entry.Get(as_writable_bytes(span(temp)), 4);
    EXPECT_EQ(OkStatus(), result.status());
    EXPECT_EQ(5u, result.size());
    EXPECT_STREQ("bad!", temp);
  }
}

TEST_F(EmptyInitializedKvs, Iteration_GetValue) {
  ASSERT_EQ(OkStatus(), kvs_.Put("key", uint32_t(0xfeedbeef)));

  for (KeyValueStore::Item entry : kvs_) {
    uint32_t value = 0;
    EXPECT_EQ(OkStatus(), entry.Get(&value));
    EXPECT_EQ(uint32_t(0xfeedbeef), value);
  }
}

TEST_F(EmptyInitializedKvs, Iteration_GetValue_TooSmall) {
  ASSERT_EQ(OkStatus(), kvs_.Put("key", uint32_t(0xfeedbeef)));

  for (KeyValueStore::Item entry : kvs_) {
    uint8_t value = 0;
    EXPECT_EQ(Status::InvalidArgument(), entry.Get(&value));
    EXPECT_EQ(0u, value);
  }
}

TEST_F(EmptyInitializedKvs, Iteration_GetValue_TooLarge) {
  ASSERT_EQ(OkStatus(), kvs_.Put("key", uint32_t(0xfeedbeef)));

  for (KeyValueStore::Item entry : kvs_) {
    uint64_t value = 0;
    EXPECT_EQ(Status::InvalidArgument(), entry.Get(&value));
    EXPECT_EQ(0u, value);
  }
}

TEST_F(EmptyInitializedKvs, Iteration_EmptyAfterDeletion) {
  ASSERT_EQ(OkStatus(), kvs_.Put("kEy", as_bytes(span("123"))));
  ASSERT_EQ(OkStatus(), kvs_.Delete("kEy"));

  for (KeyValueStore::Item entry : kvs_) {
    static_cast<void>(entry);
    FAIL();
  }
}

TEST_F(EmptyInitializedKvs, Iterator) {
  ASSERT_EQ(OkStatus(), kvs_.Put("kEy", as_bytes(span("123"))));

  for (KeyValueStore::iterator it = kvs_.begin(); it != kvs_.end(); ++it) {
    EXPECT_STREQ(it->key(), "kEy");

    char temp[sizeof("123")] = {};
    EXPECT_EQ(OkStatus(), it->Get(&temp));
    EXPECT_STREQ("123", temp);
  }
}

TEST_F(EmptyInitializedKvs, Iterator_PostIncrement) {
  ASSERT_EQ(OkStatus(), kvs_.Put("kEy", as_bytes(span("123"))));

  KeyValueStore::iterator it = kvs_.begin();
  EXPECT_EQ(it++, kvs_.begin());
  EXPECT_EQ(it, kvs_.end());
}

TEST_F(EmptyInitializedKvs, Iterator_PreIncrement) {
  ASSERT_EQ(OkStatus(), kvs_.Put("kEy", as_bytes(span("123"))));

  KeyValueStore::iterator it = kvs_.begin();
  EXPECT_EQ(++it, kvs_.end());
  EXPECT_EQ(it, kvs_.end());
}

TEST_F(EmptyInitializedKvs, Basic) {
  // Add some data
  uint8_t value1 = 0xDA;
  ASSERT_EQ(OkStatus(),
            kvs_.Put(keys[0], as_bytes(span(&value1, sizeof(value1)))));

  uint32_t value2 = 0xBAD0301f;
  ASSERT_EQ(OkStatus(), kvs_.Put(keys[1], value2));

  // Verify data
  uint32_t test2;
  EXPECT_EQ(OkStatus(), kvs_.Get(keys[1], &test2));
  uint8_t test1;
  ASSERT_EQ(OkStatus(), kvs_.Get(keys[0], &test1));

  EXPECT_EQ(test1, value1);
  EXPECT_EQ(test2, value2);

  // Delete a key
  EXPECT_EQ(OkStatus(), kvs_.Delete(keys[0]));

  // Verify it was erased
  EXPECT_EQ(kvs_.Get(keys[0], &test1), Status::NotFound());
  test2 = 0;
  ASSERT_EQ(
      OkStatus(),
      kvs_.Get(keys[1], span(reinterpret_cast<byte*>(&test2), sizeof(test2)))
          .status());
  EXPECT_EQ(test2, value2);

  // Delete other key
  ASSERT_EQ(OkStatus(), kvs_.Delete(keys[1]));

  // Verify it was erased
  EXPECT_EQ(kvs_.size(), 0u);
}

}  // namespace pw::kvs
