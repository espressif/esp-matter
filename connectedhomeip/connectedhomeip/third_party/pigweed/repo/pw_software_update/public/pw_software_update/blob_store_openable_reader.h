// Copyright 2022 The Pigweed Authors
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

#include "pw_blob_store/blob_store.h"
#include "pw_software_update/openable_reader.h"
#include "pw_status/status.h"
#include "pw_stream/stream.h"

namespace pw::software_update {

class BlobStoreOpenableReader final : public OpenableReader {
 public:
  explicit constexpr BlobStoreOpenableReader(blob_store::BlobStore& blob_store)
      : blob_store_(blob_store), blob_reader_(blob_store_) {}

  Status Open() override { return blob_reader_.Open(); }
  Status Close() override { return blob_reader_.Close(); }
  bool IsOpen() override { return blob_reader_.IsOpen(); }
  stream::SeekableReader& reader() override { return blob_reader_; }

 private:
  blob_store::BlobStore& blob_store_;
  blob_store::BlobStore::BlobReader blob_reader_;
};

}  // namespace pw::software_update
