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

#include "pw_malloc_freelist/freelist_malloc.h"

#include <memory>

#include "gtest/gtest.h"
#include "pw_allocator/freelist_heap.h"
#include "pw_span/span.h"

namespace pw::allocator {

TEST(FreeListMalloc, ReplacingMalloc) {
  constexpr size_t kAllocSize = 256;
  constexpr size_t kReallocSize = 512;
  constexpr size_t kCallocNum = 4;
  constexpr size_t kCallocSize = 64;
  constexpr size_t zero = 0;

  auto deleter = [](void* ptr) { free(ptr); };

  std::unique_ptr<void, decltype(deleter)> ptr1(malloc(kAllocSize), deleter);
  const FreeListHeap::HeapStats& freelist_heap_stats =
      pw_freelist_heap->heap_stats();
  ASSERT_NE(ptr1.get(), nullptr);
  EXPECT_EQ(freelist_heap_stats.bytes_allocated, kAllocSize);
  EXPECT_EQ(freelist_heap_stats.cumulative_allocated, kAllocSize);
  EXPECT_EQ(freelist_heap_stats.cumulative_freed, zero);

  std::unique_ptr<void, decltype(deleter)> ptr2(
      realloc(ptr1.release(), kReallocSize), deleter);
  ASSERT_NE(ptr2.get(), nullptr);
  EXPECT_EQ(freelist_heap_stats.bytes_allocated, kReallocSize);
  EXPECT_EQ(freelist_heap_stats.cumulative_allocated,
            kAllocSize + kReallocSize);
  EXPECT_EQ(freelist_heap_stats.cumulative_freed, kAllocSize);

  std::unique_ptr<void, decltype(deleter)> ptr3(calloc(kCallocNum, kCallocSize),
                                                deleter);
  ASSERT_NE(ptr3.get(), nullptr);
  EXPECT_EQ(freelist_heap_stats.bytes_allocated,
            kReallocSize + kCallocNum * kCallocSize);
  EXPECT_EQ(freelist_heap_stats.cumulative_allocated,
            kAllocSize + kReallocSize + kCallocNum * kCallocSize);
  EXPECT_EQ(freelist_heap_stats.cumulative_freed, kAllocSize);
  free(ptr2.release());
  EXPECT_EQ(freelist_heap_stats.bytes_allocated, kCallocNum * kCallocSize);
  EXPECT_EQ(freelist_heap_stats.cumulative_allocated,
            kAllocSize + kReallocSize + kCallocNum * kCallocSize);
  EXPECT_EQ(freelist_heap_stats.cumulative_freed, kAllocSize + kReallocSize);
  free(ptr3.release());
  EXPECT_EQ(freelist_heap_stats.bytes_allocated, zero);
  EXPECT_EQ(freelist_heap_stats.cumulative_allocated,
            kAllocSize + kReallocSize + kCallocNum * kCallocSize);
  EXPECT_EQ(freelist_heap_stats.cumulative_freed,
            kAllocSize + kReallocSize + kCallocNum * kCallocSize);
}

}  // namespace pw::allocator
