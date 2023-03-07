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

#include "pw_blob_store/blob_store.h"

#include <algorithm>

#include "pw_assert/check.h"
#include "pw_blob_store/internal/metadata_format.h"
#include "pw_bytes/byte_builder.h"
#include "pw_bytes/span.h"
#include "pw_kvs/checksum.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/key_value_store.h"
#include "pw_log/log.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_status/try.h"
#include "pw_stream/stream.h"

namespace pw::blob_store {

using internal::BlobMetadataHeader;
using internal::ChecksumValue;

Status BlobStore::Init() {
  if (initialized_) {
    return OkStatus();
  }

  PW_LOG_INFO("Init BlobStore");

  const size_t flash_write_size_alignment =
      flash_write_size_bytes_ % partition_.alignment_bytes();
  PW_CHECK_UINT_EQ(flash_write_size_alignment, 0);
  PW_CHECK_UINT_GE(flash_write_size_bytes_, partition_.alignment_bytes());
  const size_t partition_size_alignment =
      partition_.size_bytes() % flash_write_size_bytes_;
  PW_CHECK_UINT_EQ(partition_size_alignment, 0);
  if (!write_buffer_.empty()) {
    PW_CHECK_UINT_GE(write_buffer_.size_bytes(), flash_write_size_bytes_);
  }

  ResetChecksum();
  initialized_ = true;

  if (LoadMetadata().ok()) {
    PW_LOG_DEBUG("BlobStore init - Have valid blob of %u bytes",
                 static_cast<unsigned>(write_address_));
    return OkStatus();
  }

  // No saved blob, assume it has not been erased yet even if it has to avoid
  // having to scan the potentially massive partition.
  PW_LOG_DEBUG("BlobStore init - No valid blob, assuming not erased");
  return OkStatus();
}

Status BlobStore::LoadMetadata() {
  write_address_ = 0;
  flash_address_ = 0;
  file_name_length_ = 0;
  valid_data_ = false;

  BlobMetadataHeader metadata;
  metadata.reset();

  // For kVersion1 metadata versions, only the first member of
  // BlobMetadataHeaderV2 will be populated. If a file name is present,
  // kvs_.Get() will return RESOURCE_EXHAUSTED as the file name won't fit in the
  // BlobMetadtataHeader object, which is intended behavior.
  if (StatusWithSize sws = kvs_.acquire()->Get(
          MetadataKey(), as_writable_bytes(span(&metadata, 1)));
      !sws.ok() && !sws.IsResourceExhausted()) {
    return Status::NotFound();
  }

  if (!ValidateChecksum(metadata.v1_metadata.data_size_bytes,
                        metadata.v1_metadata.checksum)
           .ok()) {
    PW_LOG_ERROR("BlobStore init - Invalidating blob with invalid checksum");
    Invalidate().IgnoreError();  // TODO(b/242598609): Handle Status properly
    return Status::DataLoss();
  }

  write_address_ = metadata.v1_metadata.data_size_bytes;
  flash_address_ = metadata.v1_metadata.data_size_bytes;
  file_name_length_ = metadata.file_name_length;
  valid_data_ = true;

  return OkStatus();
}

size_t BlobStore::MaxDataSizeBytes() const { return partition_.size_bytes(); }

Status BlobStore::OpenWrite() {
  if (!initialized_) {
    return Status::FailedPrecondition();
  }

  // Writer can only be opened if there are no other writer or readers already
  // open.
  if (writer_open_ || readers_open_ != 0) {
    return Status::Unavailable();
  }

  PW_LOG_DEBUG("Blob writer open");

  writer_open_ = true;

  // Clear any existing contents.
  Invalidate().IgnoreError();  // TODO(b/242598609): Handle Status properly

  return OkStatus();
}

StatusWithSize BlobStore::GetFileName(span<char> dest) const {
  if (!initialized_) {
    return StatusWithSize(Status::FailedPrecondition(), 0);
  }

  if (file_name_length_ == 0) {
    return StatusWithSize(Status::NotFound(), 0);
  }

  const size_t bytes_to_read =
      std::min(dest.size_bytes(), static_cast<size_t>(file_name_length_));

  Status status = bytes_to_read == file_name_length_
                      ? OkStatus()
                      : Status::ResourceExhausted();

  // Read file name from KVS.
  constexpr size_t kFileNameOffset = sizeof(BlobMetadataHeader);
  const StatusWithSize kvs_read_sws =
      kvs_.acquire()->Get(MetadataKey(),
                          as_writable_bytes(dest.first(bytes_to_read)),
                          kFileNameOffset);
  status.Update(kvs_read_sws.status());
  return StatusWithSize(status, kvs_read_sws.size());
}

Status BlobStore::OpenRead() {
  if (!initialized_) {
    return Status::FailedPrecondition();
  }

  // Reader can only be opened if there is no writer open.
  if (writer_open_) {
    return Status::Unavailable();
  }

  if (!HasData()) {
    PW_LOG_ERROR("Blob reader unable open without valid data");
    return Status::FailedPrecondition();
  }

  PW_LOG_DEBUG("Blob reader open");

  readers_open_++;
  return OkStatus();
}

Status BlobStore::CloseRead() {
  PW_CHECK_UINT_GT(readers_open_, 0);
  readers_open_--;
  PW_LOG_DEBUG("Blob reader close");
  return OkStatus();
}

Status BlobStore::Write(ConstByteSpan data) {
  if (!ValidToWrite()) {
    return Status::DataLoss();
  }
  if (data.size_bytes() == 0) {
    return OkStatus();
  }
  if (WriteBytesRemaining() == 0) {
    return Status::OutOfRange();
  }
  if (WriteBytesRemaining() < data.size_bytes()) {
    return Status::ResourceExhausted();
  }
  if ((write_buffer_.empty()) &&
      ((data.size_bytes() % flash_write_size_bytes_) != 0)) {
    return Status::InvalidArgument();
  }

  if (!EraseIfNeeded().ok()) {
    return Status::DataLoss();
  }

  // Write in (up to) 3 steps:
  // 1) Finish filling write buffer and if full write it to flash.
  // 2) Write as many whole block-sized chunks as the data has remaining
  //    after 1.
  // 3) Put any remaining bytes less than flash write size in the write buffer.

  // Step 1) If there is any data in the write buffer, finish filling write
  //         buffer and if full write it to flash.
  if (!WriteBufferEmpty()) {
    PW_DCHECK(!write_buffer_.empty());
    size_t bytes_in_buffer = WriteBufferBytesUsed();

    // Non-deferred writes only use the first flash_write_size_bytes_ of the
    // write buffer to buffer writes less than flash_write_size_bytes_.
    PW_CHECK_UINT_GT(flash_write_size_bytes_, bytes_in_buffer);

    // Not using WriteBufferBytesFree() because non-deferred writes (which
    // is this method) only use the first flash_write_size_bytes_ of the write
    // buffer.
    size_t buffer_remaining = flash_write_size_bytes_ - bytes_in_buffer;

    // Add bytes up to filling the flash write size.
    size_t add_bytes = std::min(buffer_remaining, data.size_bytes());
    std::memcpy(write_buffer_.data() + bytes_in_buffer, data.data(), add_bytes);
    write_address_ += add_bytes;
    bytes_in_buffer += add_bytes;
    data = data.subspan(add_bytes);

    if (bytes_in_buffer != flash_write_size_bytes_) {
      // If there was not enough bytes to finish filling the write buffer, there
      // should not be any bytes left.
      PW_DCHECK(data.size_bytes() == 0);
      return OkStatus();
    }

    // The write buffer is full, flush to flash.
    if (!CommitToFlash(write_buffer_.first(flash_write_size_bytes_)).ok()) {
      return Status::DataLoss();
    }
  }

  // At this point, if data.size_bytes() > 0, the write buffer should be empty.
  // This invariant is checked as part of of steps 2 & 3.

  // Step 2) Write as many block-sized chunks as the data has remaining after
  //         step 1.
  PW_DCHECK(WriteBufferEmpty());

  const size_t final_partial_write_size_bytes =
      data.size_bytes() % flash_write_size_bytes_;

  if (data.size_bytes() >= flash_write_size_bytes_) {
    const size_t write_size_bytes =
        data.size_bytes() - final_partial_write_size_bytes;
    write_address_ += write_size_bytes;
    if (!CommitToFlash(data.first(write_size_bytes)).ok()) {
      return Status::DataLoss();
    }
    data = data.subspan(write_size_bytes);
  }

  // step 3) Put any remaining bytes to the buffer. Put the bytes starting at
  //         the begining of the buffer, since it must be empty if there are
  //         still bytes due to step 1 either cleaned out the buffer or didn't
  //         have any more data to write.
  if (final_partial_write_size_bytes > 0) {
    PW_DCHECK_INT_LT(data.size_bytes(), flash_write_size_bytes_);
    PW_DCHECK(!write_buffer_.empty());

    // Don't need to DCHECK that buffer is empty, nothing writes to it since the
    // previous time it was DCHECK'ed
    std::memcpy(write_buffer_.data(), data.data(), data.size_bytes());
    write_address_ += data.size_bytes();
  }

  return OkStatus();
}

Status BlobStore::AddToWriteBuffer(ConstByteSpan data) {
  if (!ValidToWrite()) {
    return Status::DataLoss();
  }
  if (WriteBytesRemaining() == 0) {
    return Status::OutOfRange();
  }
  if (WriteBufferBytesFree() < data.size_bytes()) {
    return Status::ResourceExhausted();
  }

  size_t bytes_in_buffer = WriteBufferBytesUsed();

  std::memcpy(
      write_buffer_.data() + bytes_in_buffer, data.data(), data.size_bytes());
  write_address_ += data.size_bytes();

  return OkStatus();
}

Status BlobStore::Flush() {
  if (!ValidToWrite()) {
    return Status::DataLoss();
  }
  if (WriteBufferBytesUsed() == 0) {
    return OkStatus();
  }
  // Don't need to check available space, AddToWriteBuffer() will not enqueue
  // more than can be written to flash.

  // If there is no buffer there should never be any bytes enqueued.
  PW_DCHECK(!write_buffer_.empty());

  if (!EraseIfNeeded().ok()) {
    return Status::DataLoss();
  }

  ByteSpan data = span(write_buffer_.data(), WriteBufferBytesUsed());
  size_t write_size_bytes =
      (data.size_bytes() / flash_write_size_bytes_) * flash_write_size_bytes_;
  if (!CommitToFlash(data.first(write_size_bytes)).ok()) {
    return Status::DataLoss();
  }
  data = data.subspan(write_size_bytes);
  PW_DCHECK_INT_LT(data.size_bytes(), flash_write_size_bytes_);

  // Only a multiple of flash_write_size_bytes_ are written in the flush. Any
  // remainder is held until later for either a flush with
  // flash_write_size_bytes buffered or the writer is closed.
  if (!WriteBufferEmpty()) {
    PW_DCHECK_UINT_EQ(data.size_bytes(), WriteBufferBytesUsed());
    // For any leftover bytes less than the flash write size, move them to the
    // start of the bufer.
    std::memmove(write_buffer_.data(), data.data(), data.size_bytes());
  } else {
    PW_DCHECK_UINT_EQ(data.size_bytes(), 0);
  }

  return OkStatus();
}

Status BlobStore::FlushFinalPartialChunk() {
  size_t bytes_in_buffer = WriteBufferBytesUsed();

  PW_DCHECK_UINT_GT(bytes_in_buffer, 0);
  PW_DCHECK_UINT_LE(bytes_in_buffer, flash_write_size_bytes_);
  PW_DCHECK_UINT_LE(flash_write_size_bytes_,
                    MaxDataSizeBytes() - flash_address_);

  // If there is no buffer there should never be any bytes enqueued.
  PW_DCHECK(!write_buffer_.empty());

  PW_LOG_DEBUG(
      "  Remainder %u bytes in write buffer to zero-pad to flash write "
      "size and commit",
      static_cast<unsigned>(bytes_in_buffer));

  // Zero out the remainder of the buffer.
  auto zero_span = write_buffer_.subspan(bytes_in_buffer);
  std::memset(zero_span.data(),
              static_cast<int>(partition_.erased_memory_content()),
              zero_span.size_bytes());

  ConstByteSpan remaining_bytes = write_buffer_.first(flash_write_size_bytes_);
  return CommitToFlash(remaining_bytes, bytes_in_buffer);
}

Status BlobStore::CommitToFlash(ConstByteSpan source, size_t data_bytes) {
  if (data_bytes == 0) {
    data_bytes = source.size_bytes();
  }

  flash_erased_ = false;
  StatusWithSize result = partition_.Write(flash_address_, source);
  flash_address_ += data_bytes;
  if (checksum_algo_ != nullptr) {
    checksum_algo_->Update(source.first(data_bytes));
  }

  if (!result.status().ok()) {
    valid_data_ = false;
  }

  return result.status();
}

// Needs to be in .cc file since PW_CHECK doesn't like being in .h files.
size_t BlobStore::WriteBufferBytesUsed() const {
  PW_CHECK_UINT_GE(write_address_, flash_address_);
  return write_address_ - flash_address_;
}

// Needs to be in .cc file since PW_DCHECK doesn't like being in .h files.
size_t BlobStore::WriteBufferBytesFree() const {
  PW_DCHECK_UINT_GE(write_buffer_.size_bytes(), WriteBufferBytesUsed());
  size_t buffer_remaining = write_buffer_.size_bytes() - WriteBufferBytesUsed();
  return std::min(buffer_remaining, WriteBytesRemaining());
}

Status BlobStore::EraseIfNeeded() {
  if (flash_address_ == 0) {
    // Always just erase. Erase is smart enough to only erase if needed.
    return Erase();
  }
  return OkStatus();
}

StatusWithSize BlobStore::Read(size_t offset, ByteSpan dest) const {
  if (!HasData()) {
    return StatusWithSize::FailedPrecondition();
  }
  if (offset >= ReadableDataBytes()) {
    return StatusWithSize::OutOfRange();
  }

  size_t available_bytes = ReadableDataBytes() - offset;
  size_t read_size = std::min(available_bytes, dest.size_bytes());

  return partition_.Read(offset, dest.first(read_size));
}

Result<ConstByteSpan> BlobStore::GetMemoryMappedBlob() const {
  if (!HasData()) {
    return Status::FailedPrecondition();
  }

  std::byte* mcu_address = partition_.PartitionAddressToMcuAddress(0);
  if (mcu_address == nullptr) {
    return Status::Unimplemented();
  }
  return ConstByteSpan(mcu_address, ReadableDataBytes());
}

size_t BlobStore::ReadableDataBytes() const {
  // TODO(davidrogers): clean up state related to readable bytes.
  return flash_address_;
}

Status BlobStore::Erase() {
  // If already erased our work here is done.
  if (flash_erased_) {
    // The write buffer might already have bytes when this call happens, due to
    // a deferred write.
    PW_DCHECK_UINT_LE(write_address_, write_buffer_.size_bytes());
    PW_DCHECK_UINT_EQ(flash_address_, 0);

    // Erased blobs should be valid as soon as the flash is erased. Even though
    // there are 0 bytes written, they are valid.
    PW_DCHECK(valid_data_);
    return OkStatus();
  }

  // If any writes have been performed, reset the state.
  if (flash_address_ != 0) {
    Invalidate().IgnoreError();  // TODO(b/242598609): Handle Status properly
  }

  PW_TRY(partition_.Erase());

  flash_erased_ = true;

  // Blob data is considered valid as soon as the flash is erased. Even though
  // there are 0 bytes written, they are valid.
  valid_data_ = true;
  return OkStatus();
}

Status BlobStore::Invalidate() {
  // Blob data is considered valid if the flash is erased. Even though
  // there are 0 bytes written, they are valid.
  valid_data_ = flash_erased_;
  ResetChecksum();
  write_address_ = 0;
  flash_address_ = 0;
  file_name_length_ = 0;

  Status status = kvs_.acquire()->Delete(MetadataKey());

  return (status.ok() || status.IsNotFound()) ? OkStatus() : Status::Internal();
}

Status BlobStore::ValidateChecksum(size_t blob_size_bytes,
                                   ChecksumValue expected) {
  if (blob_size_bytes == 0) {
    PW_LOG_INFO("Blob unable to validate checksum of an empty blob");
    return Status::Unavailable();
  }

  if (checksum_algo_ == nullptr) {
    if (expected != 0) {
      PW_LOG_ERROR(
          "Blob invalid to have a checkum value with no checksum algo");
      return Status::DataLoss();
    }

    return OkStatus();
  }

  PW_LOG_DEBUG("Validate checksum of 0x%08x in flash for blob of %u bytes",
               static_cast<unsigned>(expected),
               static_cast<unsigned>(blob_size_bytes));
  PW_TRY(CalculateChecksumFromFlash(blob_size_bytes));

  Status status = checksum_algo_->Verify(as_bytes(span(&expected, 1)));
  PW_LOG_DEBUG("  checksum verify of %s", status.str());

  return status;
}

Status BlobStore::CalculateChecksumFromFlash(size_t bytes_to_check) {
  if (checksum_algo_ == nullptr) {
    return OkStatus();
  }

  checksum_algo_->Reset();

  kvs::FlashPartition::Address address = 0;
  const kvs::FlashPartition::Address end = bytes_to_check;

  constexpr size_t kReadBufferSizeBytes = 32;
  std::array<std::byte, kReadBufferSizeBytes> buffer;
  while (address < end) {
    const size_t read_size = std::min(size_t(end - address), buffer.size());
    PW_TRY(partition_.Read(address, span(buffer).first(read_size)));

    checksum_algo_->Update(buffer.data(), read_size);
    address += read_size;
  }

  // Safe to ignore the return from Finish, checksum_algo_ keeps the state
  // information that it needs.
  checksum_algo_->Finish();
  return OkStatus();
}

Status BlobStore::BlobWriter::SetFileName(std::string_view file_name) {
  if (!open_) {
    return Status::FailedPrecondition();
  }
  PW_DCHECK_NOTNULL(file_name.data());
  PW_DCHECK(store_.writer_open_);

  if (file_name.length() > MaxFileNameLength()) {
    return Status::ResourceExhausted();
  }

  // Stage the file name to the encode buffer, just past the BlobMetadataHeader
  // struct.
  constexpr size_t kFileNameOffset = sizeof(BlobMetadataHeader);
  const ByteSpan file_name_dest = metadata_buffer_.subspan(kFileNameOffset);
  std::memcpy(file_name_dest.data(), file_name.data(), file_name.length());

  store_.file_name_length_ = file_name.length();
  return OkStatus();
}

Status BlobStore::BlobWriter::Open() {
  PW_DCHECK(!open_);
  PW_DCHECK_UINT_GE(metadata_buffer_.size_bytes(),
                    sizeof(internal::BlobMetadataHeader));

  const Status status = store_.OpenWrite();
  if (status.ok()) {
    open_ = true;
  }
  return status;
}

// Validates and commits BlobStore metadata to KVS.
//
// 1. Finalize checksum calculation.
// 2. Check the calculated checksum against data actually committed to flash.
// 3. Build the metadata header into the metadata buffer, placing it before the
//    staged file name (if any).
// 4. Commit the metadata to KVS.
Status BlobStore::BlobWriter::WriteMetadata() {
  // Finalize the in-progress checksum, if any.
  ChecksumValue calculated_checksum = 0;
  if (store_.checksum_algo_ != nullptr) {
    ConstByteSpan checksum = store_.checksum_algo_->Finish();
    std::memcpy(&calculated_checksum,
                checksum.data(),
                std::min(checksum.size(), sizeof(ChecksumValue)));
  }

  // Check the in-memory checksum against the data that was actually committed
  // to flash.
  if (!store_.ValidateChecksum(store_.flash_address_, calculated_checksum)
           .ok()) {
    PW_CHECK_OK(store_.Invalidate());
    return Status::DataLoss();
  }

  // Encode the metadata header. This follows the latest struct behind
  // BlobMetadataHeader. Currently, the order is as follows:
  // - Encode checksum.
  // - Encode stored data size.
  // - Encode version magic.
  // - Encode file name size.
  // - File name, if present, is already staged at the end.
  //
  // Open() guarantees the metadata buffer is large enough to fit the metadata
  // header.
  ByteBuilder metadata_builder(metadata_buffer_);
  metadata_builder.PutUint32(calculated_checksum);
  metadata_builder.PutUint32(store_.flash_address_);
  metadata_builder.PutUint32(internal::MetadataVersion::kLatest);
  metadata_builder.PutUint8(store_.file_name_length_);
  PW_DCHECK_INT_EQ(metadata_builder.size(), sizeof(BlobMetadataHeader));
  PW_DCHECK_OK(metadata_builder.status());

  // If a filename was provided, it is already written to the correct location
  // in the buffer. When the file name was set, the metadata buffer was verified
  // to fit the requested name in addition to the metadata header. If it doesn't
  // fit now, something's very wrong.
  const size_t bytes_to_write =
      metadata_builder.size() + store_.file_name_length_;
  PW_DCHECK(metadata_buffer_.size_bytes() >= bytes_to_write);

  // Do final commit to KVS.
  return store_.kvs_.acquire()->Put(store_.MetadataKey(),
                                    metadata_buffer_.first(bytes_to_write));
}

Status BlobStore::BlobWriter::Close() {
  if (!open_) {
    return Status::FailedPrecondition();
  }
  open_ = false;

  // This is a lambda so the BlobWriter will be unconditionally closed even if
  // the final flash commits fail. This lambda may early return to Close() if
  // errors are encountered, but Close() will not return without updating both
  // the BlobWriter and BlobStore such that neither are open for writes
  // anymore.
  auto do_close_write = [&]() -> Status {
    // If not valid to write, there was data loss and the close will result in a
    // not valid blob. Don't need to flush any write buffered bytes.
    if (!store_.ValidToWrite()) {
      return Status::DataLoss();
    }

    if (store_.write_address_ == 0) {
      return OkStatus();
    }

    PW_LOG_DEBUG(
        "Blob writer close of %u byte blob, with %u bytes still in write "
        "buffer",
        static_cast<unsigned>(store_.write_address_),
        static_cast<unsigned>(store_.WriteBufferBytesUsed()));

    // Do a Flush of any flash_write_size_bytes_ sized chunks so any remaining
    // bytes in the write buffer are less than flash_write_size_bytes_.
    PW_TRY(store_.Flush());

    // If any bytes remain in buffer it is because it is a chunk less than
    // flash_write_size_bytes_. Pad the chunk to flash_write_size_bytes_ and
    // write it to flash.
    if (!store_.WriteBufferEmpty()) {
      PW_TRY(store_.FlushFinalPartialChunk());
    }
    PW_DCHECK(store_.WriteBufferEmpty());

    if (!WriteMetadata().ok()) {
      return Status::DataLoss();
    }

    return OkStatus();
  };

  const Status status = do_close_write();
  store_.writer_open_ = false;

  if (!status.ok()) {
    store_.valid_data_ = false;
    return Status::DataLoss();
  }
  return OkStatus();
}

size_t BlobStore::BlobReader::ConservativeLimit(LimitType limit) const {
  if (open_ && limit == LimitType::kRead) {
    return store_.ReadableDataBytes() - offset_;
  }
  return 0;
}

Status BlobStore::BlobReader::Open(size_t offset) {
  PW_DCHECK(!open_);
  PW_TRY(store_.Init());
  if (!store_.HasData()) {
    return Status::FailedPrecondition();
  }
  if (offset >= store_.ReadableDataBytes()) {
    return Status::InvalidArgument();
  }

  offset_ = offset;
  Status status = store_.OpenRead();
  if (status.ok()) {
    open_ = true;
  }
  return status;
}

size_t BlobStore::BlobReader::DoTell() {
  return open_ ? offset_ : kUnknownPosition;
}

Status BlobStore::BlobReader::DoSeek(ptrdiff_t offset, Whence origin) {
  if (!open_) {
    return Status::FailedPrecondition();
  }

  // Note that Open ensures HasData() which in turn guarantees
  // store_.ReadableDataBytes() > 0.

  size_t pos = offset_;
  PW_TRY(CalculateSeek(offset, origin, store_.ReadableDataBytes() - 1, pos));
  offset_ = pos;

  return OkStatus();
}

StatusWithSize BlobStore::BlobReader::DoRead(ByteSpan dest) {
  if (!open_) {
    return StatusWithSize::FailedPrecondition();
  }

  StatusWithSize status = store_.Read(offset_, dest);
  if (status.ok()) {
    offset_ += status.size();
  }
  return status;
}

}  // namespace pw::blob_store
