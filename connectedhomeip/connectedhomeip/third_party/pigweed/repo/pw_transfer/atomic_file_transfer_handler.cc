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
#include "pw_transfer/atomic_file_transfer_handler.h"

#include <filesystem>
#include <system_error>

#include "pw_log/log.h"
#include "pw_status/status.h"
#include "pw_stream/std_file_stream.h"
#include "pw_transfer_private/filename_generator.h"

namespace pw::transfer {

namespace {
// Linux Error code for Cross-Device Link Error.
constexpr auto CROSS_DEVICE_LINK_ERROR = 18;

pw::Status EnsureDirectoryExists(std::string_view filepath) {
  const auto target_directory = std::filesystem::path{filepath}.parent_path();
  return std::filesystem::exists(target_directory) ||
                 std::filesystem::create_directories(target_directory)
             ? pw::OkStatus()
             : pw::Status::Internal();
}

// Copy file and remove on succes.
// If the copy fails, the file `input_target` is not removed.
pw::Status CopyFile(const std::string_view input_target,
                    const std::string_view output_target) {
  auto err = std::error_code{};
  std::filesystem::copy(input_target,
                        output_target,
                        std::filesystem::copy_options::overwrite_existing,
                        err);
  if (err) {
    PW_LOG_ERROR("Error with status code: %d (%s) during copy of file %s",
                 err.value(),
                 err.message().c_str(),
                 input_target.data());
    return pw::Status::Internal();
  }
  PW_LOG_INFO("Successfully copied the file.");
  if (!std::filesystem::remove(input_target)) {
    PW_LOG_WARN("Failed to remove tmp file %s", input_target.data());
  }
  return pw::OkStatus();
}

// Uses the same approach as unix `mv` command. First try to rename. If we get
// a cross-device link error, copies then deletes input_target.
pw::Status RenameFile(const std::string_view input_target,
                      const std::string_view output_target) {
  auto err = std::error_code{};
  std::filesystem::rename(input_target, output_target, err);
  if (err && err.value() == CROSS_DEVICE_LINK_ERROR) {
    PW_LOG_INFO("%s[%d] during rename of file %s. Trying Copy/Remove.",
                err.message().c_str(),
                err.value(),
                input_target.data());
    return CopyFile(input_target, output_target);
  }
  return err ? pw::Status::Internal() : pw::OkStatus();
}

}  // namespace

Status AtomicFileTransferHandler::PrepareRead() {
  auto file_path = path_.c_str();
  PW_LOG_DEBUG("Preparing read for file %s", file_path);
  if (!std::filesystem::exists(file_path)) {
    PW_LOG_ERROR("File does not exist, path: %s", file_path);
    return Status::NotFound();
  }
  set_reader(stream_.emplace<stream::StdFileReader>(file_path));
  return OkStatus();
}

void AtomicFileTransferHandler::FinalizeRead(Status) {
  std::get<stream::StdFileReader>(stream_).Close();
}

Status AtomicFileTransferHandler::PrepareWrite() {
  const std::string tmp_file = GetTempFilePath(path_);
  PW_LOG_DEBUG("Preparing write for file %s", tmp_file.c_str());
  set_writer(stream_.emplace<stream::StdFileWriter>(tmp_file.c_str()));
  return OkStatus();
}

Status AtomicFileTransferHandler::FinalizeWrite(Status status) {
  std::get<stream::StdFileWriter>(stream_).Close();
  auto tmp_file = GetTempFilePath(path_);
  if (!status.ok() || !std::filesystem::exists(tmp_file) ||
      std::filesystem::is_empty(tmp_file)) {
    PW_LOG_ERROR("Transfer unsuccesful, attempt to remove temp file %s",
                 tmp_file.c_str());
    // Remove temp file if transfer fails.
    return std::filesystem::remove(tmp_file) ? status : Status::Aborted();
  }

  const auto directory_exists_status = EnsureDirectoryExists(path_);
  if (!directory_exists_status.ok()) {
    std::filesystem::remove(tmp_file);
    return directory_exists_status;
  }

  PW_LOG_DEBUG(
      "Copying file from: %s, to: %s", tmp_file.c_str(), path_.c_str());
  const auto rename_status = RenameFile(tmp_file, path_);
  if (!rename_status.ok()) {
    std::filesystem::remove(tmp_file);
    return rename_status;
  }

  PW_LOG_INFO("File transfer was successful.");
  return OkStatus();
}

}  // namespace pw::transfer
