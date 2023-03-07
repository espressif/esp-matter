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

#include <filesystem>
#include <string>
#include <string_view>
#include <variant>

#include "pw_status/status.h"
#include "pw_stream/std_file_stream.h"
#include "pw_transfer/handler.h"

namespace pw::transfer {

// The AtomicFileTransferHandler is intended to be used as a transfer
// handler for files. It ensures that the target file of the transfer is always
// in a correct state. In particular, the transfer is first done to a temporary
// file and once complete, the original targeted file is updated.
class AtomicFileTransferHandler : public ReadWriteHandler {
 public:
  AtomicFileTransferHandler(uint32_t resource_id, std::string_view file_path)
      : ReadWriteHandler(resource_id), path_(file_path) {}

  AtomicFileTransferHandler(const AtomicFileTransferHandler& rhs) = delete;
  AtomicFileTransferHandler& operator=(const AtomicFileTransferHandler&) =
      delete;
  ~AtomicFileTransferHandler() override = default;

  // Function called prior to initializing a read transfer.
  Status PrepareRead() override;
  // Function called after a read transfer is done.
  // Status indicates whether transfer was done successfully.
  void FinalizeRead(Status) override;
  // Function called prior to initializing a write transfer.
  Status PrepareWrite() override;
  // Function called after a write transfer is done.
  // Status indicates whether transfer was done successfully.
  Status FinalizeWrite(Status) override;

 private:
  std::string path_;
  std::variant<std::monostate, stream::StdFileReader, stream::StdFileWriter>
      stream_{};
};

}  // namespace pw::transfer
