# Connect: MAC Mode Device

## Introduction

MAC mode is suitable for applications that need to be fully IEEE 802.15.4
compatible and interoperable (when used with a 15.4-compliant radio
configuration). Supports connections between devices that are in the range of
each other, the network layer is not implemented thus it must be implemented by
the application if required.

## Prerequisites

The provided examples are applicable for any EFR32 device which supports
Connect.

Further readings:

- [Connect Online Documentation](https://docs.silabs.com/connect-stack/latest/)
- [Connect Tutorial
  Series](https://community.silabs.com/s/article/connect-tutorial-series?language=en_US)

## Prepare the Application

Open the *Connect - SoC MAC Mode Device* example through Simplicity
Studio project wizard.

Compile the projects and download the firmware image to two devices.

## Network Establishment

Note: before any further steps it is recommended to issue the `leave` command on
all devices that deconfigures the devices (clears their network settings).

There are two ways to configure the network parameters in MAC mode:

- commissioning the parameters
- forming the network by a device and joining to this device with others

Actually, under the hood forming a network in MAC mode is the same as
commissioning the parameters except that the node ID will be fixed as `0x0000`.

### Commissioning the Network Parameters

First, the network parameters must be commissioned. The device must ensure to
set a unique address and the corresponding PAN ID:

```txt
> commission 0x06 0x0001 0x01ff 0 0
```

The parameters of the command are the

- node type (this can be `0x06` for MAC devices and 0x07 for sleepy MAC devices)
- node ID
- PAN ID
- channel
- transmit power

Node ID must be uniquely chosen for every device in the same PAN.

Note: Connect devices store their network parameters in a non-volatile memory
area (NVM) and the MAC Mode Device application exploits this feature that means
no network configuration is necessary after every power cycle or reset.

Once the network parameters are commissioned the device is ready to send and
receive messages. Issue the `commission` command on every node.

### Forming a Network

First, the network must be formed i.e. setup the coordinator device:

```txt
> form 0x1ff 0 0
Network up
Network formed
>
```

The parameters of the command are the

- PAN ID
- channel
- transmit power

Note: if the network is formed by the `form` command the address of the device
will be `0x0000` hence node ID is not specified just as node type which will be
`0x06` (MAC device).

To allow devices to join the network the coordinator must permit joining:

```txt
> pjoin 255
Permit join set 0xFF
>
```

The parameter of `pjoin` is the duration in seconds while the devices are
permitted to join. The value `0` disables joining while `255` enables joining
indefinitely.

Once the coordinator set up devices can join the network by issuing the
following command:

```txt
> join 6 0x1ff 0 0 0
Started the joining process
> Network up
```

The parameters of the command are the

- node type (this can be `0x06` for MAC devices and 0x07 for sleepy MAC devices)
- PAN ID
- channel
- transmit power
- flag to use long address (see below)

The coordinator will display information about the joining of the devices:

```txt
> Node with short address 0xB04E joined as mac mode device
```

The coordinator assigns a random short address to every joined device if the
long address flag is `0`. If that flag is specified as `1` the assigned address
of the joining device will be `0xFFFE` that means the device will use its long
address (EUI64 of the device) in the communications.

## Retrieving the Network Parameters

The `info` command can be used to check the network parameters. If the network
is configured the output should be similar to the example below:

```txt
> info
Info:
         MCU Id: 0xB57FFFEB59FA4
  Network state: 0x02
      Node type: 0x06
        Node id: 0xB04E
         Pan id: 0x01FF
        Channel: 0
          Power: 0
     TX options: MAC acks enabled, security disabled, priority disabled
>
```

## De-Configuring the Network Parameters

To de-configure, the network parameters, issue the `leave` command.

## Communication Between Devices

To test the communication between any two parties issue the `send` command:

```txt
> send 0x0022 0xe188 {} 0x0000 {} 0x01ff 0x01ff {1122334455667788}  
MAC frame submitted
>
```

On the device with the short address `0x000` (the coordinator) the message
should be received:

```txt
> unsecured, acked
MAC RX: Data from 0xE188:{ 11 22 33 44 55 66 77 88}
```

The parameters of the `send` command are:

- MAC frame info (specifying the addressing mode)
- source short address
- source long address (if not specified use `{}` i.e. empty hex string)
- destination short address
- destination long address (if not specified use `{}` i.e. empty hex string)
- source PAN ID
- destination PAN ID
- message to transmit

The MAC frame info is a "nibble mask" indicating which of the following fields
are used in the parameter list:

- 0x000F - source ID mode (0x00 = none, 0x02 = short, 0x03 = long)
- 0x00F0 - destination ID mode (0x00 = none, 0x02 = short, 0x03 = long)
- 0x0F00 - the source pan ID is specified (0x01) or not (0x00).
- 0xF000 - the destination pan ID is specified (0x01) or not (0x00).

From the example above, value `0x0022` means that the short source and
destination addresses are used and PAN IDs are ignored.

Due to the flexibility of IEEE802.15.4 addressing mode the are multiple
combinations of specifying the source / destination addresses and the PAN ID(s)
- not all combinations are described in this document but the following example
demonstrates how to use the long addresses in case of inter-PAN communication:

```txt
send 0x1133 0x0000 {a4 9f b5 fe ff 57 0b 00} 0x0000 {B9 9f b5 fe ff 57 0b 00} 0xccdd 0xaabb {aabbccdd}   
MAC frame submitted
> 
```

The destination should print something similar:

```txt
> unsecured, acked
MAC RX: Data from A49FB5FEFF57 B 0:{ AA BB CC DD}
```

## Data Request

In the case of sleepy devices, the radio is turned off if no active transmission
is in progress or waiting for acknowledge. This means it is not possible to send
messages to these devices anytime as they will not receive them. However, sleepy
devices can poll the coordinator for messages sent to them. However, messages
sent to the sleepy devices are stored in the coordinator's indirect queue which
will send them upon data request (poll) or purged after the indirect queue
timeout. The sleepy device sends a data request to the coordinator which will
notify the device by the acknowledge that there is a pending message (by setting
the pending flag in the acknowledge to `1`). If that flag is set, the device
will not turn off the radio immediately after receiving the acknowledge but
keeps it turned on for receiving the pending message. Only one pending message
can be received at a time, so to receive multiple pending messages the device
must poll the coordinator multiple times.

First, set up a network consisting of a coordinator and enable joining:

```txt
> form 0x01ff 0 0                                                               
Network up                                                                      
Network formed                                                                  
> pjoin 255                                                                     
Permit join set 0xFF                                                            
>
```

Then join with a device as sleepy:

```txt
> join 0x07 0x01ff 0 0 0                                                        
Started the joining process                                                     
> Network up                                                                    
```

The coordinator will assign a short address to the device:

```txt
> Node with short address 0x3351 joined as sleepy mac mode device
```

Sending the message directly will fail:

```txt
> send 0x0022 0x0000 {} 0x3351 {} 0x01ff 0x01ff {aabbccdd}                      
MAC frame submitted                                                             
> MAC TX: 0x40                                                                  
```

The value `0x40` means `EMBER_MAC_NO_ACK_RECEIVED`.

To send a message to a sleepy node the message must be passed to the indirect
queue by setting bit 4 of the TX options to `1` (i.e. `0x08`, but the ACK should
be enabled which is bit 1 (`0x02`):

```txt
> set_options 0x0a                                                              
Send options set: 0x0A                                                          
>
```

Now, the send command doesn't return with fail immediately:

```txt
> send 0x0022 0x0000 {} 0x3351 {} 0x01ff 0x01ff {aabbccdd}                      
MAC frame submitted
>
```

Issuing the `poll` command on the sleepy device will gather the pending message
from the coordinator:

```txt
> poll                                                                            
Poll status 0x00> unsecured, acked                                              
MAC RX: Data from 0x0000:{ AA BB CC DD}                                         
```

However, sending the message and not polling within the timeout will cause the
coordinator to purge the message from the indirect queue:

```txt
>send 0x0022 0x0000 {} 0x3351 {} 0x01ff 0x01ff {aabbccdd}                       
MAC frame submitted                                                             
> MAC TX: 0x41                                                                  
```

The value `0x41` means `EMBER_MAC_INDIRECT_TIMEOUT`.

It is also possible to purge the indirect queue on the coordinator manually by
issuing the `purge_indirect` command before exceeding the timeout:

```txt
> send 0x0022 0x0000 {} 0x3351 {} 0x01ff 0x01ff {aabbccde}                      
MAC frame submitted                                                             
> purge_indirect                                                                
MAC TX: 0x42                                                                    
Purge indirect success                                                          
>
```

## Additional Helper Commands

### Beacons

By issuing the `active_scan` command the device transmits a *beacon request* to
the surrounding devices and they will respond with a *beacon*. By default, the
beacon payload is an empty string but it can be set to any desired value to
differentiate between devices:

```txt
> set_beacon_payload {01234567} 
Set beacon payload: {01234567}: status=0x00
>
```

To request and gather the beacons of the devices in range issue the following
command:

```txt
> active_scan 0
Start active scanning: channel 0, status=0x00
> BEACON: panId 0xCCDD source 0x2022 payload {01234567}
Active scan complete
```

The parameter of the command is the channel to scan.

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
encrypted (`1`) or unencrypted (`0`), `bit 0` controls whether the acknowledge
is required (`1`) for the transmitted message or not (`0`) so, `0x03` means
security and acknowledge are both enabled.

Note: `set_tx_options` controls only the encryption of the message to transmit.

On the receive side, the messages will be accepted (forwarded to the application
code) if:

- the transmitted message is not encrypted
- the transmitted message is encrypted with the key same as applied on the
  receiver side

## Additional Test Features

### Energy Scan

The device can measure the RSSI value for a given number of samples on the
specified channel and report the value of the energy level:

```txt
> start_energy_scan 0 100
Start energy scanning: channel 0, samples 100
> Energy scan complete, mean=-83 min=-87 max=-79 var=1
```

## Restart the Device

The command `reset` restarts the device.

## Conclusion

Connect MAC mode is a versatile tool to implement IEEE802.15.4 compatible /
interoperable devices with the freedom of comprehensive API functions and
features of the Connect stack.

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
