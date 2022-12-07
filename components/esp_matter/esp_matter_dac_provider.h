// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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

#include <credentials/DeviceAttestationCredsProvider.h>

namespace esp_matter {

/**
 * @brief Get the DAC Provider implementation based on the configuration option
 *
 *        If CONFIG_EXAMPLE_DAC_PROVIDER option is enabled then implementation which uses test data is used.
 *
 *        If CONFIG_FACTORY_PARTITION_DAC_PROVIDER option is enabled then implementation which reads attestation
 *        information from factory partition is used.
 *
 *        If CONFIG_SEC_CERT_DAC_PROVIDER option is enabled then implementation which reads attestation information
 *        from the esp_secure_cert partition is used.
 *
 * @return Pointer to the object of type DeviceAttestationCredentialsProvider
 */
chip::Credentials::DeviceAttestationCredentialsProvider * get_dac_provider(void);

} // namespace esp_matter
