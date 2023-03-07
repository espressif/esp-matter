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

#include <cstring>

#include "pw_bytes/span.h"
#include "pw_rpc/internal/call_context.h"
#include "pw_rpc/internal/server_call.h"
#include "pw_rpc/method_type.h"

namespace pw::rpc::internal::test {

// Fake server reader/writer classes for testing use. These also serve as a
// model for how the RPC implementations (raw, pwpb, Nanopb) structure their
// reader/writer classes.
//
// Readers/writers use an unusual inheritance hierarchy. Rather than having the
// ServerReaderWriter inherit from both the Reader and Writer classes, the
// readers and writers inherit from it, but hide the unsupported functionality.
// A ReaderWriter defines conversions to Reader and Writer, so it acts as if it
// inherited from both. This approach is unusual but necessary to have all
// classes use a single IntrusiveList::Item base and to avoid virtual methods or
// virtual inheritance.
//
// Call's public API is intended for rpc::Server, so hide the public methods
// with private inheritance.
class FakeServerReaderWriter : private ServerCall {
 public:
  constexpr FakeServerReaderWriter() = default;

  // On a real reader/writer, this constructor would not be exposed.
  FakeServerReaderWriter(const LockedCallContext& context,
                         MethodType type = MethodType::kBidirectionalStreaming)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock())
      : ServerCall(context, type) {}

  FakeServerReaderWriter(FakeServerReaderWriter&&) = default;
  FakeServerReaderWriter& operator=(FakeServerReaderWriter&&) = default;

  // Pull in protected functions from the hidden Call base as needed.
  //
  // Note: these functions all acquire `rpc_lock()`. However, the
  // `PW_LOCKS_EXCLUDED(rpc_lock())` on their original definitions does not
  // appear to carry through here.
  using Call::active;
  using Call::set_on_error;
  using Call::set_on_next;
  using ServerCall::set_on_client_stream_end;

  Status Finish(Status status = OkStatus()) {
    return CloseAndSendResponse(status);
  }

  using Call::Write;

  // Expose a few additional methods for test use.
  ServerCall& as_server_call() { return *this; }
};

class FakeServerWriter : private FakeServerReaderWriter {
 public:
  constexpr FakeServerWriter() = default;

  FakeServerWriter(const LockedCallContext& context)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock())
      : FakeServerReaderWriter(context, MethodType::kServerStreaming) {}
  FakeServerWriter(FakeServerWriter&&) = default;
  FakeServerWriter& operator=(FakeServerWriter&&) = default;

  // Common reader/writer functions.
  using FakeServerReaderWriter::active;
  using FakeServerReaderWriter::Finish;
  using FakeServerReaderWriter::set_on_error;
  using FakeServerReaderWriter::Write;

  // Functions for test use.
  using FakeServerReaderWriter::as_server_call;
};

class FakeServerReader : private FakeServerReaderWriter {
 public:
  constexpr FakeServerReader() = default;

  FakeServerReader(const LockedCallContext& context)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock())
      : FakeServerReaderWriter(context, MethodType::kClientStreaming) {}

  FakeServerReader(FakeServerReader&&) = default;
  FakeServerReader& operator=(FakeServerReader&&) = default;

  using FakeServerReaderWriter::active;
  using FakeServerReaderWriter::as_server_call;
};

}  // namespace pw::rpc::internal::test
