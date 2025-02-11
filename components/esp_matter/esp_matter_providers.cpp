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

#include <credentials/examples/DeviceAttestationCredsExample.h>
#include <esp_log.h>
#include <esp_matter_providers.h>
#include <platform/ESP32/ESP32DeviceInfoProvider.h>
#include <platform/ESP32/ESP32FactoryDataProvider.h>
#include <platform/ESP32/ESP32SecureCertDACProvider.h>

using namespace chip::DeviceLayer;
using namespace chip::Credentials;

constexpr char TAG[] = "esp_matter_providers";

namespace esp_matter {

#if CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER
static ESP32FactoryDataProvider factory_data_provider;
#endif

#if CONFIG_ENABLE_ESP32_DEVICE_INFO_PROVIDER
static ESP32DeviceInfoProvider device_info_provider;

#endif

#if CONFIG_CUSTOM_DAC_PROVIDER
static DeviceAttestationCredentialsProvider *s_custom_dac_provider = NULL;

void set_custom_dac_provider(DeviceAttestationCredentialsProvider *provider)
{
    s_custom_dac_provider = provider;
}
#endif

#if CONFIG_CUSTOM_COMMISSIONABLE_DATA_PROVIDER
static CommissionableDataProvider *s_custom_commissionable_data_provider = NULL;

void set_custom_commissionable_data_provider(CommissionableDataProvider *provider)
{
    s_custom_commissionable_data_provider = provider;
}
#endif

#if CONFIG_CUSTOM_DEVICE_INSTANCE_INFO_PROVIDER
static DeviceInstanceInfoProvider *s_custom_device_instance_info_provider = NULL;

void set_custom_device_instance_info_provider(DeviceInstanceInfoProvider *provider)
{
    s_custom_device_instance_info_provider = provider;
}
#endif

#if CONFIG_CUSTOM_DEVICE_INFO_PROVIDER
static DeviceInfoProvider *s_custom_device_info_provider = NULL;

void set_custom_device_info_provider(DeviceInfoProvider *provider)
{
    s_custom_device_info_provider = provider;
}
#endif

DeviceAttestationCredentialsProvider *get_dac_provider(void)
{
#if CONFIG_SEC_CERT_DAC_PROVIDER
    static ESP32SecureCertDACProvider instance;
    return &instance;
#elif CONFIG_FACTORY_PARTITION_DAC_PROVIDER
    return &factory_data_provider;
#elif CONFIG_CUSTOM_DAC_PROVIDER
    if (s_custom_dac_provider) {
        return s_custom_dac_provider;
    } else {
        ESP_LOGE(TAG, "custom_dac_provider cannot be NULL");
    }
#elif CONFIG_EXAMPLE_DAC_PROVIDER
    return chip::Credentials::Examples::GetExampleDACProvider();
#endif
    return NULL;
}

void setup_providers()
{
#if CONFIG_FACTORY_COMMISSIONABLE_DATA_PROVIDER
    SetCommissionableDataProvider(&factory_data_provider);
#elif CONFIG_CUSTOM_COMMISSIONABLE_DATA_PROVIDER
    if (s_custom_commissionable_data_provider) {
        SetCommissionableDataProvider(s_custom_commissionable_data_provider);
    }
#endif
    // We should do nothing if CONFIG_EXAMPLE_COMMISSIONABLE_DATA_PROVIDER is enabled as
    // LegacyTemporaryCommissionableDataProvider is set in GenericConfigurationManagerImpl<ConfigClass>::Init()

#if CONFIG_FACTORY_DEVICE_INSTANCE_INFO_PROVIDER
    SetDeviceInstanceInfoProvider(&factory_data_provider);
#elif CONFIG_CUSTOM_DEVICE_INSTANCE_INFO_PROVIDER
    if (s_custom_device_instance_info_provider) {
        SetDeviceInstanceInfoProvider(s_custom_device_instance_info_provider);
    }
#endif
    // We should do nothing if CONFIG_EXAMPLE_DEVICE_INSTANCE_INFO_PROVIDER is enabled as
    // GenericDeviceInstanceInfoProvider is set in GenericConfigurationManagerImpl<ConfigClass>::Init()

#if CONFIG_FACTORY_DEVICE_INFO_PROVIDER
    SetDeviceInfoProvider(&device_info_provider);
#elif CONFIG_CUSTOM_DEVICE_INFO_PROVIDER
    if (s_custom_device_info_provider) {
        SetDeviceInfoProvider(s_custom_device_info_provider);
    }
#endif
    SetDeviceAttestationCredentialsProvider(get_dac_provider());
}

} // namespace esp_matter
