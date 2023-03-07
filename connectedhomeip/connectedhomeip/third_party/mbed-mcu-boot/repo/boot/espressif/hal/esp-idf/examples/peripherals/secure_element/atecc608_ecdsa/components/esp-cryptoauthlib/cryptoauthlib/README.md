CryptoAuthLib - Microchip CryptoAuthentication Library {#mainpage}
====================================================

Introduction
------------------------
This library implements the APIs required to communicate with Microchip Security
device. The family of devices supported currently are:

- [ATSHA204A](http://www.microchip.com/ATSHA204A)
- [ATECC108A](http://www.microchip.com/ATECC108A)
- [ATECC508A](http://www.microchip.com/ATECC508A)
- [ATECC608A](http://www.microchip.com/ATECC608A)
- [ATECC608B](http://www.microchip.com/ATECC608B)

The best place to start is with the [Microchip Trust Platform](https://www.microchip.com/design-centers/security-ics/trust-platform)

Online API documentation is at https://microchiptech.github.io/cryptoauthlib/

Latest software and examples can be found at:
  - https://www.microchip.com/design-centers/security-ics/trust-platform
  - http://www.microchip.com/SWLibraryWeb/product.aspx?product=CryptoAuthLib


Prerequisite hardware to run CryptoAuthLib examples:
  - [CryptoAuth Trust Platform Development Kit](https://www.microchip.com/developmenttools/ProductDetails/DM320118)

Alternatively a Microchip MCU and Adapter Board:
  - [ATSAMR21 Xplained Pro]( http://www.microchip.com/atsamr21-xpro )
    or [ATSAMD21 Xplained Pro]( http://www.microchip.com/ATSAMD21-XPRO )
  - [CryptoAuth Xplained Pro Extension](http://www.microchip.com/developmenttools/productdetails.aspx?partno=atcryptoauth-xpro-b )
    or [CryptoAuthentication SOIC Socket Board](http://www.microchip.com/developmenttools/productdetails.aspx?partno=at88ckscktsoic-xpro )
    to accept SOIC parts

For most development, using socketed top-boards is preferable until your
configuration is well tested, then you can commit it to a CryptoAuth Xplained
Pro Extension, for example. Keep in mind that once you lock a device, it will
not be changeable.

Licensing
---------------------------

The CryptoAuthLib license can be found in the accompaning [license.txt](https://github.com/MicrochipTech/cryptoauthlib/blob/main/license.txt)
file.

Cryptoauthlib also includes optional third party software subject to their own licensing terms. If you are using one of these optional components please
verify the terms of those licenses as well in the third_party/<module> directories.


Examples
-----------

  - Watch [CryptoAuthLib Documents](http://www.microchip.com/design-centers/security-ics/cryptoauthentication/overview )
    for new examples coming online.
  - Node Authentication Example Using Asymmetric PKI is a complete, all-in-one
    example demonstrating all the stages of crypto authentication starting from
    provisioning the Crypto Authentication device ATECC608/ATECC508A with keys
    and certificates to demonstrating an authentication sequence using
    asymmetric techniques.
    http://www.microchip.com/SWLibraryWeb/product.aspx?product=CryptoAuthLib

Configuration
-----------
In order to properly configured the library there must be a header file in your
project named `atca_config.h` at minimum this needs to contain defines for the
hal and device types being used. Most integrations have an configuration mechanism
for generating this file. See the [atca_config.h.in](lib/atca_config.h.in) template
which is configured by CMake for Linux, MacOS, & Windows projects.

An example of the configuration:

```
/* Cryptoauthlib Configuration File */
#ifndef ATCA_CONFIG_H
#define ATCA_CONFIG_H

/* Include HALS */
#define ATCA_HAL_I2C

/* Included device support */
#define ATCA_ATECC608_SUPPORT

/* \brief How long to wait after an initial wake failure for the POST to
 *         complete.
 * If Power-on self test (POST) is enabled, the self test will run on waking
 * from sleep or during power-on, which delays the wake reply.
 */
#ifndef ATCA_POST_DELAY_MSEC
#define ATCA_POST_DELAY_MSEC 25
#endif

#endif // ATCA_CONFIG_H
```

There are two major compiler defines that affect the operation of the library.
  - ATCA_NO_POLL can be used to revert to a non-polling mechanism for device
    responses. Normally responses are polled for after sending a command,
    giving quicker response times. However, if ATCA_NO_POLL is defined, then
    the library will simply delay the max execution time of a command before
    reading the response.
  - ATCA_NO_HEAP can be used to remove the use of malloc/free from the main
    library. This can be helpful for smaller MCUs that don't have a heap
    implemented. If just using the basic API, then there shouldn't be any code
    changes required. The lower-level API will no longer use the new/delete
    functions and the init/release functions should be used directly.


Release notes
-----------
See [Release Notes](release_notes.md)


Host Device Support
---------------

CryptoAuthLib will run on a variety of platforms from small micro-controllers
to desktop host systems.  The current list of hardware abstraction layer
support includes:

Rich OS Hosts:
  - Linux Kit Protocol over HID USB
  - Linux I2C
  - Linux SPI
  - Windows Kit Protocol over HID USB

Microcontrollers:
  - Microchip AVR, SAM, & PIC families. See [hal readme](lib/hal/readme.md)

If you have specific microcontrollers or Rich OS platforms you need support
for, please contact us through the Microchip portal with your request.

CryptoAuthLib Architecture
----------------------------
Cryptoauthlib API documentation is at https://microchiptech.github.io/cryptoauthlib/

The library is structured to support portability to:
  - multiple hardware/microcontroller platforms
  - multiple environments including bare-metal, RTOS and Windows/Linux/MacOS
  - multiple chip communication protocols (I2C, SPI, and SWI)

All platform dependencies are contained within the HAL (hardware abstraction
layer).


Directory Structure
-----------------------
```
lib - primary library source code
lib/atcacert - certificate data and i/o methods
lib/calib - the Basic Cryptoauth API
lib/crypto - Software crypto implementations external crypto libraries support (primarily SHA1 and SHA256)
lib/hal - hardware abstraction layer code for supporting specific platforms
lib/host - support functions for common host-side calculations
lib/jwt - json web token functions
test - Integration test and examples. See test/cmd-processor.c for main() implementation.

For production code, test directories should be excluded by not compiling it
into a project, so it is up to the developer to include or not as needed.  Test
code adds significant bulk to an application - it's not intended to be included
in production code.
```

Tests
------------

There is a set of integration tests found in the test directory which will at least
partially demonstrate the use of the objects.  Some tests may depend upon a
certain device being configured in a certain way and may not work for all
devices or specific configurations of the device.

The test/cmd-processor.c file contains a main() function for running the tests.
It implements a command-line interface. Typing help will bring up the list of
commands available.

One first selects a device type, with one of the following commands:
  - 204 (ATSHA204A)
  - 108 (ATECC108A)
  - 508 (ATECC508A)
  - 608 (ATECC608A/B)

From there the following unit test sweets are available:
  - unit (test command builder functions)
  - basic (test basic API functions)
  - cio (test certification i/o functions)
  - cd (test certificate data functions)
  - util (test utility functions)
  - crypto (test software crypto functions)

Tests available depend on the lock level of the device. The unit tests
won't lock the config or data zones automatically to allow retesting at desired
lock levels. Therefore, some commands will need to be repeated after locking
to exercise all available tests.

Starting from a blank device, the sequence of commands to exercise all unit
tests is:
```text
unit
basic
lockcfg
unit
basic
lockdata
unit
basic
cio
cd
util
crypto
```

Using CryptoAuthLib (Microchip CryptoAuth Library)
===========================================

The best place to start is with the [Microchip Trust Platform](https://www.microchip.com/design-centers/security-ics/trust-platform)

Also application examples are included as part of the Harmony 3 framework and can be copied from the Harmony Content Manager
or found with the Harmony 3 Framework [Cryptoauthlib_apps](https://github.com/Microchip-MPLAB-Harmony/cryptoauthlib_apps)


Incorporating CryptoAuthLib in a Linux project using USB HID devices
-----------------------------------------
The Linux HID HAL files use the Linux udev development software package.

To install the udev development package under Ubuntu Linux, please type the
following command at the terminal window:

```bash
sudo apt-get install libudev-dev
```

This adds the udev development development software package to the Ubuntu Linux
installation.

The Linux HID HAL files also require a udev rule to be added to change the
permissions of the USB HID Devices.  Please add a new udev rule for the
Microchip CryptoAuth USB devices.

```bash
cd /etc/udev/rules.d
sudo touch mchp-cryptoauth.rules
```

Edit the mchp-cryptoauth.rules file and add the following line to the file:
```text
SUBSYSTEM=="hidraw", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2312", MODE="0666"
```
