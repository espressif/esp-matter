# Wi-SUN - SoC CoAP Collector

The Wi-SUN CoAP Collector sample application demonstrates the use of the Constrained Application Protocol (CoAP) to emulate a metering-like application. The CoAP Collector's purpose is to retrieve measurements from the devices running the Wi-SUN CoAP Meter application in the same Wi-SUN network. It also shows an implementation example of an application layer library on top of the Wi-SUN stack (i.e., CoAP).

## Getting Started

To get started with Wi-SUN and Simplicity Studio, see [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf).

Wi-SUN CoAP Collector sample application exposes a command-line interface to interact with the Wi-SUN stack and configure the CoAP Meters to monitor. The goal of this procedure is to create the Wi-SUN network described in the following figure and have the CoAP Collector monitor the CoAP Meter.

![CoAP Application Wi-SUN Network](readme_img1.png)

To get started with the example, follow the steps below:

* Flash the "Wi-SUN Border Router" demonstration to a device and start the Border Router.
* Create and build the CoAP Meter project.
* Flash the CoAP Meter project to a second device.
* Create and build the CoAP Collector project.
* Flash the CoAP Collector project to a third device.
* Using Simplicity Studio, open consoles on both the Meter and Collector devices.
* Wait for the CoAP Collector and Meter to join the Wi-SUN Border Router network.

Refer to the associated sections in [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf) for step-by-step guidelines for each operation.

## Monitor a CoAP Meter

The three Wi-SUN devices (Border Router, CoAP Meter, CoAP Collector) are now part of the same Wi-SUN network. To check the commands exposed in the CoAP Collector application, enter:

    wisun help

The CoAP Collector application has two specific commands: `wisun register_meter [IPv6 address]` and `wisun remove_meter [IPv6 address]`. Use the first command to register the CoAP Meter.

    wisun register_meter [CoAP Meter Global IPv6 address]

The CoAP Collector starts monitoring and retrieving sensor data from the CoAP Meter. The sensor data consists of actual temperature and relative humidity sensor measurements from the SI7021 I²C sensor and dummy lux values. Each udpate is output in the console as shown below.

    {
    token_len:      0
    coap_status:    0
    msg_code:       69
    msg_type:       0
    content_format: 50
    msg_id:         7
    payload_len:    62
    uri_path_len:   11

    token:

    uri_path:
    measurement

    payload:
    {
    "id": 1,
    "temp": 28.29,
    "hum": 30.98,
    "lx": 580
    }

    }

A CoAP Collector can monitor several CoAP Meters simultaneously. To monitor an additional CoAP Meter, repeat the registration process with a CoAP Meter device connected to the same Wi-SUN network.

To stop monitoring a device, enter:

    wisun remove_meter [CoAP Meter Global IPv6 address]

## Troubleshooting

Before programming the radio board mounted on the WSTK, ensure the power supply switch is in the AEM position (right side), as shown.

![Radio Board Power Supply Switch](readme_img0.png)

## Resources

* [Wi-SUN Stack API documentation](https://docs.silabs.com/wisun/latest)
* [AN1330: Wi-SUN Mesh Network Performance](https://www.silabs.com/documents/public/application-notes/an1330-wi-sun-network-performance.pdf)
* [AN1332: Wi-SUN Network Setup and Configuration](https://www.silabs.com/documents/public/application-notes/an1332-wi-sun-network-configuration.pdf)
* [AN1364: Wi-SUN Network Performance Measurement Application](https://www.silabs.com/documents/public/application-notes/an1364-wi-sun-network-performance-measurement-app.pdf)
* [QSG181: Wi-SUN Quick-Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf)
* [UG495: Wi-SUN Developer's Guide](https://www.silabs.com/documents/public/user-guides/ug495-wi-sun-developers-guide.pdf)

## Report Bugs & Get Support

You are always encouraged and welcome to ask any questions or report any issues you found to us via [Silicon Labs Community](https://community.silabs.com/s/topic/0TO1M000000qHc6WAE/wisun).
