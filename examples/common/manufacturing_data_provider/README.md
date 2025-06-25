# ESP32 Manufacturing Data Provider

#### Overview

The ESP32ManufacturingDataProvider is a unified wrapper around ESP32's Factory Data Provider and Secure Cert Data Provider. It provides a flexible fallback mechanism for accessing manufacturing data from either the factory partition or the secure cert partition based on configurable precedence rules.

Example: Modules manufactured have data in the factory partition for manufacturing data, but now we are migrating to store the manufacturing data in the secure cert partition. We want to support both approaches in the same codebase.

#### How to use in your project

```cpp
#include <ESP32ManufacturingDataProvider.h>

{
// Create provider with default precedence (FactoryFirst)
// This checks the data in the factory partition first, and if it's not valid,
// it will check the data in the secure cert partition.
static chip::DeviceLayer::ESP32ManufacturingDataProvider provider;

// Set as the commissionable data provider using esp_matter functions
// Must be called before esp_matter::start()
set_custom_commissionable_data_provider(&provider);
set_custom_device_instance_info_provider(&provider);
}
```

NOTE: Please make sure that the `examples/common/manufacturing_data_provider` is added as the
source directory and include path in the `CMakeLists.txt` of the project.

#### How to change the precedence

Default precedence is `FactoryFirst`, so if you want to change it to `SecureCertFirst`, you can do so by passing the
precedence to the constructor.

```cpp
{
    using namespace chip::DeviceLayer;
    static ESP32ManufacturingDataProvider provider(ESP32ManufacturingDataProvider::Precedence::SecureCertFirst);

    // Set as the commissionable data provider using esp_matter functions
    // Must be called before esp_matter::start()
    set_custom_commissionable_data_provider(&provider);
    set_custom_device_instance_info_provider(&provider);
}
```

#### Configuration Requirements

Ensure both underlying providers are available by enabling these configs:

```
CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER=y
CONFIG_ENABLE_ESP32_DEVICE_INSTANCE_INFO_PROVIDER=y
CONFIG_SEC_CERT_DAC_PROVIDER=y
```
