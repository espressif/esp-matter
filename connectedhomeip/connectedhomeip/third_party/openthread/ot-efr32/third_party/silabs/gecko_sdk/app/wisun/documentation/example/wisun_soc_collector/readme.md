# Wi-SUN - SoC Collector

The Wi-SUN Collector sample application demonstrates a data collector implementation on top of a UDP client socket to emulate a metering-like application.

## Getting Started

To get started with Wi-SUN and Simplicity Studio, see [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf).

The Wi-SUN Collector sample application exposes a command-line interface to interact with the Wi-SUN stack and configure which Meters to monitor. The goal of this procedure is to create the Wi-SUN network described in the following figure and have the Collector monitor the Meter.

![Collector/Meter Application Wi-SUN Network](readme_img1.png)

To get started with the example, follow the steps below:

* Flash the "Wi-SUN Border Router" demonstration to a device and start the Border Router.
* Create and build the Meter project.
* Flash the Meter project to a second device.
* Create and build the Collector project.
* Flash the Collector project to a third device.
* Using Simplicity Studio, open consoles on both the Meter and Collector devices.
* Wait for the Collector and Meter to join the Wi-SUN Border Router network.

Refer to the associated sections in [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf) if you want step-by-step guidelines for each operation.

## Monitor a Meter

The three Wi-SUN devices (Border Router, Meter, Collector) are now part of the same Wi-SUN network. To check the commands exposed in the Collector application, enter:

    wisun help

The Collector application has two specific commands: `wisun register_meter [IPv6 address]` and `wisun remove_meter [IPv6 address]`. Use the first command to register the Meter.

    wisun register_meter [Meter Global IPv6 address]

The Collector starts monitoring and retrieving sensor data from the Meter. Each udpate is output in the console as shown.

    [fd2a:6e01:9bfc:990c:20d:6fff:fe20:b6f9: {id: 1}{temp: 28.52}{hum: 36.01}{lx: 480}]

A Collector can monitor several Meters simultaneously. To monitor an additional Meter, repeat the registration process with a Meter device connected to the same Wi-SUN network.

To stop monitoring a device, enter:

    wisun remove_meter [Meter Global IPv6 address]

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
