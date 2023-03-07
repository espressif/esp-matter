# Connect:  Light DMP and Switch examples

## Introduction

The Connect Empty Example DMP and the Switch Example projects are described in one document as those are counterparts of each other. The Light DMP example is capable to receive the toggle command from both proprietary and BLE devices. The Switch example implements the proprietary communication with the Light DMP application. It is also possible to control the Light DMP application using Bluetooth with the official [EFR Connect](https://www.silabs.com/developers/efr-connect-mobile-app) application.

## Prerequisites

The provided method is applicable for any EFR32 device which supports Proprietary protocols in the case of the Switch example and Bluetooth **and** Proprietary protocols for the Light DMP example application.

Further readings:

- [Connect Online Documentation](https://docs.silabs.com/connect-stack/latest/)
- [Bluetooth Online Documentation](https://docs.silabs.com/bluetooth/latest/)
- [Connect Tutorial Series](https://community.silabs.com/s/article/connect-tutorial-series?language=en_US)

## Prepare the application

Open the *Connect Bluetooth DMP - SoC Light* through Simplicity Studio project wizard.

Compile the code and download it to one radio board.

Open the *Connect - SoC Switch* through Simplicity Studio project wizard.

Compile the code and download it to one or more radio board(s).

## Testing the application

The proprietary side of the communication can be tested by issuing commands on CLI of the Switch example (once the Switch is connected to the Light DMP example application it is also possible to toggle LED0 on the Light DMP's mainboard (BRD4001A) by pushing BTN1 of the Switch's mainboard).

To test the Bluetooth functions a BLE-capable device is necessary (a client that can connect to the Connect Empty Example DMP (Bluetooth server) application) for example a mobile phone that supports BLE).

## Network establishment

Note: before any further steps it is recommended to issue the `leave` command on all devices that deconfigures the devices (clears their network settings).

First, a network must be formed. This requires issuing the `form` command on the Light DMP device:

```txt
> form
network formed, radio_channel: 0 PAN ID: 0xBEEF
```

The next step is to join the Light DMP device with the Switch. This requires issuing the `join` command on the switch device:

```txt
> join                                                                          
join sleepy to the network on channel: 0, PAN ID: 0xBEEF                      
Network up                                                                      
```

If the connection is established successfully, the *Network up* message should appear on the CLI. Otherwise, if the join process is failed an error message will be shown, for example:

```txt
Stack status: 0xAB
```

The LED state of the Light DMP device can be toggled by issuing the following command on the Switch device:

```txt
> toggle_light                                                                  
TX: Data to 0x0000:
```

If the message is received the Light DMP application will respond:

```txt
Toggle message from node: 1, light is on
```

## Additional commands

### Network parameters

The channel is set to `0` by default but it can be overwritten by a CLI command:

```txt
> set_channel 1
```

Similarly, the PAN ID has a default value (`0xbeef`) it can be changed:

```txt
> set_pan_id 0x01ff
```

Note: re-configuring the channel or the PAN ID must be executed before forming the network / joining the network. If the network is already established the `leave` command must be issued then the `form` and/or `join` commands.

### Network information

The `info` command can be used to check the network parameters. If the network is configured the output should be similar to the example below:

```txt
> info
Info:
         MCU Id: 0xB57FFFEB59FA4
  Network state: 0x02
      Node type: 0x05
        Node id: 0x0001
         Pan id: 0xBEEF
        Channel: 0
          Power: 0
     TX options: MAC acks enabled, security disabled, priority disabled
>
```

### Restart the device

The command `reset` restarts the device.

## Notes

Take extra care when designing DMP applications as not all protocols are compatible with each other. Since the EFR32 devices have a single radio simultaneous transmit or receiving on different channels / with different PHYs are not possible, one protocol uses the radio exclusively, furthermore, the hardware needs time to switch between protocols.

Regarding DMP in general please navigate to [UG305: Dynamic Multiprotocol User’s
Guide](https://www.silabs.com/documents/public/user-guides/ug305-dynamic-multiprotocol-users-guide.pdf).

Describing Bluetooth details is out of the scope of this document. For Bluetooth please consult Silicon Labs Bluetooth materials.

## Conclusion

The example demonstrates the ease of exploiting the Dynamic Multi-Protocol capability of the EFR32 device using the Connect and the BLE stacks beside each other.
