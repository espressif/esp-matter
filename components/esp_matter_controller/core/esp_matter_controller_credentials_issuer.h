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

#pragma once

#include <esp_check.h>
#include <esp_err.h>

#include <controller/ExampleOperationalCredentialsIssuer.h>
#include <controller/OperationalCredentialsDelegate.h>
#include <crypto/CHIPCryptoPAL.h>
#include <lib/core/CHIPPersistentStorageDelegate.h>
#include <lib/core/DataModelTypes.h>
#include <lib/core/NodeId.h>
#include <lib/support/Span.h>

namespace esp_matter {
namespace controller {
class credentials_issuer {
public:
    virtual ~credentials_issuer() {}

    /**
     * This function is used to initialize the Credentials Issuer, if needed.
     *
     * @param[in] storage A reference to the storage, where the Credentials Issuer can optionally use to access the
     * keypair in storage.
     * @return ESP_OK on success
     * @return error in case of failure.
     */
    virtual esp_err_t initialize_credentials_issuer(chip::PersistentStorageDelegate &storage) = 0;

    /**
     * This function is used to get the OperationalCredentialsDelegate which is used to generate NOC chains for End
     * Devices
     *
     * @return OperationalCredentialsDelegate of the Controller/Commissioner
     */
    virtual chip::Controller::OperationalCredentialsDelegate *get_delegate() = 0;

    /**
     * This function is used to Generate NOC Chain for the Controller/Commissioner.
     *
     * @param[in] nodeId   The desired NodeId for the generated NOC Chain - May be optional/unused in some
     * implementations.
     * @param[in] fabricId The desired FabricId for the generated NOC Chain - May be optional/unused in some
     * implementations.
     * @param[in] keypair  The desired Keypair for the generated NOC Chain - May be optional/unused in some
     * implementations.
     * @param[in,out] rcac  Buffer to hold the Root Certificate of the generated NOC Chain.
     * @param[in,out] icac  Buffer to hold the Intermediate Certificate of the generated NOC Chain.
     * @param[in,out] noc   Buffer to hold the Leaf Certificate of the generated NOC Chain.
     *
     * @return ESP_OK on success
     * @return error in case of failure.
     */
    virtual esp_err_t generate_controller_noc_chain(chip::NodeId node_id, chip::FabricId fabric,
                                                    chip::Crypto::P256Keypair &keypair, chip::MutableByteSpan &rcac,
                                                    chip::MutableByteSpan &icac, chip::MutableByteSpan &noc) = 0;
};

void set_custom_credentials_issuer(credentials_issuer *issuer);

credentials_issuer *get_credentials_issuer();

} // namespace controller
} // namespace esp_matter
