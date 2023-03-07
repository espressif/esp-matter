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

#include "pw_bytes/span.h"
#include "pw_status/status.h"

namespace pw::tls_client {
// Fill |dest| with fresh, full-entropy random bytes upon each call.
// |dest| must contain full entropy. Backend should take care of the possible
// conditioning in its implementation.
Status GetRandomBytes(ByteSpan dest);

// An overloaded variant for accomodating C API interfaces, i.e. mbed TLS.
inline Status GetRandomBytes(span<unsigned char> dest) {
  return GetRandomBytes(as_writable_bytes(dest));
}
}  // namespace pw::tls_client
