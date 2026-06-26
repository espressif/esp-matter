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

#include <credentials/CHIPCert.h>
#include <credentials/attestation_verifier/DeviceAttestationVerifier.h>
#include <lib/support/Span.h>

namespace chip {
namespace Credentials {

#ifdef CONFIG_DCL_REVOCATION_POINTS_REVOKED_DAC_CHAIN_CHECK
class dcl_revocation_point_da_revocation_delegate : public DeviceAttestationRevocationDelegate {
public:
    typedef enum {
        DCL_MAIN_NET,
        DCL_TEST_NET,
    } dcl_net_type_t;
    static constexpr size_t k_crl_url_max_len = 256;
    static constexpr size_t k_crl_max_len = 1200;

    dcl_revocation_point_da_revocation_delegate(dcl_revocation_point_da_revocation_delegate &other) = delete;
    void operator=(const dcl_revocation_point_da_revocation_delegate &) = delete;

    static dcl_revocation_point_da_revocation_delegate &get_instance()
    {
        static dcl_revocation_point_da_revocation_delegate instance;
        return instance;
    }

    void set_dcl_net_type(dcl_net_type_t net_type)
    {
        m_dcl_net_base_url = (net_type == DCL_MAIN_NET) ? k_main_net_base_url : k_test_net_base_url;
    }

    bool IsCertRevoked(const ByteSpan &certDer, bool isPAI);

    esp_err_t fetch_revocation_set_from_dcl(const ByteSpan &issuer_skid, MutableByteSpan &crl_signer_cert,
                                            MutableByteSpan &crl_signer_delegator, MutableCharSpan &crl_url);
    esp_err_t fetch_crl(const CharSpan &crl_url, MutableByteSpan &crl);

    void CheckForRevokedDACChain(
        const DeviceAttestationVerifier::AttestationInfo &info,
        Callback::Callback<DeviceAttestationVerifier::OnAttestationInformationVerification> *onCompletion) override;

private:
    static constexpr char *k_test_net_base_url = "https://on.test-net.dcl.csa-iot.org";
    static constexpr char *k_main_net_base_url = "https://on.dcl.csa-iot.org";
    const char *m_dcl_net_base_url = k_main_net_base_url;
    dcl_revocation_point_da_revocation_delegate() = default;
};
#endif // CONFIG_DCL_REVOCATION_POINTS_REVOKED_DAC_CHAIN_CHECK

#ifdef CONFIG_CUSTOM_REVOKED_DAC_CHAIN_CHECK
/** Set the custom device attestation revocation delegate for the DA revocation check
 *
 *  This API should be called before matter_controller_client::setup_commissioner(). Otherwise
 *  the commissioner will not check DA revocation during commissioning.
 *
 *  If passing nullptr for this function, the commissioner will not check DA revocation during commissioning.
 *
 * @param[in] delegate the custom DA revocation delegate
 */
void set_custom_da_revocation_delegate(DeviceAttestationRevocationDelegate *delegate);
#endif // CONFIG_CUSTOM_REVOKED_DAC_CHAIN_CHECK

/** Get the device attestation revocation delegate for the DA revocation check
 *
 * @return The device attestation revocation delegate used by the commissioner
 */
DeviceAttestationRevocationDelegate *get_da_revocation_delegate();

} // namespace Credentials
} // namespace chip
