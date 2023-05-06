// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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
#include <platform/CommissionableDataProvider.h>
#include <platform/DeviceInfoProvider.h>
#include <platform/DeviceInstanceInfoProvider.h>
#include <sdkconfig.h>

namespace esp_matter {

/**
 * @brief Set the custom DAC provider
 *
 *      This should be called before esp_matter::start().
 */
#if CONFIG_CUSTOM_DAC_PROVIDER
void set_custom_dac_provider(chip::Credentials::DeviceAttestationCredentialsProvider *provider);
#endif

/**
 * @brief Set the custom commissionable data provider
 *
 *      This should be called before esp_matter::start().
 */
#if CONFIG_CUSTOM_COMMISSIONABLE_DATA_PROVIDER
void set_custom_commissionable_data_provider(chip::DeviceLayer::CommissionableDataProvider *provider);
#endif

/**
 * @brief Set the custom device instance info provider
 *
 *      This should be called before esp_matter::start().
 */
#if CONFIG_CUSTOM_DEVICE_INSTANCE_INFO_PROVIDER
void set_custom_device_instance_info_provider(chip::DeviceLayer::DeviceInstanceInfoProvider *provider);
#endif

/**
 * @brief Set the custom device info provider
 *
 *      This should be called before esp_matter::start().
 */
#if CONFIG_CUSTOM_DEVICE_INFO_PROVIDER
void set_custom_device_info_provider(chip::DeviceLayer::DeviceInfoProvider *provider);
#endif

void setup_providers();
} // namespace esp_matter
