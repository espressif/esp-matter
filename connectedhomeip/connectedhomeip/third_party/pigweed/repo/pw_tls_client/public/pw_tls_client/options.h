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

#include "pw_assert/assert.h"
#include "pw_assert/check.h"
#include "pw_stream/stream.h"
#include "pw_string/util.h"

namespace pw::tls_client {

class SessionOptions {
 public:
  // Sets the TLS server name. This is typically a domain name (e.g.
  // www.google.com) used to differentiate any other virtual domain names
  // resident on the same physical server. The option is used as the Server
  // Name Indication(SNI) extension during TLS handshake.
  //
  // Callers need to ensure that the memory backing |server_name| is valid until
  // being passed to Session::Create(), where backend has a chance to load or
  // make a copy.
  constexpr SessionOptions& set_server_name(std::string_view server_name) {
    server_name_ = server_name;
    return *this;
  }

  // Set the underlying transport for the TLS connection. The transport is
  // provided through an instance of stream::ReaderWriter. Callers should
  // guarantee that the transport object outlives the Session instance to be
  // built.
  constexpr SessionOptions& set_transport(stream::ReaderWriter& transport) {
    transport_ = &transport;
    return *this;
  }

  constexpr pw::stream::ReaderWriter* transport() const { return transport_; }

  constexpr std::string_view server_name() const { return server_name_; }

 private:
  std::string_view server_name_;
  pw::stream::ReaderWriter* transport_ = nullptr;

  // TODO(zyecheng): Expand the list as necessary to cover aspects such as
  // certificate verification/revocation check policies.
};

}  // namespace pw::tls_client
