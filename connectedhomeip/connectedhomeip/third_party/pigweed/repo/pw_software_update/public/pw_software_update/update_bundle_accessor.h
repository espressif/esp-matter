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
#include "pw_protobuf/map_utils.h"
#include "pw_protobuf/message.h"
#include "pw_software_update/blob_store_openable_reader.h"
#include "pw_software_update/bundled_update_backend.h"
#include "pw_software_update/manifest_accessor.h"
#include "pw_software_update/openable_reader.h"

namespace pw::software_update {

class BundledUpdateBackend;

// Name of the top-level Targets metadata.
constexpr std::string_view kTopLevelTargetsName = "targets";

// Name of the "user manifest" target file. The "user manifest" is a product
// specific blob that is opaque to upstream but need to be passed around in
// manifest handling (for now).
constexpr std::string_view kUserManifestTargetFileName = "user_manifest";

// UpdateBundleAccessor is the trusted custodian of a staged incoming update
// bundle.
//
// It takes exclusive ownership of the blob_store that represents a staged,
// *untrusted* bundle, and presents convenient and *trusted* accessors.
//
// ALL ACCESS to the staged update bundle MUST GO THROUGH the
// `UpdateBundleAccessor`.
class UpdateBundleAccessor {
 public:
  // UpdateBundleAccessor
  // update_reader - The staged incoming software update bundle.
  // backend - Project-specific BundledUpdateBackend.
  // self_verification - When set to true, perform a voluntary best effort
  //     verification against available metadata in the incoming bundle itself.
  //     Self verification does NOT use any on-device metadata, thus does not
  //     guard against malicious attacks. Self-verification is primarily meant
  //     to de-risk 0-day verification turn-on.
  constexpr UpdateBundleAccessor(OpenableReader& update_reader,
                                 BundledUpdateBackend& backend,
                                 bool self_verification = false)
      : optional_blob_store_reader_unused_(),
        update_reader_(update_reader),
        backend_(backend),
        self_verification_(self_verification) {}

  // Overloaded constructor to maintain backwards compatibility. This should be
  // removed once users have migrated.
  constexpr UpdateBundleAccessor(blob_store::BlobStore& blob_store,
                                 BundledUpdateBackend& backend,
                                 bool self_verification = false)
      : optional_blob_store_openeable_reader_(blob_store),
        update_reader_(optional_blob_store_openeable_reader_),
        backend_(backend),
        self_verification_(self_verification) {}

  ~UpdateBundleAccessor() {
    if (&update_reader_ == &optional_blob_store_openeable_reader_) {
      optional_blob_store_openeable_reader_.~BlobStoreOpenableReader();
    }
  }

  // Opens and verifies the software update bundle.
  //
  // Verification covers the following:
  //
  // 1. If a Root metadata is included with the incoming bundle, the Root
  //    metadata will be verified and used as the new Root metadata to verify
  //    other metadata in the bundle.
  //
  // 2. The Targets metadata is verified using the Root metadata.
  //
  // 3. All target payloads referenced in the Targets metadata are verified.
  //
  // Limitations and customizations (compared to standard TUF):
  //
  // 1. Does not yet support arbitrary Root key rotations. Which means
  //    There is only one (reliable) chance to rotate the Root key for all
  //    devices. Rotation of the Targets key is still unlimited.
  // 2. Timestamp and Snapshot metadata are not used or supported.
  // 3. Assumes a single top-level Targets metadata and no delegations.
  // 4. The top-level Targets metadata doubles as the software update
  //    "manifest". Anti-rollback IS supported via the Targets metadata version.
  // 5. Supports "personalization", where the staged bundle may have been
  //    stripped of any target payloads that the device already have. For those
  //    personalized-out targets, verification relies on the cached manifest of
  //    a previous successful update to verify target length and hash.
  //
  // Returns:
  // OK - Bundle was successfully opened and verified.
  Status OpenAndVerify();

  // Closes the bundle by invalidating the verification and closing
  // the reader to release the read-only lock
  //
  // Returns:
  // OK - success.
  // DATA_LOSS - Error writing data or fail to verify written data.
  Status Close();

  // Writes out the manifest of the staged bundle via a backend-supplied writer.
  //
  // Returns:
  // FAILED_PRECONDITION - Bundle is not open and verified.
  Status PersistManifest();

  // Returns a reader for the (verified) payload bytes of a specified target
  // file.
  //
  // Returns:
  // A reader instance for the target file.
  stream::IntervalReader GetTargetPayload(std::string_view target_name);
  stream::IntervalReader GetTargetPayload(protobuf::String target_name);

  // Exposes "manifest" information from the incoming update bundle once it has
  // passed verification.
  ManifestAccessor GetManifest();

  // Returns the total number of bytes of all target payloads listed in the
  // manifest *AND* exists in the bundle.
  Result<uint64_t> GetTotalPayloadSize();

 private:
  // Union is a temporary measure to allow for migration from the BlobStore
  // constructor to the OpenableReader constructor. The BlobStoreOpenableReader
  // should never be accessed directly. Access it through the update_reader_.
  union {
    BlobStoreOpenableReader optional_blob_store_openeable_reader_;
    char optional_blob_store_reader_unused_;
  };

  OpenableReader& update_reader_;
  BundledUpdateBackend& backend_;
  protobuf::Message bundle_;
  // The current, cached, trusted `SignedRootMetadata{}`.
  protobuf::Message trusted_root_;
  bool self_verification_;
  bool bundle_verified_ = false;

  // Opens the bundle for read-only access and readies the parser.
  Status DoOpen();

  // Performs TUF and downstream custom verification.
  Status DoVerify();

  // The method checks whether the update bundle contains a root metadata
  // different from the on-device one. If it does, it performs the following
  // verification and upgrade flow:
  //
  // 1. Verify the signatures according to the on-device trusted
  //    root metadata obtained from the backend.
  // 2. Verify content of the new root metadata, including:
  //    1) Check role magic field.
  //    2) Check signature requirement. Specifically, check that no key is
  //       reused across different roles and keys are unique in the same
  //       requirement.
  //    3) Check key mapping. Specifically, check that all keys are unique,
  //       ECDSA keys, and the key ids are exactly the SHA256 of `key type +
  //       key scheme + key value`.
  // 3. Verify the signatures against the new root metadata.
  // 4. Check rollback.
  // 5. Update on-device root metadata.
  Status UpgradeRoot();

  // The method verifies the top-level targets metadata against the trusted
  // root. The verification includes the following:
  //
  // 1. Verify the signatures of the targets metadata.
  // 2. Check the content of the targets metadata.
  // 3. Check rollback against the version from on-device manifest, if one
  //    exists (the manifest may be reset in the case of key rotation).
  Status VerifyTargetsMetadata();

  // A helper to get the on-device trusted root metadata. It returns an
  // instance of SignedRootMetadata proto message.
  protobuf::Message GetOnDeviceTrustedRoot();

  // A helper to get an accessor to the on-device manifest. The on-device
  // manifest is a serialized `message Manifest{...}` that represents the
  // current running firmware of the device. The on-device manifest storage
  // MUST meet the following requirements.
  //
  // 1. MUST NOT get wiped by a factory reset, otherwise a FDR can be used
  //    to circumvent anti-rollback check.
  // 2. MUST be kept in-sync with the actual firmware on-device. If any
  //    mechanism is used to modify the firmware (e.g. via flashing), the
  //    on-device manifest MUST be updated to reflect the change as well.
  //    The on-device manifest CAN be erased if updating it is too cumbersome
  //    BUT ONLY ON DEV DEVICES as erasing the on-device manifest defeats
  //    anti-rollback.
  // 3. MUST be integrity-protected and checked. Corrupted on-device manifest
  //    cannot be used as it may brick a device as a result of anti-rollback
  //    check. Integrity check is added and enforced by the backend via
  //    `BundledUpdateBackend` callbacks.
  //
  ManifestAccessor GetOnDeviceManifest();

  // Verify all targets referenced in the manifest (Targets metadata) has a
  // payload blob either within the bundle or on-device, in both cases
  // measuring up to the length and hash recorded in the manifest.
  Status VerifyTargetsPayloads();

  // Verify a target specified by name measures up to the expected length and
  // SHA256 hash. Additionally call the backend to perform any product-specific
  // validations.
  Status VerifyTargetPayload(ManifestAccessor manifest,
                             std::string_view name,
                             protobuf::Uint64 expected_length,
                             protobuf::Bytes expected_sha256);

  // For a target the payload of which is included in the bundle, verify
  // it measures up to the expected length and sha256 hash.
  Status VerifyInBundleTargetPayload(protobuf::Uint64 expected_length,
                                     protobuf::Bytes expected_sha256,
                                     stream::IntervalReader payload_reader);

  // For a target with no corresponding payload in the bundle, verify
  // its on-device payload bytes measures up to the expected length and sha256
  // hash.
  Status VerifyOutOfBundleTargetPayload(std::string_view name,
                                        protobuf::Uint64 expected_length,
                                        protobuf::Bytes expected_sha256);
};

}  // namespace pw::software_update
