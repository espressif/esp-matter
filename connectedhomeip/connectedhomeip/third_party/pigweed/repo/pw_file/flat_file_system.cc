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

#define PW_LOG_MODULE_NAME "FS"

#include "pw_file/flat_file_system.h"

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "pw_assert/check.h"
#include "pw_bytes/span.h"
#include "pw_file/file.pwpb.h"
#include "pw_log/log.h"
#include "pw_protobuf/decoder.h"
#include "pw_protobuf/encoder.h"
#include "pw_protobuf/serialized_size.h"
#include "pw_rpc/raw/server_reader_writer.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"

namespace pw::file {

using Entry = FlatFileSystemService::Entry;

Status FlatFileSystemService::EnumerateFile(
    Entry& entry, pwpb::ListResponse::StreamEncoder& output_encoder) {
  StatusWithSize sws = entry.Name(file_name_buffer_);
  if (!sws.ok()) {
    return sws.status();
  }
  {
    pwpb::Path::StreamEncoder encoder = output_encoder.GetPathsEncoder();

    encoder
        .WritePath(reinterpret_cast<const char*>(file_name_buffer_.data()),
                   sws.size())
        .IgnoreError();
    encoder.WriteSizeBytes(entry.SizeBytes()).IgnoreError();
    encoder.WritePermissions(entry.Permissions()).IgnoreError();
    encoder.WriteFileId(entry.FileId()).IgnoreError();
  }
  return output_encoder.status();
}

void FlatFileSystemService::EnumerateAllFiles(RawServerWriter& writer) {
  for (Entry* entry : entries_) {
    PW_DCHECK_NOTNULL(entry);
    // For now, don't try to pack entries.
    pwpb::ListResponse::MemoryEncoder encoder(encoding_buffer_);
    if (Status status = EnumerateFile(*entry, encoder); !status.ok()) {
      if (status != Status::NotFound()) {
        PW_LOG_ERROR("Failed to enumerate file (id: %u) with status %d",
                     static_cast<unsigned>(entry->FileId()),
                     static_cast<int>(status.code()));
      }
      continue;
    }

    Status write_status = writer.Write(encoder);
    if (!write_status.ok()) {
      writer.Finish(write_status)
          .IgnoreError();  // TODO(b/242598609): Handle Status properly
      return;
    }
  }
  writer.Finish(OkStatus())
      .IgnoreError();  // TODO(b/242598609): Handle Status properly
}

void FlatFileSystemService::List(ConstByteSpan request,
                                 RawServerWriter& writer) {
  protobuf::Decoder decoder(request);
  // If a file name was provided, try and find and enumerate the file.
  while (decoder.Next().ok()) {
    if (decoder.FieldNumber() !=
        static_cast<uint32_t>(pwpb::ListRequest::Fields::PATH)) {
      continue;
    }

    std::string_view file_name_view;
    if (!decoder.ReadString(&file_name_view).ok() ||
        file_name_view.length() == 0) {
      writer.Finish(Status::DataLoss())
          .IgnoreError();  // TODO(b/242598609): Handle Status properly
      return;
    }

    // Find and enumerate the file requested.
    Result<Entry*> result = FindFile(file_name_view);
    if (!result.ok()) {
      writer.Finish(result.status())
          .IgnoreError();  // TODO(b/242598609): Handle Status properly
      return;
    }

    pwpb::ListResponse::MemoryEncoder encoder(encoding_buffer_);
    Status proto_encode_status = EnumerateFile(*result.value(), encoder);
    if (!proto_encode_status.ok()) {
      writer.Finish(proto_encode_status)
          .IgnoreError();  // TODO(b/242598609): Handle Status properly
      return;
    }

    writer.Finish(writer.Write(encoder))
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    return;
  }

  // If no path was provided in the ListRequest, just enumerate everything.
  EnumerateAllFiles(writer);
}

void FlatFileSystemService::Delete(ConstByteSpan request,
                                   rpc::RawUnaryResponder& responder) {
  protobuf::Decoder decoder(request);
  while (decoder.Next().ok()) {
    if (decoder.FieldNumber() !=
        static_cast<uint32_t>(pwpb::DeleteRequest::Fields::PATH)) {
      continue;
    }

    std::string_view file_name_view;
    if (!decoder.ReadString(&file_name_view).ok()) {
      responder.Finish({}, Status::DataLoss()).IgnoreError();
      return;
    }
    responder.Finish({}, FindAndDeleteFile(file_name_view)).IgnoreError();
    return;
  }
  responder.Finish({}, Status::InvalidArgument()).IgnoreError();
}

Result<Entry*> FlatFileSystemService::FindFile(std::string_view file_name) {
  Status search_status;
  for (Entry* entry : entries_) {
    PW_DCHECK_NOTNULL(entry);
    StatusWithSize sws = entry->Name(file_name_buffer_);

    // If there not an exact file name length match, don't try and check against
    // a prefix.
    if (!sws.ok() || file_name.length() != sws.size()) {
      if (sws.status() != Status::NotFound()) {
        PW_LOG_ERROR("Failed to read file name (id: %u) with status %d",
                     static_cast<unsigned>(entry->FileId()),
                     static_cast<int>(sws.status().code()));
      }
      continue;
    }

    if (memcmp(file_name.data(), file_name_buffer_.data(), file_name.size()) ==
        0) {
      return entry;
    }
  }

  search_status.Update(Status::NotFound());
  return search_status;
}

Status FlatFileSystemService::FindAndDeleteFile(std::string_view file_name) {
  Result<Entry*> result = FindFile(file_name);
  if (!result.ok()) {
    return result.status();
  }

  return result.value()->Delete();
}

}  // namespace pw::file
