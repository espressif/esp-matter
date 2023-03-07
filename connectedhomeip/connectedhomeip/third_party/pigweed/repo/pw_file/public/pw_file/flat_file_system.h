// Copyright 2021 The Pigweed Authors
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
#include <cstdint>
#include <string_view>

#include "pw_bytes/span.h"
#include "pw_file/file.pwpb.h"
#include "pw_file/file.raw_rpc.pb.h"
#include "pw_protobuf/serialized_size.h"
#include "pw_result/result.h"
#include "pw_rpc/raw/server_reader_writer.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"

namespace pw::file {

// This implements the pw.file.FileSystem RPC service. This implementation
// has a strict limitation that everything is treated as if the file system
// was "flat" (i.e. no directories). This means there's no concept of logical
// directories, despite any "path like" naming that may be employed by a user.
class FlatFileSystemService
    : public pw_rpc::raw::FileSystem::Service<FlatFileSystemService> {
 public:
  class Entry {
   public:
    using FilePermissions = ::pw::file::pwpb::Path::Permissions;
    using Id = uint32_t;

    Entry() = default;
    virtual ~Entry() = default;

    // All readable files MUST be named, and names must be globally unique to
    // prevent ambiguity. Unnamed file entries will NOT be enumerated by a
    // FlatFileSystemService. The returned status must indicate the length
    // of the string written to `dest`, and should NOT include any null
    // terminator that may have been written.
    //
    // Note: The bounded string written to `dest` is not expected to be
    // null-terminated, and should be treated like a std::string_view.
    //
    // Returns:
    //   OK - Successfully read file name to `dest`.
    //   NOT_FOUND - No file to enumerate for this entry.
    //   RESOURCE_EXHAUSTED - `dest` buffer too small to fit the full file name.
    virtual StatusWithSize Name(span<char> dest) = 0;

    virtual size_t SizeBytes() = 0;
    virtual FilePermissions Permissions() const = 0;

    // Deleting a file, if allowed, should cause the backing data store to be
    // cleared. Read-only files should also no longer enumerate (i.e. Name()
    // should return NOT_FOUND). Write-only and read/write files may still
    // enumerate but with SizeBytes() reporting zero.
    virtual Status Delete() = 0;

    // File IDs must be globally unique, and map to a pw_transfer
    // TransferService read/write handler.
    virtual Id FileId() const = 0;
  };

  // Returns the size of encoding buffer guaranteed to support encoding
  // minimum_entries paths with file names up max_file_name_length.
  static constexpr size_t EncodingBufferSizeBytes(size_t max_file_name_length,
                                                  size_t minimum_entries = 1) {
    return minimum_entries *
           protobuf::SizeOfDelimitedField(
               pwpb::ListResponse::Fields::PATHS,
               EncodedPathProtoSizeBytes(max_file_name_length));
  }

  // Constructs a flat file system from a static list of file entries.
  //
  // Args:
  //   entry_list - A list of pointers to all Entry objects that may
  //     contain files. These pointers may not be null. The span's underlying
  //     buffer must outlive this object.
  //   encoding_buffer - Used internally by this class to encode its responses.
  //   file_name_buffer - Used internally by this class to find and enumerate
  //     files. Should be large enough to hold the longest expected file name.
  //     The span's underlying buffer must outlive this object.
  //   max_file_name_length - Number of bytes to reserve for the file name.
  constexpr FlatFileSystemService(span<Entry*> entry_list,
                                  span<std::byte> encoding_buffer,
                                  span<char> file_name_buffer)
      : encoding_buffer_(encoding_buffer),
        file_name_buffer_(file_name_buffer),
        entries_(entry_list) {}

  // Method definitions for pw.file.FileSystem.
  void List(ConstByteSpan request, RawServerWriter& writer);

  // Returns:
  //   OK - File successfully deleted.
  //   NOT_FOUND - Could not find
  void Delete(ConstByteSpan request, rpc::RawUnaryResponder& responder);

 private:
  // Returns the maximum size of a single encoded Path proto.
  static constexpr size_t EncodedPathProtoSizeBytes(
      size_t max_file_name_length) {
    return protobuf::SizeOfFieldString(pwpb::Path::Fields::PATH,
                                       max_file_name_length) +
           protobuf::SizeOfFieldEnum(pwpb::Path::Fields::PERMISSIONS,
                                     pwpb::Path::Permissions::READ_AND_WRITE) +
           protobuf::SizeOfFieldUint32(pwpb::Path::Fields::SIZE_BYTES) +
           protobuf::SizeOfFieldUint32(pwpb::Path::Fields::FILE_ID);
  }

  Result<Entry*> FindFile(std::string_view file_name);
  Status FindAndDeleteFile(std::string_view file_name);

  Status EnumerateFile(Entry& entry,
                       pwpb::ListResponse::StreamEncoder& output_encoder);
  void EnumerateAllFiles(RawServerWriter& writer);

  const span<std::byte> encoding_buffer_;
  const span<char> file_name_buffer_;
  const span<Entry*> entries_;
};

// Provides the encoding and file name buffers to a FlatFileSystemService.
template <unsigned kMaxFileNameLength,
          unsigned kMinGuaranteedEntriesPerResponse = 1>
class FlatFileSystemServiceWithBuffer : public FlatFileSystemService {
 public:
  constexpr FlatFileSystemServiceWithBuffer(span<Entry*> entry_list)
      : FlatFileSystemService(entry_list, encoding_buffer_, file_name_buffer_) {
  }

 private:
  static_assert(kMaxFileNameLength > 0u);

  std::byte encoding_buffer_[EncodingBufferSizeBytes(
      kMaxFileNameLength, kMinGuaranteedEntriesPerResponse)];
  char file_name_buffer_[kMaxFileNameLength];
};
}  // namespace pw::file
