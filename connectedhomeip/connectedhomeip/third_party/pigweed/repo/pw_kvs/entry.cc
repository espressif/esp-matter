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

#define PW_LOG_MODULE_NAME "KVS"
#define PW_LOG_LEVEL PW_KVS_LOG_LEVEL

#include "pw_kvs/internal/entry.h"

#include <cinttypes>
#include <cstring>

#include "pw_kvs_private/config.h"
#include "pw_log/log.h"
#include "pw_status/try.h"

namespace pw::kvs::internal {

static_assert(
    kMaxFlashAlignment >= Entry::kMinAlignmentBytes,
    "Flash alignment is required to be at least Entry::kMinAlignmentBytes");

constexpr size_t kWriteBufferSize =
    std::max(kMaxFlashAlignment, 4 * Entry::kMinAlignmentBytes);

using std::byte;

Status Entry::Read(FlashPartition& partition,
                   Address address,
                   const internal::EntryFormats& formats,
                   Entry* entry) {
  EntryHeader header;
  PW_TRY(partition.Read(address, sizeof(header), &header));

  if (partition.AppearsErased(as_bytes(span(&header.magic, 1)))) {
    return Status::NotFound();
  }
  if (header.key_length_bytes > kMaxKeyLength) {
    return Status::DataLoss();
  }

  const EntryFormat* format = formats.Find(header.magic);
  if (format == nullptr) {
    PW_LOG_ERROR("Found corrupt magic: %" PRIx32 " at address %u",
                 header.magic,
                 unsigned(address));
    return Status::DataLoss();
  }

  *entry = Entry(&partition, address, *format, header);
  return OkStatus();
}

Status Entry::ReadKey(FlashPartition& partition,
                      Address address,
                      size_t key_length,
                      char* key) {
  if (key_length == 0u || key_length > kMaxKeyLength) {
    return Status::DataLoss();
  }

  return partition.Read(address + sizeof(EntryHeader), key_length, key)
      .status();
}

Entry::Entry(FlashPartition& partition,
             Address address,
             const EntryFormat& format,
             Key key,
             span<const byte> value,
             uint16_t value_size_bytes,
             uint32_t transaction_id)
    : Entry(&partition,
            address,
            format,
            {.magic = format.magic,
             .checksum = 0,
             .alignment_units =
                 alignment_bytes_to_units(partition.alignment_bytes()),
             .key_length_bytes = static_cast<uint8_t>(key.size()),
             .value_size_bytes = value_size_bytes,
             .transaction_id = transaction_id}) {
  if (checksum_algo_ != nullptr) {
    span<const byte> checksum = CalculateChecksum(key, value);
    std::memcpy(&header_.checksum,
                checksum.data(),
                std::min(checksum.size(), sizeof(header_.checksum)));
  }
}

StatusWithSize Entry::Write(Key key, span<const byte> value) const {
  FlashPartition::Output flash(partition(), address_);
  return AlignedWrite<kWriteBufferSize>(
      flash,
      alignment_bytes(),
      {as_bytes(span(&header_, 1)), as_bytes(span(key)), value});
}

Status Entry::Update(const EntryFormat& new_format,
                     uint32_t new_transaction_id) {
  checksum_algo_ = new_format.checksum;
  header_.magic = new_format.magic;
  header_.alignment_units =
      alignment_bytes_to_units(partition_->alignment_bytes());
  header_.transaction_id = new_transaction_id;

  // If we could write the header last, we could avoid reading the entry twice
  // when moving an entry. However, to support alignments greater than the
  // header size, we first read the entire value to calculate the new checksum,
  // then write the full entry in WriteFrom.
  return CalculateChecksumFromFlash();
}

StatusWithSize Entry::Copy(Address new_address) const {
  PW_LOG_DEBUG("Copying entry from %u to %u as ID %" PRIu32,
               unsigned(address()),
               unsigned(new_address),
               transaction_id());

  FlashPartition::Output output(partition(), new_address);
  AlignedWriterBuffer<kWriteBufferSize> writer(alignment_bytes(), output);

  // Use this object's header rather than the header in flash of flash, since
  // this Entry may have been updated.
  PW_TRY_WITH_SIZE(writer.Write(&header_, sizeof(header_)));

  // Write only the key and value from the original entry.
  FlashPartition::Input input(partition(), address() + sizeof(EntryHeader));
  PW_TRY_WITH_SIZE(writer.Write(input, key_length() + value_size()));
  return writer.Flush();
}

StatusWithSize Entry::ReadValue(span<byte> buffer, size_t offset_bytes) const {
  if (offset_bytes > value_size()) {
    return StatusWithSize::OutOfRange();
  }

  const size_t remaining_bytes = value_size() - offset_bytes;
  const size_t read_size = std::min(buffer.size(), remaining_bytes);

  StatusWithSize result = partition().Read(
      address_ + sizeof(EntryHeader) + key_length() + offset_bytes,
      buffer.subspan(0, read_size));
  PW_TRY_WITH_SIZE(result);

  if (read_size != remaining_bytes) {
    return StatusWithSize::ResourceExhausted(read_size);
  }
  return StatusWithSize(read_size);
}

Status Entry::ValueMatches(span<const std::byte> value) const {
  if (value_size() != value.size_bytes()) {
    return Status::NotFound();
  }

  Address address = address_ + sizeof(EntryHeader) + key_length();
  Address end = address + value_size();
  const std::byte* value_ptr = value.data();

  std::array<std::byte, 2 * kMinAlignmentBytes> buffer;
  while (address < end) {
    const size_t read_size = std::min(size_t(end - address), buffer.size());
    PW_TRY(partition_->Read(address, span(buffer).first(read_size)));

    if (std::memcmp(buffer.data(), value_ptr, read_size) != 0) {
      return Status::NotFound();
    }

    address += read_size;
    value_ptr += read_size;
  }

  return OkStatus();
}

Status Entry::VerifyChecksum(Key key, span<const byte> value) const {
  if (checksum_algo_ == nullptr) {
    return header_.checksum == 0 ? OkStatus() : Status::DataLoss();
  }
  CalculateChecksum(key, value);
  return checksum_algo_->Verify(checksum_bytes());
}

Status Entry::VerifyChecksumInFlash() const {
  // Read the entire entry piece-by-piece into a small buffer. If the entry is
  // 32 B or less, only one read is required.
  union {
    EntryHeader header_to_verify;
    byte buffer[sizeof(EntryHeader) * 2];
  };

  size_t bytes_to_read = size();
  size_t read_size = std::min(sizeof(buffer), bytes_to_read);

  Address read_address = address_;

  // Read the first chunk, which includes the header, and compare the checksum.
  PW_TRY(partition().Read(read_address, read_size, buffer));

  if (header_to_verify.checksum != header_.checksum) {
    PW_LOG_ERROR("Expected checksum 0x%08" PRIx32 ", found 0x%08" PRIx32,
                 header_.checksum,
                 header_to_verify.checksum);
    return Status::DataLoss();
  }

  if (checksum_algo_ == nullptr) {
    return header_.checksum == 0 ? OkStatus() : Status::DataLoss();
  }

  // The checksum is calculated as if the header's checksum field were 0.
  header_to_verify.checksum = 0;

  checksum_algo_->Reset();

  while (true) {
    // Add the chunk in the buffer to the checksum.
    checksum_algo_->Update(buffer, read_size);

    bytes_to_read -= read_size;
    if (bytes_to_read == 0u) {
      break;
    }

    // Read the next chunk into the buffer.
    read_address += read_size;
    read_size = std::min(sizeof(buffer), bytes_to_read);
    PW_TRY(partition().Read(read_address, read_size, buffer));
  }

  checksum_algo_->Finish();
  return checksum_algo_->Verify(checksum_bytes());
}

void Entry::DebugLog() const {
  PW_LOG_DEBUG("Entry [%s]: ", deleted() ? "tombstone" : "present");
  PW_LOG_DEBUG("   Address      = 0x%x", unsigned(address_));
  PW_LOG_DEBUG("   Transaction  = %u", unsigned(transaction_id()));
  PW_LOG_DEBUG("   Magic        = 0x%x", unsigned(magic()));
  PW_LOG_DEBUG("   Checksum     = 0x%x", unsigned(header_.checksum));
  PW_LOG_DEBUG("   Key length   = 0x%x", unsigned(key_length()));
  PW_LOG_DEBUG("   Value length = 0x%x", unsigned(value_size()));
  PW_LOG_DEBUG("   Entry size   = 0x%x", unsigned(size()));
  PW_LOG_DEBUG("   Alignment    = 0x%x", unsigned(alignment_bytes()));
}

span<const byte> Entry::CalculateChecksum(const Key key,
                                          span<const byte> value) const {
  checksum_algo_->Reset();

  {
    EntryHeader header_for_checksum = header_;
    header_for_checksum.checksum = 0;

    checksum_algo_->Update(&header_for_checksum, sizeof(header_for_checksum));
    checksum_algo_->Update(as_bytes(span(key)));
    checksum_algo_->Update(value);
  }

  AddPaddingBytesToChecksum();

  return checksum_algo_->Finish();
}

Status Entry::CalculateChecksumFromFlash() {
  header_.checksum = 0;

  if (checksum_algo_ == nullptr) {
    return OkStatus();
  }

  checksum_algo_->Reset();
  checksum_algo_->Update(&header_, sizeof(header_));

  Address address = address_ + sizeof(EntryHeader);
  // To handle alignment changes, do not read the padding. The padding is added
  // after checksumming the key and value from flash.
  const Address end = address_ + content_size();

  std::array<std::byte, 2 * kMinAlignmentBytes> buffer;
  while (address < end) {
    const size_t read_size = std::min(size_t(end - address), buffer.size());
    PW_TRY(partition_->Read(address, span(buffer).first(read_size)));

    checksum_algo_->Update(buffer.data(), read_size);
    address += read_size;
  }

  AddPaddingBytesToChecksum();

  span checksum = checksum_algo_->Finish();
  std::memcpy(&header_.checksum,
              checksum.data(),
              std::min(checksum.size(), sizeof(header_.checksum)));
  return OkStatus();
}

void Entry::AddPaddingBytesToChecksum() const {
  constexpr byte padding[kMinAlignmentBytes - 1] = {};
  size_t padding_to_add = Padding(content_size(), alignment_bytes());

  while (padding_to_add != 0u) {
    const size_t chunk_size = std::min(padding_to_add, sizeof(padding));
    checksum_algo_->Update(padding, chunk_size);
    padding_to_add -= chunk_size;
  }
}

}  // namespace pw::kvs::internal
