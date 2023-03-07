// Copyright 2022 The Pigweed Authors
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

#include "pw_function/function.h"
#include "pw_rpc/raw/client_reader_writer.h"
#include "pw_transfer/internal/context.h"

namespace pw::transfer::internal {

class ClientContext final : public Context {
 public:
  constexpr ClientContext() : on_completion_(nullptr) {}

  void set_on_completion(Function<void(Status)>&& on_completion) {
    on_completion_ = std::move(on_completion);
  }

  // In client-side transfer contexts, a session ID may not yet have been
  // assigned by the server, in which case resource_id is used as the context
  // identifier.
  constexpr uint32_t id() const {
    return session_id() == kUnassignedSessionId ? resource_id() : session_id();
  }

 private:
  Status FinalCleanup(Status status) override;

  Function<void(Status)> on_completion_;
};

}  // namespace pw::transfer::internal
