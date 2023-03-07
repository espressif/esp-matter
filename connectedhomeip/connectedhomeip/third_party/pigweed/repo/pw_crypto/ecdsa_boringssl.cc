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
#define PW_LOG_MODULE_NAME "ECDSA-BSSL"
#define PW_LOG_LEVEL PW_LOG_LEVEL_WARN

#include "pw_preprocessor/compiler.h"

PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Wcast-qual");
PW_MODIFY_DIAGNOSTIC(ignored, "-Wgnu-anonymous-struct");
PW_MODIFY_DIAGNOSTIC(ignored, "-Wnested-anon-types");
#include "openssl/bn.h"
#include "openssl/ec.h"
#include "openssl/ecdsa.h"
#include "openssl/nid.h"
PW_MODIFY_DIAGNOSTICS_POP();

#include "pw_crypto/ecdsa.h"
#include "pw_log/log.h"

namespace pw::crypto::ecdsa {

constexpr size_t kP256CurveOrderBytes = 32;

Status VerifyP256Signature(ConstByteSpan public_key,
                           ConstByteSpan digest,
                           ConstByteSpan signature) {
  const uint8_t* public_key_bytes =
      reinterpret_cast<const uint8_t*>(public_key.data());
  const uint8_t* digest_bytes = reinterpret_cast<const uint8_t*>(digest.data());
  const uint8_t* signature_bytes =
      reinterpret_cast<const uint8_t*>(signature.data());

  // Allocate objects needed for ECDSA verification. BoringSSL relies on
  // dynamic allocation.
  bssl::UniquePtr<EC_GROUP> group(
      EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1));
  if (!group) {
    return Status::ResourceExhausted();
  }

  bssl::UniquePtr<EC_POINT> pub_key(EC_POINT_new(group.get()));
  bssl::UniquePtr<EC_KEY> key(EC_KEY_new());
  bssl::UniquePtr<ECDSA_SIG> sig(ECDSA_SIG_new());
  if (!(pub_key && key && sig)) {
    return Status::ResourceExhausted();
  }

  // Load the public key.
  if (!EC_POINT_oct2point(group.get(),
                          pub_key.get(),
                          public_key_bytes,
                          public_key.size(),
                          nullptr)) {
    PW_LOG_DEBUG("Bad public key format");
    return Status::InvalidArgument();
  }

  if (!EC_KEY_set_group(key.get(), group.get())) {
    return Status::InvalidArgument();
  }

  if (!EC_KEY_set_public_key(key.get(), pub_key.get())) {
    return Status::InvalidArgument();
  }

  // Load the signature.
  if (signature.size() != kP256CurveOrderBytes * 2) {
    PW_LOG_DEBUG("Bad signature format");
    return Status::InvalidArgument();
  }

  if (!(BN_bin2bn(signature_bytes, kP256CurveOrderBytes, sig->r) &&
        BN_bin2bn(signature_bytes + kP256CurveOrderBytes,
                  kP256CurveOrderBytes,
                  sig->s))) {
    return Status::Internal();
  }

  // Digest must be 32 bytes or longer (and will be truncated).
  if (digest.size() < kP256CurveOrderBytes) {
    PW_LOG_DEBUG("Digest is too short");
    return Status::InvalidArgument();
  }

  // Verify the signature.
  if (!ECDSA_do_verify(digest_bytes, digest.size(), sig.get(), key.get())) {
    PW_LOG_DEBUG("Signature verification failed");
    return Status::Unauthenticated();
  }

  return OkStatus();
}

}  // namespace pw::crypto::ecdsa
