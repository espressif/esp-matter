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

#define PW_LOG_MODULE_NAME "PWSU"
#define PW_LOG_LEVEL PW_LOG_LEVEL_WARN

#include "pw_software_update/bundled_update_service_pwpb.h"

#include <mutex>
#include <string_view>

#include "pw_log/log.h"
#include "pw_result/result.h"
#include "pw_software_update/config.h"
#include "pw_software_update/manifest_accessor.h"
#include "pw_software_update/update_bundle.pwpb.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_status/try.h"
#include "pw_string/string_builder.h"
#include "pw_string/util.h"
#include "pw_sync/borrow.h"
#include "pw_sync/mutex.h"
#include "pw_tokenizer/tokenize.h"

namespace pw::software_update {
namespace {
using BorrowedStatus =
    sync::BorrowedPointer<BundledUpdateStatus::Message, sync::Mutex>;

// TODO(keir): Convert all the CHECKs in the RPC service to gracefully report
// errors.
#define SET_ERROR(res, message, ...)                                     \
  do {                                                                   \
    PW_LOG_ERROR(message, __VA_ARGS__);                                  \
    if (!IsFinished()) {                                                 \
      Finish(res);                                                       \
      {                                                                  \
        BorrowedStatus borrowed_status = status_.acquire();              \
        size_t note_size = borrowed_status->note.max_size();             \
        borrowed_status->note.resize(note_size);                         \
        PW_TOKENIZE_TO_BUFFER(                                           \
            &borrowed_status->note, &(note_size), message, __VA_ARGS__); \
        borrowed_status->note.resize(note_size);                         \
      }                                                                  \
    }                                                                    \
  } while (false)
}  // namespace

Status BundledUpdateService::GetStatus(const pw::protobuf::Empty::Message&,
                                       BundledUpdateStatus::Message& response) {
  response = *status_.acquire();
  return OkStatus();
}

Status BundledUpdateService::Start(const StartRequest::Message& request,
                                   BundledUpdateStatus::Message& response) {
  std::lock_guard lock(mutex_);
  // Check preconditions.
  const BundledUpdateState::Enum state = status_.acquire()->state;
  if (state != BundledUpdateState::Enum::kInactive) {
    SET_ERROR(BundledUpdateResult::Enum::kUnknownError,
              "Start() can only be called from INACTIVE state. "
              "Current state: %d. Abort() then Reset() must be called first",
              static_cast<int>(state));
    response = *status_.acquire();
    return Status::FailedPrecondition();
  }

  {
    BorrowedStatus borrowed_status = status_.acquire();
    PW_DCHECK(!borrowed_status->transfer_id.has_value());
    PW_DCHECK(!borrowed_status->result.has_value());
    PW_DCHECK(
        !borrowed_status->current_state_progress_hundreth_percent.has_value());
    PW_DCHECK(borrowed_status->bundle_filename.empty());
    PW_DCHECK(borrowed_status->note.empty());
  }

  // Notify the backend of pending transfer.
  if (const Status status = backend_.BeforeUpdateStart(); !status.ok()) {
    SET_ERROR(BundledUpdateResult::Enum::kUnknownError,
              "Backend error on BeforeUpdateStart()");
    response = *status_.acquire();
    return status;
  }

  // Enable bundle transfer.
  Result<uint32_t> possible_transfer_id =
      backend_.EnableBundleTransferHandler(request.bundle_filename);
  if (!possible_transfer_id.ok()) {
    SET_ERROR(BundledUpdateResult::Enum::kTransferFailed,
              "Couldn't enable bundle transfer");
    response = *status_.acquire();
    return possible_transfer_id.status();
  }

  // Update state.
  {
    BorrowedStatus borrowed_status = status_.acquire();
    borrowed_status->transfer_id = possible_transfer_id.value();
    if (!request.bundle_filename.empty()) {
      borrowed_status->bundle_filename = request.bundle_filename;
    }
    borrowed_status->state = BundledUpdateState::Enum::kTransferring;
    response = *borrowed_status;
  }
  return OkStatus();
}

Status BundledUpdateService::SetTransferred(
    const pw::protobuf::Empty::Message&,
    BundledUpdateStatus::Message& response) {
  const BundledUpdateState::Enum state = status_.acquire()->state;

  if (state != BundledUpdateState::Enum::kTransferring &&
      state != BundledUpdateState::Enum::kInactive) {
    std::lock_guard lock(mutex_);
    SET_ERROR(BundledUpdateResult::Enum::kUnknownError,
              "SetTransferred() can only be called from TRANSFERRING or "
              "INACTIVE state. State: %d",
              static_cast<int>(state));
    response = *status_.acquire();
    return OkStatus();
  }

  NotifyTransferSucceeded();

  response = *status_.acquire();
  return OkStatus();
}

// TODO(elipsitz): Check for "ABORTING" state and bail if it's set.
void BundledUpdateService::DoVerify() {
  std::lock_guard guard(mutex_);
  const BundledUpdateState::Enum state = status_.acquire()->state;

  if (state == BundledUpdateState::Enum::kVerified) {
    return;  // Already done!
  }

  // Ensure we're in the right state.
  if (state != BundledUpdateState::Enum::kTransferred) {
    SET_ERROR(BundledUpdateResult::Enum::kVerifyFailed,
              "DoVerify() must be called from TRANSFERRED state. State: %d",
              static_cast<int>(state));
    return;
  }

  status_.acquire()->state = BundledUpdateState::Enum::kVerifying;

  // Notify backend about pending verify.
  if (const Status status = backend_.BeforeBundleVerify(); !status.ok()) {
    SET_ERROR(BundledUpdateResult::Enum::kVerifyFailed,
              "Backend::BeforeBundleVerify() failed");
    return;
  }

  // Do the actual verify.
  Status status = bundle_.OpenAndVerify();
  if (!status.ok()) {
    SET_ERROR(BundledUpdateResult::Enum::kVerifyFailed,
              "Bundle::OpenAndVerify() failed");
    return;
  }
  bundle_open_ = true;

  // Have the backend verify the user_manifest if present.
  if (!backend_.VerifyManifest(bundle_.GetManifest()).ok()) {
    SET_ERROR(BundledUpdateResult::Enum::kVerifyFailed,
              "Backend::VerifyUserManifest() failed");
    return;
  }

  // Notify backend we're done verifying.
  status = backend_.AfterBundleVerified();
  if (!status.ok()) {
    SET_ERROR(BundledUpdateResult::Enum::kVerifyFailed,
              "Backend::AfterBundleVerified() failed");
    return;
  }
  status_.acquire()->state = BundledUpdateState::Enum::kVerified;
}

Status BundledUpdateService::Verify(const pw::protobuf::Empty::Message&,
                                    BundledUpdateStatus::Message& response) {
  std::lock_guard lock(mutex_);
  const BundledUpdateState::Enum state = status_.acquire()->state;

  // Already done? Bail.
  if (state == BundledUpdateState::Enum::kVerified) {
    PW_LOG_DEBUG("Skipping verify since already verified");
    return OkStatus();
  }

  // TODO(elipsitz): Remove the transferring permitted state here ASAP.
  // Ensure we're in the right state.
  if ((state != BundledUpdateState::Enum::kTransferring) &&
      (state != BundledUpdateState::Enum::kTransferred)) {
    SET_ERROR(BundledUpdateResult::Enum::kVerifyFailed,
              "Verify() must be called from TRANSFERRED state. State: %d",
              static_cast<int>(state));
    response = *status_.acquire();
    return Status::FailedPrecondition();
  }

  // TODO(elipsitz): We should probably make this mode idempotent.
  // Already doing what was asked? Bail.
  if (work_enqueued_) {
    PW_LOG_DEBUG("Verification is already active");
    return OkStatus();
  }

  // The backend's ApplyReboot as part of DoApply() shall be configured
  // such that this RPC can send out the reply before the device reboots.
  const Status status = work_queue_.PushWork([this] {
    {
      std::lock_guard y_lock(this->mutex_);
      PW_DCHECK(this->work_enqueued_);
    }
    this->DoVerify();
    {
      std::lock_guard y_lock(this->mutex_);
      this->work_enqueued_ = false;
    }
  });
  if (!status.ok()) {
    SET_ERROR(BundledUpdateResult::Enum::kVerifyFailed,
              "Unable to equeue apply to work queue");
    response = *status_.acquire();
    return status;
  }
  work_enqueued_ = true;

  response = *status_.acquire();
  return OkStatus();
}

Status BundledUpdateService::Apply(const pw::protobuf::Empty::Message&,
                                   BundledUpdateStatus::Message& response) {
  std::lock_guard lock(mutex_);
  const BundledUpdateState::Enum state = status_.acquire()->state;

  // We do not wait to go into a finished error state if we're already
  // applying, instead just let them know that yes we are working on it --
  // hold on.
  if (state == BundledUpdateState::Enum::kApplying) {
    PW_LOG_DEBUG("Apply is already active");
    return OkStatus();
  }

  if ((state != BundledUpdateState::Enum::kTransferred) &&
      (state != BundledUpdateState::Enum::kVerified)) {
    SET_ERROR(BundledUpdateResult::Enum::kApplyFailed,
              "Apply() must be called from TRANSFERRED or VERIFIED state. "
              "State: %d",
              static_cast<int>(state));
    return Status::FailedPrecondition();
  }

  // TODO(elipsitz): We should probably make these all idempotent properly.
  if (work_enqueued_) {
    PW_LOG_DEBUG("Apply is already active");
    return OkStatus();
  }

  // The backend's ApplyReboot as part of DoApply() shall be configured
  // such that this RPC can send out the reply before the device reboots.
  const Status status = work_queue_.PushWork([this] {
    {
      std::lock_guard y_lock(this->mutex_);
      PW_DCHECK(this->work_enqueued_);
    }
    // Error reporting is handled in DoVerify and DoApply.
    this->DoVerify();
    this->DoApply();
    {
      std::lock_guard y_lock(this->mutex_);
      this->work_enqueued_ = false;
    }
  });
  if (!status.ok()) {
    SET_ERROR(BundledUpdateResult::Enum::kApplyFailed,
              "Unable to equeue apply to work queue");
    response = *status_.acquire();
    return status;
  }
  work_enqueued_ = true;

  return OkStatus();
}

void BundledUpdateService::DoApply() {
  std::lock_guard guard(mutex_);
  const BundledUpdateState::Enum state = status_.acquire()->state;

  PW_LOG_DEBUG("Attempting to apply the update");
  if (state != BundledUpdateState::Enum::kVerified) {
    SET_ERROR(BundledUpdateResult::Enum::kApplyFailed,
              "Apply() must be called from VERIFIED state. State: %d",
              static_cast<int>(state));
    return;
  }

  status_.acquire()->state = BundledUpdateState::Enum::kApplying;

  if (const Status status = backend_.BeforeApply(); !status.ok()) {
    SET_ERROR(BundledUpdateResult::Enum::kApplyFailed,
              "BeforeApply() returned unsuccessful result: %d",
              static_cast<int>(status.code()));
    return;
  }

  // In order to report apply progress, quickly scan to see how many bytes
  // will be applied.
  Result<uint64_t> total_payload_bytes = bundle_.GetTotalPayloadSize();
  PW_CHECK_OK(total_payload_bytes.status());
  size_t target_file_bytes_to_apply =
      static_cast<size_t>(total_payload_bytes.value());

  protobuf::RepeatedMessages target_files =
      bundle_.GetManifest().GetTargetFiles();
  PW_CHECK_OK(target_files.status());

  size_t target_file_bytes_applied = 0;
  for (pw::protobuf::Message file_name : target_files) {
    std::array<std::byte, MAX_TARGET_NAME_LENGTH> buf = {};
    protobuf::String name = file_name.AsString(static_cast<uint32_t>(
        pw::software_update::TargetFile::Fields::FILE_NAME));
    PW_CHECK_OK(name.status());
    const Result<ByteSpan> read_result = name.GetBytesReader().Read(buf);
    PW_CHECK_OK(read_result.status());
    const ConstByteSpan file_name_span = read_result.value();
    const std::string_view file_name_view(
        reinterpret_cast<const char*>(file_name_span.data()),
        file_name_span.size_bytes());
    if (file_name_view.compare(kUserManifestTargetFileName) == 0) {
      continue;  // user_manifest is not applied by the backend.
    }
    // Try to get an IntervalReader for the current file.
    stream::IntervalReader file_reader =
        bundle_.GetTargetPayload(file_name_view);
    if (file_reader.status().IsNotFound()) {
      PW_LOG_INFO(
          "Contents of file %s missing from bundle; ignoring",
          pw::MakeString<MAX_TARGET_NAME_LENGTH>(file_name_view).c_str());
      continue;
    }
    if (!file_reader.ok()) {
      SET_ERROR(BundledUpdateResult::Enum::kApplyFailed,
                "Could not open contents of file %s from bundle; "
                "aborting update apply phase",
                MakeString<MAX_TARGET_NAME_LENGTH>(file_name_view).c_str());
      return;
    }

    const size_t bundle_offset = file_reader.start();
    if (const Status status = backend_.ApplyTargetFile(
            file_name_view, file_reader, bundle_offset);
        !status.ok()) {
      SET_ERROR(BundledUpdateResult::Enum::kApplyFailed,
                "Failed to apply target file: %d",
                static_cast<int>(status.code()));
      return;
    }
    target_file_bytes_applied += file_reader.interval_size();
    const uint32_t progress_hundreth_percent =
        (static_cast<uint64_t>(target_file_bytes_applied) * 100 * 100) /
        target_file_bytes_to_apply;
    PW_LOG_DEBUG("Apply progress: %zu/%zu Bytes (%ld%%)",
                 target_file_bytes_applied,
                 target_file_bytes_to_apply,
                 static_cast<unsigned long>(progress_hundreth_percent / 100));
    {
      BorrowedStatus borrowed_status = status_.acquire();
      borrowed_status->current_state_progress_hundreth_percent =
          progress_hundreth_percent;
    }
  }

  // TODO(davidrogers): Add new APPLY_REBOOTING to distinguish between pre and
  // post reboot.

  // Finalize the apply.
  if (const Status status = backend_.ApplyReboot(); !status.ok()) {
    SET_ERROR(BundledUpdateResult::Enum::kApplyFailed,
              "Failed to do the apply reboot: %d",
              static_cast<int>(status.code()));
    return;
  }

  // TODO(davidrogers): Move this to MaybeFinishApply() once available.
  Finish(BundledUpdateResult::Enum::kSuccess);
}

Status BundledUpdateService::Abort(const pw::protobuf::Empty::Message&,
                                   BundledUpdateStatus::Message& response) {
  std::lock_guard lock(mutex_);
  const BundledUpdateState::Enum state = status_.acquire()->state;

  if (state == BundledUpdateState::Enum::kApplying) {
    return Status::FailedPrecondition();
  }

  if (state == BundledUpdateState::Enum::kInactive ||
      state == BundledUpdateState::Enum::kFinished) {
    SET_ERROR(BundledUpdateResult::Enum::kUnknownError,
              "Tried to abort when already INACTIVE or FINISHED");
    return Status::FailedPrecondition();
  }
  // TODO(elipsitz): Switch abort to async; this state change isn't externally
  // visible.
  status_.acquire()->state = BundledUpdateState::Enum::kAborting;

  SET_ERROR(BundledUpdateResult::Enum::kAborted, "Update abort requested");
  response = *status_.acquire();
  return OkStatus();
}

Status BundledUpdateService::Reset(const pw::protobuf::Empty::Message&,
                                   BundledUpdateStatus::Message& response) {
  std::lock_guard lock(mutex_);
  const BundledUpdateState::Enum state = status_.acquire()->state;

  if (state == BundledUpdateState::Enum::kInactive) {
    return OkStatus();  // Already done.
  }

  if (state != BundledUpdateState::Enum::kFinished) {
    SET_ERROR(
        BundledUpdateResult::Enum::kUnknownError,
        "Reset() must be called from FINISHED or INACTIVE state. State: %d",
        static_cast<int>(state));
    response = *status_.acquire();
    return Status::FailedPrecondition();
  }

  {
    BorrowedStatus status = status_.acquire();
    *status = {};  // Force-init all fields to zero.
    status->state = BundledUpdateState::Enum::kInactive;
  }

  // Reset the bundle.
  if (bundle_open_) {
    // TODO(elipsitz): Revisit whether this is recoverable; maybe eliminate
    // CHECK.
    PW_CHECK_OK(bundle_.Close());
    bundle_open_ = false;
  }

  response = *status_.acquire();
  return OkStatus();
}

void BundledUpdateService::NotifyTransferSucceeded() {
  std::lock_guard lock(mutex_);
  const BundledUpdateState::Enum state = status_.acquire()->state;

  if (state != BundledUpdateState::Enum::kTransferring) {
    // This can happen if the update gets Abort()'d during the transfer and
    // the transfer completes successfully.
    PW_LOG_WARN(
        "Got transfer succeeded notification when not in TRANSFERRING state. "
        "State: %d",
        static_cast<int>(state));
  }

  const bool transfer_ongoing = status_.acquire()->transfer_id.has_value();
  if (transfer_ongoing) {
    backend_.DisableBundleTransferHandler();
    status_.acquire()->transfer_id.reset();
  } else {
    PW_LOG_WARN("No ongoing transfer found, forcefully set TRANSFERRED.");
  }

  status_.acquire()->state = BundledUpdateState::Enum::kTransferred;
}

void BundledUpdateService::Finish(BundledUpdateResult::Enum result) {
  if (result == BundledUpdateResult::Enum::kSuccess) {
    BorrowedStatus borrowed_status = status_.acquire();
    borrowed_status->current_state_progress_hundreth_percent.reset();
  } else {
    // In the case of error, notify backend that we're about to abort the
    // software update.
    PW_CHECK_OK(backend_.BeforeUpdateAbort());
  }

  // Turn down the transfer if one is in progress.
  const bool transfer_ongoing = status_.acquire()->transfer_id.has_value();
  if (transfer_ongoing) {
    backend_.DisableBundleTransferHandler();
  }
  status_.acquire()->transfer_id.reset();

  // Close out any open bundles.
  if (bundle_open_) {
    // TODO(elipsitz): Revisit this check; may be able to recover.
    PW_CHECK_OK(bundle_.Close());
    bundle_open_ = false;
  }
  {
    BorrowedStatus borrowed_status = status_.acquire();
    borrowed_status->state = BundledUpdateState::Enum::kFinished;
    borrowed_status->result = result;
  }
}

}  // namespace pw::software_update
