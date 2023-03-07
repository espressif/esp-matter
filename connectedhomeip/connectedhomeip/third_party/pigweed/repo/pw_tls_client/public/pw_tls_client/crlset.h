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

namespace pw::tls_client::crlset {

// The following APIs will be used to verify the chains of certificates the TLS
// client builds. For example, for each certificate in the chain,
// IsCertificateBlocked() is used to check if it needs to be blocked. Then
// IsCertificateRevoked() is used to check if it is revoked by the issuer (the
// certificate one level up the chain). If either of them returns true, the
// chain should be rejected.

// Query whether a certificate needs to be blocked according to the hardcoded
// CRLSet. Callers need to provide the sha256 of the Subject Public Key Info
// (SPKI) of the certificate.
bool IsCertificateBlocked(ConstByteSpan certificate);

// Query whether a certificate is revoked by its issuer according to the
// hardcoded CRLSet. Callers need to provide the sha256 of the SPKI of the
// issuer (in |issuer|) and the serial number (as byte sequences) of the
// target certificate to query.
bool IsCertificateRevoked(ConstByteSpan issuer, ConstByteSpan serial);

}  // namespace pw::tls_client::crlset
