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

#include "pw_allocator/freelist_heap.h"

#include "gtest/gtest.h"
#include "pw_span/span.h"

namespace pw::allocator {

TEST(FreeListHeap, CanAllocate) {
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = 512;
  alignas(Block) std::byte buf[N] = {std::byte(0)};

  FreeListHeapBuffer allocator(buf);

  void* ptr = allocator.Allocate(kAllocSize);

  ASSERT_NE(ptr, nullptr);
  // In this case, the allocator should be returning us the start of the chunk.
  EXPECT_EQ(ptr, &buf[0] + sizeof(Block) + PW_ALLOCATOR_POISON_OFFSET);
}

TEST(FreeListHeap, AllocationsDontOverlap) {
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = 512;
  alignas(Block) std::byte buf[N] = {std::byte(0)};

  FreeListHeapBuffer allocator(buf);

  void* ptr1 = allocator.Allocate(kAllocSize);
  void* ptr2 = allocator.Allocate(kAllocSize);

  ASSERT_NE(ptr1, nullptr);
  ASSERT_NE(ptr2, nullptr);

  uintptr_t ptr1_start = reinterpret_cast<uintptr_t>(ptr1);
  uintptr_t ptr1_end = ptr1_start + kAllocSize;
  uintptr_t ptr2_start = reinterpret_cast<uintptr_t>(ptr2);

  EXPECT_GT(ptr2_start, ptr1_end);
}

TEST(FreeListHeap, CanFreeAndRealloc) {
  // There's not really a nice way to test that Free works, apart from to try
  // and get that value back again.
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = 512;
  alignas(Block) std::byte buf[N] = {std::byte(0)};

  FreeListHeapBuffer allocator(buf);

  void* ptr1 = allocator.Allocate(kAllocSize);
  allocator.Free(ptr1);
  void* ptr2 = allocator.Allocate(kAllocSize);

  EXPECT_EQ(ptr1, ptr2);
}

TEST(FreeListHeap, ReturnsNullWhenAllocationTooLarge) {
  constexpr size_t N = 2048;
  alignas(Block) std::byte buf[N] = {std::byte(0)};

  FreeListHeapBuffer allocator(buf);

  EXPECT_EQ(allocator.Allocate(N), nullptr);
}

TEST(FreeListHeap, ReturnsNullWhenFull) {
  constexpr size_t N = 2048;
  alignas(Block) std::byte buf[N] = {std::byte(0)};

  FreeListHeapBuffer allocator(buf);

  EXPECT_NE(
      allocator.Allocate(N - sizeof(Block) - 2 * PW_ALLOCATOR_POISON_OFFSET),
      nullptr);
  EXPECT_EQ(allocator.Allocate(1), nullptr);
}

TEST(FreeListHeap, ReturnedPointersAreAligned) {
  constexpr size_t N = 2048;
  alignas(Block) std::byte buf[N] = {std::byte(0)};

  FreeListHeapBuffer allocator(buf);

  void* ptr1 = allocator.Allocate(1);

  // Should be aligned to native pointer alignment
  uintptr_t ptr1_start = reinterpret_cast<uintptr_t>(ptr1);
  size_t alignment = alignof(void*);

  EXPECT_EQ(ptr1_start % alignment, static_cast<size_t>(0));

  void* ptr2 = allocator.Allocate(1);
  uintptr_t ptr2_start = reinterpret_cast<uintptr_t>(ptr2);

  EXPECT_EQ(ptr2_start % alignment, static_cast<size_t>(0));
}

#if defined(CHECK_TEST_CRASHES) && CHECK_TEST_CRASHES

// TODO(amontanez): Ensure that this test triggers an assert.
TEST(FreeListHeap, CannotFreeNonOwnedPointer) {
  // This is a nasty one to test without looking at the internals of FreeList.
  // We can cheat; create a heap, allocate it all, and try and return something
  // random to it. Try allocating again, and check that we get nullptr back.
  constexpr size_t N = 2048;
  alignas(Block) std::byte buf[N] = {std::byte(0)};

  FreeListHeapBuffer allocator(buf);

  void* ptr =
      allocator.Allocate(N - sizeof(Block) - 2 * PW_ALLOCATOR_POISON_OFFSET);

  ASSERT_NE(ptr, nullptr);

  // Free some random address past the end
  allocator.Free(static_cast<std::byte*>(ptr) + N * 2);

  void* ptr_ahead = allocator.Allocate(1);
  EXPECT_EQ(ptr_ahead, nullptr);

  // And try before
  allocator.Free(static_cast<std::byte*>(ptr) - N);

  void* ptr_before = allocator.Allocate(1);
  EXPECT_EQ(ptr_before, nullptr);
}
#endif  // CHECK_TEST_CRASHES

TEST(FreeListHeap, CanRealloc) {
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = 512;
  constexpr size_t kNewAllocSize = 768;
  alignas(Block) std::byte buf[N] = {std::byte(1)};

  FreeListHeapBuffer allocator(buf);

  void* ptr1 = allocator.Allocate(kAllocSize);
  void* ptr2 = allocator.Realloc(ptr1, kNewAllocSize);

  ASSERT_NE(ptr1, nullptr);
  ASSERT_NE(ptr2, nullptr);
}

TEST(FreeListHeap, ReallocHasSameContent) {
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = sizeof(int);
  constexpr size_t kNewAllocSize = sizeof(int) * 2;
  alignas(Block) std::byte buf[N] = {std::byte(1)};
  // Data inside the allocated block.
  std::byte data1[kAllocSize];
  // Data inside the reallocated block.
  std::byte data2[kAllocSize];

  FreeListHeapBuffer allocator(buf);

  int* ptr1 = reinterpret_cast<int*>(allocator.Allocate(kAllocSize));
  *ptr1 = 42;
  memcpy(data1, ptr1, kAllocSize);
  int* ptr2 = reinterpret_cast<int*>(allocator.Realloc(ptr1, kNewAllocSize));
  memcpy(data2, ptr2, kAllocSize);

  ASSERT_NE(ptr1, nullptr);
  ASSERT_NE(ptr2, nullptr);
  // Verify that data inside the allocated and reallocated chunks are the same.
  EXPECT_EQ(std::memcmp(data1, data2, kAllocSize), 0);
}

TEST(FreeListHeap, ReturnsNullReallocFreedPointer) {
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = 512;
  constexpr size_t kNewAllocSize = 256;
  alignas(Block) std::byte buf[N] = {std::byte(0)};

  FreeListHeapBuffer allocator(buf);

  void* ptr1 = allocator.Allocate(kAllocSize);
  allocator.Free(ptr1);
  void* ptr2 = allocator.Realloc(ptr1, kNewAllocSize);

  EXPECT_EQ(nullptr, ptr2);
}

TEST(FreeListHeap, ReallocSmallerSize) {
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = 512;
  constexpr size_t kNewAllocSize = 256;
  alignas(Block) std::byte buf[N] = {std::byte(0)};

  FreeListHeapBuffer allocator(buf);

  void* ptr1 = allocator.Allocate(kAllocSize);
  void* ptr2 = allocator.Realloc(ptr1, kNewAllocSize);

  // For smaller sizes, Realloc will not shrink the block.
  EXPECT_EQ(ptr1, ptr2);
}

TEST(FreeListHeap, ReallocTooLarge) {
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = 512;
  constexpr size_t kNewAllocSize = 4096;
  alignas(Block) std::byte buf[N] = {std::byte(0)};

  FreeListHeapBuffer allocator(buf);

  void* ptr1 = allocator.Allocate(kAllocSize);
  void* ptr2 = allocator.Realloc(ptr1, kNewAllocSize);

  // Realloc() will not invalidate the original pointer if Reallc() fails
  EXPECT_NE(nullptr, ptr1);
  EXPECT_EQ(nullptr, ptr2);
}

TEST(FreeListHeap, CanCalloc) {
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = 128;
  constexpr size_t kNum = 4;
  constexpr int size = kNum * kAllocSize;
  alignas(Block) std::byte buf[N] = {std::byte(1)};
  constexpr std::byte zero{0};

  FreeListHeapBuffer allocator(buf);

  std::byte* ptr1 =
      reinterpret_cast<std::byte*>(allocator.Calloc(kNum, kAllocSize));

  // Calloc'd content is zero.
  for (int i = 0; i < size; i++) {
    EXPECT_EQ(ptr1[i], zero);
  }
}

TEST(FreeListHeap, CanCallocWeirdSize) {
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = 143;
  constexpr size_t kNum = 3;
  constexpr int size = kNum * kAllocSize;
  alignas(Block) std::byte buf[N] = {std::byte(132)};
  constexpr std::byte zero{0};

  FreeListHeapBuffer allocator(buf);

  std::byte* ptr1 =
      reinterpret_cast<std::byte*>(allocator.Calloc(kNum, kAllocSize));

  // Calloc'd content is zero.
  for (int i = 0; i < size; i++) {
    EXPECT_EQ(ptr1[i], zero);
  }
}

TEST(FreeListHeap, CallocTooLarge) {
  constexpr size_t N = 2048;
  constexpr size_t kAllocSize = 2049;
  alignas(Block) std::byte buf[N] = {std::byte(1)};

  FreeListHeapBuffer allocator(buf);

  EXPECT_EQ(allocator.Calloc(1, kAllocSize), nullptr);
}
}  // namespace pw::allocator
