# CPC Secondary UART

This sample application demonstrates how to open CPC user endpoints, and perform read and write operations.
The application receives data on user endpoints and echos it back.

A python script, **cpc_interactive_client.py**, can be used to simulate a host application.

## Installation and Use

Once the CPC secondary application is built and installed on the secondary, the CPC daemon and library can
be installed on the host device from the [Github repository](https://github.com/SiliconLabs/cpc-daemon).

Once the applications are installed, the secondary can be connected via the VCOM port to the host,
using a USB cable.

The sample application on the secondary will open two user endpoints, SL_CPC_ENDPOINT_USER_ID_0 and SL_CPC_ENDPOINT_USER_ID_1.
The Python script **cpc_interactive_client.py**, located in the script folder on the host, can be used to open and close
endpoints, and write to and read data from them.
