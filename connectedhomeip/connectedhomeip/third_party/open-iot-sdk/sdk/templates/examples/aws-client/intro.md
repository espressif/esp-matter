This example demonstrates the use of the `coreMQTT` library from the AWS IOT SDK.

The example connects to the AWS server, publish the message `Hello World!` in the
MQTT topic `OpenIoTSDK/demo`. It wait for the acknowledgement from the server and
release the connection.

The network management part is provided by the library `iot-sdk-aws-network-manager`
which establish the TCP connection to the AWS server then encrypts it. This library
is built on top of other building blocks present in this SDK.

## Configuration

AWS credentials must be filled in to access the AWS service.
These credentials are displayed in the files:
- [aws-credentials/aws_clientcredential.h](aws-credentials/aws_clientcredential.h)
- [aws-credentials/aws_clientcredential_keys.h](aws-credentials/aws_clientcredential_keys.h)

The application specifically requires:
- `clientcredentialMQTT_BROKER_ENDPOINT`: fully-qualified DNS name of your MQTT broker
- `clientcredentialMQTT_BROKER_PORT`: MQTT broker port
- `keyCA1_ROOT_CERTIFICATE_PEM`: AWS root certificate
- `keyCLIENT_CERTIFICATE_PEM`: PEM-encoded client certificate.
- `keyCLIENT_PRIVATE_KEY_PEM`: PEM-encoded client private key.
- `keyCLIENT_PUBLIC_KEY_PEM`: PEM-encoded client public key.
- `clientcredentialIOT_THING_NAME`: Name of the IoT thing.
