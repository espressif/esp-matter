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
#include "pw_software_update/manifest_accessor.h"

#include "pw_software_update/config.h"
#include "pw_software_update/update_bundle.pwpb.h"
#include "pw_software_update/update_bundle_accessor.h"

namespace pw::software_update {

ManifestAccessor ManifestAccessor::FromBundle(protobuf::Message bundle) {
  protobuf::Message targets_metadata =
      bundle
          .AsStringToMessageMap(static_cast<uint32_t>(
              UpdateBundle::Fields::TARGETS_METADATA))[kTopLevelTargetsName]
          .AsMessage(static_cast<uint32_t>(
              SignedTargetsMetadata::Fields::SERIALIZED_TARGETS_METADATA));

  protobuf::Bytes user_manifest =
      bundle.AsStringToBytesMap(static_cast<uint32_t>(
          UpdateBundle::Fields::TARGET_PAYLOADS))[kUserManifestTargetFileName];

  return ManifestAccessor(targets_metadata, user_manifest);
}

ManifestAccessor ManifestAccessor::FromManifest(protobuf::Message manifest) {
  protobuf::Message targets_metadata =
      manifest.AsStringToMessageMap(static_cast<uint32_t>(
          Manifest::Fields::TARGETS_METADATA))[kTopLevelTargetsName];

  protobuf::Bytes user_manifest =
      manifest.AsBytes(static_cast<uint32_t>(Manifest::Fields::USER_MANIFEST));

  return ManifestAccessor(targets_metadata, user_manifest);
}

protobuf::RepeatedMessages ManifestAccessor::GetTargetFiles() {
  PW_TRY(status());
  return targets_metadata_.AsRepeatedMessages(
      static_cast<uint32_t>(TargetsMetadata::Fields::TARGET_FILES));
}

protobuf::Uint32 ManifestAccessor::GetVersion() {
  PW_TRY(status());
  return targets_metadata_
      .AsMessage(
          static_cast<uint32_t>(TargetsMetadata::Fields::COMMON_METADATA))
      .AsUint32(static_cast<uint32_t>(CommonMetadata::Fields::VERSION));
}

Status ManifestAccessor::Export(stream::Writer& writer) {
  PW_TRY(status());

  // Write out the targets metadata map.
  stream::MemoryReader name_reader(as_bytes(span(kTopLevelTargetsName)));
  stream::IntervalReader metadata_reader =
      targets_metadata_.ToBytes().GetBytesReader();
  std::byte stream_pipe_buffer[WRITE_MANIFEST_STREAM_PIPE_BUFFER_SIZE];
  PW_TRY(protobuf::WriteProtoStringToBytesMapEntry(
      static_cast<uint32_t>(Manifest::Fields::TARGETS_METADATA),
      name_reader,
      kTopLevelTargetsName.size(),
      metadata_reader,
      metadata_reader.interval_size(),
      stream_pipe_buffer,
      writer));

  // The user manifest is optional, write it out if available().
  stream::IntervalReader user_manifest_reader = user_manifest_.GetBytesReader();
  if (user_manifest_reader.ok()) {
    protobuf::StreamEncoder encoder(writer, {});
    PW_TRY(encoder.WriteBytesFromStream(
        static_cast<uint32_t>(Manifest::Fields::USER_MANIFEST),
        user_manifest_reader,
        user_manifest_reader.interval_size(),
        stream_pipe_buffer));
  }

  return OkStatus();
}

protobuf::Message ManifestAccessor::GetTargetFile(protobuf::String name) {
  PW_TRY(status());

  std::array<std::byte, MAX_TARGET_NAME_LENGTH> name_buf = {};

  stream::IntervalReader name_reader = name.GetBytesReader();
  PW_TRY(name_reader.status());

  if (name_reader.interval_size() > name_buf.size()) {
    return Status::OutOfRange();
  }

  Result<ByteSpan> read_result = name_reader.Read(name_buf);
  PW_TRY(read_result.status());

  const ConstByteSpan name_span = read_result.value();
  const std::string_view name_view(
      reinterpret_cast<const char*>(name_span.data()), name_span.size_bytes());

  return GetTargetFile(name_view);
}

protobuf::Message ManifestAccessor::GetTargetFile(std::string_view name) {
  PW_TRY(status());

  for (protobuf::Message target_file : GetTargetFiles()) {
    protobuf::String target_name = target_file.AsString(
        static_cast<uint32_t>(TargetFile::Fields::FILE_NAME));
    Result<bool> compare_result = target_name.Equal(name);
    PW_TRY(compare_result.status());
    if (compare_result.value()) {
      return target_file;
    }
  }

  return Status::NotFound();
}

}  // namespace pw::software_update
