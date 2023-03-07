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

#include "pw_blob_store/blob_store.h"
#include "pw_file/flat_file_system.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_sync/lock_annotations.h"
#include "pw_sync/virtual_basic_lockable.h"

namespace pw::blob_store {

class FlatFileSystemBlobStoreEntry final
    : public file::FlatFileSystemService::Entry {
 public:
  using file::FlatFileSystemService::Entry::FilePermissions;
  using file::FlatFileSystemService::Entry::Id;

  // File IDs must be globally unique, and map to a pw_transfer TransferService
  // read/write handler ID.
  //
  // TODO(pwbug/492): When BlobStore access is thread-safe, the mutex can be
  // dropped.
  FlatFileSystemBlobStoreEntry(Id file_id,
                               FilePermissions permissions,
                               BlobStore& blob_store,
                               sync::VirtualBasicLockable& blob_store_lock)
      : file_id_(file_id),
        permissions_(permissions),
        initialized_(false),
        blob_store_(blob_store),
        blob_store_lock_(blob_store_lock) {}

  // Initializes the underlying BlobStore. Calling this before use is optional,
  // as this class will also lazy-init
  Status Init();

  StatusWithSize Name(span<char> dest) final;

  size_t SizeBytes() final;

  FilePermissions Permissions() const final { return permissions_; }

  Status Delete() final;

  Id FileId() const override { return file_id_; }

 private:
  // Initializes the BlobStore if uninitialized, and CHECK()s initialization
  // to ensure it succeeded.
  void EnsureInitialized();

  const Id file_id_;
  const FilePermissions permissions_;
  bool initialized_ PW_GUARDED_BY(blob_store_lock_);
  blob_store::BlobStore& blob_store_ PW_GUARDED_BY(blob_store_lock_);
  sync::VirtualBasicLockable& blob_store_lock_;
};

}  // namespace pw::blob_store
