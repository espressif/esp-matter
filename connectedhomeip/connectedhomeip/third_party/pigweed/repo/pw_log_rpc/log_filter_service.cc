// Copyright 2020 The Pigweed Authors
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
#include "pw_log_rpc/log_filter_service.h"

#include "pw_log/log.h"
#include "pw_log/proto/log.pwpb.h"
#include "pw_log_rpc/log_filter.h"
#include "pw_protobuf/decoder.h"

namespace pw::log_rpc {

namespace GetFilterRequest = ::pw::log::pwpb::GetFilterRequest;
namespace SetFilterRequest = ::pw::log::pwpb::SetFilterRequest;
namespace FilterRule = ::pw::log::pwpb::FilterRule;

Status FilterService::SetFilterImpl(ConstByteSpan request) {
  protobuf::Decoder decoder(request);
  PW_TRY(decoder.Next());
  if (static_cast<SetFilterRequest::Fields>(decoder.FieldNumber()) !=
      SetFilterRequest::Fields::FILTER_ID) {
    return Status::InvalidArgument();
  }
  ConstByteSpan filter_id;
  PW_TRY(decoder.ReadBytes(&filter_id));
  Result<Filter*> filter = filter_map_.GetFilterFromId(filter_id);
  if (!filter.ok()) {
    return Status::NotFound();
  }

  PW_TRY(decoder.Next());
  ConstByteSpan filter_buffer;
  if (static_cast<SetFilterRequest::Fields>(decoder.FieldNumber()) !=
      SetFilterRequest::Fields::FILTER) {
    return Status::InvalidArgument();
  }
  PW_TRY(decoder.ReadBytes(&filter_buffer));

  return filter.value()->UpdateRulesFromProto(filter_buffer);
}

StatusWithSize FilterService::GetFilterImpl(ConstByteSpan request,
                                            ByteSpan response) {
  protobuf::Decoder decoder(request);
  PW_TRY_WITH_SIZE(decoder.Next());
  if (static_cast<GetFilterRequest::Fields>(decoder.FieldNumber()) !=
      GetFilterRequest::Fields::FILTER_ID) {
    return StatusWithSize::InvalidArgument();
  }
  ConstByteSpan filter_id;
  PW_TRY_WITH_SIZE(decoder.ReadBytes(&filter_id));
  Result<Filter*> filter = filter_map_.GetFilterFromId(filter_id);
  if (!filter.ok()) {
    return StatusWithSize::NotFound();
  }

  log::pwpb::Filter::MemoryEncoder encoder(response);
  for (auto& rule : (*filter)->rules()) {
    FilterRule::StreamEncoder rule_encoder = encoder.GetRuleEncoder();
    rule_encoder.WriteLevelGreaterThanOrEqual(rule.level_greater_than_or_equal)
        .IgnoreError();
    rule_encoder.WriteModuleEquals(rule.module_equals).IgnoreError();
    rule_encoder.WriteAnyFlagsSet(rule.any_flags_set).IgnoreError();
    rule_encoder.WriteAction(static_cast<FilterRule::Action>(rule.action))
        .IgnoreError();
    rule_encoder.WriteThreadEquals(rule.thread_equals).IgnoreError();
    PW_TRY_WITH_SIZE(rule_encoder.status());
  }
  PW_TRY_WITH_SIZE(encoder.status());

  return StatusWithSize(encoder.size());
}

StatusWithSize FilterService::ListFilterIdsImpl(ByteSpan response) {
  log::pwpb::FilterIdListResponse::MemoryEncoder encoder(response);
  for (auto& filter : filter_map_.filters()) {
    PW_TRY_WITH_SIZE(encoder.WriteFilterId(filter.id()));
  }
  return StatusWithSize(encoder.size());
}

}  // namespace pw::log_rpc
