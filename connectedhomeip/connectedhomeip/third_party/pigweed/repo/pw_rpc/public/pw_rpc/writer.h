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

#include "pw_rpc/internal/call.h"

namespace pw::rpc {

// The Writer class allows writing requests or responses to a streaming RPC.
// ClientWriter, ClientReaderWriter, ServerWriter, and ServerReaderWriter
// classes can be used as a generic Writer.
class Writer : private internal::Call {
 public:
  // Writers cannot be created directly. They may only be used as a reference to
  // an existing call object.
  Writer() = delete;

  Writer(const Writer&) = delete;
  Writer(Writer&&) = delete;

  Writer& operator=(const Writer&) = delete;
  Writer& operator=(Writer&&) = delete;

  using internal::Call::active;
  using internal::Call::channel_id;

  using internal::Call::Write;

 private:
  friend class internal::Call;
};

namespace internal {

constexpr Call::operator Writer&() { return static_cast<Writer&>(*this); }

constexpr Call::operator const Writer&() const {
  return static_cast<const Writer&>(*this);
}

}  // namespace internal
}  // namespace pw::rpc
