# Connect: Empty Example

## Introduction

The Connect Empty project is a boilerplate code for starting Connect stack-based
applications. It does not do anything except initializing the Connect stack and
only the absolutely necessary components are installed.

## Prerequisites

The provided method is applicable for any EFR32 device which supports Connect.

Further readings:

- [Connect Online Documentation](https://docs.silabs.com/connect-stack/latest/)
- [Connect Tutorial
  Series](https://community.silabs.com/s/article/connect-tutorial-series?language=en_US)

## Prepare the Application

Open the *Connect - SoC Empty* through Simplicity Studio project wizard.

Compile the code and download it to two (or optionally more) radio boards.

## Testing the Application

Since the application does not utilize CLI, LEDs, or even the radio, without
modification the only way to test the application is to debug the compiled code.

### Examples of Extending the Application

The first step to being able to establish communication between devices is to
configure the network.

If the network was configured previously, the network parameters can be
retrieved by the
[`emberNetworkInit()`](https://docs.silabs.com/connect-stack/latest/group-network-management#ga47f3cf943b5e9890fa016d871943c705)
API function.

Otherwise, the network must be configured to one of the three supported modes
(*(Extended) Star Mode* / *Direct Mode* / *MAC Mode*) using the network
management API calls (the order of the API calls below does not reflect the
modes):

- [`emberJoinCommissioned()`](https://docs.silabs.com/connect-stack/latest/group-network-management#gaf8bbf94c1a141948a2385316fe8caec1)
- [`emberJoinNetwork()`](https://docs.silabs.com/connect-stack/latest/group-network-management#gaa87bbe048e8e1c8adeb89c9962305f96)
- [`emberJoinNetworkExtended()`](https://docs.silabs.com/connect-stack/latest/group-network-management#ga261299d4f87e26f7ed76c9e8297f9bde)

Connect uses callbacks to notify the application about various events. One of
these events is the incoming message that can be retrieved by implementing the
[`emberAfIncomingMacMessageCallback()`](https://docs.silabs.com/connect-stack/latest/group-app-framework-common#ga33ea9e454ca4af7cc66e64a0f0203ccf).
Implementing this callback the application will be able to process the received
messages.

To send a message the application can call
[`emberMessageSend()`](https://docs.silabs.com/connect-stack/latest/group-message#ga13fe76cafa6f769497cc6a684c96de6e).

The preferred way of executing single or re-occurring operations is to create
events as described in the [event
documentation](https://docs.silabs.com/connect-stack/latest/group-event).

The above is not a comprehensive list of the Connect API rather a few frequently
used functions.

## Conclusion

Although the example does not do anything spectacular, it is an ideal starting
point for an application that exploits the Connect stack capabilities but the
implementation needs to start from scratch.

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
