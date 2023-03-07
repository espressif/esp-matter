# Qorvo Thread SDKs

This repository contains the specific files needed to enable Thread on Qorvo platforms.

Example applications and build instructions for each of our platforms can be found [below](#supported-platforms).

## License file

Source code in this repository is covered by the license agreement in [Qorvo_EULA.txt](Qorvo_EULA.txt).

## Description

For each of the supported platforms listed below, Qorvo provides a *Full Thread Device* and a *Minimal Thread Device* library,
required to build and run OpenThread. The ftd library supports the *Commissioner* role, the mtd library does not.

Those libraries are located in

    ./<platform>/lib/libQorvo<platform>_ftd.a
    ./<platform>/lib/libQorvo<platform>_mtd.a

Additionally, a library for hardware supported cryptography is provided as well on platforms that support this.

    ./<platform>/lib/libmbedtls_alt.a

a purely software implementation is used on the other platforms.

## Supported platforms

### SoC platforms

#### QPG6095

Thread examples are available for the QPG6095 Development Kit board. See [QPG6095 instructions](qpg6095/doc/README.md) to setup an OpenThread application on this platform.

All files required are stored in this repository under

    ./qpg6095

#### QPG6100

Thread examples are available for the QPG6100 Development Kit board. See [QPG6100 instructions](qpg6100/doc/README.md) to setup an OpenThread application on this platform.

All files required are stored in this repository under

    ./qpg6100

### Transceiver/Co-Processor platforms

#### GP712

Thread examples are available for the GP712 Evaluation kit with Raspberry Pi. See [GP712 instructions](gp712/doc/README.md) to setup an OpenThread application on this platform.

All files required are stored in this repository under

    ./gp712

#### QPG7015M

Thread examples are available for the QPG7015M Evaluation kit with Raspberry Pi. See [QPG7015M instructions](qpg7015m/doc/README.md) to setup an OpenThread application on this platform.

All files required are stored in this repository under

    ./qpg7015m

## More information

For more information on our product line and support options
Please visit [www.qorvo.com](www.qorvo.com)
or contact us at <LPW.support@qorvo.com>
