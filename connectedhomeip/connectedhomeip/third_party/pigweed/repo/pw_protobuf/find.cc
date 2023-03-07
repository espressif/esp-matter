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

#include "pw_protobuf/find.h"

namespace pw::protobuf {

Status FindDecodeHandler::ProcessField(CallbackDecoder& decoder,
                                       uint32_t field_number) {
  if (field_number != field_number_) {
    // Continue to the next field.
    return OkStatus();
  }

  found_ = true;
  if (nested_handler_ == nullptr) {
    return Status::Cancelled();
  }

  span<const std::byte> submessage;
  if (Status status = decoder.ReadBytes(&submessage); !status.ok()) {
    return status;
  }

  CallbackDecoder subdecoder;
  subdecoder.set_handler(nested_handler_);
  return subdecoder.Decode(submessage);
}

}  // namespace pw::protobuf
