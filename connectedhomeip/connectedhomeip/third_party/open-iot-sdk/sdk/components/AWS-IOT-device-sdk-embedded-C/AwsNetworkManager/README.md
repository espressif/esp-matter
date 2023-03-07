# IoT SDK AWS Network Manager

The component `iot-sdk-aws-network-manager` exposes a network interface that can
be consumed by the `coreMQTT` and `coreHTTP` library.

# Usage

To use the IoT SDK AWS Network Manager, the following variables should be defined in the application :

```c
// Configuration of the connection
static const AwsNetworkContextConfig_t networkConfig = {
    // configuration
};

// Callback invoked when the TLS connection has been successfully established
static int connect_mqtt(NetworkContext_t *ctx);

// Transport interface usable in coreMQTT and coreHTTP initialization functions
static TransportInterface_t transport = {
    .send = AwsNetwork_send,
    .recv = AwsNetwork_recv,
    .pNetworkContext = &network_context
};
```

The component's API can then be used to connect to AWS :

```c
// Initialize the network
AwsNetwork_init(&network_context, &networkConfig);

// Connect to the server
int res = AwsNetwork_connect(&network_context, connect_cb);
if (res) {
    // process error
}

// Operate on the connection using the coreMQTT or coreHTTP libraries

// Close the connection
AwsNetwork_close(&network_context);
```

# Connection

The connect operation uses the `backoffAlgorithm` library to schedule retry. If
the application requires additional processing to be done during the connection
process - for example connecting to the MQTT broker - it is recommended to make
this processing part of the _global_ connect. The callback passed to `AwsNetwork_connect`
fulfil that role.

# Limitation

`iot-sdk-aws-network-manager` is an easy entry point to set up a connection to the
AWS services using the Open IoT SDK features. However it does not pretend to cover
all the existing use cases and configuration required to connect to the AWS cloud.
For example, plain text connection is not supported.

## License

Files are licensed under the Apache 2.0 license.
