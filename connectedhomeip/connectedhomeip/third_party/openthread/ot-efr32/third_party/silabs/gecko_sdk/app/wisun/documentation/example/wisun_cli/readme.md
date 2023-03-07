# Wi-SUN - SoC CLI

The Wi-SUN CLI (Command-Line Interface) sample application allows developers to easily evaluate the Wi-SUN stack APIs. The Wi-SUN command line interface provides a serial interface to a number of the Wi-SUN stack functions. For example, it can be used to connect the Wi-SUN device to a Wi-SUN border router and exchange IP packets or perform RF tests.

## Getting Started

To get started with Wi-SUN and Simplicity Studio, see [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf).

This example exposes a command-line interface to interact with the Wi-SUN stack. To get started with the example, follow the steps below:

* Flash the "Wi-SUN Border Router" demonstration to a device and start the Border Router.
* Create and build the Wi-SUN CLI project.
* Flash the Wi-SUN CLI project to a second device.
* Using Simplicity Studio, open a console on the device running the Wi-SUN CLI project.

Refer to the associated sections in [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf) if you want step-by-step guidelines for each operation. To fully evaluate the Wi-SUN CLI features, another device running the Wi-SUN CLI application might be required. The Wi-SUN CLI application can also interact with the other Wi-SUN examples (Wi-SUN Ping, Wi-SUN TCP/UDP Server/Client...).

> The Wi-SUN CLI example can be used to evaluate and test the Wi-SUN stack but should not be used to create productized applications. Developers should implement their own C application running in the EFR32 and using the Wi-SUN stack API.

## Wi-SUN Commands

To see the available commands, enter the following command in the console.

    wisun help

The list of available commands is output on the console with the associated help. Following is an extended description and examples of how to use each command.

| Command | Description | Example |
|---|---|---|
| wisun connect | Connect to the predefined Wi-SUN network 'Wi-SUN Network' | > wisun connect |
| wisun disconnect | Disconnect from the Wi-SUN network | > wisun disconnect |
| wisun get \<section>.[setting] | Get a setting variable. Specifying only the section retrieves all settings of the section. | > wisun get wisun.ip_addresses<br>> wisun get wisun <br>> wisun get statistics <br>> wisun get app |
| wisun mac_allow \<mac address> | Add an allowed MAC address to the access list<br>broadcast address: allow all MAC addresses<br>unique address: allow the given MAC address | > wisun mac_allow ff:ff:ff:ff:ff:ff:ff:ff<br>> wisun mac_allow ffffffffffffffff |
| wisun mac_deny \<mac address> | Add a denied MAC address to the access list<br>broadcast address: deny all MAC addresses<br>unique address: deny the given MAC address | > wisun mac_deny 00:01:02:03:04:05:06:07<br>> wisun mac_deny 0001020304050607 |
| wisun ping \<remote address> | Ping a remote host | > wisun ping fd00:7283:7e00:0:fd6f:d00:a8c0:20fe |
| wisun reset | Reset variables to default values | > wisun reset |
| wisun save | Save variables to NVM | > wisun save |
| wisun set \<section>.\<setting> \<value> | Set a setting variable | > wisun set wisun.network_size small |
| wisun socket_close \<socket> | Close an open socket | > wisun socket_close 3 |
| wisun socket_list | List open sockets | > wisun socket_list |
| wisun socket_read \<socket> \<amount of bytes> | Read buffered data from a socket | > wisun socket_read 3 14 |
| wisun socket_set_option \<socket> \<option> \<option data> | Configure a socket | > wisun socket_set_option 3 event_mode indication |
| wisun socket_write \<socket> \<string> | Write a string to a socket | > wisun socket_write 3 "hello world" |
| wisun socket_writeto \<socket> \<remote address> \<remote port> \<string\> | Write a string to an unconnected socket, i.e. UDP server socket | > wisun socket_writeto 3 fc00::1 5001 "hello world" |
| wisun tcp_client \<remote address> \<remote port> | Open a TCP connection to a remote host | > wisun tcp_client fd24:120b:802c:0001:705d:9179:8607:fd21 5001 |
| wisun tcp_server \<local port> | Open a TCP server port | > wisun tcp_server 5001 |
| wisun udp_client \<remote address> \<remote port> | Open a UDP connection to a remote host | > wisun udp_client fd00:6172:6d00:0:3038:5115:26:27 7 |
| wisun udp_server \<local port> | Open a UDP server port | > wisun udp_server 5001 |
| wisun socket_get_option \<socket Id> \<option> | Get a socket option| > wisun socket_get_option |
| wisun clear_credential_cache | Clear the wisun credential cache | > wisun clear_credential_cache |
| wisun reset_statistics | Reset the statistics | > wisun reset_statistics |
| wisun set_trace_level \<string> | Set [trace level](https://docs.silabs.com/wisun/latest/wisun-stack-api/sl-wisun-types#sl-wisun-trace-level-t) for a [trace group](https://docs.silabs.com/wisun/latest/wisun-stack-api/sl-wisun-types#sl-wisun-trace-group-t) | > wisun set_trace_level all,debug<br>[Set 64 trace groups]<br>> wisun set_trace_level 1,none;4,info<br>[Set 2 trace groups]| 
|wisun set_unicast_tx_mode \<value>| Configure unicast tx mode | > wisun set_unicast_tx_mode 0<br>[Unicast Tx mode set to 0 suceeded]<br>> wisun set_unicast_tx_mode 1<br>[Unicast Tx mode set to 1 suceeded]|
|

## Wi-SUN Settings

The command-line interface maintains a number of settings. The Wi-SUN settings are distributed in three sections: *wisun*, *statistics*, and *app*. They can be listed by entering:

    wisun get <section name>

The Wi-SUN stack settings are listed with their current state/value. Some of them can be modified by using the following command prototype:

    wisun set <section name>.<settings name> <value>

To modify the network name the Wi-SUN device should connect to, enter:

    wisun set wisun.network_name "My Network"

Next time you issue the `wisun connect` command, the device starts a connection process with the Wi-SUN network named "My Network".

### *wisun* Section Settings

The settings in the *wisun* section are directly related to the Wi-SUN stack behavior. A detailed setting list can be found below.

| Variable | R/W | Type | Values | Description |
|---|---|---|---|---|
| wisun.network_name | R/W | string | up to 31 ASCII characters | Name of the Wi-SUN network |
| wisun.regulatory_domain | R/W | integer | WW (0): Worldwide<br>NA (1): North America<br>JP (2): Japan<br>EU (3): Europe<br>CN (4): China<br>IN (5): India<br>MX (6): Mexico<br>BZ (7): Brazil<br>AZ (8): Australia/New Zealand<br>KR (9): South Korea<br>PH (10): Philippines<br>MY (11): Malaysia<br>HK (12): Hong Kong<br>SG (13): Singapore<br>TH (14): Thailand<br>VN (15): Vietnam<br>application (255): application specific | Regulatory domain of the Wi-SUN network |
| wisun.operating_class | R/W | integer | 1 to 4<br>application (255): application specific | Operating class of the Wi-SUN network to use |
| wisun.operating_mode | R/W | integer | 0x1a: 1a<br>0x1b: 1b<br>0x2a: 2a<br>0x2b: 2b<br>0x3: 3<br>0x4a: 4a<br>0x4b: 4b<br>0x5: 5 | Operating mode of the Wi-SUN network to use |
| wisun.network_size | R/W | integer | 0: automatic<br>1: small<br>2: medium<br>3: large<br>4: test<br>5: certification | Estimated size of the Wi-SUN network |
| wisun.tx_power | R/W | integer | -45 to 20 | Maximum TX power in dBm |
| wisun.unicast_dwell_interval | R/W | integer | 10 to 255 | Unicast dwell interval in milliseconds |
| wisun.ip_addresses | R | list of IPv6 addresses |  | List of all IP addresses assigned to the device |
| wisun.border_router | R | list of IPv6 addresses |  | List of known IPv6 addresses of the border router |
| wisun.parents | R | list of IPv6 addresses |  | List of known IPv6 addresses of the parents |
| wisun.neighbors | R | list of IPv6 addresses |  | List of known IPv6 addresses of the RPL neighbors  |
| wisun.phy.ch0_frequency | R/W | integer |  | Central frequency of the first channel in kHz<br>(available only when application-specific channel plan is selected) |
| wisun.phy.number_of_channels | R/W | integer |  | Number of channels<br>(available only when application-specific channel plan is selected) |
| wisun.phy.channel_spacing | R/W | integer | 0: 100 kHz<br>1: 200 kHz<br>2: 400 kHz<br>3: 600 kHz | Channel spacing<br>(available only when application-specific channel plan is selected) |
| wisun.unicast_channel_mask.XXX-XXX | R/W | hexadecimal |  | Application-specific channel mask |
| wisun.join_state | R | integer | 0: Idle<br>1: Select PAN<br>2: Authenticate<br>3: Acquire PAN Config<br>4: Configure Routing<br>5: Operational | Current join state in the connection process |
| wisun.mac | R/W | MAC address |  | MAC address to use |
|

### *statistics* Section Settings

The settings part of the *statistics* section are counters maintained by the Wi-SUN stack. A detailed setting list can be found below.

| Variable | Read/Write | Description |
|---|---|---|
| statistics.phy | R | PHY statistics |
| statistics.mac | R | MAC statistics |
| statistics.fhss | R | Frequency hopping statistics |
| statistics.wisun | R | Wi-SUN layer statistics |
| statistics.network | R | 6LoWPAN/IP stack statistics |
|

### *app* Section Settings

The settings in the *app* section relate to the application options. A detailed setting list follows.

| Variable | R/W | Type | Values | Description |
|---|---|---|---|---|
| app.printable_data_length | R/W | integer | 0: received socket data is not printed<br>1 - 64: amount of characters per line | If enabled, received socket data is printed |
| app.printable_data_as_hex | R/W | integer | 0: print received socket data as ASCII<br>1: print received socket data as hex | Output type for received socket data |
|

## RF test Commands

To see the available commands, enter the following command in the console.

    rftest help

The list of available commands is output on the console with the associated help. Following is an extended description and examples of how to use each command.

| Command | Description | Example |
|---|---|---|
| rftest start_stream \<channel ID> | Start an RF test stream on a specific channel | > rftest start_stream 5<br>RF Test stream started 21 |
| rftest stop_stream | Stop the RF test | > rftest stop_stream<br>RF Test stream stopped 2 |
|rftest start_tone \<channel ID>| Start an RF test tone on a specific channel | > rftest start_tone 5<br>RF Test tone started 21|
|rftest stop_tone | Stop RF test tone | > rftest stop_tone<br>RF Test tone stopped 2|
| rftest set_tx_power \<value> | Set Tx power for the RF test in dBm |> rftest set_tx_power 17<br>RF Test tx Power set to  17|
|

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
