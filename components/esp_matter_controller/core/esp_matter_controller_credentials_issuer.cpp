// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_check.h>
#include <esp_err.h>
#include <esp_matter_controller_credentials_issuer.h>

namespace esp_matter {
namespace controller {

class example_credentials_issuer : public credentials_issuer {
    esp_err_t initialize_credentials_issuer(chip::PersistentStorageDelegate &storage) override
    {
        return m_operational_creds_issuer.Initialize(storage) == CHIP_NO_ERROR ? ESP_OK : ESP_FAIL;
    }

    chip::Controller::OperationalCredentialsDelegate *get_delegate() override { return &m_operational_creds_issuer; }

    esp_err_t generate_controller_noc_chain(chip::NodeId node_id, chip::FabricId fabric_id,
                                            chip::Crypto::P256Keypair &keypair, chip::MutableByteSpan &rcac,
                                            chip::MutableByteSpan &icac, chip::MutableByteSpan &noc) override
    {
        CHIP_ERROR err = m_operational_creds_issuer.GenerateNOCChainAfterValidation(
            node_id, fabric_id, chip::kUndefinedCATs, keypair.Pubkey(), rcac, icac, noc);
        return err == CHIP_NO_ERROR ? ESP_OK : ESP_FAIL;
    }

private:
    chip::Controller::ExampleOperationalCredentialsIssuer m_operational_creds_issuer;
};

static credentials_issuer *s_custom_credentials_issuer = nullptr;

void set_custom_credentials_issuer(credentials_issuer *issuer)
{
    s_custom_credentials_issuer = issuer;
}

credentials_issuer *get_credentials_issuer()
{
#ifdef CONFIG_TEST_OPERATIONAL_CREDS_ISSUER
    static example_credentials_issuer s_creds_issuer;
    return &s_creds_issuer;
#elif defined(CONFIG_CUSTOM_OPERATIONAL_CREDS_ISSUER)
    return s_custom_credentials_issuer;
#endif
    return nullptr;
}

} // namespace controller
} // namespace esp_matter
