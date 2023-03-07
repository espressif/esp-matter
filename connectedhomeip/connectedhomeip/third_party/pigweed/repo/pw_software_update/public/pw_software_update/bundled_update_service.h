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

#include "pw_software_update/bundled_update.rpc.pb.h"
#include "pw_software_update/bundled_update_backend.h"
#include "pw_software_update/update_bundle_accessor.h"
#include "pw_status/status.h"
#include "pw_sync/borrow.h"
#include "pw_sync/lock_annotations.h"
#include "pw_sync/mutex.h"
#include "pw_work_queue/work_queue.h"

namespace pw::software_update {

// Implementation class for pw.software_update.BundledUpdate.
// See bundled_update.proto for RPC method documentation.
class BundledUpdateService
    : public pw_rpc::nanopb::BundledUpdate::Service<BundledUpdateService> {
 public:
  BundledUpdateService(UpdateBundleAccessor& bundle,
                       BundledUpdateBackend& backend,
                       work_queue::WorkQueue& work_queue)
      : unsafe_status_{.state =
                           pw_software_update_BundledUpdateState_Enum_INACTIVE},
        status_(unsafe_status_, status_mutex_),
        backend_(backend),
        bundle_(bundle),
        bundle_open_(false),
        work_queue_(work_queue),
        work_enqueued_(false) {}

  Status GetStatus(const pw_protobuf_Empty& request,
                   pw_software_update_BundledUpdateStatus& response);

  // Sync
  Status Start(const pw_software_update_StartRequest& request,
               pw_software_update_BundledUpdateStatus& response);

  // Sync
  Status SetTransferred(const pw_protobuf_Empty& request,
                        pw_software_update_BundledUpdateStatus& response);

  // Async
  Status Verify(const pw_protobuf_Empty& request,
                pw_software_update_BundledUpdateStatus& response);

  // Async
  Status Apply(const pw_protobuf_Empty& request,
               pw_software_update_BundledUpdateStatus& response);

  // Currently sync, should be async.
  // TODO(keir): Make this async to support aborting verify/apply.
  Status Abort(const pw_protobuf_Empty& request,
               pw_software_update_BundledUpdateStatus& response);

  // Sync
  Status Reset(const pw_protobuf_Empty& request,
               pw_software_update_BundledUpdateStatus& response);

  // Notify the service that the bundle transfer has completed. The service has
  // no way to know when the bundle transfer completes, so users must invoke
  // this method in their transfer completion handler.
  //
  // After this call, the service will be in TRANSFERRED state if and only if
  // it was in the TRANSFERRING state.
  void NotifyTransferSucceeded();

  // TODO(davidrogers) Add a MaybeFinishApply() method that is called after
  // reboot to finish any need apply and verify work.

  // TODO(keir): VerifyProgress - to update % complete.
  // TODO(keir): ApplyProgress - to update % complete.

 private:
  // Top-level lock for OTA state coherency. May be held for extended periods.
  sync::Mutex mutex_;
  // Nested lock for safe status updates and queries.
  sync::Mutex status_mutex_ PW_ACQUIRED_AFTER(mutex_);
  pw_software_update_BundledUpdateStatus unsafe_status_
      PW_GUARDED_BY(status_mutex_);
  sync::Borrowable<pw_software_update_BundledUpdateStatus, sync::Mutex> status_;
  BundledUpdateBackend& backend_ PW_GUARDED_BY(mutex_);
  UpdateBundleAccessor& bundle_ PW_GUARDED_BY(mutex_);
  bool bundle_open_ PW_GUARDED_BY(mutex_);
  work_queue::WorkQueue& work_queue_ PW_GUARDED_BY(mutex_);
  bool work_enqueued_ PW_GUARDED_BY(mutex_);

  void DoVerify() PW_LOCKS_EXCLUDED(status_mutex_);
  void DoApply() PW_LOCKS_EXCLUDED(status_mutex_);
  void Finish(_pw_software_update_BundledUpdateResult_Enum result)
      PW_EXCLUSIVE_LOCKS_REQUIRED(mutex_) PW_LOCKS_EXCLUDED(status_mutex_);
  bool IsFinished() PW_EXCLUSIVE_LOCKS_REQUIRED(mutex_)
      PW_LOCKS_EXCLUDED(status_mutex_) {
    return status_.acquire()->state ==
           pw_software_update_BundledUpdateState_Enum_FINISHED;
  }
};

}  // namespace pw::software_update
