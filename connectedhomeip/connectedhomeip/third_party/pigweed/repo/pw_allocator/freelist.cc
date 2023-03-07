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

namespace pw::allocator {

Status FreeList::AddChunk(span<std::byte> chunk) {
  // Check that the size is enough to actually store what we need
  if (chunk.size() < sizeof(FreeListNode)) {
    return Status::OutOfRange();
  }

  union {
    FreeListNode* node;
    std::byte* bytes;
  } aliased;

  aliased.bytes = chunk.data();

  unsigned short chunk_ptr = FindChunkPtrForSize(chunk.size(), false);

  // Add it to the correct list.
  aliased.node->size = chunk.size();
  aliased.node->next = chunks_[chunk_ptr];
  chunks_[chunk_ptr] = aliased.node;

  return OkStatus();
}

span<std::byte> FreeList::FindChunk(size_t size) const {
  if (size == 0) {
    return span<std::byte>();
  }

  unsigned short chunk_ptr = FindChunkPtrForSize(size, true);

  // Check that there's data. This catches the case where we run off the
  // end of the array
  if (chunks_[chunk_ptr] == nullptr) {
    return span<std::byte>();
  }

  // Now iterate up the buckets, walking each list to find a good candidate
  for (size_t i = chunk_ptr; i < chunks_.size(); i++) {
    union {
      FreeListNode* node;
      std::byte* data;
    } aliased;
    aliased.node = chunks_[static_cast<unsigned short>(i)];

    while (aliased.node != nullptr) {
      if (aliased.node->size >= size) {
        return span<std::byte>(aliased.data, aliased.node->size);
      }

      aliased.node = aliased.node->next;
    }
  }

  // If we get here, we've checked every block in every bucket. There's
  // nothing that can support this allocation.
  return span<std::byte>();
}

Status FreeList::RemoveChunk(span<std::byte> chunk) {
  unsigned short chunk_ptr = FindChunkPtrForSize(chunk.size(), true);

  // Walk that list, finding the chunk.
  union {
    FreeListNode* node;
    std::byte* data;
  } aliased, aliased_next;

  // Check head first.
  if (chunks_[chunk_ptr] == nullptr) {
    return Status::NotFound();
  }

  aliased.node = chunks_[chunk_ptr];
  if (aliased.data == chunk.data()) {
    chunks_[chunk_ptr] = aliased.node->next;

    return OkStatus();
  }

  // No? Walk the nodes.
  aliased.node = chunks_[chunk_ptr];

  while (aliased.node->next != nullptr) {
    aliased_next.node = aliased.node->next;
    if (aliased_next.data == chunk.data()) {
      // Found it, remove this node out of the chain
      aliased.node->next = aliased_next.node->next;
      return OkStatus();
    }

    aliased.node = aliased.node->next;
  }

  return Status::NotFound();
}

unsigned short FreeList::FindChunkPtrForSize(size_t size, bool non_null) const {
  unsigned short chunk_ptr = 0;
  for (chunk_ptr = 0u; chunk_ptr < sizes_.size(); chunk_ptr++) {
    if (sizes_[chunk_ptr] >= size &&
        (!non_null || chunks_[chunk_ptr] != nullptr)) {
      break;
    }
  }

  return chunk_ptr;
}

}  // namespace pw::allocator
