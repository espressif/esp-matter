# Wi-SUN - SoC TCP Server

The Wi-SUN TCP Server sample application provides a simple echo server implementation based on the connection-oriented TCP protocol. The sample application creates a TCP socket where all incoming packets are echoed back to the sender. The TCP Server leverages the POSIX-like socket component on top of the Wi-SUN stack.

## Getting Started

To get started with Wi-SUN and Simplicity Studio, see [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf).

The Wi-SUN TCP Server  sample application exposes a command-line interface to interact with the Wi-SUN stack. The goal of this procedure is to create the Wi-SUN network described in the following figure and have the TCP Client exchange messages with the TCP Server.

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

## TCP Echo Server

The three Wi-SUN devices (Border Router, TCP Client, TCP Server) are now part of the same Wi-SUN network. After a successful connection, the TCP Server application creates a socket on port 4567 by default.

    [Port: 4567]
    TCP server socket() [4] done.
    TCP server bind() [0] done.
    TCP server listen() [0] done.
    [Waiting for connection request]

The TCP Server socket is ready to accept an incoming connection from the TCP Client. The TCP server socket can only maintain a connection with a single client at a time. Refer to the Wi-SUN - SoC TCP Client readme to connect the TCP Client and send TCP packets.

On the TCP Client connection, the TCP Server outputs the following message on the console.

    TCP server accept() [5] done.

When a packet is received, the TCP echo server sends the data back to the sender.

    [fd00:7283:7e00:0:20d:6fff:fe20:bd45] hello world!
    [Data sent on socket: 5]

If the TCP Client closes the socket, the following message is output on the console.

    [Socket closing: 5]
    [Waiting for connection request]

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
