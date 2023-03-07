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

#include <array>

#include "pw_containers/vector.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::allocator {

template <size_t kNumBuckets>
class FreeListBuffer;

// Basic freelist implementation for an allocator.
// This implementation buckets by chunk size, with a list of user-provided
// buckets. Each bucket is a linked list of storage chunks. Because this
// freelist uses the added chunks themselves as list nodes, there is lower bound
// of sizeof(FreeList.FreeListNode) bytes for chunks which can be added to this
// freelist. There is also an implicit bucket for "everything else", for chunks
// which do not fit into a bucket.
//
// Each added chunk will be added to the smallest bucket under which it fits. If
// it does not fit into any user-provided bucket, it will be added to the
// default bucket.
//
// As an example, assume that the FreeList is configured with buckets of sizes
// {64, 128, 256 and 512} bytes. The internal state may look like the following.
//
// bucket[0] (64B) --> chunk[12B] --> chunk[42B] --> chunk[64B] --> NULL
// bucket[1] (128B) --> chunk[65B] --> chunk[72B] --> NULL
// bucket[2] (256B) --> NULL
// bucket[3] (512B) --> chunk[312B] --> chunk[512B] --> chunk[416B] --> NULL
// bucket[4] (implicit) --> chunk[1024B] --> chunk[513B] --> NULL
//
// Note that added chunks should be aligned to a 4-byte boundary.
//
// This class is split into two parts; FreeList implements all of the
// logic, and takes in pointers for two pw::Vector instances for its storage.
// This prevents us from having to specialise this class for every kMaxSize
// parameter for the vector. FreeListBuffer then provides the storage for these
// two pw::Vector instances and instantiates FreeListInternal.
class FreeList {
 public:
  // Remove copy/move ctors
  FreeList(const FreeList& other) = delete;
  FreeList(FreeList&& other) = delete;
  FreeList& operator=(const FreeList& other) = delete;
  FreeList& operator=(FreeList&& other) = delete;

  // Adds a chunk to this freelist. Returns:
  //   OK: The chunk was added successfully
  //   OUT_OF_RANGE: The chunk could not be added for size reasons (e.g. if
  //                 the chunk is too small to store the FreeListNode).
  Status AddChunk(span<std::byte> chunk);

  // Finds an eligible chunk for an allocation of size `size`. Note that this
  // will return the first allocation possible within a given bucket, it does
  // not currently optimize for finding the smallest chunk. Returns a span
  // representing the chunk. This will be "valid" on success, and will have size
  // = 0 on failure (if there were no chunks available for that allocation).
  span<std::byte> FindChunk(size_t size) const;

  // Remove a chunk from this freelist. Returns:
  //   OK: The chunk was removed successfully
  //   NOT_FOUND: The chunk could not be found in this freelist.
  Status RemoveChunk(span<std::byte> chunk);

 private:
  // For a given size, find which index into chunks_ the node should be written
  // to.
  unsigned short FindChunkPtrForSize(size_t size, bool non_null) const;

 private:
  template <size_t kNumBuckets>
  friend class FreeListBuffer;

  struct FreeListNode {
    // TODO(jgarside): Double-link this? It'll make removal easier/quicker.
    FreeListNode* next;
    size_t size;
  };

  constexpr FreeList(Vector<FreeListNode*>& chunks, Vector<size_t>& sizes)
      : chunks_(chunks), sizes_(sizes) {}

  Vector<FreeListNode*>& chunks_;
  Vector<size_t>& sizes_;
};

// Holder for FreeList's storage.
template <size_t kNumBuckets>
class FreeListBuffer : public FreeList {
 public:
  // These constructors are a little hacky because of the initialization order.
  // Because FreeList has a trivial constructor, this is safe, however.
  explicit FreeListBuffer(std::initializer_list<size_t> sizes)
      : FreeList(chunks_, sizes_), sizes_(sizes), chunks_(kNumBuckets + 1, 0) {}
  explicit FreeListBuffer(std::array<size_t, kNumBuckets> sizes)
      : FreeList(chunks_, sizes_),
        sizes_(sizes.begin(), sizes.end()),
        chunks_(kNumBuckets + 1, 0) {}

 private:
  Vector<size_t, kNumBuckets> sizes_;
  Vector<FreeList::FreeListNode*, kNumBuckets + 1> chunks_;
};

}  // namespace pw::allocator
