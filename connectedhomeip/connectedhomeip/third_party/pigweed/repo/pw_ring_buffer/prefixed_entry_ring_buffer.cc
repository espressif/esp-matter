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

#include "pw_ring_buffer/prefixed_entry_ring_buffer.h"

#include <algorithm>
#include <cstring>

#include "pw_assert/assert.h"
#include "pw_assert/check.h"
#include "pw_status/try.h"
#include "pw_varint/varint.h"

namespace pw {
namespace ring_buffer {

using std::byte;
using Entry = PrefixedEntryRingBufferMulti::Entry;
using Reader = PrefixedEntryRingBufferMulti::Reader;
using iterator = PrefixedEntryRingBufferMulti::iterator;

void PrefixedEntryRingBufferMulti::Clear() {
  write_idx_ = 0;
  for (Reader& reader : readers_) {
    reader.read_idx_ = 0;
    reader.entry_count_ = 0;
  }
}

Status PrefixedEntryRingBufferMulti::SetBuffer(span<byte> buffer) {
  if ((buffer.data() == nullptr) ||  //
      (buffer.size_bytes() == 0) ||  //
      (buffer.size_bytes() > kMaxBufferBytes)) {
    return Status::InvalidArgument();
  }

  buffer_ = buffer.data();
  buffer_bytes_ = buffer.size_bytes();

  Clear();
  return OkStatus();
}

Status PrefixedEntryRingBufferMulti::AttachReader(Reader& reader) {
  if (reader.buffer_ != nullptr) {
    return Status::InvalidArgument();
  }
  reader.buffer_ = this;

  if (readers_.empty()) {
    reader.read_idx_ = write_idx_;
    reader.entry_count_ = 0;
  } else {
    const Reader& slowest_reader = GetSlowestReader();
    reader.read_idx_ = slowest_reader.read_idx_;
    reader.entry_count_ = slowest_reader.entry_count_;
  }

  readers_.push_back(reader);
  return OkStatus();
}

Status PrefixedEntryRingBufferMulti::DetachReader(Reader& reader) {
  if (reader.buffer_ != this) {
    return Status::InvalidArgument();
  }
  reader.buffer_ = nullptr;
  reader.read_idx_ = 0;
  reader.entry_count_ = 0;
  readers_.remove(reader);
  return OkStatus();
}

Status PrefixedEntryRingBufferMulti::InternalPushBack(
    span<const byte> data,
    uint32_t user_preamble_data,
    bool pop_front_if_needed) {
  if (buffer_ == nullptr) {
    return Status::FailedPrecondition();
  }

  // Prepare a single buffer that can hold both the user preamble and entry
  // length.
  byte preamble_buf[varint::kMaxVarint32SizeBytes * 2];
  size_t user_preamble_bytes = 0;
  if (user_preamble_) {
    user_preamble_bytes =
        varint::Encode<uint32_t>(user_preamble_data, preamble_buf);
  }
  size_t length_bytes = varint::Encode<uint32_t>(
      data.size_bytes(), span(preamble_buf).subspan(user_preamble_bytes));
  size_t total_write_bytes =
      user_preamble_bytes + length_bytes + data.size_bytes();
  if (buffer_bytes_ < total_write_bytes) {
    return Status::OutOfRange();
  }

  if (pop_front_if_needed) {
    // PushBack() case: evict items as needed.
    // Drop old entries until we have space for the new entry.
    while (RawAvailableBytes() < total_write_bytes) {
      InternalPopFrontAll();
    }
  } else if (RawAvailableBytes() < total_write_bytes) {
    // TryPushBack() case: don't evict items.
    return Status::ResourceExhausted();
  }

  // Write the new entry into the ring buffer.
  RawWrite(span(preamble_buf, user_preamble_bytes + length_bytes));
  RawWrite(data);

  // Update all readers of the new count.
  for (Reader& reader : readers_) {
    reader.entry_count_++;
  }
  return OkStatus();
}

auto GetOutput(span<byte> data_out, size_t* write_index) {
  return [data_out, write_index](span<const byte> src) -> Status {
    size_t copy_size = std::min(data_out.size_bytes(), src.size_bytes());

    memcpy(data_out.data() + *write_index, src.data(), copy_size);
    *write_index += copy_size;

    return (copy_size == src.size_bytes()) ? OkStatus()
                                           : Status::ResourceExhausted();
  };
}

Status PrefixedEntryRingBufferMulti::InternalPeekFront(
    const Reader& reader, span<byte> data, size_t* bytes_read_out) const {
  *bytes_read_out = 0;
  return InternalRead(reader, GetOutput(data, bytes_read_out), false);
}

Status PrefixedEntryRingBufferMulti::InternalPeekFront(
    const Reader& reader, ReadOutput output) const {
  return InternalRead(reader, output, false);
}

Status PrefixedEntryRingBufferMulti::InternalPeekFrontWithPreamble(
    const Reader& reader, span<byte> data, size_t* bytes_read_out) const {
  *bytes_read_out = 0;
  return InternalRead(reader, GetOutput(data, bytes_read_out), true);
}

Status PrefixedEntryRingBufferMulti::InternalPeekFrontWithPreamble(
    const Reader& reader, ReadOutput output) const {
  return InternalRead(reader, output, true);
}

Status PrefixedEntryRingBufferMulti::InternalPeekFrontPreamble(
    const Reader& reader, uint32_t& user_preamble_out) const {
  if (reader.entry_count_ == 0) {
    return Status::OutOfRange();
  }
  // Figure out where to start reading (wrapped); accounting for preamble.
  EntryInfo info = FrontEntryInfo(reader);
  user_preamble_out = info.user_preamble;
  return OkStatus();
}

// TODO(b/235351046): Consider whether this internal templating is required, or
// if we can simply promote GetOutput to a static function and remove the
// template. T should be similar to Status (*read_output)(span<const byte>)
template <typename T>
Status PrefixedEntryRingBufferMulti::InternalRead(
    const Reader& reader,
    T read_output,
    bool include_preamble_in_output,
    uint32_t* user_preamble_out) const {
  if (buffer_ == nullptr) {
    return Status::FailedPrecondition();
  }
  if (reader.entry_count_ == 0) {
    return Status::OutOfRange();
  }

  // Figure out where to start reading (wrapped); accounting for preamble.
  EntryInfo info = FrontEntryInfo(reader);
  size_t read_bytes = info.data_bytes;
  size_t data_read_idx = reader.read_idx_;
  if (user_preamble_out) {
    *user_preamble_out = info.user_preamble;
  }
  if (include_preamble_in_output) {
    read_bytes += info.preamble_bytes;
  } else {
    data_read_idx = IncrementIndex(data_read_idx, info.preamble_bytes);
  }

  // Read bytes, stopping at the end of the buffer if this entry wraps.
  size_t bytes_until_wrap = buffer_bytes_ - data_read_idx;
  size_t bytes_to_copy = std::min(read_bytes, bytes_until_wrap);
  Status status = read_output(span(buffer_ + data_read_idx, bytes_to_copy));

  // If the entry wrapped, read the remaining bytes.
  if (status.ok() && (bytes_to_copy < read_bytes)) {
    status = read_output(span(buffer_, read_bytes - bytes_to_copy));
  }
  return status;
}

void PrefixedEntryRingBufferMulti::InternalPopFrontAll() {
  // Forcefully pop all readers. Find the slowest reader, which must have
  // the highest entry count, then pop all readers that have the same count.
  //
  // It is expected that InternalPopFrontAll is called only when there is
  // something to pop from at least one reader. If no readers exist, or all
  // readers are caught up, this function will assert.
  size_t entry_count = GetSlowestReader().entry_count_;
  PW_DCHECK_INT_NE(entry_count, 0);
  // Otherwise, pop the readers that have the largest value.
  for (Reader& reader : readers_) {
    if (reader.entry_count_ == entry_count) {
      reader.PopFront()
          .IgnoreError();  // TODO(b/242598609): Handle Status properly
    }
  }
}

const Reader& PrefixedEntryRingBufferMulti::GetSlowestReader() const {
  PW_DCHECK_INT_GT(readers_.size(), 0);
  const Reader* slowest_reader = &(*readers_.begin());
  for (const Reader& reader : readers_) {
    if (reader.entry_count_ > slowest_reader->entry_count_) {
      slowest_reader = &reader;
    }
  }
  return *slowest_reader;
}

Status PrefixedEntryRingBufferMulti::Dering() {
  if (buffer_ == nullptr || readers_.empty()) {
    return Status::FailedPrecondition();
  }

  // Check if by luck we're already deringed.
  Reader& slowest_reader = GetSlowestReaderWritable();
  if (slowest_reader.read_idx_ == 0) {
    return OkStatus();
  }

  return InternalDering(slowest_reader);
}

Status PrefixedEntryRingBufferMulti::InternalDering(Reader& dering_reader) {
  if (buffer_ == nullptr || readers_.empty()) {
    return Status::FailedPrecondition();
  }

  auto buffer_span = span(buffer_, buffer_bytes_);
  std::rotate(buffer_span.begin(),
              buffer_span.begin() + dering_reader.read_idx_,
              buffer_span.end());

  // If the new index is past the end of the buffer,
  // alias it back (wrap) to the start of the buffer.
  if (write_idx_ < dering_reader.read_idx_) {
    write_idx_ += buffer_bytes_;
  }
  write_idx_ -= dering_reader.read_idx_;

  for (Reader& reader : readers_) {
    if (&reader == &dering_reader) {
      continue;
    }
    if (reader.read_idx_ < dering_reader.read_idx_) {
      reader.read_idx_ += buffer_bytes_;
    }
    reader.read_idx_ -= dering_reader.read_idx_;
  }

  dering_reader.read_idx_ = 0;
  return OkStatus();
}

Status PrefixedEntryRingBufferMulti::InternalPopFront(Reader& reader) {
  if (buffer_ == nullptr) {
    return Status::FailedPrecondition();
  }
  if (reader.entry_count_ == 0) {
    return Status::OutOfRange();
  }

  // Advance the read pointer past the front entry to the next one.
  EntryInfo info = FrontEntryInfo(reader);
  size_t entry_bytes = info.preamble_bytes + info.data_bytes;
  size_t prev_read_idx = reader.read_idx_;
  reader.read_idx_ = IncrementIndex(prev_read_idx, entry_bytes);
  reader.entry_count_--;
  return OkStatus();
}

size_t PrefixedEntryRingBufferMulti::InternalFrontEntryDataSizeBytes(
    const Reader& reader) const {
  if (reader.entry_count_ == 0) {
    return 0;
  }
  return FrontEntryInfo(reader).data_bytes;
}

size_t PrefixedEntryRingBufferMulti::InternalFrontEntryTotalSizeBytes(
    const Reader& reader) const {
  if (reader.entry_count_ == 0) {
    return 0;
  }
  EntryInfo info = FrontEntryInfo(reader);
  return info.preamble_bytes + info.data_bytes;
}

PrefixedEntryRingBufferMulti::EntryInfo
PrefixedEntryRingBufferMulti::FrontEntryInfo(const Reader& reader) const {
  Result<PrefixedEntryRingBufferMulti::EntryInfo> entry_info =
      RawFrontEntryInfo(reader.read_idx_);
  PW_CHECK_OK(entry_info.status());
  return entry_info.value();
}

Result<PrefixedEntryRingBufferMulti::EntryInfo>
PrefixedEntryRingBufferMulti::RawFrontEntryInfo(size_t source_idx) const {
  // Entry headers consists of: (optional prefix byte, varint size, data...)

  // If a preamble exists, extract the varint and it's bytes in bytes.
  size_t user_preamble_bytes = 0;
  uint64_t user_preamble_data = 0;
  byte varint_buf[varint::kMaxVarint32SizeBytes];
  if (user_preamble_) {
    RawRead(varint_buf, source_idx, varint::kMaxVarint32SizeBytes);
    user_preamble_bytes = varint::Decode(varint_buf, &user_preamble_data);
    if (user_preamble_bytes == 0u) {
      return Status::DataLoss();
    }
  }

  // Read the entry header; extract the varint and it's bytes in bytes.
  RawRead(varint_buf,
          IncrementIndex(source_idx, user_preamble_bytes),
          varint::kMaxVarint32SizeBytes);
  uint64_t entry_bytes;
  size_t length_bytes = varint::Decode(varint_buf, &entry_bytes);
  if (length_bytes == 0u) {
    return Status::DataLoss();
  }

  EntryInfo info = {};
  info.preamble_bytes = user_preamble_bytes + length_bytes;
  info.user_preamble = static_cast<uint32_t>(user_preamble_data);
  info.data_bytes = entry_bytes;
  return info;
}

// Comparisons ordered for more probable early exits, assuming the reader is
// not far behind the writer compared to the size of the ring.
size_t PrefixedEntryRingBufferMulti::RawAvailableBytes() const {
  // Compute slowest reader. If no readers exist, the entire buffer can be
  // written.
  if (readers_.empty()) {
    return buffer_bytes_;
  }

  size_t read_idx = GetSlowestReader().read_idx_;
  // Case: Not wrapped.
  if (read_idx < write_idx_) {
    return buffer_bytes_ - (write_idx_ - read_idx);
  }
  // Case: Wrapped
  if (read_idx > write_idx_) {
    return read_idx - write_idx_;
  }
  // Case: Matched read and write heads; empty or full.
  for (const Reader& reader : readers_) {
    if (reader.read_idx_ == read_idx && reader.entry_count_ != 0) {
      return 0;
    }
  }
  return buffer_bytes_;
}

void PrefixedEntryRingBufferMulti::RawWrite(span<const std::byte> source) {
  if (source.size_bytes() == 0) {
    return;
  }

  // Write until the end of the source or the backing buffer.
  size_t bytes_until_wrap = buffer_bytes_ - write_idx_;
  size_t bytes_to_copy = std::min(source.size(), bytes_until_wrap);
  memcpy(buffer_ + write_idx_, source.data(), bytes_to_copy);

  // If there wasn't space in the backing buffer, wrap to the front.
  if (bytes_to_copy < source.size()) {
    memcpy(
        buffer_, source.data() + bytes_to_copy, source.size() - bytes_to_copy);
  }
  write_idx_ = IncrementIndex(write_idx_, source.size());
}

void PrefixedEntryRingBufferMulti::RawRead(byte* destination,
                                           size_t source_idx,
                                           size_t length_bytes) const {
  if (length_bytes == 0) {
    return;
  }

  // Read the pre-wrap bytes.
  size_t bytes_until_wrap = buffer_bytes_ - source_idx;
  size_t bytes_to_copy = std::min(length_bytes, bytes_until_wrap);
  memcpy(destination, buffer_ + source_idx, bytes_to_copy);

  // Read the post-wrap bytes, if needed.
  if (bytes_to_copy < length_bytes) {
    memcpy(destination + bytes_to_copy, buffer_, length_bytes - bytes_to_copy);
  }
}

size_t PrefixedEntryRingBufferMulti::IncrementIndex(size_t index,
                                                    size_t count) const {
  // Note: This doesn't use modulus (%) since the branch is cheaper, and we
  // guarantee that count will never be greater than buffer_bytes_.
  index += count;
  if (index > buffer_bytes_) {
    index -= buffer_bytes_;
  }
  return index;
}

Status PrefixedEntryRingBufferMulti::Reader::PeekFrontWithPreamble(
    span<byte> data,
    uint32_t& user_preamble_out,
    size_t& entry_bytes_read_out) const {
  entry_bytes_read_out = 0;
  return buffer_->InternalRead(
      *this, GetOutput(data, &entry_bytes_read_out), false, &user_preamble_out);
}

iterator& iterator::operator++() {
  PW_DCHECK_OK(iteration_status_);
  PW_DCHECK_INT_NE(entry_count_, 0);

  Result<EntryInfo> info = ring_buffer_->RawFrontEntryInfo(read_idx_);
  if (!info.status().ok()) {
    SkipToEnd(info.status());
    return *this;
  }

  // It is guaranteed that the buffer is deringed at this point.
  read_idx_ += info.value().preamble_bytes + info.value().data_bytes;
  entry_count_--;

  if (entry_count_ == 0) {
    SkipToEnd(OkStatus());
    return *this;
  }

  if (read_idx_ >= ring_buffer_->TotalUsedBytes()) {
    SkipToEnd(Status::DataLoss());
    return *this;
  }

  info = ring_buffer_->RawFrontEntryInfo(read_idx_);
  if (!info.status().ok()) {
    SkipToEnd(info.status());
    return *this;
  }
  return *this;
}

const Entry& iterator::operator*() const {
  PW_DCHECK_OK(iteration_status_);
  PW_DCHECK_INT_NE(entry_count_, 0);

  Result<EntryInfo> info = ring_buffer_->RawFrontEntryInfo(read_idx_);
  PW_DCHECK_OK(info.status());

  entry_ = {
      .buffer = span<const byte>(
          ring_buffer_->buffer_ + read_idx_ + info.value().preamble_bytes,
          info.value().data_bytes),
      .preamble = info.value().user_preamble,
  };
  return entry_;
}

}  // namespace ring_buffer
}  // namespace pw
