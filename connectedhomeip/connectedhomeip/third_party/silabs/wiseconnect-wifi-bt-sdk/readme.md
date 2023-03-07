# WiSeConnect Wi-Fi and Bluetooth Software

WiSeConnect documentation is [available online](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-overview/) ...

- [Getting Started with WiSeConnect](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-overview/)
- [Evaluation Kit User's Guide](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-rs9116x-evk/) 
- [Simple API (SAPI) Reference Guide](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-sapi-reference/)
- [AT-Command Programmers Reference Manuals](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-programming-reference-manuals/)
- [Example Documentation](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-example-applications-overview/)
- [Application Notes](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-application-notes/)
- [Release Notes](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-release-notes/)

# SDK Outline
The WiSeConnect SDK contains the following files and folders.

```
  +--examples
  |  +--featured         > Full-featured examples including cloud, powersave, throughput and RF test apps 
  |  +--snippets         > Simpler examples to demonstrate specific features
  |  +--at_commands      > Scripts to demonstrate usage of AT Commands  
  |-   
  +--firmware                         > RS9116 firmware image(s)
  |  +--RS916.2.6.0.0.34.rps            > This firmware image is valid for RS9116 1.5 revision chip/module
  |  +--RS9116.2.6.0.0.34.rps           > This firmware image is valid for RS9116 1.4/1.3 revision chip/module
  |-   
  +--platforms            
  |  +--efx32            > Board Support for EFR32 Hosts
  |  +--stm32            > Board Support for STM32 Hosts
  |  +--efm32            > Board Support for EFM32 Hosts
  |-   
  +--resources   
  |  +--certificates     > Security certificates and keys to secure Wi-Fi, TCP and HTTP connections 
  |  +--scripts          > Scripts to assist with testing of example applications
  |  +--gain_tables      > Gain tables are added
  |
  +--sapi                
  |  +--bluetooth        > APIs to access Bluetooth Classic and Bluetooth Low Energy features
  |  +--common           > Common APIs including device init, driver init, firmware query, ...
  |  +--crypto           > APIs to access cryptographic functions
  |  +--driver           > Driver source for various host interfaces incluing SPI, SDIO and UART
  |  +--include          > Full collection of API function prototypes
  |  +--logging          > Used for logging
  |  +--network          > Collection of network related protocols including MQTT, HTTP, BSD sockets, ...
  |  +--rtos             > Wrapper files for various RTOS implementations
  |  +--wlan             > APIs to access Wi-Fi features including scan, join, powersave, ...
  |
  +--third_party
  |  +--amazon-freertos  > Amazon FreeRTOS source
  |  +--aws_sdk          > AWS IoT SDK source
  |  +--azure_sdk        > Azure SDK source
  |  +--freertos         > FreeRTOS source
  |  +--mqtt_client      > Paho MQTT source
  |  +--sbc_audio_codec  > Sub-band audio encoder/decoder
  |
  +--utilities           
  |  +--python                 > Python scripts for UART/USB-CDC interface
  |  +--usb_cdc                > Windows inf file for RS9116 usb_cdc port detection
  |  +--ble_provisioning_apps  > Bluetooth provisioning apps for Android and Windows
  |  +--advanced_logging       > It is used for advanced logging
```
