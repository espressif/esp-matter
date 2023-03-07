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
#include "pw_persistent_ram/persistent.h"

#include <type_traits>

#include "gtest/gtest.h"
#include "pw_random/xor_shift.h"

namespace pw::persistent_ram {
namespace {

class PersistentTest : public ::testing::Test {
 protected:
  PersistentTest() { ZeroPersistentMemory(); }

  // Emulate invalidation of persistent section(s).
  void ZeroPersistentMemory() { memset(&buffer_, 0, sizeof(buffer_)); }

  // Allocate a chunk of aligned storage that can be independently controlled.
  std::aligned_storage_t<sizeof(Persistent<uint32_t>),
                         alignof(Persistent<uint32_t>)>
      buffer_;
};

TEST_F(PersistentTest, DefaultConstructionAndDestruction) {
  {  // Emulate a boot where the persistent sections were invalidated.
    // Although the fixture always does this, we do this an extra time to be
    // 100% confident that an integrity check cannot be accidentally selected
    // which results in reporting there is valid data when zero'd.
    ZeroPersistentMemory();
    auto& persistent = *(new (&buffer_) Persistent<uint32_t>());
    EXPECT_FALSE(persistent.has_value());

    persistent = 42;
    ASSERT_TRUE(persistent.has_value());
    EXPECT_EQ(42u, persistent.value());

    persistent.~Persistent();  // Emulate shutdown / global destructors.
  }

  {  // Emulate a boot where persistent memory was kept as is.
    auto& persistent = *(new (&buffer_) Persistent<uint32_t>());
    ASSERT_TRUE(persistent.has_value());
    EXPECT_EQ(42u, persistent.value());
  }
}

TEST_F(PersistentTest, Reset) {
  {  // Emulate a boot where the persistent sections were invalidated.
    auto& persistent = *(new (&buffer_) Persistent<uint32_t>());
    persistent = 42u;
    EXPECT_TRUE(persistent.has_value());
    persistent.Invalidate();

    persistent.~Persistent();  // Emulate shutdown / global destructors.
  }

  {  // Emulate a boot where persistent memory was kept as is.
    auto& persistent = *(new (&buffer_) Persistent<uint32_t>());
    EXPECT_FALSE(persistent.has_value());
  }
}

TEST_F(PersistentTest, Emplace) {
  auto& persistent = *(new (&buffer_) Persistent<uint32_t>());
  EXPECT_FALSE(persistent.has_value());

  persistent.emplace(42u);
  ASSERT_TRUE(persistent.has_value());
  EXPECT_EQ(42u, persistent.value());
}

class MutablePersistentTest : public ::testing::Test {
 protected:
  struct Coordinate {
    int x;
    int y;
    int z;
  };
  MutablePersistentTest() { ZeroPersistentMemory(); }

  // Emulate invalidation of persistent section(s).
  void ZeroPersistentMemory() { memset(&buffer_, 0, sizeof(buffer_)); }
  void RandomFillMemory() {
    random::XorShiftStarRng64 rng(0x9ad75);
    rng.Get(span<std::byte>(reinterpret_cast<std::byte*>(&buffer_),
                            sizeof(buffer_)));
  }

  // Allocate a chunk of aligned storage that can be independently controlled.
  std::aligned_storage_t<sizeof(Persistent<Coordinate>),
                         alignof(Persistent<Coordinate>)>
      buffer_;
};

TEST_F(MutablePersistentTest, DefaultConstructionAndDestruction) {
  {
    // Emulate a boot where the persistent sections were invalidated.
    // Although the fixture always does this, we do this an extra time to be
    // 100% confident that an integrity check cannot be accidentally selected
    // which results in reporting there is valid data when zero'd.
    ZeroPersistentMemory();
    auto& persistent = *(new (&buffer_) Persistent<Coordinate>());
    EXPECT_FALSE(persistent.has_value());

    // Default construct of a Coordinate.
    persistent.emplace(Coordinate({.x = 5, .y = 6, .z = 7}));
    ASSERT_TRUE(persistent.has_value());
    {
      auto mutable_persistent = persistent.mutator();
      mutable_persistent->x = 42;
      (*mutable_persistent).y = 1337;
      mutable_persistent->z = -99;
      ASSERT_FALSE(persistent.has_value());
    }

    EXPECT_EQ(1337, persistent.value().y);
    EXPECT_EQ(-99, persistent.value().z);

    persistent.~Persistent();  // Emulate shutdown / global destructors.
  }

  {
    // Emulate a boot where persistent memory was kept as is.
    auto& persistent = *(new (&buffer_) Persistent<Coordinate>());
    ASSERT_TRUE(persistent.has_value());
    EXPECT_EQ(42, persistent.value().x);
  }
}

TEST_F(MutablePersistentTest, ResetObject) {
  {
    // Emulate a boot where the persistent sections were lost and ended up in
    // random data.
    RandomFillMemory();
    auto& persistent = *(new (&buffer_) Persistent<Coordinate>());

    // Default construct of a Coordinate.
    ASSERT_FALSE(persistent.has_value());
    {
      auto mutable_persistent = persistent.mutator(GetterAction::kReset);
      mutable_persistent->x = 42;
    }

    EXPECT_EQ(42, persistent.value().x);
    EXPECT_EQ(0, persistent.value().y);
    EXPECT_EQ(0, persistent.value().z);

    persistent.~Persistent();  // Emulate shutdown / global destructors.
  }

  {
    // Emulate a boot where persistent memory was kept as is.
    auto& persistent = *(new (&buffer_) Persistent<Coordinate>());
    ASSERT_TRUE(persistent.has_value());
    EXPECT_EQ(42, persistent.value().x);
    EXPECT_EQ(0, persistent.value().y);
  }
}

}  // namespace
}  // namespace pw::persistent_ram
