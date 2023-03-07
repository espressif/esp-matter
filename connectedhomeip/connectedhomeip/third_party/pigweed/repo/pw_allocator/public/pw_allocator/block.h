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

// WARNING: This code is a experimental WIP & exploration only, and is far from
// usable.
#pragma once

#include <cstdint>

#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::allocator {

#if defined(PW_ALLOCATOR_POISON_ENABLE) && PW_ALLOCATOR_POISON_ENABLE
// Add poison offset of sizeof(void*) bytes before and after usable space in all
// Blocks.
#define PW_ALLOCATOR_POISON_OFFSET sizeof(void*)
#else
// Set the poison offset to 0 bytes; will not add poisson space before and
// after usable space in all Blocks.
#define PW_ALLOCATOR_POISON_OFFSET static_cast<size_t>(0)
#endif  // PW_ALLOCATOR_POISON_ENABLE

// The "Block" type is intended to be a building block component for
// allocators. In the this design, there is an explicit pointer to next and
// prev from the block header; the size is not encoded. The below diagram shows
// what this would look like for two blocks.
//
//   .------+---------------------------------.-----------------------------
//   |            Block A (first)             |       Block B (second)
//
//   +------+------+--------------------------+------+------+---------------
//   | Next | Prev |   usable space           | Next | Prev | usable space..
//   +------+------+--------------------------+------+--+---+---------------
//   ^  |                                     ^         |
//   |  '-------------------------------------'         |
//   |                                                  |
//   '----------- Block B's prev points to Block A -----'
//
// One use for these blocks is to use them as allocations, where each block
// represents an allocation handed out by malloc(). These blocks could also be
// used as part of a slab or buddy allocator.
//
// Each block also contains flags for whether it is the last block (i.e. whether
// the "next" pointer points to a valid block, or just denotes the end of this
// block), and whether the block is in use. These are encoded into the last two
// bits of the "next" pointer, as follows:
//
//  .-----------------------------------------------------------------------.
//  |                            Block                                      |
//  +-----------------------------------------------------------------------+
//  |              Next            | Prev |         usable space            |
//  +----------------+------+------+      +                                 |
//  |   Ptr[N..2]    | Last | Used |      |                                 |
//  +----------------+------+------+------+---------------------------------+
//  ^
//  |
//  '----------- Next() = Next & ~0x3 --------------------------------->
//
// The first block in a chain is denoted by a nullptr "prev" field, and the last
// block is denoted by the "Last" bit being set.
//
// Note, This block class requires that the given block is aligned to a
// alignof(Block*) boundary. Because of this alignment requirement, each
// returned block will also be aligned to a alignof(Block*) boundary, and the
// size will always be rounded up to a multiple of alignof(Block*).
//
// This class must be constructed using the static Init call.
class Block final {
 public:
  // No copy/move
  Block(const Block& other) = delete;
  Block& operator=(const Block& other) = delete;
  Block(Block&& other) = delete;
  Block& operator=(Block&& other) = delete;

  // Create the first block for a given memory region.
  // Note that the start of the given memory region must be aligned to an
  // alignof(Block) boundary.
  // Returns:
  //   INVALID_ARGUMENT if the given region is unaligned to too small, or
  //   OK otherwise.
  static Status Init(const span<std::byte> region, Block** block);

  // Returns a pointer to a Block, given a pointer to the start of the usable
  // space inside the block (i.e. the opposite operation to UsableSpace()). In
  // reality, this method just subtracts the appropriate amount from
  // usable_space to point to the start of the owning block.
  //
  // Be aware that this method does not do any checking; passing a random
  // pointer will return a non-null pointer.
  static Block* FromUsableSpace(std::byte* usable_space) {
    return reinterpret_cast<Block*>(usable_space - sizeof(Block) -
                                    PW_ALLOCATOR_POISON_OFFSET);
  }

  // Size including the header.
  size_t OuterSize() const {
    return reinterpret_cast<intptr_t>(Next()) -
           reinterpret_cast<intptr_t>(this);
  }

  // Usable bytes inside the block.
  size_t InnerSize() const {
    return OuterSize() - sizeof(*this) - 2 * PW_ALLOCATOR_POISON_OFFSET;
  }

  // Return the usable space inside this block.
  std::byte* UsableSpace() {
    return reinterpret_cast<std::byte*>(this) + sizeof(*this) +
           PW_ALLOCATOR_POISON_OFFSET;
  }

  // Split this block, such that this block has an inner size of
  // `head_block_inner_size`, and return a new block in the remainder of the
  // space in `new_block`.
  //
  // The "remainder" block will be aligned to a alignof(Block*) boundary (and
  // `head_block_inner_size` will be rounded up). If the remaining space is not
  // large enough to store a new `Block` after rounding, no splitting will
  // occur.
  //
  // This may return the following:
  //   OK: The split completed successfully.
  //   INVALID_ARGUMENT: new_block is null
  //   FAILED_PRECONDITION: This block is in use and cannot be split.
  //   OUT_OF_RANGE: The requested size for "this" block is greater than the
  //                 current inner_size.
  //   RESOURCE_EXHAUSTED: The split cannot occur because the "remainder" block
  //                       would not be large enough to store a block header.
  Status Split(size_t head_block_inner_size, Block** new_block);

  // Merge this block with the one that comes after it.
  // This function will not merge blocks if either are in use.
  //
  // This may return the following:
  //   OK: Merge was successful.
  //   OUT_OF_RANGE: Attempting to merge the "last" block.
  //   FAILED_PRECONDITION: The blocks could not be merged because one of them
  //                        was in use.
  Status MergeNext();

  // Merge this block with the one that comes before it.
  // This function will not merge blocks if either are in use.
  //
  // Warning: merging with a previous block will invalidate this block instance.
  // do not perform any operations on this instance after merging.
  //
  // This may return the following:
  //   OK: Merge was successful.
  //   OUT_OF_RANGE: Attempting to merge the "first" block.
  //   FAILED_PRECONDITION: The blocks could not be merged because one of them
  //                        was in use.
  Status MergePrev();

  // Returns whether this block is in-use or not
  bool Used() const { return (NextAsUIntPtr() & kInUseFlag) == kInUseFlag; }

  // Returns whether this block is the last block or
  // not (i.e. whether NextBlock points to a valid block or not).
  // This is needed because NextBlock points to the end of this block,
  // whether there is a valid block there or not.
  bool Last() const { return (NextAsUIntPtr() & kLastFlag) == kLastFlag; }

  // Mark this block as in-use
  void MarkUsed() {
    next_ = reinterpret_cast<Block*>((NextAsUIntPtr() | kInUseFlag));
  }

  // Mark this block as free
  void MarkFree() {
    next_ = reinterpret_cast<Block*>((NextAsUIntPtr() & ~kInUseFlag));
  }

  // Mark this block as the last one in the chain.
  void MarkLast() {
    next_ = reinterpret_cast<Block*>((NextAsUIntPtr() | kLastFlag));
  }

  // Clear the "last" bit from this block.
  void ClearLast() {
    next_ = reinterpret_cast<Block*>((NextAsUIntPtr() & ~kLastFlag));
  }

  // Fetch the block immediately after this one.
  // Note: you should also check Last(); this function may return a valid
  // block, even if one does not exist.
  Block* Next() const {
    return reinterpret_cast<Block*>(
        (NextAsUIntPtr() & ~(kInUseFlag | kLastFlag)));
  }

  // Return the block immediately before this one. This will return nullptr
  // if this is the "first" block.
  Block* Prev() const { return prev_; }

  // Return true if the block is aligned, the prev/next field matches with the
  // previous and next block, and the poisoned bytes is not damaged. Otherwise,
  // return false to indicate this block is corrupted.
  bool IsValid() const { return CheckStatus() == BlockStatus::VALID; }

  // Uses PW_DCHECK to log information about the reason if a block is invalid.
  // This function will do nothing if the block is valid.
  void CrashIfInvalid();

 private:
  static constexpr uintptr_t kInUseFlag = 0x1;
  static constexpr uintptr_t kLastFlag = 0x2;
  static constexpr std::byte POISON_PATTERN[8] = {std::byte{0x92},
                                                  std::byte{0x88},
                                                  std::byte{0x0a},
                                                  std::byte{0x00},
                                                  std::byte{0xec},
                                                  std::byte{0xdc},
                                                  std::byte{0xae},
                                                  std::byte{0x4e}};
  enum BlockStatus {
    VALID,
    MISALIGNED,
    PREV_MISMATCHED,
    NEXT_MISMATCHED,
    POISON_CORRUPTED
  };

  Block() = default;

  // Helper to reduce some of the casting nesting in the block management
  // functions.
  uintptr_t NextAsUIntPtr() const { return reinterpret_cast<uintptr_t>(next_); }

  void PoisonBlock();
  bool CheckPoisonBytes() const;
  BlockStatus CheckStatus() const;

  // Note: Consider instead making these next/prev offsets from the current
  // block, with templated type for the offset size. There are some interesting
  // tradeoffs here; perhaps a pool of small allocations could use 1-byte
  // next/prev offsets to reduce size further.
  Block* next_;
  Block* prev_;
};

}  // namespace pw::allocator
