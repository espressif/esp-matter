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
#define PW_LOG_MODULE_NAME "SHA256-MTLS"
#define PW_LOG_LEVEL PW_LOG_LEVEL_WARN

#include "pw_crypto/sha256.h"
#include "pw_status/status.h"

namespace pw::crypto::sha256::backend {

Status DoInit(NativeSha256Context& ctx) {
  // mbedtsl_sha256_init() never fails (returns void).
  mbedtls_sha256_init(&ctx);

  if (mbedtls_sha256_starts_ret(&ctx, /* is224 = */ 0)) {
    return Status::Internal();
  }

  return OkStatus();
}

Status DoUpdate(NativeSha256Context& ctx, ConstByteSpan data) {
  if (mbedtls_sha256_update_ret(
          &ctx,
          reinterpret_cast<const unsigned char*>(data.data()),
          data.size())) {
    return Status::Internal();
  }

  return OkStatus();
}

Status DoFinal(NativeSha256Context& ctx, ByteSpan out_digest) {
  if (mbedtls_sha256_finish_ret(
          &ctx, reinterpret_cast<unsigned char*>(out_digest.data()))) {
    return Status::Internal();
  }

  return OkStatus();
}

}  // namespace pw::crypto::sha256::backend
