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

#include "pw_crypto/sha256_mock.h"

#include "pw_crypto/sha256.h"
#include "pw_status/status.h"

namespace pw::crypto::sha256::backend {

namespace {
ErrorKind g_injected_error = ErrorKind::kNone;
}  // namespace

void InjectError(const ErrorKind err) { g_injected_error = err; }

Status DoInit(NativeSha256Context&) {
  return (g_injected_error == ErrorKind::kInit) ? Status::Internal()
                                                : OkStatus();
}

Status DoUpdate(NativeSha256Context&, ConstByteSpan) {
  return (g_injected_error == ErrorKind::kUpdate) ? Status::Internal()
                                                  : OkStatus();
}

Status DoFinal(NativeSha256Context&, ByteSpan) {
  return (g_injected_error == ErrorKind::kFinal) ? Status::Internal()
                                                 : OkStatus();
}

}  // namespace pw::crypto::sha256::backend
