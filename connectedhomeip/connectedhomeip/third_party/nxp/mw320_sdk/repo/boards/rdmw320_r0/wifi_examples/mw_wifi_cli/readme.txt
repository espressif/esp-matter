Overview
========
This is the Wi-Fi CLI example to demonstrate the CLI support usage. The CLI module allows user to add CLIs in application.


Toolchain supported
===================
- IAR embedded Workbench  8.50.9
- GCC ARM Embedded  10.2.1

Hardware requirements
=====================
- Mini USB cable
- RDMW320-R0 board
- J-Link Debug Probe
- Personal Computer

Board settings
==============

Prepare the Demo
================
The example requires M4 core to load WIFI firmware from flash. So the partition table layout.bin and
the WIFI firmware xxx.fw.bin need to be prepared in the flash before running the example.
Use following steps to get the flash contents ready:
1.  Connect a USB cable between the host PC and Mini USB port on the target board.
2.  Connect j-link probe between the host PC and JTAG port on the target board.
3.  Open jlink.exe, execute following commands
    J-Link>connect
      Device>88MW320
      TIF>s
      Speed>
    J-Link>loadbin <sdk_path>\tools\boot2\layout.bin 0x1F004000
    J-Link>loadbin <sdk_path>\boards\rdmw320_r0\wifi_examples\common\mw30x_uapsta_W14.88.36.p144.fw.bin 0x1F150000

    To create your own layout.bin and wifi firmware bin for flash partition, please use the tool located at
    <sdk_path>\tools\mw_img_conv to convert the layout configuration file and WIFI firmware to the images suitable
    for flash partition. For example,
    # mw_img_conv layout layout.txt layout.bin
    # mw_img_conv wififw mw30x_uapsta_W14.88.36.p144.bin mw30x_uapsta_W14.88.36.p144.fw.bin
    Please note the wifi firmware binary should not be compressed.

Now use the general way to debug the example
1.  Connect a USB cable between the host PC and Mini USB port on the target board.
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Launch the debugger in your IDE to begin running the demo.

To make the demo bootable on board power on, additional steps are needed as follows:
1.  Because the debugger downloads the application without considering XIP flash offset setting in FLASHC, we creates
    a special linker file with manual offset to make the wifi example debuggable (without conflicting the partition
    table area in flash). To make it bootable, we need to change the linker file to the one in devices, e.g.
    <sdk_path>\devices\88MW320\iar\88MW320_xx_xxxx_flash.icf and build the application again.
2.  Create bootable MCU firmware
    Run <sdk_path>\tools\mw_img_conv\mw320\mkimg.sh with the built binary file, for example,
    # ./mkimg.sh mw_wifi_cli.bin
    Then mw_wifi_cli.fw.bin will be created.
3.  Write needed components to flash partition in jlink.exe according to partition table (layout.txt), you may skip
    the component which has not been changed on flash.
    J-Link>connect
      Device>88MW320
      TIF>s
      Speed>
    J-Link>exec SetFlashDLNoRMWThreshold = 0xFFFF        // SET RMW threshold to 64kB, so size < 64KB will be WMW.
    J-Link>loadbin <sdk_path>\tools\boot2\boot2.bin 0x1F000000
    J-Link>loadbin <sdk_path>\tools\boot2\layout.bin 0x1F004000
    J-Link>loadbin mw_wifi_cli.fw.bin 0x1F010000
    J-Link>loadbin <sdk_path>\boards\rdmw320_r0\wifi_examples\common\mw30x_uapsta_W14.88.36.p144.fw.bin 0x1F150000
4.  Reset your board and then the application is running.
Running the demo
================
When the demo starts, a welcome message would appear on the terminal, press enter for command prompt.
Press tab or type help to list out all available CLI commands.
Note: There might be a message "[psm] Error: psm: Could not read key key_size from keystore. Please flash correct boot2" on startup.
The message means the demo cannot get keys for secure PSM from keystore area filled by boot2. In debugging or non-secure boot case,
it's an expected result because no boot loader fills the keystore. If the error message occurs, the demo then loads the hard-coded
keys to move on. So the error message doesn't have impact on the application flow.

    ----------------------------------------
    wifi cli demo
    ----------------------------------------
    Initialize CLI
    ----------------------------------------
    Initialize WLAN Driver
    ----------------------------------------
    MAC Address: 00:50:43:24:37:E0
    [net] Initialized TCP/IP networking stack
    ----------------------------------------
    app_cb: WLAN: received event 10
    ----------------------------------------
    app_cb: WLAN initialized
    ----------------------------------------
    WLAN CLIs are initialized
    ----------------------------------------
    CLIs Available:
    ----------------------------------------

    help
    wlan-version
    wlan-mac
    wlan-scan
    wlan-scan-opt ssid <ssid> bssid ...
    wlan-add <profile_name> ssid <ssid> bssid...
    wlan-remove <profile_name>
    wlan-list
    wlan-connect <profile_name>
    wlan-start-network <profile_name>
    wlan-stop-network
    wlan-disconnect
    wlan-stat
    wlan-info
    wlan-address
    wlan-get-uap-channel
    wlan-get-uap-sta-list
    ping [-s <packet_size>] [-c <packet_count>] [-W <timeout in sec>] <ip_address>
    iperf [-s|-c <host>|-a|-h] [options]
    dhcp-stat
    save-profile <profile_name>
    load-profile
    reset-profile
    wlan-ieee-power-save <on/off> <wakeup condition>
    wlan-deepsleep <on/off>
    mcu-power-mode <pm0/pm1/pm2/pm4> [<pm2_io_exclude_mask>]
    ----------------------------------------

    # help

    help
    wlan-version
    wlan-mac
    wlan-scan
    wlan-scan-opt ssid <ssid> bssid ...
    wlan-add <profile_name> ssid <ssid> bssid...
    wlan-remove <profile_name>
    wlan-list
    wlan-connect <profile_name>
    wlan-start-network <profile_name>
    wlan-stop-network
    wlan-disconnect
    wlan-stat
    wlan-info
    wlan-address
    wlan-get-uap-channel
    wlan-get-uap-sta-list
    ping [-s <packet_size>] [-c <packet_count>] [-W <timeout in sec>] <ip_address>
    iperf [-s|-c <host>|-a|-h] [options]
    dhcp-stat
    save-profile <profile_name>
    load-profile
    reset-profile
    wlan-ieee-power-save <on/off> <wakeup condition>
    wlan-deepsleep <on/off>
    mcu-power-mode <pm0/pm1/pm2/pm4> [<pm2_io_exclude_mask>]

    # wlan-version
    WLAN Driver Version   : v1.3.r21.p1
    WLAN Firmware Version : w8845-R0, RF878X, FP88, 14.88.36.p144, WPA2_CVE_FIX 1, PVE_FIX 1

    # wlan-mac
    MAC address
    00:50:43:24:37:E0

    # wlan-scan
      Scan scheduled...

    # 3 networks found:
      94:10:3E:02:60:F0  "nxp_mrvl" Infra
              channel: 1
              rssi: -25 dBm
              security: OPEN
              WMM: YES
      94:10:3E:02:60:F1  "nxp_mrvl_5ghz" Infra
              channel: 36
              rssi: -39 dBm
              security: OPEN
              WMM: YES
      90:72:40:21:B3:1A  "apple_g" Infra
              channel: 11
              rssi: -51 dBm
              security: WPA2
              WMM: YES

    # wlan-scan-opt
      Usage:
          wlan-scan-opt ssid <ssid> bssid <bssid> channel <channel> probes <probes>
      Error: invalid number of arguments

    # wlan-scan-opt ssid apple_g
      Scan for ssid "apple_g" scheduled...

    # 2 networks found:
      90:72:40:21:B3:1A  "apple_g" Infra
              channel: 11
              rssi: -52 dBm
              security: WPA2
              WMM: YES
      90:72:40:21:B3:1B  "apple_g" Infra
              channel: 149
              rssi: -60 dBm
              security: WPA2
              WMM: YES

    # wlan-add
      Usage:
      For Station interface
        For DHCP IP Address assignment:
          wlan-add <profile_name> ssid <ssid> [wpa2 <secret>]
          wlan-add <profile_name> ssid <ssid> [owe_only]
          wlan-add <profile_name> ssid <ssid> [wpa3 sae] <secret>
        For static IP address assignment:
          wlan-add <profile_name> ssid <ssid>
          ip:<ip_addr>,<gateway_ip>,<netmask>
          [bssid <bssid>] [channel <channel number>]
          [wpa2 <secret>]
      For Micro-AP interface
          wlan-add <profile_name> ssid <ssid>
          ip:<ip_addr>,<gateway_ip>,<netmask>
          role uap [bssid <bssid>]
          [channel <channelnumber>]
          [wpa2 <secret>]
      Error: invalid number of arguments

    # wlan-add abc ssid nxp_mrvl
      Added "abc"

    # wlan-connect abc
      Connecting to network...
      Use 'wlan-stat' for current connection status.

    # Connected to following BSS : SSID = [nxp_mrvl], IP = [192.168.10.152]

    # wlan-stat
      Station connected (Active)
      uAP stopped

    # wlan-info
      Station connected to:
      "abc"
              SSID: nxp_mrvl
              BSSID: 94:10:3E:02:60:F0
              channel: 1
              role: Infra
              security: none

              IPv4 Address
              address: DHCP
                      IP:             192.168.10.152
                      gateway:        192.168.10.1
                      netmask:        255.255.255.0
                      dns1:           192.168.10.1
                      dns2:           0.0.0.0
      uAP not started

    #
    # wlan-add abd ssid NXP_Soft_AP ip:192.168.10.1,192.168.10.1,255.255.255.0 role uap wpa2 12345678
      Added "abd"

    # wlan-start-network abd

    # ----------------------------------------
      app_cb: WLAN: received event 14
      ----------------------------------------
      app_cb: WLAN: UAP Started
      ----------------------------------------
      Soft AP "NXP_Soft_AP" Started successfully
      ----------------------------------------
      DHCP Server started successfully

    # wlan-info
      Station connected to:
      "abc"
              SSID: nxp_mrvl
              BSSID: 94:10:3E:02:60:F0
              channel: 1
              role: Infra
              security: none

              IPv4 Address
              address: DHCP
                      IP:             192.168.10.152
                      gateway:        192.168.10.1
                      netmask:        255.255.255.0
                      dns1:           192.168.10.1
                      dns2:           0.0.0.0
      uAP started as:
      "abd"
              SSID: NXP_Soft_AP
              BSSID: 00:50:43:24:37:E0
              channel: 1
              role: uAP
              security: WPA2

              IPv4 Address
              address: STATIC
                      IP:             192.168.10.1
                      gateway:        192.168.10.1
                      netmask:        255.255.255.0
                      dns1:           192.168.10.1
                      dns2:           0.0.0.0

    #
    # wlan-disconnect

    # ----------------------------------------
      app_cb: WLAN: received event 9
      ----------------------------------------
      app_cb: disconnected

    # wlan-info
      Station not connected
      uAP started as:
      "abd"
              SSID: NXP_Soft_AP
              BSSID: 00:50:43:24:37:E0
              channel: (Auto)
              role: uAP
              security: WPA2

              IPv4 Address
              address: STATIC
                      IP:             192.168.10.1
                      gateway:        192.168.10.1
                      netmask:        255.255.255.0
                      dns1:           192.168.10.1
                      dns2:           0.0.0.0

    #

    # wlan-list
      2 networks:
      "abc"
              SSID: nxp_mrvl
              BSSID: 00:00:00:00:00:00
              channel: (Auto)
              role: Infra
              security: none
      "abd"
              SSID: NXP_Soft_AP
              BSSID: 00:00:00:00:00:00
              channel: (Auto)
              role: uAP
              security: WPA2

              IPv4 Address
              address: STATIC
                      IP:             192.168.10.1
                      gateway:        192.168.10.1
                      netmask:        255.255.255.0
                      dns1:           192.168.10.1
                      dns2:           0.0.0.0

    # save-profile abc
    [i] mflash_save_file success

    # wlan-remove abc
      Removed "abc"

    # wlan-list
      1 network:
      "abd"
              SSID: NXP_Soft_AP
              BSSID: 00:00:00:00:00:00
              channel: (Auto)
              role: uAP
              security: WPA2

              IPv4 Address
              address: STATIC
                      IP:             192.168.10.1
                      gateway:        192.168.10.1
                      netmask:        255.255.255.0
                      dns1:           192.168.10.1
                      dns2:           0.0.0.0

    # load-profile

    # wlan-list
      2 networks:
      "abc"
              SSID: nxp_mrvl
              BSSID: 00:00:00:00:00:00
              channel: (Auto)
              role: Infra
              security: none
      "abd"
              SSID: NXP_Soft_AP
              BSSID: 00:00:00:00:00:00
              channel: (Auto)
              role: uAP
              security: WPA2

              IPv4 Address
              address: STATIC
                      IP:             192.168.10.1
                      gateway:        192.168.10.1
                      netmask:        255.255.255.0
                      dns1:           192.168.10.1
                      dns2:           0.0.0.0

    # wlan-address
      not connected

    # wlan-get

    # wlan-get-uap-channel
      uAP channel: 0

    #

    # dhcp-stat
    DHCP Server Lease Duration : 86400 seconds
    No IP-MAC mapping stored

    # wlan-deepsleep on
    Deep sleep mode change requested!

    # ----------------------------------------
    app_cb: WLAN: received event 12
    ----------------------------------------
    app_cb: WLAN: PS_ENTER

    # wlan-deepsleep off
    Deep sleep mode change requested!

    # ----------------------------------------
    app_cb: WLAN: received event 13
    ----------------------------------------
    app_cb: WLAN: PS EXIT

    # mcu-power-mode pm1

    # Enter idle for 18 ticks
    Exit from power mode 1

    # Enter idle for 98 ticks
    Exit from power mode 1
    mcu-power-mode pm2
    Error: PM2 need 3rd parameter.

    # Enter idle for 92 ticks
    Exit from power mode 1
    mcu-power-mode pm2 0

    # Enter idle for 61 ticks
    Exit from power mode 1

    # Enter idle for 82 ticks
    Exit from power mode 2
    mcu-power-mode pm0

    #

