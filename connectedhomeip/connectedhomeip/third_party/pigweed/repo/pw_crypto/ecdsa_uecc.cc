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
#define PW_LOG_MODULE_NAME "ECDSA-UECC"
#define PW_LOG_LEVEL PW_LOG_LEVEL_WARN

#include "pw_crypto/ecdsa.h"
#include "pw_log/log.h"
#include "uECC.h"

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

  uECC_Curve curve = uECC_secp256r1();

  // Supports SEC 1 uncompressed form (04||X||Y) only.
  if (public_key.size() != (2 * kP256CurveOrderBytes + 1) ||
      public_key_bytes[0] != 0x04) {
    PW_LOG_DEBUG("Bad public key format");
    return Status::InvalidArgument();
  }

  // Make sure the public key is on the curve.
  if (!uECC_valid_public_key(public_key_bytes + 1, curve)) {
    return Status::InvalidArgument();
  }

  // Signature expected in raw format (r||s)
  if (signature.size() != kP256CurveOrderBytes * 2) {
    PW_LOG_DEBUG("Bad signature format");
    return Status::InvalidArgument();
  }

  // Digests must be at least 32 bytes. Digests longer than 32
  // bytes are truncated to 32 bytes.
  if (digest.size() < kP256CurveOrderBytes) {
    PW_LOG_DEBUG("Digest is too short");
    return Status::InvalidArgument();
  }

  // Verify the signature.
  if (!uECC_verify(public_key_bytes + 1,
                   digest_bytes,
                   digest.size(),
                   signature_bytes,
                   curve)) {
    PW_LOG_DEBUG("Signature verification failed");
    return Status::Unauthenticated();
  }

  return OkStatus();
}

}  // namespace pw::crypto::ecdsa
