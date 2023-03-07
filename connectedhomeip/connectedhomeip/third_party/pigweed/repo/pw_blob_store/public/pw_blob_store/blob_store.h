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

#include "pw_assert/assert.h"
#include "pw_blob_store/internal/metadata_format.h"
#include "pw_bytes/span.h"
#include "pw_kvs/checksum.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/key_value_store.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_status/try.h"
#include "pw_stream/seek.h"
#include "pw_stream/stream.h"
#include "pw_sync/borrow.h"

namespace pw::blob_store {

// BlobStore is a storage container for a single blob of data. BlobStore is
// a FlashPartition-backed persistent storage system with integrated data
// integrity checking that serves as a lightweight alternative to a file
// system.
//
// Write and read are only done using the BlobWriter and BlobReader classes.
//
// Once a blob write is closed, reopening to write will discard the previous
// blob.
//
// Write blob:
//  0) Create BlobWriter instance
//  1) BlobWriter::Open().
//  2) Add data using BlobWriter::Write().
//  3) BlobWriter::Close().
//
// Read blob:
//  0) Create BlobReader instance
//  1) BlobReader::Open().
//  2) Read data using BlobReader::Read() or
//     BlobReader::GetMemoryMappedBlob().
//  3) BlobReader::Close().
class BlobStore {
 public:
  // Implement the stream::Writer and erase interface for a BlobStore. If not
  // already erased, the Write will do any needed erase.
  //
  // Only one writter (of either type) is allowed to be open at a time.
  // Additionally, writers are unable to open if a reader is already open.
  class BlobWriter : public stream::NonSeekableWriter {
   public:
    constexpr BlobWriter(BlobStore& store, ByteSpan metadata_buffer)
        : store_(store), metadata_buffer_(metadata_buffer), open_(false) {}
    BlobWriter(const BlobWriter&) = delete;
    BlobWriter& operator=(const BlobWriter&) = delete;
    ~BlobWriter() override {
      if (open_) {
        Close().IgnoreError();  // TODO(b/242598609): Handle Status properly
      }
    }

    static constexpr size_t RequiredMetadataBufferSize(
        size_t max_file_name_size) {
      return max_file_name_size + sizeof(internal::BlobMetadataHeader);
    }

    // Open a blob for writing/erasing. Open will invalidate any existing blob
    // that may be stored, and will not retain the previous file name. Can not
    // open when already open. Only one writer is allowed to be open at a time.
    // Returns:
    //
    // Preconditions:
    // This writer must not already be open.
    // This writer's metadata encode buffer must be at least the size of
    // internal::BlobMetadataHeader.
    //
    // OK - success.
    // UNAVAILABLE - Unable to open, another writer or reader instance is
    //     already open.
    Status Open();

    // Finalize a blob write. Flush all remaining buffered data to storage and
    // store blob metadata. Close fails in the closed state, do NOT retry Close
    // on error. An error may or may not result in an invalid blob stored.
    // Returns:
    //
    // OK - success.
    // DATA_LOSS - Error writing data or fail to verify written data.
    Status Close();

    bool IsOpen() { return open_; }

    // Erase the blob partition and reset state for a new blob. Explicit calls
    // to Erase are optional, beginning a write will do any needed Erase.
    // Returns:
    //
    // OK - success.
    // UNAVAILABLE - Unable to erase while reader is open.
    // [error status] - flash erase failed.
    Status Erase() {
      return open_ ? store_.Erase() : Status::FailedPrecondition();
    }

    // Discard the current blob. Any written bytes to this point are considered
    // invalid. Returns:
    //
    // OK - success.
    // FAILED_PRECONDITION - not open.
    Status Discard() {
      return open_ ? store_.Invalidate() : Status::FailedPrecondition();
    }

    // Sets file name to be associated with the data written by this
    // ``BlobWriter``. This may be changed any time before Close() is called.
    //
    // Calling Discard() or Erase() will clear any set file name.
    //
    // The underlying buffer behind file_name may be invalidated after this
    // function returns as the string is copied to the internally managed encode
    // buffer.
    //
    // Preconditions:
    // This writer must be open.
    //
    // OK - successfully set file name.
    // RESOURCE_EXHAUSTED - File name too large to fit in metadata encode
    //   buffer, file name not set.
    Status SetFileName(std::string_view file_name);

    size_t CurrentSizeBytes() const {
      return open_ ? store_.write_address_ : 0;
    }

    // Max file name length, not including null terminator (null terminators
    // are not stored).
    size_t MaxFileNameLength() {
      return metadata_buffer_.size_bytes() <
                     sizeof(internal::BlobMetadataHeader)
                 ? 0
                 : metadata_buffer_.size_bytes() -
                       sizeof(internal::BlobMetadataHeader);
    }

   protected:
    Status DoWrite(ConstByteSpan data) override {
      return open_ ? store_.Write(data) : Status::FailedPrecondition();
    }

    // Commits changes to KVS as a BlobStore metadata entry.
    Status WriteMetadata();

    BlobStore& store_;
    ByteSpan metadata_buffer_;
    bool open_;

   private:
    // Probable (not guaranteed) minimum number of bytes at this time that can
    // be written. This is not necessarily the full number of bytes remaining in
    // the blob. Returns zero if, in the current state, Write would return
    // status other than OK. See stream.h for additional details.
    size_t ConservativeLimit(LimitType limit) const override {
      if (open_ && limit == LimitType::kWrite) {
        return store_.WriteBytesRemaining();
      }
      return 0;
    }
  };

  template <size_t kMaxFileNameSize = 0>
  class BlobWriterWithBuffer final : public BlobWriter {
   public:
    constexpr BlobWriterWithBuffer(BlobStore& store)
        : BlobWriter(store, buffer_), buffer_() {}

   private:
    std::array<std::byte, RequiredMetadataBufferSize(kMaxFileNameSize)> buffer_;
  };

  // Implement the stream::Writer and erase interface with deferred action for a
  // BlobStore. If not already erased, the Flush will do any needed erase.
  //
  // Only one writter (of either type) is allowed to be open at a time.
  // Additionally, writers are unable to open if a reader is already open.
  class DeferredWriter : public BlobWriter {
   public:
    constexpr DeferredWriter(BlobStore& store, ByteSpan metadata_buffer)
        : BlobWriter(store, metadata_buffer) {}
    DeferredWriter(const DeferredWriter&) = delete;
    DeferredWriter& operator=(const DeferredWriter&) = delete;
    ~DeferredWriter() override {}

    // Flush data in the write buffer. Only a multiple of flash_write_size_bytes
    // are written in the flush. Any remainder is held until later for either
    // a flush with flash_write_size_bytes buffered or the writer is closed.
    Status Flush() {
      return open_ ? store_.Flush() : Status::FailedPrecondition();
    }

    // Probable (not guaranteed) minimum number of bytes at this time that can
    // be written. This is not necessarily the full number of bytes remaining in
    // the blob. Returns zero if, in the current state, Write would return
    // status other than OK. See stream.h for additional details.
    size_t ConservativeLimit(LimitType limit) const final {
      if (open_ && limit == LimitType::kWrite) {
        // Deferred writes need to fit in the write buffer.
        return store_.WriteBufferBytesFree();
      }
      return 0;
    }

   private:
    // Similar to normal Write, but instead immediately writing out to flash,
    // it only buffers the data. A flush or Close is reqired to get bytes
    // writen out to flash.
    //
    // AddToWriteBuffer will continue to accept new data after Flush has an
    // erase error (buffer space permitting). Write errors during Flush will
    // result in no new data being accepted.
    Status DoWrite(ConstByteSpan data) final {
      return open_ ? store_.AddToWriteBuffer(data)
                   : Status::FailedPrecondition();
    }
  };

  template <size_t kMaxFileNameSize = 0>
  class DeferredWriterWithBuffer final : public DeferredWriter {
   public:
    constexpr DeferredWriterWithBuffer(BlobStore& store)
        : DeferredWriter(store, buffer_), buffer_() {}

   private:
    std::array<std::byte, RequiredMetadataBufferSize(kMaxFileNameSize)> buffer_;
  };

  // Implement stream::Reader interface for BlobStore. Multiple readers may be
  // open at the same time, but readers may not be open with a writer open.
  class BlobReader final : public stream::SeekableReader {
   public:
    constexpr BlobReader(BlobStore& store)
        : store_(store), open_(false), offset_(0) {}

    BlobReader(const BlobReader&) = delete;
    BlobReader& operator=(const BlobReader&) = delete;

    ~BlobReader() override {
      if (open_) {
        Close().IgnoreError();
      }
    }

    // Open to do a blob read at the given offset in to the blob. Can not open
    // when already open. Multiple readers can be open at the same time.
    // Returns:
    //
    //   OK - success.
    //   FAILED_PRECONDITION - No readable blob available.
    //   INVALID_ARGUMENT - Invalid offset.
    //   UNAVAILABLE - Unable to open, already open.
    //
    Status Open(size_t offset = 0);

    // Finish reading a blob. Close fails in the closed state, do NOT retry
    // Close on error. Returns:
    //
    //   OK - success
    //   FAILED_PRECONDITION - already closed
    //
    Status Close() {
      if (!open_) {
        return Status::FailedPrecondition();
      }
      open_ = false;
      return store_.CloseRead();
    }

    // Copies the file name of the stored data to `dest`, and returns the number
    // of bytes written to the destination buffer. The string is not
    // null-terminated.
    //
    // Returns:
    //   OK - File name copied, size contains file name length.
    //   RESOURCE_EXHAUSTED - `dest` too small to fit file name, size contains
    //     first N bytes of the file name.
    //   NOT_FOUND - No file name set for this blob.
    //   FAILED_PRECONDITION - not open
    //
    StatusWithSize GetFileName(span<char> dest) {
      return open_ ? store_.GetFileName(dest)
                   : StatusWithSize::FailedPrecondition();
    }

    bool IsOpen() const { return open_; }

    // Get a span with the MCU pointer and size of the data. Returns:
    //
    //   OK with span - Valid span respresenting the blob data
    //   FAILED_PRECONDITION - Reader not open.
    //   UNIMPLEMENTED - Memory mapped access not supported for this blob.
    //   FAILED_PRECONDITION - Writer is closed
    //
    Result<ConstByteSpan> GetMemoryMappedBlob() {
      return open_ ? store_.GetMemoryMappedBlob()
                   : Status::FailedPrecondition();
    }

   private:
    // Probable (not guaranteed) minimum number of bytes at this time that can
    // be read. Returns zero if, in the current state, Read would return status
    // other than OK. See stream.h for additional details.
    size_t ConservativeLimit(LimitType limit) const override;

    size_t DoTell() override;

    Status DoSeek(ptrdiff_t offset, Whence origin) override;

    StatusWithSize DoRead(ByteSpan dest) override;

    BlobStore& store_;
    bool open_;
    size_t offset_;
  };

  // BlobStore
  // name - Name of blob store, used for metadata KVS key
  // partition - Flash partiton to use for this blob. Blob uses the entire
  //     partition for blob data.
  // checksum_algo - Optional checksum for blob integrity checking. Use nullptr
  //     for no check.
  // kvs - KVS used for storing blob metadata.
  // write_buffer - Used for buffering writes. Needs to be at least
  //     flash_write_size_bytes.
  // flash_write_size_bytes - Size in bytes to use for flash write operations.
  //     This should be chosen to balance optimal write size and required buffer
  //     size. Must be greater than or equal to flash write alignment, less than
  //     or equal to flash sector size.
  BlobStore(std::string_view name,
            kvs::FlashPartition& partition,
            kvs::ChecksumAlgorithm* checksum_algo,
            sync::Borrowable<kvs::KeyValueStore>& kvs,
            ByteSpan write_buffer,
            size_t flash_write_size_bytes)
      : name_(name),
        partition_(partition),
        checksum_algo_(checksum_algo),
        kvs_(kvs),
        write_buffer_(write_buffer),
        flash_write_size_bytes_(flash_write_size_bytes),
        initialized_(false),
        valid_data_(false),
        flash_erased_(false),
        writer_open_(false),
        readers_open_(0),
        write_address_(0),
        flash_address_(0),
        file_name_length_(0) {}

  BlobStore(const BlobStore&) = delete;
  BlobStore& operator=(const BlobStore&) = delete;

  // Initialize the blob instance. Checks if storage is erased or has any stored
  // blob data. Returns:
  //
  // OK - success.
  Status Init();

  // Maximum number of data bytes this BlobStore is able to store.
  size_t MaxDataSizeBytes() const;

  // Get the current data state of the blob without needing to instantiate
  // and/or open a reader or writer. This check is independent of any writers or
  // readers of this blob that might exist (open or closed).
  //
  // NOTE: This state can be changed by any writer that is open(ed) for this
  //       blob. Readers can not be opened until any open writers are closed.
  //
  // true -  Blob is valid/OK and has at least 1 data byte.
  // false -  Blob is either invalid or does not have any data bytes
  bool HasData() const { return (valid_data_ && ReadableDataBytes() > 0); }

 private:
  Status LoadMetadata();

  // Open to do a blob write. Returns:
  //
  // OK - success.
  // UNAVAILABLE - Unable to open writer, another writer or reader instance is
  //     already open.
  Status OpenWrite();

  // Open to do a blob read. Returns:
  //
  // OK - success.
  // FAILED_PRECONDITION - Unable to open, no valid blob available.
  Status OpenRead();

  // Finalize a blob write. Flush all remaining buffered data to storage and
  // store blob metadata. Returns:
  //
  // OK - success, valid complete blob.
  // DATA_LOSS - Error during write (this close or previous write/flush). Blob
  //     is closed and marked as invalid.
  Status CloseRead();

  // Write/append data to the in-progress blob write. Data is written
  // sequentially, with each append added directly after the previous. Data is
  // not guaranteed to be fully written out to storage on Write return. Returns:
  //
  // OK - successful write/enqueue of data.
  // RESOURCE_EXHAUSTED - unable to write all of requested data at this time. No
  //     data written.
  // OUT_OF_RANGE - Writer has been exhausted, similar to EOF. No data written,
  //     no more will be written.
  // DATA_LOSS - Error during write (this write or previous write/flush). No
  //     more will be written by following Write calls for current blob (until
  //     erase/new blob started).
  Status Write(ConstByteSpan data);

  // Similar to Write, but instead immediately writing out to flash, it only
  // buffers the data. A flush or Close is reqired to get bytes writen out to
  // flash.
  //
  // AddToWriteBuffer will continue to accept new data after Flush has an erase
  // error (buffer space permitting). Write errors during Flush will result in
  // no new data being accepted.
  //
  // OK - successful write/enqueue of data.
  // RESOURCE_EXHAUSTED - unable to write all of requested data at this time. No
  //     data written.
  // OUT_OF_RANGE - Writer has been exhausted, similar to EOF. No data written,
  //     no more will be written.
  // DATA_LOSS - Error during a previous write/flush. No more will be written by
  //     following Write calls for current blob (until erase/new blob started).
  Status AddToWriteBuffer(ConstByteSpan data);

  // Flush data in the write buffer. Only a multiple of flash_write_size_bytes
  // are written in the flush. Any remainder is held until later for either a
  // flush with flash_write_size_bytes buffered or the writer is closed.
  //
  // OK - successful write/enqueue of data.
  // DATA_LOSS - Error during write (this flush or previous write/flush). No
  //     more will be written by following Write calls for current blob (until
  //     erase/new blob started).
  Status Flush();

  // Flush a chunk of data in the write buffer smaller than
  // flash_write_size_bytes. This is only for the final flush as part of the
  // CloseWrite. The partial chunk is padded to flash_write_size_bytes and a
  // flash_write_size_bytes chunk is written to flash.
  //
  // OK - successful write/enqueue of data.
  // DATA_LOSS - Error during write (this flush or previous write/flush). No
  //     more will be written by following Write calls for current blob (until
  //     erase/new blob started).
  Status FlushFinalPartialChunk();

  // Commit data to flash and update flash_address_ with data bytes written. The
  // only time data_bytes should be manually specified is for a CloseWrite with
  // an unaligned-size chunk remaining in the buffer that has been zero padded
  // to alignment.
  Status CommitToFlash(ConstByteSpan source, size_t data_bytes = 0);

  // Blob is valid/OK to write to. Blob is considered valid to write if no data
  // has been written due to the auto/implicit erase on write start.
  //
  // true - Blob is valid and OK to write to.
  // false - Blob has previously had an error and not valid for writing new
  //     data.
  bool ValidToWrite() { return (valid_data_ == true) || (flash_address_ == 0); }

  bool WriteBufferEmpty() const { return flash_address_ == write_address_; }

  size_t WriteBufferBytesUsed() const;

  size_t WriteBufferBytesFree() const;

  Status EraseIfNeeded();

  // Read valid data. Attempts to read the lesser of output.size_bytes() or
  // available bytes worth of data. Returns:
  //
  // OK with span of bytes read - success, between 1 and dest.size_bytes() were
  //     read.
  // INVALID_ARGUMENT - offset is invalid.
  // FAILED_PRECONDITION - Reader unable/not in state to read data.
  // RESOURCE_EXHAUSTED - unable to read any bytes at this time. No bytes read.
  //     Try again once bytes become available.
  // OUT_OF_RANGE - Reader has been exhausted, similar to EOF. No bytes read, no
  //     more will be read.
  StatusWithSize Read(size_t offset, ByteSpan dest) const;

  // Get a span with the MCU pointer and size of the data. Returns:
  //
  // OK with span - Valid span respresenting the blob data
  // FAILED_PRECONDITION - Blob not in a state to read data
  // UNIMPLEMENTED - Memory mapped access not supported for this blob.
  Result<ConstByteSpan> GetMemoryMappedBlob() const;

  // Size of blob/readable data, in bytes.
  size_t ReadableDataBytes() const;

  size_t WriteBytesRemaining() const {
    return MaxDataSizeBytes() - write_address_;
  }

  Status Erase();

  Status Invalidate();

  void ResetChecksum() {
    if (checksum_algo_ != nullptr) {
      checksum_algo_->Reset();
    }
  }

  Status ValidateChecksum(size_t blob_size_bytes,
                          internal::ChecksumValue expected);

  Status CalculateChecksumFromFlash(size_t bytes_to_check);

  const std::string_view MetadataKey() const { return name_; }

  // Copies the file name of the stored data to `dest`, and returns the number
  // of bytes written to the destination buffer. The string is not
  // null-terminated.
  //
  // Returns:
  //   OK - File name copied, size contains file name length.
  //   RESOURCE_EXHAUSTED - `dest` too small to fit file name, size contains
  //     first N bytes of the file name.
  //   NOT_FOUND - No file name set for this blob.
  //   FAILED_PRECONDITION - BlobStore has not been initialized.
  StatusWithSize GetFileName(span<char> dest) const;

  std::string_view name_;
  kvs::FlashPartition& partition_;
  // checksum_algo_ of nullptr indicates no checksum algorithm.
  kvs::ChecksumAlgorithm* const checksum_algo_;
  sync::Borrowable<kvs::KeyValueStore>& kvs_;
  ByteSpan write_buffer_;

  // Size in bytes of flash write operations. This should be chosen to balance
  // optimal write size and required buffer size. Must be GE flash write
  // alignment, LE flash sector size.
  const size_t flash_write_size_bytes_;

  //
  // Internal state for Blob store
  //
  // TODO(davidrogers): Consolidate blob state to a single struct

  // Initialization has been done.
  bool initialized_;

  // Bytes stored are valid and good. Blob is OK to read and write to. Set as
  // soon as blob is erased. Even when bytes written is still 0, they are valid.
  bool valid_data_;

  // Blob partition is currently erased and ready to write a new blob.
  bool flash_erased_;

  // BlobWriter instance is currently open
  bool writer_open_;

  // Count of open BlobReader instances
  size_t readers_open_;

  // Current index for end of overall blob data. Represents current byte size of
  // blob data since the FlashPartition starts at address 0.
  kvs::FlashPartition::Address write_address_;

  // Current index of end of data written to flash. Number of buffered data
  // bytes is write_address_ - flash_address_.
  kvs::FlashPartition::Address flash_address_;

  // Length of the stored blob's filename.
  size_t file_name_length_;
};

// Creates a BlobStore with the buffer of kBufferSizeBytes.
//
// kBufferSizeBytes - Size in bytes of write buffer to create.
// name - Name of blob store, used for metadata KVS key
// partition - Flash partition to use for this blob. Blob uses the entire
//     partition for blob data.
// checksum_algo - Optional checksum for blob integrity checking. Use nullptr
//     for no check.
// kvs - KVS used for storing blob metadata.
// write_buffer - Used for buffering writes. Needs to be at least
//     flash_write_size_bytes.
// flash_write_size_bytes - Size in bytes to use for flash write operations.
//     This should be chosen to balance optimal write size and required buffer
//     size. Must be greater than or equal to flash write alignment, less than
//     or equal to flash sector size.

template <size_t kBufferSizeBytes>
class BlobStoreBuffer : public BlobStore {
 public:
  explicit BlobStoreBuffer(std::string_view name,
                           kvs::FlashPartition& partition,
                           kvs::ChecksumAlgorithm* checksum_algo,
                           sync::Borrowable<kvs::KeyValueStore>& kvs,
                           size_t flash_write_size_bytes)
      : BlobStore(name,
                  partition,
                  checksum_algo,
                  kvs,
                  buffer_,
                  flash_write_size_bytes) {}

 private:
  std::array<std::byte, kBufferSizeBytes> buffer_;
};

}  // namespace pw::blob_store
