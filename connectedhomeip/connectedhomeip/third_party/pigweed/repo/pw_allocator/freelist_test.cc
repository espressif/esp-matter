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

#include "pw_allocator/freelist.h"

#include "gtest/gtest.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

using std::byte;

namespace pw::allocator {

static const size_t SIZE = 8;
static const std::array<size_t, SIZE> example_sizes = {
    64, 128, 256, 512, 1024, 2048, 4096, 8192};

TEST(FreeList, EmptyListHasNoMembers) {
  FreeListBuffer<SIZE> list(example_sizes);

  auto item = list.FindChunk(4);
  EXPECT_EQ(item.size(), static_cast<size_t>(0));
  item = list.FindChunk(128);
  EXPECT_EQ(item.size(), static_cast<size_t>(0));
}

TEST(FreeList, CanRetrieveAddedMember) {
  FreeListBuffer<SIZE> list(example_sizes);
  constexpr size_t kN = 512;

  byte data[kN] = {std::byte(0)};

  auto status = list.AddChunk(span(data, kN));
  EXPECT_EQ(status, OkStatus());

  auto item = list.FindChunk(kN);
  EXPECT_EQ(item.size(), kN);
  EXPECT_EQ(item.data(), data);
}

TEST(FreeList, CanRetrieveAddedMemberForSmallerSize) {
  FreeListBuffer<SIZE> list(example_sizes);
  constexpr size_t kN = 512;

  byte data[kN] = {std::byte(0)};

  ASSERT_EQ(OkStatus(), list.AddChunk(span(data, kN)));
  auto item = list.FindChunk(kN / 2);
  EXPECT_EQ(item.size(), kN);
  EXPECT_EQ(item.data(), data);
}

TEST(FreeList, CanRemoveItem) {
  FreeListBuffer<SIZE> list(example_sizes);
  constexpr size_t kN = 512;

  byte data[kN] = {std::byte(0)};

  ASSERT_EQ(OkStatus(), list.AddChunk(span(data, kN)));
  auto status = list.RemoveChunk(span(data, kN));
  EXPECT_EQ(status, OkStatus());

  auto item = list.FindChunk(kN);
  EXPECT_EQ(item.size(), static_cast<size_t>(0));
}

TEST(FreeList, FindReturnsSmallestChunk) {
  FreeListBuffer<SIZE> list(example_sizes);
  constexpr size_t kN1 = 512;
  constexpr size_t kN2 = 1024;

  byte data1[kN1] = {std::byte(0)};
  byte data2[kN2] = {std::byte(0)};

  ASSERT_EQ(OkStatus(), list.AddChunk(span(data1, kN1)));
  ASSERT_EQ(OkStatus(), list.AddChunk(span(data2, kN2)));

  auto chunk = list.FindChunk(kN1 / 2);
  EXPECT_EQ(chunk.size(), kN1);
  EXPECT_EQ(chunk.data(), data1);

  chunk = list.FindChunk(kN1);
  EXPECT_EQ(chunk.size(), kN1);
  EXPECT_EQ(chunk.data(), data1);

  chunk = list.FindChunk(kN1 + 1);
  EXPECT_EQ(chunk.size(), kN2);
  EXPECT_EQ(chunk.data(), data2);
}

TEST(FreeList, FindReturnsCorrectChunkInSameBucket) {
  // If we have two values in the same bucket, ensure that the allocation will
  // pick an appropriately sized one.
  FreeListBuffer<SIZE> list(example_sizes);
  constexpr size_t kN1 = 512;
  constexpr size_t kN2 = 257;

  byte data1[kN1] = {std::byte(0)};
  byte data2[kN2] = {std::byte(0)};

  // List should now be 257 -> 512 -> NULL
  ASSERT_EQ(OkStatus(), list.AddChunk(span(data1, kN1)));
  ASSERT_EQ(OkStatus(), list.AddChunk(span(data2, kN2)));

  auto chunk = list.FindChunk(kN2 + 1);
  EXPECT_EQ(chunk.size(), kN1);
}

TEST(FreeList, FindCanMoveUpThroughBuckets) {
  // Ensure that finding a chunk will move up through buckets if no appropriate
  // chunks were found in a given bucket
  FreeListBuffer<SIZE> list(example_sizes);
  constexpr size_t kN1 = 257;
  constexpr size_t kN2 = 513;

  byte data1[kN1] = {std::byte(0)};
  byte data2[kN2] = {std::byte(0)};

  // List should now be:
  // bkt[3] (257 bytes up to 512 bytes) -> 257 -> NULL
  // bkt[4] (513 bytes up to 1024 bytes) -> 513 -> NULL
  ASSERT_EQ(OkStatus(), list.AddChunk(span(data1, kN1)));
  ASSERT_EQ(OkStatus(), list.AddChunk(span(data2, kN2)));

  // Request a 300 byte chunk. This should return the 513 byte one
  auto chunk = list.FindChunk(kN1 + 1);
  EXPECT_EQ(chunk.size(), kN2);
}

TEST(FreeList, RemoveUnknownChunkReturnsNotFound) {
  FreeListBuffer<SIZE> list(example_sizes);
  constexpr size_t kN = 512;

  byte data[kN] = {std::byte(0)};
  byte data2[kN] = {std::byte(0)};

  ASSERT_EQ(OkStatus(), list.AddChunk(span(data, kN)));
  auto status = list.RemoveChunk(span(data2, kN));
  EXPECT_EQ(status, Status::NotFound());
}

TEST(FreeList, CanStoreMultipleChunksPerBucket) {
  FreeListBuffer<SIZE> list(example_sizes);
  constexpr size_t kN = 512;

  byte data1[kN] = {std::byte(0)};
  byte data2[kN] = {std::byte(0)};

  ASSERT_EQ(OkStatus(), list.AddChunk(span(data1, kN)));
  ASSERT_EQ(OkStatus(), list.AddChunk(span(data2, kN)));

  auto chunk1 = list.FindChunk(kN);
  ASSERT_EQ(OkStatus(), list.RemoveChunk(chunk1));
  auto chunk2 = list.FindChunk(kN);
  ASSERT_EQ(OkStatus(), list.RemoveChunk(chunk2));

  // Ordering of the chunks doesn't matter
  EXPECT_TRUE(chunk1.data() != chunk2.data());
  EXPECT_TRUE(chunk1.data() == data1 || chunk1.data() == data2);
  EXPECT_TRUE(chunk2.data() == data1 || chunk2.data() == data2);
}

}  // namespace pw::allocator
