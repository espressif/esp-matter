Overview
========
This demo implements a simple web based Wi-Fi configuration utility for connecting the board to the local Wireless network. 

Initially, the board doesn't have the credentials to join the local network, so it starts its own Access Point with SSID: "nxp_configuration_access_point" and password: "NXP0123456789".

The user can connect their device to this SSID and access the HTML UI under https://192.168.1.1. The board will scan for the nearby Wi-Fi networks and display a list of them on this page. By clicking on the entries, the user can choose their network, enter the credentials and connect. The board will attempt to join this Wi-Fi network as a client and if it succeeds, it will disconnect its AP and save the credentials to its mflash memory.

On successive restarts, it checks the mflash memory and uses the saved credentials to directly connect to the local Wi-Fi network without starting the AP. 

The site allows the user to clear the credentials from the flash memory and reset the board to AP mode. If connection fails, user can also set device to AP mode through serial connection.

The source files for the web interface are located in the webui directory. Use the `<path_to_sdk>/middleware/lwip/src/apps/httpsrv/mkfs/mkfs.pl webui` Perl script in order to convert the webui files into the httpsrv_fs_data.c which is used in order to flash the static files onto the board. Make sure the mkfsl.pl script is executed from the same directory where the file httpsrv_fs_data.c and the directory webui are.

Note that Microsoft Internet Explorer is not supported by this webconfig example.


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
    # ./mkimg.sh mw_wifi_webconfig.bin
    Then mw_wifi_webconfig.fw.bin will be created.
3.  Write needed components to flash partition in jlink.exe according to partition table (layout.txt), you may skip
    the component which has not been changed on flash.
    J-Link>connect
      Device>88MW320
      TIF>s
      Speed>
    J-Link>exec SetFlashDLNoRMWThreshold = 0xFFFF        // SET RMW threshold to 64kB, so size < 64KB will be WMW.
    J-Link>loadbin <sdk_path>\tools\boot2\boot2.bin 0x1F000000
    J-Link>loadbin <sdk_path>\tools\boot2\layout.bin 0x1F004000
    J-Link>loadbin mw_wifi_webconfig.fw.bin 0x1F010000
    J-Link>loadbin <sdk_path>\boards\rdmw320_r0\wifi_examples\common\mw30x_uapsta_W14.88.36.p144.fw.bin 0x1F150000
4.  Reset your board and then the application is running.
Running the demo
================
1. When the demo starts, basic initialization proceeds
2. After that, device will wait for connection and configuration:
Note: There might be a message "[psm] Error: psm: Could not read key key_size from keystore. Please flash correct boot2" on startup.
The message means the demo cannot get keys for secure PSM from keystore area filled by boot2. In debugging or non-secure boot case,
it's an expected result because no boot loader fills the keystore. If the error message occurs, the demo then loads the hard-coded
keys to move on. So the error message doesn't have impact on the application flow.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Starting webconfig DEMO
[i] Trying to load data from mflash.
[i] Nothing stored yet
[i] Initializing WiFi connection...
MAC Address: 00:50:43:24:37:E0
[net] Initialized TCP/IP networking stack
WLAN initialized
WLAN FW Version: w8845-R0, RF878X, FP88, 14.88.36.p144, WPA2_CVE_FIX 1, PVE_FIX 1
[i] Successfully initialized WiFi module
Starting Access Point: SSID: nxp_configuration_access_point, Chnl: 1
[wlcm] Warn: NOTE: uAP will automatically switch to the channel that station is on.
Soft AP started successfully
This also starts DHCP Server with IP 192.168.1.1
 Now join that network on your device and connect to this IP: 192.168.1.1
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

4. Connect to the access point and in your web browser enter https://192.168.1.1
5. Wait for the scan to finish and click on the desired network to join.
6. Enter the network password and click on connect.
6. After you send credentials, device will try connecting to the AP and if successful saves the credentials to the mflash.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Client => 28:C6:3F:21:6B:AF Associated with Soft AP

Initiating scan...
  nxp
     BSSID         : 00:1F:7B:31:03:9A
     RSSI          : -34dBm (off-channel)
     Channel       : 5

[i] Chosen ssid: nxp
[i] Chosen passphrase: "NXP0123456789"
[i] Joining: nxp
Client => 28:C6:3F:21:6B:AF Associated with Soft AP
Connected to following BSS:SSID = [nxp], IP = [192.168.0.63]
[i] Successfully joined: nxp
 Now join that network on your device and connect to this IP: 192.168.0.63
[i] mflash_save_file success
[i] Stopping AP!
Soft AP stopped successfully
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

7. Connect to the network "nxp" and browse "https://192.168.0.63" in the browser to see the device in client mode.
