/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <json_parser.h>

#include <credentials/attestation_verifier/DeviceAttestationVerifier.h>

namespace chip {
namespace Credentials {

class json_set_da_revocation_delegate : public DeviceAttestationRevocationDelegate {
public:
    json_set_da_revocation_delegate(const char *revocation_json_set_start, const char *revocation_json_set_end)
        : m_revocation_json_set_start(revocation_json_set_start)
        , m_revocation_json_set_end(revocation_json_set_end)
        , m_revocation_json_set_len(revocation_json_set_end - revocation_json_set_start) {}
    ~json_set_da_revocation_delegate() = default;
    void CheckForRevokedDACChain(
        const DeviceAttestationVerifier::AttestationInfo &info,
        Callback::Callback<DeviceAttestationVerifier::OnAttestationInformationVerification> *onCompletion) override;

private:
    bool IsCertRevoked(const ByteSpan &certDer, bool isPAI);
    const char *m_revocation_json_set_start;
    const char *m_revocation_json_set_end;
    const size_t m_revocation_json_set_len;
};

} // namespace Credentials
} // namespace chip
