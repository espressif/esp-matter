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

#include <cstddef>

#include "gtest/gtest.h"
#include "pw_build/test_blob.h"

namespace pw::build {
namespace {

static_assert(test::ns::kFirstBlob0123.size() == 4);
static_assert(test::ns::kFirstBlob0123.data() != nullptr);

static_assert(test::ns::kSecondBlob0123.size() == 4);
static_assert(test::ns::kSecondBlob0123.data() != nullptr);

TEST(CcBlobLibraryTest, FirstBlobContentsMatch) {
  EXPECT_EQ(test::ns::kFirstBlob0123[0], std::byte{0});
  EXPECT_EQ(test::ns::kFirstBlob0123[1], std::byte{1});
  EXPECT_EQ(test::ns::kFirstBlob0123[2], std::byte{2});
  EXPECT_EQ(test::ns::kFirstBlob0123[3], std::byte{3});
}

TEST(CcBlobLibraryTest, SecondBlobContentsMatch) {
  EXPECT_EQ(test::ns::kSecondBlob0123[0], std::byte{0});
  EXPECT_EQ(test::ns::kSecondBlob0123[1], std::byte{1});
  EXPECT_EQ(test::ns::kSecondBlob0123[2], std::byte{2});
  EXPECT_EQ(test::ns::kSecondBlob0123[3], std::byte{3});
}

TEST(CcBlobLibraryTest, FirstBlobAlignedTo512) {
  // This checks that the variable is aligned to 512, but cannot guarantee that
  // alignas was specified correctly, since it could be aligned to 512 by
  // coincidence.
  const uintptr_t addr = reinterpret_cast<uintptr_t>(&test::ns::kFirstBlob0123);
  constexpr uintptr_t kAlignmentMask = static_cast<uintptr_t>(512 - 1);
  EXPECT_EQ(addr & kAlignmentMask, 0u);
}

}  // namespace
}  // namespace pw::build
