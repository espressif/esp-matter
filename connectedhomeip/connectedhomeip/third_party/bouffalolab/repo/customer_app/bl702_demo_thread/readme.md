# Openthread

Current source code of openthread is based on [Thread Reference 2020-08-18](https://github.com/openthread/openthread/releases/tag/thread-reference-20200818). 

# Command line example

Command line example is for openthread official examples: `ot-cli-ftd` and `ot-cli-mtd`, which doesn't have extra configurations during startup. 

## Build

Type following command to build:

```shell
./genotcli
```

`genotcli` script has four following build options to build FTD/MTD devices and build whether bouffalolab command line set together. 

| option        | comments                                                     |
| ------------- | ------------------------------------------------------------ |
| CONFIG_FTD    | `0`, build MTD image, which enables joiner<br />`1`, build FTD image, which enables both of commissioner and joiner. |
| CONFIG_PREFIX | `0`, original openthread cli command names, eg, type `state` to get status of device.<br/>`1`, build bouffalolab command line set together, openthread command line set works as `otc` command's sub set. eg, type`otc state` to get status of device. |
| CONFIG_USB_CDC | `1`, enable CDC; <br />`0`, disabe CDC|
| CONFIG_BT_OAD_SERVER | `1`, enable BLE for OTA upgrade; <br /> `0`, disable BLE |

## Execution - 1

1. Build a FTD image and a MTD image,  (Two FTD images are also OK) and download to the eval boards.

2. Start a network, types command as following:

   ```shell
   > dataset init new
   Done
   > dataset commit active
   Done
   > ifconfig up
   Done
   > thread start
   Done
   ```

   After a while , type `state` to check the device whether to be leader.

3. Start the commission role, types command as following: 

   ```shell
   > commissioner start
   Done
   ```

   And then, types command as following to let other devices to attach:

   ```shell
   > commissioner joiner add * J01NME
   ```

4. Start another device and attach to the network as a joiner, type commands as following:

   ```shell
   > ifconfig up
   Done
   joiner start J01NME
   > Done
   ```

   After get `Join success!` confirmation message, and type following command to start the device.

   ```shell
   > thread start
   Done
   ```

5.  Check the status

   After a while, type `state` command to get device status.

6. Ping each other

   Type `ipaddr`  to get IP address, and then `ping <ip address>` in command line of another device. And then sniffer tool should captured ICMPv6 packets.

Please refer the [official page](https://openthread.google.cn/guides/build/commissioning) for more detail information.

## Execution - 2

While，directly configuring dataset to attach the device to the network also works.

1. Build a FTD image and a MTD image,  (Two FTD images are also OK) and download to the eval boards.

2. Start a network, types command as following:

   ```shell
   > dataset init new
   Done
   > dataset commit active
   Done
   > ifconfig up
   Done
   > thread start
   Done
   ```

   After a while , type `state` to check the device whether to be leader.

3. Get current active dataset of the network, and type command as folllowing:
   ```shell
   > dataset active -x
   0e080000000000010000000300000b35060004001fffe00208ee2c03898c34f15c0708fd38d25691e24621051015f62338e7c77a9b19dad9f3165f9405030f4f70656e5468726561642d633763390102c7c90410a304ef9fae8a047130e4461361f9489c0c0402a0fff8
   Done
   ```
4. Start another device and attach to the network with dataset, type commands as following:

   ```shell
   > dataset set active 0e080000000000010000000300000b35060004001fffe00208ee2c03898c34f15c0708fd38d25691e24621051015f62338e7c77a9b19dad9f3165f9405030f4f70656e5468726561642d633763390102c7c90410a304ef9fae8a047130e4461361f9489c0c0402a0fff8
   dataset set active 0e080000000000010000000300000b35060004001fffe00208ee2c03898c34f15c0708fd38d25691e24621051015f62338e7c77a9b19dad9f3165f9405030f4f70656e5468726561642d633763390102c7c90410a304ef9fae8a047130e4461361f9489c0c0402a0fff8
   Done
   > ifconfig up
   Done
   > thread start
   Done
   ```

5.  Check the status

   After a while, type `state` command to get device status.

6. Ping each other

   Type `ipaddr`  to get IP address, and then `ping <ip address>` in command line of another device. And then sniffer tool should captured ICMPv6 packets.

# LED example

LED example can toggle each other's on-board led through connecting IO12 PIN to VDD.

## Build

Type following command to build:

```shell
./genotled
```

`genotled` script has two build options to build FTD/MTD devices and build whether bouffalolab command  line set together.  

| option        | comments                                                     |
| ------------- | ------------------------------------------------------------ |
| CONFIG_FTD    | `0`, build MTD(SED) image, which sets rx-on-when-idle=false during startup<br />`1`, build FTD image. |
| CONFIG_PREFIX | `0`, original openthread cli command names, eg, type `state` to get status of device. <br />`1`, build bouffalolab command line set together, openthread command line set works as `otc` command's sub set. eg, type `otc state` to get status of device. |
| CONFIG_OTDEMO | `1`, use UDP to send on/off message <br />`2`, use CoAP to send on/off message |
| CONFIG_PP     | Set polling period of SED device, which is valid when CONFIG_FTD=0. And unit is `ms`. |

## Execution

1. Build a FTD image and a MTD image with UDP or CoAP selected,  (Two FTD images are also OK) and download to the eval boards.

2. Power on FTD device, a network with PANID `0xB702` will start. 

   > Please observe on-board LED on/off status to check whether it becomes a leader.

3. Power on another FTD device or MTD/SED device, it will attach to the network.

   > Please observe on-board LED on/off status to check whether it attaches to the network.

4. Connecting PIN IO12 to VDD to trigger UDP/CoAP message send out with on/off control.

   > It will go through all neighbors (just parent for MTD/SED) to send out message.

5. Observe RX0_LED wtheter to be toggled.

## LED indications

|TX1_LED| RX1_LED| 状态|
|-|-|-|
|off|off|thread is not started|
|on|off|router|
|on|on|child|
|off|on|leader|


# NCP/RCP example
## Build

Type following command to build:

```shell
./genotncp
```

`genotcli` script has two following build options NCP image.

| option        | comments                                                     |
| ------------- | ------------------------------------------------------------ |
| CONFIG_NCP    | `1`, build NCP device, and `CONFIG_FTD` option is valid to configure <br />`0`, build RCP device. |
| CONFIG_FTD    | `0`, build MTD image;<br />`1`, build FTD image. |

> **NOTE**, NCP/RCP only works with UART.
## pyspinel & sniffer

Please refer to [offical guide](https://openthread.io/guides/pyspinel) for more detail.
### without extcap

```shell
python3 sniffer.py -c <channel> -u <device_of_uart_> --crc -b <uart_baudrate> | <path_to_wireshark> -k -i -
```

### extcap

Note that `COMMON_BAUDRATE` in `extcap_ot.py` doesn't support 2000000 Uart baudrate, please change uart baudrate with DTS file.

## Border Router with Raspberry Pi

Please refer to [Border Router](https://openthread.io/guides/border-router) and [Raspberry Pi](https://openthread.io/guides/border-router/raspberry-pi) for more detail information.

**Note**, by default otbr-agent uses UART baudrate 115200 for communication. 
- Please change baudrate with DTS file;
- Or change `OTBR_AGENT_OPTS` in /etc/default/otbr-agent to like `spinel+hdlc+uart:///dev/ttyUSB0?uart-baudrate=<baudrate>`



