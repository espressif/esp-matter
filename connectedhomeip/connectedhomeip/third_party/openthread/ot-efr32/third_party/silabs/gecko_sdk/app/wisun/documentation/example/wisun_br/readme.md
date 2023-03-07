# Wi-SUN - SoC Border Router

The Wi-SUN Border Router demonstration provides a Wi-SUN Border Router implementation running entirely on the EFR32. It provides an easy and quick means to evaluate the Silicon Labs Wi-SUN stack solution without deploying an expensive and cumbersome production-grade Wi-SUN Border Router. A CLI (Command-Line Interface) is exposed to facilitate the configuration.

> The Wi-SUN Border Router demonstration is delivered only in a binary format. The implementation does not scale for a production-grade Border Router maintaining several thousand Wi-SUN nodes.

## Getting Started

To get started with Wi-SUN and Simplicity Studio, see [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf).

The Wi-SUN Border Router demonstration is required to use the other Wi-SUN sample applications. The Wi-SUN Border Router creates a Wi-SUN network the Wi-SUN nodes can join. When part of the same network, the Wi-SUN nodes are able to exchange IP packets.

To get started with the demonstration, follow these steps:

* Flash the "Wi-SUN Border Router" demonstration to a compatible device.
* In Simplicity Studio, open a console on the device.
* Start the Border Router using the CLI command `wisun start`.

Refer to the associated sections in [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf) if you want step-by-step guidelines for each operation.

## Wi-SUN Border Router Commands

To see the available commands, enter the following command in the console:

    wisun help

The list of available commands is output on the console with the associated help. Following is an extended description and examples of how to use each command.

|  | Command | Description | Example |
|---|---|---|---|
|  | wisun start | Start the border router | > wisun start |
|  | wisun get \<domain>.[key] | Get a setting variable. Specifying only the domain retrieves all keys of the domain. | > wisun get wisun.ip_addresses<br>> wisun get wisun |
|  | wisun set \<domain>.\<key> \<value> | Set a setting variable | > wisun set wisun.network_size test |
|  | wisun save | Save variables to NVM | > wisun save |
|  | wisun reset | Reset variables to default values | > wisun reset |
|  | wisun set_pti | Enable or disable PTI | > wisun set_pti 1 |
|  | certificate_store | Store a certificate | > wisun certificate_store |
|  | certificate_remove | Remove a certificate | > wisun certificate_remove 1  |
|  | certificate_list | List installed certificates | > wisun certificate_list |

## Wi-SUN Border Router Settings

The command-line interface maintains a number of settings. The Wi-SUN settings are distributed in two sections: *wisun* and *app*. They can be listed by entering:

    wisun get <section name>

The Wi-SUN stack settings are listed with their current state/value. Some of them can be modified by using the following command prototype:

    wisun set <section name>.<settings name> <value>

To modify the network name used by the Wi-SUN Border Router, enter:

    wisun set wisun.network_name "My Network"

Next time you issue the `wisun start` command, the Border Router creates a Wi-SUN network named "My Network".

### *wisun* Section Settings

The settings in the *wisun* section are directly related to the Wi-SUN stack behavior. The following is a detailed setting list.

| Variable | R/W | Type | Values | Description |
|---|---|---|---|---|
| wisun.state | R | string | initialized (1)<br>started (2) | State of the border router |
| wisun.network_name | R/W | string | up to 31 ASCII characters | Name of the Wi-SUN network |
| wisun.regulatory_domain | R/W | integer | WW (0): Worldwide<br>NA (1): North America<br>JP (2): Japan<br>EU (3): Europe<br>CN (4): China<br>IN (5): India<br>MX (6): Mexico<br>BZ (7): Brazil<br>AZ (8): Australia/New Zealand<br>KR (9): South Korea<br>PH (10): Philippines<br>MY (11): Malaysia<br>HK (12): Hong Kong<br>SG (13): Singapore<br>TH (14): Thailand<br>VN (15): Vietnam<br>application (255): application specific | Regulatory domain of the Wi-SUN network |
| wisun.operating_class | R/W | integer | 1 to 4<br>application (255): application specific | Operating class of the Wi-SUN network to use |
| wisun.operating_mode | R/W | integer | 0x1a: 1a<br>0x1b: 1b<br>0x2a: 2a<br>0x2b: 2b<br>0x3: 3<br>0x4a: 4a<br>0x4b: 4b<br>0x5: 5 | Operating mode of the Wi-SUN network to use |
| wisun.network_size | R/W | integer | automatic (0)<br>small (1)<br>medium (2)<br>large (3) | Estimated size of the Wi-SUN network |
| wisun.tx_power | R/W | integer | -45 to 20 | Maximum TX power in dBm |
| wisun.ip_addresses | R | list of IPv6 addresses |  | List of all IP addresses assigned to the device |
| wisun.phy.ch0_frequency | R/W | integer |  | Central frequency of the first channel in kHz<br>(available only when application-specific channel plan is selected) |
| wisun.phy.number_of_channels | R/W | integer |  | Number of channels<br>(available only when application-specific channel plan is selected) |
| wisun.phy.channel_spacing | R/W | integer | 0: 100 kHz<br>1: 200 kHz<br>2: 400 kHz<br>3: 600 kHz | Channel spacing<br>(available only when application-specific channel plan is selected) |
| wisun.unicast_channel_mask.XXX-XXX | R/W | hexadecimal |  | Application-specific channel mask |
| wisun.fixed_channel | R/W | integer | 0 to 255 | The border router uses the single fixed channel specified |
| wisun.unicast_dwell_interval | R/W | integer | 15 to 255 | Unicast dwell interval in milliseconds |
| wisun.broadcast_dwell_interval | R/W | integer | 100 to 255 | Broadcast dwell interval in milliseconds |
| wisun.broadcast_interval | R/W | integer | Broadcast dwell interval to 16777 | Broadcast interval in milliseconds |

### *app* Section Settings

The settings in the *app* section relate to the application options. A detailed setting list follows.

| Variable | R/W | Type | Values | Description |
|---|---|---|---|---|
| app.autostart | R/W | integer | 0: the border router does not start on reset<br>1: the border router automatically starts on reset | If enabled, the border router automatically starts |
| app.printable_data_length | R/W | integer | 0: received socket data is not printed<br>1 - 64: amount of characters per line | If enabled, received socket data is printed |
| app.printable_data_as_hex | R/W | integer | 0: print received socket data as ASCII<br>1: print received socket data as hex | Output type for received socket data |

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
