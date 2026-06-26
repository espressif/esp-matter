// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <lib/support/Span.h>
#include <mbedtls/x509_crl.h>

using chip::ByteSpan;

namespace esp_matter {
namespace controller {
namespace attestation_verification {

/** This function will do the following validation for the DA Certificate.
 *
 *  If DA certificate is PAI, the subject and SKID of its issuer should match the subject of the CRLSignerCertificate
 *  or the CRLSignerCertificate's issuer.
 *  If DA certificate is DAC, the subject and SKID of its issuer should match the subject of the CRLSignerDelegator or
 *  the CRLSignerCertificate.
 *
 *  @param[in] is_pai Whether the DA certificate is PAI
 *  @param[in] da_cert The DA certificate in DER format
 *  @param[in] crl_signer_cert The CRLSignerCertificate in DER format
 *  @param[in] crl_signer_delegator The CRLSignerDelegator in DER format, could be empty.
 *
 *  @return Whether the validation is passed.
 */
bool cross_validate_cert(bool is_pai, const ByteSpan &da_cert, const ByteSpan &crl_signer_cert,
                         const ByteSpan &crl_signer_delegator);

/** This function will check whether the CRL is issued by the CRLSignerCertificate.
 *
 * @param[in] crl The MbedTLS parsed CRL
 * @param[in] crl_signer_cert The CRLSignerCertificate in DER format
 *
 * @return Whether the CRL is issued by the CRLSignerCertificate
 */
bool check_crl_signer_cert(const mbedtls_x509_crl *crl, const ByteSpan &crl_signer_cert);

/** This function will check the serialNumber of the DA certificate is in the CRL revocation sets.
 *
 * @param[in] da_cert The DA certificate in DER format
 * @param[in] crl The MbedTLS parsed CRL
 *
 * @return whether the serialNumber of the DA certificate is in the CRL revocation sets
 */
bool is_da_cert_serial_number_revoked(const ByteSpan &da_cert, const mbedtls_x509_crl *crl);
} // namespace attestation_verification
} // namespace controller
} // namespace esp_matter
