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

#include "pw_preprocessor/compiler.h"

PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Wcast-qual");
// "-Wpedantic" because some downstream compiler don't recognize the following
// two commented-out options.
// PW_MODIFY_DIAGNOSTIC(ignored, "-Wgnu-anonymous-struct");
// PW_MODIFY_DIAGNOSTIC(ignored, "-Wnested-anon-types");
PW_MODIFY_DIAGNOSTIC(ignored, "-Wpedantic");
#include "openssl/sha.h"
PW_MODIFY_DIAGNOSTICS_POP();

namespace pw::crypto::sha256::backend {

typedef SHA256_CTX NativeSha256Context;

}  // namespace pw::crypto::sha256::backend
