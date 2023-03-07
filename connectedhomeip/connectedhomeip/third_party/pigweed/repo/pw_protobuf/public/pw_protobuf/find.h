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
#pragma once

#include "pw_protobuf/decoder.h"

namespace pw::protobuf {

// DecodeHandler that searches for a specific field in a proto message. If the
// field is found, it cancels the decode operation. Supports searching for
// nested fields by passing in another instance of a FindDecodeHandler for the
// nested message.
class FindDecodeHandler final : public DecodeHandler {
 public:
  constexpr FindDecodeHandler(uint32_t field_number)
      : FindDecodeHandler(field_number, nullptr) {}

  constexpr FindDecodeHandler(uint32_t field_number, FindDecodeHandler* nested)
      : field_number_(field_number), found_(false), nested_handler_(nested) {}

  Status ProcessField(CallbackDecoder& decoder, uint32_t field_number) override;

  bool found() const { return found_; }

  void set_nested_handler(FindDecodeHandler* nested) {
    nested_handler_ = nested;
  }

 private:
  uint32_t field_number_;
  bool found_;
  FindDecodeHandler* nested_handler_;
};

}  // namespace pw::protobuf
