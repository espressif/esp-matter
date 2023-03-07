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

#include <cstring>

#include "pw_assert/check.h"
#include "pw_log/log.h"

namespace pw::allocator {

FreeListHeap::FreeListHeap(span<std::byte> region, FreeList& freelist)
    : freelist_(freelist), heap_stats_() {
  Block* block;
  PW_CHECK_OK(
      Block::Init(region, &block),
      "Failed to initialize FreeListHeap region; misaligned or too small");

  freelist_.AddChunk(BlockToSpan(block))
      .IgnoreError();  // TODO(b/242598609): Handle Status properly

  region_ = region;
  heap_stats_.total_bytes = region.size();
}

void* FreeListHeap::Allocate(size_t size) {
  // Find a chunk in the freelist. Split it if needed, then return

  auto chunk = freelist_.FindChunk(size);

  if (chunk.data() == nullptr) {
    return nullptr;
  }
  freelist_.RemoveChunk(chunk)
      .IgnoreError();  // TODO(b/242598609): Handle Status properly

  Block* chunk_block = Block::FromUsableSpace(chunk.data());

  chunk_block->CrashIfInvalid();

  // Split that chunk. If there's a leftover chunk, add it to the freelist
  Block* leftover;
  auto status = chunk_block->Split(size, &leftover);
  if (status == PW_STATUS_OK) {
    freelist_.AddChunk(BlockToSpan(leftover))
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }

  chunk_block->MarkUsed();

  heap_stats_.bytes_allocated += size;
  heap_stats_.cumulative_allocated += size;
  heap_stats_.total_allocate_calls += 1;

  return chunk_block->UsableSpace();
}

void FreeListHeap::Free(void* ptr) {
  std::byte* bytes = static_cast<std::byte*>(ptr);

  if (bytes < region_.data() || bytes >= region_.data() + region_.size()) {
    InvalidFreeCrash();
    return;
  }

  Block* chunk_block = Block::FromUsableSpace(bytes);
  chunk_block->CrashIfInvalid();

  size_t size_freed = chunk_block->InnerSize();
  // Ensure that the block is in-use
  if (!chunk_block->Used()) {
    InvalidFreeCrash();
    return;
  }
  chunk_block->MarkFree();
  // Can we combine with the left or right blocks?
  Block* prev = chunk_block->Prev();
  Block* next = nullptr;

  if (!chunk_block->Last()) {
    next = chunk_block->Next();
  }

  if (prev != nullptr && !prev->Used()) {
    // Remove from freelist and merge
    freelist_.RemoveChunk(BlockToSpan(prev))
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    chunk_block->MergePrev()
        .IgnoreError();  // TODO(b/242598609): Handle Status properly

    // chunk_block is now invalid; prev now encompasses it.
    chunk_block = prev;
  }

  if (next != nullptr && !next->Used()) {
    freelist_.RemoveChunk(BlockToSpan(next))
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    chunk_block->MergeNext()
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }
  // Add back to the freelist
  freelist_.AddChunk(BlockToSpan(chunk_block))
      .IgnoreError();  // TODO(b/242598609): Handle Status properly

  heap_stats_.bytes_allocated -= size_freed;
  heap_stats_.cumulative_freed += size_freed;
  heap_stats_.total_free_calls += 1;
}

// Follows constract of the C standard realloc() function
// If ptr is free'd, will return nullptr.
void* FreeListHeap::Realloc(void* ptr, size_t size) {
  if (size == 0) {
    Free(ptr);
    return nullptr;
  }

  // If the pointer is nullptr, allocate a new memory.
  if (ptr == nullptr) {
    return Allocate(size);
  }

  std::byte* bytes = static_cast<std::byte*>(ptr);

  // TODO(chenghanzh): Enhance with debug information for out-of-range and more.
  if (bytes < region_.data() || bytes >= region_.data() + region_.size()) {
    return nullptr;
  }

  Block* chunk_block = Block::FromUsableSpace(bytes);
  if (!chunk_block->Used()) {
    return nullptr;
  }
  size_t old_size = chunk_block->InnerSize();

  // Do nothing and return ptr if the required memory size is smaller than
  // the current size.
  // TODO(keir): Currently do not support shrink of memory chunk.
  if (old_size >= size) {
    return ptr;
  }

  void* new_ptr = Allocate(size);
  // Don't invalidate ptr if Allocate(size) fails to initilize the memory.
  if (new_ptr == nullptr) {
    return nullptr;
  }
  memcpy(new_ptr, ptr, old_size);

  Free(ptr);
  return new_ptr;
}

void* FreeListHeap::Calloc(size_t num, size_t size) {
  void* ptr = Allocate(num * size);
  if (ptr != nullptr) {
    memset(ptr, 0, num * size);
  }
  return ptr;
}

void FreeListHeap::LogHeapStats() {
  PW_LOG_INFO(" ");
  PW_LOG_INFO("    The current heap information: ");
  PW_LOG_INFO("          The total heap size is %u bytes.",
              static_cast<unsigned int>(heap_stats_.total_bytes));
  PW_LOG_INFO("          The current allocated heap memory is %u bytes.",
              static_cast<unsigned int>(heap_stats_.bytes_allocated));
  PW_LOG_INFO("          The cumulative allocated heap memory is %u bytes.",
              static_cast<unsigned int>(heap_stats_.cumulative_allocated));
  PW_LOG_INFO("          The cumulative freed heap memory is %u bytes.",
              static_cast<unsigned int>(heap_stats_.cumulative_freed));
  PW_LOG_INFO(
      "          malloc() is called %u times. (realloc()/calloc() counted as "
      "one time)",
      static_cast<unsigned int>(heap_stats_.total_allocate_calls));
  PW_LOG_INFO(
      "          free() is called %u times. (realloc() counted as one time)",
      static_cast<unsigned int>(heap_stats_.total_free_calls));
  PW_LOG_INFO(" ");
}

// TODO(keir): Add stack tracing to locate which call to the heap operation
// caused the corruption.
void FreeListHeap::InvalidFreeCrash() {
  PW_DCHECK(false, "You tried to free an invalid pointer!");
}

}  // namespace pw::allocator
