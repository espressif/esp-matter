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

#include "pw_blob_store/flat_file_system_entry.h"

#include <cstddef>
#include <mutex>

#include "pw_assert/check.h"
#include "pw_blob_store/blob_store.h"
#include "pw_file/flat_file_system.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_sync/virtual_basic_lockable.h"

namespace pw::blob_store {

Status FlatFileSystemBlobStoreEntry::Init() {
  std::lock_guard lock(blob_store_lock_);
  if (initialized_) {
    return OkStatus();
  }
  const Status status = blob_store_.Init();
  initialized_ = status.ok();
  return status;
}

void FlatFileSystemBlobStoreEntry::EnsureInitialized() {
  {  // Only hold lock for initial check so Init() doesn't recursively lock.
    std::lock_guard lock(blob_store_lock_);
    if (initialized_) {
      return;
    }
  }

  // Don't inline the Init() into the DCHECK() as disabling the DCHECK()
  // statement would disable the Init() call as well.
  const Status status = Init();
  PW_DCHECK_OK(status);
}

StatusWithSize FlatFileSystemBlobStoreEntry::Name(span<char> dest) {
  EnsureInitialized();
  std::lock_guard lock(blob_store_lock_);
  BlobStore::BlobReader reader(blob_store_);
  if (const Status status = reader.Open(); !status.ok()) {
    // When a BlobStore is empty, Open() reports FAILED_PRECONDITION. The
    // FlatFileSystemService expects NOT_FOUND when a file is not present at the
    // entry.
    if (status.IsFailedPrecondition()) {
      return StatusWithSize(Status::NotFound(), 0);
    } else if (status.IsUnavailable()) {
      return StatusWithSize(Status::Unavailable(), 0);
    } else {
      return StatusWithSize(Status::Internal(), 0);
    }
  }
  return reader.GetFileName(dest);
}

size_t FlatFileSystemBlobStoreEntry::SizeBytes() {
  EnsureInitialized();
  std::lock_guard lock(blob_store_lock_);
  BlobStore::BlobReader reader(blob_store_);
  if (!reader.Open().ok()) {
    return 0;
  }
  return reader.ConservativeReadLimit();
}

// TODO(b/234888404): This file can be deleted even though it is read-only.
// This type of behavior should be possible to express via the FileSystem RPC
// service.
Status FlatFileSystemBlobStoreEntry::Delete() {
  EnsureInitialized();
  std::lock_guard lock(blob_store_lock_);

  BlobStore::BlobWriterWithBuffer blob_writer(blob_store_);
  PW_TRY(blob_writer.Open());
  return blob_writer.Discard();
}

}  // namespace pw::blob_store
