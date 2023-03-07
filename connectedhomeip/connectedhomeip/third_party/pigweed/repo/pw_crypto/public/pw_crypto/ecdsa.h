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

namespace pw::crypto::ecdsa {

// VerifyP256Signature verifies the `signature` of `digest` using `public_key`.
//
// `public_key` is a byte string in SEC 1 uncompressed form (0x04||X||Y), which
// is exactly 65 bytes. Compressed forms (02/03||X) *may* not be supported
// by some backends, e.g. Mbed TLS.
//
// `digest` is a raw byte string, truncated to 32 bytes.
//
// `signature` is a raw byte string (r||s) of exactly 64 bytes.
//
// Returns Status::OkStatus() for a successful verification, or an error Status
// otherwise.
Status VerifyP256Signature(ConstByteSpan public_key,
                           ConstByteSpan digest,
                           ConstByteSpan signature);

}  // namespace pw::crypto::ecdsa
