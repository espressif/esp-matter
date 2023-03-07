# Validating Thread

After building and programming the Thread executable, we will have to verify the proper operation thereof.

---

- [Validating Thread](#validating-thread)
  - [Nomenclature](#nomenclature)
  - [Interacting with the Thread stack through the Command Line Interface](#interacting-with-the-thread-stack-through-the-command-line-interface)
    - [Getting `help`](#getting-help)
    - [The `dataset` api](#the-dataset-api)
  - [Forming a Thread network](#forming-a-thread-network)
    - [Starting a Thread Network](#starting-a-thread-network)
    - [Joining a second node to the Thread Network](#joining-a-second-node-to-the-thread-network)
  - [Device information](#device-information)
    - [Common](#common)
    - [Router](#router)
    - [Parent](#parent)
    - [Child](#child)
  - [Ping between devices](#ping-between-devices)
  - [Reset a device and validate reattachment](#reset-a-device-and-validate-reattachment)

---

## Nomenclature

- **FTD**: Full Thread Device
- **MTD**: Minimal Thread Device
  - **MED**: Minimal End Device
  - **SED**: Sleepy End Device

## Interacting with the Thread stack through the Command Line Interface

To interact with the Thread stack, commands need to be typed on the Command Line Interface (CLI). The format in which these commands and there output will be presented is as follows:

    > some_command argA argB
    optional_output
    return_value

The lines that start with `>` will be the actual commands to be typed on the CLI.  The lines that follow will contain the output of the CLI. Note that the output based on random values such as the generated Thread Master Key will observed while running the commands will differ from the values used in this manual.  
The final line will contain a return code. The value is `Done` when everything went well. Other possibilities are (but not limited to)

- `Error 6: Parse`: when a provided argument is in the wrong format
- `Error 7: InvalidArgs`: when a provided argument is of the wrong type
- `Error 23: NotFound`: when requested data does not exist
- `Error 35: InvalidCommand`: when the command is not recognized

The error `Error 35: InvalidCommand` is also returned when a valid command is not supported by a certain build. E.g. with the build

    script/build gp712 -DOT_JOINER=1

a commissioner command will result in

    > commissioner start
    InvalidCommand

Whereas with the build

    script/build gp712 -DOT_JOINER=1 -DOT_COMMISSIONER=1

a commissioner command will result in

    > commissioner start
    Done

**Note**: The CLI server doesn’t contain an echo. This results in the commands results only being displayed after `enter` is pressed.

### Getting `help`

To know the most common commands supported by CLI, the `help` command can be used

    > help
    bufferinfo
    ...
    version
    Done

Additional info on all the commands can be found in the [CLI Reference Manual](https://github.com/openthread/openthread/blob/main/src/cli/README.md)

### The `dataset` api

Currently there are two ways for setting the the `panid`, the `masterkey` and some other network variables. There is the older, direct way e.g.

    > channel 15
    Done
    > panid 0xf00d
    Done

and the newer, recommended api through the `dataset` command e.g.

    > dataset channel 15
    Done
    > dataset panid 0xf00d
    Done

In the below examples the latter, newer and more extensive dataset api will be used

## Forming a Thread network

To form a first Thread network we will use two devices. The first, more capable, device will fulfill the *Leader* role, will act as *Commissioner* and also as a parent.

The first device needs to be build as follows

    script/build <platform> -DOT_COMMISSIONER=1 -DOT_JOINER=1

Note the explicit enabling of the *Commissioner* role through `-DOT_COMMISSIONER=1`.
It is very important that the *FTD* executable (`<platform>-ot-cli-ftd`) is programmed on the device under test.

The second device can be less capable

    script/build <platform> -DOT_JOINER=1

Here, it is up to the reader to either program the *FTD* or the *MTD* executable. Both will work in the below scenarios.

### Starting a Thread Network

Demonstrate successful protocol timers by forming a Thread network and verifying the node has transitioned to the Leader state.

Over the serial interface type in the following commands (here prepended with `>` and followed by the expected output):

    > dataset channel 15
    Done
    > dataset panid 0xf00d
    Done
    > dataset init new
    Done
    > ifconfig up
    Done
    > thread start
    Done
    > state
    leader
    Done

This will, in order, specify the channel and panid of the new Thread network, enable the new dataset, bring up the WPAN Thread interface and start the Thread Network.

To display all the information about the newly formed Thread network, type

    > dataset active
    Active Timestamp: 0
    Channel: 11
    Channel Mask: 0x07fff800
    Ext PAN ID: dead00beef00cafe
    Mesh Local Prefix: fdde:ad00:beef:0::/64
    Master Key: d5d1efe77855947c03b3bac50f48e9e3
    Network Name: OpenThread
    PAN ID: 0xf00d
    PSKc: a7e44fe41bd7a7b0502337d4038a9d3d
    Security Policy: 672, onrcb
    Done

**Note:** In the above example, because we did not specify them up front, the Thread Network Master Key and the Thread Mesh Local Prefix are selected at random by the node. They can be queried (and set) through the `dataset masterkey` and `dataset meshlocalprefix` commands.

    > dataset masterkey
    d5d1efe77855947c03b3bac50f48e9e3
    Done
    > dataset meshlocalprefix
    fdde:ad00:beef:0::/64
    Done

### Joining a second node to the Thread Network

In the CLI on the first device, type:

    > commissioner start
    Commissioner petitioning
    Done
    > commissioner joiner add * J01NME
    Done

The first command makes the device request the network for approval to be the commissioner on the Network. The second command opens joining for any device (`*`) with the passphrase `J01NME` The joining window will be open for the default 30 seconds.

If the second devices has to function as a *MTD*, first type

    > mode r

for a *MED* and

    > mode -

for a *SED*.

Now, on the second device, with the enabled Joiner role (we assume a *FTD*), run:

    > ifconfig up
    Done
    > joiner start J01NME
    Done
    > thread start
    Done
    > state
    router
    Done

The above sequence, in order,

- brings up the radio interface
- joins the device to the network with the passphrase `J01NME`. This takes some time. At the end of this step the device has all the required network information (Master Key, Mesh-local Prefix, …). At this point the device is not yet connected to the network, though
- starts the Thread stack. After this step the device will be fully connected to the network.
- checks the state of the device.  
    **Note:** becoming a router takes some time (can be well over a minute, as per the specification). If the device is not a *FTD*, the device will never become a router (by design). In this case the output of the `state` command will be `child`.

## Device information

The CLI provides some commands to get insight in both the devices and the network.

### Common

Network information is obtained through

    > dataset active

Each device can list its own addresses

    > ipaddr
    fdde:ad00:beef:0:0:ff:fe00:fc00
    fdde:ad00:beef:0:0:ff:fe00:800
    fdde:ad00:beef:0:5b:3bcd:deff:7786
    fe80:0:0:0:6447:6e10:cf7:ee29
    Done

Or a specific ip address can be queried through a second parameter

    > ipaddr rloc
    fdde:ad00:beef:0:0:ff:fe00:800
    Done

A list of neighbors (both routers and children)

    > neighbor table
    | Role | RLOC16 | Age | Avg RSSI | Last RSSI |R|D|N| Extended MAC |
    +------+--------+-----+----------+-----------+-+-+-+--------------+
    Done

### Router

    > router table
    | ID | RLOC16 | Next Hop | Path Cost | LQ In | LQ Out | Age | Extended Mac |Link |
    +----+--------+----------+-----------+-------+--------+-----+--------------+-----+
    Done

### Parent

On the parent (the *FTD*), we can get the list of children (this will be zero if the second device became a router)

    > child list
    1
    Done

And for each of the listed IDs we can query device information

    > child <id>
    Child ID: 1
    Rloc: 0801
    Ext Addr: 2e5e8adffa9a7ad8
    Mode: rdn
    Net Data: 38
    Timeout: 300
    Age: 55
    Link Quality In: 3
    RSSI: -55
    Done

The ip addresses of the children can be obtained with

    > childip
    ...
    Done

### Child

The child can display information on its parent

    > parent
    Ext Addr: 66476e10cf7ee29
    Rloc: 0800
    Link Quality In: 3
    Link Quality Out: 3
    Age: 7
    Done

## Ping between devices

To obtain the IPv6 addresses of the first device run

    > ipaddr
    fdde:ad00:beef:0:0:ff:fe00:fc00
    fdde:ad00:beef:0:0:ff:fe00:800
    fdde:ad00:beef:0:5b:3bcd:deff:7786
    fe80:0:0:0:6447:6e10:cf7:ee29
    Done

On the second device, now run:

    > ping fdde:ad00:beef:0:5b:3bcd:deff:7786
    16 bytes from fdde:ad00:beef:0:5b:3bcd:deff:7786: icmp_seq=1 hlim=64
    time=24ms

Of course the flow can be reversed as well

**Note:** if one of the devices is a SED, the ping command will likely fail because the default ping timeout is shorter than the default SED `childtimeout`. The `ping` command can take some more parameters

    > ping <dst> [size] [count] [interval] [hoplimit] [timeout]

by assuring `[timeout]` is larger than `childtimeout`, the ping should succeed.  
`childtimeout` is a CLI command used to both set and query the current value of the Child timeout.

## Reset a device and validate reattachment

Demonstrate non-volatile functionality by resetting the device and validating its reattachment to the same network without user intervention.

On the second device first run

    > reset
    Done

This will reset the device but will keep the Active Dataset in non-volatile memory. This can be easily verified by running

    > dataset active
    Active Timestamp: 0
    Channel: 11
    Channel Mask: 0x07fff800
    Ext PAN ID: dead00beef00cafe
    Mesh Local Prefix: fdde:ad00:beef:0::/64
    Master Key: d5d1efe77855947c03b3bac50f48e9e3
    Network Name: OpenThread
    PAN ID: 0xf00d
    PSKc: a7e44fe41bd7a7b0502337d4038a9d3d
    Security Policy: 672, onrcb
    Done

Since resetting will disable the network interface and the Thread stack, run the following to enable both:

    > ifconfig up
    Done
    > thread start
    Done

After bringing up the network interface and the Thread stack again. The device will reattach to the Thread Network and after a while become a router again (Same remarks as in the [_Joining a second node_](#joining-a-second-node-to-the-thread-network) case apply).

Power cycling the device will have the same effect as running the `reset` command. So, similarly

    > ifconfig up
    Done
    > thread start
    Done

has to be run, to rejoin the device to the network.

On the other hand

    > factoryreset
    Done

will remove the Active Dataset from non-volatile memory. This can easily seen by running

    > dataset active
    Error 23: NotFound

To rejoin the device to the network, a full join procedure as described in [_Joining a second node_](#joining-a-second-node-to-the-thread-network) has to be executed.
