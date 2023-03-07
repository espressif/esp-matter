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

#pragma once

#include <cstddef>

#include "pw_allocator/block.h"
#include "pw_allocator/freelist.h"
#include "pw_span/span.h"

namespace pw::allocator {

class FreeListHeap {
 public:
  template <size_t kNumBuckets>
  friend class FreeListHeapBuffer;
  struct HeapStats {
    size_t total_bytes;
    size_t bytes_allocated;
    size_t cumulative_allocated;
    size_t cumulative_freed;
    size_t total_allocate_calls;
    size_t total_free_calls;
  };
  FreeListHeap(span<std::byte> region, FreeList& freelist);

  void* Allocate(size_t size);
  void Free(void* ptr);
  void* Realloc(void* ptr, size_t size);
  void* Calloc(size_t num, size_t size);

  void LogHeapStats();

 private:
  span<std::byte> BlockToSpan(Block* block) {
    return span<std::byte>(block->UsableSpace(), block->InnerSize());
  }

  void InvalidFreeCrash();

  span<std::byte> region_;
  FreeList& freelist_;
  HeapStats heap_stats_;
};

template <size_t kNumBuckets = 6>
class FreeListHeapBuffer {
 public:
  static constexpr std::array<size_t, kNumBuckets> defaultBuckets{
      16, 32, 64, 128, 256, 512};

  FreeListHeapBuffer(span<std::byte> region)
      : freelist_(defaultBuckets), heap_(region, freelist_) {}

  void* Allocate(size_t size) { return heap_.Allocate(size); }
  void Free(void* ptr) { heap_.Free(ptr); }
  void* Realloc(void* ptr, size_t size) { return heap_.Realloc(ptr, size); }
  void* Calloc(size_t num, size_t size) { return heap_.Calloc(num, size); }

  const FreeListHeap::HeapStats& heap_stats() const {
    return heap_.heap_stats_;
  }

  void LogHeapStats() { heap_.LogHeapStats(); }

 private:
  FreeListBuffer<kNumBuckets> freelist_;
  FreeListHeap heap_;
};

}  // namespace pw::allocator
