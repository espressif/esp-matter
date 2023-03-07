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

#include "pw_file/flat_file_system.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

#include "gtest/gtest.h"
#include "pw_bytes/span.h"
#include "pw_file/file.pwpb.h"
#include "pw_protobuf/decoder.h"
#include "pw_rpc/raw/test_method_context.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"

namespace pw::file {
namespace {

class FakeFile : public FlatFileSystemService::Entry {
 public:
  constexpr FakeFile(std::string_view file_name, size_t size, uint32_t file_id)
      : name_(file_name), size_(size), file_id_(file_id) {}

  StatusWithSize Name(span<char> dest) override {
    if (name_.empty()) {
      return StatusWithSize(Status::NotFound(), 0);
    }

    size_t bytes_to_copy = std::min(dest.size_bytes(), name_.size());
    memcpy(dest.data(), name_.data(), bytes_to_copy);
    if (bytes_to_copy != name_.size()) {
      return StatusWithSize(Status::ResourceExhausted(), bytes_to_copy);
    }

    return StatusWithSize(OkStatus(), bytes_to_copy);
  }

  size_t SizeBytes() override { return size_; }

  FlatFileSystemService::Entry::FilePermissions Permissions() const override {
    return FlatFileSystemService::Entry::FilePermissions::NONE;
  }

  Status Delete() override { return Status::Unimplemented(); }

  FlatFileSystemService::Entry::Id FileId() const override { return file_id_; }

 private:
  std::string_view name_;
  size_t size_;
  uint32_t file_id_;
};

bool EntryHasName(FlatFileSystemService::Entry* entry) {
  std::array<char, 4> expected_name;
  StatusWithSize file_name_sws = entry->Name(expected_name);
  return file_name_sws.size() != 0;
}

// Compares a serialized Path message to a flat file system entry.
void ComparePathToEntry(ConstByteSpan serialized_path,
                        FlatFileSystemService::Entry* entry) {
  std::array<char, 64> expected_name;
  StatusWithSize file_name_sws = entry->Name(expected_name);

  // A partial name read shouldn't happen.
  ASSERT_EQ(OkStatus(), file_name_sws.status());

  protobuf::Decoder decoder(serialized_path);
  while (decoder.Next().ok()) {
    switch (decoder.FieldNumber()) {
      case static_cast<uint32_t>(pw::file::pwpb::Path::Fields::PATH): {
        std::string_view serialized_name;
        EXPECT_EQ(OkStatus(), decoder.ReadString(&serialized_name));
        size_t name_bytes_to_read =
            std::min(serialized_name.size(), file_name_sws.size());
        EXPECT_EQ(0,
                  memcmp(expected_name.data(),
                         serialized_name.data(),
                         name_bytes_to_read));
        break;
      }

      case static_cast<uint32_t>(pw::file::pwpb::Path::Fields::PERMISSIONS): {
        uint32_t seralized_permissions;
        EXPECT_EQ(OkStatus(), decoder.ReadUint32(&seralized_permissions));
        EXPECT_EQ(static_cast<uint32_t>(entry->Permissions()),
                  seralized_permissions);
        break;
      }

      case static_cast<uint32_t>(pw::file::pwpb::Path::Fields::SIZE_BYTES): {
        uint32_t serialized_file_size;
        EXPECT_EQ(OkStatus(), decoder.ReadUint32(&serialized_file_size));
        EXPECT_EQ(static_cast<uint32_t>(entry->SizeBytes()),
                  serialized_file_size);
        break;
      }

      case static_cast<uint32_t>(pw::file::pwpb::Path::Fields::FILE_ID): {
        uint32_t serialized_file_id;
        EXPECT_EQ(OkStatus(), decoder.ReadUint32(&serialized_file_id));
        EXPECT_EQ(static_cast<uint32_t>(entry->FileId()), serialized_file_id);
        break;
      }

      default:
        // unexpected result.
        // TODO(amontanez) something here.
        break;
    }
  }
}

size_t ValidateExpectedPaths(
    span<FlatFileSystemService::Entry*> flat_file_system,
    const rpc::PayloadsView& results) {
  size_t serialized_path_entry_count = 0;
  size_t file_system_index = 0;
  for (ConstByteSpan response : results) {
    protobuf::Decoder decoder(response);
    while (decoder.Next().ok()) {
      constexpr uint32_t kListResponsePathsFieldNumber =
          static_cast<uint32_t>(pw::file::pwpb::ListResponse::Fields::PATHS);
      EXPECT_EQ(decoder.FieldNumber(), kListResponsePathsFieldNumber);
      if (decoder.FieldNumber() != kListResponsePathsFieldNumber) {
        return 0;
      }

      serialized_path_entry_count++;

      // Skip any file system entries without names.
      while (!EntryHasName(flat_file_system[file_system_index])) {
        file_system_index++;
        EXPECT_GT(flat_file_system.size(), file_system_index);
      }

      // There's a 1:1 mapping in the same order for all files that have a name.
      ConstByteSpan serialized_path;
      EXPECT_EQ(OkStatus(), decoder.ReadBytes(&serialized_path));
      ComparePathToEntry(serialized_path,
                         flat_file_system[file_system_index++]);
    }
  }
  return serialized_path_entry_count;
}

TEST(FlatFileSystem, EncodingBufferSizeBytes) {
  EXPECT_EQ(FlatFileSystemService::EncodingBufferSizeBytes(10),
            2u /* path nested message key and size */ + 12 /* path */ +
                2 /* permissions */ + 6 /* size_bytes */ + 6 /* file_id */);
  EXPECT_EQ(FlatFileSystemService::EncodingBufferSizeBytes(10, 2),
            2 * (1u + 1 + 12 + 2 + 6 + 6));
  EXPECT_EQ(FlatFileSystemService::EncodingBufferSizeBytes(100, 3),
            3 * (1u + 1 + 102 + 2 + 6 + 6));
}

TEST(FlatFileSystem, List_NoFiles) {
  PW_RAW_TEST_METHOD_CONTEXT(FlatFileSystemServiceWithBuffer<1>, List)
  ctx{span<FlatFileSystemService::Entry*>()};
  ctx.call(ConstByteSpan());

  EXPECT_TRUE(ctx.done());
  EXPECT_EQ(OkStatus(), ctx.status());
  EXPECT_EQ(0u, ctx.responses().size());
}

TEST(FlatFileSystem, List_OneFile) {
  FakeFile file{"compressed.zip.gz", 2, 1231};
  std::array<FlatFileSystemService::Entry*, 1> static_file_system{&file};

  PW_RAW_TEST_METHOD_CONTEXT(FlatFileSystemServiceWithBuffer<20>, List)
  ctx(static_file_system);
  ctx.call(ConstByteSpan());

  EXPECT_EQ(1u, ValidateExpectedPaths(static_file_system, ctx.responses()));
}

TEST(FlatFileSystem, List_ThreeFiles) {
  std::array<FakeFile, 3> files{
      {{"SNAP_001", 372, 9}, {"tokens.csv", 808, 15038202}, {"a.txt", 0, 2}}};
  std::array<FlatFileSystemService::Entry*, 3> static_file_system{
      &files[0], &files[1], &files[2]};

  PW_RAW_TEST_METHOD_CONTEXT(FlatFileSystemServiceWithBuffer<10>, List)
  ctx(static_file_system);
  ctx.call(ConstByteSpan());

  EXPECT_EQ(3u, ValidateExpectedPaths(static_file_system, ctx.responses()));
}

TEST(FlatFileSystem, List_UnnamedFile) {
  FakeFile file{"", 0, 0};
  std::array<FlatFileSystemService::Entry*, 1> static_file_system{&file};

  PW_RAW_TEST_METHOD_CONTEXT(FlatFileSystemServiceWithBuffer<10>, List)
  ctx(static_file_system);
  ctx.call(ConstByteSpan());

  EXPECT_EQ(0u, ValidateExpectedPaths(static_file_system, ctx.responses()));
}

TEST(FlatFileSystem, List_FileMissingName) {
  std::array<FakeFile, 3> files{
      {{"SNAP_001", 372, 9}, {"", 808, 15038202}, {"a.txt", 0, 2}}};
  std::array<FlatFileSystemService::Entry*, 3> static_file_system{
      &files[0], &files[1], &files[2]};

  PW_RAW_TEST_METHOD_CONTEXT(FlatFileSystemServiceWithBuffer<10>, List)
  ctx(static_file_system);
  ctx.call(ConstByteSpan());

  EXPECT_EQ(2u, ValidateExpectedPaths(static_file_system, ctx.responses()));
}

}  // namespace
}  // namespace pw::file
