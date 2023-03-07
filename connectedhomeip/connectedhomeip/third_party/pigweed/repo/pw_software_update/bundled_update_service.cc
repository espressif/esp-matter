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

#include "pw_software_update/bundled_update_service.h"

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
    sync::BorrowedPointer<pw_software_update_BundledUpdateStatus, sync::Mutex>;
using BundledUpdateState = pw_software_update_BundledUpdateState_Enum;
using BundledUpdateStatus = pw_software_update_BundledUpdateStatus;

// TODO(keir): Convert all the CHECKs in the RPC service to gracefully report
// errors.
#define SET_ERROR(res, message, ...)                                          \
  do {                                                                        \
    PW_LOG_ERROR(message, __VA_ARGS__);                                       \
    if (!IsFinished()) {                                                      \
      Finish(res);                                                            \
      {                                                                       \
        BorrowedStatus borrowed_status = status_.acquire();                   \
        size_t note_size = sizeof(borrowed_status->note.bytes);               \
        PW_TOKENIZE_TO_BUFFER(                                                \
            borrowed_status->note.bytes, &(note_size), message, __VA_ARGS__); \
        borrowed_status->note.size = note_size;                               \
        borrowed_status->has_note = true;                                     \
      }                                                                       \
    }                                                                         \
  } while (false)
}  // namespace

Status BundledUpdateService::GetStatus(const pw_protobuf_Empty&,
                                       BundledUpdateStatus& response) {
  response = *status_.acquire();
  return OkStatus();
}

Status BundledUpdateService::Start(
    const pw_software_update_StartRequest& request,
    BundledUpdateStatus& response) {
  std::lock_guard lock(mutex_);
  // Check preconditions.
  const BundledUpdateState state = status_.acquire()->state;
  if (state != pw_software_update_BundledUpdateState_Enum_INACTIVE) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_UNKNOWN_ERROR,
              "Start() can only be called from INACTIVE state. "
              "Current state: %d. Abort() then Reset() must be called first",
              static_cast<int>(state));
    response = *status_.acquire();
    return Status::FailedPrecondition();
  }

  {
    BorrowedStatus borrowed_status = status_.acquire();
    PW_DCHECK(!borrowed_status->has_transfer_id);
    PW_DCHECK(!borrowed_status->has_result);
    PW_DCHECK(borrowed_status->current_state_progress_hundreth_percent == 0);
    PW_DCHECK(borrowed_status->bundle_filename[0] == '\0');
    PW_DCHECK(borrowed_status->note.size == 0);
  }

  // Notify the backend of pending transfer.
  if (const Status status = backend_.BeforeUpdateStart(); !status.ok()) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_UNKNOWN_ERROR,
              "Backend error on BeforeUpdateStart()");
    response = *status_.acquire();
    return status;
  }

  // Enable bundle transfer.
  Result<uint32_t> possible_transfer_id =
      backend_.EnableBundleTransferHandler(string::ClampedCString(
          request.bundle_filename, sizeof(request.bundle_filename)));
  if (!possible_transfer_id.ok()) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_TRANSFER_FAILED,
              "Couldn't enable bundle transfer");
    response = *status_.acquire();
    return possible_transfer_id.status();
  }

  // Update state.
  {
    BorrowedStatus borrowed_status = status_.acquire();
    borrowed_status->transfer_id = possible_transfer_id.value();
    borrowed_status->has_transfer_id = true;
    if (request.has_bundle_filename) {
      const StatusWithSize sws =
          string::Copy(request.bundle_filename,
                       borrowed_status->bundle_filename,
                       sizeof(borrowed_status->bundle_filename));
      PW_DCHECK_OK(sws.status(),
                   "bundle_filename options max_sizes do not match");
      borrowed_status->has_bundle_filename = true;
    }
    borrowed_status->state =
        pw_software_update_BundledUpdateState_Enum_TRANSFERRING;
    response = *borrowed_status;
  }
  return OkStatus();
}

Status BundledUpdateService::SetTransferred(const pw_protobuf_Empty&,
                                            BundledUpdateStatus& response) {
  const BundledUpdateState state = status_.acquire()->state;

  if (state != pw_software_update_BundledUpdateState_Enum_TRANSFERRING &&
      state != pw_software_update_BundledUpdateState_Enum_INACTIVE) {
    std::lock_guard lock(mutex_);
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_UNKNOWN_ERROR,
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

// TODO(keir): Check for "ABORTING" state and bail if it's set.
void BundledUpdateService::DoVerify() {
  std::lock_guard guard(mutex_);
  const BundledUpdateState state = status_.acquire()->state;

  if (state == pw_software_update_BundledUpdateState_Enum_VERIFIED) {
    return;  // Already done!
  }

  // Ensure we're in the right state.
  if (state != pw_software_update_BundledUpdateState_Enum_TRANSFERRED) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_VERIFY_FAILED,
              "DoVerify() must be called from TRANSFERRED state. State: %d",
              static_cast<int>(state));
    return;
  }

  status_.acquire()->state =
      pw_software_update_BundledUpdateState_Enum_VERIFYING;

  // Notify backend about pending verify.
  if (const Status status = backend_.BeforeBundleVerify(); !status.ok()) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_VERIFY_FAILED,
              "Backend::BeforeBundleVerify() failed");
    return;
  }

  // Do the actual verify.
  Status status = bundle_.OpenAndVerify();
  if (!status.ok()) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_VERIFY_FAILED,
              "Bundle::OpenAndVerify() failed");
    return;
  }
  bundle_open_ = true;

  // Have the backend verify the user_manifest if present.
  if (!backend_.VerifyManifest(bundle_.GetManifest()).ok()) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_VERIFY_FAILED,
              "Backend::VerifyUserManifest() failed");
    return;
  }

  // Notify backend we're done verifying.
  status = backend_.AfterBundleVerified();
  if (!status.ok()) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_VERIFY_FAILED,
              "Backend::AfterBundleVerified() failed");
    return;
  }
  status_.acquire()->state =
      pw_software_update_BundledUpdateState_Enum_VERIFIED;
}

Status BundledUpdateService::Verify(const pw_protobuf_Empty&,
                                    BundledUpdateStatus& response) {
  std::lock_guard lock(mutex_);
  const BundledUpdateState state = status_.acquire()->state;

  // Already done? Bail.
  if (state == pw_software_update_BundledUpdateState_Enum_VERIFIED) {
    PW_LOG_DEBUG("Skipping verify since already verified");
    return OkStatus();
  }

  // TODO(keir): Remove the transferring permitted state here ASAP.
  // Ensure we're in the right state.
  if ((state != pw_software_update_BundledUpdateState_Enum_TRANSFERRING) &&
      (state != pw_software_update_BundledUpdateState_Enum_TRANSFERRED)) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_VERIFY_FAILED,
              "Verify() must be called from TRANSFERRED state. State: %d",
              static_cast<int>(state));
    response = *status_.acquire();
    return Status::FailedPrecondition();
  }

  // TODO(keir): We should probably make this mode idempotent.
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
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_VERIFY_FAILED,
              "Unable to equeue apply to work queue");
    response = *status_.acquire();
    return status;
  }
  work_enqueued_ = true;

  response = *status_.acquire();
  return OkStatus();
}

Status BundledUpdateService::Apply(const pw_protobuf_Empty&,
                                   BundledUpdateStatus& response) {
  std::lock_guard lock(mutex_);
  const BundledUpdateState state = status_.acquire()->state;

  // We do not wait to go into a finished error state if we're already
  // applying, instead just let them know that yes we are working on it --
  // hold on.
  if (state == pw_software_update_BundledUpdateState_Enum_APPLYING) {
    PW_LOG_DEBUG("Apply is already active");
    return OkStatus();
  }

  if ((state != pw_software_update_BundledUpdateState_Enum_TRANSFERRED) &&
      (state != pw_software_update_BundledUpdateState_Enum_VERIFIED)) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_APPLY_FAILED,
              "Apply() must be called from TRANSFERRED or VERIFIED state. "
              "State: %d",
              static_cast<int>(state));
    return Status::FailedPrecondition();
  }

  // TODO(keir): We should probably make these all idempotent properly.
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
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_APPLY_FAILED,
              "Unable to equeue apply to work queue");
    response = *status_.acquire();
    return status;
  }
  work_enqueued_ = true;

  return OkStatus();
}

void BundledUpdateService::DoApply() {
  std::lock_guard guard(mutex_);
  const BundledUpdateState state = status_.acquire()->state;

  PW_LOG_DEBUG("Attempting to apply the update");
  if (state != pw_software_update_BundledUpdateState_Enum_VERIFIED) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_APPLY_FAILED,
              "Apply() must be called from VERIFIED state. State: %d",
              static_cast<int>(state));
    return;
  }

  status_.acquire()->state =
      pw_software_update_BundledUpdateState_Enum_APPLYING;

  if (const Status status = backend_.BeforeApply(); !status.ok()) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_APPLY_FAILED,
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
      SET_ERROR(pw_software_update_BundledUpdateResult_Enum_APPLY_FAILED,
                "Could not open contents of file %s from bundle; "
                "aborting update apply phase",
                MakeString<MAX_TARGET_NAME_LENGTH>(file_name_view).c_str());
      return;
    }

    const size_t bundle_offset = file_reader.start();
    if (const Status status = backend_.ApplyTargetFile(
            file_name_view, file_reader, bundle_offset);
        !status.ok()) {
      SET_ERROR(pw_software_update_BundledUpdateResult_Enum_APPLY_FAILED,
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
      borrowed_status->has_current_state_progress_hundreth_percent = true;
    }
  }

  // TODO(davidrogers): Add new APPLY_REBOOTING to distinguish between pre and
  // post reboot.

  // Finalize the apply.
  if (const Status status = backend_.ApplyReboot(); !status.ok()) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_APPLY_FAILED,
              "Failed to do the apply reboot: %d",
              static_cast<int>(status.code()));
    return;
  }

  // TODO(davidrogers): Move this to MaybeFinishApply() once available.
  Finish(pw_software_update_BundledUpdateResult_Enum_SUCCESS);
}

Status BundledUpdateService::Abort(const pw_protobuf_Empty&,
                                   BundledUpdateStatus& response) {
  std::lock_guard lock(mutex_);
  const BundledUpdateState state = status_.acquire()->state;

  if (state == pw_software_update_BundledUpdateState_Enum_APPLYING) {
    return Status::FailedPrecondition();
  }

  if (state == pw_software_update_BundledUpdateState_Enum_INACTIVE ||
      state == pw_software_update_BundledUpdateState_Enum_FINISHED) {
    SET_ERROR(pw_software_update_BundledUpdateResult_Enum_UNKNOWN_ERROR,
              "Tried to abort when already INACTIVE or FINISHED");
    return Status::FailedPrecondition();
  }
  // TODO(keir): Switch abort to async; this state change isn't externally
  // visible.
  status_.acquire()->state =
      pw_software_update_BundledUpdateState_Enum_ABORTING;

  SET_ERROR(pw_software_update_BundledUpdateResult_Enum_ABORTED,
            "Update abort requested");
  response = *status_.acquire();
  return OkStatus();
}

Status BundledUpdateService::Reset(const pw_protobuf_Empty&,
                                   BundledUpdateStatus& response) {
  std::lock_guard lock(mutex_);
  const BundledUpdateState state = status_.acquire()->state;

  if (state == pw_software_update_BundledUpdateState_Enum_INACTIVE) {
    return OkStatus();  // Already done.
  }

  if (state != pw_software_update_BundledUpdateState_Enum_FINISHED) {
    SET_ERROR(
        pw_software_update_BundledUpdateResult_Enum_UNKNOWN_ERROR,
        "Reset() must be called from FINISHED or INACTIVE state. State: %d",
        static_cast<int>(state));
    response = *status_.acquire();
    return Status::FailedPrecondition();
  }

  {
    *status_.acquire() = {
        .state = pw_software_update_BundledUpdateState_Enum_INACTIVE};
  }

  // Reset the bundle.
  if (bundle_open_) {
    // TODO(keir): Revisit whether this is recoverable; maybe eliminate CHECK.
    PW_CHECK_OK(bundle_.Close());
    bundle_open_ = false;
  }

  response = *status_.acquire();
  return OkStatus();
}

void BundledUpdateService::NotifyTransferSucceeded() {
  std::lock_guard lock(mutex_);
  const BundledUpdateState state = status_.acquire()->state;

  if (state != pw_software_update_BundledUpdateState_Enum_TRANSFERRING) {
    // This can happen if the update gets Abort()'d during the transfer and
    // the transfer completes successfully.
    PW_LOG_WARN(
        "Got transfer succeeded notification when not in TRANSFERRING state. "
        "State: %d",
        static_cast<int>(state));
  }

  const bool transfer_ongoing = status_.acquire()->has_transfer_id;
  if (transfer_ongoing) {
    backend_.DisableBundleTransferHandler();
    status_.acquire()->has_transfer_id = false;
  } else {
    PW_LOG_WARN("No ongoing transfer found, forcefully set TRANSFERRED.");
  }

  status_.acquire()->state =
      pw_software_update_BundledUpdateState_Enum_TRANSFERRED;
}

void BundledUpdateService::Finish(
    pw_software_update_BundledUpdateResult_Enum result) {
  if (result == pw_software_update_BundledUpdateResult_Enum_SUCCESS) {
    BorrowedStatus borrowed_status = status_.acquire();
    borrowed_status->current_state_progress_hundreth_percent = 0;
    borrowed_status->has_current_state_progress_hundreth_percent = false;
  } else {
    // In the case of error, notify backend that we're about to abort the
    // software update.
    PW_CHECK_OK(backend_.BeforeUpdateAbort());
  }

  // Turn down the transfer if one is in progress.
  const bool transfer_ongoing = status_.acquire()->has_transfer_id;
  if (transfer_ongoing) {
    backend_.DisableBundleTransferHandler();
  }
  status_.acquire()->has_transfer_id = false;

  // Close out any open bundles.
  if (bundle_open_) {
    // TODO(keir): Revisit this check; may be able to recover.
    PW_CHECK_OK(bundle_.Close());
    bundle_open_ = false;
  }
  {
    BorrowedStatus borrowed_status = status_.acquire();
    borrowed_status->state =
        pw_software_update_BundledUpdateState_Enum_FINISHED;
    borrowed_status->result = result;
    borrowed_status->has_result = true;
  }
}

}  // namespace pw::software_update
