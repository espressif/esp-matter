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

#include <string_view>

#include "pw_stream/stream.h"
#include "pw_tls_client/options.h"
#include "pw_tls_client/status.h"

// The backend shall provide the following header that implements a
// backend::SessionImplementation type.
#include "pw_tls_client_backends/backend_types.h"

namespace pw::tls_client {

// Session provides APIs for performing TLS communication.
class Session : public stream::NonSeekableReaderWriter {
 public:
  Session() = delete;

  Session(const Session&) = delete;
  Session& operator=(const Session&) = delete;

  // Resources allocated during Session::Create() will be released in the
  // destructor. For example, backend may choose to allocate Session from a pool
  // during Session::Create() and returns it in the destructor.
  //
  // Close() will be called if the Session is open. Since Close() returns a
  // pw::Status which cannot be forwarded from a destructor for callers to
  // check. Backend shall assert check that it is OkStatus().
  ~Session() override;

  // Starts a TLS connection. The backend performs TLS handshaking and
  // certificate verification/revocation/expiration check.
  //
  // Calling Open() on an already opened Session returns error.
  Status Open();

  // Closes a TLS connection. The backend sends shutdown notification to the
  // server. Note that the shutdown notification is part of the TLS protocol.
  // Nothing is done to the underlying transport used to create this Session.
  // The closing of the underlying transport is managed by the users of this
  // class. Calling Close() on an unopened Session returns error.
  Status Close();

  // If Open()/Close()/Read()/Write() fails, the method returns a more detailed
  // code indicating the last error. See definition of TLSStatus in status.h.
  // The backend is responsible for mapping TLS library errors into the ones
  // defined in TLSStatus.
  TLSStatus GetLastTLSStatus();

  // Factory method for creating an instance of Session according to the given
  // options. Backend allocates and initializes library specific data structures
  // (implmented in |session_impl_|) for operating as a TLS client.
  //
  // The client remains inactive until the call of Open().
  static Result<Session*> Create(const SessionOptions& option);

 private:
  // A Session instance should only be created from Create().
  Session(const SessionOptions& option);

  StatusWithSize DoRead(ByteSpan dest) override;
  Status DoWrite(ConstByteSpan data) override;

  // An opaque object up to the backend to implement.
  backend::SessionImplementation session_impl_;
};

}  // namespace pw::tls_client
