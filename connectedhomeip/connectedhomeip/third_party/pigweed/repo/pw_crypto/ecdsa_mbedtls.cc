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
#define PW_LOG_MODULE_NAME "ECDSA-MTLS"
#define PW_LOG_LEVEL PW_LOG_LEVEL_WARN

#include "mbedtls/ecdsa.h"
#include "pw_crypto/ecdsa.h"
#include "pw_function/function.h"
#include "pw_log/log.h"

namespace pw::crypto::ecdsa {

namespace {

// Defer calls a given function upon exiting a scope.
class Defer {
 public:
  Defer(Function<void()>&& callback) : callback_(std::move(callback)) {}
  ~Defer() { callback_(); }

 private:
  Function<void()> callback_;
};

}  // namespace

constexpr size_t kP256CurveOrderBytes = 32;

Status VerifyP256Signature(ConstByteSpan public_key,
                           ConstByteSpan digest,
                           ConstByteSpan signature) {
  // Use a local structure to avoid going over the default inline storage
  // for the `cleanup` callable used below.
  struct {
    // The elliptic curve group.
    mbedtls_ecp_group grp;
    // The public key point.
    mbedtls_ecp_point Q;
    // The signature (r, s).
    mbedtls_mpi r, s;
  } ctx;

  const uint8_t* public_key_data =
      reinterpret_cast<const uint8_t*>(public_key.data());
  const uint8_t* digest_data = reinterpret_cast<const uint8_t*>(digest.data());
  const uint8_t* signature_data =
      reinterpret_cast<const uint8_t*>(signature.data());

  // These init functions never fail.
  mbedtls_ecp_group_init(&ctx.grp);
  mbedtls_ecp_point_init(&ctx.Q);
  mbedtls_mpi_init(&ctx.r);
  mbedtls_mpi_init(&ctx.s);

  // Auto clean up on exit.
  Defer cleanup([&ctx](void) {
    mbedtls_ecp_group_free(&ctx.grp);
    mbedtls_ecp_point_free(&ctx.Q);
    mbedtls_mpi_free(&ctx.r);
    mbedtls_mpi_free(&ctx.s);
  });

  // Load the curve parameters.
  if (mbedtls_ecp_group_load(&ctx.grp, MBEDTLS_ECP_DP_SECP256R1)) {
    return Status::Internal();
  }

  // Load the public key.
  if (mbedtls_ecp_point_read_binary(
          &ctx.grp, &ctx.Q, public_key_data, public_key.size())) {
    PW_LOG_DEBUG("Bad public key format");
    return Status::InvalidArgument();
  }

  // Load the signature.
  if (signature.size() != kP256CurveOrderBytes * 2) {
    PW_LOG_DEBUG("Bad signature format");
    return Status::InvalidArgument();
  }

  if (mbedtls_mpi_read_binary(&ctx.r, signature_data, kP256CurveOrderBytes) ||
      mbedtls_mpi_read_binary(&ctx.s,
                              signature_data + kP256CurveOrderBytes,
                              kP256CurveOrderBytes)) {
    return Status::Internal();
  }

  // Digest must be 32 bytes or longer (and be truncated).
  if (digest.size() < kP256CurveOrderBytes) {
    PW_LOG_DEBUG("Digest is too short");
    return Status::InvalidArgument();
  }

  // Verify the signature.
  if (mbedtls_ecdsa_verify(
          &ctx.grp, digest_data, digest.size(), &ctx.Q, &ctx.r, &ctx.s)) {
    PW_LOG_DEBUG("Signature verification failed");
    return Status::Unauthenticated();
  }

  return OkStatus();
}

}  // namespace pw::crypto::ecdsa
