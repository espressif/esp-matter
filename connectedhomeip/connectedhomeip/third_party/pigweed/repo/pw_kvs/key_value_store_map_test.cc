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

#include <cstdlib>
#include <random>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "pw_span/span.h"

#define DUMP_KVS_CONTENTS 0

#if DUMP_KVS_CONTENTS
#include <iostream>
#endif  // DUMP_KVS_CONTENTS

#include "gtest/gtest.h"
#include "pw_kvs/crc16_checksum.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_partition_with_stats.h"
#include "pw_kvs/internal/entry.h"
#include "pw_kvs/key_value_store.h"
#include "pw_log/log.h"
#include "pw_string/string_builder.h"

namespace pw::kvs {
namespace {

constexpr size_t kMaxEntries = 256;
constexpr size_t kMaxUsableSectors = 256;

constexpr std::string_view kChars =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";

struct TestParameters {
  size_t sector_size;
  size_t sector_count;
  size_t sector_alignment;
  size_t redundancy;
  size_t partition_start_sector;
  size_t partition_sector_count;
  size_t partition_alignment;
};

enum Options {
  kNone,
  kReinit,
  kReinitWithFullGC,
  kReinitWithPartialGC,
};

template <typename T>
std::set<T> difference(const std::set<T> lhs, const std::set<T> rhs) {
  std::set<T> diff;
  std::set_difference(lhs.begin(),
                      lhs.end(),
                      rhs.begin(),
                      rhs.end(),
                      std::inserter(diff, diff.begin()));

  return diff;
}

template <const TestParameters& kParams>
class KvsTester {
 public:
  KvsTester()
      : partition_(&flash_,
                   kParams.partition_start_sector,
                   kParams.partition_sector_count,
                   kParams.partition_alignment),
        // For KVS magic value always use a random 32 bit integer rather than a
        // human readable 4 bytes. See pw_kvs/format.h for more information.
        kvs_(&partition_, {.magic = 0xc857e51d, .checksum = nullptr}) {
    EXPECT_EQ(OkStatus(), partition_.Erase());
    Status result = kvs_.Init();
    EXPECT_EQ(OkStatus(), result);

    if (!result.ok()) {
      std::abort();
    }
  }

  ~KvsTester() { CompareContents(); }

  void Test_RandomValidInputs(int iterations,
                              uint_fast32_t seed,
                              Options options) {
    std::mt19937 random(seed);
    std::uniform_int_distribution<unsigned> distro;
    auto random_int = [&] { return distro(random); };

    auto random_string = [&](size_t length) {
      std::string value;
      for (size_t i = 0; i < length; ++i) {
        value.push_back(kChars[random_int() % kChars.size()]);
      }
      return value;
    };

    partition_.ResetCounters();

    for (int i = 0; i < iterations; ++i) {
      if (options != kNone && random_int() % 10 == 0) {
        Init();
      }

      // One out of 4 times, delete a key.
      if (random_int() % 4 == 0) {
        // Either delete a non-existent key or delete an existing one.
        if (empty() || random_int() % 8 == 0) {
          Delete("not_a_key" + std::to_string(random_int()));
        } else {
          Delete(RandomPresentKey());
        }
      } else {
        std::string key;

        // Either add a new key or replace an existing one.
        // TODO(davidrogers): Using %2 (or any less than 16) fails with
        // redundancy due to KVS filling up and not being able to write the
        // second redundant entry, returning error. After re-init() the new key
        // is picked up, resulting in a mis-match between KVS and the test map.
        if (empty() || random_int() % 16 == 0) {
          key = random_string(random_int() %
                              (internal::Entry::kMaxKeyLength + 1));
        } else {
          key = RandomPresentKey();
        }

        Put(key, random_string(random_int() % kMaxValueLength));
      }

      if (options == kReinitWithFullGC && random_int() % 250 == 0) {
        GCFull();
      } else if (options == kReinitWithPartialGC && random_int() % 40 == 0) {
        GCPartial();
      }
    }

    // Only save for tests that have enough data to be interesting.
    if (partition_.sector_count() > 2 && partition_.total_erase_count() > 20) {
      pw::StringBuffer<64> label;
      label << "Random";
      label << partition_.sector_count();
      label << "Sector";
      label << iterations;
      label << ((options != kNone) ? "Reinit" : "");
      label << ((options == kReinitWithFullGC) ? "FullGC" : "");
      label << ((options == kReinitWithPartialGC) ? "PartialGC" : "");
      label << ((kvs_.redundancy() > 1) ? "Redundant" : "");

      // Ignore error to allow test to pass on platforms where writing out the
      // stats is not possible.
      partition_.SaveStorageStats(kvs_, label.data()).IgnoreError();
    }
  }

  void Test_Put() {
    Put("base_key", "base_value");
    for (int i = 0; i < 100; ++i) {
      Put("other_key", std::to_string(i));
    }
    for (int i = 0; i < 100; ++i) {
      Put("key_" + std::to_string(i), std::to_string(i));
    }
  }

  void Test_PutAndDelete_RelocateDeletedEntriesShouldStayDeleted() {
    for (int i = 0; i < 100; ++i) {
      std::string str = "key_" + std::to_string(i);
      Put(str, std::string(kMaxValueLength, '?'));
      Delete(str);
    }
  }

 private:
  void CompareContents() {
#if DUMP_KVS_CONTENTS
    std::set<std::string> map_keys, kvs_keys;

    std::cout << "/==============================================\\\n";
    std::cout << "KVS EXPECTED CONTENTS\n";
    std::cout << "------------------------------------------------\n";
    std::cout << "Entries: " << map_.size() << '\n';
    std::cout << "------------------------------------------------\n";
    for (const auto& [key, value] : map_) {
      std::cout << key << " = [" << value << "]\n";
      map_keys.insert(key);
    }
    std::cout << "\\===============================================/\n";

    std::cout << "/==============================================\\\n";
    std::cout << "KVS ACTUAL CONTENTS\n";
    std::cout << "------------------------------------------------\n";
    std::cout << "Entries: " << kvs_.size() << '\n';
    std::cout << "------------------------------------------------\n";
    for (const auto& item : kvs_) {
      std::cout << item.key() << " = " << item.ValueSize().size() << " B\n";
      kvs_keys.insert(std::string(item.key()));
    }
    std::cout << "\\===============================================/\n";

    auto missing_from_kvs = difference(map_keys, kvs_keys);

    if (!missing_from_kvs.empty()) {
      std::cout << "MISSING FROM KVS: " << missing_from_kvs.size() << '\n';
      for (auto& key : missing_from_kvs) {
        std::cout << key << '\n';
      }
    }

    auto missing_from_map = difference(kvs_keys, map_keys);
    if (!missing_from_map.empty()) {
      std::cout << "MISSING FROM MAP: " << missing_from_map.size() << '\n';
      for (auto& key : missing_from_map) {
        std::cout << key << '\n';
      }
    }
#endif  // DUMP_KVS_CONTENTS

    EXPECT_EQ(map_.size(), kvs_.size());

    size_t count = 0;

    for (auto& item : kvs_) {
      count += 1;

      auto map_entry = map_.find(std::string(item.key()));
      if (map_entry == map_.end()) {
        PW_LOG_CRITICAL(
            "Entry %s missing from map%s",
            item.key(),
            deleted_.count(item.key()) > 0u ? " [was deleted previously]" : "");
      } else if (map_entry != map_.end()) {
        EXPECT_EQ(map_entry->first, item.key());

        char value[kMaxValueLength + 1] = {};
        EXPECT_EQ(OkStatus(),
                  item.Get(as_writable_bytes(span(value))).status());
        EXPECT_EQ(map_entry->second, std::string(value));
      }
    }

    EXPECT_EQ(count, map_.size());
  }

  // Adds a key to the KVS, if there is room for it.
  void Put(const std::string& key, const std::string& value) {
    StartOperation("Put", key);
    EXPECT_LE(value.size(), kMaxValueLength);

    Status result = kvs_.Put(key, as_bytes(span(value)));

    if (key.empty() || key.size() > internal::Entry::kMaxKeyLength) {
      EXPECT_EQ(Status::InvalidArgument(), result);
    } else if (map_.size() == kvs_.max_size()) {
      EXPECT_EQ(Status::ResourceExhausted(), result);
    } else if (result.IsResourceExhausted()) {
      EXPECT_FALSE(map_.empty());
    } else if (result.ok()) {
      map_[key] = value;
      deleted_.erase(key);
    } else {
      PW_LOG_CRITICAL("Put: unhandled result %s", result.str());
      std::abort();
    }

    FinishOperation("Put", result, key);
  }

  // Deletes a key from the KVS if it is present.
  void Delete(const std::string& key) {
    StartOperation("Delete", key);

    Status result = kvs_.Delete(key);

    if (key.empty() || key.size() > internal::Entry::kMaxKeyLength) {
      EXPECT_EQ(Status::InvalidArgument(), result);
    } else if (map_.count(key) == 0) {
      EXPECT_EQ(Status::NotFound(), result);
    } else if (result.ok()) {
      map_.erase(key);

      if (deleted_.count(key) > 0u) {
        PW_LOG_CRITICAL("Deleted key that was already deleted %s", key.c_str());
        std::abort();
      }

      deleted_.insert(key);
    } else if (result.IsResourceExhausted()) {
      PW_LOG_WARN("Delete: RESOURCE_EXHAUSTED could not delete key %s",
                  key.c_str());
    } else {
      PW_LOG_CRITICAL("Delete: unhandled result \"%s\"", result.str());
      std::abort();
    }
    FinishOperation("Delete", result, key);
  }

  void Init() {
    StartOperation("Init");
    Status status = kvs_.Init();
    EXPECT_EQ(OkStatus(), status);
    FinishOperation("Init", status);
  }

  void GCFull() {
    StartOperation("GCFull");
    Status status = kvs_.FullMaintenance();
    EXPECT_EQ(OkStatus(), status);

    KeyValueStore::StorageStats post_stats = kvs_.GetStorageStats();
    if (post_stats.in_use_bytes > ((partition_.size_bytes() * 70) / 100)) {
      EXPECT_EQ(post_stats.reclaimable_bytes, 0U);
    }

    FinishOperation("GCFull", status);
  }

  void GCPartial() {
    StartOperation("GCPartial");
    KeyValueStore::StorageStats pre_stats = kvs_.GetStorageStats();
    Status status = kvs_.PartialMaintenance();
    KeyValueStore::StorageStats post_stats = kvs_.GetStorageStats();
    if (pre_stats.reclaimable_bytes != 0) {
      EXPECT_EQ(OkStatus(), status);
      EXPECT_LT(post_stats.reclaimable_bytes, pre_stats.reclaimable_bytes);
    } else {
      EXPECT_EQ(Status::NotFound(), status);
      EXPECT_EQ(post_stats.reclaimable_bytes, 0U);
    }
    FinishOperation("GCPartial", status);
  }

  // Logs that an operation started and checks that the KVS matches the map. If
  // a key is provided, that is included in the logs.
  void StartOperation(const std::string& operation,
                      const std::string& key = "") {
    count_ += 1;
    if (key.empty()) {
      PW_LOG_DEBUG("[%3u] START %s", count_, operation.c_str());
    } else {
      PW_LOG_DEBUG(
          "[%3u] START %s for '%s'", count_, operation.c_str(), key.c_str());
    }
    AbortIfMismatched("Pre-" + operation);
  }

  // Logs that an operation finished and checks that the KVS matches the map.
  // If a key is provided, that is included in the logs.
  void FinishOperation(const std::string& operation,
                       Status result,
                       const std::string& key = "") {
    if (key.empty()) {
      PW_LOG_DEBUG(
          "[%3u] FINISH %s <%s>", count_, operation.c_str(), result.str());
    } else {
      PW_LOG_DEBUG("[%3u] FINISH %s <%s> for '%s'",
                   count_,
                   operation.c_str(),
                   result.str(),
                   key.c_str());
    }
    AbortIfMismatched(operation);
  }

  bool empty() const { return map_.empty(); }

  std::string RandomPresentKey() const {
    return map_.empty() ? "" : map_.begin()->second;
  }

  void AbortIfMismatched(const std::string& stage) {
    if (kvs_.size() != map_.size()) {
      PW_LOG_CRITICAL("%s: size mismatch", stage.c_str());
      CompareContents();
      std::abort();
    }
  }

  static constexpr size_t kMaxValueLength = 64;

  static FakeFlashMemoryBuffer<kParams.sector_size,
                               (kParams.sector_count * kParams.redundancy)>
      flash_;

  FlashPartitionWithStatsBuffer<kMaxEntries> partition_;

  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors, kParams.redundancy> kvs_;
  std::unordered_map<std::string, std::string> map_;
  std::unordered_set<std::string> deleted_;
  unsigned count_ = 0;
};

template <const TestParameters& kParams>
FakeFlashMemoryBuffer<kParams.sector_size,
                      (kParams.sector_count * kParams.redundancy)>
    KvsTester<kParams>::flash_ =
        FakeFlashMemoryBuffer<kParams.sector_size,
                              (kParams.sector_count * kParams.redundancy)>(
            kParams.sector_alignment);

#define _TEST(fixture, test, ...)                  \
  _TEST_VARIANT(fixture, test, test, __VA_ARGS__); \
  static_assert(true, "Macros must be terminated with a semicolon")

#define _TEST_VARIANT(fixture, test, variant, ...)                     \
  TEST_F(fixture, test##variant) { tester_.Test_##test(__VA_ARGS__); } \
  static_assert(true, "Macros must be terminated with a semicolon")

// Defines a test fixture that runs all tests against a flash with the specified
// parameters.
#define RUN_TESTS_WITH_PARAMETERS(name, ...)                                  \
  class name : public ::testing::Test {                                       \
   protected:                                                                 \
    static constexpr TestParameters kParams = {__VA_ARGS__};                  \
                                                                              \
    KvsTester<kParams> tester_;                                               \
  };                                                                          \
  /* Run each test defined in the KvsTester class with these parameters. */   \
  _TEST(name, Put);                                                           \
  _TEST(name, PutAndDelete_RelocateDeletedEntriesShouldStayDeleted);          \
  _TEST_VARIANT(name, RandomValidInputs, 1, 1000, 6006411, kNone);            \
  _TEST_VARIANT(name, RandomValidInputs, 1WithReinit, 500, 6006411, kReinit); \
  _TEST_VARIANT(name, RandomValidInputs, 2, 100, 123, kNone);                 \
  _TEST_VARIANT(name, RandomValidInputs, 2WithReinit, 100, 123, kReinit);     \
  _TEST_VARIANT(name,                                                         \
                RandomValidInputs,                                            \
                1ReinitFullGC,                                                \
                300,                                                          \
                6006411,                                                      \
                kReinitWithFullGC);                                           \
  _TEST_VARIANT(                                                              \
      name, RandomValidInputs, 2ReinitFullGC, 300, 123, kReinitWithFullGC);   \
  _TEST_VARIANT(name,                                                         \
                RandomValidInputs,                                            \
                1ReinitPartialGC,                                             \
                100,                                                          \
                6006411,                                                      \
                kReinitWithPartialGC);                                        \
  _TEST_VARIANT(name,                                                         \
                RandomValidInputs,                                            \
                2ReinitPartialGC,                                             \
                200,                                                          \
                123,                                                          \
                kReinitWithPartialGC);                                        \
  static_assert(true, "Don't forget a semicolon!")

RUN_TESTS_WITH_PARAMETERS(Basic,
                          .sector_size = 4 * 1024,
                          .sector_count = 4,
                          .sector_alignment = 16,
                          .redundancy = 1,
                          .partition_start_sector = 0,
                          .partition_sector_count = 4,
                          .partition_alignment = 16);

RUN_TESTS_WITH_PARAMETERS(BasicRedundant,
                          .sector_size = 4 * 1024,
                          .sector_count = 4,
                          .sector_alignment = 16,
                          .redundancy = 2,
                          .partition_start_sector = 0,
                          .partition_sector_count = 4,
                          .partition_alignment = 16);

RUN_TESTS_WITH_PARAMETERS(LotsOfSmallSectors,
                          .sector_size = 160,
                          .sector_count = 100,
                          .sector_alignment = 32,
                          .redundancy = 1,
                          .partition_start_sector = 5,
                          .partition_sector_count = 95,
                          .partition_alignment = 32);

RUN_TESTS_WITH_PARAMETERS(LotsOfSmallSectorsRedundant,
                          .sector_size = 160,
                          .sector_count = 100,
                          .sector_alignment = 32,
                          .redundancy = 2,
                          .partition_start_sector = 5,
                          .partition_sector_count = 95,
                          .partition_alignment = 32);

RUN_TESTS_WITH_PARAMETERS(OnlyTwoSectors,
                          .sector_size = 4 * 1024,
                          .sector_count = 20,
                          .sector_alignment = 16,
                          .redundancy = 1,
                          .partition_start_sector = 18,
                          .partition_sector_count = 2,
                          .partition_alignment = 64);

}  // namespace
}  // namespace pw::kvs
