# Concurrent Mode

## 1. Purpose / Scope

This application demonstrates how to configure the RS9116W EVK in both Wi-Fi Station mode and Access Point mode and how to transfer data in both modes.

## 2. Prerequisites / Setup Requirements

Before running the application, the user will need the following things to setup.

### 2.1 Hardware Requirements

* Windows with Host interface(UART/ SPI/ SDIO).
* Silicon Labs [RS9116 Wi-Fi Evaluation Kit](https://www.silabs.com/development-tools/wireless/wi-fi/rs9116x-sb-evk-development-kit)
* Host MCU Eval Kit. This example has been tested with:
  - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)
  - Silicon Labs [WSTK + EFM32GG11](https://www.silabs.com/development-tools/mcu/32-bit/efm32gg11-starter-kit)
  - [STM32F411 Nucleo](https://st.com/)
* Wireless Access Point.
* Windows PC2 (Remote PC) with iperf application.
* Windows PC with Wifi Station.

![Setup Diagram for Concurrent Mode Example](resources/readme/image105.png)

### 2.2 Software Requirements

- [WiSeConnect SDK](https://github.com/SiliconLabs/wiseconnect-wifi-bt-sdk/) 
- Embedded Development Environment
  
   - For STM32, use licensed [Keil IDE](https://www.keil.com/demo/eval/arm.htm)

   - For Silicon Labs EFx32, use the latest version of [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio)

- [Iperf Application](https://iperf.fr/iperf-download.php) in Windows PC (Remote PC)

## 3. Application Build Environment

### 3.1 Platform

The Application can be built and executed on below Host platforms
* [STM32F411 Nucleo](https://st.com/)
* [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit) 
* [WSTK + EFM32GG11](https://www.silabs.com/development-tools/mcu/32-bit/efm32gg11-starter-kit)

### 3.2 Host Interface

* By default, the application is configured to use the SPI bus for interfacing between Host platforms(STM32F411 Nucleo / EFR32MG21) and the RS9116W EVK.
* This application is also configured to use the SDIO bus for interfacing between Host platforms(EFM32GG11) and the RS9116W EVK.

### 3.3 Project Configuration

The Application is provided with the project folder containing Keil and Simplicity Studio project files.

* Keil Project
  - The Keil project is used to evaluate the application on STM32.
  - Project path: `<SDK>/examples/snippets/wlan/concurrent_mode/projects/concurrent_mode-nucleo-f411re.uvprojx`

* Simplicity Studio
  - The Simplicity Studio project is used to evaluate the application on EFR32MG21.
  - Project path: 
    - If the Radio Board is **BRD4180A** or **BRD4181A**, then access the path `<SDK>/examples/snippets/wlan/concurrent_mode/projects/concurrent_mode-brd4180a-mg21.slsproj`
    - If the Radio Board is **BRD4180B** or **BRD4181B**, then access the path `<SDK>/examples/snippets/wlan/concurrent_mode/projects/concurrent_mode-brd4180b-mg21.slsproj`
    - User can find the Radio Board version as given below 

![EFR Radio Boards](resources/readme/image105a.png) 

  - EFM32GG11 platform
    - The Simplicity Studio project is used to evaluate the application on EFM32GG11.
      - Project path:`<SDK>/examples/snippets/wlan/concurrent_mode/projects/concurrent_mode-brd2204a-gg11.slsproj`


### 3.4 Bare Metal/RTOS Support

This application supports bare metal and RTOS environment. By default, the application project files (Keil and Simplicity studio) are provided with bare metal configuration in the SDK.

## 4. Application Configuration Parameters

The application can be configured to suit user requirements and development environment. Read through the following sections and make any changes needed.

### 4.1 Open rsi_concurrent_mode.c file

#### 4.1.1 User must update the below parameters  

SSID refers to the name of the Access point.

```c
#define SSID                          "SILABS_AP"
```

STA_SECURITY_TYPE refers to the type of security. In concurrent mode STA supports Open, WPA and WPA2 securities.

Valid configurations are:

   - RSI_OPEN - For OPEN security mode

   - RSI_WPA - For WPA security mode

   - RSI_WPA2 - For WPA2 security mode

```c
#define STA_SECURITY_TYPE             RSI_WPA2
```

STA_PSK refers to the STA secret key to connect with the secured Access Point.

```c
#define STA_PSK                       "1234567890"
```

DEVICE_PORT port refers internal TCP client port number

```c
#define DEVICE_PORT                        5001
```

REMOTE_PORT port refers remote TCP server port number which is opened in Windows PC2.

```c
#define REMOTE_PORT                        5001
```

SERVER_IP_ADDRESS refers remote peer (Windows PC2) IP address to connect with TCP server socket.

   IP address should be in long format and in little endian byte order.

   Example: To configure "192.168.0.100" as remote IP address, update the macro **SERVER_IP_ADDRESS** as **0x6400A8C0**.

```c
#define SERVER_IP_ADDRESS                  0x6400A8C0
```

#### 4.1.2 The desired parameters are provided below. User can also modify the parameters as per their needs and requirements.

AP_SSID refers to the name of the WiSeConnect Access point would be created.

```c
#define AP_SSID                       "SILABS_AP"
```

AP_CHANNEL_NO refers to the channel in which AP would be started

```c
#define AP_CHANNEL_NO                 11
```

> Note: 
> 1. Valid values for CHANNEL_NO are 1 to 11 in 2.4GHz band and 36 to 48 & 149 to 165 in 5GHz. In this example default configured band is 2.4GHz. If user wants to use 5GHz band then user has to set RSI_BAND macro to 5GHz band in rsi_wlan_config.h file.
> 2. In concurrent mode, STA and AP should be configured in same channel. Configure the AP_CHANNEL_NO to same channel in which Wireless Access point (to which WiSeConnect STA connects) exist.

AP_SECURITY_TYPE refers to the security type of the WiSeConnect Access Point. Access point supports OPEN, WPA-PSK, WPA2-PSK security modes.

   Valid configurations are:

   - RSI_OPEN - For OPEN security mode

   - RSI_WPA - For WPA security mode

   - RSI_WPA2 - For WPA2 security mode

```c
#define SECURITY_TYPE                   RSI_WPA2
```

AP_ENCRYPTION_TYPE refers to the type of Encryption method .Access point supports OPEN, TKIP and CCMP methods.

Valid configurations are:

   - RSI_CCMP - For CCMP encryption

   - RSI_TKIP - For TKIP encryption

   - RSI_NONE - For open encryption

```c
#define AP_ENCRYPTION_TYPE                RSI_CCMP
```

AP_PSK refers to the secret key if the Access point to be configured in WPA/WPA2-PSK security modes.

```c
#define AP_PSK                           “1234567890”
```

BEACON_INTERVAL refers to the time delay between two consecutive beacons in milliseconds in AP mode. Allowed values are integers from 100 to 1000 which are multiples of 100.

```c
#define BEACON_INTERVAL                    100
```

DTIM_INTERVAL refers DTIM interval of the Access Point. Allowed values are from 1 to 255.

```c
#define DTIM_INTERVAL                       4
```

NUMEBR_OF_PACKETS refers how many packets to send from TCP client to TCP server

```c
#define NUMBER_OF_PACKETS                 1000
```

DHCP_MODE refers whether IP address configured through DHCP or STATIC in STA mode

```c
#define DHCP_MODE                         1
```

> Note:
> If user wants to configure STA IP address through DHCP then set DHCP_MODE to 1 and skip configuring the following DEVICE_IP, GATEWAY and NETMASK macros.
> (Or)
> If user wants to configure STA IP address through STATIC then set DHCP_MODE macro to "0" and configure following DEVICE_IP, GATEWAY and NETMASK macros.

IP address to be configured to the device in STA mode should be in long format and in little endian byte order.

   Example: To configure "192.168.0.10" as IP address, update the macro DEVICE_IP as 0x010AA8C0.

```c
#define DEVICE_IP                       0X0A00A8C0
```

IP address of the gateway should also be in long format and in little endian byte order.

   Example: To configure "192.168.0.1" as Gateway, update the macro GATEWAY as 0x0100A8C0

```c
#define GATEWAY                         0x0100A8C0
```

IP address of the network mask should also be in long format and in little endian byte order

   Example: To configure "255.255.255.0" as network mask, update the macro NETMASK as 0x00FFFFFF

```c
#define NETMASK                         0x00FFFFFF
```

> Note:
> 1. This application is not providing the facility to configure the Access Point’s IP Parameters. Default IP address of the Silicon Labs Access point is “192.168.100.76”
> 2. In concurrent mode, IP networks of Silicon Labs STA and Silicon Labs Access  Point both should be different. Configure Wireless Access Point IP network(Ex: 192.168.0.1) other than Silicon Labs Access point IP network.

### 4.2 Open rsi_wlan_config.h file.User can also modify the below parameters as per their needs and requirements.


```c
#define CONCURRENT_MODE                 RSI_ENABLE
#define RSI_FEATURE_BIT_MAP             FEAT_SECURITY_PSK
#define RSI_TCP_IP_BYPASS               RSI_DISABLE
#define RSI_TCP_IP_FEATURE_BIT_MAP      (TCP_IP_FEAT_DHCPV4_SERVER | TCP_IP_FEAT_DHCPV4_CLIENT)
#define RSI_CUSTOM_FEATURE_BIT_MAP      0
#define RSI_EXT_CUSTOM_FEAT_BIT_MAP     0
#define RSI_BAND                        RSI_BAND_2P4GHZ
```

**Power save configuration**

   - By default, the application is configured without power save.

```c
#define ENABLE_POWER_SAVE 0
```
   - If user wants to run the application in power save, modify the below macro.

```c
#define ENABLE_POWER_SAVE 1
```

## 5. Testing the Application

Follow the steps below for the successful execution of the application.

### 5.1 Loading the RS9116W Firmware

Refer [Getting started with a PC](https://docs.silabs.com/rs9116/latest/wiseconnect-getting-started) to load the firmware into RS9116W EVK. The firmware file is located in `<SDK>/firmware/`

### 5.2 Building the Application on Host Platform

#### 5.2.1 Using STM32

Refer [Getting started with STM32](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-stm32/)

- Open the project `<SDK>/examples/snippets/wlan/concurrent_mode/projects/concurrent_mode-nucleo-f411re.uvprojx` 
- Build and Debug the project
- Check for the RESET pin:
  - If RESET pin is connected from STM32 to RS9116W EVK, then user need not press the RESET button on RS9116W EVK before free run.
  - If RESET pin is not connected from STM32 to RS9116W EVK, then user need to press the RESET button on RS9116W EVK before free run.
- Free run the project
- Then continue the common steps from **5.3**

#### 5.2.2 Using EFX32

Refer [Getting started with EFX32](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-efx32/), for settin-up EFR & EFM host platforms

- Open Simplicity Studio and import the EFR32/EFM32 project from `<SDK>/examples/snippets/wlan/concurrent_mode/projects`
    - Select the appropriate .slsproj as per Radio Board type mentioned in **Section 3.3** for EFR32 board.
   (or)
    - Select the *.brd2204a-gg11.slsproj  for EFM32GG11 board.
- Compile and flash the project in to Host MCU
- Debug the project
- Check for the RESET pin:
  - If RESET pin is connected from STM32 to RS9116W EVK, then user need not press the RESET button on RS9116W EVK before free run
  - If RESET pin is not connected from STM32 to RS9116W EVK, then user need to press the RESET button on RS9116W EVK before free run
- Free run the project
- Then continue the common steps from **5.3**

### 5.3 Common Steps

1. Configure the access point in OPEN/WPA-PSK/WPA2-PSK mode to connect RS9116W EVK in STA mode.

2. Open iperf application in Windows PC2(Remote PC) command prompt which is connected to the access point
3. Open TCP server using the below command in command prompt.

  `iperf.exe –s -p <SERVER_PORT> -i 1`

4. After the program gets executed, RS9116W EVK connects to the access point.

5. On other side, RS9116W EVK acts as an access point with provided configurations in **Section 4.1.2**

6. After successful connection in STA mode, RS9116W EVK connects to TCP server socket opened on Windows PC2 (Remote PC) using TCP client socket and sends configured NUMBER_OF_PACKETS to remote TCP server. Refer the below image for reception of TCP data on TCP server.

![Reception of TCP data on TCP server.](resources/readme/image107.png)

7. Connect Windows PC with Wifi station to RS9116W EVK access point

8. After successful connection, open command prompt inWindows PC with WiFi station and initiate ping by using below command

  `ping <RS9116W EVK ip address> -t`

9. The RS9116W EVK access point gives Ping reply for the received Ping Request. The below image depicts the ping success.

![Ping Success](resources/readme/image108.png)

## Compressed Debug Logging

To enable the compressed debug logging feature please refer to [Logging User Guide](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-sapi-reference/logging-user-guide)