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
PW_MODIFY_DIAGNOSTIC(ignored, "-Wswitch-enum");
#include "mbedtls/certs.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
PW_MODIFY_DIAGNOSTICS_POP();

#include "pw_status/status.h"
#include "pw_tls_client/options.h"

namespace pw::tls_client::backend {
class SessionImplementation {
 public:
  SessionImplementation(SessionOptions options);
  ~SessionImplementation();
  Status Setup();
  void SetTlsStatus(TLSStatus status) { tls_status_ = status; }
  TLSStatus GetTlsStatus() { return tls_status_; }

  // The method is for test only. When given a non-Ok status, it will override
  // the status returned by entropy source pw::tls_client::GetRandomBytes();
  static void SetEntropySourceStatus(Status status);

 private:
  // mbedtls entropy
  mbedtls_entropy_context entropy_ctx_;
  mbedtls_ctr_drbg_context drbg_ctx_;

  // SSL data structure
  mbedtls_ssl_context ssl_ctx_;

  // Configuration data structure
  mbedtls_ssl_config ssl_config_;

  // A copy of the option when creating the client.
  SessionOptions session_options_;

  TLSStatus tls_status_ = TLSStatus::kOk;

  static int MbedTlsWrite(void* ctx, const uint8_t* buf, size_t len);
  static int MbedTlsRead(void* ctx, unsigned char* buf, size_t len);
  static int MbedTlsEntropySource(void* ctx,
                                  unsigned char* out,
                                  size_t len,
                                  size_t* output_length);

  static Status entropy_source_status_;
};

}  // namespace pw::tls_client::backend
