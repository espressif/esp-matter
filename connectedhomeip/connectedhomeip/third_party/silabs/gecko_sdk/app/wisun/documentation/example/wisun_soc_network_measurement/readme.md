# Wi-SUN - SoC Network Measurement

The Wi-SUN Network Measurement application provides a performance measurement solution based on the ping mechanism. This application leverages the LCD screen to be used without any additional device.

## Getting Started

To get started with Wi-SUN and Simplicity Studio, see [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf).

The SoC term stands for "System on Chip", meaning that this is a standalone application running on the EFR32 without any external MCU required.

This example exposes a command-line interface to interact with the Wi-SUN stack. To get started with the example, follow the steps below:

* Flash the "Wi-SUN Border Router" demonstration to a device and start the Border Router.
* Create and build the Wi-SUN - SoC Network Measurement project.
* Flash the Wi-SUN Network Measurement project to a second device.
* Wait for the connection to complete.

![Connection Screen](readme_img1.png)

* When the main menu appears, you can start a test.
* Use either the LCD screen and push buttons or the CLI to interact with the ping test tool.
* Use the CLI to execute iPerf tests.

For additional information and guidelines, refer to the dedicated application note: [AN1364: Wi-SUN Network Performance Measurement Application](https://www.silabs.com/documents/public/application-notes/an1364-wi-sun-network-performance-measurement-app.pdf).

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
