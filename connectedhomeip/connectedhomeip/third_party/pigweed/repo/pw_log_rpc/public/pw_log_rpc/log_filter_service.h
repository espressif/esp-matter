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

#include "pw_log/proto/log.raw_rpc.pb.h"
#include "pw_log_rpc/log_filter_map.h"
#include "pw_status/status_with_size.h"

namespace pw::log_rpc {

// Provides a way to retrieve and modify log filters.
class FilterService final
    : public log::pw_rpc::raw::Filters::Service<FilterService> {
 public:
  FilterService(FilterMap& filter_map) : filter_map_(filter_map) {}

  //  Modifies a log filter and its rules. The filter must be registered in the
  //  provided filter map.
  void SetFilter(ConstByteSpan request, rpc::RawUnaryResponder& responder) {
    responder.Finish({}, SetFilterImpl(request)).IgnoreError();
  }

  // Retrieves a log filter and its rules. The filter must be registered in the
  // provided filter map.
  void GetFilter(ConstByteSpan request, rpc::RawUnaryResponder& responder) {
    std::byte buffer[kFilterResponseBufferSize] = {};
    StatusWithSize result = GetFilterImpl(request, buffer);
    responder.Finish(span(buffer).first(result.size()), result.status())
        .IgnoreError();
  }

  void ListFilterIds(ConstByteSpan, rpc::RawUnaryResponder& responder) {
    std::byte buffer[kFilterIdsResponseBufferSize] = {};
    StatusWithSize result = ListFilterIdsImpl(buffer);
    responder.Finish(span(buffer).first(result.size()), result.status())
        .IgnoreError();
  }

 private:
  static constexpr size_t kMinSupportedFilters = 4;

  static constexpr size_t kFilterResponseBufferSize =
      protobuf::TagSizeBytes(log::pwpb::Filter::Fields::RULE) +
      protobuf::kMaxSizeOfLength +
      kMinSupportedFilters *
          (protobuf::SizeOfFieldEnum(
               log::pwpb::FilterRule::Fields::LEVEL_GREATER_THAN_OR_EQUAL, 7) +
           protobuf::SizeOfFieldBytes(
               log::pwpb::FilterRule::Fields::MODULE_EQUALS,
               cfg::kMaxModuleNameBytes) +
           protobuf::SizeOfFieldUint32(
               log::pwpb::FilterRule::Fields::ANY_FLAGS_SET, 1) +
           protobuf::SizeOfFieldEnum(log::pwpb::FilterRule::Fields::ACTION, 2) +
           protobuf::SizeOfFieldBytes(
               log::pwpb::FilterRule::Fields::THREAD_EQUALS,
               cfg::kMaxThreadNameBytes));

  static constexpr size_t kFilterIdsResponseBufferSize =
      kMinSupportedFilters *
      protobuf::SizeOfFieldBytes(
          log::pwpb::FilterIdListResponse::Fields::FILTER_ID, 4);

  Status SetFilterImpl(ConstByteSpan request);
  StatusWithSize GetFilterImpl(ConstByteSpan request, ByteSpan response);
  StatusWithSize ListFilterIdsImpl(ByteSpan response);

  FilterMap& filter_map_;
};

}  // namespace pw::log_rpc
