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

#include "pw_allocator/block.h"

#include <cstring>

#include "pw_assert/check.h"
#include "pw_span/span.h"

namespace pw::allocator {

Status Block::Init(const span<std::byte> region, Block** block) {
  // Ensure the region we're given is aligned and sized accordingly
  if (reinterpret_cast<uintptr_t>(region.data()) % alignof(Block) != 0) {
    return Status::InvalidArgument();
  }

  if (region.size() < sizeof(Block)) {
    return Status::InvalidArgument();
  }

  union {
    Block* block;
    std::byte* bytes;
  } aliased;
  aliased.bytes = region.data();

  // Make "next" point just past the end of this block; forming a linked list
  // with the following storage. Since the space between this block and the
  // next are implicitly part of the raw data, size can be computed by
  // subtracting the pointers.
  aliased.block->next_ =
      reinterpret_cast<Block*>(region.data() + region.size_bytes());
  aliased.block->MarkLast();

  aliased.block->prev_ = nullptr;
  *block = aliased.block;
#if defined(PW_ALLOCATOR_POISON_ENABLE) && PW_ALLOCATOR_POISON_ENABLE
  (*block)->PoisonBlock();
#endif  // PW_ALLOCATOR_POISON_ENABLE
  return OkStatus();
}

Status Block::Split(size_t head_block_inner_size, Block** new_block) {
  if (new_block == nullptr) {
    return Status::InvalidArgument();
  }

  // Don't split used blocks.
  // TODO(jgarside): Relax this restriction? Flag to enable/disable this check?
  if (Used()) {
    return Status::FailedPrecondition();
  }

  // First round the head_block_inner_size up to a alignof(Block) bounary.
  // This ensures that the next block header is aligned accordingly.
  // Alignment must be a power of two, hence align()-1 will return the
  // remainder.
  auto align_bit_mask = alignof(Block) - 1;
  size_t aligned_head_block_inner_size = head_block_inner_size;
  if ((head_block_inner_size & align_bit_mask) != 0) {
    aligned_head_block_inner_size =
        (head_block_inner_size & ~align_bit_mask) + alignof(Block);
  }

  // (1) Are we trying to allocate a head block larger than the current head
  // block? This may happen because of the alignment above.
  if (aligned_head_block_inner_size > InnerSize()) {
    return Status::OutOfRange();
  }

  // (2) Does the resulting block have enough space to store the header?
  // TODO(jgarside): What to do if the returned section is empty (i.e. remaining
  // size == sizeof(Block))?
  if (InnerSize() - aligned_head_block_inner_size <
      sizeof(Block) + 2 * PW_ALLOCATOR_POISON_OFFSET) {
    return Status::ResourceExhausted();
  }

  // Create the new block inside the current one.
  Block* new_next = reinterpret_cast<Block*>(
      // From the current position...
      reinterpret_cast<intptr_t>(this) +
      // skip past the current header...
      sizeof(*this) +
      // add the poison bytes before usable space ...
      PW_ALLOCATOR_POISON_OFFSET +
      // into the usable bytes by the new inner size...
      aligned_head_block_inner_size +
      // add the poison bytes after the usable space ...
      PW_ALLOCATOR_POISON_OFFSET);

  // If we're inserting in the middle, we need to update the current next
  // block to point to what we're inserting
  if (!Last()) {
    Next()->prev_ = new_next;
  }

  // Copy next verbatim so the next block also gets the "last"-ness
  new_next->next_ = next_;
  new_next->prev_ = this;

  // Update the current block to point to the new head.
  next_ = new_next;

  *new_block = next_;

#if defined(PW_ALLOCATOR_POISON_ENABLE) && PW_ALLOCATOR_POISON_ENABLE
  PoisonBlock();
  (*new_block)->PoisonBlock();
#endif  // PW_ALLOCATOR_POISON_ENABLE

  return OkStatus();
}

Status Block::MergeNext() {
  // Anything to merge with?
  if (Last()) {
    return Status::OutOfRange();
  }

  // Is this or the next block in use?
  if (Used() || Next()->Used()) {
    return Status::FailedPrecondition();
  }

  // Simply enough, this block's next pointer becomes the next block's
  // next pointer. We then need to re-wire the "next next" block's prev
  // pointer to point back to us though.
  next_ = Next()->next_;

  // Copying the pointer also copies the "last" status, so this is safe.
  if (!Last()) {
    Next()->prev_ = this;
  }

  return OkStatus();
}

Status Block::MergePrev() {
  // We can't merge if we have no previous. After that though, merging with
  // the previous block is just MergeNext from the previous block.
  if (prev_ == nullptr) {
    return Status::OutOfRange();
  }

  // WARNING: This class instance will still exist, but technically be invalid
  // after this has been invoked. Be careful when doing anything with `this`
  // After doing the below.
  return prev_->MergeNext();
}

// TODO(b/234875269): Add stack tracing to locate which call to the heap
// operation caused the corruption.
// TODO(jgarside): Add detailed information to log report and leave succinct
// messages in the crash message.
void Block::CrashIfInvalid() {
  switch (CheckStatus()) {
    case VALID:
      break;
    case MISALIGNED:
      PW_DCHECK(false,
                "The block at address %p is not aligned.",
                static_cast<void*>(this));
      break;
    case NEXT_MISMATCHED:
      PW_DCHECK(false,
                "The 'prev' field in the next block (%p) does not match the "
                "address of the current block (%p).",
                static_cast<void*>(Next()->Prev()),
                static_cast<void*>(this));
      break;
    case PREV_MISMATCHED:
      PW_DCHECK(false,
                "The 'next' field in the previous block (%p) does not match "
                "the address of the current block (%p).",
                static_cast<void*>(Prev()->Next()),
                static_cast<void*>(this));
      break;
    case POISON_CORRUPTED:
      PW_DCHECK(false,
                "The poisoned pattern in the block at %p is corrupted.",
                static_cast<void*>(this));
      break;
  }
}

// This function will return a Block::BlockStatus that is either VALID or
// indicates the reason why the Block is invalid. If the Block is invalid at
// multiple points, this function will only return one of the reasons.
Block::BlockStatus Block::CheckStatus() const {
  // Make sure the Block is aligned.
  if (reinterpret_cast<uintptr_t>(this) % alignof(Block) != 0) {
    return BlockStatus::MISALIGNED;
  }

  // Test if the prev/next pointer for this Block matches.
  if (!Last() && (this >= Next() || this != Next()->Prev())) {
    return BlockStatus::NEXT_MISMATCHED;
  }

  if (Prev() && (this <= Prev() || this != Prev()->Next())) {
    return BlockStatus::PREV_MISMATCHED;
  }

#if defined(PW_ALLOCATOR_POISON_ENABLE) && PW_ALLOCATOR_POISON_ENABLE
  if (!this->CheckPoisonBytes()) {
    return BlockStatus::POISON_CORRUPTED;
  }
#endif  // PW_ALLOCATOR_POISON_ENABLE
  return BlockStatus::VALID;
}

// Paint sizeof(void*) bytes before and after the usable space in Block as the
// randomized function pattern.
void Block::PoisonBlock() {
#if defined(PW_ALLOCATOR_POISON_ENABLE) && PW_ALLOCATOR_POISON_ENABLE
  std::byte* front_region = reinterpret_cast<std::byte*>(this) + sizeof(*this);
  memcpy(front_region, POISON_PATTERN, PW_ALLOCATOR_POISON_OFFSET);

  std::byte* end_region =
      reinterpret_cast<std::byte*>(Next()) - PW_ALLOCATOR_POISON_OFFSET;
  memcpy(end_region, POISON_PATTERN, PW_ALLOCATOR_POISON_OFFSET);
#endif  // PW_ALLOCATOR_POISON_ENABLE
}

bool Block::CheckPoisonBytes() const {
#if defined(PW_ALLOCATOR_POISON_ENABLE) && PW_ALLOCATOR_POISON_ENABLE
  std::byte* front_region = reinterpret_cast<std::byte*>(
      reinterpret_cast<intptr_t>(this) + sizeof(*this));
  if (std::memcmp(front_region, POISON_PATTERN, PW_ALLOCATOR_POISON_OFFSET)) {
    return false;
  }
  std::byte* end_region = reinterpret_cast<std::byte*>(
      reinterpret_cast<intptr_t>(this->Next()) - PW_ALLOCATOR_POISON_OFFSET);
  if (std::memcmp(end_region, POISON_PATTERN, PW_ALLOCATOR_POISON_OFFSET)) {
    return false;
  }
#endif  // PW_ALLOCATOR_POISON_ENABLE
  return true;
}

}  // namespace pw::allocator
