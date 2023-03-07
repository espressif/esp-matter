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

#include "pw_protobuf/message.h"
#include "pw_result/result.h"
#include "pw_stream/stream.h"

namespace pw::software_update {
// ManifestAccessor exposes manifest information from either a *verified* update
// bundle (`message UpdateBundle`) or a *trusted* on-device manifest
// (`message Manifest`).
//
// Instantiation MUST go-through the UpdateBundleAccessor class. e.g.:
//
// ManifestAccessor manifest = bundle.GetManifest();
// PW_TRY(manifest.status());  // Fails if `bundle` is not yet verified.
class ManifestAccessor {
 public:
  ManifestAccessor() = default;

  Status status() { return targets_metadata_.status(); }
  bool ok() { return status().ok(); }

  // Retrieves the "user manifest" blob, which is product specific and optional.
  pw::stream::IntervalReader GetUserManifest() {
    return user_manifest_.GetBytesReader();
  }

  // Enumerates all target files as a list of `message TargetFile{...}`.
  protobuf::RepeatedMessages GetTargetFiles();

  // Given a name, return a `message TargetFile{...}` descriptor.
  protobuf::Message GetTargetFile(protobuf::String name);
  protobuf::Message GetTargetFile(std::string_view name);

  // Returns the manifest version number.
  protobuf::Uint32 GetVersion();

  // TODO(alizhang): Deprecate WriteManifest() once backend code has changed
  // to UpdateBundleAccessor::PersistManifest() where the backend is given
  // chances to prepare and release the manifest writer.
  Status WriteManifest(stream::Writer& writer) { return Export(writer); }

 private:
  friend class UpdateBundleAccessor;

  protobuf::Message targets_metadata_;
  protobuf::Bytes user_manifest_;

  ManifestAccessor(Status status) : targets_metadata_(status) {}
  ManifestAccessor(protobuf::Message targets_metadata,
                   protobuf::Bytes user_manifest)
      : targets_metadata_(targets_metadata), user_manifest_(user_manifest) {}

  // Constructs a `ManifestAccessor` from an update bundle.
  static ManifestAccessor FromBundle(protobuf::Message bundle);

  // Constructs a `ManifestAccessor` from a saved `message Manifest{...}`.
  static ManifestAccessor FromManifest(protobuf::Message manifest);

  // Exports a serialized `message Manifest{...}`.
  Status Export(stream::Writer& writer);
};

}  // namespace pw::software_update
