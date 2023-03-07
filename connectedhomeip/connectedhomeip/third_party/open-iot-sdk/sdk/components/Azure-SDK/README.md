# Azure IoT SDK C

This component pulls in the Azure IoT C SDK and related libraries.
The Azure IOT C SDK allows applications written in C99 or later or C++ to communicate easily with Azure IoT Hub, Azure IoT Central and to Azure IoT Device Provisioning.

Refer to the [Azure IoT SDK C](https://github.com/Azure/azure-iot-sdk-c) for more information.

## Dependencies

Azure IoT SDK C requires others components to build:
 - CMSIS RTOS API
 - CMSIS SOCKET API
 - LwIP sockets
 - IoT SDK IP Network API
 - Azure C shared utility
 - NTP client (optional)

Before building makes sure that these targets are accessible.

<a name="config"> </a>
## Config

This component defines its own libraries based on Azure IoT SDK sources.
It uses mbedtls as TLS cryptography library and MQTT transport layer.

### Mbedtls

Depending on the mbedtls configuration, it may be required to provide some additional functions and settings.
Refer to the [Mbedtls](../mbedtls/README.md) for more information.

### time()

The Azure SDK requires the implementation of `time_t time(time_t *dst)` function. It's called by other components such as Mbedtls or IoT Hub client which require real system time.

The user can provide their own implementation and link it to `azure-iot-sdk-platform` target.

Example:
```
target_include_directories(azure-iot-sdk-platform
    INTERFACE
        custom-time-support-library
)
```

The Azure SDK component contains NTP time library implementation that provides setting real value of time in the system by time() function. The NTP client start and update time value every 30s. Link `azure-sdk-ntp-time` library to `azure-iot-sdk-platform` target to enable NTP time support.
```
target_link_libraries(azure-iot-sdk-platform
    PUBLIC
        azure-sdk-ntp-time
)
```

### IoT Hub

Depending on the library components used, it may be required to provide the configuration file to the application.

The user can create a configuration file specified and includes its path to main target.

Example:

```
target_include_directories(my-target
    INTERFACE
        iothub-config
)
```

The configuration file can contains parameters such as:
 - *IoT Hub X509 certificate* - it's required to handshake with cloud server
 - *IoT Hub connection string* - contains IoT Hub credentials
 - *IoT Hub device X509* certificate and private key - device authorization via signed X509 certificate

A sample configuration can be found in `examples/azure-sdk/iothub-config/iothub_config.h`

### Logging

The Azure SDK logging support can be enabled by setting `IOTSDK_AZURE_SDK_LOGGING=ON` in project Cmake configuration. Then provide logging callback function and pass it to Azure SDK by `xlogging_set_log_function()`. This allows redirect logs from the component to the right output stream.

Example:
```c
#include "azure_c_shared_utility/xlogging.h"
...

static void iothub_logging(
    LOG_CATEGORY log_category, const char *file, const char *func, int line, unsigned int options, const char *fmt, ...)
{
    printf(...);
}
...

int main(void)
{
    ...
    xlogging_set_log_function(iothub_logging);
    ...
}
```

## Usage

Applications must link against `azure-iothub-client`.
To use provisioning instead of direct endpoint connection the application must also link against
`azure-provisioning-client`.

Using some Azure IoT SDK functionalities will require prior configuration. See [Config](#config) chapter.

### IoT Hub base

Use `IoTHub_Init()` function to start working with Azure client.
This setup Open IoT SDK platform and initialise all necessary resources.
`IoTHub_Deinit()` close IoT Hub platform implementation and free resources.

Example:

```c
#include "iothub.h"

...
{
    int res = IoTHub_Init();
    if (res) {
        return res;
    }

    ....

    IoTHub_Deinit();
}
```

### IoT Hub client

After IoT Hub initialization user can start working with the client API (`iothub_device_client_ll.h`). It provides functions to establish a connection and communicate with the IoT Hub service.

Example:

```c
#include "iothub_client_options.h"
#include "iothub_device_client_ll.h"
#include "iothub_message.h"
#include "iothubtransportmqtt.h"


static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void *ctx)
{
    ((void)ctx);

    printf("IoT Hub connection status %d", result);
}

...

{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (!iotHubClientHandle) {
        printf("IoTHubDeviceClient_LL_CreateFromConnectionString failed\r\n");
        return 1;
    }

    IoTHubDeviceClient_LL_SetOption(iotHubClientHandle, OPTION_TRUSTED_CERT, certificates);

    bool urlEncodeOn = true;
    IoTHubDeviceClient_LL_SetOption(iotHubClientHandle, OPTION_AUTO_URL_ENCODE_DECODE, &urlEncodeOn);

    IoTHubDeviceClient_LL_SetConnectionStatusCallback(iotHubClientHandle, connection_status_callback, NULL);

    ...

    IoTHubDeviceClient_LL_DoWork(iotHubClientHandle);

    ...

    IoTHubDeviceClient_LL_Destroy(iotHubClientHandle);
}
```

__NOTE:__

Simple Azure SDK usage can be found in `examples/azure-sdk/main.c`

## License

The Azure IoT C SDK is released under the MIT open source license.
