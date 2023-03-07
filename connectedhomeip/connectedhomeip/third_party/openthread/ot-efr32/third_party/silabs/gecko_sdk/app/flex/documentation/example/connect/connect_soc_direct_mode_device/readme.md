# Connect: Direct Mode Device

## Introduction

The Connect Direct Mode Device example pair demonstrates how to set up a simple
communication link between two or more devices. In *direct mode* there is no
central authority that coordinates the devices, assigns addresses, or routes the
messages between the participants - this means if those features are required,
they must be implemented at the application level. *Direct mode* supports
connections between devices that are in the range of each other. This mode is
suitable for proprietary applications that prefer more complete control of
network behavior.

## Prerequisites

The provided examples are applicable for any EFR32 device which supports
Connect.

Further readings:

- [Connect Online Documentation](https://docs.silabs.com/connect-stack/latest/)
- [Connect Tutorial
  Series](https://community.silabs.com/s/article/connect-tutorial-series?language=en_US)

## Prepare the Application

Open the *Connect - SoC Direct Mode Device* example through Simplicity
Studio project wizard.

Compile the projects and download the firmware image to two devices.

## Network Establishment

Note: before any further steps it is recommended to issue the `leave` command on
all devices that deconfigures the devices (clears their network settings).

First, the network parameters must be commissioned. Since there is no central
(*coordinator*) node in the network who is responsible for assigning and
accounting the addresses of the devices every device must ensure to set a unique
address and the corresponding PAN ID:

```txt
> commission 0x0001 0x01ff 0 0
```

The parameters of the command are the

- node ID
- PAN ID
- channel
- transmit power

Node ID must be uniquely chosen for every device in the same PAN and only
devices that are in the same PAN and are on the same channel can communicate
with each other.

Note: Connect devices store their network parameters in a non-volatile memory
area (NVM) and the Direct Mode Device application exploits this feature that
means no network configuration is necessary after every power cycle or reset.

Once the network parameters are commissioned the device is ready to send and
receive messages. Issue the `commission` command on every node.

The `info` command can be used to check the network parameters. If the network
is configured the output should be similar to the example below:

```txt
> info
Info:
         MCU Id: 0xB57FFFEB59FA4
  Network state: 0x02
      Node type: 0x05
        Node id: 0x0001
         Pan id: 0x01FF
        Channel: 0
          Power: 0
     TX options: MAC acks enabled, security disabled, priority disabled
>
```

To de-configure the network parameters, issue the `leave` command.

## Communication Between Devices

To test the communication between any two parties issue the `data` command:

```txt
> data 0x0002 {AABBCCDD}
TX: Data to 0x0002:{AA BB CC DD}: status=0x00
> 

```

Assuming a device exists with node ID 0x0002, the message should be received by
that node:

```txt
> RX: Data from 0x0001:{AA BB CC DD}
```

Any node can send messages to any other node that has been correctly configured
and that is in range.

## Additional Helper Commands

### Channel Selection

It is possible to change the radio channel when the network parameters are
already commissioned.

```txt
> set_channel 10
Radio channel set, status=0x00
>
```

Devices must be on the same channel to successfully communicate. The number of
possible channels depends on the configured PHY. PHYs can be configured through
Simplicity Studio's Radio Configurator.

### Security

To enable communicating with encrypted messages first a network key should be
specified:

```txt
> set_key {00112233445566778899aabbccddeeff}
```

The above command echoes back the key if it is applied:

```txt
Security key set: {00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF}
```

The encryption key must match on all devices to successfully receive encrypted
messages.

To enable encryption use

```txt
> set_tx_options 0x03
```

The command will print the current values:

```txt
TX options set: MAC acks enabled, security enabled, priority disabled
```

The parameter is a bitfield: `bit 1` specifies whether the message will be sent
encrypted (`1`) or unencrypted (`0`), `bit 0` controls whether acknowledge is
required (`1`) for the transmitted message or not (`0`) so, `0x03` means
security and acknowledge are both enabled.

Note: `set_tx_options` controls only the encryption of the message to transmit.

On the receive side, the messages will be accepted (forwarded to the application
code) if:

- the transmitted message is not encrypted
- the transmitted message is encrypted with the key same as applied on the
  receiver side

## Controlling the Radio State

Connect Direct Mode allows the application to directly disable or enable the
radio operation. To lower the power consumption it is possible to turn off the
radio when neither transmission is in progress nor incoming messages are
expected.

```txt
> toggle_radio 0  
Radio is turned OFF
> toggle_radio 1
Radio is turned ON
> 
```

Note: when the radio is off the device will not receive messages, but
transmission automatically enables the radio for the transmission period then
turns it off.

## Additional Test Features

### Energy Scan

The device can measure the RSSI value for a given number of samples on the
specified channel and report the value of the energy level:

```txt
> start_energy_scan 0 100
Start energy scanning: channel 0, samples 100
> Energy scan complete, mean=-83 min=-87 max=-79 var=1
```

### Sending Multiple Messages to a Destination Node

To test the communication with multiple messages frequently transmitted use the
`tx_test` command, for example:

```txt
> tx_test 10 5 0x0001
```

The destination node should receive the messages sent by the transmitter and if
the printing is enabled it will display the incoming messages:

```txt
RX: TX test packet from 0x0002:{ 00 01 02 03 04 05 06 07 08 09}
RX: TX test packet from 0x0002:{ 00 01 02 03 04 05 06 07 08 09}
RX: TX test packet from 0x0002:{ 00 01 02 03 04 05 06 07 08 09}
RX: TX test packet from 0x0002:{ 00 01 02 03 04 05 06 07 08 09}
RX: TX test packet from 0x0002:{ 00 01 02 03 04 05 06 07 08 09}
```

Printing of the messages can be controlled by the `tx_test_print_enable` command
(`0` (default) means disabled, `1` means enabled).

### Accounting Connect Stack Events

Counters of various Connect events can be retrieved by the `counter` command
passing the counter type as the argument. For details of the stack counters see
the [Stack
Counters](https://docs.silabs.com/connect-stack/latest/group-stack-counters)
section of the documentation.

### Restart the Device

The command `reset` restarts the device.

## Conclusion

Connect Direct Mode is an easy way to build ad-hoc networks where the routing
function and automatic address assignment are not a requirement or these
functions planned to implement at the application level.

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
