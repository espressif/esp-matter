# Wi-SUN - SoC TCP Client

The Wi-SUN TCP Client sample application provides a simple client implementation based on the connection-oriented TCP protocol. The sample application operates with a command-line interface to create, close, read, or write sockets. The TCP Client leverages the POSIX-like socket component on top of the Wi-SUN stack.

## Getting Started

To get started with Wi-SUN and Simplicity Studio, see [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf).

The Wi-SUN TCP Client sample application exposes a command-line interface to interact with the Wi-SUN stack and configure TCP sockets. The goal of this procedure is to create the Wi-SUN network described in the following figure and have the TCP Client exchange messages with the TCP Server.

![TCP Application Wi-SUN Network](readme_img1.png)

To get started with the example, follow the steps below:

* Flash the "Wi-SUN Border Router" demonstration to a first device and start the Border Router.
* Create and build the TCP Server project.
* Flash the TCP Server project to a second device.
* Create and build the TCP Client project.
* Flash the TCP Client project to a third device.
* Using Simplicity Studio, open consoles on both the TCP Server and TCP Client devices.
* Wait for the TCP Server and TCP Client to join the Wi-SUN Border Router network.

Refer to the associated sections in [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf) if you want step-by-step guidelines to go through each operation.

## Send TCP Packets

The three Wi-SUN devices (Border Router, TCP Client, TCP Server) are now part of the same Wi-SUN network. To check the commands exposed in the TCP Client application, enter:

    wisun help

The TCP Client application has four specific commands: `wisun tcp_client`, `wisun socket_close`, `wisun socket_write` and `wisun socket_read`. Use the first command to open a TCP socket with the TCP Server.

    wisun tcp_client [TCP Server Global IPv6 address] 4567

Port 4567 is the default port used by the TCP Server application. The TCP Client application replies with the ID of the created socket.

    > wisun tcp_client fd00:7283:7e00:0:20d:6fff:fe20:b6f9 4567
    [Socket created: 4]

If the following steps, ID '4' is used to refer to the socket. The next step is to send a TCP packet with the newly created socket. Use the `socket_write` command to send the packet.

    wisun socket_write [Socket ID] [Message]

For example.

    > wisun socket_write 4 "hello world!"
    > [Data sent on socket: 4]

You can then use the socket to read data echoed back by the TCP Server. Use the `socket_read` command to send the packet.

    wisun socket_read [Socket ID] [Number of bytes to read]

For example.

    > wisun socket_read 4 5
    hello

Finally, you can close the TCP socket using the `socket_close` command.

    wisun socket_close [Socket ID]

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
