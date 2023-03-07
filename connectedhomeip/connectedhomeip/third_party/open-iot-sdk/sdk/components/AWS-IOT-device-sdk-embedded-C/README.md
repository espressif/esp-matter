# AWS-IOT-device-sdk-embedded-C

This component exposes the following libraries from the Aws IoT Device SDK:
 - `backoff_algorithm` : Utility library to calculate backoff period.
 - `CoreMQTT` : API implementing MQTT functionalities.
 - `CoreJSON`: API implementing JSON functionalities.
 - `CoreHTTP` : API implementing HTTP functionalities.

A transport interface named `iot-sdk-aws-network-manager` and based on the other
components provided by the Open IoT SDK is also available.

# Config

The configuration of `coreMQTT` and `coreHTTP` can be set by extending respectively
the `coreMQTT-config` and `coreHTTP-config`.

The application can add the compile time definition `MQTT_DO_NOT_USE_CUSTOM_CONFIG`
to `coreMQTT-config` or `HTTP_DO_NOT_USE_CUSTOM_CONFIG` to `coreHTTP-config` to
enable the default config.

A custom config can be provided in a file named `core_mqtt_config.h` or `core_http_config.h`.
This file must be visible on the path of the `coreMQTT-config` or `coreHTTP-config` targets.

## License

AWS-IOT-device-sdk-embedded-C is licensed under the MIT license.
