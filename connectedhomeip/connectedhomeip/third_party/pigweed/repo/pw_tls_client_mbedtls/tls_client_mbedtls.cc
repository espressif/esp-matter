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

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_tls_client/entropy.h"
#include "pw_tls_client/session.h"
#include "pw_tls_client_mbedtls/backend_types.h"

namespace pw::tls_client {
namespace backend {

int SessionImplementation::MbedTlsWrite(void* ctx,
                                        const uint8_t* buf,
                                        size_t len) {
  PW_CHECK_NOTNULL(ctx);
  PW_CHECK_NOTNULL(buf);
  auto writer =
      static_cast<SessionImplementation*>(ctx)->session_options_.transport();
  PW_CHECK_NOTNULL(writer);
  return writer->Write(buf, len).ok() ? len : -1;
}

int SessionImplementation::MbedTlsRead(void* ctx,
                                       unsigned char* buf,
                                       size_t len) {
  PW_CHECK_NOTNULL(ctx);
  PW_CHECK_NOTNULL(buf);
  auto reader =
      static_cast<SessionImplementation*>(ctx)->session_options_.transport();
  PW_CHECK_NOTNULL(reader);
  auto res = reader->Read(buf, len);
  if (!res.ok()) {
    return -1;
  }
  return res.value().empty() ? MBEDTLS_ERR_SSL_WANT_READ : res.value().size();
}

Status SessionImplementation::entropy_source_status_ = OkStatus();

void SessionImplementation::SetEntropySourceStatus(Status status) {
  entropy_source_status_ = status;
}

// Entropy source callback
int SessionImplementation::MbedTlsEntropySource(void* ctx,
                                                unsigned char* out,
                                                size_t len,
                                                size_t* output_length) {
  Status status;
  if (entropy_source_status_ != OkStatus()) {
    status = entropy_source_status_;
  } else {
    status = GetRandomBytes({out, len});
  }

  if (!status.ok()) {
    PW_LOG_DEBUG("Failed to generate random bytes");
    auto session_impl = static_cast<SessionImplementation*>(ctx);
    session_impl->SetTlsStatus(pw::tls_client::TLSStatus::kEntropySourceFailed);
    return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
  }
  *output_length = len;
  return 0;
}

SessionImplementation::SessionImplementation(SessionOptions options)
    : session_options_(options) {
  mbedtls_ssl_init(&ssl_ctx_);
  mbedtls_ssl_config_init(&ssl_config_);
  mbedtls_ctr_drbg_init(&drbg_ctx_);
  mbedtls_entropy_init(&entropy_ctx_);
}

SessionImplementation::~SessionImplementation() {
  mbedtls_ssl_free(&ssl_ctx_);
  mbedtls_ssl_config_free(&ssl_config_);
  mbedtls_ctr_drbg_free(&drbg_ctx_);
  mbedtls_entropy_free(&entropy_ctx_);
}

Status SessionImplementation::Setup() {
  int ret = 0;

  // Set up default configuration.
  ret = mbedtls_ssl_config_defaults(
      &ssl_config_,
      // Configured as client.
      MBEDTLS_SSL_IS_CLIENT,
      // Statndard TLS. The other option is MBEDTLS_SSL_TRANSPORT_DATAGRAM
      // for DTLS, which we'll consider later.
      MBEDTLS_SSL_TRANSPORT_STREAM,
      // This option is used in all MbedTLS native examples.
      // The other option is MBEDTLS_SSL_PRESET_SUITEB.
      // However, there is no document/comment availalbe on what they do.
      // Base on the source code, these options will restrict the version
      // of TLS protocol. MBEDTLS_SSL_PRESET_SUITEB forces TLS 1.2.
      // MBEDTLS_SSL_PRESET_DEFAULT is more relaxed. But since we
      // define MBEDTLS_SSL_PROTO_TLS1_2 for all configs. There shouldn't be
      // any difference.
      MBEDTLS_SSL_PRESET_DEFAULT);
  if (ret) {
    return Status::Internal();
  }

  // Set up an entropy source.
  ret = mbedtls_entropy_add_source(&entropy_ctx_,
                                   MbedTlsEntropySource,
                                   this,
                                   1,
                                   MBEDTLS_ENTROPY_SOURCE_STRONG);
  if (ret) {
    return Status::Internal();
  }

  // Set up drbg.
  unsigned char personalized_bytes[] = "pw_tls_client";
  ret = mbedtls_ctr_drbg_seed(&drbg_ctx_,
                              mbedtls_entropy_func,
                              &entropy_ctx_,
                              personalized_bytes,
                              sizeof(personalized_bytes));
  if (ret) {
    if (ret == MBEDTLS_ERR_CTR_DRBG_ENTROPY_SOURCE_FAILED) {
      tls_status_ = TLSStatus::kEntropySourceFailed;
    }
    return Status::Internal();
  }

  // The API does not fail.
  mbedtls_ssl_conf_rng(&ssl_config_, mbedtls_ctr_drbg_random, &drbg_ctx_);

  // The API does not fail.
  mbedtls_ssl_conf_authmode(&ssl_config_, MBEDTLS_SSL_VERIFY_REQUIRED);

  // TODO(b/235289501): Add logic for loading trust anchors.

  // Load configuration to SSL.
  ret = mbedtls_ssl_setup(&ssl_ctx_, &ssl_config_);
  if (ret) {
    return Status::Internal();
  }

  // Set up transport.
  // The API does not fail.
  mbedtls_ssl_set_bio(&ssl_ctx_, this, MbedTlsWrite, MbedTlsRead, nullptr);

  ret = mbedtls_ssl_set_hostname(&ssl_ctx_,
                                 session_options_.server_name().data());
  if (ret) {
    return Status::Internal();
  }

  return OkStatus();
}

}  // namespace backend

Session::Session(const SessionOptions& options) : session_impl_(options) {}

Session::~Session() = default;

Result<Session*> Session::Create(const SessionOptions& options) {
  if (!options.transport()) {
    PW_LOG_DEBUG("Must provide a transport");
    return Status::Internal();
  }

  auto sess = new Session(options);
  if (!sess) {
    return Status::ResourceExhausted();
  }

  // Set up the client.
  auto setup_status = sess->session_impl_.Setup();
  if (!setup_status.ok()) {
    PW_LOG_DEBUG("Failed to setup");
    // TODO(b/235289501): `tls_status_` may be set, but the session object will
    // be released. Map `tls_stauts_` to string and print out here so that
    // the information can be catched.
    delete sess;
    return setup_status;
  }

  return sess;
}

Status Session::Open() {
  // TODO(b/235289501): To implement
  return Status::Unimplemented();
}

Status Session::Close() {
  // TODO(b/235289501): To implement
  return Status::Unimplemented();
}

StatusWithSize Session::DoRead(ByteSpan) {
  // TODO(b/235289501): To implement
  return StatusWithSize(Status::Unimplemented(), 0);
}

Status Session::DoWrite(ConstByteSpan) {
  // TODO(b/235289501): To implement
  return Status::Unimplemented();
}

TLSStatus Session::GetLastTLSStatus() { return session_impl_.GetTlsStatus(); }

}  // namespace pw::tls_client
