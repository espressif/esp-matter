Overview
========

The lwip_httpsrv_ota_freertos demo application demonstrates OTA update through HTTPServer set up on lwIP TCP/IP and the MbedTLS stack with
FreeRTOS. The user uses an Internet browser to upload new version of firmware and restarts the board to perform the update


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
    # ./mkimg.sh mw_lwip_httpssrv_ota.bin
    Then mw_lwip_httpssrv_ota.fw.bin will be created.
3.  Write needed components to flash partition in jlink.exe according to partition table (layout.txt), you may skip
    the component which has not been changed on flash.
    J-Link>connect
      Device>88MW320
      TIF>s
      Speed>
    J-Link>exec SetFlashDLNoRMWThreshold = 0xFFFF        // SET RMW threshold to 64kB, so size < 64KB will be WMW.
    J-Link>loadbin <sdk_path>\tools\boot2\boot2.bin 0x1F000000
    J-Link>loadbin <sdk_path>\tools\boot2\layout.bin 0x1F004000
    J-Link>loadbin mw_lwip_httpssrv_ota.fw.bin 0x1F010000
    J-Link>loadbin <sdk_path>\boards\rdmw320_r0\wifi_examples\common\mw30x_uapsta_W14.88.36.p144.fw.bin 0x1F150000
4.  Reset your board and then the application is running.
Running the demo
================
1. When the demo runs successfully, the terminal will display the following:
        [OTA demo] Build Time: Dec 28 2020--11:27:42
        Initializing WiFi connection...
        MAC Address: 00:50:43:24:37:E0
        [net] Initialized TCP/IP networking stack
        WLAN initialized
        WLAN FW Version: w8845-R0, RF878X, FP88, 14.88.36.p144, WPA2_CVE_FIX 1, PVE_FIX 1
        Successfully initialized WiFi module
        Starting Access Point: SSID: nxp-ota, Chnl: 1
        [wlcm] Warn: NOTE: uAP will automatically switch to the channel that station is on.
        Soft AP started successfully
        This also starts DHCP Server with IP 192.168.1.1
        Network ready IP: 192.168.1.1

2. Connect you PC to WiFi network provided by the board (SSID: nxp-ota, password: NXP0123456789).
   Your PC should acquire IP configuration automatically via DHCP.
        Client => 28:C6:3F:21:6B:AF Associated with Soft AP

3. Open web browser and type https://192.168.1.1 (IP address of the board) on the browser address bar.
   The browser should show the main web page of the example.
4. Go to OTA page, select file with udpated firmware and upload it.
5. After the file is uploaded, click "Reboot" button to start the demo in test mode.
        update new image gen_level to [0x80000000], result 0

6. Once the updated firmware executes, the terminal shows:
        [OTA demo] Build Time: Feb  3 2021--14:20:27
        Running in test mode: reset image gen_level to [0], result 0
        Initializing WiFi connection...
        MAC Address: 00:50:43:24:37:E0
        [net] Initialized TCP/IP networking stack
        WLAN initialized
        WLAN FW Version: w8845-R0, RF878X, FP88, 14.88.36.p144, WPA2_CVE_FIX 1, PVE_FIX 1
        Successfully initialized WiFi module
        Starting Access Point: SSID: nxp-ota, Chnl: 1
        [wlcm] Warn: NOTE: uAP will automatically switch to the channel that station is on.
        Soft AP started successfully
        This also starts DHCP Server with IP 192.168.1.1
        Network ready IP: 192.168.1.1
        Client => 28:C6:3F:21:6B:AF Associated with Soft AP
7. Redo step 2 and 3, the "Accept update" button becomes active. Click it to make the update permanent.
        update new image gen_level to [2], result 0

Modifying content of static web pages
To modify content available through the web server you must complete following steps:
  1. Modify, add or delete files in folder "boards\<board_name>\lwip_examples\lwip_httpsrv_ota\webpage".
  2. Run the script file "middleware\lwip\src\apps\httpsrv\mkfs\mkfs.pl <directory name>" to generate new "httpsrv_fs_data.c".
     Make sure to execute it from a folder where the file "httpsrv_fs_data.c" is. For example:
        C:\sdk\boards\<board_name>\lwip_examples\lwip_httpssrv_ota> C:\sdk\middleware\lwip\src\apps\httpsrv\mkfs\mkfs.pl webpage
		Processing file webpage/favicon.ico
        Processing file webpage/httpsrv.css
        Processing file webpage/index.html
        Processing file webpage/NXP_logo.png
        Processing file webpage/ota.shtml
        Processing file webpage/ota_reboot.html
        Processing file webpage/request.js
        Processing file webpage/welcome.html
		Done.
  3. Make sure the "httpsrv_fs_data.c" file has been overwritten with the newly generated content.
  4. Re-compile the HTTP server application example and download it to your board. 
