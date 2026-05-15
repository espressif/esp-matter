// Copyright 2023-2025 Espressif Systems (Shanghai) PTE LTD
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

#include <credentials/CHIPCert.h>
#include <credentials/attestation_verifier/DeviceAttestationVerifier.h>
#include <dirent.h>
#include <esp_err.h>
#include <lib/support/IntrusiveList.h>

namespace chip {
namespace Credentials {

#ifdef CONFIG_SPIFFS_ATTESTATION_TRUST_STORE
typedef struct paa_der_cert {
    uint8_t m_buffer[kMaxDERCertLength] = {0};
    size_t m_len = 0;
} paa_der_cert_t;

class paa_der_cert_iterator {
public:
    paa_der_cert_iterator(const char *path);
    ~paa_der_cert_iterator()
    {
        release();
    };
    size_t count()
    {
        return m_count;
    }
    bool next(paa_der_cert_t &item);
    void release();

private:
    DIR *m_dir = NULL;
    char m_path[16] = {0};
    size_t m_count = 0;
    size_t m_index = 0;
};

class spiffs_attestation_trust_store : public AttestationTrustStore {
public:
    spiffs_attestation_trust_store(spiffs_attestation_trust_store &other) = delete;
    void operator=(const spiffs_attestation_trust_store &) = delete;

    static spiffs_attestation_trust_store &get_instance()
    {
        static spiffs_attestation_trust_store instance;
        return instance;
    }

    CHIP_ERROR GetProductAttestationAuthorityCert(const ByteSpan &skid,
                                                  MutableByteSpan &outPaaDerBuffer) const override;

    esp_err_t init();

private:
    bool m_is_initialized = false;
    spiffs_attestation_trust_store() {}
};
#endif // CONFIG_SPIFFS_ATTESTATION_TRUST_STORE

#ifdef CONFIG_DCL_ATTESTATION_TRUST_STORE
class dcl_attestation_trust_store : public AttestationTrustStore {
public:
    typedef enum {
        DCL_MAIN_NET,
        DCL_TEST_NET,
    } dcl_net_type_t;

    dcl_attestation_trust_store(dcl_attestation_trust_store &other) = delete;
    void operator=(const dcl_attestation_trust_store &) = delete;

    static dcl_attestation_trust_store &get_instance()
    {
        static dcl_attestation_trust_store instance;
        return instance;
    }

    CHIP_ERROR GetProductAttestationAuthorityCert(const ByteSpan &skid,
                                                  MutableByteSpan &outPaaDerBuffer) const override;

    void SetDCLNetType(dcl_net_type_t type)
    {
        m_dcl_net_base_url = (type == DCL_MAIN_NET) ? k_main_net_base_url : k_test_net_base_url;
    }

private:
    static constexpr char *k_test_net_base_url = "https://on.test-net.dcl.csa-iot.org";
    static constexpr char *k_main_net_base_url = "https://on.dcl.csa-iot.org";
    const char *m_dcl_net_base_url = k_main_net_base_url;
    dcl_attestation_trust_store() {}
};
#endif // CONFIG_DCL_ATTESTATION_TRUST_STORE

#ifdef CONFIG_CUSTOM_ATTESTATION_TRUST_STORE
/** Set the custom attestation trust store for device attestation verification
 *
 *  This API MUST be called before matter_controller_client::setup_commissioner(). Otherwise
 *  the commissioner setup will fail.
 *
 *  If passing nullptr for this function, the commissioner setup will fail.
 *
 * @param[in] store the custom attestation trust store
 */
void set_custom_attestation_trust_store(AttestationTrustStore *store);
#endif // CONFIG_CUSTOM_ATTESTATION_TRUST_STORE

/** Get the device attestation trust store for DA verification
 *
 * @return the device attestation trust store used by the commissioner
 */
const AttestationTrustStore *get_attestation_trust_store();

} // namespace Credentials
} // namespace chip
