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

#include <string_view>

#include "pw_result/result.h"
#include "pw_software_update/manifest_accessor.h"
#include "pw_software_update/update_bundle_accessor.h"
#include "pw_status/status.h"
#include "pw_stream/interval_reader.h"
#include "pw_stream/stream.h"

namespace pw::software_update {

// TODO(b/235273688): update documentation for backend api contract
class BundledUpdateBackend {
 public:
  virtual ~BundledUpdateBackend() = default;

  // Perform optional, product-specific validations to the specified target
  // file, using whatever metadata available in manifest.
  //
  // This is called for each target file after the standard verification has
  // passed.
  virtual Status VerifyTargetFile(
      [[maybe_unused]] ManifestAccessor manifest,
      [[maybe_unused]] std::string_view target_file_name) {
    // TODO(backend): Perform any additional product-specific validations.
    // It is safe to assume the target's payload has passed standard
    // verification.
    return OkStatus();
  }

  // Perform any product-specific tasks needed before starting update sequence.
  virtual Status BeforeUpdateStart() { return OkStatus(); }

  // Attempts to enable the transfer service transfer handler, returning the
  // transfer_id if successful. This is invoked after BeforeUpdateStart();
  virtual Result<uint32_t> EnableBundleTransferHandler(
      std::string_view bundle_filename) = 0;

  // Disables the transfer service transfer handler. This is invoked after
  // either BeforeUpdateAbort() or BeforeBundleVerify().
  virtual void DisableBundleTransferHandler() = 0;

  // Perform any product-specific abort tasks before marking the update as
  // aborted in bundled updater.  This should set any downstream state to a
  // default no-update-pending state.
  // TODO(keir): Revisit invariants; should this instead be "Abort()"? This is
  // called for all error paths in the service and needs to reset. Furthermore,
  // should this be async?
  virtual Status BeforeUpdateAbort() { return OkStatus(); }

  // Perform any product-specific tasks needed before starting verification.
  virtual Status BeforeBundleVerify() { return OkStatus(); }

  // Perform any product-specific bundle verification tasks (e.g. hw version
  // match check), done after TUF bundle verification process.
  virtual Status VerifyManifest(
      [[maybe_unused]] ManifestAccessor manifest_accessor) {
    return OkStatus();
  }

  // Perform product-specific tasks after all bundle verifications are complete.
  virtual Status AfterBundleVerified() { return OkStatus(); }

  // Perform any product-specific tasks before apply sequence started
  virtual Status BeforeApply() { return OkStatus(); }

  // Get status information from update backend. This will not be called when
  // BundledUpdater is in a step where it has entire control with no operation
  // handed over to update backend.
  virtual int64_t GetStatus() { return 0; }

  // Update the specific target file on the device.
  virtual Status ApplyTargetFile(std::string_view target_file_name,
                                 stream::Reader& target_payload,
                                 size_t update_bundle_offset) = 0;

  // Backend to probe the device manifest and prepare a ready-to-go reader
  // for it. See the comments to `GetCurrentManfestReader()` for more context.
  virtual Status BeforeManifestRead() {
    // Todo(backend):
    // 1. Probe device to see if a well-formed manifest already exists.
    // 2. If not, return `Status::NotFound()`. Note this will cause
    //    anti-rollback to skip. So please don't always return
    //    `Status::NotFound()`!
    // 3. If yes, instantiate and activate a reader for the manifest!
    // 4. Return any unexpected condition as errors but note this will cause
    //    the current software update session to abort.
    return OkStatus();
  }

  // Backend to provide a ready-to-go reader for the on-device manifest blob.
  // This function is called after a successful `BeforeManifestRead()`,
  // potentially more than once.
  //
  // This manifest blob is a serialized `message Manifest{...}` as defined in
  // update_bundle.proto.
  //
  // This manifest blob is ALWAYS and EXCLUSIVELY persisted by a successful
  // software update. Thus it may not available before the first software
  // update, in which case `BeforeManifestRead()` should've returned
  // `Status::NotFound()`.
  //
  // This manifest contains trusted metadata of all software currently running
  // on the device and used for anti-rollback checks. It MUST NOT be tampered
  // by factory resets, flashing, or any other means other than software
  // updates.
  virtual Result<stream::SeekableReader*> GetCurrentManifestReader() {
    // Todo(backend):
    // 1. Double check if a ready-to-go reader has been prepared by
    //    `BeforeManifestRead()`.
    // 2. If yes (expected), return the reader.
    // 3. If not (unexpected), return `Status::FailedPrecondition()`.
    return Status::Unimplemented();
  }

  // TODO(alizhang): Deprecate GetCurrentManifestReader in favor of
  // `GetManifestReader()`.
  virtual Result<stream::SeekableReader*> GetManifestReader() {
    return GetCurrentManifestReader();
  }

  // Backend to prepare for on-device manifest update, e.g. make necessary
  // efforts to ready the manifest writer. The manifest writer is used to
  // persist a new manifest on-device following a successful software update.
  // Manifest writing is never mixed with reading (i.e. reader and writer are
  // used sequentially).
  virtual Status BeforeManifestWrite() {
    // Todo(backend):
    // 1. Instantiate and activate a manifest writer pointing at a persistent
    //    storage that at least could survive a factory data reset (FDR), if not
    //    tamper-resistant.
    return OkStatus();
  }

  // Backend to provide a ready-to-go writer for the on-device manifest blob.
  // This function is called after a successful `BeforeManifestWrite()`,
  // potentially more than once.
  //
  // This manifest blob is a serialized `message Manifest{...}` as defined in
  // update_bundle.proto.
  //
  // This manifest blob is ALWAYS and EXCLUSIVELY persisted by a successful
  // software update.
  //
  // This manifest contains trusted metadata of all software currently running
  // on the device and used for anti-rollback checks. It MUST NOT be tampered
  // by factory resets, flashing, or any other means other than software
  // updates.
  virtual Result<stream::Writer*> GetManifestWriter() {
    // Todo(backend):
    // 1. Double check a writer is ready to go as result of
    //    `BeforeManifestWrite()`.
    // 2. If yes (expected), simply return the writer.
    // 3. If not (unexpected), return `Status::FailedPrecondition()`.
    return Status::Unimplemented();
  }

  // Backend to finish up manifest writing.
  virtual Status AfterManifestWrite() {
    // Todo(backend):
    // Protect the newly persisted manifest blob. This is to make manifest
    // probing / reading easier and more reliable. This could involve taking
    // a measurement (e.g. checksum) and storing that measurement in a
    // FDR-safe tag, replicating the manifest in a backup location if the
    // backing media is unreliable (e.g. raw NAND) etc.
    //
    // It is safe to assume the writing has been successful in this function.
    return OkStatus();
  }

  // Do any work needed to finish the apply of the update and do a required
  // reboot of the device!
  //
  // NOTE: If successful this method does not return and reboots the device, it
  // only returns on failure to finalize.
  //
  // NOTE: ApplyReboot shall be configured such to allow pending RPC or logs to
  // send out the reply before the device reboots.
  virtual Status ApplyReboot() = 0;

  // Do any work needed to finalize the update including optionally doing a
  // reboot of the device! The software update state and breadcrumbs are not
  // cleaned up until this method returns OK.
  //
  // This method is called after the reboot done as part of ApplyReboot().
  //
  // If this method does an optional reboot, it will be called again after the
  // reboot.
  //
  // NOTE: PostRebootFinalize shall be configured such to allow pending RPC or
  // logs to send out the reply before the device reboots.
  virtual Status PostRebootFinalize() { return OkStatus(); }

  // Get reader of the device's root metadata.
  //
  // This method MUST return a valid root metadata once verified OTA is enabled.
  // An invalid or corrupted root metadata will result in permanent OTA
  // failures.
  virtual Result<stream::SeekableReader*> GetRootMetadataReader() {
    return Status::Unimplemented();
  }

  // Write a given root metadata to persistent storage in a failsafe manner.
  //
  // The updating must be atomic/fail-safe. An invalid or corrupted root
  // metadata will result in permanent OTA failures.
  virtual Status SafelyPersistRootMetadata(
      [[maybe_unused]] stream::IntervalReader root_metadata) {
    return Status::Unimplemented();
  }
};

}  // namespace pw::software_update
