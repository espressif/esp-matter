# Connect: Sink/Sensor Network

## Introduction

The Connect Sink/Sensor example pair demonstrates how to set up a simple Connect
Star or Connect Extended Star network. A Connect network consist of one
*coordinator*, optional *range extender(s)* and *end-device(s)*, or *sleepy
end-device(s)*. The Connect stack does not support real mesh networking
*end-devices* can join to the *coordinator* or a *range extender* while a *range
extender* can join only to the *coordinator*. When the network is established
any device can send messages to any other device (including the *coordinator*,
*range extenders* or *end-devices*). The Connect stack uses static routing, once
the devices are joined the only way to reconfigure the routes is to leave the
network and re-join again.

Besides the basic functions, the Sink/Sensor example pair is capable to
demonstrate the security feature of the Connect stack.

## Prerequisites

The provided examples are applicable for any EFR32 device which supports
Connect.

Further readings:

- [Connect Online Documentation](https://docs.silabs.com/connect-stack/latest/)
- [Connect Tutorial
  Series](https://community.silabs.com/s/article/connect-tutorial-series?language=en_US)

## Prepare the Application

Open the *Connect - SoC Sensor* and *Connect - SoC Sink* examples
through Simplicity Studio project wizard.

Compile both projects and download Sink (acts as *coordinator*) to one device
and Sensor (acts as *end-device*, *sleepy end-device* or *range extender*) to
one or more radio boards.

## Network Establishment

Note: before any further steps it is recommended to issue the `leave` command on
all devices that deconfigures the devices (clears their network settings).

First, a network must be formed. This requires issuing the form command on the
Sink device:

```txt
form 0
```

The parameter of the command is the channel number. The channel number must be
the same for all devices in the network.

By default, the *coordinator* does not allow devices to join. The permit join
command must be issued to enable joining:

```txt
pjoin 255
```

The parameter is the duration until the coordinator accepts joining requests.
The special value `0` disables joining while `255` means no join timeout.

Note: Connect devices store their network parameters in a non-volatile memory
area (NVM) and the Sink and Sensor applications exploit this feature that means
no network configuration is necessary after every power cycle or reset. However,
the permit join state must be set on every restart.

If the *coordinator* established the network other devices can join. The
simplest example is to join as a normal *end-device* on a device with the Sensor
app loaded:

```txt
join 0
```

The parameter specifies the radio channel on which the device tries to connect
to the network. Once the join process is successfully completed the Sensor node
starts sending the temperature and humidity values to the Sink and informational
messages displayed on both ends:

Sensor

```txt
TX: Data to 0x0000: C8 6B 00 00 93 C5 00 00: 0x00
TX: Data to 0x0000: D3 6B 00 00 84 C5 00 00: 0x00
TX: Data to 0x0000: E9 6B 00 00 84 C5 00 00: 0x00
```

Sink:

```txt
RX: Data from 0x0001: C8 6B 00 00 93 C5 00 00 Temperature: 27.592C Humidity: 50.579%
RX: Data from 0x0001: D3 6B 00 00 84 C5 00 00 Temperature: 27.603C Humidity: 50.564%
RX: Data from 0x0001: E9 6B 00 00 84 C5 00 00 Temperature: 27.625C Humidity: 50.564%
```

The measurement/transmit period can be controlled by issuing the following
command where the parameter is the period specified in milliseconds:

```txt
set_report_period 2000
```

## Joining as *Sleepy End-Device*

Connect supports low-energy end nodes called *sleepy end-devices*. When a device
joins as sleepy it goes to low power mode between active operations to keep its
power consumption low whenever it is possible. To join as *sleepy end-device*
issue the following command on the Sensor node:

```txt
join_sleepy 0
```

Note: if the device was joined previously, the `leave` command must be issued
first.

## Creating Extended Star Network by Adding *Range Extenders*

This demonstration requires at least three devices, one with the Sink firmware
and the others loaded with the Sensor firmware.

Issue the `leave` command on all devices except the Sink (*coordinator*). Then
join as *range extender* to the Sink:

```txt
join_extender 0
```

The *range-extender* acts as an *end-device* and will report the temperature and
humidity values to the Sink. If that is annoying the `set_report_period` command
can be used to increase the frequency of the transmission.

Once the *range extender* joined the permit join state of the Sink
(*coordinator*) must be disabled. Issue the following command on the Sink:

```txt
pjoin 0
```

However the *range extender* must accept join requests so, issue the `pjoin`
command on that device:

```txt
pjoin 255
```

Once the above is completed, *end-devices* can join the network by issuing

```txt
join 0
```

or

```txt
join_sleepy 0
```

Enabling and disabling permission of joining is necessary due to the network
joining priority policy. Any device first tries to join to the *coordinator*.
Thus if the *coordinator* accepts join requests all devices will join to the
*coordinator*. Otherwise, devices try to join to the *range extenders*. The
*end-devices* will join to the *range extender* that replied first to the
request. Basically, in a Connect network, `pjoin` is the way to control the
network structure.

## Security

To enable communicating with encrypted messages first a network key should be
specified:

```txt
set_key {00112233445566778899aabbccddeeff}
```

The encryption key must match on all devices to successfully receive encrypted
messages.

To enable encryption use

```txt
set_tx_options 0x03
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

## Notes

Connect does not implement end-to-end acknowledges. The built-in acknowledge
method only supports acknowledge between two neighbor devices. If end-to-end
acknowledge is a requirement it must be implemented in the application code.

## Conclusion

Using Connect (Extended) Star mode is an easy and quick way to establish a
robust network that supports several devices that can communicate with each
other (optionally in a secure way).

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
